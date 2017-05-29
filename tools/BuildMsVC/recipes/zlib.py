# -*- coding: utf-8 -*-

import zipfile, urllib.request, shutil
import os
import subprocess
import glob
from recipes.recipe import Recipe
from recipes.helpers import helpers


class Zlib(Recipe):
    '''Build zlib
    '''

    BuildType = "Release"

    def __init__(self, env):
        super(Zlib, self).__init__(env)
        self.homepath = os.path.join(self.env.getBuildDir(), "zlib-1.2.11")
        self.path = os.path.join(self.homepath, "contrib", "vstudio", "vc14")

    def name(self):
        return "zlib"

    def download(self):
        archive = "zlib1211.zip"
        url = 'http://www.zlib.net/{}'.format(archive)
        if helpers.download(url, archive):
            self.setDownloaded()
        else:
            return False
        with zipfile.ZipFile(archive) as zf:
            zf.extractall()
        return True

    def buildVersion(self, type):
        print ("Entering: ", self.path)
        os.chdir(self.path)
        cmdline = ["MSBuild.exe", "zlibvc.sln"]
        cmdline.append("/p:BuildInParallel=true")
        cmdline.append('/p:Configuration={}'.format(type))
        cmdline.append("/t:zlibvc")

        out = open(os.path.join(self.homepath, "build-out.txt"), "w")
        err = open(os.path.join(self.homepath, "build-err.txt"), "w")
        
        process = subprocess.Popen(cmdline, shell=True, stderr=err, stdout=subprocess.PIPE) #stdout=out, 
        for line in process.stdout:
            print (line)
            print (line, file=out)
        process.wait()

        result = process.returncode == 0
        if result:
            self.setBuilt()
        return result		
		
    def build(self):
        '''if not self.buildVersion("Debug"):
          return False
        '''
        if not self.buildVersion("Release"):
            return False
        return True
		
    def install(self):
        os.chdir(os.path.join(self.path, "x64", 'ZlibDll{}'.format(self.BuildType)))
        helpers.copy(glob.glob("*.lib"), self.env.getInstallLibDir())
        helpers.copy(glob.glob("*.dll"), self.env.getInstallLibDir())
        os.chdir(self.homepath)
        helpers.copy(glob.glob("*.h"), self.env.getInstallIncludeDir())
        self.setInstalled()

        return True
