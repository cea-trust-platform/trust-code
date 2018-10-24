/****************************************************************************
* Copyright (c) 2018, CEA
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
// File:        Op_Diff_VDF_Elem_base.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Op_Diff_VDF_Elem_base_included
#define Op_Diff_VDF_Elem_base_included

#include <Op_Diff_VDF_base.h>
#include <ItVDFEl.h>
#include <Op_VDF_Elem.h>
class Eval_VDF_Elem;
//
// .DESCRIPTION class Op_Diff_VDF_Elem_base
//
//  Cette classe represente l'operateur de diffusion associe a une equation de
//  transport.
//  La discretisation est VDF
//  Le champ diffuse est scalaire
//  Le champ de diffusivite est uniforme
//  L'iterateur associe est de type Iterateur_VDF_Elem
//  L'evaluateur associe est de type Eval_Diff_VDF_const_Elem

//
// .SECTION voir aussi
//
//

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_VDF_Elem_base
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_VDF_Elem_base : public Op_Diff_VDF_base, public Op_VDF_Elem
{

  Declare_base_sans_constructeur(Op_Diff_VDF_Elem_base);

public:
  inline Op_Diff_VDF_Elem_base(const Iterateur_VDF_base& iterateur);

  void associer(const Zone_dis& , const Zone_Cl_dis& ,
                const Champ_Inc& );
  void associer_diffusivite(const Champ_base& );

  const Champ_base& diffusivite() const;
  inline  void dimensionner(Matrice_Morse& ) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  double calculer_dt_stab() const;
protected:
  virtual Eval_VDF_Elem& get_eval_elem()=0;
};


//
// Fonctions inline de la classe Op_Diff_VDF_Elem_base
//


// Ce constructeur permet de creer des classes filles des evalateurs
// (utilise dans le constructeur de Op_Diff_VDF_var_Elem_temp_FTBM)
inline Op_Diff_VDF_Elem_base::Op_Diff_VDF_Elem_base(const Iterateur_VDF_base& iterateur)
  : Op_Diff_VDF_base(iterateur)
{
  declare_support_masse_volumique(1);
}
// Description:
// on dimensionne notre matrice.
inline  void Op_Diff_VDF_Elem_base::dimensionner(Matrice_Morse& matrice) const
{
  Op_VDF_Elem::dimensionner(iter.zone(), iter.zone_Cl(), matrice);
}

inline void Op_Diff_VDF_Elem_base::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_VDF_Elem::modifier_pour_Cl(iter.zone(), iter.zone_Cl(), matrice, secmem);
}

#endif
