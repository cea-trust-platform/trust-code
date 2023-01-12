#!/bin/bash
# CUDA TOOLKIT
CUDA_VERSION=11.8.0 && installer=cuda_$CUDA_VERSION"_520.61.05_linux.run" # Ne converge pas (c-amg dans PAR_gpu_3D) semble se produire des 11.3,11.6,11.7, par contre sa-amg fonctionne sauf si Cuda plus recent que driver CUDENSE handle...
CUDA_VERSION=11.2.0 && installer=cuda_$CUDA_VERSION"_460.27.04_linux.run" 
[ "$TRUST_TMP" = "" ] && TRUST_TMP="."
CUDA_BIN=$TRUST_TMP/cuda-$CUDA_VERSION/bin
if [ ! -f $CUDA_BIN/nvcc ]
then
   echo "It may take some minutes to download Cuda Toolkit $CUDA_VERSION ..."
   (
      cd $TRUST_TMP
      if [ ! -f $installer ]
      then
          wget https://developer.download.nvidia.com/compute/cuda/$CUDA_VERSION/local_installers/$installer 1>/dev/null 2>&1 
          [ $? != 0 ] && echo "Error when downloading. See https://developer.nvidia.com/cuda-downloads" && exit -1
      fi
      echo "It may take some minutes to install Cuda Toolkit..."
      # --override pour ne pas verifier la version de gcc par rapport a Cuda (risque...)
      # ToDo supprimer la fenetre lors d'une remote install par ssh...
      bash ./$installer --override --silent --no-drm --no-man-page --toolkit --installpath=`dirname $CUDA_BIN`
      [ $? != 0 ] && echo "Error during Cuda install under `dirname $CUDA_BIN`" && exit -1
      rm -f $installer
      # Bizarre petra:
      chmod gou+x $CUDA_BIN/*
   )
fi
