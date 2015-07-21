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
// File:        Op_Diff_K_Eps_VEF_Face_Q1.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_K_Eps_VEF_Face_Q1.h>
#include <Modele_turbulence_hyd_K_Eps.h>
#include <Champ_Q1NC.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Les_Cl.h>

Implemente_instanciable(Op_Diff_K_Eps_VEF_Face_Q1,"Op_Diff_K_Eps_VEF_Q1NC",Op_Diff_K_Eps_VEF_base);

////  printOn
//

Sortie& Op_Diff_K_Eps_VEF_Face_Q1::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Diff_K_Eps_VEF_Face_Q1::readOn(Entree& s )
{
  return s ;
}


// Description:
// complete l'evaluateur
void Op_Diff_K_Eps_VEF_Face_Q1::associer(const Zone_dis& zone_dis,
                                         const Zone_Cl_dis& zone_cl_dis,
                                         const Champ_Inc& ch_diffuse)
{
  const Champ_Q1NC& inco = ref_cast(Champ_Q1NC,ch_diffuse.valeur());
  const Zone_VEF& zVEF = ref_cast(Zone_VEF,zone_dis.valeur());
  const Zone_Cl_VEF& zclVEF = ref_cast(Zone_Cl_VEF,zone_cl_dis.valeur());
  inconnue_ = inco;
  la_zone_vef = zVEF;
  la_zcl_vef=zclVEF;
}


void Op_Diff_K_Eps_VEF_Face_Q1::associer_diffusivite_turbulente()
{
  assert(mon_equation.non_nul());
  double Prandtl_K, Prandtl_Eps;
  const Transport_K_Eps& eqn_transport = ref_cast(Transport_K_Eps,mon_equation.valeur());
  const Modele_turbulence_hyd_K_Eps& mod_turb = ref_cast(Modele_turbulence_hyd_K_Eps,eqn_transport.modele_turbulence());
  const Champ_Fonc& diff_turb = mod_turb.viscosite_turbulente();
  Prandtl_K = mod_turb.get_Prandtl_K();
  Prandtl_Eps = mod_turb.get_Prandtl_Eps();

  Op_Diff_K_Eps_VEF_base::associer_diffusivite_turbulente(diff_turb);
  Op_Diff_K_Eps_VEF_base::associer_Pr_K_Eps(Prandtl_K,Prandtl_Eps);
}

// ATTENTION le diffu intervenant dans les fonctions n'est que LOCAL (on appelle d_mu apres)

// Fonction utile visc
//

static double viscA_Q1(const Zone_VEF& la_zone,int num_face,int num2,int dimension,
                       int num_elem,double diffu)
{
  double constante = (diffu)/la_zone.volumes(num_elem);
  double Valeur=0;
  // Cerr << "num_face " << num_face << " et num2 " << num2 << " et num_elem " << num_elem << finl;
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

DoubleTab& Op_Diff_K_Eps_VEF_Face_Q1::ajouter(const DoubleTab& inconnue,  DoubleTab& resu) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int i,j,num_face;
  int n1 = zone_VEF.nb_faces();
  int elem;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  double valA, d_mu;
  const DoubleTab& mu_turb=diffusivite_turbulente_->valeurs();

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
                      if ( ( (j= elem_faces(elem,i)) > num_face ) && (j !=
                                                                      fac_asso) )
                        {
                          valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem,d_mu);

                          resu(num_face,0)+=valA*porosite_face(num_face)*inconnue(j,0)/Prdt_K;
                          resu(num_face,0)-=valA*porosite_face(num_face)*inconnue(num_face,0)/Prdt_K;
                          resu(num_face,1)+=valA*porosite_face(num_face)*inconnue(j,1)/Prdt_Eps;
                          resu(num_face,1)-=valA*porosite_face(num_face)*inconnue(num_face,1)/Prdt_Eps;


                          resu(j,0)+=0.5*valA*porosite_face(j)*inconnue(num_face,0)/Prdt_K;
                          resu(j,0)-=0.5*valA*porosite_face(j)*inconnue(j,0)/Prdt_K;
                          resu(j,1)+=0.5*valA*porosite_face(j)*inconnue(num_face,1)/Prdt_Eps;
                          resu(j,1)-=0.5*valA*porosite_face(j)*inconnue(j,1)/Prdt_Eps;

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
                      valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem,d_mu);

                      resu(num_face,0)+=valA*porosite_face(num_face)*inconnue(j,0)/Prdt_K;
                      resu(num_face,0)-=valA*porosite_face(num_face)*inconnue(num_face,0)/Prdt_K;

                      resu(num_face,1)+=valA*porosite_face(num_face)*inconnue(j,1)/Prdt_Eps;
                      resu(num_face,1)-=valA*porosite_face(num_face)*inconnue(num_face,1)/Prdt_Eps;


                      resu(j,0)+=valA*porosite_face(j)*inconnue(num_face,0)/Prdt_K;
                      resu(j,0)-=valA*porosite_face(j)*inconnue(j,0)/Prdt_K;
                      resu(j,1)+=valA*porosite_face(j)*inconnue(num_face,1)/Prdt_Eps;
                      resu(j,1)-=valA*porosite_face(j)*inconnue(j,1)/Prdt_Eps;

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
                  valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem,d_mu);

                  resu(num_face,0)+=valA*porosite_face(num_face)*inconnue(j,0)/Prdt_K;
                  resu(num_face,0)-=valA*porosite_face(num_face)*inconnue(num_face,0)/Prdt_K;
                  resu(num_face,1)+=valA*porosite_face(num_face)*inconnue(j,1)/Prdt_Eps;
                  resu(num_face,1)-=valA*porosite_face(num_face)*inconnue(num_face,1)/Prdt_Eps;


                  resu(j,0)+=valA*porosite_face(j)*inconnue(num_face,0)/Prdt_K;
                  resu(j,0)-=valA*porosite_face(j)*inconnue(j,0)/Prdt_K;
                  resu(j,1)+=valA*porosite_face(j)*inconnue(num_face,1)/Prdt_Eps;
                  resu(j,1)-=valA*porosite_face(j)*inconnue(j,1)/Prdt_Eps;
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
              resu(face,0) +=
                la_cl_paroi.flux_impose(face-ndeb,0)*zone_VEF.surface(face)*porosite_face(face);
              resu(face,1) +=
                la_cl_paroi.flux_impose(face-ndeb,1)*zone_VEF.surface(face)*porosite_face(face);
            }
        }
    }
  modifier_flux(*this);
  return resu;
}


DoubleTab& Op_Diff_K_Eps_VEF_Face_Q1::calculer(const DoubleTab& inconnue ,
                                               DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inconnue,resu);

}


/////////////////////////////////////////
// Methode pour l'implicite
/////////////////////////////////////////

void Op_Diff_K_Eps_VEF_Face_Q1::ajouter_contribution(const DoubleTab& transporte,
                                                     Matrice_Morse& matrice ) const

{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  int i,j,k,nc,l,num_face;
  int n1 = zone_VEF.nb_faces();
  int elem;
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  double valA, d_mu,Prdt;
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
                      if ( ( (j= elem_faces(elem,i)) > num_face ) && (j !=
                                                                      fac_asso) )
                        {
                          valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem,d_mu);
                          for ( nc=0; nc<nb_comp; nc++)
                            {
                              if (nc==0)
                                {
                                  Prdt = Prdt_K;
                                }
                              else
                                {
                                  Prdt = Prdt_Eps;
                                }

                              for (k=tab1[num_face*nb_comp+nc]-1;
                                   k<tab1[num_face*nb_comp+nc+1]-1; k++)
                                {
                                  if (tab2[k]-1==num_face*nb_comp+nc)
                                    coeff(k)+=valA*porosite_face(num_face*nb_comp+nc)/Prdt;
                                  if (tab2[k]-1==j*nb_comp+nc)
                                    coeff(k)-=valA*porosite_face(j*nb_comp+nc)/Prdt;
                                }
                              for (k=tab1[j*nb_comp+nc]-1;
                                   k<tab1[j*nb_comp+nc+1]-1; k++)
                                {
                                  if (tab2[k]-1==num_face*nb_comp+nc)
                                    coeff(k)-=valA*porosite_face(num_face*nb_comp+nc)/Prdt;
                                  if (tab2[k]-1==j*nb_comp+nc)
                                    coeff(k)+=valA*porosite_face(j*nb_comp+nc)/Prdt;
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
                      valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem,d_mu);
                      for ( nc=0; nc<nb_comp; nc++)
                        {
                          if (nc==0)
                            {
                              Prdt = Prdt_K;
                            }
                          else
                            {
                              Prdt = Prdt_Eps;
                            }

                          for (k=tab1[num_face*nb_comp+nc]-1;
                               k<tab1[num_face*nb_comp+nc+1]-1; k++)
                            {
                              if (tab2[k]-1==num_face*nb_comp+nc)
                                coeff(k)+=valA*porosite_face(num_face*nb_comp+nc)/Prdt;
                              if (tab2[k]-1==j*nb_comp+nc)
                                coeff(k)-=valA*porosite_face(j*nb_comp+nc)/Prdt;
                            }
                          for (k=tab1[j*nb_comp+nc]-1; k<tab1[j*nb_comp+nc+1]-1;
                               k++)
                            {
                              if (tab2[k]-1==num_face*nb_comp+nc)
                                coeff(k)-=valA*porosite_face(num_face*nb_comp+nc)/Prdt;
                              if (tab2[k]-1==j*nb_comp+nc)
                                coeff(k)+=valA*porosite_face(j*nb_comp+nc)/Prdt;
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
                  valA = viscA_Q1(zone_VEF,num_face,j,dimension,elem,d_mu);
                  for ( nc=0; nc<nb_comp; nc++)
                    {
                      if (nc==0)
                        {
                          Prdt = Prdt_K;
                        }
                      else
                        {
                          Prdt = Prdt_Eps;
                        }

                      for (k=tab1[num_face*nb_comp+nc]-1;
                           k<tab1[num_face*nb_comp+nc+1]-1; k++)
                        {
                          if (tab2[k]-1==num_face*nb_comp+nc)
                            coeff(k)+=valA*porosite_face(num_face*nb_comp+nc)/Prdt;
                          if (tab2[k]-1==j*nb_comp+nc)
                            coeff(k)-=valA*porosite_face(j*nb_comp+nc)/Prdt;
                        }
                      for (k=tab1[j*nb_comp+nc]-1; k<tab1[j*nb_comp+nc+1]-1;
                           k++)
                        {
                          if (tab2[k]-1==num_face*nb_comp+nc)
                            coeff(k)-=valA*porosite_face(num_face*nb_comp+nc)/Prdt;
                          if (tab2[k]-1==j*nb_comp+nc)
                            coeff(k)+=valA*porosite_face(j*nb_comp+nc)/Prdt;
                        }
                    }

                }
            }
        }
    }
  // Cerr << "la matrice de K_EPS " << matrice << finl;
}

void Op_Diff_K_Eps_VEF_Face_Q1::contribue_au_second_membre(DoubleTab& resu ) const
{

  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  //  const IntTab& elem_faces = zone_VEF.elem_faces();
  //  const IntTab& face_voisins = zone_VEF.face_voisins();

  //  int n1 = zone_VEF.nb_faces();
  int n_bord;
  //  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();


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
              resu(face,0) +=
                la_cl_paroi.flux_impose(face-ndeb,0)*zone_VEF.surface(face);
              resu(face,1) +=
                la_cl_paroi.flux_impose(face-ndeb,1)*zone_VEF.surface(face);
            }
        }
    }
}


