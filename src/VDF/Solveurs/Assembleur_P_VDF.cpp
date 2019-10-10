/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Assembleur_P_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Solveurs
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Assembleur_P_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Zone_VDF.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Neumann_sortie_libre.h>
#include <Entree_fluide_vitesse_imposee.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Champ_front_instationnaire_base.h>
#include <Champ_front_var_instationnaire.h>
#include <Matrice_Bloc.h>
#include <Debog.h>
#include <Option_VDF.h>
#include <Champ_Fonc_Face.h>
#include <Matrice_Morse_Sym.h>

Implemente_instanciable_sans_constructeur(Assembleur_P_VDF,"Assembleur_P_VDF",Assembleur_base);

Assembleur_P_VDF::Assembleur_P_VDF() :
  has_P_ref(0)
{
}

Sortie& Assembleur_P_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom() ;
}

Entree& Assembleur_P_VDF::readOn(Entree& s )
{
  return Assembleur_base::readOn(s);
}

// Description:
// Remplit le tableau faces avec la liste des indices des faces periodiques
// dans le tableau faces_voisins. Chaque face periodique figure deux fois
// dans faces_voisins (a chaque face correspond la face opposee). On ne
// met dans le tableau faces que celle des deux qui a l'indice le + petit
// dans la liste des faces de chaque bord periodique.
// Valeur de retour:
// nombre de faces periodiques (egal a la taille du tableau faces).
int Assembleur_P_VDF::liste_faces_periodiques(ArrOfInt& faces)
{
  // On commence par surestimer largement la taille du tableau :
  // nombre de faces de bord
  const int nb_faces_bord = la_zone_VDF.valeur().nb_faces_bord();
  faces.resize_array(nb_faces_bord);

  // Recherche des faces periodiques dans les conditions aux limites:
  const Conds_lim& les_cl = la_zone_Cl_VDF.valeur().les_conditions_limites();
  const int nb_cl = les_cl.size();
  int nb_faces_periodiques = 0;
  for (int num_cl = 0; num_cl < nb_cl; num_cl++)
    {
      const Cond_lim_base& la_cl = les_cl[num_cl].valeur();
      // Selectionne uniquement les conditions Periodique
      if ( ! sub_type(Periodique,la_cl))
        continue;
      const Periodique& la_cl_perio = ref_cast(Periodique, la_cl);
      const Front_VF&    frontiere = ref_cast(Front_VF, la_cl.frontiere_dis());
      const int nb_faces_cl = frontiere.nb_faces();
      const int num_premiere_face = frontiere.num_premiere_face();
      for (int i = 0; i < nb_faces_cl; i++)
        {
          // Numero de la face opposee dans le tableau des faces du bord:
          const int face_associee = la_cl_perio.face_associee(i);
          if (face_associee > i)
            {
              const int num_face_global = num_premiere_face + i;
              faces[nb_faces_periodiques] = num_face_global;
              nb_faces_periodiques++;
            }
        }
    }

  // Taille finale du tableau faces
  faces.resize_array(nb_faces_periodiques);
  return nb_faces_periodiques;
}

// Description:
//  Determine les elements non nuls de la matrice et prepare le stockage.
//  Matrice creuse de taille nb_elements (lignes) * nb_elem_tot (colonnes)
//  Codee comme une matrice bloc composee de deux matrices morse:
//   * Matrice carree symetrique nb_elements * nb_elements
//     (contient les termes M(i,j) ou i et j sont des numeros d'elements reels)
//   * Matrice rectangle nb_elements * (nb_elem_tot - nb_elem)
//     (contient les termes M(i,j) ou i est reel et j est virtuel
int Assembleur_P_VDF::construire(Matrice& la_matrice)
{
  int i;
  const Zone_VDF& zone_vdf   = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_vdf.face_voisins();

  // Comptage du nombre total d'elements non nuls:
  // matrice carree : nombre de faces internes / 2 + nb_elem + nbfaces periodiques
  //                  (chaque face interne donne un coef, et on a un element
  //                   diagonal et chaque face periodique donne aussi un coef)
  // matrice rectangle : nombre de faces de joint


  // Premiere etape : comptage du nombre d'elements non nuls sur chaque ligne
  // Pour chaque ligne de la matrice carree, nombre d'elements non nuls
  const int nb_elem     = zone_vdf.nb_elem();
  const int nb_elem_tot = zone_vdf.nb_elem_tot();
  ArrOfInt carre_nb_non_zero(nb_elem);
  // Idem pour le rectangle
  ArrOfInt rect_nb_non_zero(nb_elem);
  // Il y a l'element sur la diagonale :
  carre_nb_non_zero = 1;
  rect_nb_non_zero = 0;
  int carre_nb_non_zero_tot = nb_elem;
  int rect_nb_non_zero_tot = 0;

  // Plus un element non nul pour chaque face interne et chaque face periodique
  // (matrice symetrique, on ne stocke que l'element m(line,col) avec col>line)

  ArrOfInt liste_faces_perio;
  const int nb_faces_periodiques = liste_faces_periodiques(liste_faces_perio);
  const int nb_faces_internes = zone_vdf.nb_faces_internes();
  const int premiere_face_interne = zone_vdf.premiere_face_int();
  for (i = 0; i < nb_faces_internes + nb_faces_periodiques; i++)
    {
      int face;
      if (i < nb_faces_internes) // Astuce pour boucler sur les faces internes et periodiques
        face = premiere_face_interne + i;
      else
        face = liste_faces_perio[i - nb_faces_internes];

      int elem0 = face_voisins(face,0);
      int elem1 = face_voisins(face,1);
      if (elem0 > elem1)
        {
          int tmp = elem1;
          elem1 = elem0;
          elem0 = tmp;
        }
      if (elem0 < nb_elem)   // elem0 est reel
        {
          if (elem1 < nb_elem)      // elem1 reel
            {
              carre_nb_non_zero[elem0] ++;
              carre_nb_non_zero_tot ++;
            }
          else                      // elem1 virtuel
            {
              rect_nb_non_zero[elem0] ++;
              rect_nb_non_zero_tot ++;
            }
        }
    }

  // Typage et dimensionnement de la matrice de pression
  la_matrice.typer("Matrice_Bloc");
  Matrice_Bloc& matrice =ref_cast(Matrice_Bloc , la_matrice.valeur());
  matrice.dimensionner(1,2);
  matrice.get_bloc(0,0).typer("Matrice_Morse_Sym");
  matrice.get_bloc(0,1).typer("Matrice_Morse");
  Matrice_Morse_Sym& carre = ref_cast(Matrice_Morse_Sym ,matrice.get_bloc(0,0).valeur());
  Matrice_Morse&      rect  = ref_cast(Matrice_Morse ,     matrice.get_bloc(0,1).valeur());

  carre.dimensionner(nb_elem, carre_nb_non_zero_tot);
  rect.dimensionner(nb_elem, nb_elem_tot - nb_elem, rect_nb_non_zero_tot);

  {
    const int nb_faces_bord = zone_vdf.nb_faces_bord();
    les_coeff_pression.resize_array(nb_faces_bord);
  }
  ArrOfInt& carre_tab1 = carre.get_set_tab1();
  ArrOfInt& rect_tab1 = rect.get_set_tab1();

  // Matrice creuse, stockage morse avec des indices fortran:
  // lignes numerotees 1..n, colonnes 1..m
  // Le k-ieme coefficient non nul de la ligne i (1<=i<=n) est (avec 1<=k)
  //   M(i,j) = coeff_[tab1_[k]]     en fortran
  //   M(i,j) = coeff_[tab1_[k-1]-1] en C
  // Le numero j de la colonne ou se trouve ce coefficient (1<=j<=m) est
  //   j = tab2_[tab1_[k]]     en fortran
  //   j = tab2_[tab1_[k-1]-1] en C
  //
  // Calcul de l'indice du premier coefficient de la ligne i
  // dans le tableau d'indices morse des deux matrices (tab1_)
  {
    int indice = 1; // tab1_ contient un indice fortran (1er element en 1)
    for (i = 0; i < nb_elem; i++)
      {
        carre_tab1[i] = indice;
        indice += carre_nb_non_zero[i];
      }
    carre_tab1[i] = indice;

    indice = 1;
    for (i = 0; i < nb_elem; i++)
      {
        rect_tab1[i] = indice;
        indice += rect_nb_non_zero[i];
      }
    rect_tab1[i] = indice;
  }

  // Deuxieme etape : remplissage de tab2_ = numero de la colonne de chaque
  // terme non nul de la matrice
  ArrOfInt& carre_tab2 = carre.get_set_tab2();
  ArrOfInt& rect_tab2 = rect.get_set_tab2();

  carre_tab2 = -1;
  rect_tab2 = -1;

  // Terme diagonal:
  for (i = 1; i <= nb_elem; i++)
    carre_tab2[carre_tab1[i-1]-1] = i; // Indice fortran 1<=i<=nb_elem

  carre_nb_non_zero = 1; // Nombre de coefficients non nuls sur chaque ligne
  rect_nb_non_zero = 0;

  // Termes extra-diagonaux:
  for (int i_face = 0; i_face < nb_faces_internes + nb_faces_periodiques; i_face++)
    {

      // Calcul du numero de la face a traiter
      const int face = (i_face < nb_faces_internes)
                       ? premiere_face_interne + i_face
                       : liste_faces_perio[i_face - nb_faces_internes];

      int elem0 = face_voisins(face,0);
      int elem1 = face_voisins(face,1);
      if (elem0 > elem1)
        {
          int tmp = elem1;
          elem1 = elem0;
          elem0 = tmp;
        }
      assert(elem0 >= 0);            // Verifie qu'on a bien deux elements voisins
      if (elem0 < nb_elem)                              // elem0 est reel
        {
          const int ligne = elem0 + 1;                 // Indice fortran
          if (elem1 < nb_elem)                            // elem1 est reel aussi
            {
              const int colonne = elem1 + 1;             // Indice fortran
              const int n = carre_nb_non_zero[ligne-1]++;
              const int index = carre_tab1[ligne-1] + n; // Indice fortran dans tab2
              carre_tab2[index - 1] = colonne;
            }
          else                                           // elem1 est virtuel
            {
              const int colonne = elem1 - nb_elem + 1;  // Indice fortran
              const int n = rect_nb_non_zero[ligne-1]++;
              const int index = rect_tab1[ligne-1] + n; // Indice fortran dans tab2
              rect_tab2[index - 1] = colonne;
            }
        }
    }

  return 1;
}

// Description:
//  Calcul des coefficients de la matrice de pression avec un champ de rho.
//  Si rho_ptr == 0, on calcule la matrice -div( porosite * grad P ),
//  sinon on calcule -div( porosite/rho grad P ) et *rho_ptr doit etre un Champ_Fonc_Face.
// Precondition:
//  La matrice doit etre typee et dimensionnee, tab1_ et tab2_ remplis
//  avec les elements non nuls de la matrice.
//  On suppose que le tableau tab2_ est inchange depuis son remplissage dans
//  "construire". Le remplissage se fait avec des boucles identiques pour que
//  tab2 et coeff se correspondent.
//
// A essayer : trier les elements d'une ligne par ordre croissant de numero de
// colonne et utiliser un acces matrice(ligne,colonne) avec recherche binaire.
// C'est plus lisible.
//
// La matrice M est telle que si le vecteur P(i) contient la valeur de la pression au centre
// des elements Omega(i), alors le vecteur A=M*P est tel que A(i) vaut :
//  A(i) = - INTEGRALE sur Omega(i)             (div (porosite/rho * grad P) * dOmega)
//       = - INTEGRALE sur le bord de Omega(i)) (porosite/rho * gradP * Normale * ds)
//
// On discretise grad P = (P(i+1) - P(i)) / h, avec h = volume(Omega') / surface(face)
// (Omega' est le volume de controle de la qdm)

int Assembleur_P_VDF::remplir(Matrice& la_matrice, const DoubleVect& volumes_entrelaces,const Champ_Don_base * rho_ptr)
{
  const Zone_VDF& zone_vdf   = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_vdf.face_voisins();
  const DoubleVect& face_surfaces = zone_vdf.face_surfaces();
  //const DoubleVect & volumes_entrelaces = zone_vdf.volumes_entrelaces();
  const DoubleVect& porosite_face = zone_vdf.porosite_face();


  const DoubleVect * valeurs_rho = 0;
  if (rho_ptr)
    {
      assert(sub_type(Champ_Fonc_Face, *rho_ptr));
      valeurs_rho = & (rho_ptr->valeurs());
    }

  // Raccourcis vers la partie carree (coefficients elements reels/reels)
  // et la partie rectangulaire (elements reels / elements virtuels) de la matrice
  Matrice_Bloc& matrice = ref_cast(Matrice_Bloc, la_matrice.valeur());
  Matrice_Morse_Sym& carre = ref_cast(Matrice_Morse_Sym, matrice.get_bloc(0,0).valeur());
  Matrice_Morse&      rect  = ref_cast(Matrice_Morse,     matrice.get_bloc(0,1).valeur());

  const int nb_elem = zone_vdf.nb_elem();
  ArrOfInt carre_nb_non_zero(nb_elem);
  ArrOfInt rect_nb_non_zero(nb_elem);
  carre_nb_non_zero = 1;
  rect_nb_non_zero = 0;

  ArrOfInt& carre_tab1 = carre.get_set_tab1();
  ArrOfInt& rect_tab1 = rect.get_set_tab1();
  ArrOfDouble& carre_coeff = carre.get_set_coeff();
  ArrOfDouble& rect_coeff = rect.get_set_coeff();

  carre_coeff = 0.;
  rect_coeff = 0.;

  // Traitement des faces internes et periodiques :
  // Pour chaque face entre deux elements elem0 et elem1, y a quatre termes a ajouter :
  //   M(elem0,elem0)
  //   M(elem0,elem1)
  //   M(elem1,elem1)
  //   M(elem1,elem0)  (omis car la matrice est stockee symetrique)

  // Construction de la liste des faces periodiques
  ArrOfInt liste_faces_perio;
  const int nb_faces_periodiques = liste_faces_periodiques(liste_faces_perio);
  const int nb_faces_internes = zone_vdf.nb_faces_internes();
  const int premiere_face_interne = zone_vdf.premiere_face_int();
  for (int i_face = 0; i_face < nb_faces_internes + nb_faces_periodiques; i_face++)
    {

      // Calcul du numero de la face a traiter
      const int num_face = (i_face < nb_faces_internes)
                           ? premiere_face_interne + i_face
                           : liste_faces_perio[i_face - nb_faces_internes];
      // Calcul de rho sur cette face
      const double rho_face = (valeurs_rho) ? (*valeurs_rho)[num_face] : 1.;
      // Calcul du coefficient
      const double surface  = face_surfaces[num_face];
      const double volume   = volumes_entrelaces[num_face];
      const double porosite = porosite_face[num_face];
      const double coefficient = surface * surface * porosite / (volume * rho_face);
      // Numeros des deux elements voisins (le plus petit dans elem0)
      int elem0 = face_voisins(num_face,0);
      int elem1 = face_voisins(num_face,1);
      if (elem0 > elem1)
        {
          int tmp = elem1;
          elem1 = elem0;
          elem0 = tmp;
        }
      if (elem0 < nb_elem)
        {
          // elem0 est reel
          const int ligne = elem0 + 1;   // Indice fortran
          // Indice fortran de l'element diagonal (elem0, elem0)
          const int index_diag = carre_tab1[ligne-1];
          carre_coeff[index_diag - 1] += coefficient;
          if (elem1 < nb_elem)
            {
              // elem1 est reel aussi
              // Indice fortran de l'element diagonal (elem1, elem1)
              const int index_diag1 = carre_tab1[elem1]; // a la ligne elem1+1
              // Indice fortran de l'element extradiagonal (elem0, elem1)
              const int n = carre_nb_non_zero[ligne-1]++;
              const int index = index_diag + n;
              // Coefficient diagonal
              carre_coeff[index_diag1 - 1] += coefficient;
              // Coefficient extra-diagonal
              carre_coeff[index - 1] = - coefficient;
              assert(carre.get_tab2()(index - 1) == elem1 + 1);
            }
          else
            {
              // elem1 est virtuel
              const int n = rect_nb_non_zero[ligne-1]++;
              const int index = rect_tab1[ligne-1] + n; // Indice fortran dans tab2
              // Coefficient extra-diagonal
              rect_coeff[index - 1] = - coefficient;
              assert(rect.get_tab2()(index - 1) == elem1 - nb_elem + 1);
            }
        }
    }

  // Traitement des conditions aux limites
  const Conds_lim& les_cl = la_zone_Cl_VDF.valeur().les_conditions_limites();
  const int nb_cl = les_cl.size();
  for (int num_cl = 0; num_cl < nb_cl; num_cl++)
    {
      const Cond_lim_base& la_cl = les_cl[num_cl].valeur();
      const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());

      // Test sur les conditions limites en 2D RZ (on doit avoir symetrie selon l'axe de revolution)
      if (bidim_axi && !sub_type(Symetrie,la_cl))
        {
          const int ndeb = la_front_dis.num_premiere_face();
          const int nfin = ndeb + la_front_dis.nb_faces();
          if (nfin>ndeb && est_egal(face_surfaces[ndeb],0))
            {
              Cerr << "\nFirst face surface is smaller than PrecisionGeom = " << precision_geom << finl;
              Cerr << "May be you have an error in the definition of the boundary conditions." << finl;
              Cerr << "The axis of revolution for this 2D calculation is along Y." << finl;
              Cerr << "So you must specify symmetry boundary condition (symetrie keyword) for the boundary " << la_front_dis.le_nom() << finl;
              exit();
            }
        }

      // Pour chaque face de bord entre elem0 et un element fictif exterieur
      // a pression imposee P0, on a :
      //    grad P = (P(elem0) - P0) * surface / volume_entrelace
      // elem0 est une inconnue, P0 est ajoute au second membre dans "modifier_secmem".
      if (sub_type(Neumann_sortie_libre,la_cl))
        {
          has_P_ref = 1;
          carre.set_est_definie(1);
          const int ndeb = la_front_dis.num_premiere_face();
          const int nfin = ndeb + la_front_dis.nb_faces();
          for (int num_face = ndeb; num_face < nfin; num_face++)
            {
              // Calcul de rho sur cette face
              const double rho_face = (valeurs_rho) ? (*valeurs_rho)[num_face] : 1.;
              // Calcul du coefficient a ajouter dans la matrice
              const double surface  = face_surfaces[num_face];
              // Attention: le volume entrelace a une valeur particuliere au bord
              // (voir Zone_VDF::calculer_volumes_entrelaces() )
              const double volume   = volumes_entrelaces[num_face];
              const double porosite = porosite_face[num_face];
              const double coefficient = Option_VDF::coeff_P_neumann * surface * surface * porosite / (volume * rho_face);
              assert(coefficient > 0.);
              // Numero de l'element voisin (l'un est -1, l'autre est un element reel)
              const int elem0 = face_voisins(num_face, 0);
              const int elem1 = face_voisins(num_face, 1);
              assert(elem0 == -1 || elem1 == -1);
              const int elem = elem0 + elem1 + 1;
              // Ajout du coefficient a la matrice
              assert(elem < nb_elem);
              const int index = carre_tab1[elem]; // Indice fortran
              carre_coeff[index - 1] += coefficient;
              les_coeff_pression[num_face] = coefficient;
            }
        }
      else
        {
          // Pour les autres conditions aux limites, aucun terme supplementaire dans
          // la matrice (grad P scalaire n = 0 sur le bord,
          // ou derivee en temps de grad P scalaire n = 0 sur le bord)
        }
    }
  has_P_ref = (int)mp_max(has_P_ref);

  // Verification sanitaire: pas d'element nul sur la diagonale
  for (int i = 0; i < nb_elem; i++)
    {
      const int index = carre_tab1[i];
      const double coeff_diagonal = carre_coeff[index - 1];
      if (coeff_diagonal == 0.)
        {
          // La maille i n'a pas de voisin: pression quelconque
          carre_coeff[index - 1] = 1.;
        }
    }

  carre.compacte();
  rect.compacte();
  return 1;
}

// Description:
// Modification du second membre pour appliquer les conditions aux limites.
// Les conditions prises en charge sont
//  Neumann_sortie_libre,
//  Entree_fluide_vitesse_imposee,
//  Dirichlet_paroi_defilante (rien a faire),
//  Dirichlet_paroi_fixe (rien a faire),
//  Symetrie (rien a faire)
int Assembleur_P_VDF::modifier_secmem(DoubleTab& secmem)
{
  const Zone_Cl_VDF& la_zone_cl = la_zone_Cl_VDF.valeur();
  int nb_cond_lim = la_zone_cl.nb_cond_lim();

  for (int indice_cl = 0; indice_cl < nb_cond_lim; indice_cl++)
    {
      const Cond_lim_base& la_cl_base =
        la_zone_cl.les_conditions_limites(indice_cl).valeur();

      const Front_VF& frontiere_vf = ref_cast(Front_VF, la_cl_base.frontiere_dis());

      if (sub_type(Neumann_sortie_libre, la_cl_base))
        {
          modifier_secmem_pression_imposee(ref_cast( Neumann_sortie_libre, la_cl_base),
                                           frontiere_vf,
                                           secmem);
        }
      else if (sub_type(Entree_fluide_vitesse_imposee, la_cl_base))
        {
          modifier_secmem_vitesse_imposee(ref_cast(Entree_fluide_vitesse_imposee ,la_cl_base),
                                          frontiere_vf,
                                          secmem);
        }
      else if (sub_type(Dirichlet_paroi_defilante, la_cl_base))
        {
          // Pour une paroi defilante, rien a faire.
        }
      else if (sub_type(Dirichlet_paroi_fixe, la_cl_base))
        {
          // Rien a faire non plus.
        }
      else if (sub_type(Symetrie, la_cl_base))
        {
          // Encore rien a faire
        }
      else if (sub_type(Periodique, la_cl_base))
        {
          // Rien a faire
        }
      else
        {
          Cerr << "Erreur dans Assembleur_P_VDF::modifier_secmem\n la condition aux limites ";
          Cerr << la_cl_base.que_suis_je() << " n'est pas prise en charge." << finl;
          assert(0);
          exit();
        }
    }
  secmem.echange_espace_virtuel();
  return 1;
}

// Description:
// Modification du second membre du solveur en pression pour une condition
//  "Neumann_sortie_libre".
// Calcul en "increment de pression" :
//  ajouter l'increment de pression, c'est a dire zero (c.l. instationnaire non supportee)
// Calcul en "pression" :
//  Ajout du terme Pimpose * surface / volume_entrelace au second membre dans la discretisation de la
//  pression au bord (entre un element elem0 et un element fictif exterieur a pression imposee) :
//    grad P = (P(elem0) - Pimpose) * surface / volume_entrelace

void Assembleur_P_VDF::modifier_secmem_pression_imposee(const Neumann_sortie_libre& cond_lim,
                                                        const Front_VF& frontiere_vf,
                                                        DoubleTab& secmem)
{
  const Zone_VDF& la_zone = la_zone_VDF.valeur();
  const IntTab& face_voisins = la_zone.face_voisins();
  if (get_resoudre_increment_pression())
    {
      /*
        const Champ_front_base & champ_front = cond_lim.champ_front().valeur();
        if (sub_type(Champ_front_instationnaire_base, champ_front)
        || sub_type(Champ_front_var_instationnaire, champ_front)) {
        Cerr << "Erreur dans Assembleur_P_VDF::modifier_secmem_pression_imposee\n ";
        Cerr << champ_front.que_suis_je();
        Cerr << " + resoudre_increment_pression non code" << finl;
        assert(0);
        exit();
        } else {
        // Champ stationnaire, on ajoute un increment de pression nul.
        // Donc rien a faire.
        }
      */
    }
  else
    {
      const int nb_faces = frontiere_vf.nb_faces();
      const int num_premiere_face = frontiere_vf.num_premiere_face();
      for (int i = 0; i < nb_faces; i++)
        {
          const int num_face = num_premiere_face + i;
          const double Pimp = cond_lim.flux_impose(i);
          const double coef = les_coeff_pression[num_face] * Pimp;
          const int elem = face_voisins(num_face, 0) + face_voisins(num_face, 1) + 1;
          secmem[elem] += coef;
        }
    }
}

// Description:
// Modification du second membre du systeme en pression pour une condition aux limites
// de vitesse imposee.
// Si on resout en increment de pression, ...
// sinon rien a faire.
void Assembleur_P_VDF::modifier_secmem_vitesse_imposee(const Entree_fluide_vitesse_imposee& cond_lim,
                                                       const Front_VF& frontiere_vf,
                                                       DoubleTab& secmem)
{
  const Champ_front_base& champ_front = cond_lim.champ_front().valeur();
  const Zone_VDF& la_zone = la_zone_VDF.valeur();
  const DoubleVect& face_surfaces = la_zone.face_surfaces();
  const IntTab& face_voisins = la_zone.face_voisins();

  if (get_resoudre_en_u())
    {
      int gpoint_variable;
      if (sub_type(Champ_front_instationnaire_base, champ_front))
        gpoint_variable = 0; // Instationnaire uniforme
      else if (sub_type(Champ_front_var_instationnaire, champ_front))
        gpoint_variable = 1; // Instationnaire variable
      else
        gpoint_variable = -1; // Stationnaire

      if (gpoint_variable >= 0)
        {
          const DoubleTab& tab_gpoint = (gpoint_variable)
                                        ? ref_cast(Champ_front_var_instationnaire,  champ_front).Gpoint()
                                        : ref_cast(Champ_front_instationnaire_base, champ_front).Gpoint();

          const int nb_faces = frontiere_vf.nb_faces();
          const int num_premiere_face = frontiere_vf.num_premiere_face();
          for (int i = 0; i < nb_faces; i++)
            {
              const int num_face = num_premiere_face + i;
              const double surface = face_surfaces(num_face);
              const int elem0 = face_voisins(num_face, 0);
              const int elem1 = face_voisins(num_face, 1);
              // gpoint est relatif a la normale a la face (elle pointe vers elem1)
              // La normale est-elle entrante ou sortante ?
              const double signe = (elem0 < 0) ? 1. : -1.;
              // Numero de l'element adjacent a la face de bord
              const int elem = elem0 + elem1 + 1;
              const int ori = la_zone.orientation(num_face);
              const double gpoint = (gpoint_variable) ? tab_gpoint(i, ori) : tab_gpoint(ori);

              secmem[elem] += signe * surface * gpoint;
            }
        }
      else
        {
          // Le champ frontiere est stationnaire, rien a faire.
        }
    }
  else
    {
      // Resolution en pression: la condition aux limites est imposee ailleurs
    }
}

int Assembleur_P_VDF::modifier_solution(DoubleTab& pression)
{
  // Projection :
  double press_0;
  if(!has_P_ref)
    {
      // On prend la pression minimale comme pression de reference
      // afin d'avoir la meme pression de reference en sequentiel et parallele
      press_0=DMAXFLOAT;
      int nb_elem=la_zone_VDF.valeur().zone().nb_elem();
      for(int n=0; n<nb_elem; n++)
        if (pression[n] < press_0)
          press_0 = pression[n];
      press_0 = mp_min(press_0);
      pression -=press_0;
      pression.echange_espace_virtuel();
    }
  return 1;
}
int Assembleur_P_VDF::assembler_mat(Matrice& matrice,const DoubleVect& volumes_entrelaces,int incr_pression,int resoudre_en_u)
{
  if (! matrice.non_nul())
    {
      if (je_suis_maitre())
        Cerr << "Assemblage de la matrice pression : Assembleur_P_VDF::assembler" << finl;
      // Par defaut, resolution en increment de pression
      construire(matrice);
    }
  set_resoudre_increment_pression(incr_pression);
  set_resoudre_en_u(resoudre_en_u);

  remplir(matrice,volumes_entrelaces, 0);
  return 1;
}

// Description:
//  Assemblage de la matrice de pression M telle que
//   M*P = div(porosite * grad (P))
//  et calcul des coefficients pour modifier_secmem.
int Assembleur_P_VDF::assembler(Matrice& matrice)
{
  if (je_suis_maitre())
    Cerr << "Assemblage de la matrice pression : Assembleur_P_VDF::assembler" << finl;
  // Par defaut, resolution en increment de pression
  set_resoudre_increment_pression(1);
  set_resoudre_en_u(1);
  construire(matrice);
  const Zone_VDF& zone_vdf   = la_zone_VDF.valeur();

  const DoubleVect& volumes_entrelaces = zone_vdf.volumes_entrelaces();
  remplir(matrice,volumes_entrelaces, 0);
  return 1;
}

// Description:
//  Assemblage de la matrice de pression M telle que
//   M*P = div(porosite/rho * grad (P))
//  et calcul des coefficients pour modifier_secmem.
// Parametre: matrice
//  Signification: La matrice a assembler.
//  Contrainte:    Soit la matrice n'est pas encore typee (alors on la "construit"),
//                 soit c'est la meme que lors de l'appel precedent.
// Parametre: rho
//  Contrainte: Doit etre un champ de type Champ_Fonc_Face.
int Assembleur_P_VDF::assembler_rho_variable(Matrice& matrice,
                                             const Champ_Don_base& rho)
{
  // assembler_rho_variable a ete introduit pour le front-tracking:
  // il faut dire explicitement si on resout en increment de pression
  assert(get_resoudre_increment_pression() >= 0);
  // idem pour resoudre en u
  assert(get_resoudre_en_u() >= 0);
  // Si la matrice n'a pas encore ete typee, il faut la construire :
  if (! matrice.non_nul())
    {
      if (je_suis_maitre())
        {
          Cerr << "Assemblage de la matrice pression : ";
          Cerr << "Assembleur_P_VDF::assembler_rho_variable" << finl;
        }
      construire(matrice);
    }
  const Zone_VDF& zone_vdf   = la_zone_VDF.valeur();

  const DoubleVect& volumes_entrelaces = zone_vdf.volumes_entrelaces();
  remplir(matrice,volumes_entrelaces, & rho);
  return 1;
}

// Description:
//    Assemble la matrice de pression pour un fluide quasi compressible.
//    La matrice M est telle que M*P = div( porosite * grad(P) ).
//    Le drapeau resoudre_increment_pression est mis a zero s'il n'a pas
//    encore ete assigne.
// Precondition:
// Parametre: DoubleTab& tab_rho
//    Signification: mass volumique
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: lecture
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Assembleur_P_VDF::assembler_QC(const DoubleTab& tab_rho, Matrice& matrice)
{
  // Par defaut pour le qc: resolution en pression et pas en increment pression.
  if (get_resoudre_increment_pression() < 0)
    {
      set_resoudre_increment_pression(1);
      set_resoudre_en_u(0);
    }
  if (! matrice.non_nul())
    {
      if (je_suis_maitre())
        {
          Cerr << "Assemblage de la matrice pression : ";
          Cerr << "Assembleur_P_VDF::assembler_QC" << finl;
        }
      construire(matrice);
      const Zone_VDF& zone_vdf   = la_zone_VDF.valeur();

      const DoubleVect& volumes_entrelaces = zone_vdf.volumes_entrelaces();
      remplir(matrice,volumes_entrelaces, 0);

      Matrice_Bloc& matrice_bloc=ref_cast(Matrice_Bloc,matrice.valeur());
      Matrice_Morse_Sym& la_matrice =ref_cast(Matrice_Morse_Sym,matrice_bloc.get_bloc(0,0).valeur());
      if (la_matrice.get_est_definie()!=1)
        {
          if ((je_suis_maitre()) && (la_matrice.nb_lignes()==0) && (la_matrice.nb_colonnes()==0))
            {
              Cerr<<"Pressure matrix will not be defined."<<finl;
              exit();
            }

          if ((la_matrice.nb_lignes()>0) && (la_matrice.nb_colonnes()>0))
            {
              Cerr<<"la_matrice(0,0)"<<la_matrice(0,0)<<finl;
              Cerr<<"Pas de pression imposee  --> P(0)=0"<<finl;
              if (je_suis_maitre())    la_matrice(0,0) *= 2;
            }
          la_matrice.set_est_definie(1);
        }
    }
  return 1;
}

const Zone_dis_base& Assembleur_P_VDF::zone_dis_base() const
{
  return la_zone_VDF.valeur();
}

const Zone_Cl_dis_base& Assembleur_P_VDF::zone_Cl_dis_base() const
{
  return la_zone_Cl_VDF.valeur();
}

void Assembleur_P_VDF::associer_zone_dis_base(const Zone_dis_base& la_zone_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, la_zone_dis);
}

void Assembleur_P_VDF::associer_zone_cl_dis_base(const Zone_Cl_dis_base& la_zone_Cl_dis)
{
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, la_zone_Cl_dis);
}

void Assembleur_P_VDF::completer(const Equation_base& Eqn)
{
  // CCa 30/04/99 : je ne sais pas si je dois faire qqchose
  ;
}
