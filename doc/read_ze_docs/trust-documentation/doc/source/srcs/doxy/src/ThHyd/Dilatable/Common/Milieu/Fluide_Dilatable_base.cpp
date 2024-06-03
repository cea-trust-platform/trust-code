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

#include <Fluide_Dilatable_base.h>
#include <Loi_Etat_Multi_GP_QC.h>
#include <Discretisation_base.h>
#include <Champ_Fonc_Fonction.h>
#include <Navier_Stokes_std.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Domaine_VF.h>
#include <Param.h>

Implemente_base_sans_constructeur(Fluide_Dilatable_base,"Fluide_Dilatable_base",Fluide_base);
// XD fluide_dilatable_base fluide_base fluide_dilatable_base -1 Basic class for dilatable fluids.

Fluide_Dilatable_base::Fluide_Dilatable_base():traitement_PTh(0),Pth_(-1.),Pth_n(-1.),Pth1(-1.) {}

Sortie& Fluide_Dilatable_base::printOn(Sortie& os) const
{
  Fluide_base::ecrire(os);
  return os;
}

Entree& Fluide_Dilatable_base::readOn(Entree& is)
{
  Fluide_base::readOn(is);
  return is;
}

void Fluide_Dilatable_base::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  Cerr<<"Fluide_Dilatable_base::discretiser"<<finl;

  const Domaine_dis_base& domaine_dis=pb.equation(0).domaine_dis();
  double temps=pb.schema_temps().temps_courant();

  // les champs seront nommes par le milieu_base
  Champ_Don ch_rho;
  dis.discretiser_champ("temperature",domaine_dis,"masse_volumique_p","neant",1,temps,ch_rho);
  rho = ch_rho.valeur();

  Champ_Don& cp = capacite_calorifique();
  if (cp.est_nul() || !sub_type(Champ_Uniforme,cp.valeur())) //ie Cp non constant : gaz reels
    {
      Cerr<<"Heat capacity Cp is discretized once more for space variable case."<<finl;
      dis.discretiser_champ("temperature",domaine_dis,"cp_prov","neant",1,temps,cp);
    }

  if (lambda.est_nul() || ((!sub_type(Champ_Uniforme,lambda.valeur())) && (!sub_type(Champ_Fonc_Tabule,lambda.valeur()))))
    {
      // cas particulier etait faux en VEF voir quand cela sert (FM slt) : sera nomme par milieu_base
      dis.discretiser_champ("champ_elem",domaine_dis,"neant","neant",1,temps,lambda);
    }

  dis.discretiser_champ("vitesse", domaine_dis,"rho_comme_v","kg/m3",1,temps,rho_comme_v);
  champs_compris_.ajoute_champ(rho_comme_v);

  dis.discretiser_champ("champ_elem",domaine_dis,"mu_sur_Schmidt","kg/(m.s)",1,temps,mu_sur_Sc);
  champs_compris_.ajoute_champ(mu_sur_Sc);

  dis.discretiser_champ("champ_elem",domaine_dis,"nu_sur_Schmidt","m2/s",1,temps,nu_sur_Sc);
  champs_compris_.ajoute_champ(nu_sur_Sc);

  Champ_Don& ptot = pression_tot();
  dis.discretiser_champ("champ_elem",domaine_dis,"pression_tot","Pa",1,temps,ptot);
  champs_compris_.ajoute_champ(ptot);

  dis.discretiser_champ("temperature",domaine_dis,"rho_gaz","kg/m3",1,temps,rho_gaz);
  champs_compris_.ajoute_champ(rho_gaz);

  Fluide_base::discretiser(pb,dis);
}

void Fluide_Dilatable_base::set_param(Param& param)
{
  Fluide_base::set_param(param);
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

int Fluide_Dilatable_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
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
      double mu0,T0,C=-1,Slambda=-1;
      Nom prob;
      is>>prob;
      is>>motlu;
      if (motlu!="MU0") warn_syntax_Sutherland();
      is>>mu0;
      is>>motlu;
      if (motlu!="T0") warn_syntax_Sutherland();
      is>>T0;
      is>>motlu;
      if (motlu=="SLAMBDA")
        {
          is >> Slambda;
          is >> motlu;
        }
      if (motlu!="C") warn_syntax_Sutherland();
      is>>C;

      mu.typer("Sutherland");
      Sutherland& mu_suth = ref_cast(Sutherland,mu.valeur());
      mu_suth.set_val_params(prob,mu0,C,T0);
      mu_suth.lire_expression();

      //On stocke la valeur de C (ici Slambda) pour construire(cf creer_champs_non_lus())
      //la loi de Sutherland qui concerne la conductivite
      if (Slambda!=-1)
        {
          lambda.typer("Sutherland");
          Sutherland& lambda_suth = ref_cast(Sutherland,lambda.valeur());
          lambda_suth.set_prob(prob);
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
  else return Fluide_base::lire_motcle_non_standard(mot,is);
}

void Fluide_Dilatable_base::warn_syntax_Sutherland()
{
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "Error in call to Sutherland function :" << finl;
  Cerr << "The syntax has changed in version 1.8.4." << finl;
  Cerr << finl << "  A specification of kind : sutherland problem_name mu0 1.85e-5 T0 300 [Slambda 10] C 10 was expected. "<<finl;
  Cerr << finl << "Please update your dataset or contact TRUST support team." << finl;
  Process::exit();
}

/*! @brief Verifie que les champs lus l'ont ete correctement.
 *
 * @throws l'une des proprietes (rho mu Cp ou lambda) du fluide n'a pas ete definie
 */
void Fluide_Dilatable_base::verifier_coherence_champs(int& err,Nom& msg)
{
  msg="";
  if (rho.non_nul()) { }
  else
    {
      msg += "The density rho has not been specified. \n";
      err = 1;
    }
  if (mu.non_nul())
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          if (mu(0,0) <= 0)
            {
              msg += "The dynamical viscosity mu is not striclty positive. \n";
              err = 1;
            }
        }
    }
  else
    {
      msg += "The dynamical viscosity mu has not been specified. \n";
      err = 1;
    }
  if (lambda.non_nul()) { }
  else
    {
      msg += "The conductivity lambda has not been specified. \n";
      err = 1;
    }
  if (Cp.non_nul()) { }
  else
    {
      msg += "The heat capacity Cp has not been specified. \n";
      err = 1;
    }

  Milieu_base::verifier_coherence_champs(err,msg);
}

/*! @brief Complete le fluide avec un Cp constant
 *
 * @param (double Cp) le cp du fluide
 */
void Fluide_Dilatable_base::set_Cp(double Cp_)
{
  Cp.typer("Champ_Uniforme");
  Champ_Uniforme& ch_Cp = ref_cast(Champ_Uniforme,Cp.valeur());
  ch_Cp.dimensionner(1,1);
  DoubleTab& tab_Cp = Cp.valeurs();
  tab_Cp(0,0) = Cp_;
}

void Fluide_Dilatable_base::update_rho_cp(double temps)
{
  rho_cp_comme_T_.changer_temps(temps);
  rho_cp_comme_T_.valeur().changer_temps(temps);
  DoubleTab& rho_cp = rho_cp_comme_T_.valeurs();
  if (sub_type(Champ_Uniforme,rho))
    rho_cp = rho.valeurs()(0, 0);
  else
    {
      // AB: rho_cp = rho.valeurs() turns rho_cp into a 2 dimensional array with 1 compo. We want to stay mono-dim:
      rho_cp = 1.;
      tab_multiply_any_shape(rho_cp, rho.valeurs());
    }
  if (sub_type(Champ_Uniforme, Cp.valeur()))
    rho_cp *= Cp.valeurs()(0, 0);
  else
    tab_multiply_any_shape(rho_cp,Cp.valeurs());
}

/*! @brief Renvoie le tableau des valeurs de le temperature
 *
 */
const DoubleTab& Fluide_Dilatable_base::temperature() const
{
  return ch_temperature().valeurs();
}

/*! @brief Renvoie le champ de le temperature
 *
 */
const Champ_Don& Fluide_Dilatable_base::ch_temperature() const
{
  return loi_etat_->ch_temperature();
}

Champ_Don& Fluide_Dilatable_base::ch_temperature()
{
  return loi_etat_->ch_temperature();
}

/*! @brief Prepare le pas de temps
 *
 */
void Fluide_Dilatable_base::preparer_pas_temps()
{
  loi_etat_->mettre_a_jour(le_probleme_->schema_temps().temps_courant());
  eos_tools_->mettre_a_jour(le_probleme_->schema_temps().temps_courant());
}

void Fluide_Dilatable_base::abortTimeStep()
{
  loi_etat()->abortTimeStep();
  Pth_=Pth_n;
}

void Fluide_Dilatable_base::creer_champs_non_lus()
{
  // on s'occupe de lamda si mu uniforme et CP uniforme
  // on type lambda en champ uniforme et on met lambda=mu*Cp/Pr
  //
  if (mu.non_nul())
    {
      if ((lambda.est_nul())||(!sub_type(Champ_Fonc_Tabule,lambda.valeur())))
        if ((sub_type(Champ_Uniforme,mu.valeur()))&&(sub_type(Loi_Etat_GP_base,loi_etat_.valeur())))
          {
            if (!sub_type(Loi_Etat_Multi_GP_QC,loi_etat_.valeur()))
              {
                // Si mu uniforme et si la loi d'etat est celle d'un gaz parfait
                double lold=-1;
                if (lambda.non_nul())
                  lold=lambda.valeurs()(0,0);
                lambda.typer(mu.valeur().le_type());
                lambda=mu;

                loi_etat_->calculer_lambda();
                double lo=lambda.valeurs()(0,0);
                if (lold!=-1)
                  {
                    if (!est_egal(lold,lo))
                      {
                        Cerr << "Error : mu, lambda, Cp and Prandtl are all specified in your data set." << finl;
                        Cerr << "It is observed that your imput values leads to :" << finl;
                        Cerr.precision(20);
                        Cerr << "mu/(lambda*Cp)=" << loi_etat_->Prandt()*(lo/lold) << " and Prandtl=" << loi_etat_->Prandt() << finl;
                        Cerr<<"Please modify your data set by specifying for instance Prandtl = "<<loi_etat_->Prandt()*(lo/lold)<<finl;
                        Process::exit();
                      }
                  }
              }
          }

      if (lambda.non_nul())
        {
          if (sub_type(Sutherland,lambda.valeur()))
            {
              if (!sub_type(Champ_Uniforme,Cp.valeur()))
                {
                  Cerr << "A sutherland law cannot be requested for the conductivity "<<finl;
                  Cerr << "by indicating Slambda if the heat capacity (Cp) is not uniform."<<finl;
                  Process::exit();
                }
              Sutherland& mu_suth = ref_cast(Sutherland,mu.valeur());
              const double mu0 = mu_suth.get_A();
              Sutherland& lambda_suth = ref_cast(Sutherland,lambda.valeur());
              double lambda0 = mu0/loi_etat_->Prandt()*Cp(0,0);
              lambda_suth.set_A(lambda0);
              lambda_suth.lire_expression();
            }
        }
    }
}

/*! @brief Initialise les parametres du fluide.
 *
 */
int Fluide_Dilatable_base::initialiser(const double temps)
{
  Cerr << "Fluide_Dilatable_base::initialiser()" << finl;
  if (sub_type(Champ_Don_base, rho))
    ref_cast(Champ_Don_base, rho).initialiser(temps);

  mu.initialiser(temps);
  lambda.initialiser(temps);
  Cp.initialiser(temps);
  update_rho_cp(temps);

  if (coeff_absorption_.non_nul() && indice_refraction_.non_nul())
    initialiser_radiatives(temps);

  return 1;
}

// Initialisation des proprietes radiatives du fluide incompressible
// (Pour un fluide incompressible semi transparent).
void Fluide_Dilatable_base::initialiser_radiatives(const double temps)
{
  coeff_absorption_.initialiser(temps);
  indice_refraction_.initialiser(temps);
  longueur_rayo_.initialiser(temps);
  if (sub_type(Champ_Uniforme,kappa().valeur()))
    longueur_rayo()->valeurs()(0,0)=1/(3*kappa()(0,0));
  else
    {
      DoubleTab& l_rayo = longueur_rayo_.valeurs();
      const DoubleTab& K = kappa().valeurs();
      for (int i=0; i<kappa().nb_valeurs_nodales(); i++)
        l_rayo[i] = 1/(3*K[i]);
    }
}

/*! @brief Calcule la pression totale : pression thermodynamique + pression hydrodynamique
 *
 */
void Fluide_Dilatable_base::calculer_pression_tot()
{
  DoubleTab& tab_Ptot = pression_tot_.valeurs();
  const int n = tab_Ptot.dimension_tot(0);
  DoubleTab tab_PHyd(n, 1);
  if( n != pression_->valeurs().dimension_tot(0) )
    {
      // Interpolation de pression_ aux elements (ex: P1P0)
      const Domaine_dis_base& domaine_dis= pression_-> domaine_dis_base();
      const Domaine_VF& domaine = ref_cast(Domaine_VF, domaine_dis);
      const DoubleTab& centres_de_gravites=domaine.xp();
      pression_->valeur().valeur_aux(centres_de_gravites,tab_PHyd);
    }
  else  tab_PHyd = pression_->valeurs();
  // impl dans les classes filles
  remplir_champ_pression_tot(n,tab_PHyd,tab_Ptot);
}

const Champ_base& Fluide_Dilatable_base::get_champ(const Motcle& nom) const
{
  try
    {
      return Fluide_base::get_champ(nom);
    }
  catch (Champs_compris_erreur&)
    {
    }
  try
    {
      return loi_etat_->get_champ(nom);
    }
  catch (Champs_compris_erreur&)
    {
    }
  throw Champs_compris_erreur();
}

void Fluide_Dilatable_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  Fluide_base::get_noms_champs_postraitables(nom,opt);
  loi_etat_->get_noms_champs_postraitables(nom,opt);
}

void Fluide_Dilatable_base::mettre_a_jour(double temps)
{
  rho.mettre_a_jour(temps);
  ch_temperature().mettre_a_jour(temps); // Note : it denotes the species Y1 for Pb_Hydraulique_Melange_Binaire_QC
  rho->changer_temps(temps);
  ch_temperature()->changer_temps(temps);
  mu->changer_temps(temps);
  lambda->changer_temps(temps);
  Cp.mettre_a_jour(temps);
  update_rho_cp(temps);
  write_mean_edo(temps); // si besoin (i.e. QC)
}

/*! @brief Prepare le fluide au calcul.
 *
 */
void Fluide_Dilatable_base::preparer_calcul()
{
  Cerr << "Fluide_Dilatable_base::preparer_calcul()" << finl;
  Milieu_base::preparer_calcul();
  Fluide_Dilatable_base::update_pressure_fields(le_probleme_->schema_temps().temps_courant()); // Child can have an overload
  loi_etat_->preparer_calcul();
  prepare_pressure_edo(); // si besoin (i.e. QC)
  calculer_coeff_T();
}

void Fluide_Dilatable_base::update_pressure_fields(double temps)
{
  calculer_pression_tot();
  pression_tot_.mettre_a_jour(temps);
}

/*! @brief Complete le fluide avec les champs inconnus associes au probleme
 *
 * @param (Pb_Thermohydraulique& pb) le probleme a resoudre
 */
void Fluide_Dilatable_base::completer(const Probleme_base& pb)
{
  le_probleme_ = pb;
  inco_chaleur_ = pb.equation(1).inconnue();
  vitesse_ = pb.equation(0).inconnue();
  const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,pb.equation(0));
  pression_ = eqn_hydr.pression();

  Nom typ = pb.equation(0).discretisation().que_suis_je();
  if (typ=="VEFPreP1B") typ = "VEF";

  eos_tools_.typer(typ);
  eos_tools_->associer_domaines(pb.equation(0).domaine_dis(),pb.equation(0).domaine_Cl_dis());
  eos_tools_->associer_fluide(*this);
  loi_etat_->assoscier_probleme(pb);
  initialiser_inco_ch();
  eos_tools_->mettre_a_jour(pb.schema_temps().temps_courant());
  loi_etat_->initialiser();
}
