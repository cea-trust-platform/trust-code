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
// File:        Traitement_particulier_NS_Brech_VEF.h
// Directory:   $TRUST_ROOT/src/VEF
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_Brech_VEF_included
#define Traitement_particulier_NS_Brech_VEF_included

#include <Traitement_particulier_NS_VEF.h>
#include <Champ_Fonc_P1NC.h>
#include <Ref_Champ_Inc_base.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_Brech_VEF
//     Cette classe permet de faire les traitements particuliers
//     pour le calcul des flux pour le cas Brech :
//
// .SECTION voir aussi
//      Navier_Stokes_Turbulent, Traitement_particulier_base,
//      Traitement_particulier_VEF
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_Brech_VEF : public Traitement_particulier_NS_VEF
{
  Declare_instanciable_sans_constructeur(Traitement_particulier_NS_Brech_VEF);

public :

  Traitement_particulier_NS_Brech_VEF();
  /*
    inline int a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const ;
    inline int comprend_champ(const Motcle& mot) const ;
  */
  inline void post_traitement_particulier(void) ;
  Entree& lire(Entree& );


  /////////////////////////////////////////////////////

protected :
  Champ_Fonc_P1NC ch_ri ;
  Champ_Fonc_P1NC  ch_p ;

private:
  void  calculer_terme_production_K(const Zone_VEF& ,const Zone_Cl_VEF& ,
                                    DoubleVect& ,
                                    const DoubleTab& ) const;

  void calculer_terme_destruction_K(const Zone_VEF& , const Zone_Cl_VEF& , DoubleVect& ,
                                    const DoubleTab& ,
                                    const DoubleVect&  , const DoubleVect&  ) const;


  void post_traitement_particulier_calcul_flux() ;
  void post_traitement_particulier_Richardson() ;
  void post_traitement_particulier_calcul_pression() ;
  DoubleTab R_loc ;
  DoubleTab C_trans ;
  DoubleVect r_int ;
  DoubleVect r_out ;
  DoubleVect delta_r ;
  DoubleVect  delta_teta ;
  int c_flux, nb ;
  int c_Richardson ;
  int c_pression;

};


#endif
