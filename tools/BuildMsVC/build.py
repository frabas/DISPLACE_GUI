# -*- coding: utf-8 -*-

import os
from env import Env
from recipes.zlib import Zlib
from recipes.boost import Boost
from recipes.iconv import iconv
from recipes.mpir import mpir
from recipes.mpfr import mpfr
from recipes.cgal import cgal
from recipes.proj4 import proj4
from recipes.geos import geos
from recipes.gdal import gdal
from recipes.geographiclib import geographiclib
from recipes.helpers import helpers
from recipes.sqlite import SQLite
import pathlib

class Dependency(object):

    def __init__(self, env):
        self.env = env
        self.dependencies = [ SQLite(env)
                             ]

    def prepare(self):
        if not os.path.exists(env.getBuildDir()):
            pathlib.Path(env.getBuildDir()).mkdir(parents=True, exist_ok=True)
        if not os.path.exists(env.getInstallDir()):
            pathlib.Path(env.getInstallDir()).mkdir(parents=True, exist_ok=True)
        if not os.path.exists(env.getStateDir()):
            pathlib.Path(env.getStateDir()).mkdir(parents=True, exist_ok=True)
        helpers.mkdir(env.getInstallBinDir())
        helpers.mkdir(env.getInstallLibDir())
        helpers.mkdir(env.getInstallIncludeDir())

    def run(self, module):
        try:
            if module.isInstalled():
                return True
            os.chdir(self.env.getBuildDir())

            if not module.checkPrerequisites():
                print ('Check Prerequisites for {} failed.'.format(module.name()))
                return False

            if not module.isDownloaded() :
                if not module.download():
                    raise Exception("Cannot download")
            else:
                print ('{} Already downloaded, skipping.'.format(module.name()))

            os.chdir(self.env.getBuildDir())
            if not module.isBuilt():
                if not module.build():
                    raise Exception("Cannot build")
            else:
                print ('{} Already built, skipping.'.format(module.name()))

            os.chdir(self.env.getBuildDir())
            if not module.isInstalled():
                if not module.install():
                    raise Exception ("Cannot install")
            
        except Exception as ex:
            print ("Error: ", ex)
            return False
        else:
            return True

    def getDependencies(self):
        return self.dependencies


if os.environ.get("VCINSTALLDIR") == None:
    print ("VCINSTALLDIR missing. Run vcvarsall.bat (arch) before running this script.")
    exit(1)

env = Env()
print ("Displace Build tool v.", env.getVersion())
print ("Running from: ", env.getRootDir())
print ("Will be installed in: ", env.getInstallDir())

dep = Dependency(env)
dep.prepare()
recipes = dep.getDependencies()
for recipe in recipes:
    print ("Processing: ", recipe.name())
    if not dep.run(recipe):
        print ("Aborted.")
        exit (1)
