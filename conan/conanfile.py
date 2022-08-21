from conans import ConanFile
import os


class ZipUtilsConan(ConanFile):
    scm = {
        'type': 'git',
        'subfolder': '',
        'url': 'https://github.com/ZhekehZ/Cpp-zip-utils.git',
        'revision': os.environ.get('ZIP_UTILS_REVISION', 'auto')
    }

    name = "ZipUtils"
    version = "0.1"
    description = "A header only c++ library that provides some " \
                  "structured_binding-friendly functions"
    url = scm['url']
    license = "MIT"
    author = "Eugene Kravchenko"

    default_user = '_'
    default_channel = '_'

    no_copy_source = True

    def package(self):
        self.copy("*.hpp")
