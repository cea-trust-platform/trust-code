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
// File:        Transport_K_Eps_Bas_Reynolds.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

// .SECTION voir aussi
//  Transport_K_Eps
//////////////////////////////////////////////////////////////////////////////

#ifndef Transport_K_Eps_Bas_Reynolds_included
#define Transport_K_Eps_Bas_Reynolds_included

#include <Transport_K_Eps_non_std.h>
#include <Modele_Fonc_Bas_Reynolds.h>
#include <Ref_Modele_Fonc_Bas_Reynolds.h>

class Motcle;

class Transport_K_Eps_Bas_Reynolds : public Transport_K_Eps_non_std
{

  Declare_instanciable(Transport_K_Eps_Bas_Reynolds);

public :

  void associer_milieu_base(const Milieu_base&);
  void associer_modele_turbulence(const Mod_turb_hyd_RANS&);
  inline const Modele_Fonc_Bas_Reynolds& modele_fonc() const;
  inline  Modele_Fonc_Bas_Reynolds& modele_fonc();
  inline const Champ_Inc& vitesse_transportante();
  const Motcle& domaine_application() const;
  void completer();

private :

  REF(Modele_Fonc_Bas_Reynolds) mon_modele_fonc;

};


// Fonctions inline:

inline const Modele_Fonc_Bas_Reynolds& Transport_K_Eps_Bas_Reynolds::modele_fonc() const
{
  return mon_modele_fonc.valeur();
}

inline Modele_Fonc_Bas_Reynolds& Transport_K_Eps_Bas_Reynolds::modele_fonc()
{
  return mon_modele_fonc.valeur();
}


#endif


