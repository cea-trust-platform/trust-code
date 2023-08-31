#!/bin/sh

archive=$TRUST_ROOT/externalpackages/kokkos/kokkos-3.7.02.tgz
build_dir=$TRUST_ROOT/build/kokkos
KOKKOS_ROOT_DIR=$TRUST_ROOT/env/gpu/kokkos
# Log file of the process:
log_file=$TRUST_ROOT/kokkos_compile.log

if [ ! -f $KOKKOS_ROOT_DIR/bin/nvcc_wrapper ]; then
    echo "# Installing Kokkos ..."
    # Sub shell to get back to correct dir:
    (  
        mkdir -p $build_dir

        cd $build_dir
        tar xzf $archive || exit -1
        src_dir=$build_dir/kokkos

        mkdir -p BUILD
        cd BUILD

        CMAKE_OPT="-DCMAKE_CXX_COMPILER=$src_dir/bin/nvcc_wrapper"   # For now take wrapper as in the sources of Kokkos - later will used installed one.
        CMAKE_OPT="$CMAKE_OPT -DKokkos_ENABLE_CUDA=ON "
        CMAKE_OPT="$CMAKE_OPT -DCMAKE_INSTALL_PREFIX=$KOKKOS_ROOT_DIR -DKokkos_ENABLE_CUDA_LAMBDA=ON"
        echo "cmake ../kokkos $CMAKE_OPT" >$log_file
        cmake ../kokkos $CMAKE_OPT 1>>$log_file 2>&1 
        [ $? != 0 ] && echo "Error when configuring Kokkos (CMake) - look at $log_file" && exit -1

        make -j$TRUST_NB_PHYSICAL_PROCS install  1>>$log_file 2>&1
        [ $? != 0 ] && echo "Error when compiling Kokkos - look at $log_file" && exit -1
       
        # Clean build:
        rm -rf $build_dir
    )   
    echo "# Kokkos OK! Path to nvcc_wrapper is: $KOKKOS_ROOT_DIR/bin/nvcc_wrapper"
else
    echo "# Kokkos: already installed. Doing nothing."
fi

