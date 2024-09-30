/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Loi_paroi_base.h>
#include <Pb_Multiphase.h>
#include <Domaine_VF.h>
#include <TRUSTTrav.h>
#include <Cond_lim_base.h>
#include <math.h>
#include <Dirichlet_loi_paroi.h>
#include <Frottement_global_impose.h>
#include <Echange_impose_base.h>

Implemente_base(Loi_paroi_base, "Loi_paroi_base", Correlation_base);

Sortie& Loi_paroi_base::printOn(Sortie& os) const
{
  return os;
}

Entree& Loi_paroi_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("eps_y_p", &eps_y_p_);
  param.lire_avec_accolades_depuis(is);

  return is;
}

void Loi_paroi_base::completer()
{
  Domaine_VF& domaine = ref_cast(Domaine_VF, pb_->domaine_dis());
  int nf_tot = domaine.nb_faces_tot();

  // We create the table that enables us to know for which BC we must calculate the boundary law
  Faces_a_calculer_.resize(nf_tot, 1);
  domaine.creer_tableau_faces(Faces_a_calculer_);

  for (int i = 0 ; i <pb_->nombre_d_equations() ; i++)
    for (int j = 0 ; j<pb_->equation(i).domaine_Cl_dis().nb_cond_lim(); j++)
      {
        Cond_lim& cond_lim_loc = pb_->equation(i).domaine_Cl_dis().les_conditions_limites(j);
        if sub_type(Dirichlet_loi_paroi, cond_lim_loc.valeur())
          ref_cast(Dirichlet_loi_paroi, cond_lim_loc.valeur()).liste_faces_loi_paroi(Faces_a_calculer_);  // met des 1 si doit remplir la table
        else if sub_type(Frottement_global_impose, cond_lim_loc.valeur())
          ref_cast(Frottement_global_impose, cond_lim_loc.valeur()).liste_faces_loi_paroi(Faces_a_calculer_);  // met des 1 si doit remplir la table
        else if sub_type(Echange_impose_base, cond_lim_loc.valeur())
          ref_cast(Echange_impose_base, cond_lim_loc.valeur()).liste_faces_loi_paroi(Faces_a_calculer_);  // met des 1 si doit remplir la table
      }
  Faces_a_calculer_.echange_espace_virtuel();

  valeurs_loi_paroi_["y_plus"] = DoubleTab(nf_tot,1); // pour l'instant, turbulence dans seulement une phase
  valeurs_loi_paroi_["u_tau"] = DoubleTab(nf_tot,1);

  DoubleTab& tab_y_p = valeurs_loi_paroi_["y_plus"];
  for (int i = 0 ; i < tab_y_p.dimension_tot(0) ; i ++)
    for (int n = 0 ; n < tab_y_p.dimension_tot(1) ; n++) tab_y_p(i,n) = ( (i<nf_tot) && (Faces_a_calculer_(i,0)) ) ? 1 : -1.;

  if (sub_type(Navier_Stokes_std, pb_->equation(0)) && pb_->has_champ("y_plus")) valeurs_loi_paroi_["y_plus_elem"] = DoubleTab(nf_tot,1);
}

void Loi_paroi_base::mettre_a_jour(double temps)
{
  if (temps > tps_loc)
    {
      const DoubleTab& vit = pb_->get_champ("vitesse").passe();
      const DoubleTab& nu  = ref_cast(Fluide_base, pb_->milieu()).viscosite_cinematique().valeurs();
      calc_y_plus(vit, nu);

      if (sub_type(Navier_Stokes_std, pb_->equation(0)) && pb_->has_champ("y_plus"))
        {
          Domaine_VF& domaine = ref_cast(Domaine_VF, pb_->domaine_dis());
          const IntTab& f_e = domaine.face_voisins();
          int nf_tot = domaine.nb_faces_tot();
          DoubleTab& y_p = valeurs_loi_paroi_["y_plus"], &y_p_e = valeurs_loi_paroi_["y_plus_elem"];
          for (int f = 0 ; f < nf_tot ; f ++)
            if (Faces_a_calculer_(f,0)==1)
              {
                int c = (f_e(f,0)>=0) ? 0 : 1 ;
                if (f_e(f, (c==0) ? 1 : 0 ) >= 0) Process::exit("Error in the definition of the boundary conditions for wall laws");
                int e = f_e(f,c);
                y_p_e(e,0) = y_p(f,0);
              }
          ref_cast(Navier_Stokes_std, pb_->equation(0)).update_y_plus(y_p_e);
        }
      tps_loc = temps;
    }
}

