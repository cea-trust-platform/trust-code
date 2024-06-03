/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Terme_Source_inc_th_VDF_Face_included
#define Terme_Source_inc_th_VDF_Face_included

#include <Terme_Source_inc_th.h>
#include <TRUSTTabs_forward.h>
#include <Source_base.h>
#include <TRUST_Ref.h>

class Convection_Diffusion_Temperature;
class Navier_Stokes_std;
class Probleme_base;
class Domaine_Cl_VDF;
class Champ_Don;
class Domaine_VDF;

/*! @brief class Terme_Source_inc_th_VDF_Face Cette classe permet de conserver le debit dans une simulation
 *
 *   temporelle de inc
 *
 *
 * @sa Terme_Source_inc
 */
class Terme_Source_inc_th_VDF_Face : public Source_base, public Terme_Source_inc_th
{
  Declare_instanciable(Terme_Source_inc_th_VDF_Face);

public :
  void associer_pb(const Probleme_base& ) override;
  DoubleTab& calculer(DoubleTab& ) const override;
  /*    virtual const Champ_Don& beta() const =0; */
  inline const Champ_Don& beta() const ;
  void mettre_a_jour(double temps) override
  {
    Terme_Source_inc_th::mettre_a_jour(temps);
  }
  /*    void mettre_a_jour( double temps); */

  inline void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const override {}
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override;

  inline int has_interface_blocs() const override
  {
    return 1;
  };

protected :

  REF(Domaine_VDF) le_dom_VDF;
  REF(Domaine_Cl_VDF) le_dom_Cl_VDF;
  void associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis& ) override;
  /*    void calculer_cell_cent_vel(DoubleTab& ); */
  /*    void calculer_debit_ES(double& ,double&,double&  ) const; */
  /*    void calculer_debit(double& ,double& ) const; */


};
#endif
inline const Champ_Don& Terme_Source_inc_th_VDF_Face::beta() const
{
  return beta_t_.valeur();
}

