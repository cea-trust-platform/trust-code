/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Reaction.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Chimie
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <Reaction.h>
#include <Parser_U.h>
#include <Param.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Discretisation_base.h>

Implemente_instanciable(Reaction,"Reaction",Objet_U);
Implemente_liste(Reaction);

Sortie& Reaction::printOn(Sortie& os) const
{
  os<<" { "<<finl;
  os<<"reactifs "<<reactifs_<<finl;
  os<<"produits "<<produits_<<finl;
  os<<"exposant_beta "<<beta_<<finl;
  os<<"constante_taux_reaction "<<constante_taux_reaction_<<finl;
  /*  os<<"c0_D_G "<<c0_D_G_<<" Pa.m2/s"<<finl;
      os<<"c1_D_G "<<c1_D_G_<<" Pa.m2/s/K"<<finl;
      os<<"c2_D_G "<<c2_D_G_<<" Pa.m2/s/K2"<<finl;
      os<<"d_D_G  "<<d_D_G_<<" Pa.m2/s/K^(3/2)"<<finl;
      os<<"c_Re05Sc033 "<<c_Re05Sc033_<<finl; */
  os<<"Sc_t "<<Sc_t_<<finl;
  /*  os<<"d_min "<<d_min_<<finl;
      os<<"d_max "<<d_max_<<finl; */
  os<<"enthalpie_reaction "<<enthalpie_reaction_<<" J/mol"<<finl;
  os<<"energie_activation "<<Ea_<<" J/mol"<<finl;
  os<<"proportion_max_admissible "<<proportion_max_admissible_<<finl;
  os<<"nb_sous_pas_de_temps_reaction "<<nb_sous_pas_de_temps_reaction_<<finl;
  os<<"contre_reaction "<<contre_reaction_<<finl;
  os<<"contre_energie_activation "<<c_r_Ea_<<finl;
  /*
    os<<"nb_iter_impl_contre_reaction "<<nb_iter_impl_contre_reaction_<<finl;
    os<<"sous_relax__impl_contre_reaction "<<sous_relax__impl_contre_reaction_<<finl; */
  os<<" } "<<finl;
  return os;
}

void extract_coef_local(ArrOfDouble& coeff_reactifs,const Nom& reactifs_,const Motcles& list_var)
{
  int sz=list_var.size();
  assert(sz==list_var.size());

  Parser_U parser;
  parser.setNbVar(sz);
  for (int i=0; i<sz; i++)
    parser.addVar(list_var[i]);

  Nom copie(reactifs_);
  parser.setString(copie);
  parser.parseString();
  coeff_reactifs.resize_array(sz);
  for (int i=0; i<sz; i++)
    {
      for (int j=0; j<sz; j++)
        parser.setVar(j,0);
      parser.setVar(i,1);
      double val=parser.eval();
      coeff_reactifs[i]=val;
    }
  Cerr<<reactifs_<<" "<<list_var<<" "<<coeff_reactifs<<finl;
  for (int i=0; i<sz; i++) Cerr<<coeff_reactifs[i]<<"*"<<list_var[i]<<"+";
  Cerr<<finl;
}

// Descrition:
// prend une liste de nom de variables et extrait les coefficients de la reaction, et verifie que les masses molaires sont coherentes
void Reaction::extract_coef(ArrOfDouble& coeff_reactifs,ArrOfDouble& coeff_produits,const Motcles& list_var,const ArrOfDouble& masse_molaire) const
{
  assert(masse_molaire.size_array()==list_var.size());
  extract_coef_local(coeff_reactifs,reactifs_, list_var);
  extract_coef_local(coeff_produits,produits_, list_var);
}

void Reaction::completer(const Motcles& list_var,const ArrOfDouble& masse_molaire)
{
  // test

  ArrOfDouble coeff_reactifs;
  ArrOfDouble coeff_produits;
  extract_coef(coeff_reactifs,coeff_produits,list_var,masse_molaire);
  int sz=list_var.size();
  double mreactif=0,mproduit=0;
  coeff_Y_.resize_array(sz);
  coeff_Y_=0;
  coeff_stoechio_.resize_array(sz);
  coeff_stoechio_=0;
  coeff_activite_.resize_array(sz);
  coeff_activite_=0;
  for (int p=0; p<sz; p++)
    {
      mreactif+=masse_molaire[p]*coeff_reactifs[p];
      mproduit+=masse_molaire[p]*coeff_produits[p];
      coeff_Y_[p]+=masse_molaire[p]*coeff_reactifs[p];
      coeff_Y_[p]-=masse_molaire[p]*coeff_produits[p];
      coeff_stoechio_[p]+=coeff_reactifs[p];
      coeff_stoechio_[p]-=coeff_produits[p];
    }
  if (!est_egal(mreactif,mproduit))
    {
      Cerr<<" erreur dans la chimie de "<<(*this)<<finl;
      Cerr<<" mreactif "<< mreactif<<finl;
      Cerr<<" mproduit "<< mproduit<<finl;
      exit();
    }
  //coeff_Y_/=mreactif;
  save_alias_=list_var;
  Cerr<<" Extraction des activites "<<finl;
  extract_coef_local(coeff_activite_,activite_,list_var);
}

Entree& Reaction::readOn(Entree& is)
{
  Ea_=0;                            // J/mol
  beta_=0;
  c_r_Ea_=0;
  constante_taux_reaction_=1e30;    // 1/s en gaz, m/s en grains (et suivant nombre de produits d'activite !)
  /*  c0_D_G_ =0.;                      // Pa.m2/s
      c1_D_G_ =0.;                      // Pa.m2/s/K
      c2_D_G_ =0.;                      // Pa.m2/s/K2
      d_D_G_=1e30;                      // Pa.m2/s/K^(3/2)
      c_Re05Sc033_=0.3;                 // constante de la loi de type Ranz-Marschall pour D_G (e.g. 0.3)
  */
  Sc_t_=0.8;                        // Nombre de Schmidt turbulent dans le calcul de D_G
  /*  d_min_=1e-10;
      d_max_=1.; */
  proportion_max_admissible_=1e30;
  proportion_max_sur_delta_t_=1;    // mol/kg
  activite_="0";
  nb_sous_pas_de_temps_reaction_ =1;
  contre_reaction_=-1;
  //  nb_iter_impl_contre_reaction_=1;      // pas d'iteration
  //sous_relax__impl_contre_reaction_=1.; // pas de sous-relaxation
  Param param(que_suis_je());
  param.ajouter( "reactifs",&reactifs_,Param::REQUIRED);
  param.ajouter( "produits",&produits_,Param::REQUIRED);
  param.ajouter( "constante_taux_reaction",&constante_taux_reaction_);
  param.ajouter( "enthalpie_reaction",&enthalpie_reaction_,Param::REQUIRED);
  param.ajouter( "energie_activation",&Ea_);


  param.ajouter( "exposant_beta",&beta_);
  param.ajouter_non_std("coefficients_activites",(this));


  param.ajouter( "contre_reaction",&contre_reaction_);
  param.ajouter( "contre_energie_activation",&c_r_Ea_);
  /* param.ajouter( "c0_D_G", &c0_D_G_);
     param.ajouter( "c1_D_G", &c1_D_G_);
     param.ajouter( "c2_D_G", &c2_D_G_);
     param.ajouter( "d_D_G",&d_D_G_);
     param.ajouter( "c_Re05Sc033 ",&c_Re05Sc033_);

     param.ajouter( "d_min",&d_min_);
     param.ajouter( "d_max",&d_max_);


     param.ajouter( "proportion_max_admissible",&proportion_max_admissible_);

     param.ajouter( "nb_sous_pas_de_temps_reaction",&nb_sous_pas_de_temps_reaction_);

     param.ajouter( "nb_iter_impl_contre_reaction",&nb_iter_impl_contre_reaction_);
     param.ajouter( "sous_relax__impl_contre_reaction",&sous_relax__impl_contre_reaction_);
  */
  param.ajouter( "Sc_t",&Sc_t_);
  param.lire_avec_accolades_depuis(is);
  /*
    if (((c0_D_G_+c1_D_G_*1.E3+c2_D_G_*1.E6+d_D_G_*3.E4)>1.e29)&&(constante_taux_reaction_>1.e29))
    { // NB : D_G=1/p*(c0+c1*T+c2*T^2+c32*T^(3/2))
    Cerr<<" L'une des deux valeurs D_G(T) ou constante_taux_reaction doit etre << 1E30 !"<<finl;
    Cerr<<"   c0_D_G & d_D_G : "<<c0_D_G_<<" & "<<d_D_G_ <<finl;
    Cerr<<"   constante_taux_reaction : "<< constante_taux_reaction_ <<finl;
    exit();
    }
  */
  if (Sc_t_==0.)
    {
      Cerr<<" Une valeur nulle du Schmidt turbulent est impossible ! Essayer plutot 1E30 !"<<finl;
      exit();
    }

  return is;
}

int Reaction::lire_motcle_non_standard(const Motcle& motlu, Entree& is)
{
  if (motlu=="coefficients_activites")
    {
      Nom motlubis;
      is >> motlubis;
      if (motlubis!="{")
        {
          Cerr<<"On attendait { et non " <<motlubis<<" dans lire_motcle_non_standard de "<<que_suis_je()<<finl;
          exit();
        }
      is>>motlubis;
      while (motlubis!="}")
        {
          // on lit deux mots et on fait +mot1*mot2
          activite_+="+";
          activite_+=motlubis;
          activite_+="*";
          is>>motlubis;
          activite_+=motlubis;
          is>>motlubis;
        }
    }
  else
    {
      Cerr<<"On attendait coefficients_activites et non " <<motlu<<" dans lire_motcle_non_standard de "<<que_suis_je()<<finl;
      exit();
    }
  return 1;
}
double Reaction::calculer_pas_de_temps() const
{
  return proportion_max_admissible_/(proportion_max_sur_delta_t_+DMINFLOAT);
}

void Reaction::discretiser_omega(const Probleme_base& pb,const Nom& nom)
{
  const Equation_base& eqn=pb.equation(0);
  eqn.discretisation().discretiser_champ("temperature",eqn.zone_dis().valeur(),nom,"unit", -1,0.,omega_);
}
void Reaction::reagir(VECT(REF(Champ_Inc_base))& liste_C,double deltat) const
{
  // il faut savoir e que l'on veut faire ....

  //  Cerr<<__FILE__<<" "<<(int)__LINE__<<" non  code "<<finl;  exit();
  int size=liste_C.size();

  ArrOfDouble C(size),C0(size);

  int nb_case= liste_C[0].valeur().valeurs().size();
  if ((beta_!=0)||(Ea_!=0)||((c_r_Ea_!=0)&&(contre_reaction_>0)))
    {
      Cerr<<"Reaction :  Donnees incompatibles avec le fait que l on n a pas de temperature"<<finl;
      Cerr<<*this<<finl;
      exit();
    }

  for (int elem=0; elem<nb_case; elem++)
    {


      // initialisation et rabotage des C
      for (int i=0; i<size; i++)
        {
          if (coeff_Y_[i]!=0.) // c'est un reactif ou un produit
            {
              C(i)=liste_C[i].valeur().valeurs()(elem);
              if (C(i)<0)
                {

                  if (C(i)<-1e-5)
                    {
                      Cerr<<" on rabote C_"<<i<<" dans la maille "<<elem<<" dans la chimie !!!!!! "<<C(i)<<finl;

                      exit();
                    }
                  C(i)=0;
                }
            }
          else
            C(i)=0;
        }

      // calcul du taux de reaction implicite
      C0=C;
      double proportion_directe;
      double proportion=calcul_proportion_implicite(C,C0,deltat,1e-7,proportion_directe);
      for (int i=0; i<size; i++)
        if (coeff_Y_[i]!=0.) // c'est un reactif ou un produit
          {
            DoubleTab& C_i = liste_C[i].valeur().valeurs(); // kg/kg
            //    C_i(elem)-=proportion*coeff_stoechio_[i];
            C_i(elem)=C0(i)-proportion*coeff_stoechio_[i];
          }
    }

}

double Reaction::calcul_proportion_implicite(ArrOfDouble& C_temp,const ArrOfDouble& C0, double deltat, double seuil, double& proportion_directe) const
{

  int nbc=C0.size_array();
  // double proportion_directe_es;
  double proportion=-1, omegapoint, dy_omega, proportion_inverse=-1;

  double rho_mel=1 ; //tab_rho(elem);                // kg/m3
  double T_elem=1 ; //temperature(elem);             // K
  omegapoint=constante_taux_reaction_;  // 1/s     // prefacteur du taux de reaction volumique local
  //omegapoint*=pow(T_elem,beta_)*exp(-Ea_/(R_gaz_parfait*T_elem)); // Arrhenius et facteur temperature
  dy_omega=omegapoint/rho_mel;          // m3/kg/s // prefacteur du taux de reaction volumique local
  //proportion=dy_omega*deltat;           // m3/kg a ce stade si une seule activite exposant 1
  double produit_activite=1;
  double produit_contre=1;
  double R_gaz_parfait=8.3143;

  double securite=1-1e-6;

  if (contre_reaction_>0)
    securite=0.5;
  ArrOfDouble& C=C_temp;
  C=C0;

  int ite=0;
  double dmax=2*seuil;

  while((dmax>seuil)&&(ite<1000))
    // while(ite<5)
    {

      dmax=0;
      produit_activite=1;
      produit_contre=1;
      proportion=dy_omega*deltat;
      // calcul du taux de reaction brut
      for (int i=0; i<nbc; i++)
        {
          if (coeff_Y_[i]>0.) // c'est un reactif gazeux => produit par son activite
            {
              produit_activite*=pow(C(i),coeff_activite_[i]);
            }
          else if (contre_reaction_>0)
            if (coeff_Y_[i]<0.)
              {
                produit_contre*=pow(C(i),coeff_activite_[i]);
              }
        } // mol/kg a ce stade

      assert(produit_contre>=0);
      proportion_directe=produit_activite*proportion;
      if (contre_reaction_>0)
        {
          proportion_inverse=produit_contre/(contre_reaction_*exp(-c_r_Ea_/(R_gaz_parfait*T_elem)))*proportion;  // contre-Arrhenius
          proportion=proportion_directe-proportion_inverse;
        }
      else
        proportion=proportion_directe;

      //      proportion*=produit_activite;
      if (1)
        {
          // conditionnement en temps pour ne pas consommer plus que ce qui est present
          if (proportion>0)
            for (int i=0; i<nbc; i++)
              {
                if (coeff_Y_[i]>0.) // c'est un reactif
                  {
                    if (proportion > C(i)/coeff_stoechio_[i]*securite) Cerr<<" on limite" <<finl;
                    proportion=min(proportion,C(i)/coeff_stoechio_[i]*securite);
                  }
              }
          else
            for (int i=0; i<nbc; i++)
              {
                if (coeff_Y_[i]<0.) // c'est un reactif car on est dans le cas contre treaction
                  {
                    // on prend le max car proportion et Y_i(elem)/coeff_Y_[i] sont negatifs
                    proportion=max(proportion,C(i)/coeff_stoechio_[i]*securite);
                  }
              }
        }
      proportion_max_sur_delta_t_=max(proportion_max_sur_delta_t_,dabs(proportion));
      //if (ite==0)
      //  proportion_directe_es=proportion_directe;
      for (int i=0; i<nbc; i++)

        if (1)
          {
            if (coeff_Y_[i]!=0.) // c'est un reactif car on est dans le cas contre treaction
              {
                double pond=-1;
                if (coeff_Y_[i]>0)
                  {
                    if (C(i)>0)
                      pond=proportion_directe*coeff_stoechio_[i]/C(i);
                    else
                      pond=0;
                  }
                else
                  {
                    if ((contre_reaction_>0)&&(C(i)>0))
                      pond=-proportion_inverse*coeff_stoechio_[i]/C(i);
                    else
                      pond=0;
                  }
                if (pond<0)
                  exit();
                //pond=0;
                double nc=((C0(i)-proportion*coeff_stoechio_[i]/2.+pond/2.*C(i))/(1.+pond/2.)-C(i));
                double dc=dabs(nc);
                C(i)+=nc;
                if (dc>dmax)
                  dmax=dc;
                //Cerr<<ite<<" i "<< i<< " "<<C(i)-C0(i)<<" dc"<< dmax <<finl;
              }
          }
        else
          {
            double nc=C0(i)-proportion*coeff_stoechio_[i]/2.-C(i);
            C(i)+=nc;
            double dc=dabs(nc);
            if (dc>dmax)
              dmax=dc;
          }


      ite++;
      //    Cerr<<ite<< " ici " <<proportion <<" "<<dmax<<" direct " <<proportion_directe<<finl;
      //dmax=0;

    }
  //  Cerr<<"convergence en  "<<ite <<" "<<dmax<<" "<<proportion_directe<<finl;

  return proportion;
}
