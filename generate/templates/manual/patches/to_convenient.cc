NAN_METHOD(GitPatch::ToConvenient) {
  Nan::EscapableHandleScope scope;

  git_error_clear();

  { // lock master scope start
    nodegit::LockMaster lockMaster(
      /*asyncAction: */false,
      Nan::ObjectWrap::Unwrap<GitPatch>(info.This())->GetValue()
    );

    const PatchData * result = createFromRaw(
      Nan::ObjectWrap::Unwrap<GitPatch>(info.This())->GetValue()
    );

    // null checks on pointers
    if (!result) {
      return info.GetReturnValue().Set(scope.Escape(Nan::Undefined()));
    }

    v8::Local<v8::Value> v8ConversionSlot;
    // start convert_to_v8 block
    if (result != NULL) {
      v8ConversionSlot = ConvenientPatch::New((void *) result);
    }
    else {
      v8ConversionSlot = Nan::Null();
    }
    // end convert_to_v8 block

    return info.GetReturnValue().Set(scope.Escape(v8ConversionSlot));
  }
}
