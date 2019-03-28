/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Loi_Paroi_Nu_Impose_PolyMAC.h
// Directory:   $TRUST_ROOT/src/PolyMAC/Turbulence
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Loi_Paroi_Nu_Impose_PolyMAC_included
#define Loi_Paroi_Nu_Impose_PolyMAC_included

#include <Paroi_scal_hyd_base_PolyMAC.h>
#include <Champ_Don.h>
#include <Parser_U.h>

//.DESCRIPTION classe Loi_Paroi_Nu_Impose_PolyMAC
//
//
//

//.SECTION  voir aussi
//  Paroi_std_hyd_PolyMAC
/**
 * Cette classe permet d'imposer une loi de paroi de type nombre de Nusselt imposee.
 * La fonction de Nusselt rentree par l'utilisateur peut etre une fonction de l'espace, du nombre de Reynolds
 * et du nombre de Prandtl.
 * Ces nombres de Reynolds et de Prandtl sont evalues a partir des quantites en paroi (vitesse, temperature etc...). Il convient donc de tenir compte de cela dans l'entree de la correlation du Nusselt.
 * En effet, souvent les correlations de Nusselt utilise la vitesse moyenne dans la conduite.
 * C'est pourquoi cette loi de paroi s'appliquera souvent dans le cas ou le probleme fluide ets resolu
 * avec une seulle maille afin que la vitesse calcule represente exactement par conservation du debit la vitesse moyenne de l'ecoulement.
 * Syntaxe :
 * Loi_Paroi_Nu_Impose { nusselt fontion_de_(x,y,z,Re,Pr)
 diam_hydr valeur_diametre_hydraulique
 }
*/

class Loi_Paroi_Nu_Impose_PolyMAC : public Paroi_scal_hyd_base_PolyMAC //Turbulence_paroi_scal_base , public Loi_Paroi_scal_Nu_Impose
{

  Declare_instanciable(Loi_Paroi_Nu_Impose_PolyMAC);

public:
  int calculer_scal(Champ_Fonc_base& ) ;
  void imprimer_nusselt(Sortie&) const;

protected:

  Champ_Don diam_hydr;
  Parser_U nusselt;
  //de quoi calculer la vitesse tangente aux faces de bord
  mutable int init_done;
  mutable IntTab i_face[2];                          //interpolation
  mutable DoubleTab base_faces[2];                   //bases des plans normaux aux faces de bord
};


///////////////////////////////////////////////////////////
//
//  Fonctions inline de la classe Loi_Paroi_Nu_Impose_PolyMAC
//
///////////////////////////////////////////////////////////


#endif

