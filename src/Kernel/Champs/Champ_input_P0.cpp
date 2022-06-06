/****************************************************************************
* Copyright (c) 2022, CEA
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
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_input_P0.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Interprete.h>
#include <Domaine.h>
#include <ICoCoExceptions.h>

#include <Convert_ICoCoTrioField.h>
#include <Zone_VF.h>

Implemente_instanciable(Champ_input_P0,"Champ_input_P0",Champ_Fonc_P0_base);

using ICoCo::WrongArgument;

Entree& Champ_input_P0::readOn(Entree& is)
{
//#ifdef INT_is_64_
//  Cerr << "Champ_input_P0 uses medcoupling and is not supported for 64bits integer indices" << finl;
//  Process::exit();
//#endif

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
          valeurs_(ele,c)=initial_value_[c];
    }

  if (ma_sous_zone.non_nul())
    {
      const Sous_Zone& ssz=ma_sous_zone.valeur();
      nb_elems_reels_loc_ = mon_pb->domaine().zone(0).les_elems().dimension(0);
      for (int i = nb_elems_reels_sous_zone_ = 0; i < ssz.nb_elem_tot(); i++)
        nb_elems_reels_sous_zone_ += (ssz[i] < nb_elems_reels_loc_);
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

  /* coordonnees */
  const DoubleTab& sommets=mon_pb->domaine().les_sommets();
  afield._nbnodes=sommets.dimension(0);
  afield._mesh_dim = afield._space_dim = sommets.dimension(1);
  affecte_double_avec_doubletab(&afield._coords,sommets);

  /* connectivites */
  const Zone_VF& zvf = ref_cast(Zone_VF, zone_dis_base());
  afield._nb_elems = ma_sous_zone.non_nul() ? nb_elems_reels_sous_zone_ : zvf.nb_elem();
  Motcle type_elem = zvf.zone().type_elem()->que_suis_je();
  if (type_elem != "POLYEDRE") //cas simple -> il suffit de copier les_elems
    {
      const IntTab& conn = zvf.zone().les_elems();
      //le seul moyen qu'on a d'eviter que des polygones soient pris pour des quadrilateres est d'avoir un tableau de connectivite de largeur > 4...
      afield._nodes_per_elem = std::max(conn.dimension(1), type_elem == "POLYGONE" ? (int) 5 : 0);
      afield._connectivity = new int[afield._nb_elems * afield._nodes_per_elem];
      for (int i = 0; i < afield._nb_elems; i++)
        for (int j = 0; j < afield._nodes_per_elem; j++)
          {
            const int e = ma_sous_zone.non_nul() ? ma_sous_zone.valeur()[i] : i; //numero de l'element
            afield._connectivity[afield._nodes_per_elem * i + j] = j < conn.dimension(1) ? conn(e, j) : -1;
          }
    }
  else //polyedres -> il faut reconstruire une connectivite de type MEDCoupling a la main
    {
      afield._nodes_per_elem = zvf.elem_faces().dimension(1) * (zvf.face_sommets().dimension(1) + 1); //un -1 apres chaque face
      int *p = afield._connectivity = new int[afield._nb_elems * afield._nodes_per_elem];
      for (int i = 0, j, k, e, f, s; i < (ma_sous_zone.non_nul() ? ma_sous_zone.valeur().nb_elem_tot() : zvf.nb_elem()); i++)
        {
          if (ma_sous_zone.non_nul() && ma_sous_zone.valeur()[i] >= zvf.nb_elem()) continue; //element non reel de la sous-zone -> on saute
          e = ma_sous_zone.non_nul() ? ma_sous_zone.valeur()[i] : i; //numero de l'element
          int *pf = p + afield._nodes_per_elem; //fin de la ligne
          /* insertion de la connectivite de chaque face, suivie d'un -1 */
          for (j = 0; j < zvf.elem_faces().dimension(1) && (f = zvf.elem_faces(e, j)) >= 0; j++, *p = -1, p++)
            for (k = 0; k < zvf.face_sommets().dimension(1) && (s = zvf.face_sommets(f, k)) >= 0; k++) *p = s, p++;
          /* des -1 jusqu'a la ligne suivante */
          for ( ; p < pf; p++) *p = -1;
        }
    }
}

// Description
// Fills valeurs_ on either the whole domain or a subzone
void Champ_input_P0::setValue(const TrioField& afield)
{
  if (ma_sous_zone.non_nul())
    {
      const Sous_Zone& ssz=ma_sous_zone.valeur();
      if (afield._nb_elems != nb_elems_reels_sous_zone_)
        throw WrongArgument(mon_pb.le_nom().getChar(),"setInputField","afield","should have the same _nb_elems as returned by getInputFieldTemplate");
      assert(valeurs_.dimension(1)==nb_compo_);
      if (afield._nb_field_components!=valeurs_.dimension(1))
        throw WrongArgument(mon_pb.le_nom().getChar(),"setInputField","afield","should have the same _nb_field_components as returned by getInputFieldTemplate");
      for (int i=0, j, k = -1; i<ssz.nb_elem_tot(); i++)
        if (ssz[i] < nb_elems_reels_loc_)
          for (j = 0, k++ ; j<nb_compo_; j++)
            valeurs_(ssz[i],j)=afield._field[k*nb_compo_+j];
    }
  else
    Champ_Input_Proto::setValueOnTab(afield,valeurs_);

}
