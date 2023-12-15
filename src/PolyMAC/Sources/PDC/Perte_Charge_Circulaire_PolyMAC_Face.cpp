/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Perte_Charge_Circulaire_PolyMAC_Face.h>
#include <Motcle.h>
#include <Param.h>

Implemente_instanciable(Perte_Charge_Circulaire_PolyMAC_Face, "Perte_Charge_Circulaire_Face_PolyMAC", Perte_Charge_PolyMAC);

Sortie& Perte_Charge_Circulaire_PolyMAC_Face::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Perte_Charge_Circulaire_PolyMAC_Face::readOn(Entree& s)
{
  Perte_Charge_PolyMAC::readOn(s);
  if (v->nb_comp() != dimension)
    {
      Cerr << "Il faut definir le champ direction a " << dimension << " composantes" << finl;
      Process::exit();
    }
  return s;
}

void Perte_Charge_Circulaire_PolyMAC_Face::set_param(Param& param)
{
  Perte_Charge_PolyMAC::set_param(param);
  param.ajouter_non_std("lambda_ortho", (this), Param::REQUIRED);
  param.ajouter("diam_hydr_ortho", &diam_hydr_ortho, Param::REQUIRED);
  param.ajouter("direction", &v, Param::REQUIRED);
}

int Perte_Charge_Circulaire_PolyMAC_Face::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "lambda")
    {
      lambda.addVar("Re_tot");
      lambda.addVar("Re_long");
      return Perte_Charge_PolyMAC::lire_motcle_non_standard(mot, is);
    }
  else if (mot == "lambda_ortho")
    return lire_motcle_non_standard_impl(mot, is);
  else
    return Perte_Charge_PolyMAC::lire_motcle_non_standard(mot, is);
}

void Perte_Charge_Circulaire_PolyMAC_Face::coeffs_perte_charge(const DoubleVect& u, const DoubleVect& pos, double t, double norme_u,
                                                               double dh, double nu, double reynolds, double& coeff_ortho,
                                                               double& coeff_long, double& u_l, DoubleVect& av_valeur) const
{
  coeffs_perte_charge_impl(u, pos, t, norme_u, dh, nu, reynolds, coeff_ortho, coeff_long, u_l, av_valeur, lambda);
}

/////////////////////////////////////////////////

Implemente_instanciable(Perte_Charge_Circulaire_PolyMAC_P0P1NC_Face, "Perte_Charge_Circulaire_Face_PolyMAC_P0P1NC|Perte_Charge_Circulaire_Face_PolyMAC_P0", Perte_Charge_PolyMAC_P0P1NC);

Sortie& Perte_Charge_Circulaire_PolyMAC_P0P1NC_Face::printOn(Sortie& s) const { return s << que_suis_je() << finl; }

Entree& Perte_Charge_Circulaire_PolyMAC_P0P1NC_Face::readOn(Entree& s)
{
  Perte_Charge_PolyMAC_P0P1NC::readOn(s);
  if (v->nb_comp() != dimension)
    {
      Cerr << "Il faut definir le champ direction a " << dimension << " composantes" << finl;
      Process::exit();
    }
  return s;
}

void Perte_Charge_Circulaire_PolyMAC_P0P1NC_Face::set_param(Param& param)
{
  Perte_Charge_PolyMAC_P0P1NC::set_param(param);
  param.ajouter_non_std("lambda_ortho", (this), Param::REQUIRED);
  param.ajouter("diam_hydr_ortho", &diam_hydr_ortho, Param::REQUIRED);
  param.ajouter("direction", &v, Param::REQUIRED);
}

int Perte_Charge_Circulaire_PolyMAC_P0P1NC_Face::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "lambda")
    {
      lambda.addVar("Re_tot");
      lambda.addVar("Re_long");
      return Perte_Charge_PolyMAC_P0P1NC::lire_motcle_non_standard(mot, is);
    }
  else if (mot == "lambda_ortho")
    return lire_motcle_non_standard_impl(mot, is);
  else
    return Perte_Charge_PolyMAC_P0P1NC::lire_motcle_non_standard(mot, is);
}

void Perte_Charge_Circulaire_PolyMAC_P0P1NC_Face::coeffs_perte_charge(const DoubleVect& u, const DoubleVect& pos, double t, double norme_u,
                                                                      double dh, double nu, double reynolds, double& coeff_ortho,
                                                                      double& coeff_long, double& u_l, DoubleVect& av_valeur) const
{
  coeffs_perte_charge_impl(u, pos, t, norme_u, dh, nu, reynolds, coeff_ortho, coeff_long, u_l, av_valeur, lambda);
}
