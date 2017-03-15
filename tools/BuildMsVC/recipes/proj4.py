from recipes.recipe import Recipe
from recipes.helpers import helpers
import os,tarfile,shutil,glob, patch


class proj4(Recipe):
    """description of class"""

    url = "http://download.osgeo.org/proj/proj-4.9.3.tar.gz"
    archive = "proj-4.9.3.tar.gz"
    BuildType = "Release"

    def __init__(self, env):
        super(proj4, self).__init__(env)
        self.path = os.path.join(env.getBuildDir(), "proj-4.9.3")
        self.bldpath = os.path.join(self.path, "build")

    def name(self):
        return "proj4"

    def download(self):
        if not helpers.download(self.url, self.archive):
            return False
        
        with tarfile.open(self.archive, mode='r:gz') as tf:
            tf.extractall()
        
        self.setDownloaded()
        return True

    def build(self):
        os.chdir(self.path)
        helpers.mkdir (self.bldpath)
        os.chdir(self.bldpath)

        os.environ.putenv("OSGEO4W_ROOT", os.path.join(self.env.getInstallDir()))
        os.environ.putenv("GMP_DIR", os.path.join(self.env.getInstallDir()))
        os.environ.putenv("MPFR_DIR", os.path.join(self.env.getInstallDir()))
        os.environ.putenv("BOOST_ROOT", os.path.join(self.env.getInstallDir()))

        cmdline = ["cmake", self.path,
                   "-G", "Visual Studio 14 2015 Win64",
                   "-DCMAKE_BUILD_TYPE=Release", 
                   ]

        result = helpers.execute(cmdline, "configure-out.txt", "configure-err.txt")
        if not result:
            print("Cannot configure.")
            return False

        cmdline = ["cmake", "--build", ".", "--config", self.BuildType]

        result = helpers.execute(cmdline, "compile-out.txt", "compile-err.txt")
        if result:
            self.setBuilt()
        return result

    def install(self):
        os.chdir(self.bldpath)

        cmdline = ["cmake", "--build", ".", "--config", self.BuildType, "--target", "INSTALL"]
        result = helpers.execute(cmdline, "compile-out.txt", "compile-err.txt")
        if not result:
            return False

        os.chdir(self.env.getInstallDir())
        helpers.move(glob.glob("local\\lib\\*.*"), "lib\\")
        helpers.move(glob.glob("local\\include\\*.*"), "include\\")
        
        self.setInstalled()
        return True