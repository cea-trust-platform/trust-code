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
// File:        Eval_Puiss_Th_QC_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible/VDF
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Puiss_Th_QC_VDF_Elem_included
#define Eval_Puiss_Th_QC_VDF_Elem_included

#include <Evaluateur_Source_VDF_Elem.h>
#include <Ref_Champ_Don.h>
#include <DoubleTab.h>
#include <Ref_Fluide_Quasi_Compressible.h>

////////////////////////////////////////////////////////////////////////////
//
//  CLASS Eval_Puiss_Th_QC_VDF_Elem
//
////////////////////////////////////////////////////////////////////////////

class Eval_Puiss_Th_QC_VDF_Elem: public Evaluateur_Source_VDF_Elem
{

public:

  inline Eval_Puiss_Th_QC_VDF_Elem();
  void associer_puissance(const Champ_Don& Q);
  void mettre_a_jour( );
  inline double calculer_terme_source(int ) const;
  inline void calculer_terme_source(int , DoubleVect& ) const;

protected:

  REF(Champ_Don) la_puissance;
  DoubleTab puissance;
};


//
//   Fonctions inline de la classe Eval_Puiss_Th_QC_VDF_Elem
//
inline Eval_Puiss_Th_QC_VDF_Elem::Eval_Puiss_Th_QC_VDF_Elem() {}
inline double Eval_Puiss_Th_QC_VDF_Elem::calculer_terme_source(int num_elem) const
{
  if ((puissance.size_array()==1)&&(puissance.nb_dim()==2))
    return puissance(0,0)*volumes(num_elem)*porosite_vol(num_elem);
  else
    {
      if (puissance.nb_dim()==1)
        return puissance(num_elem)*volumes(num_elem)*porosite_vol(num_elem);
      else
        return puissance(num_elem,0)*volumes(num_elem)*porosite_vol(num_elem);
    }
}

inline void Eval_Puiss_Th_QC_VDF_Elem::calculer_terme_source(int , DoubleVect& ) const
{
  ;
}

#endif

