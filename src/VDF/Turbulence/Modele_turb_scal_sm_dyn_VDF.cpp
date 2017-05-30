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
// File:        Modele_turb_scal_sm_dyn_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////
#include <Modele_turb_scal_sm_dyn_VDF.h>
#include <Convection_Diffusion_std.h>
#include <Op_Dift_VDF_base.h>
#include <Operateur.h>
#include <Probleme_base.h>
#include <Milieu_base.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Turbulence_hyd_sous_maille_SMAGO_DYN_VDF.h>
#include <Param.h>
#include <Debog.h>

Implemente_instanciable(Modele_turb_scal_sm_dyn_VDF,"Modele_turbulence_scal_sous_maille_dyn_VDF",Modele_turbulence_scal_base);

//////////////////////////////////////////////////////////////////////////
Sortie& Modele_turb_scal_sm_dyn_VDF::printOn(
  Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}


//////////////////////////////////////////////////////////////////////////
Entree& Modele_turb_scal_sm_dyn_VDF::readOn(
  Entree& s )
{
  ////return Modele_turb_scal_Prandtl_sous_maille::readOn(s) ;
  dynamique_y2=0;
  return Modele_turbulence_scal_base::readOn(s) ;
}

void Modele_turb_scal_sm_dyn_VDF::set_param(Param& param)
{
  param.ajouter("DYNAMIQUE_Y2",&dynamique_y2);
  param.ajouter_non_std("STABILISE",this);
  Modele_turbulence_scal_base::set_param(param);
}


//////////////////////////////////////////////////////////////////////////
void Modele_turb_scal_sm_dyn_VDF::associer(
  const Zone_dis& zone_dis,
  const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF,zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());
}

////////////////////////////////////////////////////////////////////////


int Modele_turb_scal_sm_dyn_VDF::lire_motcle_non_standard(
  const Motcle& motlu,
  Entree& is)
{
  Motcle motlutmp;

  //dynamique_y2=0;

  Motcles les_motcles(1);
  {
    les_motcles[0] = "STABILISE";
  }
  Motcles les_motcles_stabilise(4);
  {
    les_motcles_stabilise[0] = "6_points";
    les_motcles_stabilise[1] = "plans_paralleles";
    les_motcles_stabilise[2] = "moy_euler";
    les_motcles_stabilise[3] = "moy_lagrange";
  }


  int rang2;
  int rang = les_motcles.search(motlu);
  switch(rang)
    {

    case 0:
      {
        is >> motlutmp;
        rang2 = les_motcles_stabilise.search(motlutmp);
        switch(rang2)
          {
          case 0:
            {
              methode_stabilise="6_POINTS";
              break;
            }
          case 1:
            {
              methode_stabilise="PLANS_PARALLELES";
              is >> motlutmp;
              if (motlutmp=="Nb_points")
                {
                  is >> N_c_;
                  calcul_tableaux_correspondance(N_c_, compt_c_, corresp_c_);
                }
              else
                {
                  Cerr << "le mot clef stablise plans_paralleles doit necessairement etre suivi de Nb_points !!!\n" << finl;
                  assert(0);
                  exit();
                }
              break;
            }
          case 2:
            {
              methode_stabilise="MOY_EULER";
              break;
            }
          case 3:
            {
              methode_stabilise="MOY_LAGRANGE";
              calc_elem_elem();
              break;
            }
          default :
            {
              Cerr << "Erreur a la lecture des donnees dans : " << que_suis_je() << finl;
              Cerr << motlutmp << " n'est pas compris; Les mots compris sont : " << finl;
              Cerr << les_motcles_stabilise ;
              assert(0);
              exit();
            }
          }
        break;
      }
    default :
      {
        return Modele_turbulence_scal_base::lire_motcle_non_standard(motlu,is);

      }
    }

  return 1;
}

////////////////////////////////////////////////////////////////////////
void Modele_turb_scal_sm_dyn_VDF::mettre_a_jour(double )
{
  calculer_diffusivite_turbulente();
  la_diffusivite_turbulente->valeurs().echange_espace_virtuel();
}

//////////////////////////////////////////////////////////////////////
Champ_Fonc& Modele_turb_scal_sm_dyn_VDF::calculer_diffusivite_turbulente()
{
  DoubleTab& alpha_t = la_diffusivite_turbulente.valeurs();
  Equation_base& eq_NS_turb = equation().probleme().equation(0);
  const RefObjU& modele_turbulence_hydr = eq_NS_turb.get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mod_turb_hydr = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const Champ_Fonc& champ = ref_cast(Champ_Fonc,mod_turb_hydr.viscosite_turbulente());

  Debog::verifier("la_viscosite_turbulente",champ.valeurs());
  double temps = champ.temps();
  int nb_elem_tot = la_zone_VDF->zone().nb_elem_tot();
  DoubleTab l(nb_elem_tot);
  cell_cent_vel.resize(nb_elem_tot,dimension);

  DoubleTab filt_vel(0, dimension);
  la_zone_VDF->zone().creer_tableau_elements(filt_vel);

  const DoubleTab& teta = equation().inconnue().valeurs();
  DoubleTab filt_teta;
  la_zone_VDF->zone().creer_tableau_elements(filt_teta);
  DoubleTab Lj(filt_vel);

  DoubleTab Sij_grid_scale(0, dimension, dimension);
  la_zone_VDF->zone().creer_tableau_elements(Sij_grid_scale);
  DoubleTab Sij_test_scale(Sij_grid_scale);

  DoubleTab S_test_scale_norme(nb_elem_tot);
  DoubleTab S_grid_scale_norme(nb_elem_tot);
  DoubleTab grad_teta(filt_vel);
  DoubleTab filt_grad_teta(filt_vel);
  DoubleTab Mj(nb_elem_tot,dimension);
  model_coeff.resize(nb_elem_tot);

  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_length_scale(l,la_zone_VDF.valeur());
  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_cell_cent_vel(cell_cent_vel,la_zone_VDF.valeur(),eq_NS_turb.inconnue());
  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_filter_field(cell_cent_vel,filt_vel,la_zone_VDF.valeur());
  calculer_filter_coeff(teta,filt_teta);
  calculer_Lj(teta,filt_teta,cell_cent_vel,filt_vel,Lj);
  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_Sij(Sij_grid_scale,la_zone_VDF.valeur(),la_zone_Cl_VDF.valeur(),eq_NS_turb.inconnue());
  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_Sij_vel_filt(filt_vel,Sij_test_scale,la_zone_VDF.valeur());
  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_S_norme(Sij_grid_scale,S_grid_scale_norme,la_zone_VDF->zone().nb_elem_tot());
  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_S_norme(Sij_test_scale,S_test_scale_norme,la_zone_VDF->zone().nb_elem_tot());
  calculer_grad_teta(teta,grad_teta);
  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_filter_field(grad_teta,filt_grad_teta,la_zone_VDF.valeur());
  calculer_Mj(S_grid_scale_norme,S_test_scale_norme,grad_teta,filt_grad_teta,l,Mj);
  calculer_model_coefficient(Lj,Mj);

  for (int element_number=0; element_number<nb_elem_tot; element_number++)
    alpha_t(element_number) = model_coeff(element_number)*l(element_number)*l(element_number)*S_grid_scale_norme(element_number);

  la_diffusivite_turbulente.changer_temps(temps);
  Debog::verifier("la_diffusivite_turbulente",la_diffusivite_turbulente.valeurs());
  return la_diffusivite_turbulente;
}

//////////////////////////////////////////////////////////////////////
void Modele_turb_scal_sm_dyn_VDF::calculer_filter_coeff(
  const DoubleTab& coeff,
  DoubleTab& filt_coeff)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  int nb_elem = zone_VDF.zone().nb_elem();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  int element_number;
  int num0,num1,num2,num3,num4,num5;
  int f0,f1,f2,f3,f4,f5;

  DoubleTab temp1(filt_coeff);
  DoubleTab temp2(filt_coeff);

  if (dimension ==2)
    {
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,1);
          if ( (num0 == -1) || (num2 == -1) )
            temp1(element_number)=coeff(element_number);
          else
            {
              temp1(element_number)=0.25*(coeff(num0)+
                                          2.0*coeff(element_number)+coeff(num2));
            }
        }
      temp1.echange_espace_virtuel();
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if ( (num1 == -1) || (num3 == -1) )
            filt_coeff(element_number)=temp1(element_number);
          else
            {
              filt_coeff(element_number)=0.25*(temp1(num1)+
                                               2.0*temp1(element_number)+temp1(num3));
            }
        }
      filt_coeff.echange_espace_virtuel();
    }
  else
    {
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if ( (num0 == -1) || (num3 == -1) )
            temp1(element_number)=coeff(element_number);
          else
            {
              temp1(element_number)=0.25*(coeff(num0)+
                                          2.0*coeff(element_number)+coeff(num3));
            }
        }
      temp1.echange_espace_virtuel();
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          f4 = elem_faces(element_number,4);
          num4 = face_voisins(f4,1);
          if ( (num1 == -1) || (num4 == -1) )
            temp2(element_number)=temp1(element_number);
          else
            {
              temp2(element_number)=0.25*(temp1(num1)+
                                          2.0*temp1(element_number)+temp1(num4));
            }
        }
      temp2.echange_espace_virtuel();
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,0);
          f5 = elem_faces(element_number,5);
          num5 = face_voisins(f5,1);
          if ( (num2 == -1) || (num5 == -1) )
            filt_coeff(element_number)=temp2(element_number);
          else
            {
              filt_coeff(element_number)=0.25*(temp2(num2)+
                                               2.0*temp2(element_number)+
                                               temp2(num5));
            }
        }
      filt_coeff.echange_espace_virtuel();
    }
}

//////////////////////////////////////////////////////////////////////
void Modele_turb_scal_sm_dyn_VDF::calculer_Lj(
  const DoubleTab& teta,
  const DoubleTab& filt_teta,
  const DoubleTab& vel,
  const DoubleTab& filt_vel,
  DoubleTab& Lj)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  int element_number;

  DoubleTab temp(nb_elem_tot,dimension);
  DoubleTab filt_u_teta(Lj);



  for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    {
      for (int j=0 ; j<dimension ; j++)
        temp(element_number,j)=vel(element_number,j)*teta(element_number);
    }

  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_filter_field(temp,filt_u_teta,la_zone_VDF.valeur());

  for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    {
      for (int j=0 ; j<dimension ; j++)
        Lj(element_number,j)=filt_u_teta(element_number,j)-
                             filt_vel(element_number,j)*filt_teta(element_number);
    }
}

//////////////////////////////////////////////////////////////////////
void Modele_turb_scal_sm_dyn_VDF::calculer_grad_teta(
  const DoubleVect& teta,
  DoubleTab& grad_teta)
{
  const int nb_elem = la_zone_VDF->zone().nb_elem();
  const int nb_faces_tot = la_zone_VDF->nb_faces();
  const int nb_faces_bord = la_zone_VDF->zone().nb_faces_bord();

  const Op_Dift_VDF_base& operateur_diff =  ref_cast(Op_Dift_VDF_base,(mon_equation->operateur(0)).l_op_base());
  const DoubleVect& flux_bords = operateur_diff.flux_bords();
  // Si flux_bords n'est pas encore rempli (1er pas de temps)
  // alors on le calcule en appelant la methode ajouter de l'operateur
  // de diffusion
  if (flux_bords.size_array()==0)
    {
      DoubleTab resu(la_zone_VDF->nb_faces_tot());
      operateur_diff.ajouter(ref_cast(DoubleTab,teta),resu);
    }
  const DoubleVect& faces_surfaces = la_zone_VDF->face_surfaces();
  DoubleTab grad_teta_face(nb_faces_tot);
  int element_number;

  int face, n0, n1, ori;
  const IntTab& face_voisins = la_zone_VDF->face_voisins();
  const IntVect& orientation = la_zone_VDF->orientation();
  const DoubleTab& xp = la_zone_VDF->xp();

  const Milieu_base& le_milieu=equation().probleme().milieu();
  const Champ_Don& lambda = le_milieu.conductivite();


  //Calcul du gradient aux faces

  // Boucle sur les faces de bord
  for (face=0; face<nb_faces_bord; face++)
    grad_teta_face(face)=flux_bords(face)/faces_surfaces(face);

  if ( (sub_type(Champ_Uniforme,lambda.valeur())) )
    {
      double coef = lambda(0,0);
      for (face=0; face<nb_faces_bord; face++)
        grad_teta_face(face) /= coef;
    }
  else
    {
      int num_elem;
      for (face=0; face<nb_faces_bord; face++)
        {
          num_elem=face_voisins(face,0);
          if (num_elem != -1)
            grad_teta_face(face) /= (lambda(num_elem));
          else
            {
              num_elem = face_voisins(face,1);
              grad_teta_face(face) /= (lambda(num_elem));
            }
        }
    }

  // Boucle sur les faces internes
  for (face=la_zone_VDF->premiere_face_int(); face<nb_faces_tot; face++)
    {
      n0 = face_voisins(face,0);
      n1 = face_voisins(face,1);
      ori = orientation(face);
      grad_teta_face(face) = (teta(n1)-teta(n0))/(xp(n1,ori)-xp(n0,ori));
    }
  //Calcul du gradient aux elements
  const IntTab& elem_faces = la_zone_VDF.valeur().elem_faces();
  int num0,num1,num2,num3,num4,num5;
  for (element_number=0 ; element_number<nb_elem ; element_number ++)
    {
      num0 = elem_faces(element_number,0);
      num1 = elem_faces(element_number,1);
      num2 = elem_faces(element_number,2);
      num3 = elem_faces(element_number,3);

      if (dimension == 2)
        {
          grad_teta(element_number,0)=
            (grad_teta_face[num0]+grad_teta_face[num2])/2;
          grad_teta(element_number,1)=
            (grad_teta_face[num1]+grad_teta_face[num3])/2;
        }
      else
        {
          num4 = elem_faces(element_number,4);
          num5 = elem_faces(element_number,5);
          grad_teta(element_number,0)=
            (grad_teta_face[num0]+grad_teta_face[num3])/2;
          grad_teta(element_number,1)=
            (grad_teta_face[num1]+grad_teta_face[num4])/2;
          grad_teta(element_number,2)=
            (grad_teta_face[num2]+grad_teta_face[num5])/2;
        }
    }
  grad_teta.echange_espace_virtuel();
}

//////////////////////////////////////////////////////////////////////
void Modele_turb_scal_sm_dyn_VDF::calculer_Mj(
  const DoubleTab& S_grid_scale_norme,
  const DoubleTab& S_test_scale_norme,
  const DoubleTab& grad_teta,
  const DoubleTab& filt_grad_teta,
  const DoubleTab& l,
  DoubleTab& Mj)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  int element_number;
  const double alpha=sqrt(6.0);
  int j;
  DoubleTab temp(nb_elem_tot,dimension);
  DoubleTab S_norme_filt_grad_teta(grad_teta);


  for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    for (j=0 ; j<dimension ; j++)
      temp(element_number,j)= S_grid_scale_norme(element_number) * grad_teta(element_number,j);

  Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_filter_field(temp,S_norme_filt_grad_teta,la_zone_VDF.valeur());
  for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    {
      for (j=0 ; j<dimension ; j++)
        Mj(element_number,j) = l[element_number] * l[element_number] *
                               ( S_norme_filt_grad_teta(element_number,j) - alpha * alpha * S_test_scale_norme(element_number) * filt_grad_teta(element_number,j) );
    }
}

//////////////////////////////////////////////////////////////////////
void Modele_turb_scal_sm_dyn_VDF::calculer_model_coefficient(
  const DoubleTab& Lj,
  const DoubleTab& Mj )
{
  int nb_elem_tot = la_zone_VDF->zone().nb_elem_tot();
  DoubleTab haut(nb_elem_tot);
  DoubleTab bas(nb_elem_tot);
  int i,j;

  // Evaluate the dynamic model coeficient C
  int elem;
  if (dynamique_y2 == 0)
    {
      for (elem=0; elem<nb_elem_tot; elem++)
        {
          haut(elem)=0.;
          bas(elem)=0.;
          for (j=0 ; j<dimension ; j++)
            {
              haut(elem)+=Lj(elem,j)*Mj(elem,j);
              bas(elem)+=Mj(elem,j)*Mj(elem,j);
            }
        }
    }
  else
    {
      for (elem=0; elem<nb_elem_tot; elem++)
        {
          haut(elem)=Lj(elem,1);
          bas(elem)=Mj(elem,1);
        }
    }
  stabilise_moyenne(haut,bas);

  i=0;
  j=0;
  for (elem=0; elem<nb_elem_tot; elem++)
    {
      if (model_coeff[elem] < 0.0)
        {
          model_coeff[elem]=0.0;
          i++;
        }
      else if (model_coeff[elem] > 0.5)
        {
          model_coeff[elem]=0.5;
          j++;
        }
    }
  if (i!=0)
    Cerr << i << " elements ont un coefficient inferieur a 0.0 (modele dynamique thermique)" << finl;
  else if (j!=0)
    Cerr << j << " elements ont un coefficient superieur a 0.5 (modele dynamique thermique)" << finl;
}

//////////////////////////////////////////////////////////////////////

void Modele_turb_scal_sm_dyn_VDF::stabilise_moyenne(
  const DoubleTab& haut,
  const DoubleTab& bas )
{
  if (methode_stabilise=="plans_paralleles")
    stabilise_moyenne_plans_paralleles(haut,bas);
  else if (methode_stabilise=="6_points")
    stabilise_moyenne_6_points(haut,bas);
  else if (methode_stabilise=="moy_euler")
    stabilise_moyenne_euler(haut,bas);
  else if (methode_stabilise=="moy_lagrange")
    stabilise_moyenne_lagrange(haut,bas);
  else
    {
      Cerr << "probleme dans la definition de la methode de stabilisation" << finl;
      assert(0);
      exit();
    }
}


//////////////////////////////////////////////////////////////////////

void Modele_turb_scal_sm_dyn_VDF::stabilise_moyenne_6_points(
  const DoubleTab& haut,
  const DoubleTab& bas )
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  int element_number;
  int num0,num1,num2,num3,num4,num5;
  int f0,f1,f2,f3,f4,f5;

  DoubleTab temp1(nb_elem_tot);
  DoubleTab temp2(nb_elem_tot);
  DoubleTab model_coeff_tmp(nb_elem_tot);

  // Evaluate the dynamic model coeficient C
  for (element_number=0; element_number<nb_elem_tot; element_number++)
    {
      if(dabs(bas(element_number)) < 1.e-12)
        model_coeff[element_number]=0.;
      else
        model_coeff[element_number]=haut[element_number]/bas[element_number];
    }

  // Stabilisation du modele par moyennage sur 6 noeuds

  if (dimension ==2)
    {
      for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,1);
          if ( (num0 == -1) || (num2 == -1) )
            temp1(element_number)=3.0*model_coeff(element_number);
          else
            {
              temp1(element_number)=model_coeff(num0)+
                                    model_coeff(element_number)+
                                    model_coeff(num2);
            }
        }

      for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
        {
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if ( (num1 == -1) || (num3 == -1) )
            ////model_coeff(element_number)=temp1(element_number)/3.0;
            model_coeff_tmp(element_number)=temp1(element_number)/3.0;
          else
            {
              model_coeff_tmp(element_number)=(temp1(num1)+
                                               temp1(element_number)+
                                               temp1(num3))/9.0;
            }
        }
    }
  else
    {
      for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if ( (num0 == -1) || (num3 == -1) )
            temp1(element_number)=3.0*model_coeff(element_number);
          else
            {
              temp1(element_number)=model_coeff(num0)+
                                    model_coeff(element_number)+
                                    model_coeff(num3);
            }
        }

      for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
        {
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          f4 = elem_faces(element_number,4);
          num4 = face_voisins(f4,1);
          if ( (num1 == -1) || (num4 == -1) )
            temp2(element_number)=3.0*temp1(element_number);
          else
            {
              temp2(element_number)=temp1(num1)+
                                    temp1(element_number)+
                                    temp1(num4);
            }
        }

      for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
        {
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,0);
          f5 = elem_faces(element_number,5);
          num5 = face_voisins(f5,1);
          if ( (num2 == -1) || (num5 == -1) )
            model_coeff_tmp(element_number)=temp2(element_number)/9.0;
          else
            {
              model_coeff_tmp(element_number)=(temp2(num2)+
                                               temp2(element_number)+
                                               temp2(num5))/27.0;
            }
        }
    }
  model_coeff=model_coeff_tmp;
}

//////////////////////////////////////////////////////////////////////
void Modele_turb_scal_sm_dyn_VDF::stabilise_moyenne_plans_paralleles(
  const DoubleTab& haut,
  const DoubleTab& bas )
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  int nb_elem = zone_VDF.zone().nb_elem();
  DoubleVect coeff_m_tmp(N_c_);

  // Evaluate the dynamic model coeficient C
  for (int element_number=0; element_number<nb_elem_tot; element_number++)
    {
      if(dabs(bas(element_number)) < 1.e-12)
        model_coeff[element_number]=0.;
      else
        model_coeff[element_number]=haut[element_number]/bas[element_number];
    }

  for (int j=0 ; j< N_c_ ; j++)
    coeff_m_tmp(j) =0.0;

  for (int element_number=0 ; element_number<nb_elem ; element_number ++)
    coeff_m_tmp(corresp_c_(element_number)) += model_coeff(element_number);

  for (int j=0 ; j< N_c_ ; j++)
    {
      coeff_m_tmp(j) = mp_sum(coeff_m_tmp(j));
      coeff_m_tmp(j) /= mp_sum(compt_c_(j));
    }

  for (int element_number=0 ; element_number<nb_elem ; element_number ++)
    model_coeff(element_number) = coeff_m_tmp(corresp_c_(element_number));
  model_coeff.echange_espace_virtuel();
}

//////////////////////////////////////////////////////////////////////

void Modele_turb_scal_sm_dyn_VDF::stabilise_moyenne_euler(
  const DoubleTab& haut,
  const DoubleTab& bas )
{
  int nb_0=0;
  static int init = 1;
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  static DoubleTab haut_moy(nb_elem_tot);
  static DoubleTab bas_moy(nb_elem_tot);
  double dt = mon_equation->schema_temps().pas_de_temps();
  int element_number;
  DoubleTab haut_moy_tmp(nb_elem_tot),bas_moy_tmp(nb_elem_tot);
  double eps,T;
  DoubleVect l(nb_elem_tot);

  if (init==1)
    {
      init=0;
      //                         for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
      //                                 {
      //                                         haut_moy(element_number)=haut(element_number);
      //                                         bas_moy(element_number)=bas(element_number);
      //                                 }
      haut_moy=haut;
      bas_moy=bas;
      model_coeff=0.16*0.16;
    }
  else
    {
      Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_length_scale(l,la_zone_VDF.valeur());

      for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
        {
          double a;
          a=haut_moy(element_number)*bas_moy(element_number);

          if (a>=0.)
            {
              T=1.5*l(element_number)*pow(a,-1/8);
              // T=1.5*l(element_number)*exp(-1/8*log(a));
              eps=dt/T/(1+dt/T);
            }
          else
            eps=1.;

          haut_moy_tmp(element_number)= eps*haut(element_number) +
                                        (1-eps)*haut_moy(element_number);
          if (haut_moy_tmp(element_number) < 0.0)
            {
              haut_moy_tmp(element_number)=0.0;
              nb_0++;
            }
          bas_moy_tmp(element_number)= eps*bas(element_number) +
                                       (1-eps)*bas_moy(element_number);
          if (bas_moy_tmp(element_number)!=0.)
            model_coeff(element_number) =
              haut_moy_tmp(element_number)/bas_moy_tmp(element_number);
          else
            model_coeff(element_number) =        0.16*0.16 ;

          //                                         if (( model_coeff(element_number)>= 0.0 ) & (model_coeff(element_number)<= 0.5 ) )
          //                                                         a=1.0; //on ne fait rien
          //                                         else
          //                                                 {
          //                                                         Cerr << " aie aie aie " << model_coeff(element_number) << " " << haut_moy_tmp(element_number) << " " << bas_moy_tmp(element_number) << " " << eps << finl;
          //                                                         model_coeff(element_number)=0.16*0.16;
          //                                                 }
        }
      haut_moy=haut_moy_tmp;
      bas_moy=bas_moy_tmp;
    }
  if (nb_0!=0)
    Cerr << "Dans calcul viscosite on a " << nb_0 << " elements avec un coefficient negatif" << finl;
}

//////////////////////////////////////////////////////////////////////

void Modele_turb_scal_sm_dyn_VDF::stabilise_moyenne_lagrange(
  const DoubleTab& haut,
  const DoubleTab& bas )
{
  int nb_0=0;
  static int init = 1;
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  static DoubleTab haut_moy(nb_elem_tot);
  static DoubleTab bas_moy(nb_elem_tot);
  double dt = mon_equation->schema_temps().pas_de_temps();
  int element_number;
  DoubleTab haut_moy_tmp(nb_elem_tot),bas_moy_tmp(nb_elem_tot);
  double haut_moy_int,bas_moy_int;
  DoubleVect dist(8);
  IntVect num(8);
  double eps,T;

  DoubleVect l(nb_elem_tot);



  if (init==1)
    {
      init=0;
      haut_moy=bas;
      bas_moy=bas;
      model_coeff=0.16*0.16;
    }
  else
    {
      Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_length_scale(l,la_zone_VDF.valeur());

      for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
        {
          calcul_voisins(element_number,num,dist);
          Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::interpole(num,dist,haut_moy,haut_moy_int);
          Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::interpole(num,dist,bas_moy,bas_moy_int);

          double a;
          a=haut_moy(element_number)*bas_moy(element_number);

          if (a>=0.)
            {
              T=1.5*l(element_number)*pow(a,-1/8);
              // T=1.5*l(element_number)*exp(-1/8*log(a));
              eps=dt/T/(1+dt/T);
            }
          else
            eps=1.;

          haut_moy_tmp(element_number)= eps*haut(element_number) +
                                        (1-eps)*haut_moy_int;
          if (haut_moy_tmp(element_number) < 0.0)
            {
              haut_moy_tmp(element_number)=0.0;
              nb_0++;
            }
          bas_moy_tmp(element_number)= eps*bas(element_number) +
                                       (1-eps)*bas_moy_int;
          if (bas_moy_tmp(element_number)!=0.)
            model_coeff(element_number) =
              haut_moy_tmp(element_number)/bas_moy_tmp(element_number);
          else
            model_coeff(element_number) =        0.16*0.16 ;
        }
      haut_moy=haut_moy_tmp;
      bas_moy=bas_moy_tmp;
    }
  if (nb_0!=0)
    Cerr << "Dans calcul viscosite on a " << nb_0 << " elements avec un coefficient negatif" << finl;

}


//////////////////////////////////////////////////////////////////////

void Modele_turb_scal_sm_dyn_VDF::calcul_voisins(
  const int element_number,
  IntVect& num,
  DoubleVect& dist)
{
  const Probleme_base& mon_pb = equation().probleme();
  const Equation_base& eqn_hydr = mon_pb.equation(0);
  double dt = eqn_hydr.schema_temps().pas_de_temps();
  int i,j;
  IntTab indice(3);

  if (elem_elem_(element_number,1,1,1)==-1)
    {
      for (j=0; j<8; j++)
        {
          num[j]=element_number;
          dist[j]=1.0;
        }
    }
  else
    {
      for (i=0; i<3; i++)
        {
          if (cell_cent_vel(element_number,i)>0)
            indice(i)=2;
          else
            indice(i)=0;
        }
      num[0]=elem_elem_(element_number,1,1,1);
      num[1]=elem_elem_(element_number,1,1,indice(2));
      num[2]=elem_elem_(element_number,1,indice(1),1);
      num[3]=elem_elem_(element_number,1,indice(1),indice(2));
      num[4]=elem_elem_(element_number,indice(0),1,1);
      num[5]=elem_elem_(element_number,indice(0),1,indice(2));
      num[6]=elem_elem_(element_number,indice(0),indice(1),1);
      num[7]=elem_elem_(element_number,indice(0),indice(1),indice(2));

      const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
      const DoubleTab& xp = zone_VDF.xp();
      DoubleTab x(3);
      for (i=0; i<3; i++)
        x(i)=xp(element_number,i)-cell_cent_vel(element_number,i)*dt;
      dist=0.0;
      for (j=0; j<8; j++)
        {
          for (i=0; i<3; i++)
            dist(j) += (xp(num[j],i)-x(i))*(xp(num[j],i)-x(i));
          dist(j)=sqrt(dist(j));
        }

      // verification que l'on ne se trouve pas sur un element
      double d;
      d =  (xp(num[0],0)-xp(num[4],0))*(xp(num[0],0)-xp(num[4],0));
      d += (xp(num[0],1)-xp(num[2],1))*(xp(num[0],1)-xp(num[2],1));
      d += (xp(num[0],2)-xp(num[1],2))*(xp(num[0],2)-xp(num[1],2));
      int flag=0;
      for (j=0; j<8; j++)
        if (dist(j)*dist(j)<d/10000)
          flag=1;
      if (flag==1)
        for (j=0; j<8; j++)
          {
            num[j]=element_number;
            dist[j]=1.0;
          }
    }
}

//////////////////////////////////////////////////////////////////////
/*
  void Modele_turb_scal_sm_dyn_VDF::interpole(
  const IntVect& num,
  const DoubleVect& dist,
  const DoubleVect& val,
  double& val_int)
  {
  double K=0.0;
  val_int=0.0;
  for (int i=0; i< 8; i++)
  {
  K += 1/dist(i);
  val_int += val(num(i))/dist(i) ;
  }
  val_int /= K ;
  }
*/
//////////////////////////////////////////////////////////////////////

void Modele_turb_scal_sm_dyn_VDF::calc_elem_elem(void)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  int element_number,f,elem;

  //elem_elem(element_number,i,j,k) i-x, j-y k-z et 0-avant 1-meme_position, 2-apres

  elem_elem_.resize(nb_elem_tot,3,3,3);

  for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    {
      elem_elem_(element_number,1,1,1)=element_number;  // xyz
      f = elem_faces(element_number,0);
      elem_elem_(element_number,0,1,1)=face_voisins(f,0);  // x-
      elem=elem_elem_(element_number,0,1,1);
      if (elem!=-1)
        {
          f = elem_faces(elem,1);
          elem_elem_(element_number,0,0,1)=face_voisins(f,0); // x- y-
          f = elem_faces(elem,2);
          elem_elem_(element_number,0,1,0)=face_voisins(f,0); // x- z-
          f = elem_faces(elem,4);
          elem_elem_(element_number,0,2,1)=face_voisins(f,1); // x- y+
          f = elem_faces(elem,5);
          elem_elem_(element_number,0,1,2)=face_voisins(f,1); // x- z+
        }
      f = elem_faces(element_number,3);
      elem_elem_(element_number,2,1,1)=face_voisins(f,1);  // x+
      elem=elem_elem_(element_number,2,1,1);
      if (elem!=-1)
        {
          f = elem_faces(elem,1);
          elem_elem_(element_number,2,0,1)=face_voisins(f,0); // x+ y-
          f = elem_faces(elem,2);
          elem_elem_(element_number,2,1,0)=face_voisins(f,0); // x+ z-
          f = elem_faces(elem,4);
          elem_elem_(element_number,2,2,1)=face_voisins(f,1); // x+ y+
          f = elem_faces(elem,5);
          elem_elem_(element_number,2,1,2)=face_voisins(f,1); // x+ z+
        }
      f = elem_faces(element_number,1);
      elem_elem_(element_number,1,0,1)=face_voisins(f,1);  // y-
      elem=elem_elem_(element_number,1,0,1);
      if (elem!=-1)
        {
          f = elem_faces(elem,2);
          elem_elem_(element_number,1,0,0)=face_voisins(f,0); // y- z-
          f = elem_faces(elem,5);
          elem_elem_(element_number,1,0,2)=face_voisins(f,1); // y- z+
        }
      f = elem_faces(element_number,4);
      elem_elem_(element_number,1,2,1)=face_voisins(f,1);  // y+
      elem=elem_elem_(element_number,1,2,1);
      if (elem!=-1)
        {
          f = elem_faces(elem,2);
          elem_elem_(element_number,1,2,0)=face_voisins(f,0); // y+ z-
          f = elem_faces(elem,5);

          elem_elem_(element_number,1,2,2)=face_voisins(f,1); // y+ z+
        }
      f = elem_faces(element_number,2);
      elem_elem_(element_number,1,1,0)=face_voisins(f,0);  // z-
      f = elem_faces(element_number,5);
      elem_elem_(element_number,1,1,2)=face_voisins(f,1);  // z+
      elem=elem_elem_(element_number,1,2,2);
      if (elem!=-1)
        {
          f = elem_faces(elem,0);
          elem_elem_(element_number,0,2,2)=face_voisins(f,0); // x- y+ z+
          f = elem_faces(elem,3);
          elem_elem_(element_number,2,2,2)=face_voisins(f,1); // x+ y+ z+
        }
      elem=elem_elem_(element_number,1,2,0);
      if (elem!=-1)
        {
          f = elem_faces(elem,0);
          elem_elem_(element_number,0,2,0)=face_voisins(f,0); // x- y+ z-
          f = elem_faces(elem,3);
          elem_elem_(element_number,2,2,0)=face_voisins(f,1); // x+ y+ z-
        }
      elem=elem_elem_(element_number,1,0,2);
      if (elem!=-1)
        {
          f = elem_faces(elem,0);
          elem_elem_(element_number,0,0,2)=face_voisins(f,0); // x- y- z+
          f = elem_faces(elem,3);
          elem_elem_(element_number,2,0,2)=face_voisins(f,1); // x+ y- z+
        }
      elem=elem_elem_(element_number,1,0,0);
      if (elem!=-1)
        {
          f = elem_faces(elem,0);
          elem_elem_(element_number,0,0,0)=face_voisins(f,0); // x- y- z-
          f = elem_faces(elem,3);
          elem_elem_(element_number,2,0,0)=face_voisins(f,1); // x+ y- z-
        }

      elem_elem_(element_number,1,1,1)=0;
      for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
          for (int k=0; k<3; k++)
            if (elem_elem_(element_number,i,j,k)==-1)
              elem_elem_(element_number,1,1,1)=-1;
    }


}



//////////////////////////////////////////////////////////////////////

void Modele_turb_scal_sm_dyn_VDF::calcul_tableaux_correspondance(
  int& N_c,
  IntVect& compt_c,
  IntVect& corresp_c)
{
  // Initialisation de : Yuv + compt_c + corresp_c
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& xp = zone_VDF.xp();
  int nb_elems = zone_VDF.zone().nb_elem();
  DoubleTab Y_c ;
  int num_elem,j,indic_c,trouve;
  double y;
  j=0;
  indic_c = 0;

  // dimensionnement aux valeurs rentrees dans le jeu de donnees
  Y_c.resize(N_c);
  compt_c.resize(N_c);
  corresp_c.resize(nb_elems);

  // initialisation
  Y_c = -100.;
  compt_c = 0;
  corresp_c = -1;

  for (num_elem=0; num_elem<nb_elems; num_elem++)
    {
      y = xp(num_elem,1);
      trouve = 0;
      for (j=0; j<indic_c+1; j++)
        {
          if(j==N_c)
            {
              Cerr << "nb_points=" << N_c << " est trop petit pour le nombre de plans paralleles." << finl;
              Cerr << "Augmenter cette valeur." << finl;
              exit();
            }
          if(y==Y_c[j])
            {
              corresp_c[num_elem] = j;
              compt_c[j] ++;
              j=indic_c+1;
              trouve = 1;
              break;
            }
        }
      if (trouve==0)
        {
          corresp_c[num_elem] = indic_c;
          Y_c[indic_c]=y;
          compt_c[indic_c] ++;
          indic_c++;
        }
    }
  N_c = indic_c;  // nombre de y pour les elements
  compt_c.resize(N_c);
}

