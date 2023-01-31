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

#ifndef Y_plus_Champ_Face_included
#define Y_plus_Champ_Face_included

#include <Ref_Champ_Face_VDF.h>
#include <Champ_Fonc_P0_VDF.h>
#include <Zone_Cl_dis_base.h>
#include <Ref_Zone_Cl_VDF.h>
#include <Champ_y_plus.h>

/*! @brief classe Y_plus_Champ_Face
 *
 * @sa Champ_y_plus Champ_Fonc_P0_VDF
 */
class Y_plus_Champ_Face: public Champ_y_plus, public Champ_Fonc_P0_VDF
{
  Declare_instanciable(Y_plus_Champ_Face);
public:
  void mettre_a_jour(double) override;
  void associer_champ(const Champ_Face_VDF&);
  void me_calculer(double) override;
  const Zone_Cl_dis_base& zone_Cl_dis_base() const;

  inline const Champ_Face_VDF& mon_champ() const { return mon_champ_.valeur(); }

  inline void associer_domaine_Cl_dis_base(const Zone_Cl_dis_base& le_dom_Cl_dis_base)
  {
    le_dom_Cl_VDF = (const Zone_Cl_VDF&) le_dom_Cl_dis_base;
  }

protected:
  REF(Champ_Face_VDF) mon_champ_;
  REF(Zone_Cl_VDF) le_dom_Cl_VDF;
};

#endif /* Y_plus_Champ_Face_included */
