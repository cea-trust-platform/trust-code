#!/bin/bash

if [ "$TRUST_USE_OPENMP" = 1 ] || [ "$TRUST_USE_KOKKOS_SIMD" = 1 ]
then
   # Kokkos pour SIMD ou GPU (C++17):
   archive=$TRUST_ROOT/externalpackages/kokkos/kokkos-4.2.00.tgz
else
   # Kokkos Serial (C++14)
   archive=$TRUST_ROOT/externalpackages/kokkos/kokkos-3.7.02.tgz
fi

build_dir=$TRUST_ROOT/build/kokkos
KOKKOS_ROOT_DIR=$TRUST_ROOT/lib/src/LIBKOKKOS
# Log file of the process:
log_file=$TRUST_ROOT/kokkos_compile.log

if [ ! -f $KOKKOS_ROOT_DIR/lib64/libkokkos.a ]; then
    echo "# Installing `basename $archive` ..."
    # Sub shell to get back to correct dir:
    (  
      mkdir -p $build_dir

      cd $build_dir
      tar xzf $archive || exit -1
      src_dir=$build_dir/kokkos
      
      # Set this flag to 1 to have Kokkos compiled/linked in Debug mode for $exec_debug or when developping on GPU:
      if [ $HOST = $TRUST_HOST_ADMIN ] || [ "$TRUST_USE_OPENMP" = 1 ]
      then
         build_debug=1
      else
         build_debug=$TRUST_ENABLE_KOKKOS_DEBUG
      fi
      BUILD_TYPES="Release `[ "$build_debug" = "1" ] && echo Debug`"
      for CMAKE_BUILD_TYPE in $BUILD_TYPES
      do
        rm -rf BUILD
        mkdir -p BUILD
        cd BUILD
	if [ "$TRUST_CC_BASE_EXTP" != "" ] && [ "$TRUST_USE_CUDA" = 1 ]
	then
           CMAKE_OPT="-DCMAKE_CXX_COMPILER=$TRUST_CC_BASE_EXTP"
	else
	   CMAKE_OPT="-DCMAKE_CXX_COMPILER=$TRUST_CC_BASE"
	   # To use nvc++ as device compiler (nvcc ~ nvc++ -gpu):
	   [ "`basename $TRUST_CC_BASE`" = nvc++ ] && CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_IMPL_NVHPC_AS_DEVICE_COMPILER=ON"
	fi    
	CMAKE_OPT="$CMAKE_OPT -DCMAKE_CXX_FLAGS=-fPIC"
        if [ "$TRUST_USE_CUDA" = 1 ]
        then
           # Kokkos use CUDA_ROOT pour trouver nvcc !!!!
           # Or le configure de TRUST fixe aussi CUDA_ROOT parfois a la racine $NVHPC et specifie parfois NVHPC_CUDA_HOME
           # C'est un peu le bazar tout cela...
           [ "$NVHPC_CUDA_HOME" != "" ] && export CUDA_ROOT=$NVHPC_CUDA_HOME
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_CUDA=ON -DKokkos_ENABLE_CUDA_LAMBDA=ON"
           # KOKKOS ARCH (Cuda Compute Capability):
           if [ "$TRUST_CUDA_CC" = 70 ]
           then
              CMAKE_OPT="$CMAKE_OPT -DKokkos_ARCH_VOLTA$TRUST_CUDA_CC=ON"
           elif [ "$TRUST_CUDA_CC" = 80 ] || [ "$TRUST_CUDA_CC" = 86 ]
           then
              CMAKE_OPT="$CMAKE_OPT -DKokkos_ARCH_AMPERE$TRUST_CUDA_CC=ON"
           else
              echo "KOKKOS_ARCH not set!" && exit -1
           fi
           # To mix Kokkos with OpenMP:
           [ "$TRUST_USE_OPENMP" = 1 ] && CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_CUDA_RELOCATABLE_DEVICE_CODE=ON"
        elif [ "$TRUST_USE_ROCM" = 1 ]
        then
           # Impossible de mixer HIP et OpenMP dans une meme translation unit, on utilise le backend OPENMPTARGET
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_HIP=ON -DCMAKE_CXX_STANDARD=17"
           #CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_HIP_MULTIPLE_KERNEL_INSTANTIATIONS" # faster but slow build
           #CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_OPENMPTARGET=ON -DCMAKE_CXX_STANDARD=17"
           [ "$ROCM_ARCH" = gfx90a ] && CMAKE_OPT="$CMAKE_OPT -DKokkos_ARCH_AMD_GFX90A=ON"
        fi
        # On ne construit les examples que la ou cela marche...
        [ "$TRUST_USE_ROCM" != 1 ] && [ "$TRUST_CUDA_CC" != 70 ] && CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_EXAMPLES=ON"
        CMAKE_INSTALL_PREFIX=$KOKKOS_ROOT_DIR/$TRUST_ARCH`[ $CMAKE_BUILD_TYPE = Release ] && echo _opt`
        CMAKE_OPT="$CMAKE_OPT -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$CMAKE_INSTALL_PREFIX -DCMAKE_INSTALL_LIBDIR=lib64"
        # Activation de check supplementaires au run-time en mode debug:
        if [ $CMAKE_BUILD_TYPE = Debug ]
        then
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_DEBUG_BOUNDS_CHECK=ON"           # Use bounds checking: increase run time
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_DEBUG_DUALVIEW_MODIFY_CHECK=ON"  # Debug check on dual views
        fi
        # Autres options possibles: See https://kokkos.github.io/kokkos-core-wiki/keywords.html#cmake-keywords
        echo "cmake ../kokkos $CMAKE_OPT" | tee $log_file
        cmake ../kokkos $CMAKE_OPT 2>&1 | tee -a $log_file
        [ ${PIPESTATUS[0]} != 0 ] && echo "Error when configuring Kokkos (CMake) - look at $log_file" && exit -1

        make -j$TRUST_NB_PHYSICAL_PROCS install 2>&1 | tee -a $log_file
        [ ${PIPESTATUS[0]} != 0 ] && echo "Error when compiling Kokkos - look at $log_file" && exit -1
        echo "Kokkos $CMAKE_BUILD_TYPE installed under $CMAKE_INSTALL_PREFIX"
        cd ..
      done
      # Cree liens pour autres builds
      (cd $KOKKOS_ROOT_DIR;
      for build in _opt_gcov _opt_pg
      do
         ln -s -f $TRUST_ARCH"_opt" $TRUST_ARCH$build
      done
      main_tgt=""
      [ "$build_debug" != "1" ] && main_tgt="_opt"
      for build in _semi_opt _pg _gcov
      do
         ln -s -f $TRUST_ARCH$main_tgt $TRUST_ARCH$build
      done
      if [ "$build_debug" != "1" ]; then
        ln -s -f $TRUST_ARCH"_opt" $TRUST_ARCH
      fi
      )
      # Clean build:
      rm -rf $build_dir $log_file
    )
    #echo "# Kokkos OK! Path to Kokkos is: $KOKKOS_ROOT_DIR"
else
    echo "# Kokkos: already installed. Doing nothing."
fi

