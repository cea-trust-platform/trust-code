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

#include <Paroi_hyd_base_VEF.h>
#include <Modele_turbulence_hyd_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Equation_base.h>
#include <TRUSTTrav.h>
#include <Scatter.h>

Implemente_base(Paroi_hyd_base_VEF, "Paroi_hyd_base_VEF", Turbulence_paroi_base);

Sortie& Paroi_hyd_base_VEF::printOn(Sortie& s) const { return s << que_suis_je() << " " << le_nom(); }

Entree& Paroi_hyd_base_VEF::readOn(Entree& s) { return Turbulence_paroi_base::readOn(s); }

void Paroi_hyd_base_VEF::associer(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis_base& domaine_Cl_dis)
{
  le_dom_VEF = ref_cast(Domaine_VEF, domaine_dis);
  le_dom_Cl_VEF = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis);
}

void Paroi_hyd_base_VEF::init_lois_paroi_()
{
  const Domaine_VF& zvf = le_dom_VEF.valeur();
  const int nb_faces_bord = le_dom_VEF->nb_faces_bord();
  tab_u_star_.resize(nb_faces_bord);
  tab_d_plus_.resize(nb_faces_bord);
  if (!Cisaillement_paroi_.get_md_vector().non_nul())
    {
      Cisaillement_paroi_.resize(0, dimension);
      zvf.creer_tableau_faces_bord(Cisaillement_paroi_);
    }
}

DoubleTab& Paroi_hyd_base_VEF::corriger_derivee_impl(DoubleTab& d) const
{
  if (flag_face_keps_imposee_)
    {
      int size = d.dimension_tot(0);
      assert(size == face_keps_imposee_.size_array());
      for (int face = 0; face < size; face++)
        {
          if (face_keps_imposee_(face) != -2)
            {
              d(face, 0) = 0;
              d(face, 1) = 0;
            }
        }
    }
  return d;
}

void Paroi_hyd_base_VEF::imprimer_premiere_ligne_ustar(int boundaries_, const LIST(Nom) &boundaries_list, const Nom& nom_fichier_) const
{
  imprimer_premiere_ligne_ustar_impl(boundaries_, boundaries_list, nom_fichier_, le_dom_VEF.valeur(), le_dom_Cl_VEF);
}

void Paroi_hyd_base_VEF::imprimer_ustar_mean_only(Sortie& os, int boundaries_, const LIST(Nom) &boundaries_list, const Nom& nom_fichier_) const
{
  imprimer_ustar_mean_only_impl(os, boundaries_, boundaries_list, nom_fichier_, le_dom_VEF.valeur(), le_dom_Cl_VEF);
}
