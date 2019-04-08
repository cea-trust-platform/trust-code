/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        grad_U_Champ_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Champs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <grad_U_Champ_Face_PolyMAC.h>
#include <Champ_Face_PolyMAC.h>
#include <Zone_Cl_PolyMAC.h>

Implemente_instanciable(grad_U_Champ_Face_PolyMAC,"grad_U_Champ_Face_PolyMAC",Champ_Fonc_P0_PolyMAC);


//     printOn()
/////

Sortie& grad_U_Champ_Face_PolyMAC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& grad_U_Champ_Face_PolyMAC::readOn(Entree& s)
{
  return s ;
}

void grad_U_Champ_Face_PolyMAC::associer_champ(const Champ_Face_PolyMAC& la_vitesse)
{
  vitesse_= la_vitesse;
}

void grad_U_Champ_Face_PolyMAC::me_calculer(double tps)
{
  vitesse_->interp_gve(vitesse_.valeur().valeurs(), valeurs());
}


