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

#ifndef Neumann_sortie_libre_included
#define Neumann_sortie_libre_included

#include <Neumann_val_ext.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe  Neumann_sortie_libre
//    Cette classe represente une frontiere ouverte sans vitesse imposee
//    Pour les equations de Navier_Stokes on impose necessairement
//    la pression sur une telle frontiere
//    Pour traiter l'hydraulique, on derive donc de la classe
//    Neumann_sortie_libre la classe Sortie_libre_pression_imposee
//    Les conditions aux limites de type Neumann_sortie_libre ou des
//    types derives se traduisent par des flux diffusifs nuls.
//    En revanche, le traitement des flux convectifs impose de connaitre
//    le champ convecte a l'exterieur de la frontiere en cas de re-entree
//    de fluide. C'est pourquoi la classe porte un Champ_front
//    (membre le_champ_ext).
//    Dans les operateurs de calcul, les conditions aux limites
//    de type Neumann_sortie_libre et des types derives seront traites
//    de maniere identique
// .SECTION voir aussi
//     Neumann Sortie_libre_pression_imposee
//////////////////////////////////////////////////////////////////////////////
class Neumann_sortie_libre : public Neumann_val_ext
{

  Declare_instanciable(Neumann_sortie_libre);

public:

  const DoubleTab& tab_ext() const override;
  DoubleTab& tab_ext() override;

  double val_ext(int i) const override;
  double val_ext(int i,int j) const override;
  int compatible_avec_eqn(const Equation_base&) const override;
  int initialiser(double temps) override;
  void associer_fr_dis_base(const Frontiere_dis_base& ) override ;
  void verifie_ch_init_nb_comp() const override;

  void fixer_nb_valeurs_temporelles (int nb_cases) override;
  void mettre_a_jour(double temps) override;
  void set_temps_defaut(double temps) override;
  void changer_temps_futur(double temps, int i) override;
  int avancer(double temps) override;
  int reculer(double temps) override;

protected:

  Champ_front le_champ_ext;

};

#endif
