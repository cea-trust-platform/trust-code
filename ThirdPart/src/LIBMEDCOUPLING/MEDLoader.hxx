// Copyright (C) 2007-2016  CEA/DEN, EDF R&D
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
// Author : Anthony Geay (CEA/DEN)

#ifndef __MEDLOADER_HXX__
#define __MEDLOADER_HXX__

#include "MEDLoaderDefines.hxx"
#include "InterpKernelException.hxx"
#include "MEDCouplingRefCountObject.hxx"
#include "NormalizedUnstructuredMesh.hxx"
#include "MCAuto.hxx"

#include <list>
#include <vector>

namespace MEDCoupling
{
  class DataArrayInt;
  class MEDCouplingMesh;
  class MEDCouplingUMesh;
  class MEDCouplingFieldDouble;
  class MEDFileWritable;

  MEDLOADER_EXPORT void SetEpsilonForNodeComp(double val);
  MEDLOADER_EXPORT void SetCompPolicyForCell(int val);
  MEDLOADER_EXPORT void SetTooLongStrPolicy(int val);
  MEDLOADER_EXPORT bool HasXDR();
  MEDLOADER_EXPORT std::string MEDFileVersionStr();
  MEDLOADER_EXPORT std::string MEDFileVersionOfFileStr(const std::string& fileName);
  MEDLOADER_EXPORT void MEDFileVersion(int& major, int& minor, int& release);
  MEDLOADER_EXPORT void CheckFileForRead(const std::string& fileName);
  MEDLOADER_EXPORT std::vector<std::string> GetMeshNames(const std::string& fileName);
  MEDLOADER_EXPORT std::vector< std::vector< std::pair<INTERP_KERNEL::NormalizedCellType,int> > > GetUMeshGlobalInfo(const std::string& fileName, const std::string& meshName, int &meshDim, int& spaceDim, int& numberOfNodes);
  MEDLOADER_EXPORT std::vector< std::pair<std::string,std::string> > GetComponentsNamesOfField(const std::string& fileName, const std::string& fieldName);
  MEDLOADER_EXPORT std::vector<std::string> GetMeshNamesOnField(const std::string& fileName, const std::string& fieldName);
  MEDLOADER_EXPORT std::vector<std::string> GetMeshGroupsNames(const std::string& fileName, const std::string& meshName);
  MEDLOADER_EXPORT std::vector<std::string> GetMeshFamiliesNames(const std::string& fileName, const std::string& meshName);
  MEDLOADER_EXPORT std::vector<std::string> GetMeshFamiliesNamesOnGroup(const std::string& fileName, const std::string& meshName, const std::string& grpName);
  MEDLOADER_EXPORT std::vector<std::string> GetMeshGroupsNamesOnFamily(const std::string& fileName, const std::string& meshName, const std::string& famName);
  MEDLOADER_EXPORT std::vector<std::string> GetAllFieldNames(const std::string& fileName);
  MEDLOADER_EXPORT std::vector<std::string> GetAllFieldNamesOnMesh(const std::string& fileName, const std::string& meshName);
  MEDLOADER_EXPORT std::vector<MEDCoupling::TypeOfField> GetTypesOfField(const std::string& fileName, const std::string& meshName, const std::string& fieldName);
  MEDLOADER_EXPORT std::vector<std::string> GetFieldNamesOnMesh(MEDCoupling::TypeOfField type, const std::string& fileName, const std::string& meshName);
  MEDLOADER_EXPORT std::vector<std::string> GetCellFieldNamesOnMesh(const std::string& fileName, const std::string& meshName);
  MEDLOADER_EXPORT std::vector<std::string> GetNodeFieldNamesOnMesh(const std::string& fileName, const std::string& meshName);
  MEDLOADER_EXPORT std::vector< std::pair<int,int> > GetFieldIterations(MEDCoupling::TypeOfField type, const std::string& fileName, const std::string& meshName, const std::string& fieldName);
  MEDLOADER_EXPORT std::vector< std::pair<int,int> > GetCellFieldIterations(const std::string& fileName, const std::string& meshName, const std::string& fieldName);
  MEDLOADER_EXPORT std::vector< std::pair<int,int> > GetNodeFieldIterations(const std::string& fileName, const std::string& meshName, const std::string& fieldName);
  MEDLOADER_EXPORT std::vector< std::pair< std::pair<int,int>, double> > GetAllFieldIterations(const std::string& fileName, const std::string& fieldName);
  MEDLOADER_EXPORT double GetTimeAttachedOnFieldIteration(const std::string& fileName, const std::string& fieldName, int iteration, int order);
  MEDLOADER_EXPORT MEDCoupling::MEDCouplingUMesh *ReadUMeshFromFamilies(const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::vector<std::string>& fams);
  MEDLOADER_EXPORT MEDCoupling::MEDCouplingUMesh *ReadUMeshFromGroups(const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::vector<std::string>& grps);
  MEDLOADER_EXPORT MEDCoupling::MEDCouplingMesh *ReadMeshFromFile(const std::string& fileName, const std::string& meshName, int meshDimRelToMax=0);
  MEDLOADER_EXPORT MEDCoupling::MEDCouplingMesh *ReadMeshFromFile(const std::string& fileName, int meshDimRelToMax=0);
  MEDLOADER_EXPORT MEDCoupling::MEDCouplingUMesh *ReadUMeshFromFile(const std::string& fileName, const std::string& meshName, int meshDimRelToMax=0);
  MEDLOADER_EXPORT MEDCoupling::MEDCouplingUMesh *ReadUMeshFromFile(const std::string& fileName, int meshDimRelToMax=0);
  MEDLOADER_EXPORT int ReadUMeshDimFromFile(const std::string& fileName, const std::string& meshName);
  MEDLOADER_EXPORT MCAuto<MEDCoupling::MEDCouplingFieldDouble> ReadField(const std::string& fileName);
  MEDLOADER_EXPORT MCAuto<MEDCoupling::MEDCouplingFieldDouble> ReadField(const std::string& fileName, const std::string& fieldName);
  MEDLOADER_EXPORT MCAuto<MEDCoupling::MEDCouplingFieldDouble> ReadField(const std::string& fileName, const std::string& fieldName, int iteration, int order);
  MEDLOADER_EXPORT MCAuto<MEDCoupling::MEDCouplingFieldDouble> ReadField(MEDCoupling::TypeOfField type, const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::string& fieldName, int iteration, int order);
  MEDLOADER_EXPORT std::vector<MEDCoupling::MEDCouplingFieldDouble *> ReadFieldsOnSameMesh(MEDCoupling::TypeOfField type, const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::string& fieldName,
                                                                                           const std::vector<std::pair<int,int> >& its);
  MEDLOADER_EXPORT std::vector<MEDCoupling::MEDCouplingFieldDouble *> ReadFieldsCellOnSameMesh(const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::string& fieldName,
                                                                                               const std::vector<std::pair<int,int> >& its);
  MEDLOADER_EXPORT std::vector<MEDCoupling::MEDCouplingFieldDouble *> ReadFieldsNodeOnSameMesh(const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::string& fieldName,
                                                                                               const std::vector<std::pair<int,int> >& its);
  MEDLOADER_EXPORT std::vector<MEDCoupling::MEDCouplingFieldDouble *> ReadFieldsGaussOnSameMesh(const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::string& fieldName,
                                                                                                const std::vector<std::pair<int,int> >& its);
  MEDLOADER_EXPORT std::vector<MEDCoupling::MEDCouplingFieldDouble *> ReadFieldsGaussNEOnSameMesh(const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::string& fieldName,
                                                                                                  const std::vector<std::pair<int,int> >& its);
  MEDLOADER_EXPORT MEDCoupling::MEDCouplingFieldDouble *ReadFieldCell(const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::string& fieldName, int iteration, int order);
  MEDLOADER_EXPORT MEDCoupling::MEDCouplingFieldDouble *ReadFieldNode(const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::string& fieldName, int iteration, int order);
  MEDLOADER_EXPORT MEDCoupling::MEDCouplingFieldDouble *ReadFieldGauss(const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::string& fieldName, int iteration, int order);
  MEDLOADER_EXPORT MEDCoupling::MEDCouplingFieldDouble *ReadFieldGaussNE(const std::string& fileName, const std::string& meshName, int meshDimRelToMax, const std::string& fieldName, int iteration, int order);
  MEDLOADER_EXPORT void WriteMesh(const std::string& fileName, const MEDCoupling::MEDCouplingMesh *mesh, bool writeFromScratch);
  MEDLOADER_EXPORT void WriteUMesh(const std::string& fileName, const MEDCoupling::MEDCouplingUMesh *mesh, bool writeFromScratch);
  MEDLOADER_EXPORT void WriteUMeshDep(const std::string& fileName, const MEDCoupling::MEDCouplingUMesh *mesh, bool writeFromScratch);
  MEDLOADER_EXPORT void WriteUMeshesPartition(const std::string& fileName, const std::string& meshName, const std::vector<const MEDCoupling::MEDCouplingUMesh *>& meshes, bool writeFromScratch);
  MEDLOADER_EXPORT void WriteUMeshesPartitionDep(const std::string& fileName, const std::string& meshName, const std::vector<const MEDCoupling::MEDCouplingUMesh *>& meshes, bool writeFromScratch);
  MEDLOADER_EXPORT void WriteUMeshes(const std::string& fileName, const std::vector<const MEDCoupling::MEDCouplingUMesh *>& meshes, bool writeFromScratch);
  MEDLOADER_EXPORT void WriteField(const std::string& fileName, const MEDCoupling::MEDCouplingFieldDouble *f, bool writeFromScratch);
  MEDLOADER_EXPORT void WriteFieldDep(const std::string& fileName, const MEDCoupling::MEDCouplingFieldDouble *f, bool writeFromScratch);
  MEDLOADER_EXPORT void WriteFieldUsingAlreadyWrittenMesh(const std::string& fileName, const MEDCoupling::MEDCouplingFieldDouble *f);

  MEDLOADER_EXPORT void AssignStaticWritePropertiesTo(MEDCoupling::MEDFileWritable& obj);
}

#endif
