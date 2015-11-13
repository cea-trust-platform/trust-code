#! /bin/bash
#
# --
# HDF5's installation functions
#
# Author : Eli LAUCOIN (CEA)
# --

# removing any HDF5 previous installation
uninstall_HDF() {
    if test -f "${TRUST_MED_ROOT}/lib/libhdf5.a" && test -d "${TRUST_MED_HDF_INSTALL}/HDF"
    then
	uninstall_package HDF
    fi

    #################################################################
    # J'ai pas bien compris a quoi ca sert...

    # rm -rf "${TRUST_ROOT}MEDimpl/hdf5.h"

    # Le uninstall_package ne suffit pas donc je rajoute ceci (PL):
    #cd `dirname ${TRUST_MED_ROOT}`;make clean
    ##################################################################
}

# generating report on HDF5 installation
generate_HDF_report() {
    HDF_version="${1}"
    HDF_location="${2}"

    ok="yes"

    # checking that libhdf5.a exists in installation directory
    if test "x_${ok}_x" = "x_yes_x" && test -e "${TRUST_MED_HDF_INSTALL}/HDF/lib/libhdf5.a"
    then
	ok="yes"
    else
	ok="no"
    fi

    # checking that libhdf5.a exists in TRUST_MED_ROOT directory
    if test "x_${ok}_x" = "x_yes_x" && test -e "${TRUST_MED_ROOT}/lib/libhdf5.a"
    then
	ok="yes"
    else
	ok="no"
    fi

    # generating report
    if test "x_${ok}_x" = "x_yes_x"
    then

	if test "x_${HDF_location}_x" = "x_external_x"
        then
	    HDF_location="${HDF_location} (${EXTERNAL_HDF_HOME})"
	fi

	echo "HDF5 has been successfully installed" >  "${TRUST_MED_HDF_INSTALL}/HDF/report.txt"
	echo ""                                     >> "${TRUST_MED_HDF_INSTALL}/HDF/report.txt"
	echo "HDF5 version  : ${HDF_version}"       >> "${TRUST_MED_HDF_INSTALL}/HDF/report.txt"
	echo "HDF5 location : ${HDF_location}"      >> "${TRUST_MED_HDF_INSTALL}/HDF/report.txt"	
    else
	uninstall_HDF
	raise_invalid_HDF_report_generation_exception
    fi
}

# HDF5 installation procedure
install_HDF() {
    HDF_version="${1}"
    HDF_location="${2}"
    
    echo "installing ${HDF_location} HDF5 ${HDF_version}"

    # defining several useful variables 
    TRUST_MED_HDF_BUILD="${TRUST_MED_HDF_ROOT}/build"
    TRUST_MED_HDF_INSTALL="${TRUST_MED_HDF_ROOT}/install"
    TRUST_MED_HDF_ARCHIVES="${TRUST_MED_HDF_SRC}/archives"
    export TRUST_MED_HDF_BUILD
    export TRUST_MED_HDF_INSTALL
    export TRUST_MED_HDF_ARCHIVES

    # removing any previous HDF5 installation
    uninstall_HDF

    # initializing installation
    initialize_installation

    if test "${HDF_location}" = "internal"
    then
	# defining the HDF archive
	case "x_${HDF_version}_x" in
	    "x_1.8_x") HDF_archive="hdf5-1.8.14"     ;;
	    "x_1.6_x") HDF_archive="hdf5-1.6.9"     ;;
	    *) raise_invalid_HDF_version_exception ;;
	esac

	# configuring HDF
	configure_package "HDF" "${HDF_version}" "${HDF_archive}"

        # building HDF
        build_package "${HDF_archive}"

        # installing HDF
	install_package "HDF" "${HDF_version}" "${HDF_archive}"

        # activating HDF in TRUST_MED_ROOT directory
	activate_package "${TRUST_MED_HDF_INSTALL}/HDF" "${TRUST_MED_ROOT}"

    elif test "${HDF_location}" = "external"
    then
        # activating HDF in installation directory
	activate_package "${EXTERNAL_HDF_HOME}" "${TRUST_MED_HDF_INSTALL}/HDF"

        # activating HDF in TRUST_MED_ROOT directory
	activate_package "${EXTERNAL_HDF_HOME}" "${TRUST_MED_ROOT}"
    else
	raise_invalid_HDF_location_exception
    fi

    # finalizing installation
    finalize_installation

    # generating report on HDF installation
    generate_HDF_report "${HDF_version}" "${HDF_location}"
}
