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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Paroi_negligeable_scal_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_negligeable_scal_VDF.h>
Implemente_instanciable(Paroi_negligeable_scal_VDF,"negligeable_scalaire_VDF",Paroi_scal_hyd_base_VDF);


//     printOn()
/////

Sortie& Paroi_negligeable_scal_VDF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_negligeable_scal_VDF::readOn(Entree& s)
{
  return s ;
}

// int Paroi_negligeable_scal_VDF::init_lois_paroi()
// {
//   const Zone_VDF& zvdf = la_zone_VDF.valeur();
//   tab_d_equiv_.resize(zvdf.nb_faces_bord());

//   if (axi)
//     for (int num_face=0; num_face<zvdf.nb_faces_bord(); num_face++)
//       tab_d_equiv_[num_face] = zvdf.dist_norm_bord_axi(num_face);
//   else
//     for (int num_face=0; num_face<zvdf.nb_faces_bord(); num_face++)
//       tab_d_equiv_[num_face] = zvdf.dist_norm_bord(num_face);
//   return 1;
// }

/////////////////////////////////////////////////////////////////////////
//
//    Implementation des fonctions de la classe Paroi_negligeable_scal_VDF
//
////////////////////////////////////////////////////////////////////////

int Paroi_negligeable_scal_VDF::calculer_scal(Champ_Fonc_base& diffusivite_turb)
{
  // On ne calcule rien dans le cas ou la loi de paroi est negligeable
  return 1;
}


// Description:
//    Give a boolean indicating that we don't use equivant distance
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: boolean
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
//
bool Paroi_negligeable_scal_VDF::use_equivalent_distance() const
{
  return false;
}
