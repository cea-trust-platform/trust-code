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

#include <Champ_P1_isoP1Bulle.h>
#include <Op_Div_VEFP1B_Elem.h>
#include <Domaine_Cl_dis.h>
#include <Domaine_Cl_VEF.h>
#include <Op_Rot_VEFP1B.h>
#include <Matrice_Morse.h>
#include <Domaine_dis.h>
#include <TRUSTList.h>
#include <Front_VF.h>
#include <Domaine.h>

Implemente_instanciable(Op_Rot_VEFP1B, "Op_Rot_VEFPreP1B_P1NC", Operateur_base);

Sortie& Op_Rot_VEFP1B::printOn(Sortie& s) const { return s << que_suis_je(); }

Entree& Op_Rot_VEFP1B::readOn(Entree& s) { return s; }

void Op_Rot_VEFP1B::associer_coins(const ArrOfInt& arr) { coins = arr; }

int est_un_coin(int num, const ArrOfInt& coins)
{
  for (int i = 0; i < coins.size_array(); i++)
    if (num == coins[i])
      return 1;
  return 0;
}

void Op_Rot_VEFP1B::associer(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_Cl_dis, const Champ_Inc&)
{
  le_dom_vef = ref_cast(Domaine_VEF, domaine_dis.valeur());
  la_zcl_vef = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis.valeur());
}

DoubleTab& Op_Rot_VEFP1B::calculer(const DoubleTab& vorticite, DoubleTab& rot) const
{
  rot = 0;
  return ajouter(vorticite, rot);

}

//Methode de calcul du rotationnel de la vorticite.
DoubleTab& Op_Rot_VEFP1B::ajouter(const DoubleTab& vorticite, DoubleTab& rot) const
{
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const Domaine& domaine = domaine_VEF.domaine();
  const IntTab& face_voisins = domaine_VEF.face_voisins();

  int elem0, elem1;
  int sommet_global, face_opp;

  //Quelques variables bien utiles
  DoubleTab vecteur_normal_elem0(dimension);
  DoubleTab vecteur_normal_elem1(dimension);
  DoubleTab vecteur_normal_opp_elem0(dimension);
  DoubleTab vecteur_normal_opp_elem1(dimension);

  IntList sommets_elem0, sommets_elem1;
  IntList faces_opp_elem0, faces_opp_elem1;

  // On traite les conditions limites:
  // pour l'instant pas de conditions aux limites pour la vorticite.
  // Sortie libre = pas de conditions aux limites ????

  // On traite les faces internes i.e. sans les conditions aux limites
  int premiere_face_int = domaine_VEF.premiere_face_int();
  int nb_faces = domaine_VEF.nb_faces();

  if (dimension > 2)
    {
      Cerr << "Erreur OpRot: seul le cas 2D est etudie pour le moment" << finl;
      Process::exit();
    }

  for (int face = premiere_face_int; face < nb_faces; face++)
    {
      int modulo;
      elem0 = face_voisins(face, 0);
      elem1 = face_voisins(face, 1);

      // Calcul du rotationnel de la vorticite: partie P0 de l'inconnue

      /* Calcul du vecteur tangent adequat pour la partie P0 de l'inconnue */
      vecteur_normal_elem0 = vecteur_normal(face, elem0);
      vecteur_normal_elem1 = vecteur_normal(face, elem1);

      /* Calcul du rotationnel de la vorticite proprement dit */
      /* Partie P0 */
      for (int composante = 0; composante < dimension; composante++)
        {
          modulo = (1 + composante) % dimension;
          rot(face, composante) += pow(-1., modulo) * (vorticite(elem0) * vecteur_normal_elem0(modulo) + vorticite(elem1) * vecteur_normal_elem1(modulo));

        }

      // Calcul du rotationnel de la vorticite: partie P1 de l'inconnue

      /* On a besoin pour chaque triangle de connaitre la liste */
      /* des sommets qui en font parti ainsi que les faces */
      /* opposees a ces sommets */
      if (!sommets_elem0.est_vide())
        sommets_elem0.vide();
      if (!sommets_elem1.est_vide())
        sommets_elem1.vide();
      if (!faces_opp_elem0.est_vide())
        faces_opp_elem0.vide();
      if (!faces_opp_elem1.est_vide())
        faces_opp_elem1.vide();

      for (int i = 0; i < domaine.nb_som_elem(); i++)
        {
          sommets_elem0.add_if_not(domaine.sommet_elem(elem0, i));
          faces_opp_elem0.add_if_not(domaine_VEF.elem_faces(elem0, i));
          sommets_elem1.add_if_not(domaine.sommet_elem(elem1, i));
          faces_opp_elem1.add_if_not(domaine_VEF.elem_faces(elem1, i));
        }

      /* Maintenant, on calcule la contribution au rotationnel */
      /* pour la partie P1 de l'inconnue */
      /* ATTENTION: compte tenu de la definition de l'espace */
      /* vectoriel engendrant omega, il NE FAUT PAS tenir compte */
      /* du sommet de plus GRAND numero */
      for (int composante = 0; composante < dimension; composante++)
        {
          modulo = (1 + composante) % dimension;

          ///////////////////////////////
          /* Partie (w,curl Psi_c e_i) */
          //////////////////////////////
          double sommation_sommets = 0.;
          //On fait les calculs pour "elem0"
          for (int i = 0; i < sommets_elem0.size(); i++)
            {
              sommet_global = sommets_elem0[i];

              //Pour eliminer le dernier sommet
              if (sommet_global != domaine.nb_som() - 1)
                sommation_sommets += vorticite(domaine.nb_elem() + sommet_global);
            }

          rot(face, composante) += -pow(-1., modulo) / (dimension + 1) * vecteur_normal_elem0(modulo) * sommation_sommets;

          //On fait les calculs pour "elem1"
          sommation_sommets = 0.;
          for (int i = 0; i < sommets_elem1.size(); i++)
            {
              sommet_global = sommets_elem1[i];

              //Pour eliminer le dernier sommet
              if (sommet_global != domaine.nb_som() - 1)
                sommation_sommets += vorticite(domaine.nb_elem() + sommet_global);
            }

          rot(face, composante) += -pow(-1., modulo) / (dimension + 1) * vecteur_normal_elem1(modulo) * sommation_sommets;

          //////////////////////////////////////
          /* Fin de partie (w,curl Psi_c e_i) */
          /////////////////////////////////////
          ///////////////////////////////
          /* Partie (curl w,Psi_c e_i) */
          //////////////////////////////
          //On fait les calculs pour "elem0"
          sommation_sommets = 0.;
          for (int i = 0; i < sommets_elem0.size(); i++)
            {
              sommet_global = sommets_elem0[i];

              //Pour eliminer le dernier sommet
              if (sommet_global != domaine.nb_som() - 1)
                {
                  face_opp = faces_opp_elem0[i];
                  vecteur_normal_opp_elem0 = vecteur_normal(face_opp, elem0);

                  sommation_sommets += vorticite(domaine.nb_elem() + sommet_global) * vecteur_normal_opp_elem0(modulo);
                }

            }

          rot(face, composante) += pow(-1., modulo) / (dimension * (dimension + 1)) * sommation_sommets;

          //On fait les calculs pour "elem1"
          sommation_sommets = 0.;
          for (int i = 0; i < sommets_elem1.size(); i++)
            {
              sommet_global = sommets_elem1[i];

              //Pour eliminer le dernier sommet
              if (sommet_global != domaine.nb_som() - 1)
                {
                  face_opp = faces_opp_elem1[i];
                  vecteur_normal_opp_elem1 = vecteur_normal(face_opp, elem1);

                  sommation_sommets += vorticite(domaine.nb_elem() + sommet_global) * vecteur_normal_opp_elem1(modulo);
                }

            }

          // C_h^t
          rot(face, composante) += pow(-1., modulo) / (dimension * (dimension + 1)) * sommation_sommets;

          //////////////////////////////////////
          /* Fin de partie (curl w,Psi_c e_i) */
          /////////////////////////////////////
        } //fin du for sur "composante" pour la partie P1 de l'inconnue

    } //fin du for sur "face"

  return rot;

}

DoubleTab Op_Rot_VEFP1B::vecteur_normal(const int face, const int elem) const
{
  assert(dimension == 2);

  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  DoubleTab le_vecteur_normal(dimension);

  for (int composante = 0; composante < dimension; composante++)
    le_vecteur_normal(composante) = domaine_VEF.face_normales(face, composante) * domaine_VEF.oriente_normale(face, elem);

  return le_vecteur_normal;
}
