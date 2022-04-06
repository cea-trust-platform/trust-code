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
// File:        Sortie_libre_Gradient_Pression_impose_VEFPreP1B.cpp
// Directory:   $TRUST_ROOT/src/P1NCP1B/Cond_Lim
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////



#include <Sortie_libre_Gradient_Pression_impose_VEFPreP1B.h>
#include <Navier_Stokes_std.h>
#include <Milieu_base.h>
#include <Champ_P1_isoP1Bulle.h>
#include <distances_VEF.h>
#include <Discretisation_base.h>
#include <Debog.h>
#include <Scatter.h>

Implemente_instanciable(Sortie_libre_Gradient_Pression_impose_VEFPreP1B,"Frontiere_ouverte_Gradient_Pression_impose_VEFPreP1B",Sortie_libre_Gradient_Pression_impose_VEF);


//// printOn
//

Sortie& Sortie_libre_Gradient_Pression_impose_VEFPreP1B::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Sortie_libre_Gradient_Pression_impose_VEFPreP1B::readOn(Entree& s )
{
  return Sortie_libre_Gradient_Pression_impose_VEF::readOn(s);
}

//// readOn
//

////////////////////////////////////////////////////////////////
//
//           Implementation de fonctions
//
//     de la classe Sortie_libre_Gradient_Pression_impose_VEFPreP1B
//
////////////////////////////////////////////////////////////////


int Sortie_libre_Gradient_Pression_impose_VEFPreP1B::initialiser(double temps)
{

  Cond_lim_base::initialiser(temps);
  const Zone_Cl_dis_base& la_zone_Cl = zone_Cl_dis();
  const Equation_base& eqn = la_zone_Cl.equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,eqn);
  const Champ_P1_isoP1Bulle& pression = ref_cast(Champ_P1_isoP1Bulle,eqn_hydr.pression().valeur());
  const Zone_VEF& ma_zone_VEF = ref_cast(Zone_VEF,eqn.zone_dis().valeur());
  const IntTab& face_voisins  = ma_zone_VEF.face_voisins();
  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  pression_interne = pression;
  int nb_faces_loc = le_bord.nb_faces_tot();
  trace_pression_int.resize(nb_faces_loc);
  coeff.resize(nb_faces_loc);
  for (int ind_face=0; ind_face<nb_faces_loc ; ind_face++)
    {
      int face = le_bord.num_face(ind_face);
      int elem=face_voisins(face,0);
      if (dimension==2)
        coeff[ind_face] = distance_2D(face,elem,ma_zone_VEF) * 3. ;
      else
        coeff[ind_face] = distance_3D(face,elem,ma_zone_VEF) * 4. ;
    }
  // On n'appelle pas mettre_a_jour car la mise a jour du champ_front pourrait dependre de l'exterieur !
  return calculer_trace_pression();
}

void Sortie_libre_Gradient_Pression_impose_VEFPreP1B::mettre_a_jour(double temps)
{
  Cond_lim_base::mettre_a_jour(temps);
  if (!calculer_trace_pression())
    {
      Cerr << "Erreur dans Sortie_libre_Gradient_Pression_impose_VEFPreP1B::mettre_a_jour" << finl;
      exit();
    }
}

// Retourne 1 si OK, 0 sinon.
int Sortie_libre_Gradient_Pression_impose_VEFPreP1B::calculer_trace_pression()
{
  if (!pression_interne.non_nul())
    {
      Cerr << "Sortie_libre_Gradient_Pression_impose_VEFPreP1B::calculer_trace_pression() : la pression n'est pas definie!" << finl;
      return 0;
    }
  Debog::verifier("pression_interne:",pression_interne);
  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  int nb_faces_loc = le_bord.nb_faces_tot();
  for (int ind_face=0; ind_face<nb_faces_loc; ind_face++)
    {
      int face = le_bord.num_face(ind_face);
      trace_pression_int[ind_face] = pression_interne->valeur_au_bord(face);
    }
  return 1;
}

int Sortie_libre_Gradient_Pression_impose_VEFPreP1B::
compatible_avec_discr(const Discretisation_base& discr) const
{
  if (discr.que_suis_je() == "VEFPreP1B")
    return 1;
  else
    {
      err_pas_compatible(discr);
      return 0;
    }
}



