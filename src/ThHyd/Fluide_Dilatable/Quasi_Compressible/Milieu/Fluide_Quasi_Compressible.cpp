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
// File:        Fluide_Quasi_Compressible.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Milieu
// Version:     /main/49
//
//////////////////////////////////////////////////////////////////////////////

#include <Fluide_Quasi_Compressible.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Champ_Fonc_Tabule.h>
#include <EChaine.h>
#include <Discretisation_base.h>
#include <Loi_Etat_Melange_GP.h>
#include <Zone_Cl_dis.h>
#include <SFichier.h>
#include <Param.h>
#include <Champ_Fonc_Fonction.h>
#include <Neumann_sortie_libre.h>
#include <Zone_VF.h>

Implemente_instanciable_sans_constructeur(Fluide_Quasi_Compressible,"Fluide_Quasi_Compressible",Fluide_Dilatable_base);

Fluide_Quasi_Compressible::Fluide_Quasi_Compressible() : traitement_rho_gravite_(0),
  temps_debut_prise_en_compte_drho_dt_(-DMAXFLOAT),omega_drho_dt_(1.) { }

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
Sortie& Fluide_Quasi_Compressible::printOn(Sortie& os) const
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
Entree& Fluide_Quasi_Compressible::readOn(Entree& is)
{
  Fluide_Dilatable_base::readOn(is);
  return is;
}


void Fluide_Quasi_Compressible::checkTraitementPth(const Zone_Cl_dis& zone_cl)
{
  /*
   * traitement_PTh=0 => resolution classique de l'edo
   * traitement_PTh=1 => pression calculee pour conserver la masse
   * traitement_PTh=2 => pression laissee cste.
   */

  // Pas de verification si EDO choisi (traitement_PTh=0)
  if (traitement_PTh==0) return;
  int pression_imposee=0;
  int size=zone_cl.les_conditions_limites().size();
  assert(size!=0);
  for (int n=0; n<size; n++)
    {
      const Cond_lim& la_cl = zone_cl.les_conditions_limites(n);
      if (sub_type(Neumann_sortie_libre, la_cl.valeur()))
        {
          pression_imposee=1;
        }
    }
  if (pression_imposee && traitement_PTh!=2)
    {
      if (Process::je_suis_maitre())
        {
          Cerr << "The Traitement_Pth option selected is not coherent with the boundaries conditions." << finl;
          Cerr << "Traitement_Pth constant must be used for the case of free outlet." << finl;
        }
      Process::exit();
    }
  if (!pression_imposee && traitement_PTh!=1)
    {
      if (Process::je_suis_maitre())
        {
          Cerr << "The Traitement_Pth option selected is not coherent with the boundaries conditions." << finl;
          Cerr << "Traitement_Pth conservation_masse must be used for the case without free outlet." << finl;
        }
      Process::exit();
    }
}

void Fluide_Quasi_Compressible::set_param(Param& param)
{
  Fluide_Dilatable_base::set_param(param);
  param.ajouter("temps_debut_prise_en_compte_drho_dt",&temps_debut_prise_en_compte_drho_dt_);
  param.ajouter("omega_relaxation_drho_dt",&omega_drho_dt_);
  param.ajouter_non_std("loi_etat",(this),Param::REQUIRED);
  //Lecture de mu et lambda pas specifiee obligatoire car option sutherland possible
  param.supprimer("mu");
  param.ajouter_non_std("mu",(this));
  param.ajouter_non_std("sutherland",(this));
  param.ajouter_non_std("pression",(this),Param::REQUIRED);
  param.ajouter_non_std("Traitement_PTh",(this));
  param.ajouter_non_std("Traitement_rho_gravite",(this));
  param.supprimer("beta_th");
  param.ajouter_non_std("beta_th",(this));
  param.supprimer("beta_co");
  param.ajouter_non_std("beta_co",(this));
}

int Fluide_Quasi_Compressible::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="loi_etat")
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
  else if (mot=="pression")
    {
      is>>Pth_;
      Pth_n = Pth_;
      rho.typer("Champ_Uniforme");
      DoubleTab& tab_rho=rho.valeurs();
      tab_rho.resize(1,1);
      tab_rho(0,0) = 1.;
      return 1;
    }
  else if (mot=="Traitement_PTh")
    {
      Motcle trait;
      is >> trait;
      Motcles les_options(3);
      {
        les_options[0] = "edo";
        les_options[1] = "conservation_masse";
        les_options[2] = "constant";
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
  else if (mot=="Traitement_rho_gravite")
    {
      Motcle trait;
      is >> trait;
      Motcles les_options(2);
      {
        les_options[0] = "standard";
        les_options[1] = "moins_rho_moyen";
      }
      traitement_rho_gravite_=les_options.search(trait);
      if (traitement_rho_gravite_ == -1)
        {
          Cerr<< trait << " is not understood as an option of the keyword " << motlu <<finl;
          Cerr<< "One of the following options was expected : " << les_options << finl;
          Process::exit();
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
void Fluide_Quasi_Compressible::completer(const Probleme_base& pb)
{

  if ((loi_etat_->que_suis_je() == "Loi_Etat_Rho_T" || loi_etat_->que_suis_je() == "Loi_Etat_Melange_Binaire" )
      && traitement_PTh == 0)
    {
      Cerr << "The option Traitement_PTh EDO is not allowed with the state law " << loi_etat_->que_suis_je() << finl;
      Cerr << "Set **traitement_pth** constant or conservation_masse in the Fluide_Quasi_Compressible bloc definition." << finl;
      Process::exit();
    }

  Cerr<<"Fluide_Dilatable_base::completer Pth="<<Pth_<<finl;
  inco_chaleur_ = pb.equation(1).inconnue();

  if (pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_QC" || pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_Turbulent_QC")
    assert(inco_chaleur_.le_nom()=="fraction_massique");


  vitesse_ = pb.equation(0).inconnue();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,pb.equation(0));
  pression_ = eqn_hydr.pression();

  Nom typ = pb.equation(0).discretisation().que_suis_je();
  if (typ=="VEFPreP1B")
    {
      typ = "VEF";
    }
  typ += "_";

  // Pb_Hydraulique_Melange_Binaire_QC/Pb_Hydraulique_Melange_Binaire_Turbulent_QC is currently an isobar problem
  // The EDO_pth is not solved (traitement_pth constant)
  // However, we need a typer because we will need to access calculer_rho_face in calculer_masse_volumique (loi_etat)
  // EDO_Pression_th_VDF/VEF_Melange_Binaire not implemented yet
  // just typer Gaz_Parfait instead...
  if (pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_QC" || pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_Turbulent_QC")
    typ +="Gaz_Parfait";
  else
    typ += loi_etat_->type_fluide();

  // TODO : XXX
  // All this is not good... EDO_Pth_ should only be used if traitement_PTh = 0
  // I think we can do better ...

  EDO_Pth_.typer(typ);
  EDO_Pth_->associer_zones(pb.equation(0).zone_dis(),pb.equation(0).zone_Cl_dis());
  EDO_Pth_->associer_fluide(*this);
  EDO_Pth_->mettre_a_jour_CL(Pth_);

  initialiser_inco_ch();

  EDO_Pth_->mettre_a_jour(0.);
  loi_etat_->initialiser();

  output_file_ = Objet_U::nom_du_cas();
  output_file_ += "_";
  output_file_ += pb.le_nom();
  output_file_ += ".evol_glob";

  Cerr << "Warning! evol_glob file renamed " << output_file_ << finl;
  if(je_suis_maitre())
    {
      SFichier fic (output_file_);
      fic<<"# Time sum(T*dv)/sum(dv)[K] sum(rho*dv)/sum(dv)[kg/m3] Pth[Pa]"<<finl;
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
void Fluide_Quasi_Compressible::preparer_pas_temps()
{
  Fluide_Dilatable_base::preparer_pas_temps();
  EDO_Pth_->mettre_a_jour(0);
  EDO_Pth_->mettre_a_jour_CL(Pth_);
}


void Fluide_Quasi_Compressible::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  Motcle nom_var;
  if (pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_QC" || pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_Turbulent_QC")
    nom_var="fraction_massique"; // because we do not even have a temperature variable
  else
    nom_var="temperature";

  const Zone_dis_base& zone_dis=pb.equation(0).zone_dis();
  double temps=pb.schema_temps().temps_courant();
  //
  Cerr<<"Fluide_Dilatable_base::discretiser"<<finl;
  // les champs seront nommes par le milieu_base
  Champ_Don ch_rho;
  dis.discretiser_champ("temperature",zone_dis,"masse_volumique_p","neant",1,temps,ch_rho);
  rho = ch_rho.valeur();

  // NOTE : ch_temperature() is the temperature field
  // unless for a Pb_Hydraulique_Melange_Binaire_QC & Pb_Hydraulique_Melange_Binaire_Turbulent_QC
  // where it denotes the species Y1 (mass fraction)
  Champ_Don& ch_TK = ch_temperature();

  if (pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_QC" || pb.que_suis_je()=="Pb_Hydraulique_Melange_Binaire_Turbulent_QC")
    dis.discretiser_champ("temperature",zone_dis,"fraction_massique","neant",1,temps,ch_TK);
  else
    dis.discretiser_champ("temperature",zone_dis,"temperature","K",1,temps,ch_TK);

  if (type_fluide()!="Gaz_Parfait")
    loi_etat().valeur().champs_compris().ajoute_champ(ch_TK);

  Champ_Don& cp = capacite_calorifique();
  if (!cp.non_nul() || !sub_type(Champ_Uniforme,cp.valeur()))    //ie Cp non constant : gaz reels
    {
      Cerr<<"Heat capacity Cp is discretized once more for space variable case."<<finl;
      dis.discretiser_champ("temperature",zone_dis,"cp_prov","neant",1,temps,cp);
    }
  if (!lambda.non_nul() || ((!sub_type(Champ_Uniforme,lambda.valeur())) && (!sub_type(Champ_Fonc_Tabule,lambda.valeur()))))
    {
      // cas particulier etait faux en VEF voir quand cela sert (FM slt)
      // sera nomme par milieu_base
      dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",1,temps,lambda);

    }
  dis.discretiser_champ("vitesse", zone_dis,"rho_comme_v","kg/m3",1,temps,rho_comme_v);
  champs_compris_.ajoute_champ(rho_comme_v);

  // mu_sur_Schmidt
  dis.discretiser_champ("champ_elem",zone_dis,"mu_sur_Schmidt","kg/(m.s)",1,temps,mu_sur_Sc);
  champs_compris_.ajoute_champ(mu_sur_Sc);

  // nu_sur_Schmidt
  dis.discretiser_champ("champ_elem",zone_dis,"nu_sur_Schmidt","m2/s",1,temps,nu_sur_Sc);
  champs_compris_.ajoute_champ(nu_sur_Sc);

  // pression_tot
  //  pas comme la pression !!!
  Champ_Don& ptot = pression_tot();
  dis.discretiser_champ("champ_elem",zone_dis,"pression_tot","Pa",1,temps,ptot);
  champs_compris_.ajoute_champ(ptot);
  dis.discretiser_champ("temperature",zone_dis,"rho_gaz","kg/m3",1,temps,rho_gaz);
  champs_compris_.ajoute_champ(rho_gaz);

  Fluide_Dilatable_base::discretiser(pb,dis);
}

void Fluide_Quasi_Compressible::prepare_pressure_edo()
{
  EDO_Pth_->completer();
  EDO_Pth_->mettre_a_jour(0.);
}

void Fluide_Quasi_Compressible::write_mean_edo(double temps)
{
  double Ch_m = EDO_Pth_->moyenne_vol(inco_chaleur_->valeurs());
  double rhom = EDO_Pth_->moyenne_vol(rho.valeurs());

  if(je_suis_maitre())
    {
      SFichier fic (output_file_,ios::app);
      fic<<temps <<" "<<Ch_m<<" "<<rhom<<" "<<Pth_<<finl;
    }
}
