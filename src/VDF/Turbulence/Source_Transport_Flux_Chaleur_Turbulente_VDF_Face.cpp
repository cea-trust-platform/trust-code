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
// File:        Source_Transport_Flux_Chaleur_Turbulente_VDF_Face.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////


#include <Source_Transport_Flux_Chaleur_Turbulente_VDF_Face.h>
#include <Modele_turbulence_scal_Fluctuation_Temperature.h>
#include <Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <Convection_Diffusion_Temperature.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <IntTrav.h>
#include <Champ_Uniforme.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <DoubleTrav.h>

Implemente_instanciable_sans_constructeur(Source_Transport_Flux_Chaleur_Turbulente_VDF_Face,"Source_Transport_Flux_Chaleur_Turbulente_VDF_Face",Source_base);

//// printOn
//

Sortie& Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}


//// readOn
//

Entree& Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::readOn(Entree& is )
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;

  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "On attendait { pour commencer a lire les constantes de Source_Transport_Flux_Chaleur_Turbulente" << finl;
      exit();
    }
  Cerr << "Lecture des constantes de Source_Transport_Flux_Chaleur_Turbulente" << finl;
  Motcles les_mots(3);
  {
    les_mots[0] = "C1_teta";
    les_mots[1] = "C2_teta";
    les_mots[2] = "C3_teta";
  }
  is >> motlu;
  while (motlu != accolade_fermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            is >> C1;
            break;
          }
        case 1 :
          {
            is >> C2;
            break;
          }
        case 2 :
          {
            is >> C3;
            break;
          }
        default :
          {
            Cerr << "On ne comprend pas le mot cle : " << motlu << "dans Source_Transport_Flux_Chaleur_Turbulente" << finl;
            exit();
          }
        }

      is >> motlu;
    }

  return is ;
}

void Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::associer_pb(const Probleme_base& pb )
{
  eq_hydraulique = pb.equation(0);
  mon_eq_transport_Flux_Chaleur_Turb_ = ref_cast(Transport_Flux_Chaleur_Turbulente,equation());
  const Convection_Diffusion_Temperature& eqn_th =
    ref_cast(Convection_Diffusion_Temperature,pb.equation(1));
  eq_thermique = eqn_th;
  const Fluide_Incompressible& fluide = eq_thermique->fluide();
  beta_t = fluide.beta_t();
  gravite_ = fluide.gravite();
}

void Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::associer_zones(const Zone_dis& zone_dis,
                                                                       const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VDF = ref_cast(Zone_VDF, zone_dis.valeur());
  la_zone_Cl_VDF = ref_cast(Zone_Cl_VDF, zone_Cl_dis.valeur());
}

////////////////////////////////////////////////////////////
//
//   Methode pour determiner la production par le gradient
//   moyen de la temperature
////////////////////////////////////////////////////////////

DoubleTab& Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::calculer_uiuj(const Zone_VDF& zone_VDF, const DoubleTab& vit, const DoubleTab& visco_turb, DoubleTab& uiuj) const
{
  uiuj= 0;

  // Calcul des du/dy+dv/dx ...et des gradients par elements

  //  const Zone& la_zone=zone_VDF.zone();
  //  int nb_faces_elem = la_zone.nb_faces_elem();
  int nb_elem = zone_VDF.nb_elem();
  int nb_elem_tot = zone_VDF.nb_elem_tot();
  int n_deb = zone_VDF.premiere_arete_interne();
  int n_fin = zone_VDF.nb_aretes_internes() + n_deb;
  //  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& Qdm = zone_VDF.Qdm();
  const IntVect& orientation = zone_VDF.orientation();
  IntTrav num(4);
  double sum,visco_moy,rac_visc;
  int i, num_elem, n_arete;
  //  int premiere_face_int = zone_VDF.premiere_face_int();

  DoubleTrav du_dy(nb_elem_tot);
  DoubleTrav du_dz(nb_elem_tot);
  DoubleTrav dv_dx(nb_elem_tot);
  DoubleTrav dv_dz(nb_elem_tot);
  DoubleTrav dw_dx(nb_elem_tot);
  DoubleTrav dw_dy(nb_elem_tot);

  if (Objet_U::dimension == 2)
    {

      DoubleVect coef(2);

      // Boucle sur les aretes internes  pour le calcul
      // des moyennes des derivees croisees

      for (n_arete =n_deb; n_arete<n_fin; n_arete++)
        {

          for (i=0; i<4; i++)
            num[i] = Qdm(n_arete,i);
          visco_moy=0;
          if (visco_turb(face_voisins(num[0],0))>1.e-10 &&
              visco_turb(face_voisins(num[0],1))>1.e-10 &&
              visco_turb(face_voisins(num[1],0))>1.e-10 &&
              visco_turb(face_voisins(num[1],1))>1.e-10)
            visco_moy = 0.25 * (visco_turb(face_voisins(num[0],0))
                                + visco_turb(face_voisins(num[0],1))
                                + visco_turb(face_voisins(num[1],0))
                                + visco_turb(face_voisins(num[1],1)));
          rac_visc=sqrt(visco_moy);
          coef[0] = rac_visc * (vit(num[1])-vit(num[0]))/zone_VDF.dist_face(num[0],num[1],0);
          coef[1] = rac_visc * (vit(num[3])-vit(num[2]))/zone_VDF.dist_face(num[2],num[3],1);
          dv_dx(face_voisins(num[0],0)) += coef[0];
          dv_dx(face_voisins(num[0],1)) += coef[0];
          dv_dx(face_voisins(num[1],0)) += coef[0];
          dv_dx(face_voisins(num[1],1)) += coef[0];
          du_dy(face_voisins(num[2],0)) += coef[1];
          du_dy(face_voisins(num[2],1)) += coef[1];
          du_dy(face_voisins(num[3],0)) += coef[1];
          du_dy(face_voisins(num[3],1)) += coef[1];
        }

      // Boucle sur les elements

      for (num_elem=0; num_elem<nb_elem; num_elem++)
        {

          sum = 0.25*(du_dy(num_elem)+dv_dx(num_elem));
          uiuj(num_elem) += sum;
        }
    }

  else if (Objet_U::dimension == 3)
    {

      DoubleVect coef(3);

      // Boucle sur les aretes internes  pour le calcul
      // des moyennes des derivees croisees

      for (n_arete =n_deb; n_arete<n_fin; n_arete++)
        {

          for (i=0; i<4; i++)
            num[i] = Qdm(n_arete,i);
          visco_moy=0;
          if (visco_turb(face_voisins(num[0],0))>1.e-10 &&
              visco_turb(face_voisins(num[0],1))>1.e-10 &&
              visco_turb(face_voisins(num[1],0))>1.e-10 &&
              visco_turb(face_voisins(num[1],1))>1.e-10)
            visco_moy = 0.25 * (visco_turb(face_voisins(num[0],0))
                                + visco_turb(face_voisins(num[0],1))
                                + visco_turb(face_voisins(num[1],0))
                                + visco_turb(face_voisins(num[1],1)));
          rac_visc=sqrt(visco_moy);
          if (orientation(num[0]) == 2)
            {

              if (orientation(num[2]) == 0)
                {
                  coef[0] = rac_visc * (vit(num[1])-vit(num[0]))/zone_VDF.dist_face(num[0],num[1],0);
                  coef[1] = rac_visc * (vit(num[3])-vit(num[2]))/zone_VDF.dist_face(num[2],num[3],2);
                  dw_dx(face_voisins(num[0],0)) += coef[0];
                  dw_dx(face_voisins(num[0],1)) += coef[0];
                  dw_dx(face_voisins(num[1],0)) += coef[0];
                  dw_dx(face_voisins(num[1],1)) += coef[0];
                  du_dz(face_voisins(num[2],0)) += coef[1];
                  du_dz(face_voisins(num[2],1)) += coef[1];
                  du_dz(face_voisins(num[3],0)) += coef[1];
                  du_dz(face_voisins(num[3],1)) += coef[1];
                }
              else if (orientation(num[2]) == 1)
                {
                  coef[0] = rac_visc * (vit(num[1])-vit(num[0]))/zone_VDF.dist_face(num[0],num[1],1);
                  coef[1] = rac_visc * (vit(num[3])-vit(num[2]))/zone_VDF.dist_face(num[2],num[3],2);
                  dw_dy(face_voisins(num[0],0)) += coef[0];
                  dw_dy(face_voisins(num[0],1)) += coef[0];
                  dw_dy(face_voisins(num[1],0)) += coef[0];
                  dw_dy(face_voisins(num[1],1)) += coef[0];
                  dv_dz(face_voisins(num[2],0)) += coef[1];
                  dv_dz(face_voisins(num[2],1)) += coef[1];
                  dv_dz(face_voisins(num[3],0)) += coef[1];
                  dv_dz(face_voisins(num[3],1)) += coef[1];
                }
            }

          else if (orientation(num[0]) == 1)
            {
              coef[0] = rac_visc * (vit(num[1])-vit(num[0]))/zone_VDF.dist_face(num[0],num[1],0);
              coef[1] = rac_visc * (vit(num[3])-vit(num[2]))/zone_VDF.dist_face(num[2],num[3],1);
              dv_dx(face_voisins(num[0],0)) += coef[0];
              dv_dx(face_voisins(num[0],1)) += coef[0];
              dv_dx(face_voisins(num[1],0)) += coef[0];
              dv_dx(face_voisins(num[1],1)) += coef[0];
              du_dy(face_voisins(num[2],0)) += coef[1];
              du_dy(face_voisins(num[2],1)) += coef[1];
              du_dy(face_voisins(num[3],0)) += coef[1];
              du_dy(face_voisins(num[3],1)) += coef[1];
            }

        }

      // Boucle sur les elements

      for (int num_elemb=0; num_elemb<nb_elem; num_elemb++)
        {

          coef[0] = 0.25*(du_dy(num_elemb)+dv_dx(num_elemb));
          coef[1] = 0.25*(du_dz(num_elemb)+dw_dx(num_elemb));
          coef[2] = 0.25*(dw_dy(num_elemb)+dv_dz(num_elemb));

          sum = coef[0]*coef[0] + coef[1]*coef[1] + coef[2]*coef[2];
          uiuj(num_elemb) += sum;
        }
    }
  return uiuj;
}

DoubleTab& Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::calculer_Grad_U(const Zone_VDF& zone_VDF, const DoubleTab& vit, const DoubleTab& visco_turb, DoubleTab& Grad_U) const
{
  Grad_U= 0;

  // Calcul des du/dy+dv/dx ...et des gradients par elements

  //  const Zone& la_zone=zone_VDF.zone();
  //  int nb_faces_elem = la_zone.nb_faces_elem();
  int nb_elem = zone_VDF.nb_elem();
  int nb_elem_tot = zone_VDF.nb_elem_tot();
  int n_deb = zone_VDF.premiere_arete_interne();
  int n_fin = zone_VDF.nb_aretes_internes() + n_deb;
  //  const IntTab& elem_faces = zone_VDF.elem_faces();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntTab& Qdm = zone_VDF.Qdm();
  const IntVect& orientation = zone_VDF.orientation();
  IntTrav num(4);
  double visco_moy,rac_visc;
  int i, num_elem, n_arete;
  //  int premiere_face_int = zone_VDF.premiere_face_int();

  DoubleTrav du_dy(nb_elem_tot);
  DoubleTrav du_dz(nb_elem_tot);
  DoubleTrav dv_dx(nb_elem_tot);
  DoubleTrav dv_dz(nb_elem_tot);
  DoubleTrav dw_dx(nb_elem_tot);
  DoubleTrav dw_dy(nb_elem_tot);

  if (Objet_U::dimension == 2)
    {

      DoubleVect coef(2);

      // Boucle sur les aretes internes  pour le calcul
      // des moyennes des derivees croisees

      for (n_arete =n_deb; n_arete<n_fin; n_arete++)
        {

          for (i=0; i<4; i++)
            num[i] = Qdm(n_arete,i);
          visco_moy=0;
          if (visco_turb(face_voisins(num[0],0))>1.e-10 &&
              visco_turb(face_voisins(num[0],1))>1.e-10 &&
              visco_turb(face_voisins(num[1],0))>1.e-10 &&
              visco_turb(face_voisins(num[1],1))>1.e-10)
            visco_moy = 0.25 * (visco_turb(face_voisins(num[0],0))
                                + visco_turb(face_voisins(num[0],1))
                                + visco_turb(face_voisins(num[1],0))
                                + visco_turb(face_voisins(num[1],1)));
          rac_visc=sqrt(visco_moy);
          coef[0] = rac_visc * (vit(num[1])-vit(num[0]))/zone_VDF.dist_face(num[0],num[1],0);
          coef[1] = rac_visc * (vit(num[3])-vit(num[2]))/zone_VDF.dist_face(num[2],num[3],1);
          dv_dx(face_voisins(num[0],0)) += coef[0];
          dv_dx(face_voisins(num[0],1)) += coef[0];
          dv_dx(face_voisins(num[1],0)) += coef[0];
          dv_dx(face_voisins(num[1],1)) += coef[0];
          du_dy(face_voisins(num[2],0)) += coef[1];
          du_dy(face_voisins(num[2],1)) += coef[1];
          du_dy(face_voisins(num[3],0)) += coef[1];
          du_dy(face_voisins(num[3],1)) += coef[1];
        }

      // Boucle sur les elements

      for (num_elem=0; num_elem<nb_elem; num_elem++)
        {
          Grad_U(num_elem,0,1)= du_dy(num_elem);
          Grad_U(num_elem,1,0)= dv_dx(num_elem);
        }
    }

  else if (Objet_U::dimension == 3)
    {

      //  DoubleTrav du_dy(nb_elem_tot);
      //  DoubleTrav du_dz(nb_elem_tot);
      //  DoubleTrav dv_dx(nb_elem_tot);
      //  DoubleTrav dv_dz(nb_elem_tot);
      //  DoubleTrav dw_dx(nb_elem_tot);
      //  DoubleTrav dw_dy(nb_elem_tot);
      DoubleVect coef(3);

      // Boucle sur les aretes internes  pour le calcul
      // des moyennes des derivees croisees

      for (n_arete =n_deb; n_arete<n_fin; n_arete++)
        {

          for (i=0; i<4; i++)
            num[i] = Qdm(n_arete,i);
          visco_moy=0;
          if (visco_turb(face_voisins(num[0],0))>1.e-10 &&
              visco_turb(face_voisins(num[0],1))>1.e-10 &&
              visco_turb(face_voisins(num[1],0))>1.e-10 &&
              visco_turb(face_voisins(num[1],1))>1.e-10)
            visco_moy = 0.25 * (visco_turb(face_voisins(num[0],0))
                                + visco_turb(face_voisins(num[0],1))
                                + visco_turb(face_voisins(num[1],0))
                                + visco_turb(face_voisins(num[1],1)));
          rac_visc=sqrt(visco_moy);
          if (orientation(num[0]) == 2)
            {

              if (orientation(num[2]) == 0)
                {
                  coef[0] = rac_visc * (vit(num[1])-vit(num[0]))/zone_VDF.dist_face(num[0],num[1],0);
                  coef[1] = rac_visc * (vit(num[3])-vit(num[2]))/zone_VDF.dist_face(num[2],num[3],2);
                  dw_dx(face_voisins(num[0],0)) += coef[0];
                  dw_dx(face_voisins(num[0],1)) += coef[0];
                  dw_dx(face_voisins(num[1],0)) += coef[0];
                  dw_dx(face_voisins(num[1],1)) += coef[0];
                  du_dz(face_voisins(num[2],0)) += coef[1];
                  du_dz(face_voisins(num[2],1)) += coef[1];
                  du_dz(face_voisins(num[3],0)) += coef[1];
                  du_dz(face_voisins(num[3],1)) += coef[1];
                }
              else if (orientation(num[2]) == 1)
                {
                  coef[0] = rac_visc * (vit(num[1])-vit(num[0]))/zone_VDF.dist_face(num[0],num[1],1);
                  coef[1] = rac_visc * (vit(num[3])-vit(num[2]))/zone_VDF.dist_face(num[2],num[3],2);
                  dw_dy(face_voisins(num[0],0)) += coef[0];
                  dw_dy(face_voisins(num[0],1)) += coef[0];
                  dw_dy(face_voisins(num[1],0)) += coef[0];
                  dw_dy(face_voisins(num[1],1)) += coef[0];
                  dv_dz(face_voisins(num[2],0)) += coef[1];
                  dv_dz(face_voisins(num[2],1)) += coef[1];
                  dv_dz(face_voisins(num[3],0)) += coef[1];
                  dv_dz(face_voisins(num[3],1)) += coef[1];
                }
            }

          else if (orientation(num[0]) == 1)
            {
              coef[0] = rac_visc * (vit(num[1])-vit(num[0]))/zone_VDF.dist_face(num[0],num[1],0);
              coef[1] = rac_visc * (vit(num[3])-vit(num[2]))/zone_VDF.dist_face(num[2],num[3],1);
              dv_dx(face_voisins(num[0],0)) += coef[0];
              dv_dx(face_voisins(num[0],1)) += coef[0];
              dv_dx(face_voisins(num[1],0)) += coef[0];
              dv_dx(face_voisins(num[1],1)) += coef[0];
              du_dy(face_voisins(num[2],0)) += coef[1];
              du_dy(face_voisins(num[2],1)) += coef[1];
              du_dy(face_voisins(num[3],0)) += coef[1];
              du_dy(face_voisins(num[3],1)) += coef[1];
            }

        }

      // Boucle sur les elements

      for (int num_elemb=0; num_elemb<nb_elem; num_elemb++)
        {
          Grad_U(num_elemb,0,1)= du_dy(num_elemb);
          Grad_U(num_elemb,1,0)= dv_dx(num_elemb);
          Grad_U(num_elemb,0,2)= du_dz(num_elemb);
          Grad_U(num_elemb,1,2)= dv_dz(num_elemb);
        }
    }
  return Grad_U;
}



DoubleTab& Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::calculer_gteta2(const Zone_VDF& zone_VDF, DoubleTab& gteta2 ,const DoubleTab& fluctu_temp, double beta,const DoubleVect& gravite) const
{
  //
  // gteta2 est discretise au centre des elements
  //

  int nb_elem = zone_VDF.nb_elem();
  int nb_faces= zone_VDF.nb_faces();
  DoubleTrav u_teta(nb_faces);
  //  const DoubleVect& porosite_face = zone_VDF.porosite_face();
  gteta2 = 0;

  //                ------->  -------->
  // Calcul de beta.gravite . tetacarre

  const Zone& la_zone=zone_VDF.zone();
  int nb_faces_elem = la_zone.nb_faces_elem();

  IntTrav numfa(nb_faces_elem);
  DoubleVect coef(dimension);
  //  const IntTab& les_elem_faces = zone_VDF.elem_faces();

  for (int elem=0; elem<nb_elem; elem++)
    for (int dim=0; dim<dimension; dim++)
      gteta2(elem,dim) = beta*gravite(dim)*fluctu_temp(elem,0) ;
  return gteta2;
}
DoubleTab& Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::calculer_gteta2(const Zone_VDF& zone_VDF,DoubleTab& gteta2 ,const DoubleTab& fluctu_temp,const DoubleTab& beta,const DoubleVect& gravite) const
{
  //
  // gteta2 est discretise au centre des elements
  //

  int nb_elem = zone_VDF.nb_elem();
  int nb_faces= zone_VDF.nb_faces();
  DoubleTrav u_teta(nb_faces);
  //  const DoubleVect& porosite_face = zone_VDF.porosite_face();
  gteta2 = 0;

  //                ------->  -------->
  // Calcul de beta.gravite . tetacarre

  const Zone& la_zone=zone_VDF.zone();
  int nb_faces_elem = la_zone.nb_faces_elem();

  IntTrav numfa(nb_faces_elem);
  DoubleVect coef(dimension);
  //  const IntTab& les_elem_faces = zone_VDF.elem_faces();

  for (int elem=0; elem<nb_elem; elem++)
    for (int dim=0; dim<dimension; dim++)
      gteta2(elem,dim) = beta(elem)*gravite(dim)*fluctu_temp(elem,0) ;
  return gteta2;
}


DoubleTab& Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::ajouter(DoubleTab& resu) const
{
  const Zone_VDF& zone_VDF = la_zone_VDF.valeur();
  const Zone_Cl_VDF& zone_Cl_VDF = la_zone_Cl_VDF.valeur();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const IntVect& orientation = zone_VDF.orientation();
  const DoubleTab& temper = eq_thermique->inconnue().valeurs();
  const DoubleTab& vit = eq_hydraulique->inconnue().valeurs();
  const RefObjU& modele_turbulence_hydr = eq_hydraulique->get_modele(TURBULENCE);
  const Mod_turb_hyd_base& mon_modele = ref_cast(Mod_turb_hyd_base,modele_turbulence_hydr.valeur());
  const DoubleTab& visco_turb = mon_modele.viscosite_turbulente().valeurs();
  const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& modele_bas_Re = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,mon_modele);
  const Transport_K_Eps_base& mon_eq_transport_K_Eps_Bas_Re = modele_bas_Re.eqn_transp_K_Eps();
  const DoubleTab& K_Eps_Bas_Re = mon_eq_transport_K_Eps_Bas_Re.inconnue().valeurs();
  const Modele_turbulence_scal_base& le_modele_scalaire = ref_cast(Modele_turbulence_scal_base,eq_thermique->get_modele(TURBULENCE).valeur());
  const Modele_turbulence_scal_Fluctuation_Temperature& modele_Fluctu_Temp = ref_cast(Modele_turbulence_scal_Fluctuation_Temperature,le_modele_scalaire);
  const Transport_Fluctuation_Temperature& mon_eq_transport_Fluctu_Temp =  modele_Fluctu_Temp.equation_Fluctu();
  const DoubleTab& Fluctu_Temperature = mon_eq_transport_Fluctu_Temp.inconnue().valeurs();
  const DoubleTab& Flux_Chaleur_Turb = mon_eq_transport_Flux_Chaleur_Turb_->inconnue().valeurs();
  const DoubleVect& g = eq_thermique->fluide().gravite().valeurs();
  const Champ_Don& ch_beta = beta_t.valeur();
  const DoubleTab& tab_beta = ch_beta.valeurs();
  const DoubleVect& volumes = zone_VDF.volumes();
  int nb_elem = zone_VDF.nb_elem();
  const DoubleVect& porosite_surf = zone_VDF.porosite_face();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();
  int ori,ori1,num_face,elem1,elem2;
  double vol;
  int nb_faces = zone_VDF.nb_faces();
  DoubleTab Grad_U(nb_elem,dimension,dimension);
  DoubleTab gteta2(nb_elem,dimension);
  DoubleTab uiuj(nb_elem);

  if (sub_type(Champ_Uniforme,ch_beta.valeur()))
    calculer_gteta2(zone_VDF,gteta2 ,Fluctu_Temperature,tab_beta(0,0),g);
  else
    calculer_gteta2(zone_VDF, gteta2 ,Fluctu_Temperature,tab_beta,g);

  calculer_uiuj(zone_VDF,vit,visco_turb,uiuj);
  calculer_Grad_U(zone_VDF,vit,visco_turb,Grad_U);
  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);

      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      for (num_face=ndeb; num_face<nfin; num_face++)
        {

          vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
          ori = orientation(num_face);
          ori1 = orientation(num_face+(1+dimension)%dimension);
          elem1 = face_voisins(num_face,0);
          if (elem1 != -1)
            {
              double contrib = (-1*temper(elem1))/zone_VDF.dist_norm_bord(num_face)*uiuj(elem1)
                               -(1-C2)*Flux_Chaleur_Turb(num_face)*Grad_U(elem1,ori,ori1)
                               -(1-C3)*gteta2(elem1,ori);
              if (K_Eps_Bas_Re(elem1,0)>1.e-10)
                contrib+=-C1*Flux_Chaleur_Turb(num_face)*K_Eps_Bas_Re(elem1,1)/(K_Eps_Bas_Re(elem1,0));
              resu(num_face)+=contrib*vol;
            }
          else
            {
              elem2 = face_voisins(num_face,1);
              double contrib = (-1*temper(elem2))/zone_VDF.dist_norm_bord(num_face)*uiuj(elem2)
                               -(1-C2)*Flux_Chaleur_Turb(num_face)*Grad_U(elem2,ori,ori1)
                               -(1-C3)*gteta2(elem2,ori);
              if (K_Eps_Bas_Re(elem2,0)>1.e-10)
                contrib+=-C1*Flux_Chaleur_Turb(num_face)*K_Eps_Bas_Re(elem2,1)/(K_Eps_Bas_Re(elem2,0));
              resu(num_face)+=contrib*vol;
            }
        }
    }

  // Traitement des faces internes
  for (num_face=zone_VDF.premiere_face_int(); num_face<nb_faces; num_face++)
    {

      // vol et ori1 faux dans la ancienne version dans la suite
      // on initialise et on fait planter
      //      vol=-1;ori1=-1;
      vol = volumes_entrelaces(num_face)*porosite_surf(num_face);
      // Que vaut ori1 reellement en 3D ????
      ori1 = orientation(num_face+(1+dimension)%dimension);
      // On recupere l'orientation de la face n+1
      // est ce que l'on cherche ????

      Cerr<<"(1+2)%2 " <<(1+2)%2<< " (1+3)%3 "<<(1+3)%3<<finl;
      Cerr<<" Si vous utilisiez "<<que_suis_je()<<" priere de contacter l'assistance TRUST"<<finl;
      Cerr<<" un gros bug a ete trouve ...."<<finl;
      assert(0);
      exit();
      ori=orientation(num_face);
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);
      double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
      double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));

      double reyn=a*uiuj(elem1)+b*uiuj(elem2);
      double A=-1*(temper(elem2)-temper(elem1))/zone_VDF.dist_norm(num_face)*reyn;
      double B=0;
      if (a*K_Eps_Bas_Re(elem1,0)+b*K_Eps_Bas_Re(elem2,0)>1.e-10)
        B=-C1*Flux_Chaleur_Turb(num_face)*((a*K_Eps_Bas_Re(elem1,1)+b*K_Eps_Bas_Re(elem2,1))
                                           /(a*K_Eps_Bas_Re(elem1,0)+b*K_Eps_Bas_Re(elem2,0)));
      double C=-(1-C2)*Flux_Chaleur_Turb(num_face)*(a*Grad_U(elem1,ori,ori1)+b*Grad_U(elem2,ori,ori1));
      double D=-(1-C3)*(a*gteta2(elem1,ori)+b*gteta2(elem2,ori));
      //if (elem1==16 && elem2==17) {
      //   Cerr << "Flux transport:" << finl;
      //   Cerr << "reyn=" << reyn << " total=" << (A+B+C+D)*vol << finl;
      //   Cerr << "A=" << A << " B=" << B << " C=" << C << " D=" << D << finl;
      //}
      resu(num_face)+=(A+B+C+D)*vol;
    }
  return resu;
}

DoubleTab& Source_Transport_Flux_Chaleur_Turbulente_VDF_Face::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}
