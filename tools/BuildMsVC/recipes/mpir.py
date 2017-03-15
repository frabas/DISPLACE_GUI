from recipes.recipe import Recipe
from recipes.helpers import helpers
import os,zipfile,shutil,glob

class mpir(Recipe):
    """description of class"""

    archive = "mpir-master.zip"
    unpackdir = "mpir-master"
    url = "https://github.com/BrianGladman/mpir/archive/master.zip"

    Processor = "gc" # Check MPIR documentation. Build Generic Intel version
    BuildType = "Release"

    def __init__(self, env):
        super(mpir, self).__init__(env)
        self.path = os.path.join(env.getBuildDir(), "mpir")
        self.bldpath = os.path.join(self.path, "build.vc14")

    def name(self):
        return "MPIR"

    def checkPrerequisites(self):
        if not os.path.exists("C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\bin\\vsyasm.exe"):
            print ("Cannot find C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC\\bin\\vsyasm.exe")
            return False

        #if shutil.which("vsyasm.exe") == None:
        #    print ("Cannot find vsyasm.exe")
        #    return False
        return True

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
        cmdline = ["msbuild", 
                   self.Processor,
                   "DLL",
                   "x64",
                   self.BuildType
                   ]
        cmdline.append("/p:BuildInParallel=true")
        #cmdline.append('/p:Configuration={}'.format(self.BuildType))

        result = helpers.execute(cmdline, "build-out.txt", "build-err.txt")
        if result:
            self.setBuilt()
        return result

    def install(self):
        relpath = os.path.join(self.bldpath, 
                               'dll_mpir_{}'.format(self.Processor),
                               "x64",
                               self.BuildType)
        os.chdir(relpath)
        hdrdir = os.path.join(self.env.getInstallIncludeDir(),
                              "mpir")
        libdir = self.env.getInstallLibDir()

        helpers.mkdir(hdrdir)
        helpers.copy(glob.glob("*.h"), hdrdir)
        #print ('Copy: {} from {} to {}'.format(glob.glob("*.h"), relpath, hdrdir))
        helpers.copy(glob.glob("*.dll"), libdir)
        helpers.copy(glob.glob("*.lib"), libdir)
        
        # Copy mpir.dll to gmp.dll
        os.chdir(libdir)
        shutil.copy("mpir.dll", "gmp.dll")
        shutil.copy("mpir.lib", "gmp.lib")
        
        self.setInstalled()
        return True