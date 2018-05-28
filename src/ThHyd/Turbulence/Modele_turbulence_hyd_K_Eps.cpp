/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Modele_turbulence_hyd_K_Eps.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/52
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_turbulence_hyd_K_Eps.h>
#include <Probleme_base.h>
#include <Debog.h>
#include <Modifier_nut_pour_QC.h>
#include <Schema_Temps_base.h>
#include <Schema_Temps.h>
#include <stat_counters.h>
#include <Modele_turbulence_scal_base.h>
#include <Param.h>
#include <communications.h>
#include <Fluide_Incompressible.h>
#include <DoubleTrav.h>
#include <Champ_Uniforme.h>

Implemente_instanciable(Modele_turbulence_hyd_K_Eps,"Modele_turbulence_hyd_K_Epsilon",Mod_turb_hyd_RANS);


// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Modele_turbulence_hyd_K_Eps::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


// Description:
//    Simple appel a Mod_turb_hyd_RANS::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Modele_turbulence_hyd_K_Eps::readOn(Entree& s )
{
  return Mod_turb_hyd_RANS::readOn(s); // XD k_epsilon mod_turb_hyd_rans k_epsilon -1 Turbulence model (k-eps).
}

void Modele_turbulence_hyd_K_Eps::set_param(Param& param)
{
  Mod_turb_hyd_RANS::set_param(param);
  param.ajouter_non_std("Transport_K_Epsilon",(this),Param::REQUIRED); // XD_ADD_P transport_k_epsilon Keyword to define the (k-eps) transportation equation.
  param.ajouter_non_std("Modele_Fonc_Bas_Reynolds",(this)); // XD_ADD_P modele_fonction_bas_reynolds_base This keyword is used to set the bas Reynolds model used.
  param.ajouter("CMU",&LeCmu); // XD_ADD_P double Keyword to modify the Cmu constant of k-eps model : Nut=Cmu*k*k/eps Default value is 0.09
  param.ajouter("PRANDTL_K",&Prandtl_K); // XD_ADD_P double Keyword to change the Prk value (default 1.0).
  param.ajouter("PRANDTL_EPS",&Prandtl_Eps); // XD_ADD_P double Keyword to change the Pre value (default 1.3).
}

int Modele_turbulence_hyd_K_Eps::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="Transport_K_Epsilon")
    {
      eqn_transp_K_Eps().associer_modele_turbulence(*this);
      is >> eqn_transp_K_Eps();
      return 1;
    }
  else if (mot=="Modele_Fonc_Bas_Reynolds")
    {
      Cerr << "Lecture du modele bas reynolds associe " << finl;
      mon_modele_fonc.associer_eqn(eqn_transp_K_Eps());
      is >> mon_modele_fonc;
      Cerr << "mon_modele_fonc.que_suis_je() avant discretisation " << mon_modele_fonc.que_suis_je() << finl;
      mon_modele_fonc.valeur().discretiser();
      Cerr << "mon_modele_fonc.que_suis_je() " << mon_modele_fonc.valeur().que_suis_je() << finl;
      mon_modele_fonc.valeur().lire_distance_paroi();
      return 1;
    }
  else
    return Mod_turb_hyd_RANS::lire_motcle_non_standard(mot,is);
  return 1;
}

// Description:
//    Calcule la viscosite turbulente au temps demande.
// Precondition:
// Parametre: double temps
//    Signification: le temps auquel il faut calculer la viscosite
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Fonc&
//    Signification: la viscosite turbulente au temps demande
//    Contraintes:
// Exception: erreur de taille de visco_turb_K_eps
// Effets de bord:
// Postcondition:
Champ_Fonc& Modele_turbulence_hyd_K_Eps::calculer_viscosite_turbulente(double temps)
{
  const Champ_base& chK_Eps=eqn_transp_K_Eps().inconnue().valeur();
  Nom type=chK_Eps.que_suis_je();
  const Zone_Cl_dis& la_zone_Cl_dis = eqn_transp_K_Eps().zone_Cl_dis();
  const DoubleTab& tab_K_Eps = chK_Eps.valeurs();
  DoubleTab& visco_turb =  la_viscosite_turbulente.valeurs();

  DoubleTab Cmu(tab_K_Eps.dimension_tot(0)) ;

  // K_Eps(i,0) = K au noeud i
  // K_Eps(i,1) = Epsilon au noeud i
  int n = tab_K_Eps.dimension(0);

  DoubleTrav Fmu,D(tab_K_Eps.dimension_tot(0));
  D=0;
  int is_modele_fonc=(mon_modele_fonc.non_nul());
  // is_modele_fonc=0;
  if (is_modele_fonc)
    {
      // pour avoir nu en incompressible et mu en QC
      // et non comme on a divise K et eps par rho (si on est en QC)
      // on veut toujours nu
      const Champ_Don ch_visco=ref_cast(Fluide_Incompressible,eqn_transp_K_Eps().milieu()).viscosite_cinematique();
      const Champ_Don& ch_visco_cin =ref_cast(Fluide_Incompressible,eqn_transp_K_Eps().milieu()).viscosite_cinematique();
      // const Champ_Don& ch_visco_cin_ou_dyn =((const Op_Diff_K_Eps&) eqn_transp_K_Eps().operateur(0)).diffusivite();

      const DoubleTab& tab_visco = ch_visco_cin->valeurs();
      //      const DoubleTab& tab_visco = ch_visco.valeurs();
      Fmu.resize(tab_K_Eps.dimension_tot(0));
      const Zone_dis& la_zone_dis = eqn_transp_K_Eps().zone_dis();

      mon_modele_fonc.Calcul_Fmu( Fmu,la_zone_dis,la_zone_Cl_dis,tab_K_Eps,ch_visco);
      /*const DoubleTab& vit = eqn_transp_K_Eps().probleme().equation(0).inconnue().valeurs();
      D=Fmu;
      D=0;
      if (0)
        mon_modele_fonc.Calcul_D(D,la_zone_dis,eqn_transp_K_Eps().zone_Cl_dis(),vit,tab_K_Eps,ch_visco_cin);
       */
      int is_Cmu_constant = mon_modele_fonc.Calcul_is_Cmu_constant();
      if (is_Cmu_constant==0)
        {
          Cerr<< " On utilise un Cmu non constant "<< finl;
          const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
          mon_modele_fonc.Calcul_Cmu(Cmu, la_zone_dis, la_zone_Cl_dis,
                                     vitesse, tab_K_Eps, LeEPS_MIN);

          /*Paroi*/
          Nom lp=eqn_transp_K_Eps().modele_turbulence().loi_paroi().valeur().que_suis_je();
          if (lp!="negligeable_VEF")
            {
              DoubleTab visco_tab(visco_turb.dimension_tot(0));
              assert(sub_type(Champ_Uniforme,ch_visco_cin.valeur()));
              visco_tab = tab_visco(0,0);
              const int idt =  mon_equation->schema_temps().nb_pas_dt();
              const DoubleTab& tab_paroi = loi_paroi().valeur().Cisaillement_paroi();
              mon_modele_fonc.Calcul_Cmu_Paroi(Cmu, la_zone_dis, la_zone_Cl_dis,visco_tab, visco_turb, tab_paroi, idt,
                                               vitesse, tab_K_Eps, LeEPS_MIN);
            }
        }
      else
        Cerr<< " On utilise un Cmu constant "<< finl;

    }


  // dans le cas d'une zone nulle on doit effectuer le dimensionnement
  double non_prepare=1;
  Debog::verifier("Modele_turbulence_hyd_K_Eps::calculer_viscosite_turbulente la_viscosite_turbulente before",la_viscosite_turbulente.valeurs());
  Debog::verifier("Modele_turbulence_hyd_K_Eps::calculer_viscosite_turbulente tab_K_Eps",tab_K_Eps);
  if (visco_turb.size() == n)
    non_prepare=0.;
  non_prepare=mp_max(non_prepare);

  if (non_prepare==1)
    {
      Champ_Inc visco_turb_au_format_K_eps;

      visco_turb_au_format_K_eps.typer(type);
      Champ_Inc_base& ch_visco_turb_K_eps=visco_turb_au_format_K_eps.valeur();
      ch_visco_turb_K_eps.associer_zone_dis_base(eqn_transp_K_Eps().zone_dis().valeur());
      ch_visco_turb_K_eps.nommer("diffusivite_turbulente");
      ch_visco_turb_K_eps.fixer_nb_comp(1);
      ch_visco_turb_K_eps.fixer_nb_valeurs_nodales(n);
      ch_visco_turb_K_eps.fixer_unite("inconnue");
      ch_visco_turb_K_eps.changer_temps(0.);
      DoubleTab& visco_turb_K_eps =  ch_visco_turb_K_eps.valeurs();

      if(visco_turb_K_eps.size() != n)
        {
          Cerr << "visco_turb_K_eps size is " << visco_turb_K_eps.size()
               << " instead of " << n << finl;
          exit();
        }
      // A la fin de cette boucle, le tableau visco_turb_K_eps
      // contient les valeurs de la viscosite turbulente
      // au centre des faces du maillage.
      // Debog::verifier("Modele_turbulence_hyd_K_Eps::calculer_viscosite_turbulente visco_turb_K_eps before",visco_turb_K_eps);
      for (int i=0; i<n; i++)
        {
          if (tab_K_Eps(i,1) <= LeEPS_MIN)
            visco_turb_K_eps[i] = 0;
          else
            {
              if (is_modele_fonc)
                {
                  int is_Cmu_constant = mon_modele_fonc.Calcul_is_Cmu_constant();
                  if (is_Cmu_constant)
                    visco_turb_K_eps[i] = Fmu(i)*LeCmu*tab_K_Eps(i,0)*tab_K_Eps(i,0)/(tab_K_Eps(i,1)+D(i));
                  else
                    visco_turb_K_eps[i] = Fmu(i)*Cmu(i)*tab_K_Eps(i,0)*tab_K_Eps(i,0)/(tab_K_Eps(i,1)+D(i));
                }
              else
                visco_turb_K_eps[i] = LeCmu*tab_K_Eps(i,0)*tab_K_Eps(i,0)/(tab_K_Eps(i,1)+D(i));
            }
        }
      // Debog::verifier("Modele_turbulence_hyd_K_Eps::calculer_viscosite_turbulente visco_turb_K_eps after",visco_turb_K_eps);

      // On connait donc la viscosite turbulente au centre des faces de chaque element
      // On cherche maintenant a interpoler cette viscosite turbulente au centre des
      // elements.
      la_viscosite_turbulente->affecter(visco_turb_au_format_K_eps.valeur());
      Debog::verifier("Modele_turbulence_hyd_K_Eps::calculer_viscosite_turbulente visco_turb_au_format_K_eps",visco_turb_au_format_K_eps.valeur());
    }
  else
    {
      for (int i=0; i<n; i++)
        {
          if (tab_K_Eps(i,1) <= LeEPS_MIN)
            {
              visco_turb[i] = 0;
            }
          else
            {
              if (is_modele_fonc)
                {
                  int is_Cmu_constant = mon_modele_fonc.Calcul_is_Cmu_constant();
                  if (is_Cmu_constant)
                    visco_turb[i] =Fmu(i)*LeCmu*tab_K_Eps(i,0)*tab_K_Eps(i,0)/(tab_K_Eps(i,1)+D(i));
                  else
                    {
                      visco_turb[i] =Fmu(i)*Cmu(i)*tab_K_Eps(i,0)*tab_K_Eps(i,0)/(tab_K_Eps(i,1)+D(i));
                    }
                }
              else
                visco_turb[i] = LeCmu*tab_K_Eps(i,0)*tab_K_Eps(i,0)/(tab_K_Eps(i,1)+D(i));
            }
        }
    }

  la_viscosite_turbulente.changer_temps(temps);
  Debog::verifier("Modele_turbulence_hyd_K_Eps::calculer_viscosite_turbulente la_viscosite_turbulente after",la_viscosite_turbulente.valeurs());
  return la_viscosite_turbulente;
}

void imprimer_evolution_keps(const Champ_Inc& le_champ_K_Eps, const Schema_Temps_base& sch, double LeCmu, int avant)
{
  if (sch.nb_pas_dt()==0 || sch.limpr())
    {
      const DoubleTab& K_Eps = le_champ_K_Eps.valeurs();
      double k_min=DMAXFLOAT;
      double eps_min=DMAXFLOAT;
      double nut_min=DMAXFLOAT;
      double k_max=0;
      double eps_max=0;
      double nut_max=0;
      int loc_k_min=-1;
      int loc_eps_min=-1;
      int loc_nut_min=-1;
      int loc_k_max=-1;
      int loc_eps_max=-1;
      int loc_nut_max=-1;
      int size=K_Eps.dimension(0);
      for (int n=0; n<size; n++)
        {
          const double& k = K_Eps(n,0);
          const double& eps = K_Eps(n,1);
          double nut = 0;
          if (eps > 0) nut = LeCmu*k*k/eps;
          if (k < k_min)
            {
              k_min = k;
              loc_k_min = n;
            }
          else if (k > k_max)
            {
              k_max = k;
              loc_k_max = n;
            }
          if (eps < eps_min)
            {
              eps_min = eps;
              loc_eps_min = n;
            }
          else if (eps > eps_max)
            {
              eps_max = eps;
              loc_eps_max = n;
            }
          if (nut < nut_min)
            {
              nut_min = nut;
              loc_nut_min = n;
            }
          else if (nut > nut_max)
            {
              nut_max = nut;
              loc_nut_max = n;
            }
        }
      /*
      k_min = Process::mp_min(k_min);
      eps_min = Process::mp_min(eps_min);
      nut_min = Process::mp_min(nut_min);
      k_max = Process::mp_max(k_max);
      eps_max = Process::mp_max(eps_max);
      nut_max = Process::mp_max(nut_max);
       */
      ArrOfDouble values(3);

      values(0)=k_min;
      values(1)=eps_min;
      values(2)=nut_min;
      mp_min_for_each_item(values);
      k_min=values(0);
      eps_min=values(1);
      nut_min=values(2);

      values(0)=k_max;
      values(1)=eps_max;
      values(2)=nut_max;
      mp_max_for_each_item(values);
      k_max=values(0);
      eps_max=values(1);
      nut_max=values(2);
      if (Process::je_suis_maitre())
        {
          Cout << finl << "K_Eps evolution (" << (avant?"before":"after") << " law of the wall applies) at time " << le_champ_K_Eps.temps() << ":" << finl;
          Cout << "min(k)=" << k_min;
          if (Process::nproc()==1) Cout << " located at node " << loc_k_min;
          Cout << finl;
          Cout << "min(eps)=" << eps_min;
          if (Process::nproc()==1) Cout << " located at node " << loc_eps_min;
          Cout << finl;
          Cout << "min(nut)=" << nut_min;
          if (Process::nproc()==1) Cout << " located at node " << loc_nut_min;
          Cout << finl;
          Cout << "max(k)=" << k_max;
          if (Process::nproc()==1) Cout << " located at node " << loc_k_max;
          Cout << finl;
          Cout << "max(eps)=" << eps_max;
          if (Process::nproc()==1) Cout << " located at node " << loc_eps_max;
          Cout << finl;
          Cout << "max(nut)=" << nut_max;
          if (Process::nproc()==1) Cout << " located at node " << loc_nut_max;
          Cout << finl;
        }
    }
}

int Modele_turbulence_hyd_K_Eps::preparer_calcul()
{
  eqn_transp_K_Eps().preparer_calcul();
  Mod_turb_hyd_base::preparer_calcul();
  // GF pour initialiser la loi de paroi thermique en TBLE
  if (equation().probleme().nombre_d_equations()>1)
    {
      const RefObjU& modele_turbulence = equation().probleme().equation(1).get_modele(TURBULENCE);
      if (sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()))
        {
          Turbulence_paroi_scal& loi_paroi_T = ref_cast_non_const(Modele_turbulence_scal_base,modele_turbulence.valeur()).loi_paroi();
          loi_paroi_T.init_lois_paroi();
        }
    }
  // GF quand on demarre un calcul il est bon d'utliser la ldp
  // encore plus quand on fait une reprise !!!!!!!!
  Champ_Inc& ch_K_Eps = K_Eps();

  const Milieu_base& mil=equation().probleme().milieu();
  diviser_par_rho_si_qc(ch_K_Eps.valeurs(),mil);
  imprimer_evolution_keps(ch_K_Eps,eqn_transp_K_Eps().schema_temps(),LeCmu,1);
  loipar.calculer_hyd(ch_K_Eps);
  eqn_transp_K_Eps().controler_K_Eps();
  calculer_viscosite_turbulente(ch_K_Eps.temps());
  limiter_viscosite_turbulente();
  // on remultiplie K_eps par rho
  multiplier_par_rho_si_qc(ch_K_Eps.valeurs(),mil);
  Correction_nut_et_cisaillement_paroi_si_qc(*this);
  la_viscosite_turbulente.valeurs().echange_espace_virtuel();
  Debog::verifier("Modele_turbulence_hyd_K_Eps::preparer_calcul la_viscosite_turbulente",la_viscosite_turbulente.valeurs());
  imprimer_evolution_keps(ch_K_Eps,eqn_transp_K_Eps().schema_temps(),LeCmu,0);
  return 1;
}

bool Modele_turbulence_hyd_K_Eps::initTimeStep(double dt)
{
  return eqn_transport_K_Eps.initTimeStep(dt);
}

// Description:
//    Effectue une mise a jour en temps du modele de turbulence.
//    Met a jour l'equation de transport K-epsilon,
//    calcule la loi de paroi et la viscosite turbulente
//    au nouveau temps.
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Modele_turbulence_hyd_K_Eps::mettre_a_jour(double temps)
{
  Champ_Inc& ch_K_Eps = K_Eps();
  Schema_Temps_base& sch =eqn_transp_K_Eps().schema_temps();
  // Voir Schema_Temps_base::faire_un_pas_de_temps_pb_base
  eqn_transp_K_Eps().zone_Cl_dis().mettre_a_jour(temps);
  if (!eqn_transp_K_Eps().equation_non_resolue())
    sch.faire_un_pas_de_temps_eqn_base(eqn_transp_K_Eps());
  eqn_transp_K_Eps().mettre_a_jour(temps);

  statistiques().begin_count(nut_counter_);
  const Milieu_base& mil=equation().probleme().milieu();
  Debog::verifier("Modele_turbulence_hyd_K_Eps::mettre_a_jour la_viscosite_turbulente before",la_viscosite_turbulente.valeurs());
  // on divise K_eps par rho en QC pour revenir a K et Eps
  diviser_par_rho_si_qc(ch_K_Eps.valeurs(),mil);
  imprimer_evolution_keps(ch_K_Eps,eqn_transp_K_Eps().schema_temps(),LeCmu,1);
  loipar.calculer_hyd(ch_K_Eps);
  eqn_transp_K_Eps().controler_K_Eps();
  calculer_viscosite_turbulente(ch_K_Eps.temps());
  limiter_viscosite_turbulente();
  // on remultiplie K_eps par rho
  multiplier_par_rho_si_qc(ch_K_Eps.valeurs(),mil);
  Correction_nut_et_cisaillement_paroi_si_qc(*this);
  la_viscosite_turbulente.valeurs().echange_espace_virtuel();
  Debog::verifier("Modele_turbulence_hyd_K_Eps::mettre_a_jour la_viscosite_turbulente after",la_viscosite_turbulente.valeurs());
  imprimer_evolution_keps(ch_K_Eps,eqn_transp_K_Eps().schema_temps(),LeCmu,0);
  statistiques().end_count(nut_counter_);
}

const Equation_base& Modele_turbulence_hyd_K_Eps::equation_k_eps(int i) const
{
  assert ((i==0));
  return eqn_transport_K_Eps;

}
const Champ_base&  Modele_turbulence_hyd_K_Eps::get_champ(const Motcle& nom) const
{

  try
    {
      return Mod_turb_hyd_RANS::get_champ(nom);
    }
  catch (Champs_compris_erreur)
    {
    }
  if (mon_modele_fonc.non_nul())
    {
      try
        {
          return  mon_modele_fonc.valeur().get_champ(nom);
        }
      catch (Champs_compris_erreur)
        {
        }
    }
  throw Champs_compris_erreur();

}
void Modele_turbulence_hyd_K_Eps::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Mod_turb_hyd_RANS::get_noms_champs_postraitables(nom,opt);
  if (mon_modele_fonc.non_nul())
    mon_modele_fonc.valeur().get_noms_champs_postraitables(nom,opt);

}
void Modele_turbulence_hyd_K_Eps::verifie_loi_paroi()
{
  Nom lp=loipar.valeur().que_suis_je();
  if (lp=="negligeable_VEF" || lp=="negligeable_VDF")
    if (!associe_modele_fonction().non_nul())
      {
        Cerr<<"The turbulence model of type "<<que_suis_je()<<finl;
        Cerr<<"must not be used with a wall law of type negligeable or with a modele_function."<<finl;
        Cerr<<"Another wall law must be selected with this kind of turbulence model."<<finl;
      }
}
