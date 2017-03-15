# -*- coding: utf-8 -*-

import os


class Env(object):
    ''' Keeps track of the environment variables
    '''

    VersionValue = 1

    def __init__(self):
        self.cwd = os.getcwd()
        self.builddir = os.path.join(self.cwd, "build")
        self.installdir = os.path.join(self.cwd, "install")
        self.statedir = os.path.join(self.builddir, "state")
        self.datadir = os.path.join(self.cwd, "data")

    def getRootDir(self):
        return self.cwd

    def getBuildDir(self):
        return self.builddir

    def getInstallDir(self):
        return self.installdir

    def getInstallLibDir(self):
        return os.path.join(self.getInstallDir(), "lib")

    def getInstallIncludeDir(self):
        return os.path.join(self.getInstallDir(), "include")

    def getInstallBinDir(self):
        return os.path.join(self.getInstallDir(), "bin")

    def getStateDir(self):
        return self.statedir

    def getDataDir(self):
        return self.datadir

    def getVersion(self):
        return self.VersionValue