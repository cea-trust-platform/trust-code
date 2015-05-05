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
// File:        Navier_Stokes_Turbulent.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Navier_Stokes_Turbulent_included
#define Navier_Stokes_Turbulent_included

#include <Navier_Stokes_std.h>
#include <Mod_turb_hyd.h>
class Champ_Fonc;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Navier_Stokes_Turbulent
//     Cette classe represente l'equation de la dynamique pour un fluide
//     visqueux verifiant la condition d'incompressibilite div U = 0 avec
//     modelisation de la turbulence.
//     Un membre de type Mod_turb_hyd representera le modele de turbulence.
// .SECTION voir aussi
//      Navier_Stokes_std Mod_turb_hyd Pb_Hydraulique_Turbulent
//      Pb_Thermohydraulique_Turbulent
//////////////////////////////////////////////////////////////////////////////
class Navier_Stokes_Turbulent : public Navier_Stokes_std
{
  Declare_instanciable(Navier_Stokes_Turbulent);

public :

  void set_param(Param& titi);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  inline const Champ_Fonc& viscosite_turbulente() const;
  inline const Mod_turb_hyd& modele_turbulence() const;
  int sauvegarder(Sortie&) const;
  int reprendre(Entree&);
  virtual int preparer_calcul();
  virtual bool initTimeStep(double dt);
  void mettre_a_jour(double );
  virtual void completer();
  const Champ_Don& diffusivite_pour_transport();
  const Champ_base& diffusivite_pour_pas_de_temps();

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////

  void imprimer(Sortie& ) const;
  const RefObjU& get_modele(Type_modele type) const;
  virtual void imprime_residu(SFichier&);
  virtual Nom expression_residu();

protected:
  Entree& lire_op_diff_turbulent(Entree& is);

  Mod_turb_hyd le_modele_turbulence;


};



// Description:
//    Renvoie le champ representant la viscosite turbulente.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Fonc&
//    Signification: le champ representant la viscosite turbulente
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Fonc& Navier_Stokes_Turbulent::viscosite_turbulente() const
{
  return le_modele_turbulence.viscosite_turbulente();
}


// Description:
//    Renvoie le modele de turbulence (Hydraulique) associe a l'equation.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Mod_turb_hyd&
//    Signification: le modele de turbulence (Hydraulique) associe a l'equation
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Mod_turb_hyd& Navier_Stokes_Turbulent::modele_turbulence() const
{
  return le_modele_turbulence;
}


#endif
