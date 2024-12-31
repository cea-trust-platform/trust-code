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

#ifndef Champ_Fonc_Elem_PolyVEF_rot_included
#define Champ_Fonc_Elem_PolyVEF_rot_included

#include <Champ_Fonc_Elem_PolyMAC_P0_rot.h>

/*! @brief class Champ_Fonc_Elem_PolyVEF_rot for the calculation of the vorticity
 *       This field is a Champ_Fonc_Elem_PolyVEF with 1 value per element and per phase in 2D and 3 in 3D
 *
 *       It isn't a Champ_Fonc_Face_PolyVEF as there is no physical justification to project the vorticity on a face
 *       In 3D, Champ_Fonc_Elem_PolyVEF_TC::valeurs()(e, n*D + d) returns the value of phase n in element e along the d component
 *       The vorticity is calculated by hand in 2D and 3D using the values of the gradient
 *
 */

class Champ_Fonc_Elem_PolyVEF_rot: public Champ_Fonc_Elem_PolyMAC_P0_rot
{
  Declare_instanciable(Champ_Fonc_Elem_PolyVEF_rot);
public:
  void me_calculer_2D() override;
  void me_calculer_3D() override;
};

#endif /* Champ_Fonc_Elem_PolyVEF_rot_included */
