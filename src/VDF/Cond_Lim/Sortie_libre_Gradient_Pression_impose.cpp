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

#include <Sortie_libre_Gradient_Pression_impose.h>
#include <Domaine_Cl_dis_base.h>
#include <Navier_Stokes_std.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>
#include <Champ_P0_VDF.h>
#include <Domaine_VDF.h>

Implemente_instanciable(Sortie_libre_Gradient_Pression_impose, "Frontiere_ouverte_Gradient_Pression_impose", Neumann_sortie_libre);
// XD frontiere_ouverte_gradient_pression_impose neumann frontiere_ouverte_gradient_pression_impose -1 Normal imposed pressure gradient condition on the open boundary called bord (edge). This boundary condition may be only used in VDF discretization. The imposed $\partial P/\partial n$ value is expressed in Pa.m-1.
// XD attr ch front_field_base ch 0 Boundary field type.


Sortie& Sortie_libre_Gradient_Pression_impose::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Sortie_libre_Gradient_Pression_impose::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Hydraulique"), Motcle("indetermine") };
  if (supp_discs.size() == 0) supp_discs = { Nom("VDF") };

  s >> le_champ_front;
  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext.valeurs().resize(1, dimension);
  return s;
}

void Sortie_libre_Gradient_Pression_impose::completer()
{
  Cerr << "Sortie_libre_Gradient_Pression_impose::completer()" << finl;
  const Domaine_Cl_dis_base& le_dom_Cl = domaine_Cl_dis();
  const Equation_base& eqn = le_dom_Cl.equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std, eqn);
  const Domaine_VDF& domaine_vdf = ref_cast(Domaine_VDF, eqn.domaine_dis().valeur());
  const Champ_P0_VDF& pression = ref_cast(Champ_P0_VDF, eqn_hydr.pression().valeur());
  const IntTab& face_voisins = domaine_vdf.face_voisins();
  const DoubleVect& volumes_entrelaces = domaine_vdf.volumes_entrelaces();
  const DoubleVect& face_surfaces = domaine_vdf.face_surfaces();

  le_dom_VDF = domaine_vdf;
  pression_interne = pression;

  const Front_VF& le_bord = ref_cast(Front_VF, frontiere_dis());
  int nb_faces_loc = le_bord.nb_faces();
  int ndeb = le_bord.num_premiere_face();
  int face;

  coeff.resize(nb_faces_loc);
  trace_pression_int.resize(nb_faces_loc);

  for (face = ndeb; face < ndeb + nb_faces_loc; face++)
    if (face_voisins(face, 0) != -1)
      coeff[face - ndeb] = volumes_entrelaces[face] / face_surfaces[face];
    else
      coeff[face - ndeb] = -volumes_entrelaces[face] / face_surfaces[face];

  Cerr << "Sortie_libre_Gradient_Pression_impose::completer() ok" << finl;
}

void Sortie_libre_Gradient_Pression_impose::mettre_a_jour(double temps)
{
  //Cerr<<"Sortie_libre_Gradient_Pression_impose::mettre_a_jour"<<finl;
  Cond_lim_base::mettre_a_jour(temps);

  const Front_VF& le_bord = ref_cast(Front_VF, frontiere_dis());
  int ndeb = le_bord.num_premiere_face();
  int nb_faces_loc = le_bord.nb_faces();

  assert(pression_interne.non_nul());
  for (int face = ndeb; face < ndeb + nb_faces_loc; face++)
    trace_pression_int[face - ndeb] = pression_interne->valeur_au_bord(face);
}

double Sortie_libre_Gradient_Pression_impose::flux_impose(int face) const
{
  if (le_champ_front.valeurs().size() == 1)
    return (trace_pression_int[face] + coeff[face] * le_champ_front(0, 0));
  else if (le_champ_front.valeurs().line_size() == 1)
    return (trace_pression_int[face] + coeff[face] * le_champ_front(face, 0));
  else
    Cerr << "Sortie_libre_Gradient_Pression_impose::flux_impose erreur" << finl;
  exit();
  return 0.;
}

double Sortie_libre_Gradient_Pression_impose::flux_impose(int face, int ncomp) const
{
  if (ncomp == 0) return flux_impose(face);

  Cerr << "Sortie_libre_Gradient_Pression_impose::flux_impose(int  , int )" << finl;
  Cerr << "On ne sait imposer que la composante normale du gradient" << finl;
  Process::exit();
  return 0.;
}

double Sortie_libre_Gradient_Pression_impose::grad_P_imp(int face) const
{
  const Milieu_base& mil = mon_dom_cl_dis->equation().milieu();
  if (sub_type(Champ_Uniforme, mil.masse_volumique().valeur()))
    {
      const Champ_Uniforme& rho = ref_cast(Champ_Uniforme, mil.masse_volumique().valeur());
      double d_rho = rho(0, 0);
      if (le_champ_front.valeurs().size() == 1)
        return le_champ_front(0, 0) / d_rho;
      else if (le_champ_front.valeurs().line_size() == 1)
        return le_champ_front(face, 0) / d_rho;
      else
        Cerr << "Sortie_libre_Gradient_Pression_impose::grad_P_imp() erreur" << finl;
    }
  else
    {
      //quasi compressible
      const DoubleTab& tab_rho = mil.masse_volumique().valeurs();
      int elem = le_dom_VDF->face_voisins(face, 0);
      if (elem == -1)
        elem = le_dom_VDF->face_voisins(face, 1);
      double d_rho = tab_rho(elem);
      if (le_champ_front.valeurs().size() == 1)
        return le_champ_front(0, 0) / d_rho;
      else if (le_champ_front.valeurs().line_size() == 1)
        return le_champ_front(face, 0) / d_rho;
      else
        Cerr << "Sortie_libre_Gradient_Pression_impose::grad_P_imp() erreur" << finl;
    }
  exit();
  return 0.;
}
