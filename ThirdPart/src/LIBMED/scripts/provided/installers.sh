#! /bin/bash
#
# --
# Generic installation functions
#
# Author : Eli LAUCOIN (CEA)
# --


# checking compilers
check_compilers() {
    # defining CC
    CC=${TRUST_cc_BASE}
    export CC

    # defining F77
    F77=${TRUST_F77_BASE}
    export F77

    # defining CXX
    CXX=${TRUST_CC_BASE}
    export CXX

    # defining FC
    FC=${TRUST_F77_BASE}
    export FC
    
    # Probleme vu a la compilation de MED si la variable d'environnement MV est definie
    unset MV
}

# removing any specified package
uninstall_package() {
    package="${1}"
    here=`pwd`

    for directory in bin include lib
      do
      cd "${TRUST_MED_HDF_INSTALL}/${package}/${directory}"
      for f in `ls`
	do
	rm -f "${TRUST_MED_ROOT}/${directory}/${f}"
      done
    done
    cd "${here}"
    rm -rf "${TRUST_MED_HDF_INSTALL}/${package}"
}

# initializing installation
initialize_installation() {

    # checking compilers
    check_compilers

    # creating a build directory
    if test ! -d "${TRUST_MED_HDF_BUILD}"
    then 
	mkdir -p "${TRUST_MED_HDF_BUILD}"
    fi
}

# configuring any specified package
configure_package() {
    name="${1}"
    version="${2}"
    archive="${3}"

    here=`pwd`

    # extracting the archive
    tgzarchive="${archive}.tar.gz"
    tararchive="${archive}.tar"
    gunzip -c "${TRUST_MED_HDF_ARCHIVES}/${tgzarchive}" > "${TRUST_MED_HDF_BUILD}/${tararchive}"
    cd "${TRUST_MED_HDF_BUILD}"
    tar xf "${TRUST_MED_HDF_BUILD}/${tararchive}"
    cd "${TRUST_MED_HDF_BUILD}/${archive}"
    [ $? -ne 0 ] && echo pb with ${archive} && exit 1
    # Remplacement de workdir car utilise sur une machine (IBM vargas)
    # et met une erreur: workdir readonly variable...
    #workdir=`pwd`
    TRUST_MED_workdir=`pwd`

    # defining configure options
    configure_options="--prefix=${TRUST_MED_HDF_INSTALL}/${name}"
    case "x_${name}_${version}_x" in
	"x_HDF_1.6_x") configure_options="${configure_options} --disable-stream-vfd --enable-using-memchecker" ;;
	"x_HDF_1.8_x") configure_options="${configure_options} --disable-stream-vfd --enable-using-memchecker --enable-debug=yes  " ;;
	"x_MED_2.3_x") configure_options="${configure_options} --enable-static"                                ;;
	"x_MED_3.0_x") configure_options="${configure_options} --enable-static"                                ;;
	* ) raise_invalid_package_version_exception "${name}" "${version}"                                     ;;
    esac

    # defining compilation flags
    if test "x_${TRUST_ARCH}_x" = "x_linux_x"
    then 
       CFLAGS="${CFLAGS} -fPIC"
       CPPFLAGS="${CPPFLAGS} -fPIC"
    fi
    export CFLAGS
    export CPPFLAGS
    # Hack du configure pour MED (suppression de -lz)
    if [ "${name}" = MED ]
    then
       modified_file="${TRUST_MED_workdir}/configure"
       sed_ "${modified_file}" "s? -lz ? ?g"
       chmod +x ${TRUST_MED_workdir}/configure
    fi
    # Hack de Makefile.in pour MED (non creation de la doc)
    if [ "${name}" = MED ]
    then
       modified_file="${TRUST_MED_workdir}/Makefile.in"
       sed_ "${modified_file}" "s?tests tools doc?tools?g"
    fi

    # calling configure
    ${TRUST_MED_workdir}/configure ${configure_options} 1>configure_${archive}.log 2>&1 || exit -1

    # applying various corrections to generated files
    modified_file="${TRUST_MED_workdir}/bin/install-sh"
    if test -f "${modified_file}"
    then
	chmod +x "${modified_file}"
    fi

    modified_file="${TRUST_MED_workdir}/Makefile"
    if test -f "${modified_file}" && test "`grep 'INSTALL=bin/install-sh' ${modified_file}`" = ""
    then
	sed_ "${modified_file}" "s?../bin/install-sh?bin/install-sh?g"
    fi
    
    case "x_${name}_${version}_x" in
	"x_HDF_1.6_x")
	    # rien a faire
	    ;;
	
	"x_HDF_1.8_x")
            # rien a faire
	    ;;

	"x_MED_2.3_x")
	    modified_file="${TRUST_MED_workdir}/src/tools/misc/MED2cstring.c"
	    if test -f "${modified_file}"
	    then
		sed_ "${modified_file}" "s?return -1?return NULL?g"
	    fi

	    modified_file="${TRUST_MED_workdir}/tools/src/medconforme/Makefile"
	    if test -f "${modified_file}" && test "`grep 'INSTALL=../../../bin/install-sh' ${modified_file}`" = ""
	    then
		sed_ "${modified_file}" "s?../bin/install-sh?../../../bin/install-sh?g"
	    fi

	    modified_file="${TRUST_MED_workdir}/tools/src/mdump/Makefile"
	    if test -f "${modified_file}" && test "`grep 'INSTALL=../../../bin/install-sh' ${modified_file}`" = ""
	    then
		sed_ "${modified_file}" "s?../bin/install-sh?../../../bin/install-sh?g"
	    fi
	    ;;

	"x_MED_3.0_x")
	    # a priori rien a faire
	    ;;

	* ) raise_invalid_package_version_exception "${name}" "${version}" ;;
    esac
 
    cd "${here}"
}

# building any specified package
build_package() {
    archive="${1}"

    here=`pwd`

    # calling make
    cd "${TRUST_MED_HDF_BUILD}/${archive}"
    ${TRUST_MAKE} 1>make_${archive}.log 2>&1 || exit -1
    cd "${here}"
}

# installing any specified package
install_package() {
    name="${1}"
    version="${2}"
    archive="${3}"

    here=`pwd`

    # creating destination directory if needed
    rm -rf "${TRUST_MED_HDF_INSTALL}/${name}"
    mkdir -p "${TRUST_MED_HDF_INSTALL}/${name}"

    # calling make install
    cd "${TRUST_MED_HDF_BUILD}/${archive}"
    # ${TRUST_MAKE} make -j ne semble pas marcher si on ne fait pas les test med
    make  install 1>install_${archive}.log 2>&1 || exit -1

    # applying corrections for med
    if test "x_${name}_x" = "x_MED_x"
    then
	if test "x_${version}_x" = "x_2.3_x"
        then
	    for rep in `find . -name .libs`
	    do
	      cd "${rep}"
	      if test "`ls -l | grep *.o | grep -v ~$ | wc -l`" != "0"
	      then
		  ar cru ${TRUST_MED_HDF_INSTALL}/${name}/lib/libmed.a *.o
	      fi
	      cd "${TRUST_MED_workdir}"
	    done
	fi
	# TODO : faut-il propager les memes corrections pour med-3.0 ?
    fi

    cd "${here}"
}

# activating any specified package
activate_package() {
    package_directory="${1}"
    active_directory="${2}"
    here=`pwd`
    
    if test ! -d "${active_directory}"
    then
	mkdir -p "${active_directory}"
    fi

    for directory in bin include lib
    do
      if test ! -d "${active_directory}/${directory}" 
      then
	  mkdir "${active_directory}/${directory}"
      fi
    done

    for directory in bin include lib
    do
      cd "${active_directory}/${directory}"
      for f in `ls "${package_directory}/${directory}"`
      do 
	ln -s ../../install/`basename ${package_directory}`/${directory}/${f} .
      done
    done

    cd "${here}"
}

# finalizing installation
finalize_installation() {
    rm -rf "${TRUST_MED_HDF_BUILD}"
}
