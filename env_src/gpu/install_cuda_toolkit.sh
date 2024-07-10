#!/bin/bash
# CUDA TOOLKIT
CUDA_VERSION=11.8.0 && installer=cuda_$CUDA_VERSION"_520.61.05_linux.run" # Ne converge pas (c-amg dans PAR_gpu_3D) semble se produire des 11.3,11.6,11.7, par contre sa-amg fonctionne sauf si Cuda plus recent que driver CUDENSE handle...
CUDA_VERSION=11.2.0 && installer=cuda_$CUDA_VERSION"_460.27.04_linux.run" 
CUDA_VERSION=11.4.0 && installer=cuda_$CUDA_VERSION"_470.42.01_linux.run" # Necessaire suite a l'update d'AmgX (v2.3.0 ne compile plus avec nvcc 11.2)
INSTALL=$TRUST_ROOT/env/gpu/install
CUDA_BIN=$INSTALL/cuda-$CUDA_VERSION/bin
if [ ! -f $CUDA_BIN/nvcc ]
then
   echo "It may take some minutes to download Cuda Toolkit $CUDA_VERSION ..."
   (
      if [ ! -f $TRUST_TMP/$installer ]
      then
         cd $TRUST_TMP
         wget https://developer.download.nvidia.com/compute/cuda/$CUDA_VERSION/local_installers/$installer 1>/dev/null 2>&1 
         [ $? != 0 ] && echo "Error when downloading. See https://developer.nvidia.com/cuda-downloads" && exit -1
      fi
      mkdir -p $INSTALL && cd $INSTALL
      echo "It may take some minutes to install Cuda Toolkit..."
      # --override pour ne pas verifier la version de gcc par rapport a Cuda (risque...)
      # ToDo supprimer la fenetre lors d'une remote install par ssh... Done: --nox11
      log=$TRUST_TMP/cuda-installer.log
      rm -f $log 
      [ $? != 0 ] && echo "Error, you need to delete $log file before install." && exit -1
      bash $TRUST_TMP/$installer --override --silent --no-drm --no-man-page --toolkit --nox11 --installpath=`dirname $CUDA_BIN` --tmpdir=$TRUST_TMP
      err=$?
      rm -f $log
      [ $err != 0 ] && echo "Error during Cuda install under `dirname $CUDA_BIN`" && exit -1
      #rm -f $installer
      # Bizarre petra:
      chmod gou+x $CUDA_BIN/*
   )
fi
