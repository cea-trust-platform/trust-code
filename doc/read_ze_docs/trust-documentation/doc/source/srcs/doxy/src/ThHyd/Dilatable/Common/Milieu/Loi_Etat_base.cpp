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

#include <Fluide_Dilatable_base.h>
#include <Schema_Temps_base.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Uniforme.h>
#include <Loi_Etat_base.h>
#include <Probleme_base.h>
#include <Domaine_VF.h>
#include <Debog.h>

Implemente_base_sans_constructeur(Loi_Etat_base,"Loi_Etat_base",Objet_U);
// XD loi_etat_base objet_u loi_etat_base -1 Basic class for state laws used with a dilatable fluid.

Loi_Etat_base::Loi_Etat_base() : Pr_(-1.), debug(0) { }

Sortie& Loi_Etat_base::printOn(Sortie& os) const
{
  os << que_suis_je() << finl;
  return os;
}

Entree& Loi_Etat_base::readOn(Entree& is)
{
  return is;
}

/*! @brief Associe le fluide a la loi d'etat
 *
 * @param (Fluide_Dilatable_base& fl) le fluide associe
 */
void Loi_Etat_base::associer_fluide(const Fluide_Dilatable_base& fl)
{
  le_fluide = fl;
}

/*! @brief Associe le probleme a la loi d'etat
 *
 * @param (Fluide_Dilatable_base& fl) le fluide associe
 */
void Loi_Etat_base::assoscier_probleme(const Probleme_base& pb)
{
  le_prob_ = pb;
}

/*! @brief Renvoie le champ de le temperature
 *
 */
const Champ_Don& Loi_Etat_base::ch_temperature() const
{
  return temperature_;
}

Champ_Don& Loi_Etat_base::ch_temperature()
{
  return temperature_;
}

/*! @brief Initialise l'inconnue de l'equation de chaleur : ne fai rien
 *
 */
void Loi_Etat_base::initialiser_inco_ch()
{
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  tab_rho_n=tab_rho;
  tab_rho_np1=tab_rho;
  calculer_masse_volumique();
  double t = le_prob_->schema_temps().temps_courant();
  mettre_a_jour(t);
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
}

/*! @brief Prepare le fluide au calcul.
 *
 */
void Loi_Etat_base::preparer_calcul()
{
  remplir_T();
  calculer_masse_volumique();
  double t = le_prob_->schema_temps().temps_courant();
  mettre_a_jour(t);
}

/*! @brief Met a jour la loi d'etat et le champ rho
 *
 * @param (double temps) le temps de calcul
 */
void Loi_Etat_base::mettre_a_jour(double temps)
{
  //remplissage de rho avec rho(n+1)
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int i, n=tab_rho.size_totale();
  for (i=0 ; i<n ; i++)
    {
      tab_rho_n(i) = tab_rho_np1(i);
      tab_rho(i,0) = tab_rho_np1(i);
    }
  le_fluide->masse_volumique().mettre_a_jour(temps);
}

/*! @brief Calcule la viscosite
 *
 */
void Loi_Etat_base::calculer_mu()
{
  Champ_Don& mu = le_fluide->viscosite_dynamique();
  if (!sub_type(Champ_Uniforme,mu.valeur()))
    {
      // E. Saikali : Pourquoi pas Champ_fonc_xyz pour mu ???
      if (sub_type(Champ_Fonc_Tabule,mu.valeur()) || sub_type(Champ_Don_base,mu.valeur()))
        mu.mettre_a_jour(temperature_.valeur().temps());
      else
        {
          Cerr<<"The viscosity field mu of type "<<mu.valeur().que_suis_je()<<" is not recognized.";
          Process::exit();
        }
    }
}

/*! @brief Calcule la conductivite
 *
 */
void Loi_Etat_base::calculer_lambda()
{
  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& lambda = le_fluide->conductivite();
  DoubleTab& tab_lambda = lambda.valeurs();

  int i, n = tab_lambda.size();
  // La conductivite est soit un champ uniforme soit calculee a partir de la viscosite dynamique et du Pr
  if (!sub_type(Champ_Uniforme,lambda.valeur()))
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          double mu0 = tab_mu(0,0);
          for (i=0 ; i<n ; i++) tab_lambda(i,0) = mu0 * tab_Cp(i,0) / Pr_;
        }
      else
        {
          for (i=0 ; i<n ; i++) tab_lambda(i,0) = tab_mu(i,0) * tab_Cp(i,0) / Pr_;
        }
    }
  tab_lambda.echange_espace_virtuel();
}

/*! @brief Calcule la viscosite cinematique
 *
 */
void Loi_Etat_base::calculer_nu()
{
  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& nu= le_fluide->viscosite_cinematique ();
  DoubleTab& tab_nu = nu.valeurs();

  int i, n = tab_nu.size(), isVDF=0;
  if (nu.valeur().que_suis_je()=="Champ_Fonc_P0_VDF") isVDF = 1;

  if (isVDF)
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          double mu0 = tab_mu(0,0);
          for (i=0 ; i<n ; i++) tab_nu(i,0) = mu0 / tab_rho(i,0);
        }
      else
        {
          for (i=0 ; i<n ; i++) tab_nu(i,0) = tab_mu(i,0) / tab_rho(i,0);
        }
    }
  else // VEF
    {
      const IntTab& elem_faces=ref_cast(Domaine_VF,le_fluide->vitesse().domaine_dis_base()).elem_faces();
      double rhoelem;
      int face, nfe = elem_faces.line_size();
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          double mu0 = tab_mu(0,0);
          for (i=0 ; i<n ; i++)
            {
              rhoelem=0;
              for (face=0; face<nfe; face++) rhoelem += tab_rho(elem_faces(i,face),0);
              rhoelem /= nfe;
              tab_nu(i,0) = mu0 / rhoelem;
            }
        }
      else
        {
          for (i=0 ; i<n ; i++)
            {
              rhoelem=0;
              for (face=0; face<nfe; face++) rhoelem += tab_rho(elem_faces(i,face),0);
              rhoelem /= nfe;
              tab_nu(i,0) = tab_mu(i,0) / rhoelem;
            }
        }
    }
  tab_nu.echange_espace_virtuel();
  Debog::verifier("Loi_Etat_base::calculer_nu tab_nu",tab_nu);
}

/*! @brief Calcule la diffusivite
 *
 */
void Loi_Etat_base::calculer_alpha()
{
  const Champ_Don& lambda = le_fluide->conductivite();
  const DoubleTab& tab_lambda = lambda.valeurs();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  DoubleTab& tab_alpha = le_fluide->diffusivite().valeurs();

  int i, n = tab_alpha.size();
  if (sub_type(Champ_Uniforme,lambda.valeur()))
    {
      double lambda0 = tab_lambda(0,0);
      for (i=0 ; i<n ; i++) tab_alpha(i,0) = lambda0 / (tab_rho(i,0)*tab_Cp(i,0));
    }
  else
    {
      for (i=0 ; i<n ; i++) tab_alpha(i,0) = tab_lambda(i,0) / (tab_rho(i,0)*tab_Cp(i,0));
    }
  tab_alpha.echange_espace_virtuel();
  Debog::verifier("Loi_Etat_base::calculer_alpha alpha",tab_alpha);
}


/*! @brief Ne fait rien Surcharge dans Loi_Etat_Melange_GP
 *
 */
void Loi_Etat_base::calculer_mu_sur_Sc()
{
  /* Do nothing : overloaded later */
}

/*! @brief Ne fait rien Surcharge dans Loi_Etat_Melange_Binaire
 *
 */
void Loi_Etat_base::calculer_nu_sur_Sc()
{
  /* Do nothing : overloaded later */
}

/*! @brief Recalcule la masse volumique
 *
 */
void Loi_Etat_base::calculer_masse_volumique()
{
  const DoubleTab& tab_ICh = le_fluide->inco_chaleur().valeurs();
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  double Pth = le_fluide->pression_th();
  int n=tab_rho.size();
  for (int som=0 ; som<n ; som++)
    {
      tab_rho_np1(som) = calculer_masse_volumique(Pth,tab_ICh(som,0));
      tab_rho(som,0) = 0.5 * ( tab_rho_n(som) + tab_rho_np1(som) );
    }
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
  le_fluide->calculer_rho_face(tab_rho_np1);
}

/*! @brief Cas gaz parfait : ne fait rien Cas gaz Reel : doit recalculer l'enthalpie a partir de la pression et la temperature
 *
 */
double Loi_Etat_base::calculer_H(double Pth_, double T_) const
{
  return T_;
}

double Loi_Etat_base::Drho_DP(double,double) const
{
  Cerr<<"Drho_DP doit etre code dans la classe fille "<<que_suis_je()<<" pour etre utilisee"<<finl;
  abort();
  return 0;
}
double Loi_Etat_base::Drho_DT(double,double) const
{
  Cerr<<"Drho_DT doit etre code dans la classe fille "<<que_suis_je()<<" pour etre utilisee"<<finl;
  abort();
  return 0;
}
double Loi_Etat_base::De_DP(double,double) const
{
  Cerr<<"De_DP doit etre code dans la classe fille "<<que_suis_je()<<" pour etre utilisee"<<finl;
  abort();
  return 0;
}
double Loi_Etat_base::De_DT(double,double) const
{
  Cerr<<"De_DT doit etre code dans la classe fille "<<que_suis_je()<<" pour etre utilisee"<<finl;
  abort();
  return 0;
}

void Loi_Etat_base::creer_champ(const Motcle& motlu)
{
  /* Do nothing */
}

const Champ_base& Loi_Etat_base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);
}

void Loi_Etat_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<"Loi_Etat_base : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());
}

void Loi_Etat_base::abortTimeStep()
{
  int i, n=tab_rho_n.size_totale();
  for (i=0 ; i<n ; i++)
    tab_rho_np1(i) = tab_rho_n(i);
}
