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

#include <Terme_Source_Canal_perio_VDF_Face.h>
#include <Fluide_Dilatable_base.h>
#include <Pb_Thermohydraulique.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet_homogene.h>
#include <Domaine_Cl_VDF.h>
#include <Periodique.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <Domaine_VDF.h>

Implemente_instanciable(Terme_Source_Canal_perio_VDF_Face, "Canal_perio_VDF_Face", Terme_Source_Canal_perio);
Implemente_instanciable(Terme_Source_Canal_perio_QC_VDF_Face, "Canal_perio_QC_VDF_Face", Terme_Source_Canal_perio_VDF_Face);

Sortie& Terme_Source_Canal_perio_QC_VDF_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Terme_Source_Canal_perio_QC_VDF_Face::readOn(Entree& s) { return Terme_Source_Canal_perio::readOn(s); }

Sortie& Terme_Source_Canal_perio_VDF_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Terme_Source_Canal_perio_VDF_Face::readOn(Entree& s) { return Terme_Source_Canal_perio::readOn(s); }

void Terme_Source_Canal_perio_VDF_Face::associer_domaines(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_VDF = ref_cast(Domaine_VDF, domaine_dis.valeur());
  le_dom_Cl_VDF = ref_cast(Domaine_Cl_VDF, domaine_Cl_dis.valeur());
}

void Terme_Source_Canal_perio_VDF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  const Domaine_VF& domaine_VF = le_dom_VDF.valeur();
  const Domaine_Cl_dis_base& domaine_Cl_dis = le_dom_Cl_VDF.valeur();
  const IntVect& orientation = le_dom_VDF->orientation();
  const DoubleVect& porosite_surf = equation().milieu().porosite_face();
  const DoubleVect& volumes_entrelaces = domaine_VF.volumes_entrelaces();
  int ncomp;
  ArrOfDouble s(source());

  // Boucle sur les conditions limites pour traiter les faces de bord
  int n_bord, ndeb, nfin;
  for (n_bord = 0; n_bord < domaine_VF.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = domaine_Cl_dis.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()) || sub_type(Periodique, la_cl.valeur()) || sub_type(Symetrie, la_cl.valeur()))
        {

          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              double vol = volumes_entrelaces(num_face) * porosite_surf(num_face);
              ncomp = orientation(num_face);
              secmem(num_face) += s[ncomp] * vol;
            }
        }
      else if ((sub_type(Dirichlet, la_cl.valeur())) || (sub_type(Dirichlet_homogene, la_cl.valeur())))
        {
          // do nothing
        }
    }

  // Boucle sur les faces internes
  ndeb = domaine_VF.premiere_face_int();
  int nb_faces = domaine_VF.nb_faces();
  for (int num_face = ndeb; num_face < nb_faces; num_face++)
    {
      double vol = volumes_entrelaces(num_face) * porosite_surf(num_face);
      ncomp = orientation(num_face);
      secmem(num_face) += s[ncomp] * vol;
    }

}

void Terme_Source_Canal_perio_VDF_Face::calculer_debit(double& debit_e) const
{
  const Domaine_VF& domaine_VF = le_dom_VDF.valeur();
  const Domaine_Cl_dis_base& domaine_Cl_dis = le_dom_Cl_VDF.valeur();
  const DoubleTab& vitesse = equation().inconnue()->valeurs();
  const DoubleVect& porosite_surf = equation().milieu().porosite_face();
  int ndeb, nfin, num_face;
  int nb_bords = domaine_VF.nb_front_Cl();
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_dis.les_conditions_limites(n_bord);

      if (sub_type(Periodique, la_cl.valeur()))
        {
          const Periodique& perio = ref_cast(Periodique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
          if (bord_periodique_ == le_bord.le_nom())
            {
              int axe = perio.direction_periodicite();
              assert(axe == direction_ecoulement_);
              debit_e = 0.;
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces() / 2;

              if (equation().probleme().is_dilatable() == 1)
                {
                  // Si l'on est en Quasi/Weakly Compressible, il faut conserver
                  // le debit massique et non pas le debit volumique.
                  // C'est pour cela que dans le cas QC/WC, on multiplie les vecteurs vitesse
                  // par la masse volumique discretisee aux faces pour que lorsqu'on integre sur la surface,
                  // on obtienne bien un debit massique et non pas un debit volumique.
                  const DoubleTab& tab_rho_face = ref_cast(Fluide_Dilatable_base,equation().milieu()).rho_discvit();

                  for (num_face = ndeb; num_face < nfin; num_face++)
                    {
                      double debit_face = porosite_surf[num_face] * vitesse[num_face] * std::fabs(domaine_VF.face_normales(num_face, axe));
                      debit_e += tab_rho_face[num_face] * debit_face;
                    }
                }
              else
                {
                  for (num_face = ndeb; num_face < nfin; num_face++)
                    {
                      double debit_face = porosite_surf[num_face] * vitesse[num_face] * std::fabs(domaine_VF.face_normales(num_face, axe));
                      debit_e += debit_face;
                    }
                }
            }
        }
    }
  debit_e = mp_sum(debit_e);
}

