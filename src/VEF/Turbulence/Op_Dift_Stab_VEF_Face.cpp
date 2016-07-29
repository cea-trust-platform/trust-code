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
// File:        Op_Dift_Stab_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Turbulence
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Dift_Stab_VEF_Face.h>
#include <Champ_P1NC.h>
#include <Periodique.h>
#include <Neumann_paroi.h>
#include <Echange_global_impose.h>
#include <Neumann_homogene.h>
#include <Symetrie.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Scalaire_impose_paroi.h>
#include <Pb_Thermohydraulique_Turbulent.h>
#include <Debog.h>
#include <DoubleTrav.h>
#include <Check_espace_virtuel.h>
#include <Porosites_champ.h>
#include <Paroi_negligeable_VEF.h>
#include <Paroi_negligeable_scal_VEF.h>

Implemente_instanciable(Op_Dift_Stab_VEF_Face,"Op_Dift_VEF_P1NC_stab",Op_Dift_VEF_Face);


double my_minimum(double a,double b,double c)
{
  if (a<=b)
    if (a<=c) return a;
    else return c;
  else if (b<=c) return b;
  else return c;
}

double my_maximum(double a,double b,double c)
{
  if (a>=b)
    if (a>=c) return a;
    else return c;
  else if (b>=c) return b;
  else return c;
}

double my_minimum(double a,double b)
{
  if (a<=b) return a;
  else return b;
}

double my_maximum(double a,double b)
{
  if (a>=b) return a;
  else return b;
}

Sortie& Op_Dift_Stab_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Dift_Stab_VEF_Face::readOn(Entree& is )
{
  Cerr<<"Je suis dans Op_Dift_Stab_VEF_Face::readOn()"<<finl;

  Motcle accouverte = "{" , accfermee = "}" ;
  Motcles les_mots(6);
  {
    les_mots[0]="nu";
    les_mots[1]="nut";
    les_mots[2]="nu_transp";
    les_mots[3]="nut_transp";
    les_mots[4]="standard";
    les_mots[5]="new_jacobian";
  }

  Motcle motlu;

  nu_lu_=1;
  nut_lu_=1;
  nu_transp_lu_=0;
  nut_transp_lu_=1;
  standard_=0;
  new_jacobian_=0;


  is>>motlu;
  if (motlu!=accouverte)
    if (Process::je_suis_maitre())
      {
        Cerr<<"Error in Op_Dift_Stab_VEF_Face::readOn()"<<finl;
        Cerr<<"Option keywords must be preceded by an open brace"<<finl;
        Process::exit();
      }

  is>>motlu;
  while (motlu!=accfermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          is>>nu_lu_;
          break;
        case 1 :
          is>>nut_lu_;
          break;
        case 2 :
          is>>nu_transp_lu_;
          break;
        case 3 :
          is>>nut_transp_lu_;
          break;
        case 4 :
          is>>standard_;
          break;
        case 5 :
          is>>new_jacobian_;
          break;
        default :
          if (Process::je_suis_maitre())
            {
              Cerr<<"Error in Op_Dift_Stab_VEF_Face::readOn()"<<finl;
              Cerr<<"Word "<<motlu<<" is unknown"<<finl;
              Cerr<<"Known keywords are : "<<les_mots<<finl;
              Process::exit();
            }
          break;
        }
      is>>motlu;
    }

  if (Process::je_suis_maitre())
    {
      Cerr<<"Options read in Op_Dift_Stab_VEF_Face operator are : ";
      Cerr<<"nu="<<nu_lu_<<" nut="<<nut_lu_<<" nu_transp=" <<nu_transp_lu_<<" nut_transp="<<nut_transp_lu_<<" standard="<<standard_<< finl;
    }

  is>>motlu;
  if (motlu!=accfermee)
    if (Process::je_suis_maitre())
      {
        Cerr<<"Error in Op_Dift_Stab_VEF_Face::readOn()"<<finl;
        Cerr<<"Final known keyword is the closing brace sign"<<finl;
        Process::exit();
      }

  return is ;
}

DoubleTab& Op_Dift_Stab_VEF_Face::ajouter(const DoubleTab& inconnue_org,DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const DoubleTab& nu_turb=la_diffusivite_turbulente->valeurs();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();

  const Nature_du_champ nature_champ=equation().inconnue().valeur().nature_du_champ();

  //  const int nb_dim=resu.nb_dim();
  //int nb_comp=1;
  //if(nb_dim==2) nb_comp=resu.dimension(1);

  DoubleTab resu2(resu);
  resu2=0.;


  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  DoubleTab nu,nu_turb_m;
  DoubleTab tab_inconnue_;

  const int marq=phi_psi_diffuse(equation());

  remplir_nu(nu_);
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  modif_par_porosite_si_flag(nu_turb,nu_turb_m,!marq,porosite_elem);
  const DoubleTab& inconnue=modif_par_porosite_si_flag(inconnue_org,tab_inconnue_,marq,porosite_face);

  assert_espace_virtuel_vect(nu);
  assert_espace_virtuel_vect(inconnue);
  assert_espace_virtuel_vect(nu_turb_m);
  Debog::verifier("Op_Dift_Stab_VEF_Face::ajouter nu",nu);
  Debog::verifier("Op_Dift_Stab_VEF_Face::ajouter nu_turb",nu_turb_m);
  Debog::verifier("Op_Dift_Stab_VEF_Face::ajouter inconnue_org",inconnue_org);
  Debog::verifier("Op_Dift_Stab_VEF_Face::ajouter inconnue",inconnue);

  if (nature_champ==scalaire)
    ajouter_cas_scalaire(inconnue,nu,nu_turb_m,resu2);
  if (nature_champ==vectoriel)
    ajouter_cas_vectoriel(inconnue,nu,nu_turb_m,resu2);
  if (nature_champ==multi_scalaire)
    ajouter_cas_multiscalaire(inconnue,nu,nu_turb_m,resu2);
  modifie_pour_Cl(inconnue,resu2);

  resu-=resu2;//-= car le laplacien est place en terme source dans l'equation
  modifier_flux(*this);
  return resu;
}

void Op_Dift_Stab_VEF_Face::modifie_pour_Cl(const DoubleTab& inconnue, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF =  la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  const DoubleVect& inconnueVect=ref_cast(DoubleVect,inconnue);
  DoubleVect& resuVect=ref_cast(DoubleVect,resu);

  const int nb_bords=les_cl.size();

  int nb_comp=1;
  if(resu.nb_dim()==2) nb_comp=resu.dimension(1);

  DoubleTab& tab_flux_bords = flux_bords_;
  tab_flux_bords.resize(zone_VEF.nb_faces_bord(),nb_comp);
  tab_flux_bords=0.;

  int num1=0, num2=0;
  int n_bord=0;
  int face=0;
  int face_associee=0;
  int ind_face=0;
  int dim=0;

  double surface=0.;
  double flux=0.;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1 = 0;
      num2 = le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              face_associee=le_bord.num_face(la_cl_perio.face_associee(ind_face));

              if (face<face_associee)
                for (dim=0; dim<nb_comp; dim++)
                  {
                    resuVect[face*nb_comp+dim]+=resuVect[face_associee*nb_comp+dim];
                    resuVect[face_associee*nb_comp+dim]=resuVect[face*nb_comp+dim];
                  }
            }
        }
      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi = ref_cast(Neumann_paroi,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              surface=zone_VEF.surface(face);

              for (dim=0; dim<nb_comp; dim++)
                {
                  flux=la_cl_paroi.flux_impose(ind_face,dim)*surface;
                  resuVect[face*nb_comp+dim]-=flux;
                  tab_flux_bords(face,dim)=flux;
                }
            }
        }
      if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi = ref_cast(Echange_externe_impose,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              surface=zone_VEF.surface(face);

              for (dim=0; dim<nb_comp; dim++)
                {
                  flux=la_cl_paroi.h_imp(ind_face,dim)*(la_cl_paroi.T_ext(ind_face,dim)-inconnueVect[face*nb_comp+dim])*surface;
                  resuVect[face*nb_comp+dim]-=flux;
                  tab_flux_bords(face,dim)=flux;
                }
            }
        }
      if (sub_type(Neumann_homogene,la_cl.valeur())
          || sub_type(Symetrie,la_cl.valeur())
          || sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);

              for (dim=0; dim<nb_comp; dim++)
                tab_flux_bords(face,dim)=0.;
            }
        }

    }
}

void Op_Dift_Stab_VEF_Face::ajouter_operateur_centre(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  int nb_comp=1;
  if(resuTab.nb_dim()==2) nb_comp=resuTab.dimension(1);

  const DoubleVect& inconnue=ref_cast(DoubleVect,inconnueTab);
  DoubleVect& resu=ref_cast(DoubleVect,resuTab);

  int elem=0;
  int facei_loc=0,facei=0;
  int facej_loc=0,facej=0;
  int dim=0;

  double inc_i=0.;
  double inc_j=0.;
  double delta_ij=0.;

  const IntTab& elem_faces=zone_VEF.elem_faces();

  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);

        for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
          {
            facej=elem_faces(elem,facej_loc);

            const double& aij=Aij(elem,facei_loc,facej_loc);

            for (dim=0; dim<nb_comp; dim++)
              {
                inc_i=inconnue[facei*nb_comp+dim];
                inc_j=inconnue[facej*nb_comp+dim];
                delta_ij=aij*(inc_j-inc_i);

                resu[facei*nb_comp+dim]+=delta_ij;
                resu[facej*nb_comp+dim]-=delta_ij;
              }
          }
      }
}

// void Op_Dift_Stab_VEF_Face::ajouter_operateur_centre_vectoriel(const DoubleTab& Aij_diag, const DoubleTab& Aij_extradiag, const DoubleTab& inconnue, DoubleTab& resu2)
// {
//   const Zone_VEF& zone_VEF = la_zone_vef.valeur();

//   const int nb_elem_tot=zone_VEF.nb_elem_tot();
//   const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

//   int nb_comp=1;
//   if(resuTab.nb_dim()==2) nb_comp=resuTab.dimension(1);

//   const DoubleVect& inconnue=ref_cast(DoubleVect,inconnueTab);
//   DoubleVect& resu=ref_cast(DoubleVect,resuTab);

//   int elem=0;
//   int facei_loc=0,facei=0;
//   int facej_loc=0,facej=0;
//   int dim=0;
//   int dim2=0;

//   double inc_i=0.;
//   double inc_j=0.;
//   double delta_ij=0.;

//   const IntTab& elem_faces=zone_VEF.elem_faces();

//   for (elem=0;elem<nb_elem_tot;elem++)
//     for (facei_loc=0;facei_loc<nb_faces_elem;facei_loc++)
//     {
//       facei=elem_faces(elem,facei_loc);

//       for (facej_loc=facei_loc+1;facej_loc<nb_faces_elem;facej_loc++)
//         {
//           facej=elem_faces(elem,facej_loc);

//           //Ajout de la partie diagonale : on tient compte de la symetrie
//           //de la matrice Aij_diag
//           for (dim=0;dim<nb_comp;dim++)
//             {
//               const double& aij=Aij_diag(elem,facei_loc,facej_loc,dim);

//               inc_i=inconnue[facei*nb_comp+dim];
//               inc_j=inconnue[facej*nb_comp+dim];
//               delta_ij=aij*(inc_j-inc_i);

//               resu[facei*nb_comp+dim]+=delta_ij;
//               resu[facej*nb_comp+dim]-=delta_ij;
//             }

//           //Ajout de la partie extra-diagonale : on tient compte de la symetrie
//           //de la matrice Aij_extradiag
//           for (dim=0;dim<nb_comp;dim++)
//             for (dim2=dim+1;dim2<nb_comp;dim2++)
//               {
//                 const double& aij=Aij_extradiag(elem,facei_loc,facej_loc,dim,dim2);

//                 inc_i=inconnue[facei*nb_comp+dim2];
//                 inc_j=inconnue[facej*nb_comp+dim2];
//                 delta_ij=aij*(inc_j-inc_i);
//                 resu[facei*nb_comp+dim]+=delta_ij;

//                 inc_i=inconnue[facei*nb_comp+dim];
//                 inc_j=inconnue[facej*nb_comp+dim];
//                 delta_ij=aij*(inc_j-inc_i);
//                 resu[facej*nb_comp+dim2]-=delta_ij;
//               }
//         }
//     }
// }

void Op_Dift_Stab_VEF_Face::ajouter_operateur_centre_vectoriel(const DoubleTab& Aij_diag, const DoubleTab& nu, const DoubleTab& inconnueTab, DoubleTab& resu2) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  int nb_comp=1;
  if(resu2.nb_dim()==2) nb_comp=resu2.dimension(1);

  const DoubleVect& inconnue=ref_cast(DoubleVect,inconnueTab);
  DoubleVect& resu=ref_cast(DoubleVect,resu2);

  int elem=0;
  int facei_loc=0,facei=0;
  int facej_loc=0,facej=0;
  int dim=0;
  int dim2=0;

  double inc_i=0.;
  double inc_j=0.;
  double delta_ij=0.;
  double nu_elem=0.;

  const IntTab& elem_faces=zone_VEF.elem_faces();

  for (elem=0; elem<nb_elem_tot; elem++)
    {
      nu_elem=nu(elem);
      for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
        {
          facei=elem_faces(elem,facei_loc);

          //Ajout de la partie diagonale : on tient compte de la symetrie
          //de la matrice Aij_diag
          for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
            {
              facej=elem_faces(elem,facej_loc);

              for (dim=0; dim<nb_comp; dim++)
                {
                  const double& aij=Aij_diag(elem,facei_loc,facej_loc,dim);

                  inc_i=inconnue[facei*nb_comp+dim];
                  inc_j=inconnue[facej*nb_comp+dim];
                  delta_ij=aij*(inc_j-inc_i);

                  resu[facei*nb_comp+dim]+=delta_ij;
                  resu[facej*nb_comp+dim]-=delta_ij;
                }
            }

          //Ajout de la partie extra-diagonale : on tient compte de la symetrie
          //de la partie extradiagonale de la matrice du laplacien
          for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
            if (facej_loc!=facei_loc)
              {
                facej=elem_faces(elem,facej_loc);

                for (dim=0; dim<nb_comp; dim++)
                  for (dim2=dim+1; dim2<nb_comp; dim2++)
                    {
                      const double& aij=aij_extradiag(elem,facei,facej,dim,dim2,nu_elem);

                      inc_i=inconnue[facei*nb_comp+dim2];
                      inc_j=inconnue[facej*nb_comp+dim2];
                      delta_ij=aij*(inc_j-inc_i);
                      resu[facei*nb_comp+dim]+=delta_ij;

                      inc_i=inconnue[facei*nb_comp+dim];
                      inc_j=inconnue[facej*nb_comp+dim];
                      delta_ij=aij*(inc_j-inc_i);
                      resu[facej*nb_comp+dim2]-=delta_ij;
                    }
              }
        }
    }
}

void Op_Dift_Stab_VEF_Face::ajouter_diffusion(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  int nb_comp=1;
  if(resuTab.nb_dim()==2) nb_comp=resuTab.dimension(1);

  const DoubleVect& inconnue=ref_cast(DoubleVect,inconnueTab);
  DoubleVect& resu=ref_cast(DoubleVect,resuTab);

  int elem=0;
  int facei_loc=0,facei=0;
  int facej_loc=0,facej=0;
  int dim=0;

  double inc_i=0.;
  double inc_j=0.;
  double delta_ij=0.;
  double dij=0.;

  const IntTab& elem_faces=zone_VEF.elem_faces();

  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);

        for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
          {
            facej=elem_faces(elem,facej_loc);

            const double& aij=Aij(elem,facei_loc,facej_loc);

            if (aij>0.)
              {
                dij=-aij;

                for (dim=0; dim<nb_comp; dim++)
                  {
                    inc_i=inconnue[facei*nb_comp+dim];
                    inc_j=inconnue[facej*nb_comp+dim];
                    delta_ij=dij*(inc_j-inc_i);

                    resu[facei*nb_comp+dim]+=delta_ij;
                    resu[facej*nb_comp+dim]-=delta_ij;
                  }
              }
          }
      }
}

void Op_Dift_Stab_VEF_Face::ajouter_diffusion_vectoriel(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  int nb_comp=1;
  if(resuTab.nb_dim()==2) nb_comp=resuTab.dimension(1);

  const DoubleVect& inconnue=ref_cast(DoubleVect,inconnueTab);
  DoubleVect& resu=ref_cast(DoubleVect,resuTab);

  int elem=0;
  int facei_loc=0,facei=0;
  int facej_loc=0,facej=0;
  int dim=0;

  double inc_i=0.;
  double inc_j=0.;
  double delta_ij=0.;
  double dij=0.;

  const IntTab& elem_faces=zone_VEF.elem_faces();

  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);

        for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
          {
            facej=elem_faces(elem,facej_loc);

            for (dim=0; dim<nb_comp; dim++)
              {
                const double& aij=Aij(elem,facei_loc,facej_loc,dim);

                if (aij>0.)
                  {
                    dij=-aij;

                    inc_i=inconnue[facei*nb_comp+dim];
                    inc_j=inconnue[facej*nb_comp+dim];
                    delta_ij=dij*(inc_j-inc_i);

                    resu[facei*nb_comp+dim]+=delta_ij;
                    resu[facej*nb_comp+dim]-=delta_ij;
                  }
              }
          }
      }
}

void Op_Dift_Stab_VEF_Face::ajouter_antidiffusion(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_tot=zone_VEF.nb_faces_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  int nb_comp=1;
  if(resuTab.nb_dim()==2) nb_comp=resuTab.dimension(1);

  const DoubleVect& inconnue=ref_cast(DoubleVect,inconnueTab);
  DoubleVect& resu=ref_cast(DoubleVect,resuTab);

  const DoubleTab& xv=zone_VEF.xv();
  DoubleTab rij(Objet_U::dimension);
  DoubleTab rji(rij);

  int elem=0;
  int facei_loc=0,facei=0;
  int facej_loc=0,facej=0;
  int dim=0;
  int dim2=0;

  double inc_i=0.;
  double inc_j=0.;
  double delta_ij=0.;
  double delta_imax=0.,delta_imin=0.;
  double delta_jmax=0.,delta_jmin=0.;
  double sij=0.;
  double muij=0.,muji=0.;

  bool ok_facei=false;
  bool ok_facej=false;

  const IntTab& elem_faces=zone_VEF.elem_faces();

  DoubleTab Minima(nb_faces_tot);
  DoubleTab Maxima(nb_faces_tot);


  for (dim=0; dim<nb_comp; dim++)
    {
      calculer_min(inconnueTab,dim,Minima);
      calculer_max(inconnueTab,dim,Maxima);

      for (elem=0; elem<nb_elem_tot; elem++)
        for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
          {
            facei=elem_faces(elem,facei_loc);
            ok_facei=is_dirichlet_faces_(facei);
            inc_i=inconnue[facei*nb_comp+dim];

            delta_imin=Minima(facei)-inc_i;
            delta_imax=Maxima(facei)-inc_i;
            assert(delta_imax>=0.);
            assert(delta_imin<=0.);

            for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
              {
                const double& aij=Aij(elem,facei_loc,facej_loc);

                if (aij>0.)
                  {
                    facej=elem_faces(elem,facej_loc);
                    ok_facej=is_dirichlet_faces_(facej);
                    inc_j=inconnue[facej*nb_comp+dim];

                    for (dim2=0; dim2<Objet_U::dimension; dim2++)
                      rij(dim2)=xv(facej,dim2)-xv(facei,dim2);

                    rji=rij;
                    rji*=-1.;

                    delta_ij=inc_i-inc_j;
                    delta_jmin=Minima(facej)-inc_j;
                    delta_jmax=Maxima(facej)-inc_j;
                    assert(delta_jmax>=0.);
                    assert(delta_jmin<=0.);

                    muij=calculer_gradients(facei,rij);
                    muji=calculer_gradients(facej,rji);

                    sij=0.; //reste a 0 si que des faces de Dirichlet
                    if (delta_ij>0.)
                      {
                        muij*=delta_imax;
                        muji*=-delta_jmin;

                        if (!ok_facei && !ok_facej) //pas de face de Dirichlet
                          sij=my_minimum(muij,delta_ij,muji);
                        if (!ok_facei && ok_facej) //facej Dirichlet et pas facei
                          sij=my_minimum(muij,delta_ij);
                        if (ok_facei && !ok_facej) //facei Dirichlet et pas facej
                          sij=my_minimum(delta_ij,muji);
                      }
                    else if (delta_ij<0.)
                      {
                        muij*=delta_imin;
                        muji*=-delta_jmax;

                        if (!ok_facei && !ok_facej) //pas de face de Dirichlet
                          sij=my_maximum(muij,delta_ij,muji);
                        if (!ok_facei && ok_facej) //facej Dirichlet et pas facei
                          sij=my_maximum(muij,delta_ij);
                        if (ok_facei && !ok_facej) //facei Dirichlet et pas facej
                          sij=my_maximum(delta_ij,muji);
                      }

                    resu[facei*nb_comp+dim]-=aij*sij;
                    resu[facej*nb_comp+dim]+=aij*sij;

                  }
              }
          }
    }
}

void Op_Dift_Stab_VEF_Face::ajouter_antidiffusion_vectoriel(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_tot=zone_VEF.nb_faces_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  int nb_comp=1;
  if(resuTab.nb_dim()==2) nb_comp=resuTab.dimension(1);


  const DoubleVect& inconnue=ref_cast(DoubleVect,inconnueTab);
  DoubleVect& resu=ref_cast(DoubleVect,resuTab);

  const DoubleTab& xv=zone_VEF.xv();
  DoubleTab rij(Objet_U::dimension);
  DoubleTab rji(rij);

  int elem=0;
  int facei_loc=0,facei=0;
  int facej_loc=0,facej=0;
  int dim=0;
  int dim2=0;

  double inc_i=0.;
  double inc_j=0.;
  double delta_ij=0.;
  double delta_imax=0.,delta_imin=0.;
  double delta_jmax=0.,delta_jmin=0.;
  double sij=0.;
  double muij=0.,muji=0.;

  bool ok_facei=false;
  bool ok_facej=false;

  const IntTab& elem_faces=zone_VEF.elem_faces();

  DoubleTab Minima(nb_faces_tot);
  DoubleTab Maxima(nb_faces_tot);


  for (dim=0; dim<nb_comp; dim++)
    {
      calculer_min(inconnueTab,dim,Minima);
      calculer_max(inconnueTab,dim,Maxima);

      for (elem=0; elem<nb_elem_tot; elem++)
        for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
          {
            facei=elem_faces(elem,facei_loc);
            ok_facei=is_dirichlet_faces_(facei);
            inc_i=inconnue[facei*nb_comp+dim];

            delta_imin=Minima(facei)-inc_i;
            delta_imax=Maxima(facei)-inc_i;
            assert(delta_imax>=0.);
            assert(delta_imin<=0.);

            for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
              {
                const double& aij=Aij(elem,facei_loc,facej_loc,dim);

                if (aij>0.)
                  {
                    facej=elem_faces(elem,facej_loc);
                    ok_facej=is_dirichlet_faces_(facej);
                    inc_j=inconnue[facej*nb_comp+dim];

                    for (dim2=0; dim2<Objet_U::dimension; dim2++)
                      rij(dim2)=xv(facej,dim2)-xv(facei,dim2);

                    rji=rij;
                    rji*=-1.;

                    delta_ij=inc_i-inc_j;
                    delta_jmin=Minima(facej)-inc_j;
                    delta_jmax=Maxima(facej)-inc_j;
                    assert(delta_jmax>=0.);
                    assert(delta_jmin<=0.);

                    muij=calculer_gradients(facei,rij);
                    muji=calculer_gradients(facej,rji);

                    sij=0.; //reste a 0 si que des faces de Dirichlet
                    if (delta_ij>0.)
                      {
                        muij*=delta_imax;
                        muji*=-delta_jmin;

                        if (!ok_facei && !ok_facej) //pas de face de Dirichlet
                          sij=my_minimum(muij,delta_ij,muji);
                        if (!ok_facei && ok_facej) //facej Dirichlet et pas facei
                          sij=my_minimum(muij,delta_ij);
                        if (ok_facei && !ok_facej) //facei Dirichlet et pas facej
                          sij=my_minimum(delta_ij,muji);
                      }
                    else if (delta_ij<0.)
                      {
                        muij*=delta_imin;
                        muji*=-delta_jmax;

                        if (!ok_facei && !ok_facej) //pas de face de Dirichlet
                          sij=my_maximum(muij,delta_ij,muji);
                        if (!ok_facei && ok_facej) //facej Dirichlet et pas facei
                          sij=my_maximum(muij,delta_ij);
                        if (ok_facei && !ok_facej) //facei Dirichlet et pas facej
                          sij=my_maximum(delta_ij,muji);
                      }

                    resu[facei*nb_comp+dim]-=aij*sij;
                    resu[facej*nb_comp+dim]+=aij*sij;

                  }
              }
          }
    }
}

void Op_Dift_Stab_VEF_Face::calculer_coefficients(const DoubleTab& nu, DoubleTab& Aij) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();

  const DoubleTab& face_normales=zone_VEF.face_normales();
  const DoubleVect& volumes=zone_VEF.volumes();

  double volume=0.;
  double signei=0.;
  double signej=0.;
  double psc=0.;
  double nu_elem=0.;

  int elem=0;
  int facei_loc=0,facei=0;
  int facej_loc=0,facej=0;
  int dim=0;

  assert(Aij.nb_dim()==3);
  assert(Aij.dimension(0)==nb_elem_tot);
  assert(Aij.dimension(1)==nb_faces_elem);
  assert(Aij.dimension(2)==nb_faces_elem);

  for (elem=0; elem<nb_elem_tot; elem++)
    {
      nu_elem=nu(elem);
      volume=1./volumes(elem);
      volume*=nu_elem;

      for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
        {
          facei=elem_faces(elem,facei_loc);

          signei=1.;
          if (face_voisins(facei,0)!=elem) signei=-1.;

          for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
            {
              facej=elem_faces(elem,facej_loc);

              signej=1.;
              if (face_voisins(facej,0)!=elem) signej=-1.;

              psc=0.;
              for (dim=0; dim<Objet_U::dimension; dim++)
                psc+=face_normales(facei,dim)*face_normales(facej,dim);
              psc*=signei*signej;
              psc*=volume;

              Aij(elem,facei_loc,facej_loc)=psc;
              Aij(elem,facej_loc,facei_loc)=psc;
            }
        }
    }
}

void Op_Dift_Stab_VEF_Face::calculer_coefficients_vectoriel_diag(const DoubleTab& nu, const DoubleTab& nu2, DoubleTab& Aij) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();
  const int nb_comp=equation().inconnue().valeurs().dimension(1);

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();

  const DoubleTab& face_normales=zone_VEF.face_normales();
  const DoubleVect& volumes=zone_VEF.volumes();

  double volume=0.;
  double signei=0.;
  double signej=0.;
  double psc=0.;
  double nu_elem=0.;
  double nu2_elem=0.;

  int elem=0;
  int facei_loc=0,facei=0;
  int facej_loc=0,facej=0;
  int dim=0;

  assert(nb_comp!=0);
  assert(nb_comp==Objet_U::dimension);
  assert(Aij.nb_dim()==4);
  assert(Aij.dimension(0)==nb_elem_tot);
  assert(Aij.dimension(1)==nb_faces_elem);
  assert(Aij.dimension(2)==nb_faces_elem);
  assert(Aij.dimension(3)==nb_comp);

  for (elem=0; elem<nb_elem_tot; elem++)
    {
      volume=1./volumes(elem);

      nu_elem=nu(elem);
      nu_elem*=volume;

      nu2_elem=nu2(elem);
      nu2_elem*=volume;

      for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
        {
          facei=elem_faces(elem,facei_loc);

          signei=1.;
          if (face_voisins(facei,0)!=elem) signei=-1.;

          for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
            {
              facej=elem_faces(elem,facej_loc);

              signej=1.;
              if (face_voisins(facej,0)!=elem) signej=-1.;

              //Partie standard
              psc=0.;
              for (dim=0; dim<Objet_U::dimension; dim++)
                psc+=face_normales(facei,dim)*face_normales(facej,dim);
              psc*=signei*signej;
              psc*=nu_elem;

              for (dim=0; dim<nb_comp; dim++)
                {
                  Aij(elem,facei_loc,facej_loc,dim)=psc;
                  Aij(elem,facej_loc,facei_loc,dim)=psc;
                }

              //Partie transposee
              for (dim=0; dim<nb_comp; dim++)
                {
                  psc=face_normales(facei,dim)*face_normales(facej,dim);
                  psc*=signei*signej;
                  psc*=nu2_elem;

                  Aij(elem,facei_loc,facej_loc,dim)+=psc;
                  Aij(elem,facej_loc,facei_loc,dim)+=psc;
                }
            }
        }
    }
}

// void Op_Dift_Stab_VEF_Face::calculer_coefficients_vectoriel_extradiag(const DoubleTab& nu, DoubleTab& Aij) const
// {
//   const Zone_VEF& zone_VEF = la_zone_vef.valeur();

//   const int nb_elem_tot=zone_VEF.nb_elem_tot();
//   const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();
//   const int nb_comp=equation().inconnue().valeurs().nb_dim();

//   const IntTab& elem_faces=zone_VEF.elem_faces();
//   const IntTab& face_voisins=zone_VEF.face_voisins();

//   const DoubleTab& face_normales=zone_VEF.face_normales();
//   const DoubleVect& volumes=zone_VEF.volumes();

//   double volume=0.;
//   double signei=0.;
//   double signej=0.;
//   double psc=0.;
//   double nu_elem=0.;
//   double nu2_elem=0.;

//   int elem=0;
//   int facei_loc=0,facei=0;
//   int facej_loc=0,facej=0;
//   int dim=0;
//   int dim2=0;

//   assert(nb_comp!=0);
//   assert(Aij.nb_dim()==5);
//   assert(Aij.dimension(0)==nb_elem_tot);
//   assert(Aij.dimension(1)==nb_faces_elem);
//   assert(Aij.dimension(2)==nb_faces_elem);
//   assert(Aij.dimension(3)==nb_comp);
//   assert(Aij.dimension(4)==nb_comp);

//   for (elem=0;elem<nb_elem_tot;elem++)
//     {
//       volume=1./volumes(elem);

//       nu_elem=nu(elem);
//       nu_elem*=volume;

//       for (facei_loc=0;facei_loc<nb_faces_elem;facei_loc++)
//         {
//           facei=elem_faces(elem,facei_loc);

//           signei=1.;
//           if (face_voisins(facei,0)!=elem) signei=-1.;

//           for (facej_loc=facei_loc+1;facej_loc<nb_faces_elem;facej_loc++)
//             {
//               facej=elem_faces(elem,facej_loc);

//               signej=1.;
//               if (face_voisins(facej,0)!=elem) signej=-1.;

//               //Partie transposee
//               for (dim=0;dim<nb_comp;dim++)
//                 for (dim2=dim+1;dim2<nb_comp;dim2++)
//                   {
//                     psc=face_normales(facei,dim2)*face_normales(facej,dim);
//                     psc*=signei*signej;
//                     psc*=nu_elem;

//                     Aij(elem,facei_loc,facej_loc,dim,dim2)=psc;
//                     Aij(elem,facej_loc,facei_loc,dim2,dim)=psc;
//                   }
//             }
//         }
//     }
// }

double Op_Dift_Stab_VEF_Face::calculer_gradients(int facei, const DoubleTab& rij) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& get_num_fac_loc=zone_VEF.get_num_fac_loc();

  const DoubleTab& face_normales=zone_VEF.face_normales();
  const DoubleVect& volumes=zone_VEF.volumes();

  double volume=0.;
  double signek=0.;
  double psc=0.;

  int elem=0;
  int elem_loc=0;
  int facei_loc=0;
  int facek_loc=0,facek=0;
  int dim=0;

  double mu_ij=0.;
  for (elem_loc=0; elem_loc<2; elem_loc++)
    {
      facei_loc=get_num_fac_loc(facei,elem_loc);
      elem=face_voisins(facei,elem_loc);

      if (elem!=-1)
        {
          volume+=volumes(elem);

          for (facek_loc=0; facek_loc<nb_faces_elem; facek_loc++)
            if (facek_loc!=facei_loc)
              {
                facek=elem_faces(elem,facek_loc);

                signek=1.;
                if (face_voisins(facek,0)!=elem) signek=-1.;

                psc=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  psc+=face_normales(facek,dim)*rij(dim);
                psc*=signek;
                if (psc<0.) psc*=-1.;

                mu_ij+=psc;
              }
        }
    }

  mu_ij/=volume;
  mu_ij*=2.;
  return mu_ij;
}

void Op_Dift_Stab_VEF_Face::calculer_min(const DoubleTab& inconnueTab, int& dim, DoubleTab& Minima) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();
  const int nb_faces_tot=zone_VEF.nb_faces_tot();

  const IntTab& elem_faces=zone_VEF.elem_faces();

  int elem=0;
  int facei_loc=0,facei=0;
  int facej_loc=0,facej=0;

  int nb_comp = 1;
  if(inconnueTab.nb_dim()==2) nb_comp=inconnueTab.dimension(1);

  double inc_i=0.;
  double inc_j=0.;

  const DoubleVect& inconnue=ref_cast(DoubleVect,inconnueTab);

  assert(Minima.nb_dim()==1);
  assert(Minima.dimension(0)==nb_faces_tot);
  assert(dim<nb_comp);

  for (facei=0; facei<nb_faces_tot; facei++)
    Minima(facei)=inconnue[facei*nb_comp+dim];

  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);

        inc_i=inconnue[facei*nb_comp+dim];
        double& mini=Minima(facei);

        for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
          {
            facej=elem_faces(elem,facej_loc);

            inc_j=inconnue[facej*nb_comp+dim];
            double& minj=Minima(facej);

            if (inc_j<mini) mini=inc_j;
            if (inc_i<minj) minj=inc_i;
          }
      }

  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  const int nb_bords=les_cl.size();

  int num1=0, num2=0;
  int n_bord=0;
  int ind_face=0;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1 = 0;
      num2 = le_bord.nb_faces_tot();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei=le_bord.num_face(ind_face);
              facej=le_bord.num_face(la_cl_perio.face_associee(ind_face));

              if (facei<facej)
                {
                  double& mini=Minima(facei);
                  double& minj=Minima(facej);

                  if (mini<minj) minj=mini;
                  if (minj<mini) mini=minj;
                }
            }
        }
    }
}

void Op_Dift_Stab_VEF_Face::calculer_max(const DoubleTab& inconnueTab, int& dim, DoubleTab& Maxima) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();
  const int nb_faces_tot=zone_VEF.nb_faces_tot();

  const IntTab& elem_faces=zone_VEF.elem_faces();

  int elem=0;
  int facei_loc=0,facei=0;
  int facej_loc=0,facej=0;

  int nb_comp = 1;
  if(inconnueTab.nb_dim()==2) nb_comp=inconnueTab.dimension(1);

  double inc_i=0.;
  double inc_j=0.;

  const DoubleVect& inconnue=ref_cast(DoubleVect,inconnueTab);

  assert(Maxima.nb_dim()==1);
  assert(Maxima.dimension(0)==nb_faces_tot);
  assert(dim<nb_comp);

  for (facei=0; facei<nb_faces_tot; facei++)
    Maxima(facei)=inconnue[facei*nb_comp+dim];

  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);

        inc_i=inconnue[facei*nb_comp+dim];
        double& maxi=Maxima(facei);

        for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
          {
            facej=elem_faces(elem,facej_loc);

            inc_j=inconnue[facej*nb_comp+dim];
            double& maxj=Maxima(facej);

            if (inc_j>maxi) maxi=inc_j;
            if (inc_i>maxj) maxj=inc_i;
          }
      }

  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  const int nb_bords=les_cl.size();

  int num1=0, num2=0;
  int n_bord=0;
  int ind_face=0;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1 = 0;
      num2 = le_bord.nb_faces_tot();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei=le_bord.num_face(ind_face);
              facej=le_bord.num_face(la_cl_perio.face_associee(ind_face));

              if (facei<facej)
                {
                  double& maxi=Maxima(facei);
                  double& maxj=Maxima(facej);

                  if (maxi>maxj) maxj=maxi;
                  if (maxj>maxi) maxi=maxj;
                }
            }
        }

    }
}

void Op_Dift_Stab_VEF_Face::completer()
{
  Op_Dift_VEF_Face::completer();

  {
    const Zone_VEF& zone_VEF=la_zone_vef.valeur();
    const Zone_Cl_VEF& zone_Cl_VEF=la_zcl_vef.valeur();

    const Conds_lim& les_cl=zone_Cl_VEF.les_conditions_limites();

    const int nb_bord=les_cl.size();
    const int nb_faces_tot=zone_VEF.nb_faces_tot();

    int ind_face=-1;

    is_dirichlet_faces_.resize(nb_faces_tot);
    is_dirichlet_faces_=0;

    for (int n_bord=0; n_bord<nb_bord; n_bord++)
      {
        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        int nb_faces_bord_tot=le_bord.nb_faces_tot();
        int face=-1;

        if ( (sub_type(Dirichlet,la_cl.valeur()))
             || (sub_type(Dirichlet_homogene,la_cl.valeur()))
           )
          for (ind_face=0; ind_face<nb_faces_bord_tot; ind_face++)
            {
              face = le_bord.num_face(ind_face);
              is_dirichlet_faces_(face)=1;
            }
      }
  }
}

void Op_Dift_Stab_VEF_Face::ajouter_cas_scalaire(const DoubleTab& inconnue,const DoubleTab& nu,const DoubleTab& nu_turb_m, DoubleTab& resu2) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  DoubleTab Aij(nb_elem_tot,nb_faces_elem,nb_faces_elem);

  DoubleTab nu_total(nu);
  nu_total+=nu_turb_m;

  calculer_coefficients(nu_total,Aij);
  ajouter_operateur_centre(Aij,inconnue,resu2);
  if (!standard_)
    {
      ajouter_diffusion(Aij,inconnue,resu2);
      ajouter_antidiffusion(Aij,inconnue,resu2);
    }
}

void Op_Dift_Stab_VEF_Face::ajouter_cas_multiscalaire(const DoubleTab& inconnue,const DoubleTab& nu,const DoubleTab& nu_turb_m, DoubleTab& resu2) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  DoubleTab Aij(nb_elem_tot,nb_faces_elem,nb_faces_elem);

  DoubleTab nu_total(nu);
  nu_total+=nu_turb_m;

  calculer_coefficients(nu_total,Aij);
  ajouter_operateur_centre(Aij,inconnue,resu2);
  if (!standard_)
    {
      ajouter_diffusion(Aij,inconnue,resu2);
      ajouter_antidiffusion(Aij,inconnue,resu2);
    }
}

void Op_Dift_Stab_VEF_Face::ajouter_cas_vectoriel(const DoubleTab& inconnue,const DoubleTab& nu,const DoubleTab& nu_turb_m, DoubleTab& resu2) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();
  const int nb_comp=resu2.dimension(1);

  DoubleTab Aij_diag(nb_elem_tot,nb_faces_elem,nb_faces_elem,nb_comp);
  //   DoubleTab Aij_extradiag(nb_elem_tot,nb_faces_elem,nb_faces_elem,nb_comp,nb_comp);

  DoubleTab nu_total(nu);
  if (nu_lu_==0) nu_total=0.;
  if (nut_lu_) nu_total+=nu_turb_m;

  DoubleTab nu_transp_total(nu);
  if (nu_transp_lu_==0) nu_transp_total=0.;
  if (nut_transp_lu_) nu_transp_total+=nu_turb_m;

  calculer_coefficients_vectoriel_diag(nu_total,nu_transp_total,Aij_diag);

  //   calculer_coefficients_vectoriel_extradiag(nu_transp_total,Aij_extradiag);
  //   ajouter_operateur_centre_vectoriel(Aij_diag,Aij_extradiag,inconnue,resu2);

  ajouter_operateur_centre_vectoriel(Aij_diag,nu_transp_total,inconnue,resu2);
  if (!standard_)
    {
      ajouter_diffusion_vectoriel(Aij_diag,inconnue,resu2);
      ajouter_antidiffusion_vectoriel(Aij_diag,inconnue,resu2);
    }
}

void Op_Dift_Stab_VEF_Face::ajouter_contribution_cl(const DoubleTab& transporte, Matrice_Morse& matrice, const DoubleTab& nu, const DoubleTab& nu_turb, const DoubleVect& porosite_eventuelle) const
{
  if (!new_jacobian_)
    Op_Dift_VEF_Face::ajouter_contribution_cl(transporte,matrice,nu,nu_turb,porosite_eventuelle);
  else
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
      int nb_dim = transporte.nb_dim();
      int nb_comp = 1;
      if(nb_dim==2)
        nb_comp=transporte.dimension(1);

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
                              if (valA<0.) valA=0.;
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
                                      if (nb_dim!=1) // on ajoute grad_U transpose
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

                      if (!sub_type(Paroi_negligeable_scal_VEF,loiparth.valeur()))
                        {
                          //Comme on ne doit pas utiliser cet operateur sans la loi Pironneau,
                          //on ne doit jamais passer ici
                          Cerr<<"Error in  Op_Dift_Stab_VEF_Face::ajouter_contribution_cl()"<<finl;
                          Cerr<<"Case not coded yet"<<finl;
                          Process::exit();
                        }
                    }
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
                        if (valA<0.) valA=0.;
                        int orientation = 1;
                        if ( (elem == face_voisins(j,0)) || (face_voisins(num_face,(0+1)%2) == face_voisins(j,(0+1)%2)))
                          orientation = -1;
                        // retire terme croisee pour l'intant
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
                            if (nb_dim!=1) // on ajoute grad_U transpose
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
}

void Op_Dift_Stab_VEF_Face::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const
{
  if (!new_jacobian_)
    Op_Dift_VEF_Face::ajouter_contribution(transporte,matrice);
  else
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
      int nb_dim = transporte.nb_dim();
      int nb_comp = 1;
      if(nb_dim==2)
        nb_comp=transporte.dimension(1);

      int nb_faces_elem = zone_VEF.zone().nb_faces_elem();

      const DoubleTab& nu_turb_=la_diffusivite_turbulente->valeurs();
      //       const DoubleTab& face_normale = zone_VEF.face_normales();
      //      const DoubleVect& volumes = zone_VEF.volumes();
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
                          //double tmp = 0;
                          if (nb_dim!=1) // on ajoutera grad_U transpose
                            {
                              //int orientation = 1;
                              //if ( (elem == face_voisins(j,l)) || (face_voisins(num_face,1-l) == face_voisins(j,1-l)))
                              // orientation = -1;
                              //  tmp = orientation * nu_turb(elem) / volumes(elem);
                            }
                          double valA = viscA(num_face,j,elem,d_nu);
                          if (valA<0.) valA=0.;

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
                              if (nb_dim!=1) // on ajoute grad_U transpose
                                {
                                  for (int nc2=0; nc2<nb_comp; nc2++)
                                    {
                                      int n1=num_face*nb_comp+nc2;
                                      int j1=j*nb_comp+nc2;
                                      double coeff_s=0.;//tmp*face_normale(num_face,nc2)*face_normale(j,nc);
                                      matrice(n0,n1)+=coeff_s*porosite_eventuelle(num_face);
                                      matrice(n0,j1)-=coeff_s*porosite_eventuelle(j);
                                      if(j<nb_faces) // On traite les faces reelles
                                        {
                                          double coeff_s2=0.;//tmp*face_normale(num_face,nc)*face_normale(j,nc2);
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
}

void Op_Dift_Stab_VEF_Face::ajouter_contribution_multi_scalaire(const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  if (!new_jacobian_)
    Op_Dift_VEF_Face::ajouter_contribution_multi_scalaire(transporte,matrice);
  else
    {
      modifier_matrice_pour_periodique_avant_contribuer(matrice,equation());
      // On remplit le tableau nu car l'assemblage d'une
      // matrice avec ajouter_contribution peut se faire
      // avant le premier pas de temps
      remplir_nu(nu_);
      const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
      const Zone_VEF& zone_VEF = la_zone_vef.valeur();
      const IntTab& elemfaces = zone_VEF.elem_faces();
      const IntTab& face_voisins = zone_VEF.face_voisins();

      int nb_faces = zone_VEF.nb_faces();
      int nb_comp = 1;
      int nb_dim = transporte.nb_dim();
      if(nb_dim==2)
        nb_comp=transporte.dimension(1);

      int i,j,num_face0;
      int elem1;
      int nb_faces_elem = zone_VEF.zone().nb_faces_elem();

      double valA, d_nu;
      const DoubleTab& nu_turb_=la_diffusivite_turbulente->valeurs();
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
          //const IntTab& elemfaces = zone_VEF.elem_faces();
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
                      int elem = face_voisins(num_face0,l);

                      // On elimine les elements avec CL de paroi (rang>=1)
                      // int rang = rang_elem_non_std(elem);
                      //if (rang<0)
                      {
                        for (i=0; i<nb_faces_elem; i++)
                          {
                            if ( (j= elemfaces(elem,i)) > num_face0 )
                              {

                                int fac_loc=0;
                                int ok=1;
                                while ((fac_loc<nb_faces_elem) && (elemfaces(elem,fac_loc)!=num_face0)) fac_loc++;
                                if (fac_loc==nb_faces_elem)
                                  ok=0;
                                for (int nc=0; nc<nb_comp; nc++)
                                  {
                                    d_nu = nu(elem,nc)+nu_turb(elem);
                                    valA = viscA(num_face0,j,elem,d_nu);
                                    if (valA<0.) valA=0.;

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


                      if (!sub_type(Paroi_negligeable_scal_VEF,loiparth.valeur()))
                        {
                          const DoubleTab& face_normale = zone_VEF.face_normales();
                          const DoubleVect& vol = zone_VEF.volumes();

                          const Paroi_scal_hyd_base_VEF& paroi_scal_vef = ref_cast(Paroi_scal_hyd_base_VEF,loiparth.valeur());
                          const DoubleVect& d_equiv=paroi_scal_vef.equivalent_distance(n_bord);
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
                                  for (i=0; i<nb_faces_elem; i++)
                                    {
                                      j= elemfaces(elem1,i);

                                      for (int ii=0; ii<nb_faces_elem; ii++)
                                        {
                                          le_mauvais_gradient=0;
                                          int jj=elemfaces(elem1,ii);
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
                  for (int ii=0; ii<nb_faces_elem; ii++)
                    if (( (j= elemfaces(elem,ii)) > num_face ) || (ind_face>=nb_faces_bord_reel))
                      {
                        //int orientation = zone_VEF.oriente_normale(j,elem);
                        // retire terme croisee pour l'intant
                        //orientation=0;
                        for (int nc=0; nc<nb_comp; nc++)
                          {
                            d_nu = nu(elem,nc)+nu_turb(elem);
                            valA = viscA(num_face,j,elem,d_nu);
                            if (valA<0.) valA=0.;
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
      int numpremiereface=zone_VEF.premiere_face_int();
      for (num_face0=numpremiereface; num_face0<nb_faces; num_face0++)
        {
          // ICI
          for (int l=0; l<2; l++)
            {
              int elem = face_voisins(num_face0,l);
              // On elimine les elements avec CL de paroi (rang>=1)
              // int rang = rang_elem_non_std(elem);
              //if (rang<0)
              {
                for (i=0; i<nb_faces_elem; i++)
                  {
                    if ( (j= elemfaces(elem,i)) > num_face0 )
                      {
                        //int orientation = 1;
                        //if ( (elem == face_voisins(j,l)) || (face_voisins(num_face,(l+1)%2) == face_voisins(j,(l+1)%2)))
                        //orientation = -1;

                        for (int nc=0; nc<nb_comp; nc++)
                          {
                            d_nu = nu(elem,nc)+nu_turb(elem);
                            valA = viscA(num_face0,j,elem,d_nu);
                            if (valA<0.) valA=0.;
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
}
