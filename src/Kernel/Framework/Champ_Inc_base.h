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
// File:        Champ_Inc_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_Inc_base_included
#define Champ_Inc_base_included

#include <Champ_base.h>
#include <Roue.h>
#include <MorEqn.h>
class DoubleTab;
class Frontiere_dis_base;
class MD_Vector;
class Zone_dis;
class Zone_dis_base;
class Domaine;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     Classe Champ_Inc_base
//     Classe de base des champs inconnues qui sont les champs calcules
//     par une equation.
//     Un objet de type Roue est associe au Champ_Inc, cette roue permet
//     de gerer le nombre de valeurs du temps pour lesquels le champ
//     doit rester en memoire. C'est le schema en temps qui guide le
//     nombre de valeurs a garder.
//     Champ_Inc est un morceaux d'equation car il herite de MorEqn.
// .SECTION voir aussi
//      MorEqn Champ_Inc Champ_base Ch_proto Equation_base
//     Classe abstraite
//     Methodes abstraites:
//       const Zone_dis_base& associer_zone_dis_base(const Zone_dis_base&)
//       const Zone_dis_base& zone_dis_base() const
//       DoubleTab& remplir_coord_noeuds(DoubleTab& ) const
//////////////////////////////////////////////////////////////////////////////
class Champ_Inc_base : public Champ_base, public MorEqn
{
  Declare_base(Champ_Inc_base);

public:
  //
  // Methode reimplementees
  //
  int fixer_nb_valeurs_nodales(int );
  double changer_temps(const double& temps);
  void   mettre_a_jour(double temps);
  int reprendre(Entree& ) ;
  int sauvegarder(Sortie& ) const;
  Champ_base& affecter_compo(const Champ_base& , int compo) ;
  //
  // Methodes viruelles pures implementees ici
  //
  Champ_base&      affecter_(const Champ_base& ) ;
  virtual void verifie_valeurs_cl();
  DoubleTab&       valeurs() ;
  const DoubleTab& valeurs() const ;

  virtual DoubleVect& valeur_a(const DoubleVect& position,
                               DoubleVect& valeurs) const ;

  virtual DoubleTab&  valeur_aux(const DoubleTab& positions,
                                 DoubleTab& valeurs) const ;
  virtual DoubleVect& valeur_aux_compo(const DoubleTab& positions,
                                       DoubleVect& valeurs, int ncomp) const ;

  //
  // Nouvelles methodes
  //
  int  nb_valeurs_nodales() const;
  virtual int fixer_nb_valeurs_temporelles(int );
  virtual int nb_valeurs_temporelles() const;
  double         changer_temps_futur(const double&, int i=1);
  double         changer_temps_passe(const double&, int i=1);
  double         recuperer_temps_futur(int i=1) const;
  double         recuperer_temps_passe(int i=1) const;
  DoubleTab&              valeurs(double temps) ;
  const DoubleTab&        valeurs(double temps) const ;
  // Operateurs de conversion implicite
  operator DoubleTab& ();
  operator const DoubleTab& () const ;

  DoubleTab&       futur(int i=1) ;
  const DoubleTab& futur(int i=1) const ;
  DoubleTab&       passe(int i=1) ;
  const DoubleTab& passe(int i=1) const ;
  Champ_Inc_base&  avancer(int i=1);
  Champ_Inc_base&  reculer(int i=1);

  int lire_donnees(Entree& ) ;

  void associer_eqn(const Equation_base&);
  virtual int imprime(Sortie& , int ) const;
  virtual int a_une_zone_dis_base() const
  {
    return 1;
  };

  DoubleTab& trace(const Frontiere_dis_base&, DoubleTab&) const;
  virtual DoubleTab& trace(const Frontiere_dis_base&, DoubleTab& , double ) const;
  virtual int     remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const;
  virtual int     remplir_coord_noeuds_et_polys_compo(DoubleTab&, IntVect&, int) const;
  virtual DoubleTab& remplir_coord_noeuds(DoubleTab& ) const=0;
  virtual DoubleTab& remplir_coord_noeuds_compo(DoubleTab&, int ) const;

  virtual double integrale_espace(int ncomp) const;
  const Domaine& domaine() const;

  // Obsolete method: signature changed in order to generate a compiler error if old code is not removed
  virtual void creer_espace_distant(int dummy) { };
protected:
  // Par defaut on initialise les valeurs a zero
  virtual void creer_tableau_distribue(const MD_Vector&,
                                       Array_base::Resize_Options = Array_base::COPY_INIT);

  Roue_ptr les_valeurs ;

};

#endif

