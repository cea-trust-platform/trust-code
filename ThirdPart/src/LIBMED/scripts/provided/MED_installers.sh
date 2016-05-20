#! /bin/bash
#
# --
# MED's installation functions
#
# Author : Eli LAUCOIN (CEA)
# --


# removing any MED previous installation
uninstall_MED() {
    library=${TRUST_MED_ROOT}/lib/libmed.a
    test ! -e "${library}"  && library=${TRUST_MED_ROOT}/lib/libmed.so
    if test -f "${library}" && test -d "${TRUST_MED_HDF_INSTALL}/MED"
    then
	uninstall_package MED
    fi

    rm -rf "${TRUST_ROOT}/lib/libmed.a"
}

generate_MED_report() {
    MED_version="${1}"
    MED_location="${2}"
    HDF_version="${3}"
    HDF_location="${4}"

    ok="yes"

    # checking that libmed.a exists in installation directory
    library=${TRUST_MED_ROOT}/lib/libmed.a
    test ! -e "${library}"  && library=${TRUST_MED_ROOT}/lib/libmed.so
    if test "x_${ok}_x" = "x_yes_x" && test -e "${library}"
    then
	ok="yes"
    else
	ok="no"
    fi

    # checking that med.h exists in installation directory
    if test "x_${ok}_x" = "x_yes_x" && test -e "${TRUST_MED_HDF_INSTALL}/MED/include/med.h"
    then
	ok="yes"
    else
	ok="no"
    fi

    # checking that libmed.a exists in TRUST_MED_ROOT directory
    if test "x_${ok}_x" = "x_yes_x" && test -e "${library}"
    then
	ok="yes"
    else
	ok="no"
    fi

    # checking that med.h exists in TRUST_MED_ROOT directory
    if test "x_${ok}_x" = "x_yes_x" && test -e "${TRUST_MED_ROOT}/include/med.h"
    then
	ok="yes"
    else
	ok="no"
    fi

    # generating report
    if test "x_${ok}_x" = "x_yes_x"
    then
	if test "x_${MED_location}_x" = "x_external_x"
        then
	    MED_location="${MED_location} (${EXTERNAL_MED_HOME})"
	fi

	if test "x_${HDF_location}_x" = "x_external_x"
        then
	    hdf_location="${HDF_location} (${EXTERNAL_HDF_HOME})"
	fi

	echo "MED has been successfully installed" >  "${TRUST_MED_HDF_INSTALL}/MED/report.txt"
	echo ""                                    >> "${TRUST_MED_HDF_INSTALL}/MED/report.txt"
	echo "MED  version  : ${MED_version}"      >> "${TRUST_MED_HDF_INSTALL}/MED/report.txt"
	echo "MED  location : ${MED_location}"     >> "${TRUST_MED_HDF_INSTALL}/MED/report.txt"
	echo "HDF5 version  : ${HDF_version}"      >> "${TRUST_MED_HDF_INSTALL}/MED/report.txt"
	echo "HDF5 location : ${HDF_location}"     >> "${TRUST_MED_HDF_INSTALL}/MED/report.txt"
    else
	uninstall_MED
	raise_invalid_MED_report_generation_exception
    fi
}

# MED installation procedure
install_MED() {
    MED_version="${1}"
    MED_location="${2}"
    HDF_version="${3}"
    HDF_location="${4}"

    echo "installing ${MED_location} MED ${MED_version} with ${HDF_location} HDF5 ${HDF_version}"

    # defining several useful variables 
    TRUST_MED_HDF_BUILD="${TRUST_MED_HDF_ROOT}/build"
    TRUST_MED_HDF_INSTALL="${TRUST_MED_HDF_ROOT}/install"
    TRUST_MED_HDF_ARCHIVES="${TRUST_MED_HDF_SRC}/archives"
    export TRUST_MED_HDF_BUILD
    export TRUST_MED_HDF_INSTALL
    export TRUST_MED_HDF_ARCHIVES
    
    # Python cause MED 3.2.7 build fail with some incomplete Python install 
    env=$TRUST_ROOT/exec/python/env.sh
    #[ -f $env ] && source $env
    # Vu sur Ubuntu 12.04 (is213120) sh pointe vers un dash et non bash, donc le makefile faisant sh
    # source n'est alors pas reconnu...
    [ -f $env ] && . $env

    # removing any previous MED installation
    uninstall_MED

    # initializing installation
    initialize_installation

    if test "${MED_location}" = "internal"
    then
	# defining the MED archive
	case "x_${MED_version}_x" in
	    "x_3.2_x") MED_archive="med-3.2.0"     ;;
	    "x_2.3_x") MED_archive="med-2.3.6"     ;;
	    *) raise_invalid_MED_version_exception ;;
	esac

	# defining HDF5HOME (saving its previous value if needed)
	if test "x_${HDF5HOME}_x" != "x__x"
	then
	    TRUST_OLD_HDF5HOME=${HDF5HOME}
	    export TRUST_OLD_HDF5HOME
	fi
	HDF5HOME="${TRUST_MED_ROOT}"
	export HDF5HOME
        # defining LIBS (problem on Ubuntu 11.10)
        LIBS="-lm"
        # Add -lz (problem on curie)
        [ "`ls /usr/*/libz.so 2>/dev/null`" != "" ] && LIBS=$LIBS" -lz"
	# cygwin
        [ "`ls /lib/libz.dll.a 2>/dev/null`" != "" ] && LIBS=$LIBS" -lz"
        export LIBS

	# configuring MED
	configure_package "MED" "${MED_version}" "${MED_archive}"

        # building MED
        build_package "${MED_archive}"

        # installing MED
	install_package "MED" "${MED_version}" "${MED_archive}"

        # activating MED in TRUST_MED_ROOT directory
	activate_package "${TRUST_MED_HDF_INSTALL}/MED" "${TRUST_MED_ROOT}"

	# restoring previous definition of HDF5HOME if needed
	if test "x_${TRUST_OLD_HDF5HOME}_x" != "x__x"
	then
	    HDF5HOME="${TRUST_OLD_HDF5HOME}"
	    export HDF5HOME
	    unset TRUST_OLD_HDF5HOME
	fi

    elif test "${MED_location}" = "external"
    then
        # activating MED in installation directory
	activate_package "${EXTERNAL_MED_HOME}" "${TRUST_MED_HDF_INSTALL}/MED"

        # activating MED in TRUST_MED_ROOT directory
	activate_package "${EXTERNAL_MED_HOME}" "${TRUST_MED_ROOT}"
    else
	raise_invalid_MED_location_exception
    fi
    # to force rebuild dependencies
    touch ${TRUST_MED_ROOT}/include/med.h
    # finalizing installation
    finalize_installation

    # generating report on MED installation
    generate_MED_report  "${MED_version}" "${MED_location}" "${HDF_version}" "${HDF_location}"
}
