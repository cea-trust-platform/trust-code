/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        Convert_ICoCoTrioField.cpp
// Directory:   $TRUST_ROOT/src/Kernel/ICoCo
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Convert_ICoCoTrioField.h>
#include <ICoCoTrioField.h>

#include <ICoCoMEDField.hxx>
#include <Domaine.h>
#include <Champ_base.h>
#include <Zone_dis_base.h>
#include <PE_Groups.h>
#include <Comm_Group.h>

void affecte_double_avec_doubletab(double** p, const ArrOfDouble& trio)
{
  *p=new double[trio.size_array()];
  memcpy(*p,trio.addr(),trio.size_array()*sizeof(double));
}

void affecte_int_avec_inttab(True_int** p, const ArrOfInt& trio)
{
  int sz=trio.size_array();
  *p=new True_int[sz];
  if (sizeof(int)==sizeof(True_int))
    memcpy(*p,trio.addr(),sz*sizeof(int));
  else
    for (int i=0; i<sz; i++)
      (*p)[i]=True_int(trio[i]);
}

ICoCo::TrioField build_ICoCoField(const std::string& name,const Domaine& dom,  const DoubleTab& values,const int is_p1, const double& t1,const double& t2 )
{
  ICoCo::TrioField afield;
  afield.clear();
  afield._space_dim=dom.dimension;
  afield.setName(name);

  afield._mesh_dim=afield._space_dim;

  const DoubleTab& coord=dom.les_sommets();
  //ch.get_copy_coordinates(coord);
  affecte_double_avec_doubletab(&afield._coords,coord);

  afield._nbnodes=coord.dimension(0);
  Motcle type_elem_=dom.zone(0).type_elem()->que_suis_je();
  Motcle type_elem(type_elem_);
  type_elem.prefix("_AXI");
  if (type_elem!=Motcle(type_elem_))
    {
      if (type_elem=="QUADRILATERE_2D")
        type_elem="SEGMENT_2D";
      if (type_elem=="RECTANGLE_2D")
        type_elem="RECTANGLE";

    }
  if ((type_elem=="RECTANGLE") ||(type_elem=="QUADRANGLE")||(type_elem=="TRIANGLE")|| (type_elem=="TRIANGLE_3D")||(type_elem=="QUADRANGLE_3D"))
    afield._mesh_dim=2;
  else if ((type_elem=="HEXAEDRE")|| (type_elem=="HEXAEDRE_VEF")||(type_elem=="POLYEDRE")||(type_elem=="PRISME")||  (type_elem=="TETRAEDRE"))
    afield._mesh_dim=3;
  else if ((type_elem=="SEGMENT_2D")||(type_elem=="SEGMENT"))
    afield._mesh_dim=1;
  else
    {
      Cerr<<type_elem<< " not coded" <<finl;
      Process::exit();
    }

  const IntTab& les_elems=dom.zone(0).les_elems();
  //ch.get_copy_connectivity(ELEMENT,NODE,les_elems);
  affecte_int_avec_inttab(&afield._connectivity,les_elems);

  afield._nodes_per_elem=les_elems.dimension(1);
  afield._nb_elems=les_elems.dimension(0);

  afield._itnumber=0;
  afield._time1=t1;
  afield._time2=t2;



  afield._type=is_p1;

  afield._has_field_ownership=true;
  affecte_double_avec_doubletab(&afield._field,values);
  if (values.nb_dim()>1)
    afield._nb_field_components=values.dimension(1);
  else
    afield._nb_field_components=(1);
  return afield;
}
ICoCo::TrioField buildTrioField_from_champ_base(const Champ_base& ch)
{
  if (ch.a_une_zone_dis_base())
    {
      int is_p1= 0;
      double t1=ch.temps();
      double t2=t1;
      return build_ICoCoField(ch.le_nom().getString(),ch.zone_dis_base().zone().domaine(),  ch.valeurs(), is_p1,t1,t2 );

    }
  else
    {
      Cerr<<"In ICoCo::TrioField buildTrioField_from_champ_base "<<ch.le_nom()<<" has no zone_dis"<<finl;
      Process::exit();
    }
  // on arrive jamais la
  throw;
}




#ifndef NO_MEDFIELD
#include <MEDCouplingUMesh.hxx>
#include <MEDCouplingFieldDouble.hxx>
#include <MCAuto.hxx>

#include <string.h>
#include <iostream>
#include <iomanip>

using ICoCo::TrioField;
using ICoCo::MEDField;
using std::vector;


/*!
 * This method is non const only due to this->_field that can be modified (to point to the same zone than returned object).
 * So \b warning, to access to \a this->_field only when the returned object is alive.
 */
MEDField build_medfield(TrioField& triofield)
{
  MEDCoupling::MCAuto<MEDCoupling::MEDCouplingUMesh> mesh(MEDCoupling::MEDCouplingUMesh::New("",triofield._mesh_dim));
  MEDCoupling::MCAuto<MEDCoupling::DataArrayDouble> coo(MEDCoupling::DataArrayDouble::New());
  coo->alloc(triofield._nbnodes,triofield._space_dim);
  mesh->setCoords(coo);
  double *ptr(coo->getPointer());
  std::copy(triofield._coords,triofield._coords+triofield._space_dim*triofield._nbnodes,ptr);
  mesh->allocateCells(triofield._nb_elems);
  INTERP_KERNEL::NormalizedCellType elemtype;
  switch(triofield._mesh_dim)
    {
    case 0 :
      {
        switch (triofield._nodes_per_elem)
          {
          case 0: // cas field vide
            elemtype=INTERP_KERNEL::NORM_SEG2; // pour eviter warning
            break;
          default:
            throw INTERP_KERNEL::Exception("incompatible Trio field - wrong nb of nodes per elem");
          }
        break;
      }
    case 1:
      {
        switch (triofield._nodes_per_elem)
          {
          case 2:
            elemtype=INTERP_KERNEL::NORM_SEG2;
            break;
          default:
            throw INTERP_KERNEL::Exception("incompatible Trio field - wrong nb of nodes per elem");
          }
        break;
      }
    case 2:
      {
        switch (triofield._nodes_per_elem)
          {
          case 3:
            elemtype=INTERP_KERNEL::NORM_TRI3;
            break;
          case 4 :
            elemtype=INTERP_KERNEL::NORM_QUAD4;
            break;
          default:
            throw INTERP_KERNEL::Exception("incompatible Trio field - wrong nb of nodes per elem");
          }
        break;
      }
    case 3:
      {
        switch (triofield._nodes_per_elem)
          {
          case 4:
            elemtype=INTERP_KERNEL::NORM_TETRA4;
            break;
          case 8 :
            elemtype=INTERP_KERNEL::NORM_HEXA8;
            break;
          default:
            throw INTERP_KERNEL::Exception("incompatible Trio field - wrong nb of nodes per elem");
          }
        break;
      default:
        throw INTERP_KERNEL::Exception("incompatible Trio field - wrong mesh dimension");
      }
    }
  //creating a connectivity table that complies to MED (1 indexing)
  //and passing it to _mesh
  MEDCoupling::MCAuto<MEDCoupling::MEDCouplingFieldDouble> field;
  True_int *conn(new True_int[triofield._nodes_per_elem]);
  for (int i=0; i<triofield._nb_elems; i++)
    {

      for(int j=0; j<triofield._nodes_per_elem; j++)
        {
          conn[j]=triofield._connectivity[i*triofield._nodes_per_elem+j];
        }
      if (elemtype==INTERP_KERNEL::NORM_QUAD4)
        {
          // dans trio pas la meme numerotation
          True_int tmp=conn[3];
          conn[3]=conn[2];
          conn[2]=tmp;
        }
      if (elemtype==INTERP_KERNEL::NORM_HEXA8)
        {
          // dans trio pas la meme numerotation
          True_int tmp=conn[3];
          conn[3]=conn[2];
          conn[2]=tmp;
          tmp=conn[7];
          conn[7]=conn[6];
          conn[6]=tmp;
        }
      mesh->insertNextCell(elemtype,triofield._nodes_per_elem,conn);
    }
  delete [] conn;
  mesh->finishInsertingCells();
  //


  std::vector<int> cells;
  if ((mesh->getSpaceDimension() == 2 || mesh->getSpaceDimension() == 3) && mesh->getMeshDimension() == 2)
    mesh->checkButterflyCells(cells);
  if (!cells.empty())
    {
      Cerr<<" cells are butterflyed "<<cells[0]<<finl;
      PE_Groups::groupe_TRUST().abort();
      Process::abort();
      Process::exit();
    }
  //field on the sending end
  int nb_case=triofield.nb_values();
  if (triofield._type==0)
    {
      field =  MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_CELLS,MEDCoupling::ONE_TIME);
    }
  else
    {
      field =  MEDCoupling::MEDCouplingFieldDouble::New(MEDCoupling::ON_NODES,MEDCoupling::ONE_TIME );
    }
  field->setMesh(mesh);
  field->setNature(MEDCoupling::IntensiveMaximum);
  MEDCoupling::MCAuto<MEDCoupling::DataArrayDouble> fieldArr(MEDCoupling::DataArrayDouble::New());
  fieldArr->alloc(field->getNumberOfTuplesExpected(),triofield._nb_field_components);
  field->setName(triofield.getName().c_str());
  std::string meshName("SupportOf_");
  meshName+=triofield.getName();
  mesh->setName(meshName.c_str());
  field->setTime(triofield._time1,0,triofield._itnumber);
  if (triofield._field!=0)
    {
      for (int i =0; i<nb_case; i++)
        for (int j=0; j<triofield._nb_field_components; j++)
          {
            fieldArr->setIJ(i,j,triofield._field[i*triofield._nb_field_components+j]);
          }
    }
  //field on the receiving end
  else
    {
      // the trio field points to the pointer inside the MED field
      triofield._field=fieldArr->getPointer();
      for (int i=0; i<triofield._nb_field_components*nb_case; i++)
        triofield._field[i]=0.0;
    }
  field->setArray(fieldArr);
  return MEDField(field);
}
MEDField build_medfield_from_champ_base(const Champ_base& ch)
{
  TrioField fl =  buildTrioField_from_champ_base(ch);


  return build_medfield(fl);
}


#else
namespace ICoCo
{
class MEDField
{
};
}
ICoCo::MEDField build_medfield(ICoCo::TrioField& ch)
{
  Cerr<<"Version compiled without MEDCoupling"<<finl;
  Process::exit();
  throw;
}
ICoCo::MEDField build_medfield_from_champ_base(const Champ_base& ch)
{
  Cerr<<"Version compiled without MEDCoupling"<<finl;
  Process::exit();
  throw;
}
#endif
