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
// File:        Chimie.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Chimie
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////


#include <Chimie.h>
#include <Probleme_base.h>
#include <Motcle.h>
#include <Param.h>
#include <Parser_U.h>
#include <Zone_VF.h>
#include <Discretisation_base.h>
#include <Debog.h>
#include <Convection_Diffusion_Concentration.h>
#include <dlsinterf.h>
#include <Constituant.h>


Implemente_instanciable(Chimie,"Chimie",Objet_U);
Implemente_vect(REF(Champ_Inc_base));







Sortie& Chimie::printOn(Sortie& os) const
{
  os<<(*this).le_nom()<<finl;
  os<<reactions_<<finl;
  return os;
}

Entree& Chimie::readOn(Entree& is)
{
  modele_micro_melange_=0;
  constante_modele_micro_melange_=1.;
  Param param(que_suis_je());
  param.ajouter("reactions",&reactions_,Param::REQUIRED);
  param.ajouter("modele_micro_melange",&modele_micro_melange_);
  param.ajouter("constante_modele_micro_melange",&constante_modele_micro_melange_);
  param.ajouter("espece_en_competition_micro_melange",&espece_en_competition_micro_melange_);
  param.lire_avec_accolades_depuis(is);
  return is;

}

double Chimie::calculer_pas_de_temps() const
{
  double dt=1e30;
  double dt_reaction;
  for (int i=0; i<reactions_.size(); i++) // boucle sur l'ensemble des reactions
    {
      dt_reaction=reactions_[i].calculer_pas_de_temps();
      if(i==0) Cout<<"Pour le(s) reaction(s), dt_max = ";
      Cout<<dt_reaction<<" ";
      if (i==(reactions_.size()-1)) Cout<<finl;
      dt=std::min(dt,dt_reaction);
    }
  // double dt_n=pb_->schema_temps().pas_de_temps();
  // filtre Butterworth :
  //        alpha=1    -> pas de filtre
  //        alpha->0   -> tres filtre
  // double alpha_Butterworth=1.;
  // return (dt_n*(1.-alpha_Butterworth)+alpha_Butterworth*dt);
  return dt;
}



void  Chimie::completer(const Probleme_base& pb)
{
  // si pas de reaction on ne fait rien
  if (reactions_.size()==0)
    return;
  pb_=pb;
  // 1) determination du nombre de FM
  int nb_fm=0;
  int nb_grains=0;
  int nb_concentration=0;
  //  Motcles alias;
  ArrOfDouble masses_molaires;
  for (int n=0; n<pb.nombre_d_equations(); n ++)
    {

      if (sub_type(Convection_Diffusion_Concentration,pb.equation(n)))
        nb_concentration++;

    }

  // 2 stockage des alias et des masses molaires
  masses_molaires.resize_array(nb_grains+nb_fm+nb_concentration);
  alias.dimensionner(nb_grains+nb_fm+nb_concentration);
  nb_grains_=nb_grains;
  int nb=0;
  for (int n=0; n<pb.nombre_d_equations(); n ++)
    {
      if (sub_type(Convection_Diffusion_Concentration,pb.equation(n)))
        {
          const Convection_Diffusion_Concentration& eq= ref_cast(Convection_Diffusion_Concentration,pb.equation(n));
          masses_molaires[nb]=eq.masse_molaire();
          alias[nb]=eq.inconnue().valeur().le_nom();
          REF(Champ_Inc_base) inco;
          inco = eq.inconnue().valeur();
          liste_C_.add(inco);
          nb++;
        }
    }
  assert(liste_ai_.size()==nb_grains_);
  // verification de l'unicite des  alias
  marqueur_espece_en_competition_micro_melange_=-1;
  {
    LIST(Nom) test;
    for (int n=0; n<alias.size(); n++)
      {
        int deja=test.contient(alias[n]);
        if (!deja)
          test.add(alias[n]);
        else
          {
            Cerr<<alias[n] <<" defini plusieurs fois !!!!!"<<finl;
            //Cerr<<test<< " "<<alias<<finl;
            exit();
          }
        if (espece_en_competition_micro_melange_!=Nom())
          {
            if (espece_en_competition_micro_melange_==alias[n])
              marqueur_espece_en_competition_micro_melange_=n;
          }
      }
    assert(test.size()==nb_fm+nb_grains_+nb_concentration);
  }
  if (espece_en_competition_micro_melange_!=Nom())
    {
      if (marqueur_espece_en_competition_micro_melange_==-1)
        {
          Cerr<<" espece_en_competition_micro_melange non trouve: "<< espece_en_competition_micro_melange_<<finl;
          exit();
        }
      else
        Cerr<<" marqueur_espece_en_competition_micro_melange "<<marqueur_espece_en_competition_micro_melange_<<finl;
    }
  if(min_array(masses_molaires)<0)
    {
      Cerr<< "pb avec les masses_molaires "<<masses_molaires<<finl;
      exit();
    }
  if (liste_Y_.size()>0)
    {
      Puissance_volumique_=(liste_Y_[0].valeur().valeurs()); // dimensionnement du tableau...
      Puissance_volumique_=0.;
    }
  else
    {
      assert(liste_C_.size()!=0);
    }
  //alias_=alias;
  for (int i=0; i<reactions_.size(); i++)
    {
      reactions_[i].completer(alias,masses_molaires);
    }
}

void Chimie::discretiser(const Probleme_base& pb)
{
  for (int i=0; i<reactions_.size(); i++)
    {
      Nom test("omega_");
      test+=Nom(i);
      reactions_[i].discretiser_omega(pb,test);
    }
}

int Chimie::preparer_calcul()
{
  return 1;
}



void  Chimie::mettre_a_jour(double temps)
{
  if (reactions_.size()==0) // pas de reaction
    return;

  double dt=pb_->schema_temps().pas_de_temps();

  if (liste_C_.size())
    {
      // On suppose qu'il n'y a pas de temperature pour l'instant...
      int old=0;
      int nbr=reactions_.size();
      if (old)
        {
          for (int i=0; i<nbr; i++)
            {
              Reaction& reaction=reactions_[i];
              int nb_sous_pas_de_temps_reaction=reaction.nb_sous_pas_de_temps_reaction();

              const double dt_chimie=dt/nb_sous_pas_de_temps_reaction;
              for (int n=0; n<nb_sous_pas_de_temps_reaction; n++)
                reaction.reagir(liste_C_,dt_chimie);
            }

          for (int i=0; i<liste_C_.size(); i++)
            liste_C_[i].valeur().valeurs().echange_espace_virtuel();
          return;
        }
      int vef=0;
      Zone_VF& zvf = ref_cast(Zone_VF,liste_C_[0].valeur().equation().zone_dis().valeur());
      //if (sub_type(Zone_VEF, zvf))
      if (zvf.que_suis_je().debute_par("Zone_VEF"))
        vef=1;

      const IntTab& face_voisins = zvf.face_voisins();
      const ArrOfDouble& volume=zvf.volumes();
      int newd=1;
      if (newd)
        {
          int nbrtot=0;
          ArrOfInt marq_contre(2*nbr);
          for (int i=0; i<nbr; i++)
            {
              Reaction& reaction=reactions_[i];
              marq_contre(nbrtot++)=i+1;
              if (reaction.contre_reaction_>0)
                marq_contre(nbrtot++)=-(i+1);

              if ((reaction.beta_!=0)||(reaction.Ea_!=0)||((reaction.c_r_Ea_!=0)&&(reaction.contre_reaction_>0)))
                {
                  Cerr<<"Reaction :  Donnees incompatibles avec le fait que l on n a pas de temperature"<<finl;
                  Cerr<<reaction<<finl;
                  exit();
                }
            }
          marq_contre.resize_array(nbrtot);

          int nbc=liste_C_.size();

          F77NAME(INITTAILLECOMMON)(&nbrtot,&nbc);
          ArrOfDouble pstochio(nbc);
          ArrOfInt pactivite(nbc);
          F77NAME(SETMARQUEUR)(&marqueur_espece_en_competition_micro_melange_);
          for (int i=0; i<nbrtot; i++)
            {
              int ir=marq_contre(i);
              int ir2=ir;
              if (ir<0) ir2=-ir2;
              ir2-=1;
              Reaction& reaction=reactions_[ir2];
              pstochio=reaction.coeff_stoechio_;
              pactivite=0;

              double cw=reaction.constante_taux_reaction_;
              if (ir<0)
                {
                  pstochio*=-1;
                  assert(reaction.contre_reaction_>0);
                  cw/=reaction.contre_reaction_;
                }
              for (int ic=0; ic<nbc; ic++)
                {
                  if (pstochio[ic]>0)
                    pactivite(ic)=(int)reaction.coeff_activite_[ic];
                }
              int ii=i+1;
              int avec_contre_reaction=(reaction.contre_reaction_>0);
              F77NAME(INITREACTIONCOMMON)(&(ii), pstochio.addr(), pactivite.addr(),&avec_contre_reaction) ;
              F77NAME(SETCWREACTION)(&(ii), &cw);

            }
          // F77NAME(PRINT_COMMON)();
          // abort();
          int mf=21;
          int lrw;
          int liw;
          if (mf==21)
            {
              liw=20+nbc;
              lrw=22+9*nbc+nbc*nbc;
            }
          else
            throw;
          assert(mf==21);


          ArrOfDouble rwork(lrw);
          ArrOfInt iwork(liw);


          DoubleTab tau_mel;
          if (modele_micro_melange_>0)
            {

              const DoubleTab& visc_turb=liste_C_[0].valeur().equation().probleme().get_champ("viscosite_turbulente").valeurs();
              tau_mel=visc_turb;
              const DoubleTab& D_moleculaire = ref_cast(Convection_Diffusion_Concentration,liste_C_[0].valeur().equation()).constituant().diffusivite_constituant().valeurs();
              double D_mol=0.;
              if (D_moleculaire.dimension(0) == 1)
                D_mol = D_moleculaire(0, 0);
              else
                {
                  Cerr << "D_mol pas code pour champ non uniforme" << finl;
                  exit();
                }
              double invsct=1./0.9;
              for (int element=0; element<visc_turb.dimension(0); element++)
                {
                  double delta=exp(log(volume[element])/double(dimension));
                  //  Cerr<<" iii "<<delta<<" "<<D_mol<<finl;
                  tau_mel(element)=constante_modele_micro_melange_*delta*delta/(D_mol+visc_turb(element)*invsct);
                }
              tau_mel.echange_espace_virtuel();
            }
          int nb_elem=liste_C_[0].valeur().valeurs().size();
          ArrOfDouble C(nbc);

          for (int elem=0; elem<nb_elem; elem++)
            {

              double tau_melange=-1;
              if (modele_micro_melange_>0)
                {
                  if (vef==0)
                    tau_melange=tau_mel(elem);
                  else
                    {
                      if (face_voisins(elem,1)!=-1)
                        tau_melange = 0.5*(tau_mel(face_voisins(elem,0))+tau_mel(face_voisins(elem,1)));
                      else
                        tau_melange = (tau_mel(face_voisins(elem,0)));
                    }
                }
              // tau_melange <0 pas pris en compte
              // recuperation des valeurs initiales
              for (int i=0; i<nbc; i++)
                {
                  C(i)=liste_C_[i].valeur().valeurs()(elem);

                }
              for (int i=0; i<nbc; i++)
                {
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
              int init=1;
              if (init)
                {

                }
              double t=0,tout=dt;
              int itol=1;
              double rtol=0;
              double atol=1e-10;
              F77NAME(DLSODECHIMIES)(&nbc, C.addr(),&t, &tout,&tau_melange, &itol, &rtol, &atol, rwork.addr(), &lrw, iwork.addr(), &liw);
              // mise a jour des inconnues
              for (int i=0; i<liste_C_.size(); i++)
                liste_C_[i].valeur().valeurs()(elem)=C(i);
            }
          for (int i=0; i<liste_C_.size(); i++)
            liste_C_[i].valeur().valeurs().echange_espace_virtuel();
          return;
        }
      // on calcule le nb_sous_pas_temps_max
      int nb_sous_pas_de_temps_reaction_max=1;
      int nbr_directe=0;

      ArrOfInt marq_directe(nbr);
      for (int i=0; i<nbr; i++)
        {
          Reaction& reaction=reactions_[i];
          int nb_sous_pas_de_temps_reaction=reaction.nb_sous_pas_de_temps_reaction();
          if (nb_sous_pas_de_temps_reaction>nb_sous_pas_de_temps_reaction_max)
            nb_sous_pas_de_temps_reaction_max=nb_sous_pas_de_temps_reaction;
          if (reaction.contre_reaction_<=0)
            marq_directe(nbr_directe++)=i;

          if ((reaction.beta_!=0)||(reaction.Ea_!=0)||((reaction.c_r_Ea_!=0)&&(reaction.contre_reaction_>0)))
            {
              Cerr<<"Reaction :  Donnees incompatibles avec le fait que l on n a pas de temperature"<<finl;
              Cerr<<reaction<<finl;
              exit();
            }
        }
      const double dt_chimie=dt/nb_sous_pas_de_temps_reaction_max;

      int nbc=liste_C_.size();


      //      double R_gaz_parfait=8.3143;

      ArrOfDouble C(nbc),C_tmp(nbc), proportion_eq(nbr_directe);

      int nb_elem=liste_C_[0].valeur().valeurs().size();
      for (int elem=0; elem<nb_elem; elem++)
        {


          // recuperation des valeurs initiales
          for (int i=0; i<nbc; i++)
            {
              C(i)=liste_C_[i].valeur().valeurs()(elem);
            }
          for (int n=0; n<nb_sous_pas_de_temps_reaction_max; n++)
            {
              // rabotage eventuel
              for (int i=0; i<nbc; i++)
                if (C(i)<0)
                  {
                    if (C(i)<-1e-5)
                      {
                        Cerr<<" on rabote C_"<<i<<" dans la maille "<<elem<<" dans la chimie !!!!!! "<<C(i)<<finl;
                        exit();
                      }
                    C(i)=0;
                  }
              // calcul des proportions initiales des reactions non equilibre

              for (int i=0; i<nbr_directe; i++)
                {
                  Reaction& reaction=reactions_[marq_directe[i]];
                  assert(reaction.contre_reaction_<=0);

                  double proportion_directe;
                  reaction.calcul_proportion_implicite(C_tmp,C,dt_chimie,1e-7,proportion_directe);
                  if (0)
                    {
                      double proportion_directe2;
                      reaction.calcul_proportion_implicite(C_tmp,C,dt_chimie,1e9,proportion_directe2);
                      if (proportion_directe==proportion_directe2)

                        {
                          exit();
                        }
                    }
                  // on utilise proportion_directe pour ne pas avoir la limitation par la securite
                  proportion_eq(i)=proportion_directe;


                }
              // pour chaque consitutant on regarde si il faut limiter ....
              double securite=1-1e-6;
              for (int c=0; c<nbc; c++)
                {
                  double St=0;
                  for (int i=0; i<nbr_directe; i++)
                    {
                      Reaction& reaction=reactions_[marq_directe[i]];
                      assert(reaction.contre_reaction_<=0);
                      if (reaction.coeff_Y_[c]>0.)
                        St+=proportion_eq(i)*reaction.coeff_stoechio_[c];
                    }

                  double rapport=St/(securite);
                  //  Cerr<< c<<" ici0b "<< rapport <<" st "<<St<<" "<<C(c)<<finl;

                  if (rapport>C(c))
                    {
                      // on risque de trop consommer .... on limite.
                      Cerr<< c<<" limite ici0 "<< rapport <<" st "<<St<<" "<<C(c)<<" "<<C(c)/rapport<<finl;
                      // exit();
                      for (int i=0; i<nbr_directe; i++)
                        {
                          Reaction& reaction=reactions_[marq_directe[i]];
                          assert(reaction.contre_reaction_<=0);

                          if (reaction.coeff_Y_[c]>0.)
                            proportion_eq(i)*=C(c)/rapport;

                        }
                    }
                }
              // on fait reagir les reactions directes C(c)-=St;
              for (int c=0; c<nbc; c++)
                {
                  double St=0;
                  for (int i=0; i<nbr_directe; i++)
                    {
                      Reaction& reaction=reactions_[marq_directe[i]];
                      assert(reaction.contre_reaction_<=0);
                      if (reaction.coeff_Y_[c]!=0.)
                        St+=proportion_eq(i)*reaction.coeff_stoechio_[c];
                    }

                  // Cerr<<C(c)<<" "<< c<< " la  st "<<St<<" ";
                  //assert(St<=(C(c)*securite));
                  C(c)-=St;
                  assert(C(c)>=0);
                  // Cerr<< C(c)<<finl;

                }
              // on taite maintenant les reactions equilibrees les unes apres les autres

              securite=0.5;
              for (int reac=0; reac<nbr; reac++)
                {
                  Reaction& reaction=reactions_[reac];

                  if (reaction.contre_reaction_>0)
                    {
                      double proportion_directe;
                      double proportion=reaction.calcul_proportion_implicite(C_tmp, C,dt_chimie,1e-7,proportion_directe);
                      for (int i=0; i<nbc; i++)
                        if (reaction.coeff_Y_[i]!=0.) // c'est un reactif ou un produit
                          {
                            C(i)=C(i)-proportion*reaction.coeff_stoechio_[i];
                          }
                    }
                }
            }
          // reaction.reagir(liste_C_,dt_chimie);
          for (int i=0; i<liste_C_.size(); i++)
            liste_C_[i].valeur().valeurs()(elem)=C(i);

        }

      for (int i=0; i<liste_C_.size(); i++)
        liste_C_[i].valeur().valeurs().echange_espace_virtuel();

      return;
    }
  Cerr<<"Chimie dispo qu'avec des concentrations"<<finl;
  exit();

}

const Champ_base& Chimie::get_champ(const Motcle& nom) const
{
  int nb_reac=reactions_.size();
  for (int i=0; i<nb_reac; i++)
    {
      Nom test("omega_");
      test+=Nom(i);
      if (nom==test)
        return reactions_[i].get_omega();
    }

  throw Champs_compris_erreur();
  return get_champ(nom);
}

void Chimie::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<" Chimie :";
  int nb_reac=reactions_.size();
  for (int i=0; i<nb_reac; i++)
    {
      Nom test("omega_");
      test+=Nom(i);
      if (opt==DESCRIPTION)
        Cerr<< test;
      else
        nom.add(test);
    }
  if (opt==DESCRIPTION)
    Cerr<<finl;
}

// Description : sauvegarde du terme source de temperature
int Chimie::sauvegarder(Sortie& fich) const
{
  return 1;
}

// Description : reprise du terme source de temperature
// on est deja bien positionne ????
int Chimie::reprendre(Entree& is)
{
  return 1;

}

