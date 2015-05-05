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
// File:        Op_Diff_K_Eps_Bas_Re_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_K_Eps_Bas_Re_VEF_Face.h>
#include <Modele_turbulence_hyd_K_Eps_Bas_Reynolds.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Champ_P1NC.h>
#include <Zone_VEF.h>
#include <Les_Cl.h>
#include <DoubleTrav.h>



Implemente_instanciable(Op_Diff_K_Eps_Bas_Re_VEF_Face,"Op_Diff_K_Eps_Bas_Re_VEF_const_P1NC",Op_Diff_K_Eps_Bas_Re_VEF_base);

////  printOn
//


Sortie& Op_Diff_K_Eps_Bas_Re_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//


Entree& Op_Diff_K_Eps_Bas_Re_VEF_Face::readOn(Entree& s )
{
  return s ;
}


// Description:
// complete l'evaluateur
void Op_Diff_K_Eps_Bas_Re_VEF_Face::associer(const Zone_dis& zone_dis,
                                             const Zone_Cl_dis& zone_cl_dis,
                                             const Champ_Inc& ch_diffuse)
{
  const Champ_P1NC& inco = ref_cast(Champ_P1NC,ch_diffuse.valeur());
  const Zone_VEF& zVEF = ref_cast(Zone_VEF,zone_dis.valeur());
  const Zone_Cl_VEF& zclVEF = ref_cast(Zone_Cl_VEF,zone_cl_dis.valeur());
  inconnue_ = inco;
  la_zone_vef = zVEF;
  la_zcl_vef=zclVEF;
}


void Op_Diff_K_Eps_Bas_Re_VEF_Face::associer_diffusivite_turbulente()
{
  assert(mon_equation.non_nul());
  const Transport_K_Eps_Bas_Reynolds& eqn_transport = ref_cast(Transport_K_Eps_Bas_Reynolds,mon_equation.valeur());
  const Modele_turbulence_hyd_K_Eps_Bas_Reynolds& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps_Bas_Reynolds,eqn_transport.modele_turbulence());
  const Champ_Fonc& diff_turb = mod_turb.viscosite_turbulente();
  Op_Diff_K_Eps_Bas_Re_VEF_base::associer_diffusivite_turbulente(diff_turb);
}

const Champ_base& Op_Diff_K_Eps_Bas_Re_VEF_Face::diffusivite() const
{
  return diffusivite_.valeur();
}

void Op_Diff_K_Eps_Bas_Re_VEF_Face::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = diffu;
}


// ATTENTION le diffu intervenant dans les fonctions n'est que LOCAL (on appelle d_mu apres)

// Fonction utile visc
// mu <Si, Sj> / |K|

static double viscA(const Zone_VEF& la_zone,int num_face,int num2,int dimension,
                    int num_elem,double diffu)
{
  double pscal;

  pscal = la_zone.face_normales(num_face,0)*la_zone.face_normales(num2,0)
          + la_zone.face_normales(num_face,1)*la_zone.face_normales(num2,1);

  if (dimension == 3)
    pscal += la_zone.face_normales(num_face,2)*la_zone.face_normales(num2,2);

  if ( (la_zone.face_voisins(num_face,0) == la_zone.face_voisins(num2,0)) ||
       (la_zone.face_voisins(num_face,1) == la_zone.face_voisins(num2,1)))
    return -(pscal*diffu)/la_zone.volumes(num_elem);
  else
    return (pscal*diffu)/la_zone.volumes(num_elem);

}

void Op_Diff_K_Eps_Bas_Re_VEF_Face::remplir_nu(DoubleTab& nu) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  // On dimensionne nu
  if (nu.size()==0)
    nu.resize(zone_VEF.nb_elem_tot());
  const DoubleTab& diffu=diffusivite().valeurs();
  if (diffu.size()==1)
    nu = diffu(0,0);
  else
    nu = diffu;

  nu.echange_espace_virtuel();
}

DoubleTab& Op_Diff_K_Eps_Bas_Re_VEF_Face::ajouter(const DoubleTab& inconnue,  DoubleTab& resu) const
{
  remplir_nu(nu_);
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int i,j,num_face;
  int n1 = zone_VEF.nb_faces();
  int elem;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  //  int nb_elem = zone_VEF.zone().nb_elem();
  double valA,valA_bis, d_mu;
  const DoubleTab& mu_turb=diffusivite_turbulente_->valeurs();

  // A la paroi les conditions sont:
  // Pour k on a 0
  // Pour epsilon on a D
  // on calcul D a la paroi
  DoubleTrav D(n1);
  // mon_modele_fonc.Calcul_D(D,zone_dis_keps,zcl_keps,vit,inconnue,nu);

  D=0.;
  // On traite les faces bord :
  int n_bord;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              fac_asso = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
              for (int k=0; k<2; k++)
                {
                  elem = face_voisins(num_face,k);
                  d_mu = mu_turb(elem);
                  for (i=0; i<nb_faces_elem; i++)
                    {
                      if ( ( (j= elem_faces(elem,i)) > num_face ) && (j != fac_asso) )
                        {

                          valA = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_K);
                          valA_bis = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_Eps);

                          resu(num_face,0)+=valA*inconnue(j,0);
                          resu(num_face,0)-=valA*inconnue(num_face,0);

                          resu(num_face,1)+=valA_bis*inconnue(j,1);
                          resu(num_face,1)-=valA_bis*inconnue(num_face,1);

                          resu(j,0)+=0.5*valA*inconnue(num_face,0);
                          resu(j,0)-=0.5*valA*inconnue(j,0);

                          resu(j,1)+=0.5*valA_bis*inconnue(num_face,1);
                          resu(j,1)-=0.5*valA_bis*inconnue(j,1);
                        }
                    }
                }
            }
        }
      else if ( sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) )
        {
          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              elem = face_voisins(num_face,0);
              d_mu = mu_turb(elem);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem,i)) > num_face )
                    {
                      valA = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_K);
                      valA_bis = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_Eps);

                      resu(num_face,0)+=valA*inconnue(j,0);
                      resu(num_face,0)-=valA*inconnue(num_face,0);

                      resu(num_face,1)+=valA_bis*inconnue(j,1);
                      resu(num_face,1)-=valA_bis*inconnue(num_face,1);

                      resu(j,0)+=valA*inconnue(num_face,0);
                      resu(j,0)-=valA*inconnue(j,0);

                      resu(j,1)+=valA_bis*inconnue(num_face,1);
                      resu(j,1)-=valA_bis*inconnue(j,1);
                    }
                }
            }
        }
      else if ( sub_type(Symetrie,la_cl.valeur()) )
        {
          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              elem = face_voisins(num_face,0);
              d_mu = mu_turb(elem);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem,i)) > num_face )
                    {
                      valA = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_K);
                      valA_bis = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_Eps);

                      resu(num_face,0)+=valA*inconnue(j,0);
                      resu(num_face,0)-=valA*inconnue(num_face,0);

                      resu(num_face,1)+=valA_bis*inconnue(j,1);
                      resu(num_face,1)-=valA_bis*inconnue(num_face,1);

                      resu(j,0)+=valA*inconnue(num_face,0);
                      resu(j,0)-=valA*inconnue(j,0);

                      resu(j,1)+=valA_bis*inconnue(num_face,1);
                      resu(j,1)-=valA_bis*inconnue(j,1);
                    }
                }
            }
        }
      else   //  conditions aux limites autres que periodicite
        {
          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              elem = face_voisins(num_face,0);
              d_mu = mu_turb(elem);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem,i)) > num_face )
                    {
                      valA = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_K);
                      valA_bis = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_Eps);

                      resu(num_face,0)+=valA*inconnue(j,0);
                      resu(num_face,0)-=valA*inconnue(num_face,0);

                      resu(num_face,1)+=valA_bis*inconnue(j,1);
                      resu(num_face,1)-=valA_bis*inconnue(num_face,1);

                      resu(j,0)+=valA*inconnue(num_face,0);
                      resu(j,0)-=valA*inconnue(j,0);

                      resu(j,1)+=valA_bis*inconnue(num_face,1);
                      resu(j,1)-=valA_bis*inconnue(j,1);
                    }
                }
            }
        }
    }

  // On traite les faces internes

  for (num_face=zone_VEF.premiere_face_int(); num_face<n1; num_face++)
    {
      for (int k=0; k<2; k++)
        {
          elem = face_voisins(num_face,k);
          d_mu = mu_turb(elem);
          for (i=0; i<nb_faces_elem; i++)
            {
              if ( (j= elem_faces(elem,i)) > num_face )
                {
                  valA = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_K);
                  valA_bis = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_Eps);

                  resu(num_face,0)+=valA*inconnue(j,0);
                  resu(num_face,0)-=valA*inconnue(num_face,0);
                  resu(num_face,1)+=valA_bis*inconnue(j,1);
                  resu(num_face,1)-=valA_bis*inconnue(num_face,1);

                  resu(j,0)+=valA*inconnue(num_face,0);
                  resu(j,0)-=valA*inconnue(j,0);
                  resu(j,1)+=valA_bis*inconnue(num_face,1);
                  resu(j,1)-=valA_bis*inconnue(j,1);
                }
            }
        }
    }

  // Prise en compte des conditions aux limites de Neumnan a la paroi
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
            {
              resu(face,0) += la_cl_paroi.flux_impose(face-ndeb,0)*zone_VEF.surface(face);
              resu(face,1) += la_cl_paroi.flux_impose(face-ndeb,1)*zone_VEF.surface(face);
            }
        }
    }

  modifier_flux(*this);
  return resu;
}


DoubleTab& Op_Diff_K_Eps_Bas_Re_VEF_Face::calculer(const DoubleTab& inconnue , DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inconnue,resu);

}


/////////////////////////////////////////
// Methode pour l'implicite
/////////////////////////////////////////

void Op_Diff_K_Eps_Bas_Re_VEF_Face::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const

{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int i,j,k,nc,l,num_face;
  int n1 = zone_VEF.nb_faces();
  int elem;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  double valA, valA_bis, d_mu,Prdt;
  const DoubleTab& mu_turb=diffusivite_turbulente_->valeurs();


  IntVect& tab1 = matrice.tab1_;
  IntVect& tab2 = matrice.tab2_;
  DoubleVect& coeff = matrice.coeff_;

  int nb_comp = 1;
  int nb_dim = transporte.nb_dim();

  if(nb_dim==2)
    nb_comp=transporte.dimension(1);


  // On traite les faces bord
  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              fac_asso = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
              for ( l=0; l<2; l++)
                {
                  elem = face_voisins(num_face,l);
                  d_mu = mu_turb(elem);
                  for (i=0; i<nb_faces_elem; i++)
                    {
                      if ( ( (j= elem_faces(elem,i)) > num_face ) && (j != fac_asso) )
                        {
                          valA = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_K);
                          valA_bis = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_Eps);
                          for ( nc=0; nc<nb_comp; nc++)
                            {
                              if (nc==0)
                                {
                                  Prdt = valA;
                                }
                              else
                                {
                                  Prdt = valA_bis;
                                }

                              for (k=tab1[num_face*nb_comp+nc]-1; k<tab1[num_face*nb_comp+nc+1]-1; k++)
                                {
                                  if (tab2[k]-1==num_face*nb_comp+nc)
                                    coeff(k)+=Prdt;
                                  if (tab2[k]-1==j*nb_comp+nc)
                                    coeff(k)-=Prdt;
                                }
                              for (k=tab1[j*nb_comp+nc]-1; k<tab1[j*nb_comp+nc+1]-1; k++)
                                {
                                  if (tab2[k]-1==num_face*nb_comp+nc)
                                    coeff(k)-=Prdt;
                                  if (tab2[k]-1==j*nb_comp+nc)
                                    coeff(k)+=Prdt;
                                }
                            }
                        }
                    }
                }
            }
        }

      else   //  conditions aux limites autres que periodicite
        {
          for (num_face=ndeb; num_face<nfin; num_face++)
            {
              elem = face_voisins(num_face,0);
              d_mu = mu_turb(elem);
              for (i=0; i<nb_faces_elem; i++)
                {
                  if ( (j= elem_faces(elem,i)) > num_face )
                    {
                      valA = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_K);
                      valA_bis = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_Eps);
                      for ( nc=0; nc<nb_comp; nc++)
                        {
                          if (nc==0)
                            {
                              Prdt = valA;
                            }
                          else
                            {
                              Prdt = valA_bis;
                            }

                          for (k=tab1[num_face*nb_comp+nc]-1; k<tab1[num_face*nb_comp+nc+1]-1; k++)
                            {
                              if (tab2[k]-1==num_face*nb_comp+nc)
                                coeff(k)+=Prdt;
                              if (tab2[k]-1==j*nb_comp+nc)
                                coeff(k)-=Prdt;
                            }
                          for (k=tab1[j*nb_comp+nc]-1; k<tab1[j*nb_comp+nc+1]-1; k++)
                            {
                              if (tab2[k]-1==num_face*nb_comp+nc)
                                coeff(k)-=Prdt;
                              if (tab2[k]-1==j*nb_comp+nc)
                                coeff(k)+=Prdt;
                            }
                        }

                    }
                }
            }
        }
    }

  // On traite les faces internes

  for (num_face=zone_VEF.premiere_face_int(); num_face<n1; num_face++)
    {
      for ( l=0; l<2; l++)
        {
          elem = face_voisins(num_face,l);
          d_mu = mu_turb(elem);
          for (i=0; i<nb_faces_elem; i++)
            {
              if ( (j= elem_faces(elem,i)) > num_face )
                {
                  valA = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_K);
                  valA_bis = viscA(zone_VEF,num_face,j,dimension,elem,nu_(elem)+d_mu/Prdt_Eps);
                  for ( nc=0; nc<nb_comp; nc++)
                    {
                      if (nc==0)
                        {
                          Prdt = valA;
                        }
                      else
                        {
                          Prdt = valA_bis;
                        }

                      for (k=tab1[num_face*nb_comp+nc]-1; k<tab1[num_face*nb_comp+nc+1]-1; k++)
                        {
                          if (tab2[k]-1==num_face*nb_comp+nc)
                            coeff(k)+=Prdt;
                          if (tab2[k]-1==j*nb_comp+nc)
                            coeff(k)-=Prdt;
                        }
                      for (k=tab1[j*nb_comp+nc]-1; k<tab1[j*nb_comp+nc+1]-1; k++)
                        {
                          if (tab2[k]-1==num_face*nb_comp+nc)
                            coeff(k)-=Prdt;
                          if (tab2[k]-1==j*nb_comp+nc)
                            coeff(k)+=Prdt;
                        }
                    }

                }
            }
        }
    }
}

void Op_Diff_K_Eps_Bas_Re_VEF_Face::contribue_au_second_membre(DoubleTab& resu ) const
{

  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  int n_bord;
  // Prise en compte des conditions aux limites de Neumnan a la paroi
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
            {
              resu(face,0) += la_cl_paroi.flux_impose(face-ndeb,0)*zone_VEF.surface(face);
              resu(face,1) += la_cl_paroi.flux_impose(face-ndeb,1)*zone_VEF.surface(face);
            }
        }
    }
}

