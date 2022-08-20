from conans import ConanFile

class ZipUtilsConan(ConanFile):
    name = "ZipUtils"
    version = "0.1"

    # No settings/options are necessary, this is header only
    exports_sources = "../*"
    no_copy_source = True

    def layout(self):
        self.folders.imports = "include"

    def package(self):
        self.copy("*.hpp")
