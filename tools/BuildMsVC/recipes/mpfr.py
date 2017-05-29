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

    def compile(self, btype):
        os.chdir(self.bldpath)
        cmdline = ["msbuild", "dll_mpfr.sln", "/t:dll_mpfr"
                   ]
        cmdline.append("/p:BuildInParallel=true")
        cmdline.append('/p:Configuration={}'.format(btype))
        cmdline.append("/p:Platform=x64")

        result = helpers.execute(cmdline, "build-out.txt", "build-err.txt")
        return result

    def deploy(self, btype):
        relpath = os.path.join(self.path, 
                               "dll",
                               "x64",
                               btype)
        os.chdir(relpath)
        libdir = self.env.getInstallLibDir()

        helpers.copy(glob.glob("*.h"), self.env.getInstallIncludeDir())
        helpers.copy(glob.glob("*.dll"), libdir)
        helpers.copy(glob.glob("*.lib"), libdir)        
        return True


    def build(self):
        if not self.compile("Debug"):
            return False
        if not self.compile("Release"):
            return False
        self.setBuilt()
        return True

    def install(self):
        if not self.deploy("Debug"):
            return False
        if not self.deploy("Release"):
            return False

        self.setInstalled()
        return True
