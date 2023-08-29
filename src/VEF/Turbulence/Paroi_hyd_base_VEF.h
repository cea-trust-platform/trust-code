/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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

#ifndef Paroi_hyd_base_VEF_included
#define Paroi_hyd_base_VEF_included

#include <Turbulence_paroi_base.h>
#include <Domaine_VEF.h>
#include <Domaine_Cl_VEF.h>
#include <TRUST_Ref.h>


/*! @brief CLASS: Paroi_hyd_base_VEF Classe de base des lois de paroi hydraulique en VEF
 *
 * .SECTION  voir aussi
 *  Turbulence_paroi_base
 *
 */
class Paroi_hyd_base_VEF : public Turbulence_paroi_base
{

  Declare_base(Paroi_hyd_base_VEF);

public:

  void associer(const Domaine_dis& ,const Domaine_Cl_dis& ) override;
  void init_lois_paroi_();
  DoubleTab& corriger_derivee_impl(DoubleTab& d) const override;
  inline const ArrOfInt& face_keps_imposee() const
  {
    return face_keps_imposee_ ;
  };
  void imprimer_premiere_ligne_ustar(int boundaries_, const LIST(Nom)& boundaries_list, const Nom& nom_fichier_) const override;
  void imprimer_ustar_mean_only(Sortie&, int, const LIST(Nom)&, const Nom& ) const override;

protected:

  REF(Domaine_VEF) le_dom_VEF;
  REF(Domaine_Cl_VEF) le_dom_Cl_VEF;
  IntVect face_keps_imposee_; // avec descripteur parallele
  int flag_face_keps_imposee_;
};

#endif
