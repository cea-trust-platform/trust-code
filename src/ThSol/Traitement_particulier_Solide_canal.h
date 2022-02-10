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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Traitement_particulier_Solide_canal.h
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_Solide_canal_included
#define Traitement_particulier_Solide_canal_included

#include <Traitement_particulier_Solide_base.h>

class Conduction;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_Solide_canal
//
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_Solide_canal : public Traitement_particulier_Solide_base
{
  Declare_base(Traitement_particulier_Solide_canal);

public :

  void preparer_calcul_particulier(void) override ;
  void post_traitement_particulier(void) override =0 ;
  Entree& lire(Entree& ) override;
  inline void en_cours_de_resolution(int , DoubleTab&, DoubleTab& ,double) override ;
  inline int a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const  ;
  inline int comprend_champ(const Motcle& mot) const  ;

protected :
  void init_calcul_stats(void);
  virtual void init_calcul_moyenne(void) = 0;

  DoubleVect Y;
  DoubleVect Tmoy_temp;
  DoubleVect T2_moy_temp;
  /*    DoubleVect val_moy_temp,val_moy_Temp_temp;*/
  IntVect corresp, compt;
  double temps_deb,temps_fin;
  double dt_post_stat,dt_post_inst;
  int N;
};


inline int Traitement_particulier_Solide_canal::a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const
{
  return 0 ;
}

inline int Traitement_particulier_Solide_canal::comprend_champ(const Motcle& mot) const
{
  return 0 ;
}

inline void Traitement_particulier_Solide_canal::en_cours_de_resolution(int i, DoubleTab& a, DoubleTab& b,double c)
{
  ;
}
#endif
