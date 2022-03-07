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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Solveur_Masse_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Solveur_Masse_base_included
#define Solveur_Masse_base_included
#include <Interface_blocs.h>
#include <MorEqn.h>

class Zone_dis;
class Zone_dis_base;
class Zone_Cl_dis;
class Zone_Cl_dis_base;
class DoubleTab;
#include <DoubleVect.h>
class Matrice_Base;
class Matrice_Morse;


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Solveur_Masse_base
//     Represente la matrice de masse d'une equation. Une equation
//     contient une reference a une matrice de masse.
//     Cette classe est la base de la hierarchie des matrices de masse.
// .SECTION voir aussi
//     SolveurMasse Equation_base
//     Classe abstraite.
//     Methodes abstraites:
//       DoubleTab& appliquer(DoubleTab&) const
//       void associer_zone_dis_base(const Zone_dis_base& )
//       void associer_zone_cl_dis_base(const Zone_Cl_dis_base& )
//////////////////////////////////////////////////////////////////////////////

class Solveur_Masse_base : public Objet_U, public MorEqn
{
  Declare_base(Solveur_Masse_base);

public :

  virtual DoubleTab& appliquer(DoubleTab&) const;

  virtual void mettre_a_jour(double temps);
  virtual void associer_zone_dis_base(const Zone_dis_base& ) =0;
  virtual void associer_zone_cl_dis_base(const Zone_Cl_dis_base& ) =0;
  virtual void assembler();

  virtual Matrice_Base& ajouter_masse(double dt, Matrice_Base& matrice, int penalisation=1) const;
  virtual DoubleTab& ajouter_masse(double dt, DoubleTab& x, const DoubleTab& y, int penalisation=1) const;
  virtual Matrice_Base& ajouter_masse_dt_local(DoubleVect& dt_locaux, Matrice_Base& matrice, int penalisation=1) const;
  virtual DoubleTab& ajouter_masse_dt_local(DoubleVect& dt_locaux, DoubleTab& x, const DoubleTab& y, int penalisation=1) const;
  virtual void get_masse_dt_local(DoubleVect& m_dt_locaux, DoubleVect& dt_locaux, int penalisation=1);
  virtual void get_masse_divide_by_local_dt(DoubleVect& m_dt_locaux, DoubleVect& dt_locaux, int penalisation=1);

  virtual DoubleTab& corriger_solution(DoubleTab& x, const DoubleTab& y, int incr = 0) const;
  void set_name_of_coefficient_temporel(const Nom&);
  int has_coefficient_temporel() const
  {
    return has_coefficient_temporel_;
  }
  const Nom& get_name_of_coefficient_temporel() const
  {
    return name_of_coefficient_temporel_;
  }

  // cette methode remplace la methode appliquer des version avant 154
  virtual DoubleTab& appliquer_impl(DoubleTab& x) const =0;

  // j'ajoute une methode dimensionner()
  // qui dimensionne la matrice pour le cas ou tous les operateurs sont negligeables
  virtual void dimensionner(Matrice_Morse& matrix) const;

  /* interface {dimensionner,ajouter}_blocs -> cf Equation_base.h */
  virtual int has_interface_blocs() const
  {
    return 0;
  };
  virtual void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const;
  virtual void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, double dt, const tabs_t& semi_impl, int resoudre_en_increments) const;

  // j'ajoute une methode completer()
  // qui est appelee par la methode completer() des equations.
  // L'implementation par defaut ne fait rien.
  virtual void completer(void);

  // j'ajoute une methode preparer_calcul()
  // qui est appelee par la methode preparer_calcul des equations
  // L'implementation par defaut ne fait rien
  virtual void preparer_calcul(void);
protected :
  Nom name_of_coefficient_temporel_; // nom du coefficient temporelle
  int has_coefficient_temporel_;
private:
  mutable double penalisation_;
};

#endif
