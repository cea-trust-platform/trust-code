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
// File:        Terme_Source_Constituant.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Terme_Source_Constituant_included
#define Terme_Source_Constituant_included

#include <Ref_Champ_base.h>
#include <Champ_Don.h>
#include <SFichier.h>
#include <Equation_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Terme_Source_Constituant
//    Cette classe represente un terme source de l'equation de transport des constituants
//    de type degagement volumique de puissance thermique.!!!!A MODIFIER
//    Un objet Terme_Source_Constituant contient la puissance (Champ donne
//    utilisateur) et des references a la masse volumique (rho) et la chaleur.
//    specifique (Cp).
// .SECTION voir aussi
//    Classe non instanciable.
//    L'implementation des termes dependra de leur discretisation.
//////////////////////////////////////////////////////////////////////////////
class Terme_Source_Constituant
{

public :

  inline void associer_champs(const Champ_base&);
  void lire_donnees(Entree& );
  void ouvrir_fichier(const Equation_base& eq, const Nom& out, const Nom& qsj, const Nom& description, SFichier& os,const Nom& type, const int& flag) const;
  int completer(const Champ_Inc_base& inco);

  inline const Champ_Don& get_source() const
  {
    return la_source_constituant ;
  };

  void mettre_a_jour(double temps)
  {
    la_source_constituant.mettre_a_jour(temps);
  };

protected:
  int colw_;
  REF(Champ_base) rho_ref;
  Champ_Don la_source_constituant;

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
/*
  inline void Terme_Source_Constituant::associer_champs(const Champ_base& rho,
  const Champ_Don& cp)
  {
  rho_ref=rho;
  Cp=cp;
  }
*/
inline void Terme_Source_Constituant::associer_champs(const Champ_base& rho)
{
  rho_ref=rho;
}

#endif
