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
// File:        Schema_Euler_Implicite.h
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Schema_Euler_Implicite_included
#define Schema_Euler_Implicite_included

//////////////////////////////////////////////////////////
// .DESCRIPTION class Schema_Euler_Implicite
//  Il herite de schema implicite base et porte un solveur par exemple
//  le Simpler pour effectuer les Faire_un_pas_de_temps..

#include <Schema_Implicite_base.h>
#include <vector>
#include <set>
class Probleme_Couple;

class Schema_Euler_Implicite : public Schema_Implicite_base
{

  Declare_instanciable(Schema_Euler_Implicite);

public :

  bool initTimeStep(double dt) override;
  void set_param(Param& ) override;
  int lire_motcle_non_standard(const Motcle& mot, Entree& is) override;

  ////////////////////////////////
  //                            //
  // Caracteristiques du schema //
  //                            //
  ////////////////////////////////

  int nb_valeurs_temporelles() const override;
  int nb_valeurs_futures() const override;
  double temps_futur(int i) const override;
  double temps_defaut() const override;

  /////////////////////////////////////////
  //                                     //
  // Fin des caracteristiques du schema  //
  //                                     //
  /////////////////////////////////////////

  void Initialiser_Champs(Probleme_base&);
  void test_stationnaire(Probleme_base&);
  int Iterer_Pb(Probleme_base&,int ite, int& ok);
  bool iterateTimeStep(bool& converged) override;
  virtual int faire_un_pas_de_temps_pb_couple(Probleme_Couple&, int& ok);
  int faire_un_pas_de_temps_eqn_base(Equation_base&) override;
  inline const double& residu_old() const
  {
    return residu_old_ ;
  };
  int mettre_a_jour() override;
  int reprendre(Entree& ) override;
  inline void completer(void) override { } ;
  int resolution_monolithique(const Nom& nom) const;
protected:
  int nb_ite_max;
  double residu_old_,facsec_max_;
  int nb_ite_sans_accel_;
  std::vector<std::set<std::string>> resolution_monolithique_;
};

#endif

