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

#include <Transport_turbulent_aire_interfaciale.h>
#include <EcritureLectureSpecial.h>
#include <Schema_Implicite_base.h>
#include <Scalaire_impose_paroi.h>
#include <Echange_global_impose.h>
#include <Neumann_sortie_libre.h>
#include <Op_Conv_negligeable.h>
#include <Frontiere_dis_base.h>
#include <Aire_interfaciale.h>
#include <Navier_Stokes_std.h>
#include <Champ_Uniforme.h>
#include <Matrice_Morse.h>
#include <Pb_Multiphase.h>
#include <Neumann_paroi.h>
#include <Discret_Thyd.h>
#include <Domaine_VF.h>
#include <TRUSTTrav.h>
#include <EChaine.h>
#include <Domaine.h>
#include <Param.h>
#include <Avanc.h>
#include <Debog.h>
#include <SETS.h>

#include <EChaine.h>
#include <Neumann_paroi.h>
#include <Scalaire_impose_paroi.h>
#include <Echange_global_impose.h>
#include <Milieu_composite.h>
#include <math.h>


Implemente_instanciable(Aire_interfaciale,"Aire_interfaciale|Interfacial_area",Convection_Diffusion_std);

Sortie& Aire_interfaciale::printOn(Sortie& is) const
{
  return Convection_Diffusion_std::printOn(is);
}

Entree& Aire_interfaciale::readOn(Entree& is)
{
  terme_diffusif.associer_eqn(*this);
  terme_convectif.associer_eqn(*this);

  assert(l_inco_ch_.non_nul());
  assert(le_fluide_.non_nul());
  Convection_Diffusion_std::readOn(is);

  terme_convectif.set_fichier("Convection_interfacial_area");
  terme_convectif.set_description((Nom)"interfacial area transfer rate=Integral(-A*u*ndS) [kg] if SI units used");

  Pb_Multiphase *pbm = sub_type(Pb_Multiphase, probleme()) ? &ref_cast(Pb_Multiphase, probleme()) : nullptr;

  if (!pbm || pbm->nb_phases() == 1) Process::exit(que_suis_je() + " : not needed for single-phase flow!");

  for (int n = 0; n < pbm->nb_phases(); n++)  //recherche de n_l, n_g : phase {liquide,gaz}_continu en priorite
    {
      if (pbm->nom_phase(n).debute_par("liquide") && (n_l < 0 || pbm->nom_phase(n).finit_par("continu")))  n_l = n;
      if (( pbm->nom_phase(n).finit_par("group1")))  n_g1 = n;
      if (( pbm->nom_phase(n).finit_par("group2")))  n_g2 = n;
    }
  if (n_l < 0) Process::exit(que_suis_je() + " : liquid phase not found!");

  if (pbm->has_correlation("diametre_bulles")) Process::exit(que_suis_je() + " : the interfacial area equation sets bubble diameter, there cannot be an exterior correlation !");

  if (pbm)
    {
      const QDM_Multiphase& qdm = ref_cast(QDM_Multiphase, pbm->equation_qdm());

      if (discretisation().is_vdf())
        {
          Cerr << "The turbulent operator of Aire_interfaciale equation is not yet ported to the VDF discretization ..." << finl;
          Process::exit();
        }

      if (qdm.operateur_diff()->is_turb() && terme_diffusif->is_turb())
        {
          terme_diffusif.set_fichier("Diffusion_interfacial_area");
          terme_diffusif.set_description((Nom)"interfacial area transfer rate=Integral(mu_t/0.405*grad(ai)*ndS) [W] if SI units used");
          has_diff_turb_  = true;

          /*
           * Teste si la corr Transport_turbulent_aire_interfaciale ...
           */
          if (terme_diffusif->correlation_viscosite_turbulente())
            {
              if (!sub_type(Transport_turbulent_aire_interfaciale, *terme_diffusif->correlation_viscosite_turbulente()))
                {
                  Cerr << "Error in you Aire_interfaciale::readOn !! \n You can only use the Transport_turbulent_aire_interfaciale correlation for the diffusion operator !" << finl;
                  Process::exit();
                }
            }
        }

    }

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
  dis.discretiser_champ("temperature",domaine_dis(),"interfacial_area","m-1", N,nb_valeurs_temp,temps,l_inco_ch_);//une aire interfaciale par phase
  l_inco_ch_->fixer_nature_du_champ(multi_scalaire);
  l_inco_ch_->fixer_nom_compo(0, Nom("tau"));
  champs_compris_.ajoute_champ(l_inco_ch_);

  Cerr << "Bubble diameter discretization" << finl;
  //On utilise temperature pour la directive car discretisation identique
  Noms noms(N), unites(N);
  noms[0] = "diametre_bulles";
  unites[0] = "m";
  Motcle typeChamp = "champ_elem" ;
  const Domaine_dis_base& z = probleme().domaine_dis();
  dis.discretiser_champ(typeChamp, z, multi_scalaire, noms , unites, N, 0, diametre_bulles_);

  champs_compris_.ajoute_champ(diametre_bulles_);

  Equation_base::discretiser();
  Cerr << "Echelle_temporelle_turbulente::discretiser() ok" << finl;
}

const Milieu_base& Aire_interfaciale::milieu() const
{
  return le_fluide_.valeur();
}

Milieu_base& Aire_interfaciale::milieu()
{
  return le_fluide_.valeur();
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
  le_fluide_ = un_fluide;
}

const Operateur& Aire_interfaciale::operateur(int i) const
{
  if (!has_diff_turb_)
    {
      if (i)
        {
          Cerr << "Error for Aire_interfaciale::operateur(int i)" << finl;
          Cerr << "Aire_interfaciale has " << nombre_d_operateurs() <<" operators "<<finl;
          Cerr << "and you are trying to access the " << i <<" th one."<< finl;
          exit();
        }

      return terme_convectif;
    }

  switch(i)
    {
    case 0:
      return terme_diffusif;
    case 1:
      return terme_convectif;
    default :
      Cerr << "Error for Aire_interfaciale::operateur(int i)" << finl;
      Cerr << "Aire_interfaciale has " << nombre_d_operateurs() <<" operators "<<finl;
      Cerr << "and you are trying to access the " << i <<" th one."<< finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

Operateur& Aire_interfaciale::operateur(int i)
{
  if (!has_diff_turb_)
    {
      if (i)
        {
          Cerr << "Error for Aire_interfaciale::operateur(int i)" << finl;
          Cerr << "Aire_interfaciale has " << nombre_d_operateurs() <<" operators "<<finl;
          Cerr << "and you are trying to access the " << i <<" th one."<< finl;
          exit();
        }

      return terme_convectif;
    }

  switch(i)
    {
    case 0:
      return terme_diffusif;
    case 1:
      return terme_convectif;
    default :
      Cerr << "Error for Aire_interfaciale::operateur(int i)" << finl;
      Cerr << "Aire_interfaciale has " << nombre_d_operateurs() <<" operators "<<finl;
      Cerr << "and you are trying to access the " << i <<" th one."<< finl;
      exit();
    }
  // Pour les compilos!!
  return terme_diffusif;
}

void Aire_interfaciale::mettre_a_jour(double temps)
{
  Convection_Diffusion_std::mettre_a_jour(temps);

  int i, n, N = ref_cast(Pb_Multiphase, probleme()).nb_phases(), Np = probleme().get_champ("pression").valeurs().line_size();
  const Pb_Multiphase& pbm = ref_cast(Pb_Multiphase, probleme());

  const DoubleTab& alpha = probleme().get_champ("alpha").passe();
  const DoubleTab& a_i = inconnue().passe();
  const DoubleTab& rho_p   = milieu().masse_volumique().passe();
  const DoubleTab& temp_p  = pbm.equation_energie().inconnue().passe();
  const DoubleTab& press_p = ref_cast(QDM_Multiphase,pbm.equation_qdm()).pression().passe();
  DoubleTab& d_b = diametre_bulles_->valeurs();
  double D_crit = 0.011;
  int cR = (rho_p.dimension_tot(0) == 1);
  const Milieu_composite& milc = ref_cast(Milieu_composite, milieu());

  diametre_bulles_->mettre_a_jour(temps);


  const double void_fraction_threshold = 1e-5; // small void fraction is ignored to avoid numerical problems with group 2
  const double interfacial_area_threshold = 1e-5; //ai is replaced by this value if too small
  const double min_diameter = 1e-5; // diameter is replaced by this value if too small
  for (n = 0; n < N; n++)
    {
      for (i = 0; i < d_b.dimension_tot(0); i++)
        {
          if (n != n_l)
            {
              if (a_i(i, n)<=interfacial_area_threshold)
                d_b(i, n) = min_diameter;
              else
                d_b(i, n) = std::max(min_diameter, 6 * alpha(i, n)/a_i(i, n) );
            }

          if ( 0. < n_g1 )
            {
              if(milc.has_interface(n_g1, n_l))
                {
                  Interface_base& sat = milc.get_interface(n_l, n_g1);
                  D_crit = 4. * std::sqrt(sat.sigma(temp_p(i,n_l), press_p(i,n_l * (Np > 1))) / g / (rho_p(!cR * i, n_l)-rho_p(!cR * i, n_g1)));
                }
              else if (milc.has_saturation(n_g1, n_l))
                {
                  Saturation_base& z_sat = milc.get_saturation(n_g1,  n_l);
                  DoubleTab& sig = z_sat.get_sigma_tab();
                  D_crit = 4. * std::sqrt(sig(i) / g / (rho_p(!cR * i, n_l)-rho_p(!cR * i, n_g1)));
                }
              if (n==n_g1)
                {
                  d_b(i, n_g1) = std::min(std::max(d_b(i, n_g1),min_diameter),D_crit);
                }
              if (n==n_g2)
                {
                  d_b(i, n_g2) = (alpha(i, n_g2)>void_fraction_threshold) ? std::min(std::max(d_b(i, n_g2),D_crit),10.*D_crit) : D_crit ;
                }
            }
        }
    }

  for (i = 0; i < d_b.dimension_tot(0); i++) d_b(i, n_l) = 0.;

}

