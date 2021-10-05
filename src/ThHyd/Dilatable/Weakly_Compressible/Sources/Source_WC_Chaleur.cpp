/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Source_WC_Chaleur.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Weakly_Compressible/Sources
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_Weakly_Compressible.h>
#include <Source_WC_Chaleur.h>
#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Zone_VF.h>

Implemente_base(Source_WC_Chaleur,"Source_WC_Chaleur",Source_Chaleur_Fluide_Dilatable_base);

Sortie& Source_WC_Chaleur::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Source_WC_Chaleur::readOn(Entree& is)
{
  return is;
}

// Description:
//    Ajoute le terme source weakly compressible
//    d P_tot / d t = del P / del t + u.grad(P_tot)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Source_WC_Chaleur::ajouter(DoubleTab& resu) const
{
  double dt_ = mon_equation->schema_temps().temps_courant() - mon_equation->schema_temps().temps_precedent();
  if (dt_ <= 0.) return resu; // On calcul pas ce terme source si dt<=0

  Fluide_Weakly_Compressible& FWC = ref_cast_non_const(Fluide_Weakly_Compressible,le_fluide.valeur());
  DoubleTab& Ptot = FWC.pression_th_tab(), Ptot_n = FWC.pression_thn_tab(); // present & passe

  Ptot.echange_espace_virtuel();

  DoubleTab UgradP(Ptot); // champ elem en vdf et face en vef
  compute_interpolate_gradP(UgradP,Ptot);

  assert (resu.dimension(0) == UgradP.dimension(0));
  assert (resu.size() == UgradP.size());

  for (int i=0 ; i< resu.dimension(0); i++)
    {
      double dpth = ( Ptot(i,0) - Ptot_n(i,0) ) / dt_ +  UgradP(i,0);
      resu(i) += dpth * volumes(i)*porosites(i) ;
    }
  return resu;
}

const DoubleTab& Source_WC_Chaleur::correct_grad_boundary(const Zone_VF& zone, DoubleTab& grad_Ptot) const
{
  // We dont have a CL for Ptot => we apply explicitly to have a null gradient on the boundary ...
  const Zone_Cl_dis& zone_cl = mon_equation->zone_Cl_dis();
  for (int n_bord=0; n_bord<zone.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_cl.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      // recuperer face et remplace gradient par 0
      const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
      for (int num_face=ndeb; num_face<nfin; num_face++) grad_Ptot(num_face,0) = 0.;
    }
  return grad_Ptot;
}
