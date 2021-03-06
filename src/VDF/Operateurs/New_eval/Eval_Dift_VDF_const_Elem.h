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
#include <Ref_Turbulence_paroi_scal.h>
#include <DoubleVects.h>
class Turbulence_paroi_scal;

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

  inline double get_equivalent_distance(int boundary_index,int local_face) const
  {
    return equivalent_distance[boundary_index](local_face);
  }

  void associer_loipar(const Turbulence_paroi_scal& );
  void associer_modele_turbulence(const Modele_turbulence_scal_base& );
  void mettre_a_jour( );

private:
  REF(Modele_turbulence_scal_base) le_modele_turbulence;
  REF(Turbulence_paroi_scal) loipar;
  VECT(DoubleVect) equivalent_distance;
  int ind_Fluctu_Term;

};

#endif /* Eval_Diff_VDF_const_Elem_included */
