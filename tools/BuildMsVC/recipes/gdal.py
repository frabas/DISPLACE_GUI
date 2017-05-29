from recipes.recipe import Recipe
from recipes.helpers import helpers
import os,tarfile,shutil,glob
import re

class gdal(Recipe):
    """description of class"""

    url="http://download.osgeo.org/gdal/1.11.5/gdal-1.11.5.tar.gz"
    archive = "gdal-1.11.5.tar.gz"

    def __init__(self, env):
        super(gdal, self).__init__(env)
        self.path = os.path.join(env.getBuildDir(), "gdal-1.11.5")
        self.bldpath = self.path

    def name(self):
        return "gdal"

    def download(self):
        if not helpers.download(self.url, self.archive):
            return False
        
        with tarfile.open(self.archive, mode='r:gz') as tf:
            tf.extractall()
        
        self.setDownloaded()
        return True

    def compile(self):
        os.chdir(self.path)

        with open("nmake.local", "w") as cfg:
            cfg.write('GDAL_HOME={}\n'.format(self.env.getInstallDir()))
            cfg.write("WIN64=YES\n")
            cfg.write("PLATFORM=x64")
            cfg.write('GEOS_DIR={}\n'.format(self.env.getInstallDir()))
            cfg.write('GEOS_CFLAGS = -I{}\\include -DHAVE_GEOS\n'.format(self.env.getInstallDir()))
            cfg.write('GEOS_LIB = {}\\lib\\geos_c.lib\n'.format(self.env.getInstallDir()))
            cfg.write('PROJ_INCLUDE = -I{}'.format(self.env.getInstallIncludeDir()))
            cfg.write('PROJ_LIBRARY = {}\proj_4_9.lib'.format(self.env.getInstallLibDir()))

        with open("nmake.opt", "a") as nm:
            nm.write("\nEXTERNAL_LIBS =	$(EXTERNAL_LIBS) legacy_stdio_definitions.lib\n")

        helpers.mkdir (self.bldpath)
        os.chdir(self.bldpath)

        shutil.copy(os.path.join(self.env.getDataDir(), "gdal","cpl_config.h"),
                    "port")

        cmdline = ["nmake", "/f", "makefile.vc",
                   'GDAL_ROOT={}'.format(self.path),
                   'INCDIR={}\include\gdal'.format(self.path),
                   "MSC_VER=1900", "WIN64=YES"
                   ]

        result = helpers.execute(cmdline, "build-out.txt", "build-err.txt")
        if not result:
            print("Cannot configure.")
            return False

        if result:
            self.setBuilt()
        return result

    def deploy(self):
        os.chdir(self.bldpath)

        cmdline = ["nmake", 
                   'INCDIR={}\include\gdal'.format(self.env.getInstallDir()),
                   "/f", "makefile.vc", 
                   "devinstall"]
        result = helpers.execute(cmdline, "install-out.txt", "install-err.txt")
        if not result:
            return False

        #os.chdir(self.env.getInstallDir())
        #helpers.move(glob.glob("bin\\*.dll"), "lib\\")
        
        self.setInstalled()
        return True

    def build(self):
        if not self.compile():
            return False
        self.setBuilt()
        return True

    def install(self):
        if not self.deploy():
            return False

        self.setInstalled()
        return True
