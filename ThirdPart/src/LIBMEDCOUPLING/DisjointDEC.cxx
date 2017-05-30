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

#include "DisjointDEC.hxx"
#include "CommInterface.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ComponentTopology.hxx"
#include "ParaFIELD.hxx"
#include "ParaMESH.hxx"
#include "ICoCoField.hxx"
#include "ICoCoMEDField.hxx"
#include "MPIProcessorGroup.hxx"

#include <cmath>
#include <iostream>


namespace MEDCoupling
{

  /*!
   * \anchor DisjointDEC-det
   * \class DisjointDEC
   *
   * \section DisjointDEC-over Overview
   *
   * Abstract interface class representing a link between two
   * processor groups for exchanging mesh or field data. The two processor groups must
   * have a void intersection (\ref MEDCoupling::OverlapDEC "OverlapDEC" is to be considered otherwise).
   * The %DEC is initialized by attaching a field on the receiving or on the
   * sending side.
   *
   * The data is sent or received through calls to the (abstract) methods recvData() and sendData().
   *
   * One can attach either a \c MEDCoupling::ParaFIELD, or a
   * \c ICoCo::Field, or directly a \c MEDCoupling::MEDCouplingFieldDouble instance.
   * See the various signatures of the method DisjointDEC::attachLocalField()
   *
   * The derivations of this class should be considered for practical instanciation:
   * - \ref InterpKernelDEC-det "InterpKernelDEC"
   * - \ref ExplicitCoincidentDEC-det "ExplicitCoincidentDEC"
   * - \ref StructuredCoincidentDEC-det "StructuredCoincidentDEC"
   *
   * \section DisjointDEC-options DisjointDEC options
   * The options supported by %DisjointDEC objects are the same that the ones supported for all
   * DECs in general and are all inherited from the class \ref MEDCoupling::DECOptions "DECOptions"
   *
  */

  DisjointDEC::DisjointDEC(ProcessorGroup& source_group, ProcessorGroup& target_group):
      _local_field(0),
      _source_group(&source_group),
      _target_group(&target_group),
      _comm_interface(0),
      _owns_field(false),
      _owns_groups(false),
      _union_comm(MPI_COMM_NULL)
  {
    checkPartitionGroup();
    _union_group = source_group.fuse(target_group);  
  }
  
  DisjointDEC::DisjointDEC(const DisjointDEC& s):
      DEC(s),
      _local_field(0),
      _union_group(0),
      _source_group(0),
      _target_group(0),
      _comm_interface(0),
      _owns_field(false),
      _owns_groups(false),
      _union_comm(MPI_COMM_NULL)
  {
    copyInstance(s);
  }

  DisjointDEC & DisjointDEC::operator=(const DisjointDEC& s)
  {
    cleanInstance();
    copyInstance(s);
    return *this;
     
  }

  void DisjointDEC::copyInstance(const DisjointDEC& other)
  {
    DEC::copyFrom(other);
    if (other._union_comm != MPI_COMM_NULL)
      {
        // Tricky: the DEC is responsible for the management of _union_comm. And this comm is referenced by
        // the MPIProcGroups (source/targets). In the case where _union_comm is not NULL we must take care of rebuilding the
        // MPIProcGroups with a communicator that will survive the destruction of 'other'.
        _owns_groups = true;
        MPI_Comm_dup(other._union_comm, &_union_comm);
//        std::cout << "DUP union comm - new is "<< _union_comm << "\n";
        _target_group = new MPIProcessorGroup(*_comm_interface, other._target_group->getProcIDs(), _union_comm);
        _source_group = new MPIProcessorGroup(*_comm_interface, other._source_group->getProcIDs(), _union_comm);
      }
    else{
      if(other._target_group)
        {
          _target_group=other._target_group->deepCopy();
          _owns_groups=true;
        }
      if(other._source_group)
        {
          _source_group=other._source_group->deepCopy();
          _owns_groups=true;
        }
    }
    if (_source_group && _target_group)
      _union_group = _source_group->fuse(*_target_group);
  }

  DisjointDEC::DisjointDEC(const std::set<int>& source_ids,
                           const std::set<int>& target_ids,
                           const MPI_Comm& world_comm):
     _local_field(0),
     _owns_field(false),
     _owns_groups(true),
     _comm_interface(0),
     _union_comm(MPI_COMM_NULL)
  {
    MEDCoupling::CommInterface comm;
    // Create the list of procs including source and target
    std::set<int> union_ids; // source and target ids in world_comm
    union_ids.insert(source_ids.begin(),source_ids.end());
    union_ids.insert(target_ids.begin(),target_ids.end());
    if(union_ids.size()!=(source_ids.size()+target_ids.size()))
      throw INTERP_KERNEL::Exception("DisjointDEC constructor : source_ids and target_ids overlap partially or fully. This type of DEC does not support it! OverlapDEC class could be the solution!");
    int* union_ranks_world=new int[union_ids.size()]; // ranks of sources and targets in world_comm
    std::copy(union_ids.begin(), union_ids.end(), union_ranks_world);

    // Create a communicator on these procs
    MPI_Group union_group,world_group;
    comm.commGroup(world_comm,&world_group);
    comm.groupIncl(world_group,union_ids.size(),union_ranks_world,&union_group);
    comm.commCreate(world_comm,union_group,&_union_comm);
    delete[] union_ranks_world;
    if (_union_comm==MPI_COMM_NULL)
      { // This process is not in union
        _source_group=0;
        _target_group=0;
        _union_group=0;
        comm.groupFree(&union_group);
        comm.groupFree(&world_group);
        return;
      }

    // Translate source_ids and target_ids from world_comm to union_comm
    int* source_ranks_world=new int[source_ids.size()]; // ranks of sources in world_comm
    std::copy(source_ids.begin(), source_ids.end(),source_ranks_world);
    int* source_ranks_union=new int[source_ids.size()]; // ranks of sources in union_comm
    int* target_ranks_world=new int[target_ids.size()]; // ranks of targets in world_comm
    std::copy(target_ids.begin(), target_ids.end(),target_ranks_world);
    int* target_ranks_union=new int[target_ids.size()]; // ranks of targets in union_comm
    MPI_Group_translate_ranks(world_group,source_ids.size(),source_ranks_world,union_group,source_ranks_union);
    MPI_Group_translate_ranks(world_group,target_ids.size(),target_ranks_world,union_group,target_ranks_union);
    std::set<int> source_ids_union;
    for (int i=0;i<(int)source_ids.size();i++)
      source_ids_union.insert(source_ranks_union[i]);
    std::set<int> target_ids_union;
    for (int i=0;i<(int)target_ids.size();i++)
      target_ids_union.insert(target_ranks_union[i]);
    delete [] source_ranks_world;
    delete [] source_ranks_union;
    delete [] target_ranks_world;
    delete [] target_ranks_union;

    // Create the MPIProcessorGroups
    _source_group = new MPIProcessorGroup(comm,source_ids_union,_union_comm);
    _target_group = new MPIProcessorGroup(comm,target_ids_union,_union_comm);
    _union_group = _source_group->fuse(*_target_group);
    comm.groupFree(&union_group);
    comm.groupFree(&world_group);
  }

  DisjointDEC::~DisjointDEC()
  {
    cleanInstance();
  }

  void DisjointDEC::cleanInstance()
  {
    if(_owns_field)
      {
        delete _local_field;
      }
    _local_field=0;
    _owns_field=false;
    if(_owns_groups)
      {
        delete _source_group;
        delete _target_group;
      }
    _owns_groups=false;
    _source_group=0;
    _target_group=0;
    delete _union_group;
    _union_group=0;
    if (_union_comm != MPI_COMM_NULL)
      _comm_interface->commFree(&_union_comm);
  }

  /**
   * Check that the sources and targets procs form a partition of the world communicator referenced in the groups.
   * This world communicator is not necessarily MPI_WORLD_COMM, but it has to be covered completly for the DECs to work.
   */
  void DisjointDEC::checkPartitionGroup() const
  {
    int size = -1;
    MPIProcessorGroup * tgt = static_cast<MPIProcessorGroup *>(_target_group);
    MPIProcessorGroup * src = static_cast<MPIProcessorGroup *>(_source_group);
    MPI_Comm comm_t = tgt->getWorldComm();
    MPI_Comm comm_s = src->getWorldComm();
    if (comm_t != comm_s)
      throw INTERP_KERNEL::Exception("DisjointDEC constructor: Inconsistent world communicator when building DisjointDEC");
    MPI_Comm_size(comm_t, &size);

    std::set<int> union_ids; // source and target ids in world_comm
    union_ids.insert(src->getProcIDs().begin(),src->getProcIDs().end());
    union_ids.insert(tgt->getProcIDs().begin(),tgt->getProcIDs().end());
    if(union_ids.size()!=size)
      throw INTERP_KERNEL::Exception("DisjointDEC constructor: source_ids and target_ids do not form a partition of the communicator! Restrain the world communicator passed to MPIProcessorGroup ctor.");
  }

  void DisjointDEC::setNature(NatureOfField nature)
  {
    if(_local_field)
      _local_field->getField()->setNature(nature);
  }

  /*! Attaches a local field to a DEC.
    If the processor is on the receiving end of the DEC, the field
    will be updated by a recvData() call.
    Reversely, if the processor is on the sending end, the field will be read, possibly transformed, and sent appropriately to the other side.
  */
  void DisjointDEC::attachLocalField(const ParaFIELD *field, bool ownPt)
  {
    if(!isInUnion())
      return ;
    if(_owns_field)
      delete _local_field;
    _local_field=field;
    _owns_field=ownPt;
    _comm_interface=&(field->getTopology()->getProcGroup()->getCommInterface());
    compareFieldAndMethod();
  }

  /*! Attaches a local field to a DEC. The method will test whether the processor
    is on the source or the target side and will associate the mesh underlying the 
    field to the local side.

    If the processor is on the receiving end of the DEC, the field
    will be updated by a recvData() call.
    Reversely, if the processor is on the sending end, the field will be read, possibly transformed,
    and sent appropriately to the other side.
  */

  void DisjointDEC::attachLocalField(MEDCouplingFieldDouble *field)
  {
    if(!isInUnion())
      return ;
    ProcessorGroup* local_group;
    if (_source_group->containsMyRank())
      local_group=_source_group;
    else if (_target_group->containsMyRank())
      local_group=_target_group;
    else
      throw INTERP_KERNEL::Exception("Invalid procgroup for field attachment to DEC");
    ParaMESH *paramesh=new ParaMESH(static_cast<MEDCouplingPointSet *>(const_cast<MEDCouplingMesh *>(field->getMesh())),*local_group,field->getMesh()->getName());
    ParaFIELD *tmp=new ParaFIELD(field, paramesh, *local_group);
    tmp->setOwnSupport(true);
    attachLocalField(tmp,true);
    //_comm_interface=&(local_group->getCommInterface());
  }

  /*! 
    Attaches a local field to a DEC.
    If the processor is on the receiving end of the DEC, the field
    will be updated by a recvData() call.
    Reversely, if the processor is on the sending end, the field will be read, possibly transformed, and sent appropriately to the other side.
    The field type is a generic ICoCo Field, so that the DEC can couple a number of different fields :
    - a ICoCo::MEDField, that is created from a MEDCoupling structure
    
  */
  void DisjointDEC::attachLocalField(const ICoCo::MEDField *field)
  {
    if(!isInUnion())
      return ;
    if(!field)
      throw INTERP_KERNEL::Exception("DisjointDEC::attachLocalField : ICoCo::MEDField pointer is NULL !");
    attachLocalField(field->getField());
  }
  
  /*!
    Computes the field norm over its support 
    on the source side and renormalizes the field on the target side
    so that the norms match.

    \f[
    I_{source}=\sum_{i=1}^{n_{source}}V_{i}.|\Phi^{source}_{i}|^2,
    \f]

    \f[
    I_{target}=\sum_{i=1}^{n_{target}}V_{i}.|\Phi^{target}_{i}|^2,
    \f]
    
    \f[
    \Phi^{target}:=\Phi^{target}.\sqrt{I_{source}/I_{target}}.
    \f]

  */
  void DisjointDEC::renormalizeTargetField(bool isWAbs)
  {
    if (_source_group->containsMyRank())
      for (int icomp=0; icomp<_local_field->getField()->getArray()->getNumberOfComponents(); icomp++)
        {
          double total_norm = _local_field->getVolumeIntegral(icomp+1,isWAbs);
          double source_norm = total_norm;
          _comm_interface->broadcast(&source_norm, 1, MPI_DOUBLE, 0,* dynamic_cast<MPIProcessorGroup*>(_union_group)->getComm());

        }
    if (_target_group->containsMyRank())
      {
        for (int icomp=0; icomp<_local_field->getField()->getArray()->getNumberOfComponents(); icomp++)
          {
            double total_norm = _local_field->getVolumeIntegral(icomp+1,isWAbs);
            double source_norm=total_norm;
            _comm_interface->broadcast(&source_norm, 1, MPI_DOUBLE, 0,* dynamic_cast<MPIProcessorGroup*>(_union_group)->getComm());

            if (fabs(total_norm)>1e-100)
              _local_field->getField()->applyLin(source_norm/total_norm,0.0,icomp+1);
          }
      }
  }

  bool DisjointDEC::isInSourceSide() const
  {
    if(!_source_group)
      return false;
    return _source_group->containsMyRank();
  }

  bool DisjointDEC::isInTargetSide() const
  {
    if(!_target_group)
      return false;
    return _target_group->containsMyRank();
  }

  bool DisjointDEC::isInUnion() const
  {
    if(!_union_group)
      return false;
    return _union_group->containsMyRank();
  }

  void DisjointDEC::compareFieldAndMethod() const
  {
    if (_local_field)
      {
        TypeOfField entity = _local_field->getField()->getTypeOfField();
        if ( getMethod() == "P0" )
          {
            if ( entity != ON_CELLS )
              throw INTERP_KERNEL::Exception("Field support and interpolation method mismatch."
                                             " For P0 interpolation, field must be on MED_CELL's");
          }
        else if ( getMethod() == "P1" )
          {
            if ( entity != ON_NODES )
              throw INTERP_KERNEL::Exception("Field support and interpolation method mismatch."
                                             " For P1 interpolation, field must be on MED_NODE's");
          }
        else if ( getMethod() == "P1d" )
          {
            if ( entity != ON_CELLS )
              throw INTERP_KERNEL::Exception("Field support and interpolation method mismatch."
                                             " For P1d interpolation, field must be on MED_CELL's");
            if ( _target_group->containsMyRank() )
              throw INTERP_KERNEL::Exception("Projection to P1d field not supported");
          }
        else
          {
            throw INTERP_KERNEL::Exception("Unknown interpolation method. Possible methods: P0, P1, P1d");
          }
      }
  }

  /*!
    If way==true, source procs call sendData() and target procs call recvData().
    if way==false, it's the other way round.
  */
  void DisjointDEC::sendRecvData(bool way)
  {
    if(!isInUnion())
      return;
    if(isInSourceSide())
      {
        if(way)
          sendData();
        else
          recvData();
      }
    else if(isInTargetSide())
      {
        if(way)
          recvData();
        else
          sendData();
      }
  }
}
