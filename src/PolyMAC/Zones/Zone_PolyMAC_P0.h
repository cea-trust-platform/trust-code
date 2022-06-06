
/****************************************************************************
* Copyright (c) 2015, CEA
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

#ifndef Zone_PolyMAC_P0_included
#define Zone_PolyMAC_P0_included

#include <Champ_front_var_instationnaire.h>
#include <Echange_global_impose.h>
#include <Neumann_sortie_libre.h>
#include <Matrice_Morse_Sym.h>
#include <Neumann_homogene.h>
#include <Static_Int_Lists.h>
#include <Zone_PolyMAC.h>
#include <SolveurSys.h>
#include <TRUSTLists.h>
#include <Periodique.h>
#include <TRUSTTrav.h>
#include <Dirichlet.h>
#include <Conds_lim.h>
#include <Symetrie.h>
#include <Zone_Poly_base.h>
#include <Domaine.h>
#include <Lapack.h>
#include <math.h>
#include <vector>
#include <string>
#include <array>
#include <map>

class Geometrie;

//
// .DESCRIPTION class Zone_PolyMAC_P0
//
// 	Classe instanciable qui derive de Zone_VF.
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


class Zone_PolyMAC_P0 : public Zone_PolyMAC
{

  Declare_instanciable(Zone_PolyMAC_P0);

public :

  void discretiser() override;

  //stencil du gradient aux faces fgrad : fsten_eb([fsten_d(f), fsten_d(f + 1)[]) -> elements e, faces de bord ne_tot + f
  void init_stencils() const;
  mutable IntTab fsten_d, fsten_eb;

  //pour u.n champ T aux elements, interpole [n_f.nu.grad T]_f
  //en preservant exactement les champs verifiant [nu grad T]_e = cte.
  //Entrees : N             : nombre de composantes
  //          is_p          : 1 si on traite le champ de pression (inversion Neumann / Dirichlet)
  //          cls           : conditions aux limites
  //          fcl(f, 0/1/2) : donnes sur les CLs (type de CL, indice de CL, indice dans la CL) (cf. Champ_{P0,Face}_PolyMAC_P0)
  //          nu(e, n, ..)  : diffusivite aux elements (optionnel)
  //          som_ext       : liste de sommets a ne pas traiter (ex. : traitement direct des Echange_Contact dans Op_Diff_PolyMAC_P0_Elem)
  //          virt          : 1 si on veut aussi le flux aux faces virtuelles
  //          full_stencil  : 1 si on veut le stencil complet (pour dimensionner())
  //Sorties : phif_d(f, 0/1)                       : indices dans phif_{e,c} / phif_{pe,pc} du flux a f dans [phif_d(f, 0/1), phif_d(f + 1, 0/1)[
  //          phif_e(i), phif_c(i, n, c)           : indices/coefficients locaux (pas d'Echange_contact) et diagonaux (composantes independantes)
  void fgrad(int N, int is_p, const Conds_lim& cls, const IntTab& fcl, const DoubleTab *nu, const IntTab *som_ext,
             int virt, int full_stencil, IntTab& phif_d, IntTab& phif_e, DoubleTab& phif_c) const;

  //MD_Vectors pour Champ_Face_PolyMAC_P0 (faces + d x elems)
  MD_Vector mdv_ch_face;

private:
  mutable int first_fgrad_ = 1; //pour n'afficher le message "MPFA-O MPFA-O(h) VFSYM" qu'une seule fois par calcul
};

#endif
