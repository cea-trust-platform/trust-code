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
// File:        Modele_turbulence_Longueur_Melange_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/40
//
//////////////////////////////////////////////////////////////////////////////

#include <Modele_turbulence_Longueur_Melange_VEF.h>
#include <Champ_P1NC.h>
#include <Paroi_std_hyd_VEF.h>
#include <LecFicDiffuse.h>
#include <EcritureLectureSpecial.h>
#include <VEF_discretisation.h>
#include <Motcle.h>
#include <Debog.h>
#include <Schema_Temps_base.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Modele_turbulence_Longueur_Melange_VEF,"Modele_turbulence_hyd_Longueur_Melange_VEF",Mod_turb_hyd_RANS_0_eq);

Modele_turbulence_Longueur_Melange_VEF::Modele_turbulence_Longueur_Melange_VEF()
{
  cas=0;
  diametre_=2.;
  hauteur_=2.;
  dmax_=-1.;
}
//// printOn
//

Sortie& Modele_turbulence_Longueur_Melange_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


Entree& Modele_turbulence_Longueur_Melange_VEF::readOn(Entree& is)
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  const LIST(Nom)& params_lu = param. get_list_mots_lus();
  if (params_lu.contient(Motcle("canalx")))
    cas=1;
  else if (params_lu.contient(Motcle("tuyauz")))
    cas=2;
  else if (params_lu.contient(Motcle("tuyaux")))
    cas=21;
  else if (params_lu.contient(Motcle("tuyauy")))
    cas=22;
  else if ((params_lu.contient(Motcle("dmax"))) || (params_lu.contient(Motcle("fichier"))))
    cas=4;
  else
    {
      Cerr << " Error while reading the Longueur_Melange turbulence model. " <<finl;
      Cerr << " Case not selected, choose among : canalx tuyauz dmax fichier " << finl;
      Cerr << " Please refer to the TRUST User manual for a detailed documentation. " << finl;
      exit();
    }

  return is;
}

void Modele_turbulence_Longueur_Melange_VEF::set_param(Param& param)
{
  Mod_turb_hyd_RANS_0_eq::set_param(param);
  param.ajouter("canalx",&hauteur_);
  param.ajouter_condition("value_of_canalx_eq_2"," canalx has been coded presently only for h=2.");
  param.ajouter("tuyauz",&diametre_);
  param.ajouter("tuyaux",&diametre_);
  param.ajouter("tuyauy",&diametre_);
  param.ajouter_condition("value_of_tuyauz_eq_2"," tuyauz has been coded presently only for d=2.");
  param.ajouter_condition("value_of_tuyaux_eq_2"," tuyaux has been coded presently only for d=2.");
  param.ajouter_condition("value_of_tuyauy_eq_2"," tuyauy has been coded presently only for d=2.");
  param.ajouter("dmax",&dmax_);
  param.ajouter_non_std("verif_dparoi",(this));
  param.ajouter("fichier",&nom_fic);
}

int Modele_turbulence_Longueur_Melange_VEF::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="verif_dparoi")
    {
      Cerr << "The keyword " << mot << " is obsolete." << finl;
      Cerr << "Use keyword Distance_paroi if you wish to postprocess the wall length." << finl;
      exit();
      return 1;
    }
  else
    return Mod_turb_hyd_RANS_0_eq::lire_motcle_non_standard(mot,is);
  return 1;
}

void Modele_turbulence_Longueur_Melange_VEF::associer(
  const Zone_dis& zone_dis,
  const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VEF = ref_cast(Zone_VEF,zone_dis.valeur());
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF,zone_Cl_dis.valeur());
}

Champ_Fonc& Modele_turbulence_Longueur_Melange_VEF::calculer_viscosite_turbulente()
{
  const double Kappa = 0.415;
  double Cmu = CMU;

  double temps = mon_equation->inconnue().temps();
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  DoubleVect& k = energie_cinetique_turb_.valeurs();
  const int nb_elem = zone_VEF.nb_elem();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const DoubleTab& xp = zone_VEF.xp();

  Sij2.resize(nb_elem_tot);

  calculer_Sij2();

  if (visco_turb.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }

  Debog::verifier("Modele_turbulence_Longueur_Melange_VEF::calculer_viscosite_turbulente visco_turb 0",visco_turb);

  if (k.size() != nb_elem)
    {
      Cerr << "Size error for the array containing the values of the turbulent kinetic energy." << finl;
      exit();
    }

  //    CANAL PLAN suivant (Ox - h=2) **********************************
  if(cas==1)
    {
      for (int elem=0 ; elem<nb_elem ; elem++)
        {
          double y = xp(elem,1);
          if(y>1.) y=2.-y;

          visco_turb(elem)=Kappa*Kappa*y*y*(1.-y)*sqrt(2.*Sij2[elem]);
          k(elem)=pow(visco_turb(elem)/(Cmu*y),2);
        }
    }
  else
    //    CYLINDRE suivant (D=2) ************************************

    if((cas==2)||(cas==21)||(cas==22))
      {
        int dir1;
        int dir2;

        if(cas==2)
          {
            dir1 = 0 ;  //Tuyau suivant z
            dir2 = 1;
          }
        else if(cas==21)
          {
            dir1 = 1 ;  //Tuyau suivant x
            dir2 = 2;
          }
        else
          {
            dir1 = 0 ;  //Tuyau suivant y
            dir2 = 2;
          }

        for (int elem=0 ; elem<nb_elem ; elem++)
          {
            double x=xp(elem,dir1);
            double y=xp(elem,dir2);
            double r = sqrt(x*x+y*y);

            visco_turb(elem)=Kappa*Kappa*(1.-r)*(1.-r)*(1.-r)*sqrt(2.*Sij2[elem]);
            k(elem)=pow(visco_turb(elem)/(Cmu*(1.-r)),2);
          }
      }
  //    CAS NON TYPE ***************************************************
    else if(cas==4)
      {
        calculer_f_amortissement();

        const DoubleTab& wall_length = wall_length_.valeurs();

        for (int elem=0 ; elem<nb_elem ; elem++)
          {
            double f_vd =  f_amortissement(elem);
            double wl   =  wall_length(elem);
            double lm   = (wl<=dmax_) ? wl : dmax_*exp(-2.*(wl-dmax_)/dmax_);

            visco_turb(elem)=pow(f_vd*f_vd*Kappa*lm,2)*sqrt(2.*Sij2[elem]);
            k(elem)=pow(visco_turb(elem)/(Cmu*lm),2);
          }
      }
    else
      {
        Cerr<<cas<<" non prevu "<<que_suis_je()<<finl;
        Cerr<<que_suis_je()<<" Case "<<cas<<" not known."<<finl;
        exit();
      }

  Debog::verifier("Modele_turbulence_Longueur_Melange_VEF::calculer_viscosite_turbulente visco_turb 1",visco_turb);


  la_viscosite_turbulente.changer_temps(temps);

  wall_length_.changer_temps(temps);
  return la_viscosite_turbulente;
}

void Modele_turbulence_Longueur_Melange_VEF::calculer_energie_cinetique_turb()
{
  // PQ : 11/08/06 :    L'estimation de k repose sur les expressions :
  //                                 - nu_t = C_mu * k^2 / eps
  //                                - eps = k^(3/2) / l
  //

  /////////////////////////////////////////////////////////////////////////////////////////////////
  // pour des raisons de commodite, l'estimation de k est realisee dans calculer_viscosite_turbulente()
  /////////////////////////////////////////////////////////////////////////////////////////////////

  double temps = mon_equation->inconnue().temps();

  energie_cinetique_turb_.changer_temps(temps);
}

void Modele_turbulence_Longueur_Melange_VEF::calculer_Sij2()
{
  const DoubleTab& la_vitesse = mon_equation->inconnue().valeurs();
  const Champ_P1NC& ch= ref_cast(Champ_P1NC,mon_equation->inconnue().valeur());
  const Zone_Cl_VEF& zone_Cl_VEF = la_zone_Cl_VEF.valeur();
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_elem = zone_VEF.nb_elem_tot();

  DoubleTab duidxj(nb_elem,dimension,dimension);
  int i,j;
  double Sij;

  Sij2=0.;

  DoubleTab ubar(la_vitesse);
  //  ch.filtrer_L2(ubar);                // Patrick : on travaille sur le champ filtre.

  ch.calcul_gradient(ubar,duidxj,zone_Cl_VEF);

  for (int elem=0 ; elem<nb_elem ; elem++)
    {
      for ( i=0 ; i<dimension ; i++)
        for ( j=0 ; j<dimension ; j++)
          {
            //Deplacement du calcul de Sij
            Sij=0.5*(duidxj(elem,i,j) + duidxj(elem,j,i));
            Sij2(elem)+=Sij*Sij;
          }
    }
}

void Modele_turbulence_Longueur_Melange_VEF::lire_distance_paroi( )
{

  // PQ : 25/02/04 recuperation de la distance a la paroi dans Wall_length.xyz

  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  DoubleTab& wall_length = wall_length_.valeurs();
  wall_length=-1.;


  LecFicDiffuse fic;
  fic.set_bin(1);
  if(!fic.ouvrir(nom_fic))
    {
      Cerr << " File " <<nom_fic<< " doesn't exist. To generate it, please, refer to html.doc (Distance_paroi) " << finl;
      exit();
    }

  Noms nom_paroi;

  fic >> nom_paroi;

  if(je_suis_maitre())
    {

      Cerr << "Recall : " <<nom_fic<< " obtained from boundaries nammed : "<< finl;
      for (int b=0; b<nom_paroi.size(); b++)
        {
          Cerr << nom_paroi[b]<< finl;
          //test pour s'assurer de la coherence de Wall_length.xyz avec le jeu de donnees :
          zone_VEF.rang_frontiere(nom_paroi[b]);
        }
    }
  EcritureLectureSpecial::lecture_special(zone_VEF, fic, wall_length);
}

void Modele_turbulence_Longueur_Melange_VEF::calculer_f_amortissement( )
{
  const double Kappa  = 0.415;
  const double A_plus = 26.;

  // PQ : 23/02/04 calcul de la fonction d'amortissement de Van Driest
  //            a partir de la distance a la paroi et d'une approximation
  //            du frottement

  // la fonction d'amortissement f_vd = 1 - exp(-y+/A+) intervient dans le calcul de nu_t
  // la puissance 4 (calibrage issu d'un canal plan donnant les "meilleurs" resultats).
  //
  // on restreint le calcul de f_vd qu'aux valeurs de y+ t.q. : f_vd^4 < 0.99
  // soit : y+ < -A+. ln(1-0.99^(1/4)) = 155
  // soit, encore d'apres la loi de Reichard   u+ < 17.96
  // d'ou  :
  //                 f_vd^4 < 0.99          =>   u+.y+ < 2784
  //

  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_elem = zone_VEF.nb_elem();
  DoubleTab& wall_length = wall_length_.valeurs();
  int nb_face_elem = zone_VEF.zone().nb_faces_elem();
  const IntTab& elem_faces = zone_VEF.elem_faces();

  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, mon_equation->milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  const DoubleTab& vit = mon_equation->inconnue().valeurs();

  f_amortissement.resize(nb_elem);
  f_amortissement=1.;

  ArrOfDouble vc(dimension);
  double dist,d_visco,norm_v,u_plus_d_plus,dp,up1,up2,r,y_plus;
  int iter;
  const int itmax  = 25;
  double seuil=0.001;

  double visco=-1;
  int l_unif;

  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    l_unif = 0;

  if ((!l_unif) && (tab_visco.local_min_vect()<DMINFLOAT))
    //   on ne doit pas changer tab_visco ici !
    { 
      Cerr<<" visco <=0 ?"<<finl;
      exit();
    } 
  //tab_visco+=DMINFLOAT;

  for (int elem=0 ; elem<nb_elem ; elem++)
    {
      vc=0.;
      dist=wall_length(elem);

      if (l_unif) d_visco = visco;
      else        d_visco = tab_visco[elem];

      for (int i=0; i<nb_face_elem; i++)
        {
          int face=elem_faces(elem,i);

          for (int comp=0; comp<dimension; comp++)
            vc[comp]+=vit(face,comp);
        }

      vc /= dimension;
      norm_v = norme_array(vc);

      u_plus_d_plus = norm_v*dist/d_visco;

      if( u_plus_d_plus<1. ) // pour eviter de faire tourner la procedure iterative
        {
          y_plus = sqrt(u_plus_d_plus);
          f_amortissement(elem) -= exp(-y_plus/A_plus);
        }
      else if( u_plus_d_plus<2784. ) // cf. explication plus haut
        {
          up1  = u_plus_d_plus/100.;
          iter = 0;
          r    = 1.;

          while((iter++<itmax) && (r>seuil))
            {
              dp  = u_plus_d_plus/ up1 ;
              up2 = ((1/Kappa)*log(1+Kappa*dp))+7.8*(1-exp(-dp/11.)-exp(-dp/3.)*dp/11.); // Equation de Reichardt
              up1 = 0.5*(up1+up2);
              r   = dabs(up1-up2)/up1;
            }
          if (iter >= itmax) erreur_non_convergence();

          y_plus = u_plus_d_plus/up1;
          f_amortissement(elem) -= exp(-y_plus/A_plus);
        }
    }
}

int Modele_turbulence_Longueur_Melange_VEF::preparer_calcul( )
{
  if (cas==4)
    lire_distance_paroi();

  Mod_turb_hyd_base::preparer_calcul();
  mettre_a_jour(0.);

  return 1;
}

void Modele_turbulence_Longueur_Melange_VEF::discretiser()
{
  const Discretisation_base& dis=ref_cast(Discretisation_base, mon_equation->discretisation());
  Mod_turb_hyd_RANS_0_eq::discretiser();
  dis.discretiser_champ("champ_elem",mon_equation->zone_dis().valeur(),"distance_paroi","m",1,mon_equation->schema_temps().temps_courant(),wall_length_);
  champs_compris_.ajoute_champ(wall_length_);
}
