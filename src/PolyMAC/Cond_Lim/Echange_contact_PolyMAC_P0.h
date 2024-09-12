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

#ifndef Echange_contact_PolyMAC_P0_included
#define Echange_contact_PolyMAC_P0_included

#include <Echange_contact_PolyMAC_P0P1NC.h>
#include <Op_Diff_PolyMAC_P0_Elem.h>
#include <Domaine_forward.h>

/*! @brief classe : Echange_contact_PolyMAC_P0 Outre le champ_front representant la temperature de paroi,
 *
 *   cette classe possede un autre champ_front avec autant de valeurs
 *   temporelles qui represente la temperature dans l'autre probleme.
 *
 */
class Echange_contact_PolyMAC_P0  : public Echange_contact_PolyMAC_P0P1NC
{
  Declare_instanciable(Echange_contact_PolyMAC_P0);
public :
  void init_op() const;

  mutable OBS_PTR(Op_Diff_PolyMAC_P0_Elem) diff, o_diff; //operateurs de diffusion de chaque cote

  /* faces, sommets de l'autre cote de la frontiere */
  void init_fs_dist() const; //initialisation de f_dist, s_dist
  mutable std::map<int, int> s_dist; //s_dist[sommet de ce cote] = sommet de l'autre cote
  mutable int fs_dist_init_ = 0;
};

#endif /* Echange_contact_PolyMAC_P0_included */
