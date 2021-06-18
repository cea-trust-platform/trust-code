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
// File:        Op_Dift_VEF_Face_Q1.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_VEF_Face_Q1.h>
#include <Champ_Q1NC.h>
#include <Champ_Don.h>
#include <Periodique.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Neumann_paroi.h>
#include <Echange_externe_impose.h>
#include <Champ_Uniforme.h>
#include <Debog.h>
#include <DoubleTrav.h>
#include <Milieu_base.h>
#include <Zone_Cl_VEF.h>

Implemente_instanciable(Op_Dift_VEF_Face_Q1,"Op_Dift_VEF_Q1NC",Op_Dift_VEF_base);

//// printOn
//

Sortie& Op_Dift_VEF_Face_Q1::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Dift_VEF_Face_Q1::readOn(Entree& s )
{
  return s ;
}


void Op_Dift_VEF_Face_Q1::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = ref_cast(Champ_Uniforme, diffu);
}

const Champ_base& Op_Dift_VEF_Face_Q1::diffusivite() const
{
  return diffusivite_.valeur();
}

double Op_Dift_VEF_Face_Q1::calculer_dt_stab() const
{
  // Calcul de dt_stab
  // La diffusivite est constante par elements donc
  // il faut calculer dt_diff pour chaque element et
  //  dt_stab=Min(dt_diff (K) = h(K)*h(K)/(2*dimension*diffu2_(K)))
  // ou diffu2_ est la somme des 2 diffusivite laminaire et turbulente

  int num_face;
  double vol,surf;
  double surf_max=0;
  double dt_stab=1.e30;
  double coef;
  const Zone_VEF& la_zone_VEF = la_zone_vef.valeur();
  const DoubleVect& volumes = la_zone_VEF.volumes();
  const IntTab& elem_faces = la_zone_VEF.elem_faces();
  const DoubleTab& face_normales = la_zone_VEF.face_normales();
  const Zone& la_zone= la_zone_VEF.zone();
  int nb_faces_elem = la_zone.nb_faces_elem();
  double diffu = (diffusivite_.valeur())(0,0);
  const DoubleVect& diffu_turb=diffusivite_turbulente()->valeurs();
  double diffu2_;
  int la_zone_nb_elem=la_zone.nb_elem();
  for (int num_elem=0; num_elem<la_zone_nb_elem; num_elem++)
    {
      surf_max = 1.e-30;
      for (int i=0; i<nb_faces_elem; i++)
        {
          num_face = elem_faces(num_elem,i);
          surf = face_normales(num_face,0)*face_normales(num_face,0);
          for (int j=1; j<dimension; j++)
            surf += face_normales(num_face,j)*face_normales(num_face,j);
          surf_max = (surf > surf_max)? surf : surf_max;
        }
      vol = volumes(num_elem);
      vol *=vol/surf_max;
      diffu2_ = diffu + diffu_turb[num_elem];
      coef=vol/(2.*dimension*(diffu2_+DMINFLOAT));
      dt_stab = (coef < dt_stab )? coef : dt_stab;

    }

  return Process::mp_min(dt_stab);
}


// ATTENTION le diffu intervenant dans les fonctions n'est que LOCAL (on appelle d_mu apres)

// Fonction utile visc
// mu <Si, Sj> / |K|

static double viscA_Q1(const Zone_VEF& la_zone,int num_face,int num2,int dimension,
                       int num_elem,double diffu)
{
  double constante = (diffu)/la_zone.volumes(num_elem);
  double Valeur=0;

  if(dimension==2)
    {
      Valeur=la_zone.face_normales(num_face,0)*
             la_zone.face_normales(num2,1) -
             la_zone.face_normales(num_face,1)*la_zone.face_normales(num2,0);
      Valeur=constante*dabs(Valeur);
    }
  else
    {
      Valeur=(la_zone.face_normales(num_face,1)*la_zone.face_normales(num2,2) -
              la_zone.face_normales(num_face,2)*la_zone.face_normales(num2,1)) +
             (la_zone.face_normales(num_face,2)*la_zone.face_normales(num2,0) -
              la_zone.face_normales(num_face,0)*la_zone.face_normales(num2,2)) +
             (la_zone.face_normales(num_face,0)*la_zone.face_normales(num2,1) -
              la_zone.face_normales(num_face,1)*la_zone.face_normales(num2,0));
      Valeur=constante*dabs(Valeur);
    }
  return Valeur;
}

DoubleTab& Op_Dift_VEF_Face_Q1::ajouter(const DoubleTab& inconnue,
                                        DoubleTab& resu) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  int i,j,num_face;
  int n1 = zone_VEF.nb_faces();
  int elem0,elem1,elem2;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  const int nb_comp=resu.line_size();
  double valA, d_mu;
  double mu=(diffusivite_.valeur())(0,0);
  const DoubleTab& mu_turb=diffusivite_turbulente()->valeurs();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  DoubleVect n(dimension);
  DoubleTrav Tgrad(dimension,dimension);

  // On traite les faces bord

  if (nb_comp==1)

    for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
      {
        Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres nb_compo= 1 nbords, resu",resu);

        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        //const IntTab& elem_faces = zone_VEF.elem_faces();
        int nb_faces_bord = le_bord.nb_faces();
        int num1=0;
        int num2=nb_faces_bord;

        if (sub_type(Periodique,la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
            int fac_asso;
            for (int ind_face=num1; ind_face<num2; ind_face++)
              {
                num_face = le_bord.num_face(ind_face);
                fac_asso = le_bord.num_face(la_cl_perio.face_associee(ind_face));
                elem1 = face_voisins(num_face,0);
                d_mu = mu+mu_turb(elem1);
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( ( (j= elem_faces(elem1,i)) > num_face ) && (j != fac_asso) )
                      {
                        valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem1,d_mu);
                        resu(num_face,0)+=valA*inconnue(j,0)*porosite_face(num_face);
                        resu(num_face,0)-=valA*inconnue(num_face,0)*porosite_face(num_face);
                        resu(j,0)+=0.5*valA*inconnue(num_face,0)*porosite_face(j);
                        resu(j,0)-=0.5*valA*inconnue(j,0)*porosite_face(j);
                      }
                  }
                elem2 = face_voisins(num_face,1);
                d_mu = mu+mu_turb(elem2);
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( ( (j= elem_faces(elem2,i)) > num_face ) && ( j != fac_asso ) )
                      {
                        valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem2,d_mu);
                        resu(num_face,0)+=valA*inconnue(j,0)*porosite_face(num_face);
                        resu(num_face,0)-=valA*inconnue(num_face,0)*porosite_face(num_face);
                        resu(j,0)+=0.5*valA*inconnue(num_face,0)*porosite_face(j);
                        resu(j,0)-=0.5*valA*inconnue(j,0)*porosite_face(j);
                      }
                  }
              }
          }
        else
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              num_face = le_bord.num_face(ind_face);
              elem1 = face_voisins(num_face,0);
              d_mu = mu+mu_turb(elem1);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem1,i)) > num_face )
                    {
                      valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem1,d_mu);
                      resu(num_face,0)+=valA*inconnue(j,0)*porosite_face(num_face);
                      resu(num_face,0)-=valA*inconnue(num_face,0)*porosite_face(num_face);
                      resu(j,0)+=valA*inconnue(num_face,0)*porosite_face(j);
                      resu(j,0)-=valA*inconnue(j,0)*porosite_face(j);
                    }
                }
            }
      }
  else // nb_comp > 1
    for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
      {
        Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres nb_compo> 1 nbords, resu",resu);

        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        //const IntTab& elem_faces = zone_VEF.elem_faces();
        int nb_faces_bord_tot = le_bord.nb_faces_tot();
        int nb_faces_bord = le_bord.nb_faces();
        int num1=0;
        int num2=nb_faces_bord;

        if (sub_type(Periodique,la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
            int fac_asso;
            for (int ind_face=num1; ind_face<num2; ind_face++)
              {
                num_face = le_bord.num_face(ind_face);
                fac_asso = le_bord.num_face(la_cl_perio.face_associee(ind_face));
                elem1 = face_voisins(num_face,0);
                d_mu = mu+mu_turb(elem1);
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( ( (j= elem_faces(elem1,i)) > num_face ) && (j != fac_asso ) )
                      {
                        valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem1,d_mu);
                        for (int nc=0; nc<nb_comp; nc++)
                          {
                            resu(num_face,nc)+=valA*inconnue(j,nc)*porosite_face(num_face);
                            resu(num_face,nc)-=valA*inconnue(num_face,nc)*porosite_face(num_face);
                            resu(j,nc)+=0.5*valA*inconnue(num_face,nc)*porosite_face(j);
                            resu(j,nc)-=0.5*valA*inconnue(j,nc)*porosite_face(j);
                          }
                      }
                  }
                elem2 = face_voisins(num_face,1);
                d_mu = mu+mu_turb(elem2);
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( ( (j= elem_faces(elem2,i)) > num_face ) && (j!= fac_asso ) )
                      {
                        valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem2,d_mu);
                        for (int nc=0; nc<nb_comp; nc++)
                          {
                            resu(num_face,nc)+=valA*inconnue(j,nc)*porosite_face(num_face);
                            resu(num_face,nc)-=valA*inconnue(num_face,nc)*porosite_face(num_face);
                            resu(j,nc)+=0.5*valA*inconnue(num_face,nc)*porosite_face(j);
                            resu(j,nc)-=0.5*valA*inconnue(j,nc)*porosite_face(j);
                          }
                      }
                  }
              }
            Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres periodique , resu",resu);

          }
        else if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) ||
                 sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
          {
            if (le_modele_turbulence.valeur().utiliser_loi_paroi())
              {
                if (dimension == 2 )
                  {
                    for (int ind_face=num1; ind_face<num2; ind_face++)
                      {
                        num_face = le_bord.num_face(ind_face);
                        int elem=face_voisins(num_face,0);
                        // Calcul du gradient gradU=tau*Ny*N
                        // Au bord, n toujours oriente vers l'exterieur
                        n[0]=face_normale(num_face,0);
                        n[1]=face_normale(num_face,1);
                        n/=norme_array(n);
                        double signe=0;
                        Tgrad(0,0)=tau_tan_(num_face,0)*n[1]*n[0];
                        Tgrad(0,1)=tau_tan_(num_face,0)*n[1]*n[1];
                        Tgrad(1,0)=-tau_tan_(num_face,0)*n[0]*n[0];
                        Tgrad(1,1)=-tau_tan_(num_face,0)*n[0]*n[1];
                        // On determine le sens du frottement parietal :
                        signe=0;
                        for (i=0; i<nb_faces_elem; i++)
                          {
                            j=elem_faces(elem,i);
                            // Calcul du signe de (Utan-Uparoi)*tan :
                            signe+=(inconnue(j,1)-inconnue(num_face,1))*face_normale(num_face,0)
                                   -(inconnue(j,0)-inconnue(num_face,0))*face_normale(num_face,1);
                          }
                        if (signe<0) signe=-1.;
                        else signe=1.;
                        // On calcule la contribution sur chaque face de l'element :
                        for (i=0; i<nb_faces_elem; i++)
                          {
                            j=elem_faces(elem,i);
                            double ori=1.;
                            if ( zone_VEF.face_voisins(j,0) != elem ) ori=-1;
                            if (j != num_face)
                              for (int nc=0; nc<nb_comp; nc++)
                                resu(j,nc)-=ori*signe*(Tgrad(nc,0)*face_normale(j,0)+Tgrad(nc,1)*face_normale(j,1))*porosite_face(j);
                          }
                      }
                    Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres dirichlet dim2, resu",resu);
                  }
                else if (dimension==3)
                  {
                    for (int ind_face=num1; ind_face<num2; ind_face++)
                      {
                        num_face = le_bord.num_face(ind_face);
                        int elem=face_voisins(num_face,0);
                        // Calcul du gradient gradU
                        // Au bord, n toujours oriente vers l'exterieur
                        n[0]=face_normale(num_face,0);
                        n[1]=face_normale(num_face,1);
                        n[2]=face_normale(num_face,2);
                        n/=norme_array(n);
                        // Difference avec le 2D car ici tau_tan est signe !
                        // A tester malgre tout !
                        Tgrad(0,0)=tau_tan_(num_face,0)*n[0];
                        Tgrad(0,1)=tau_tan_(num_face,0)*n[1];
                        Tgrad(0,2)=tau_tan_(num_face,0)*n[2];
                        Tgrad(1,0)=tau_tan_(num_face,1)*n[0];
                        Tgrad(1,1)=tau_tan_(num_face,1)*n[1];
                        Tgrad(1,2)=tau_tan_(num_face,1)*n[2];
                        Tgrad(2,0)=tau_tan_(num_face,2)*n[0];
                        Tgrad(2,1)=tau_tan_(num_face,2)*n[1];
                        Tgrad(2,2)=tau_tan_(num_face,2)*n[2];
                        d_mu = mu+mu_turb(elem);
                        // On calcule la contribution sur chaque face de l'element :
                        for (i=0; i<nb_faces_elem; i++)
                          {
                            j=elem_faces(elem,i);
                            double ori=1.;
                            if ( zone_VEF.face_voisins(j,0) != elem ) ori=-1;
                            if (j != num_face)
                              for (int nc=0; nc<nb_comp; nc++)
                                resu(j,nc)-=ori*(Tgrad(nc,0)*face_normale(j,0)
                                                 +Tgrad(nc,1)*face_normale(j,1)
                                                 +Tgrad(nc,2)*face_normale(j,2))*porosite_face(j);
                          }
                      }
                    Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres dirichlet dim3, resu",resu);
                  }
              }
            Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres dirichlet (je crois), resu",resu);
          }
        else
          {
            Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter avant else des bords, resu",resu);
            num1 = 0;
            num2 = nb_faces_bord_tot;
            for (int ind_face=num1; ind_face<num2; ind_face++)
              {
                num_face = le_bord.num_face(ind_face);
                int elem=face_voisins(num_face,0);
                d_mu = mu+mu_turb(elem);
                // Boucle sur les faces :
                for (int ii=0; ii<nb_faces_elem; ii++)
                  {
                    j= elem_faces(elem,ii);
                    if (((j > num_face) && (ind_face<nb_faces_bord)) || ((j!=num_face) && (ind_face>=nb_faces_bord)))
                      {
                        valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem,d_mu);
                        for (int nc=0; nc<nb_comp; nc++)
                          {
                            resu(num_face,nc)+=valA*inconnue(j,nc)*porosite_face(num_face);
                            resu(num_face,nc)-=valA*inconnue(num_face,nc)*porosite_face(num_face);
                            resu(j,nc)+=valA*inconnue(num_face,nc)*porosite_face(j);
                            resu(j,nc)-=valA*inconnue(j,nc)*porosite_face(j);
                          }
                      }
                  }
              }
            Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres else des bords, resu",resu);
          }
      }

  Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres bords, resu",resu);

  // On traite les faces internes

  for (num_face=zone_VEF.premiere_face_int(); num_face<n1; num_face++)
    {
      for (int kk=0; kk<2; kk++)
        {
          elem0 = face_voisins(num_face,kk);
          d_mu = mu+mu_turb(elem0);
          // On elimine les elements avec CL de paroi (rang>=1)
          int rang = rang_elem_non_std(elem0);
          if (rang<1)
            {
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem0,i)) > num_face )
                    {
                      valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem0,d_mu);
                      for (int nc=0; nc<nb_comp; nc++)
                        {
                          resu(num_face,nc)+=valA*inconnue(j,nc)*porosite_face(num_face);
                          resu(num_face,nc)-=valA*inconnue(num_face,nc)*porosite_face(num_face);
                          resu(j,nc)+=valA*inconnue(num_face,nc)*porosite_face(j);
                          resu(j,nc)-=valA*inconnue(j,nc)*porosite_face(j);
                        }
                    }
                }
            }
        }
    }

  Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres interne, resu",resu);

  int nb_comp_bis = resu.line_size();
  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            for (int comp=0; comp<nb_comp_bis; comp++)
              resu(face,comp) += la_cl_paroi.flux_impose(face-ndeb,comp)*zone_VEF.surface(face)*porosite_face(face);
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          for (int face=ndeb; face<nfin; face++)
            for (int comp=0; comp<nb_comp_bis; comp++)
              resu(face,comp) += la_cl_paroi.h_imp(face-ndeb,comp)*(la_cl_paroi.T_ext(face-ndeb)-inconnue(face,comp))*zone_VEF.surface(face)*porosite_face(face);
        }
    }

  Debog::verifier("Op_Dift_VEF_Face_Q1::ajouter apres fin, resu",resu);
  modifier_flux(*this);
  return resu;
}

DoubleTab& Op_Dift_VEF_Face_Q1::calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inconnue,resu);
}



/////////////////////////////////////////
// Methodes pour l'implicite
/////////////////////////////////////////

void Op_Dift_VEF_Face_Q1::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const

{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  int i,j,kk,l,num_face0;
  int n1 = zone_VEF.nb_faces();
  int elem,elem1,elem2;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();

  int nb_comp = 1;
  int nb_dim = transporte.nb_dim();

  if(nb_dim==2)
    nb_comp=transporte.dimension(1);

  double valA, d_mu;
  double mu=(diffusivite_.valeur())(0,0);
  const DoubleTab& mu_turb=diffusivite_turbulente()->valeurs();
  //  const DoubleTab& face_normale = zone_VEF.face_normales();
  DoubleVect n(dimension);
  DoubleTrav Tgrad(dimension,dimension);

  IntVect& tab1 = matrice.get_set_tab1();
  IntVect& tab2 = matrice.get_set_tab2();
  DoubleVect& coeff = matrice.get_set_coeff();

  // On traite les faces bord

  if (nb_dim==1)

    for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
      {
        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        //const IntTab& elem_faces = zone_VEF.elem_faces();
        int num1 = le_bord.num_premiere_face();
        int num2 = num1 + le_bord.nb_faces();

        if (sub_type(Periodique,la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
            int fac_asso;
            for (num_face0=num1; num_face0<num2; num_face0++)
              {
                fac_asso = la_cl_perio.face_associee(num_face0-num1) + num1;
                elem1 = face_voisins(num_face0,0);
                d_mu = mu+mu_turb(elem1);
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( ( (j= elem_faces(elem1,i)) > num_face0 ) && (j != fac_asso) )
                      {
                        valA = viscA_Q1(zone_VEF,num_face0,j,dimension,elem1,d_mu);
                        for (kk=tab1[num_face0]-1; kk<tab1[num_face0+1]-1; kk++)
                          {
                            if (tab2[kk]-1==num_face0)
                              coeff(kk) +=valA*porosite_face(num_face0);
                            if (tab2[kk]-1==j)
                              coeff(kk) -=0.5*valA*porosite_face(j);
                          }
                        for (kk=tab1[j]-1; kk<tab1[j+1]-1; kk++)
                          {
                            if (tab2[kk]-1==num_face0)
                              coeff(kk) -= valA*porosite_face(num_face0);
                            if (tab2[kk]-1==j)
                              coeff(kk) +=0.5*valA*porosite_face(j);
                          }
                      }
                  }
                elem2 = face_voisins(num_face0,1);
                d_mu = mu+mu_turb(elem2);
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( ( (j= elem_faces(elem2,i)) > num_face0 ) && ( j != fac_asso ) )
                      {
                        valA = viscA_Q1(zone_VEF,num_face0,j,dimension,elem2,d_mu);
                        for (kk=tab1[num_face0]-1; kk<tab1[num_face0+1]-1; kk++)
                          {
                            if (tab2[kk]-1==num_face0)
                              coeff(kk) +=valA*porosite_face(num_face0);
                            if (tab2[kk]-1==j)
                              coeff(kk) -=0.5*valA*porosite_face(j);
                          }
                        for (kk=tab1[j]-1; kk<tab1[j+1]-1; kk++)
                          {
                            if (tab2[kk]-1==num_face0)
                              coeff(kk) -= valA*porosite_face(num_face0);
                            if (tab2[kk]-1==j)
                              coeff(kk) +=0.5*valA*porosite_face(j);
                          }
                      }
                  }
              }
          }
        else
          for (num_face0=num1; num_face0<num2; num_face0++)
            {
              elem1 = face_voisins(num_face0,0);
              d_mu = mu+mu_turb(elem1);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem1,i)) > num_face0 )
                    {
                      valA = viscA_Q1(zone_VEF,num_face0,j,dimension,elem1,d_mu);
                      for (kk=tab1[num_face0]-1; kk<tab1[num_face0+1]-1; kk++)
                        {
                          if (tab2[kk]-1==num_face0)
                            coeff(kk) += valA*porosite_face(num_face0);
                          if (tab2[kk]-1==j)
                            coeff(kk) -= valA*porosite_face(j);
                        }
                      for (kk=tab1[j]-1; kk<tab1[j+1]-1; kk++)
                        {
                          if (tab2[kk]-1==num_face0)
                            coeff(kk) -= valA*porosite_face(num_face0);
                          if (tab2[kk]-1==j)
                            coeff(kk) += valA*porosite_face(j);
                        }
                    }
                }
            }
      }
  else // nb_comp > 1
    for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
      {
        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        //const IntTab& elem_faces = zone_VEF.elem_faces();
        int num1 = le_bord.num_premiere_face();
        int num2 = num1 + le_bord.nb_faces();

        if (sub_type(Periodique,la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
            int fac_asso;
            for (num_face0=num1; num_face0<num2; num_face0++)
              {
                fac_asso = la_cl_perio.face_associee(num_face0-num1)+num1;
                elem1 = face_voisins(num_face0,0);
                d_mu = mu+mu_turb(elem1);
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( ( (j= elem_faces(elem1,i)) > num_face0 ) && (j != fac_asso ) )
                      {
                        valA = viscA_Q1(zone_VEF,num_face0,j,dimension,elem1,d_mu);
                        for (int nc=0; nc<nb_comp; nc++)
                          {
                            for (kk=tab1[num_face0*nb_comp+nc]-1; kk<tab1[num_face0*nb_comp+nc+1]-1; kk++)
                              {
                                if (tab2[kk]-1==num_face0*nb_comp+nc)
                                  coeff(kk)+=valA*porosite_face(num_face0*nb_comp+nc);
                                if (tab2[kk]-1==j*nb_comp+nc)
                                  coeff(kk)-=0.5*valA*porosite_face(j*nb_comp+nc);
                              }
                            for (kk=tab1[j*nb_comp+nc]-1; kk<tab1[j*nb_comp+nc+1]-1; kk++)
                              {
                                if (tab2[kk]-1==num_face0*nb_comp+nc)
                                  coeff(kk)-=valA*porosite_face(num_face0*nb_comp+nc);
                                if (tab2[kk]-1==j*nb_comp+nc)
                                  coeff(kk)+=0.5*valA*porosite_face(j*nb_comp+nc);
                              }
                          }
                      }
                  }
                elem2 = face_voisins(num_face0,1);
                d_mu = mu+mu_turb(elem2);
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( ( (j= elem_faces(elem2,i)) > num_face0 ) && (j!= fac_asso ) )
                      {
                        valA = viscA_Q1(zone_VEF,num_face0,j,dimension,elem2,d_mu);
                        for (int nc=0; nc<nb_comp; nc++)
                          {

                            for (kk=tab1[num_face0*nb_comp+nc]-1; kk<tab1[num_face0*nb_comp+nc+1]-1; kk++)
                              {
                                if (tab2[kk]-1==num_face0*nb_comp+nc)
                                  coeff(kk)+=valA*porosite_face(num_face0*nb_comp+nc);
                                if (tab2[kk]-1==j*nb_comp+nc)
                                  coeff(kk)-=0.5*valA*porosite_face(j*nb_comp+nc);
                              }
                            for (kk=tab1[j*nb_comp+nc]-1; kk<tab1[j*nb_comp+nc+1]-1; kk++)
                              {
                                if (tab2[kk]-1==num_face0*nb_comp+nc)
                                  coeff(kk)-=valA*porosite_face(num_face0*nb_comp+nc);
                                if (tab2[kk]-1==j*nb_comp+nc)
                                  coeff(kk)+=0.5*valA*porosite_face(j*nb_comp+nc);
                              }
                          }
                      }
                  }
              }
          }

        else
          for (int num_face=num1; num_face<num2; num_face++)
            {
              int elembis=face_voisins(num_face,0);
              d_mu = mu+mu_turb(elembis);
              // Boucle sur les faces :
              for (int ii=0; ii<nb_faces_elem; ii++)
                if ( (j= elem_faces(elembis,ii)) > num_face )
                  {
                    valA = viscA_Q1(zone_VEF,num_face,j,dimension,elembis,d_mu);
                    for (int nc=0; nc<nb_comp; nc++)
                      {
                        for (kk=tab1[num_face*nb_comp+nc]-1; kk<tab1[num_face*nb_comp+nc+1]-1; kk++)
                          {
                            if (tab2[kk]-1==num_face*nb_comp+nc)
                              coeff(kk)+=valA*porosite_face(num_face*nb_comp+nc);
                            if (tab2[kk]-1==j*nb_comp+nc)
                              coeff(kk)-=valA*porosite_face(j*nb_comp+nc);
                          }
                        for (kk=tab1[j*nb_comp+nc]-1; kk<tab1[j*nb_comp+nc+1]-1; kk++)
                          {
                            if (tab2[kk]-1==num_face*nb_comp+nc)
                              coeff(kk)-=valA*porosite_face(num_face*nb_comp+nc);
                            if (tab2[kk]-1==j*nb_comp+nc)
                              coeff(kk)+=valA*porosite_face(j*nb_comp+nc);
                          }
                      }
                  }
            }
      }


  // On traite les faces internes

  if (nb_dim == 1)
    for (num_face0=zone_VEF.premiere_face_int(); num_face0<n1; num_face0++)
      {
        for (l=0; l<2; l++)
          {
            elem = face_voisins(num_face0,l);
            d_mu = mu+mu_turb(elem);
            // On elimine les elements avec CL de paroi (rang>=1)
            int rang = rang_elem_non_std(elem);
            if (rang<1)
              {
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( (j= elem_faces(elem,i)) > num_face0 )
                      {
                        valA = viscA_Q1(zone_VEF,num_face0,j,dimension,elem,d_mu);
                        {
                          for (kk=tab1[num_face0]-1; kk<tab1[num_face0+1]-1; kk++)
                            {
                              if (tab2[kk]-1==num_face0)
                                coeff(kk)+=valA*porosite_face(num_face0);
                              if (tab2[kk]-1==j)
                                coeff(kk)-=valA*porosite_face(j);
                            }
                          for (kk=tab1[j]-1; kk<tab1[j+1]-1; kk++)
                            {
                              if (tab2[kk]-1==num_face0)
                                coeff(kk)-=valA*porosite_face(num_face0);
                              if (tab2[kk]-1==j)
                                coeff(kk)+=valA*porosite_face(j);
                            }

                        }

                      }
                  }
              }
          }
      }
  else // nb_comp > 1
    for (num_face0=zone_VEF.premiere_face_int(); num_face0<n1; num_face0++)
      {
        for (int ll=0; ll<2; ll++)
          {
            elem = face_voisins(num_face0,ll);
            d_mu = mu+mu_turb(elem);
            // On elimine les elements avec CL de paroi (rang>=1)
            int rang = rang_elem_non_std(elem);
            if (rang<1)
              {
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( (j= elem_faces(elem,i)) > num_face0 )
                      {
                        valA = viscA_Q1(zone_VEF,num_face0,j,dimension,elem,d_mu);
                        for (int nc=0; nc<nb_comp; nc++)
                          {
                            for (kk=tab1[num_face0*nb_comp+nc]-1; kk<tab1[num_face0*nb_comp+nc+1]-1; kk++)
                              {
                                if (tab2[kk]-1==num_face0*nb_comp+nc)
                                  coeff(kk) += valA*porosite_face(num_face0*nb_comp+nc);
                                if (tab2[kk]-1==j*nb_comp+nc)
                                  coeff(kk) -= valA*porosite_face(j*nb_comp+nc);
                              }
                            for (kk=tab1[j*nb_comp+nc]-1; kk<tab1[j*nb_comp+nc+1]-1; kk++)
                              {
                                if (tab2[kk]-1==num_face0*nb_comp+nc)
                                  coeff(kk) -= valA*porosite_face(num_face0*nb_comp+nc);
                                if (tab2[kk]-1==j*nb_comp+nc)
                                  coeff(kk) += valA*porosite_face(j*nb_comp+nc);
                              }
                          }
                      }
                  }
              }
          }
      }
}

void Op_Dift_VEF_Face_Q1::contribue_au_second_membre(DoubleTab& resu ) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  //  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  //  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  int i,j;
  //  int n1 = zone_VEF.nb_faces();
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  int elem;
  int nb_comp = 1;
  int nb_dim = resu.nb_dim();
  if(nb_dim==2)
    nb_comp=resu.dimension(1);
  const DoubleTab& face_normale = zone_VEF.face_normales();
  DoubleVect n(dimension);
  DoubleTrav Tgrad(dimension,dimension);

  // On traite les faces bord
  if (nb_comp!=1)
    {
      for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          const IntTab& elem_faces = zone_VEF.elem_faces();
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();

          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) ||
              sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
            {
              if (dimension == 2 )
                {
                  for (int num_face=num1; num_face<num2; num_face++)
                    {
                      int elembis=face_voisins(num_face,0);
                      // Calcul du gradient gradU=tau*Ny*N
                      // Au bord, n toujours oriente vers l'exterieur
                      n[0]=face_normale(num_face,0);
                      n[1]=face_normale(num_face,1);
                      n/=norme_array(n);
                      double signe=0;
                      Tgrad(0,0)=tau_tan_(num_face,0)*n[1]*n[0];
                      Tgrad(0,1)=tau_tan_(num_face,0)*n[1]*n[1];
                      Tgrad(1,0)=-tau_tan_(num_face,0)*n[0]*n[0];
                      Tgrad(1,1)=-tau_tan_(num_face,0)*n[0]*n[1];
                      // On determine le sens du frottement parietal :
                      signe=0;
                      for (i=0; i<nb_faces_elem; i++)
                        {
                          j=elem_faces(elembis,i);
                          // Calcul du signe de (Utan-Uparoi)*tan :
                          signe+=(inconnue_->valeurs()(j,1)-inconnue_->valeurs()(num_face,1))*face_normale(num_face,0)
                                 -(inconnue_->valeurs()(j,0)-inconnue_->valeurs()(num_face,0))*face_normale(num_face,1);
                        }
                      if (signe<0) signe=-1.;
                      else signe=1.;
                      // On calcule la contribution sur chaque face de l'element :
                      for (i=0; i<nb_faces_elem; i++)
                        {
                          j=elem_faces(elembis,i);
                          double ori=1.;
                          if ( zone_VEF.face_voisins(j,0) != elembis ) ori=-1;
                          if (j != num_face)
                            for (int nc=0; nc<nb_comp; nc++)
                              resu(j,nc)-=ori*signe*(Tgrad(nc,0)*face_normale(j,0)+Tgrad(nc,1)*face_normale(j,1));
                        }
                    }
                }
              else if (dimension==3)
                {
                  for (int num_face=num1; num_face<num2; num_face++)
                    {
                      elem=face_voisins(num_face,0);

                      // Calcul du gradient gradU
                      // Au bord, n toujours oriente vers l'exterieur

                      n[0]=face_normale(num_face,0);
                      n[1]=face_normale(num_face,1);
                      n[2]=face_normale(num_face,2);
                      n/=norme_array(n);

                      // Difference avec le 2D car ici tau_tan est signe !
                      // A tester malgre tout !

                      Tgrad(0,0)=tau_tan_(num_face,0)*n[0];
                      Tgrad(0,1)=tau_tan_(num_face,0)*n[1];
                      Tgrad(0,2)=tau_tan_(num_face,0)*n[2];
                      Tgrad(1,0)=tau_tan_(num_face,1)*n[0];
                      Tgrad(1,1)=tau_tan_(num_face,1)*n[1];
                      Tgrad(1,2)=tau_tan_(num_face,1)*n[2];
                      Tgrad(2,0)=tau_tan_(num_face,2)*n[0];
                      Tgrad(2,1)=tau_tan_(num_face,2)*n[1];
                      Tgrad(2,2)=tau_tan_(num_face,2)*n[2];

                      // On calcule la contribution sur chaque face de l'element :
                      for (i=0; i<nb_faces_elem; i++)
                        {
                          j=elem_faces(elem,i);
                          double ori=1.;
                          if ( zone_VEF.face_voisins(j,0) != elem ) ori=-1;
                          if (j != num_face)
                            for (int nc=0; nc<nb_comp; nc++)
                              resu(j,nc)-=ori*(Tgrad(nc,0)*face_normale(j,0)
                                               +Tgrad(nc,1)*face_normale(j,1)
                                               +Tgrad(nc,2)*face_normale(j,2));
                        }
                    }
                }
            }
        }
    }
  if (resu.nb_dim() == 1)
    {
      for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

          if (sub_type(Neumann_paroi,la_cl.valeur()))
            {
              const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi, la_cl.valeur());
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();
              for (int face=ndeb; face<nfin; face++)
                resu[face] += la_cl_paroi.flux_impose(face-ndeb)*zone_VEF.surface(face);
            }
          else if (sub_type(Echange_externe_impose,la_cl.valeur()))
            {
              Cerr << "Non code pour Echange_externe_impose" << finl;
              assert(0);
            }
        }
    }
  else
    {
      //int nb_comp = resu.dimension(1);
      for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
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
                  resu(face,comp) += la_cl_paroi.flux_impose(face-ndeb,comp)*zone_VEF.surface(face);
            }
        }
    }
}


