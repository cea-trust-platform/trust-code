#!/bin/bash
# NVIDIA HPC SDK: select version according to driver
NVIDIA_VERSION=`nvidia-smi 2>/dev/null | awk '/CUDA Version/ {v=$(NF-1);gsub("\\\.","",v);print v}'`
if [ "$NVIDIA_VERSION" = "" ] || [ $NVIDIA_VERSION -le 121 ]
then
   SDK_VERSION=23.5  && CUDA_VERSION=12.1 && installer=nvhpc_2023_235_Linux_x86_64_cuda_$CUDA_VERSION  && installer_md5sum=eff38d63c4d08ca5c2962dad049a6833
elif [ $NVIDIA_VERSION -le 125 ]
then
   SDK_VERSION=24.7  && CUDA_VERSION=12.5 && installer=nvhpc_2024_247_Linux_x86_64_cuda_$CUDA_VERSION  && installer_md5sum=c7fd37d1a1605f60b2c54890ee0c805e
else
   SDK_VERSION=24.11 && CUDA_VERSION=12.6 && installer=nvhpc_2024_2411_Linux_x86_64_cuda_$CUDA_VERSION && installer_md5sum=77e0a195e6830ce9749ba6a85bf48fc5
fi

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
   rm -rf $INSTALL
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
