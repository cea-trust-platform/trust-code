/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Champ_val_tot_sur_vol_VDF.h>
#include <Equation_base.h>
#include <Milieu_base.h>
#include <Sous_Domaine.h>
#include <Domaine_VF.h>

Implemente_instanciable(Champ_val_tot_sur_vol_VDF,"Valeur_totale_sur_volume_VDF",Champ_val_tot_sur_vol_base);


Sortie& Champ_val_tot_sur_vol_VDF::printOn(Sortie& os) const
{
  Champ_val_tot_sur_vol_base::printOn(os);
  return os;
}

Entree& Champ_val_tot_sur_vol_VDF::readOn(Entree& is)
{
  Champ_val_tot_sur_vol_base::readOn(is);
  return is;
}

DoubleVect& Champ_val_tot_sur_vol_VDF::eval_contrib_loc(const Domaine_dis_base& zdis,const Domaine_Cl_dis_base& zcldis,DoubleVect& vol_glob_pond)
{
  const Domaine_VF& zvf = ref_cast(Domaine_VF,zdis);
  const int nb_elem = zvf.nb_elem();
  int size_vol = les_sous_domaines.size()+1;
  vol_glob_pond.resize(size_vol);

  const DoubleVect& vol = zvf.volumes();
  const DoubleVect& por_elem = zcldis.equation().milieu().porosite_elem();

  int cpt=1;
  for (auto& itr : les_sous_domaines)
    {
      const Sous_Domaine& sz = itr.valeur();
      int size_sz = sz.nb_elem_tot();
      int el;

      for (int elem=0; elem<size_sz; elem++)
        {
          el = sz(elem);
          //On ne retient que les elements reels
          if (el<nb_elem)
            vol_glob_pond(cpt) += vol(el)*por_elem(el);
        }
      cpt++;
    }

  for (int elem=0; elem<nb_elem; elem++)
    vol_glob_pond(0) += vol(elem)*por_elem(elem);

  vol_glob_pond(0) = mp_sum(vol_glob_pond(0));

  for (int i=1; i<size_vol; i++)
    {
      vol_glob_pond(i) = mp_sum(vol_glob_pond(i));
      vol_glob_pond(0) -= vol_glob_pond(i);
    }

  return vol_glob_pond;
}
