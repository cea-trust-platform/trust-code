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
// File:        Op_Dift_EF_Q1.cpp
// Directory:   $TRUST_ROOT/src/EF/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_EF_Q1.h>
#include <Zone_EF.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>
#include <Debog.h>
#include <TRUSTTrav.h>
#include <Probleme_base.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Neumann_paroi.h>
#include <Echange_global_impose.h>
#include <Echange_interne_global_impose.h>
#include <Echange_couplage_thermique.h>
#include <Echange_interne_global_parfait.h>
#include <Champ_front_calc_interne.h>
#include <Param.h>
#include <Op_Conv_EF.h>
#include <vector>
#include <Champ_Fonc_P0_base.h>

Implemente_instanciable_sans_constructeur(Op_Dift_EF_Q1,"Op_Dift_EF_Q1",Op_Dift_EF_base);

Op_Dift_EF_Q1::Op_Dift_EF_Q1():transpose_(1),transpose_partout_(0),nouvelle_expression_(0) { }

Sortie& Op_Dift_EF_Q1::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Entree& Op_Dift_EF_Q1::readOn(Entree& s ) { return s ; }

Implemente_instanciable(Op_Dift_EF_Q1_option,"Op_Dift_EF_Q1_option",Op_Dift_EF_Q1);

Sortie& Op_Dift_EF_Q1_option::printOn(Sortie& s ) const { return s << que_suis_je()  ; }

Entree& Op_Dift_EF_Q1_option::readOn(Entree& s )
{
  Param param(que_suis_je());
  param.ajouter("grad_u_transpose", &transpose_ );
  param.ajouter("grad_u_transpose_partout", &transpose_partout_ );
  param.ajouter("nouvelle_expression",&nouvelle_expression_);
  param.ajouter_condition("(value_of_grad_u_transpose_EQ_0)_OR_(value_of_grad_u_transpose_EQ_1)"," grad_u_transpose doit valoir 0 ou 1 ");
  param.ajouter_condition("(value_of_grad_u_transpose_partout_EQ_0)_OR_(value_of_grad_u_transpose_partout_EQ_1)"," grad_u_transpose_partout doit valoir 0 ou 1 ");
  param.ajouter_condition("(value_of_grad_u_transpose_partout_EQ_0)_OR_((value_of_grad_u_transpose_partout_EQ_1)_AND_(value_of_grad_u_transpose_EQ_1))"," si grad_u_transpose_partout vaut 1 alors grad_u_transpose doit valoir 1");
  param.lire_avec_accolades_depuis(s);

  return s ;
}

void Op_Dift_EF_Q1::remplir_marqueur_elem_CL_paroi(ArrOfInt& marqueur,const Zone_EF& zone_EF,const Zone_Cl_EF& zone_Cl_EF) const
{
  const IntTab& face_voisins=zone_EF.face_voisins();
  marqueur.resize_array(zone_EF.nb_elem_tot());
  if (!(le_modele_turbulence.valeur().utiliser_loi_paroi())) return;

  int nb_bords=zone_EF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_EF.les_conditions_limites(n_bord);
      // Paroi fixe, Paroi defilante :
      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) ||
          sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nfin =le_bord.nb_faces_tot();
          for (int ind_face=0; ind_face<nfin; ind_face++)
            {
              int face=le_bord.num_face(ind_face);
              int elem=face_voisins(face,0);
              marqueur(elem)=1;
            }
        }
    }
}

// Description:
// associe le champ de diffusivite
void Op_Dift_EF_Q1::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = diffu;
}

void Op_Dift_EF_Q1::remplir_nu(DoubleTab& nu) const
{
  const Zone_EF& zone_EF = la_zone_EF.valeur();
  // On dimensionne nu
  if (!nu.get_md_vector().non_nul())
    zone_EF.zone().creer_tableau_elements(nu);
  const DoubleTab& diffu=diffusivite().valeurs();
  if (diffu.size()==1)
    nu = diffu(0,0);
  else if (diffu.nb_dim()==1)
    nu = diffu;
  else
    {
      assert(diffu.dimension(1)==1);
      for (int i=0; i<diffu.size_totale(); i++) nu(i)=diffu(i,0);
    }

//  nu.echange_espace_virtuel();
}


DoubleTab& Op_Dift_EF_Q1::ajouter(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  if ((equation().nombre_d_operateurs()>1)&&sub_type(Op_Conv_EF,equation().operateur(1).l_op_base()))
    ref_cast(Op_Conv_EF,equation().operateur(1).l_op_base()).ajouter_a_la_diffusion(tab_inconnue,resu);
  //    ref_cast(Op_Conv_EF,equation().operateur(1).l_op_base()).ajouter_sous_cond(tab_inconnue,resu,0,0,1);
  if (nouvelle_expression_==1)
    return ajouter_new(tab_inconnue,resu);



  remplir_nu(nu_);
  //  const Zone_Cl_EF& zone_Cl_EF = la_zcl_EF.valeur();
  const Zone_EF& zone_ef = la_zone_EF.valeur();
  int nb_som_elem=zone_ef.zone().nb_som_elem();

  int N = resu.line_size();
  Nature_du_champ nat= equation().inconnue().valeur().nature_du_champ();
  if (nat==vectoriel)
    {
      if ((dimension==3)&&(nb_som_elem==8))
        return ajouter_vectoriel_dim3_nbn_8(tab_inconnue,resu);
      else if ((dimension==2)&&(nb_som_elem==4))
        {
          return ajouter_vectoriel_dim2_nbn_4(tab_inconnue,resu);
        }
      else
        {
          Cerr<<__FILE__<<(int)__LINE__<< "cas non optimise "<<finl;
          return ajouter_vectoriel_gen(tab_inconnue,resu);
        }
    }
  else
    {
      if (N != 1)
        {
          Cerr<<__FILE__<<(int)__LINE__<< "cas non prevu "<<finl;
          assert(0);
          exit();
          return ajouter(tab_inconnue,resu);
        }
      if ((dimension==3)&&(nb_som_elem==8))
        return ajouter_scalaire_dim3_nbn_8(tab_inconnue,resu);
      else if ((dimension==2)&&(nb_som_elem==4))
        {
          return ajouter_scalaire_dim2_nbn_4(tab_inconnue,resu);
        }
      else
        {
          Cerr<<__FILE__<<(int)__LINE__<< "cas non optimise "<<finl;
          return ajouter_scalaire_gen(tab_inconnue,resu);
        }

    }
  //
}



DoubleTab& Op_Dift_EF_Q1::ajouter_vectoriel_dim3_nbn_8(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_vectoriel_template<AJOUTE_VECT::D3_8>(tab_inconnue,resu);
}

DoubleTab& Op_Dift_EF_Q1::ajouter_vectoriel_dim2_nbn_4(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_vectoriel_template<AJOUTE_VECT::D2_4>(tab_inconnue,resu);
}

DoubleTab& Op_Dift_EF_Q1::ajouter_scalaire_dim3_nbn_8(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_scalaire_template<AJOUTE_SCAL::D3_8>(tab_inconnue, resu);
}

DoubleTab& Op_Dift_EF_Q1::ajouter_scalaire_dim2_nbn_4(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_scalaire_template<AJOUTE_SCAL::D2_4>(tab_inconnue, resu);
}

DoubleTab& Op_Dift_EF_Q1::ajouter_scalaire_gen(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_scalaire_template<AJOUTE_SCAL::GEN>(tab_inconnue, resu);
}

DoubleTab& Op_Dift_EF_Q1::ajouter_vectoriel_gen(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_vectoriel_template<AJOUTE_VECT::GEN>(tab_inconnue,resu);
}

DoubleTab& Op_Dift_EF_Q1::ajouter_new(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  Cerr<<"NEW"<<finl;
  remplir_nu(nu_);
  //  const Zone_Cl_EF& zone_Cl_EF = la_zcl_EF.valeur();
  //const Zone_EF& zone_EF = la_zone_EF.valeur();

  DoubleVect diffu_turb(diffusivite_turbulente()->valeurs());
  DoubleTab diffu(nu_);

  const int N = resu.line_size();
  ArrOfInt marqueur_neuman;
  ArrOfInt marqueur_paroi=0;
  const Zone_EF& zone_ef=ref_cast(Zone_EF,equation().zone_dis().valeur());
  if(N > 1)
    {
      remplir_marqueur_sommet_neumann( marqueur_neuman,zone_ef,la_zcl_EF.valeur(),transpose_partout_ );
      remplir_marqueur_elem_CL_paroi( marqueur_paroi,zone_ef,la_zcl_EF.valeur() );
    }

  const DoubleVect& volumes= zone_ef.volumes();

  const DoubleTab& bij=zone_ef.Bij();
  const DoubleTab& bij_thilde=zone_ef.Bij_thilde();
  int nb_elem_tot=zone_ef.zone().nb_elem_tot();
  int nb_som_elem=zone_ef.zone().nb_som_elem();
  const IntTab& elems=zone_ef.zone().les_elems() ;

  for (int elem=0; elem<nb_elem_tot; elem++)
    if (elem_contribue(elem)&&(!marqueur_paroi(elem)))
      {
        double pond=1./volumes(elem);
        assert(N == dimension || N == 1);

        for (int i1=0; i1<nb_som_elem; i1++)
          {
            int glob=elems(elem,i1);
            int transpose = (marqueur_neuman[glob] == 1 || N == 1) ? 0 : transpose_;
            for (int i2=0; i2<nb_som_elem; i2++)
              {
                int glob2=elems(elem,i2);
                for (int n = 0; n < N; n++)
                  for (int d = 0; d < dimension; d++)
                    resu(glob, n) -= bij(elem, i1, d) * (bij_thilde(elem, i2, d) * tab_inconnue(glob2, n) + transpose * bij_thilde(elem, i2, n) * tab_inconnue(glob2, d)) * (diffu[elem]+diffu_turb[elem]) * pond;
              }
          }
      }
  //  Debog::verifier(" Op_Dift_EF_Q1::ajouter, resu 0 ",resu);
  //  Journal()<<max(resu)<<" "<<min(resu)<<finl;


  // on ajoute la contribution des bords
  ajouter_bords(tab_inconnue,resu);
  return resu;
}

DoubleTab& Op_Dift_EF_Q1::calculer(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  resu = 0;
  return ajouter(tab_inconnue,resu);
}



/////////////////////////////////////////
// Methode pour l'implicite
/////////////////////////////////////////


// essai
inline double& coeff_opt(Matrice_Morse& matrice,int i, int j)
{
  const IntVect& tab1_=matrice.get_tab1();
  const IntVect& tab2_=matrice.get_tab2();
  int k1=tab1_[i]-1;
  int k2=tab1_[i+1]-1;
  int k;
  for (k=k1; k<k2; k++)
    if (tab2_[k]-1 == j) return(matrice.get_set_coeff()(k));
  Cerr << "i ou j ne conviennent pas " << finl;
  Cerr << "i=" << i << finl;
  Cerr << "j=" << j << finl;
  Cerr << "n_lignes=" << matrice.nb_lignes() << finl;
  Cerr << "n_colonnes=" << matrice.nb_colonnes() << finl;
  assert( k<tab1_[i]-1);
  Process::exit();
  return coeff_opt(matrice,i,j);
}
//#define matrice_coef(i,j) coeff_opt(matrice,i,j)
#define  matrice_coef(i,j) matrice.coef(i,j)
void Op_Dift_EF_Q1::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const
{
  if (1)
    if ((equation().nombre_d_operateurs()>1)&&sub_type(Op_Conv_EF,equation().operateur(1).l_op_base()))
      ref_cast(Op_Conv_EF,equation().operateur(1).l_op_base()).ajouter_contribution_a_la_diffusion(transporte,matrice);

  if (nouvelle_expression_==1)
    {
      ajouter_contribution_new(transporte,matrice);
    }
  // On remplit le tableau nu car l'assemblage d'une
  // matrice avec ajouter_contribution peut se faire
  // avant le premier pas de temps
  remplir_nu(nu_);

  DoubleVect diffu_turb(diffusivite_turbulente()->valeurs());
  DoubleTab diffu(nu_);

  const int N = transporte.line_size();
  const Zone_EF& zone_ef=ref_cast(Zone_EF,equation().zone_dis().valeur());
  const DoubleVect& volumes_thilde= zone_ef.volumes_thilde();
  const DoubleVect& volumes= zone_ef.volumes();

  const DoubleTab& bij=zone_ef.Bij();
  int nb_elem_tot=zone_ef.zone().nb_elem_tot();
  int nb_som_elem=zone_ef.zone().nb_som_elem();
  const IntTab& elems=zone_ef.zone().les_elems() ;
  int nb_som=zone_ef.zone().nb_som();

  ArrOfInt marqueur_neuman;
  remplir_marqueur_sommet_neumann( marqueur_neuman,zone_ef,la_zcl_EF.valeur(),transpose_partout_ );
  ArrOfInt marqueur_paroi = 0;
  remplir_marqueur_elem_CL_paroi( marqueur_paroi,zone_ef,la_zcl_EF.valeur() );
  for (int elem=0; elem<nb_elem_tot; elem++)
    if (elem_contribue(elem)&&(!marqueur_paroi(elem)))
      {
        double pond=volumes_thilde(elem)/volumes(elem)/volumes(elem);

        for (int i1=0; i1<nb_som_elem; i1++)
          {
            int glob=elems(elem,i1);

            int transpose = (marqueur_neuman[glob] == 1 || N == 1) ? 0 : transpose_;
            if (glob<nb_som)
              for (int i2=0; i2<nb_som_elem; i2++)
                {
                  int glob2=elems(elem,i2);
                  double cb=0;
                  for (int b=0; b<dimension; b++)
                    cb+=bij(elem,i1,b)*bij(elem,i2,b);
                  for (int n = 0; n < N; n++)
                    {
                      matrice_coef(glob * N + n, glob2 * N + n) += cb * (diffu[elem]+diffu_turb[elem]) * pond;
                      if (transpose)
                        for (int d = 0; d < dimension; d++)
                          matrice_coef(glob * N + n, glob2 * N + d) += bij(elem, i1, d) * bij(elem, i2, n) * (diffu[elem]+diffu_turb[elem]) * pond;
                    }
                }
          }
      }
  if (N == 1) ajouter_contributions_bords(matrice);



}

void Op_Dift_EF_Q1::ajouter_contribution_new(const DoubleTab& transporte, Matrice_Morse& matrice ) const
{
  //Cerr<<" NEW"<<finl;
  // On remplit le tableau nu car l'assemblage d'une
  // matrice avec ajouter_contribution peut se faire
  // avant le premier pas de temps
  remplir_nu(nu_);

  DoubleVect diffu_turb(diffusivite_turbulente()->valeurs());
  DoubleTab diffu(nu_);

  const int N = transporte.line_size();
  const Zone_EF& zone_ef=ref_cast(Zone_EF,equation().zone_dis().valeur());
  //const DoubleVect& volumes_thilde= zone_ef.volumes_thilde();
  const DoubleVect& volumes= zone_ef.volumes();

  const DoubleTab& bij=zone_ef.Bij();
  const DoubleTab& bij_thilde=zone_ef.Bij_thilde();
  int nb_elem_tot=zone_ef.zone().nb_elem_tot();
  int nb_som_elem=zone_ef.zone().nb_som_elem();
  const IntTab& elems=zone_ef.zone().les_elems() ;
  int nb_som=zone_ef.zone().nb_som();

  ArrOfInt marqueur_neuman;
  remplir_marqueur_sommet_neumann( marqueur_neuman,zone_ef,la_zcl_EF.valeur(),transpose_partout_ );
  ArrOfInt marqueur_paroi = 0;
  remplir_marqueur_elem_CL_paroi( marqueur_paroi,zone_ef,la_zcl_EF.valeur() );
  for (int elem=0; elem<nb_elem_tot; elem++)
    if (elem_contribue(elem)&&(!marqueur_paroi(elem)))
      {
        double pond=1./volumes(elem);

        for (int i1=0; i1<nb_som_elem; i1++)
          {
            int glob=elems(elem,i1);

            int transpose = (marqueur_neuman[glob] == 1 || N == 1) ? 0 : transpose_;
            if (glob<nb_som)
              for (int i2=0; i2<nb_som_elem; i2++)
                {
                  int glob2=elems(elem,i2);
                  double cb=0;
                  for (int b=0; b<dimension; b++)
                    cb+=bij(elem,i1,b)*bij_thilde(elem,i2,b);
                  for (int n = 0; n < N; n++)
                    {
                      matrice_coef(glob * N + n, glob2 * N + n) += cb * (diffu[elem]+diffu_turb[elem]) * pond;
                      if (transpose)
                        for (int d = 0; d < dimension; d++)
                          matrice_coef(glob * N + n, glob2 * N + d) += bij(elem, i1, d) * bij_thilde(elem, i2, n) * (diffu[elem]+diffu_turb[elem]) * pond;
                    }
                }
          }
      }




}
void Op_Dift_EF_Q1::contribuer_au_second_membre(DoubleTab& resu ) const
{

  if ((equation().nombre_d_operateurs()>1)&&sub_type(Op_Conv_EF,equation().operateur(1).l_op_base()))
    ref_cast(Op_Conv_EF,equation().operateur(1).l_op_base()).contribue_au_second_membre_a_la_diffusion(resu);
  const DoubleTab& tab_inconnue=equation().inconnue().valeurs();
  ajouter_bords(tab_inconnue,resu,0);
}
void Op_Dift_EF_Q1::ajouter_bords(const DoubleTab& tab_inconnue,DoubleTab& resu,  int contrib_interne ) const
{
  // a mettre dans calculer_flux_bord....

  const Zone_Cl_EF& zone_Cl_EF = la_zcl_EF.valeur();
  const Zone_EF& zone_EF = la_zone_EF.valeur();
  flux_bords_=0.;
  // const DoubleTab& tab_inconnue=equation().inconnue().valeurs();
  // on parcourt toutes les faces de bord et on calcule lambda*gradT
  const Zone_EF& zone_ef=ref_cast(Zone_EF,equation().zone_dis().valeur());
  const IntTab& face_voisins=zone_ef.face_voisins();
  const DoubleTab& bij=zone_ef.Bij();
  int nb_som_elem=zone_ef.zone().nb_som_elem();
  const IntTab& elems=zone_ef.zone().les_elems() ;
  const DoubleTab& face_normales=zone_ef.face_normales();
  const DoubleVect& volumes_thilde= zone_ef.volumes_thilde();
  const DoubleVect& volumes= zone_ef.volumes();

  const IntTab& face_sommets=zone_ef.face_sommets();
  int nb_som_face=zone_ef.nb_som_face();
  int nb_faces_elem = zone_ef.zone().nb_faces_elem();
  const IntTab& elem_faces = zone_ef.elem_faces();
  int nb_som_free = nb_som_elem-nb_som_face;

  const int N = resu.line_size();

  DoubleVect diffu_turb(diffusivite_turbulente()->valeurs());
  DoubleTab diffu(nu_);

  if (N > 1)
    {
      for (int n_bord=0; n_bord<zone_ef.nb_front_Cl(); n_bord++)
        {

          const Cond_lim& la_cl = zone_Cl_EF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nb_faces_bord = le_bord.nb_faces();
          int num1=0;
          int num2=nb_faces_bord;

          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) ||
              sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
            {
              if (le_modele_turbulence.valeur().utiliser_loi_paroi())
                {
                  DoubleVect n(dimension);
                  DoubleTrav Tgrad(dimension,dimension);
                  ArrOfDouble effort_face(dimension);
                  ArrOfDouble effort_elem(dimension);
                  ArrOfInt som_CL(nb_som_face);

                  // Boucle sur les faces de la CL
                  for (int ind_face=num1; ind_face<num2; ind_face++)
                    {
                      int num_face = le_bord.num_face(ind_face);
                      for (int isom=0; isom<nb_som_face; isom++)
                        {
                          int glob2=face_sommets(num_face,isom);
                          som_CL(isom) = glob2;
                        }
                      int elem=face_voisins(num_face,0);
                      double pond=volumes_thilde(elem)/volumes(elem);
                      for (int a=0; a<dimension; a++) n[a]=face_normales(num_face,a);
                      n/=norme_array(n);

                      // Calcul du gradient gradU via tau
                      // Au bord, n toujours oriente vers l'exterieur
                      for (int nc=0; nc<dimension; nc++)
                        for (int nc2=0; nc2<dimension; nc2++)
                          Tgrad(nc,nc2)=tau_tan_(num_face,nc)*n[nc2];

                      // Boucle sur les faces de l'element
                      // (Sigma_e somme_face n = 0 => -Sigma_e somme_face_cl n = + Sigma_e somme_face_non_cl n)
                      effort_elem = 0.;
                      for (int i=0; i<nb_faces_elem; i++)
                        {
                          int face_i=elem_faces(elem,i);
                          // On calcule la contribution de la face :
                          effort_face = 0.;
                          double ori=1.;
                          if ( face_voisins(face_i,0) != elem ) ori=-1;
                          if (face_i != num_face)
                            for (int nc=0; nc<dimension; nc++)
                              for (int nc2=0; nc2<dimension; nc2++)
                                effort_face(nc)+=ori*(Tgrad(nc,nc2)*face_normales(face_i,nc2))*pond;
                          //  Cerr<<"force cisaillement face "<<i<<" => "<<effort_face<<finl;
                          effort_elem+=effort_face;
                        }
                      //  Cerr<<"force cisaillement elem "<<elem<<" => "<<effort_elem<<finl;

                      // On calcule la contribution a chaque noeud de l'element :
                      // (les nb_som_face noeuds de la face CL Dirichlet ne sont pas concernes)
                      int nb_iok = 0;
                      for (int jsom=0; jsom<nb_som_elem; jsom++)
                        {
                          int num_som = elems(elem,jsom);
                          int iok = 1;
                          for (int isom=0; isom<nb_som_face; isom++)
                            if (num_som == som_CL(isom)) iok = 0;
                          nb_iok += iok;
                          if (iok)
                            for (int nc=0; nc<dimension; nc++)
                              resu(num_som,nc)+= effort_elem(nc)/(nb_som_free);
                        }
                      if (nb_iok != nb_som_free)
                        {
                          Cerr<<"Op_Dift_EF_Q1::ajouter_bords: erreur nombre de noeuds"<<finl;
                          exit();
                        }
                    }
                }
            }
        }
      // Cerr<<__PRETTY_FUNCTION__<<" non code pour les vecteurs"<<finl;
      modifier_flux(*this);
      return;
    }
  int premiere_face_int=zone_ef.premiere_face_int();
  for (int face=0; face<premiere_face_int; face++)
    {
      int elem=face_voisins(face,0);
      if (elem==-1) face_voisins(face,1);

      double pond= volumes_thilde(elem)/volumes(elem)/volumes(elem);

      for (int i1=0; i1<nb_som_elem; i1++)
        {

          int glob2=elems(elem,i1);
          {

            for (int a=0; a<dimension; a++)
              {
                flux_bords_(face,0)+=face_normales(face,a)*bij(elem,i1,a)*tab_inconnue(glob2)*(diffu[elem]+diffu_turb[elem])*pond;
              }
          }
        }

    }

  // Neumann :
  int n_bord;
  int nb_bords=zone_Cl_EF.nb_cond_lim();

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_EF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {

              double flux=la_cl_paroi.flux_impose(face-ndeb)*zone_EF.surface(face);
              for (int i1=0; i1<nb_som_face; i1++)
                {
                  int glob2=face_sommets(face,i1);
                  {

                    resu[glob2] += flux/nb_som_face;
                  }
                  flux_bords_(face,0) = flux;
                }
            }
        }
      else if (sub_type(Echange_couplage_thermique, la_cl.valeur()))
        {
          const Echange_couplage_thermique& la_cl_paroi = ref_cast(Echange_couplage_thermique, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {

              double h=la_cl_paroi.h_imp(face-ndeb);
              double Text=la_cl_paroi.T_ext(face-ndeb);
              double phiext=la_cl_paroi.flux_exterieur_impose(face-ndeb);

              double tm=0;
              if (contrib_interne)
                {
                  for (int i1=0; i1<nb_som_face; i1++)
                    {
                      int glob2=face_sommets(face,i1);
                      tm+=tab_inconnue(glob2);
                    }
                  tm/=nb_som_face;
                }
              double flux=(phiext+h*(Text-tm))*zone_EF.surface(face);
              flux_bords_(face,0) = flux;
              flux/=nb_som_face;
              for (int i1=0; i1<nb_som_face; i1++)
                {
                  int glob2=face_sommets(face,i1);
                  resu[glob2] += flux;
                }
            }
        }
      else if (sub_type(Echange_interne_global_parfait, la_cl.valeur()))
        {
          if (contrib_interne)
            {
              const Echange_interne_global_parfait& la_cl_paroi = ref_cast(Echange_interne_global_parfait, la_cl.valeur());
              const Champ_front_calc_interne& Text = ref_cast(Champ_front_calc_interne, la_cl_paroi.T_ext().valeur());
              const IntTab& fmap = Text.face_map();
              std::vector<bool> hit(nfin-ndeb);
              std::fill(hit.begin(), hit.end(), false);
              for (int face=ndeb; face<nfin; face++)
                {
                  int opp_face = fmap(face-ndeb)+ndeb;   // face on the other side of the inner wall:

                  // STRONG assumption : 1D, only one node per face:
                  int som=face_sommets(face,0);
                  int som_opp=face_sommets(opp_face,0);
                  if (!hit[face-ndeb])  // first time we encounter one of the face of the pair
                    {
                      // This where we write the heat equation skipping duplicated face: -A.T(i-1) + (A*T(i)+B.T(i)) - B.T(i+2)
                      // By default, we just have -A.T(i-1) + A*T(i)
                      // Recompute the extra bit of flux coming from the other side:
                      //    Get opposite element
                      int elem1 = zone_EF.face_voisins(opp_face, 0);
                      int elem_opp = (elem1 != -1) ? elem1 : zone_EF.face_voisins(opp_face, 1);
                      //    Other face of the opposite element (i.e. face "after" opp_face)
                      int f1 = zone_EF.elem_faces(elem_opp, 0);
                      int face_plus_2 = f1 != opp_face ? f1 : zone_EF.elem_faces(elem_opp, 1);  // 2 faces per elem max - 1D
                      int som_p2 = face_sommets(face_plus_2, 0);

                      double pond = volumes_thilde(elem_opp)/volumes(elem_opp)/volumes(elem_opp); // taken from ajouter() ...
                      double B = nu_(elem_opp)*pond;
                      resu[som] -= B*(tab_inconnue[som]-tab_inconnue[som_p2]);
                    }
                  else
                    {
                      // This is where we set a Dirichlet T(face) = T(opposite_face)
                      // Initially, flux for this som is -B.T(i+1) + B.T(i+2)   with B = nu(i+1)  (nu being the diffusivity)
                      // -> We want to have  B.T(i) - B.T(i+1)
                      int elem = zone_EF.face_voisins(face, 0);
                      int f1 = zone_EF.elem_faces(elem, 0);
                      int face_p2 = f1 != face ? f1 : zone_EF.elem_faces(elem, 1);  // other face of the current elem
                      int som_p2=face_sommets(face_p2,0);

                      double pond = volumes_thilde(elem)/volumes(elem)/volumes(elem); // taken from ajouter() ...
                      double B = nu_(elem)*pond;
                      resu[som] += B*(-tab_inconnue[som_p2] + tab_inconnue[som_opp]); // remove B.T(i+2) and add B.T(i)
                    }
                  hit[face-ndeb] = true;
                  hit[opp_face-ndeb] = true;
                }
            }
          else   // contrib_interne=0 --> implicit
            {
              for (int face=ndeb; face<nfin; face++)
                {
                  flux_bords_(face,0) = 0.0;
                }
            }
        }
      else if (sub_type(Echange_interne_global_impose, la_cl.valeur()))
        {
          const Echange_interne_global_impose& la_cl_paroi = ref_cast(Echange_interne_global_impose, la_cl.valeur());
          const DoubleVect& surface_gap = la_cl_paroi.surface_gap();
          for (int face=ndeb; face<nfin; face++)
            {
              double h=la_cl_paroi.h_imp(face-ndeb);
              const Champ_front_calc_interne& Text = ref_cast(Champ_front_calc_interne, la_cl_paroi.T_ext().valeur());
              const IntTab& fmap = Text.face_map();
              int opp_face = fmap(face-ndeb)+ndeb;

              double tm=0.0;
              double to=0.0;  // opposite temp.
              double flux=0.0;
              if (contrib_interne)  // explicit case
                {
                  for (int i1=0; i1<nb_som_face; i1++)
                    {
                      int glob2=face_sommets(face,i1);
                      int glob3 =face_sommets(opp_face,i1);
                      tm+=tab_inconnue(glob2);
                      to+=tab_inconnue(glob3);
                    }
                  tm/=nb_som_face;
                  to/=nb_som_face;
                  //flux=h*(to-tm)*zone_EF.surface(face);
                  flux=h*(to-tm)*surface_gap(face-ndeb);
                }
              else   // implicit case via contribuer_au_second_membre()
                {
                  flux = 0.0;
                }
              flux_bords_(face,0) = flux;
              flux/=nb_som_face;
              for (int i1=0; i1<nb_som_face; i1++)
                {
                  int glob2=face_sommets(face,i1);
                  resu[glob2] += flux;
                }
            }
        }
      else if (sub_type(Echange_global_impose, la_cl.valeur()))
        {
          const Echange_global_impose& la_cl_paroi = ref_cast(Echange_global_impose, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {

              double h=la_cl_paroi.h_imp(face-ndeb);
              double Text=la_cl_paroi.T_ext(face-ndeb);


              double tm=0;
              if (contrib_interne)
                {
                  for (int i1=0; i1<nb_som_face; i1++)
                    {
                      int glob2=face_sommets(face,i1);
                      {
                        tm+=tab_inconnue(glob2);
                      }
                    }
                  tm/=nb_som_face;
                }
              double flux=h*(Text-tm)*zone_EF.surface(face);
              flux_bords_(face,0) = flux;
              flux/=nb_som_face;
              for (int i1=0; i1<nb_som_face; i1++)
                {
                  int glob2=face_sommets(face,i1);
                  {
                    resu[glob2] += flux;
                  }

                }
            }
        }


    }
  modifier_flux(*this);
}




void Op_Dift_EF_Q1::ajouter_contributions_bords(Matrice_Morse& matrice ) const
{
  const Zone_Cl_EF& zone_Cl_EF = la_zcl_EF.valeur();
  const Zone_EF& zone_EF = la_zone_EF.valeur();
  const Zone_EF& zone_ef=ref_cast(Zone_EF,equation().zone_dis().valeur());

  const IntTab& face_sommets=zone_ef.face_sommets();
  int nb_som_face=zone_ef.nb_som_face();

  int N = equation().inconnue().valeurs().line_size();

  if (N > 1)
    {
      // Cerr<<__PRETTY_FUNCTION__<<" non code pour les vecteurs"<<finl;
      throw;
      return;
    }

  // Neumann :
  int n_bord;
  int nb_bords=zone_Cl_EF.nb_cond_lim();

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_EF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Echange_couplage_thermique, la_cl.valeur()))
        {
          //  matrice.imprimer(Cout);
          const Echange_couplage_thermique& la_cl_paroi = ref_cast(Echange_couplage_thermique, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {

              double h=la_cl_paroi.h_imp(face-ndeb);
              double dphi_dT=la_cl_paroi.derivee_flux_exterieur_imposee(face-ndeb);

              double tm=1./(nb_som_face*nb_som_face);
              double flux=(dphi_dT+h)*zone_EF.surface(face)*tm;

              for (int i1=0; i1<nb_som_face; i1++)
                {
                  int glob2=face_sommets(face,i1);
                  for (int j1=0; j1<nb_som_face; j1++)
                    {
                      int glob1=face_sommets(face,j1);
                      matrice.coef(glob1,glob2) += flux;
                    }
                }
            }
          //  matrice.imprimer(Cout);exit();
        }
      else if (sub_type(Echange_interne_global_parfait, la_cl.valeur()))
        {
          const Echange_interne_global_parfait& la_cl_paroi = ref_cast(Echange_interne_global_parfait, la_cl.valeur());
          const Champ_front_calc_interne& Text = ref_cast(Champ_front_calc_interne, la_cl_paroi.T_ext().valeur());
          const IntTab& fmap = Text.face_map();
          std::vector<bool> hit(nfin-ndeb);
          std::fill(hit.begin(), hit.end(), false);
          for (int face=ndeb; face<nfin; face++)
            {
              // face on the other side of the inner wall:
              int opp_face = fmap(face-ndeb)+ndeb;
              // element attached to it
              int elem1 = zone_EF.face_voisins(opp_face, 0);
              int elem_opp = (elem1 != -1) ? elem1 : zone_EF.face_voisins(opp_face, 1);
              // other face of the opposite element (i.e. face "after" opp_face)
              int f1 = zone_EF.elem_faces(elem_opp, 0);
              int face_plus_2 = f1 != opp_face ? f1 : zone_EF.elem_faces(elem_opp, 1);  // 2 faces per elem max - 1D
              // other face of the current elem
              int elem = zone_EF.face_voisins(face, 0);
              f1 = zone_EF.elem_faces(elem, 0);
              int face_min_1 = f1 != face ? f1 : zone_EF.elem_faces(elem, 1);

              for (int i1=0; i1<nb_som_face; i1++)
                {
                  int som=face_sommets(face,i1);
                  int som_opp=face_sommets(opp_face,i1);
                  if (!hit[face-ndeb])  // first time we encounter one of the face of the pair
                    {
                      for (int j1=0; j1<nb_som_face; j1++)
                        {
                          // heat equation skipping duplicated face: -T(i-1) + 2*T(i) - T(i+2)
                          int som_p2=face_sommets(face_plus_2,j1);
                          matrice.coef(som,som_p2) -= matrice.coef(som_opp, som_opp);
                          matrice.coef(som, som) += matrice.coef(som_opp, som_opp);
                        }
                      hit[face-ndeb] = true;
                      hit[opp_face-ndeb] = true;
                    }
                  else   // we have already handled the opposite side of the wall
                    for (int j1=0; j1<nb_som_face; j1++)
                      {
                        // Dirichlet T(face) = T(opposite_face) and cancelling right term in the tri-band
                        int som_m1=face_sommets(face_min_1, j1);
                        matrice.coef(som,som_opp) = -matrice.coef(som,som);
                        matrice.coef(som,som_m1) = 0;
                      }
                }
            }
//          matrice.imprimer(Cout);
        }
      else if (sub_type(Echange_interne_global_impose, la_cl.valeur()))
        {
          const Echange_interne_global_impose& la_cl_paroi = ref_cast(Echange_interne_global_impose, la_cl.valeur());
          const Champ_front_calc_interne& Text = ref_cast(Champ_front_calc_interne, la_cl_paroi.T_ext().valeur());
          const IntTab& fmap = Text.face_map();
          const DoubleVect& surface_gap = la_cl_paroi.surface_gap();
          for (int face=ndeb; face<nfin; face++)
            {
              double h=la_cl_paroi.h_imp(face-ndeb);
              double tm=1./(nb_som_face*nb_som_face);
              //double flux=h*zone_EF.surface(face)*tm;
              double flux=h*surface_gap(face-ndeb)*tm;
              int opp_face = fmap(face-ndeb)+ndeb;

              for (int i1=0; i1<nb_som_face; i1++)
                {
                  int glob2=face_sommets(face,i1);
                  int glob3=face_sommets(opp_face,i1);
                  for (int j1=0; j1<nb_som_face; j1++)
                    {
                      int glob1=face_sommets(face,j1);
                      matrice.coef(glob1,glob2) += flux;
                      matrice.coef(glob1,glob3) -= flux;
                    }
                }
            }
//          matrice.imprimer(Cout);
        }
      else if (sub_type(Echange_global_impose, la_cl.valeur()))
        {
          //  matrice.imprimer(Cout);
          const Echange_global_impose& la_cl_paroi = ref_cast(Echange_global_impose, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {
              double h=la_cl_paroi.h_imp(face-ndeb);
              double tm=1./(nb_som_face*nb_som_face);
              double flux=h*zone_EF.surface(face)*tm;
              for (int i1=0; i1<nb_som_face; i1++)
                {
                  int glob2=face_sommets(face,i1);
                  for (int j1=0; j1<nb_som_face; j1++)
                    {
                      int glob1=face_sommets(face,j1);
                      matrice.coef(glob1,glob2) += flux;
                    }
                }
            }
        }
    }
}
void Op_Dift_EF_Q1::verifier() const
{
  static int testee=0;
  if(testee)
    return;
  testee=1;
}

void Op_Dift_EF_Q1::calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const
{
}

double Op_Dift_EF_Q1::calculer_dt_stab() const
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
  const Zone_EF& ma_zone_EF = la_zone_EF.valeur();

  const Zone& ma_zone= ma_zone_EF.zone();

  DoubleVect diffu_turb(diffusivite_turbulente()->valeurs());
  DoubleTab diffu(nu_);
  if (equation().que_suis_je().debute_par("Convection_Diffusion_Temp"))
    {
      double rhocp = mon_equation->milieu().capacite_calorifique().valeurs()(0, 0) * mon_equation->milieu().masse_volumique().valeurs()(0, 0);
      diffu_turb /= rhocp;
      diffu /= rhocp;
    }
  double alpha;

  int ma_zone_nb_elem=ma_zone.nb_elem();
  if (has_champ_masse_volumique())
    {
      const DoubleTab& rho_elem = get_champ_masse_volumique().valeurs();
      assert(rho_elem.size_array()==ma_zone_EF.nb_elem_tot());
      for (int num_elem=0; num_elem<ma_zone_nb_elem; num_elem++)
        {
          alpha = diffu[num_elem] + diffu_turb[num_elem]; // PQ : 06/03
          alpha/=rho_elem[num_elem];
          // dt=1/(dimension/(pas*pas))/(2*alpha)

          coef=ma_zone_EF.carre_pas_maille(num_elem)/(2.*dimension*(alpha+DMINFLOAT));

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
          int nb_dim = valeurs_diffusivite.nb_dim();
          for (int num_elem=0; num_elem<ma_zone_nb_elem; num_elem++)
            {
              alpha =  diffu[num_elem] + diffu_turb[num_elem]; // PQ : 06/03
              if(unif_diffu_dt==0)
                valeurs_diffusivite_dt=(nb_dim==1?valeurs_diffusivite(num_elem):valeurs_diffusivite(num_elem,0));
              alpha*=valeurs_diffusivite_dt/(diffu[num_elem]+DMINFLOAT);
              // dt=1/(dimension/(pas*pas))/(2*alpha)
              coef=ma_zone_EF.carre_pas_maille(num_elem)/(2.*dimension*(alpha+DMINFLOAT));
              assert(coef>=0);
              // dt_stab=min(dt);
              if (coef<dt_stab) dt_stab = coef;
            }
        }
      else
        {
          int nb_comp = valeurs_diffusivite.dimension(1);
          for (int nc=0; nc<nb_comp; nc++)
            {
              for (int num_elem=0; num_elem<ma_zone_nb_elem; num_elem++)
                {
                  alpha =  diffu(num_elem,nc) + diffu_turb[num_elem];
                  //if(unif_diffu_dt==0)
                  valeurs_diffusivite_dt=valeurs_diffusivite(0,nc);
                  alpha*=valeurs_diffusivite_dt/(diffu(num_elem,nc)+DMINFLOAT);
                  coef=ma_zone_EF.carre_pas_maille(num_elem)/(2.*dimension*(alpha+DMINFLOAT));

                  if (coef<dt_stab) dt_stab = coef;
                }
            }
        }
    }

  dt_stab = Process::mp_min(dt_stab);
  return dt_stab;
}

