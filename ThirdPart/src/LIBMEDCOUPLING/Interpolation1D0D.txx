// Copyright (C) 2018  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author : Anthony Geay (EDF R&D)

#ifndef __INTERPOLATION1D0D_TXX__
#define __INTERPOLATION1D0D_TXX__

#include "Interpolation1D0D.hxx"
#include "Interpolation.txx"
#include "MeshElement.txx"
#include "PointLocator3DIntersectorP0P0.txx"
#include "PointLocator3DIntersectorP0P1.txx"
#include "PointLocator3DIntersectorP1P0.txx"
#include "PointLocator3DIntersectorP1P1.txx"
#include "Log.hxx"

#include "BBTree.txx"

#include "InterpKernelAssert.hxx"

namespace INTERP_KERNEL
{
  /**
   *  Very similar to Interpolation3D::interpolateMeshes, except for the bounding boxes that can be
   *  adjusted in a similar fashion as in InterpolationPlanar::performAdjustmentOfBB()
   **/
  template<class MyMeshType, class MatrixType>
  int Interpolation1D0D::interpolateMeshes(const MyMeshType& srcMesh, const MyMeshType& targetMesh, MatrixType& result, const std::string& method)
  {
#if __cplusplus >= 201103L
    constexpr int SPACEDIM=MyMeshType::MY_SPACEDIM;
    using ConnType=typename MyMeshType::MyConnType;
    IKAssert(SPACEDIM==3);

    if(InterpolationOptions::getIntersectionType() != PointLocator)
      INTERP_KERNEL::Exception("Invalid 1D/0D intersection type specified : must be PointLocator.");

    std::string methC ( InterpolationOptions::filterInterpolationMethod(method) );
    if(methC!="P1P1")
      throw Exception("Invalid method chosen must be in \"P1P1\".");

    const double epsilon(getPrecision());
    // create MeshElement objects corresponding to each element of the two meshes
    const unsigned long numSrcElems(srcMesh.getNumberOfElements()), numTargetElems(targetMesh.getNumberOfElements());

    LOG(2, "Source mesh has " << numSrcElems << " elements and target mesh has " << numTargetElems << " elements ");

    std::vector<MeshElement<ConnType>*> srcElems(numSrcElems);

    std::map<MeshElement<ConnType>*, int> indices;

    for(unsigned long i = 0 ; i < numSrcElems ; ++i)
      srcElems[i] = new MeshElement<ConnType>(i, srcMesh);       

    // create empty maps for all source elements
    result.resize(targetMesh.getNumberOfNodes());

    // create BBTree structure
    // - get bounding boxes
    std::vector<double> bboxes(2*SPACEDIM*numSrcElems);
    int* srcElemIdx = new int[numSrcElems];
    for(unsigned long i = 0; i < numSrcElems ; ++i)
      {
        // get source bboxes in right order
        srcElems[i]->getBoundingBox()->toCompactData(bboxes.data()+6*i);
        srcElemIdx[i] = srcElems[i]->getIndex();
      }

    adjustBoundingBoxes(bboxes);
    const double *bboxPtr(nullptr);
    if(numSrcElems>0)
      bboxPtr=&bboxes[0];
    BBTree<SPACEDIM,ConnType> tree(bboxPtr, srcElemIdx, 0, numSrcElems);
    const ConnType *trgConnPtr(targetMesh.getConnectivityPtr()),*trgConnIPtr(targetMesh.getConnectivityIndexPtr());
    const ConnType *srcConnPtr(srcMesh.getConnectivityPtr()),*srcConnIPtr(srcMesh.getConnectivityIndexPtr());
    const double *trgCooPtr(targetMesh.getCoordinatesPtr()),*srcCooPtr(srcMesh.getCoordinatesPtr());
    for(unsigned long i = 0; i < numTargetElems; ++i)
      {
        IKAssert(trgConnIPtr[i+1]==i+1 && trgConnIPtr[i]==i);
        std::vector<ConnType> srcSegCondidates;
        const double *trgCellPosition(trgCooPtr+SPACEDIM*trgConnPtr[i]);
        typename MatrixType::value_type& resRow(result[trgConnPtr[i]]);
        tree.getElementsAroundPoint(trgCellPosition, srcSegCondidates);
        for(auto srcSeg: srcSegCondidates)
          {
            IKAssertMsg(srcConnIPtr[srcSeg+1]==2*(srcSeg+1) && srcConnIPtr[srcSeg]==2*srcSeg,"Only implemented for linear 1D source");
            double bc0(0.),bc1(0.);
            ConnType srcNode0(srcConnPtr[2*srcSeg]),srcNode1(srcConnPtr[2*srcSeg+1]);
            if(IsPointOn3DSeg(srcCooPtr+SPACEDIM*srcNode0,srcCooPtr+SPACEDIM*srcNode1,trgCellPosition,epsilon,bc0,bc1))
              {
                resRow.insert(std::make_pair(srcNode0,bc0));
                resRow.insert(std::make_pair(srcNode1,bc1));
                continue;
              }
          }
      }
    delete [] srcElemIdx;
    for(unsigned long i = 0 ; i < numSrcElems ; ++i)
      delete srcElems[i];
    return srcMesh.getNumberOfNodes();
#else
  throw INTERP_KERNEL::Exception("Go buying a C++11 compiler :)");
#endif
  }
}

#endif
