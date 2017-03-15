from recipes.recipe import Recipe
from recipes.helpers import helpers
import os,zipfile,shutil,glob
import re

class iconv(Recipe):
    """description of class"""

    url="https://github.com/LuaDist/libiconv/archive/master.zip"
    archive = "libiconv-master.zip"
    BuildType = "Release"

    def __init__(self, env):
        super(iconv, self).__init__(env)
        self.path = os.path.join(env.getBuildDir(), "libiconv-master")
        self.bldpath = os.path.join(self.path, "build")

    def name(self):
        return "libiconv"

    def download(self):
        if not helpers.download(self.url, self.archive):
            return False
        
        with zipfile.ZipFile(self.archive) as zf:
            zf.extractall()

        self.setDownloaded()
        return True

    def build(self):
        os.chdir(self.path)
        helpers.mkdir (self.bldpath)
        os.chdir(self.bldpath)

        cmdline = ["cmake", self.path,
                   "-G", "Visual Studio 14 2015 Win64",
                   "-DCMAKE_BUILD_TYPE=Release", 
                   '-DCMAKE_INSTALL_PREFIX={}'.format(self.env.getInstallDir())
                   ]

        result = helpers.execute(cmdline, "configure-out.txt", "configure-err.txt")
        if not result:
            print("Cannot configure.")
            return False

        #cmdline = ["nmake", "/f", "makefile.vc", "MSVC_VER=1400"]
        cmdline = ["cmake", "--build", ".", "--config", self.BuildType]

        result = helpers.execute(cmdline, "compile-out.txt", "compile-err.txt")
        if result:
            self.setBuilt()
        return result

    def install(self):
        os.chdir(self.bldpath)

        cmdline = ["cmake", 
                   "--build", ".", "--config", self.BuildType, "--target", "install"]
        result = helpers.execute(cmdline, "install-out.txt", "install-err.txt")
        if not result:
            return False

        os.chdir(self.env.getInstallDir())
        helpers.move(glob.glob("bin\\*.dll"), "lib\\")
        
        self.setInstalled()
        return True
