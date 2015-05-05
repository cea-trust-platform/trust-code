#! /bin/bash
#
# --
# Exceptions functions
#
# Author : Eli LAUCOIN (CEA)
# --

raise_undefined_TRUST_ROOT_exception() {
    echo "--------------------------------------------"
    echo "USAGE ERROR : undefined variable TRUST_ROOT"
    echo "--------------------------------------------"
    echo ""
    exit -1
}

raise_undefined_TRUST_MED_ROOT_exception() {
    echo "------------------------------------------------"
    echo "USAGE ERROR : undefined variable TRUST_MED_ROOT"
    echo "------------------------------------------------"
    echo ""
    exit -1
}

raise_invalid_USE_MED_VERSION_exception() {
    echo "---------------------------------------------------"
    echo "USAGE ERROR : invalid value of USE_MED_VERSION"
    echo ""
    echo "  Current value : ${USE_MED_VERSION}"
    echo ""
    echo "  Valid values are : '3.0' and '2.3'"
    echo ""
    echo "  If USE_MED_VERSION is not defined, default is 3.0"
    echo "---------------------------------------------------"
    echo ""
    exit -1
}

raise_undefined_EXTERNAL_HDF_HOME_exception() {
    echo "--------------------------------------------------"
    echo "USAGE ERROR : undefined variable EXTERNAL_HDF_HOME"
    echo "--------------------------------------------------"
    echo ""
    exit -1
}

raise_invalid_external_HDF_installation_directory_exception() {
    echo "---------------------------------------------------------------"
    echo "USAGE ERROR : invalid variable EXTERNAL_HDF_HOME"
    echo ""
    echo "  EXTERNAL_HDF_HOME must be a valid HDF5 installation directory"
    echo "---------------------------------------------------------------"
    echo ""
    exit -1
}

raise_invalid_external_HDF_version_exception() {
    echo "--------------------------------------------------------------------"
    echo "USAGE ERROR : bad version of external installation of HDF5"
    echo ""
    echo "  EXTERNAL_HDF_HOME must be a valid installation directory of either"
    echo "  version 1.8 or version 1.6 of HDF5"
    echo ""
    echo "  The need for version 1.8 or version 1.6 of HDF5 depends"
    echo "  on the choosen version of MED :"
    echo ""
    echo "     MED version 3.0 ==> HDF5 version 1.8"
    echo "     MED version 2.3 ==> HDF5 version 1.6"
    echo "--------------------------------------------------------------------"
    echo ""
    exit -1
}

raise_undefined_EXTERNAL_MED_HOME_exception() {
    echo "--------------------------------------------------"
    echo "USAGE ERROR : undefined variable EXTERNAL_MED_HOME"
    echo "--------------------------------------------------"
    echo ""
    exit -1
}

raise_invalid_external_MED_installation_directory_exception() {
    echo "--------------------------------------------------------------"
    echo "USAGE ERROR : invalid variable EXTERNAL_MED_HOME"
    echo ""
    echo "  EXTERNAL_MED_HOME must be a valid MED installation directory"
    echo "--------------------------------------------------------------"
    echo ""
    exit -1
}

raise_invalid_external_MED_version_exception() {
    echo "--------------------------------------------------------------------"
    echo "USAGE ERROR : bad version of external installation of MED"
    echo ""
    echo "  EXTERNAL_MED_HOME must be a valid installation directory of either"
    echo "  version 3.0 or version 2.3 of MED."
    echo "--------------------------------------------------------------------"
    echo ""
    exit -1
}

raise_internal_HDF_external_MED_exception() {
    echo "------------------------------------------------------------------------------------------"
    echo "USAGE ERROR : can't load an external version of MED while using a provided version of HDF5"
    echo ""
    echo "  Please define USE_EXTERNAL_HDF and EXTERNAL_HDF_HOME, or reset USE_EXTERNAL_MED"
    echo "------------------------------------------------------------------------------------------"
    echo ""
    exit -1
}
raise_HDF_version_incompatible_with_specification_exception() {
    echo "---------------------------------------------------------------------------"
    echo "USAGE ERROR : specified version of MED is incompatible"
    echo "              with detected external version of HDF5"
    echo ""
    echo "  Specified version of MED (USE_MED_VERSION)                         : ${1}"
    echo "  Detected version of HDF5 (USE_EXTERNAL_HDF5 and EXTERNAL_HDF_HOME) : ${2}"
    echo ""
    echo " Trio-U accepts only one of the following associations :"
    echo "     MED version 3.0  and  HDF5 version 1.8"
    echo "     MED version 2.3  and  HDF5 version 1.6"
    echo "---------------------------------------------------------------------------"
    echo ""
    exit -1
}

raise_MED_version_incompatible_with_specification_exception() {
    echo "--------------------------------------------------------------------------"
    echo "USAGE ERROR : specified version of MED is incompatible"
    echo "              with detected external version of MED"
    echo ""
    echo "  Specified version of MED (USE_MED_VERSION)                        : ${1}"
    echo "  Detected version of MED  (USE_EXTERNAL_MED and EXTERNAL_MED_HOME) : ${2}"
    echo "--------------------------------------------------------------------------"
    echo ""
    exit -1
}

raise_incompatible_MED_HDF_versions_exception() {
    echo "--------------------------------------------------------"
    echo "USAGE ERROR : incompatible versions of MED and HDF5"
    echo ""
    echo "  Choosen version of MED  : ${1}"
    echo "  Choosen version of HDF5 : ${2}"
    echo ""
    echo " Trio-U accepts only one of the following associations :"
    echo "     MED version 3.0  and  HDF5 version 1.8"
    echo "     MED version 2.3  and  HDF5 version 1.6"
    echo "--------------------------------------------------------"
    echo ""
    exit -1
}

raise_invalid_HDF_configuration_exception() {
    echo "-------------------------------------------"
    echo "INTERNAL ERROR : invalid HDF5 configuration"
    echo ""
    echo "TRUST_HDF_VERSION  = ${1}"
    echo "TRUST_HDF_LOCATION = ${2}"
    echo "-------------------------------------------"
    echo ""
    exit -1
}

raise_invalid_MED_configuration_exception() {
    echo "------------------------------------------"
    echo "INTERNAL ERROR : invalid MED configuration"
    echo ""
    echo "TRUST_HDF_VERSION  = ${1}"
    echo "TRUST_HDF_LOCATION = ${2}"
    echo "TRUST_MED_VERSION  = ${3}"
    echo "TRUST_MED_LOCATION = ${4}"
    echo "------------------------------------------"
    echo ""
    exit -1
}

raise_invalid_HDF_report_generation_exception() {
    echo "-----------------------------------------------------------"
    echo "INTERNAL ERROR : can't generated HDF5's installation report"
    echo "-----------------------------------------------------------"
    echo ""
    exit -1
}

raise_invalid_MED_report_generation_exception() {
    echo "----------------------------------------------------------"
    echo "INTERNAL ERROR : can't generated MED's installation report"
    echo "----------------------------------------------------------"
    echo ""
    exit -1
}

raise_invalid_HDF_version_exception() {
    echo "-------------------------------------"
    echo "INTERNAL ERROR : invalid HDF5 version"
    echo "-------------------------------------"
    echo ""
    exit -1
}

raise_invalid_HDF_location_exception() {
    echo "--------------------------------------"
    echo "INTERNAL ERROR : invalid HDF5 location"
    echo "--------------------------------------"
    echo ""
    exit -1
}

raise_invalid_MED_version_exception() {
    echo "------------------------------------"
    echo "INTERNAL ERROR : invalid MED version"
    echo "------------------------------------"
    echo ""
    exit -1
}

raise_invalid_MED_location_exception() {
    echo "-------------------------------------"
    echo "INTERNAL ERROR : invalid MED location"
    echo "-------------------------------------"
    echo ""
    exit -1
}

raise_invalid_package_version_exception() {
    echo "--------------------------------------------"
    echo "INTERNAL ERROR : invalid ${1} version : ${2}"
    echo "--------------------------------------------"
    echo ""
    exit -1
}
    
