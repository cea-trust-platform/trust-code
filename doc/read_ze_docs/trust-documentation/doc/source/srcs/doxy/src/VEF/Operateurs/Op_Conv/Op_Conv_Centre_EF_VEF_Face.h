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

#ifndef Op_Conv_Centre_EF_VEF_Face_included
#define Op_Conv_Centre_EF_VEF_Face_included

#include <Op_Conv_VEF_base.h>
/*! @brief class Op_Conv_Centre_EF_VEF_Face
 *
 *   Cette classe represente l'operateur de convection associe a une equation de
 *   transport d'un scalaire.
 *   La discretisation est VEF
 *   Le champ convecte est scalaire ou vecteur de type Champ_P1NC
 *   Le schema de convection est du type Centre
 *   On calcule le flux sur la surface a l aide des fonctions de forme
 *
 *
 * @sa Operateur_Conv_base
 */
class Op_Conv_Centre_EF_VEF_Face: public Op_Conv_VEF_base
{

public:
  Declare_instanciable(Op_Conv_Centre_EF_VEF_Face);

public:

  DoubleTab& ajouter(const DoubleTab& , DoubleTab& ) const override;

protected:

  // DoubleVect porosite_face;
};

#endif
