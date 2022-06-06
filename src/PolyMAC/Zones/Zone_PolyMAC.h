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

#ifndef Zone_PolyMAC_included
#define Zone_PolyMAC_included

#include <Champ_front_var_instationnaire.h>
#include <Echange_global_impose.h>
#include <Neumann_sortie_libre.h>
#include <Matrice_Morse_Sym.h>
#include <Static_Int_Lists.h>
#include <Neumann_homogene.h>
#include <Elem_poly.h>
#include <TRUSTLists.h>
#include <SolveurSys.h>
#include <Periodique.h>
#include <Dirichlet.h>
#include <Conds_lim.h>
#include <TRUSTTrav.h>
#include <Symetrie.h>
#include <Zone_Poly_base.h>
#include <Domaine.h>
#include <Lapack.h>
#include <vector>
#include <string>
#include <math.h>
#include <array>
#include <map>

#ifndef __clang__
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif

class Geometrie;

//
// .DESCRIPTION class Zone_PolyMAC
//
// 	Classe instanciable qui derive de Zone_Poly_base.
// 	Cette classe contient les informations geometriques que demande
// 	la methode des Volumes Elements Finis (element de Crouzeix-Raviart)
// 	La classe porte un certain nombre d'informations concernant les faces
// 	Dans cet ensemble de faces on fait figurer aussi les faces du bord et
//      des joints. Pour manipuler les faces on distingue 2 categories:
//           - les faces non standard qui sont sur un joint, un bord ou qui sont
//             internes tout en appartenant a un element du bord
//           - les faces standard qui sont les faces internes n'appartenant pas
//             a un element du bord
//      Cette distinction correspond au traitement des conditions aux limites:les
//      faces standard ne "voient pas" les conditions aux limites.
//      L'ensemble des faces est numerote comme suit:
//           - les faces qui sont sur une Zone_joint apparaissent en premier
//    	       (dans l'ordre du vecteur les_joints)
//   	     - les faces qui sont sur une Zone_bord apparaissent ensuite
//	       (dans l'ordre du vecteur les_bords)
//  	     - les faces internes non standard apparaissent ensuite
//           - les faces internes standard en dernier
//      Finalement on trouve regroupees en premier toutes les faces non standard
//      qui vont necessiter un traitement particulier
//      On distingue deux types d'elements
//           - les elements non standard : ils ont au moins une face de bord
//           - les elements standard : ils n'ont pas de face de bord
//      Les elements standard (resp. les elements non standard) ne sont pas ranges
//      de maniere consecutive dans l'objet Zone. On utilise le tableau
//      rang_elem_non_std pour acceder de maniere selective a l'un ou
//      l'autre des types d'elements
//
//


class Zone_PolyMAC : public Zone_Poly_base
{

  Declare_instanciable(Zone_PolyMAC);

public :

  void discretiser() override;

  inline const DoubleVect& longueur_aretes() const
  {
    return longueur_aretes_;
  };
  inline const DoubleTab& ta() const
  {
    return ta_;
  };

  void orthocentrer();

  //som_arete[som1][som2 > som1] -> arete correspondant a (som1, som2)
  std::vector<std::map<int, int> > som_arete;

  //matrices locales par elements (operateurs de Hodge) permettant de faire des interpolations :
  void M2(const DoubleTab *nu, int e, DoubleTab& m2) const; //normales aux faces -> tangentes aux faces duales :   (nu x_ef.v)    = m2 (|f|n_ef.v)
  void W2(const DoubleTab *nu, int e, DoubleTab& w2) const; //tangentes aux faces duales -> normales aux faces :   (nu |f|n_ef.v) = w2 (x_ef.v)
  void M1(const DoubleTab *nu, int e, DoubleTab& m1) const; //normales aux aretes duales -> tangentes aux aretes : (nu|a|t_a.v)   = m1 (S_ea.v)
  void W1(const DoubleTab *nu, int e, DoubleTab& w1) const; //tangentes aux aretes -> normales aux aretes duales : (nu S_ea.v)    = w1 (|a|t_a.v)
  //possibilite pour le tenseur nu :
  //nul -> nu = Id ci-dessous
  //isotrope -> nu(n_e, N) avec n_e = 1 (tenseur constant) / nb_elem_tot() (tenseur par element), et N un nombre de composantes
  //anisotrope -> nu(n_e, N, D) (anisotrope diagonal) ou nu(n_e, N, D, D) (anisotrope complet)
  //la matrice en sortie est de taile (n_f, n_f, N) (pour M2/W2) ou (n_a, n_a, N) (pour M1 / W1)

  //MD_Vectors pour Champ_Elem_PolyMAC (elems + faces) et pour Champ_Face_PolyMAC (faces + aretes)
  mutable MD_Vector mdv_elems_faces, mdv_faces_aretes;

private:
  DoubleVect longueur_aretes_; //longueur des aretes
  mutable DoubleTab ta_;       //vecteurs tangents aux aretes
};

#endif
