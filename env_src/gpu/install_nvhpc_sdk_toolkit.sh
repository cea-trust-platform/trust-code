#!/bin/bash
# NVIDIA HPC SDK
SDK_VERSION=22.9 && CUDA_VERSION=11.7 && installer=nvhpc_2022_229_Linux_x86_64_cuda_$CUDA_VERSION  # 11.7 uniquement
SDK_VERSION=22.2 && CUDA_VERSION=11.6 && installer=nvhpc_2022_222_Linux_x86_64_cuda_multi # 11.6, 11.2, 11.0, 10.2 
SDK_VERSION=22.1 && CUDA_VERSION=11.5 && installer=nvhpc_2022_221_Linux_x86_64_cuda_multi # 11.5, 11.0, 10.2 (nvcc 11.5 OK pour gcc 11.0 de Fedora 34)
INSTALL=$TRUST_ROOT/env/gpu/install
NVHPC=$INSTALL/nvhpc-$SDK_VERSION/Linux_x86_64/$SDK_VERSION/compilers
if [ ! -f $NVHPC/bin/nvc++ ]
then
   echo "It may take some minutes to download NVidia HPC SDK $SDK_VERSION..."
   (
   if [ ! -f $TRUST_TMP/$installer.tar.gz ]
   then
      cd $TRUST_TMP
      wget https://developer.download.nvidia.com/hpc-sdk/$SDK_VERSION/$installer.tar.gz 1>/dev/null 2>&1 
      [ $? != 0 ] && echo "Error when downloading. See https://developer.nvidia.com/nvidia-hpc-sdk-releases" && exit -1
   fi
   mkdir -p $INSTALL && cd $INSTALL
   echo "It may take some minutes to install NVidia HPC SDK..."
   tar xpzf $TRUST_TMP/$installer.tar.gz && rm -f $installer.tar.gz
   echo -e "\n1\n$INSTALL/nvhpc-$SDK_VERSION\n" | $installer/install
   [ $? != 0 ] && echo "Error during NVidia HPC SDK install under $NVHPC" && exit -1
   #rm -r -f $installer
   )
fi
