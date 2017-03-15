# -*- coding: utf-8 -*-

import os
import glob
from env import Env

class Recipe(object):

    def __init__(self, env):
        self.env = env

    def name(self):
        raise Exception("Calling undefined Recipe")

    def check(self):
        return True

    def download(self):
        return True

#    def unpack():
#        return True

    def build(self):
        return True

    def checkPrerequisites(self):
        return True

    def install(self):
        return True

    def getStateFileName(self, state):
        return os.path.join(self.env.getStateDir(), ('{}.{}'.format(self.name(), state)))

    def isDownloaded(self):
        #print ('{} is downloaded ? {}'.format(self.getStateFileName("download"), os.path.isfile(self.getStateFileName("download"))))
        return os.path.isfile(self.getStateFileName("download"))

    def isInstalled(self):
        return os.path.isfile(self.getStateFileName("install"))

#    def isUnpacked(self):
#        return os.path.isfile(getStateFileName("unpack"))

    def isBuilt(self):
        return os.path.isfile(self.getStateFileName("build"))

    def clearState(self):
        return os.remove(glob(self.getStateFileName("*")))

    def setState(self, state):
        open(self.getStateFileName(state), 'a').close()

    def setDownloaded(self):
        self.setState("download")

    def setBuilt(self):
        self.setState("build")

    def setInstalled(self):
        self.setState("install")
