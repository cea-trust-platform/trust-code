/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Paroi_scal_hyd_base_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Paroi_scal_hyd_base_VEF.h>
#include <Paroi_std_hyd_VEF.h>
#include <Convection_Diffusion_std.h>
#include <Modele_turbulence_scal_base.h>
#include <EcrFicPartage.h>
#include <Champ_Uniforme.h>
#include <Fluide_Incompressible.h>
#include <Fluide_Quasi_Compressible.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Neumann_paroi.h>
#include <Probleme_base.h>
#include <SFichier.h>
#include <Modifier_pour_QC.h>
#include <Paroi_decalee_Robin.h>

Implemente_base(Paroi_scal_hyd_base_VEF,"Paroi_scal_hyd_base_VEF",Turbulence_paroi_scal_base);

//     printOn()
/////

Sortie& Paroi_scal_hyd_base_VEF::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Paroi_scal_hyd_base_VEF::readOn(Entree& s)
{
  return s ;
}

/////////////////////////////////////////////////////////////////////
//
//  Implementation des fonctions de la classe Paroi_std_hyd_VEF
//
/////////////////////////////////////////////////////////////////////
void Paroi_scal_hyd_base_VEF::associer(const Zone_dis& zone_dis,const Zone_Cl_dis& zone_Cl_dis)
{
  la_zone_VEF = ref_cast(Zone_VEF,zone_dis.valeur());
  la_zone_Cl_VEF = ref_cast(Zone_Cl_VEF,zone_Cl_dis.valeur());
  // On initialise tout de suite la loi de paroi
  Paroi_scal_hyd_base_VEF::init_lois_paroi();
}

DoubleVect& Paroi_scal_hyd_base_VEF::equivalent_distance_name(DoubleVect& d_eq, const Nom& nom_bord) const
{
  int nb_boundaries=la_zone_VEF->zone().nb_front_Cl();
  for (int n_bord=0; n_bord<nb_boundaries; n_bord++)
    {
      const Front_VF& fr_vf = la_zone_VEF->front_VF(n_bord);
      int nb_faces=fr_vf.nb_faces();
      if (fr_vf.le_nom() == nom_bord)
        {
          d_eq.resize(fr_vf.nb_faces());
          for (int ind_face=0; ind_face<nb_faces; ind_face++)
            d_eq(ind_face) = equivalent_distance(n_bord,ind_face);
        }
    }
  return d_eq;
}

int Paroi_scal_hyd_base_VEF::init_lois_paroi()
{
  int nb_faces_bord_reelles=la_zone_VEF->nb_faces_bord();
  tab_d_reel_.resize(nb_faces_bord_reelles);
  //  positions_Pf_.resize(nb_faces_bord_reelles,dimension);
  // elems_plus_.resize(nb_faces_bord_reelles);

  // Initialisations de equivalent_distance_, tab_d_reel, positions_Pf, elems_plus
  // On initialise les distances equivalentes avec les distances geometriques
  const DoubleTab& face_normales = la_zone_VEF->face_normales();
  //  const DoubleTab& xv = la_zone_VEF->xv();
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleVect& volumes_maille = zone_VEF.volumes();
  const DoubleVect& surfaces_face = zone_VEF.face_surfaces();

  if (axi)
    {
      Cerr<<"Attention, rien n'est fait en Axi pour le VEF"<<finl;
      exit();
    }
  else
    {
      int nb_front=zone_VEF.nb_front_Cl();
      equivalent_distance_.dimensionner(nb_front);
      for (int n_bord=0; n_bord<nb_front; n_bord++)
        {
          const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

          int size=le_bord.nb_faces();
          DoubleVect& dist_equiv = equivalent_distance_[n_bord];
          // Note B.M.: on passe ici deux fois: une fois au readOn (par Paroi_scal_hyd_base_VEF::associer())
          //  et une fois par Modele_turbulence_scal_base::preparer_calcul())
          // donc tester si pas deja fait:
          if (!dist_equiv.get_md_vector().non_nul())
            le_bord.frontiere().creer_tableau_faces(dist_equiv, Array_base::NOCOPY_NOINIT);
          //assert(dist_equiv.get_md_vector() == le_bord.frontiere().md_vector_faces());

          for (int ind_face=0; ind_face<size; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              int elem, autre_face;
              elem = face_voisins(num_face,0);
              if (elem == -1)
                elem = face_voisins(num_face,1);

              // Recherche d'une face autre que la face de paroi appartenant a l'element
              // elem.
              autre_face = elem_faces(elem,0);
              if (autre_face == num_face)
                autre_face = elem_faces(elem,1);

              double distance =  volumes_maille(elem)/surfaces_face(num_face);

              tab_d_reel_[num_face] = distance;

              dist_equiv(ind_face) = distance;

              double ratio = 0.;
              int i;
              for (i=0; i<dimension; i++)
                ratio += (face_normales(num_face,i) * face_normales(num_face,i));
              ratio = sqrt(ratio) ;
              /*
              // Les tableaux positions_Pf_, elems_plus_ ne sont calcules que sur les faces reelles
              for (i=0; i<dimension; i++)
              positions_Pf_(num_face,i)=xv(num_face,i) - tab_d_reel_[num_face]*(face_normales(num_face,i)/ratio);

              if (dimension == 2)
              elems_plus_(num_face) = la_zone_VEF->zone().chercher_elements(positions_Pf_(num_face,0), positions_Pf_(num_face,1));
              else
              elems_plus_(num_face) = la_zone_VEF->zone().chercher_elements(positions_Pf_(num_face,0), positions_Pf_(num_face,1),positions_Pf_(num_face,2));


              if (elems_plus_(num_face) < 0)
              {
              for (i=0; i<dimension; i++)
              positions_Pf_(num_face,i)=xv(num_face,i) - tab_d_reel_[num_face]*(face_normales(num_face,i)/ratio);

              if (dimension == 2)
              elems_plus_(num_face) = la_zone_VEF->zone().chercher_elements(positions_Pf_(num_face,0), positions_Pf_(num_face,1));
              else
              elems_plus_(num_face) = la_zone_VEF->zone().chercher_elements(positions_Pf_(num_face,0), positions_Pf_(num_face,1),positions_Pf_(num_face,2));
              }
              */
            }

          dist_equiv.echange_espace_virtuel();
        }
    }

  return 1;
}


void Paroi_scal_hyd_base_VEF::imprimer_nusselt(Sortie& os) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  int ndeb,nfin,elem;
  const Convection_Diffusion_std& eqn = mon_modele_turb_scal->equation();
  const Equation_base& eqn_hydr = eqn.probleme().equation(0);
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& conductivite = le_fluide.conductivite();
  const DoubleTab& temperature = eqn.probleme().equation(1).inconnue().valeurs();

  const Champ_Fonc& la_diffusivite_turbulente = mon_modele_turb_scal->diffusivite_turbulente();
  DoubleTab alpha_t = la_diffusivite_turbulente.valeurs();
  diviser_par_rho_si_qc(alpha_t,le_fluide);
  const Champ_Don& masse_volumique = le_fluide.masse_volumique();
  const Champ_Don& capacite_calorifique = le_fluide.capacite_calorifique();

  EcrFicPartage Nusselt;
  ouvrir_fichier_partage(Nusselt,"Nusselt");

  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_VEF->les_conditions_limites(n_bord);
      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur())) ||
           (sub_type(Dirichlet_paroi_defilante,la_cl.valeur())) ||
           (sub_type(Paroi_decalee_Robin,la_cl.valeur())) )
        {
          const Zone_Cl_VEF& zone_Cl_VEF_th = ref_cast(Zone_Cl_VEF, eqn.probleme().equation(1).zone_Cl_dis().valeur());
          const Cond_lim& la_cl_th = zone_Cl_VEF_th.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

          if(je_suis_maitre())
            {
              Nusselt << finl;
              Nusselt << "Bord " << le_bord.le_nom() << finl;
              if ( (sub_type(Neumann_paroi,la_cl_th.valeur())))
                {
                  if (dimension == 2)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "\tFace a\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "X\t\t| Y\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)\t| T face de bord (K)\t| Tparoi equiv.(K) " << finl;
                      Nusselt << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                    }
                  if (dimension == 3)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "\tFace a\t\t\t\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "X\t\t| Y\t\t\t| Z\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)\t| T face de bord (K)\t| Tparoi equiv.(K)" << finl;
                      Nusselt <<        "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                    }
                }
              else
                {
                  if (dimension == 2)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "\tFace a\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "X\t\t| Y\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K) " << finl;
                      Nusselt << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                    }
                  if (dimension == 3)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "\tFace a\t\t\t\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "X\t\t| Y\t\t\t| Z\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K) " << finl;
                      Nusselt <<        "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                    }
                }
            }
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              double x=zone_VEF.xv(num_face,0);
              double y=zone_VEF.xv(num_face,1);
              double lambda,lambda_t;
              double rho,Cp;
              elem = face_voisins(num_face,0);
              if (elem == -1)
                elem = face_voisins(num_face,1);
              if (sub_type(Champ_Uniforme,conductivite.valeur()))
                lambda = conductivite(0,0);
              else
                {
                  if (conductivite.nb_comp()==1)
                    lambda = conductivite(elem);
                  else
                    lambda = conductivite(elem,0);
                }
              if (sub_type(Champ_Uniforme,masse_volumique.valeur()))
                rho = masse_volumique(0,0);
              else
                {
                  if (masse_volumique.nb_comp()==1)
                    rho = masse_volumique(elem);
                  else
                    rho = masse_volumique(elem,0);
                }
              if (sub_type(Champ_Uniforme,capacite_calorifique.valeur()))
                Cp = capacite_calorifique(0,0);
              else
                {
                  if (capacite_calorifique.nb_comp()==1)
                    Cp = capacite_calorifique(elem);
                  else
                    Cp = capacite_calorifique(elem,0);
                }
              if (sub_type(Fluide_Quasi_Compressible,le_fluide))
                alpha_t(elem) /= Cp;
              lambda_t=alpha_t(elem)*rho*Cp;
              if (dimension == 2)
                Nusselt << x << "\t| " << y;
              if (dimension == 3)
                {
                  double z=zone_VEF.xv(num_face,2);
                  Nusselt << x << "\t| " << y << "\t| " << z;
                }

              // on doit calculer Tfluide premiere maille sans prendre en compte Tparoi
              double tfluide=0.;
              int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
              double surface_face=zone_VEF.face_surfaces(num_face);
              double surface_pond;
              int j;
              for (int i=0; i<nb_faces_elem; i++)
                {
                  if ( (j=elem_faces(elem,i)) != num_face )
                    {
                      surface_pond = 0.;
                      for (int kk=0; kk<dimension; kk++)
                        surface_pond -= (zone_VEF.face_normales(j,kk)*zone_VEF.oriente_normale(j,elem)*zone_VEF.face_normales(num_face,kk)*
                                         zone_VEF.oriente_normale(num_face,elem))/(surface_face*surface_face);
                      tfluide+=temperature(j)*surface_pond;
                    }
                }

              double d_equiv=equivalent_distance_[n_bord](num_face-ndeb);

              if ( (sub_type(Neumann_paroi,la_cl_th.valeur())))
                {
                  // dans ce cas on va imprimer Tfluide (moyenne premiere maille), Tface et on Tparoi recalcule avec d_equiv
                  const Neumann_paroi& la_cl_neum = ref_cast(Neumann_paroi,la_cl_th.valeur());
                  double tparoi = temperature(num_face);
                  double flux = la_cl_neum.flux_impose(num_face-ndeb)*rho*Cp;
                  double tparoi_equiv = tfluide+flux/(lambda+lambda_t)*d_equiv;
                  Nusselt << "\t| " << d_equiv << "\t| " << (lambda+lambda_t)/lambda*tab_d_reel_[num_face]/d_equiv << "\t| " << (lambda+lambda_t)/d_equiv << "\t| " << tfluide << "\t| " << tparoi << "\t| " << tparoi_equiv << finl;
                }
              else
                {
                  // on imprime Tfluide seulement car normalement Tface=Tparoi est connu
                  Nusselt << "\t| " << d_equiv << "\t| " << (lambda+lambda_t)/lambda*tab_d_reel_[num_face]/d_equiv << "\t| " << (lambda+lambda_t)/d_equiv << "\t| " << tfluide << finl;
                }
            }
          Nusselt.syncfile();
        }
    }
  if(je_suis_maitre())
    Nusselt << finl << finl;
  Nusselt.syncfile();
}

