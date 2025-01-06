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

#include <Fluide_Dilatable_base.h>
#include <Champ_Fonc_Tabule.h>
#include <Loi_Etat_GP_base.h>
#include <Champ_Uniforme.h>
#include <Domaine_VF.h>
#include <Motcle.h>

Implemente_base_sans_constructeur(Loi_Etat_GP_base,"Loi_Etat_Gaz_Parfait_base",Loi_Etat_base);
// XD loi_etat_gaz_parfait_base loi_etat_base loi_etat_gaz_parfait_base -1 Basic class for perfect gases state laws used with a dilatable fluid.

Loi_Etat_GP_base::Loi_Etat_GP_base() : Cp_(-1), R_(-1) { }

Sortie& Loi_Etat_GP_base::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Loi_Etat_GP_base::readOn(Entree& is)
{
  return is;
}

/*! @brief Renvoie le type de fluide associe.
 *
 * @param (Sortie& os) le flot de sortie pour l'impression
 * @return (Sortie&) le flot de sortie modifie
 */
const Nom Loi_Etat_GP_base::type_fluide() const
{
  return "Gaz_Parfait";
}

/*! @brief Associe le fluide a la loi d'etat
 *
 * @param (Fluide_Dilatable_base& fl) le fluide associe
 */
void Loi_Etat_GP_base::associer_fluide(const Fluide_Dilatable_base& fl)
{
  Loi_Etat_base::associer_fluide(fl);
  le_fluide->set_Cp(Cp_);
}

/*! @brief Initialise la loi d'etat : calcul Pth
 *
 */
void Loi_Etat_GP_base::initialiser()
{
  const DoubleTab& tab_Temp = le_fluide->inco_chaleur().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  int ntot=tab_Temp.size_totale();
  tab_rho_n=tab_Temp;
  for (int i=0 ; i<ntot ; i++)
    tab_rho_n(i) = tab_rho(i,0);
  remplir_T();
}

/*! @brief Remplit le tableau de la temperature : T=temp+273.
 *
 * 15
 *
 */
void Loi_Etat_GP_base::remplir_T()
{
  const DoubleTab& tab_Temp = le_fluide->inco_chaleur().valeurs();
  temperature_->valeurs() = tab_Temp;
}

/*! @brief Calcule le Cp NE FAIT RIEN : le Cp est constant
 *
 */
void Loi_Etat_GP_base::calculer_Cp()
{
  /* Do nothing */
}

/*! @brief Calcule la conductivite
 *
 */
void Loi_Etat_GP_base::calculer_lambda()
{
  const Champ_Don_base& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don_base& lambda = le_fluide->conductivite();
  DoubleTab& tab_lambda =lambda.valeurs();
  //La conductivite est soit un champ uniforme soit calculee a partir de la viscosite dynamique et du Pr
  if (sub_type(Champ_Fonc_Tabule,lambda))
    {
      lambda.mettre_a_jour(temperature_->temps());
      return;
    }
  if (!sub_type(Champ_Uniforme,lambda))
    {
      if (sub_type(Champ_Uniforme,mu))
        {
          double mu0 = tab_mu(0,0);
          tab_lambda *= (mu0 * Cp_ / Pr_);
        }
      else
        {
          tab_lambda = tab_mu;
          tab_lambda *= Cp_ / Pr_;
        }
    }
  else
    {
      if (sub_type(Champ_Uniforme,mu))
        {
          tab_lambda(0,0) = mu.valeurs()(0,0) * Cp_ / Pr_;
        }
      else
        {
          Cerr << finl ;
          Cerr << "If lambda is of type Champ_Uniform, mu must also be of type Champ_Uniforme !"<< finl;
          Cerr << "If needed, you can use a Champ_Fonc_Fonction or a Champ_Fonc_Tabule with a constant for mu."<< finl ;
          Cerr << finl ;
          Process::exit();
        }
    }
  tab_lambda.echange_espace_virtuel();
}

/*! @brief Calcule la diffusivite
 *
 */
void Loi_Etat_GP_base::calculer_alpha()
{
  const Champ_Don_base& champ_lambda = le_fluide->conductivite();
  const DoubleTab& tab_lambda = champ_lambda.valeurs();
  Champ_Don_base& champ_alpha = le_fluide->diffusivite();
  DoubleTab& tab_alpha = le_fluide->diffusivite().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();

  int isVDF=0;
  if (champ_alpha.que_suis_je()=="Champ_Fonc_P0_VDF") isVDF = 1;
  int n=tab_alpha.size();
  bool lambda_uniforme = sub_type(Champ_Uniforme,champ_lambda);
  if (isVDF)
    {
      ToDo_Kokkos("critical");
      for (int i=0 ; i<n ; i++)
        tab_alpha(i,0) = (lambda_uniforme ? tab_lambda(0,0) : tab_lambda(i,0)) / (tab_rho(i,0) * Cp_);
    }
  else
    {
      const IntTab& tab_elem_faces = ref_cast(Domaine_VF,le_fluide->vitesse().domaine_dis_base()).elem_faces();
      int nfe = tab_elem_faces.line_size();
      double Cp = Cp_;
      CIntTabView elem_faces = tab_elem_faces.view_ro();
      CDoubleTabView lambda = tab_lambda.view_ro();
      CDoubleTabView rho = tab_rho.view_ro();
      DoubleTabView alpha = tab_alpha.view_wo();
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), n, KOKKOS_LAMBDA(
                             const int i)
      {
        double rhoelem=0;
        for (int face=0; face<nfe; face++) rhoelem += rho(elem_faces(i,face),0);
        rhoelem /= nfe;
        alpha(i,0) = (lambda_uniforme ? lambda(0,0) : lambda(i,0))/ ( rhoelem * Cp );
      });
      end_gpu_timer(__KERNEL_NAME__);
    }
  tab_alpha.echange_espace_virtuel();
}

double Loi_Etat_GP_base::calculer_masse_volumique(double P, double T) const
{
  if (inf_ou_egal(T,0))
    {
      Cerr << finl << "Error, we find a temperature of " << T << " !" << finl;
      Cerr << "Either your calculation has diverged or you don't define" << finl;
      Cerr << "temperature in Kelvin somewhere in your data file." << finl;
      Cerr << "Check your data file." << finl;
      Process::exit();
    }
  return P / ( R_ * T );
}

/*! @brief Calcule la pression avec la temperature et la masse volumique
 *
 */
double Loi_Etat_GP_base::inverser_Pth(double T, double rho)
{
  return rho * R_ * T;
}

/*! @brief Calcule la masse volumique
 *
 */
void Loi_Etat_GP_base::calculer_masse_volumique()
{
  Loi_Etat_base::calculer_masse_volumique();
}
