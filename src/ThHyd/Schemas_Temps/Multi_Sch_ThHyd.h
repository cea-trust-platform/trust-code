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
// File:        Multi_Sch_ThHyd.h
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Multi_Sch_ThHyd_included
#define Multi_Sch_ThHyd_included

#include <Schema_Temps.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Multi_Sch_ThHyd
// .SECTION voir aussi
//     Schema_Temps_base
//////////////////////////////////////////////////////////////////////////////
class Multi_Sch_ThHyd: public Schema_Temps_base
{

  Declare_instanciable(Multi_Sch_ThHyd);

public :

  ////////////////////////////////
  //                            //
  // Caracteristiques du schema //
  //                            //
  ////////////////////////////////

  virtual int nb_valeurs_temporelles() const;
  virtual int nb_valeurs_futures() const;
  virtual double temps_futur(int i) const;
  virtual double temps_defaut() const;

  /////////////////////////////////////////
  //                                     //
  // Fin des caracteristiques du schema  //
  //                                     //
  /////////////////////////////////////////

  virtual int faire_un_pas_de_temps_eqn_base(Equation_base&);
  virtual bool iterateTimeStep(bool& converged);
  //virtual void lire(const Motcle&, Entree&);
  virtual void set_param(Param& titi);
  virtual int mettre_a_jour();

  virtual bool corriger_dt_calcule(double&) const;
  virtual void completer();
  virtual void changer_temps_courant(const double&);
  virtual int stop() const;
  virtual void imprimer(Sortie&) const;
  /*   virtual int lsauv() const; */


private :
  int nb_ss_pas_dt;
  Schema_Temps sch_ns;
  Schema_Temps sch_scalaires;
};


#endif
