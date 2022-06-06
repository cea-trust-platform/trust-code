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

#ifndef Terme_Puissance_Thermique_included
#define Terme_Puissance_Thermique_included


#include <Ref_Champ_base.h>
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

  void lire_donnees(Entree&,const Equation_base& eqn);
  void mettre_a_jour(double temps);
  void modify_name_file(Nom& ) const;
  const DoubleTab& puissance_thermique() const
  {
    return la_puissance->valeurs();
  }
  void initialiser_champ_puissance(const Equation_base& eqn);

protected:
  Champ_Don la_puissance_lu, la_puissance;

};
#endif
