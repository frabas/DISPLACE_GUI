from recipes.recipe import Recipe
from recipes.helpers import helpers
import os,zipfile,shutil,glob

class SQLite(Recipe):

    version=3210000
    archive=("sqlite-amalgamation-%s.zip" % version)
    url=("https://sqlite.org/2017/%s" % archive)

    def __init__(self, env):
        super(SQLite, self).__init__(env)
        self.path = os.path.join(env.getBuildDir(), ("sqlite-amalgamation-%s" % self.version))
        self.bldpath = os.path.join(self.path, "build")

    def name(self):
        return "SQLITE"

    def download(self):
        if not helpers.download(self.url, self.archive):
            return False

        with zipfile.ZipFile(self.archive) as zf:
            zf.extractall()

        self.setDownloaded()
        return True

    def build_type(self, build_type):
        os.chdir(self.path)
        helpers.mkdir(self.bldpath)
        os.chdir(self.bldpath)
        helpers.mkdir(build_type)
        os.chdir(build_type)

        shutil.copy(os.path.join(self.env.getDataDir(), "sqlite","CMakeLists.txt"),
                    "../../")

        cmdline = ["cmake", self.path,
                   "-G", "Visual Studio 14 2015 Win64",
                   '-DCMAKE_BUILD_TYPE=Release'.format(build_type),
                   '-DCMAKE_INSTALL_PREFIX={}'.format(self.env.getInstallDir())
                   ]

        result = helpers.execute(cmdline, "configure-out.txt", "configure-err.txt")
        if not result:
            print("Cannot configure.")
            return False

        cmdline = ["cmake", "--build", ".", "--config", build_type]

        result = helpers.execute(cmdline, "compile-out.txt", "compile-err.txt")
        if result:
            self.setBuilt()
        return result

    def build(self):
        """
        result = self.build_type("Debug")
        if not result:
            return result
        """
        result = self.build_type("Release")
        return result

    def install(self):
        os.chdir(self.path)
        libdir = self.env.getInstallLibDir()

        helpers.copy(glob.glob("*.h"), self.env.getInstallIncludeDir())

        os.chdir(os.path.join(self.bldpath, "Release", "Release"))   # Yes, that's not an error.

        helpers.copy(glob.glob("*.dll"), libdir)
        helpers.copy(glob.glob("*.lib"), libdir)
        return True

