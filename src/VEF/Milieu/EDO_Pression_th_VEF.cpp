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

#include <Loi_Etat_Multi_GP_WC.h>
#include <Loi_Etat_Multi_GP_QC.h>
#include <EDO_Pression_th_VEF.h>
#include <Domaine_VEF.h>
#include <Periodique.h>
#include <Champ_P1NC.h>

Implemente_base(EDO_Pression_th_VEF, "EDO_Pression_th_VEF", EDO_Pression_th_base);

Sortie& EDO_Pression_th_VEF::printOn(Sortie& os) const { return EDO_Pression_th_base::printOn(os); }

Entree& EDO_Pression_th_VEF::readOn(Entree& is) { return EDO_Pression_th_base::readOn(is); }

void EDO_Pression_th_VEF::completer()
{
  if (!ref_cast(Domaine_VEF,le_dom.valeur()).get_alphaE())
    {
      Cerr << "Le modele quasi compressible ne fonctionne pas encore avec cette discretisation." << finl;
      Cerr << "En VEF, la discretisation doit avoir le support P0. Donc utiliser P1Bulle ou P0P1." << finl;
      Process::exit();
    }

  EDO_Pression_th_base::completer();
}

void EDO_Pression_th_VEF::calculer_grad(const DoubleTab& inco, DoubleTab& grad)
{
  assert(0);
  const Domaine_VEF& dom = ref_cast(Domaine_VEF, le_dom.valeur());
  const IntTab& face_voisins = dom.face_voisins();
  const DoubleTab& face_normales = dom.face_normales();
  const DoubleVect& volumes_entrelaces = dom.volumes_entrelaces();
  const DoubleVect& volumes_entrelaces_Cl = ref_cast(Domaine_Cl_VEF,le_dom_Cl.valeur()).volumes_entrelaces_Cl();

  double diff;
  int face, comp1;

  // Gradient d'un champ scalaire localise aux centres des elements
  // On initialise grad a zero
  grad = 0;
  if (1 == 1)
    {
      int elem1, elem2;
      // On traite les faces des joints:
      for (face = 0; face < dom.nb_faces_joint(); face++)
        {
          elem1 = face_voisins(face, 0);
          if (elem1 == -1)
            elem1 = face_voisins(face, 1);
          diff = -inco[elem1];
          for (comp1 = 0; comp1 < dimension; comp1++)
            grad(face, comp1) = diff * face_normales(face, comp1);
        }

      // On traite les conditions limites
      // Seule la condition aux limites de type Periodicite modifie le gradient
      for (int n_bord = 0; n_bord < dom.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = le_dom_Cl->les_conditions_limites(n_bord);
          if (sub_type(Periodique, la_cl.valeur()))
            {
              //      const Periodique& la_cl_perio = (Periodique&) la_cl.valeur();
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              for (face = num1; face < num2; face++)
                {
                  elem1 = face_voisins(face, 0);
                  elem2 = face_voisins(face, 1);
                  diff = inco[elem2] - inco[elem1];
                  for (int comp2 = 0; comp2 < dimension; comp2++)
                    grad(face, comp2) = diff * face_normales(face, comp2);
                }
            }
        }

      // On traite les faces internes
      //    Cerr << "Faces internes." << finl;
      for (face = dom.premiere_face_int(); face < dom.nb_faces(); face++)
        {
          elem1 = face_voisins(face, 0);
          elem2 = face_voisins(face, 1);
          diff = inco[elem2] - inco[elem1];
          for (int comp = 0; comp < dimension; comp++)
            grad(face, comp) = diff * face_normales(face, comp);
        }
      // On divise par le volume!!!
      int premiere_fac_std = dom.premiere_face_std();
      for (face = 0; face < premiere_fac_std; face++)
        {
          if (volumes_entrelaces_Cl(face) != 0)
            for (int comp = 0; comp < dimension; comp++)
              grad(face, comp) /= volumes_entrelaces_Cl(face);
          else
            for (int comp = 0; comp < dimension; comp++)
              grad(face, comp) = 0.;
        }
      for (face = premiere_fac_std; face < dom.nb_faces(); face++)
        {
          for (int comp = 0; comp < dimension; comp++)
            grad(face, comp) /= volumes_entrelaces(face);
        }
    }
  else
    {
      if (Objet_U::dimension == 2)
        {
          int elem, nb_som = dom.domaine().nb_som();
          int som, nsom, nse = dom.domaine().nb_som_elem();
          const IntTab& som_elem = dom.domaine().les_elems();
          int s0, s1, elem0, elem1;
          const IntTab& face_sommets = dom.face_sommets();
          const DoubleTab& coord_sommets = dom.domaine().coord_sommets();
          const DoubleTab& xp = dom.xp();
          //    const DoubleTab& xv = dom.xv();
          DoubleTab incosom(nb_som);
          DoubleTab volsom(nb_som);
          incosom = 0;
          volsom = 0;
          double v, inc;
          //calcul de P aux sommets
          for (elem = 0; elem < dom.nb_elem(); elem++)
            {
              v = dom.volumes(elem);
              inc = inco[elem];
              for (som = 0; som < nse; som++)
                {
                  nsom = som_elem(elem, som);
                  incosom(nsom) += v * inc;
                  volsom(nsom) += v;
                }
            }
          for (nsom = 0; nsom < nb_som; nsom++)
            {
              incosom(nsom) /= volsom(nsom);
            }
          double xs0, ys0, xs1, ys1, xe0, ye0, xe1, ye1, ds, de;
          //faces de bord
          for (int n_bord = 0; n_bord < dom.nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl = le_dom_Cl->les_conditions_limites(n_bord);

              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              for (face = num1; face < num2; face++)
                {
                  s0 = face_sommets(face, 0);
                  s1 = face_sommets(face, 1);
                  xs0 = coord_sommets(s0, 0);
                  ys0 = coord_sommets(s0, 1);
                  xs1 = coord_sommets(s1, 0);
                  ys1 = coord_sommets(s1, 1);
                  v = volumes_entrelaces(face);
                  ds = (incosom(s1) - incosom(s0)) / ((xs1 - xs0) * (xs1 - xs0) + (ys1 - ys0) * (ys1 - ys0));
                  grad(face, 0) = v * (ds * (xs1 - xs0));
                  grad(face, 1) = v * (ds * (ys1 - ys0));
                }
            }

          //faces internes
          for (face = dom.premiere_face_int(); face < dom.nb_faces(); face++)
            {
              s0 = face_sommets(face, 0);
              s1 = face_sommets(face, 1);
              elem0 = face_voisins(face, 0);
              elem1 = face_voisins(face, 1);
              xs0 = coord_sommets(s0, 0);
              ys0 = coord_sommets(s0, 1);
              xs1 = coord_sommets(s1, 0);
              ys1 = coord_sommets(s1, 1);
              xe0 = xp(elem0, 0);
              ye0 = xp(elem0, 1);
              xe1 = xp(elem1, 0);
              ye1 = xp(elem1, 1);
              v = volumes_entrelaces(face);
              ds = (incosom(s1) - incosom(s0)) / ((xs1 - xs0) * (xs1 - xs0) + (ys1 - ys0) * (ys1 - ys0));
              de = (inco(elem1) - inco(elem0)) / ((xe1 - xe0) * (xe1 - xe0) + (ye1 - ye0) * (ye1 - ye0));
              grad(face, 0) = v * (ds * (xs1 - xs0) + de * (xe1 - xe0));
              grad(face, 1) = v * (ds * (ys1 - ys0) + de * (ye1 - ye0));
            }
        }
      else
        Cerr << "ATTENTION : gradient de l'inco mal calcule en 3D" << finl;
    }
}

double EDO_Pression_th_VEF::masse_totale(double P, const DoubleTab& T)
{
  const Domaine_VEF& dom = ref_cast(Domaine_VEF, le_dom.valeur());
  int nb_faces = dom.nb_faces();

  // assert(tab.dimension(0)==nb_faces);
  const Loi_Etat_base& loi_ = ref_cast(Loi_Etat_base, le_fluide_->loi_etat().valeur());

  DoubleVect tmp;
  tmp.copy(T, RESIZE_OPTIONS::NOCOPY_NOINIT); // just copy the structure
  if (!sub_type(Loi_Etat_Multi_GP_QC, loi_))
    {
      for (int i = 0; i < nb_faces; i++)
        tmp[i] = loi_.calculer_masse_volumique(P, T[i]);
    }
  else
    {
      const Loi_Etat_Multi_GP_QC& loi_mel_GP = ref_cast(Loi_Etat_Multi_GP_QC, loi_);
      const DoubleTab& Masse_mol_mel = loi_mel_GP.masse_molaire();
      for (int i = 0; i < nb_faces; i++)
        {
          double r = 8.3143 / Masse_mol_mel[i];
          tmp[i] = loi_mel_GP.calculer_masse_volumique(P, T[i], r);
        }
    }
  const double M = Champ_P1NC::calculer_integrale_volumique(dom, tmp, FAUX_EN_PERIO);
  return M;
}

double EDO_Pression_th_VEF::masse_totale(const DoubleTab& P, const DoubleTab& T)
{
  const Domaine_VEF& dom = ref_cast(Domaine_VEF, le_dom.valeur());
  int nb_faces = dom.nb_faces();

  assert(P.dimension(0) == T.dimension(0));
  const Loi_Etat_base& loi_ = ref_cast(Loi_Etat_base, le_fluide_->loi_etat().valeur());

  DoubleVect tmp;
  tmp.copy(T, RESIZE_OPTIONS::NOCOPY_NOINIT); // just copy the structure
  if (!sub_type(Loi_Etat_Multi_GP_WC, loi_))
    {
      for (int i = 0; i < nb_faces; i++)
        tmp[i] = loi_.calculer_masse_volumique(P(i), T(i));
    }
  else
    {
      const Loi_Etat_Multi_GP_WC& loi_mel_GP = ref_cast(Loi_Etat_Multi_GP_WC, loi_);
      const DoubleTab& Masse_mol_mel = loi_mel_GP.masse_molaire();
      for (int i = 0; i < nb_faces; i++)
        {
          double r = 8.3143 / Masse_mol_mel[i];
          tmp[i] = loi_mel_GP.calculer_masse_volumique(P(i), T(i), r);
        }
    }
  const double M = Champ_P1NC::calculer_integrale_volumique(dom, tmp, FAUX_EN_PERIO);
  return M;
}
