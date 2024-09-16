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

#include <Terme_Source_Qdm_VDF_Face.h>
#include <Neumann_sortie_libre.h>
#include <Discretisation_base.h>
#include <Dirichlet_homogene.h>
#include <Champ_Uniforme.h>
#include <Equation_base.h>
#include <Pb_Multiphase.h>
#include <Domaine_Cl_VDF.h>
#include <Domaine_Cl_dis.h>
#include <Milieu_base.h>
#include <Periodique.h>
#include <Dirichlet.h>
#include <Symetrie.h>
#include <Domaine_VDF.h>

Implemente_instanciable(Terme_Source_Qdm_VDF_Face, "Source_Qdm_VDF_Face", Source_base);

Sortie& Terme_Source_Qdm_VDF_Face::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Terme_Source_Qdm_VDF_Face::readOn(Entree& s)
{
  s >> la_source;
  if (la_source->nb_comp() != equation().inconnue()->nb_comp())
    {
      Cerr << "Erreur a la lecture du terme source de type " << que_suis_je() << finl;
      Cerr << "le champ source doit avoir " << equation().inconnue()->nb_comp() << " composantes" << finl;
      exit();
    }
  return s;
}

void Terme_Source_Qdm_VDF_Face::associer_domaines(const Domaine_dis_base& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_VDF = ref_cast(Domaine_VDF, domaine_dis);
  le_dom_Cl_VDF = ref_cast(Domaine_Cl_VDF, domaine_Cl_dis.valeur());
}

void Terme_Source_Qdm_VDF_Face::ajouter_blocs(matrices_t matrices, DoubleTab& resu, const tabs_t& semi_impl) const
{
  const Domaine_VDF& domaine_VDF = le_dom_VDF.valeur();
  const Domaine_Cl_VDF& domaine_Cl_VDF = le_dom_Cl_VDF.valeur();
  const IntTab& face_voisins = domaine_VDF.face_voisins();
  const IntVect& orientation = domaine_VDF.orientation();
  const DoubleVect& porosite_surf = equation().milieu().porosite_face();
  const DoubleVect& volumes_entrelaces = domaine_VDF.volumes_entrelaces();

  // useful only if multiphase problem
  const DoubleTab* alp = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()).equation_masse().inconnue()->passe() : nullptr;
  const DoubleTab* rho = alp ? &equation().milieu().masse_volumique()->passe() : nullptr;

  const int cR = alp ? ((*rho).dimension_tot(0) == 1) : 0, nb_comp = equation().inconnue()->valeurs().line_size();

  double vol;
  int ndeb, nfin, ncomp, num_face, elem1, elem2;

  if (sub_type(Champ_Uniforme, la_source.valeur()))
    {
      const DoubleVect& s = la_source->valeurs();

      // Boucle sur les conditions limites pour traiter les faces de bord : pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source
      for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = domaine_Cl_VDF.les_conditions_limites(n_bord);

          if (sub_type(Periodique, la_cl.valeur()))
            {
              if (alp) Process::exit("Terme_Source_Qdm_VDF_Face : periodic CL not yet available for Pb_Multiphase !");

              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (int k = 0; k < nb_comp; k++)
                for (num_face = ndeb; num_face < nfin; num_face++)
                  {
                    vol = volumes_entrelaces(num_face);
                    ncomp = orientation(num_face);
                    resu(num_face, k) += s(nb_comp * ncomp + k) * vol;
                  }
            }
          else if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (int k = 0; k < nb_comp; k++)
                for (num_face = ndeb; num_face < nfin; num_face++)
                  {
                    vol = volumes_entrelaces(num_face) * porosite_surf(num_face);
                    ncomp = orientation(num_face);
                    double alpha_rho = 1.0;
                    if (alp)
                      {
                        elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);
                        const int e = ( elem1 > -1 ? elem1 : elem2);
                        double a = (*alp)(e, k), r = (*rho)(!cR * e, k);
                        alpha_rho = a * r;
                      }
                    resu(num_face, k) += s(nb_comp * ncomp + k) * vol * alpha_rho;
                  }

            }
          else if (sub_type(Symetrie, la_cl.valeur())) { /* Do nothing */}
          else if ((sub_type(Dirichlet, la_cl.valeur())) || (sub_type(Dirichlet_homogene, la_cl.valeur()))) { /* Do nothing */}
        }

      // Boucle sur les faces internes
      ndeb = domaine_VDF.premiere_face_int();
      for (int k = 0; k < nb_comp; k++)
        for (num_face = domaine_VDF.premiere_face_int(); num_face < domaine_VDF.nb_faces(); num_face++)
          {
            vol = volumes_entrelaces(num_face) * porosite_surf(num_face);
            ncomp = orientation(num_face);
            double alpha_rho = 1.0;
            if (alp)
              {
                elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);
                double a = 0.5 * ((*alp)(elem1, k) + (*alp)(elem2, k)), r = 0.5 * ((*rho)(!cR * elem1, k) + (*rho)(!cR * elem2, k));
                alpha_rho = a * r;
              }
            resu(num_face, k) += s(nb_comp * ncomp + k) * vol * alpha_rho;
          }
    }
  else // le champ source n'est plus uniforme
    {
      const DoubleTab *s_tmp = nullptr;
      DoubleTab eval;
      if (la_source->que_suis_je().contient("_som_"))
        {
          // Need to interpolate
          const int N = resu.dimension(1), D = dimension;
          eval.resize(resu.dimension(0), N * D);
          la_source->valeur_aux(domaine_VDF.xp(), eval);
          s_tmp = &eval;
        }
      else
        s_tmp = &(la_source->valeurs());
      const DoubleTab& s = *s_tmp;

      // Boucle sur les conditions limites pour traiter les faces de bord : pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source
      for (int n_bord = 0; n_bord < domaine_VDF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = domaine_Cl_VDF.les_conditions_limites(n_bord);

          if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (int k = 0; k < nb_comp; k++)
                for (num_face = ndeb; num_face < nfin; num_face++)
                  {
                    vol = volumes_entrelaces(num_face) * porosite_surf(num_face);
                    ncomp = orientation(num_face);
                    elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);
                    const int e = (elem1 > -1 ? elem1 : elem2);
                    double alpha_rho = 1.0;
                    if (alp)
                      {
                        double a = (*alp)(e, k), r = (*rho)(!cR * e, k);
                        alpha_rho = a * r;
                      }
                    resu(num_face, k) += s(e, nb_comp * ncomp + k) * vol * alpha_rho;
                  }
            }
          else if (sub_type(Symetrie, la_cl.valeur())) { /* Do nothing */}
          else if ((sub_type(Dirichlet, la_cl.valeur())) || (sub_type(Dirichlet_homogene, la_cl.valeur()))) { /* Do nothing */}
          else if (sub_type(Periodique, la_cl.valeur()))
            {
              if (alp) Process::exit("Terme_Source_Qdm_VDF_Face : periodic CL not yet available for Pb_Multiphase !");

              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();

              for (int k = 0; k < nb_comp; k++)
                for (num_face = ndeb; num_face < nfin; num_face++)
                  {
                    vol = volumes_entrelaces(num_face) * porosite_surf(num_face);
                    ncomp = orientation(num_face);
                    double s_face = 0.5 * (s(face_voisins(num_face, 0), nb_comp * ncomp + k) + s(face_voisins(num_face, 1), nb_comp * ncomp + k));
                    resu(num_face, k) += s_face * vol;
                  }
            }
        }

      // Boucle sur les faces internes
      ndeb = domaine_VDF.premiere_face_int();

      for (int k = 0; k < nb_comp; k++)
        for (num_face = domaine_VDF.premiere_face_int(); num_face < domaine_VDF.nb_faces(); num_face++)
          {
            vol = volumes_entrelaces(num_face) * porosite_surf(num_face);
            ncomp = orientation(num_face);
            elem1 = face_voisins(num_face, 0), elem2 = face_voisins(num_face, 1);
            double alpha_rho = 1.0;
            if (alp)
              {
                double a = 0.5 * ((*alp)(elem1, k) + (*alp)(elem2, k)), r = 0.5 * ((*rho)(!cR * elem1, k) + (*rho)(!cR * elem2, k));
                alpha_rho = a * r;
              }
            double s_face = 0.5 * (s(elem1, nb_comp * ncomp + k) + s(elem2, nb_comp * ncomp + k));
            resu(num_face,k ) += s_face * vol * alpha_rho;
          }
    }
}

void Terme_Source_Qdm_VDF_Face::mettre_a_jour(double temps)
{
  la_source->mettre_a_jour(temps);
}

int Terme_Source_Qdm_VDF_Face::initialiser(double temps)
{
  equation().discretisation().nommer_completer_champ_physique(equation().domaine_dis(), "source_qdm", "", la_source.valeur(), equation().probleme());
  la_source->initialiser(temps);
  return Source_base::initialiser(temps);
}
