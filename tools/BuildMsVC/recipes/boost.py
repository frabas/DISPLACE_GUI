import zipfile, urllib.request, shutil
import os
import subprocess
import glob
from recipes.recipe import Recipe
from recipes.helpers import helpers


class Boost(Recipe):
    """Build boost.org libraries"""

    def __init__(self, env):
        super(Boost, self).__init__(env)
        self.path = os.path.join(env.getBuildDir(), "boost_1_63_0")

    def name(self):
        return "Boost"

    def download(self):
        archive = "boost_1_63_0.zip"
        url = 'https://sourceforge.net/projects/boost/files/boost/1.63.0/{}'.format(archive)
        if helpers.download(url, archive):
            self.setDownloaded()
        else:
            return False
        with zipfile.ZipFile(archive) as zf:
            zf.extractall()
        return True

    def getB2Options(self):
        return ["address-model=64",
                "toolset=msvc",
                "link=shared", 
                "threading=multi",
		"runtime-link=shared",
                "--with-regex",
                "--with-test",
                "--with-program_options",
                ]

    def build(self):
        print ("Entering: ", self.path)
        os.chdir(self.path)

        cmdline = ["bootstrap"]
        result = helpers.execute(cmdline, os.path.join(self.path, "bootstrap-out.txt"), os.path.join(self.path, "bootstrap-err.txt"))

        if not result:
            return False

        cmdline = ["b2"]
        cmdline.append("variant=release")
        cmdline.extend(self.getB2Options())
        cmdline.append("stage")
                   
        result = helpers.execute(cmdline, os.path.join(self.path, "build-out.txt"), os.path.join(self.path, "build-err.txt"))

        if not result:
            return False

        cmdline = ["b2"]
        cmdline.append("variant=debug")
        cmdline.extend(self.getB2Options())
        cmdline.append("define=_ITERATOR_DEBUG_LEVEL=2")
        cmdline.append("stage")
                   
        result = helpers.execute(cmdline, os.path.join(self.path, "build-out.txt"), os.path.join(self.path, "build-err.txt"))
        

        if result:
            self.setBuilt()

        return result

    def install(self):
        os.chdir(self.path)

        cmdline = ["b2"]
        cmdline.append('--prefix={}'.format(self.env.getInstallDir()))
        cmdline.append("variant=release,debug")
        cmdline.extend(self.getB2Options())
        cmdline.append("install")

        result = helpers.execute(cmdline, os.path.join(self.path, "install-out.txt"), os.path.join(self.path, "install-err.txt"))

        if not result:
            return False

        # Fix the include tree
        os.chdir(self.env.getInstallIncludeDir())
        shutil.rmtree ("boost", ignore_errors = True)
        shutil.move(os.path.join("boost-1_63","boost"), ".")
        os.rmdir("boost-1_63")

        self.setInstalled()

        return True
