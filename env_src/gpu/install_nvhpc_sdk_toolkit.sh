#!/bin/bash
# NVIDIA HPC SDK
SDK_VERSION=22.9 && CUDA_VERSION=11.7
installer=nvhpc_2022_229_Linux_x86_64_cuda_$CUDA_VERSION
[ "$TRUST_TMP" = "" ] && TRUST_TMP="."
NVHPC=$TRUST_TMP/nvhpc-$SDK_VERSION/Linux_x86_64/$SDK_VERSION/compilers
if [ ! -f $NVHPC/bin/nvc++ ]
then
   echo "It may take some minutes to download NVidia HPC SDK $SDK_VERSION..."
   (
   cd $TRUST_TMP
   if [ ! -f $installer.tar.gz ]
   then
      wget https://developer.download.nvidia.com/hpc-sdk/$SDK_VERSION/$installer.tar.gz 1>/dev/null 2>&1 
      [ $? != 0 ] && echo "Error when downloading. See https://developer.nvidia.com/nvidia-hpc-sdk-releases" && exit -1
   fi
   echo "It may take some minutes to install NVidia HPC SDK..."
   tar xpzf $installer.tar.gz && rm -f $installer.tar.gz
   echo -e "\n1\n$TRUST_TMP/nvhpc-$SDK_VERSION\n" | $installer/install
   [ $? != 0 ] && echo "Error during NVidia HPC SDK install under $NVHPC" && exit -1
   rm -r -f $installer
   )
fi
