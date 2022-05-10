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
// File:        Frottement_interfacial_Tomiyama.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Frottement_interfacial_Tomiyama.h>
#include <Pb_Multiphase.h>
#include <Milieu_composite.h>
#include <math.h>

Implemente_instanciable(Frottement_interfacial_Tomiyama, "Frottement_interfacial_Tomiyama", Frottement_interfacial_base);

Sortie& Frottement_interfacial_Tomiyama::printOn(Sortie& os) const
{
  return os;
}

Entree& Frottement_interfacial_Tomiyama::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("constante_gravitation", &g_);
  param.ajouter("contamination", &contamination_);
  param.lire_avec_accolades_depuis(is);
  if (! ((contamination_==0)|(contamination_==1)|(contamination_==2) )) Process::exit("Frottement_interfacial_Tomiyama : only 3 contamination levels exist : 0,1,2 !");

  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, pb_.valeur()) ? &ref_cast(Pb_Multiphase, pb_.valeur()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;
  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  for (int k = 0; k < pbm->nb_phases(); k++) if (k != n_l)
      if (!(ref_cast(Milieu_composite, pbm->milieu()).has_interface(n_l, k))) Process::exit("Frottement_interfacial_Tomiyama : one must define an interface and have a surface tension !");

  return is;
}

void Frottement_interfacial_Tomiyama::completer()
{
  if (!pb_->has_champ("diametre_bulles")) Process::exit("Frottement_interfacial_Tomiyama : there must be a bubble diameter field !");
}


void Frottement_interfacial_Tomiyama::coefficient(const DoubleTab& alpha, const DoubleTab& p, const DoubleTab& T,
                                                  const DoubleTab& rho, const DoubleTab& mu, const DoubleTab& sigma, double Dh,
                                                  const DoubleTab& ndv, const DoubleTab& d_bulles, DoubleTab& coeff) const
{
  int N = ndv.dimension(0);

  coeff = 0;

  for (int k = 0; k < N; k++) if (k!=n_l)
      {

        double Re = rho(n_l) * ndv(n_l,k) * d_bulles(k)/mu(n_l) + 1.e-6;
        double Eo = g_ * std::abs(rho(n_l)-rho(k)) * d_bulles(k)*d_bulles(k)/sigma(n_l,k);
        double Cd = -1;
        if (contamination_==0) Cd = std::max( std::min( 16./Re*(1+0.15*std::pow(Re, 0.687)) , 48./Re )   , 8.*Eo/(3.*(Eo+4.)));
        if (contamination_==1) Cd = std::max( std::min( 24./Re*(1+0.15*std::pow(Re, 0.687)) , 72./Re )   , 8.*Eo/(3.*(Eo+4.)));
        if (contamination_==2) Cd = std::max(           24./Re*(1+0.15*std::pow(Re, 0.687))              , 8.*Eo/(3.*(Eo+4.)));

        coeff(k, n_l, 1) = 3./4.*Cd/d_bulles(k) * alpha(k) * rho(n_l);
        coeff(k, n_l, 0) = coeff(k, n_l, 1) * ndv(n_l,k);
        coeff(n_l, k, 0) =  coeff(k, n_l, 0);
        coeff(n_l, k, 1) =  coeff(k, n_l, 1);
      }
}
