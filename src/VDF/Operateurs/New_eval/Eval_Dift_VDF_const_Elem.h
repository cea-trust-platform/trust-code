/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Eval_Dift_VDF_const_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/New_eval
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Dift_VDF_const_Elem_included
#define Eval_Dift_VDF_const_Elem_included

#include <Eval_Dift_VDF_const2.h>
#include <Eval_Diff_VDF_Elem.h>
#include <Ref_Modele_turbulence_scal_base.h>
#include <Turbulence_paroi_scal.h>
#include <Ref_Turbulence_paroi_scal.h>
#include <DoubleVects.h>

//
// .DESCRIPTION class Eval_Dift_VDF_const_Elem
// Evaluateur VDF pour la diffusion totale (laminaire et turbulente)
// Le champ diffuse est scalaire (Champ_P0_VDF)
// Le champ de diffusivite est constant.
//

// .SECTION
// voir aussi Eval_Dift_VDF_const2

class Eval_Dift_VDF_const_Elem : public Eval_Diff_VDF_Elem<Eval_Dift_VDF_const_Elem>,
  public Eval_Dift_VDF_const2
{
public :
  inline Eval_Dift_VDF_const_Elem() : ind_Fluctu_Term(1) {}
  static constexpr bool Is_Modif_Deq = true;

  inline int get_ind_Fluctu_Term() const
  {
    return ind_Fluctu_Term;
  }

  inline void associer_loipar(const Turbulence_paroi_scal& loi_paroi)
  {
    Eval_Dift_VDF_const2::associer_loipar(loi_paroi);
    ind_Fluctu_Term = 0;
  }

  void associer_modele_turbulence(const Modele_turbulence_scal_base& mod)
  {
    // On remplit la reference au modele de turbulence et le tableau k:
    le_modele_turbulence = mod;
    ind_Fluctu_Term = 0;
    if (!loipar.non_nul())
      ind_Fluctu_Term = 1;
  }

  //void Eval_Dift_VDF_const_Elem::mettre_a_jour()
  //{
  //  Eval_Dift_VDF_const2::mettre_a_jour();
  //  if (loipar.non_nul() || (ind_Fluctu_Term != 1))
  //    {
  //      int s=loipar->tab_equivalent_distance_size();
  //      equivalent_distance.dimensionner(s);
  //      for(int i=0; i<s; i++)
  //        {
  //          equivalent_distance[i].ref(loipar->tab_equivalent_distance(i));
  //        }
  //    }
  //}

private:
  REF(Modele_turbulence_scal_base) le_modele_turbulence;
  int ind_Fluctu_Term;

};

#endif /* Eval_Diff_VDF_const_Elem_included */
