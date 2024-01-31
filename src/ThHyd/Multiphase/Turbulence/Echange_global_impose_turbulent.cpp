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

#include <Echange_global_impose_turbulent.h>
#include <Probleme_base.h>
#include <Domaine_VF.h>
#include <Frontiere.h>
#include <Motcle.h>
#include <math.h>

Implemente_base(Echange_global_impose_turbulent,"Echange_global_impose_turbulent",Echange_global_impose);

Sortie& Echange_global_impose_turbulent::printOn(Sortie& s ) const {return Echange_global_impose::printOn(s);}

Entree& Echange_global_impose_turbulent::readOn(Entree& s )
{
  h_imp_.typer("Champ_front_vide");
  le_champ_front.typer("Champ_front_vide");
  return s;
}

void Echange_global_impose_turbulent::liste_faces_loi_paroi(IntTab& tab)
{
  int nf = la_frontiere_dis.valeur().frontiere().nb_faces(), f1 = la_frontiere_dis.valeur().frontiere().num_premiere_face();
  int N = tab.line_size();

  for (int f =0 ; f < nf ; f++)
    for (int n = 0 ; n<N ; n++)
      tab(f + f1, n) |= 1;
}

int Echange_global_impose_turbulent::compatible_avec_eqn(const Equation_base& eqn) const
{
  Motcle dom_app=eqn.domaine_application();
  Motcle Turbulence="Turbulence";

  if (dom_app==Turbulence)
    return 1;
  else err_pas_compatible(eqn);
  return 0;
}

void Echange_global_impose_turbulent::mettre_a_jour(double tps)
{
  if (mon_temps!=tps) {me_calculer() ; mon_temps=tps;}
}

int Echange_global_impose_turbulent::initialiser(double temps)
{
  h_.resize(0,domaine_Cl_dis().equation().inconnue().valeurs().line_size());
  la_frontiere_dis.valeur().frontiere().creer_tableau_faces(h_);

  h_grad_.resize(0,domaine_Cl_dis().equation().inconnue().valeurs().line_size());
  la_frontiere_dis.valeur().frontiere().creer_tableau_faces(h_grad_);

  T_.resize(0,domaine_Cl_dis().equation().inconnue().valeurs().line_size());
  la_frontiere_dis.valeur().frontiere().creer_tableau_faces(T_);

  correlation_loi_paroi_ = domaine_Cl_dis().equation().probleme().get_correlation("Loi_paroi");

  return 1;
}
