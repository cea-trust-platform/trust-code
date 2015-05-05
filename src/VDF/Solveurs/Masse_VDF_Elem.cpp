/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Masse_VDF_Elem.cpp
// Directory:   $TRUST_ROOT/src/VDF/Solveurs
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Masse_VDF_Elem.h>
#include <Zone_Cl_VDF.h>
#include <Zone_VDF.h>
#include <Les_Cl.h>
#include <Debog.h>

Implemente_instanciable(Masse_VDF_Elem,"Masse_VDF_P0_VDF",Solveur_Masse_base);


//     printOn()
/////

Sortie& Masse_VDF_Elem::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

Entree& Masse_VDF_Elem::readOn(Entree& s)
{
  return s ;
}


////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Masse_VDF_Elem
//
////////////////////////////////////////////////////////////////


DoubleTab& Masse_VDF_Elem::appliquer_impl(DoubleTab& sm) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& porosite_elem = zone_VDF.porosite_elem();
  int nb_elem = zone_VDF.nb_elem();
  if(nb_elem==0)
    {
      sm.echange_espace_virtuel();
      return sm;
    }
  int nb_comp = sm.size()/nb_elem;
  int nb_dim=sm.nb_dim();
  assert((nb_comp*nb_elem == sm.size())||(nb_dim==3));
  if (nb_dim == 1)
    for (int num_elem=0; num_elem<nb_elem; num_elem++)
      sm(num_elem) /= (volumes(num_elem)*porosite_elem(num_elem));
  else if (nb_dim == 2)
    {
      for (int num_elem=0; num_elem<nb_elem; num_elem++)
        for (int k=0; k<nb_comp; k++)
          sm(num_elem,k) /= (volumes(num_elem)*porosite_elem(num_elem));
    }
  else if (sm.nb_dim() == 3)
    {
      //int d0=sm.dimension(0);
      int d1=sm.dimension(1);
      int d2=sm.dimension(2);
      for (int num_elem=0; num_elem<nb_elem; num_elem++)
        for (int k=0; k<d1; k++)
          for (int d=0; d<d2; d++)
            sm(num_elem,k,d) /= (volumes(num_elem)*porosite_elem(num_elem));
    }
  else
    {
      Cerr<< "Masse_VDF_Elem::appliquer ne peut pas s'appliquer a un DoubleTab a "<<sm.nb_dim()<<" dimensions"<<endl;
      assert(0);
      exit();
    }
  sm.echange_espace_virtuel();
  return sm;
}

void Masse_VDF_Elem::associer_zone_dis_base(const Zone_dis_base& la_zone_dis_base)
{
  la_zone_VDF = ref_cast(Zone_VDF, la_zone_dis_base);
}

void Masse_VDF_Elem::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis_base)
{
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, la_zone_Cl_dis_base);
}
