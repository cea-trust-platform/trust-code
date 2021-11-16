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
// File:        Terme_Source_inc_th_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_inc_th_VDF_Face.h>
#include <math.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Periodique.h>
#include <Pb_Fluide_base.h>
#include <Convection_Diffusion_Temperature.h>
#include <Navier_Stokes_std.h>
#include <Fluide_base.h>
#include <DoubleTrav.h>

Implemente_instanciable(Terme_Source_inc_th_VDF_Face,"inc_th_VDF_P0_VDF",Source_base);

//// printOn
//

Sortie& Terme_Source_inc_th_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Terme_Source_inc_th_VDF_Face::readOn(Entree& s )
{
  return Terme_Source_inc_th::lire_donnees(s);
}

void Terme_Source_inc_th_VDF_Face::associer_pb(const Probleme_base& pb)
{
  if (sub_type(Pb_Fluide_base,pb) && (pb.nombre_d_equations()>1))
    {
      const Equation_base& eqn = pb.equation(1);
      if (sub_type(Convection_Diffusion_Temperature,eqn))
        {
          const Navier_Stokes_std& eqn_hydr = ref_cast(Navier_Stokes_std,pb.equation(0));
          const Convection_Diffusion_Temperature& eqn_thermo = ref_cast(Convection_Diffusion_Temperature,eqn);
          Terme_Source_inc_th::associer_eqn(eqn_hydr);
          Terme_Source_inc_th::associer_eqn(eqn_thermo);
        }
    }
  else
    {
      Cerr << "Error for the method Terme_Source_inc_th_VDF_Face::associer_pb" << finl;
      Cerr << "The source term "<<que_suis_je()<<" cannot be activated for the problem "<<pb.que_suis_je()<< finl;
      exit();
    }
  const Fluide_base& fluide=eq_thermique().fluide();
  beta_t_=fluide.beta_t();
}


void Terme_Source_inc_th_VDF_Face::associer_zones(const Zone_dis& zone_dis,
                                                  const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
}

// void Terme_Source_inc_th_VDF_Face::mettre_a_jour(double temps)
//   //void Turbulence_hyd_sous_maille_inc_VDF::mettre_a_jour(double )
// {
//   int nb_elem_tot = la_zone_VDF->zone().nb_elem_tot();
//   DoubleTrav cell_cent_vel(nb_elem_tot,dimension);
//   calculer_cell_cent_vel(cell_cent_vel);
// }

DoubleTab& Terme_Source_inc_th_VDF_Face::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zone_Cl_VDF = la_zone_Cl_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntVect& orientation = zone_VDF.orientation();
  //const DoubleTab& xv = zone_VDF.xv();
  const DoubleVect& porosite_surf = zone_VDF.porosite_face();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();
  const Zone& zone = la_zone_VDF->zone();
  int nb_elem = zone.nb_elem();
  const Champ_Uniforme& ch_beta = ref_cast(Champ_Uniforme,beta().valeur());
  const DoubleTab& vitesse = eq_hydraulique().inconnue().valeurs();
  const DoubleVect& temperature = eq_thermique().inconnue().valeurs();
  DoubleVect cell_cent_temp(temperature);
  int ndeb,nfin,ncomp,num_face;
  //   double debit_e=0.,dt,vol,Surf;
  //double vol,Surf;
  double vol;
  DoubleTrav s(3);
  s = 0.;
  //int nb_elem_tot = la_zone_VDF->zone().nb_elem_tot();
  //   double dQij=0;
  //   double si;

  //   static int deb=0;
  //   static double source=0.;
  //   static double debnm1;
  //   static double debit_ref=0.;
  //   Cerr << " debut de inc" << finl;
  //   double tps = eq_hydraulique().inconnue().temps();
  //   const DoubleVect& vit = eq_hydraulique().inconnue().valeurs();

  // calculer_cell_cent_vel
  // -------------------------------------------------------------------


  // Exemple XTOF
  //   Cerr << " debut de parallel " << finl;
  DoubleTab cell_cent_vel(0, dimension);
  la_zone_VDF.valeur().zone().creer_tableau_elements(cell_cent_vel);
  //    DoubleTrav toto(cell_cent_vel);

  DoubleTab temp1(cell_cent_vel);
  DoubleTab temp2(cell_cent_vel);
  DoubleTab filt_vel(cell_cent_vel);
  DoubleTab inc_dv(cell_cent_vel);

  //   DoubleTrav cell_cent_vel(nb_elem_tot,dimension);

  // -------------------------------------------------------------------
  //   Cerr << " vitesse " << vitesse << finl;
  //   Cerr << " temperature " << cell_cent_temp << finl;
  DoubleVect temp1_t(cell_cent_temp);
  DoubleVect temp2_t(cell_cent_temp);
  DoubleVect filt_temp(cell_cent_temp);
  DoubleVect inc_dt(cell_cent_temp);
  DoubleVect dQij_j(cell_cent_temp);

  //   const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  //   const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  //   int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  const IntTab& elem_faces = zone_VDF.elem_faces();
  int element_number;
  int num0=-1,num1,num2=-1,num3,num4=-1,num5=-1;
  //   Cerr << " vitesse " << vitesse << finl;
  // This is to calculate the cell centered velocity values

  DoubleTab uij_filt(cell_cent_vel);
  //   DoubleTab uij_filt(nb_elem,dimension,dimension);
  for (element_number=0 ; element_number<nb_elem ; element_number ++)
    {
      //       Cerr << me() << " " << element_number << finl;
      cell_cent_temp(element_number)*=ch_beta(0,0);
      temp1_t(element_number)=0.;
      temp2_t(element_number)=0.;
      filt_temp(element_number)=0.;
      inc_dt(element_number)=0.;
      dQij_j(element_number)=0.;
      num0 = elem_faces(element_number,0);
      num1 = elem_faces(element_number,1);
      num2 = elem_faces(element_number,2);
      num3 = elem_faces(element_number,3);
      //       uij_filt(element_number,0,0)=uij_filt_1(element_number,0);
      //       uij_filt(element_number,1,0)=uij_filt_1(element_number,1);
      //       uij_filt(element_number,0,1)=uij_filt_2(element_number,0);
      //       uij_filt(element_number,1,1)=uij_filt_2(element_number,1);
      if (dimension == 3)
        {
          //           uij_filt(element_number,0,2)=uij_filt_3(element_number,0);
          //           uij_filt(element_number,1,2)=uij_filt_3(element_number,1);
          //           uij_filt(element_number,2,2)=uij_filt_3(element_number,2);
          //           uij_filt(element_number,2,0)=uij_filt_1(element_number,2);
          //           uij_filt(element_number,2,1)=uij_filt_2(element_number,2);
          num4 = elem_faces(element_number,4);
          num5 = elem_faces(element_number,5);
          inc_dv(element_number,0)=0.;
          inc_dv(element_number,1)=0.;
          inc_dv(element_number,2)=0.;
        }

      if (dimension == 2)
        {
          cell_cent_vel(element_number,0)=0.5*(vitesse[num0]+vitesse[num2]);
          cell_cent_vel(element_number,1)=0.5*(vitesse[num1]+vitesse[num3]);
          inc_dv(element_number,0)=0.;
          inc_dv(element_number,1)=0.;
        }
      else
        {
          cell_cent_vel(element_number,0)=0.5*(vitesse[num0]+vitesse[num3]);
          cell_cent_vel(element_number,1)=0.5*(vitesse[num1]+vitesse[num4]);
          cell_cent_vel(element_number,2)=0.5*(vitesse[num2]+vitesse[num5]);
        }
    }
  cell_cent_vel.echange_espace_virtuel();

  //   Cerr << " calculer_filter_field" << finl;
  //
  //   calculer_filter_field(cell_cent_vel,filt_vel);
  //
  // calculate filter vector, unfilter tensor and
  // increment fields in the same loop to speed up calculation
  //
  // -------------------------------------------------------------------

  //    DoubleTab toto(cell_cent_vel);

  DoubleTab Qij(cell_cent_vel);
  DoubleTab Lij(cell_cent_vel);
  DoubleTab ttemp1(cell_cent_vel);
  DoubleTab ttemp2(cell_cent_vel);
  //   DoubleTab Qij(uij_filt);
  //   DoubleTab Lij(uij_filt);
  //   DoubleTab ttemp1(uij_filt);
  //   DoubleTab ttemp2(uij_filt);
  //   DoubleTrav filt_vel(nb_elem_tot,dimension);
  //   DoubleTrav inc_dv(nb_elem_tot,dimension);
  //   inc_dv.resize(nb_elem_tot,dimension);
  //   DoubleTrav uij_filt(nb_elem_tot,dimension,dimension);

  // -------------------------------------------------------------------
  //   const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  //   const IntTab& face_voisins = zone_VDF.face_voisins();
  //   int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  //   const IntTab& elem_faces = zone_VDF.elem_faces();
  //   int element_number;
  //   int num0,num1,num2,num3,num4,num5;
  int f0,f1,f2,f3,f4,f5;
  int i;
  int num0_1,num1_1,num2_1,num3_1,num4_1,num5_1;
  int f0_1,f1_1,f2_1,f3_1,f4_1,f5_1;

  //   DoubleTrav temp1(nb_elem_tot,dimension);
  //   DoubleTrav temp2(nb_elem_tot,dimension);

  // This is to calculate the filtered velocity field
  //   Cerr << me() << " boucle 1 " << finl;
  if (dimension ==2)
    {
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          //           for (i=0 ; i<dimension ; i++)
          //             for (j=0 ; j<dimension ; j++)
          //               uij_filt(element_number,i,j)=cell_cent_vel(element_number,i)*
          //                 cell_cent_vel(element_number,j);
          for (i=0 ; i<dimension ; i++)
            {
              uij_filt(element_number,i)=cell_cent_vel(element_number,i)*
                                         cell_cent_temp(element_number);
            }
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,1);
          if ( (num0 == -1) || (num2 == -1) )
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  temp1(element_number,i)=cell_cent_vel(element_number,i);
                  inc_dv(element_number,i)=0.;
                }
              temp1_t(element_number)=cell_cent_temp(element_number);
              inc_dt(element_number)=0.;
            }
          else
            {
              {
                for (i=0 ; i<dimension ; i ++)
                  {
                    temp1(element_number,i)=0.25*(cell_cent_vel(num0,i)+
                                                  2.0*cell_cent_vel(element_number,i)+
                                                  cell_cent_vel(num2,i));
                  }
                temp1_t(element_number)=0.25*(cell_cent_temp(num0)+
                                              2.0*cell_cent_temp(element_number)+
                                              cell_cent_temp(num2));
              }
              f0_1 = elem_faces(num0,0);
              num0_1 = face_voisins(f0_1,0);
              f2_1 = elem_faces(num2,2);
              num2_1 = face_voisins(f2_1,1);
              if ( (num0_1 == -1) || (num2_1 == -1) )
                {
                  for (i=0 ; i<dimension ; i ++)
                    {
                      inc_dv(element_number,i)+=(cell_cent_vel(num0,i))-(cell_cent_vel(num2,i));
                    }
                  inc_dt(element_number)+=(cell_cent_temp(num0))-(cell_cent_temp(num2));
                }
              else
                {
                  for (i=0 ; i<dimension ; i ++)
                    {
                      inc_dv(element_number,i)+=(cell_cent_vel(num0_1,i))-(cell_cent_vel(num2_1,i));
                    }
                  inc_dt(element_number)+=(cell_cent_temp(num0_1))-(cell_cent_temp(num2_1));
                }
            }
        }
      temp1.echange_espace_virtuel();
      temp1_t.echange_espace_virtuel();

      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if ( (num1 == -1) || (num3 == -1) )
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  filt_vel(element_number,i)=temp1(element_number,i);
                  inc_dv(element_number,i)=0.;
                }
              filt_temp(element_number)=temp1_t(element_number);
              inc_dt(element_number)=0.;
            }
          else
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  filt_vel(element_number,i)=0.25*(temp1(num1,i)+
                                                   2.0*temp1(element_number,i)+
                                                   temp1(num3,i));
                }
              filt_temp(element_number)=0.25*(temp1_t(num1)+
                                              2.0*temp1_t(element_number)+
                                              temp1_t(num3));
              f1_1 = elem_faces(num0,1);
              num1_1 = face_voisins(f1_1,0);
              f3_1 = elem_faces(num3,3);
              num3_1 = face_voisins(f3_1,1);
              if ( (num1_1 == -1) || (num3_1 == -1) )
                {
                  for (i=0 ; i<dimension ; i ++)
                    inc_dv(element_number,i)+=(cell_cent_vel(num1,i))-(cell_cent_vel(num3,i));
                  inc_dt(element_number)+=(cell_cent_temp(num1))-(cell_cent_temp(num3));
                }
              else
                {
                  for (i=0 ; i<dimension ; i ++)
                    {
                      inc_dv(element_number,i)+=(cell_cent_vel(num1_1,i))-(cell_cent_vel(num3_1,i));
                    }
                  inc_dt(element_number)+=(cell_cent_temp(num1_1))-(cell_cent_temp(num3_1));
                }
            }
        }
    }
  else // dimension == 3
    {
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          //           for (i=0 ; i<dimension ; i++)
          //             for (j=0 ; j<dimension ; j++)
          //               uij_filt(element_number,i,j)=cell_cent_vel(element_number,i)*
          //                 cell_cent_vel(element_number,j);
          //           Cerr << " elem " << element_number << finl;
          for (i=0 ; i<dimension ; i++)
            {
              uij_filt(element_number,i)=cell_cent_vel(element_number,i)*
                                         cell_cent_temp(element_number);
            }
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if ( (num0 == -1) || (num3 == -1) )
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  temp1(element_number,i)=cell_cent_vel(element_number,i);
                  inc_dv(element_number,i)+=0.;
                }
              temp1_t(element_number)=cell_cent_temp(element_number);
              inc_dt(element_number)+=0.;
            }
          else
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  temp1(element_number,i)=0.25*(cell_cent_vel(num0,i)+
                                                2.0*cell_cent_vel(element_number,i)+
                                                cell_cent_vel(num3,i));
                }
              temp1_t(element_number)=0.25*(cell_cent_temp(num0)+
                                            2.0*cell_cent_temp(element_number)+
                                            cell_cent_temp(num3));
              //               Cerr << " not num0 et num3 " << num0 << " " << num3 << finl;
              f0_1 = elem_faces(num0,0);
              num0_1 = face_voisins(f0_1,0);
              f3_1 = elem_faces(num3,3);
              num3_1 = face_voisins(f3_1,1);
              if ( (num0_1 == -1) || (num3_1 == -1) )
                {
                  for (i=0 ; i<dimension ; i ++)
                    inc_dv(element_number,i)+=(cell_cent_vel(num0,i))-(cell_cent_vel(num3,i));
                  inc_dt(element_number)+=(cell_cent_temp(num0))-(cell_cent_temp(num3));
                }
              else
                {
                  //                   Cerr << " not num0_1 et num3_1 " << num0_1 << " " << num3_1 << finl;
                  for (i=0 ; i<dimension ; i ++)
                    {
                      inc_dv(element_number,i)+=(cell_cent_vel(num0_1,i))-(cell_cent_vel(num3_1,i));
                    }
                  inc_dt(element_number)+=(cell_cent_temp(num0_1))-(cell_cent_temp(num3_1));
                }
            }
        }

      temp1.echange_espace_virtuel();
      temp1_t.echange_espace_virtuel();
      Cerr << " part 1 " << finl;
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          f4 = elem_faces(element_number,4);
          num4 = face_voisins(f4,1);
          if ( (num1 == -1) || (num4 == -1) )
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  temp2(element_number,i)=temp1(element_number,i);
                  inc_dv(element_number,i)+=0.;
                }
              temp2_t(element_number)=temp1_t(element_number);
              inc_dt(element_number)+=0.;
            }
          else
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  temp2(element_number,i)=0.25*(temp1(num1,i)+
                                                2.0*temp1(element_number,i)+
                                                temp1(num4,i));
                }
              temp2_t(element_number)=0.25*(temp1_t(num1)+
                                            2.0*temp1_t(element_number)+
                                            temp1_t(num4));
              //               Cerr << " not num1 et num4 " << num1 << " " << num4 << finl;
              f1_1 = elem_faces(num1,1);
              num1_1 = face_voisins(f1_1,0);
              f4_1 = elem_faces(num4,4);
              num4_1 = face_voisins(f4_1,1);
              if ( (num1_1 == -1) || (num4_1 == -1) )
                {
                  for (i=0 ; i<dimension ; i ++)
                    inc_dv(element_number,i)+=(cell_cent_vel(num1,i))-(cell_cent_vel(num4,i));
                  inc_dt(element_number)+=(cell_cent_temp(num1))-(cell_cent_temp(num4));
                }
              else
                {
                  //                   Cerr << " not num1_1 et num4_1 " << num1_1 << " " << num4_1 << finl;
                  for (i=0 ; i<dimension ; i ++)
                    {
                      inc_dv(element_number,i)+=(cell_cent_vel(num1_1,i))-(cell_cent_vel(num4_1,i));
                    }
                  inc_dt(element_number)+=(cell_cent_temp(num1_1))-(cell_cent_temp(num4_1));
                }
            }
        }

      temp2.echange_espace_virtuel();
      temp2_t.echange_espace_virtuel();
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,0);
          f5 = elem_faces(element_number,5);
          num5 = face_voisins(f5,1);
          if ( (num2 == -1) || (num5 == -1) )
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  filt_vel(element_number,i)=temp2(element_number,i);
                  inc_dv(element_number,i)+=0.;
                }
              filt_temp(element_number)=temp2_t(element_number);
              inc_dt(element_number)+=0.;
            }
          else
            {
              for (i=0 ; i<dimension ; i ++)
                {
                  filt_vel(element_number,i)=0.25*(temp2(num2,i)+
                                                   2.0*temp2(element_number,i)+
                                                   temp2(num5,i));
                }
              filt_temp(element_number)=0.25*(temp2_t(num2)+
                                              2.0*temp2_t(element_number)+
                                              temp2_t(num5));
              //               Cerr << " not num2 et num5 " << num2 << " " << num5 << finl;
              f2_1 = elem_faces(num2,2);
              num2_1 = face_voisins(f2_1,0);
              f5_1 = elem_faces(num5,5);
              num5_1 = face_voisins(f5_1,1);
              if ( (num2_1 == -1) || (num5_1 == -1) )
                {
                  for (i=0 ; i<dimension ; i ++)
                    inc_dv(element_number,i)+=(cell_cent_vel(num2,i))-(cell_cent_vel(num5,i));
                  inc_dt(element_number)+=(cell_cent_temp(num2))-(cell_cent_temp(num5));
                }
              else
                {
                  //                   Cerr << " not num2_1 et num5_1 " << num2_1 << " " << num5_1 << finl;
                  for (i=0 ; i<dimension ; i ++)
                    {
                      inc_dv(element_number,i)+=(cell_cent_vel(num2_1,i))-(cell_cent_vel(num5_1,i));
                    }
                  inc_dt(element_number)+=(cell_cent_temp(num2_1))-(cell_cent_temp(num5_1));
                }
            }
        }
    }
  filt_vel.echange_espace_virtuel();
  inc_dv.echange_espace_virtuel();
  filt_temp.echange_espace_virtuel();
  inc_dt.echange_espace_virtuel();
  uij_filt.echange_espace_virtuel();
  Cerr << " first loop " << finl;
  //
  //   calculer_Qij(inc_dv,Qij);
  // -------------------------------------------------------------------

  //   DoubleTrav Qij(nb_elem_tot,dimension,dimension);

  // -------------------------------------------------------------------
  //   const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  //   int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  //   int element_number;

  // This is to calculate the Qij tensor

  //   for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
  //     {
  //       for (int i=0 ; i<dimension ; i++)
  //         for (int j=0 ; j<dimension ; j++)
  //             Qij(element_number,i,j)=inc_dv(element_number,i)*inc_dv(element_number,j);
  //     }

  //   Cerr << " calculer_Lij " << finl;
  //
  //   calculer_Lij(cell_cent_vel,filt_vel,Lij);
  //
  // calculate Qij and uij_filt in the same loop to speed up calculation
  //
  // -------------------------------------------------------------------

  //   DoubleTrav Lij(nb_elem_tot,dimension,dimension);

  // -------------------------------------------------------------------
  //   const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  //   int nb_elem_tot = zone_VDF.zone().nb_elem_tot();
  //   int element_number;

  //   DoubleTrav uij_filt(nb_elem_tot,dimension,dimension);
  //   DoubleTrav ttemp1(nb_elem_tot,dimension,dimension);
  //   DoubleTrav ttemp2(nb_elem_tot,dimension,dimension);

  // This is to calculate the Lij term for the C coefficient

  //   for (element_number=0 ; element_number<nb_elem_tot ; element_number ++)
  //     {
  //       for (int i=0 ; i<dimension ; i++)
  //         for (int j=0 ; j<dimension ; j++)
  //           {
  //             uij_filt(element_number,i,j)=cell_cent_vel(element_number,i)*
  //               cell_cent_vel(element_number,j);
  //           }
  //     }

  //   calculer_filter_tensor(uij_filt);
  //   Cerr << " calculer_uij_filt " << finl;

  // This is to calculate the filtered tensor field
  //   int j;
  //   Cerr << me() << " boucle 2 " << finl;
  if (dimension ==2)
    {
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,1);
          if ( (num0 == -1) || (num2 == -1) )
            //             for (i=0 ; i<dimension ; i ++)
            //               for (j=0 ; j<dimension ; j ++)
            //                 ttemp1(element_number,i,j)=uij_filt(element_number,i,j);
            for (i=0 ; i<dimension ; i++)
              {
                ttemp1(element_number,i)=uij_filt(element_number,i);
              }
          else
            {
              //               for (i=0 ; i<dimension ; i ++)
              //                 for (j=0 ; j<dimension ; j ++)
              //                   {
              //                     ttemp1(element_number,i,j)=0.25*(uij_filt(num0,i,j)+
              //                                                     2.0*uij_filt(element_number,i,j)+
              //                                                     uij_filt(num2,i,j));
              //                   }
              for (i=0 ; i<dimension ; i++)
                {
                  ttemp1(element_number,i)=0.25*(uij_filt(num0,i)+
                                                 2.0*uij_filt(element_number,i)+
                                                 uij_filt(num2,i));
                }
            }
        }

      ttemp1.echange_espace_virtuel();
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if ( (num1 == -1) || (num3 == -1) )
            //             for (i=0 ; i<dimension ; i ++)
            //               for (j=0 ; j<dimension ; j ++)
            //                 uij_filt(element_number,i,j)=ttemp1(element_number,i,j);
            for (i=0 ; i<dimension ; i++)
              {
                uij_filt(element_number,i)=ttemp1(element_number,i);
              }
          else
            {
              //               for (i=0 ; i<dimension ; i ++)
              //                 for (j=0 ; j<dimension ; j ++)
              //                   {
              //                     uij_filt(element_number,i,j)=0.25*(ttemp1(num1,i,j)+
              //                                                       2.0*ttemp1(element_number,i,j)+
              //                                                       ttemp1(num3,i,j));
              //                   }
              for (i=0 ; i<dimension ; i++)
                {
                  uij_filt(element_number,i)=0.25*(ttemp1(num0,i)+
                                                   2.0*ttemp1(element_number,i)+
                                                   ttemp1(num2,i));
                }
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
            //             for (i=0 ; i<dimension ; i ++)
            //               for (j=0 ; j<dimension ; j ++)
            //                 ttemp1(element_number,i,j)=uij_filt(element_number,i,j);
            for (i=0 ; i<dimension ; i++)
              {
                ttemp1(element_number,i)=uij_filt(element_number,i);
              }
          else
            {
              //               for (i=0 ; i<dimension ; i ++)
              //                 for (j=0 ; j<dimension ; j ++)
              //                   {
              //                     ttemp1(element_number,i,j)=0.25*(uij_filt(num0,i,j)+
              //                                                     2.0*uij_filt(element_number,i,j)+
              //                                                     uij_filt(num3,i,j));
              //                   }
              for (i=0 ; i<dimension ; i++)
                {
                  ttemp1(element_number,i)=0.25*(uij_filt(num0,i)+
                                                 2.0*uij_filt(element_number,i)+
                                                 uij_filt(num2,i));
                }
            }
        }

      ttemp1.echange_espace_virtuel();
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          f4 = elem_faces(element_number,4);
          num4 = face_voisins(f4,1);
          if ( (num1 == -1) || (num4 == -1) )
            //             for (i=0 ; i<dimension ; i ++)
            //               for (j=0 ; j<dimension ; j ++)
            //                 ttemp2(element_number,i,j)=ttemp1(element_number,i,j);
            for (i=0 ; i<dimension ; i++)
              {
                ttemp2(element_number,i)=ttemp1(element_number,i);
              }
          else
            {
              //               for (i=0 ; i<dimension ; i ++)
              //                 for (j=0 ; j<dimension ; j ++)
              //                   {
              //                     ttemp2(element_number,i,j)=0.25*(ttemp1(num1,i,j)+
              //                                                     2.0*ttemp1(element_number,i,j)+
              //                                                     ttemp1(num4,i,j));
              //                   }
            }
        }

      ttemp2.echange_espace_virtuel();
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,0);
          f5 = elem_faces(element_number,5);
          num5 = face_voisins(f5,1);
          if ( (num2 == -1) || (num5 == -1) )
            //             for (i=0 ; i<dimension ; i ++)
            //               for (j=0 ; j<dimension ; j ++)
            //                 uij_filt(element_number,i,j)=ttemp2(element_number,i,j);
            for (i=0 ; i<dimension ; i++)
              {
                uij_filt(element_number,i)=ttemp2(element_number,i);
              }
          else
            {
              //               for (i=0 ; i<dimension ; i ++)
              //                 for (j=0 ; j<dimension ; j ++)
              //                   {
              //                     uij_filt(element_number,i,j)=0.25*(ttemp2(num2,i,j)+
              //                                                       2.0*ttemp2(element_number,i,j)+
              //                                                       ttemp2(num5,i,j));
              //                   }
              for (i=0 ; i<dimension ; i++)
                {
                  uij_filt(element_number,i)=0.25*(ttemp2(num0,i)+
                                                   2.0*ttemp2(element_number,i)+
                                                   ttemp2(num2,i));
                }
            }
        }
    }
  // finish calculating Lij

  uij_filt.echange_espace_virtuel();
  //   Cerr << me() << " boucle 3 " << finl;
  for (element_number=0 ; element_number<nb_elem ; element_number ++)
    {
      //       for (int i=0 ; i<dimension ; i++)
      //         for (int j=0 ; j<dimension ; j++)
      //           {
      //             Qij(element_number,i,j)=inc_dv(element_number,i)*inc_dv(element_number,j);
      //             Lij(element_number,i,j)=uij_filt(element_number,i,j)-
      //               filt_vel(element_number,i)*filt_vel(element_number,j);
      //           }
      for (int ii=0 ; ii<dimension ; ii++)
        {
          Qij(element_number,ii)=inc_dv(element_number,ii)*inc_dt(element_number);
          Lij(element_number,ii)=uij_filt(element_number,ii)-
                                 filt_vel(element_number,ii)*filt_temp(element_number);
        }
    }

  Lij.echange_espace_virtuel();
  Qij.echange_espace_virtuel();
  //
  //   calculer_f(Lij,Qij);
  // -------------------------------------------------------------------

  DoubleVect f(nb_elem);

  // -------------------------------------------------------------------
  DoubleVect temp_1(nb_elem);
  DoubleVect temp_2(nb_elem);

  // Evaluate the dynamic model coefficient f
  int elem;
  if (dimension == 2)
    for (elem=0; elem<nb_elem; elem++)
      {
        //         temp_1(elem)=0.;
        //         temp_2(elem)=0.;
        //     for (int k=0 ; k<dimension ; k++)
        //         for (int l=0 ; l<dimension ; l++)
        //           {
        //             temp_1(elem)+=Lij(elem,k,k);
        //             temp_2(elem)+=4.*Qij(elem,l,l);
        //           }

        temp_1(elem)=Lij(elem,0)+Lij(elem,1);
        temp_2(elem)=4.*Qij(elem,0)+4.*Qij(elem,1);
        if(fabs(temp_2(elem)) < 1.e-12)
          f(elem)=0.;
        else
          f(elem)=temp_1(elem)/temp_2(elem);
      }
  else
    for (elem=0; elem<nb_elem; elem++)
      {
        //         temp_1(elem)=0.;
        //         temp_2(elem)=0.;

        temp_1(elem)=Lij(elem,0)+Lij(elem,1)+Lij(elem,2);
        temp_2(elem)=4.*Qij(elem,0)+4.*Qij(elem,1)+4.*Qij(elem,2);
        if(fabs(temp_2(elem)) < 1.e-12)
          f(elem)=0.;
        else
          f(elem)=temp_1(elem)/temp_2(elem);
      }

  f.echange_espace_virtuel();
  //   Cerr << " calculer_dQij_j" << f << " " << Qij << finl;
  //
  //   calculer_dQij_j(f,Qij,dQij_j);
  // -------------------------------------------------------------------

  //   DoubleTrav dQij_j(nb_elem_tot,dimension);
  //   dQij_j.resize(nb_elem_tot,dimension);

  // -------------------------------------------------------------------
  //const IntTab& Qdm = zone_VDF.Qdm();
  //   const IntVect& orientation = zone_VDF.orientation();

  //
  // Calculate the source term dQij_j = d/dxj ( f Qij )
  //
  //   Cerr << me() << " boucle 4 " << nb_elem << finl;
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
          dQij_j(element_number)=0.5*((f(num2)*Qij(num2,0)-f(num0)*Qij(num0,0))/
                                      zone_VDF.dim_elem(element_number,0));
          //           dQij_j(element_number,0)+=0.5*((f(num3)*Qij(num3,0)-f(num1)*Qij(num1,0))/
          //                                            zone_VDF.dim_elem(element_number,1));
          dQij_j(element_number)+=0.5*((f(num2)*Qij(num2,1)-f(num0)*Qij(num0,1))/
                                       zone_VDF.dim_elem(element_number,0));
          //           dQij_j(element_number,1)+=0.5*((f(num3)*Qij(num3,1)-f(num1)*Qij(num1,1))/
          //                                            zone_VDF.dim_elem(element_number,1));
          //           dQij_j(element_number,0)=0.5*((f(num2)*Qij_1(num2,0)-f(num0)*Qij_1(num0,0))/
          //                                            zone_VDF.dim_elem(element_number,0));
          //           dQij_j(element_number,0)+=0.5*((f(num3)*Qij_2(num3,0)-f(num1)*Qij_2(num1,0))/
          //                                            zone_VDF.dim_elem(element_number,1));
          //           dQij_j(element_number,1)=0.5*((f(num2)*Qij_1(num2,1)-f(num0)*Qij_1(num0,1))/
          //                                            zone_VDF.dim_elem(element_number,0));
          //           dQij_j(element_number,1)+=0.5*((f(num3)*Qij_2(num3,1)-f(num1)*Qij_2(num1,1))/
          //                                            zone_VDF.dim_elem(element_number,1));
        }
    }
  else
    {
      for (element_number=0 ; element_number<nb_elem ; element_number ++)
        {
          f0 = elem_faces(element_number,0);
          num0 = face_voisins(f0,0);
          if (num0 == -1 || num0 >= nb_elem)
            {
              //               Cerr << me() << " bord num0 " << finl;
              num0 = element_number;
            }
          f1 = elem_faces(element_number,1);
          num1 = face_voisins(f1,0);
          if (num1 == -1 || num0 >= nb_elem)
            {
              //               Cerr << me() << " bord num1 " << finl;
              num1 = element_number;
            }
          f2 = elem_faces(element_number,2);
          num2 = face_voisins(f2,0);
          if (num2 == -1 || num2 >= nb_elem)
            {
              //               Cerr << me() << " bord num2 " << finl;
              num2 = element_number;
            }
          f3 = elem_faces(element_number,3);
          num3 = face_voisins(f3,1);
          if (num3 == -1 || num3 >= nb_elem)
            {
              //               Cerr << me() << " bord num3 " << finl;
              num3 = element_number;
            }
          f4 = elem_faces(element_number,4);
          num4 = face_voisins(f4,1);
          if (num4 == -1 || num4 >= nb_elem)
            {
              //               Cerr << me() << " bord num4 " << finl;
              num4 = element_number;
            }
          f5 = elem_faces(element_number,5);
          num5 = face_voisins(f5,1);
          if (num5 == -1 || num5 >= nb_elem)
            {
              //               Cerr << me() << " bord num5 " << finl;
              num5 = element_number;
            }
          dQij_j(element_number)=0.5*((f(num3)*Qij(num3,0)-f(num0)*Qij(num0,0))/
                                      zone_VDF.dim_elem(element_number,0));
          //           dQij_j(element_number,0)+=0.5*((f(num4)*Qij(num4,0)-f(num1)*Qij(num1,0))/
          //                                            zone_VDF.dim_elem(element_number,1));
          //           dQij_j(element_number,0)+=0.5*((f(num5)*Qij(num5,0)-f(num2)*Qij(num2,0))/
          //                                            zone_VDF.dim_elem(element_number,2));
          //           dQij_j(element_number,0)=0.5*((f(num3)*Qij_1(num3,0)-f(num0)*Qij_1(num0,0))/
          //                                            zone_VDF.dim_elem(element_number,0));
          //           dQij_j(element_number,0)+=0.5*((f(num4)*Qij_2(num4,0)-f(num1)*Qij_2(num1,0))/
          //                                            zone_VDF.dim_elem(element_number,1));
          //           dQij_j(element_number,0)+=0.5*((f(num5)*Qij_3(num5,0)-f(num2)*Qij_3(num2,0))/
          //                                            zone_VDF.dim_elem(element_number,2));


          //           dQij_j(element_number,1)=0.5*((f(num3)*Qij(num3,1)-f(num0)*Qij(num0,1))/
          //                                            zone_VDF.dim_elem(element_number,0));
          dQij_j(element_number)+=0.5*((f(num4)*Qij(num4,1)-f(num1)*Qij(num1,1))/
                                       zone_VDF.dim_elem(element_number,1));
          //           dQij_j(element_number,1)+=0.5*((f(num5)*Qij(num5,1)-f(num2)*Qij(num2,1))/
          //                                            zone_VDF.dim_elem(element_number,2));
          //           dQij_j(element_number,1)=0.5*((f(num3)*Qij_1(num3,1)-f(num0)*Qij_1(num0,1))/
          //                                            zone_VDF.dim_elem(element_number,0));
          //           dQij_j(element_number,1)+=0.5*((f(num4)*Qij_2(num4,1)-f(num1)*Qij_2(num1,1))/
          //                                            zone_VDF.dim_elem(element_number,1));
          //           dQij_j(element_number,1)+=0.5*((f(num5)*Qij_3(num5,1)-f(num2)*Qij_3(num2,1))/
          //                                            zone_VDF.dim_elem(element_number,2));

          //           dQij_j(element_number,2)=0.5*((f(num3)*Qij(num3,2)-f(num0)*Qij(num0,2))/
          //                                            zone_VDF.dim_elem(element_number,0));
          //           dQij_j(element_number,2)+=0.5*((f(num4)*Qij(num4,2)-f(num1)*Qij(num1,2))/
          //                                            zone_VDF.dim_elem(element_number,1));
          dQij_j(element_number)+=0.5*((f(num5)*Qij(num5,2)-f(num2)*Qij(num2,2))/
                                       zone_VDF.dim_elem(element_number,2));
          //           dQij_j(element_number,2)=0.5*((f(num3)*Qij_1(num3,2)-f(num0)*Qij_1(num0,2))/
          //                                            zone_VDF.dim_elem(element_number,0));
          //           dQij_j(element_number,2)+=0.5*((f(num4)*Qij_2(num4,2)-f(num1)*Qij_2(num1,2))/
          //                                            zone_VDF.dim_elem(element_number,1));
          //           dQij_j(element_number,2)+=0.5*((f(num5)*Qij_3(num5,2)-f(num2)*Qij_3(num2,2))/
          //                                            zone_VDF.dim_elem(element_number,2));
        }
    }
  dQij_j.echange_espace_virtuel();
  double min_f=1.e6, max_f=-1.e6;
  double min_q=1.e6, max_q=-1.e6;
  double min_dq=1.e6, max_dq=-1.e6;
  double min_dq1=1.e6, max_dq1=-1.e6, sum_t=0.;
  for (element_number=0 ; element_number<nb_elem ; element_number ++)
    {
      if (inc_dv(element_number,0)<min_q)
        min_q=inc_dv(element_number,0);
      if (inc_dv(element_number,0)>max_q)
        max_q=inc_dv(element_number,0);
      if (inc_dt(element_number)<min_f)
        min_f=inc_dt(element_number);
      if (inc_dt(element_number)>max_f)
        max_f=inc_dt(element_number);
      if (inc_dv(element_number,1)<min_dq)
        min_dq=inc_dv(element_number,1);
      if (inc_dv(element_number,1)>max_dq)
        max_dq=inc_dv(element_number,1);
      if (inc_dv(element_number,2)<min_dq1)
        min_dq1=inc_dv(element_number,2);
      if (inc_dv(element_number,2)>max_dq1)
        max_dq1=inc_dv(element_number,2);
      sum_t+=inc_dt(element_number);
    }
  Cerr << me() << " min/max " << min_f << " " << max_f << " "  << min_dq1 << " " << max_dq1 << finl;
  Cerr << " min/max " << (sum_t/nb_elem) << " "  << min_q << " " << max_q << " " << min_dq << " " << max_dq << finl;

  double inc_s=0;
  //   Cerr << "Force de volume (gradient de pression moteur) = " << source << finl;

  // Boucle sur les conditions limites pour traiter les faces de bord
  //   Cerr << me() << " cond lims " << finl;
  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {

      // pour chaque Condition Limite on regarde son type
      // Si face de Dirichlet ou de Symetrie on ne fait rien
      // Si face de Neumann on calcule la contribution au terme source

      const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {

          //const Neumann_sortie_libre& la_cl_neumann = ref_cast(Neumann_sortie_libre,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
              ncomp = orientation(num_face);
              num0 = face_voisins(num_face,0);
              if (num0 == -1)
                num0 = face_voisins(num_face,1);
              inc_s=dQij_j(num0)*vol;
              inc_s=0.;
              //               resu(num_face)+= s(ncomp)*vol;
              resu(num_face)+= inc_s;
            }
        }

      else if (sub_type(Symetrie,la_cl.valeur()))
        ;
      else if ( (sub_type(Dirichlet,la_cl.valeur()))
                ||
                (sub_type(Dirichlet_homogene,la_cl.valeur()))
              )
        ;
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
              ncomp = orientation(num_face);
              num0 = face_voisins(num_face,0);
              if (num0 == -1)
                num0 = face_voisins(num_face,1);
              inc_s=dQij_j(num0)*vol;
              inc_s=0.;
              //               resu(num_face)+= s(ncomp)*vol;
              resu(num_face)+= inc_s;

            }
        }
    }

  // Boucle sur les faces internes
  Cerr << me() << " faces internes thermique" << ch_beta(0,0) << finl;

  double min_inc=1.e6, max_inc=-1.e6;
  int num_e1, n_comp1;
  int num_e2, n_comp2;
  ndeb = zone_VDF.premiere_face_int();
  for (num_face =zone_VDF.premiere_face_int(); num_face<zone_VDF.nb_faces(); num_face++)
    {

      vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
      ncomp = orientation(num_face);
      num0 = face_voisins(num_face,0);
      num1 = face_voisins(num_face,1);
      inc_s=0.5*(dQij_j(num0)+dQij_j(num1))*vol;
      //       resu(num_face)+= s(ncomp)*vol;
      if (inc_s<min_inc)
        {
          min_inc=inc_s;
          num_e1=num0;
          n_comp1=ncomp;
        }
      if (inc_s>max_inc)
        {
          max_inc=inc_s;
          num_e2=num0;
          n_comp2=ncomp;
        }
    }
  //       inc_s=0.;
  resu(num_face)+= inc_s;
  Cerr << min_inc << " " << num_e1 << " " << n_comp1 << finl;
  Cerr << max_inc << " " << num_e2 << " " << n_comp2 << finl;
  resu.echange_espace_virtuel();
  return resu;
}

DoubleTab& Terme_Source_inc_th_VDF_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  ajouter(resu);
  resu.echange_espace_virtuel();
  return resu;
}


