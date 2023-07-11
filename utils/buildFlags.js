var fs = require("fs");
var path = require("path");

// Always run preinstall and don't cleanup during postinstall
var isGitRepo = true;

// try {
//   fs.statSync(path.join(__dirname, "..", ".git"));
//   isGitRepo = true;
// } catch (e) {
//   isGitRepo = false;
// }

module.exports = {
  debugBuild: !!process.env.BUILD_DEBUG,
  isElectron: process.env.npm_config_runtime === "electron",
  isGitRepo: isGitRepo,
  isNwjs: process.env.npm_config_runtime === "node-webkit",
  mustBuild: !!(isGitRepo || process.env.BUILD_DEBUG || process.env.BUILD_ONLY)
};
