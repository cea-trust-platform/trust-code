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
// File:        Traitement_particulier_NS_THI.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_THI_included
#define Traitement_particulier_NS_THI_included

#include <Traitement_particulier_NS_base.h>
#include <IntTab.h>
#include <Champ_Don.h>
#include <Zone_VF.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_THI
//     Cette classe permet de faire les traitements particuliers
//     pour les calculs de THI, en particulier initialisation
//     particuliere et calculs des spectres!!
//
// .SECTION voir aussi
//      Navier_Stokes_Turbulent, Traitement_particulier_base,
//      Traitement_particulier_VEF
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_THI : public Traitement_particulier_NS_base
{
  Declare_base(Traitement_particulier_NS_THI) ;

public :

  Entree& lire(Entree& );
  virtual void preparer_calcul_particulier(void) ;
  virtual void post_traitement_particulier(void) ;
  void en_cours_de_resolution(int , DoubleTab&, DoubleTab& ,double) ;
  inline void sauver_stat(void)  const ;
  inline void reprendre_stat(void)  ;
  inline int a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const  ;
  inline int comprend_champ(const Motcle& mot) const  ;

protected :

  virtual void renorm_Ec(void) =0;
  virtual void calcul_spectre_operateur(int , DoubleTab&, DoubleTab& ,double) =0;
  virtual void init_calc_spectre(void)=0;
  virtual void calcul_spectre(void)=0;
  virtual void sorties_globales(void)=0;
  int& calcul_nb_som_dir(const Zone&);
  int& calcul_nb_elem_dir(const Zone&);
  void msg_erreur_maillage(const char*);

  int init,oui_transf,oui_calc_spectre;
  double Ec_init;
  int nb_som_dir;
  int nb_elem_dir;
  int fac_init;
};

inline void Traitement_particulier_NS_THI::sauver_stat(void)  const
{
  ;
}

inline void Traitement_particulier_NS_THI::reprendre_stat(void)
{
  ;
}

inline int Traitement_particulier_NS_THI::a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const
{
  return 0 ;
}

inline int Traitement_particulier_NS_THI::comprend_champ(const Motcle& mot) const
{
  return 0 ;
}

#endif
