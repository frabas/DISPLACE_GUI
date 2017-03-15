from recipes.recipe import Recipe
from recipes.helpers import helpers
import os,tarfile,shutil,glob, patch


class geos(Recipe):
    """description of class"""

    url="http://download.osgeo.org/geos/geos-3.5.1.tar.bz2"
    archive = "geos-3.5.1.tar.bz2"
    BuildType = "Release"

    def __init__(self, env):
        super(geos, self).__init__(env)
        self.path = os.path.join(env.getBuildDir(), "geos-3.5.1")
        self.bldpath = self.path

    def name(self):
        return "geos"

    def download(self):
        if not helpers.download(self.url, self.archive):
            return False
        
        with tarfile.open(self.archive, mode='r:bz2') as tf:
            tf.extractall()
        
        self.setDownloaded()
        return True

    def build(self):
        os.chdir(self.path)
        shutil.copy(os.path.join(self.env.getDataDir(), "geos", "GenerateSourceGroups.cmake"),
                    os.path.join("cmake", "modules"))
        helpers.mkdir (self.bldpath)
        os.chdir(self.bldpath)

        os.environ.putenv("OSGEO4W_ROOT", os.path.join(self.env.getInstallDir()))
        os.environ.putenv("GMP_DIR", os.path.join(self.env.getInstallDir()))
        os.environ.putenv("MPFR_DIR", os.path.join(self.env.getInstallDir()))
        os.environ.putenv("BOOST_ROOT", os.path.join(self.env.getInstallDir()))

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

        #cmdline = ["nmake", "/f", "makefile.vc", "MSVC_VER=1400", "install"]
        cmdline = ["cmake", 
                   "--build", ".", "--config", self.BuildType, "--target", "install"]
        result = helpers.execute(cmdline, "install-out.txt", "install-err.txt")
        if not result:
            return False

        os.chdir(self.env.getInstallDir())
        helpers.move(glob.glob("bin\\*.dll"), "lib\\")
        
        self.setInstalled()
        return True
