/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Fluide_Weakly_Compressible.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Weakly_Compressible/Milieu
// Version:     /main/49
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_Weakly_Compressible.h>
#include <Loi_Etat_Multi_GP_WC.h>
#include <Neumann_sortie_libre.h>
#include <Champ_Fonc_Fonction.h>
#include <Discretisation_base.h>
#include <Schema_Temps_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Zone_Cl_dis.h>
#include <Zone_VF.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Fluide_Weakly_Compressible,"Fluide_Weakly_Compressible",Fluide_Dilatable_base);
// XD fluide_weakly_compressible fluide_dilatable_base fluide_weakly_compressible -1 Weakly-compressible flow with a low mach number assumption; this means that the thermo-dynamic pressure (used in state law) can vary in space.
// XD attr sutherland bloc_sutherland sutherland 1 Sutherland law for viscosity and for conductivity.
// XD attr beta_th suppress_param beta_th 1 suppress
// XD attr beta_co suppress_param beta_co 1 suppress
// XD attr lambda field_base lambda_u 1 Conductivity (W.m-1.K-1).
// XD attr mu field_base mu 1 Dynamic viscosity (kg.m-1.s-1).
// XD ref gravite field_base

Fluide_Weakly_Compressible::Fluide_Weakly_Compressible() : use_total_pressure_(0), use_hydrostatic_pressure_(0),
  sim_resumed_(0), time_activate_ptot_(-1.) {}

Sortie& Fluide_Weakly_Compressible::printOn(Sortie& os) const
{
  os << que_suis_je() << finl;
  Fluide_Dilatable_base::ecrire(os);
  return os;
}

Entree& Fluide_Weakly_Compressible::readOn(Entree& is)
{
  Fluide_Dilatable_base::readOn(is);
  return is;
}

void Fluide_Weakly_Compressible::set_param(Param& param)
{
  Fluide_Dilatable_base::set_param(param);
  param.ajouter("pression_thermo",&Pth_); // XD_ADD_P double Initial thermo-dynamic pressure used in the assosciated state law.
  param.ajouter("pression_xyz",&Pth_xyz_); // XD_ADD_P field_base Initial thermo-dynamic pressure used in the assosciated state law. It should be defined with as a Champ_Fonc_xyz.
  param.ajouter("use_total_pressure",&use_total_pressure_); // XD_ADD_P int Flag (0 or 1) used to activate and use the total pressure in the assosciated state law.
  param.ajouter("use_hydrostatic_pressure",&use_hydrostatic_pressure_); // XD_ADD_P int Flag (0 or 1) used to activate and use the hydro-static pressure in the assosciated state law.
  param.ajouter("time_activate_ptot",&time_activate_ptot_); // XD_ADD_P double Time (in seconds) at which the total pressure will be used in the assosciated state law.

  // Param non-standard
  param.ajouter_non_std("loi_etat",(this),Param::REQUIRED); // XD_ADD_P loi_etat_base The state law that will be associated to the Weakly-compressible fluid.
  param.ajouter_non_std("Traitement_PTh",(this));
  // Lecture de mu et lambda pas specifiee obligatoire car option sutherland possible
  // On supprime.. et on ajoute non-standard
  param.supprimer("mu");
  param.ajouter_non_std("mu",(this));
  param.ajouter_non_std("sutherland",(this));
  param.supprimer("beta_th");
  param.ajouter_non_std("beta_th",(this));
  param.supprimer("beta_co");
  param.ajouter_non_std("beta_co",(this));
}

int Fluide_Weakly_Compressible::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="Traitement_PTh")
    {
      Motcle trait;
      is >> trait;
      // TODO : FIXME : should have CONSERVATION_MASSE too !!
      Motcles les_options(1);
      {
        les_options[0] = "constant";
      }
      traitement_PTh=les_options.search(trait);
      if (traitement_PTh == -1)
        {
          Cerr<< trait << " is not understood as an option of the keyword " << motlu <<finl;
          Cerr<< "One of the following options was expected : " << les_options << finl;
          Process::exit();
        }
      return 1;
    }
  else if (mot=="loi_etat")
    {
      is>>loi_etat_;
      loi_etat_->associer_fluide(*this);
      return 1;
    }
  else if (mot=="mu")
    {
      is>>mu;
      mu->nommer("mu");
      return 1;
    }
  else if (mot=="sutherland")
    {
      double mu0,T0,C=-1;
      double Slambda=-1;

      is>>motlu;
      if (motlu!="MU0")
        {
          Cerr<<"A specification of kind : sutherland mu0 1.85e-5 T0 300 [Slambda 10] C 10 was expected."<<finl;
          Process::exit();
        }
      is>>mu0;
      is>>motlu;
      if (motlu!="T0")
        {
          Cerr<<"A specification of kind : sutherland mu0 1.85e-5 T0 300 [Slambda 10] C 10 was expected."<<finl;
          Process::exit();
        }
      is>>T0;
      is>>motlu;
      if (motlu=="SLAMBDA")
        {
          is >> Slambda;
          is >> motlu;
        }
      if (motlu!="C")
        {
          Cerr<<"A specification of kind : sutherland mu0 1.85e-5 T0 300 [Slambda 10] C 10 was expected."<<finl;
          Process::exit();
        }
      is>>C;

      mu.typer("Sutherland");
      Sutherland& mu_suth = ref_cast(Sutherland,mu.valeur());
      mu_suth.set_val_params(mu0,C,T0);
      mu_suth.lire_expression();

      //On stocke la valeur de C (ici Slambda) pour construire(cf creer_champs_non_lus())
      //la loi de Sutherland qui concerne la conductivite
      if (Slambda!=-1)
        {
          lambda.typer("Sutherland");
          Sutherland& lambda_suth = ref_cast(Sutherland,lambda.valeur());
          lambda_suth.set_Tref(T0);
          lambda_suth.set_C(Slambda);
        }
      return 1;
    }
  else if ((mot=="beta_th") || (mot=="beta_co"))
    {
      Cerr<<"The keyword "<<mot<<" has not to be read for a "<<que_suis_je()<<" type medium."<<finl;
      Cerr<<"Please remove it from your data set."<<finl;
      Process::exit();
      return -1;
    }
  else
    return Fluide_Dilatable_base::lire_motcle_non_standard(mot,is);
}

// Description:
//    Complete le fluide avec les champs inconnus associes au probleme
// Precondition:
// Parametre: Pb_Thermohydraulique& pb
//    Signification: le probleme a resoudre
//    Valeurs par defaut:
//    Contraintes:
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Fluide_Weakly_Compressible::completer(const Probleme_base& pb)
{
  Cerr<<"Fluide_Weakly_Compressible::completer" << finl;

  // XXX : currently we support only open configurations if use_total_hydro_pressure
  // THERE IS A PROBLEM HERE
  // TODO : FIXME
  if (use_total_hydro_pressure()) checkTraitementPth(pb.equation(0).zone_Cl_dis());

  if (Pth_xyz_.non_nul())
    {
      if (Pth_xyz_->que_suis_je() != "Champ_Fonc_xyz" ) // TODO : check if it is generic
        {
          Cerr << "Error in your data file : This is not allowed !"<< finl;
          Cerr << "Pression_xyz should be defined with Champ_Fonc_xyz !" << finl;
          Process::exit();
        }
      if (Pth_ != -1 || Pth_n != -1)
        {
          Cerr << "Error in your data file : This is not allowed !"<< finl;
          Cerr << "You can not specify both pression_xyz and pression_thermo !" << finl;
          Process::exit();
        }
      if (use_total_hydro_pressure())
        {
          Cerr << "Error in your data file : This is not allowed !"<< finl;
          Cerr << "You need to define pression_thermo when using the flags use_total_pressure or use_hydrostatic_pressure !" << finl;
          Process::exit();
        }
    }
  else // pression_xyz not specified in data file
    {
      if (Pth_ != -1) Pth_n = Pth_;
      else
        {
          Cerr << "Error in your data file : This is not allowed !"<< finl;
          Cerr << "You need to specify either pression_xyz or pression_thermo !" << finl;
          Process::exit();
        }

      if ( use_total_pressure_ && time_activate_ptot_==-1. )
        {
          Cerr << "Error in your data file : This is not allowed !"<< finl;
          Cerr << "You should define time_activate_ptot when using the flag use_total_pressure !" << finl;
          Process::exit();
        }

      if ( use_total_pressure_ && use_hydrostatic_pressure_ )
        {
          Cerr << "Error in your data file : This is not allowed !"<< finl;
          Cerr << "Not possible to activate the two flags use_total_pressure and use_hydrostatic_pressure at same time !" << finl;
          Process::exit();
        }

      if ( use_hydrostatic_pressure_ && !a_gravite() )
        {
          Cerr << "Error in your data file : This is not allowed !"<< finl;
          Cerr << "Not possible to activate the flag use_hydrostatic_pressure without defining a gravity vector !" << finl;
          Process::exit();
        }
    }

  // XXX : On l'a besoin pour initialiser rho ...
  if ( !use_saved_data() ) initialiser_pth_for_EOS(pb); // soit à initialiser
  else pression_eos_.valeurs() = Pth_tab_; // soit à reprendre


  Fluide_Dilatable_base::completer(pb);

  mettre_a_jour_bis();
}

void Fluide_Weakly_Compressible::mettre_a_jour_bis()
{
  assert(pression_eos_.non_nul());
  pression_eos_.mettre_a_jour(le_probleme_->schema_temps().temps_courant());

  if (pression_hydro_.non_nul())
    pression_hydro_.mettre_a_jour(le_probleme_->schema_temps().temps_courant());

  if (unsolved_species_.non_nul())
    unsolved_species_.mettre_a_jour(le_probleme_->schema_temps().temps_courant());
}

void Fluide_Weakly_Compressible::initialiser_pth_for_EOS(const Probleme_base& pb)
{
  if (use_pth_xyz()) initialiser_pth_xyz(pb);
  else initialiser_pth();

  pression_eos_.valeurs() = Pth_tab_;
}

void Fluide_Weakly_Compressible::initialiser_pth_xyz(const Probleme_base& pb)
{
  Cerr<<"Initializing the thermo-dynamic pressure Pth_xyz ..." << finl;
  int isVDF = 0; // VDF or VEF ??
  if (pb.equation(0).discretisation().que_suis_je()=="VDF") isVDF = 1;

  // We know that mu is always stored on elems and that Champ_Don rho_xyz_ is evaluated on elements
  assert (Pth_xyz_->valeurs().size() == viscosite_dynamique().valeurs().size());

  if (isVDF)
    {
      // Disc VDF => Pth_xyz_ on elems => we do nothing
      Pth_tab_=Pth_n_tab_=Pth_xyz_->valeurs();
    }
  else
    {
      // we use rho for affecter because the field is on the faces in VEF (as rho)
      Champ_base& ch_rho = masse_volumique();
      ch_rho.affecter_(Pth_xyz_);
      Pth_tab_=Pth_n_tab_=ch_rho.valeurs();
    }
}

void Fluide_Weakly_Compressible::initialiser_pth()
{
  Cerr<<"Initializing the thermo-dynamic pressure ..." << finl;
  // Pth_tab_ doit avoir meme dimensions que rho (elem en VDF et faces en VEF)
  Pth_tab_ = masse_volumique().valeurs();
  const int n = Pth_tab_.dimension_tot(0);
  for (int i=0; i<n; i++) Pth_tab_(i) = Pth_;
  Pth_n_tab_ = Pth_tab_;
}

void Fluide_Weakly_Compressible::preparer_pas_temps()
{
  Fluide_Dilatable_base::preparer_pas_temps();
}

void Fluide_Weakly_Compressible::prepare_pressure_edo()
{
  /* Do nothing */
}

void Fluide_Weakly_Compressible::write_mean_edo(double t)
{
  /* Do nothing */
}

void Fluide_Weakly_Compressible::checkTraitementPth(const Zone_Cl_dis& zone_cl)
{
  // TODO : FIXME : we should be able to use this in closed cavities too !!
  int pression_imposee=0;
  int size=zone_cl.les_conditions_limites().size();
  assert(size!=0);
  for (int n=0; n<size; n++)
    {
      const Cond_lim& la_cl = zone_cl.les_conditions_limites(n);
      if (sub_type(Neumann_sortie_libre, la_cl.valeur())) pression_imposee=1;
    }

  if (!pression_imposee)
    {
      Cerr << "At least one open boundary is currently required for the weakly compressible problem !" << finl;
      Process::exit();
    }
}

void Fluide_Weakly_Compressible::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  const Zone_dis_base& zone_dis=pb.equation(0).zone_dis();
  double temps=pb.schema_temps().temps_courant();

  // In *_Melange_Binaire_WC we do not even have a temperature variable ...
  // it is the species mass fraction Y1... Although named ch_temperature
  Champ_Don& ch_TK = ch_temperature();
  if (pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_WC" || pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_Turbulent_WC")
    dis.discretiser_champ("temperature",zone_dis,"fraction_massique","neant",1,temps,ch_TK);
  else
    dis.discretiser_champ("temperature",zone_dis,"temperature","K",1,temps,ch_TK);

  if (type_fluide()!="Gaz_Parfait")
    loi_etat()->champs_compris().ajoute_champ(ch_TK);

  Fluide_Dilatable_base::discretiser(pb,dis);

  // XXX XXX : Champs pour WC : comme la temperature car elem en VDF et faces en VEF
  Champ_Don& phydro = pression_hydro();
  dis.discretiser_champ("temperature",zone_dis,"pression_hydro","Pa",1,temps,phydro);
  champs_compris_.ajoute_champ(phydro);

  Champ_Don& peos = pression_eos();
  dis.discretiser_champ("temperature",zone_dis,"pression_eos","Pa",1,temps,peos);
  champs_compris_.ajoute_champ(peos);

  // Seulement pour multi-especes
  if (pb.que_suis_je()=="Pb_Thermohydraulique_Especes_WC")
    {
      Champ_Don& yn = fraction_massique_nonresolue();
      dis.discretiser_champ("temperature",zone_dis,"fraction_massique_nonresolue","neant",1,temps,yn);
      champs_compris_.ajoute_champ(yn);
    }
}

void Fluide_Weakly_Compressible::abortTimeStep()
{
  Fluide_Dilatable_base::abortTimeStep();
  Pth_tab_ = Pth_n_tab_;
}

void Fluide_Weakly_Compressible::update_pressure_fields(double temps)
{
  Pth_n_tab_= Pth_tab_; // previous dt
  Fluide_Dilatable_base::update_pressure_fields(temps);
  if (a_gravite())
    {
      calculer_pression_hydro();
      pression_hydro_.mettre_a_jour(temps);
    }
  // for post-processing
  pression_eos_.valeurs() = Pth_tab_;
  pression_eos_.mettre_a_jour(temps);
}

// Description:
//    Calcule la pression totale : pression thermodynamique + pression hydrodynamique
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Fluide_Weakly_Compressible::calculer_pression_tot()
{
  Fluide_Dilatable_base::calculer_pression_tot();
  if (use_total_hydro_pressure()) remplir_champ_pression_for_EOS();
}

void Fluide_Weakly_Compressible::remplir_champ_pression_tot(int n, const DoubleTab& PHydro, DoubleTab& PTot)
{
  P_NS_ = PHydro;
  if ( use_pth_xyz() )
    for (int i=0 ; i<n ; i++) PTot(i,0) = PHydro(i,0) + Pth_tab_(i,0);
  else
    for (int i=0 ; i<n ; i++) PTot(i,0) = PHydro(i,0) + Pth_;
}

// Description:
//    Calcule la pression hydrostatique = rho*g*z
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Fluide_Weakly_Compressible::calculer_pression_hydro()
{
  DoubleTab& tab_Phydro = pression_hydro_.valeurs();
  const Zone_dis_base& zone_dis= pression_-> zone_dis_base();
  const Zone_VF& zone = ref_cast(Zone_VF, zone_dis);
  int is_VDF = zone_dis.que_suis_je() == "Zone_VDF" ? 1 : 0;
  const DoubleTab& centres_de_gravites = is_VDF ? zone.xp() : zone.xv();
  const DoubleTab& tab_rho = rho_np1(), grav = gravite().valeurs();
  const int n = tab_Phydro.dimension_tot(0);
  assert (n ==  tab_rho.dimension_tot(0) && a_gravite());
  for (int i=0 ; i<n ; i++)
    {
      double gz=0.;
      for (int dir=0; dir<dimension; dir++) gz+=centres_de_gravites(i,dir)*grav(0,dir);

      tab_Phydro(i) =  tab_rho(i)*gz;
    }
  if (use_total_hydro_pressure()) remplir_champ_pression_for_EOS();
}

// Description:
//    Calcule la pression utilisee dans les lois d'etat WC
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Fluide_Weakly_Compressible::remplir_champ_pression_for_EOS()
{
  if (use_total_pressure())
    {
      double t0= le_probleme_->schema_temps().temps_courant(), t1 = le_probleme_->schema_temps().temps_futur(1);
      // time to activate ptot and change the eos pressure
      if ( t0 < time_activate_ptot_ && t1 > time_activate_ptot_ )
        for (int i=0 ; i<Pth_tab_.dimension_tot(0) ; i++) Pth_tab_(i,0) = P_NS_(i,0) + Pth_;
    }
  else if (use_hydrostatic_pressure())
    {
      if ( le_probleme_->schema_temps().nb_pas_dt() ==0 && use_saved_data() ) { /* do nothing &  use saved data */ }
      else
        {
          Pth_tab_ = pression_hydro_.valeurs();
          const int n = Pth_tab_.dimension_tot(0);
          for (int i=0 ; i<n ; i++) Pth_tab_(i) += Pth_; // present dt
        }
    }
  else
    {
      Cerr << "The method Fluide_Weakly_Compressible::remplir_champ_pression_for_EOS() should not be called !" << finl;
      Process::exit();
    }
}
