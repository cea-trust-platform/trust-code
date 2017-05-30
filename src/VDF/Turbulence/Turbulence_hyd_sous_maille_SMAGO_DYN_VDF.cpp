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
// File:        Turbulence_hyd_sous_maille_SMAGO_DYN_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/23
//
//////////////////////////////////////////////////////////////////////////////
#include <Turbulence_hyd_sous_maille_SMAGO_DYN_VDF.h>
#include <Zone.h>
#include <DoubleTrav.h>
#include <Champ_Face.h>
#include <Champ_Fonc_P0_VDF.h>
#include <Debog.h>
#include <IntTrav.h>
#include <SFichier.h>
#include <Schema_Temps_base.h>
#include <stat_counters.h>
#include <Param.h>
#include <Zone_VDF.h>
#include <Equation_base.h>
#include <Zone_Cl_VDF.h>

Implemente_instanciable_sans_constructeur(Turbulence_hyd_sous_maille_SMAGO_DYN_VDF,"Modele_turbulence_hyd_sous_maille_SMAGO_DYN_VDF",Turbulence_hyd_sous_maille_Smago_VDF);


Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::Turbulence_hyd_sous_maille_SMAGO_DYN_VDF()
{
  /*
    Noms& nom=champs_compris_.liste_noms_compris();
    nom.dimensionner(1);
    nom[0]="dynamic_coefficient";
  */
}

//////////////////////////////////////////////////////////////////////
Sortie& Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << " " << le_nom();
}

//////////////////////////////////////////////////////////////////////
Entree& Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::readOn(Entree& s )
{
  return Turbulence_hyd_sous_maille_Smago_VDF::readOn(s) ;
}

void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::set_param(Param& param)
{
  Turbulence_hyd_sous_maille_Smago_VDF::set_param(param);
  param.ajouter_non_std("stabilise",(this),Param::REQUIRED);
}

int Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlutmp;
  if (mot=="stabilise")
    {
      Motcles les_motcles(4);
      {
        les_motcles[0] = "6_points";
        les_motcles[1] = "plans_paralleles";
        les_motcles[2] = "moy_euler";
        les_motcles[3] = "moy_lagrange";
      }

      is >> motlutmp;
      int rang = les_motcles.search(motlutmp);
      switch(rang)
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
                Cerr << "The keywords stablise plans_paralleles must be followed by the keyword Nb_points.\n" << finl;
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
            Cerr << "Error while reading : " << que_suis_je() << finl;
            Cerr << motlutmp << " is not understood. Available keywords are : " << finl;
            Cerr << les_motcles ;
            assert(0);
            exit();
          }
        }
      return 1;
    }
  else
    return Turbulence_hyd_sous_maille_Smago_VDF::lire_motcle_non_standard(mot,is);
  return 1;
}

void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::associer(
  const Zone_dis& zone_dis,
  const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF,zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF,zone_Cl_dis.valeur());

  Cerr << "Discretisation de coeff_field" << finl;
  Cerr << "coeff_field discretization" << finl;
  coeff_field.typer("Champ_Fonc_P0_VDF");
  Champ_Fonc_P0_VDF& coeff=ref_cast(Champ_Fonc_P0_VDF, coeff_field.valeur());
  coeff.associer_zone_dis_base(la_zone_VDF.valeur());
  coeff.nommer("dynamic_coefficient");
  coeff.fixer_nb_comp(1);
  coeff.fixer_nb_valeurs_nodales(la_zone_VDF->nb_elem());
  coeff.fixer_unite("adim");
  coeff.changer_temps(0.);

  champs_compris_.ajoute_champ(coeff_field);
}

//////////////////////////////////////////////////////////////////////
int Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::preparer_calcul()
{
  Mod_turb_hyd_ss_maille::preparer_calcul();
  mettre_a_jour(0.);
  return 1;
}

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::mettre_a_jour(double temps)
{
  statistiques().begin_count(nut_counter_);

  int nb_elem_tot = la_zone_VDF->zone().nb_elem_tot();
  DoubleTab Sij_test_scale(0,dimension,dimension);
  la_zone_VDF->zone().creer_tableau_elements(Sij_test_scale);
  DoubleTrav Sij_grid_scale(Sij_test_scale);

  DoubleTrav S_test_scale_norme(nb_elem_tot);
  DoubleTrav S_grid_scale_norme(nb_elem_tot);
  cell_cent_vel_.resize(nb_elem_tot,dimension);

  DoubleTab filt_vel(0,dimension);
  la_zone_VDF->zone().creer_tableau_elements(filt_vel);
  DoubleTrav Lij(Sij_test_scale);
  DoubleTrav Mij(Sij_test_scale);
  DoubleTrav l(nb_elem_tot);

  calculer_length_scale(l,la_zone_VDF.valeur());
  calculer_cell_cent_vel(cell_cent_vel_,la_zone_VDF.valeur(),mon_equation->inconnue());
  calculer_filter_field(cell_cent_vel_,filt_vel,la_zone_VDF.valeur());
  calculer_Lij(cell_cent_vel_,filt_vel,Lij);

  calculer_Sij(Sij_grid_scale,la_zone_VDF.valeur(),la_zone_Cl_VDF.valeur(),mon_equation->inconnue());
  calculer_Sij_vel_filt(filt_vel,Sij_test_scale,la_zone_VDF.valeur());

  calculer_S_norme(Sij_grid_scale,S_grid_scale_norme,la_zone_VDF->zone().nb_elem_tot());
  calculer_S_norme(Sij_test_scale,S_test_scale_norme,la_zone_VDF->zone().nb_elem_tot());

  calculer_Mij(Sij_grid_scale,Sij_test_scale,l,Mij);
  Debog::verifier("Lij",Lij);
  calculer_model_coefficient(Lij,Mij);
  Debog::verifier("model_coeff",coeff_field);
  calculer_viscosite_turbulente(S_grid_scale_norme,l);
  calculer_energie_cinetique_turb(S_grid_scale_norme,l);
  loipar->calculer_hyd(la_viscosite_turbulente,energie_cinetique_turbulente());
  limiter_viscosite_turbulente();
  controler_grandeurs_turbulentes();
  coeff_field.changer_temps(temps);
  la_viscosite_turbulente.valeurs().echange_espace_virtuel();
  Debog::verifier("la_viscosite_turbulente",la_viscosite_turbulente.valeurs());

  statistiques().end_count(nut_counter_);
}


//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_cell_cent_vel(
  DoubleTab& cell_cent_vel, const Zone_VDF& zone_VDF, Champ_Inc& inco)
{
  const DoubleTab& vitesse = inco.valeurs();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  int element_number;
  int num0,num1,num2,num3,num4=-1,num5=-1;

  // This is to calculate the cell centered velocity values

  for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    {
      num0 = elem_faces(element_number,0);
      num1 = elem_faces(element_number,1);
      num2 = elem_faces(element_number,2);
      num3 = elem_faces(element_number,3);
      if (dimension == 3)
        {
          num4 = elem_faces(element_number,4);
          num5 = elem_faces(element_number,5);
        }

      if (dimension == 2)
        {
          cell_cent_vel(element_number,0)=0.5*(vitesse[num0]+vitesse[num2]);
          cell_cent_vel(element_number,1)=0.5*(vitesse[num1]+vitesse[num3]);
        }
      else
        {
          cell_cent_vel(element_number,0)=0.5*(vitesse[num0]+vitesse[num3]);
          cell_cent_vel(element_number,1)=0.5*(vitesse[num1]+vitesse[num4]);
          cell_cent_vel(element_number,2)=0.5*(vitesse[num2]+vitesse[num5]);
        }
    }
}

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_filter_field(
  const DoubleTab& in_vel,
  DoubleTab& out_vel,
  const Zone_VDF& zone_VDF)
{
  const IntTab& face_voisins = zone_VDF.face_voisins();
  int nb_elem = zone_VDF.zone().nb_elem();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  int element_number;
  int num0,num1,num2,num3,num4,num5;
  int f0,f1,f2,f3,f4,f5;
  int i;

  DoubleTrav temp1(out_vel);
  DoubleTrav temp2(out_vel);

  // This is to calculate the filtered velocity field

  if (dimension ==2)
    {
      for (element_number=0 ; element_number<nb_elem ; element_number++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,1);
          if ( (num0 == -1) || (num2 == -1) )
            for (i=0 ; i<dimension ; i++)
              temp1(element_number,i)=in_vel(element_number,i);
          else
            {
              for (i=0 ; i<dimension ; i++)
                {
                  temp1(element_number,i)=0.25*(in_vel(num0,i)+
                                                2.0*in_vel(element_number,i)+in_vel(num2,i));
                }
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
            for (i=0 ; i<dimension ; i ++)
              out_vel(element_number,i)=temp1(element_number,i);
          else
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  out_vel(element_number,i)=0.25*(temp1(num1,i)+
                                                  2.0*temp1(element_number,i)+temp1(num3,i));
                }
            }
        }
      out_vel.echange_espace_virtuel();
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
            for (i=0 ; i<dimension ; i ++)
              temp1(element_number,i)=in_vel(element_number,i);
          else
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  temp1(element_number,i)=0.25*(in_vel(num0,i)+
                                                2.0*in_vel(element_number,i)+in_vel(num3,i));
                }
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
            for (i=0 ; i<dimension ; i ++)
              temp2(element_number,i)=temp1(element_number,i);
          else
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  temp2(element_number,i)=0.25*(temp1(num1,i)+
                                                2.0*temp1(element_number,i)+temp1(num4,i));
                }
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
            for (i=0 ; i<dimension ; i ++)
              out_vel(element_number,i)=temp2(element_number,i);
          else
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  out_vel(element_number,i)=0.25*(temp2(num2,i)+
                                                  2.0*temp2(element_number,i)+
                                                  temp2(num5,i));
                }
            }
        }
      out_vel.echange_espace_virtuel();
    }
}


//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_filter_tensor(
  DoubleTab& in_vel)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  int nb_elem = zone_VDF.zone().nb_elem();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  int element_number;
  int num0,num1,num2,num3,num4,num5;
  int f0,f1,f2,f3,f4,f5;
  int i,j;

  DoubleTrav temp1(in_vel);
  DoubleTrav temp2(in_vel);

  // This is to calculate the filtered tensor field

  if (dimension ==2)
    {
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,1);
          if ( (num0 == -1) || (num2 == -1) )
            for (i=0 ; i<dimension ; i ++)
              for (j=0 ; j<dimension ; j ++)
                temp1(element_number,i,j)=in_vel(element_number,i,j);
          else
            {
              for (i=0 ; i<dimension ; i ++)
                for (j=0 ; j<dimension ; j ++)
                  {
                    temp1(element_number,i,j)=0.25*(in_vel(num0,i,j)+                                                    2.0*in_vel(element_number,i,j)+in_vel(num2,i,j));
                  }
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
            for (i=0 ; i<dimension ; i ++)
              for (j=0 ; j<dimension ; j ++)
                in_vel(element_number,i,j)=temp1(element_number,i,j);
          else
            {
              for (i=0 ; i<dimension ; i ++)
                for (j=0 ; j<dimension ; j ++)
                  {
                    in_vel(element_number,i,j)=0.25*(temp1(num1,i,j)+                                                      2.0*temp1(element_number,i,j)+temp1(num3,i,j));
                  }
            }
        }
      in_vel.echange_espace_virtuel();
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
            for (i=0 ; i<dimension ; i ++)
              for (j=0 ; j<dimension ; j ++)
                temp1(element_number,i,j)=in_vel(element_number,i,j);
          else
            {
              for (i=0 ; i<dimension ; i ++)
                for (j=0 ; j<dimension ; j ++)
                  {
                    temp1(element_number,i,j)=0.25*(in_vel(num0,i,j)+                                                    2.0*in_vel(element_number,i,j)+in_vel(num3,i,j));
                  }
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
            for (i=0 ; i<dimension ; i ++)
              for (j=0 ; j<dimension ; j ++)
                temp2(element_number,i,j)=temp1(element_number,i,j);
          else
            {
              for (i=0 ; i<dimension ; i ++)
                for (j=0 ; j<dimension ; j ++)
                  {
                    temp2(element_number,i,j)=0.25*(temp1(num1,i,j)+                                                    2.0*temp1(element_number,i,j)+temp1(num4,i,j));
                  }
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
            for (i=0 ; i<dimension ; i ++)
              for (j=0 ; j<dimension ; j ++)
                in_vel(element_number,i,j)=temp2(element_number,i,j);
          else
            {
              for (i=0 ; i<dimension ; i ++)
                for (j=0 ; j<dimension ; j ++)
                  {
                    in_vel(element_number,i,j)=0.25*(temp2(num2,i,j)+                                                      2.0*temp2(element_number,i,j)+temp2(num5,i,j));
                  }
            }
        }
      in_vel.echange_espace_virtuel();
    }
}

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_Lij(
  const DoubleTab& cell_cent_vel,
  const DoubleTab& filt_vel,
  DoubleTab& Lij)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  int element_number;

  DoubleTab uij_filt(Lij);

  // This is to calculate the Lij term for the C coefficient

  for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    {
      for (int i=0 ; i<dimension ; i++)
        for (int j=0 ; j<dimension ; j++)
          uij_filt(element_number,i,j)=cell_cent_vel(element_number,i)*
                                       cell_cent_vel(element_number,j);
    }
  calculer_filter_tensor(uij_filt);

  for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    {
      for (int i=0 ; i<dimension ; i++)
        for (int j=0 ; j<dimension ; j++)
          Lij(element_number,i,j)=uij_filt(element_number,i,j)-
                                  filt_vel(element_number,i)*filt_vel(element_number,j);
    }
}


//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_Mij(
  const DoubleTab& Sij_grid_scale,
  const DoubleTab& Sij_test_scale,
  const DoubleVect& l,
  DoubleTab& Mij)
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();

  DoubleTrav Sij_grid_scale_norme(nb_elem_tot);
  DoubleTrav Sij_test_scale_norme(nb_elem_tot);
  DoubleTrav S_norme_Sij_filt(Mij);
  const double alpha=sqrt(6.0);

  // This is to calculate the Mij term for the C coefficient

  calculer_S_norme(Sij_grid_scale,Sij_grid_scale_norme,la_zone_VDF->zone().nb_elem_tot());
  calculer_S_norme(Sij_test_scale,Sij_test_scale_norme,la_zone_VDF->zone().nb_elem_tot());
  for (int element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    for (int i=0 ; i<dimension ; i++)
      for (int j=0 ; j<dimension ; j++)
        S_norme_Sij_filt(element_number,i,j) = Sij_grid_scale_norme(element_number) * Sij_grid_scale(element_number,i,j);

  calculer_filter_tensor(S_norme_Sij_filt);

  for (int element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    for (int i=0 ; i<dimension ; i++)
      for (int j=0 ; j<dimension ; j++)
        Mij(element_number,i,j) = 2 * l[element_number] * l[element_number] *
                                  ( S_norme_Sij_filt(element_number,i,j) - alpha*alpha*Sij_test_scale_norme(element_number)*Sij_test_scale(element_number,i,j) );
}

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_model_coefficient(
  const DoubleTab& Lij,
  const DoubleTab& Mij)
{
  DoubleVect& model_coeff = coeff_field;
  int nb_elem_tot = la_zone_VDF->zone().nb_elem_tot();
  DoubleTab haut, bas;
  la_zone_VDF->zone().creer_tableau_elements(haut);
  la_zone_VDF->zone().creer_tableau_elements(bas);
  Debog::verifier("bas",bas);
  Debog::verifier("haut",haut);
  // Evaluate the dynamic model coeficient C
  for (int elem=0; elem<nb_elem_tot; elem++)
    {
      haut(elem)=0.;
      bas(elem)=0.;
      for (int i=0 ; i<dimension ; i++)
        for (int j=0 ; j<dimension ; j++)
          {
            haut(elem)+=Lij(elem,i,j)*Mij(elem,i,j);
            bas(elem)+=Mij(elem,i,j)*Mij(elem,i,j);
          }
    }

  stabilise_moyenne(haut,bas);

  int i=0,j=0;
  for (int elem=0; elem<nb_elem_tot; elem++)
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
    Cerr << i << " elements ont un coefficient inferieur a 0.0 (modele dynamique)" << finl;
  else if (j!=0)
    Cerr << j << " elements ont un coefficient superieur a 0.5 (modele dynamique)" << finl;
}

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_length_scale(
  DoubleVect& l, const Zone_VDF& zone_VDF)
{
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntVect& orientation = zone_VDF.orientation();
  int elem;

  // calcul de la longueur caracteristique de l'element

  for(elem=0 ; elem<nb_elem_tot ; elem ++)
    {
      double l_int;
      if (dimension==3)
        {
          l_int = zone_VDF.dim_elem(elem,orientation(elem_faces(elem,0)));
          l_int *= zone_VDF.dim_elem(elem,orientation(elem_faces(elem,1)));
          l_int *= zone_VDF.dim_elem(elem,orientation(elem_faces(elem,2)));
          l(elem)=exp((log(l_int)/3));
        }
      else
        {
          l_int = zone_VDF.dim_elem(elem,orientation(elem_faces(elem,0)));
          l_int *= zone_VDF.dim_elem(elem,orientation(elem_faces(elem,1)));
          l[elem]=sqrt(l_int);
        }
    }
}

//////////////////////////////////////////////////////////////////////
Champ_Fonc& Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_viscosite_turbulente(
  const DoubleVect& SMA_barre,
  const DoubleVect& l)
{



  DoubleVect& model_coeff = coeff_field;
  double temps = mon_equation->inconnue().temps();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  int nb_elem = la_zone_VDF->zone().nb_elem();

  if (visco_turb.size() != la_zone_VDF->zone().nb_elem())
    {
      Cerr << "Size error for the array containing the values of the turbulent viscosity." << finl;
      exit();
    }

  Debog::verifier("Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_viscosite_turbulente visco_turb 0",visco_turb);

  for (int elem=0; elem<nb_elem; elem++)
    visco_turb[elem]=model_coeff[elem]*l[elem]*l[elem]*sqrt(SMA_barre[elem]);

  Debog::verifier("Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_viscosite_turbulente visco_turb 1",visco_turb);

  la_viscosite_turbulente.changer_temps(temps);
  return la_viscosite_turbulente;
}

//////////////////////////////////////////////////////////////////////

Champ_Fonc& Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_viscosite_turbulente()
{
  return la_viscosite_turbulente;
}

Champ_Fonc& Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_energie_cinetique_turb(
  const DoubleVect& SMA_barre,
  const DoubleVect& l)
{
  DoubleVect& model_coeff = coeff_field;
  double temps = mon_equation->inconnue().temps();
  DoubleVect& k = energie_cinetique_turb_.valeurs();
  int nb_elem_tot = la_zone_VDF->zone().nb_elem_tot();
  static double co_mu = 1./(0.094*0.094);

  for (int elem=0 ; elem<nb_elem_tot; elem++)
    {
      k[elem]=co_mu*model_coeff[elem]*model_coeff[elem]*l[elem]*l[elem]*SMA_barre[elem];
    }

  energie_cinetique_turb_.changer_temps(temps);
  return energie_cinetique_turb_;
}

void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_energie_cinetique_turb()
{

}
//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::controler_grandeurs_turbulentes()
{
  static const double Cmu = CMU;
  int nb_elem_tot = la_zone_VDF->zone().nb_elem_tot();
  DoubleTab& visco_turb = la_viscosite_turbulente.valeurs();
  DoubleVect& energie_turb = energie_cinetique_turb_.valeurs();
  double k,eps;

  for (int elem=0; elem<nb_elem_tot; elem++)
    {
      k = energie_turb[elem];
      if (k <= 1.e-10)
        energie_turb[elem] = 0.;
      else
        {
          eps = Cmu*k*k/(visco_turb[elem]+DMINFLOAT);
          if (eps < 1.e-15)
            visco_turb[elem] = 0;
        }
    }
}


//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_S_norme(
  const DoubleTab& Sij,
  DoubleVect& S_norme,
  int nb_elem_tot)
{
  S_norme*=0.0;
  for (int element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    for (int i=0 ; i<dimension ; i++)
      for (int j=0 ; j<dimension ; j++)
        S_norme(element_number)
        += Sij(element_number,i,j)*Sij(element_number,i,j);
  for (int element_number=0 ; element_number<nb_elem_tot ; element_number ++)
    S_norme(element_number)=sqrt(2*S_norme(element_number));
}

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_Sij(
  DoubleTab& Sij, const Zone_VDF& zone_VDF, const Zone_Cl_VDF& zone_Cl_VDF, Champ_Inc& inco)
{
  Champ_Face& vit = ref_cast(Champ_Face, inco.valeur());
  const DoubleTab& vitesse = inco.valeurs();
  const int nb_elem_tot = zone_VDF.nb_elem_tot();
  const int nb_elem = zone_VDF.nb_elem();
  DoubleTab gij(nb_elem_tot,dimension,dimension);

  vit.calcul_duidxj(vitesse,gij,zone_Cl_VDF);
  // Calcul de Sij
  for(int elem=0 ; elem<nb_elem ; elem ++)
    for(int i=0 ; i<dimension ; i++)
      for(int j=0 ; j<dimension ; j ++)
        Sij(elem,i,j)=0.5*(gij(elem,i,j)+gij(elem,j,i));
  Sij.echange_espace_virtuel();
}


//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calculer_Sij_vel_filt(
  const DoubleTab& in_vel,
  DoubleTab& out_vel,
  const Zone_VDF& zone_VDF)
{
  int nb_elem = zone_VDF.zone().nb_elem();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& elem_faces = zone_VDF.elem_faces();

  int element_number;
  int num0,num1,num2,num3,num4,num5;
  int f0,f1,f2,f3,f4,f5;

  if (dimension ==2)
    {
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          if (num0 == -1)
            num0 = element_number;
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          if (num1 == -1)
            num1 = element_number;
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,1);
          if (num2 == -1)
            num2 = element_number;
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if (num3 == -1)
            num3 = element_number;
          out_vel(element_number,0,0)=0.5*((in_vel(num2,0)-in_vel(num0,0))/
                                           zone_VDF.dim_elem(element_number,0));
          out_vel(element_number,0,1)=out_vel(element_number,1,0)=0.25*
                                                                  ((in_vel(num3,0)-in_vel(num1,0))/
                                                                   zone_VDF.dim_elem(element_number,1)+
                                                                   (in_vel(num2,1)-in_vel(num0,1))/
                                                                   zone_VDF.dim_elem(element_number,0));
          out_vel(element_number,1,1)=0.5*((in_vel(num3,1)-in_vel(num1,1))/
                                           zone_VDF.dim_elem(element_number,1));
        }
    }
  else
    {
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          if (num0 == -1)
            num0 = element_number;
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          if (num1 == -1)
            num1 = element_number;
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,0);
          if (num2 == -1)
            num2 = element_number;
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if (num3 == -1)
            num3 = element_number;
          f4 = elem_faces(element_number,4);
          num4 = face_voisins(f4,1);
          if (num4 == -1)
            num4 = element_number;
          f5 = elem_faces(element_number,5);
          num5 = face_voisins(f5,1);
          if (num5 == -1)
            num5 = element_number;
          out_vel(element_number,0,0)=0.5*((in_vel(num3,0)-in_vel(num0,0))/
                                           zone_VDF.dim_elem(element_number,0));
          out_vel(element_number,0,1)=out_vel(element_number,1,0)=0.25*
                                                                  ((in_vel(num4,0)-in_vel(num1,0))/
                                                                   zone_VDF.dim_elem(element_number,1)+
                                                                   (in_vel(num3,1)-in_vel(num0,1))/
                                                                   zone_VDF.dim_elem(element_number,0));
          out_vel(element_number,0,2)=out_vel(element_number,2,0)=0.25*
                                                                  ((in_vel(num5,0)-in_vel(num2,0))/
                                                                   zone_VDF.dim_elem(element_number,2)+
                                                                   (in_vel(num3,2)-in_vel(num0,2))/
                                                                   zone_VDF.dim_elem(element_number,0));
          out_vel(element_number,1,1)=0.5*((in_vel(num4,1)-in_vel(num1,1))/
                                           zone_VDF.dim_elem(element_number,1));
          out_vel(element_number,1,2)=out_vel(element_number,2,1)=0.25*
                                                                  ((in_vel(num5,1)-in_vel(num2,1))/
                                                                   zone_VDF.dim_elem(element_number,2)+
                                                                   (in_vel(num4,2)-in_vel(num1,2))/
                                                                   zone_VDF.dim_elem(element_number,1));
          out_vel(element_number,2,2)=0.5*((in_vel(num5,2)-in_vel(num2,2))/
                                           zone_VDF.dim_elem(element_number,2));
        }
    }
  out_vel.echange_espace_virtuel();
}


//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::stabilise_moyenne(
  const DoubleTab& haut,
  const DoubleTab& bas )
{
  if (methode_stabilise=="plans_paralleles")
    stabilise_moyenne_plans_paralleles(haut,bas);
  else if (methode_stabilise=="6_points")
    stabilise_moyenne_6_points(haut,bas);
  else if (methode_stabilise=="moy_euler" || methode_stabilise=="moy_lagrange")
    stabilise_moyenne_euler_lagrange(haut,bas);
  else
    {
      Cerr << "Problem with the definition of the stabilisation method." << finl;
      assert(0);
      exit();
    }
}

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::stabilise_moyenne_6_points(
  const DoubleTab& haut,
  const DoubleTab& bas )
{
  DoubleVect& model_coeff = coeff_field;
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  int nb_elem = zone_VDF.zone().nb_elem();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  int element_number;
  int num0,num1,num2,num3,num4,num5;
  int f0,f1,f2,f3,f4,f5;

  DoubleTrav temp1(model_coeff);
  DoubleTrav temp2(model_coeff);
  // Evaluate the dynamic model coeficient C
  for (element_number=0; element_number<nb_elem_tot; element_number++)
    {
      if(dabs(bas(element_number)) < 1.e-12)
        model_coeff[element_number]=0.;
      else
        model_coeff[element_number]=haut[element_number]/bas[element_number];
    }
  // Stabilisation du modele par moyennage sur 6 noeuds
  if (dimension==2)
    {
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
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
      temp1.echange_espace_virtuel();
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if ( (num1 == -1) || (num3 == -1) )
            model_coeff(element_number)=temp1(element_number)/3.0;
          else
            {
              model_coeff(element_number)=(temp1(num1)+
                                           temp1(element_number)+
                                           temp1(num3))/9.0;
            }
        }
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
            temp1(element_number)=3.0*model_coeff(element_number);
          else
            {
              temp1(element_number)=model_coeff(num0)+
                                    model_coeff(element_number)+
                                    model_coeff(num3);
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
            temp2(element_number)=3.0*temp1(element_number);
          else
            {
              temp2(element_number)=temp1(num1)+
                                    temp1(element_number)+
                                    temp1(num4);
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
            model_coeff(element_number)=temp2(element_number)/9.0;
          else
            {
              model_coeff(element_number)=(temp2(num2)+
                                           temp2(element_number)+
                                           temp2(num5))/27.0;
            }
        }
    }
  model_coeff.echange_espace_virtuel();
}

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::stabilise_moyenne_plans_paralleles(
  const DoubleTab& haut,
  const DoubleTab& bas )
{
  DoubleVect& model_coeff = coeff_field;
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  int nb_elem = zone_VDF.zone().nb_elem();
  int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  DoubleVect model_coeff_tmp(N_c_);

  // Evaluate the dynamic model coeficient C
  for (int element_number=0; element_number<nb_elem_tot; element_number++)
    {
      if(dabs(bas(element_number)) < 1.e-12)
        model_coeff[element_number]=0.;
      else
        model_coeff[element_number]=haut[element_number]/bas[element_number];
    }
  Debog::verifier("model_coeff apres haut/bas",model_coeff);
  Debog::verifier("corresp_c",corresp_c_);
  for (int j=0 ; j< N_c_ ; j++)
    model_coeff_tmp(j) =0.0;

  for (int element_number=0 ; element_number<nb_elem ; element_number ++)
    model_coeff_tmp(corresp_c_(element_number)) += model_coeff(element_number);

  for (int j=0 ; j< N_c_ ; j++)
    {
      model_coeff_tmp(j) = mp_sum(model_coeff_tmp(j));
      model_coeff_tmp(j) /= mp_sum(compt_c_(j));
    }
  for (int element_number=0 ; element_number<nb_elem ; element_number ++)
    model_coeff(element_number) = model_coeff_tmp(corresp_c_(element_number));
  model_coeff.echange_espace_virtuel();
}

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::stabilise_moyenne_euler_lagrange(
  const DoubleTab& haut,
  const DoubleTab& bas
)
{
  static int init = 1;
  // Attention: on stocke ces deux tableaux d'un appel a l'autre:
  static DoubleTrav haut_moy;
  static DoubleTrav bas_moy;

  DoubleVect& model_coeff = coeff_field.valeur().valeurs();

  if (init==1)
    {
      init=0;
      haut_moy=haut;
      bas_moy=bas;
      model_coeff = 0.16*0.16;
    }
  else
    {
      const int lagrange = (methode_stabilise=="moy_lagrange");

      int nb_0=0;
      DoubleVect l;
      la_zone_VDF.valeur().zone().creer_tableau_elements(l);
      calculer_length_scale(l, la_zone_VDF.valeur());

      const int nmax = haut.size_totale();
      const double dt = mon_equation->schema_temps().pas_de_temps();

      DoubleVect dist(8);
      IntVect num(8);

      for (int elem=0 ; elem<nmax ; elem ++)
        {
          const double hmoy = haut_moy(elem);
          const double bmoy = bas_moy(elem);
          double hmoy_int, bmoy_int;
          if (lagrange)
            {
              calcul_voisins(elem,num,dist);
              interpole(num,dist,haut_moy,hmoy_int);
              interpole(num,dist,bas_moy,bmoy_int);
            }
          else
            {
              hmoy_int = hmoy;
              bmoy_int = bmoy;
            }
          const double h = haut(elem);
          const double b = bas(elem);
          const double l_elem = l(elem);
          const double a = hmoy * bmoy;
          double eps;
          if (a>=0.)
            {
              double T=1.5 * l_elem * pow(a,-1/8);
              // T=1.5*l(elem)*exp(-1/8*log(a));
              double dt_T = dt / T;
              eps = dt_T / (1. + dt / T);
            }
          else
            {
              eps = 1.;
            }

          double hmoy_new = eps * h + (1-eps) * hmoy_int;
          if (hmoy_new < 0.)
            {
              hmoy_new = 0.0;
              nb_0++;
            }
          double bmoy_new = eps * b + (1-eps) * bmoy_int;
          double model_c;
          if (bmoy_new != 0.)
            model_c = hmoy_new / bmoy_new;
          else
            model_c = 0.16*0.16 ;

          model_coeff(elem) = model_c;
          haut_moy(elem) = hmoy_new;
          bas_moy(elem) = bmoy_new;

          //                                         if (( model_coeff(elem)>= 0.0 ) & (model_coeff(elem)<= 0.5 ) )
          //                                                         a=1.0; //on ne fait rien
          //                                         else
          //                                                 {
          //                                                         Cerr << " aie aie aie " << model_coeff(elem) << " " << haut_moy_tmp(elem) << " " << bas_moy_tmp(element_number) << " " << eps << finl;
          //                                                         model_coeff(elem)=0.16*0.16;
          //                                                 }
        }
      if (nb_0!=0)
        {
          Cerr << "Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::stabilise_moyenne_euler_lagrange"<< finl;
          Cerr << nb_0 << " elements have a negative coefficient." << finl;
        }
    }
}

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calcul_voisins(
  const int element_number,
  IntVect& num,
  DoubleVect& dist)
{
  double dt = mon_equation->schema_temps().pas_de_temps();
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
          if (cell_cent_vel_(element_number,i)>0)
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
      DoubleTrav x(3);
      for (i=0; i<3; i++)
        x(i)=xp(element_number,i)-cell_cent_vel_(element_number,i)*dt;
      dist=0.0;
      for (j=0; j<8; j++)
        {
          for (i=0; i<3; i++)
            {
              if (num[j]==-1)
                {
                  Cerr << "Problem with the algorithm Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calcul_voisins" << finl;
                  exit();
                }
              dist(j) += (xp(num[j],i)-x(i))*(xp(num[j],i)-x(i));
            }
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
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::interpole(
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

//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calc_elem_elem()
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

      //Version initiale, boucles ?????
      /*
        for (int i=0; i<3; i++)
        for (int j=0; i<3; i++)
        for (int k=0; i<3; i++)
        if (elem_elem(element_number,i,j,k)==-1)
        elem_elem(element_number,1,1,1)=-1;
      */
      //A remplacer par ????

      for (int i=0; i<3; i++)
        for (int j=0; j<3; j++)
          for (int k=0; k<3; k++)
            if (elem_elem_(element_number,i,j,k)==-1)
              elem_elem_(element_number,1,1,1)=-1;


    }


}
//////////////////////////////////////////////////////////////////////
void Turbulence_hyd_sous_maille_SMAGO_DYN_VDF::calcul_tableaux_correspondance(
  int& N_c,
  IntVect& compt_c,
  IntVect& corresp_c)
{
  // Initialisation de : Yuv + compt_c + corresp_c
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& xp = zone_VDF.xp();
  int nb_elems = zone_VDF.zone().nb_elem();
  DoubleTrav Y_c ;
  int num_elem,j,indic_c,trouve;
  double y;
  j=0;
  indic_c = 0;

  // dimensionnement aux valeurs rentrees dans le jeu de donnees
  Y_c.resize(N_c);
  compt_c.resize(N_c);
  zone_VDF.zone().creer_tableau_elements(corresp_c);

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
              Cerr << "nb_points=" << N_c << " is too low  for the parallel planes number." << finl;
              Cerr << "Please increase this value." << finl;
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
          compt_c[indic_c]++;
          indic_c++;
        }
    }
  N_c = indic_c;  // nombre de y pour les elements
  compt_c.resize(N_c);
}
