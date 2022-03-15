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
// File:        Op_Diff_VEF_Face_Penalise.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////
#include <Op_Diff_VEF_Face_Penalise.h>
#include <Dirichlet_paroi_fixe.h>


Implemente_instanciable(Op_Diff_VEF_Face_Penalise,"Op_Diff_VEFpenalise_const_P1NC",Op_Diff_VEF_Face);

/* Implementation obligatoire de la fonction printOn() */
Sortie& Op_Diff_VEF_Face_Penalise::
printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

/* Implementation obligatoire de la fonction readOn() */
Entree& Op_Diff_VEF_Face_Penalise::readOn(Entree& is )
{
  /* REMARQUE IMPORTANTE: on a cree cette classe par derivation mais
   * mais elle doit uniquement s'appliquer sur des equations de
   * Navier-Stokes non turbulentes d'ou les tests suivants
   */

  Cerr<<"je suis dans Op_Diff_VEF_Face_Penalise::readOn()"<<finl;

  /* On teste la dimension du probleme car la theorie n'a ete validee
   * que sur du 2D
   */
  if (dimension != 2)
    {
      Cerr << "Erreur dans Op_Diff_VEF_Face_Penalise::readOn()" << finl;
      Cerr << "La dimension du probleme doit etre 2" << finl;
      Process::exit();
    }

  return is ;
}

/*
 *
 *
 *
 *
 *
 */

//Description:
// methode qui calcule la contribution de l'operateur.
DoubleTab& Op_Diff_VEF_Face_Penalise::
calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  resu = 0.;
  return ajouter(inconnue,resu);
}

/*
 *
 *
 *
 *
 *
 */

//Description:
//methode qui calcule la vitesse au temps n+1 lorsque le
//schema explicite est utilise.
DoubleTab& Op_Diff_VEF_Face_Penalise::
ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  //  Cerr << "J'entre dans ajouter() de penalisation" << finl;
  int nb_composante,numero_global_face,local;
  int face_penalisation,face;
  double coeff;
  IntList voisinage_ordre1,voisinage_ordre1_strict;
  IntList voisinage_ordre2,voisinage_ordre2_strict;
  IntList ensemble_faces;

  /* On calcule le resultat du a la matrice de penalisation
   * i.e. somme_j U_j P_i,j ou P_i,j est le terme de la matrice
   * de penalisation.
   */
  for (nb_composante = 0; nb_composante < dimension; nb_composante++)
    {
      for (numero_global_face = 0; numero_global_face < zone_vef().nb_faces();
           numero_global_face++)
        {
          /* Calcul des voisinages de numero_global_face */
          voisinage(numero_global_face,voisinage_ordre1);
          voisinage(voisinage_ordre1,voisinage_ordre2);
          reduction(voisinage_ordre1,voisinage_ordre2,voisinage_ordre2_strict);
          //           Cerr << "Nombre iterations " << numero_global_face << finl;
          //           Cerr << "Nombre de faces " << zone_vef().nb_faces() << finl;

          /* Calcul de resu(numero_face_global,nb_comp)
           * pour le voisinage_ordre2_strict
           */
          for (local = 0 ; local < voisinage_ordre2_strict.size(); local++)
            {
              face = voisinage_ordre2_strict[local];
              faces_communes(numero_global_face,face,ensemble_faces);
              //               Cerr << "Taille voisinage_ordre_1 " << voisinage_ordre1.size()
              //                    << finl;
              //               Cerr << "Taille voisinage_ordre2 " << voisinage_ordre2.size()
              //                    << finl;
              //               Cerr << "Taille voisinage_ordre2_strict " << voisinage_ordre2_strict.size() << finl;
              //               Cerr << "Taille ensemble_faces " << ensemble_faces.size() << finl;

              // if (ensemble_faces.size() != 1)
              //                 {
              //                   Cerr << "Erreur dans ajouter() de la penalisation." << finl;
              //                   Process::exit();
              //                 }

              for (int mm = 0; mm < ensemble_faces.size(); mm++)
                {
                  face_penalisation = ensemble_faces[mm];

                  resu(numero_global_face,nb_composante) +=
                    inconnue(face,nb_composante)*
                    signe(numero_global_face,face)*
                    coefficient_penalisation(face_penalisation)*
                    longueur(face_penalisation)*
                    1./3.;
                }

            }

          /* Calcul de resu(numero_global_face,nb_comp)
           * pour le voisinage_ordre1_strict
           */
          for (local = 0 ; local < voisinage_ordre1_strict.size(); local++)
            {
              assert(voisinage_ordre1_strict.size() == 2
                     || voisinage_ordre1_strict.size() == 4);

              face = voisinage_ordre1_strict[local];
              face_penalisation = autre_face(numero_global_face,face);

              resu(numero_global_face,nb_composante) +=
                inconnue(face,nb_composante)*
                coefficient_penalisation(face_penalisation)*
                longueur(face_penalisation)*
                (-1./3.);
            }

          /* Cas 1: la face consideree est au bord du domaine.
           * et numero_global_face = face
           */
          if (numero_global_face < zone_vef().nb_faces_bord())
            for (local = 0; local < voisinage_ordre1.size(); local++)
              {
                assert(voisinage_ordre1.size() == 3);

                face_penalisation = voisinage_ordre1[local];

                if (numero_global_face == face_penalisation)
                  coeff = 1.;
                else
                  coeff = 1./3.;

                resu(numero_global_face,nb_composante) +=
                  inconnue(numero_global_face,nb_composante) *
                  coeff *
                  coefficient_penalisation(face_penalisation)*
                  longueur(face_penalisation);
              }

          /* Cas 2: la face est interne et numero_global_face = face */
          else
            for (local = 0; local < voisinage_ordre1_strict.size(); local++)
              {
                assert( voisinage_ordre1_strict.size() == 4);

                face_penalisation = voisinage_ordre1_strict[local];

                resu(numero_global_face,nb_composante) +=
                  inconnue(numero_global_face,nb_composante) *
                  coefficient_penalisation(face_penalisation)*
                  longueur(face_penalisation)*
                  1./3.;
              }
        }
    }


  /* Ajout de la matrice de diffusion classique */
  Op_Diff_VEF_Face::ajouter(inconnue,resu);

  //  Cerr << "Je sors de calcul_matrice_de_penalisation_" << finl;
  return resu;
}

/*
 *
 *
 *
 *
 *
 */

//Description:
//methode qui calcule le voisinage d'une face.
void Op_Diff_VEF_Face_Penalise::
voisinage(const int& Numero_face, IntList& Voisinage) const
{
  //  Cerr << "J'entre dans voisinage pour un numero face" << finl;

  /* On vide la liste Voisinage pour eviter les mauvaises surprises */
  if (! Voisinage.est_vide() ) Voisinage.vide();

  /* Declaration des principaux parametres locaux. */
  int numero_local;

  /* Le nombre de faces des elements constituant la zone
   * de discretisation.
   * REM: on suppose que l'on ne travaille pas sur des prismes.
   */
  const int nb_faces_element = zone().nb_faces_elem();

  /* Les elements voisins de Numero_face */
  const int voisin1 = zone_vef().face_voisins(Numero_face,1);
  const int voisin2 = zone_vef().face_voisins(Numero_face,0);

  /* Ici, on recupere les faces de voisin*
   * que l'on injecte dans la liste Voisinage.
   * Il faut donc que l'element existe d'ou le premier test realise.
   */
  if (voisin1 != -1)
    {
      for (numero_local = 0; numero_local < nb_faces_element; numero_local++)
        {
          /* On recupere le numero global de chacune des
           * faces de voisin*
           */
          const int numero_global_face =
            zone_vef().elem_faces(voisin1,numero_local);

          /* Puis on place ce numero dans la liste Voisinage. */
          Voisinage.add_if_not(numero_global_face);

        }//fin du for

    }//fin du if

  if (voisin2 != -1)
    {
      for (numero_local = 0; numero_local < nb_faces_element; numero_local++)
        {
          /* On recupere le numero global de chacune des
           * faces de numero_element_*
           */
          const int numero_global_face =
            zone_vef().elem_faces(voisin2,numero_local);

          /* Puis on place ce numero dans la liste Voisinage. */
          Voisinage.add_if_not(numero_global_face);

        }// fin du for

    }//fin du if

  //  Cerr << "Je sors de voisinage pour un numero de face" << finl;
}

/*
 *
 *
 *
 *
 *
 */

//Description:
//methode qui calcule le voisinage d'une liste de faces.
void  Op_Diff_VEF_Face_Penalise::
voisinage(const IntList& Ensemble_faces, IntList& Voisinage) const
{
  //  Cerr << "J'entre dans voisinage pour un ensemble de face" << finl;

  /* On vide la liste Voisinage pour eviter les mauvaises surprises */
  if (! Voisinage.est_vide() ) Voisinage.vide();

  /* Declaration des principaux parametres locaux. */
  int nb_elements_Ensemble_faces,nb_elements_liste_temporaire;
  IntList liste_temporaire;

  for (nb_elements_Ensemble_faces = 0;
       nb_elements_Ensemble_faces < Ensemble_faces.size();
       nb_elements_Ensemble_faces++)
    {
      /* Parametre interne a la boucle */
      const int numero_face_dans_Ensemble_faces =
        Ensemble_faces[nb_elements_Ensemble_faces];

      /* On vide la liste temporaire a chaque fois que l'on change de face */
      if (! liste_temporaire.est_vide() ) liste_temporaire.vide();

      /* Dans la liste temporaire, on place le voisinage
       * de numero_face_dans_Ensemble_faces
       */
      voisinage(numero_face_dans_Ensemble_faces,liste_temporaire);
      //      Cerr << "Taille liste temporaire " << liste_temporaire.size() << finl;


      for (nb_elements_liste_temporaire = 0;
           nb_elements_liste_temporaire < liste_temporaire.size();
           nb_elements_liste_temporaire++)
        {
          //          Cerr << "les elements " << liste_temporaire[nb_elements_liste_temporaire] << finl;
          /* Parametre interne a la deuxieme boucle */
          const int numero_face_dans_liste_temporaire =
            liste_temporaire[nb_elements_liste_temporaire];

          /* Enfin, on stoke dans Voisinage */
          Voisinage.add_if_not(numero_face_dans_liste_temporaire);

        }//fin du deuxieme for

    }//fin du premier for

  // Cerr << "Je sors de voisinage pour un ensemble face" << finl;
}

/*
 *
 *
 *
 *
 *
 */


double  Op_Diff_VEF_Face_Penalise::
signe(const int& Face1, const int& Face2) const
{
  //  Cerr << "J'entre dans signe" << finl;

  /* Parametres locaux a la procedure */
  int numero_local;

  /* On recupere le nombre de sommets des faces
   * des elements constituant la zone discretisee.
   * REM: on exclut le prisme par convention.
   */
  const int nb_sommets_par_face = zone_vef().nb_som_face();

  /* On cree une liste qui contient les sommets de la Face2 */
  IntList sommets_Face2;

  for (numero_local = 0 ; numero_local < nb_sommets_par_face ; numero_local++)
    sommets_Face2.add(zone_vef().face_sommets(Face2,numero_local));

  /* Enfin, on regarde si des sommets de la Face2 appartiennent
   * a la Face1.
   * Si oui, alors on retourne 1 sinon on retourne -1
   */
  for (numero_local = 0; numero_local < nb_sommets_par_face ; numero_local++)
    if ( sommets_Face2.contient(zone_vef().face_sommets(Face1,numero_local)))
      return 1.;

  return -1.;

  //  Cerr << "Je sors de signe" << finl;
}

/*
 *
 *
 *
 *
 *
 */

//Description:
//fonction membre qui retourne le coefficient de
//penalisation associee a chaque face du maillage primaire.
double  Op_Diff_VEF_Face_Penalise::
coefficient_penalisation(const int& Numero_face) const
{
  //  Cerr << "J'entre dans coefficient_penalisation" << finl;
  /* Initialisation de parametres locaux */
  double eta=0.;
  double coefficientpenalisation = 0.;

  const int voisin1 =
    zone_vef().face_voisins(Numero_face,1);

  const int voisin2 =
    zone_vef().face_voisins(Numero_face,0);

  if (voisin1 == -1 && voisin2 == -1)
    {
      Cerr << "Erreur dans Op_Dift_standard_Face_VEF_penalise::"
           << "coefficient_penalisation()" << finl;
      Process::exit();
    }

  if (voisin1 != -1 && voisin2 != -1)
    {
      /* Calcul des coefficients de penalisation proprement dit */
      coefficientpenalisation = 1./diametre(voisin1);
      eta = 1./diametre(voisin2);

      coefficientpenalisation = std::min(coefficientpenalisation,eta);
    }

  if (voisin1 == -1)
    coefficientpenalisation = 1./diametre(voisin2);

  if (voisin2 == -1)
    coefficientpenalisation = 1./diametre(voisin1);

  return coefficientpenalisation;
  //  Cerr << "Je sors de coefficient_penalisation" << finl;
}

/*
 *
 *
 *
 *
 *
 */

//Description:
//fonction membre qui renvoie la liste des faces appartenant
//au voisinage de Face1 ET de Face2.
void  Op_Diff_VEF_Face_Penalise::
faces_communes(const int& Face1,const int& Face2,
               IntList& Faces_communes) const
{

  //  Cerr <<"J'entre dans faces_communes" << finl;
  /* On vide d'abord Faces_commnunes pour eviter les erreurs. */
  if (! Faces_communes.est_vide() ) Faces_communes.vide();

  /* Declaration des parametres locaux a la procedure */
  IntList voisinage_Face1,voisinage_Face2;
  int nb_element_voisinage_Face2;

  /* On calcule chacun des voisinages de Face1 et Face2 */
  voisinage(Face1,voisinage_Face1);
  voisinage(Face2,voisinage_Face2);

  /* Ensuite, on retrouve les faces communes a ces 2 voisinages. */
  for (nb_element_voisinage_Face2 = 0;
       nb_element_voisinage_Face2 < voisinage_Face2.size();
       nb_element_voisinage_Face2++)
    {
      const int numero_face_voisinage_Face2 =
        voisinage_Face2[nb_element_voisinage_Face2];

      if (voisinage_Face1.contient(numero_face_voisinage_Face2))
        Faces_communes.add_if_not(numero_face_voisinage_Face2);

    }//fin du for

  //  Cerr << "Je sors de faces_communes" << finl;
}

/*
 *
 *
 *
 *
 *
 */

void  Op_Diff_VEF_Face_Penalise::
reduction(const IntList& Liste1,const IntList& Liste2,
          IntList& Liste_reduite) const
{
  //  Cerr << "J'entre dans reduction" << finl;
  /* On nettoie Liste_reduite afin d'eviter les erreurs */
  if (! Liste_reduite.est_vide() ) Liste_reduite.vide();

  /* Declaration des parametres locaux a la procedure */
  const IntList *liste_de_plus_petite_taille,*liste_de_plus_grande_taille;
  int nb_element_dans_liste;

  /* On teste la taille des listes passees en parametres puis
   * on effectue l'allocation en fonction du resultat.
   */
  if (Liste1.size() >= Liste2.size())
    {
      liste_de_plus_petite_taille = &Liste2;
      liste_de_plus_grande_taille = &Liste1;
    }
  else
    {
      liste_de_plus_petite_taille = &Liste1;
      liste_de_plus_grande_taille = &Liste2;
    }

  /* Enfin, on supprime de la liste_de_plus_grande_taille,
   * les elements de la liste_de_plus_petite_taille si ceux-ci
   * sont bien dans la liste_de_plus_grande_taille.
   */
  for (int ll = 0 ; ll < (*liste_de_plus_grande_taille).size() ; ll++)
    Liste_reduite.add( (*liste_de_plus_grande_taille)[ll] );

  for (nb_element_dans_liste = 0;
       nb_element_dans_liste < (*liste_de_plus_petite_taille).size();
       nb_element_dans_liste++)
    if
    (Liste_reduite.contient( (*liste_de_plus_petite_taille)
                             [nb_element_dans_liste] ) )
      Liste_reduite.suppr( (*liste_de_plus_petite_taille)
                           [nb_element_dans_liste] );

  //  Cerr << "Je sors de reduction" << finl;
}


/*
 *
 *
 *
 *
 *
 */

//Description:
//fonction membre qui renvoie le numero de l'element contenant Face1 et Face2
//s'il existe et renvoie -1 sinon.
int Op_Diff_VEF_Face_Penalise::
element_commun(const int& Face1,const int& Face2) const
{
  //  Cerr << "J'entre dans element_commun" << finl;

  /* Les elements voisins de Face1 */
  const int voisin1_Face1 = zone_vef().face_voisins(Face1,1);
  const int voisin2_Face1 = zone_vef().face_voisins(Face1,0);

  /* Les elements voisins de Face2 */
  const int voisin1_Face2 = zone_vef().face_voisins(Face2,1);
  const int voisin2_Face2 = zone_vef().face_voisins(Face2,0);

  /* On cherche l'element commun */
  if (voisin1_Face1 != -1)
    if (voisin1_Face1 == voisin1_Face2 || voisin1_Face1 == voisin2_Face2)
      return voisin1_Face1;

  if (voisin2_Face1 != -1)
    if (voisin2_Face1 == voisin1_Face2 || voisin2_Face1 == voisin2_Face2)
      return voisin2_Face1;

  Cerr << " Op_Diff_VEF_Face_Penalise::element_commun()" << finl;
  Cerr << "Attention: la face " << Face1 << " et la face " << Face2
       << "n'ont pas d'element commun" << finl;
  Cerr << "Je sors de elements_communs" << finl;
  return -1;

}

/*
 *
 *
 *
 *
 *
 */

//Description:
//fonction membre qui retourne la 3eme face de l'element Element si
// Face1 et Face2 appartiennent au meme element.
// Sinon retourne -1.
int Op_Diff_VEF_Face_Penalise::
autre_face(const int& Face1, const int& Face2)
const
{
  //  Cerr << "J'entre dans autre_face" << finl;
  /* Declaration des variables locales */
  int numero_local,lautre_face=-1;
  int elem_commun = element_commun(Face1,Face2);

  /* Le nombre de faces des elements constituant la zone
   * de discretisation.
   * REM: on suppose que l'on ne travaille pas sur des prismes.
   */
  if (elem_commun == -1)
    {
      Cerr << "Fonction element_commun" << finl;
      Cerr << "Les 2 faces n'appartiennent pas au meme element." << finl;
      Process::exit();
      return lautre_face;
    }

  const int nb_faces_element = zone().nb_faces_elem();

  for (numero_local = 0; numero_local < nb_faces_element; numero_local++)

    {
      /* On recupere le numero global de chacune des
       * faces de Element
       */
      const int numero_global_face =
        zone_vef().elem_faces(elem_commun,numero_local);

      if ( numero_global_face != Face1 && numero_global_face != Face2)
        {
          lautre_face = numero_global_face;
          break;
        }

    }//fin du for

  //  Cerr << "Je sors de autre_face" << finl;
  return lautre_face;

}
/*
 *
 *
 *
 *
 *
 */


