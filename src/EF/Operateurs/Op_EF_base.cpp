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
// File:        Op_EF_base.cpp
// Directory:   $TRUST_ROOT/src/EF/Operateurs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_EF_base.h>
#include <Zone_EF.h>
#include <Zone_Cl_EF.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <Matrice_Morse.h>
#include <Equation_base.h>
#include <Champ_Inc.h>
#include <Sortie.h>
#include <Operateur.h>
#include <Probleme_base.h>
#include <Champ_Don.h>
#include <Champ_Uniforme.h>
#include <Schema_Temps_base.h>
#include <Milieu_base.h>
#include <Operateur_base.h>
#include <Operateur_Diff_base.h>
#include <EcrFicPartage.h>
#include <Array_tools.h>
#include <Echange_interne_global_impose.h>
#include <Echange_interne_global_parfait.h>
#include <Champ_front_calc_interne.h>
#include <map>
#include <vector>

// Description
// Dimensionnement de la matrice qui devra recevoir les coefficients provenant de
// la convection, de la diffusion pour le cas des faces.
// Cette matrice a une structure de matrice morse.
// Nous commencons par calculer les tailles des tableaux tab1 et tab2.

void Op_EF_base::dimensionner(const Zone_EF& la_zone,
                              const Zone_Cl_EF& la_zone_cl,
                              Matrice_Morse& la_matrice) const
{

  int matrice_stocke=1;
  if (matrice_stocke)
    {
      //if (matrice_sto_.nb_colonnes()) PL: non faux en //, nb_colonnes() peut etre nul localement
      if (Process::mp_sum(matrice_sto_.nb_colonnes()))
        {
          la_matrice=matrice_sto_;
          return;
        }

    }
  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de
  // la convection, de la diffusion pour le cas des faces.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.
  // Pour ce faire il faut chercher les sommets voisins du sommet considere.

  int nb_som=la_zone.nb_som();
  int nfin = la_zone.nb_som_tot();
  // essai
  nb_som=nfin;

  const DoubleTab& champ_inconnue = la_zone_cl.equation().inconnue().valeurs();
  int nb_comp = champ_inconnue.line_size();

  // on ne s'occupe pas dans un premier temps des composantes

  const IntTab& elems=la_zone.zone().les_elems();
  //int nb_elem=la_zone.zone().nb_elem();
  int nb_elem_tot=la_zone.zone().nb_elem_tot();
  int nb_som_elem=la_zone.zone().nb_som_elem();
  int nb_som_face=la_zone.nb_som_face();

  // Computation of the number of non void coeffs:
  int extra_nb_coeff = 0;
  // Specific BC that will require extra terms in the matrix:
  for(int i=0; i<la_zone_cl.nb_cond_lim(); i++)
    {
      const Cond_lim_base& ccl = la_zone_cl.les_conditions_limites(i).valeur();
      if(sub_type(Echange_interne_global_impose, ccl))
        {
          if (nb_som_face != 1)
            {
              Cerr << "Boundary condition 'Echange_interne_global_impose' or 'Echange_interne_parfait' is not validated for EF and mesh_dim > 1 (i.e. sth other than a wire mesh)!!" << finl;
              Process::exit(-1);
            }
          const Echange_interne_global_impose& cl = ref_cast(Echange_interne_global_impose, ccl);
          const Champ_front_calc_interne& ch = ref_cast(Champ_front_calc_interne, cl.T_ext().valeur());
          extra_nb_coeff += ch.face_map().size_array();
          if(sub_type(Echange_interne_global_parfait, ccl))
            extra_nb_coeff += ch.face_map().size_array() / 2;
        }
    }
  int nb_coeff=(int)(nb_elem_tot*(nb_som_elem*(nb_som_elem)))+extra_nb_coeff;
  la_matrice.dimensionner(nb_som*nb_comp,nfin*nb_comp,0);
  IntTab Indice(nb_coeff,2),p(1,2);
  int tot=0;
  for (int elem=0; elem<nb_elem_tot; elem++)
    {
      for (int i1=0; i1<nb_som_elem; i1++)
        {
          int glob=elems(elem,i1);
          for (int i2=0; i2<nb_som_elem; i2++)
            {
              int glob2=elems(elem,i2);
              if (glob<nb_som)
                {
                  Indice(tot,0)=glob;
                  Indice(tot,1)=glob2;
                  tot++;
                }
            }
        }
    }
  // Specific BC that will require extra terms in the matrix:
  for(int i=0; i<la_zone_cl.nb_cond_lim(); i++)
    {
      const Cond_lim_base& ccl = la_zone_cl.les_conditions_limites(i).valeur();
      if(sub_type(Echange_interne_global_impose, ccl))
        {
          const Echange_interne_global_impose& cl = ref_cast(Echange_interne_global_impose, ccl);
          bool is_parfait = sub_type(Echange_interne_global_parfait, ccl);
          const Champ_front_calc_interne& ch = ref_cast(Champ_front_calc_interne, cl.T_ext().valeur());
          const IntTab& mp = ch.face_map();   // indices of (local) *faces*
          const Front_VF& le_bord = ref_cast(Front_VF,cl.frontiere_dis());
          int nfacedeb = le_bord.num_premiere_face();
          int nfacefin = nfacedeb + le_bord.nb_faces();
          std::vector<bool> hit(le_bord.nb_faces());
          std::fill(hit.begin(), hit.end(), false);
          for(int face_recto=nfacedeb; face_recto < nfacefin; face_recto++)
            {
              int face_verso = mp(face_recto-nfacedeb)+nfacedeb;
              int face_plus_2 = 0;

              if (is_parfait)
                {
                  // element attached to the face
                  int elem1 = la_zone.face_voisins(face_verso, 0);
                  int elem = (elem1 != -1) ? elem1 : la_zone.face_voisins(face_verso, 1);
                  // other face of the element (i.e. face "after" opp_face)
                  int f1 = la_zone.elem_faces(elem, 0);
                  face_plus_2 = f1 != face_verso ? f1 : la_zone.elem_faces(elem, 1);  // 2 faces per elem max - 1D
                }

              // all vertices of recto face are put in correspondance with all vertices of verso face:
              for(int som_idx=0; som_idx < nb_som_face; som_idx++)
                {
                  int som_recto = la_zone.face_sommets(face_recto, som_idx);
                  for(int som_idx2=0; som_idx2 < nb_som_face; som_idx2++)
                    {
                      int som_verso = la_zone.face_sommets(face_verso, som_idx2);
                      // tri-diagonal structure extended through the wall
                      // actually this puts too many coeffs in case of is_parfait, but I can't be bothered ...
                      Indice(tot, 0) = som_recto;
                      Indice(tot, 1) = som_verso;
                      tot++;

                      if (is_parfait)   // more complicated: recto face takes (i-1), i and ("next face after opp_face")
                        // verso face will just serve to connect recto and verso
                        {
                          int som_plus_2 = la_zone.face_sommets(face_plus_2, som_idx2);
                          if(!hit[face_recto-nfacedeb])  // first inner face
                            {
                              Indice(tot, 0) = som_recto;
                              Indice(tot, 1) = som_plus_2;
                              tot++;
                            }
                          else                  // opposite inner face
                            {
                              // Nothing to do! no extra tri-band coeff.
                            }
                        }
                    }
                }
              hit[face_recto-nfacedeb] = true;
              hit[face_verso-nfacedeb] = true;
            }
        }
    }

  Indice.resize(tot,2);

  tableau_trier_retirer_doublons(Indice);
  if (nb_comp==1)
    la_matrice.dimensionner(Indice);
  else
    {
      int ntot=Indice.dimension(0);
      // on cree pour chaque coeff d'origine un bloc nb_comp*nb_comp
      IntTab Indice_v(ntot*nb_comp*nb_comp,2);
      int newcas=0;
      for (int cas=0; cas<ntot; cas++)
        {
          for (int i=0; i<nb_comp; i++)
            for (int j=0; j<nb_comp; j++)
              {
                int newi=Indice(cas,0)*nb_comp+i;
                int newj=Indice(cas,1)*nb_comp+j;
                Indice_v(newcas,0)=newi;
                Indice_v(newcas,1)=newj;
                newcas++;

              }
        }
      tableau_trier_retirer_doublons(Indice_v);
      la_matrice.dimensionner(Indice_v);
      //Cerr<<Indice<<" nnnnnnnnn "<<Indice_v<<finl;
      // abort();
    }

  assert(mp_min_vect(la_matrice.get_tab2())>-1);

  if (matrice_stocke)
    {
      ref_cast_non_const(Matrice_Morse,matrice_sto_)=la_matrice;
    }
  //
  if (0)
    {
      Cerr<<la_matrice.get_tab1()<<finl;
      Cerr<<la_matrice.get_tab2()<<finl;
      la_matrice.get_set_coeff() = 1;
      Cerr<<"ici "<<tot <<" "<< nb_coeff<<finl;
      Cerr<<"Indice"<<Indice<<finl;
      la_matrice.imprimer_formatte(Cerr);

      Process::exit();
    }

}

// Description
// Modification des coef de la matrice et du second membre pour les conditions
// de Dirichlet

void Op_EF_base::modifier_pour_Cl(const Zone_EF& la_zone,
                                  const Zone_Cl_EF& la_zone_cl,
                                  Matrice_Morse& la_matrice, DoubleTab& secmem) const
{
//  Cerr<<__PRETTY_FUNCTION__<<" ne fait rien "<<finl; return;
  Cerr<<__PRETTY_FUNCTION__<<" a revoir "<<finl;

  // Dimensionnement de la matrice qui devra recevoir les coefficients provenant de
  // la convection, de la diffusion pour le cas des faces.
  // Cette matrice a une structure de matrice morse.
  // Nous commencons par calculer les tailles des tableaux tab1 et tab2.

  int   k, nbvois;
  //  int nfin = la_zone.nb_faces();
  const Conds_lim& les_cl = la_zone_cl.les_conditions_limites();

  const IntVect& tab1=la_matrice.get_tab1();
  DoubleVect& coeff = la_matrice.get_set_coeff();

  // determination de la taille du champ inconnue.
  // Cerr << "dans modifier cl " << finl;
  const DoubleTab& champ_inconnue = la_zone_cl.equation().inconnue().valeurs();
  int nb_comp = champ_inconnue.line_size();

  const IntTab& faces_sommets=la_zone.face_sommets();
  int nb_som_face=faces_sommets.dimension(1);

  int nb_som=secmem.dimension(0);
  // Cerr << "nb_comp " << nb_comp << finl;
  for (int i=0; i<les_cl.size(); i++)
    {
      const Cond_lim& la_cl = les_cl[i];
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nfaces = le_bord.nb_faces_tot();
      if (sub_type(Dirichlet,la_cl.valeur()))
        {
          const Dirichlet& la_cl_Dirichlet = ref_cast(Dirichlet,la_cl.valeur());
          for (int ind_face=0; ind_face < nfaces; ind_face++)
            for (int s=0; s<nb_som_face; s++)
              {
                int face=le_bord.num_face(ind_face);
                int som=faces_sommets(face,s);
                if (som<nb_som)
                  for (int comp=0; comp<nb_comp; comp++)
                    {
                      int m=som*nb_comp+comp;
                      int idiag = tab1[som*nb_comp+comp]-1;
                      nbvois = tab1[som*nb_comp+1+comp] - tab1[som*nb_comp+comp];
                      for (k=0; k < nbvois; k++)
                        {
                          coeff[idiag+k]=0;
                        }
                      la_matrice.coef(m,m)=1;
                      assert(la_matrice.coef(som*nb_comp+comp,som*nb_comp+comp)==1);
                      // pour les voisins


                      // pour le second membre
                      secmem(som,comp)= la_cl_Dirichlet.val_imp(ind_face,comp);
                      secmem(som,comp)= champ_inconnue(som,comp);
                    }
              }
        }
      if (sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          const Dirichlet_homogene& la_cl_Dirichlet = ref_cast(Dirichlet_homogene,la_cl.valeur());
          for (int ind_face=0; ind_face < nfaces; ind_face++)
            for (int s=0; s<nb_som_face; s++)
              {
                int face=le_bord.num_face(ind_face);
                int som=faces_sommets(face,s);

                for (int comp=0; comp<nb_comp; comp++)
                  {
                    int m=som*nb_comp+comp;
                    int idiag = tab1[som*nb_comp+comp]-1;
                    nbvois = tab1[som*nb_comp+1+comp] - tab1[som*nb_comp+comp];
                    for (k=0; k < nbvois; k++)
                      {
                        coeff[idiag+k]=0;
                      }
                    la_matrice.coef(m,m)=1;
                    assert(la_matrice.coef(som*nb_comp+comp,som*nb_comp+comp)==1);
                    // pour les voisins


                    // pour le second membre
                    secmem(som,comp)= 0;
                    if (la_cl_Dirichlet.val_imp(ind_face,comp)!=0)
                      {
                        Cerr<<"impossible non ??? "<<finl;
                        Process::exit();
                      }
                  }
              }
        }

      if (sub_type(Symetrie,la_cl.valeur()))
        {
          // on ne fait rien ici on fait ensuite
        }
    }

  // On modifie pour la symetrie

  if (la_zone_cl.equation().inconnue().valeur().nature_du_champ()==vectoriel)
    {
      la_zone_cl.imposer_symetrie_matrice_secmem(la_matrice,secmem);
    }

  /*
    la_matrice.imprimer(Cerr);
    la_matrice.imprimer_formatte(Cerr);
    Cerr<<"scd "<<secmem<<finl;
    */
  //exit();
  // Prise en compte des conditions de type periodicite
  //  Cerr << "fin de modifier" << finl;
}

// Description multiplie le flux bordpar rho cp ou rho si necessaire
void Op_EF_base::modifier_flux( const Operateur_base& op) const
{
  controle_modifier_flux_=1;
  const Zone_EF& la_zone_EF=ref_cast(Zone_EF,op.equation().zone_dis().valeur());
  DoubleTab& flux_bords_=op.flux_bords();
  int nb_compo=flux_bords_.dimension(1);
  const Probleme_base& pb=op.equation().probleme();

  // On multiplie par rho si Navier Stokes incompressible
  Nom nom_eqn=op.equation().que_suis_je();
  if (nom_eqn.debute_par("Navier_Stokes") && pb.milieu().que_suis_je()=="Fluide_Incompressible")
    {
      const Champ_base& rho = op.equation().milieu().masse_volumique().valeur();
      if (sub_type(Champ_Uniforme,rho))
        {
          double coef = rho(0,0);
          int nb_faces_bord=la_zone_EF.nb_faces_bord();
          for (int face=0; face<nb_faces_bord; face++)
            for(int k=0; k<nb_compo; k++)
              flux_bords_(face,k) *= coef;
        }
    }

}

// Description
// Impression des flux d'un operateur EF aux faces (ie: diffusion, convection)
//
int Op_EF_base::impr(Sortie& os, const Operateur_base& op) const
{
  const Zone_EF& la_zone_EF=ref_cast(Zone_EF,op.equation().zone_dis().valeur());
  DoubleTab& flux_bords_=op.flux_bords();
  if (flux_bords_.nb_dim()!=2)
    {
      Cout << "L'impression des flux n'est pas codee pour l'operateur " << op.que_suis_je() << finl;
      return 1;
    }
  if (controle_modifier_flux_==0)
    {
      Cerr<<op.que_suis_je()<<" appelle  Op_EF_base::impr sans avoir appeler  Op_EF_base::modifier_flux, on arrete tout "<<finl;
      Process::exit();
    }

  int nb_compo=flux_bords_.dimension(1);
  const Probleme_base& pb=op.equation().probleme();
  const Schema_Temps_base& sch=pb.schema_temps();
  // On n'imprime les moments que si demande et si on traite l'operateur de diffusion de la vitesse
  int impr_mom=0;
  if (la_zone_EF.zone().Moments_a_imprimer() && sub_type(Operateur_Diff_base,op) && op.equation().inconnue().le_nom()=="vitesse")
    impr_mom=1;

  const int impr_sum=(la_zone_EF.zone().Bords_a_imprimer_sum().est_vide() ? 0:1);
  const int impr_bord=(la_zone_EF.zone().Bords_a_imprimer().est_vide() ? 0:1);
  int flag=0;
  if (Process::je_suis_maitre()) flag=1;
  //SFichier Flux;
  if (!Flux.is_open()) op.ouvrir_fichier( Flux,"",flag);
  //SFichier Flux_moment;
  if (!Flux_moment.is_open()) op.ouvrir_fichier(Flux_moment,"moment",impr_mom&&flag);
  //SFichier Flux_sum;
  if (!Flux_sum.is_open()) op.ouvrir_fichier(Flux_sum,"sum",impr_sum&&flag);
  EcrFicPartage Flux_face;
  op.ouvrir_fichier_partage(Flux_face,"",impr_bord);


  // Impression du temps
  double temps=sch.temps_courant();
  if(Process::je_suis_maitre())
    {
      Flux.add_col(temps);
      if (impr_mom) Flux_moment.add_col(temps);
      if (impr_sum) Flux_sum.add_col(temps);
    }

  // Calcul des moments
  const int nb_faces =  la_zone_EF.nb_faces_tot();
  DoubleTab xgr(nb_faces,Objet_U::dimension);
  xgr=0.;
  DoubleVect moment(nb_compo);
  moment=0;
  if (impr_mom)
    {
      const DoubleTab& xgrav = la_zone_EF.xv();
      const ArrOfDouble& c_grav=la_zone_EF.zone().cg_moments();
      for (int num_face=0; num_face <nb_faces; num_face++)
        for (int i=0; i<Objet_U::dimension; i++)
          xgr(num_face,i)=xgrav(num_face,i)-c_grav(i);
    }

  // On parcours les frontieres pour sommer les flux par frontiere dans le tableau flux_bord
  DoubleVect flux_bord(nb_compo);
  DoubleVect bilan(nb_compo);
  bilan = 0;
  for (int num_cl=0; num_cl<la_zone_EF.nb_front_Cl(); num_cl++)
    {
      flux_bord=0;
      const Frontiere_dis_base& la_fr = op.equation().zone_Cl_dis().les_conditions_limites(num_cl).frontiere_dis();
      const Cond_lim& la_cl = op.equation().zone_Cl_dis().les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      for (int face=ndeb; face<nfin; face++)
        {
          for(int k=0; k<nb_compo; k++)
            flux_bord(k)+=flux_bords_(face, k);

          if (impr_mom)
            {
              // Calcul du moment exerce par le fluide sur le bord (OM/\F)
              if (Objet_U::dimension==2)
                moment(0)+=flux_bords_(face,1)*xgr(face,0)-flux_bords_(face,0)*xgr(face,1);
              else
                {
                  moment(0)+=flux_bords_(face,2)*xgr(face,1)-flux_bords_(face,1)*xgr(face,2);
                  moment(1)+=flux_bords_(face,0)*xgr(face,2)-flux_bords_(face,2)*xgr(face,0);
                  moment(2)+=flux_bords_(face,1)*xgr(face,0)-flux_bords_(face,0)*xgr(face,1);
                }
            }
        }

      // On somme les contributions de chaque processeur
      for(int k=0; k<nb_compo; k++)
        {
          flux_bord(k)=Process::mp_sum(flux_bord(k));
          if (impr_mom) moment(k)=Process::mp_sum(moment(k));
        }

      // Ecriture dans les fichiers
      if (Process::je_suis_maitre())
        {
          for(int k=0; k<nb_compo; k++)
            {
              Flux.add_col(flux_bord(k));
              if (impr_mom) Flux_moment.add_col(moment(k));
              if (la_zone_EF.zone().Bords_a_imprimer_sum().contient(la_fr.le_nom())) Flux_sum.add_col(flux_bord(k));

              // On somme les flux de toutes les frontieres pour mettre dans le tableau bilan
              bilan(k)+=flux_bord(k);
            }
        }
    }

  // On imprime les bilans et on va a la ligne
  if(Process::je_suis_maitre())
    {
      for(int k=0; k<nb_compo; k++)
        Flux.add_col(bilan(k));

      Flux << finl;
      if (impr_mom) Flux_moment << finl;
      if (impr_sum) Flux_sum << finl;
    }

  // Impression sur chaque face si demande
  for (int num_cl=0; num_cl<la_zone_EF.nb_front_Cl(); num_cl++)
    {
      const Frontiere_dis_base& la_fr = op.equation().zone_Cl_dis().les_conditions_limites(num_cl).frontiere_dis();
      const Cond_lim& la_cl = op.equation().zone_Cl_dis().les_conditions_limites(num_cl);
      const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = frontiere_dis.num_premiere_face();
      int nfin = ndeb + frontiere_dis.nb_faces();
      // Impression sur chaque face
      if (la_zone_EF.zone().Bords_a_imprimer().contient(la_fr.le_nom()))
        {
          Flux_face << "# Flux par face sur " << la_fr.le_nom() << " au temps " << temps << " : " << finl;
          const DoubleTab& xv=la_zone_EF.xv();
          for (int face=ndeb; face<nfin; face++)
            {
              if (Objet_U::dimension==2)
                Flux_face << "# Face a x= " << xv(face,0) << " y= " << xv(face,1) << " flux=" ;
              else if (Objet_U::dimension==3)
                Flux_face << "# Face a x= " << xv(face,0) << " y= " << xv(face,1) << " z= " << xv(face,2) << " flux=" ;
              for(int k=0; k<nb_compo; k++)
                Flux_face << " " << flux_bords_(face, k);
              Flux_face << finl;
            }
          Flux_face.syncfile();
        }
    }

  return 1;
}

int Op_EF_base::elem_contribue(const int& elem) const
{
  if (marqueur_elem_.size_array()==0)
    return 1;
  else if (marqueur_elem_(elem)==1)
    return 0;
  return 1;
}

void  Op_EF_base::marque_elem(const Equation_base& eqn)
{
  try
    {
      const DoubleTab& marq=eqn.get_champ("marqueur_loi_de_paroi").valeurs();
      int ntot=marq.dimension_tot(0);
      marqueur_elem_.resize_array(ntot);
      for (int n=0; n<ntot; n++)
        if (marq(n)>0)
          marqueur_elem_(n)=1;
    }
  catch  (Champs_compris_erreur)
    {
    }

}
