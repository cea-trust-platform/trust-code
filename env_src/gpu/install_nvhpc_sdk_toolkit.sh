#!/bin/bash
# NVIDIA HPC SDK
SDK_VERSION=22.9 && CUDA_VERSION=11.7 && installer=nvhpc_2022_229_Linux_x86_64_cuda_$CUDA_VERSION  # 11.7 uniquement
SDK_VERSION=22.2 && CUDA_VERSION=11.6 && installer=nvhpc_2022_222_Linux_x86_64_cuda_multi # 11.6, 11.2, 11.0, 10.2 
# Fedora 34:
SDK_VERSION=22.1 && CUDA_VERSION=11.5 && installer=nvhpc_2022_221_Linux_x86_64_cuda_multi # 11.5, 11.0, 10.2 (nvcc 11.5 OK pour gcc 11.0 de Fedora 34)
# Fedora 36 car 22.1 ne trouve pas seekg (voir reproducer): Attention, il faut aussi installer un gcc 10. ou 11. avec spack sur F36:
[ -f /usr/lib64/libstdc++.so.6.0.30 ] && SDK_VERSION=23.5 && CUDA_VERSION=12.1 && installer=nvhpc_2023_235_Linux_x86_64_cuda_multi # 12.1, 11.8, 11.0

# On passe a 23.5 pour tout le monde (permet de mieux suivre la correction des bugs du compilateur)
SDK_VERSION=23.5 && CUDA_VERSION=12.1 && installer=nvhpc_2023_235_Linux_x86_64_cuda_multi # 12.1, 11.8, 11.0

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
