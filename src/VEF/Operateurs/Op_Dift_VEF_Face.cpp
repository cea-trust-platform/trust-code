/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Op_Dift_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/95
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VEF_Face.h>
#include <Champ_P1NC.h>
#include <Periodique.h>
#include <Scalaire_impose_paroi.h>
#include <Neumann_paroi.h>
#include <Echange_externe_impose.h>
#include <Neumann_homogene.h>
#include <Symetrie.h>
#include <Neumann_sortie_libre.h>
#include <Champ_Uniforme.h>
#include <Debog.h>
#include <TRUSTTrav.h>
#include <Modele_turbulence_scal_base.h>
#include <Porosites_champ.h>
#include <Discretisation_base.h>
#include <Champ.h>
#include <Check_espace_virtuel.h>
#include <Milieu_base.h>

Implemente_instanciable_sans_constructeur(Op_Dift_VEF_Face,"Op_Dift_VEF_P1NC",Op_Dift_VEF_base);

//// printOn
//
Op_Dift_VEF_Face::Op_Dift_VEF_Face():grad_(0) { }

#define grad_transp(elem,i,j) grad(elem,j,i)

Sortie& Op_Dift_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Dift_VEF_Face::readOn(Entree& s )
{
  return s ;
}

void Op_Dift_VEF_Face::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = diffu;
}


double Op_Dift_VEF_Face::calculer_dt_stab() const
{
  // 12/04: Premiers efforts d'optimisation du code a tenir
  // compte lors de la factorisation des pas de temps de stabilite

  // Calcul de dt_stab
  // La diffusivite est constante par elements donc
  // il faut calculer dt_diff pour chaque element et
  //  dt_stab=Min(dt_diff (K) = h(K)*h(K)/(2*dimension*diffu2_(K)))
  // ou diffu2_ est la somme des 2 diffusivite laminaire et turbulente

  //GF
  // alpha_dt_stab=(alpha+alpha_t)*alpha_dt_stab/alpha
  // alpha_dt_stab=(nu+diff_nu_turb)*valeurs_diffusivite_dt/nu

  // On remplit le tableau nu contenant la diffusivite en chaque elem
  remplir_nu(nu_);

  double dt_stab=1.e30;
  double coef;
  const Zone_VEF& la_zone_VEF = la_zone_vef.valeur();
  //const DoubleVect& porosite_elem = la_zone_VEF.porosite_elem();

  const Zone& la_zone= la_zone_VEF.zone();

  DoubleVect diffu_turb(diffusivite_turbulente()->valeurs());
  DoubleTab diffu(nu_);
  if (equation().que_suis_je().debute_par("Convection_Diffusion_Temp"))
    {
      double rhocp = mon_equation->milieu().capacite_calorifique().valeurs()(0, 0) * mon_equation->milieu().masse_volumique().valeurs()(0, 0);
      diffu_turb /= rhocp;
      diffu /= rhocp;
    }
  double alpha;

  int la_zone_nb_elem=la_zone.nb_elem();
  if (has_champ_masse_volumique())
    {
      const DoubleTab& rho_elem = get_champ_masse_volumique().valeurs();
      assert(rho_elem.size_array()==la_zone_VEF.nb_elem_tot());
      for (int num_elem=0; num_elem<la_zone_nb_elem; num_elem++)
        {
          alpha = diffu[num_elem] + diffu_turb[num_elem]; // PQ : 06/03
          alpha/=rho_elem[num_elem];
          // dt=1/(dimension/(pas*pas))/(2*alpha)

          coef=la_zone_VEF.carre_pas_maille(num_elem)/(2.*dimension*(alpha+DMINFLOAT));

          // dt_stab=min(dt);
          if (coef<dt_stab) dt_stab = coef;
        }
    }
  else
    {

      const Champ_base& champ_diffusivite = diffusivite_pour_pas_de_temps();
      const DoubleTab&      valeurs_diffusivite = champ_diffusivite.valeurs();
      double valeurs_diffusivite_dt=-1;
      int unif_diffu_dt;
      const Nature_du_champ nature_champ = equation().inconnue()->nature_du_champ();

      if (sub_type(Champ_Uniforme,champ_diffusivite))
        {
          valeurs_diffusivite_dt=valeurs_diffusivite(0,0);
          unif_diffu_dt=1;
        }
      else
        unif_diffu_dt=0;
      if (nature_champ!=multi_scalaire)
        {
          for (int num_elem=0; num_elem<la_zone_nb_elem; num_elem++)
            {
              alpha =  diffu[num_elem] + diffu_turb[num_elem]; // PQ : 06/03
              if(unif_diffu_dt==0)
                valeurs_diffusivite_dt=valeurs_diffusivite(num_elem,0);
              alpha*=valeurs_diffusivite_dt/(diffu[num_elem]+DMINFLOAT);
              // dt=1/(dimension/(pas*pas))/(2*alpha)
              coef=la_zone_VEF.carre_pas_maille(num_elem)/(2.*dimension*(alpha+DMINFLOAT));
              assert(coef>=0);
              // dt_stab=min(dt);
              if (coef<dt_stab) dt_stab = coef;
            }
        }
      else
        {
          int nb_comp = valeurs_diffusivite.line_size();
          for (int nc=0; nc<nb_comp; nc++)
            {
              for (int num_elem=0; num_elem<la_zone_nb_elem; num_elem++)
                {
                  alpha =  diffu(num_elem,nc) + diffu_turb[num_elem];
                  //if(unif_diffu_dt==0)
                  valeurs_diffusivite_dt=valeurs_diffusivite(0,nc);
                  alpha*=valeurs_diffusivite_dt/(diffu(num_elem,nc)+DMINFLOAT);
                  coef=la_zone_VEF.carre_pas_maille(num_elem)/(2.*dimension*(alpha+DMINFLOAT));

                  if (coef<dt_stab) dt_stab = coef;
                }
            }
        }
    }

  dt_stab = Process::mp_min(dt_stab);
  return dt_stab;
}

// cf Op_Dift_VEF_Face::calculer_dt_stab() pour choix de calcul de dt_stab
void Op_Dift_VEF_Face::calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const
{
  if (Motcle(option)=="stabilite")
    {
      DoubleTab& es_valeurs = espace_stockage->valeurs();

      if (la_zone_vef.non_nul())
        {
          // On remplit le tableau nu contenant la diffusivite en chaque elem
          remplir_nu(nu_);

          double coef;
          const Zone_VEF& la_zone_VEF = la_zone_vef.valeur();
          const Zone& la_zone= la_zone_VEF.zone();
          const DoubleVect& diffu_turb=diffusivite_turbulente()->valeurs();
          double alpha;

          int la_zone_nb_elem=la_zone.nb_elem();
          if (has_champ_masse_volumique())
            {
              const DoubleTab& rho_elem = get_champ_masse_volumique().valeurs();
              assert(rho_elem.size_array()==la_zone_VEF.nb_elem_tot());
              for (int num_elem=0; num_elem<la_zone_nb_elem; num_elem++)
                {
                  alpha = nu_[num_elem] + diffu_turb[num_elem]; // PQ : 06/03
                  alpha/=rho_elem[num_elem];
                  coef=la_zone_VEF.carre_pas_maille(num_elem)/(2.*dimension*alpha);
                  es_valeurs(num_elem) = coef;
                }
            }
          else
            {
              const Champ_base& champ_diffusivite = diffusivite_pour_pas_de_temps();
              const DoubleTab&      valeurs_diffusivite = champ_diffusivite.valeurs();
              double valeurs_diffusivite_dt=-1;
              int unif_diffu_dt;
              if (sub_type(Champ_Uniforme,champ_diffusivite))
                {
                  valeurs_diffusivite_dt=valeurs_diffusivite(0,0);
                  unif_diffu_dt=1;
                }
              else
                unif_diffu_dt=0;
              for (int num_elem=0; num_elem<la_zone_nb_elem; num_elem++)
                {
                  alpha =  nu_[num_elem] + diffu_turb[num_elem]; // PQ : 06/03
                  if(unif_diffu_dt==0)
                    valeurs_diffusivite_dt=valeurs_diffusivite(num_elem);
                  alpha*=valeurs_diffusivite_dt/nu_[num_elem];
                  coef=la_zone_VEF.carre_pas_maille(num_elem)/(2.*dimension*alpha);
                  es_valeurs(num_elem) = coef;
                }
            }

          assert(est_egal(mp_min_vect(es_valeurs),calculer_dt_stab()));
        }
    }
  else
    Op_Dift_VEF_base::calculer_pour_post(espace_stockage,option,comp);
}

Motcle Op_Dift_VEF_Face::get_localisation_pour_post(const Nom& option) const
{
  Motcle loc;
  if (Motcle(option)=="stabilite")
    loc = "elem";
  else
    return Op_Dift_VEF_base::get_localisation_pour_post(option);
  return loc;
}
void Op_Dift_VEF_Face::ajouter_cas_scalaire(const DoubleVect& inconnue,
                                            DoubleVect& resu, DoubleTab& tab_flux_bords,
                                            const DoubleVect& nu,
                                            const DoubleVect& nu_turb,
                                            const Zone_Cl_VEF& zone_Cl_VEF,
                                            const Zone_VEF& zone_VEF ) const
{
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int i,j,num_face;
  int nb_faces = zone_VEF.nb_faces();
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  double valA, d_nu;
  int nb_front=zone_VEF.nb_front_Cl();

  // modif pour imprimer les flux sur les bords
  int size_flux_bords=zone_VEF.nb_faces_bord();
  tab_flux_bords.resize(size_flux_bords,1);
  tab_flux_bords=0.;

  const int premiere_face_int=zone_VEF.premiere_face_int();

  // contient -1 si la face n'est pas periodique et numero face_assso sinon
  ArrOfInt marq( zone_VEF.nb_faces_tot());
  marq=-1;
  // On traite les faces bord
  for (int n_bord=0; n_bord<nb_front; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = 0;
      int num2 = le_bord.nb_faces_tot();
      int nb_faces_bord_reel = le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              fac_asso = la_cl_perio.face_associee(ind_face);
              fac_asso = le_bord.num_face(fac_asso);
              num_face = le_bord.num_face(ind_face);
              marq(num_face)=fac_asso;
              for (int kk=0; kk<2; kk++)
                {
                  int elem = face_voisins(num_face,kk);
                  d_nu = nu(elem)+nu_turb(elem);
                  for (i=0; i<nb_faces_elem; i++)
                    {
                      if ( ( (j= elem_faces(elem,i)) > num_face ) && (j != fac_asso) )
                        {
                          valA = viscA(num_face,j,elem,d_nu);
                          resu(num_face)+=valA*inconnue(j);
                          resu(num_face)-=valA*inconnue(num_face);
                          if(j<nb_faces) // face reelle
                            {
                              resu(j)+=0.5*valA*inconnue(num_face);
                              resu(j)-=0.5*valA*inconnue(j);
                            }
                        }
                    }
                }
            }
        }
      else   // Il n'y a qu'une seule composante, donc on traite
        // une equation scalaire (pas la vitesse) on a pas a utiliser
        // le tau tangentiel (les lois de paroi thermiques ne calculent pas
        // d'echange turbulent a la paroi pour l'instant
        {
          const DoubleTab& face_normale = zone_VEF.face_normales();
          const DoubleVect& vol = zone_VEF.volumes();
          const Equation_base& my_eqn = zone_Cl_VEF.equation();
          const RefObjU& modele_turbulence = my_eqn.get_modele(TURBULENCE);
          if (sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()))
            {
              const Modele_turbulence_scal_base& mod_turb_scal = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
              const Turbulence_paroi_scal& loiparth = mod_turb_scal.loi_paroi();
              if (loiparth->use_equivalent_distance())
                {
                  const DoubleVect& d_equiv=loiparth->equivalent_distance(n_bord);
                  // d_equiv contient la distance equivalente pour le bord
                  // Dans d_equiv, pour les faces qui ne sont pas paroi_fixe (eg periodique, symetrie, etc...)
                  // il y a la distance geometrique grace a l'initialisation du tableau dans la loi de paroi.

                  // Les lois de parois ne s'appliquent qu'aux cas ou la CL
                  // est de type temperature imposee, car dans les autres cas
                  // (flux impose et adiabatique) le flux a la paroi est connu et fixe.
                  int nb_dim_pb=Objet_U::dimension;
                  const Cond_lim_base& cl_base=la_cl.valeur();
                  int ldp_appli=0;
                  if (sub_type(Scalaire_impose_paroi,cl_base))
                    ldp_appli=1;
                  else if (loiparth->get_flag_calcul_ldp_en_flux_impose() )
                    {
                      if ((sub_type(Neumann_paroi,cl_base))||(sub_type(Neumann_homogene,cl_base)))
                        ldp_appli=1;
                    }
                  if (ldp_appli)
                    {
                      DoubleVect le_mauvais_gradient(nb_dim_pb);
                      for (int ind_face=num1; ind_face<num2; ind_face++)
                        {
                          // Tf est la temperature fluide moyenne dans le premier element
                          // sans tenir compte de la temperature de paroi.
                          double Tf=0.;
                          double bon_gradient=0.; // c'est la norme du gradient de temperature normal a la paroi calculee a l'aide de la loi de paroi.
                          le_mauvais_gradient=0.;
                          num_face = le_bord.num_face(ind_face);
                          int elem1 = face_voisins(num_face,0);
                          if(elem1==-1) elem1 = face_voisins(num_face,1);
                          double surface_face=zone_VEF.face_surfaces(num_face);
                          double surface_pond;

                          for (i=0; i<nb_faces_elem; i++)
                            {
                              if ( (j= elem_faces(elem1,i)) != num_face )
                                {
                                  surface_pond = 0.;
                                  for (int kk=0; kk<nb_dim_pb; kk++)
                                    surface_pond -= (face_normale(j,kk)*zone_VEF.oriente_normale(j,elem1)*face_normale(num_face,kk)*
                                                     zone_VEF.oriente_normale(num_face,elem1))/(surface_face*surface_face);
                                  Tf+=inconnue(j)*surface_pond;
                                }

                              for(int kk=0; kk<nb_dim_pb; kk++)
                                le_mauvais_gradient(kk)+=inconnue(j)*face_normale(j,kk)*zone_VEF.oriente_normale(j,elem1);
                            }
                          le_mauvais_gradient/=vol(elem1);
                          // mauvais_gradient = le_mauvais_gradient.n
                          double mauvais_gradient=0;
                          for(int kk=0; kk<nb_dim_pb; kk++)
                            mauvais_gradient+=le_mauvais_gradient(kk)*face_normale(num_face,kk)/surface_face;

                          // inconnue(num_face) est la temperature de paroi : Tw.
                          // On se fiche du signe de bon gradient car c'est la norme du gradient
                          // de temperature dans l'element.
                          // Ensuite ce sera multiplie par le vecteur normal a la face de paroi
                          // qui lui a les bons signes.
                          bon_gradient=(Tf-inconnue(num_face))/d_equiv(ind_face)*(-zone_VEF.oriente_normale(num_face,elem1));

                          double nutotal=nu(elem1)+nu_turb(elem1);
                          for (i=0; i<nb_faces_elem; i++)
                            {
                              j= elem_faces(elem1,i);
                              double correction=0.;
                              for(int kk=0; kk<nb_dim_pb; kk++)
                                {
                                  double resu2 = nutotal*(bon_gradient-mauvais_gradient)*face_normale(num_face,kk)*face_normale(j,kk)* (-zone_VEF.oriente_normale(j,elem1))/surface_face;

                                  correction+=resu2;
                                }

                              resu(j)+=correction;

                              if (marq(j)!=-1)
                                resu(marq(j))+=correction;
                              // la face num_face n'est pas periodique  mar(num_face)==-1
                              // flux_bord n'est necessaire que sur les faces reelles
                              if(j==num_face && j<size_flux_bords)
                                {
                                  //      flux_bords(j,0)+=resu1;
                                  tab_flux_bords(j,0)-=correction;
                                }
                            }
                        }
                    }// CL Temperature impose
                }// loi de paroi
            }//Equation Convection_Diffusion_Turbulente

          // Fin de la correction du gradient dans la premiere maille
          // par la loi de paroi thermique.
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              num_face = le_bord.num_face(ind_face);
              int elem1 = face_voisins(num_face,0);
              d_nu = nu(elem1)+nu_turb(elem1);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if (( (j= elem_faces(elem1,i)) > num_face ) || (ind_face>=nb_faces_bord_reel))
                    {
                      valA = viscA(num_face,j,elem1,d_nu);
                      if (ind_face<nb_faces_bord_reel)
                        {
                          double flux=valA*(inconnue(j)-inconnue(num_face));
                          resu(num_face)+=flux;
                          tab_flux_bords(num_face,0) -= flux;
                        }
                      if(j<nb_faces) // face reelle
                        {
                          double  flux=valA*(inconnue(num_face)-inconnue(j));
                          if (j<premiere_face_int)
                            tab_flux_bords(j,0)-=flux;
                          resu(j)+=flux;
                        }
                    }
                }
            }
        }
    }

  // Faces internes :

  for (num_face=premiere_face_int; num_face<nb_faces; num_face++)
    {
      for (int kk=0; kk<2; kk++)
        {
          int elem = face_voisins(num_face,kk);
          d_nu = nu(elem)+nu_turb(elem);
          for (i=0; i<nb_faces_elem; i++)
            {
              if ( (j= elem_faces(elem,i)) > num_face )
                {
                  int el1,el2;
                  int contrib=1;
                  if(j>=nb_faces) // C'est une face virtuelle
                    {
                      el1 = face_voisins(j,0);
                      el2 = face_voisins(j,1);
                      if((el1==-1)||(el2==-1))
                        contrib=0;
                    }
                  if(contrib)
                    {
                      valA = viscA(num_face,j,elem,d_nu);
                      resu(num_face)+=valA*inconnue(j);
                      resu(num_face)-=valA*inconnue(num_face);
                      if(j<nb_faces) // On traite les faces reelles
                        {
                          resu(j)+=valA*inconnue(num_face);
                          resu(j)-=valA*inconnue(j);
                        }
                    }
                }
            }
        }
    }

  // Neumann :
  for (int n_bord=0; n_bord<nb_front; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              resu[face] += la_cl_paroi.flux_impose(face-ndeb)*zone_VEF.face_surfaces(face);
              tab_flux_bords(face,0) = la_cl_paroi.flux_impose(face-ndeb)*zone_VEF.face_surfaces(face);
            }
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              resu[face] += la_cl_paroi.h_imp(face-ndeb)*(la_cl_paroi.T_ext(face-ndeb)-inconnue(face))*zone_VEF.face_surfaces(face);
              tab_flux_bords(face,0) = la_cl_paroi.h_imp(face-ndeb)*(la_cl_paroi.T_ext(face-ndeb)-inconnue(face))*zone_VEF.face_surfaces(face);
            }
        }
      else if (sub_type(Neumann_homogene,la_cl.valeur())
               || sub_type(Symetrie,la_cl.valeur())
               || sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            tab_flux_bords(face,0) = 0.;
        }
    }
}

void Op_Dift_VEF_Face::ajouter_cas_vectoriel(const DoubleTab& inconnue,
                                             DoubleTab& resu, DoubleTab& tab_flux_bords,
                                             const DoubleTab& nu,
                                             const DoubleTab& nu_turb,
                                             const Zone_Cl_VEF& zone_Cl_VEF,
                                             const Zone_VEF& zone_VEF,
                                             int nbr_comp
                                            ) const
{
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nb_faces = zone_VEF.nb_faces();
  int nb_elem = zone_VEF.nb_elem();
  const DoubleTab& face_normale = zone_VEF.face_normales();

  assert(nbr_comp>1);

  // On dimensionne et initialise le tableau des bilans de flux:
  static_cast<DoubleTab&>(tab_flux_bords).resize(zone_VEF.nb_faces_bord(),nbr_comp);
  tab_flux_bords=0.;

  // Construction du tableau grad_ si necessaire
  if(!grad_.get_md_vector().non_nul())
    {
      grad_.resize(0, Objet_U::dimension, Objet_U::dimension);
      zone_VEF.zone().creer_tableau_elements(grad_);
    }
  grad_=0.;

  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  int nb_cl=les_cl.size();

  Champ_P1NC::calcul_gradient(inconnue,grad_,zone_Cl_VEF);
  if (le_modele_turbulence.valeur().utiliser_loi_paroi())
    Champ_P1NC::calcul_duidxj_paroi(grad_,nu,nu_turb,tau_tan_,zone_Cl_VEF);

  grad_.echange_espace_virtuel();

  DoubleTab Re;
  Re.resize(0, Objet_U::dimension, Objet_U::dimension);
  zone_VEF.zone().creer_tableau_elements(Re);
  Re = 0.;

  if (le_modele_turbulence.valeur().calcul_tenseur_Re(nu_turb, grad_, Re))
    {
      Cerr << "On utilise une diffusion turbulente non linaire dans NS" << finl;
      for (int elem=0; elem<nb_elem; elem++)
        for (int i=0; i<nbr_comp; i++)
          for (int j=0; j<nbr_comp; j++)
            Re(elem,i,j) *= nu_turb[elem];
    }
  else
    {
      for (int elem=0; elem<nb_elem; elem++)
        for (int i=0; i<nbr_comp; i++)
          for (int j=0; j<nbr_comp; j++)
            Re(elem,i,j) = nu_turb[elem]*(grad_(elem,i,j) + grad_(elem,j,i));
    }
  Re.echange_espace_virtuel();


  // Calcul du terme diffusif
  // boucle sur les CL
  for (int n_bord=0; n_bord<nb_cl; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          for (int num_face=num1; num_face<num2; num_face++)
            {
              for (int kk=0; kk<2; kk++)
                {
                  int elem = face_voisins(num_face,kk);
                  int ori = 1-2*kk;

                  for (int i=0; i<nbr_comp; i++)
                    for (int j=0; j<nbr_comp; j++)
                      resu(num_face,i) -= ori*face_normale(num_face,j)
                                          *(nu[elem]*grad_(elem,i,j) + Re(elem,i,j));
                } // Fin de la boucle sur les elements ayant la face comnune
            } // Fin de la boucle sur les faces
        } // Fin du cas periodique
      else
        {
          for (int num_face=num1; num_face<num2; num_face++)
            {
              int elem=face_voisins(num_face,0);

              for (int i=0; i<nbr_comp; i++)
                for (int j=0; j<nbr_comp; j++)
                  {
                    double flux = face_normale(num_face,j)
                                  *(nu[elem]*grad_(elem,i,j) + Re(elem,i,j));
                    resu(num_face,i) -= flux;
                    tab_flux_bords(num_face,i) -= flux;
                  }
            } // Fin de la boucle sur les faces
        }
    } // Fin du traitement des CL

  /////////////////
  // Faces internes
  /////////////////
  int n0 = zone_VEF.premiere_face_int();
  for (int num_face=n0; num_face<nb_faces; num_face++)
    {
      for (int kk=0; kk<2; kk++)
        {
          int elem = face_voisins(num_face,kk);
          int ori = 1-2*kk;

          for (int i=0; i<nbr_comp; i++)
            for (int j=0; j<nbr_comp; j++)
              {
                resu(num_face,i) -= ori*face_normale(num_face,j)
                                    *(nu[elem]*grad_(elem,i,j) + Re(elem,i,j));
              }
        } // Fin de la boucle sur les 2 elements comnuns a la face
    } // Fin de la boucle sur les faces internes

  // Symmetry condition
  for (int n_bord=0; n_bord<nb_cl; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Symetrie,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            tab_flux_bords(face,0) = 0.;
        }
    }
}

void Op_Dift_VEF_Face::ajouter_cas_multi_scalaire(const DoubleTab& inconnue,
                                                  DoubleTab& resu, DoubleTab& tab_flux_bords,
                                                  const DoubleTab& nu,
                                                  const DoubleVect& nu_turb,
                                                  const Zone_Cl_VEF& zone_Cl_VEF,
                                                  const Zone_VEF& zone_VEF,
                                                  int nb_comp) const
{
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int i,j,num_face;
  int nb_faces = zone_VEF.nb_faces();
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  double valA, d_nu;
  int nb_front=zone_VEF.nb_front_Cl();

  // modif pour imprimer les flux sur les bords
  int size_flux_bords=zone_VEF.nb_faces_bord();
  tab_flux_bords.resize(size_flux_bords,nb_comp);
  tab_flux_bords=0.;
  // contient -1 si la face n'est pas periodique et numero face_assso sinon
  ArrOfInt marq( zone_VEF.nb_faces_tot());
  marq=-1;
  // On traite les faces bord
  for (int n_bord=0; n_bord<nb_front; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = 0;
      int num2 = le_bord.nb_faces_tot();
      int nb_faces_bord_reel = le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              fac_asso = la_cl_perio.face_associee(ind_face);
              fac_asso = le_bord.num_face(fac_asso);
              num_face = le_bord.num_face(ind_face);
              marq(num_face)=fac_asso;
              for (int kk=0; kk<2; kk++)
                {
                  int elem = face_voisins(num_face,kk);
                  for (i=0; i<nb_faces_elem; i++)
                    {
                      if ( ( (j= elem_faces(elem,i)) > num_face ) && (j != fac_asso) )
                        {
                          for (int nc=0; nc<nb_comp; nc++)
                            {
                              d_nu = nu(elem,nc)+nu_turb(elem);
                              valA = viscA(num_face,j,elem,d_nu);
                              resu(num_face,nc)+=valA*inconnue(j,nc);
                              resu(num_face,nc)-=valA*inconnue(num_face,nc);
                              if(j<nb_faces) // face reelle
                                {
                                  resu(j,nc)+=0.5*valA*inconnue(num_face,nc);
                                  resu(j,nc)-=0.5*valA*inconnue(j,nc);
                                }
                            }
                        }
                    }
                }
            }
        }
      else   // on traite une equation scalaire (pas la vitesse) on a pas a utiliser
        // le tau tangentiel (les lois de paroi thermiques ne calculent pas
        // d'echange turbulent a la paroi pour l'instant
        {
          const DoubleTab& face_normale = zone_VEF.face_normales();
          const DoubleVect& vol = zone_VEF.volumes();
          const Equation_base& my_eqn = zone_Cl_VEF.equation();
          const RefObjU& modele_turbulence = my_eqn.get_modele(TURBULENCE);
          if (sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()))
            {
              const Modele_turbulence_scal_base& mod_turb_scal = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
              const Turbulence_paroi_scal& loiparth = mod_turb_scal.loi_paroi();
              if (loiparth->use_equivalent_distance())
                {
                  const DoubleVect& d_equiv=loiparth->equivalent_distance(n_bord);
                  // d_equiv contient la distance equivalente pour le bord
                  // Dans d_equiv, pour les faces qui ne sont pas paroi_fixe (eg periodique, symetrie, etc...)
                  // il y a la distance geometrique grace a l'initialisation du tableau dans la loi de paroi.

                  // Les lois de parois ne s'appliquent qu'aux cas ou la CL
                  // est de type temperature imposee, car dans les autres cas
                  // (flux impose et adiabatique) le flux a la paroi est connu et fixe.
                  int nb_dim_pb=Objet_U::dimension;
                  if(sub_type(Scalaire_impose_paroi,la_cl.valeur()))
                    {
                      DoubleVect le_mauvais_gradient(nb_dim_pb);
                      for (int ind_face=num1; ind_face<num2; ind_face++)
                        {
                          for (int nc=0; nc<nb_comp; nc++)
                            {
                              // Tf est la temperature fluide moyenne dans le premier element
                              // sans tenir compte de la temperature de paroi.
                              double Tf=0.;
                              double bon_gradient=0.; // c'est la norme du gradient de temperature normal a la paroi calculee a l'aide de la loi de paroi.
                              le_mauvais_gradient=0.;
                              num_face = le_bord.num_face(ind_face);
                              int elem1 = face_voisins(num_face,0);
                              if(elem1==-1) elem1 = face_voisins(num_face,1);
                              double surface_face=zone_VEF.face_surfaces(num_face);
                              double surface_pond;

                              for (i=0; i<nb_faces_elem; i++)
                                {
                                  if ( (j= elem_faces(elem1,i)) != num_face )
                                    {
                                      surface_pond = 0.;
                                      for (int kk=0; kk<nb_dim_pb; kk++)
                                        surface_pond -= (face_normale(j,kk)*zone_VEF.oriente_normale(j,elem1)*face_normale(num_face,kk)*
                                                         zone_VEF.oriente_normale(num_face,elem1))/(surface_face*surface_face);
                                      Tf+=inconnue(j,nc)*surface_pond;
                                    }

                                  for(int kk=0; kk<nb_dim_pb; kk++)
                                    le_mauvais_gradient(kk)+=inconnue(j,nc)*face_normale(j,kk)*zone_VEF.oriente_normale(j,elem1);
                                }
                              le_mauvais_gradient/=vol(elem1);
                              // mauvais_gradient = le_mauvais_gradient.n
                              double mauvais_gradient=0;
                              for(int kk=0; kk<nb_dim_pb; kk++)
                                mauvais_gradient+=le_mauvais_gradient(kk)*face_normale(num_face,kk)/surface_face;

                              // inconnue(num_face) est la temperature de paroi : Tw.
                              // On se fiche du signe de bon gradient car c'est la norme du gradient
                              // de temperature dans l'element.
                              // Ensuite ce sera multiplie par le vecteur normal a la face de paroi
                              // qui lui a les bons signes.
                              bon_gradient=(Tf-inconnue(num_face,nc))/d_equiv(ind_face)*(-zone_VEF.oriente_normale(num_face,elem1));

                              double nutotal=nu(elem1,nc)+nu_turb(elem1);
                              for (i=0; i<nb_faces_elem; i++)
                                {
                                  j= elem_faces(elem1,i);
                                  double correction=0.;
                                  for(int kk=0; kk<nb_dim_pb; kk++)
                                    {
                                      double resu2 = nutotal*(bon_gradient-mauvais_gradient)*face_normale(num_face,kk)*face_normale(j,kk)* (-zone_VEF.oriente_normale(j,elem1))/surface_face;

                                      correction+=resu2;
                                    }

                                  resu(j,nc)+=correction;

                                  if (marq(j)!=-1)
                                    resu(marq(j),nc)+=correction;
                                  // la face num_face n'est pas periodique  mar(num_face)==-1
                                  // flux_bord n'est necessaire que sur les faces reelles
                                  if(j==num_face && j<size_flux_bords)
                                    {
                                      //      flux_bords(j,0)+=resu1;
                                      tab_flux_bords(j,nc)-=correction;
                                    }
                                }
                            }
                        }
                    }// CL Temperature imposee
                }// loi de paroi
            }//Equation Convection_Diffusion_Turbulente

          // Fin de la correction du gradient dans la premiere maille
          // par la loi de paroi thermique.
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              num_face = le_bord.num_face(ind_face);
              int elem1 = face_voisins(num_face,0);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if (( (j= elem_faces(elem1,i)) > num_face ) || (ind_face>=nb_faces_bord_reel))
                    {
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          d_nu = nu(elem1,nc)+nu_turb(elem1);
                          valA = viscA(num_face,j,elem1,d_nu);
                          if (ind_face<nb_faces_bord_reel)
                            {
                              resu(num_face,nc)+=valA*inconnue(j,nc);
                              resu(num_face,nc)-=valA*inconnue(num_face,nc);
                              // modif pour imprimer les flux sur les bords
                              tab_flux_bords(num_face,nc) -= valA*(inconnue(j,nc)-inconnue(num_face,nc));
                            }
                          if(j<nb_faces) // face reelle
                            {
                              resu(j,nc)+=valA*inconnue(num_face,nc);
                              resu(j,nc)-=valA*inconnue(j,nc);
                            }
                        }
                    }
                }
            }
        }
    }

  // Faces internes :
  const int premiere_face_int=zone_VEF.premiere_face_int();
  for (num_face=premiere_face_int; num_face<nb_faces; num_face++)
    {
      for (int kk=0; kk<2; kk++)
        {
          int elem = face_voisins(num_face,kk);
          for (i=0; i<nb_faces_elem; i++)
            {
              if ( (j= elem_faces(elem,i)) > num_face )
                {
                  int el1,el2;
                  int contrib=1;
                  if(j>=nb_faces) // C'est une face virtuelle
                    {
                      el1 = face_voisins(j,0);
                      el2 = face_voisins(j,1);
                      if((el1==-1)||(el2==-1))
                        contrib=0;
                    }
                  if(contrib)
                    {
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          d_nu = nu(elem,nc)+nu_turb(elem);
                          valA = viscA(num_face,j,elem,d_nu);
                          resu(num_face,nc)+=valA*inconnue(j,nc);
                          resu(num_face,nc)-=valA*inconnue(num_face,nc);
                          if(j<nb_faces) // On traite les faces reelles
                            {
                              resu(j,nc)+=valA*inconnue(num_face,nc);
                              resu(j,nc)-=valA*inconnue(j,nc);
                            }
                        }
                    }
                }
            }
        }
    }

  // Neumann :
  for (int n_bord=0; n_bord<nb_front; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              for (int nc=0; nc<nb_comp; nc++)
                {
                  resu(face,nc) += la_cl_paroi.flux_impose(face-ndeb,nc)*zone_VEF.face_surfaces(face);
                  tab_flux_bords(face,nc) = la_cl_paroi.flux_impose(face-ndeb,nc)*zone_VEF.face_surfaces(face);
                }
            }
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            {
              for (int nc=0; nc<nb_comp; nc++)
                {
                  resu(face,nc) += la_cl_paroi.h_imp(face-ndeb,nc)*(la_cl_paroi.T_ext(face-ndeb,nc)-inconnue(face,nc))*zone_VEF.face_surfaces(face);
                  tab_flux_bords(face,nc) = la_cl_paroi.h_imp(face-ndeb,nc)*(la_cl_paroi.T_ext(face-ndeb,nc)-inconnue(face,nc))*zone_VEF.face_surfaces(face);
                }
            }
        }
      else if (sub_type(Neumann_homogene,la_cl.valeur())
               || sub_type(Symetrie,la_cl.valeur())
               || sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            for (int nc=0; nc<nb_comp; nc++)
              tab_flux_bords(face,nc) = 0.;
        }
    }
}

DoubleTab& Op_Dift_VEF_Face::ajouter(const DoubleTab& inconnue_org,
                                     DoubleTab& resu) const
{
  remplir_nu(nu_);
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const DoubleTab& nu_turb=diffusivite_turbulente()->valeurs();
  const int nb_comp = resu.line_size();

  DoubleTab nu,nu_turb_m;
  DoubleTab tab_inconnue;
  int marq=phi_psi_diffuse(equation());
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  modif_par_porosite_si_flag(nu_turb,nu_turb_m,!marq,porosite_elem);
  const DoubleTab& inconnue=modif_par_porosite_si_flag(inconnue_org,tab_inconnue,marq,porosite_face);

  const Champ_base& inco = equation().inconnue().valeur();
  const Nature_du_champ nature_champ = inco.nature_du_champ();

  assert_espace_virtuel_vect(nu);
  assert_espace_virtuel_vect(inconnue);
  assert_espace_virtuel_vect(nu_turb_m);
  Debog::verifier("Op_Dift_VEF_Face::ajouter nu",nu);
  Debog::verifier("Op_Dift_VEF_Face::ajouter nu_turb",nu_turb_m);
  Debog::verifier("Op_Dift_VEF_Face::ajouter inconnue_org",inconnue_org);
  Debog::verifier("Op_Dift_VEF_Face::ajouter inconnue",inconnue);


  if(nature_champ==scalaire)
    ajouter_cas_scalaire(inconnue, resu, flux_bords_, nu, nu_turb_m, zone_Cl_VEF, zone_VEF);
  else if (nature_champ==vectoriel)
    ajouter_cas_vectoriel(inconnue, resu, flux_bords_, nu, nu_turb_m, zone_Cl_VEF, zone_VEF, nb_comp);
  else if (nature_champ==multi_scalaire)
    ajouter_cas_multi_scalaire(inconnue, resu, flux_bords_, nu, nu_turb_m, zone_Cl_VEF, zone_VEF, nb_comp);
  modifier_flux(*this);

  return resu;
}

DoubleTab& Op_Dift_VEF_Face::calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inconnue,resu);
}



/////////////////////////////////////////
// Methodes pour l'implicite
/////////////////////////////////////////
void Op_Dift_VEF_Face::ajouter_contribution_cl(const DoubleTab& transporte, Matrice_Morse& matrice, const DoubleTab& nu, const DoubleTab& nu_turb, const DoubleVect&
                                               porosite_eventuelle) const
{
  // On traite les faces bord
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleVect& volumes = zone_VEF.volumes();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  int nb_faces = zone_VEF.nb_faces();
  const int nb_comp =transporte.line_size();

  int nb_bords=zone_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = 0;
      int num2 = le_bord.nb_faces_tot();
      int nb_faces_bord_reel = le_bord.nb_faces();
      int j;
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          // on ne parcourt que la moitie des faces volontairement...
          // GF il ne faut pas s'occuper des faces virtuelles
          num2=nb_faces_bord_reel/2;
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              int fac_asso = la_cl_perio.face_associee(ind_face);
              fac_asso = le_bord.num_face(fac_asso);
              int num_face = le_bord.num_face(ind_face);
              for (int l=0; l<2; l++)
                {
                  int elem = face_voisins(num_face,l);
                  double d_nu = nu(elem)+nu_turb(elem);
                  for (int i=0; i<nb_faces_elem; i++)
                    {
                      if ( (j= elem_faces(elem,i)) > num_face )
                        {
                          double valA = viscA(num_face,j,elem,d_nu);
                          int orientation = 1;
                          if ( (elem == face_voisins(j,l)) || (face_voisins(num_face,(l+1)%2) == face_voisins(j,(l+1)%2)))
                            orientation = -1;
                          int fac_loc=0;
                          int ok=1;
                          while ((fac_loc<nb_faces_elem) && (elem_faces(elem,fac_loc)!=num_face)) fac_loc++;
                          if (fac_loc==nb_faces_elem)
                            ok=0;
                          int contrib=1;
                          if(j>=nb_faces) // C'est une face virtuelle
                            {
                              int el1 = face_voisins(j,0);
                              int el2 = face_voisins(j,1);
                              if((el1==-1)||(el2==-1))
                                contrib=0;
                            }
                          if (contrib)
                            {
                              for (int nc=0; nc<nb_comp; nc++)
                                {
                                  int n0=num_face*nb_comp+nc;
                                  int n0perio=fac_asso*nb_comp+nc;
                                  int j0=j*nb_comp+nc;
                                  matrice(n0,n0)+=valA*porosite_eventuelle(num_face);
                                  matrice(n0,j0)-=valA*porosite_eventuelle(j);
                                  if(j<nb_faces) // On traite les faces reelles
                                    {
                                      //if (l==0)
                                      if (ok==1)
                                        matrice(j0,n0)-=valA*porosite_eventuelle(num_face);
                                      else
                                        matrice(j0,n0perio)-=valA*porosite_eventuelle(num_face);
                                      matrice(j0,j0)+=valA*porosite_eventuelle(j);
                                    }
                                  if ( nb_comp>1) // on ajoute grad_U transpose
                                    for (int nc2=0; nc2<nb_comp; nc2++)
                                      {
                                        int n1=num_face*nb_comp+nc2;
                                        int j1=j*nb_comp+nc2;
                                        double coeff_s=orientation*nu_turb(elem)/volumes(elem)*face_normale(num_face,nc2)*face_normale(j,nc);
                                        matrice(n0,n1)+=coeff_s*porosite_eventuelle(num_face);
                                        matrice(n0,j1)-= coeff_s*porosite_eventuelle(j);
                                        if(j<nb_faces) // On traite les faces reelles
                                          {
                                            double coeff_s2=orientation*nu_turb(elem)/volumes(elem)*face_normale(num_face,nc)*face_normale(j,nc2);
                                            if (ok==1)
                                              matrice(j0,n1)-=coeff_s2*porosite_eventuelle(num_face);
                                            else
                                              matrice(j0,fac_asso*nb_comp+nc2)-=coeff_s2*porosite_eventuelle(num_face);
                                            matrice(j0,j1)+=coeff_s2*porosite_eventuelle(j);
                                          }
                                      }
                                }
                            }
                        }
                    }
                }
            }
        }
      else
        {
          // pas perio
          // correction dans le cas dirichlet sur paroi temperature
          if(sub_type(Scalaire_impose_paroi,la_cl.valeur()))
            {
              const Equation_base& my_eqn = zone_Cl_VEF.equation();
              const RefObjU& modele_turbulence = my_eqn.get_modele(TURBULENCE);
              if (sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()))
                {
                  const Modele_turbulence_scal_base& mod_turb_scal = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
                  const Turbulence_paroi_scal& loiparth = mod_turb_scal.loi_paroi();
                  if (loiparth->use_equivalent_distance())
                    {
                      //const DoubleTab& face_normale = zone_VEF.face_normales();
                      const DoubleVect& vol = zone_VEF.volumes();
                      const DoubleVect& d_equiv=loiparth->equivalent_distance(n_bord);
                      int nb_dim_pb=Objet_U::dimension;

                      DoubleVect le_mauvais_gradient(nb_dim_pb);
                      for (int ind_face=num1; ind_face<num2; ind_face++)
                        {
                          int num_face = le_bord.num_face(ind_face);
                          // Tf est la temperature fluide moyenne dans le premier element
                          // sans tenir compte de la temperature de paroi.
                          double Tf=0.;
                          double bon_gradient=0.; // c'est la norme du gradient de temperature normal a la paroi
                          // calculee a l'aide de la loi de paroi.

                          int elem1 = face_voisins(num_face,0);
                          if (elem1==-1) elem1 = face_voisins(num_face,1);

                          // inconnue(num_face) est la temperature de paroi : Tw.
                          // On se fiche du signe de bon gradient car c'est la norme du gradient
                          // de temperature dans l'element.
                          // Ensuite ce sera multiplie par le vecteur normal a la face de paroi
                          // qui lui a les bons signes.
                          //bon_gradient=(Tf-inconnue(num_face))/d_equiv(num_face);
                          bon_gradient=1./d_equiv(ind_face)*(-zone_VEF.oriente_normale(num_face,elem1));

                          double surface_face=zone_VEF.face_surfaces(num_face);
                          double nutotal=nu(elem1)+nu_turb(elem1);
                          for (int i=0; i<nb_faces_elem; i++)
                            {
                              j= elem_faces(elem1,i);

                              for (int ii=0; ii<nb_faces_elem; ii++)
                                {
                                  le_mauvais_gradient=0;
                                  int jj=elem_faces(elem1,ii);
                                  double surface_pond=0;
                                  for (int kk=0; kk<nb_dim_pb; kk++)
                                    surface_pond -= (face_normale(jj,kk)*zone_VEF.oriente_normale(jj,elem1)*face_normale(num_face,kk)*
                                                     zone_VEF.oriente_normale(num_face,elem1))/(surface_face*surface_face);
                                  Tf=surface_pond;
                                  //Tf=1./(nb_faces_elem-1); // Temperature moyenne.
                                  for(int kk=0; kk<nb_dim_pb; kk++)
                                    le_mauvais_gradient(kk)+=face_normale(jj,kk)*zone_VEF.oriente_normale(jj,elem1);
                                  le_mauvais_gradient/=vol(elem1);
                                  double mauvais_gradient=0;
                                  //         double surface_face=zone_VEF.face_surfaces(num_face);
                                  for(int kk=0; kk<nb_dim_pb; kk++)
                                    mauvais_gradient+=le_mauvais_gradient(kk)*face_normale(num_face,kk)/surface_face;
                                  double resu1=0,resu2=0;
                                  for(int kk=0; kk<nb_dim_pb; kk++)
                                    {
                                      // resu1 += nutotal*le_mauvais_gradient(kk)*face_normale(j,kk)*(-zone_VEF.oriente_normale(j,elem1));
                                      resu1 += nutotal*mauvais_gradient*face_normale(num_face,kk)*face_normale(j,kk)*(-zone_VEF.oriente_normale(j,elem1))/surface_face;
                                      resu2 += nutotal*bon_gradient    *face_normale(num_face,kk)*face_normale(j,kk)*(-zone_VEF.oriente_normale(j,elem1))/surface_face;
                                    }
                                  // bon gradient_reel =bongradient*(Tf-T_face) d'ou les derivees...
                                  // mauvais gradient_reel=mauvai_gradient_j*Tj
                                  if (jj==num_face)
                                    resu2*=-1;
                                  else
                                    resu2*=Tf;
                                  matrice(j,jj)+=(resu1-resu2)*porosite_eventuelle(jj);
                                }
                            }
                        }
                    }// loi de paroi
                }//Equation Convection_Diffusion_Turbulente
            } // CL Temperature imposee
          else if  (sub_type(Echange_externe_impose,la_cl.valeur()))
            {
              const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
              //const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();
              for (int face=ndeb; face<nfin; face++)
                {
                  matrice(face,face) += la_cl_paroi.h_imp(face-ndeb)*zone_VEF.face_surfaces(face);
                }
            }

          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              int elem=face_voisins(num_face,0);
              double d_nu = nu(elem)+nu_turb(elem);
              // Boucle sur les faces :
              for (int i=0; i<nb_faces_elem; i++)
                if (( (j= elem_faces(elem,i)) > num_face ) || (ind_face>=nb_faces_bord_reel))
                  {
                    double valA = viscA(num_face,j,elem,d_nu);
                    int orientation = 1;
                    if ( (elem == face_voisins(j,0)) || (face_voisins(num_face,(0+1)%2) == face_voisins(j,(0+1)%2)))
                      orientation = -1;
                    // retire terme croise pour l'intant
                    //orientation=0;
                    for (int nc=0; nc<nb_comp; nc++)
                      {
                        int n0=num_face*nb_comp+nc;
                        int j0=j*nb_comp+nc;
                        if (ind_face<nb_faces_bord_reel)
                          {
                            matrice(n0,n0)+=valA*porosite_eventuelle(num_face);
                            matrice(n0,j0)-=valA*porosite_eventuelle(j);
                          }
                        if(j<nb_faces)
                          {
                            matrice(j0,n0)-=valA*porosite_eventuelle(num_face);
                            matrice(j0,j0)+=valA*porosite_eventuelle(j);
                          }
                        if (nb_comp>1) // on ajoute grad_U transpose
                          for (int nc2=0; nc2<nb_comp; nc2++)
                            {
                              int n1=num_face*nb_comp+nc2;
                              int j1=j*nb_comp+nc2;
                              if (ind_face<nb_faces_bord_reel)
                                {
                                  double coeff_s=orientation*nu_turb(elem)/volumes(elem)*face_normale(num_face,nc2)*face_normale(j,nc);
                                  matrice(n0,n1)+=coeff_s*porosite_eventuelle(num_face);
                                  matrice(n0,j1)-= coeff_s*porosite_eventuelle(j);
                                }
                              if (j<nb_faces)
                                {
                                  double coeff_s=orientation*nu_turb(elem)/volumes(elem)*face_normale(num_face,nc)*face_normale(j,nc2);
                                  matrice(j0,n1)-=coeff_s*porosite_eventuelle(num_face);
                                  matrice(j0,j1)+=coeff_s*porosite_eventuelle(j);
                                }
                            }
                      }
                  }
            }
        }
    }
}
void Op_Dift_VEF_Face::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const

{
  modifier_matrice_pour_periodique_avant_contribuer(matrice,equation());
  // On remplit le tableau nu car l'assemblage d'une
  // matrice avec ajouter_contribution peut se faire
  // avant le premier pas de temps
  remplir_nu(nu_);
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int nb_faces = zone_VEF.nb_faces();
  const int nb_comp =transporte.line_size();

  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();

  const DoubleTab& nu_turb_=diffusivite_turbulente()->valeurs();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  const DoubleVect& volumes = zone_VEF.volumes();
  DoubleVect n(dimension);

  DoubleTab nu,nu_turb;
  int marq=phi_psi_diffuse(equation());
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  modif_par_porosite_si_flag(nu_turb_,nu_turb,!marq,porosite_elem);

  DoubleVect porosite_eventuelle(zone_VEF.porosite_face());
  if (!marq)
    porosite_eventuelle=1;

  // On traite les conditions limites separemment car sinon
  // methode trop longue et mauvais inlining de certaines methodes
  // sur certaines machines (Matrice_Morse::operator() et viscA)
  ajouter_contribution_cl(transporte, matrice, nu, nu_turb, porosite_eventuelle);

  // On traite les faces internes
  int numpremiereface = zone_VEF.premiere_face_int();
  for (int num_face=numpremiereface; num_face<nb_faces; num_face++)
    {
      for (int l=0; l<2; l++)
        {
          int elem = face_voisins(num_face,l);
          double d_nu = nu(elem) + nu_turb(elem);
          for (int i=0; i<nb_faces_elem; i++)
            {
              int j = elem_faces(elem,i);
              if ( j > num_face )
                {
                  int contrib=1;
                  if(j>=nb_faces) // C'est une face virtuelle
                    {
                      int el1 = face_voisins(j,0);
                      int el2 = face_voisins(j,1);
                      if((el1==-1)||(el2==-1))
                        contrib=0;
                    }
                  if (contrib)
                    {
                      double tmp = 0;
                      if (nb_comp>1) // on ajoutera grad_U transpose
                        {
                          int orientation = 1;
                          if ( (elem == face_voisins(j,l)) || (face_voisins(num_face,1-l) == face_voisins(j,1-l)))
                            orientation = -1;
                          tmp = orientation * nu_turb(elem) / volumes(elem);
                        }
                      double valA = viscA(num_face,j,elem,d_nu);
                      double contrib_num_face = valA*porosite_eventuelle(num_face);
                      double contrib_j = valA*porosite_eventuelle(j);
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          int n0=num_face*nb_comp+nc;
                          int j0=j*nb_comp+nc;
                          matrice(n0,n0)+=contrib_num_face;
                          matrice(n0,j0)-=contrib_j;
                          if(j<nb_faces) // On traite les faces reelles
                            {
                              matrice(j0,n0)-=contrib_num_face;
                              matrice(j0,j0)+=contrib_j;
                            }
                          if (nb_comp>1) // on ajoute grad_U transpose
                            {
                              for (int nc2=0; nc2<nb_comp; nc2++)
                                {
                                  int n1=num_face*nb_comp+nc2;
                                  int j1=j*nb_comp+nc2;
                                  double coeff_s=tmp*face_normale(num_face,nc2)*face_normale(j,nc);
                                  matrice(n0,n1)+=coeff_s*porosite_eventuelle(num_face);
                                  matrice(n0,j1)-=coeff_s*porosite_eventuelle(j);
                                  if(j<nb_faces) // On traite les faces reelles
                                    {
                                      double coeff_s2=tmp*face_normale(num_face,nc)*face_normale(j,nc2);
                                      matrice(j0,n1)-=coeff_s2*porosite_eventuelle(num_face);
                                      matrice(j0,j1)+=coeff_s2*porosite_eventuelle(j);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  modifier_matrice_pour_periodique_apres_contribuer(matrice,equation());
}

void Op_Dift_VEF_Face::ajouter_contribution_multi_scalaire(const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  modifier_matrice_pour_periodique_avant_contribuer(matrice,equation());
  // On remplit le tableau nu car l'assemblage d'une
  // matrice avec ajouter_contribution peut se faire
  // avant le premier pas de temps
  remplir_nu(nu_);
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int nb_faces = zone_VEF.nb_faces();
  const int nb_comp =transporte.line_size();

  int i0,j,num_face0;
  int elem0,elem1;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();

  double valA, d_nu;
  const DoubleTab& nu_turb_=diffusivite_turbulente()->valeurs();
  DoubleVect n(dimension);

  DoubleTab nu,nu_turb;
  int marq=phi_psi_diffuse(equation());
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  modif_par_porosite_si_flag(nu_turb_,nu_turb,!marq,porosite_elem);

  DoubleVect porosite_eventuelle(zone_VEF.porosite_face());
  if (!marq)
    porosite_eventuelle=1;
  // On traite les faces bord
  int nb_bords=zone_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      //const IntTab& elem_faces = zone_VEF.elem_faces();
      int num1 = 0;
      int num2 = le_bord.nb_faces_tot();
      int nb_faces_bord_reel = le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          // on ne parcourt que la moitie des faces volontairement...
          num2/=2;
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              fac_asso = la_cl_perio.face_associee(ind_face);
              fac_asso= le_bord.num_face(fac_asso);
              num_face0 = le_bord.num_face(ind_face);
              // ICI
              for (int l=0; l<2; l++)
                {
                  elem0 = face_voisins(num_face0,l);

                  // On elimine les elements avec CL de paroi (rang>=1)
                  // int rang = rang_elem_non_std(elem);
                  //if (rang<0)
                  {
                    for (i0=0; i0<nb_faces_elem; i0++)
                      {
                        if ( (j= elem_faces(elem0,i0)) > num_face0 )
                          {

                            int fac_loc=0;
                            int ok=1;
                            while ((fac_loc<nb_faces_elem) && (elem_faces(elem0,fac_loc)!=num_face0)) fac_loc++;
                            if (fac_loc==nb_faces_elem)
                              ok=0;
                            for (int nc=0; nc<nb_comp; nc++)
                              {
                                d_nu = nu(elem0,nc)+nu_turb(elem0);
                                valA = viscA(num_face0,j,elem0,d_nu);
                                int n0=num_face0*nb_comp+nc;
                                int n0perio=fac_asso*nb_comp+nc;
                                int j0=j*nb_comp+nc;
                                int contrib=1;
                                if(j>=nb_faces) // C'est une face virtuelle
                                  {
                                    int el1 = face_voisins(j,0);
                                    int el2 = face_voisins(j,1);
                                    if((el1==-1)||(el2==-1))
                                      contrib=0;
                                  }
                                if(contrib)
                                  {
                                    matrice(n0,n0)+=valA*porosite_eventuelle(num_face0);
                                    matrice(n0,j0)-=valA*porosite_eventuelle(j);
                                    if(j<nb_faces) // On traite les faces reelles
                                      {
                                        //if (l==0)
                                        if (ok==1)
                                          matrice(j0,n0)-=valA*porosite_eventuelle(num_face0);
                                        else
                                          matrice(j0,n0perio)-=valA*porosite_eventuelle(num_face0);
                                        matrice(j0,j0)+=valA*porosite_eventuelle(j);
                                      }

                                  }
                              }
                          }
                      }
                  }
                }

            }
        }


      else
        {
          // pas perio
          // correction dans le cas dirichlet sur paroi temperature
          if(sub_type(Scalaire_impose_paroi,la_cl.valeur()))
            {
              const Equation_base& my_eqn = zone_Cl_VEF.equation();
              const RefObjU& modele_turbulence = my_eqn.get_modele(TURBULENCE);
              if (sub_type(Modele_turbulence_scal_base,modele_turbulence.valeur()))
                {
                  const Modele_turbulence_scal_base& mod_turb_scal = ref_cast(Modele_turbulence_scal_base,modele_turbulence.valeur());
                  const Turbulence_paroi_scal& loiparth = mod_turb_scal.loi_paroi();
                  if (loiparth->use_equivalent_distance())
                    {
                      const DoubleTab& face_normale = zone_VEF.face_normales();
                      const DoubleVect& vol = zone_VEF.volumes();
                      const DoubleVect& d_equiv=loiparth->equivalent_distance(n_bord);
                      int nb_dim_pb=Objet_U::dimension;

                      DoubleVect le_mauvais_gradient(nb_dim_pb);
                      for (int ind_face=num1; ind_face<num2; ind_face++)
                        {
                          for (int nc=0; nc<nb_comp; nc++)
                            {
                              int num_face = le_bord.num_face(ind_face);
                              // Tf est la temperature fluide moyenne dans le premier element
                              // sans tenir compte de la temperature de paroi.
                              double Tf=0.;
                              double bon_gradient=0.; // c'est la norme du gradient de temperature normal a la paroi
                              // calculee a l'aide de la loi de paroi.

                              elem1 = face_voisins(num_face,0);
                              if(elem1==-1) elem1 = face_voisins(num_face,1);

                              // inconnue(num_face) est la temperature de paroi : Tw.
                              // On se fiche du signe de bon gradient car c'est la norme du gradient
                              // de temperature dans l'element.
                              // Ensuite ce sera multiplie par le vecteur normal a la face de paroi
                              // qui lui a les bons signes.
                              //bon_gradient=(Tf-inconnue(num_face))/d_equiv(num_face);
                              bon_gradient=1./d_equiv(ind_face)*(-zone_VEF.oriente_normale(num_face,elem1));

                              double surface_face=zone_VEF.face_surfaces(num_face);
                              double nutotal=nu(elem1,nc)+nu_turb(elem1);
                              for (i0=0; i0<nb_faces_elem; i0++)
                                {
                                  j= elem_faces(elem1,i0);

                                  for (int ii=0; ii<nb_faces_elem; ii++)
                                    {
                                      le_mauvais_gradient=0;
                                      int jj=elem_faces(elem1,ii);
                                      double surface_pond=0;
                                      for (int kk=0; kk<nb_dim_pb; kk++)
                                        surface_pond -= (face_normale(jj,kk)*zone_VEF.oriente_normale(jj,elem1)*face_normale(num_face,kk)*
                                                         zone_VEF.oriente_normale(num_face,elem1))/(surface_face*surface_face);
                                      Tf=surface_pond;
                                      //Tf=1./(nb_faces_elem-1); // Temperature moyenne.
                                      for(int kk=0; kk<nb_dim_pb; kk++)
                                        le_mauvais_gradient(kk)+=face_normale(jj,kk)*zone_VEF.oriente_normale(jj,elem1);
                                      le_mauvais_gradient/=vol(elem1);
                                      double mauvais_gradient=0;
                                      //         double surface_face=zone_VEF.face_surfaces(num_face);
                                      for(int kk=0; kk<nb_dim_pb; kk++)
                                        mauvais_gradient+=le_mauvais_gradient(kk)*face_normale(num_face,kk)/surface_face;
                                      double resu1=0,resu2=0;
                                      for(int kk=0; kk<nb_dim_pb; kk++)
                                        {
                                          // resu1 += nutotal*le_mauvais_gradient(kk)*face_normale(j,kk)*(-zone_VEF.oriente_normale(j,elem1));
                                          resu1 += nutotal*mauvais_gradient*face_normale(num_face,kk)*face_normale(j,kk)*(-zone_VEF.oriente_normale(j,elem1))/surface_face;
                                          resu2 += nutotal*bon_gradient    *face_normale(num_face,kk)*face_normale(j,kk)*(-zone_VEF.oriente_normale(j,elem1))/surface_face;
                                        }
                                      // bon gradient_reel =bongradient*(Tf-T_face) d'ou les derivees...
                                      // mauvais gradient_reel=mauvai_gradient_j*Tj
                                      if (jj==num_face)
                                        resu2*=-1;
                                      else
                                        resu2*=Tf;

                                      int j0=j*nb_comp+nc;
                                      int jj0=jj*nb_comp+nc;
                                      matrice(j0,jj0)+=(resu1-resu2)*porosite_eventuelle(jj0);
                                    }
                                }
                            }
                        }
                    }// loi de paroi
                }//Equation Convection_Diffusion_Turbulente
            } // CL Temperature imposee
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              int num_face = le_bord.num_face(ind_face);
              int elem=face_voisins(num_face,0);
              // Boucle sur les faces :
              for (int i=0; i<nb_faces_elem; i++)
                if (( (j= elem_faces(elem,i)) > num_face ) || (ind_face>=nb_faces_bord_reel))
                  {
                    //int orientation = zone_VEF.oriente_normale(j,elem);
                    // retire terme croisee pour l'intant
                    //orientation=0;
                    for (int nc=0; nc<nb_comp; nc++)
                      {
                        d_nu = nu(elem,nc)+nu_turb(elem);
                        valA = viscA(num_face,j,elem,d_nu);
                        int n0=num_face*nb_comp+nc;
                        int j0=j*nb_comp+nc;
                        if (ind_face<nb_faces_bord_reel)
                          {
                            matrice(n0,n0)+=valA*porosite_eventuelle(num_face);
                            matrice(n0,j0)-=valA*porosite_eventuelle(j);
                          }
                        if(j<nb_faces)
                          {
                            matrice(j0,n0)-=valA*porosite_eventuelle(num_face);
                            matrice(j0,j0)+=valA*porosite_eventuelle(j);
                          }
                      }
                  }

            }
        }
    }

  // On traite les faces internes
  int rumpremiereface=zone_VEF.premiere_face_int();
  for (num_face0=rumpremiereface; num_face0<nb_faces; num_face0++)
    {
      // ICI
      for (int l=0; l<2; l++)
        {
          elem0 = face_voisins(num_face0,l);
          // On elimine les elements avec CL de paroi (rang>=1)
          // int rang = rang_elem_non_std(elem);
          //if (rang<0)
          {
            for (i0=0; i0<nb_faces_elem; i0++)
              {
                if ( (j= elem_faces(elem0,i0)) > num_face0 )
                  {
                    //int orientation = 1;
                    // if ( (elem == face_voisins(j,l)) || (face_voisins(num_face,(l+1)%2) == face_voisins(j,(l+1)%2)))
                    //  orientation = -1;

                    for (int nc=0; nc<nb_comp; nc++)
                      {
                        d_nu = nu(elem0,nc)+nu_turb(elem0);
                        valA = viscA(num_face0,j,elem0,d_nu);
                        int n0=num_face0*nb_comp+nc;
                        int j0=j*nb_comp+nc;
                        int contrib=1;
                        if(j>=nb_faces) // C'est une face virtuelle
                          {
                            int el1 = face_voisins(j,0);
                            int el2 = face_voisins(j,1);
                            if((el1==-1)||(el2==-1))
                              contrib=0;
                          }
                        if(contrib)
                          {
                            matrice(n0,n0)+=valA*porosite_eventuelle(num_face0);
                            matrice(n0,j0)-=valA*porosite_eventuelle(j);
                            if(j<nb_faces) // On traite les faces reelles
                              {
                                matrice(j0,n0)-=valA*porosite_eventuelle(num_face0);
                                matrice(j0,j0)+=valA*porosite_eventuelle(j);
                              }
                          }
                      }
                  }
              }
          }
        }
    }
  modifier_matrice_pour_periodique_apres_contribuer(matrice,equation());
}

void Op_Dift_VEF_Face::contribue_au_second_membre(DoubleTab& resu ) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int n_bord;
  int nb_faces = zone_VEF.nb_faces();
  //int elem;
  int nb_comp =resu.line_size();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  DoubleVect n(dimension);
  DoubleTrav Tgrad(dimension,dimension);

  // On traite les faces bord
  //  if (nb_comp!=1)
  if (equation().inconnue()->nature_du_champ()==vectoriel)
    {
      //  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
      //const Zone_VEF& zone_VEF = la_zone_vef.valeur();
      const DoubleTab& nu_turb=diffusivite_turbulente()->valeurs();
      const DoubleTab& inconnue_org=equation().inconnue().valeurs();
      DoubleTab nu,nu_turb_m;
      DoubleTab tab_inconnue;
      int marq=phi_psi_diffuse(equation());
      const DoubleVect& porosite_face = zone_VEF.porosite_face();
      const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
      // soit on a div(phi nu grad inco)
      // soit on a div(nu grad phi inco)
      // cela depend si on diffuse phi_psi ou psi
      modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
      modif_par_porosite_si_flag(nu_turb,nu_turb_m,!marq,porosite_elem);
      const DoubleTab& inconnue=modif_par_porosite_si_flag(inconnue_org,tab_inconnue,marq,porosite_face);



      DoubleTab& grad=static_cast<DoubleTab&>(grad_);
      grad=0.;

      //      const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
      //      int nb_cl=les_cl.size();

      Champ_P1NC::calcul_gradient(inconnue,grad,zone_Cl_VEF);
      DoubleTab gradsa(grad);
      if (le_modele_turbulence.valeur().utiliser_loi_paroi())
        Champ_P1NC::calcul_duidxj_paroi(grad,nu,nu_turb,tau_tan_,zone_Cl_VEF);
      grad-=gradsa;
      grad.echange_espace_virtuel();
      for (int num_face=0; num_face<nb_faces; num_face++)
        {
          //double d_nu;
          for (int kk=0; kk<2; kk++)
            {
              int elem=face_voisins(num_face,kk);
              if (elem!=-1)
                {
                  //d_nu = nu[elem]+nu_turb[elem];
                  int ori = 1-2*kk;
                  for (int i=0; i<nb_comp; i++)
                    for (int j=0; j<nb_comp; j++)
                      {
                        resu(num_face,i) -= ori*face_normale(num_face,j)*
                                            ((nu[elem]+nu_turb[elem])*grad(elem,i,j)
                                             +(nu_turb[elem])*grad_transp(elem,i,j));

                      }

                }

            }
        }
    }

  //int nb_comp = resu.dimension(1);
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            for (int comp=0; comp<nb_comp; comp++)
              resu(face,comp) += la_cl_paroi.flux_impose(face-ndeb,comp)*zone_VEF.face_surfaces(face);
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          if (resu.line_size() == 1)
            {
              const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();
              for (int face=ndeb; face<nfin; face++)
                {
                  resu[face] += la_cl_paroi.h_imp(face-ndeb)*(la_cl_paroi.T_ext(face-ndeb))*zone_VEF.face_surfaces(face);
                }
            }
          else
            {

              Cerr << "Non code pour Echange_externe_impose" << finl;
              assert(0);
            }
        }
    }

}
#undef grad_transp
