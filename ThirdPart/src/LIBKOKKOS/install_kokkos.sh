#!/bin/sh

archive=$TRUST_ROOT/externalpackages/kokkos/kokkos-3.7.02.tgz
# Attention 4.x C++17 pour TRUST GPU mais necessaire pour nvc++ -cuda
[ "$TRUST_USE_OPENMP" = 1 ] && archive=$TRUST_ROOT/externalpackages/kokkos/kokkos-4.2.00.tgz
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
      for CMAKE_BUILD_TYPE in Release Debug
      do
        rm -rf BUILD
        mkdir -p BUILD
        cd BUILD
        CMAKE_OPT="-DCMAKE_CXX_COMPILER=$TRUST_CC_BASE -DCMAKE_CXX_FLAGS=-fPIC"
        # We try to use nvc++ for Cuda to mix Kokkos with OpenMP:
        [ "$TRUST_USE_CUDA" = 1 ] && [ "$TRUST_USE_OPENMP" = 1 ] && CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_IMPL_NVHPC_AS_DEVICE_COMPILER=ON -DKokkos_ENABLE_CUDA_RELOCATABLE_DEVICE_CODE=ON"
        [ "$TRUST_USE_CUDA" = 1 ]                                && CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_CUDA=ON -DKokkos_ENABLE_CUDA_LAMBDA=ON"
        [ "$TRUST_USE_ROCM" = 1 ]                                && CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_HIP=ON" && echo "ToDo and test" && exit -1
        CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_EXAMPLES=ON"
        CMAKE_INSTALL_PREFIX=$KOKKOS_ROOT_DIR/$TRUST_ARCH`[ $CMAKE_BUILD_TYPE = Release ] && echo _opt`
        CMAKE_OPT="$CMAKE_OPT -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$CMAKE_INSTALL_PREFIX"
        # Activation de check supplementaires au run-time en mode debug:
        if [ $CMAKE_BUILD_TYPE = Debug ]
        then
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_DEBUG_BOUNDS_CHECK=ON"           # Use bounds checking: increase run time
           CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_DEBUG_DUALVIEW_MODIFY_CHECK=ON"  # Debug check on dual views
        fi
        # Autres options possibles: See https://kokkos.github.io/kokkos-core-wiki/keywords.html#cmake-keywords
        echo "cmake ../kokkos $CMAKE_OPT" >$log_file
        cmake ../kokkos $CMAKE_OPT 1>>$log_file 2>&1 
        [ $? != 0 ] && echo "Error when configuring Kokkos (CMake) - look at $log_file" && exit -1

        make -j$TRUST_NB_PHYSICAL_PROCS install 1>>$log_file 2>&1
        [ $? != 0 ] && echo "Error when compiling Kokkos - look at $log_file" && exit -1
        echo "Kokkos $CMAKE_BUILD_TYPE installed under $CMAKE_INSTALL_PREFIX"
        cd ..
      done
      # Cree liens pour autres builds
      (cd $KOKKOS_ROOT_DIR;
      for build in _opt_gcov _opt_pg
      do
         ln -s -f $TRUST_ARCH"_opt" $TRUST_ARCH$build
      done
      for build in _semi_opt _pg _gcov
      do
         ln -s -f $TRUST_ARCH $TRUST_ARCH$build
      done
      )
      # Clean build:
      rm -rf $build_dir
    )   
    #echo "# Kokkos OK! Path to Kokkos is: $KOKKOS_ROOT_DIR"
else
    echo "# Kokkos: already installed. Doing nothing."
fi

