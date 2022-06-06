/****************************************************************************
* Copyright (c) 2022, CEA
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


#ifndef Traitement_particulier_NS_EC_included
#define Traitement_particulier_NS_EC_included

#include <Traitement_particulier_NS_base.h>
#include <Champ_Don.h>
#include <SFichier.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_EC
//     Cette classe permet de faire les traitements particuliers
//     pour les calculs de  l'energie cinetique totale
//
// .SECTION voir aussi
//      Navier_Stokes_Turbulent, Traitement_particulier_base,
//      Traitement_particulier_VEF
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_EC : public Traitement_particulier_NS_base
{
  Declare_base_sans_constructeur_ni_destructeur(Traitement_particulier_NS_EC);

public :
  inline ~Traitement_particulier_NS_EC() override;
  inline Traitement_particulier_NS_EC();
  Entree& lire(Entree& ) override;
  void preparer_calcul_particulier(void) override ;
  void post_traitement_particulier(void) override ;
  inline void en_cours_de_resolution(int , DoubleTab&, DoubleTab& ,double) override ;
  inline void sauver_stat(void)  const override ;
  inline void reprendre_stat(void) override  ;
  inline int a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const  ;
  inline int comprend_champ(const Motcle& mot) const  ;

protected :
  void ouvrir_fichier(SFichier& s) const;
  virtual void calculer_Ec(double&);
  double periode,tinit,nb_bip;
  int repere_mobile_;
};

#endif

inline void Traitement_particulier_NS_EC::en_cours_de_resolution(int , DoubleTab&, DoubleTab& ,double)
{
}

inline void Traitement_particulier_NS_EC::sauver_stat(void)  const
{
  ;
}

inline void Traitement_particulier_NS_EC::reprendre_stat(void)
{
  ;
}

inline int Traitement_particulier_NS_EC::a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const
{
  return 0 ;
}

inline int Traitement_particulier_NS_EC::comprend_champ(const Motcle& mot) const
{
  return 0 ;
}
inline Traitement_particulier_NS_EC::Traitement_particulier_NS_EC():repere_mobile_(0)
{
}
inline Traitement_particulier_NS_EC::~Traitement_particulier_NS_EC() {}
