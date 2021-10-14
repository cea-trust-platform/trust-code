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
// File:        Loi_Etat_Multi_GP_QC.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Dilatable/Quasi_Compressible/Loi_Etat
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_Multi_GP_QC.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Uniforme.h>
#include <Probleme_base.h>
#include <Champ_Inc_base.h>
#include <DoubleTab.h>
#include <Zone_VF.h>
#include <Debog.h>
#include <Param.h>

Implemente_liste(REF(Espece));

Implemente_instanciable_sans_constructeur(Loi_Etat_Multi_GP_QC,"Loi_Etat_Multi_Gaz_Parfait_QC",Loi_Etat_Multi_GP_base);
// XD multi_gaz_parfait_QC loi_etat_gaz_parfait_base multi_gaz_parfait_QC -1 Class for perfect gas multi-species mixtures state law used with a quasi-compressible fluid.

Loi_Etat_Multi_GP_QC::Loi_Etat_Multi_GP_QC() : correction_fraction_(0),ignore_check_fraction_(0),
  Sc_(-1),dtol_fraction_(1.e-6) { }

Sortie& Loi_Etat_Multi_GP_QC::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

Entree& Loi_Etat_Multi_GP_QC::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("Sc",&Sc_,Param::REQUIRED); // XD_ADD_P double Schmidt number of the gas Sc=nu/D (D: diffusion coefficient of the mixing).
  param.ajouter( "Prandtl",&Pr_,Param::REQUIRED); // XD_ADD_P double Prandtl number of the gas Pr=mu*Cp/lambda
  param.ajouter( "Cp",&Cp_); // XD_ADD_P double Specific heat at constant pressure of the gas Cp.
  param.ajouter( "dtol_fraction",&dtol_fraction_); // XD_ADD_P double Delta tolerance on mass fractions for check testing (default value 1.e-6).
  param.ajouter_flag( "correction_fraction",&correction_fraction_); // XD_ADD_P flag To force mass fractions between 0. and 1.
  param.ajouter_flag( "ignore_check_fraction",&ignore_check_fraction_); // XD_ADD_P flag Not to check if mass fractions between 0. and 1.
  param.lire_avec_accolades_depuis(is);
  return is;
}

// Description:
// Associe les proprietes physiques
// d une espece a la loi d'etat
// Precondition:
// Parametre: eq
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Multi_GP_QC::associer_espece(const Convection_Diffusion_Espece_Multi_QC& eq)
{
  liste_especes.add(eq.espece());
}

// Description:
//    Calcule la masse molaire du melange (M)
//    M depend de la mase molaire de chaque espece et de la composition du melange (Yi)
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
void Loi_Etat_Multi_GP_QC::calculer_masse_molaire(DoubleTab& tab_masse_mol_mel) const
{
  const int size = tab_masse_mol_mel.size();
  ArrOfDouble inv_M(size), numer_M(size);

  for (int i=0; i<liste_Y.size(); i++)
    {
      double M_i=liste_especes(i).valeur().masse_molaire();
      const DoubleTab& Y_i=liste_Y(i).valeur().valeurs();
      double min_Y_i=local_min_vect(Y_i);
      double max_Y_i=local_max_vect(Y_i);
      if ((min_Y_i<0.-dtol_fraction_)||(max_Y_i>1.+dtol_fraction_))
        {
          Cerr << "Warning : (min_Y_i<-" << dtol_fraction_ << ")||(max_Y_i>1+" << dtol_fraction_ <<") for the " << i << "th mass fraction:" << finl;
          Cerr << "  min_Y_i = " << min_Y_i << finl;
          Cerr << "  max_Y_i = " << max_Y_i << finl;
          if (!ignore_check_fraction_) Process::exit();
        }
      for (int elem=0; elem<size; elem++)
        {
          numer_M(elem) += Y_i(elem,0);
          inv_M(elem) += Y_i(elem,0)/M_i;
        }
    }

  assert(liste_Y(0).valeur().valeurs().size()==size);

  for (int elem=0; elem<size; elem++)
    if (inv_M(elem)>1.e-8)
      tab_masse_mol_mel(elem,0) = numer_M(elem) / inv_M(elem);
}

// Description:
//    Calcule le Cp du melange
//    Le Cp depend du Cp de chaque espece et de la composition du melange (Yi)
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
void Loi_Etat_Multi_GP_QC::calculer_tab_Cp(DoubleTab& tab_Cp) const
{
  // FIXME : Actuellement on suppose que Cp est pris constant pour chacune des especes
  tab_Cp=0;
  for (int i=0; i<liste_Y.size(); i++)
    {
      const DoubleTab& Y_i=liste_Y(i).valeur().valeurs();
      const double& cp_i=liste_especes(i).valeur().capacite_calorifique().valeurs()(0,0);
      assert(cp_i>0);
      for (int elem=0; elem<Y_i.size(); elem++) tab_Cp(elem,0) += Y_i(elem,0)*cp_i;
    }
}

// Description:
//    Corrections explicites pour les fractions massiques pour forcer
//    la somme a 1 et sans valeurs negatives
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
void Loi_Etat_Multi_GP_QC::rabot(int futur)
{
  DoubleTab test( liste_Y(0).valeur().valeurs()) ;
  test=0;

  for (int i=0; i<liste_Y.size(); i++)
    {
      DoubleTab& Y_i=liste_Y(i).valeur().futur(futur);
      double min_Y_i=local_min_vect(Y_i);
      double max_Y_i=local_max_vect(Y_i);
      Cerr << "Verification of the " << i << "th mass fraction:" << finl;
      Cerr << "  min_Y_i = " << min_Y_i << finl;
      Cerr << "  max_Y_i = " << max_Y_i << finl;
      if ((min_Y_i<0.-dtol_fraction_)||(max_Y_i>1.+dtol_fraction_))
        {
          Cerr << "  Warning : (min_Y_i<-" << dtol_fraction_ << ")||(max_Y_i>1+" << dtol_fraction_ <<")" << finl;
          if (!ignore_check_fraction_) Process::exit();
        }
      for (int elem=0; elem<Y_i.size(); elem++)
        {
          if (Y_i(elem,0)<0.)
            {
              Y_i(elem,0)=0.;
              Cerr << "  Y_i forced to " << Y_i(elem,0) << " on node " << elem << finl;
            }
          if (Y_i(elem,0)>1.)
            {
              Y_i(elem,0)=1.;
              Cerr << "  Y_i forced to " << Y_i(elem,0) << " on node " << elem << finl;
            }
        }
      test+=Y_i;
    }
  double min_s=mp_min_vect(test);
  double max_s=mp_max_vect(test);
  Cerr << "Verification of the sum of the mass fractions:" << finl;
  Cerr << "  Sum(min_Y_i) =  " << min_s << finl;
  Cerr << "  Sum(max_Y_i) =  " << max_s << finl;
  for (int i=0; i<liste_Y.size(); i++)
    {
      DoubleTab& Y_i=liste_Y(i).valeur().futur(futur);
      for (int elem=0; elem<Y_i.size(); elem++) Y_i(elem,0)/=(test(elem)+DMINFLOAT);
    }
  if ((!est_egal(min_s,1.,dtol_fraction_))||(!est_egal(max_s,1.,dtol_fraction_)))
    {
      Cerr << "  Warning: the sum of the mass fractions is not equal to 1." <<finl;
      if (!ignore_check_fraction_) Process::exit();
    }
}

// Description:
//    Recalcule la masse volumique
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
void Loi_Etat_Multi_GP_QC::calculer_masse_volumique()
{
  const DoubleTab& tab_ICh = le_fluide->inco_chaleur().valeurs();
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();

  if (correction_fraction_) rabot(0);

  Loi_Etat_Multi_GP_base::calculer_masse_molaire();

  double Pth = le_fluide->pression_th();
  int n = tab_rho.size();
  for (int som=0 ; som<n ; som++)
    {
      double r = 8.3143 / masse_mol_mel(som);
      tab_rho_np1(som) = calculer_masse_volumique(Pth,tab_ICh(som,0),r);
      tab_rho(som,0) = 0.5 * ( tab_rho_n(som) + tab_rho_np1(som) );
    }
  const Champ_base& rho_m=le_fluide->get_champ("rho_gaz");
  ref_cast_non_const(DoubleTab,rho_m.valeurs())=tab_rho_np1;
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();
  le_fluide->calculer_rho_face(tab_rho_np1);
  Debog::verifier("Loi_Etat_Multi_GP_QC::calculer_masse_volumique, tab_rho_np1",tab_rho_np1);
  Debog::verifier("Loi_Etat_Multi_GP_QC::calculer_masse_volumique, tab_rho",tab_rho);
}

double Loi_Etat_Multi_GP_QC::calculer_masse_volumique(double P, double T, double r) const
{
  return Loi_Etat_Multi_GP_base::calculer_masse_volumique(P,T,r);
}

double Loi_Etat_Multi_GP_QC::calculer_masse_volumique(double P, double T) const
{
  return Loi_Etat_Multi_GP_base::calculer_masse_volumique(P,T);
}

// Description:
//    Calcule la viscosite dynamique de reference (depend des Yi)
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
void Loi_Etat_Multi_GP_QC::calculer_mu_wilke()
{
  // With Wilke formulation: https://aip.scitation.org/doi/pdf/10.1063/1.1747673
  const int size = liste_Y(0).valeur().valeurs().size(), list_size = liste_Y.size();
  DoubleTab phi(size);
  DoubleTab mu(size);
  mu = 0.;
  phi = 0.;

  for (int i=0; i<list_size; i++)
    {
      phi = 0.;
      double& M_i=liste_especes(i).valeur().masse_molaire();
      double mu_i=liste_especes(i).valeur().viscosite_dynamique().valeurs()(0,0);
      for (int j=0; j<list_size; j++)
        if (j!=i)
          {
            double& M_j=liste_especes(j).valeur().masse_molaire();
            double mu_j=liste_especes(j).valeur().viscosite_dynamique().valeurs()(0,0);
            double a=1.+sqrt(mu_i/mu_j)*pow(M_j/M_i,0.25);
            double b=sqrt(8.*(1.+(M_i/M_j)));
            /*
             * E Saikali : XXX XXX XXX
             * I think that a term is missing in this formulation
             * (phi_ij must be multiplied by Mi/Mj since we use mass fractions and not mol fractions !)
             */
            double phi_ij=a*a/b;

            const DoubleVect& y_j = liste_Y(j).valeur().valeurs();
            // node is elem (VDF) or face (VEF)
            for (int node=0; node<y_j.size(); node++) phi(node) += y_j(node)*phi_ij;
          }
      // We add the mass fraction when i = j
      const DoubleVect& y_i = liste_Y(i).valeur().valeurs();
      for (int node=0; node<y_i.size(); node++) mu(node) += mu_i * y_i(node)/ ( y_i(node) + phi(node) );
    }
  calculer_tab_mu(mu, size);
}

// Description:
//    Calcule la viscosite dynamique sur Schmidt
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
void Loi_Etat_Multi_GP_QC::calculer_mu_sur_Sc()
{
  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& mu_sur_Sc = le_fluide->mu_sur_Schmidt();
  DoubleTab& tab_mu_sur_Sc = mu_sur_Sc.valeurs();

  if (!sub_type(Champ_Uniforme,mu_sur_Sc.valeur()))
    {
      const int n = tab_mu_sur_Sc.size();
      if (sub_type(Champ_Uniforme,mu.valeur()))
        for (int i=0 ; i<n ; i++) tab_mu_sur_Sc(i,0) = tab_mu(0,0) / Sc_;
      else
        for (int i=0 ; i<n ; i++) tab_mu_sur_Sc(i,0) = tab_mu(i,0)/Sc_;
    }
  else //mu_sur_Sc uniforme
    tab_mu_sur_Sc(0,0) = tab_mu(0,0) / Sc_;

  double temps_champ = mu->temps();
  mu_sur_Sc.valeur().changer_temps(temps_champ);
  tab_mu_sur_Sc.echange_espace_virtuel();
}
