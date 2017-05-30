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

#ifndef __DISJOINTDEC_HXX__
#define __DISJOINTDEC_HXX__

#include "MEDCouplingFieldDouble.hxx"
#include "NormalizedUnstructuredMesh.hxx"
#include "DEC.hxx"

#include <mpi.h>
#include <set>

namespace ICoCo
{
  class MEDField;
}

namespace MEDCoupling
{
  class ProcessorGroup;
  class ParaFIELD;
  
  class DisjointDEC : public DEC
  {
  public:
    DisjointDEC():_local_field(0),_union_group(0),_source_group(0),_target_group(0),
    _comm_interface(0),
    _owns_field(false),_owns_groups(false),
    _union_comm(MPI_COMM_NULL)
    { }
    DisjointDEC(ProcessorGroup& source_group, ProcessorGroup& target_group);
    DisjointDEC(const DisjointDEC&);
    DisjointDEC &operator=(const DisjointDEC& s);
    DisjointDEC(const std::set<int>& src_ids, const std::set<int>& trg_ids,
                const MPI_Comm& world_comm=MPI_COMM_WORLD);
    void setNature(NatureOfField nature);
    void attachLocalField( MEDCouplingFieldDouble *field);
    void attachLocalField(const ParaFIELD *field, bool ownPt=false);
    void attachLocalField(const ICoCo::MEDField *field);
    
    virtual void prepareSourceDE() = 0;
    virtual void prepareTargetDE() = 0;
    virtual void recvData() = 0;
    virtual void sendData() = 0;
    void sendRecvData(bool way=true);
    virtual void synchronize() = 0;
    virtual ~DisjointDEC();
    virtual void computeProcGroup() { }
    void renormalizeTargetField(bool isWAbs);
    //
    ProcessorGroup *getSourceGrp() const { return _source_group; }
    ProcessorGroup *getTargetGrp() const { return _target_group; }
    bool isInSourceSide() const;
    bool isInTargetSide() const;
    bool isInUnion() const;
  protected:
    void compareFieldAndMethod() const ;
    void cleanInstance();
    void copyInstance(const DisjointDEC& other);
    void checkPartitionGroup() const;
  protected:
    const ParaFIELD* _local_field;
    //! Processor group representing the union of target and source processors
    ProcessorGroup* _union_group;
    ProcessorGroup* _source_group;
    ProcessorGroup* _target_group;
    
    const CommInterface* _comm_interface;
    bool _owns_field;
    bool _owns_groups;
    MPI_Comm _union_comm;
  };
}

#endif
