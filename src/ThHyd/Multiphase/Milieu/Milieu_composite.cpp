/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Milieu_composite.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase/Milieu
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Milieu_composite.h>
#include <Equation_base.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <EChaine.h>
#include <Zone_VF.h>
#include <Pb_Multiphase.h>
#include <Champ_Uniforme.h>
#include <Interprete.h>

Implemente_instanciable(Milieu_composite, "Milieu_composite", Fluide_base);

Entree& Milieu_composite::readOn(Entree& is)
{
  int i = 0;
  Nom mot;
  is >> mot;
  if (mot != "{") Cerr << "Milieu_composite : { expected instead of " << mot << finl, Process::exit();
  for (is >> mot; mot != "}"; is >> mot)
    {
      Cerr << "Milieu_composite : ajout de " << mot << " ... ";
      if (Interprete::objet_existant(mot))
        {
          Fluide_base& fluide = ref_cast(Fluide_base, Interprete::objet(mot));
          fluide.set_id_composite(i++);
          fluides.add(fluide);
          Cerr << "ok!" << finl;
        }
      else Process::exit("not found!");
    }
  return is;
}

Sortie& Milieu_composite::printOn(Sortie& os) const
{
  return os;
}

int Milieu_composite::initialiser(const double& temps)
{
  for (int i = 0; i < fluides.size(); i++)
    fluides[i].initialiser(temps);
  mettre_a_jour_tabs();
  return 1;
}


void Milieu_composite::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  Cerr << "Composite medium discretization." << finl;

  const int N = fluides.size();
  const Zone_dis_base& zone_dis = pb.equation(0).zone_dis();
  const double temps = pb.schema_temps().temps_courant();

  // Fluide_base::discretiser(pb, dis);
  for (int n = 0; n < N; n++)
    fluides[n].discretiser(pb, dis);

  Champ_Fonc rho_fonc;
  dis.discretiser_champ("champ_elem", zone_dis,       "masse_volumique", "kg/m^3", N, temps, rho_fonc);
  rho = rho_fonc;
  dis.discretiser_champ("champ_elem", zone_dis,   "viscosite_dynamique", "kg/m/s", N, temps,       mu);
  dis.discretiser_champ("champ_elem", zone_dis, "viscosite_cinematique",   "m2/s", N, temps,       nu);
  dis.discretiser_champ("champ_elem", zone_dis,           "diffusivite",   "m2/s", N, temps,    alpha);
  dis.discretiser_champ("champ_elem", zone_dis,          "conductivite",  "W/m/K", N, temps,   lambda);
  dis.discretiser_champ("champ_elem", zone_dis,  "capacite_calorifique", "J/kg/K", N, temps,       Cp);

  champs_compris_.ajoute_champ(rho);
  champs_compris_.ajoute_champ(mu.valeur());
  champs_compris_.ajoute_champ(nu.valeur());
  champs_compris_.ajoute_champ(alpha.valeur());
  champs_compris_.ajoute_champ(lambda.valeur());
  champs_compris_.ajoute_champ(Cp.valeur());

}

void Milieu_composite::mettre_a_jour(double temps)
{
  Milieu_base::mettre_a_jour(temps);
  for (int i = 0; i < fluides.size(); i++)
    fluides[i].mettre_a_jour(temps);
  rho.changer_temps(temps);
  rho.valeur().changer_temps(temps);
  mu.valeur().changer_temps(temps);
  nu.valeur().changer_temps(temps);
  lambda.valeur().changer_temps(temps);
  alpha.valeur().changer_temps(temps);
  Cp.valeur().changer_temps(temps);
  mettre_a_jour_tabs();
}

void Milieu_composite::mettre_a_jour_tabs()
{
  const int N = fluides.size();

  /* masse volumique */
  {
    DoubleTab& tab = rho.valeurs();
    const int Nl = rho.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].masse_volumique();
        const int crho = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs().addr()[!crho * i];
      }
  }

  /* viscosite dynamique */
  {
    DoubleTab& tab = mu.valeurs();
    const int Nl = mu.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].viscosite_dynamique();
        const int cch = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs().addr()[!cch * i];
      }
  }

  /* viscosite cinematique */
  {
    DoubleTab& tab = nu.valeurs();
    const int Nl = nu.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].viscosite_cinematique();
        const int cch = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs().addr()[!cch * i];
      }
  }

  /* diffusivite */
  {
    DoubleTab& tab = alpha.valeurs();
    const int Nl = alpha.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].diffusivite();
        const int cch = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs().addr()[!cch * i];
      }
  }

  /* conductivite */
  {
    DoubleTab& tab = lambda.valeurs();
    const int Nl = lambda.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].conductivite();
        const int cch = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs().addr()[!cch * i];
      }
  }

  /* capacite calorifique */
  {
    DoubleTab& tab = Cp.valeurs();
    const int Nl = Cp.valeurs().dimension_tot(0);
    for (int n = 0; n < N; n++)
      {
        const Champ_base& ch_n = fluides[n].capacite_calorifique();
        const int cch = sub_type(Champ_Uniforme, ch_n);
        for (int i = 0; i < Nl; i++) tab(i, n) = ch_n.valeurs().addr()[!cch * i];
      }
  }
}

void Milieu_composite::associer_equation(const Equation_base *eqn) const
{
  Fluide_base::associer_equation(eqn);
  // on fait suivre aux milieux sous-jacents (les fluide_reel en ont besoin!)
  for (int i = 0; i < fluides.size(); i++)
    fluides[i].associer_equation(eqn);
}
