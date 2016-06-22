/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Op_Dift_standard_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/31
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_standard_VEF_Face.h>
#include <Champ_P1NC.h>
#include <Les_Cl.h>
#include <Debog.h>
#include <DoubleTrav.h>

Implemente_instanciable(Op_Dift_standard_VEF_Face,"Op_Dift_VEF_P1NC_standard",Op_Dift_VEF_Face);


// Methode pour le parallel.
double tau_tang(int face,int kk,int nb_faces,
                const DoubleTab&, const ArrOfInt&);
//// printOn
//

Sortie& Op_Dift_standard_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Dift_standard_VEF_Face::readOn(Entree& is )
{
  Cerr<<"je suis dans Op_Dift_standard_VEF_Face::readOn"<<finl;
  grad_Ubar=1;
  nu_lu=1;
  nut_lu=1;
  nu_transp_lu=1;
  nut_transp_lu=1;
  filtrer_resu=1;
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle motlu;
  is >> motlu;
  while (motlu!=accfermee)
    {
      if (motlu=="defaut_bar")
        {
          is >> motlu;
          break;
        }
      else if (motlu=="grad_Ubar")
        is >> grad_Ubar;
      else if (motlu=="nu")
        is >> nu_lu;
      else if (motlu=="nut")
        is >> nut_lu;
      else if (motlu=="nu_transp")
        is >> nu_transp_lu;
      else if (motlu=="nut_transp")
        is >> nut_transp_lu;
      else if (motlu=="filtrer_resu")
        is >> filtrer_resu;
      else
        {
          Cerr << motlu << "n'est pas compris par " << que_suis_je() << finl;
          exit();
        }
      is >> motlu;
    }
  Cerr << "grad_Ubar " << grad_Ubar << " nu_lu " << nu_lu <<" nut_lu " <<  nut_lu <<" nu_transp_lu " <<  nu_transp_lu <<" nut_transp_lu " <<  nut_transp_lu <<" filtrer_resu " <<  filtrer_resu << finl;
  return is ;
}


void Op_Dift_standard_VEF_Face::ajouter_cas_vectoriel(const DoubleTab& vitesse,
                                                      DoubleTab& resu, DoubleTab& tab_flux_bords,
                                                      const DoubleTab& nu,
                                                      const DoubleTab& nu_turb,
                                                      const Zone_Cl_VEF& zone_Cl_VEF,
                                                      const Zone_VEF& zone_VEF,
                                                      const DoubleTab& tab_k,
                                                      const DoubleTab& tau_tan,
                                                      const int indic_lp_neg,
                                                      const int indic_bas_Re,
                                                      int nbr_comp) const
{
  // on cast grad et grad_transp pour pouvoir les modifier
  // on utilise plus les static car pb avec plusieurs pbs
  DoubleTab& grad=ref_cast_non_const(DoubleTab,  grad_);

  Debog::verifier("Op_Dift_standard_VEF_Face::ajouter_cas_vectoriel : resu 0 ",resu);

  DoubleVect n(Objet_U::dimension);
  DoubleVect t(Objet_U::dimension);
  DoubleTrav Tgrad(Objet_U::dimension,Objet_U::dimension);

  assert(nbr_comp>1);

  // On dimensionne et initialise le tableau des bilans de flux:
  tab_flux_bords.resize(zone_VEF.nb_faces_bord(),nbr_comp);
  tab_flux_bords=0.;

  // Construction du tableau grad_
  if(!grad.get_md_vector().non_nul())
    {
      grad.resize(0, Objet_U::dimension, Objet_U::dimension);
      zone_VEF.zone().creer_tableau_elements(grad);
    }

  // *** CALCUL DU GRADIENT ***

  DoubleTab ubar(vitesse);

  if(grad_Ubar)
    ref_cast(Champ_P1NC,inconnue_.valeur()).filtrer_L2(ubar);

  Champ_P1NC::calcul_gradient(ubar,grad,zone_Cl_VEF);
  Champ_P1NC::calcul_duidxj_paroi(grad,nu,nu_turb,tau_tan,indic_lp_neg,indic_bas_Re,zone_Cl_VEF);

  grad.echange_espace_virtuel();

  // *** CALCUL DE LA DIFFUSION ***
  Debog::verifier("Op_Dift_standard_VEF_Face::ajouter_cas_vectoriel : grad 1 ",grad);



  //  calcul_divergence(resu_div,grad,gradt,nu_div,nut_div,zone_Cl_VEF,zone_VEF,flux_bords,nb_comp);
  //  calcul_divergence(resu_div,grad,grad,nu_div,nut_div,zone_Cl_VEF,zone_VEF,flux_bords,nb_comp);
  calcul_divergence(resu,grad,grad,nu,nu_turb,zone_Cl_VEF,zone_VEF,flux_bords_,nbr_comp);

  Debog::verifier("Op_Dift_standard_VEF_Face::ajouter_cas_vectoriel : resu 1 ",resu);


  if(filtrer_resu)
    {
      ref_cast(Champ_P1NC,inconnue_.valeur()).filtrer_resu(resu);
    }
}

void Op_Dift_standard_VEF_Face::calcul_divergence(DoubleTab& dif,
                                                  const DoubleTab& grad,
                                                  const DoubleTab& gradt,
                                                  const DoubleTab& nu,
                                                  const DoubleTab& nu_turb,
                                                  const Zone_Cl_VEF& zone_Cl_VEF,
                                                  const Zone_VEF& zone_VEF,
                                                  DoubleTab& tab_flux_bords,
                                                  int nbr_comp) const
{

  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  //const DoubleVect& vol = zone_VEF.volumes();
  int nb_faces = zone_VEF.nb_faces();
  //int face_int = zone_VEF.premiere_face_int();
  int num_cl;

  int num_face0,elem10, elem2;
  int i,j;
  double nu1,nu2;
  double nu1t,nu2t;
  double flux;
  //double dst=2./3.;

  double c1,c2,c3,c4;
  c1=0.;
  c2=0.;
  c3=0.;
  c4=0.;

  if(nu_lu)                c1=1.;
  if(nut_lu)                c2=1.;
  if(nu_transp_lu)        c3=1.;
  if(nut_transp_lu)        c4=1.;

  // Traitement des bords
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence dif 0 ",dif);

  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  int nb_cl=les_cl.size();
  for (num_cl=0; num_cl<nb_cl; num_cl++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      //const IntTab& elem_faces = zone_VEF.elem_faces();
      int nb_faces_b=le_bord.nb_faces();
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + nb_faces_b;

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          IntVect fait(nb_faces_b);
          fait = 0;
          for (num_face0=num1; num_face0<num2; num_face0++)
            {
              int face_associee=la_cl_perio.face_associee(num_face0-num1);
              if (fait(num_face0-num1) == 0)
                {
                  fait(num_face0-num1) = 1;
                  fait(face_associee) = 1;
                  elem10 = face_voisins(num_face0,0);
                  elem2 = face_voisins(num_face0,1);
                  nu1 = c1*nu[elem10]+c2*nu_turb[elem10];
                  nu2 = c1*nu[elem2]+c2*nu_turb[elem2];
                  nu1t= c3*nu[elem10]+c4*nu_turb[elem10];
                  nu2t= c3*nu[elem2]+c4*nu_turb[elem2];

                  for (i=0; i<nbr_comp; i++)
                    for (j=0; j<nbr_comp; j++)
                      {
                        dif(num_face0,i) -= face_normale(num_face0,j)*
                                            (nu1*grad(elem10,i,j)+nu1t*gradt(elem10,j,i)
                                             -(nu2*grad(elem2,i,j)+nu2t*gradt(elem2,j,i)));
                        //                Cerr << num_face << " " << i << dif(num_face,i) << finl;
                      }

                  for (i=0; i<nbr_comp; i++)
                    dif(face_associee+num1,i)=dif(num_face0,i);

                  //                      dif(num_face,num_comp) -= 0.5*ori*face_normale(num_face,num_comp2)*(2*nu*Sij(elem,num_comp,num_comp2));
                  // Patrick : le coefficient 0.5 dans l'ancienne formulation est incorrect : les faces periodiques ne sont traitees qu'UNE fois
                }
            } // Fin de la boucle sur les faces
        } // Fin du cas periodique
      else if ( (sub_type(Dirichlet,la_cl.valeur()))
                || (sub_type(Dirichlet_homogene,la_cl.valeur()))
              )
        {
          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) ||
              sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) )
            {
              for (num_face0=num1; num_face0<num2; num_face0++)
                {
                  int elem1=face_voisins(num_face0,0);
                  nu1 = c1*nu[elem1]+c2*nu_turb[elem1];
                  nu1t= c3*nu[elem1]+c4*nu_turb[elem1];

                  for (i=0; i<nbr_comp; i++)
                    {
                      flux=0.;
                      for (j=0; j<nbr_comp; j++)
                        flux +=face_normale(num_face0,j)*(nu1*grad(elem1,i,j)+nu1t*gradt(elem1,j,i));

                      dif(num_face0,i) = 0.; // PQ : valable en regime turbulent d'apres profil lineaire de la vitesse dans la sous couche visqueuse
                      tab_flux_bords(num_face0,i) -= flux;
                    }
                } // Fin de la boucle sur les faces
            } // Fin du cas paroi fixe ou defilante
          else  //On est en Dirichlet ou Dirichlet homogene : Boucle speciale car les volumes de controle sont modifies
            {
              //Cerr << "Si on passe la , on est mal!!! : Ce n'est pas code!!!" << finl;
            } // Fin du traitement des CL de type Dirichlet ou Dirichlet homogene hors paroi_fixe paroi_defilante
        } // Fin du traitement des CL de type Dirichlet ou Dirichlet homogene
      else // Pour les autres conditions aux limites
        {
          for (int num_face=num1; num_face<num2; num_face++)
            {
              int elem1=face_voisins(num_face,0);
              nu1 = c1*nu[elem1]+c2*nu_turb[elem1];
              nu1t= c3*nu[elem1]+c4*nu_turb[elem1];

              for (i=0; i<nbr_comp; i++)
                {
                  flux=0.;
                  for (j=0; j<nbr_comp; j++)
                    flux +=face_normale(num_face,j)*(nu1*grad(elem1,i,j)+nu1t*gradt(elem1,j,i));

                  dif(num_face,i) = 0.; // PQ : en attendant de faire mieux
                  tab_flux_bords(num_face,i) -= flux;
                }

            }
        }
    } // Fin du traitement des CL

  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence dif 1 ",dif);
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence nu 1 ",nu);
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence nu_turb 1 ",nu_turb);
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence grad 1 ",grad);
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence gradt 1 ",gradt);


  ////////////////////////////////////////////////////////////

  for (num_face0=zone_VEF.premiere_face_int(); num_face0<nb_faces; num_face0++)
    {
      elem10 = face_voisins(num_face0,0);
      elem2 = face_voisins(num_face0,1);
      nu1 = c1*nu[elem10]+c2*nu_turb[elem10];
      nu2 = c1*nu[elem2]+c2*nu_turb[elem2];
      nu1t= c3*nu[elem10]+c4*nu_turb[elem10];
      nu2t= c3*nu[elem2]+c4*nu_turb[elem2];

      for (i=0; i<nbr_comp; i++)
        for (j=0; j<nbr_comp; j++)
          dif(num_face0,i) -= face_normale(num_face0,j)*
                              (nu1*grad(elem10,i,j)+nu1t*gradt(elem10,j,i)
                               -(nu2*grad(elem2,i,j)+nu2t*gradt(elem2,j,i)));

      //                          (nu1*grad(elem1,i,j)+nu1t*gradt(elem1,j,i)-(i==j)*dst*nu1*div(elem1)
      //                         -(nu2*grad(elem2,i,j)+nu2t*gradt(elem2,j,i)-(i==j)*dst*nu1*div(elem2)));

      //              dif(num_face,num_comp) -= ori*face_normale(num_face,num_comp2)*(2*nu*Sij(elem,num_comp,num_comp2));
    } // Fin de la boucle sur les faces internes

  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence dif 2 ",dif);

  dif.echange_espace_virtuel();
  Debog::verifier("Op_Dift_standard_VEF_Face::calcul_divergence dif 3 ",dif);

}
DoubleTab& Op_Dift_standard_VEF_Face::ajouter(const DoubleTab& inconnue,
                                              DoubleTab& resu) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const DoubleTab& nu_turb=la_diffusivite_turbulente->valeurs();
  DoubleTab nu(nu_turb);
  remplir_nu(nu);

  int nb_comp = 1;
  int nb_dim = resu.nb_dim();
  if(nb_dim==2)
    nb_comp=resu.dimension(1);

  //  if(nb_comp==1)
  //    ajouter_cas_scalaire(inconnue, resu,ref_cast(DoubleTab,flux_bords, nu, nu_turb, zone_Cl_VEF, zone_VEF);
  //  else
  ajouter_cas_vectoriel(inconnue, resu, flux_bords_,nu, nu_turb, zone_Cl_VEF, zone_VEF, k, tau_tan_,indic_lp_neg_,indic_bas_Re_,nb_comp);
  modifier_flux(*this);
  return resu;
}

