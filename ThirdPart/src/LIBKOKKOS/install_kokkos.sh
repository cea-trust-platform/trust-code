#!/bin/bash

if [ "$TRUST_USE_GPU" = 1 ] || [ "$TRUST_USE_KOKKOS_SIMD" = 1 ]
then
   # Kokkos pour SIMD ou GPU (C++17):
   #archive=$TRUST_ROOT/externalpackages/kokkos/kokkos-4.2.00.tar.gz
   #archive=$TRUST_ROOT/externalpackages/kokkos/kokkos-4.4.01.tar.gz
   archive=$TRUST_ROOT/externalpackages/kokkos/kokkos-4.5.00.tar.gz
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
      src_dir=$build_dir/`ls $build_dir | grep kokkos`

      # Set this flag to 1 to have Kokkos compiled/linked in Debug mode for $exec_debug or when developping on GPU:
      if [ $HOST = $TRUST_HOST_ADMIN ] || [ "$TRUST_USE_KOKKOS_SIMD" = 1 ]
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
        if [ "$TRUST_USE_CUDA" = 1 ]
        then
           CMAKE_OPT="-DCMAKE_CXX_COMPILER=$TRUST_CC_BASE_EXTP"
        elif [ "$TRUST_USE_ROCM" = 1 ]
        then
           CMAKE_OPT="-DCMAKE_CXX_COMPILER=hipcc" # $TRUST_CC_BASE pour profiter de ccache ?
        else # Serial
           CMAKE_OPT="-DCMAKE_CXX_COMPILER=$TRUST_CC_BASE"
           # Optimisations pour Serial (important pour F5,C3D)
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_ATOMICS_BYPASS=ON" # Serial build (skips mutexes to remediate performance regression)
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_LIBDL=OFF" # Disable Kokkos tools (profiling and fence) so 2 hacks useless:
           #sed -i '/bool profileLibraryLoaded() {/,/}/c\bool profileLibraryLoaded() \/* Disabled \*/ { return false; }' $src_dir/core/src/impl/Kokkos_Profiling.cpp || exit -1 # Disable profiling
           #sed -i '/Kokkos::Tools::Experimental::Impl::profile_fence_event<Kokkos::Serial>(/,/Kokkos::memory_fence();/d' $src_dir/core/src/Serial/Kokkos_Serial.hpp || exit -1 # Disable fence for serial without thread
        fi
        CMAKE_OPT="$CMAKE_OPT -DCMAKE_CXX_FLAGS=-fPIC"
        # ARCH:
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
           elif [ "$TRUST_CUDA_CC" = 89 ]
           then
              CMAKE_OPT="$CMAKE_OPT -DKokkos_ARCH_ADA$TRUST_CUDA_CC=ON"
           elif [ "$TRUST_CUDA_CC" = 90 ]
           then
              CMAKE_OPT="$CMAKE_OPT -DKokkos_ARCH_HOPPER$TRUST_CUDA_CC=ON"
           else
              echo "KOKKOS_ARCH not set cause TRUST_CUDA_CC=$TRUST_CUDA_CC unknown!" && exit -1
           fi
        elif [ "$TRUST_USE_ROCM" = 1 ]
        then
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_HIP=ON"
           #CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_HIP_MULTIPLE_KERNEL_INSTANTIATIONS=ON" # faster but slow build (nb: no gain on kernels.sh with gfx1100)
           CMAKE_OPT="$CMAKE_OPT -DCMAKE_CXX_STANDARD=17"
           if [ "$ROCM_ARCH" = gfx1030 ]
           then
              CMAKE_OPT="$CMAKE_OPT -DKokkos_ARCH_AMD_GFX1030=ON"
           elif [ "$ROCM_ARCH" = gfx1100 ]
           then
              CMAKE_OPT="$CMAKE_OPT -DKokkos_ARCH_AMD_GFX1100=ON"
           elif [ "$ROCM_ARCH" = gfx90a ]
           then
              CMAKE_OPT="$CMAKE_OPT -DKokkos_ARCH_AMD_GFX90A=ON" # MI250
           elif [ "$ROCM_ARCH" = gfx942 ]
           then
              CMAKE_OPT="$CMAKE_OPT -DKokkos_ARCH_AMD_GFX942_APU=ON" # MI300
           else
              echo "KOKKOS_ARCH not set cause $ROCM_ARCH unknown!" && exit -1
           fi
        fi
        [ "$TRUST_USE_KOKKOS_OPENMP" = 1 ] && CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_OPENMP=ON"	# Backend OpenMP au lieu de serial
        # On ne construit les examples que la ou cela marche...
        [ "$TRUST_USE_ROCM" != 1 ] && [ "$TRUST_CUDA_CC" != 70 ] && CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_EXAMPLES=ON"
        CMAKE_INSTALL_PREFIX=$KOKKOS_ROOT_DIR/$TRUST_ARCH`[ $CMAKE_BUILD_TYPE = Release ] && echo _opt`
        CMAKE_OPT="$CMAKE_OPT -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$CMAKE_INSTALL_PREFIX -DCMAKE_INSTALL_LIBDIR=lib64"
        # Activation de check supplementaires au run-time en mode debug:
        if [ $CMAKE_BUILD_TYPE = Debug ]
        then
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_DEBUG_BOUNDS_CHECK=ON"           # Use bounds checking: increase run time
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_DEBUG_DUALVIEW_MODIFY_CHECK=ON"  # Debug check on dual views
        else
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_DEPRECATION_WARNINGS=OFF" # Optimisation sur petits kernels sinon std::string crees dans Kokkos::check_conversion_safety !
	   if [ "`grep avx2 /proc/cpuinfo 2>/dev/null`" != "" ]
	   then
              CMAKE_OPT="$CMAKE_OPT -DKOKKOS_ARCH_AVX2=ON" # only work if TRUST is also compiled with avx option (such as mavx2), not the case in debug mode
	   else
	      CMAKE_OPT="$CMAKE_OPT -DKOKKOS_ARCH_AVX=ON"
	   fi
        fi
        # Autres options possibles: See https://kokkos.github.io/kokkos-core-wiki/keywords.html#cmake-keywords
        # et ici aussi: https://kokkos.org/kokkos-core-wiki/API/core/Macros.html
        echo "cmake $src_dir $CMAKE_OPT" | tee $log_file
        cmake $src_dir $CMAKE_OPT 2>&1 | tee -a $log_file
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

