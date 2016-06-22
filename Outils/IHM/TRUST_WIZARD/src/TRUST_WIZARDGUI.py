# -*- coding: latin-1 -*-
#****************************************************************************
# Copyright (c) 2015 - 2016, CEA
# All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# 
#*****************************************************************************

#
# Author : A. Bruneton
#
import sys, os
from trust_wiz.TrustWizDesktop import TrustWizDesktop

desktop = None

# Get SALOME PyQt interface
import SalomePyQt
import libSALOME_Swig

########################################################
# Global variables
########################################################

sgPyQt = SalomePyQt.SalomePyQt()
sg = libSALOME_Swig.SALOMEGUI_Swig()
sgDesktop = sgPyQt.getDesktop()

moduleDesktop   = {}

########################################################
# Internal methods
########################################################

def getStudyId():
    """This method returns the active study ID"""
    return sgPyQt.getStudyId()

def getStudy():
    """This method returns the active study"""

    studyId = _getStudyId()
    study = getStudyManager().GetStudyByID( studyId )
    return study

def getDesktop():
    """This method returns the current TRUST_PLOT2D desktop"""

    global desktop
    return desktop

def setDesktop( studyID ):
    """This method sets and returns TRUST_PLOT2D desktop"""

    global moduleDesktop, desktop

    if not moduleDesktop.has_key( studyID ):
      moduleDesktop[studyID] = TrustWizDesktop( sgPyQt )
      moduleDesktop[studyID].initialize()
    desktop = moduleDesktop[studyID]
    return desktop

################################################
# Callback functions
################################################

def initialize():
    """This method is called when module is initialized. It performs initialization actions"""
    setDesktop( getStudyId() )
    pass

def windows():
    """This method is called when module is initialized. It returns a map of popup windows to be used by the module"""
    wm = {}
    return wm

def views():
    """This method is called when module is initialized. It returns a list of 2D/3D views to be used by the module"""
    return []

def activate():
    """This method mimicks SALOME's module activation """
    global desktop
    desktop.showCentralWidget()
    return True

def deactivate():
    """This method is called when module is deactivated"""
    global moduleDesktop, widgetDialogBox
    moduleDesktop[getStudyId()].hideCentralWidget()
    pass
