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

#ifndef Domaine_PolyMAC_P0_included
#define Domaine_PolyMAC_P0_included

#include <Domaine_PolyMAC_P0P1NC.h>

class Domaine_PolyMAC_P0 : public Domaine_PolyMAC_P0P1NC
{
  Declare_instanciable(Domaine_PolyMAC_P0);
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

protected:
  mutable int first_fgrad_ = 1; //pour n'afficher le message "MPFA-O MPFA-O(h) VFSYM" qu'une seule fois par calcul
};

#endif /* Domaine_PolyMAC_P0_included */
