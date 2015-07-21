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
// File:        Traitement_particulier_NS_Pression_VDF.h
// Directory:   $TRUST_ROOT/src/VDF
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_Pression_VDF_included
#define Traitement_particulier_NS_Pression_VDF_included

#include <Traitement_particulier_NS_Pression.h>
#include <Champ_P0_VDF.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_NS_Pression_VDF
//     Cette classe permet de faire les traitements particuliers
//     pour les calculs de la pression
//
// .SECTION voir aussi
//      Navier_Stokes, Traitement_particulier_base,
//      Traitement_particulier_Pression
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_Pression_VDF : public Traitement_particulier_NS_Pression
{
  Declare_instanciable_sans_constructeur(Traitement_particulier_NS_Pression_VDF);

public :

  Traitement_particulier_NS_Pression_VDF();
  int a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const ;
  int comprend_champ(const Motcle& mot) const ;
  void post_traitement_particulier(void) ;
  Entree& lire(Entree& );
  /////////////////////////////////////////////////////

protected :
  Champ_P0_VDF ch_p ;

private:
  void post_traitement_particulier_calcul_pression() ;

  int c_pression;

};

#endif
