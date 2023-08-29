/****************************************************************************
* Copyright (c) 2019, CEA
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

#include <Paroi_scal_hyd_base_VEF.h>
#include <Convection_Diffusion_std.h>
#include <Modele_turbulence_scal_base.h>
#include <EcrFicPartage.h>
#include <Champ_Uniforme.h>
#include <Fluide_base.h>
#include <Fluide_Quasi_Compressible.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Neumann_paroi.h>
#include <Probleme_base.h>
#include <Domaine_VEF.h>
#include <SFichier.h>
#include <Modifier_pour_fluide_dilatable.h>
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
void Paroi_scal_hyd_base_VEF::associer(const Domaine_dis& domaine_dis,const Domaine_Cl_dis& domaine_Cl_dis)
{
  le_dom_VEF = ref_cast(Domaine_VEF,domaine_dis.valeur());
  le_dom_Cl_VEF = ref_cast(Domaine_Cl_VEF,domaine_Cl_dis.valeur());
  // On initialise tout de suite la loi de paroi
  Paroi_scal_hyd_base_VEF::init_lois_paroi();
}

DoubleVect& Paroi_scal_hyd_base_VEF::equivalent_distance_name(DoubleVect& d_eq, const Nom& nom_bord) const
{
  int nb_boundaries=le_dom_VEF->domaine().nb_front_Cl();
  for (int n_bord=0; n_bord<nb_boundaries; n_bord++)
    {
      const Front_VF& fr_vf = le_dom_VEF->front_VF(n_bord);
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
  int nb_faces_bord_reelles=le_dom_VEF->nb_faces_bord();
  tab_d_reel_.resize(nb_faces_bord_reelles);
  //  positions_Pf_.resize(nb_faces_bord_reelles,dimension);
  // elems_plus_.resize(nb_faces_bord_reelles);

  // Initialisations de equivalent_distance_, tab_d_reel, positions_Pf, elems_plus
  // On initialise les distances equivalentes avec les distances geometriques
  const DoubleTab& face_normales = le_dom_VEF->face_normales();
  //  const DoubleTab& xv = le_dom_VEF->xv();
  const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleVect& volumes_maille = domaine_VEF.volumes();
  const DoubleVect& surfaces_face = domaine_VEF.face_surfaces();

  if (axi)
    {
      Cerr<<"Attention, rien n'est fait en Axi pour le VEF"<<finl;
      exit();
    }
  else
    {
      int nb_front=domaine_VEF.nb_front_Cl();
      equivalent_distance_.dimensionner(nb_front);
      for (int n_bord=0; n_bord<nb_front; n_bord++)
        {
          const Cond_lim& la_cl = le_dom_Cl_VEF->les_conditions_limites(n_bord);
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
              elems_plus_(num_face) = le_dom_VEF->domaine().chercher_elements(positions_Pf_(num_face,0), positions_Pf_(num_face,1));
              else
              elems_plus_(num_face) = le_dom_VEF->domaine().chercher_elements(positions_Pf_(num_face,0), positions_Pf_(num_face,1),positions_Pf_(num_face,2));


              if (elems_plus_(num_face) < 0)
              {
              for (i=0; i<dimension; i++)
              positions_Pf_(num_face,i)=xv(num_face,i) - tab_d_reel_[num_face]*(face_normales(num_face,i)/ratio);

              if (dimension == 2)
              elems_plus_(num_face) = le_dom_VEF->domaine().chercher_elements(positions_Pf_(num_face,0), positions_Pf_(num_face,1));
              else
              elems_plus_(num_face) = le_dom_VEF->domaine().chercher_elements(positions_Pf_(num_face,0), positions_Pf_(num_face,1),positions_Pf_(num_face,2));
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
  const Domaine_VEF& domaine_VEF = le_dom_VEF.valeur();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  int ndeb,nfin,elem;
  const Convection_Diffusion_std& eqn = mon_modele_turb_scal->equation();
  const Equation_base& eqn_hydr = eqn.probleme().equation(0);
  const Fluide_base& le_fluide = ref_cast(Fluide_base, eqn_hydr.milieu());
  const Champ_Don& conductivite = le_fluide.conductivite();
  const DoubleTab& temperature = eqn.probleme().equation(1).inconnue().valeurs();

  const DoubleTab& conductivite_turbulente = mon_modele_turb_scal->conductivite_turbulente().valeurs();

  EcrFicPartage Nusselt;
  ouvrir_fichier_partage(Nusselt,"Nusselt");

  for (int n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = le_dom_Cl_VEF->les_conditions_limites(n_bord);
      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur())) ||
           (sub_type(Dirichlet_paroi_defilante,la_cl.valeur())) ||
           (sub_type(Paroi_decalee_Robin,la_cl.valeur())) )
        {
          const Domaine_Cl_VEF& domaine_Cl_VEF_th = ref_cast(Domaine_Cl_VEF, eqn.probleme().equation(1).domaine_Cl_dis().valeur());
          const Cond_lim& la_cl_th = domaine_Cl_VEF_th.les_conditions_limites(n_bord);
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
              double x=domaine_VEF.xv(num_face,0);
              double y=domaine_VEF.xv(num_face,1);
              double lambda,lambda_t;
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

              lambda_t=conductivite_turbulente(elem);
              if (dimension == 2)
                Nusselt << x << "\t| " << y;
              if (dimension == 3)
                {
                  double z=domaine_VEF.xv(num_face,2);
                  Nusselt << x << "\t| " << y << "\t| " << z;
                }

              // on doit calculer Tfluide premiere maille sans prendre en compte Tparoi
              double tfluide=0.;
              int nb_faces_elem = domaine_VEF.domaine().nb_faces_elem();
              double surface_face=domaine_VEF.face_surfaces(num_face);
              double surface_pond;
              int j;
              for (int i=0; i<nb_faces_elem; i++)
                {
                  if ( (j=elem_faces(elem,i)) != num_face )
                    {
                      surface_pond = 0.;
                      for (int kk=0; kk<dimension; kk++)
                        surface_pond -= (domaine_VEF.face_normales(j,kk)*domaine_VEF.oriente_normale(j,elem)*domaine_VEF.face_normales(num_face,kk)*
                                         domaine_VEF.oriente_normale(num_face,elem))/(surface_face*surface_face);
                      tfluide+=temperature(j)*surface_pond;
                    }
                }

              double d_equiv=equivalent_distance_[n_bord](num_face-ndeb);

              if ( (sub_type(Neumann_paroi,la_cl_th.valeur())))
                {
                  // dans ce cas on va imprimer Tfluide (moyenne premiere maille), Tface et on Tparoi recalcule avec d_equiv
                  const Neumann_paroi& la_cl_neum = ref_cast(Neumann_paroi,la_cl_th.valeur());
                  double tparoi = temperature(num_face);
                  double flux = la_cl_neum.flux_impose(num_face-ndeb);
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

