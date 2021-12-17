#include "../include/async_baton.h"

namespace nodegit {
  void deleteBaton(AsyncBaton *baton) {
    delete baton;
  }

  AsyncBaton::AsyncBaton()
    : asyncResource(ThreadPool::GetCurrentAsyncResource()),
    callbackErrorHandle(*ThreadPool::GetCurrentCallbackErrorHandle()),
    completedMutex(new std::mutex),
    hasCompleted(false)
  {}

  void AsyncBaton::SignalCompletion() {
    std::lock_guard<std::mutex> lock(*completedMutex);
    hasCompleted = true;
    completedCondition.notify_one();
  }

  void AsyncBaton::Done() {
    onCompletion();
  }

  Nan::AsyncResource *AsyncBaton::GetAsyncResource() {
    return asyncResource;
  }

  void AsyncBaton::SetCallbackError(v8::Local<v8::Value> error) {
    callbackErrorHandle.Reset(error);
  }

  void AsyncBaton::ExecuteAsyncPerform(AsyncCallback asyncCallback, AsyncCallback asyncCancelCb, CompletionCallback onCompletion) {
    auto jsCallback = [asyncCallback, this]() {
      asyncCallback(this);
    };
    auto cancelCallback = [asyncCancelCb, this]() {
      asyncCancelCb(this);
    };

    if (onCompletion) {
      this->onCompletion = [this, onCompletion]() {
        onCompletion(this);
      };

      ThreadPool::PostCallbackEvent(
        [jsCallback, cancelCallback](
          ThreadPool::QueueCallbackFn queueCallback
        ) {
          queueCallback(jsCallback, cancelCallback);
        }
      );
    } else {
      this->onCompletion = std::bind(&AsyncBaton::SignalCompletion, this);

      ThreadPool::PostCallbackEvent(
        [jsCallback, cancelCallback](
          ThreadPool::QueueCallbackFn queueCallback
        ) {
          queueCallback(jsCallback, cancelCallback);
        }
      );

      WaitForCompletion();
    }
  }

  void AsyncBaton::WaitForCompletion() {
    std::unique_lock<std::mutex> lock(*completedMutex);
    while (!hasCompleted) completedCondition.wait(lock);
  }
}
