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

// By default we do not use the total pressure
Fluide_Weakly_Compressible::Fluide_Weakly_Compressible() : use_total_pressure_(0) {}

// Description:
//    Ecrit les proprietes du fluide sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
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
Sortie& Fluide_Weakly_Compressible::printOn(Sortie& os) const
{
  os << que_suis_je() << finl;
  Fluide_Dilatable_base::ecrire(os);
  return os;
}


// Description:
//   Lit les caracteristiques du fluide a partir d'un flot
//   d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition:
Entree& Fluide_Weakly_Compressible::readOn(Entree& is)
{
  Fluide_Dilatable_base::readOn(is);
  return is;
}

void Fluide_Weakly_Compressible::set_param(Param& param)
{
  Fluide_Dilatable_base::set_param(param);
  param.ajouter_non_std("pression_xyz",(this),Param::REQUIRED);
  param.ajouter_non_std("loi_etat",(this),Param::REQUIRED);
  //Lecture de mu et lambda pas specifiee obligatoire car option sutherland possible
  param.supprimer("mu");
  param.ajouter_non_std("mu",(this));
  param.ajouter_non_std("sutherland",(this));
  param.ajouter_non_std("Traitement_PTh",(this));
  param.ajouter_non_std("use_total_pressure",(this));
  param.supprimer("beta_th");
  param.ajouter_non_std("beta_th",(this));
  param.supprimer("beta_co");
  param.ajouter_non_std("beta_co",(this));

}

int Fluide_Weakly_Compressible::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="pression_xyz")
    {
      is>>Pth_xyz_;
      if (Pth_xyz_.valeur().que_suis_je() != "Champ_Fonc_xyz" )
        {
          Cerr << "Pression_xyz should be defined with Champ_Fonc_xyz !" << finl;
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
  else if (mot=="Traitement_PTh")
    {
      Motcle trait;
      is >> trait;
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
  else if (mot=="use_total_pressure")
    {
      is >> use_total_pressure_;
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
  initialiser_pth_xyz(pb);
  Fluide_Dilatable_base::completer(pb);
}

void Fluide_Weakly_Compressible::initialiser_pth_xyz(const Probleme_base& pb)
{
  Cerr<<"Initializing the thermo-dynamic pressure Pth_xyz ..." << finl;
  assert(Pth_xyz_.valeur().que_suis_je() == "Champ_Fonc_xyz" );
  assert (Pth_ == -1. && Pth_n == -1.); // default by constructor

  // VDF or VEF ??
  int isVDF = 0;
  if (pb.equation(0).discretisation().que_suis_je()=="VDF") isVDF = 1;

  // We know that mu is always stored on elems and that Champ_Don rho_xyz_ is evaluated on elements
  assert (Pth_xyz_.valeur().valeurs().size() == viscosite_dynamique().valeurs().size());

  if (isVDF)
    {
      // Disc VDF => Pth_xyz_ on elems => we do nothing
      Pth_tab_=Pth_n_tab_=Pth_xyz_.valeur().valeurs();
    }
  else
    {
      // we use rho for affecter because the field is on the faces in VEF (as rho)
      Champ_base& ch_rho = masse_volumique();
      ch_rho.affecter_(Pth_xyz_);
      Pth_tab_=Pth_n_tab_=ch_rho.valeurs();
    }
}

// Description:
//    Prepare le pas de temps
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
    loi_etat().valeur().champs_compris().ajoute_champ(ch_TK);

  Fluide_Dilatable_base::discretiser(pb,dis);
}

void Fluide_Weakly_Compressible::abortTimeStep()
{
  loi_etat()->abortTimeStep();
  Pth_tab_=Pth_n_tab_;
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
  DoubleTab& tab_Ptot = pression_tot_.valeurs();
  const int n = tab_Ptot.dimension_tot(0);
  DoubleTab tab_PHyd(n, 1);
  if( n != pression_->valeurs().dimension_tot(0) )
    {
      // Interpolation de pression_ aux elements (ex: P1P0)
      const Zone_dis_base& zone_dis= pression_-> zone_dis_base();
      const Zone_VF& zone = ref_cast(Zone_VF, zone_dis);
      const DoubleTab& centres_de_gravites=zone.xp();
      pression_->valeur().valeur_aux(centres_de_gravites,tab_PHyd);
    }
  else tab_PHyd = pression_->valeurs();

  for (int i=0 ; i<n ; i++) tab_Ptot(i,0) = tab_PHyd(i,0) + Pth_tab_(i,0);

  if (use_total_pressure_) Pth_tab_= tab_Ptot; // present dt
}

void Fluide_Weakly_Compressible::update_pressure_fields(double temps)
{
  Pth_n_tab_= Pth_tab_; // previous dt
  Fluide_Dilatable_base::update_pressure_fields(temps);
}

