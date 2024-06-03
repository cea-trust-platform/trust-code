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

#include <Sortie_libre_Gradient_Pression_libre_VEFPreP1B.h>
#include <Champ_P1_isoP1Bulle.h>
#include <Navier_Stokes_std.h>
#include <Champ_Uniforme.h>
#include <distances_VEF.h>
#include <Milieu_base.h>

Implemente_instanciable(Sortie_libre_Gradient_Pression_libre_VEFPreP1B, "Frontiere_ouverte_Gradient_Pression_libre_VEFPreP1B", Neumann_sortie_libre);

Sortie& Sortie_libre_Gradient_Pression_libre_VEFPreP1B::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Sortie_libre_Gradient_Pression_libre_VEFPreP1B::readOn(Entree& is)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Hydraulique"), Motcle("indetermine") };
  if (supp_discs.size() == 0) supp_discs = { Nom("VEFPreP1B") };

  le_champ_front.typer("Champ_front_uniforme");
  le_champ_front.valeurs().resize(1, dimension);
  le_champ_front->fixer_nb_comp(1);
  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext.valeurs().resize(1, dimension);
  return is;
}

void Sortie_libre_Gradient_Pression_libre_VEFPreP1B::completer()
{

  Cerr << "Sortie_libre_Gradient_Pression_libre_VEFPreP1B::completer()" << finl;
  const Domaine_Cl_dis_base& le_dom_Cl = domaine_Cl_dis();
  const Equation_base& eqn = le_dom_Cl.equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std, eqn);

  const Domaine_VEF& mon_dom_VEF = ref_cast(Domaine_VEF, eqn.domaine_dis().valeur());

  const Champ_P1_isoP1Bulle& pression = ref_cast(Champ_P1_isoP1Bulle, eqn_hydr.pression().valeur());

  const IntTab& face_voisins = mon_dom_VEF.face_voisins();

  pression_interne = pression;

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
        coeff[face - ndeb] = distance_2D(face, elem, mon_dom_VEF) * 3.;
      }
  if (dimension == 3)
    for (face = ndeb; face < nfin; face++)
      {
        int elem = face_voisins(face, 0);
        coeff[face - ndeb] = distance_3D(face, elem, mon_dom_VEF) * 4.;
      }

  pression_temps_moins_un.resize(nb_faces_loc);
  pression_temps_moins_deux.resize(nb_faces_loc);
  pression_moins_un_temps_moins_un.resize(nb_faces_loc);
  pression_moins_un_temps_moins_deux.resize(nb_faces_loc);
  pression_moins_deux_temps_moins_un.resize(nb_faces_loc);
  pression_moins_un.resize(nb_faces_loc, 1);
  pression_moins_deux.resize(nb_faces_loc, 1);

  Pimp.resize(nb_faces_loc);

  Cerr << "Sortie_libre_Gradient_Pression_libre_VEFPreP1B::completer() ok" << finl;
}

int Sortie_libre_Gradient_Pression_libre_VEFPreP1B::initialiser(double temps)
{

  int ok = Cond_lim_base::initialiser(temps);

  const Front_VF& le_bord = ref_cast(Front_VF, frontiere_dis());

  int nb_faces_loc = le_bord.nb_faces();
  int ndeb = le_bord.num_premiere_face();
  int nfin = ndeb + nb_faces_loc;

  int face, face_loc;

  for (face = ndeb; face < nfin; face++)
    {
      face_loc = face - ndeb;
      Pimp[face_loc] = pression_interne->valeur_au_bord(face);
      pression_moins_un(face_loc, 0) = Pimp[face_loc];
      pression_moins_deux(face_loc, 0) = Pimp[face_loc];
    }

  pression_moins_un_temps_moins_un = Pimp;
  pression_temps_moins_un = Pimp;
  pression_temps_moins_deux = Pimp;
  pression_moins_un_temps_moins_deux = Pimp;
  pression_moins_deux_temps_moins_un = Pimp;

  return ok;
}

void Sortie_libre_Gradient_Pression_libre_VEFPreP1B::mettre_a_jour(double temps)
{

  const Domaine_Cl_dis_base& le_dom_Cl = domaine_Cl_dis();
  const Equation_base& eqn = le_dom_Cl.equation();
  const Domaine_VEF& mon_dom_VEF = ref_cast(Domaine_VEF, eqn.domaine_dis().valeur());
  const DoubleTab& face_normale = mon_dom_VEF.face_normales();
  const Front_VF& le_bord = ref_cast(Front_VF, frontiere_dis());

  int nb_faces_loc = le_bord.nb_faces();
  int ndeb = le_bord.num_premiere_face();
  int nfin = ndeb + nb_faces_loc;

  int face, face_loc, j;

  const DoubleTab& xv = mon_dom_VEF.xv();

  DoubleTab geom;
  geom.resize(nb_faces_loc, dimension);

  Cond_lim_base::mettre_a_jour(temps);

  for (face = ndeb; face < nfin; face++)
    {
      face_loc = face - ndeb;

      pression_temps_moins_deux(face_loc) = pression_temps_moins_un(face_loc);
      pression_temps_moins_un(face_loc) = Pimp(face_loc);
      pression_moins_un_temps_moins_deux(face_loc) = pression_moins_un_temps_moins_un(face_loc);
      pression_moins_un_temps_moins_un(face_loc) = pression_moins_un(face_loc, 0);
      pression_moins_deux_temps_moins_un(face_loc) = pression_moins_deux(face_loc, 0);
    }

  double orient;
  for (face = ndeb; face < nfin; face++)
    {
      orient = 0.;
      for (j = 0; j < dimension; j++)
        {
          orient += (face_normale(face, j) * face_normale(face, j));
        }

      orient = sqrt(orient);

      for (j = 0; j < dimension; j++)
        {
          geom(face - ndeb, j) = xv(face, j);
          geom(face - ndeb, j) -= face_normale(face, j) / orient * coeff(face - ndeb);
          ;
        }

    }

  pression_interne->valeur_aux(geom, pression_moins_un);

  for (face = ndeb; face < nfin; face++)
    {
      orient = 0.;
      for (j = 0; j < dimension; j++)
        {
          orient += (face_normale(face, j) * face_normale(face, j));
        }

      orient = sqrt(orient);

      for (j = 0; j < dimension; j++)
        {
          geom(face - ndeb, j) = xv(face, j);
          geom(face - ndeb, j) -= face_normale(face, j) / orient * 2. * coeff(face - ndeb);
        }

    }

  pression_interne->valeur_aux(geom, pression_moins_deux);

  double VPhiP;

  for (face = ndeb; face < nfin; face++)
    {
      face_loc = face - ndeb;
      double pre_m_un_t_m_deux = pression_moins_un_temps_moins_deux(face_loc);
      double pre_m_deux_t_m_un = pression_moins_deux_temps_moins_un(face_loc);
      double pre_m_un = pression_moins_un(face_loc, 0);

      if (pre_m_un_t_m_deux == pre_m_un)
        VPhiP = 0.;
      else
        VPhiP = (pre_m_un_t_m_deux - pre_m_un) / (pre_m_un + pre_m_un_t_m_deux - 2 * pre_m_deux_t_m_un);

      if (VPhiP <= 1.e-24)
        VPhiP = 0.0;
      if (VPhiP > 1.)
        VPhiP = 1.0;
      assert(VPhiP < 1.e12);

      Pimp[face_loc] = (1 - VPhiP) / (1 + VPhiP) * pression_temps_moins_un(face_loc) + (2 * VPhiP) / (1 + VPhiP) * pression_moins_un(face_loc, 0);

    }

}

double Sortie_libre_Gradient_Pression_libre_VEFPreP1B::flux_impose(int face) const
{
  return Pimp[face];
}

double Sortie_libre_Gradient_Pression_libre_VEFPreP1B::flux_impose(int face, int ncomp) const
{
  if (ncomp == 0) return flux_impose(face);

  Cerr << "Sortie_libre_Gradient_Pression_libre_VEFPreP1B::flux_impose(int  , int )" << finl;
  Cerr << "On ne sait imposer que la composante normale du gradient" << finl;
  Process::exit();
  return 0.;
}

double Sortie_libre_Gradient_Pression_libre_VEFPreP1B::Grad_P_lib_VEFPreP1B(int face) const
{
  const Milieu_base& mil = mon_dom_cl_dis->equation().milieu();
  const Champ_Uniforme& rho = ref_cast(Champ_Uniforme, mil.masse_volumique().valeur());
  double d_rho = rho(0, 0);
  if (le_champ_front.valeurs().size() == 1)
    return le_champ_front(0, 0) / d_rho;
  else if (le_champ_front.valeurs().dimension(1) == 1)
    return le_champ_front(face, 0) / d_rho;
  else
    Cerr << "Sortie_libre_Gradient_Pression_libre_VEFPreP1B::Grad_P_lib_VEFPreP1B() erreur" << finl;
  exit();
  return 0.;
}
