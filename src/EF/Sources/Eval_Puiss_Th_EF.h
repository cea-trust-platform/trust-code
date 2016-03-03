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
// File:        Eval_Puiss_Th_EF.h
// Directory:   $TRUST_ROOT/src/EF/Sources
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Puiss_Th_EF_included
#define Eval_Puiss_Th_EF_included

#include <Evaluateur_Source_EF_Som.h>
#include <Ref_Champ_Don.h>
#include <DoubleTab.h>
#include <Champ_Uniforme.h>
#include <Champ_Don.h>



////////////////////////////////////////////////////////////////////////////
//
//  CLASS Eval_Puiss_Th_EF
//
////////////////////////////////////////////////////////////////////////////

class Eval_Puiss_Th_EF: public Evaluateur_Source_EF_Som
{

public:

  inline Eval_Puiss_Th_EF();
  void completer();
  void associer_champs(const Champ_Don& ,const Champ_Don& , const Champ_Don& );
  void mettre_a_jour();
  inline double calculer_terme_source_standard(int ) const ;
  inline void calculer_terme_source_standard(int , DoubleVect&  ) const ;

protected:

  REF(Champ_Don) rho_ref;
  double rho_ref_;
  REF(Champ_Don) Cp;
  double Cp_;
  REF(Champ_Don) la_puissance;
  DoubleTab puissance;

};


//
//   Fonctions inline de la classe Eval_Puiss_Th_EF
//

inline Eval_Puiss_Th_EF::Eval_Puiss_Th_EF():rho_ref_(-123.),Cp_(-123.)  {}

inline void Eval_Puiss_Th_EF::calculer_terme_source_standard(int num_elem, DoubleVect& d) const
{
  Cerr<<"Non code"<<__FILE__<<(int)__LINE__<<finl;
  Process::exit();
  throw;
  return calculer_terme_source_standard(num_elem,d);
}

inline double Eval_Puiss_Th_EF::calculer_terme_source_standard(int num_elem) const
{
  assert(!est_egal(Cp_,-123.));
  assert(!est_egal(rho_ref_,-123.));
  double source;
  if (same_type(Champ_Uniforme,la_puissance.valeur().valeur()))
    source = puissance(0,0) / (Cp_*rho_ref_);
  else
    {
      source = puissance(num_elem,0)/(Cp_*rho_ref_);
    }
  return (source);
}


#endif

