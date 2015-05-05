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
// File:        Modele_turbulence_hyd_K_Eps_V2_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/31
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_turbulence_hyd_K_Eps_V2_VDF.h>
#include <Champ_Uniforme.h>
#include <Zone_Cl_VDF.h>
#include <Zone_VDF.h>
#include <Fluide_Incompressible.h>
#include <Schema_Temps_base.h>
#include <stat_counters.h>
Implemente_instanciable(Modele_turbulence_hyd_K_Eps_V2_VDF,"Modele_turbulence_hyd_K_Epsilon_V2_VDF",Modele_turbulence_hyd_K_Eps_V2);


// Description:
//    Ecrit le type de l'objet sur un flot de sortie.
// Precondition:
// Parametre: Sortie& s
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
Sortie& Modele_turbulence_hyd_K_Eps_V2_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


// Description:
//    Simple appel a Mod_turb_hyd_base::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Modele_turbulence_hyd_K_Eps_V2_VDF::readOn(Entree& is )
{
  return Modele_turbulence_hyd_K_Eps_V2::readOn(is);
}


void Modele_turbulence_hyd_K_Eps_V2_VDF::associer(const Zone_dis& zone_dis,
                                                  const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF,zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());
}

void Modele_turbulence_hyd_K_Eps_V2_VDF::mettre_a_jour(double temps)
{
  Champ_Inc& ch_K_Eps = K_Eps();
  Schema_Temps_base& sch = eqn_transp_K_Eps().schema_temps();

  eqn_f22.valeur().zone_Cl_dis().mettre_a_jour(temps);
  eqn_f22.resoudre();

  eqn_transp_K_Eps().zone_Cl_dis().mettre_a_jour(temps);
  sch.faire_un_pas_de_temps_eqn_base(eqn_transp_K_Eps());

  eqn_transport_V2.zone_Cl_dis().mettre_a_jour(temps);
  sch.faire_un_pas_de_temps_eqn_base(eqn_transport_V2);

  eqn_transp_K_Eps().mettre_a_jour(temps);
  eqn_transport_V2.mettre_a_jour(temps);
  eqn_f22.valeur().mettre_a_jour(temps);

  statistiques().begin_count(nut_counter_);
  eqn_transport_K_Eps_V2.controler_K_Eps();
  eqn_transport_V2.controler_V2();

  calculer_viscosite_turbulente(ch_K_Eps.temps());
  limiter_viscosite_turbulente();
  la_viscosite_turbulente.valeurs().echange_espace_virtuel();
  statistiques().end_count(nut_counter_);
}

// Description:
//    Calcule la viscosite turbulente au temps demande.
// Precondition:
// Parametre: double temps
//    Signification: le temps auquel il faut calculer la viscosite
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Fonc&
//    Signification: la viscosite turbulente au temps demande
//    Contraintes:
// Exception: erreur de taille de visco_turb_K_eps
// Effets de bord:
// Postcondition:
Champ_Fonc& Modele_turbulence_hyd_K_Eps_V2_VDF::calculer_viscosite_turbulente(double temps)
{
  const Champ_base& chK_Eps=eqn_transp_K_Eps().inconnue().valeur();
  const Champ_base& chV2=eqn_transport_V2.inconnue().valeur();
  Nom type=chK_Eps.que_suis_je();
  const DoubleTab& tab_K_Eps = chK_Eps.valeurs();
  const DoubleTab& tav_V2 = chV2.valeurs();
  DoubleTab& visco_turb =  la_viscosite_turbulente.valeurs();
  int visco_unif;
  int n = tab_K_Eps.dimension(0);
  double Cmu = 0.19;
  double visco=-1;
  double leEPS_MIN = get_LeEPS_MIN();

  DoubleTab S(n);
  DoubleTab T(n);
  T = 1.;

  Equation_base&  eq_hydraulique = mon_equation.valeur();
  Champ_Face& vit_bord = ref_cast(Champ_Face,eq_hydraulique.inconnue().valeur());

  const DoubleTab& vit = eq_hydraulique.inconnue().valeurs();
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zone_Cl_VDF = la_zone_Cl_VDF.valeur();

  calculer_Sij(zone_VDF,zone_Cl_VDF,S,vit,vit_bord);

  if ( sub_type(Fluide_Incompressible,eqn_transport_V2.milieu()))
    {
      Fluide_Incompressible& fluide_inc = ref_cast(Fluide_Incompressible,eqn_transport_V2.milieu());
      const Champ_Don& ch_visco_cin = fluide_inc.viscosite_cinematique();
      DoubleTab& tab_visco = ref_cast_non_const(DoubleTab,ch_visco_cin->valeurs());


      if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
        {
          visco = max(tab_visco(0,0),DMINFLOAT);
          visco_unif =1;
        }
      else
        {
          visco_unif=0;
        }

      Calculer_Echelle_T(tab_K_Eps,tav_V2,S,tab_visco,visco,visco_unif,T);
      //Cerr << " Dans le modele T= " << T << finl;
      // K_Eps(i,0) = K au noeud i
      // K_Eps(i,1) = Epsilon au noeud i

      if (visco_turb.size() != n)
        {
          Champ_Inc visco_turb_au_format_K_eps;

          visco_turb_au_format_K_eps.typer(type);
          Champ_Inc_base& ch_visco_turb_K_eps=visco_turb_au_format_K_eps.valeur();
          ch_visco_turb_K_eps.associer_zone_dis_base(eqn_transp_K_Eps().zone_dis().valeur());
          ch_visco_turb_K_eps.nommer("diffusivite_turbulente");
          ch_visco_turb_K_eps.fixer_nb_comp(1);
          ch_visco_turb_K_eps.fixer_nb_valeurs_nodales(n);
          ch_visco_turb_K_eps.fixer_unite("inconnue");
          ch_visco_turb_K_eps.changer_temps(0.);

          DoubleTab& visco_turb_K_eps =  ch_visco_turb_K_eps.valeurs();

          if(visco_turb_K_eps.size() != n)
            {
              Cerr << "visco_turb_K_eps est de taille " << visco_turb_K_eps.size()
                   << " au lieu de " << n << finl;
              exit();
            }
          for (int i=0; i<n; i++)
            {
              if (tab_K_Eps(i,1) <= leEPS_MIN)
                visco_turb_K_eps[i] = 0;
              else
                {
                  visco_turb_K_eps[i] = Cmu*tav_V2(i)*T(i);
                }
            }
          la_viscosite_turbulente->affecter(visco_turb_au_format_K_eps.valeur());
        }
      else
        {
          for (int i=0; i<n; i++)
            {
              if (tab_K_Eps(i,1) <= leEPS_MIN)
                visco_turb[i] = 0;
              else
                visco_turb[i] = Cmu*tav_V2(i)*T(i);
            }
        }

      la_viscosite_turbulente.changer_temps(temps);
      return la_viscosite_turbulente;

    }
  else
    {
      Cerr << " Erreur dans Modele_turbulence_hyd_K_Eps_V2::calculer_viscosite_turbulente(double temps) : on ne sait traiter que les fluides incompressibles" << finl;
      exit();
    }
  return la_viscosite_turbulente;
}

DoubleTab&  Modele_turbulence_hyd_K_Eps_V2_VDF::Calculer_Echelle_T(const DoubleTab& tab_K_Eps,const DoubleTab& tab_V2,DoubleTab& S,DoubleTab&  tab_visco,double visco,double visco_unif, DoubleTab& T)
{
  double TT,temp4,T1,T2,T3;
  double Cmu = 0.19;
  const double six = 6.;
  const double zerosix = 0.6 ;

  double compteur_keps ,compteur_kolmo , compteur_cmu_v2, compteur_eps_presque_nul;
  compteur_keps = 0.;
  compteur_kolmo = 0.;
  compteur_cmu_v2 = 0.;
  compteur_eps_presque_nul  = 0.;
  int n = tab_K_Eps.dimension(0);
  for (int i=0; i<n; i++)
    {
      if (visco_unif == 0)
        visco = tab_visco[i];

      T1 = 0.;
      T2 = 0.;
      T3 = 0.;
      if (tab_K_Eps(i,1) > 1.e-10)
        {
          double k_sur_eps = tab_K_Eps(i,0)/tab_K_Eps(i,1);
          double kolmo = six*sqrt(visco/tab_K_Eps(i,1));
          T1 += k_sur_eps;
          T2 += kolmo;
          T(i) = max(T1,T2);
          //  T(i) = max(tab_K_Eps(i,0)/tab_K_Eps(i,1),6*sqrt(visco/tab_K_Eps(i,1)));
          if (tab_V2(i) > 1.e-10 && S(i) > 1.e-10)
            {
              temp4 = (zerosix*tab_K_Eps(i,0))/(sqrt(six)*Cmu*tab_V2(i)*sqrt(S(i)));
              T3 =+ temp4;
              TT =  min(T(i),T3);
              if (T3 < T(i))
                {
                  compteur_cmu_v2 ++;
                  T(i) = TT;
                }
              else
                {
                  if (T1>T2)
                    compteur_keps ++;
                  else
                    compteur_kolmo ++;
                }
              //  T(i) = TT;
            }
        }
      else
        {
          compteur_eps_presque_nul ++;
          T(i) = 1.;
        }
    }

  Cerr << "compteur_keps " << compteur_keps << finl;
  Cerr << "compteur_kolmo " << compteur_kolmo << finl;
  Cerr << "compteur_cmu_v2 " << compteur_cmu_v2  << finl;
  Cerr << "compteur_eps_presque_nul " << compteur_eps_presque_nul  << finl;
  return T;
}

DoubleTab&  Modele_turbulence_hyd_K_Eps_V2_VDF::Calculer_Echelle_L(const DoubleTab& tab_K_Eps,const DoubleTab& v2, DoubleTab& S,DoubleTab&  tab_visco,double visco,double visco_unif,DoubleTab& L)
{
  const double CL = 0.23;
  const double Ceta = 70.;
  const double Cmu = 0.19;
  const double six = 6.;
  double temp1,temp2,temp3;
  double k,eps;
  double T1;
  int n = tab_K_Eps.dimension(0);
  for (int j=0; j<n; j++)
    {
      if (visco_unif == 0)
        visco = tab_visco[j];
      k=tab_K_Eps(j,0);
      eps = tab_K_Eps(j,1);
      if (tab_K_Eps(j,1) > 1.e-10)
        {
          temp1 = k/eps;
          temp1 = temp1*temp1*k;  //( K^3)/Eps^2  de L donc ici au carre.
          temp3 = Ceta*Ceta*visco*sqrt(visco/eps); // pour Ceta*(nu^3/4)/eps^1/4 de L donc ici au carre.
        }
      else
        {
          temp1 = 0. ;
          temp3 = 1.e-10 ;
        }
      if ( v2(j) > 1.e-10 && S(j) > 1.e-10 )
        {
          temp2 = (k*k*k)/(six*Cmu*Cmu*v2(j)*v2(j)*S(j));
          T1 = min(temp1,temp2);
          L(j) = CL*CL*max(T1,temp3);
        }
      else
        {
          L(j) = CL*CL*max(temp3,temp1);
        }

    }
  return L;
}

DoubleTab& Modele_turbulence_hyd_K_Eps_V2_VDF::calculer_Sij(const Zone_VDF& zone_VDF, const Zone_Cl_VDF& zone_Cl_VDF , DoubleTab& S, const DoubleTab& vitesse,Champ_Face& vit)
{
  S = 0. ;
  vit.calcul_S_barre_sans_contrib_paroi(vitesse,S,zone_Cl_VDF);

  // methode reprise dans Hyd_GSE_smago ou il faut dans le modele S=sqrt(2*Sij*Sij)
  // nous il faut S=sqrt(Sij*Sij)
  S /= 2;
  return  S;
}



