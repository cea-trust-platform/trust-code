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
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Weakly_Compressible/Milieu
// Version:     /main/49
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_Weakly_Compressible.h>
#include <Champ_Fonc_Fonction.h>
#include <Neumann_sortie_libre.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Schema_Temps_base.h>
#include <Discretisation_base.h>
#include <Zone_Cl_dis.h>
#include <Zone_VF.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Fluide_Weakly_Compressible,"Fluide_Weakly_Compressible",Fluide_Dilatable_base);

Fluide_Weakly_Compressible::Fluide_Weakly_Compressible() : use_total_pressure_(0), use_hydrostatic_pressure_(0),
  nb_pas_dt_pression_(-1), sim_resumed_(0) {}

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
  param.ajouter("pression_thermo",&Pth_);
  param.ajouter("pression_xyz",&Pth_xyz_);
  param.ajouter("use_total_pressure",&use_total_pressure_);
  param.ajouter("use_hydrostatic_pressure",&use_hydrostatic_pressure_);
  param.ajouter("nb_pas_dt_pression",&nb_pas_dt_pression_);

  // Param non-standard
  param.ajouter_non_std("loi_etat",(this),Param::REQUIRED);
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
  le_probleme_ = pb;
  // XXX : currently we support only open configurations
  checkTraitementPth(le_probleme_->equation(0).zone_Cl_dis());

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

      if ( use_total_pressure_ && nb_pas_dt_pression_==-1 )
        {
          Cerr << "Error in your data file : This is not allowed !"<< finl;
          Cerr << "You should define nb_pas_dt_pression when using the flag use_total_pressure !" << finl;
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

  if ( !use_saved_data() ) initialiser_pth_for_EOS(pb); // On l'a besoin pour initialiser rho ...
  Fluide_Dilatable_base::completer(pb);
}

void Fluide_Weakly_Compressible::initialiser_pth_for_EOS(const Probleme_base& pb)
{
  if (use_pth_xyz()) initialiser_pth_xyz();
  else initialiser_pth();
}

void Fluide_Weakly_Compressible::initialiser_pth_xyz()
{
  Cerr<<"Initializing the thermo-dynamic pressure Pth_xyz ..." << finl;
  int isVDF = 0; // VDF or VEF ??
  if (le_probleme_->equation(0).discretisation().que_suis_je()=="VDF") isVDF = 1;

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

  Champ_Don& phydro = pression_hydro();
  // XXX XXX : comme la temperature car elem en VDF et faces en VEF
  dis.discretiser_champ("temperature",zone_dis,"pression_hydro","Pa",1,temps,phydro);
  champs_compris_.ajoute_champ(phydro);

  Fluide_Dilatable_base::discretiser(pb,dis);
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
  // XXX : Recall that this field will be used in the EOS.. So its not just a field that varies at each dt !!!
  if ( use_pth_xyz() )
    {
      // here its just a post-processing field and not used in EOS (pth_xyzis used)
      for (int i=0 ; i<n ; i++) PTot(i,0) = PHydro(i,0) + Pth_tab_(i,0);
    }
  else
    {
      if ( use_total_pressure() )
        {
          // TODO : FIXME : Should not be like that
          if (le_probleme_->schema_temps().nb_pas_dt() == nb_pas_dt_pression_  && !use_saved_data() )
            for (int i=0 ; i<n ; i++) PTot(i,0) = PHydro(i,0) + Pth_;
          else PTot = Pth_tab_;
        }
      else
        for (int i=0 ; i<n ; i++) PTot(i,0) = PHydro(i,0) + Pth_;
    }
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
  const DoubleTab& centres_de_gravites=zone.xp();
  const DoubleTab& tab_rho = rho_np1();
  const int n = tab_Phydro.dimension_tot(0);
  assert (n ==  tab_rho.dimension_tot(0)); // TODO FIXME : VEF !! Now only VDF
  assert (a_gravite());
  const DoubleTab& grav = gravite().valeurs();
  for (int i=0 ; i<n ; i++)
    {
      double gz=0.;
      for (int dir=0; dir<dimension; dir++) gz+=centres_de_gravites(i,dir)*grav(0,dir);

      tab_Phydro(i) =  tab_rho(i)*gz;
    }
  if (use_total_hydro_pressure()) remplir_champ_pression_for_EOS();
}

void Fluide_Weakly_Compressible::remplir_champ_pression_for_EOS()
{
  if (use_total_pressure()) Pth_tab_= pression_tot_.valeurs(); // present dt
  else if (use_hydrostatic_pressure())
    {
      if ( le_probleme_->schema_temps().nb_pas_dt() ==0 && use_saved_data() ) { }
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
