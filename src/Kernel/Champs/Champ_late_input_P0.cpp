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

#include <Champ_late_input_P0.h>
#include <Probleme_base.h>
#include <Domaine.h>

Implemente_instanciable(Champ_late_input_P0,"Champ_late_input_P0",Champ_input_P0);

Sortie& Champ_late_input_P0::printOn(Sortie& os) const { return os; }

Entree& Champ_late_input_P0::readOn(Entree& is)
{
  sous_zone_ok=true;
  read(is);
  return is;
}

void Champ_late_input_P0::complete_readOn()
{
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
}
