from recipes.recipe import Recipe
from recipes.helpers import helpers
import os,zipfile,shutil,glob

class mpfr(Recipe):
    """Build MPFR"""

    archive = "mpfr-master.zip"
    unpackdir = "mpfr-master"
    url = "https://github.com/BrianGladman/mpfr/archive/master.zip"
    #url = "http://www.mpfr.org/mpfr-current/mpfr-3.1.5.zip"

    Processor = "gc" # Check MPIR documentation. Build Generic Intel version
    BuildType = "Release"

    def __init__(self, env):
        super(mpfr, self).__init__(env)
        self.path = os.path.join(env.getBuildDir(), "mpfr")
        self.bldpath = os.path.join(self.path, "build.vc14")

    def name(self):
        return "MPFR"

    def download(self):
        if helpers.download(self.url, self.archive):
            self.setDownloaded()
        else:
            return False
        with zipfile.ZipFile(self.archive) as zf:
            zf.extractall()
        
        shutil.move(self.unpackdir, self.path)            
        return True

    def build(self):
        os.chdir(self.bldpath)
        cmdline = ["msbuild", "dll_mpfr.sln", "/t:dll_mpfr"
                   ]
        cmdline.append("/p:BuildInParallel=true")
        cmdline.append('/p:Configuration={}'.format(self.BuildType))
        cmdline.append("/p:Platform=x64")

        result = helpers.execute(cmdline, "build-out.txt", "build-err.txt")
        if result:
            self.setBuilt()
        return result

    def install(self):
        relpath = os.path.join(self.path, 
                               "dll",
                               "x64",
                               self.BuildType)
        os.chdir(relpath)
        libdir = self.env.getInstallLibDir()

        helpers.copy(glob.glob("*.h"), self.env.getInstallIncludeDir())
        helpers.copy(glob.glob("*.dll"), libdir)
        helpers.copy(glob.glob("*.lib"), libdir)
        
        self.setInstalled()
        return True