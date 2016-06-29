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
// File:        Neumann_sortie_libre.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Neumann_sortie_libre_included
#define Neumann_sortie_libre_included


#include <Neumann.h>
#include <Ref_Champ_Inc.h>


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
class Neumann_sortie_libre : public Neumann
{

  Declare_instanciable(Neumann_sortie_libre);

public:

  virtual const DoubleTab& tab_ext() const;
  virtual DoubleTab& tab_ext();

  virtual double val_ext(int i) const;
  virtual double val_ext(int i,int j) const;
  virtual int compatible_avec_eqn(const Equation_base&) const;
  virtual int initialiser(double temps);
  virtual void associer_fr_dis_base(const Frontiere_dis_base& ) ;
  void verifie_ch_init_nb_comp();

  virtual void         fixer_nb_valeurs_temporelles (int nb_cases);
  virtual void mettre_a_jour(double temps);
  virtual void set_temps_defaut(double temps);
  virtual void changer_temps_futur(double temps, int i);
  virtual int avancer(double temps);
  virtual int reculer(double temps);

protected:

  Champ_front le_champ_ext;

};


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Sortie_libre_pression_imposee
//    Cette classe derive de Neumann_sortie_libre
//    Elle represente une frontiere ouverte avec condition de pression imposee.
//    L'objet de type Champ_bord le_champ_bord contient la pression et la
//    fonction flux_impose() renvoie les valeurs de cette pression.
//    champ_ext contient une valeur de la vitesse du fluide a l'exterieur
//    accessible par la methode val_ext()
// .SECTION voir aussi
//     Neumann_sortie_libre
//////////////////////////////////////////////////////////////////////////////
class Sortie_libre_pression_imposee : public Neumann_sortie_libre
{

  Declare_instanciable(Sortie_libre_pression_imposee);

public :

  int compatible_avec_eqn(const Equation_base&) const;
  double flux_impose(int i) const;
  double flux_impose(int i,int j) const;
  void completer();

protected:

  double d_rho;
};



#endif
