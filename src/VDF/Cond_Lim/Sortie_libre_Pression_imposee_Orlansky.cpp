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

#include <Sortie_libre_Pression_imposee_Orlansky.h>
#include <Domaine_Cl_dis_base.h>
#include <Navier_Stokes_std.h>
#include <Champ_Face_VDF.h>
#include <Champ_P0_VDF.h>
#include <Domaine_VDF.h>
#include <Debog.h>

Implemente_instanciable(Sortie_libre_Pression_imposee_Orlansky, "Frontiere_ouverte_Pression_imposee_Orlansky", Neumann_sortie_libre);
// XD frontiere_ouverte_pression_imposee_orlansky neumann frontiere_ouverte_pression_imposee_orlansky -1 This boundary condition may only be used with VDF discretization. There is no reference for pressure for this boundary condition so it is better to add pressure condition (with Frontiere_ouverte_pression_imposee) on one or two cells (for symetry in a channel) of the boundary where Orlansky conditions are imposed.


Sortie& Sortie_libre_Pression_imposee_Orlansky::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Sortie_libre_Pression_imposee_Orlansky::readOn(Entree& s)
{
  if (app_domains.size() == 0) app_domains = { Motcle("Hydraulique"), Motcle("indetermine") };
  if (supp_discs.size() == 0) supp_discs = { Nom("VDF") };

  le_champ_ext.typer("Champ_front_uniforme");
  le_champ_ext->valeurs().resize(1, dimension);
  le_champ_front.typer("Champ_front_uniforme");
  le_champ_front->valeurs().resize(1, dimension);
  le_champ_front->fixer_nb_comp(1);
  return s;
}

void Sortie_libre_Pression_imposee_Orlansky::completer()
{
  Cerr << "Sortie_libre_Pression_imposee_Orlansky::completer()" << finl;
  const Domaine_Cl_dis_base& le_dom_Cl = domaine_Cl_dis();
  const Equation_base& eqn = le_dom_Cl.equation();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std, eqn);
  const Domaine_VDF& domaine_vdf = ref_cast(Domaine_VDF, eqn.domaine_dis());
  const Champ_P0_VDF& pression = ref_cast(Champ_P0_VDF, eqn_hydr.pression());
  const Champ_Face_VDF& vitesse = ref_cast(Champ_Face_VDF, eqn_hydr.inconnue());
  //  const IntTab& face_voisins  = domaine_vdf.face_voisins();
  //  const DoubleVect& volumes_entrelaces = domaine_vdf.volumes_entrelaces();
  //  const DoubleVect& face_surfaces = domaine_vdf.face_surfaces();

  le_dom_VDF = domaine_vdf;
  pression_interne = pression;
  vitesse_interne = vitesse;

  const Front_VF& le_bord = ref_cast(Front_VF, frontiere_dis());
  int nb_faces_loc = le_bord.nb_faces();
  //  int ndeb = le_bord.num_premiere_face();

  le_champ_front->valeurs().resize(nb_faces_loc);
  le_champ_ext->valeurs().resize(nb_faces_loc, dimension);
  VPhiP.resize(nb_faces_loc);
  VPhiV.resize(nb_faces_loc, dimension);
  pression_temps_moins_un.resize(nb_faces_loc);
  pression_temps_moins_deux.resize(nb_faces_loc);
  pression_moins_un_temps_moins_un.resize(nb_faces_loc);
  pression_moins_un_temps_moins_deux.resize(nb_faces_loc);
  pression_moins_deux_temps_moins_un.resize(nb_faces_loc);
  pression_moins_un.resize(nb_faces_loc);
  pression_moins_deux.resize(nb_faces_loc);

  vitesse_temps_moins_un.resize(nb_faces_loc, dimension);
  vitesse_temps_moins_deux.resize(nb_faces_loc, dimension);
  vitesse_moins_un_temps_moins_un.resize(nb_faces_loc, dimension);
  vitesse_moins_un_temps_moins_deux.resize(nb_faces_loc, dimension);
  vitesse_moins_deux_temps_moins_un.resize(nb_faces_loc, dimension);
  vitesse_moins_un.resize(nb_faces_loc, dimension);
  vitesse_moins_deux.resize(nb_faces_loc, dimension);

  Cerr << "Sortie_libre_Pression_imposee_Orlansky::completer() ok" << finl;
}

void Sortie_libre_Pression_imposee_Orlansky::mettre_a_jour(double temps)
{
  //Cerr << "Sortie_libre_Pression_imposee_Orlansky::mettre_a_jour("
  //        <<temps<<")"<<finl;

  Cond_lim_base::mettre_a_jour(temps);

  assert(pression_interne.non_nul());
  const Front_VF& le_bord = ref_cast(Front_VF, frontiere_dis());
  int ndeb = le_bord.num_premiere_face();
  int nb_faces_loc = le_bord.nb_faces();

  DoubleTab& pre_bord = le_champ_front->valeurs();
  DoubleTab& vit_ext = le_champ_ext->valeurs();
  const Domaine_VDF& zvdf = ref_cast(Domaine_VDF, pression_interne->domaine_dis_base());
  const DoubleTab& pre = pression_interne->valeurs();
  const DoubleTab& vitesse = vitesse_interne->valeurs();

  int face, compo;

  for (face = ndeb; face < ndeb + nb_faces_loc; face++)
    {
      int i = face - ndeb;

      int ori = zvdf.orientation(face);

      pression_temps_moins_deux(i) = pression_temps_moins_un(i);
      pression_temps_moins_un(i) = pre_bord(i);
      pression_moins_un_temps_moins_deux(i) = pression_moins_un_temps_moins_un(i);
      pression_moins_un_temps_moins_un(i) = pression_moins_un(i);
      pression_moins_deux_temps_moins_un(i) = pression_moins_deux(i);

      int elem_un = zvdf.face_voisins(face, 0);
      if (elem_un < 0)
        elem_un = zvdf.face_voisins(face, 1);
      int face_moins_un = zvdf.elem_faces(elem_un, ori);
      if (face_moins_un == face)
        face_moins_un = zvdf.elem_faces(elem_un, ori + dimension);
      pression_moins_un[i] = pre[elem_un];

      int elem_deux = zvdf.face_voisins(face_moins_un, 0);
      if (elem_deux == elem_un)
        elem_deux = zvdf.face_voisins(face_moins_un, 1);
      pression_moins_deux[i] = pre[elem_deux];

      double pre_m_un_t_m_deux = pression_moins_un_temps_moins_deux(i);
      double pre_m_deux_t_m_un = pression_moins_deux_temps_moins_un(i);
      double pre_m_un = pression_moins_un(i);

      if (pre_m_un_t_m_deux == pre_m_un)
        VPhiP(i) = 0;
      else
        VPhiP(i) = (pre_m_un_t_m_deux - pre_m_un) / (pre_m_un + pre_m_un_t_m_deux - 2 * pre_m_deux_t_m_un);
      if (VPhiP(i) <= 1.e-24)
        VPhiP(i) = 0.0;
      if (VPhiP(i) > 1.)
        VPhiP(i) = 1.0;
      assert(VPhiP(i) < 1.e12);

      pre_bord[i] = (1 - VPhiP(i)) / (1 + VPhiP(i)) * pression_temps_moins_un(i) + (2 * VPhiP(i) / (1 + VPhiP(i))) * pression_moins_un(i);

    }
  Debog::verifier_bord("Orlansky::mettre_a_jour() : pre bord  : ", pre_bord, ndeb);
  //Debog::verifier_bord("Orlansky::mettre_a_jour() : VPhiP  : " , VPhiP, ndeb );
  //Debog::verifier_bord("Orlansky::mettre_a_jour() : VPhiV  avant : " , VPhiV ,ndeb);

  //Debog::verifier_bord("Orlansky::mettre_a_jour() : vitesse_moins_un_temps_moins_un : " , vitesse_moins_un_temps_moins_un, ndeb);
  //Debog::verifier_bord("Orlansky::mettre_a_jour() : vitesse_moins_deux : " , vitesse_moins_deux, ndeb);
  //Debog::verifier_bord("Orlansky::mettre_a_jour() : vitesse_moins_un : " , vitesse_moins_un, ndeb);

  for (compo = 0; compo < dimension; compo++)
    for (face = ndeb; face < ndeb + nb_faces_loc; face++)
      {
        int i = face - ndeb;

        int ori = zvdf.orientation(face);

        vitesse_temps_moins_deux(i, compo) = vitesse_temps_moins_un(i, compo);
        vitesse_temps_moins_un(i, compo) = vit_ext(i, compo);
        vitesse_moins_un_temps_moins_deux(i, compo) = vitesse_moins_un_temps_moins_un(i, compo);

        vitesse_moins_un_temps_moins_un(i, compo) = vitesse_moins_un(i, compo);
        vitesse_moins_deux_temps_moins_un(i, compo) = vitesse_moins_deux(i, compo);

        int elem_un = zvdf.face_voisins(face, 0);
        if (elem_un < 0)
          elem_un = zvdf.face_voisins(face, 1);
        int face_moins_un = zvdf.elem_faces(elem_un, ori);
        if (face_moins_un == face)
          face_moins_un = zvdf.elem_faces(elem_un, ori + dimension);
        double vit = 0.5 * (vitesse(zvdf.elem_faces(elem_un, compo)) + vitesse(zvdf.elem_faces(elem_un, compo + dimension)));

        vitesse_moins_un(i, compo) = vit;

        int elem_deux = zvdf.face_voisins(face_moins_un, 0);
        if (elem_deux == elem_un)
          elem_deux = zvdf.face_voisins(face_moins_un, 1);
        vit = 0.5 * (vitesse(zvdf.elem_faces(elem_deux, compo)) + vitesse(zvdf.elem_faces(elem_deux, compo + dimension)));
        vitesse_moins_deux(i, compo) = vit;

        double pre_m_un_t_m_deux = vitesse_moins_un_temps_moins_deux(i, compo);
        double pre_m_deux_t_m_un = vitesse_moins_deux_temps_moins_un(i, compo);
        double pre_m_un = vitesse_moins_un(i, compo);

        if (pre_m_un_t_m_deux == pre_m_un)
          VPhiV(i, compo) = 0;
        else
          VPhiV(i, compo) = (pre_m_un_t_m_deux - pre_m_un) / (pre_m_un + pre_m_un_t_m_deux - 2 * pre_m_deux_t_m_un);
        if (VPhiV(i, compo) <= 1.e-24)
          VPhiV(i, compo) = 0.0;
        if (VPhiV(i, compo) > 1.)
          VPhiV(i, compo) = 1.0;
        assert(VPhiV(i, compo) < 1.e12);

        vit_ext(i, compo) = (1 - VPhiV(i, compo)) / (1 + VPhiV(i, compo)) * vitesse_temps_moins_un(i, compo) + (2 * VPhiV(i, compo) / (1 + VPhiV(i, compo))) * vitesse_moins_un(i, compo);

      }
  Debog::verifier_bord("Orlansky::mettre_a_jour() :  vit_ext : ", vit_ext, ndeb);
  Debog::verifier_bord("Orlansky::mettre_a_jour() : VPhiV  : ", VPhiV, ndeb);
}

double Sortie_libre_Pression_imposee_Orlansky::flux_impose(int face) const
{
  return le_champ_front->valeurs()(face);
}

double Sortie_libre_Pression_imposee_Orlansky::flux_impose(int face, int ncomp) const
{
  if (ncomp == 0) return flux_impose(face);

  Cerr << "Sortie_libre_Pression_imposee_Orlansky::flux_impose(int  , int ). La pression est un scalaire." << finl;
  Process::exit();
  return 0.;
}
