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

#ifndef Op_Diff_Turbulent_PolyMAC_P0_Elem_included
#define Op_Diff_Turbulent_PolyMAC_P0_Elem_included

#include <Op_Diff_PolyMAC_P0_Elem.h>
#include <Correlation.h>
#include <Transport_turbulent_base.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Op_Diff_Turbulent_PolyMAC_P0_Elem
//
// Version de Op_Diff_PolyMAC_P0_Elem prenant en compte l'effet de la turbulence
// par le biais d'une correlation de type Transport_turbulent.
// (celle-ci reposera sur la modelisation de la viscosite turbulente fournie
//  par la correlation Viscosite_turbulente de l'operateur de diffusion de la QDM)
//
/////////////////////////////////////////////////////////////////////////////

class Op_Diff_Turbulent_PolyMAC_P0_Elem : public Op_Diff_PolyMAC_P0_Elem
{

  Declare_instanciable( Op_Diff_Turbulent_PolyMAC_P0_Elem ) ;
  int dimension_min_nu() const override //pour que la correlation force l'anisotrope (cf. GGDH)
  {
    return ref_cast(Transport_turbulent_base, corr.valeur()).dimension_min_nu();
  }
  void completer() override;
  void modifier_nu(DoubleTab& ) const override; //prend en compte la diffusivite turbulente

  void creer_champ(const Motcle& motlu) override;
  void mettre_a_jour(double temps) override;

protected:
  Correlation corr; //correlation de transport turbulent

  Champ_Fonc secmem_diff_; //gradient des vitesses de chaque phase
  Motcle nom_secmem_diff_; //leurs noms
};

#endif /* Op_Diff_PolyMAC_P0_Elem_included */
