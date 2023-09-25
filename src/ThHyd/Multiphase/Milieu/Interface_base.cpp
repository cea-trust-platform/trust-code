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

#include <Discretisation_base.h>
#include <Interface_base.h>
#include <QDM_Multiphase.h>
#include <Equation_base.h>
#include <Pb_Multiphase.h>
#include <Domaine_VF.h>
#include <TRUSTTrav.h>

Implemente_base(Interface_base, "Interface_base", Objet_U);
// XD saturation_base objet_u saturation_base -1 Basic class for a liquid-gas interface (used in pb_multiphase)

Sortie& Interface_base::printOn(Sortie& os) const { return os; }

void Interface_base::set_param(Param& param) { param.ajouter("tension_superficielle|surface_tension", &sigma__); }

Entree& Interface_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Interface_base::mettre_a_jour(double temps)
{
  DoubleTab& sigma_tab = ch_sigma_->valeurs();
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, pb_.valeur());
  const DoubleTab& press = ref_cast(QDM_Multiphase, pbm.equation_qdm()).pression()->valeurs(),
                   &temp = pbm.equation_energie().inconnue()->valeurs();

  // XXX Elie Saikali : Cas sans saturation !
  // pour le moment on simplifie le calcul et on prend la moyenne de T
  // FIXME : faut faire un truc pour calculer T a l'interface comme le code dans Source_Flux_interfacial_base::ajouter_blocs
  // T_interf = ( Tl / hl + Tg / hg ) / (1 / hl + 1 / hg )

  const int N = temp.line_size();
  DoubleTrav Ti(sigma_tab);
  Ti = 0.;

  for (int i = 0; i < Ti.dimension_tot(0); i++)
    {
      for (int j = 0; j < N; j++) Ti(i, 0) += temp(i, j);
      Ti(i,0) /= N;
    }

  // call sigma
  sigma(Ti.get_span(), press.get_span(), sigma_tab.get_span(), 1, 0);
  sigma_tab.echange_espace_virtuel();
}

void Interface_base::sigma(const SpanD T, const SpanD P, SpanD res, int ncomp, int ind) const
{
  if (sigma__ >= 0)
    for (int i =0; i < (int)P.size(); i++) res[i * ncomp + ind] = sigma__;
  else sigma_(T,P,res,ncomp,ind);
}

double Interface_base::sigma(const double T, const double P) const
{
  ArrayD Tt = {T}, Pp = {P}, res_ = {0.};
  sigma(SpanD(Tt),SpanD(Pp),SpanD(res_),1,0);
  return res_[0];
}
