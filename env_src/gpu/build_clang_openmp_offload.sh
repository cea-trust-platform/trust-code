#!/bin/bash
# Download and build clang compiler with OpenMP offload
# See https://gist.github.com/anjohan/9ee746295ea1a00d9ca69415f40fafc9 !
INSTALL_PREFIX=`pwd`
nproc=8
[ ! -d llvm-project ] && git clone https://github.com/llvm/llvm-project.git
cd llvm-project && git checkout llvmorg-15.0.0

# Definition
export PATH=$INSTALL_PREFIX/clang/bin:$PATH
export LD_LIBRARY_PATH=$INSTALL_PREFIX/clang/lib:$LD_LIBRARY_PATH

test=`dirname $0`/test_clang_openmp_offload.sh
$test
if [ $? != 0 ]
then
   echo "Test fail, we rebuild clang..."
   rm -r -f build build2
   # First pass
   cc=61 # ToDo improve ?
   cc=86 # A5000
   mkdir -p build && cd build
   cmake ../llvm/ -DCMAKE_BUILD_TYPE=Release \
               -DCLANG_OPENMP_NVPTX_DEFAULT_ARCH=sm_$cc \
               -DLIBOMPTARGET_NVPTX_COMPUTE_CAPABILITIES=$cc \
               -DLLVM_TARGETS_TO_BUILD="X86;NVPTX" \
               -DLLVM_ENABLE_PROJECTS="clang;openmp"
   make -j$nproc

   # Second pass
   cd ..
   mkdir build2
   cd build2
   CC=../build/bin/clang CXX=../build/bin/clang++ cmake ../llvm/ -DCMAKE_BUILD_TYPE=Release \
                                                              -DCLANG_OPENMP_NVPTX_DEFAULT_ARCH=sm_$cc \
                                                              -DLIBOMPTARGET_NVPTX_COMPUTE_CAPABILITIES=$cc \
                                                              -DLLVM_TARGETS_TO_BUILD="X86;NVPTX" \
                                                              -DLLVM_ENABLE_PROJECTS="clang;openmp" \
                                                              -DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX/clang
   make -j$nproc
   make install

fi
