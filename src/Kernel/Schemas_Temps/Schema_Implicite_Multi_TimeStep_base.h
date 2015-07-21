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
// File:        Schema_Implicite_Multi_TimeStep_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Schema_Implicite_Multi_TimeStep_base_included
#define Schema_Implicite_Multi_TimeStep_base_included

//////////////////////////////////////////////////////////
// .DESCRIPTION class Schema_Implicite_Multi_TimeStep_base
//  Il herite de schema implicite base et porte un solveur par exemple
//  le Simpler pour effectuer les Faire_un_pas_de_temps..

#include <Schema_Implicite_base.h>
class Probleme_Couple;

class Schema_Implicite_Multi_TimeStep_base : public Schema_Implicite_base
{

  Declare_base(Schema_Implicite_Multi_TimeStep_base);

public :

  virtual bool initTimeStep(double dt);
  void set_param(Param& );

  ////////////////////////////////
  //                            //
  // Caracteristiques du schema //
  //                            //
  ////////////////////////////////

  int nb_valeurs_futures() const;
  double temps_futur(int i) const;
  double temps_defaut() const;
  inline  int nb_valeurs_temporelles_effectives() const;

  /////////////////////////////////////////
  //                                     //
  // Fin des caracteristiques du schema  //
  //                                     //
  /////////////////////////////////////////

  virtual double            changer_temps(Equation_base& eqn, const double& temps)                                              = 0;
  virtual void              update_time_derivative(Equation_base& eqn, const DoubleTab& data)                                   = 0;
  virtual void              mettre_a_jour_equation(Equation_base& eqn, const double& temps)                                     = 0;
  virtual void              modify_equation_parameters(Equation_base& eqn, DoubleTab& stored_parameters)                        = 0;
  virtual void              store_equation_parameters(Equation_base& eqn, DoubleTab& stored_parameters)                         = 0;
  virtual DoubleTab&        coefficients()                                                                                      = 0;
  virtual void              compute_coefficients(double  time_step, const DoubleTab& times)                               const = 0;
  virtual void              add_multi_timestep_data(const Equation_base& eqn, Matrice_Base& mat_morse, DoubleTab& secmem) const = 0;
  virtual int               nb_pas_dt_seuil()                                                                             const = 0;
  virtual int               nb_valeurs_passees()                                                                          const = 0;
  virtual const DoubleTab&  coefficients()                                                                                const = 0;


  virtual int faire_un_pas_de_temps_pb_couple(Probleme_Couple&);

  bool   iterateTimeStep(bool& converged);
  int faire_un_pas_de_temps_eqn_base(Equation_base&);
  int mettre_a_jour();
  int reprendre(Entree& );
  inline void completer(void) { } ;

  void   Initialiser_Champs(Probleme_base&);
  void   test_stationnaire(Probleme_base&);
  int Iterer_Pb(Probleme_base&,int ite);
  inline const double& residu_old() const
  {
    return residu_old_ ;
  };

  void ajouter_inertie(Matrice_Base& mat_morse,DoubleTab& secmem,const Equation_base& eqn) const;

  //pour les schemas en temps a pas multiples
  virtual void modifier_second_membre_full_impl(const Equation_base& eqn, DoubleTab& secmem);
  virtual void modifier_second_membre(const Equation_base& eqn, DoubleTab& secmem);

protected:
  void authorized_equation(const Equation_base& eqn);
  void authorized_equation(const Equation_base& eqn) const;

  int nb_ite_max;
  double residu_old_,facsec_max_;
  int nb_ite_sans_accel_;
};

inline int Schema_Implicite_Multi_TimeStep_base::nb_valeurs_temporelles_effectives() const
{
  return nb_valeurs_temporelles()-1;
}
#endif

