#!/bin/bash
# NVIDIA HPC SDK
SDK_VERSION=22.9 && CUDA_VERSION=11.7 && installer=nvhpc_2022_229_Linux_x86_64_cuda_$CUDA_VERSION  # 11.7 uniquement
SDK_VERSION=22.2 && CUDA_VERSION=11.6 && installer=nvhpc_2022_222_Linux_x86_64_cuda_multi # 11.6, 11.2, 11.0, 10.2 
# Fedora 34:
SDK_VERSION=22.1 && CUDA_VERSION=11.5 && installer=nvhpc_2022_221_Linux_x86_64_cuda_multi # 11.5, 11.0, 10.2 (nvcc 11.5 OK pour gcc 11.0 de Fedora 34)
# Fedora 36 car 22.1 ne trouve pas seekg (voir reproducer): Attention, il faut aussi installer un gcc 10. ou 11. avec spack sur F36:
[ -f /usr/lib64/libstdc++.so.6.0.30 ] && SDK_VERSION=23.5 && CUDA_VERSION=12.1 && installer=nvhpc_2023_235_Linux_x86_64_cuda_multi # 12.1, 11.8, 11.0

# On passe a 23.5 pour tout le monde (permet de mieux suivre la correction des bugs du compilateur)
SDK_VERSION=23.5 && CUDA_VERSION=12.1 && installer=nvhpc_2023_235_Linux_x86_64_cuda_multi && installer_md5sum=4748dd45561d22cac5dfe5238e7eaf16 # 12.1, 11.8, 11.0
SDK_VERSION=23.5 && CUDA_VERSION=12.1 && installer=nvhpc_2023_235_Linux_x86_64_cuda_$CUDA_VERSION && installer_md5sum=eff38d63c4d08ca5c2962dad049a6833 # 12.1
# ! 23.9 : nvc++ -std=c++17 crashe sur TRUSTArray.cpp (omp target dans template ?) ToDo: signaler a NVidia
#SDK_VERSION=23.9 && CUDA_VERSION=12.1 && installer=nvhpc_2023_239_Linux_x86_64_cuda_multi # 12.2, 11.8, 11.0
#SDK_VERSION=24.1 && CUDA_VERSION=12.3 && installer=nvhpc_2024_221_Linux_x86_64_cuda_multi # 12.3, 11.8

INSTALL=$TRUST_ROOT/env/gpu/install
NVHPC=$INSTALL/nvhpc-$SDK_VERSION/Linux_x86_64/$SDK_VERSION/compilers
if [ ! -f $NVHPC/bin/nvc++ ]
then
   echo "It may take some minutes to download NVidia HPC SDK $SDK_VERSION ($installer) ..."
   (
   if [ ! -f $TRUST_TMP/$installer.tar.gz ]
   then
      cd $TRUST_TMP
      wget https://developer.download.nvidia.com/hpc-sdk/$SDK_VERSION/$installer.tar.gz 1>/dev/null 2>&1 
      [ $? != 0 ] && echo "Error when downloading. See https://developer.nvidia.com/nvidia-hpc-sdk-releases" && rm -f $installer.tar.gz && exit -1
   fi
   if [ "`md5sum $TRUST_TMP/$installer.tar.gz | cut -f 1 -d' '`" != "$installer_md5sum" ]
   then
      echo "md5sum not identical for $TRUST_TMP/$installer.tar.gz .... abort"
      md5sum $TRUST_TMP/$installer.tar.gz
      echo "md5sum ref:"$installer_md5sum
      rm -f $TRUST_TMP/$installer.tar.gz
      exit -1
   fi
   mkdir -p $INSTALL && cd $INSTALL
   echo "It may take some minutes to install NVidia HPC SDK..."
   tar xpzf $TRUST_TMP/$installer.tar.gz && rm -f $installer.tar.gz
   echo -e "\n1\n$INSTALL/nvhpc-$SDK_VERSION\n" | $installer/install
   [ $? != 0 ] && echo "Error during NVidia HPC SDK install under $NVHPC" && exit -1
   # Securite: mise a jour de localrc par rapport aux compilateurs (ex: spack). Vu avec Fed38 (gcc 13.0):
   # See https://forums.developer.nvidia.com/t/hpcsdk-22-7-installation-issues/226894/2
   echo "Update localrc under \$NVHPC/bin ..."
   cd $NVHPC/bin
   ./makelocalrc -d . -x -gcc `which gcc` -gpp `which g++` -g77 `which gfortran` || exit -1
   cd - 1>/dev/null 2>&1
   rm -r -f $installer
   )
fi
