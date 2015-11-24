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
// File:        Paroi_std_hyd_VEF_old.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_std_hyd_VEF_old.h>
#include <Champ_Q1NC.h>
#include <Champ_P1NC.h>
#include <Fluide_Incompressible.h>
#include <Champ_Uniforme.h>
#include <Les_Cl.h>
#include <Static_Int_Lists.h>
#include <Debog.h>
#include <IntList.h>
#include <EcrFicPartage.h>
#include <Paroi_std_hyd_VEF.h>
#include <Mod_turb_hyd_base.h>
#include <Param.h>

Implemente_instanciable(Paroi_std_hyd_VEF_old,"loi_standard_hydr_old_VEF",Paroi_hyd_base_VEF);

//     printOn()
/////

Sortie& Paroi_std_hyd_VEF_old::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_std_hyd_VEF_old::readOn(Entree& s)
{
  Paroi_hyd_base_VEF::readOn(s);
  return s ;
}

void Paroi_std_hyd_VEF_old::set_param(Param& param)
{
  Paroi_hyd_base_VEF::set_param(param);
  Paroi_log_QDM::set_param(param);
}
/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_std_hyd_VEF_old
//
/////////////////////////////////////////////////////////////////////


int Paroi_std_hyd_VEF_old::init_lois_paroi()
{
  Paroi_hyd_base_VEF::init_lois_paroi_();
  dplus_.resize(la_zone_VEF->nb_faces_tot());
  uplus_.resize(la_zone_VEF->nb_faces_tot());
  return init_lois_paroi_hydraulique();
}

// Remplissage de la table

int Paroi_std_hyd_VEF_old::init_lois_paroi_hydraulique()
{
  Cmu = mon_modele_turb_hyd->get_Cmu();
  init_lois_paroi_hydraulique_();
  return 1;
}

// On annule les valeurs des grandeurs turbulentes qui
// correspondent aux mailles de paroi

int Paroi_std_hyd_VEF_old::preparer_calcul_hyd(DoubleTab& tab)
{
  int nb_dim = tab.nb_dim();
  const IntTab& face_voisins = la_zone_VEF->face_voisins();

  // Boucle sur les bords

  int ndeb,nfin,elem;

  for (int n_bord=0; n_bord<la_zone_VEF->nb_front_Cl(); n_bord++)
    {

      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);

      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))  ||
           (sub_type(Dirichlet_paroi_defilante,la_cl.valeur())) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          if (nb_dim == 1)
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                elem = face_voisins(num_face,0);
                tab(elem) = 0;
              }
          else if (nb_dim == 2)
            {
              int nb_comp= tab.dimension(1);
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  elem = face_voisins(num_face,0);
                  for (int k=0; k<nb_comp; k++)
                    tab(elem,k) = 0;
                }
            }
          else
            {
              Cerr << "Erreur TRUST dans Paroi_std_hyd_VEF_old::preparer_calculer_hyd" << finl;
              Cerr << "Le DoubleTab tab ne peut pas avoir plus de 2 entrees" << finl;
              exit();
            }
        }             // fin de la boucle if
    }                // fin de la boucle for
  return 1;
}                   // fin de preparer_calcul_hydraulique


int Paroi_std_hyd_VEF_old::calculer_hyd(DoubleTab& tab_k_eps)
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();


  //const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& vit = eqn_hydr.inconnue().valeurs();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  const Zone& zone = zone_VEF.zone();
  int nfac = zone.nb_faces_elem();

  //int ndebint = zone_VEF.premiere_face_int();
  double visco=-1;
  int l_unif;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    l_unif = 0;

  // preparer_calcul_hyd(tab_k_eps);
  if ((!l_unif) && (tab_visco.local_min_vect()<DMINFLOAT))
    //   on ne doit pas changer tab_visco ici !
    {
      Cerr<<" visco <=0 ?"<<finl;
      exit();
    }
  //    tab_visco+=DMINFLOAT;

  int ndeb,nfin,nfc;
  double norm_v=-1;//,norm_v1,norm_v2;
  double dist=-1;
  double u_plus_d_plus,d_visco=-1,u_star;
  //double val1,val2,val3;
  ArrOfDouble val(dimension);
  IntVect num(nfac);
  Cisaillement_paroi_=0;
  int is_champ_Q1NC=sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur());
  //  methode_calcul_face_bloquee pour garder l'ancien comportement
  int  methode_calcul_face_bloquee=1;
  remplir_face_keps_imposee( flag_face_keps_imposee_, methode_calcul_face_bloquee,face_keps_imposee_, zone_VEF,la_zone_Cl_VEF,!is_champ_Q1NC);

  // Boucle sur les bords
  ArrOfInt is_defilante_face(zone_VEF.nb_faces_tot());
  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {

      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) || (sub_type(Dirichlet_paroi_defilante,la_cl.valeur())))
        {
          int is_defilante=sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) ;
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          const IntTab& elem_faces = zone_VEF.elem_faces();
          ndeb = 0;
          nfin = le_bord.nb_faces();
          //          int ndeb_virt = 0;
          //int nfin_virt = nb_faces_virt;
          if ((dimension == 2)|| (dimension ==3))
            {

              // On traite les faces de bord reelles.
              for (int ind_face=ndeb; ind_face<nfin; ind_face++)
                {
                  int num_face=le_bord.num_face(ind_face);
                  is_defilante_face[num_face]=is_defilante;
                  int elem=face_voisins(num_face,0);
                  int nf2;

                  // on determine les face de l'elements qui sont autres que le num_face traite
                  for (nf2=0; nf2<nfac; nf2++)
                    {
                      num[nf2] = elem_faces(elem,nf2);
                    }
                  // Maintenant on place le num_face en fin de tableau
                  for (nf2=0; nf2<nfac-1; nf2++)
                    {
                      num[nf2] = elem_faces(elem,nf2);
                      if (num[nf2] == num_face)
                        {
                          num[nf2] = num[nfac-1];
                          num[nfac-1] = num_face;
                        }
                    }

                  nfc=0;
                  // Boucle sur les faces :
                  for (int nf=0; nf<nfac; nf++)
                    {
                      if (num[nf]==num_face)
                        {


                          //          if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                          // Strategie pour les tetras :
                          // On impose k et eps a la paroi :
                          // approximation: d(k)/d(n) = 0 a la paroi
                          // c'est faux mais ca marche
                          tab_k_eps(num[nf],0)=0.;
                          tab_k_eps(num[nf],1)=0.;
                          int nk=0;

                          for (int k=0; k<nfac; k++)
                            //if ( (num[k] >= ndebint) && (k != nf))
                            if ( (face_keps_imposee_[num[k]]>-1) && (k != nf))
                              {

                                tab_k_eps(num[nf],0)+= tab_k_eps(num[k],0);
                                tab_k_eps(num[nf],1)+= tab_k_eps(num[k],1);
                                nk++;
                              }

                          if (nk != 0 )
                            {
                              tab_k_eps(num[nf],0)/=nk;
                              tab_k_eps(num[nf],1)/=nk;
                            }

                        }


                      // On verifie si num[nf] n'est pas une face de bord :

                      else  if ( (face_keps_imposee_[num[nf]]>-1))//if (num[nf]>=ndebint)
                        {


                          nfc++;
                          norm_v=norm_vit_lp_k(vit,num[nf],num_face,zone_VEF,val,is_defilante);
                          //  dist=distance_face(num_face,num[nf],zone_VEF);
                          //if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                          if (!is_champ_Q1NC)
                            {
                              dist=distance_face(num_face,num[nf],zone_VEF);
                            }
                          else
                            {
                              assert(sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur())) ;
                              dist=distance_face_elem(num_face,elem,zone_VEF);
                            }

                          if (l_unif)
                            d_visco = visco;
                          else
                            d_visco = tab_visco[elem];
                          u_plus_d_plus = norm_v*dist/d_visco;

                          // Calcul de u* et des grandeurs turbulentes:
                          u_star=calculer_local(u_plus_d_plus,d_visco,tab_k_eps,norm_v,dist,num[nf],num_face);
                          // Calcul de la contrainte tangentielle
                          double vit_frot = u_star*u_star;
                          for (int dir=0; dir<dimension; dir++)
                            Cisaillement_paroi_(num_face,dir) += vit_frot*val[dir];
                          //Cisaillement_paroi_(num_face,0) += u_star*u_star*val[0];
                          // Fin de la strategie du calcul generalise de la loi de paroi
                        }
                      else
                        {
                          //Cerr<<"Attention, on n'a rien fait dans Paroi_std_hyd_VEF_old::calculer_hyd"<<finl;
                          //Cerr<<"pour le numero de face num[nf] = "<<num[nf]<<" et num_face = "<<num_face<<finl;
                          //Cerr<<"ndebint = "<<ndebint<<finl;
                        }
                    }
                  // A voir si juste :
                  if (nfc != 0 )
                    for (int dir=0; dir<dimension; dir++)
                      Cisaillement_paroi_(num_face,dir)/=nfc;

                  double res=0;
                  for (int dir=0; dir<dimension; dir++)
                    {
                      res+=tau_tang(num_face,dir)*tau_tang(num_face,dir);
                    }
                  res=sqrt(sqrt(res));
                  /*
                    if (res!=sqrt(tau_tang(num_face,0)))
                    {
                    Cerr<<"ici"<<tau_tang(num_face,1)<<finl;
                    exit();
                    }
                  */
                  tab_u_star_(num_face)=res;
                  // Calcul de u+ d+
                  calculer_uplus_dplus(uplus_, dplus_, tab_u_star_, num_face, dist, d_visco, norm_v) ;
                } // Fin du traitement des faces reelles
            }



        }  // fin de la cond Dirichlet paroi_fixe

    }   // fin de la boucle for CL





#ifdef CONTROL
  // TEST faces_k_eps
  if ((!flag_face_keps_imposee_)&&(!is_champ_Q1NC))
    {
      if (0)
        {
          int nb_faces_tot=zone_VEF.nb_faces_tot();
          DoubleVect toto(nb_faces_tot);
          const ArrOfInt& renum=Debog::renum_faces();
          for (int i=0; i<nb_faces_tot; i++)
            if ((face_keps_imposee_(i)>-1)&&(Debog::mode_db==1))
              {
                if (i>1000) Cerr<<me()<<" face_virt "<<i<<finl;
                toto(i)=renum(face_keps_imposee_(i));
              }
            else toto(i)=face_keps_imposee_(i);
          barrier();
          Debog::verifier("face_keps ",toto);
        }
      int tutu=0;
      ArrOfInt test;
      //  methode_calcul_face_bloquee pour garder l'ancien comportement
      int  methode_calcul_face_bloquee=1;
      remplir_face_keps_imposee( tutu, test,methode_calcul_face_bloquee, zone_VEF,la_zone_Cl_VEF,!is_champ_Q1NC);
      test-=face_keps_imposee_;
      if (max(test)>0|| min(test)<0)
        {
          const DoubleTab& xv=zone_VEF.xv();
          Cerr<<"TEST "<<finl;
          int compteur=0,compteur2=0;
          for (int i=0; i<test.size_array(); i++)
            {
              test(i)+=face_keps_imposee_(i);
              if (test(i)!=face_keps_imposee_(i))
                {

                  Cerr<<me()<<" face "<<i<<" : " <<face_keps_imposee_(i)<<" "<<test(i)<<" pos "<<xv(i,0)<<" "<<xv(i,1);
                  if (dimension==3) Cerr<<" "<<xv(i,2);

                  if (face_keps_imposee_(i)!=-2)
                    {
                      Cerr<<" pos trouve "<<xv(face_keps_imposee_(i),0)<<" "<<xv(face_keps_imposee_(i),1);
                      if (dimension==3) Cerr<<" "<<xv(face_keps_imposee_(i),2);
                    }
                  if (test(i)!=-2)
                    {
                      Cerr<<" pos test "<<xv(test(i),0)<<" "<<xv(test(i),1);
                      if (dimension==3) Cerr<<" "<<xv(test(i),2);
                    }
                  Cerr<<finl;
                }
              if (test(i)>-1) compteur++;
              if (face_keps_imposee_(i)>-1) compteur2++;
            }
          Cerr<<"compteurs "<<compteur2<<" "<<compteur<<finl;
          exit();
        }
      // else exit();

    }
#endif

  // on recalcule partout ou c'est impose
  int nb_faces_tot=zone_VEF.nb_faces_tot();
  //int tutu=0;
  //remplir_face_keps_imposee( flag_face_keps_imposee_, face_keps_imposee_, zone_VEF,la_zone_Cl_VEF,!is_champ_Q1NC);
  //  if (!is_champ_Q1NC)
  for (int face=0; face<nb_faces_tot; face++)
    {
      int num_face=face_keps_imposee_[face];;
      if (num_face>-1)
        {
          //int elem_voisin;
          int elem=face_voisins(num_face,0);
          //if (face_voisins(face,0)!=elem) elem_voisin=face_voisins(face,0);
          //else elem_voisin=face_voisins(face,1);
          //int elem_voisin=face_voisins(num_face,0);
          // ce n'est pas le bon voisin!!!!
          double distbis;

          //dist=distance_face(num_face,face,zone_VEF);

          if (!is_champ_Q1NC)
            {
              distbis=distance_face(num_face,face,zone_VEF);
            }
          else
            {
              assert(sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur())) ;
              distbis=distance_face_elem(num_face,elem,zone_VEF);
            }

          norm_v=norm_vit_lp_k(vit,face,num_face,zone_VEF,val,is_defilante_face[num_face]);


          if (l_unif)
            d_visco = visco;
          else
            d_visco = tab_visco[elem];
          u_plus_d_plus = norm_v*distbis/d_visco;
          u_star=calculer_local(u_plus_d_plus,d_visco,tab_k_eps,norm_v,distbis,face,num_face);
        }
    }


  Cisaillement_paroi_.echange_espace_virtuel();
  tab_k_eps.echange_espace_virtuel();



  return 1;
}  // fin de calcul_hyd (K-eps)


int Paroi_std_hyd_VEF_old::calculer_hyd(DoubleTab& tab_nu_t,DoubleTab& tab_k)
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const Equation_base& eqn_hydr = mon_modele_turb_hyd->equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& vit = eqn_hydr.inconnue().valeurs();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  const Zone& zone = zone_VEF.zone();
  int nfac = zone.nb_faces_elem();

  double visco=-1;
  int l_unif;
  int n_bord;

  //   Debog::verifier("Paroistd::calhyd tab_nu_t au depart=",tab_nu_t);
  //   Debog::verifier("Paroistd::calhyd tab_k au depart=",tab_k);
  //   Process::Journal()<<"Face voisins : "<<finl;
  //   face_voisins.ecrit(Process::Journal());
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      visco = max(tab_visco(0,0),DMINFLOAT);
      l_unif = 1;
    }
  else
    l_unif = 0;

  // preparer_calcul_hyd(tab);
  if ((!l_unif) && (tab_visco.local_min_vect()<DMINFLOAT))
    //   on ne doit pas changer tab_visco ici !
    {
      Cerr<<" visco <=0 ?"<<finl;
      exit();
    }
  //tab_visco+=DMINFLOAT;

  int ndeb,nfin;
  double norm_v=-1;
  double dist=-1;
  double u_plus_d_plus,d_visco;
  double val,val1,val2,val3;
  // IntVect num(dimension);
  IntVect num(nfac);

  // Boucle sur les bords
  int nb_bords=zone_VEF.nb_front_Cl();
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      // pour chaque condition limite on regarde son type
      // On applique les lois de paroi uniquement
      // aux voisinages des parois

      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          const IntTab& elem_faces = zone_VEF.elem_faces();
          ndeb = 0;
          nfin = le_bord.nb_faces_tot();

          if (dimension == 2 )
            {
              for (int ind_face=ndeb; ind_face<nfin; ind_face++)
                {
                  int num_face=le_bord.num_face(ind_face);
                  int elem = face_voisins(num_face,0);
                  if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                    {
                      num[0]=elem_faces(elem,0);
                      num[1]=elem_faces(elem,1);

                      if (num[0]==num_face) num[0]=elem_faces(elem,2);
                      else if (num[1]==num_face) num[1]=elem_faces(elem,2);

                      dist = distance_2D(num_face,elem,zone_VEF);

                      // PQ : 03/03 : pour passer de la distance paroi / centre de gravite a la distance paroi / milieu de num[0]-num[1]
                      dist *=3./2.;
                      // norm_v=norm_2D_vit1(vit,num[0],num[1],num_face,zone_VEF,val);
                      norm_v=norm_2D_vit1_lp(vit,num_face,num[0],num[1],zone_VEF,val1,val2);

                    }
                  else if (sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur()))
                    {
                      num[0]=elem_faces(elem,0);
                      num[1]=elem_faces(elem,1);
                      num[2]=elem_faces(elem,2);

                      if (num[0]==num_face) num[0]=elem_faces(elem,3);
                      else if (num[1]==num_face) num[1]=elem_faces(elem,3);
                      else if (num[2]==num_face) num[2]=elem_faces(elem,3);

                      dist=distance_2D(num_face,elem,zone_VEF);
                      norm_v=norm_2D_vit1(vit,num[0],num[1],num[2],num_face,zone_VEF,val);
                    }
                  if (l_unif)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];

                  u_plus_d_plus = norm_v*dist/d_visco;

                  // Calcul de u* et des grandeurs turbulentes:
                  tab_u_star_(num_face)=calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);

                  // Calcul de la contrainte tangentielle

                  double vit_frot = tab_u_star(num_face)*tab_u_star(num_face);

                  if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                    {
                      Cisaillement_paroi_(num_face,0) = vit_frot*val1;
                      Cisaillement_paroi_(num_face,1) = vit_frot*val2;
                    }
                  else if (sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur()))
                    Cisaillement_paroi_(num_face,0) = vit_frot*val;


                  // Calcul de u+ d+
                  calculer_uplus_dplus(uplus_, dplus_, tab_u_star_, num_face, dist, d_visco, norm_v) ;

                  // Strategie de calcul de la loi de paroi sur tous
                  // les elements ayant un sommet pres d'une paroi
                  for (int i=0; i<nfac-1; i++)
                    {
                      int elem_voisin;
                      int num0=num[i];
                      if (face_voisins(num0,0)!=elem) elem_voisin=face_voisins(num0,0);
                      else elem_voisin=face_voisins(num0,1);

                      if (elem_voisin!=-1)
                        {
                          if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                            {
                              int num1=elem_faces(elem_voisin,0);
                              int num2=elem_faces(elem_voisin,1);
                              if (num1==num0) num1=elem_faces(elem_voisin,2);
                              else if (num2==num0) num2=elem_faces(elem_voisin,2);
                              if (rang_elem_non_std(elem_voisin)==-1)
                                {
                                  //if (num1>=nfin && num2>=nfin) {
                                  // elem_voisin n'est pas un element de bord
                                  norm_v=norm_2D_vit1(vit,num1,num2,num0,num_face,zone_VEF,val);
                                  dist = distance_2D(num_face,elem_voisin,zone_VEF);
                                  // PQ : 03/03 : pour passer de la distance paroi / centre de gravite a la distance paroi / milieu de num[0]-num[1]
                                  //        dist *=3./2.;

                                }
                            }
                          else if (sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur()))
                            {
                              int num1=elem_faces(elem_voisin,0);
                              int num2=elem_faces(elem_voisin,1);
                              int num3=elem_faces(elem_voisin,2);
                              if (num1==num0) num1=elem_faces(elem_voisin,3);
                              else if (num2==num0) num2=elem_faces(elem_voisin,3);
                              else if (num3==num0) num3=elem_faces(elem_voisin,3);

                              if (num1>=nfin && num2>=nfin && num3>=nfin)
                                {
                                  // elem_voisin n'est pas un element de bord
                                  // double dist=distance_2D(num_face,elem_voisin,zone_VEF);
                                  norm_v=norm_2D_vit1(vit,num1,num2,num3,num0,num_face,zone_VEF,val);
                                }
                            }

                          if (l_unif)
                            d_visco = visco;
                          else
                            d_visco = tab_visco[elem_voisin];
                          u_plus_d_plus = norm_v*dist/d_visco;
                          //                   double u_star=
                          // ATTENTION u_star ne sert pas mais calculer_local modifie les arguments
                          calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem_voisin,num_face);
                        }
                    }
                  // Fin de la strategie du calcul generalise de la loi de paroi
                }  // fin du for faces reelles (2D)

            }// Fin du test sur la dimension == 2
          else if (dimension == 3)
            {
              // Debut du for faces de bord (3D)

              for (int ind_face=ndeb; ind_face<nfin; ind_face++)
                {
                  int num_face=le_bord.num_face(ind_face);
                  int elem = face_voisins(num_face,0);
                  if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                    {
                      num[0] = elem_faces(elem,0);
                      num[1] = elem_faces(elem,1);
                      num[2] = elem_faces(elem,2);
                      if (num[0]==num_face) num[0]=elem_faces(elem,3);
                      else if (num[1]==num_face) num[1]=elem_faces(elem,3);
                      else if (num[2]==num_face) num[2]=elem_faces(elem,3);

                      //   norm_v=norm_3D_vit1(vit, num_face, num[0], num[1], num[2], zone_VEF, val1, val2, val3);

                      dist = distance_3D(num_face,elem,zone_VEF);
                      // PQ : 03/03 : pour passer de la distance paroi / centre de gravite a la distance paroi / milieu de num[0]-num[1]-num[2]
                      dist *= 4./3.;
                      norm_v=norm_3D_vit1_lp(vit, num_face, num[0], num[1], num[2], zone_VEF, val1, val2, val3);
                    }
                  else if (sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur()))
                    {
                      num[0]=elem_faces(elem,0);
                      num[1]=elem_faces(elem,1);
                      num[2]=elem_faces(elem,2);
                      num[3]=elem_faces(elem,3);
                      num[4]=elem_faces(elem,4);
                      if (num[0]==num_face) num[0]=elem_faces(elem,5);
                      else if (num[1]==num_face) num[1]=elem_faces(elem,5);
                      else if (num[2]==num_face) num[2]=elem_faces(elem,5);
                      else if (num[3]==num_face) num[1]=elem_faces(elem,5);
                      else if (num[4]==num_face) num[2]=elem_faces(elem,5);

                      dist=distance_3D(num_face,elem,zone_VEF);
                      norm_v=norm_3D_vit1(vit,num_face,num[0],num[1],num[2],num[3],num[4],zone_VEF,val1,val2,val3);
                    }

                  if (l_unif)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];

                  u_plus_d_plus = norm_v*dist/d_visco;

                  // Calcul de u* et des grandeurs turbulentes:
                  tab_u_star_(num_face)=calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);

                  // Calcul des deux composantes de la contrainte tangentielle:
                  double vit_frot = tab_u_star(num_face)*tab_u_star(num_face);

                  Cisaillement_paroi_(num_face,0) = vit_frot*val1;
                  Cisaillement_paroi_(num_face,1) = vit_frot*val2;
                  Cisaillement_paroi_(num_face,2) = vit_frot*val3;
                  // Calcul de u+ d+
                  calculer_uplus_dplus(uplus_, dplus_, tab_u_star_, num_face, dist, d_visco, norm_v) ;

                  // Strategie de calcul de la loi de paroi sur tous
                  // les elements ayant un sommet pres d'une paroi
                  for (int i=0; i<nfac-1; i++)
                    {
                      int elem_voisin;
                      int num0=num[i];
                      if (face_voisins(num0,0)!=elem) elem_voisin=face_voisins(num0,0);
                      else elem_voisin=face_voisins(num0,1);
                      if (elem_voisin!=-1)
                        {
                          if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                            {
                              int num1=elem_faces(elem_voisin,0);
                              int num2=elem_faces(elem_voisin,1);
                              int num3=elem_faces(elem_voisin,2);
                              if (num1==num0) num1=elem_faces(elem_voisin,3);
                              else if (num2==num0) num2=elem_faces(elem_voisin,3);
                              else if (num3==num0) num3=elem_faces(elem_voisin,3);
                              if (rang_elem_non_std(elem_voisin)==-1)
                                {
                                  //if (num1>=nfin && num2>=nfin && num3>=nfin) {
                                  // elem_voisin n'est pas un element de bord
                                  norm_v=norm_3D_vit1(vit,num1,num2,num3,num0,num_face,zone_VEF,val1,val2,val3);
                                  dist = distance_3D(num_face,elem_voisin,zone_VEF);
                                  //dist *= 4./3.;
                                }
                            }
                          else if (sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur()))
                            {
                              int num1=elem_faces(elem_voisin,0);
                              int num2=elem_faces(elem_voisin,1);
                              int num3=elem_faces(elem_voisin,2);
                              int num4=elem_faces(elem_voisin,3);
                              int num5=elem_faces(elem_voisin,4);
                              if (num1==num0) num1=elem_faces(elem_voisin,5);
                              else if (num2==num0) num2=elem_faces(elem_voisin,5);
                              else if (num3==num0) num3=elem_faces(elem_voisin,5);
                              else if (num4==num0) num2=elem_faces(elem_voisin,5);
                              else if (num5==num0) num3=elem_faces(elem_voisin,5);

                              if (num1>=nfin && num2>=nfin && num3>=nfin && num4>=nfin && num5>=nfin)
                                {
                                  // elem_voisin n'est pas un element de bord
                                  norm_v=norm_3D_vit1(vit,num0,num1,num2,num3,num4,num5,zone_VEF,val1,val2,val3);
                                  dist = distance_3D(num_face,elem_voisin,zone_VEF);
                                }
                              if (l_unif)
                                d_visco = visco;
                              else
                                d_visco = tab_visco[elem_voisin];

                              u_plus_d_plus = norm_v*dist/d_visco;
                              //                   double u_star=
                              // ATTENTION u_star ne sert pas mais calculer_local modifie les arguments
                              calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem_voisin,num_face);
                            }
                        }
                    }
                }  // fin du for faces de bord (3D)


            }  // Fin du test sur dimension == 3
        } //  fin de Dirichlet_paroi_fixe
      else if (sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
        {
          //          const Dirichlet_paroi_defilante& cl_diri = ref_cast(Dirichlet_paroi_defilante,la_cl).valeur();
          const IntTab& elem_faces = zone_VEF.elem_faces();
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          if (dimension == 2 )
            {
              // On calcule au centre des mailles de bord
              // la vitesse parallele a l'axe de la face de bord
              //
              // On calcule la norme de cette vitesse

              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  int elem = face_voisins(num_face,0);

                  if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                    {
                      num[0] = elem_faces(elem,0);
                      num[1] = elem_faces(elem,1);
                      if (num[0]==num_face) num[0]=elem_faces(elem,2);
                      else if (num[1]==num_face) num[2]=elem_faces(elem,2);

                      norm_v=norm_2D_vit2(vit,num[0],num[1],num_face,zone_VEF,val);

                      dist =distance_2D(num_face,elem,zone_VEF);
                    }
                  else if (sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur()))
                    {
                      num[0]=elem_faces(elem,0);
                      num[1]=elem_faces(elem,1);
                      num[2]=elem_faces(elem,2);

                      if (num[0]==num_face) num[0]=elem_faces(elem,3);
                      else if (num[1]==num_face) num[1]=elem_faces(elem,3);
                      else if (num[2]==num_face) num[2]=elem_faces(elem,3);

                      dist=distance_2D(num_face,elem,zone_VEF);
                      norm_v=norm_2D_vit2(vit,num[0],num[1],num[2],num_face,zone_VEF,val);

                    }
                  if (l_unif)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];

                  u_plus_d_plus = norm_v*dist/d_visco;

                  // Calcul de u* et des grandeurs turbulentes:
                  tab_u_star_(num_face)=calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);

                  // Calcul de la contrainte tangentielle a la paroi:
                  Cisaillement_paroi_(num_face,0) = tab_u_star(num_face)*tab_u_star(num_face)*val;
                  // Calcul de u+ d+
                  calculer_uplus_dplus(uplus_, dplus_, tab_u_star_, num_face, dist, d_visco, norm_v) ;

                  // Strategie de calcul de la loi de paroi sur tous
                  // les elements ayant un sommet pres d'une paroi
                  for (int i=0; i<nfac-1; i++)
                    {
                      int elem_voisin;
                      int num0=num[i];
                      if (face_voisins(num0,0)!=elem) elem_voisin=face_voisins(num0,0);
                      else elem_voisin=face_voisins(num0,1);
                      if (elem_voisin!=-1)
                        {
                          if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                            {
                              int num1=elem_faces(elem_voisin,0);
                              int num2=elem_faces(elem_voisin,1);
                              if (num1==num0) num1=elem_faces(elem_voisin,2);
                              else if (num2==num0) num2=elem_faces(elem_voisin,2);
                              if (rang_elem_non_std(elem_voisin)==-1)
                                {
                                  //if (num1>=nfin && num2>=nfin) {
                                  // elem_voisin n'est pas un element de bord
                                  norm_v=norm_2D_vit2(vit,num1,num2,num0,num_face,zone_VEF,val);
                                  dist = distance_2D(num_face,elem_voisin,zone_VEF);
                                }
                            }
                          else if (sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur()))
                            {
                              int num1=elem_faces(elem_voisin,0);
                              int num2=elem_faces(elem_voisin,1);
                              int num3=elem_faces(elem_voisin,2);
                              // Cerr << "num1 " << num1 << " num2 " << num2 << " num3 " << num3 << finl;
                              if (num1==num0) num1=elem_faces(elem_voisin,3);
                              else if (num2==num0) num2=elem_faces(elem_voisin,3);
                              else if (num3==num0) num3=elem_faces(elem_voisin,3);

                              if (num1>=nfin && num2>=nfin && num3>=nfin)
                                {
                                  // elem_voisin n'est pas un element de bord
                                  //                        double dist=distance_2D(num_face,elem_voisin,zone_VEF);
                                  norm_v=norm_2D_vit2(vit,num1,num2,num3,num0,num_face,zone_VEF,val);
                                }
                            }
                          if (l_unif)
                            d_visco = visco;
                          else
                            d_visco = tab_visco[elem_voisin];
                          u_plus_d_plus = norm_v*dist/d_visco;
                          //                   double u_star=
                          // ATTENTION u_star ne sert pas mais calculer_local modifie les arguments
                          calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem_voisin,num_face);
                        }
                    }
                }
              // Fin de la strategie du calcul generalise de la loi de paroi

            }  // fin du for faces(2D)
          else if (dimension == 3)
            {
              // On calcule au centre des mailles de bord
              // la vitesse dans le plan parallele a celui de la face de bord
              // On calcule la norme de cette vitesse

              // boucle sur les faces reelles de bord (3D)
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  int elem=face_voisins(num_face,0);

                  if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                    {
                      int num1=elem_faces(elem,0);
                      int num2=elem_faces(elem,1);
                      int num3=elem_faces(elem,2);
                      if (num1==num_face) num1=elem_faces(elem,3);
                      else if (num2==num_face) num2=elem_faces(elem,3);
                      else if (num3==num_face) num3=elem_faces(elem,3);

                      norm_v=norm_3D_vit2(vit,num_face,num1,num2,num3,zone_VEF,val1,val2,val3);

                      dist = distance_3D(num_face,elem,zone_VEF);
                    }
                  else if (sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur()))
                    {
                      num[0]=elem_faces(elem,0);
                      num[1]=elem_faces(elem,1);
                      num[2]=elem_faces(elem,2);
                      num[3]=elem_faces(elem,3);
                      num[4]=elem_faces(elem,4);
                      if (num[0]==num_face) num[0]=elem_faces(elem,5);
                      else if (num[1]==num_face) num[1]=elem_faces(elem,5);
                      else if (num[2]==num_face) num[2]=elem_faces(elem,5);
                      else if (num[3]==num_face) num[1]=elem_faces(elem,5);
                      else if (num[4]==num_face) num[2]=elem_faces(elem,5);

                      dist=distance_3D(num_face,elem,zone_VEF);
                      norm_v=norm_3D_vit2(vit,num_face,num[0],num[1],num[2],num[3],num[4],zone_VEF,val1,val2,val3);
                    }
                  if (l_unif)
                    d_visco = visco;
                  else
                    d_visco = tab_visco[elem];

                  u_plus_d_plus = norm_v*dist/d_visco;

                  // Calcul de u* et des grandeurs turbulentes:
                  tab_u_star_(num_face)=calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem,num_face);

                  // Calcul des deux composantes de la contrainte tangentielle:
                  double vit_frot = tab_u_star(num_face)*tab_u_star(num_face);

                  Cisaillement_paroi_(num_face,0) = vit_frot*val1;
                  Cisaillement_paroi_(num_face,1) = vit_frot*val2;
                  Cisaillement_paroi_(num_face,2) = vit_frot*val3;
                  // Calcul de u+ d+
                  calculer_uplus_dplus(uplus_, dplus_, tab_u_star_, num_face, dist, d_visco, norm_v) ;

                  // Strategie de calcul de la loi de paroi sur tous
                  // les elements ayant un sommet pres d'une paroi
                  for (int i=0; i<nfac-1; i++)
                    {
                      int elem_voisin;
                      int num0=num[i];
                      if (face_voisins(num0,0)!=elem) elem_voisin=face_voisins(num0,0);
                      else elem_voisin=face_voisins(num0,1);

                      if (elem_voisin!=-1)
                        {
                          if (sub_type(Champ_P1NC,eqn_hydr.inconnue().valeur()))
                            {
                              int num1=elem_faces(elem_voisin,0);
                              int num2=elem_faces(elem_voisin,1);
                              int num3=elem_faces(elem_voisin,2);
                              if (num1==num0) num1=elem_faces(elem_voisin,3);
                              else if (num2==num0) num2=elem_faces(elem_voisin,3);
                              else if (num3==num0) num3=elem_faces(elem_voisin,3);
                              if (rang_elem_non_std(elem_voisin)==-1)
                                {
                                  // elem_voisin n'est pas un element de bord
                                  // On appelle norm_3D_vit1 car il ne peut y avoir paroi defilante sur cette element !
                                  norm_v=norm_3D_vit2(vit,num1,num2,num3,num0,num_face,zone_VEF,val1,val2,val3);
                                  dist = distance_3D(num_face,elem_voisin,zone_VEF);
                                }
                            }
                          else if (sub_type(Champ_Q1NC,eqn_hydr.inconnue().valeur()))
                            {
                              int num1=elem_faces(elem_voisin,0);
                              int num2=elem_faces(elem_voisin,1);
                              int num3=elem_faces(elem_voisin,2);
                              int num4=elem_faces(elem_voisin,3);
                              int num5=elem_faces(elem_voisin,4);
                              if (num1==num0) num1=elem_faces(elem_voisin,5);
                              else if (num2==num0) num2=elem_faces(elem_voisin,5);
                              else if (num3==num0) num3=elem_faces(elem_voisin,5);
                              else if (num4==num0) num2=elem_faces(elem_voisin,5);
                              else if (num5==num0) num3=elem_faces(elem_voisin,5);

                              if (num1>=nfin && num2>=nfin && num3>=nfin && num4>=nfin && num5>=nfin)
                                {
                                  // elem_voisin n'est pas un element de bord
                                  norm_v=norm_3D_vit2(vit,num0,num1,num2,num3,num4,num5,zone_VEF,val1,val2,val3);
                                  dist = distance_3D(num_face,elem_voisin,zone_VEF);
                                }
                            }
                          if (l_unif)
                            d_visco = visco;
                          else
                            d_visco = tab_visco[elem_voisin];
                          u_plus_d_plus = norm_v*dist/d_visco;
                          //                   double u_star=
                          // ATTENTION u_star ne sert pas mais calculer_local modifie les arguments
                          calculer_local(u_plus_d_plus,d_visco,tab_nu_t,tab_k,norm_v,dist,elem_voisin,num_face);

                        }
                    } // Fin de la strategie du calcul generalise de la loi de paroi
                }  // fin du for faces (3D)
            }
        }  // fin de Dirichlet_paroi_defilante
    } // fin du for bord CL

  Cisaillement_paroi_.echange_espace_virtuel();
  tab_nu_t.echange_espace_virtuel();
  tab_k.echange_espace_virtuel();
  //   Debog::verifier("Paroistd::calhyd Cisaillement_paroi_ en fin=",Cisaillement_paroi_);
  //   Debog::verifier("Paroistd::calhyd tab_nu_t en fin=",tab_nu_t);
  //   Debog::verifier("Paroistd::calhyd tab_k en fin=",tab_k);
  return 1;
}  // fin du calcul_hyd (nu-t)


// Calcul de u+ d+
void Paroi_std_hyd_VEF_old::calculer_uplus_dplus(DoubleVect& uplus, DoubleVect& dplus, DoubleVect& tab_ustar,
                                                 int num_face, double dist, double d_visco, double dnorm_v)
{
  double ustar=tab_ustar(num_face);
  dplus(num_face)=ustar*dist/d_visco;
  if (ustar != 0)
    uplus(num_face)=dnorm_v/ustar;
}


double Paroi_std_hyd_VEF_old::calculer_local(double u_plus_d_plus,double d_visco,
                                             DoubleTab& k_eps,double norm_vit,
                                             double dist,int face,int num_face)
{
  double u_star;
  double valmin = table_hyd.val(5);
  double valmax = table_hyd.val(30);



  if (u_plus_d_plus <= valmin)
    {
      u_star=calculer_u_star_sous_couche_visq(norm_vit,d_visco,dist,num_face);
      calculer_sous_couche_visq(k_eps,face);
    }

  else if ((u_plus_d_plus > valmin) && ( u_plus_d_plus < valmax))
    {
      double d_plus;
      u_star=calculer_u_star_sous_couche_tampon(d_plus,u_plus_d_plus,d_visco,dist,num_face);
      calculer_sous_couche_tampon(u_star,k_eps,d_visco,d_plus,face,num_face);
    }

  else // if (u_plus_d_plus >= valmax)
    {
      assert(u_plus_d_plus >= valmax);
      u_star=calculer_u_star_sous_couche_log(norm_vit,d_visco,dist,num_face);
      calculer_sous_couche_log(u_star,k_eps,dist,face,num_face);
    }

  return u_star;
}



double Paroi_std_hyd_VEF_old::calculer_local(double u_plus_d_plus,double d_visco,
                                             DoubleTab& tab_nu_t,DoubleTab& tab_k,double norm_vit,
                                             double dist,int elem,int num_face)
{
  double u_star;
  double valmin = table_hyd.val(5);
  double valmax = table_hyd.val(30);
  if (u_plus_d_plus <= valmin)
    {
      u_star=calculer_u_star_sous_couche_visq(norm_vit,d_visco,dist,num_face);
      calculer_sous_couche_visq(tab_nu_t,tab_k,elem);
    }

  else if ((u_plus_d_plus > valmin) && (u_plus_d_plus < valmax))
    {
      double d_plus;
      u_star=calculer_u_star_sous_couche_tampon(d_plus,u_plus_d_plus,d_visco,dist,num_face);
      calculer_sous_couche_tampon(u_star,tab_nu_t,tab_k,d_visco,d_plus,elem,num_face);
    }

  else //if (u_plus_d_plus >= valmax)
    {
      assert(u_plus_d_plus >= valmax);
      u_star=calculer_u_star_sous_couche_log(norm_vit,d_visco,dist,num_face);
      calculer_sous_couche_log(u_star,tab_nu_t,tab_k,dist,elem,num_face);
    }
  return u_star;
}


double Paroi_std_hyd_VEF_old::calculer_u_star_sous_couche_visq(double norm_vit,
                                                               double d_visco,double dist,
                                                               int face)
{
  // Dans la sous couche visqueuse:  u* = sqrt(u*nu/d)

  return sqrt(norm_vit*d_visco/dist);
}


int Paroi_std_hyd_VEF_old::calculer_sous_couche_visq(DoubleTab& K_eps,int face)
{
  // Dans la sous couche visqueuse: k = eps = 0

  K_eps(face,0) = 0.;
  K_eps(face,1) = 0.;
  return 1;
}


int Paroi_std_hyd_VEF_old::calculer_sous_couche_visq(DoubleTab& nu_t,DoubleTab& k,int elem)
{
  // Dans la sous couche visqueuse: nu_t = k = 0

  nu_t(elem) = 0.;
  k(elem) = 0.;
  return 1;
}

double Paroi_std_hyd_VEF_old::calculer_u_star_sous_couche_tampon(double& d_plus,double u_plus_d_plus,
                                                                 double d_visco,double dist,int face)
{

  // Calcul de d_plus solution de table_hyd(inconnue) = u_plus_d_plus
  // puis calcul de u* dans la sous-couche tampon : u* = nu*d+/dist
  double d_plus_min = 5;
  double d_plus_max = 30;
  double valeur;
  double u_star=0;
  double epsilon = 1.e-12;
  double gauche = table_hyd.val(d_plus_min);
  double droite = table_hyd.val(d_plus_max);
  double deriv;
  if (gauche == droite)
    d_plus = d_plus_min;
  else
    {
      while(1)
        {
          deriv = (droite-gauche)/(d_plus_max-d_plus_min);
          d_plus = d_plus_min + (u_plus_d_plus - gauche)/deriv;
          valeur = table_hyd.val(d_plus);
          if(dabs(valeur-u_plus_d_plus) < epsilon)
            {
              return (d_visco*d_plus)/dist;
            }
          if(valeur>u_plus_d_plus)
            {
              droite=valeur;
              d_plus_max=d_plus;
            }
          else
            {
              gauche=valeur;
              d_plus_min=d_plus;
            }
        }
    }
  Cerr << "Probleme dans Paroi_std_hyd_VEF_old::calculer_u_star_sous_couche_tampon" << finl;
  exit();
  return u_star;
}


int Paroi_std_hyd_VEF_old::calculer_sous_couche_tampon(double u_star, DoubleTab& K_eps,double d_visco,
                                                       double d_plus,int face,int num_face)
{

  // Calcul des grandeurs turbulentes a partir de d_plus et de u_star
  double lm_plus = calcul_lm_plus(d_plus);
  double  deriv = Fdypar_direct(lm_plus);
  double x= lm_plus*u_star*deriv;

  // Dans la sous couche tampon :
  //
  //  k = lm+ * u* * derivee(u+d+(d+))/sqrt(Cmu)
  //
  //              2
  //  eps = k * u* * derivee(u+d+(d+))*sqrt(Cmu)/nu
  //

  //   K_eps(face,0) = max( K_eps(face,0) , x*x/sqrt(Cmu) );

  //K_eps(face,1) = max( K_eps(face,1) , (K_eps(face,0)*u_star*u_star*deriv)*sqrt(Cmu)/d_visco );

  K_eps(face,0) = x*x/sqrt(Cmu) ;

  K_eps(face,1) = (K_eps(face,0)*u_star*u_star*deriv)*sqrt(Cmu)/d_visco;

  return 1;
}


int Paroi_std_hyd_VEF_old::calculer_sous_couche_tampon(double u_star, DoubleTab& nu_t,DoubleTab& tab_k,
                                                       double d_visco,double d_plus,
                                                       int elem,int face)
{
  // Calcul des grandeurs turbulentes a partir de d_plus et de u_star
  double lm_plus = calcul_lm_plus(d_plus);
  double deriv = Fdypar_direct(lm_plus);
  double x= lm_plus*u_star*deriv;

  // Dans la sous couche tampon :
  //
  //  k = lm+ * u* * derivee(u+d+(d+))/sqrt(Cmu)
  //
  //              2
  //  eps = k * u* * derivee(u+d+(d+))*sqrt(Cmu)/nu
  //
  //  nu_t = Cmu*k*k/eps
  //

  double k = x*x/sqrt(Cmu);

  double eps = (k*u_star*u_star*deriv)*sqrt(Cmu)/d_visco;

  tab_k(elem) = k;

  nu_t(elem) = Cmu*k*k/eps;

  return 1;
}

double Paroi_std_hyd_VEF_old::calculer_u_star_sous_couche_log(double norm_vit,double d_visco,
                                                              double dist, int face)
{
  // Dans la sous couche inertielle u* est solution d'une equation
  // differentielle resolue de maniere iterative

  const double Xpini = 30.;
  //  const double Xpini = 200.;
  const int itmax  = 25;
  //  const double seuil = 0.0001;
  const double seuil = 0.01;

  const double c1 = Kappa*norm_vit;
  const double c2 = log(Erugu*dist/d_visco);  // log = logarithme neperien
  double u_star,u_star1;
  //  double F,F1;

  u_star = Xpini*d_visco/dist;

  int iter = 0;
  u_star1 = 1;

  //  while (( fabs((u_star-u_star1)/u_star1) > seuil ) && (iter++ < itmax ) )
  //     {
  //       u_star=u_star1 ;
  //       //Cout << "iter=" << iter << "\n" ;
  //       F= u_star*(log(u_star)+c2) -c1 ;
  //       F1= log(u_star)+1+c2 ;
  //       u_star1=u_star- F/F1 ;
  //       //Cout << "u_star1=" << u_star1 << "\n" ;
  //     }
  //   u_star=u_star1 ;

  while ((iter++<itmax) && (dabs(u_star1) > seuil))
    {
      u_star1 = (c1-u_star*(log(u_star) + c2))/(c1 + u_star);
      u_star = (1+u_star1)*u_star;
    }
  if (iter >= itmax) erreur_non_convergence();
  return u_star;
}


int Paroi_std_hyd_VEF_old::calculer_sous_couche_log(double u_star,DoubleTab& K_eps,double dist,
                                                    int face,int num_face)
{

  // K et Eps sont donnes par les formules suivantes:
  //
  //              2                      3
  //    k =     u*/sqrt(Cmu)  et eps = u* / Kd
  //

  double u_star_carre = u_star*u_star;

  K_eps(face,0) = u_star_carre/sqrt(Cmu);
  K_eps(face,1) = u_star_carre*u_star/(Kappa*dist);

  return 1;
}

int Paroi_std_hyd_VEF_old::calculer_sous_couche_log(double u_star, DoubleTab& nu_t,DoubleTab& tab_k,
                                                    double dist,int elem,int face)
{
  //  nu_t = Cmu*k*k/eps
  //
  //                       2                      3
  //  En utilisant  k =  u*/sqrt(Cmu)  et eps = u* / Kd
  //
  //  on calcule nu_t en fonction de u*

  tab_k(elem) = u_star*u_star/sqrt(Cmu);
  nu_t(elem) = u_star*Kappa*dist ;
  return 1;
}


////////////////////////////////////////////////////
//
// Fonctions utiles au calcul des lois de paroi:
//
////////////////////////////////////////////////////


void Paroi_std_hyd_VEF_old::imprimer_ustar(Sortie& os) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  int ndeb,nfin;
  double upmoy,dpmoy,utaumoy;
  upmoy=0.;
  dpmoy=0.;
  utaumoy=0.;
  int compt=0;

  EcrFicPartage Ustar;
  ouvrir_fichier_partage(Ustar,"Ustar");


  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur())) ||
           (sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) ))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          if(je_suis_maitre())
            {
              Ustar << finl;
              Ustar << "Bord " << le_bord.le_nom() << finl;
              if (dimension == 2)
                {
                  Ustar << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                  Ustar << "\tFace a\t\t\t\t|\t\t\t\t\t\t\t\t\t| TAU=Nu.Grad(Ut) [m2/s2]" << finl;
                  Ustar << "----------------------------------------|-----------------------------------------------------------------------|-----------------------------------------------" << finl;
                  Ustar << "X\t\t| Y\t\t\t| u+\t\t\t| d+\t\t\t| u*\t\t\t| |TAUx|\t\t| |TAUy|" << finl;
                  Ustar << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                }
              if (dimension == 3)
                {
                  Ustar << "---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                  Ustar << "\tFace a\t\t\t\t\t\t\t|\t\t\t\t\t\t\t\t\t| TAU=Nu.Grad(Ut) [m2/s2]" << finl;
                  Ustar << "----------------------------------------------------------------|-----------------------------------------------------------------------|----------------------------------------------------------------" << finl;
                  Ustar << "X\t\t| Y\t\t\t| Z\t\t\t| u+\t\t\t| d+\t\t\t| u*\t\t\t| |TAUx|\t\t| |TAUy|\t\t| |TAUz|" << finl;
                  Ustar << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|----------------" << finl;
                }
            }
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              double x=zone_VEF.xv(num_face,0);
              double y=zone_VEF.xv(num_face,1);
              if (dimension == 2)
                Ustar << x << "\t| " << y;
              if (dimension == 3)
                {
                  double z=zone_VEF.xv(num_face,2);
                  Ustar << x << "\t| " << y << "\t| " << z;
                }
              Ustar << "\t| " << uplus_(num_face) << "\t| " << dplus_(num_face) << "\t| " << tab_u_star(num_face);
              Ustar << "\t| " << Cisaillement_paroi_(num_face,1) << "\t| " << Cisaillement_paroi_(num_face,0) ;
              if (dimension == 3)
                Ustar << "\t| " << Cisaillement_paroi_(num_face,2) << finl;
              else
                Ustar << finl;

              // PQ : 03/03 : Calcul des valeurs moyennes (en supposant maillage regulier)

              upmoy +=uplus_(num_face);
              dpmoy +=dplus_(num_face);
              utaumoy +=tab_u_star(num_face);
              compt +=1;
            }
          Ustar.syncfile();
        }
    }
  upmoy = mp_sum(upmoy);
  dpmoy = mp_sum(dpmoy);
  utaumoy = mp_sum(utaumoy);
  compt = mp_sum(compt);
  if (compt && je_suis_maitre())
    {
      Ustar << finl;
      Ustar << "-------------------------------------------------------------" << finl;
      Ustar << "Calcul des valeurs moyennes (en supposant maillage regulier):" << finl;
      Ustar << "<u+>= " << upmoy/compt << " <d+>= " << dpmoy/compt << " <u*>= " << utaumoy/compt << finl;
    }
  if(je_suis_maitre())
    Ustar << finl << finl;
  Ustar.syncfile();
}



