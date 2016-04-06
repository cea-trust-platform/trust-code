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
// File:        Champ_input_P0.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_input_P0.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Interprete.h>
#include <Domaine.h>
#include <Exceptions.h>
#include <communications.h>
#include <Convert_ICoCoTrioField.h>

Implemente_instanciable(Champ_input_P0,"Champ_input_P0",Champ_Fonc_P0_base);

using ICoCo::WrongArgument;

Entree& Champ_input_P0::readOn(Entree& is)
{
  sous_zone_ok=true;
  read(is);

  valeurs_.resize(0, nb_compo_);
  mon_pb->domaine().zone(0).creer_tableau_elements(valeurs_);
  associer_zone_dis_base(mon_pb->domaine_dis().zone_dis(0));
  mon_pb->addInputField(*this);


  if (initial_value_.size_array()>0)
    {
      int nb_elem_tot=valeurs_.dimension_tot(0);
      for (int ele=0; ele< nb_elem_tot; ele++)
        for (int c=0; c<nb_compo_; c++)
          valeurs_(ele,c)=initial_value_(c);
    }

  return is;
}

Sortie& Champ_input_P0::printOn(Sortie& os) const
{
  return os;
}

void Champ_input_P0::set_nb_comp(int i)
{
  fixer_nb_comp(i);
}

void Champ_input_P0::set_name(const Nom& name)
{
  nommer(name);
}

const Nom& Champ_input_P0::get_name() const
{
  return le_nom();
}

// Description
// Provides afield with a name, a time interval, components, and no field ownership.
// The geometry includes all nodes of the domain and all elements of the sous_zone or domain.
// WEC : optimization possible here (recalculated each time)
void Champ_input_P0::getTemplate(TrioField& afield) const
{

  afield.clear();
  afield.setName(le_nom().getChar());

  afield._time1=mon_pb->presentTime();
  afield._time2=mon_pb->futureTime();

  afield._nb_field_components=nb_comp();

  const DoubleTab& sommets=mon_pb->domaine().les_sommets();
  const IntTab& elems=mon_pb->domaine().zone(0).les_elems();

  assert(sommets.nb_dim()==2);
  assert(elems.nb_dim()==2);

  afield._space_dim=sommets.dimension(1);
  afield._mesh_dim=afield._space_dim;
  afield._nbnodes=sommets.dimension(0);
  int npe=::mp_max(elems.dimension(1));
  afield._nodes_per_elem=npe;

  // Includes all the nodes, even those not used in connectivity
  // order is  x y z  x y z  x y z
  affecte_double_avec_doubletab(&afield._coords,sommets);

  assert (sizeof(int)==sizeof(int));
  if (ma_sous_zone.non_nul())
    {
      const Sous_Zone& ssz=ma_sous_zone.valeur();
      afield._nb_elems=ssz.nb_elem_tot();
      afield._connectivity=new True_int[afield._nb_elems*npe];
      for (int i=0; i<afield._nb_elems; i++)
        for (int j=0; j<npe; j++)
          afield._connectivity[i*npe+j]=elems(ssz[i],j);
    }
  else
    {
      afield._nb_elems=elems.dimension(0);
      affecte_int_avec_inttab(&afield._connectivity,elems);

    }
}

// Description
// Fills valeurs_ on either the whole domain or a subzone
void Champ_input_P0::setValue(const TrioField& afield)
{
  if (ma_sous_zone.non_nul())
    {
      const Sous_Zone& ssz=ma_sous_zone.valeur();
      if (afield._nb_elems!=ssz.nb_elem_tot())
        throw WrongArgument(mon_pb.le_nom().getChar(),"setInputField","afield","should have the same _nb_elems as returned by getInputFieldTemplate");
      assert(valeurs_.dimension(1)==nb_compo_);
      if (afield._nb_field_components!=valeurs_.dimension(1))
        throw WrongArgument(mon_pb.le_nom().getChar(),"setInputField","afield","should have the same _nb_field_components as returned by getInputFieldTemplate");
      for (int i=0; i<afield._nb_elems; i++)
        for (int j=0; j<nb_compo_; j++)
          valeurs_(ssz[i],j)=afield._field[i*nb_compo_+j];
    }
  else
    Champ_Input_Proto::setValueOnTab(afield,valeurs_);

}
