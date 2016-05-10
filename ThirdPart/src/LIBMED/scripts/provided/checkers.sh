#! /bin/bash
#
# --
# Checking functions
#
# Author : Eli LAUCOIN (CEA)
# --


# checking Trio-U's environment variables
check_environment() {
    # checking the definition of TRUST_ROOT
    test "x_${TRUST_ROOT}_x" != "x__x" || raise_undefined_TRUST_ROOT_exception
    
    # checking the definition of TRUST_MED_ROOT
    test "x_${TRUST_MED_ROOT}_x" != "x__x" || raise_undefined_TRUST_MED_ROOT_exception
}

# checking internal HDF5's version
check_internal_HDF_version() {
    # defining TRUST_HDF_LOCATION
    TRUST_HDF_LOCATION="internal"

    # defining TRUST_HDF_VERSION
    case "x_${USE_MED_VERSION}_x" in
	"x_3.2_x") TRUST_HDF_VERSION="1.8"                ;;
	"x_2.3_x") TRUST_HDF_VERSION="1.6"                ;;
	"x__x")    TRUST_HDF_VERSION="1.8"                ;;
	*)         raise_invalid_USE_MED_VERSION_exception ;;
    esac

    # exporting TRUST_HDF_LOCATION and TRUST_HDF_VERSION
    export TRUST_HDF_LOCATION
    export TRUST_HDF_VERSION
}

# checking external HDF5's version
check_external_HDF_version() {
    # defining TRUST_HDF_LOCATION
    TRUST_HDF_LOCATION="external"

    # checking the definition of HDF5HOME
    test "x_${EXTERNAL_HDF_HOME}_x" != "x__x" || raise_undefined_EXTERNAL_HDF_HOME_exception
    
    # checking that HDF5HOME points to a valid directory
    test -d "${EXTERNAL_HDF_HOME}" || raise_invalid_external_HDF_installation_directory_exception

    # checking that directory HDF5HOME contains installation directories of HDF5
    # test -d "${EXTERNAL_HDF_HOME}/bin"     || raise_invalid_external_HDF_installation_directory_exception
    test -d "${EXTERNAL_HDF_HOME}/include" || raise_invalid_external_HDF_installation_directory_exception
    test -d "${EXTERNAL_HDF_HOME}/lib"     || raise_invalid_external_HDF_installation_directory_exception

    # checking that external HDF5 library exists
    test -f "${EXTERNAL_HDF_HOME}/lib/libhdf5.a" || test -f "${EXTERNAL_HDF_HOME}/lib/libhdf5.so" || raise_invalid_external_HDF_installation_directory_exception

    # checking that external HDF5 header exists
    HDF5_header="${EXTERNAL_HDF_HOME}/include/H5public.h"
    test -f "${HDF5_header}" || raise_invalid_external_HDF_installation_directory_exception

    # defining TRUST_HDF_VERSION
    external_HDF_major_version_number=`grep "#define H5_VERS_MAJOR" ${HDF5_header} | ${TRUST_Awk} '{print $3}'`
    external_HDF_minor_version_number=`grep "#define H5_VERS_MINOR" ${HDF5_header} | ${TRUST_Awk} '{print $3}'`
    TRUST_HDF_VERSION="${external_HDF_major_version_number}.${external_HDF_minor_version_number}"

    test "x_${TRUST_HDF_VERSION}_x" = "x_1.8_x" || test "x_${TRUST_HDF_VERSION}_x" = "x_1.6_x" || raise_invalid_external_HDF_version_exception

    # exporting TRUST_HDF_LOCATION and TRUST_HDF_VERSION
    export TRUST_HDF_LOCATION
    export TRUST_HDF_VERSION
}

# checking HDF5's version
check_HDF_version() {
    test "x_${USE_EXTERNAL_HDF}_x" = "x__x" && check_internal_HDF_version || check_external_HDF_version    
}

# checking internal MED's version
check_internal_MED_version() {
    # defining TRUST_MED_LOCATION
    TRUST_MED_LOCATION="internal"

    # defining TRUST_MED_VERSION
    case "x_${USE_MED_VERSION}_x" in
	"x_3.2_x") TRUST_MED_VERSION="3.2"                ;;
	"x_2.3_x") TRUST_MED_VERSION="2.3"                ;;
	"x__x")    TRUST_MED_VERSION="3.2"                ;;
	*)         raise_invalid_USE_MED_VERSION_exception ;;
    esac

    # exporting TRUST_MED_LOCATION and TRUST_MED_VERSION
    export TRUST_MED_LOCATION
    export TRUST_MED_VERSION
}


# checking external MED's version
check_external_MED_version() {
    # defining TRUST_MED_LOCATION
    TRUST_MED_LOCATION="external"

    # checking the definition of MED2HOME
    test "x_${EXTERNAL_MED_HOME}_x" != "x__x" || raise_undefined_EXTERNAL_MED_HOME_exception
    
    # checking that MED2HOME points to a valid directory
    test -d "${EXTERNAL_MED_HOME}" || raise_invalid_external_MED_installation_directory_exception

    # checking that directory MED2HOME contains installation directories of MED
    test -d "${EXTERNAL_MED_HOME}/bin"     || raise_invalid_external_MED_installation_directory_exception
    test -d "${EXTERNAL_MED_HOME}/include" || raise_invalid_external_MED_installation_directory_exception
    test -d "${EXTERNAL_MED_HOME}/lib"     || raise_invalid_external_MED_installation_directory_exception

    # checking that external MED library exists
    test -f "${EXTERNAL_MED_HOME}/lib/libmed.a" || test -f "${EXTERNAL_MED_HOME}/lib/libmed.so" || raise_invalid_external_MED_installation_directory_exception

    # checking that external MED header exists
    MED_header="${EXTERNAL_MED_HOME}/include/med.h"
    test -f "${MED_header}"  || raise_invalid_external_MED_installation_directory_exception

    # defining TRUST_MED_VERSION
    grep "MED_MAJOR_NUM" ${MED_header} 1>/dev/null 2>&1
    if test "$?" != "0"
    then
	external_MED_major_version_number=`grep "#define MED_NUM_MAJEUR" ${MED_header} | ${TRUST_Awk} '{print $3}'`
	external_MED_minor_version_number=`grep "#define MED_NUM_MINEUR" ${MED_header} | ${TRUST_Awk} '{print $3}'`
    else
	external_MED_major_version_number=`grep "#define MED_MAJOR_NUM" ${MED_header} | ${TRUST_Awk} '{print $3}'`
	external_MED_minor_version_number=`grep "#define MED_MINOR_NUM" ${MED_header} | ${TRUST_Awk} '{print $3}'`
    fi
    TRUST_MED_VERSION="${external_MED_major_version_number}.${external_MED_minor_version_number}"

    # test "x_${TRUST_MED_VERSION}_x" = "x_3.2_x" || test "x_${TRUST_MED_VERSION}_x" = "x_2.3_x" || raise_invalid_external_MED_version_exception 

    # exporting TRUST_MED_LOCATION and TRUST_MED_VERSION
    export TRUST_MED_LOCATION
    export TRUST_MED_VERSION
}


# checking MED's version from environment variables
check_MED_version() {
    test "x_${USE_EXTERNAL_MED}_x" = "x__x" && check_internal_MED_version || check_external_MED_version
}

# checking compatibility between HDF5 and MED versions
check_versions_compatibility() {
    # checking that we do not use an external version of MED with an internal version of HDF
    test "x_${TRUST_HDF_LOCATION}_${TRUST_MED_LOCATION}_x" != "x_internal_external_x" || raise_internal_HDF_external_MED_exception
    
    # checking that HDF5 version corresponds to USE_MED_VERSION
    case "x_${USE_MED_VERSION}_${TRUST_HDF_VERSION}_x" in
	"x_3.2_1.8_x")                                                                                              ;;
	"x_2.3.2.6_x")                                                                                              ;;
	"x__1.8_x")                                                                                                 ;;
	"x__1.6_x")                                                                                                 ;;
	*) raise_HDF_version_incompatible_with_specification_exception "${USE_MED_VERSION}" "${TRUST_HDF_VERSION}" ;;
    esac

    # checking that MED version corresponds to USE_MED_VERSION
    case "x_${USE_MED_VERSION}_${TRUST_MED_VERSION}_x" in
	"x_3.2_3.2_x")                                                                                              ;;
	"x_2.3_2.3_x")                                                                                              ;;
	"x__3.2_x")                                                                                                 ;;
	"x__2.3_x")                                                                                                 ;;
	*) raise_MED_version_incompatible_with_specification_exception "${USE_MED_VERSION}" "${TRUST_MED_VERSION}" ;;
    esac

    # checking that MED and HDF versions are compatible
    case "x_${TRUST_HDF_VERSION}_${TRUST_MED_VERSION}_x" in
	"x_1.8_3.2_x")                                                                                   ;;
	"x_1.6_2.3_x")                                                                                   ;;
	*) raise_incompatible_MED_HDF_versions_exception "${TRUST_MED_VERSION}" "${TRUST_HDF_VERSION}" ;;
    esac
}
