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
// File:        RK3.h
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef RK3_included
#define RK3_included





#include <Schema_Temps_base.h>
#include <Vect_Ref_Probleme_base.h>
class Pb_MG;
class Pb_2G;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe RK3
//     Cette classe represente un schema en temps de Runge Kutta d'ordre 3
//     (cas 7 de Williamson) s'ecrit :
//     q1=h f(x0)
//     x1=x0+b1 q1
//     q2=h f(x1) +a2 q1
//     x2=x1+b2 q2
//     q3=h f(x2)+a3 q2
//     x3=x2+b3 q3
//      avec a1=0, a2=-5/9, a3=-153/128
//                              b1=1/3, b2=15/16, b3=8/15
// .SECTION voir aussi
//     Schema_Temps_base
//////////////////////////////////////////////////////////////////////////////
class RK3: public Schema_Temps_base
{

  Declare_instanciable(RK3);

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
  inline void completer();
  //  virtual int faire_un_pas_de_temps_pb_couple(Probleme_Couple&);


  //// COMMENTE POUR PASSAGE A LA V1.4.4
  ////
  //  virtual int faire_un_pas_de_temps_pb_mg(Pb_MG&);

private:

  int  trouver_tous_les_pbs(Pb_MG& , VECT(REF(Probleme_base))& );
  void  associer_pb_fins(int , Pb_MG& , VECT(REF(Probleme_base))& );
  void  nb_pb_fins(int& , Pb_MG& );



};

inline void RK3::completer()
{
}


#endif
