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
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Quasi_Compressible/Loi_Etat
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_Multi_GP_QC.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc_Tabule.h>
#include <Debog.h>
#include <Zone_VF.h>
#include <Probleme_base.h>
#include <Param.h>
#include <Champ_Inc_base.h>
#include <DoubleTab.h>

Implemente_liste(REF(Champ_Inc_base));
Implemente_liste(REF(Espece));

Implemente_instanciable_sans_constructeur(Loi_Etat_Multi_GP_QC,"Loi_Etat_Multi_Gaz_Parfait_QC",Loi_Etat_Multi_GP_base);
// XD melange_gaz_parfait loi_etat_base melange_gaz_parfait -1 Mixing of perfect gas.

Loi_Etat_Multi_GP_QC::Loi_Etat_Multi_GP_QC() : correction_fraction_(0),ignore_check_fraction_(0),
  Sc_(-1),dtol_fraction_(1.e-6) { }

// Description:
//    Imprime la loi sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Loi_Etat_Multi_GP_QC::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

// Description:
//    Lecture d'une loi sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree pour la lecture des parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition: l'objet est construit avec les parametres lus
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
//    Associe le fluide a la loi d'etat
// Precondition:
// Parametre: Fluide_Quasi_Compressible& fl
//    Signification: le fluide associe
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Multi_GP_QC::associer_fluide(const Fluide_Dilatable_base& fl)
{
  Loi_Etat_base::associer_fluide(fl);
}

//Description:
//
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
void Loi_Etat_Multi_GP_QC::initialiser_inco_ch()
{
  const int num = liste_Y(0).valeur().valeurs().size();
  Masse_mol_mel.resize(num,1);
  calculer_masse_molaire();
  Loi_Etat_base::initialiser_inco_ch();
}

// Description:
// Associe l inconnue de chaque equation de
// fraction massique a la loi d'etat
// Precondition:
// Parametre: inconnue
//    Signification: l inconnue associee
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: lecture
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Loi_Etat_Multi_GP_QC::associer_inconnue(const Champ_Inc_base& inconnue)
{
  REF(Champ_Inc_base) inco;
  inco = inconnue;
  liste_Y.add(inco);
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
void Loi_Etat_Multi_GP_QC::associer_espece(const Convection_Diffusion_fraction_massique_QC& eq)
{
  liste_especes.add(eq.espece());
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
void Loi_Etat_Multi_GP_QC::calculer_Cp()
{
  Champ_Don& Cp = le_fluide->capacite_calorifique();
  DoubleTab& tab_Cp = Cp.valeurs();
  calculer_tab_Cp(tab_Cp);
  tab_Cp.echange_espace_virtuel();
}

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
void Loi_Etat_Multi_GP_QC::calculer_masse_molaire()
{
  // rabot deplace
  calculer_masse_molaire(Masse_mol_mel);
}

void Loi_Etat_Multi_GP_QC::calculer_masse_molaire(DoubleTab& tab_Masse_mol_mel) const
{
  const int size = tab_Masse_mol_mel.size();
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
      tab_Masse_mol_mel(elem,0) = numer_M(elem) / inv_M(elem);
}

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
//    Calcule la conductivite
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
void Loi_Etat_Multi_GP_QC::calculer_lambda()
{
  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& lambda = le_fluide->conductivite();
  DoubleTab& tab_lambda = lambda.valeurs();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();
  int i, n = tab_lambda.size();

  //La conductivite est soit un champ uniforme soit calculee a partir de la viscosite dynamique et du Pr
  if (sub_type(Champ_Fonc_Tabule,lambda.valeur()))
    {
      lambda.valeur().mettre_a_jour(temperature_.valeur().temps());
      return;
    }
  if (!sub_type(Champ_Uniforme,lambda.valeur()))
    {
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          double mu0 = tab_mu(0,0);
          for (i=0 ; i<n ; i++)  tab_lambda(i,0) = mu0 * Cp_ / Pr_;
        }
      else
        {
          for (i=0 ; i<n ; i++) tab_lambda(i,0) = tab_mu(i,0) * tab_Cp(i,0) / Pr_;
        }
    }
  tab_lambda.echange_espace_virtuel();
}

// Description:
//    Calcule la diffusivite
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
void Loi_Etat_Multi_GP_QC::calculer_alpha()
{
  const Champ_Don& lambda = le_fluide->conductivite();
  const DoubleTab& tab_lambda = lambda.valeurs();
  Champ_Don& alpha=le_fluide->diffusivite();
  DoubleTab& tab_alpha = le_fluide->diffusivite().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();
  int i, n = tab_alpha.size(), isVDF = 0;
  if (alpha.valeur().que_suis_je()=="Champ_Fonc_P0_VDF") isVDF = 1;

  if (isVDF)
    {
      if (sub_type(Champ_Uniforme,lambda.valeur()))
        {
          double lambda0 = tab_lambda(0,0);
          for (i=0 ; i<n ; i++)  tab_alpha(i,0) = lambda0 / ( tab_rho(i,0) * tab_Cp(i,0) );
        }
      else
        {
          for (i=0 ; i<n ; i++) tab_alpha(i,0) = tab_lambda(i,0) / ( tab_rho(i,0) * tab_Cp(i,0) );
        }
    }
  else // VEF
    {
      const IntTab& elem_faces=ref_cast(Zone_VF,le_fluide->vitesse().zone_dis_base()).elem_faces();
      double rhoelem, Cpelem;
      int face, nfe = elem_faces.line_size();

      if (sub_type(Champ_Uniforme,lambda.valeur()))
        {
          double lambda0 = tab_lambda(0,0);
          for (i=0 ; i<n ; i++)
            {
              rhoelem = 0;
              Cpelem = 0;
              for (face=0; face<nfe; face++)
                {
                  rhoelem += tab_rho(elem_faces(i,face),0);
                  Cpelem += tab_Cp(elem_faces(i,face),0);
                }
              rhoelem /= nfe;
              Cpelem /= nfe;
              tab_alpha(i,0) = lambda0 / ( rhoelem * Cpelem );
            }
        }
      else
        {
          for (i=0 ; i<n ; i++)
            {
              rhoelem = 0;
              Cpelem = 0;
              for (face=0; face<nfe; face++)
                {
                  rhoelem += tab_rho(elem_faces(i,face),0);
                  Cpelem += tab_Cp(elem_faces(i,face),0);
                }
              rhoelem /= nfe;
              Cpelem /= nfe;
              tab_alpha(i,0) = tab_lambda(i,0) / ( rhoelem * Cpelem );
            }
        }
    }
  tab_alpha.echange_espace_virtuel();
  Debog::verifier("alpha",tab_alpha);
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

  calculer_masse_molaire();

  double Pth = le_fluide->pression_th();
  int n = tab_rho.size();
  for (int som=0 ; som<n ; som++)
    {
      double r = 8.3143 / Masse_mol_mel(som);
      tab_rho_np1(som) = calculer_masse_volumique_case(Pth,tab_ICh(som,0),r,som);
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
double Loi_Etat_Multi_GP_QC::calculer_masse_volumique_case(double P, double T, double r, int som) const
{
  if (inf_ou_egal(T,0))
    {
      Cerr << finl << "Warning: Temperature T must be defined in Kelvin." << finl;
      Cerr << "Check your data file." << finl;
      Process::exit();
    }
  return P / ( r * T );
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
void Loi_Etat_Multi_GP_QC::calculer_mu0()
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
            // node is elem (VEF) or face (VEF)
            for (int node=0; node<y_j.size(); node++) phi(node) += y_j(node)*phi_ij;
          }
      // We add the mass fraction when i = j
      const DoubleVect& y_i = liste_Y(i).valeur().valeurs();
      for (int node=0; node<y_i.size(); node++) mu(node) += mu_i * y_i(node)/ ( y_i(node) + phi(node) );
    }

  // Dynamic viscosity is on elem
  DoubleTab& tab_mu = le_fluide->viscosite_dynamique().valeurs();
  const int nb_elem = tab_mu.size();
  if (nb_elem==size) // VDF
    {
      for (int elem=0; elem < nb_elem; elem++) tab_mu(elem,0) = mu(elem);
    }
  else // VEF (average on elem from values on face);
    {
      const IntTab& elem_faces=ref_cast(Zone_VF,le_fluide->vitesse().zone_dis_base()).elem_faces();
      int nfe = elem_faces.line_size();
      tab_mu = 0;
      for (int elem=0; elem<nb_elem; elem++)
        {
          for (int face = 0; face < nfe; face++)
            tab_mu(elem,0) += mu(elem_faces(elem, face));

          tab_mu(elem,0) /= nfe;
        }
    }
  tab_mu.echange_espace_virtuel();
  Debog::verifier("tab_mu dans Loi_Etat_Multi_GP_QC::calculer_mu0", tab_mu);
}

// Description:
//    Calcule la viscosite dynamique
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
void Loi_Etat_Multi_GP_QC::calculer_mu()
{
  Champ_Don& mu = le_fluide->viscosite_dynamique();
  if (!sub_type(Champ_Uniforme,mu.valeur()))
    {
      if (sub_type(Champ_Fonc_Tabule,mu.valeur()))
        mu.mettre_a_jour(temperature_.valeur().temps());
      else
        calculer_mu0();
    }
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
double Loi_Etat_Multi_GP_QC::calculer_masse_volumique(double P, double T) const
{
  Cerr << "Error: the Loi_Etat_Multi_GP_QC::calculer_masse_volumique(double P, double T) method should not be used!" << finl;
  Process::exit();
  return -1000.;
}
