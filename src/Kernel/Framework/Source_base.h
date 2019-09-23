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
// File:        Source_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Source_base_included
#define Source_base_included

#include <MorEqn.h>
#include <DoubleVect.h>
#include <Champs_compris.h>
#include <Champs_compris_interface.h>
#include <Matrice_Bloc.h>

class DoubleTab;
class DoubleTabs;
class Probleme_base;
class Zone_dis;
class Zone_Cl_dis;
class Matrice_Morse;
class SFichier;

//////////////////////////////////////////////////////////////////////////////
// .DESCRIPTION
//     classe Source_base
//     Un objet Source_base est un terme apparaissant au second membre d'une
//     equation. Cette classe est la base de la hierarchie des Sources, une
//     source est un morceau d'equation donc Source_base herite de MorEqn.
//////////////////////////////////////////////////////////////////////////////

class Source_base : public Champs_compris_interface, public MorEqn, public Objet_U
{
  Declare_base(Source_base);

public :

  virtual DoubleTab& ajouter(DoubleTab& ) const=0;
  virtual DoubleTab& calculer(DoubleTab& ) const=0;
  virtual void mettre_a_jour(double temps);
  virtual void completer();
  virtual void dimensionner(Matrice_Morse&) const ;
  virtual void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const ;
  virtual void contribuer_au_second_membre(DoubleTab& ) const ;
  virtual int impr(Sortie& os) const;
  // temporaire : associer_zones sera rendue publique
  inline void associer_zones_public(const Zone_dis& ,const Zone_Cl_dis& );
  virtual int initialiser(double temps);
  virtual void associer_champ_rho(const Champ_base& champ_rho);
  virtual int a_pour_Champ_Fonc(const Motcle& mot, REF(Champ_base)& ch_ref) const;
  virtual void contribuer_jacobienne(Matrice_Bloc& , int ) const {}
  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

  void ouvrir_fichier(SFichier& os,const Nom&, const int& flag=1) const;
  void set_fichier(const Nom&);
  inline void set_description(const Nom& nom)
  {
    description_=nom;
  };
  inline const Nom fichier() const
  {
    return out_;
  };
  inline const Nom description() const
  {
    return description_;
  };

  inline DoubleVect& bilan()
  {
    return bilan_;
  };
  inline DoubleVect& bilan() const
  {
    return bilan_;
  };

protected :

  virtual void associer_zones(const Zone_dis& ,const Zone_Cl_dis& ) =0;
  virtual void associer_pb(const Probleme_base& ) =0;

  Nom out_;                  // Nom du fichier .out pour l'impression
  Nom description_;          // Description du terme source
  mutable DoubleVect bilan_; // Vecteur contenant les valeurs du terme source dans le domaine

  Champs_compris champs_compris_;

};

inline void Source_base::associer_zones_public(const Zone_dis& zdis, const Zone_Cl_dis& zcldis)
{
  associer_zones(zdis,zcldis);
}

#endif
