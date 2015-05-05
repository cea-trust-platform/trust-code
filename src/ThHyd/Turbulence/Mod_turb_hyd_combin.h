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
// File:        Mod_turb_hyd_combin.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Mod_turb_hyd_combin_included
#define Mod_turb_hyd_combin_included

#include <Mod_turb_hyd_base.h>
#include <Vect_Parser_U.h>

class IntTab;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Mod_turb_hyd_combin
//    Classe representant un modele de turbulence exprime a partir d'une combinaison de champs
//    et (ou) des variables d espace et de temps pour l equation de Navier Stokes
//    La syntaxe a respecter est la suivante :
//    Modele_turbulence combinaison
//                  {
//                     nb_var nb_sources_a_specif
//                     fonction ...
//                     turbulence_paroi ...
//                  }
//  -Mot cle nb_var :
//   nb_sources_a_specif est a preciser par l utilisateur : correspond aux nombre
//  de champs sources qui interviendront dans l expression du modele (0 par defaut)
//  -Mot cle fonction :
//   Une expression doit etre specifiee par l utilisateur f(x,y,z,t,nom_champs_sources)
//  -Mot cle turbulence_paroi :
//   Choix d une loi de paroi (eventuellement negligeable) a preciser par l utilisateur
//
// .SECTION voir aussi
//    Mod_turb_hyd_base
//////////////////////////////////////////////////////////////////////////////
class Mod_turb_hyd_combin : public Mod_turb_hyd_base
{
  Declare_instanciable_sans_constructeur(Mod_turb_hyd_combin);

public:

  Mod_turb_hyd_combin();
  void set_param(Param& param);
  virtual void discretiser();
  virtual void completer();
  virtual void mettre_a_jour(double );
  int preparer_calcul();
  virtual Champ_Fonc& calculer_viscosite_turbulente();
  inline virtual Champ_Fonc& energie_cinetique_turbulente();
  inline virtual const Champ_Fonc& energie_cinetique_turbulente() const;
  inline int nombre_sources();

protected:

  Champ_Fonc energie_cinetique_turb_;

  Nom la_fct; //Contient l expression de la combinaison
  int nb_var_;
  Noms les_var;
  mutable VECT(Parser_U) fxyz; //Parser utilise pour evaluer la valeur prise par la combinaison

private :

};
inline int Mod_turb_hyd_combin::nombre_sources()
{
  return nb_var_;
}
inline Champ_Fonc& Mod_turb_hyd_combin::energie_cinetique_turbulente()
{
  return energie_cinetique_turb_;
}

inline const Champ_Fonc& Mod_turb_hyd_combin::energie_cinetique_turbulente() const
{
  return energie_cinetique_turb_;
}

#endif
