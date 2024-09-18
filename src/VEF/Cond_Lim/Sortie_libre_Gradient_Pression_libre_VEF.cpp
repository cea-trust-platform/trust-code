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

#include <Sortie_libre_Gradient_Pression_libre_VEF.h>
#include <Domaine_Cl_dis_base.h>
#include <Navier_Stokes_std.h>
#include <Champ_Uniforme.h>
#include <distances_VEF.h>
#include <Champ_P0_VEF.h>
#include <Milieu_base.h>
#include <Domaine_VEF.h>

Implemente_instanciable(Sortie_libre_Gradient_Pression_libre_VEF, "Frontiere_ouverte_Gradient_Pression_libre_VEF", Neumann_sortie_libre);
// XD frontiere_ouverte_gradient_pression_libre_vef neumann frontiere_ouverte_gradient_pression_libre_vef -1 Class for outlet boundary condition in VEF like Orlansky. There is no reference for pressure for theses boundary conditions so it is better to add pressure condition (with Frontiere_ouverte_pression_imposee) on one or two cells (for symmetry in a channel) of the boundary where Orlansky conditions are imposed.


Sortie& Sortie_libre_Gradient_Pression_libre_VEF::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Sortie_libre_Gradient_Pression_libre_VEF::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Hydraulique"), Motcle("indetermine") };
  if (supp_discs.size() == 0) supp_discs = { Nom("VEF") };

  le_champ_front.typer("Champ_front_uniforme");
  le_champ_front->valeurs().resize(1, dimension);
  le_champ_front->fixer_nb_comp(1);
  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext->valeurs().resize(1, dimension);
  return s;
}

void Sortie_libre_Gradient_Pression_libre_VEF::completer()
{
  Cerr << "Sortie_libre_Gradient_Pression_libre_VEF::completer()" << finl;
  const Domaine_Cl_dis_base& le_dom_Cl = domaine_Cl_dis();
  const Equation_base& eqn = le_dom_Cl.equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std, eqn);

  //  const Domaine_VEF& le_dom_VEF = ref_cast(Domaine_VEF,eqn.domaine_dis());

  const Champ_P0_VEF& pression = ref_cast(Champ_P0_VEF, eqn_hydr.pression());

  pression_interne = pression;

  const Front_VF& le_bord = ref_cast(Front_VF, frontiere_dis());
  int nb_faces_loc = le_bord.nb_faces();

  trace_pression_int.resize(nb_faces_loc);

  Cerr << "Sortie_libre_Gradient_Pression_libre_VEF::completer() ok" << finl;
}

int Sortie_libre_Gradient_Pression_libre_VEF::initialiser(double temps)
{

  int ok = Cond_lim_base::initialiser(temps);

  const Domaine_Cl_dis_base& le_dom_Cl = domaine_Cl_dis();
  const Equation_base& eqn = le_dom_Cl.equation();
  //      const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,eqn);
  const Domaine_VEF& mon_dom_VEF = ref_cast(Domaine_VEF, eqn.domaine_dis());
  const IntTab& face_voisins = mon_dom_VEF.face_voisins();
  //      const Domaine_Cl_VEF& domaine_Cl_VEF = ref_cast(Domaine_Cl_VEF,mon_dom_cl_dis.valeur());
  //      const DoubleVect& volumes_entrelaces_Cl = domaine_Cl_VEF.volumes_entrelaces_Cl();
  //      const DoubleTab& face_normale = le_dom_VEF.face_normales();
  const Front_VF& le_bord = ref_cast(Front_VF, frontiere_dis());
  int nb_faces_loc = le_bord.nb_faces();
  int ndeb = le_bord.num_premiere_face();
  int nfin = ndeb + nb_faces_loc;
  int face;
  coeff.resize(nb_faces_loc);
  if (dimension == 2)
    for (face = ndeb; face < nfin; face++)
      {
        int elem = face_voisins(face, 0);
        if (elem != -1)
          coeff[face - ndeb] = distance_2D(face, elem, mon_dom_VEF);
        else
          {
            elem = face_voisins(face, 1);
            coeff[face - ndeb] = -distance_2D(face, elem, mon_dom_VEF);
          }
      }
  if (dimension == 3)
    for (face = ndeb; face < nfin; face++)
      {
        int elem = face_voisins(face, 0);
        if (elem != -1)
          coeff[face - ndeb] = distance_3D(face, elem, mon_dom_VEF);
        else
          {
            elem = face_voisins(face, 1);
            coeff[face - ndeb] = -distance_3D(face, elem, mon_dom_VEF);
          }
      }
  return ok;
}

void Sortie_libre_Gradient_Pression_libre_VEF::mettre_a_jour(double temps)
{
  Cond_lim_base::mettre_a_jour(temps);

  const Front_VF& le_bord = ref_cast(Front_VF, frontiere_dis());
  int ndeb = le_bord.num_premiere_face();
  int nb_faces_loc = le_bord.nb_faces();
  int nfin = ndeb + nb_faces_loc;

  assert(pression_interne.non_nul());
  for (int face = ndeb; face < nfin; face++)
    {
      trace_pression_int[face - ndeb] = pression_interne->valeur_au_bord(face);
    }

}

double Sortie_libre_Gradient_Pression_libre_VEF::flux_impose(int face) const
{
  const Domaine_Cl_dis_base& le_dom_Cl = domaine_Cl_dis();
  const Equation_base& eqn = le_dom_Cl.equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std, eqn);
  const Domaine_VEF& mon_dom_VEF = ref_cast(Domaine_VEF, eqn.domaine_dis());
  const IntTab& face_voisins = mon_dom_VEF.face_voisins();
  //  const Domaine_Cl_VEF& domaine_Cl_VEF = ref_cast(Domaine_Cl_VEF,mon_dom_cl_dis.valeur());
  //  const Front_VF& le_bord = ref_cast(Front_VF,frontiere_dis());
  const IntTab& elem_faces = mon_dom_VEF.elem_faces();
  const DoubleTab& face_normales = mon_dom_VEF.face_normales();
  const Champ_P0_VEF& pre = ref_cast(Champ_P0_VEF, eqn_hydr.pression());

  double Pimp, diff, grad;

  int elem1, elem2, face_adj, face_face_adj;

  double a1 = trace_pression_int[face];
  double a2 = coeff[face];

  double a3 = 0.;

  elem1 = face_voisins(face, 0);

  for (face_adj = 0; face_adj < dimension; face_adj++)
    {
      face_face_adj = elem_faces(elem1, face_adj);

      elem2 = face_voisins(face_face_adj, 0);
      if (elem2 != elem1)
        {
          diff = pre.valeurs()(elem2) - pre.valeurs()(elem1);

          for (int comp = 0; comp < dimension; comp++)
            {
              grad = diff * face_normales(face_face_adj, comp);
              a3 += grad;
            }
        }
    }

  a3 /= double(dimension);
  Pimp = a1 + a2 * a3;
  return Pimp;

}

double Sortie_libre_Gradient_Pression_libre_VEF::flux_impose(int face, int ncomp) const
{
  if (ncomp == 0) return flux_impose(face);

  Cerr << "Sortie_libre_Gradient_Pression_libre_VEF::flux_impose(int  , int )" << finl;
  Cerr << "On ne sait imposer que la composante normale du gradient" << finl;
  Process::exit();
  return 0.;
}

double Sortie_libre_Gradient_Pression_libre_VEF::Grad_P_lib_VEF(int face) const
{
  const Milieu_base& mil = mon_dom_cl_dis->equation().milieu();
  const Champ_Uniforme& rho = ref_cast(Champ_Uniforme, mil.masse_volumique().valeur());
  double d_rho = rho.valeurs()(0, 0);
  if (le_champ_front->valeurs().size() == 1)
    return le_champ_front->valeurs()(0, 0) / d_rho;
  else if (le_champ_front->valeurs().line_size() == 1)
    return le_champ_front->valeurs()(face, 0) / d_rho;
  else
    Cerr << "Sortie_libre_Gradient_Pression_libre_VEF::Grad_P_lib_VEF() erreur" << finl;
  exit();
  return 0.;
}
