/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Conduction_Milieu_Variable.h
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Conduction_Milieu_Variable_included
#define Conduction_Milieu_Variable_included

#include <Equation_base.h>
#include <Operateur_Diff.h>
#include <Schema_Temps.h>
#include <Ref_Solide.h>
#include <Traitement_particulier_Solide.h>

class Milieu_base;
class Champ_Don;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Conduction_Milieu_Variable
//    Cette classe represente l'equation d'evolution
//    de la temperature dans un solide de conductivite k et de masse volumique rho et chaleur specifique Cp
//        (rho.Cp) . dT/dt - div (k grad T) = (rho.Cp) . f
//    A la difference de Conduction, les termes rho et Cp peuvent etre non uniformes car cette classe
//    ne fait pas le raccourci de calculer la diffusivite k/(rho*Cp).
//    La principale modification est dans lire_motcle_non_standard() et diffusivite_pour_transport()
//
// .SECTION voir aussi
//     Equation_base
//     Conduction
//////////////////////////////////////////////////////////////////////////////
class Conduction_Milieu_Variable : public Equation_base
{

  Declare_instanciable_sans_constructeur(Conduction_Milieu_Variable);

public:

  Conduction_Milieu_Variable();
  void set_param(Param&);
  int lire_motcle_non_standard(const Motcle&, Entree&);
  inline void associer_solide(const Solide& );
  void associer_milieu_base(const Milieu_base& );
  const Milieu_base& milieu() const;
  Milieu_base& milieu();
  const Solide& solide() const;
  Solide& solide();
  int nombre_d_operateurs() const;
  const Operateur& operateur(int) const;
  Operateur& operateur(int);
  inline const Champ_Inc& inconnue() const;
  inline Champ_Inc& inconnue();
  void discretiser();
  int impr(Sortie& os) const;

  virtual const Motcle& domaine_application() const;
  virtual void mettre_a_jour(double temps);
  virtual const Champ_Don& diffusivite_pour_transport();
  virtual const Champ_base& diffusivite_pour_pas_de_temps();

  //Methodes de l interface des champs postraitables
  /////////////////////////////////////////////////////
  //Methode creer_champ pas codee a surcharger si necessaire
  virtual void creer_champ(const Motcle& motlu);
  virtual const Champ_base& get_champ(const Motcle& nom) const;
  virtual void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const;
  /////////////////////////////////////////////////////
private :

  REF(Solide) le_solide;
  Champ_Inc la_temperature;
  Operateur_Diff terme_diffusif;

protected :

  Traitement_particulier_Solide le_traitement_particulier;
//  Champ_Fonc temperature_paroi;


};


// Description:
//    Associe le milieu solide a l'equation.
// Precondition:
// Parametre: Solide& un_solide
//    Signification: le milieu solide a associer a l'equation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: l'equation a un milieu physique associe
inline void Conduction_Milieu_Variable::associer_solide(const Solide& un_solide)
{
  le_solide = un_solide;
}


// Description:
//    Renvoie le champ inconnue de l'equation,
//    i.e. la temperature.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue de l'equation: la temperature
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_Inc& Conduction_Milieu_Variable::inconnue()
{
  return la_temperature;
}


// Description:
//    Renvoie le champ inconnue de l'equation,
//    i.e. la temperature.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconnue de l'equation: la temperature
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_Inc& Conduction_Milieu_Variable::inconnue() const
{
  return la_temperature;
}



#endif


