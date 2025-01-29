/****************************************************************************
* Copyright (c) 2025, CEA
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

#ifndef T_paroi_Champ_P0_VDF_included
#define T_paroi_Champ_P0_VDF_included

#include <Champ_Fonc_P0_VDF.h>
#include <Domaine_Cl_VDF.h>
#include <TRUST_Ref.h>

class Champ_P0_VDF;

class T_paroi_Champ_P0_VDF: public Champ_Fonc_P0_VDF
{
  Declare_instanciable(T_paroi_Champ_P0_VDF);
public:
  void mettre_a_jour(double) override;
  void associer_champ(const Champ_P0_VDF&);
  const Domaine_Cl_dis_base& domaine_Cl_dis_base() const { return le_dom_Cl_VDF.valeur(); }

  inline const Champ_P0_VDF& mon_champ() const { return mon_champ_.valeur(); }

  inline void associer_domaine_Cl_dis_base(const Domaine_Cl_dis_base& le_dom_Cl_dis_base)
  {
    le_dom_Cl_VDF = ref_cast(Domaine_Cl_VDF, le_dom_Cl_dis_base);
  }

protected:
  void me_calculer(double);
  OBS_PTR(Champ_P0_VDF) mon_champ_;
  OBS_PTR(Domaine_Cl_VDF) le_dom_Cl_VDF;
};

inline double newton_T_paroi_VDF(double eps, double h, double T_ext, double lambda, double e, double T0)
{
  double Tb = T0;
  const double it_max = 100, tolerance = 1e-8;
  for (int i = 0; i < it_max; i++)
    {
      const double f = 5.67e-8 * eps * (T_ext * T_ext * T_ext * T_ext - Tb * Tb * Tb * Tb) + h * (T_ext - Tb) - lambda / e * (Tb - T0);
      const double f_p = -4 * 5.67e-8 * eps * Tb * Tb * Tb - h - lambda / e;
      const double Tb_new = Tb - f / f_p;
      if (std::abs(Tb_new - Tb) < tolerance)
        return Tb_new;
      Tb = Tb_new;
    }
  Process::exit("newton_T_paroi_VDF : newton did not converge !");
  return Tb;
}

#endif /* T_paroi_Champ_P0_VDF_included */
