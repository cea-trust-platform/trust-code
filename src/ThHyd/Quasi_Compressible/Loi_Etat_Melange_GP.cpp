/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Loi_Etat_Melange_GP.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Etat_Melange_GP.h>
#include <Champ_Uniforme.h>
#include <Champ_Fonc_Tabule.h>
#include <Debog.h>
#include <Zone_VF.h>
#include <Probleme_base.h>
#include <Param.h>

Implemente_liste(REF(Champ_Inc_base));
Implemente_liste(REF(Espece));

Implemente_instanciable_sans_constructeur(Loi_Etat_Melange_GP,"Loi_Etat_Melange_Gaz_Parfait",Loi_Etat_GP);

Loi_Etat_Melange_GP::Loi_Etat_Melange_GP()
{
  Sc_=-1;
}

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
Sortie& Loi_Etat_Melange_GP::printOn(Sortie& os) const
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
Entree& Loi_Etat_Melange_GP::readOn(Entree& is)
{
  double gamma_ = -1;
  // parametre a mettre dans grains
  rho_p_=-1;
  cp_p_=-1;
  rabot_=0;
  nb_famille_grains_ =1 ;
  Pr_=-1;
  Param param(que_suis_je());
  //param.ajouter("Sc",&Sc_,0);
  param.ajouter("Sc",&Sc_,Param::REQUIRED);
  param.ajouter( "Cp",&Cp_);
  param.ajouter( "Prandtl",&Pr_);
  param.ajouter( "gamma",&gamma_);
  param.ajouter( "rho_p",&rho_p_);
  param.ajouter( "cp_p",&cp_p_);
  param.ajouter_flag( "rabot",&rabot_);
  param.ajouter_flag( "verif_fraction",&verif_fraction_);
  param.ajouter( "nb_famille_grains",&nb_famille_grains_);


  param.lire_avec_accolades_depuis(is);




  if (Sc_==-1)
    {
      Cerr<<"ERREUR : on attendait la definition du nombre de Schmidt (constante)"<<finl;
      abort();
    }

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
void Loi_Etat_Melange_GP::associer_fluide(const Fluide_Quasi_Compressible& fl)
{
  le_fluide = fl;
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
void Loi_Etat_Melange_GP::initialiser_inco_ch()
{
  Masse_mol_mel.resize(liste_Y(0).valeur().valeurs().size());
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
void Loi_Etat_Melange_GP::associer_inconnue(const Champ_Inc_base& inconnue)
{
  REF(Champ_Inc_base) inco;
  inco = inconnue;
  liste_Y.add(inco);
}

// Description:
// Associe les proprietes physiques
// d une espece a la loi d'etat
// Precondition:
// Parametre: M, Cp, mu, lambda
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
void Loi_Etat_Melange_GP::associer_espece(const Convection_Diffusion_fraction_massique_QC& eq)
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
void Loi_Etat_Melange_GP::calculer_Cp()
{
  Champ_Don& Cp = le_fluide->capacite_calorifique();
  DoubleTab& tab_Cp = Cp.valeurs();
  calculer_tab_Cp(tab_Cp);
  tab_Cp.echange_espace_virtuel();
}
void Loi_Etat_Melange_GP::calculer_tab_Cp(DoubleTab& tab_Cp) const
{


  //Actuellement on suppose que Cp est pris constant pour chacune des especes
  tab_Cp=0;

  for (int i=0; i<liste_Y.size(); i++)
    {
      const DoubleTab& Y_i=liste_Y(i).valeur().valeurs();
      const double& cp_i=liste_especes(i).valeur().capacite_calorifique().valeurs()(0,0);
      assert(cp_i>0);
      for (int elem=0; elem<Y_i.size(); elem++)
        {
          tab_Cp(elem) +=   Y_i(elem)*cp_i;
        }
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
void Loi_Etat_Melange_GP::calculer_masse_molaire()
{
  // rabot deplace
  calculer_masse_molaire(Masse_mol_mel);
}
void Loi_Etat_Melange_GP::calculer_masse_molaire(DoubleTab& tab_Masse_mol_mel) const
{
  //Faire methode preparer_calcul() pour dimensionnement de Masse_mol_mel
  ////Masse_mol_mel.resize(liste_Y(0).valeur().valeurs().size());
  int size=tab_Masse_mol_mel.size();
  ArrOfDouble inv_M(size),numer_M(size);

  for (int i=0; i<liste_Y.size(); i++)
    {

      double M_i=liste_especes(i).valeur().masse_molaire();
      const DoubleTab& Y_i=liste_Y(i).valeur().valeurs();
      double min_Y_i=local_min_vect(Y_i);
      double max_Y_i=local_max_vect(Y_i);
      if ((min_Y_i<-1e-6)||(max_Y_i>1+1e-6))
        {
          Cerr << "Arret : (min_Y_i<-1e-6)||(max_Y_i>1+1e-6) " << i<<finl;
          Cerr << "min_Y_i " << min_Y_i << finl;
          Cerr << "max_Y_i " << max_Y_i << finl;
          exit();
        }
      for (int elem=0; elem<size; elem++)
        {
          numer_M(elem)+=Y_i(elem);
          inv_M(elem) += Y_i(elem)/M_i;
        }
    }

  assert(liste_Y(0).valeur().valeurs().size()==size);
  for (int elem=0; elem<size; elem++)
    {
      if (inv_M(elem)>1.e-8)
        tab_Masse_mol_mel(elem) = numer_M(elem)/ inv_M(elem);
    }

}
void Loi_Etat_Melange_GP::rabot(int futur)
{

  DoubleTab test( liste_Y(0).valeur().valeurs()) ;
  test=0;

  for (int i=0; i<liste_Y.size(); i++)
    {
      //Cerr<<"i="<<i<<finl;
      //Cerr<<"Masse_mol="<<liste_especes(i).valeur().masse_molaire()<<finl;
      DoubleTab& Y_i=liste_Y(i).valeur().futur(futur);
      double min_Y_i=local_min_vect(Y_i);
      double max_Y_i=local_max_vect(Y_i);
      Cerr<<" TEST Y_i "<<i<<" "<<min_Y_i<< " "<<max_Y_i<<finl;
      if (((min_Y_i<-1e-6)||(max_Y_i>1+1e-6))&&(!rabot_))
        exit();
      if (rabot_)
        for (int elem=0; elem<Y_i.size(); elem++)
          {
            if ( Y_i(elem)<0)
              {
                Cerr<<" on rabote "<<endl;
                Y_i(elem)=0.;
              }

            //Cerr<<"i, elem, Y="<<i<<" "<<elem<<" "<<liste_Y(i).valeur().valeurs()(elem)<<finl;
          }
      test+=Y_i;
    }
  double min_s=mp_min_vect(test);
  double max_s=mp_max_vect(test);
  Cerr<<" Somme des fractions min "<<min_s<< " max "<<max_s<<finl;
  if (rabot_)
    for (int i=0; i<liste_Y.size(); i++)
      {
        DoubleTab& Y_i=liste_Y(i).valeur().futur(futur);
        for (int elem=0; elem<Y_i.size(); elem++)
          {
            Y_i(elem)/=(test(elem)+DMINFLOAT);
          }
      }
  else
    {
      if ((!est_egal(min_s,1,1e-5))||(!est_egal(max_s,1,1e-5)))
        {
          Cerr<<" On arrete tout la somme ne fait plus 1 .... " <<finl;
          exit();
        }
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
void Loi_Etat_Melange_GP::calculer_lambda()
{

  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& lambda = le_fluide->conductivite();
  DoubleTab& tab_lambda = lambda.valeurs();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();
  int i, n=tab_lambda.size();

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
          for (i=0 ; i<n ; i++)
            tab_lambda[i] = mu0 * Cp_ / Pr_;
        }
      else
        {
          for (i=0 ; i<n ; i++)
            tab_lambda[i] = tab_mu[i] * tab_Cp(i) / Pr_;
        }
    }

  if (Pr_!=-1)
    {

      Cerr<< "Prandtl n est plus une option valide de "<<que_suis_je()<<finl;
      const Champ_Don& mubis = le_fluide->viscosite_dynamique();

      if (sub_type(Champ_Uniforme,mubis.valeur()))
        {

          const DoubleTab& tab_mubis = mubis.valeurs();
          const DoubleTab& tab_Cpbis = le_fluide->capacite_calorifique().valeurs();
          double mu0 = tab_mubis(0,0);
          double lambda_test = mu0 * tab_Cpbis(0)/ Pr_;


          Cerr<<"mettre (peut etre ?) lambda a "<<lambda_test<<finl;
        }
      // exit();
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
void Loi_Etat_Melange_GP::calculer_alpha()
{

  const Champ_Don& lambda = le_fluide->conductivite();
  const DoubleTab& tab_lambda = lambda.valeurs();
  Champ_Don& alpha=le_fluide->diffusivite();
  DoubleTab& tab_alpha = le_fluide->diffusivite().valeurs();
  const DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  const DoubleTab& tab_Cp = le_fluide->capacite_calorifique().valeurs();

  int isVDF=0;
  if (alpha.valeur().que_suis_je()=="Champ_Fonc_P0_VDF") isVDF=1;
  int i, n=tab_alpha.size();
  if (isVDF)
    {
      if (sub_type(Champ_Uniforme,lambda.valeur()))
        {
          double lambda0 = tab_lambda(0,0);
          for (i=0 ; i<n ; i++)
            {
              tab_alpha[i] = lambda0 / (tab_rho[i]*tab_Cp(i));
            }
        }
      else
        {
          for (i=0 ; i<n ; i++)
            {
              tab_alpha[i] = tab_lambda(i) / (tab_rho[i]*tab_Cp(i));
            }
        }
    }
  else
    {
      const IntTab& elem_faces=ref_cast(Zone_VF,le_fluide->vitesse().zone_dis_base()).elem_faces();
      double rhoelem,Cpelem;
      int nfe=elem_faces.dimension(1),face;
      if (sub_type(Champ_Uniforme,lambda.valeur()))
        {
          double lambda0 = tab_lambda(0,0);
          for (i=0 ; i<n ; i++)
            {
              rhoelem=0;
              Cpelem=0;
              for (face=0; face<nfe; face++)
                {
                  rhoelem+=tab_rho(elem_faces(i,face));
                  Cpelem+=tab_Cp(elem_faces(i,face));
                }
              rhoelem/=nfe;
              Cpelem/=nfe;
              tab_alpha[i] = lambda0 / (rhoelem*Cpelem);
            }
        }
      else
        {
          for (i=0 ; i<n ; i++)
            {
              rhoelem=0;
              Cpelem=0;
              for (face=0; face<nfe; face++)
                {
                  rhoelem+=tab_rho(elem_faces(i,face));
                  Cpelem+=tab_Cp(elem_faces(i,face));
                }
              rhoelem/=nfe;
              Cpelem/=nfe;
              tab_alpha[i] = tab_lambda(i) / (rhoelem*Cpelem);
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
void Loi_Etat_Melange_GP::calculer_masse_volumique()
{
  const DoubleTab& tab_ICh = le_fluide->inco_chaleur().valeurs();
  DoubleTab& tab_rho = le_fluide->masse_volumique().valeurs();
  double Pth = le_fluide->pression_th();
  int som, n=tab_rho.size();
  double r;

  //Correction pour calculer la masse volumique a partir de la pression
  //qui permet de conserver la masse
  if ((rabot_)||(verif_fraction_))
    {
      rabot(0);
      // rabot(0);
    }

  calculer_masse_molaire();

  for (som=0 ; som<n ; som++)
    {
      r=8.314/Masse_mol_mel(som);
      tab_rho_np1[som] = calculer_masse_volumique_case(Pth,tab_ICh[som],r,som);

      //Correction pour calculer la masse volumique a partir de la pression
      //qui permet de conserver la masse

      tab_rho[som] = (tab_rho_n[som] + tab_rho_np1[som])/2.;
    }
  const Champ_base& rho_m=le_fluide->get_champ("rho_gaz");
  ref_cast_non_const(DoubleTab,rho_m.valeurs())=tab_rho_np1;
  tab_rho.echange_espace_virtuel();
  tab_rho_np1.echange_espace_virtuel();


  le_fluide->calculer_rho_face(tab_rho_np1);

  Debog::verifier("Loi_Etat_Melange_GP::calculer_masse_volumique, tab_rho_np1",tab_rho_np1);
  Debog::verifier("Loi_Etat_Melange_GP::calculer_masse_volumique, tab_rho",tab_rho);

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
double Loi_Etat_Melange_GP::calculer_masse_volumique_case(double P, double T, double r, int som) const
{
  if (inf_ou_egal(T,0))
    {
      Cerr << finl << "Temperature T must be defined in Kelvin." << finl;
      Cerr << "Check your data file." << finl;
      exit();
    }

  return P/(r*T);
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
void Loi_Etat_Melange_GP::calculer_mu0()
{

  Champ_Don& mu = le_fluide->viscosite_dynamique();
  DoubleTab& tab_mu = mu.valeurs();
  int size;
  size = liste_Y(0).valeur().valeurs().size();
  DoubleTab phi(size);
  tab_mu =0;
  phi = 0.;
  double phi_ij,M_i,M_j,mu_i,mu_j,val1,val2;


  for (int i=0; i<liste_Y.size(); i++)
    {
      phi = 0.;
      for (int j=0; j<liste_Y.size(); j++)
        if (j!=i)
          {
            M_i=liste_especes(i).valeur().masse_molaire();
            M_j=liste_especes(j).valeur().masse_molaire();
            mu_i=liste_especes(i).valeur().viscosite_dynamique().valeurs()(0,0);
            mu_j=liste_especes(j).valeur().viscosite_dynamique().valeurs()(0,0);

            val1=1./(sqrt(8.*(1.+(M_i/M_j))));
            val2=(1.+sqrt(mu_i/mu_j)*pow(M_j/M_i,0.25))*(1.+sqrt(mu_i/mu_j)*pow(M_j/M_i,0.25));
            phi_ij=val1*val2;

            for (int elem=0; elem<liste_Y(j).valeur().valeurs().size(); elem++)
              {
                ////phi_ij = 1.;

                phi(elem) +=  liste_Y(j).valeur().valeurs()(elem)*phi_ij;
              }
          }

      for (int elem=0; elem<liste_Y(i).valeur().valeurs().size(); elem++)
        {
          tab_mu(elem) +=
            (liste_Y(i).valeur().valeurs()(elem)*liste_especes(i).valeur().viscosite_dynamique().valeurs()(0,0))/(liste_Y(i).valeur().valeurs()(elem)+phi(elem));
        }
    }
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
void Loi_Etat_Melange_GP::calculer_mu()
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
void Loi_Etat_Melange_GP::calculer_mu_sur_Sc()
{
  const Champ_Don& mu = le_fluide->viscosite_dynamique();
  const DoubleTab& tab_mu = mu.valeurs();
  Champ_Don& mu_sur_Sc = le_fluide->mu_sur_Schmidt();
  DoubleTab& tab_mu_sur_Sc = mu_sur_Sc.valeurs();

  int i, n=tab_mu_sur_Sc.size();

  if (!sub_type(Champ_Uniforme,mu_sur_Sc.valeur()))
    {
      //DoubleTab& tab_T = temperature_.valeurs();
      if (sub_type(Champ_Uniforme,mu.valeur()))
        {
          double mu0 = tab_mu(0,0);
          for (i=0 ; i<n ; i++)
            {
              tab_mu_sur_Sc[i] = mu0 /Sc_;
            }
        }
      else
        {
          for (i=0 ; i<n ; i++)
            {
              tab_mu_sur_Sc[i] = tab_mu[i]/Sc_;
            }
        }
    }
  else
    {
      //mu_sur_Sc uniforme

      double mu0 = tab_mu(0,0);
      tab_mu_sur_Sc(0,0) = mu0/Sc_;
    }

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
double Loi_Etat_Melange_GP::calculer_masse_volumique(double P, double T) const
{
  Cerr<<"Loi_Etat_Melange_GP::calculer_masse_volumique(double P, double T) ne doit pas etre utilisee "<<finl;
  exit();
  if (inf_ou_egal(T,0))
    {
      Cerr << finl << "Temperature T must be defined in Kelvin." << finl;
      Cerr << "Check your data file." << finl;
      exit();
    }

  return P/(R_*T);
}



