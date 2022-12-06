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

#include <Source_Portance_interfaciale_base.h>
#include <Portance_interfaciale_base.h>
#include <Pb_Multiphase.h>

Implemente_base(Source_Portance_interfaciale_base, "Source_Portance_interfaciale_base", Sources_Multiphase_base);

Sortie& Source_Portance_interfaciale_base::printOn(Sortie& os) const { return os; }

Entree& Source_Portance_interfaciale_base::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("beta", &beta_);
  param.lire_avec_accolades_depuis(is);

  Pb_Multiphase *pbm = sub_type(Pb_Multiphase, equation().probleme()) ? &ref_cast(Pb_Multiphase, equation().probleme()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");

  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;

  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  if (pbm->has_correlation("Portance_interfaciale")) correlation_ = pbm->get_correlation("Portance_interfaciale"); //correlation fournie par le bloc correlation
  else correlation_.typer_lire((*pbm), "Portance_interfaciale", is); //sinon -> on la lit

  pbm->creer_champ("vorticite"); // Besoin de vorticite

  return is;
}
