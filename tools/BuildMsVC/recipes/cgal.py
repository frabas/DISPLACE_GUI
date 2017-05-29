from recipes.recipe import Recipe
from recipes.helpers import helpers
import os,zipfile,shutil,glob, patch

class cgal(Recipe):
    """description of class"""

    url = "https://github.com/CGAL/cgal/releases/download/releases%2FCGAL-4.9/CGAL-4.9.zip"
    archive = "CGAL-4.9.zip"

    def __init__(self, env):
        super(cgal, self).__init__(env)
        self.path = os.path.join(env.getBuildDir(), "cgal-4.9")
        self.bldpath = os.path.join(self.path, "build")

    def name(self):
        return "cgal"

    def download(self):
        if helpers.download(self.url, self.archive):
            self.setDownloaded()
        else:
            return False
        with zipfile.ZipFile(self.archive) as zf:
            zf.extractall()
        
        return True

    def compile(self, btype):
        os.chdir(self.path)
        if os.path.exists("src\\CGAL_ImageIO"):
            shutil.rmtree("src\\CGAL_ImageIO")
        if os.path.exists("src\\CGAL_Qt5"):
            shutil.rmtree("src\\CGAL_Qt5")

        helpers.mkdir (self.bldpath)
        os.chdir(self.bldpath)

        os.environ.putenv("GMP_DIR", os.path.join(self.env.getInstallDir()))
        os.environ.putenv("MPFR_DIR", os.path.join(self.env.getInstallDir()))
        os.environ.putenv("BOOST_ROOT", os.path.join(self.env.getInstallDir()))

        cmdline = ["cmake", self.path,
                   "-G", "Visual Studio 14 2015 Win64",
                   '-DCMAKE_BUILD_TYPE={}'.format(btype), 
                   ]
                   #'-DCMAKE_INSTALL_PREFIX={}'.format(self.env.getInstallDir()),

        result = helpers.execute(cmdline, "configure-out.txt", "configure-err.txt")
        if not result:
            print("Cannot configure.")
            return False

        #cmdline = ["msbuild", "CGAL.sln", "/p:BuildInParallel=true", "/p:Platform=x64"]
        #cmdline.append('/p:Configuration={}'.format(self.BuildType))
        cmdline = ["cmake", "--build", ".", "--config", btype]

        result = helpers.execute(cmdline, "compile-out.txt", "compile-err.txt")
        return result

    def build(self):
        if not self.compile("Debug"):
            return False
        if not self.compile("Release"):
            return False
        self.setBuilt()
        return True
        
    def install(self):
        os.chdir(self.bldpath)

        libdir = self.env.getInstallLibDir()
        helpers.copy(glob.glob("lib\\Release\\*.lib"), libdir)
        helpers.copy(glob.glob("lib\\Debug\\*.lib"), libdir)
        helpers.copy(glob.glob("bin\\*.dll"), libdir)

        hdrdir = os.path.join(self.env.getInstallIncludeDir(), "CGAL")
        if os.path.exists(hdrdir):
            shutil.rmtree(hdrdir)
        shutil.copytree("..\\include\\CGAL", hdrdir)
        shutil.copy("include\\CGAL\\compiler_config.h",
                     hdrdir)
        
        self.setInstalled()
        return True
