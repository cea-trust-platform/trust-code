/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Modele_turbulence_hyd_base.h>
#include <T_paroi_Champ_P1NC.h>
#include <Champ_P1NC.h>

Implemente_instanciable(T_paroi_Champ_P1NC, "T_paroi_Champ_P1NC", Champ_Fonc_P0_VEF);

Sortie& T_paroi_Champ_P1NC::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& T_paroi_Champ_P1NC::readOn(Entree& s) { return s; }

void T_paroi_Champ_P1NC::mettre_a_jour(double tps)
{
  me_calculer(tps);
  changer_temps(tps);
  Champ_Fonc_base::mettre_a_jour(tps);
}

void T_paroi_Champ_P1NC::associer_champ(const Champ_P1NC& un_champ)
{
  mon_champ_ = un_champ;
}

void T_paroi_Champ_P1NC::me_calculer(double tps)
{
  const Domaine_dis_base& ds = mon_champ().equation().domaine_dis();
  const IntTab& face_voisins = ds.face_voisins();
  const int nb_bords = ds.nb_front_Cl();
  const DoubleTab& temp = mon_champ_->valeurs();
  DoubleTab& val = valeurs(tps);
  val = 0.;

  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_VEF->les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      for (int face = ndeb; face < nfin; face++)
        {
          const int elem = face_voisins(face, 0);
          val(elem) = temp(face);
        }
    }
}

const Domaine_Cl_dis_base& T_paroi_Champ_P1NC::domaine_Cl_dis_base() const
{
  return le_dom_Cl_VEF.valeur();
}
