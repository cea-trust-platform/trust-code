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

#include <Op_Diff_P1NC_barprim.h>
#include <Schema_Temps_base.h>
#include <Domaine_Cl_dis.h>
#include <Champ_Uniforme.h>
#include <Domaine_dis.h>
#include <Champ_P1NC.h>
#include <Periodique.h>
#include <TRUSTLists.h>
#include <TRUST_Ref.h>
#include <Champ_Inc.h>
#include <TRUSTTrav.h>
#include <EFichier.h>
#include <Domaine.h>

Implemente_instanciable(Op_Diff_P1NC_barprim, "Op_Diff_VEFBARPRIM_P1NC", Operateur_Diff_base);

Sortie& Op_Diff_P1NC_barprim::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Diff_P1NC_barprim::readOn(Entree& s) { return s; }

void Op_Diff_P1NC_barprim::associer(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis, const Champ_Inc& ch_diffuse)
{
  le_dom_vef = ref_cast(Domaine_VEF, domaine_dis.valeur());
  la_zcl_vef = ref_cast(Domaine_Cl_VEF, domaine_cl_dis.valeur());
  if (sub_type(Champ_P1NC, ch_diffuse.valeur()))
    {
      const Champ_P1NC& inco = ref_cast(Champ_P1NC, ch_diffuse.valeur());
      REF(Champ_P1NC) inconnue;
      inconnue = inco;
    }
}

void Op_Diff_P1NC_barprim::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = ref_cast(Champ_Uniforme, diffu);
}

void Op_Diff_P1NC_barprim::completer()
{
  Operateur_base::completer();
}

const Champ_base& Op_Diff_P1NC_barprim::diffusivite() const
{
  return diffusivite_;
}

double Op_Diff_P1NC_barprim::calculer_dt_stab() const
{
  double dt_stab;
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  double alpha = 1.e-38;
  // pas mp_max ?
  if (diffusivite_pour_pas_de_temps().valeurs().size() > 0)
    alpha = local_max_vect(diffusivite_pour_pas_de_temps().valeurs());
  dt_stab = domaine_VEF.carre_pas_du_maillage() / (2. * dimension * (alpha + DMINFLOAT));
  return dt_stab;
}

void Op_Diff_P1NC_barprim::calculer_divergence(const DoubleTab& grad, const DoubleVect& nu, DoubleTab& resu) const
{
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const Domaine& domaine = domaine_VEF.domaine();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  int nfe = domaine.nb_faces_elem();
  int nb_elem_tot = domaine.nb_elem_tot();
  int elem, indice, face, comp, comp2;
  //ArrOfDouble sigma(dimension);
  double normale;
  for (elem = 0; elem < nb_elem_tot; elem++)
    {
      for (indice = 0; indice < nfe; indice++)
        {
          face = elem_faces(elem, indice);
          double signe = 1;
          if (elem != face_voisins(face, 0))
            signe = -1;
          for (comp2 = 0; comp2 < dimension; comp2++)
            {
              normale = signe * face_normales(face, comp2);
              for (comp = 0; comp < dimension; comp++)
                resu(face, comp) -= nu(elem) * grad(elem, comp, comp2) * normale;
            }
        }
    }
  {
    int nb_bords = le_dom_vef->nb_front_Cl();
    for (int n_bord = 0; n_bord < nb_bords; n_bord++)
      {
        const Cond_lim& la_cl = la_zcl_vef->les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
        //Cerr << "la_cl = " << la_cl.valeur() << finl;
        int num1 = le_bord.num_premiere_face();
        int num2 = num1 + le_bord.nb_faces();
        if ((la_cl->que_suis_je() == "Paroi_fixe"))
          {
            //Cerr << "Paroi_fixe" << finl;
            for (face = num1; face < num2; face++)
              {
                for (comp2 = 0; comp2 < dimension; comp2++)
                  {
                    normale = face_normales(face, comp2);
                    for (comp = 0; comp < dimension; comp++)
                      resu(face, comp) = 0.;
                  }
              }
          }
        else if (sub_type(Periodique, la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
            ArrOfInt fait(le_bord.nb_faces());
            for (face = num1; face < num2; face++)
              {
                if (!fait[face - num1])
                  {
                    int face_associee = num1 + la_cl_perio.face_associee(face - num1);
                    fait[face - num1] = (fait[face_associee - num1] = 1);
                    for (comp = 0; comp < dimension; comp++)
                      resu(face_associee, comp) = (resu(face, comp) += resu(face_associee, comp));
                  }
              }
          }
      }
  }
}

DoubleTab& Op_Diff_P1NC_barprim::ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  DoubleTab tmp(resu);
  calculer(inconnue, tmp);
  resu += tmp;
  return resu;
}

void calculer_gradientP1NC(const DoubleTab& vitesse, const Domaine_VEF& domaine_VEF, const Domaine_Cl_VEF& domaine_Cl_VEF, DoubleTab& gradient_elem);

DoubleTab& Op_Diff_P1NC_barprim::calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  int nb_elem = le_dom_vef->domaine().nb_elem_tot();
  DoubleVect nu;
  nu.resize(nb_elem);
  nu = (diffusivite().valeurs())(0, 0);

  resu = 0.;

  const Champ_P1NC& ch = ref_cast(Champ_P1NC, (equation().inconnue().valeur()));
  DoubleTab gradient_bar;
  DoubleTab ubar(ch.valeurs());
  ch.filtrer_L2(ubar);
  calculer_gradientP1NC(ubar, le_dom_vef.valeur(), la_zcl_vef.valeur(), gradient_bar);
  DoubleTab resu_bar(resu);
  calculer_divergence(gradient_bar, nu, resu_bar);
  ch.filtrer_resu(resu_bar);
  resu += resu_bar;
  resu.echange_espace_virtuel();
  modifier_flux(*this);
  return resu;
}

