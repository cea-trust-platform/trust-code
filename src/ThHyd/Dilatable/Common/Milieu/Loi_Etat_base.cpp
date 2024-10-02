/****************************************************************************
* Copyright (c) 2024, CEA
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
#include <kokkos++.h>
#include <TRUSTArray_kokkos.tpp>

Implemente_base_sans_constructeur(Loi_Etat_base,"Loi_Etat_base",Objet_U);
// XD loi_etat_base objet_u loi_etat_base -1 Basic class for state laws used with a dilatable fluid.

Loi_Etat_base::Loi_Etat_base() : Pr_(-1.), debug(0) { }

Sortie& Loi_Etat_base::printOn(Sortie& os) const
{
  return os << que_suis_je() << finl;
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
const Champ_Don_base& Loi_Etat_base::ch_temperature() const
{
  return temperature_.valeur();
}

Champ_Don_base& Loi_Etat_base::ch_temperature()
{
  return temperature_.valeur();
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
  tab_rho_n = tab_rho_np1;
  tab_rho = tab_rho_np1;
  le_fluide->masse_volumique().mettre_a_jour(temps);
}

/*! @brief Calcule la viscosite
 *
 */
void Loi_Etat_base::calculer_mu()
{
  Champ_Don_base& mu = le_fluide->viscosite_dynamique();
  if (!sub_type(Champ_Uniforme,mu))
    {
      // E. Saikali : Pourquoi pas Champ_fonc_xyz pour mu ???
      if (sub_type(Champ_Fonc_Tabule,mu) || sub_type(Champ_Don_base,mu))
        mu.mettre_a_jour(temperature_->temps());
      else
        {
          Cerr<<"The viscosity field mu of type "<<mu.que_suis_je()<<" is not recognized.";
          Process::exit();
        }
    }
}

/*! @brief Calcule la conductivite
 *
 */
void Loi_Etat_base::calculer_lambda()
{
  const Champ_Don_base& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don_base& lambda = le_fluide->conductivite();
  DoubleTab& tab_lambda = lambda.valeurs();
  ToDo_Kokkos("critical");
  int i, n = tab_lambda.size();
  // La conductivite est soit un champ uniforme soit calculee a partir de la viscosite dynamique et du Pr
  if (!sub_type(Champ_Uniforme,lambda))
    {
      if (sub_type(Champ_Uniforme,mu))
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
  const Champ_Don_base& viscosite_dynamique = le_fluide->viscosite_dynamique();
  bool uniforme = sub_type(Champ_Uniforme,viscosite_dynamique);
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  const DoubleTab& tab_mu = viscosite_dynamique.valeurs();
  Champ_Don_base& viscosite_cinematique = le_fluide->viscosite_cinematique();
  DoubleTab& tab_nu = viscosite_cinematique.valeurs();
  int n = tab_nu.size();

  if (viscosite_cinematique.que_suis_je()=="Champ_Fonc_P0_VDF")
    {
      // VDF
      for (int i=0 ; i<n ; i++)
        tab_nu(i,0) = tab_mu(uniforme ? 0 : i,0) / tab_rho(i,0);
    }
  else // VEF
    {
      const IntTab& ef = ref_cast(Domaine_VF,le_fluide->vitesse().domaine_dis_base()).elem_faces();
      int nfe = ef.line_size();
      CIntTabView elem_faces = ef.view_ro();
      CDoubleArrView rho = static_cast<const DoubleVect&>(tab_rho).view_ro();
      CDoubleArrView mu = static_cast<const DoubleVect&>(tab_mu).view_ro();
      DoubleArrView nu = static_cast<DoubleVect&>(tab_nu).view_rw();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), n, KOKKOS_LAMBDA(const int i)
      {
        double rhoelem = 0;
        for (int face = 0; face < nfe; face++)
          rhoelem += rho(elem_faces(i, face));
        rhoelem /= nfe;
        nu(i) = mu(uniforme ? 0 : i) / rhoelem;
      });
      end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
    }
  tab_nu.echange_espace_virtuel();
  Debog::verifier("Loi_Etat_base::calculer_nu tab_nu",tab_nu);
}

/*! @brief Calcule la diffusivite
 *
 */
void Loi_Etat_base::calculer_alpha()
{
  DoubleTab& tab_alpha = le_fluide->diffusivite().valeurs();
  const Champ_Don_base& conductivite = le_fluide->conductivite();
  bool uniforme = sub_type(Champ_Uniforme,conductivite);
  int n = tab_alpha.size();
  CDoubleArrView lambda = static_cast<const DoubleVect&>(conductivite.valeurs()).view_ro();
  CDoubleArrView Cp = static_cast<const DoubleVect&>(le_fluide->capacite_calorifique().valeurs()).view_ro();
  CDoubleArrView rho = static_cast<const DoubleVect&>(le_fluide->masse_volumique().valeurs()).view_ro();
  DoubleArrView alpha = static_cast<DoubleVect&>(tab_alpha).view_rw();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), n, KOKKOS_LAMBDA(const int i)
  {
    alpha(i) = lambda(uniforme ? 0 : i) / (rho(i) * Cp(i));
  });
  end_gpu_timer(Objet_U::computeOnDevice, __KERNEL_NAME__);
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
  ToDo_Kokkos("critical, not easy cause virtual function");
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

bool Loi_Etat_base::has_champ(const Motcle& nom, OBS_PTR(Champ_base)& ref_champ) const
{
  return champs_compris_.has_champ(nom, ref_champ);
}
bool Loi_Etat_base::has_champ(const Motcle& nom) const
{
  return champs_compris_.has_champ(nom);
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
  tab_rho_np1 = tab_rho_n;
}
