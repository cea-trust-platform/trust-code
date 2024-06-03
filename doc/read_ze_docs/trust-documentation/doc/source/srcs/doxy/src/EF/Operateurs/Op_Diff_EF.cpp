/****************************************************************************
* Copyright (c) 2023, CEA
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

//static int transpose_=1;
#include <Op_Diff_EF.h>
#include <Domaine_EF.h>
#include <Champ_Uniforme.h>
#include <Milieu_base.h>
#include <Debog.h>
#include <TRUSTTrav.h>
#include <Probleme_base.h>
#include <Neumann_paroi.h>
#include <Echange_global_impose.h>
#include <Echange_interne_global_impose.h>
#include <Echange_couplage_thermique.h>
#include <Echange_interne_global_parfait.h>
#include <Champ_front_calc_interne.h>

#include <Param.h>
#include <Op_Conv_EF.h>

#include <vector>

Implemente_instanciable_sans_constructeur(Op_Diff_EF,"Op_Diff_EF",Op_Diff_EF_base);

Op_Diff_EF::Op_Diff_EF():transpose_(1),transpose_partout_(0),nouvelle_expression_(0)
{
}
//// printOn
//
Sortie& Op_Diff_EF::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Diff_EF::readOn(Entree& s )
{
  return s ;
}
Implemente_instanciable(Op_Diff_option_EF,"Op_Diff_option_EF",Op_Diff_EF);


//// printOn
//

Sortie& Op_Diff_option_EF::printOn(Sortie& s ) const
{
  return s << que_suis_je()  ;
}

//// readOn
//

Entree& Op_Diff_option_EF::readOn(Entree& s )
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

/*! @brief associe le champ de diffusivite
 *
 */
void Op_Diff_EF::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = diffu;
}

void Op_Diff_EF::completer()
{
  Operateur_base::completer();
  marque_elem(equation());
}

const Champ_base& Op_Diff_EF::diffusivite() const
{
  return diffusivite_;
}

void Op_Diff_EF::remplir_nu(DoubleTab& nu) const
{
  const Domaine_EF& domaine_EF = le_dom_EF.valeur();
  // On dimensionne nu
  if (!nu.get_md_vector().non_nul())
    domaine_EF.domaine().creer_tableau_elements(nu);
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

DoubleTab& Op_Diff_EF::ajouter(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  if ((equation().nombre_d_operateurs()>1)&&sub_type(Op_Conv_EF,equation().operateur(1).l_op_base()))
    ref_cast(Op_Conv_EF,equation().operateur(1).l_op_base()).ajouter_a_la_diffusion(tab_inconnue,resu);
  //    ref_cast(Op_Conv_EF,equation().operateur(1).l_op_base()).ajouter_sous_cond(tab_inconnue,resu,0,0,1);
  if (nouvelle_expression_==1)
    return ajouter_new(tab_inconnue,resu);

  remplir_nu(nu_);
  //  const Domaine_Cl_EF& domaine_Cl_EF = la_zcl_EF.valeur();
  const Domaine_EF& domaine_ef = le_dom_EF.valeur();
  int nb_som_elem=domaine_ef.domaine().nb_som_elem();

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
}

DoubleTab& Op_Diff_EF::ajouter_vectoriel_gen(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_vectoriel_template<AJOUTE_VECT::GEN>(tab_inconnue,resu);
}

DoubleTab& Op_Diff_EF::ajouter_vectoriel_dim2_nbn_4(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_vectoriel_template<AJOUTE_VECT::D2_4>(tab_inconnue,resu);
}

DoubleTab& Op_Diff_EF::ajouter_vectoriel_dim3_nbn_8(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_vectoriel_template<AJOUTE_VECT::D3_8>(tab_inconnue,resu);
}

DoubleTab& Op_Diff_EF::ajouter_scalaire_dim3_nbn_8(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_scalaire_template<AJOUTE_SCAL::D3_8>(tab_inconnue,resu);
}

DoubleTab& Op_Diff_EF::ajouter_scalaire_dim2_nbn_4(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_scalaire_template<AJOUTE_SCAL::D2_4>(tab_inconnue,resu);
}

DoubleTab& Op_Diff_EF::ajouter_scalaire_gen(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  return ajouter_scalaire_template<AJOUTE_SCAL::GEN>(tab_inconnue,resu);
}

DoubleTab& Op_Diff_EF::ajouter_new(const DoubleTab& tab_inconnue, DoubleTab& resu) const
{
  Cerr<<"NEW"<<finl;
  remplir_nu(nu_);
  //  const Domaine_Cl_EF& domaine_Cl_EF = la_zcl_EF.valeur();
  //const Domaine_EF& domaine_EF = le_dom_EF.valeur();


  const int N = resu.line_size();
  ArrOfInt marqueur_neuman;
  const Domaine_EF& domaine_ef=ref_cast(Domaine_EF,equation().domaine_dis().valeur());
  if(N > 1)
    remplir_marqueur_sommet_neumann( marqueur_neuman,domaine_ef,la_zcl_EF.valeur(),transpose_partout_ );

  const DoubleVect& volumes= domaine_ef.volumes();

  const DoubleTab& bij=domaine_ef.Bij();
  const DoubleTab& bij_thilde=domaine_ef.Bij_thilde();
  int nb_elem_tot=domaine_ef.domaine().nb_elem_tot();
  int nb_som_elem=domaine_ef.domaine().nb_som_elem();
  const IntTab& elems=domaine_ef.domaine().les_elems() ;

  for (int elem=0; elem<nb_elem_tot; elem++)
    if (elem_contribue(elem))
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
                    resu(glob, n) -= bij(elem, i1, d) * (bij_thilde(elem, i2, d) * tab_inconnue(glob2, n) + transpose * bij_thilde(elem, i2, n) * tab_inconnue(glob2, d)) * nu_(elem) * pond;
              }
          }
      }

  // on ajoute la contribution des bords
  ajouter_bords(tab_inconnue,resu);
  return resu;
}

DoubleTab& Op_Diff_EF::calculer(const DoubleTab& tab_inconnue, DoubleTab& resu) const
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
void Op_Diff_EF::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const
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

  const int N = transporte.line_size();
  const Domaine_EF& domaine_ef=ref_cast(Domaine_EF,equation().domaine_dis().valeur());
  const DoubleVect& volumes_thilde= domaine_ef.volumes_thilde();
  const DoubleVect& volumes= domaine_ef.volumes();

  const DoubleTab& bij=domaine_ef.Bij();
  int nb_elem_tot=domaine_ef.domaine().nb_elem_tot();
  int nb_som_elem=domaine_ef.domaine().nb_som_elem();
  const IntTab& elems=domaine_ef.domaine().les_elems() ;
  int nb_som=domaine_ef.domaine().nb_som();

  ArrOfInt marqueur_neuman;
  remplir_marqueur_sommet_neumann( marqueur_neuman,domaine_ef,la_zcl_EF.valeur(),transpose_partout_ );
  for (int elem=0; elem<nb_elem_tot; elem++)
    if (elem_contribue(elem))
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
                      matrice_coef(glob * N + n, glob2 * N + n) += cb * nu_(elem) * pond;
                      if (transpose)
                        for (int d = 0; d < dimension; d++)
                          matrice_coef(glob * N + n, glob2 * N + d) += bij(elem, i1, d) * bij(elem, i2, n) * nu_(elem) * pond;
                    }
                }
          }
      }
  if (N == 1) ajouter_contributions_bords(matrice);

}

void Op_Diff_EF::ajouter_contribution_new(const DoubleTab& transporte, Matrice_Morse& matrice ) const
{
  //Cerr<<" NEW"<<finl;
  // On remplit le tableau nu car l'assemblage d'une
  // matrice avec ajouter_contribution peut se faire
  // avant le premier pas de temps
  remplir_nu(nu_);

  const int N = transporte.line_size();
  const Domaine_EF& domaine_ef=ref_cast(Domaine_EF,equation().domaine_dis().valeur());
  //const DoubleVect& volumes_thilde= domaine_ef.volumes_thilde();
  const DoubleVect& volumes= domaine_ef.volumes();

  const DoubleTab& bij=domaine_ef.Bij();
  const DoubleTab& bij_thilde=domaine_ef.Bij_thilde();
  int nb_elem_tot=domaine_ef.domaine().nb_elem_tot();
  int nb_som_elem=domaine_ef.domaine().nb_som_elem();
  const IntTab& elems=domaine_ef.domaine().les_elems() ;
  int nb_som=domaine_ef.domaine().nb_som();

  ArrOfInt marqueur_neuman;
  remplir_marqueur_sommet_neumann( marqueur_neuman,domaine_ef,la_zcl_EF.valeur(),transpose_partout_ );
  for (int elem=0; elem<nb_elem_tot; elem++)
    if (elem_contribue(elem))
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
                      matrice_coef(glob * N + n, glob2 * N + n) += cb * nu_(elem) * pond;
                      if (transpose)
                        for (int d = 0; d < dimension; d++)
                          matrice_coef(glob * N + n, glob2 * N + d) += bij(elem, i1, d) * bij_thilde(elem, i2, n) * nu_(elem) * pond;
                    }
                }
          }
      }

}
void Op_Diff_EF::contribuer_au_second_membre(DoubleTab& resu ) const
{

  if ((equation().nombre_d_operateurs()>1)&&sub_type(Op_Conv_EF,equation().operateur(1).l_op_base()))
    ref_cast(Op_Conv_EF,equation().operateur(1).l_op_base()).contribue_au_second_membre_a_la_diffusion(resu);
  const DoubleTab& tab_inconnue=equation().inconnue().valeurs();
  ajouter_bords(tab_inconnue,resu,0);
}
void Op_Diff_EF::ajouter_bords(const DoubleTab& tab_inconnue,DoubleTab& resu,  int contrib_interne ) const
{
  // a mettre dans calculer_flux_bord....

  const Domaine_Cl_EF& domaine_Cl_EF = la_zcl_EF.valeur();
  const Domaine_EF& domaine_EF = le_dom_EF.valeur();
  flux_bords_=0.;
  // const DoubleTab& tab_inconnue=equation().inconnue().valeurs();
  // on parcourt toutes les faces de bord et on calcule lambda*gradT
  const Domaine_EF& domaine_ef=ref_cast(Domaine_EF,equation().domaine_dis().valeur());
  const IntTab& face_voisins=domaine_ef.face_voisins();
  const DoubleTab& bij=domaine_ef.Bij();
  int nb_som_elem=domaine_ef.domaine().nb_som_elem();
  const IntTab& elems=domaine_ef.domaine().les_elems() ;
  const DoubleTab& face_normales=domaine_ef.face_normales();
  const DoubleVect& volumes_thilde= domaine_ef.volumes_thilde();
  const DoubleVect& volumes= domaine_ef.volumes();

  const IntTab& face_sommets=domaine_ef.face_sommets();
  int nb_som_face=domaine_ef.nb_som_face();

  const int N = resu.line_size();

  if (N > 1)
    {
      // Cerr<<__PRETTY_FUNCTION__<<" non code pour les vecteurs"<<finl;
      modifier_flux(*this);
      return;
    }
  int premiere_face_int=domaine_ef.premiere_face_int();
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
                flux_bords_(face,0)+=face_normales(face,a)*bij(elem,i1,a)*tab_inconnue(glob2)*nu_(elem)*pond;
              }
          }
        }

    }

  // Neumann :
  int n_bord;
  int nb_bords=domaine_Cl_EF.nb_cond_lim();

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_EF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          for (int face=ndeb; face<nfin; face++)
            {

              double flux=la_cl_paroi.flux_impose(face-ndeb)*domaine_EF.surface(face);
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
              double flux=(phiext+h*(Text-tm))*domaine_EF.surface(face);
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
                      int elem1 = domaine_EF.face_voisins(opp_face, 0);
                      int elem_opp = (elem1 != -1) ? elem1 : domaine_EF.face_voisins(opp_face, 1);
                      //    Other face of the opposite element (i.e. face "after" opp_face)
                      int f1 = domaine_EF.elem_faces(elem_opp, 0);
                      int face_plus_2 = f1 != opp_face ? f1 : domaine_EF.elem_faces(elem_opp, 1);  // 2 faces per elem max - 1D
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
                      int elem = domaine_EF.face_voisins(face, 0);
                      int f1 = domaine_EF.elem_faces(elem, 0);
                      int face_p2 = f1 != face ? f1 : domaine_EF.elem_faces(elem, 1);  // other face of the current elem
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
                  //flux=h*(to-tm)*domaine_EF.surface(face);
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
              double flux=h*(Text-tm)*domaine_EF.surface(face);
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




void Op_Diff_EF::ajouter_contributions_bords(Matrice_Morse& matrice ) const
{
  const Domaine_Cl_EF& domaine_Cl_EF = la_zcl_EF.valeur();
  const Domaine_EF& domaine_EF = le_dom_EF.valeur();
  const Domaine_EF& domaine_ef=ref_cast(Domaine_EF,equation().domaine_dis().valeur());

  const IntTab& face_sommets=domaine_ef.face_sommets();
  int nb_som_face=domaine_ef.nb_som_face();

  int N = equation().inconnue().valeurs().line_size();

  if (N > 1)
    {
      // Cerr<<__PRETTY_FUNCTION__<<" non code pour les vecteurs"<<finl;
      throw;
    }

  // Neumann :
  int n_bord;
  int nb_bords=domaine_Cl_EF.nb_cond_lim();

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_EF.les_conditions_limites(n_bord);
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
              double flux=(dphi_dT+h)*domaine_EF.surface(face)*tm;

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
              int elem1 = domaine_EF.face_voisins(opp_face, 0);
              int elem_opp = (elem1 != -1) ? elem1 : domaine_EF.face_voisins(opp_face, 1);
              // other face of the opposite element (i.e. face "after" opp_face)
              int f1 = domaine_EF.elem_faces(elem_opp, 0);
              int face_plus_2 = f1 != opp_face ? f1 : domaine_EF.elem_faces(elem_opp, 1);  // 2 faces per elem max - 1D
              // other face of the current elem
              int elem = domaine_EF.face_voisins(face, 0);
              f1 = domaine_EF.elem_faces(elem, 0);
              int face_min_1 = f1 != face ? f1 : domaine_EF.elem_faces(elem, 1);

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
              //double flux=h*domaine_EF.surface(face)*tm;
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
              double flux=h*domaine_EF.surface(face)*tm;
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
void Op_Diff_EF::verifier() const
{
  static int testee=0;
  if(testee)
    return;
  testee=1;
}
