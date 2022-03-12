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
// File:        Op_Diff_VEF_Face_Stab.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_VEF_Face_Stab.h>
#include <Champ_P1NC.h>
#include <Champ_Q1NC.h>
#include <Champ_Uniforme.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Neumann_paroi.h>
#include <Echange_externe_impose.h>
#include <Neumann_sortie_libre.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Neumann_homogene.h>
#include <Ref_Champ_P1NC.h>
#include <Ref_Champ_Q1NC.h>
#include <Milieu_base.h>
#include <Debog.h>
#include <TRUSTTrav.h>
#include <Probleme_base.h>
#include <Navier_Stokes_std.h>
#include <Porosites_champ.h>
#include <Check_espace_virtuel.h>
#include <SFichier.h>
#include <ArrOfBit.h>
#include <Schema_Temps_base.h>

Implemente_instanciable(Op_Diff_VEF_Face_Stab,"Op_Diff_VEFSTAB_const_P1NC",Op_Diff_VEF_Face);

double minimum(double a,double b,double c)
{
  if (a<=b)
    if (a<=c) return a;
    else return c;
  else if (b<=c) return b;
  else return c;
}

double maximum(double a,double b,double c)
{
  if (a>=b)
    if (a>=c) return a;
    else return c;
  else if (b>=c) return b;
  else return c;
}

double minimum(double a,double b)
{
  if (a<=b) return a;
  else return b;
}

double maximum(double a,double b)
{
  if (a>=b) return a;
  else return b;
}

//// printOn
//

Sortie& Op_Diff_VEF_Face_Stab::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Diff_VEF_Face_Stab::readOn(Entree& s )
{
  Motcle motlu,accouverte="{",accfermee="}";
  Motcles les_mots(3);
  {
    les_mots[0]="standard";
    les_mots[1]="info";
    les_mots[2]="new_jacobian";
  }

  standard_=0;
  info_=0;
  new_jacobian_=0;

  s>>motlu;
  if (motlu!=accouverte)
    if (Process::je_suis_maitre())
      {
        Cerr<<"Error in Op_Diff_VEF_Face_Stab::readOn()"<<finl;
        Cerr<<"Option keywords must be preceded by an open brace"<<finl;
        Process::exit();
      }

  s>>motlu;
  while (motlu!=accfermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          s>>standard_;
          break;
        case 1 :
          s>>info_;
          break;
        case 2 :
          s>>new_jacobian_;
          break;
        default :
          if (Process::je_suis_maitre())
            {
              Cerr<<"Error in Op_Diff_VEF_Face_Stab::readOn()"<<finl;
              Cerr<<"Word "<<motlu<<" is unknown"<<finl;
              Cerr<<"Known keywords are : "<<les_mots<<finl;
              Process::exit();
            }
          break;
        }
      s>>motlu;
    }
  return s;
}


DoubleTab& Op_Diff_VEF_Face_Stab::ajouter(const DoubleTab& inconnue_org, DoubleTab& resu) const
{
  remplir_nu(nu_);
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  // soit on a div(phi nu grad inco)
  // soit on a div(nu grad phi inco)
  // cela depend si on diffuse phi_psi ou psi
  DoubleTab nu;
  DoubleTab tab_inconnue;

  int marq=phi_psi_diffuse(equation());

  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();

  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
  const DoubleTab& inconnue=modif_par_porosite_si_flag(inconnue_org,tab_inconnue,marq,porosite_face);
  //
  //
  //
  DoubleTab resu2(resu);
  resu2=0.;

  DoubleTab Aij(nb_elem_tot,nb_faces_elem,nb_faces_elem);
  calculer_coefficients(nu,Aij);

  ajouter_operateur_centre(Aij,inconnue,resu2);
  if (!standard_)
    {
      ajouter_diffusion(Aij,inconnue,resu2);
      ajouter_antidiffusion(Aij,inconnue,resu2);
    }
  modifie_pour_Cl(inconnue,resu2);

  modifier_flux(*this);

  resu-=resu2;//-= car le laplacien est place en terme source dans l'equation
  return resu;
}

void Op_Diff_VEF_Face_Stab::modifie_pour_Cl(const DoubleTab& inconnue, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF =  la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  const DoubleVect& inconnueVect=static_cast<const DoubleVect&>(inconnue);
  DoubleVect& resuVect=static_cast<DoubleVect&>(resu);

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

void Op_Diff_VEF_Face_Stab::ajouter_operateur_centre(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  int nb_comp=1;
  if(resuTab.nb_dim()==2) nb_comp=resuTab.dimension(1);

  const DoubleVect& inconnue=static_cast<const DoubleVect&>(inconnueTab);
  DoubleVect& resu=static_cast<DoubleVect&>(resuTab);

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

            const double aij=Aij(elem,facei_loc,facej_loc);

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

void Op_Diff_VEF_Face_Stab::ajouter_diffusion(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  int nb_comp=1;
  if(resuTab.nb_dim()==2) nb_comp=resuTab.dimension(1);

  const DoubleVect& inconnue=static_cast<const DoubleVect&>(inconnueTab);
  DoubleVect& resu=static_cast<DoubleVect&>(resuTab);

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

            const double aij=Aij(elem,facei_loc,facej_loc);

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

void Op_Diff_VEF_Face_Stab::ajouter_antidiffusion(const DoubleTab& Aij, const DoubleTab& inconnueTab, DoubleTab& resuTab) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_tot=zone_VEF.nb_faces_tot();
  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();

  int nb_comp=1;
  if(resuTab.nb_dim()==2) nb_comp=resuTab.dimension(1);


  const DoubleVect& inconnue=static_cast<const DoubleVect&>(inconnueTab);
  DoubleVect& resu=static_cast<DoubleVect&>(resuTab);

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
  double sij_max=DMINFLOAT;
  double sij_min=DMAXFLOAT;
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
                const double aij=Aij(elem,facei_loc,facej_loc);

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
                          sij=minimum(muij,delta_ij,muji);
                        if (!ok_facei && ok_facej) //facej Dirichlet et pas facei
                          sij=minimum(muij,delta_ij);
                        if (ok_facei && !ok_facej) //facei Dirichlet et pas facej
                          sij=minimum(delta_ij,muji);
                      }
                    else if (delta_ij<0.)
                      {
                        muij*=delta_imin;
                        muji*=-delta_jmax;

                        if (!ok_facei && !ok_facej) //pas de face de Dirichlet
                          sij=maximum(muij,delta_ij,muji);
                        if (!ok_facei && ok_facej) //facej Dirichlet et pas facei
                          sij=maximum(muij,delta_ij);
                        if (ok_facei && !ok_facej) //facei Dirichlet et pas facej
                          sij=maximum(delta_ij,muji);
                      }

                    if (info_)
                      if (delta_ij!=0.)
                        {
                          double tmp=sij/delta_ij;
                          if (tmp>sij_max) sij_max=tmp;
                          if (tmp<sij_min) sij_min=tmp;
                        }

                    resu[facei*nb_comp+dim]-=aij*sij;
                    resu[facej*nb_comp+dim]+=aij*sij;

                  }
              }
          }
    }

  {
    sij_max = Process::mp_max(sij_max);
    sij_min = Process::mp_min(sij_min);
    if (info_ && Process::je_suis_maitre())
      {
        SFichier mem_fichier("sij_memory.txt");
        mem_fichier<<sij_max<<" "<<sij_min<<finl;
      }
  }
}

void Op_Diff_VEF_Face_Stab::calculer_coefficients(const DoubleTab& nu, DoubleTab& Aij) const
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

double Op_Diff_VEF_Face_Stab::calculer_gradients(int facei, const DoubleTab& rij) const
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

void Op_Diff_VEF_Face_Stab::calculer_min(const DoubleTab& inconnueTab, int& dim, DoubleTab& Minima) const
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

  const DoubleVect& inconnue=static_cast<const DoubleVect&>(inconnueTab);

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

void Op_Diff_VEF_Face_Stab::calculer_max(const DoubleTab& inconnueTab, int& dim, DoubleTab& Maxima) const
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

  const DoubleVect& inconnue=static_cast<const DoubleVect&>(inconnueTab);

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

void Op_Diff_VEF_Face_Stab::completer()
{
  Op_Diff_VEF_Face::completer();

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

void Op_Diff_VEF_Face_Stab::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  if (!new_jacobian_)
    Op_Diff_VEF_Face::ajouter_contribution(transporte,matrice);
  else
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

      int n1 = zone_VEF.nb_faces();
      int nb_comp = 1;
      int nb_dim = transporte.nb_dim();

      DoubleTab nu;
      int marq=phi_psi_diffuse(equation());
      const DoubleVect& porosite_elem = zone_VEF.porosite_elem();

      // soit on a div(phi nu grad inco)
      // soit on a div(nu grad phi inco)
      // cela depend si on diffuse phi_psi ou psi
      modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
      DoubleVect porosite_eventuelle(zone_VEF.porosite_face());
      if (!marq)
        porosite_eventuelle=1;


      if(nb_dim==2)
        nb_comp=transporte.dimension(1);

      int i,j,num_face;
      int elem1,elem2;
      int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
      double val;

      int nb_bords=zone_VEF.nb_front_Cl();
      for (int n_bord=0; n_bord<nb_bords; n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();

          if (sub_type(Periodique,la_cl.valeur()))
            {
              const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
              int fac_asso;
              // on ne parcourt que la moitie des faces periodiques
              // on copiera a la fin le resultat dans la face associe..
              int num2b=num1+le_bord.nb_faces()/2;
              for (num_face=num1; num_face<num2b; num_face++)
                {
                  elem1 = face_voisins(num_face,0);
                  elem2 = face_voisins(num_face,1);
                  fac_asso = la_cl_perio.face_associee(num_face-num1)+num1;
                  for (i=0; i<nb_faces_elem; i++)
                    {
                      if ( (j=elem_faces(elem1,i)) > num_face )
                        {
                          val = viscA(num_face,j,elem1,nu(elem1));
                          if (val<0.) val=0.;
                          for (int nc=0; nc<nb_comp; nc++)
                            {
                              int n0=num_face*nb_comp+nc;
                              int j0=j*nb_comp+nc;

                              matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                              matrice(n0,j0)-=val*porosite_eventuelle(j);
                              matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                              matrice(j0,j0)+=val*porosite_eventuelle(j);

                            }
                        }
                      if (elem2!=-1)
                        if ( (j=elem_faces(elem2,i)) > num_face )
                          {
                            val= viscA(num_face,j,elem2,nu(elem2));
                            if (val<0.) val=0.;
                            for (int nc=0; nc<nb_comp; nc++)
                              {
                                int n0=num_face*nb_comp+nc;
                                int j0=j*nb_comp+nc;
                                int  n0perio=fac_asso*nb_comp+nc;
                                matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                                matrice(n0,j0)-=val*porosite_eventuelle(j);
                                matrice(j0,n0perio)-=val*porosite_eventuelle(num_face);
                                matrice(j0,j0)+=val*porosite_eventuelle(j);

                              }
                          }
                    }
                }

            }
          else
            {
              for (num_face=num1; num_face<num2; num_face++)
                {
                  elem1 = face_voisins(num_face,0);
                  for (i=0; i<nb_faces_elem; i++)
                    {
                      if ( (j= elem_faces(elem1,i)) > num_face )
                        {
                          val = viscA(num_face,j,elem1,nu(elem1));
                          if (val<0.) val=0.;
                          for (int nc=0; nc<nb_comp; nc++)
                            {
                              int n0=num_face*nb_comp+nc;
                              int j0=j*nb_comp+nc;

                              matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                              matrice(n0,j0)-=val*porosite_eventuelle(j);
                              matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                              matrice(j0,j0)+=val*porosite_eventuelle(j);

                            }
                        }
                    }
                }
            }
        }
      int num_premiere_face = zone_VEF.premiere_face_int();
      for (num_face=num_premiere_face; num_face<n1; num_face++)
        {
          elem1 = face_voisins(num_face,0);
          elem2 = face_voisins(num_face,1);

          for (i=0; i<nb_faces_elem; i++)
            {
              if ( (j=elem_faces(elem1,i)) > num_face )
                {
                  val = viscA(num_face,j,elem1,nu(elem1));
                  if (val<0.) val=0.;
                  for (int nc=0; nc<nb_comp; nc++)
                    {
                      int n0=num_face*nb_comp+nc;
                      int j0=j*nb_comp+nc;

                      matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                      matrice(n0,j0)-=val*porosite_eventuelle(j);
                      matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                      matrice(j0,j0)+=val*porosite_eventuelle(j);

                    }
                }
              if (elem2!=-1)
                if ( (j=elem_faces(elem2,i)) > num_face )
                  {
                    val= viscA(num_face,j,elem2,nu(elem2));
                    if (val<0.) val=0.;
                    for (int nc=0; nc<nb_comp; nc++)
                      {
                        int n0=num_face*nb_comp+nc;
                        int j0=j*nb_comp+nc;

                        matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                        matrice(n0,j0)-=val*porosite_eventuelle(j);
                        matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                        matrice(j0,j0)+=val*porosite_eventuelle(j);

                      }
                  }
            }
        }
      modifier_matrice_pour_periodique_apres_contribuer(matrice,equation());
    }
}
void Op_Diff_VEF_Face_Stab::ajouter_contribution_multi_scalaire(const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  if (!new_jacobian_)
    Op_Diff_VEF_Face::ajouter_contribution_multi_scalaire(transporte,matrice);
  else
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

      int n1 = zone_VEF.nb_faces();
      int nb_comp = 1;
      int nb_dim = transporte.nb_dim();

      DoubleTab nu;
      int marq=phi_psi_diffuse(equation());
      const DoubleVect& porosite_elem = zone_VEF.porosite_elem();

      // soit on a div(phi nu grad inco)
      // soit on a div(nu grad phi inco)
      // cela depend si on diffuse phi_psi ou psi
      modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);
      DoubleVect porosite_eventuelle(zone_VEF.porosite_face());
      if (!marq)
        porosite_eventuelle=1;


      if(nb_dim==2)
        nb_comp=transporte.dimension(1);

      int i,j,num_face;
      int elem1,elem2;
      int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
      double val;

      int nb_bords=zone_VEF.nb_front_Cl();
      for (int n_bord=0; n_bord<nb_bords; n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          if (sub_type(Periodique,la_cl.valeur()))
            {
              const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
              int fac_asso;
              // on ne parcourt que la moitie des faces periodiques
              // on copiera a la fin le resultat dans la face associe..
              int num2b=num1+le_bord.nb_faces()/2;
              for (num_face=num1; num_face<num2b; num_face++)
                {
                  elem1 = face_voisins(num_face,0);
                  elem2 = face_voisins(num_face,1);
                  fac_asso = la_cl_perio.face_associee(num_face-num1)+num1;
                  for (i=0; i<nb_faces_elem; i++)
                    {
                      if ( (j=elem_faces(elem1,i)) > num_face )
                        {
                          for (int nc=0; nc<nb_comp; nc++)
                            {
                              val = viscA(num_face,j,elem1,nu(elem1,nc));
                              if (val<0.) val=0.;

                              int n0=num_face*nb_comp+nc;
                              int j0=j*nb_comp+nc;

                              matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                              matrice(n0,j0)-=val*porosite_eventuelle(j);
                              matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                              matrice(j0,j0)+=val*porosite_eventuelle(j);

                            }
                        }
                      if (elem2!=-1)
                        if ( (j=elem_faces(elem2,i)) > num_face )
                          {
                            for (int nc=0; nc<nb_comp; nc++)
                              {
                                val = viscA(num_face,j,elem2,nu(elem1,nc));
                                if (val<0.) val=0.;
                                int n0=num_face*nb_comp+nc;
                                int j0=j*nb_comp+nc;
                                int  n0perio=fac_asso*nb_comp+nc;
                                matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                                matrice(n0,j0)-=val*porosite_eventuelle(j);
                                matrice(j0,n0perio)-=val*porosite_eventuelle(num_face);
                                matrice(j0,j0)+=val*porosite_eventuelle(j);

                              }
                          }
                    }
                }

            }
          else
            {
              for (num_face=num1; num_face<num2; num_face++)
                {
                  elem1 = face_voisins(num_face,0);
                  for (i=0; i<nb_faces_elem; i++)
                    {
                      if ( (j= elem_faces(elem1,i)) > num_face )
                        {
                          for (int nc=0; nc<nb_comp; nc++)
                            {
                              val = viscA(num_face,j,elem1,nu(elem1,nc));
                              if (val<0.) val=0.;
                              int n0=num_face*nb_comp+nc;
                              int j0=j*nb_comp+nc;

                              matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                              matrice(n0,j0)-=val*porosite_eventuelle(j);
                              matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                              matrice(j0,j0)+=val*porosite_eventuelle(j);

                            }
                        }
                    }
                }
            }
        }
      for (num_face=zone_VEF.premiere_face_int(); num_face<n1; num_face++)
        {
          elem1 = face_voisins(num_face,0);
          elem2 = face_voisins(num_face,1);

          for (i=0; i<nb_faces_elem; i++)
            {
              if ( (j=elem_faces(elem1,i)) > num_face )
                {
                  for (int nc=0; nc<nb_comp; nc++)
                    {
                      val = viscA(num_face,j,elem1,nu(elem1,nc));
                      if (val<0.) val=0.;
                      int n0=num_face*nb_comp+nc;
                      int j0=j*nb_comp+nc;

                      matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                      matrice(n0,j0)-=val*porosite_eventuelle(j);
                      matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                      matrice(j0,j0)+=val*porosite_eventuelle(j);

                    }
                }
              if (elem2!=-1)
                if ( (j=elem_faces(elem2,i)) > num_face )
                  {
                    for (int nc=0; nc<nb_comp; nc++)
                      {
                        val= viscA(num_face,j,elem2,nu(elem2,nc));
                        if (val<0.) val=0.;
                        int n0=num_face*nb_comp+nc;
                        int j0=j*nb_comp+nc;

                        matrice(n0,n0)+=val*porosite_eventuelle(num_face);
                        matrice(n0,j0)-=val*porosite_eventuelle(j);
                        matrice(j0,n0)-=val*porosite_eventuelle(num_face);
                        matrice(j0,j0)+=val*porosite_eventuelle(j);

                      }
                  }
            }
        }
      modifier_matrice_pour_periodique_apres_contribuer(matrice,equation());
    }
}
