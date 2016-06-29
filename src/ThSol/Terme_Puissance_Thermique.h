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
// File:        Terme_Puissance_Thermique.h
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Terme_Puissance_Thermique_included
#define Terme_Puissance_Thermique_included


#include <Ref_Champ_Don.h>
#include <Champ_Don.h>

class Probleme_base;
class Zone_dis_base;
class Zone_Cl_dis_base;
class Equation_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Terme_Puissance_Thermique
//    Cette classe represente un terme source de l'equation de la thermique
//    du type degagement volumique de puissance thermique.
//    Un objet Terme_Puissance_Thermique contient la puissance (Champ donne
//    utilisateur) et des references a la masse volumique (rho) et la chaleur.
//    specifique (Cp).
// .SECTION voir aussi
//    Classe non instanciable.
//    L'implementation des termes dependra de leur discretisation.
//////////////////////////////////////////////////////////////////////////////
class Terme_Puissance_Thermique
{

public :

  void preparer_source(const Probleme_base& pb);
  inline void associer_champs(const Champ_Don& , const Champ_Don&);
  void lire_donnees(Entree&,const Equation_base& eqn);
  void mettre_a_jour(double temps)
  {
    la_puissance.mettre_a_jour(temps);
  };
  void modify_name_file(Nom& ) const;
protected:

  REF(Champ_Don) rho_ref;
  REF(Champ_Don) Cp;
  Champ_Don la_puissance;

};


// Description:
//    Associe les champs donnes rho (masse volumique)
//    et Cp (chaleur specifique) a l'objet.
// Precondition:
// Parametre: Champ_Don& rho
//    Signification: champ donne representant la masse volumique
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: Champ_Don& cp
//    Signification: champ donne representant la chaleur specifique
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le terme de puissance thermique a une masse volumique
//                et une chaleur specifique associee
inline void Terme_Puissance_Thermique::associer_champs(const Champ_Don& rho,
                                                       const Champ_Don& cp)
{
  rho_ref=rho;
  Cp=cp;
}

#endif
