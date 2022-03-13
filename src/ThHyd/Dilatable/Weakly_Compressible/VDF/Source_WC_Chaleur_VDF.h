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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Source_WC_Chaleur_VDF.h
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Weakly_Compressible/VDF
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Source_WC_Chaleur_VDF_included
#define Source_WC_Chaleur_VDF_included

#include <Source_Fluide_Dilatable_VDF_Proto.h>
#include <Source_WC_Chaleur.h>

class Zone_VF;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION class  Source_WC_Chaleur_VDF
//
// Cette classe represente un terme source supplementaire
// a prendre en compte dans les equations de la chaleur
//  dans le cas ou le fluide est weakly compressible et pour
//  une discretisation VDF
//
// .SECTION voir aussi
// Source_base Fluide_Weakly_Compressible Source_WC_Chaleur
//
//////////////////////////////////////////////////////////////////////////////

class Source_WC_Chaleur_VDF : public Source_WC_Chaleur, public Source_Fluide_Dilatable_VDF_Proto
{
  Declare_instanciable(Source_WC_Chaleur_VDF);

  inline void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override {}
  inline void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override { ajouter_(secmem); }
  inline int has_interface_blocs() const override
  {
    return 1;
  };
protected:
  void associer_zones(const Zone_dis& zone,const Zone_Cl_dis& zcl) override;
  void compute_interpolate_gradP(DoubleTab& gradP, const DoubleTab& Ptot) const override;

private:
  void face_to_elem(const Zone_VF& zone, const DoubleTab& UgradP,DoubleTab& UgradP_elem) const;
  // on l'utilise pas pour le moment mais bon pour debug ...
  void compute_interpolate_gradP_old(DoubleTab& gradP, const DoubleTab& Ptot) const;
};

#endif /* Source_WC_Chaleur_VDF_included */
