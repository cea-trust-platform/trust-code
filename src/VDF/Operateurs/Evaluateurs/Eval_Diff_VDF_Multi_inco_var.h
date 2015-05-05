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
// File:        Eval_Diff_VDF_Multi_inco_var.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Eval_Diff_VDF_Multi_inco_var_included
#define Eval_Diff_VDF_Multi_inco_var_included

#include <Eval_Diff_VDF.h>
#include <Zone_VDF.h>
#include <Ref_Champ_base.h>
#include <Champ_base.h>
class Champ_base;
//
// .DESCRIPTION class Eval_Diff_VDF_Multi_inco_var
//
// Cette classe represente un evaluateur de flux diffusif
// pour un vecteur d'inconnues avec une diffusivite par
// inconnue. Le champ de diffusivite associe a chaque inconnue
// n'est pas constant.

//
// .SECTION voir aussi Evaluateur_VDF
//


class Eval_Diff_VDF_Multi_inco_var : public Eval_Diff_VDF
{

public:

  inline Eval_Diff_VDF_Multi_inco_var();
  inline void associer_diffusivite(const Champ_base& );
  inline const Champ_base& diffusivite() const;
  void mettre_a_jour();
  inline void associer(const Champ_base& );
  inline const Champ_base& get_diffusivite() const;
protected:

  REF(Champ_base) diffusivite_;
  DoubleTab dt_diffusivite;
  inline double dist_face(int, int, int) const;

};

//
//  Fonctions inline de la classe Eval_Diff_VDF_Multi_inco_var
//

// Description:
// constructeur par defaut
inline Eval_Diff_VDF_Multi_inco_var::Eval_Diff_VDF_Multi_inco_var()
{
}

// Description:
// associe le champ de diffusivite
inline void Eval_Diff_VDF_Multi_inco_var::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = ref_cast(Champ_base,diffu);
  dt_diffusivite.ref(diffu.valeurs());
}
inline void Eval_Diff_VDF_Multi_inco_var::associer(const Champ_base& diffu)
{
  associer_diffusivite( diffu);

}
inline const Champ_base& Eval_Diff_VDF_Multi_inco_var::get_diffusivite() const
{
  return diffusivite();
}


// Description:
// renvoie la distance entre les faces fac1 et fac2 dans la direction k
inline double Eval_Diff_VDF_Multi_inco_var::dist_face(int fac1, int fac2, int k) const
{
  return la_zone->dist_face(fac1, fac2, k);
}

inline const Champ_base& Eval_Diff_VDF_Multi_inco_var::diffusivite() const
{
  return diffusivite_;
}
#endif
