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
// File:        Traitement_particulier_NS_VEF.h
// Directory:   $TRUST_ROOT/src/VEF
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_NS_VEF_included
#define Traitement_particulier_NS_VEF_included

#include <Traitement_particulier_NS_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_VEF
//     Cette classe permet de ne faire aucyun traitement particulier
//     (comme avant!!!)
//     Classe utilisee par defaut??
//
// .SECTION voir aussi
//      Navier_Stokes_std, Traitement_particulier_base
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_NS_VEF : public Traitement_particulier_NS_base
{
  Declare_instanciable(Traitement_particulier_NS_VEF);

public :

  void associer_eqn(const Equation_base& );
  virtual inline void preparer_calcul_particulier(void) ;
  virtual inline void traitement_particulier_debut_mettre_a_jour(double&);
  virtual inline void traitement_particulier_fin_mettre_a_jour(double&) ;
  virtual inline void sauver_stat(void) const;
  virtual inline void reprendre_stat(void) ;
  virtual inline void post_traitement_particulier(void) ;
  virtual void en_cours_de_resolution(int , DoubleTab& ,DoubleTab& ,double) ;
  virtual Entree& lire(Entree& );
  virtual int a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const ;
  virtual int comprend_champ(const Motcle& mot) const ;

  /////////////////////////////////////////////////////

protected :


};

inline void Traitement_particulier_NS_VEF::preparer_calcul_particulier()
{
  ;
}

inline void Traitement_particulier_NS_VEF::traitement_particulier_debut_mettre_a_jour(double& temps)
{
  ;
}

inline void Traitement_particulier_NS_VEF::traitement_particulier_fin_mettre_a_jour(double& temps)
{
  ;
}

inline void Traitement_particulier_NS_VEF::post_traitement_particulier()
{
  ;
}

inline void Traitement_particulier_NS_VEF::sauver_stat() const
{
  ;
}

inline void Traitement_particulier_NS_VEF::reprendre_stat()
{
  ;
}

inline void Traitement_particulier_NS_VEF::en_cours_de_resolution(int nb_op, DoubleTab& v,DoubleTab& u, double dt)
{
  ;
}

inline int Traitement_particulier_NS_VEF::a_pour_Champ_Fonc(const Motcle& mot,
                                                            REF(Champ_base)& ch_ref) const
{
  return 0 ;
}

inline int Traitement_particulier_NS_VEF::comprend_champ(const Motcle& mot) const
{
  return 0 ;
}

#endif
