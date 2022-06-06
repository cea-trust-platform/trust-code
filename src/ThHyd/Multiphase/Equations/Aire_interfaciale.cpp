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

#include <Aire_interfaciale.h>
#include <Pb_Multiphase.h>
#include <Discret_Thyd.h>
#include <Zone_VF.h>
#include <Domaine.h>
#include <Avanc.h>
#include <Debog.h>
#include <Frontiere_dis_base.h>
#include <EcritureLectureSpecial.h>
#include <Champ_Uniforme.h>
#include <Matrice_Morse.h>
#include <Navier_Stokes_std.h>
#include <TRUSTTrav.h>
#include <Neumann_sortie_libre.h>
#include <Op_Conv_negligeable.h>
#include <Param.h>
#include <Schema_Implicite_base.h>
#include <SETS.h>
#include <EChaine.h>
#include <Neumann_paroi.h>
#include <Scalaire_impose_paroi.h>
#include <Echange_global_impose.h>

#define old_forme

Implemente_instanciable(Aire_interfaciale,"Aire_interfaciale|Interfacial_area",Convection_Diffusion_std);

Sortie& Aire_interfaciale::printOn(Sortie& is) const
{
  return Convection_Diffusion_std::printOn(is);
}

Entree& Aire_interfaciale::readOn(Entree& is)
{
  assert(l_inco_ch.non_nul());
  assert(le_fluide.non_nul());
  Convection_Diffusion_std::readOn(is);

  terme_convectif.set_fichier("Convection_interfacial_area");
  terme_convectif.set_description((Nom)"interfacial area transfer rate=Integral(-A*u*ndS) [kg] if SI units used");

  const Pb_Multiphase *pbm = sub_type(Pb_Multiphase, probleme()) ? &ref_cast(Pb_Multiphase, probleme()) : NULL;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");
  for (int n = 0; n < pbm->nb_phases(); n++) //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;
  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  if (pbm->has_correlation("diametre_bulles")) Process::exit(que_suis_je() + " : the interfacial area equation sets bubble diameter, there cannot be an exterior correlation !");

  return is;
}

void Aire_interfaciale::associer_milieu_base(const Milieu_base& un_milieu)
{
  const Fluide_base& un_fluide = ref_cast(Fluide_base,un_milieu);
  associer_fluide(un_fluide);
}

void Aire_interfaciale::discretiser()
{
  int nb_valeurs_temp = schema_temps().nb_valeurs_temporelles();
  double temps = schema_temps().temps_courant();
  const Discret_Thyd& dis=ref_cast(Discret_Thyd, discretisation());
  int N = ref_cast(Pb_Multiphase, probleme()).nb_phases();

  Cerr << "Interfacial area discretization" << finl;
  //On utilise temperature pour la directive car discretisation identique
  dis.discretiser_champ("temperature",zone_dis(),"interfacial_area","m-1", N,nb_valeurs_temp,temps,l_inco_ch);//une aire interfaciale par phase
  l_inco_ch.valeur().fixer_nature_du_champ(multi_scalaire);
  l_inco_ch.valeur().fixer_nom_compo(0, Nom("tau"));
  champs_compris_.ajoute_champ(l_inco_ch);

  Cerr << "Bubble diameter discretization" << finl;
  //On utilise temperature pour la directive car discretisation identique
  Noms noms(N), unites(N);
  noms[0] = "diametre_bulles";
  unites[0] = "m";
  Motcle typeChamp = "champ_elem" ;
  const Zone_dis& z = ref_cast(Zone_dis, probleme().domaine_dis().zone_dis(0));
  dis.discretiser_champ(typeChamp, z.valeur(), multi_scalaire, noms , unites, N, 0, diametre_bulles);

  champs_compris_.ajoute_champ(diametre_bulles);

  Equation_base::discretiser();
  Cerr << "Echelle_temporelle_turbulente::discretiser() ok" << finl;
}

const Milieu_base& Aire_interfaciale::milieu() const
{
  return le_fluide.valeur();
}

Milieu_base& Aire_interfaciale::milieu()
{
  return le_fluide.valeur();
}

int Aire_interfaciale::impr(Sortie& os) const
{
  return Equation_base::impr(os);
}

const Motcle& Aire_interfaciale::domaine_application() const
{
  static Motcle mot("Interfacial_area");
  return mot;
}

void Aire_interfaciale::associer_fluide(const Fluide_base& un_fluide)
{
  le_fluide = un_fluide;
}

const Operateur& Aire_interfaciale::operateur(int i) const
{
  if (i) Cerr << "Aire_interfaciale : wrong operator number " << i << finl, Process::exit();
  return terme_convectif;
}

Operateur& Aire_interfaciale::operateur(int i)
{
  if (i) Cerr << "Aire_interfaciale : wrong operator number " << i << finl, Process::exit();
  return terme_convectif;
}

void Aire_interfaciale::mettre_a_jour(double temps)
{
  Convection_Diffusion_std::mettre_a_jour(temps);

  int i, n, N = ref_cast(Pb_Multiphase, probleme()).nb_phases();

  const DoubleTab& alpha = probleme().get_champ("alpha").passe(), &a_i = inconnue().passe();
  DoubleTab& d_b = diametre_bulles.valeurs();

  diametre_bulles.mettre_a_jour(temps);

  for (n = 0; n < N; n++)
    for (i = 0; i < d_b.dimension_tot(0); i++)
      if (n != n_l)
        d_b(i, n) = std::max(1.e-8, ((a_i(i, n)>1.e-6) ? 6 * alpha(i, n)/a_i(i, n) : 0));

  for (i = 0; i < d_b.dimension_tot(0); i++) d_b(i, n_l) = 0;
}
