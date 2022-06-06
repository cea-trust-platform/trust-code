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

#ifndef Champ_Input_Proto_included
#define Champ_Input_Proto_included

#include <TRUSTArray.h>
#include <Ref_Probleme_base.h>
#include <ICoCoTrioField.h>
#include <Ref_Sous_Zone.h>

using ICoCo::TrioField;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     This is the base class for all the Fields which can be written by a call to
//     Problem::setInputField
//     There are for derived classes for uniform or not / domain or boundary fields.
//     All are supposed to be time-dependent.
//
//     Each Pb_base keeps a list of references to the Champ_Input_Proto it contains.
//
// .SECTION voir aussi
//    Champ_Input
//    Champ_Input_Uniforme
//    Champ_Front_Input
//    Champ_Front_Input_Uniforme
//////////////////////////////////////////////////////////////////////////////

class Champ_Input_Proto
{

public:

  virtual ~Champ_Input_Proto() {};
  virtual void getTemplate(TrioField& afield) const=0;
  virtual void setValue(const TrioField& afield)=0;
  void setInitialValue(const ArrOfDouble& vo);
  virtual void setDoubleValue(const double val);

protected:

  // Factorisation of functions from different input fields
  void read(Entree& is);
  void setValueOnTab(const TrioField& afield, DoubleTab& tab);

  // Provide access to similar functions from Champ_base and Champ_front_base
  // Used by Champ_Input_Proto::read
  virtual void set_nb_comp(int i)=0; // calls fixer_nb_comp
  virtual void set_name(const Nom& name)=0; // calls nommer

  REF(Probleme_base) mon_pb;
  bool sous_zone_ok; // This fields knows how to deal with a sous_zone
  REF(Sous_Zone) ma_sous_zone;
  ArrOfDouble initial_value_;
};

#endif
