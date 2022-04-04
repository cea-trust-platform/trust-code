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
// File:        Zone_Cl_EF.cpp
// Directory:   $TRUST_ROOT/src/EF/Zones
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Zone_Cl_EF.h>
#include <Zone_EF.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Neumann.h>
#include <Neumann_homogene.h>
#include <Periodique.h>
//#include <Champ_P0_EF.h>
#include <Champ_P1_EF.h>
#include <Domaine.h>
#include <Tri_EF.h>
#include <Tetra_EF.h>
#include <Quadri_EF.h>
#include <Equation_base.h>
#include <Champ_front_txyz.h>
#include <Champ_front_softanalytique.h>
#include <Static_Int_Lists.h>
#include <Probleme_base.h>
#include <Discretisation_base.h>
#include <Matrice_Morse.h>
#include <Dirichlet_paroi_fixe_iso_Genepi2.h>

Implemente_instanciable(Zone_Cl_EF,"Zone_Cl_EF",Zone_Cl_dis_base);
int Deux_Puissance(int n) ;
int trois_puissance(int n) ;
/*
int Deux_Puissance(int n) {
  switch(n) {
  case 0 :return 1;
  case 1 :return 2;
  case 2 :return 4;
  case 3 :return 8;
  default: return 8*Deux_Puissance((int)(n-3));
  }
}
int trois_puissance(int n)
{
  switch(n)
    {
    case 0:return 1;
    case 1:return 3;
    case 2:return 9;
    case 3:return 27;
    default : return 27*trois_puissance(n-3);
    }
}
*/
//// printOn
//

Sortie& Zone_Cl_EF::printOn(Sortie& os ) const
{
  return os;
}

//// readOn
//

Entree& Zone_Cl_EF::readOn(Entree& is )
{
  return Zone_Cl_dis_base::readOn(is) ;
}


/////////////////////////////////////////////////////////////////////
//
// Implementation des fonctions de la classe Zone_Cl_EF
//
/////////////////////////////////////////////////////////////////////

// Description:
// etape de discretisation : dimensionnement des tableaux
void Zone_Cl_EF::associer(const Zone_EF& la_zone_EF)
{
  //  int nb_elem_Cl  = la_zone_EF.nb_elem_Cl();
}

// Description:
// remplissage des tableaux
void Zone_Cl_EF::completer(const Zone_dis& une_zone_dis)
{
  modif_perio_fait_ =0;
  if (sub_type(Zone_EF,une_zone_dis.valeur()))
    {
      const Zone_EF& la_zone_EF = ref_cast(Zone_EF, une_zone_dis.valeur());
      remplir_type_elem_Cl(la_zone_EF);
    }
  else
    {
      Cerr << "Zone_Cl_EF::completer() prend comme argument une Zone_EF " << finl;
      exit();
    }
}
void construit_connectivite_sommet(int type_cl,Static_Int_Lists& som_face_bord,const Conds_lim& les_conditions_limites_,const Zone_EF& zone_EF)
{
  if (type_cl!=1) Process::exit();
  int compt=0;
  int nb_faces_tot=zone_EF.nb_faces_tot();
  ArrOfInt face_bords(nb_faces_tot);

  for(int i=0; i<les_conditions_limites_.size(); i++)
    {
      const Cond_lim_base& la_cl=les_conditions_limites_[i].valeur();
      const Front_VF& le_bord=  zone_EF.front_VF(i);
      int num2 =  le_bord.nb_faces_tot();

      if ( (sub_type(Symetrie,la_cl)))
        {
          for (int ind_face=0; ind_face<num2; ind_face++)
            {
              int face=le_bord.num_face(ind_face);
              face_bords[compt++]=face;
            }
        }
    }
  face_bords.resize_array(compt);
  ArrOfInt is_sommet_sur_bord(zone_EF.nb_som_tot());
  const IntTab& face_sommets=zone_EF.face_sommets();
  int nb_som_face=face_sommets.dimension(1);
  for (int fac=0; fac<compt; fac++)
    {
      int face=face_bords[fac];
      for (int som=0; som<nb_som_face; som++)
        {
          int sommet=face_sommets(face,som);
          is_sommet_sur_bord[sommet]++;
        }
    }

  //On exploite is_sommet_sur_bord pour fixer la taille des List que va repertorier som_face_bord
  //-som_face_bord contient nb_som_tot List
  //-chaque List est dimensionnee par le nombre de faces de bord (portant une CL de Dirichlet)
  // connectees au sommet considere
  //-une List donnee pour un sommet repertorie le numero des faces qui lui sont connectees

  som_face_bord.set_list_sizes(is_sommet_sur_bord);
  is_sommet_sur_bord=0;
  for (int fac=0; fac<compt; fac++)
    {
      int face=face_bords[fac];
      for (int som=0; som<nb_som_face; som++)
        {
          int sommet=face_sommets(face,som);
          int n=(is_sommet_sur_bord[sommet])++;
          som_face_bord.set_value(sommet,n,face);

        }
    }

}
// Description:
// appele par remplir_volumes_entrelaces_Cl() : remplissage de type_elem_Cl_
void Zone_Cl_EF::remplir_type_elem_Cl(const Zone_EF& la_zone_EF)
{
  const Zone& z = la_zone_EF.zone();

  const IntTab& faces_sommets=la_zone_EF.face_sommets();
  int nb_som_face=faces_sommets.dimension(1);
  int nb_som_tot=z.nb_som_tot();
  type_sommet_.resize_array(z.nb_som_tot());
  type_sommet_=-1;
  IntTab titi(nb_som_tot);

  for(int i=0; i<les_conditions_limites_.size(); i++)
    {
      Cond_lim_base& la_cl=les_conditions_limites_[i].valeur();
      const Front_VF& le_bord= la_zone_EF.front_VF(i);
      int num2 =  le_bord.nb_faces_tot();

      if ( (sub_type(Dirichlet,la_cl))|| (sub_type(Dirichlet_homogene,la_cl)) )
        {
          for (int ind_face=0; ind_face<num2; ind_face++)
            {
              int face=le_bord.num_face(ind_face);
              for (int s=0; s<nb_som_face; s++)
                {
                  int som=faces_sommets(face,s);
//	Si on a Dirichlet_paroi_fixe_iso_Genepi2 on ne prend pas en compte le 0 pour la moyenne
                  if (!sub_type(Dirichlet_paroi_fixe_iso_Genepi2,la_cl))
                    titi(som)++;
                  if ((type_sommet_[som]!=1)&& (type_sommet_[som]!=3))
                    type_sommet_[som]=2;
                  else
                    type_sommet_[som]=3;
                }
            }
        }
      else if ( (sub_type(Symetrie,la_cl)))
        {
          for (int ind_face=0; ind_face<num2; ind_face++)
            {
              int face=le_bord.num_face(ind_face);
              for (int s=0; s<nb_som_face; s++)
                {
                  int som=faces_sommets(face,s);
                  if (type_sommet_[som]<=1)
                    type_sommet_[som]=1;
                  else
                    type_sommet_[som]=3;
                }
            }
        }
      else  if ( (sub_type(Neumann,la_cl))|| (sub_type(Neumann_homogene,la_cl)) )
        {
          for (int ind_face=0; ind_face<num2; ind_face++)
            {
              int face=le_bord.num_face(ind_face);
              for (int s=0; s<nb_som_face; s++)
                {
                  int som=faces_sommets(face,s);
                  if (type_sommet_[som]<0)
                    type_sommet_[som]=0;
                }
            }
        }
      else
        {
          Cerr<<__FILE__<<":" <<(int)__LINE__<<" non code pour cette cl "<<la_cl.que_suis_je()<<finl;
        }

    }
  // si on a un dirichet on stocke 2*nb participant + 1 si sommet aussi de sym
  for (int som=0; som<nb_som_tot; som++)
    {
      if (type_sommet_[som]>1)
        {
          //assert(titi(som)>0);
          type_sommet_[som]+=2*(titi(som));
        }
    }
  // On cree la connectivite sommet -> face de bord symetrie
  if (equation().inconnue().valeur().nature_du_champ()==vectoriel)
    {
      equation().probleme().discretisation().discretiser_champ("VITESSE",la_zone_EF,"normales_nodales","1",dimension,0.,normales_symetrie_);
      equation().probleme().discretisation().discretiser_champ("CHAMP_SOMMETS",la_zone_EF,"normales_nodales_bis","1",dimension,0., normales_symetrie_bis_);
      Static_Int_Lists sommet_face_symetrie;
      int type_cl=1;
      construit_connectivite_sommet(type_cl,sommet_face_symetrie,les_conditions_limites_,la_zone_EF);
      // sommet_face_symetrie contient le nombre de face de symetrie associe a chaque sommet
      const DoubleTab& face_normales = la_zone_EF.face_normales();
      ArrOfDouble n(dimension),t1(dimension),t2(dimension);
      for (int som=0; som<nb_som_tot; som++)
        {
          int nbf= sommet_face_symetrie.get_list_size(som);
          if (nbf>0)
            //if ( type_sommet_(som)==1)
            {
              n=0;
              t1=0;
              t2=0;
              // on determine la normale sommet
              //int nbf= sommet_face_symetrie.get_list_size(som);
              for (int f=0; f<nbf; f++)
                {
                  int face=sommet_face_symetrie(som,f);
                  for (int d=0; d<dimension; d++)
                    n[d]+=face_normales(face,d);
                }
              n/=nbf;

              double norm_n=norme_array(n);
              n/=norm_n;
              for (int d=0; d<dimension; d++)
                normales_symetrie_.valeur().valeurs()(som,d)=n[d];
              //	    Cerr<<som<<" on doit annuler une premiere direction "<<n(0) << " " <<n(1)<<" "<<n(dimension==3?2:1)<<finl;

              for (int f=0; f<nbf; f++)
                {
                  int face=sommet_face_symetrie(som,f);
                  double prod=0;
                  for (int d=0; d<dimension; d++)
                    prod+=face_normales(face,d)*n[d];

                  double v=0,s=0;

                  for (int d=0; d<dimension; d++)
                    {
                      t1[d]=face_normales(face,d)-n[d]*prod;
                      s+=face_normales(face,d)*face_normales(face,d);
                      v+=t1[d]*n[d];
                    }

                  //Cerr<<" vv"<< v<<" "<<norme_array(t1)<<" "<<norm_n<<finl;
                  if (norme_array(t1)>(1e-4*sqrt(s)))
                    {

                      // facilite le debugage
                      if (std::fabs(min_array(t1))>max_array(t1))
                        t1*=-1;
                      t1/=norme_array(t1);


                      //	    Cerr<<som<<" on doit annuler une deuxieme direction "<<t1(0) << " " <<t1(1)<<" "<<t1(dimension==3?2:1)<<finl;
                      f=nbf;
                      for (int d=0; d<dimension; d++)
                        normales_symetrie_bis_.valeur().valeurs()(som,d)=t1[d];
                      //assert(v==0);
                    }

                }
              for (int f=0; f<nbf; f++)
                {
                  int face=sommet_face_symetrie(som,f);
                  double prod=0,prod1=0,s=0;
                  for (int d=0; d<dimension; d++)
                    {
                      prod+=face_normales(face,d)*n[d];

                      prod1+=face_normales(face,d)*t1[d];
                      s+=face_normales(face,d)*face_normales(face,d);

                    }
                  for (int d=0; d<dimension; d++)
                    t2[d]=face_normales(face,d)-n[d]*prod-t1[d]*prod1;

                  if (norme_array(t2)>(1e-4*sqrt(s)))
                    {
                      // facilite le debugage
                      if (std::fabs(min_array(t2))>max_array(t2))
                        t2*=-1;
                      t2/=norme_array(t2);
                      Cerr<<face<<" "<<nbf<<" sommet "<<som<<" "<<norme_array(t2)/s<<" on doit annuler une troiseme direction"<<t2[0] << " " <<t2[1]<<" "<<t2[2]<<finl;
                      Cerr<<som<<" "<<t1[0] << " " <<t1[1]<<" "<<t1[2]<<finl;
                      Cerr<<som<<" "<<n[0] << " " <<n[1]<<" "<<n[2]<<finl;
                      f=nbf;
                      if (!normales_symetrie_ter_.non_nul())
                        equation().probleme().discretisation().discretiser_champ("CHAMP_SOMMETS",la_zone_EF,"normales_nodales_bis","1",dimension,0., normales_symetrie_ter_);
                      for (int d=0; d<dimension; d++)
                        normales_symetrie_ter_.valeur().valeurs()(som,d)=t2[d];
                      //exit();
                    }
                }
            }
        }
      normales_symetrie_.valeur().valeurs().echange_espace_virtuel();
      normales_symetrie_bis_.valeur().valeurs().echange_espace_virtuel();
      //exit();
    }
}
// Description:
// Impose les conditions de symetrie
// c.a.d annules les composantes  du champ sur la ou les normales
// si tous_les_sommets_sym =1 alors meme les sommets appartenant aussi a un dirichlet sont mis a zero.
void Zone_Cl_EF::imposer_symetrie(DoubleTab& values,int tous_les_sommets_sym) const
{
  // return;

  assert(values.dimension(1)==dimension);
  const Zone& z = zone_dis().zone();
  int nb_som_tot=z.nb_som_tot();
  assert(values.dimension_tot(0)==nb_som_tot);

  const DoubleTab& n =normales_symetrie_.valeur().valeurs();
  const DoubleTab& n_bis =normales_symetrie_bis_.valeur().valeurs();
  int dirmax=2;
  if (normales_symetrie_ter_.non_nul()) dirmax=3;
  for (int som=0; som<nb_som_tot; som++)
    if (( type_sommet_[som]==1)|| ( tous_les_sommets_sym&&(type_sommet_[som]%2==1)))
      {
        for (int dir=0; dir<dirmax; dir++)
          {
            const DoubleTab& nn=(dir==0?n:(dir==1?n_bis:normales_symetrie_ter_.valeur().valeurs()));
            double prod=0;
            for (int d=0; d<dimension; d++)
              prod+=values(som,d)*nn(som,d);
            for (int d=0; d<dimension; d++)
              values(som,d)-=prod*nn(som,d);
          }
      }
}

void Zone_Cl_EF::imposer_symetrie_partiellement(DoubleTab& values,const Noms& a_exclure) const
{
  const IntTab& faces_sommets=zone_EF().face_sommets();
  int nb_som_face=faces_sommets.dimension(1);


  const DoubleTab& n =normales_symetrie_.valeur().valeurs();
  const DoubleTab& n_bis =normales_symetrie_bis_.valeur().valeurs();
  int dirmax=2;
  if (normales_symetrie_ter_.non_nul()) dirmax=3;
  int nbcond=nb_cond_lim();

  ArrOfInt type_sommet_bis(type_sommet_);
  for (int n_bord=0; n_bord<nbcond; n_bord++)
    {

      const Cond_lim_base& la_cl = les_conditions_limites(n_bord).valeur();
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num2 =  le_bord.nb_faces_tot();
      const Nom& nom_bord=le_bord.le_nom();

      if (sub_type(Symetrie,la_cl)&&(a_exclure.rang(nom_bord)>-1))
        {
          Cerr<<__FILE__<<(int)__LINE__<<" on  impose pas symetrie sur "<<nom_bord<<finl;
          for (int ind_face=0; ind_face<num2; ind_face++)
            {
              int face=le_bord.num_face(ind_face);
              for (int s=0; s<nb_som_face; s++)
                {
                  int som=faces_sommets(face,s);
                  type_sommet_bis[som]=3;
                }
            }
        }
    }

  for (int n_bord=0; n_bord<nbcond; n_bord++)
    {

      const Cond_lim_base& la_cl = les_conditions_limites(n_bord).valeur();
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num2 =  le_bord.nb_faces_tot();
      const Nom& nom_bord=le_bord.le_nom();

      if (sub_type(Symetrie,la_cl)&&(a_exclure.rang(nom_bord)<0))
        {
          Cerr<<__FILE__<<(int)__LINE__<<" on impose symetrie sur "<<nom_bord<<finl;
          for (int ind_face=0; ind_face<num2; ind_face++)
            {
              int face=le_bord.num_face(ind_face);
              for (int s=0; s<nb_som_face; s++)
                {
                  int som=faces_sommets(face,s);
                  if ( type_sommet_bis[som]==1)  //|| ( tous_les_sommets_sym&&(type_sommet_(som)%2==1)))
                    {
                      for (int dir=0; dir<dirmax; dir++)
                        {
                          const DoubleTab& nn=(dir==0?n:(dir==1?n_bis:normales_symetrie_ter_.valeur().valeurs()));
                          double prod=0;
                          for (int d=0; d<dimension; d++)
                            prod+=values(som,d)*nn(som,d);
                          for (int d=0; d<dimension; d++)
                            values(som,d)-=prod*nn(som,d);
                        }
                    }
                }
            }
        }
    }
}
void Zone_Cl_EF::modifie_gradient(ArrOfDouble& grad_mod, const ArrOfDouble& grad, int som) const
{
  if (type_sommet_[som]!=1) return;
  assert(grad_mod.size_array()==dimension);


  const DoubleTab& n =normales_symetrie_.valeur().valeurs();
  const DoubleTab& n_bis =normales_symetrie_bis_.valeur().valeurs();

  assert ( type_sommet_(som)>=1);
  int dirmax=2;
  if (normales_symetrie_ter_.non_nul()) dirmax=3;
  for (int dir=0; dir<dirmax; dir++)
    {
      const DoubleTab& nn=(dir==0?n:(dir==1?n_bis:normales_symetrie_ter_.valeur().valeurs()));
      double prod=0;
      for (int d=0; d<dimension; d++)
        prod+=grad[d]*nn(som,d);
      for (int d=0; d<dimension; d++)
        grad_mod[d]+=prod*nn(som,d);
    }

}


// Description :
// On transforme la_matrice et le secmem pour avoir un secmem normal aux bords , plus la matrice pour assurer que la solution sera correcte
void  Zone_Cl_EF::imposer_symetrie_matrice_secmem(Matrice_Morse& la_matrice, DoubleTab& secmem) const
{
  // return;
  assert(secmem.dimension(1)==dimension);
  const Zone& z = zone_dis().zone();
  int nb_som=z.nb_som();
  assert(secmem.dimension(0)==nb_som);
  int nb_comp=secmem.dimension(1);
  const DoubleTab& n =normales_symetrie_.valeur().valeurs();
  const DoubleTab& n_bis =normales_symetrie_bis_.valeur().valeurs();
  ArrOfDouble normale(dimension);

  const IntVect& tab1=la_matrice.get_tab1();
  const IntVect& tab2=la_matrice.get_tab2();

  const DoubleTab& champ_inconnue = equation().inconnue().valeurs();
  int dirmax=2;
  if (normales_symetrie_ter_.non_nul()) dirmax=3;
  for (int som=0; som<nb_som; som++)
    if ( type_sommet_[som]==1)
      {
        for (int dir=0; dir<dirmax; dir++)
          {
            const DoubleTab& nn=(dir==0?n:(dir==1?n_bis:normales_symetrie_ter_.valeur().valeurs()));
            for (int d=0; d<dimension; d++) normale[d]=nn(som,d);
            // On commence par recalculer secmem=secmem-A *present pour pouvoir modifier A (on en profite pour projeter)
            int nb_coeff_ligne=tab1[som*nb_comp+1] - tab1[som*nb_comp];
            for (int k=0; k<nb_coeff_ligne; k++)
              {
                for (int comp=0; comp<nb_comp; comp++)
                  {
                    int j=tab2[tab1[som*nb_comp+comp]-1+k]-1;
                    //assert(j!=(som*nb_comp+comp));
                    //if ((j>=(som*nb_comp))&&(j<(som*nb_comp+nb_comp)))

                    const    double coef_ij=la_matrice(som*nb_comp+comp,j);
                    int som2=j/nb_comp;
                    int comp2=j-som2*nb_comp;
                    secmem(som,comp)-=coef_ij*champ_inconnue(som2,comp2);
                  }
              }
            double somme_b=0;

            for (int comp=0; comp<nb_comp; comp++)
              somme_b+=secmem(som,comp)*normale[comp];
            //Cerr<<som<<" sommet " <<somme_b<<" "<<secmem(som,0)<<" "<<secmem(som,1)<<finl;
            // on retire secmem.n n
            for (int comp=0; comp<nb_comp; comp++)
              secmem(som,comp)-=somme_b*normale[comp];

            if (1)
              {
                // on doit remettre la meme diagonale partout on prend la moyenne
                double ref=0;
                for (int comp=0; comp<nb_comp; comp++)
                  {

                    int j0=som*nb_comp+comp;
                    ref+=la_matrice.coef(j0,j0);
                  }
                ref/=nb_comp;

                for (int comp=0; comp<nb_comp; comp++)
                  {
                    int j0=som*nb_comp+comp;
                    double rap=ref/la_matrice.coef(j0,j0);
                    //Cerr<<dir<<" "<<som <<" "<<comp<<" rpp "<<rap <<finl;
                    for (int k=0; k<nb_coeff_ligne; k++)
                      {

                        int j=tab2[tab1[j0]-1+k]-1;
                        la_matrice(j0,j)*=rap;
                      }
                    assert(est_egal(la_matrice(j0,j0),ref));
                  }
              }
            if (1)
              {
                // on annule tous les coef extra diagonaux du bloc
                // sur les lignes i pour lesquelles normale(d) !=0
                // c.a.d dasb(normale(d)>1e-5)
                for (int k=0; k<nb_coeff_ligne; k++)
                  {

                    for (int comp=0; comp<nb_comp; comp++)
                      if (std::fabs(normale[comp])>1e-5)
                        {
                          int j=tab2[tab1[som*nb_comp+comp]-1+k]-1;
                          if (j!=(som*nb_comp+comp))
                            if ((j>=(som*nb_comp))&&(j<(som*nb_comp+nb_comp)))
                              {
                                la_matrice(som*nb_comp+comp,j)=0;
                              }
                        }
                  }
              }
            {
              // pour les blocs extra diagonaux on assure que Aij.ni=0

              ArrOfDouble somme(nb_coeff_ligne);
              for (int k=0; k<nb_coeff_ligne; k++)
                {

                  int j=tab2[tab1[som*nb_comp]-1+k]-1;
                  for (int comp=0; comp<nb_comp; comp++)
                    somme[k]+=la_matrice(som*nb_comp+comp,j)*normale[comp];
                }
              // on retire somme ni
              for (int k=0; k<nb_coeff_ligne; k++)
                {

                  int j=tab2[tab1[som*nb_comp]-1+k]-1;
                  for (int comp=0; comp<nb_comp; comp++)
                    if ((j<(som*nb_comp))||(j>=(som*nb_comp+nb_comp)))
                      la_matrice(som*nb_comp+comp,j)-=(somme[k])*normale[comp];
                }
            }
            // Finalement on recalule secmem=secmem+A*champ_inconnue (A a ete beaucoup modiife)
            for (int k=0; k<nb_coeff_ligne; k++)
              {
                for (int comp=0; comp<nb_comp; comp++)
                  {
                    int j=tab2[tab1[som*nb_comp+comp]-1+k]-1;
                    int som2=j/nb_comp;
                    int comp2=j-som2*nb_comp;

                    const    double coef_ij=la_matrice(som*nb_comp+comp,j);
                    secmem(som,comp)+=coef_ij*champ_inconnue(som2,comp2);

                  }
              }
            {
              // verification
              double somme_b2=0;

              for (int comp=0; comp<nb_comp; comp++)
                somme_b2+=secmem(som,comp)*normale[comp];
              //Cerr<<" lllllllll "<<somme_b2<<" "<<tt<<finl;
              assert(std::fabs(somme_b2) <1e-8);
              // on retire secmem.n n
              for (int comp=0; comp<nb_comp; comp++)
                secmem(som,comp)-=somme_b2*normale[comp];

            }
          }
      }
  //  exit();
}

// Description:
// Impose les conditions aux limites a la valeur temporelle "temps" du
// Champ_Inc
void Zone_Cl_EF::imposer_cond_lim(Champ_Inc& ch, double temps)
{
  DoubleTab& ch_tab = ch->valeurs(temps);
  int nb_comp = ch->nb_comp();
  const Zone_EF& zoneEF =  zone_EF(); //ref_cast(Zone_EF,ch->equation().zone_dis().valeur());
  const IntTab& faces_sommets=zoneEF.face_sommets();
  int nb_som_face=faces_sommets.dimension(1);
  const DoubleTab& coords= zoneEF.zone().domaine().coord_sommets();

  // dans un premier temps on annule le champ sur les dirichlets
  // puis on ajoute 1/nb_cl*val_imp

  for (int n_bord=0; n_bord<nb_cond_lim(); n_bord++)
    {

      const Cond_lim_base& la_cl = les_conditions_limites(n_bord).valeur();
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num2 =  le_bord.nb_faces_tot();

      if (sub_type(Dirichlet,la_cl)||(sub_type(Dirichlet_homogene,la_cl)))
        {
          //const Dirichlet& la_cl_diri = ref_cast(Dirichlet,la_cl);

          for (int ind_face=0; ind_face<num2; ind_face++)
            {
              int face=le_bord.num_face(ind_face);
              for (int s=0; s<nb_som_face; s++)
                {
                  int som=faces_sommets(face,s);
                  assert(type_sommet_(som)>=2);


                  if (nb_comp == 1)
                    ch_tab[som]=0;
                  else
                    for (int ncomp=0; ncomp<nb_comp; ncomp++)
                      ch_tab(som,ncomp)=0;
                }
            }
        }
    }
  for (int n_bord=0; n_bord<nb_cond_lim(); n_bord++)
    {

      const Cond_lim_base& la_cl = les_conditions_limites(n_bord).valeur();
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num2 =  le_bord.nb_faces_tot();

      if (sub_type(Dirichlet,la_cl))
        {
          const Dirichlet& la_cl_diri = ref_cast(Dirichlet,la_cl);
          if (sub_type(Champ_front_softanalytique,la_cl_diri.champ_front().valeur()))
            {
              Cerr<<" Il faut utiliser Champ_front_fonc_txyz et non "<<la_cl_diri.champ_front().valeur().que_suis_je()<<finl;
              exit();
            }
          int avec_valeur_aux_sommets=0;
          if (sub_type(Champ_front_var_instationnaire,la_cl_diri.champ_front().valeur()))
            {
              const Champ_front_var_instationnaire& ch_txyz=ref_cast(Champ_front_var_instationnaire,la_cl_diri.champ_front().valeur());
              avec_valeur_aux_sommets=ch_txyz.valeur_au_temps_et_au_point_disponible();
            }
          // Pour les faces de Dirichlet on impose l'inconnue au sommet
          if (avec_valeur_aux_sommets)
            {
              const Champ_front_var_instationnaire& ch_txyz=ref_cast(Champ_front_var_instationnaire,la_cl_diri.champ_front().valeur());
              for (int ind_face=0; ind_face<num2; ind_face++)
                {
                  int face=le_bord.num_face(ind_face);
                  for (int s=0; s<nb_som_face; s++)
                    {
                      int som=faces_sommets(face,s);
                      assert(type_sommet_(som)>=4);
                      double coef=1./(type_sommet_[som]/2-1);
                      //Cerr<<"iciPB "<<coef<<finl;
                      double x,y,z=0;
                      x=coords(som,0);
                      y=coords(som,1);
                      if (dimension==3)
                        z=coords(som,2);
                      if (nb_comp == 1)
                        ch_tab[som]+=coef*ch_txyz.valeur_au_temps_et_au_point(temps,som,x,y,z,0);
                      else
                        for (int ncomp=0; ncomp<nb_comp; ncomp++)
                          ch_tab(som,ncomp)+=coef*ch_txyz.valeur_au_temps_et_au_point(temps,som,x,y,z,ncomp);
                    }
                }
            }
          else
            for (int ind_face=0; ind_face<num2; ind_face++)
              {
                int face=le_bord.num_face(ind_face);
                for (int s=0; s<nb_som_face; s++)
                  {
                    int som=faces_sommets(face,s);
                    assert(type_sommet_(som)>=4);
                    double coef=1./(type_sommet_[som]/2-1);
                    //Cerr<<"iciPB "<<coef<<finl;
                    if (nb_comp == 1)
                      ch_tab[som]+=coef*la_cl_diri.val_imp_au_temps(temps,ind_face);
                    else
                      for (int ncomp=0; ncomp<nb_comp; ncomp++)
                        ch_tab(som,ncomp)+=coef*la_cl_diri.val_imp_au_temps(temps,ind_face,ncomp);
                  }
              }

        }
      /*
      else if (sub_type(Dirichlet_homogene,la_cl))
      {
        // Pour les faces de Dirichlet on impose l'inconnue au sommet
        for (int ind_face=0; ind_face<num2; ind_face++)
      {
      int face=le_bord.num_face(ind_face);
      for (int s=0;s<nb_som_face;s++)
        {
          int som=faces_sommets(face,s);
          assert(type_sommet_(som)>=2);
          if (nb_comp == 1)
      ch_tab[som] = 0;
          else
      for (int ncomp=0; ncomp<nb_comp; ncomp++)
        ch_tab(som,ncomp) =0;
        }
      }
      }
      */
      // provsoire a fair equ'une fois
      else if ( (sub_type(Symetrie,la_cl) ) &&
                (ch->nature_du_champ()==vectoriel) )
        {
          imposer_symetrie(ch_tab);
        }
    }



}

int Zone_Cl_EF::nb_faces_sortie_libre() const
{
  exit();
  /*
  int compteur=0;
  for(int cl=0; cl<les_conditions_limites_.size(); cl++)
    {
      if(sub_type(Neumann_sortie_libre, les_conditions_limites_[cl].valeur()))
  {
    const Front_VF& le_bord=ref_cast(Front_VF,les_conditions_limites_[cl]->frontiere_dis());
    compteur+=le_bord.nb_faces();
  }
    }
  return compteur;
  */
  return -1;
}

int Zone_Cl_EF::nb_bord_periodicite() const
{
  int compteur=0;
  for(int cl=0; cl<les_conditions_limites_.size(); cl++)
    {
      if (sub_type(Periodique,les_conditions_limites_[cl].valeur()))
        compteur++;
    }
  return compteur;
}


int Zone_Cl_EF::initialiser(double temps)
{
  Zone_Cl_dis_base::initialiser(temps);

  if (nb_bord_periodicite()>0)
    {
      Cerr<<" La periodicite n'est pas code !!!"<<finl;
      abort();
    }
  return 1;
}

Zone_EF& Zone_Cl_EF::zone_EF()
{
  return ref_cast(Zone_EF, zone_dis().valeur());
}

const Zone_EF& Zone_Cl_EF::zone_EF() const
{
  return ref_cast(Zone_EF, zone_dis().valeur());
}
