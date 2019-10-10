/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Eval_Dirac_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Sources/Evaluateurs
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Dirac_VDF_Elem_included
#define Eval_Dirac_VDF_Elem_included


#include <Evaluateur_Source_VDF_Elem.h>
#include <Ref_Champ_Don.h>
#include <Zone.h>

////////////////////////////////////////////////////////////////////////////
//
//  CLASS Eval_Dirac_VDF_Elem
//
////////////////////////////////////////////////////////////////////////////

class Eval_Dirac_VDF_Elem: public Evaluateur_Source_VDF_Elem
{

public:

  inline Eval_Dirac_VDF_Elem();
  inline virtual ~Eval_Dirac_VDF_Elem() {};
  void associer_champs(const Champ_Don& ,const Champ_Don& , const Champ_Don& );
  void mettre_a_jour( );
  inline double calculer_terme_source(int ) const;
  inline void calculer_terme_source(int , DoubleVect& ) const;
  void associer_nb_elem_dirac(int );

  DoubleVect le_point;
protected:

  REF(Champ_Don) rho_ref;
  double rho_ref_;
  REF(Champ_Don) Cp;
  double Cp_;
  REF(Champ_Don) la_puissance;
  double puissance;
  REF(Zone) ma_zone;
  double nb_dirac;
};


//
//   Fonctions inline de la classe Eval_Dirac_VDF_Elem
//

inline Eval_Dirac_VDF_Elem::Eval_Dirac_VDF_Elem() {}

inline double Eval_Dirac_VDF_Elem::calculer_terme_source(int num_elem) const
{
  //Cout << "point = " << le_point << finl;
  //Cout << "elem = " << num_elem << " " << ma_zone.valeur().type_elem().contient(le_point,num_elem) << finl;
  int test =  ma_zone.valeur().type_elem().contient(le_point,num_elem) ;
  if (test == 1)
    {
      //Cout << "coucou dans Eval_Dirac_VDF_Elem !!!" << finl;
      return nb_dirac*puissance/(Cp_*rho_ref_);

    }
  else
    return 0;
}

inline void Eval_Dirac_VDF_Elem::calculer_terme_source(int , DoubleVect& ) const
{
  ;
}

#endif

