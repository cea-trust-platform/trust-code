// Copyright (C) 2007-2019  CEA/DEN, EDF R&D
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

#include "OverlapDEC.hxx"
#include "CommInterface.hxx"
#include "ParaMESH.hxx"
#include "ParaFIELD.hxx"
#include "MPIProcessorGroup.hxx"
#include "OverlapElementLocator.hxx"
#include "OverlapInterpolationMatrix.hxx"
#include "ICoCoMEDField.hxx"

namespace MEDCoupling
{
/*!
    \anchor OverlapDEC-det
    \class OverlapDEC

    \section OverlapDEC-over Overview

    The \c OverlapDEC enables the \ref InterpKerRemapGlobal "conservative remapping" of fields between
    two parallel codes. This remapping is based on the computation of intersection volumes on
    a \b single \b processor \b group. On this processor group are defined two field-templates called A
    and B. The computation is possible for 3D meshes, 2D meshes, 3D-surface meshes, 1D meshes and
    2D-curve meshes. Dimensions must be similar for the distribution templates A and B.

    The main difference with \ref InterpKernelDEC-det "InterpKernelDEC" is that this
    \ref para-dec "DEC" works with a *single* processor group, in which processors will share the work.
    Consequently each processor manages two \ref MEDCouplingFieldTemplatesPage "field templates" (A and B)
    called source and target.
    Furthermore all processors in the processor group cooperate in the global interpolation matrix
    computation. In this respect \c InterpKernelDEC is a specialization of \c OverlapDEC.

    \section ParaMEDMEMOverlapDECAlgorithmDescription Algorithm description

    Let's consider the following use case that is ran in ParaMEDMEMTest_OverlapDEC.cxx to describes
    the different steps of the computation. The processor group contains 3 processors.
    \anchor ParaMEDMEMOverlapDECImgTest1
    \image html OverlapDEC1.png "Example split of the source and target mesh among the 3 procs"

    \subsection ParaMEDMEMOverlapDECAlgoStep1 Step 1 : Bounding box exchange and global interaction between procs computation.

    In order to reduce as much as possible the amount of communications between distant processors,
    every processor computes a bounding box for A and B. Then a AllToAll communication is performed
    so that
    every processor can compute the \b global interactions between processor.
    This computation leads every processor to compute the same global TODO list expressed as a list
    of pair. A pair ( x, y ) means that proc \b x fieldtemplate A can interact with fieltemplate B of
    proc \b y because the two bounding boxes interact.
    In the \ref ParaMEDMEMOverlapDECImgTest1 "example above" this computation leads to the following
    a \b global TODO list :

    \b (0,0),(0,1),(1,0),(1,2),(2,0),(2,1),(2,2)

    Here the pair (0,2) does not appear because the bounding box of fieldtemplateA of proc#2 does
    not intersect that of fieldtemplate B on proc#0.

    Stage performed by MEDCoupling::OverlapElementLocator::computeBoundingBoxes.

    \subsection ParaMEDMEMOverlapDECAlgoStep2 Step 2 : Computation of local TODO list

    Starting from the global interaction previously computed in \ref ParaMEDMEMOverlapDECAlgoStep1
    "Step 1", each proc computes the TODO list per proc.
    The following rules is chosen : a pair (x,y) can be treated by either proc \#x or proc \#y,
    in order to reduce the amount of data transfers among
    processors. The algorithm chosen for load balancing is the following : Each processor has
    an empty \b local TODO list at the beginning. Then for each pair (k,m) in
    \b global TODO list, if proc\#k has less temporary local list than proc\#m pair, (k,m) is added
    to temporary local TODO list of proc\#k.
    If proc\#m has less temporary local TODO list than proc\#k pair, (k,m) is added to temporary
    local TODO list of proc\#m.
    If proc\#k and proc\#m have the same amount of temporary local TODO list pair, (k,m) is added to
    temporary local TODO list of proc\#k.

    In the \ref ParaMEDMEMOverlapDECImgTest1 "example above" this computation leads to the following
    local TODO list :

    - proc\#0 : (0,0)
    - proc\#1 : (0,1),(1,0)
    - proc\#2 : (1,2),(2,0),(2,1),(2,2)
    
    The algorithm described here is not perfect for this use case, we hope to enhance it soon.

    At this stage each proc knows precisely its \b local TODO list (with regard to interpolation).
    The \b local TODO list of other procs than local
    is kept for future computations.

    \subsection ParaMEDMEMOverlapDECAlgoStep3 Step 3 : Matrix echange between procs

    Knowing the \b local TODO list, the aim now is to exchange field-templates between procs.
    Each proc computes knowing TODO list per
    proc computed in \ref ParaMEDMEMOverlapDECAlgoStep2 "Step 2" the exchange TODO list :

    In the \ref ParaMEDMEMOverlapDECImgTest1 "example above" the exchange TODO list gives the
    following results :

    Sending TODO list per proc :

    - proc \#0 : Send fieldtemplate A to Proc\#1, Send fieldtemplate B to Proc\#1, Send fieldtemplate
    B to Proc\#2
    - Proc \#1 : Send fieldtemplate A to Proc\#2, Send fieldtemplate B to Proc\#2
    - Proc \#2 : No send.

    Receiving TODO list per proc :

    - proc \#0 : No receiving
    - proc \#1 : receiving fieldtemplate A from Proc\#0,  receiving fieldtemplate B from Proc\#0
    - proc \#2 : receiving fieldtemplate B from Proc\#0, receiving fieldtemplate A from Proc\#1,
    receiving fieldtemplate B from Proc\#1

    To avoid as much as possible large volumes of transfers between procs, only relevant parts of
    meshes are sent. In order for proc\#k to send fieldtemplate A to fieldtemplate B
    of proc \#m., proc\#k computes the part of mesh A contained in the boundingbox B of proc\#m. It
    implies that the corresponding cellIds or nodeIds of the
    corresponding part are sent to proc \#m too.

    Let's consider the couple (k,m) in the TODO list. This couple is treated by either k or m as
    seen in \ref ParaMEDMEMOverlapDECAlgoStep2 "here in Step2".

    As will be dealt in Step 6, for final matrix-vector computations, the resulting matrix of the
    couple (k,m) wherever it is computed (proc \#k or proc \#m)
    will be stored in \b proc\#m.

    - If proc \#k is in charge (performs the matrix computation) for this couple (k,m), target ids
    (cells or nodes) of the mesh in proc \#m are renumbered, because proc \#m has seelected a sub mesh
    of the target mesh to avoid large amounts of data to transfer. In this case as proc \#m is ultimately
     in charge of the matrix, proc \#k must keep preciously the
    source ids needed to be sent to proc\#m. No problem will appear for matrix assembling in proc m
    for source ids because no restriction was done.
    Concerning source ids to be sent for the matrix-vector computation, proc k will know precisely
    which source ids field values to send to proc \#m.
    This is embodied by OverlapMapping::keepTracksOfTargetIds in proc m.

    - If proc \#m is in charge (performs matrix computation) for this couple (k,m), source ids (cells
    or nodes) of the mesh in proc \#k are renumbered, because proc \#k has selected a sub mesh of the
     source mesh to avoid large amounts of data to transfer. In this case as proc \#k is ultimately
     in charge of the matrix, proc \#m receives the source ids
    from remote proc \#k, and thus the matrix is directly correct, no need for renumbering as
     in \ref ParaMEDMEMOverlapDECAlgoStep5 "Step 5". However proc \#k must
    keep track of the ids sent to proc \#m for the matrix-vector computation.
    This is incarnated by OverlapMapping::keepTracksOfSourceIds in proc k.

    This step is performed in MEDCoupling::OverlapElementLocator::exchangeMeshes method.

    \subsection ParaMEDMEMOverlapDECAlgoStep4 Step 4 : Computation of the interpolation matrix

    After mesh exchange in \ref ParaMEDMEMOverlapDECAlgoStep3 "Step3" each processor has all the
    required information to treat its \b local TODO list computed in
    \ref ParaMEDMEMOverlapDECAlgoStep2 "Step2". This step is potentially CPU costly, which is why
    the \b local TODO list per proc is expected to
    be as well balanced as possible.

    The interpolation is performed as the \ref MEDCoupling::MEDCouplingRemapper "remapper" does.

    This operation is performed by OverlapInterpolationMatrix::addContribution method.

    \subsection ParaMEDMEMOverlapDECAlgoStep5 Step 5 : Global matrix construction.
    
    After having performed the TODO list at the end of \ref ParaMEDMEMOverlapDECAlgoStep4 "Step4"
    we need to assemble the final matrix.
    
    The final aim is to have a distributed matrix \f$ M_k \f$ on each proc\#k. In order to reduce
    data exchange during the matrix product process,
    \f$ M_k \f$ is built using sizeof(Proc group) \c std::vector< \c std::map<int,double> \c >.

    For a proc\#k, it is necessary to fetch info of all matrices built in
    \ref ParaMEDMEMOverlapDECAlgoStep4 "Step4" where the first element in pair (i,j)
    is equal to k.

    After this step, the matrix repartition is the following after a call to
    MEDCoupling::OverlapMapping::prepare :

    - proc\#0 : (0,0),(1,0),(2,0)
    - proc\#1 : (0,1),(2,1)
    - proc\#2 : (1,2),(2,2)

    Tuple (2,1) computed on proc 2 is stored in proc 1 after execution of the function
    "prepare". This is an example of item 0 in \ref ParaMEDMEMOverlapDECAlgoStep2 "Step2".
    Tuple (0,1) computed on proc 1 is stored in proc 1 too. This is an example of item 1 in \ref ParaMEDMEMOverlapDECAlgoStep2 "Step2".

    In the end MEDCoupling::OverlapMapping::_proc_ids_to_send_vector_st will contain :

    - Proc\#0 : 0,1
    - Proc\#1 : 0,2
    - Proc\#2 : 0,1,2

    In the end MEDCoupling::OverlapMapping::_proc_ids_to_recv_vector_st will contain :

    - Proc\#0 : 0,1,2
    - Proc\#1 : 0,2
    - Proc\#2 : 1,2

    The method in charge to perform this is : MEDCoupling::OverlapMapping::prepare.
*/
  OverlapDEC::OverlapDEC(const std::set<int>& procIds, const MPI_Comm& world_comm):
      _load_balancing_algo(1),
      _own_group(true),_interpolation_matrix(0), _locator(0),
      _source_field(0),_own_source_field(false),
      _target_field(0),_own_target_field(false),
      _default_field_value(0.0),
      _comm(MPI_COMM_NULL)
  {
    MEDCoupling::CommInterface comm;
    int *ranks_world=new int[procIds.size()]; // ranks of sources and targets in world_comm
    std::copy(procIds.begin(),procIds.end(),ranks_world);
    MPI_Group group,world_group;
    comm.commGroup(world_comm,&world_group);
    comm.groupIncl(world_group,procIds.size(),ranks_world,&group);
    delete [] ranks_world;
    comm.commCreate(world_comm,group,&_comm);
    comm.groupFree(&group);
    comm.groupFree(&world_group);
    if(_comm==MPI_COMM_NULL)
      {
        _group=0;
        return ;
      }
    std::set<int> idsUnion;
    for(std::size_t i=0;i<procIds.size();i++)
      idsUnion.insert(i);
    _group=new MPIProcessorGroup(comm,idsUnion,_comm);
  }

  OverlapDEC::~OverlapDEC()
  {
    if(_own_group)
      delete _group;
    if(_own_source_field)
      delete _source_field;
    if(_own_target_field)
      delete _target_field;
    delete _interpolation_matrix;
    delete _locator;
    if (_comm != MPI_COMM_NULL)
      {
        MEDCoupling::CommInterface comm;
        comm.commFree(&_comm);
      }
  }

  void OverlapDEC::sendRecvData(bool way)
  {
    if(way)
      sendData();
    else
      recvData();
  }

  void OverlapDEC::sendData()
  {
    _interpolation_matrix->multiply(_default_field_value);
  }

  void OverlapDEC::recvData()
  {
    throw INTERP_KERNEL::Exception("Not implemented yet !!!!");
    //_interpolation_matrix->transposeMultiply();
  }
  
  void OverlapDEC::synchronize()
  {
    if(!isInGroup())
      return ;
    // Check number of components of field on both side (for now allowing void field/mesh on one proc is not allowed)
    if (!_source_field || !_source_field->getField())
      throw INTERP_KERNEL::Exception("OverlapDEC::synchronize(): currently, having a void source field on a proc is not allowed!");
    if (!_target_field || !_target_field->getField())
      throw INTERP_KERNEL::Exception("OverlapDEC::synchronize(): currently, having a void target field on a proc is not allowed!");
    if (_target_field->getField()->getNumberOfComponents() != _source_field->getField()->getNumberOfComponents())
      throw INTERP_KERNEL::Exception("OverlapDEC::synchronize(): source and target field have different number of components!");
    delete _interpolation_matrix;
    _locator = new OverlapElementLocator(_source_field,_target_field,*_group, getBoundingBoxAdjustmentAbs(), _load_balancing_algo);
    _interpolation_matrix=new OverlapInterpolationMatrix(_source_field,_target_field,*_group,*this,*this, *_locator);
    _locator->copyOptions(*this);
    _locator->exchangeMeshes(*_interpolation_matrix);
    std::vector< std::pair<int,int> > jobs=_locator->getToDoList();
    std::string srcMeth=_locator->getSourceMethod();
    std::string trgMeth=_locator->getTargetMethod();
    for(std::vector< std::pair<int,int> >::const_iterator it=jobs.begin();it!=jobs.end();it++)
      {
        const MEDCouplingPointSet *src=_locator->getSourceMesh((*it).first);
        const DataArrayInt *srcIds=_locator->getSourceIds((*it).first);
        const MEDCouplingPointSet *trg=_locator->getTargetMesh((*it).second);
        const DataArrayInt *trgIds=_locator->getTargetIds((*it).second);
        _interpolation_matrix->computeLocalIntersection(src,srcIds,srcMeth,(*it).first,trg,trgIds,trgMeth,(*it).second);
      }
    _interpolation_matrix->prepare(_locator->getProcsToSendFieldData());
    _interpolation_matrix->computeSurfacesAndDeno();
  }

  void OverlapDEC::attachSourceLocalField(ParaFIELD *field, bool ownPt)
  {
    if(!isInGroup())
      return ;
    if(_own_source_field)
      delete _source_field;
    _source_field=field;
    _own_source_field=ownPt;
  }

  void OverlapDEC::attachTargetLocalField(ParaFIELD *field, bool ownPt)
  {
    if(!isInGroup())
      return ;
    if(_own_target_field)
      delete _target_field;
    _target_field=field;
    _own_target_field=ownPt;
  }

  void OverlapDEC::attachSourceLocalField(MEDCouplingFieldDouble *field)
  {
    if(!isInGroup())
      return ;

    ParaMESH *paramesh = new ParaMESH(static_cast<MEDCouplingPointSet *>(const_cast<MEDCouplingMesh *>(field->getMesh())),
                                      *_group,field->getMesh()->getName());
    ParaFIELD *tmpField=new ParaFIELD(field, paramesh, *_group);
    tmpField->setOwnSupport(true);
    attachSourceLocalField(tmpField,true);
  }

  void OverlapDEC::attachTargetLocalField(MEDCouplingFieldDouble *field)
  {
    if(!isInGroup())
      return ;

    ParaMESH *paramesh = new ParaMESH(static_cast<MEDCouplingPointSet *>(const_cast<MEDCouplingMesh *>(field->getMesh())),
                                      *_group,field->getMesh()->getName());
    ParaFIELD *tmpField=new ParaFIELD(field, paramesh, *_group);
    tmpField->setOwnSupport(true);
    attachTargetLocalField(tmpField,true);
  }

  void OverlapDEC::attachSourceLocalField(ICoCo::MEDField *field)
  {
    attachSourceLocalField(field->getMCField());
  }

  void OverlapDEC::attachTargetLocalField(ICoCo::MEDField *field)
  {
    attachTargetLocalField(field->getMCField());
  }

  bool OverlapDEC::isInGroup() const
  {
    if(!_group)
      return false;
    return _group->containsMyRank();
  }

  void OverlapDEC::debugPrintWorkSharing(std::ostream & ostr) const
  {
    _locator->debugPrintWorkSharing(ostr);
  }
}
