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

#include <Champ_P1NC_implementation.h>
#include <Dirichlet_paroi_defilante.h>
#include <Dirichlet_paroi_fixe.h>
#include <Mod_turb_hyd_base.h>
#include <Champ_Fonc_P1NC.h>
#include <Equation_base.h>
#include <distances_VEF.h>
#include <LecFicDiffuse.h>
#include <Matrice_Bloc.h>
#include <TRUSTLists.h>
#include <Champ_P1NC.h>
#include <Periodique.h>
#include <Solv_GCP.h>
#include <Domaine.h>
#include <Debog.h>
#include <SSOR.h>


static int methode_calcul_valeurs_sommets = 2 ;

// -1 moyenne des faces
// 1 moyenne des sommets sans min/max
// 2 moyenne des sommets avec min/max (ancienne version)

Champ_P1NC_implementation::Champ_P1NC_implementation()
{
  // Initialise l'attribut
  filtrer_L2_deja_appele_=0;
  // definition des types de solveurs par defaut (sinon, lecture dans solveur.bar)
  solveur_L2.typer("Solv_GCP");
  Precond p;
  p.typer("SSOR");
  ref_cast(Solv_GCP,solveur_L2.valeur()).set_precond(p);
  solveur_L2.nommer("solveur_L2");
  solveur_H1.typer("Solv_GCP");
  ref_cast(Solv_GCP,solveur_H1.valeur()).set_precond(p);
  solveur_H1.nommer("solveur_H1");



}

static const double coeff_penalisation = 1e9;



// Description:
// Projection du champ P1NC "cha" sur l'espace des champs P1.
// Le resultat est stocke dans cha.ch_som() et renvoye (valeur de retour)
// Voir note technique 2006/010 de Patrick Quemere "Nouvelle approche VEF pour la LES...".
// Voir aussi "What is in TRUST" : Scales_Separation_P1_P1NC.
DoubleTab& valeur_P1_L2(Champ_P1NC& cha, const Domaine& dom)
{
  const Zone& mazone = dom.zone(0);

  int nb_elem_tot = mazone.nb_elem_tot();
  int nb_som = mazone.nb_som_tot();
  int nb_som_elem = mazone.nb_som_elem();
  const DoubleTab& ch = cha.valeurs();

  const Zone_VEF& zone_VEF = cha.zone_vef();
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF, cha.equation().zone_Cl_dis().valeur());
  const IntTab& elem_som = mazone.les_elems();
  double mijK, miiK;
  DoubleTab& vit_som=cha.ch_som();

  int n_bord, nb_comp=vit_som.line_size();
  DoubleTab secmem(cha.ch_som());

  secmem=0.;
  //  vit_som=0.; // PQ : 06/10/04 : dans la resolution du systeme, "solution" repart de vit_som=cha.ch_som()
  //                                   pour s'affranchir de recalculer u_bar lors d'un double appel a filtrer_L2

  if(Objet_U::dimension==2)
    {
      mijK=1./12.;
      miiK= 1./6.;
    }
  else
    {
      mijK=1./18.;
      miiK= 1./12.;
    }

  // dans le cas d'une zone nulle on doit effectuer le dimensionnement
  // mais une seule fois
  double non_prepare=0;
  if (cha.MatP1NC2P1_L2.nb_lignes() <2)
    non_prepare=1;
  non_prepare=Process::mp_min(non_prepare);
  if (non_prepare==1)
    {
      // Initialisation du solveur
      SolveurSys& solv=ref_cast(SolveurSys, (cha.solveur_L2));
      LecFicDiffuse fic;
      fic.ouvrir("solveur.bar");
      if(fic.good())
        {
          fic >> solv;
          Cerr << Process::me() << "solveur.bar ouvert pour lire: " << solv << finl;
        }
      solv->reinit();

      Cerr<<"On remplit la matrice cha.MatP1NC2P1_L2 pour le champ "<< cha.le_nom() <<finl;
      Matrice_Morse_Sym& mat=ref_cast(Matrice_Morse_Sym, (cha.MatP1NC2P1_L2));
      int rang;
      int nnz=0;
      IntLists voisins(nb_som);
      DoubleLists coeffs(nb_som);
      DoubleVect diag(nb_som);
      int elem;
      for (elem=0; elem<nb_elem_tot; elem++)
        {
          double coeff_ij=mijK*zone_VEF.volumes(elem);
          double coeff_ii=miiK*zone_VEF.volumes(elem);
          for (int isom=0; isom<nb_som_elem; isom++)
            {
              int som=elem_som(elem,isom);
              int ii=dom.get_renum_som_perio(som);
              diag[ii]+=coeff_ii;
              for (int jsom=isom+1; jsom<nb_som_elem; jsom++)
                {
                  int asom=elem_som(elem,jsom);
                  int j=dom.get_renum_som_perio(asom);
                  int i=ii;
                  if(i>j)
                    {
                      int k=j;
                      j=i;
                      i=k;
                    }
                  rang=voisins[i].rang(j);
                  if(rang==-1)
                    {
                      voisins[i].add(j);
                      coeffs[i].add(coeff_ij);
                      nnz++;
                    }
                  else
                    {
                      coeffs[i][rang]+=coeff_ij;
                    }
                }
            }
        }

      // PQ : pour le remplissage des coefficients de la matrice specifiques au CL Dirichlet,
      // on commence par traiter les conditions Dirichlet, PUIS Dirichlet_homogene pour s'assurer
      // de l'"adherence" des points sommets appartenant aux deux types de CL (aretes de bords)

      IntVect test_cl_imposee(nb_som);
      test_cl_imposee = 0;

      for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
        {
          //for n_bord
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int face;
          int num1 = 0;
          int num2 = le_bord.nb_faces_tot();
          //         if ( sub_type(Dirichlet,la_cl.valeur()))
          if (!sub_type(Periodique,la_cl.valeur()))
            {
              for (int ind_face=num1; ind_face<num2; ind_face++)
                {
                  face = le_bord.num_face(ind_face);
                  for(int isom=0; isom<Objet_U::dimension; isom++)
                    {
                      int som=zone_VEF.face_sommets(face,isom);
                      som=dom.get_renum_som_perio(som);
                      if (test_cl_imposee(som) == 0)
                        diag[som] = 0;
                      diag[som] += coeff_penalisation;
                      test_cl_imposee(som) += 1;
                    }
                }
            }
        }//for n_bord

      test_cl_imposee = 0;

      for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
        {
          //for n_bord
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int face;
          int num1 = 0;
          int num2 = le_bord.nb_faces_tot();

          if (  sub_type(Dirichlet_homogene,la_cl.valeur()) )
            {
              for (int ind_face=num1; ind_face<num2; ind_face++)
                {
                  face = le_bord.num_face(ind_face);
                  for(int isom=0; isom<Objet_U::dimension; isom++)
                    {
                      int som=zone_VEF.face_sommets(face,isom);
                      som=dom.get_renum_som_perio(som);
                      if (test_cl_imposee(som) == 0)
                        diag[som] = 0;
                      diag[som] += coeff_penalisation;
                      test_cl_imposee(som) += 1;
                    }
                }
            }
        }//for n_bord

      {
        for (int i=0; i<nb_som; i++)
          if(diag(i)==0)
            diag(i)=1.;
      }

      //      coeffs = 0.;
      //      diag = 10.;
      mat.dimensionner(nb_som, nnz+nb_som) ;
      mat.remplir(voisins, coeffs, diag) ;
      mat.compacte() ;
      mat.set_est_definie(1);

      // Modif CD 15/04/03 pour paralleliser le filtrage
      // On copie la matrice remplie ci-dessus dans une matrice bloc.
      // Cette modification permet d'utiliser le preconditionnement SSOR

      //On fait aussi la transformation en matrice bloc en sequentiel car sinon
      //probleme avec NP

      cha.dimensionner_Mat_Bloc_Morse_Sym(cha.MatP1NC2P1_L2_Parallele);
      cha.Mat_Morse_to_Mat_Bloc(cha.MatP1NC2P1_L2_Parallele);

      // Fin Modif CD 15/04/03

      //mat.imprimer_formatte(Cerr);
      //      Cerr<<"zone.nb_som_tot() = "<<mazone.nb_som_tot()<<", zone.nb_som() = "<<mazone.nb_som()<<finl;
      //      Cerr << "Matrice de filtrage OK" << finl;
      //       Cout << "Matrice de masse P1 : " << finl;
      //       mat.imprimer(Cout) << finl;
    }

  // Debog::verifier(" verif champ " , ch);

  double coeff;
  if(Objet_U::dimension==2)
    coeff=1./6.;
  else
    coeff=1./12.;

  int nb_faces=zone_VEF.nb_faces();
  int nb_faces_tot=zone_VEF.nb_faces_tot();
  ArrOfInt virtfait(nb_faces_tot-nb_faces);
  int nb_som_face=zone_VEF.nb_som_face();
  const IntTab& face_sommets=zone_VEF.face_sommets();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  int face;
  int premiere_face_int = zone_VEF.premiere_face_int();

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      //for n_bord
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_bord = le_bord.nb_faces();
      int num1 = 0;
      int num2 = le_bord.nb_faces_tot();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          // *0.5 car les faces perio sont parcourues deux fois
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              face = le_bord.num_face(ind_face);
              if (ind_face-nb_faces_bord>=0) virtfait[face-nb_faces]=1;
              int elem1=face_voisins(face,0);
              int elem2=face_voisins(face,1);
              double volume=zone_VEF.volumes(elem1);
              if(elem2!=-1)
                volume+=zone_VEF.volumes(elem2);
              for(int isom=0; isom<nb_som_face; isom++)
                {
                  int som=face_sommets(face,isom);
                  int som1=dom.get_renum_som_perio(som);
                  for(int k=0; k<nb_comp; k++)
                    secmem(som1,k)+=0.5*coeff*volume*ch(face,k);
                }
            }
        }
      else
        for (int ind_face=num1; ind_face<num2; ind_face++)
          {
            face = le_bord.num_face(ind_face);
            if (ind_face-nb_faces_bord>=0) virtfait[face-nb_faces]=1;
            int elem1=face_voisins(face,0);
            int elem2=face_voisins(face,1);
            double volume=zone_VEF.volumes(elem1);
            if(elem2!=-1)
              volume+=zone_VEF.volumes(elem2);
            for(int isom=0; isom<nb_som_face; isom++)
              {
                int som=face_sommets(face,isom);
                int som1=dom.get_renum_som_perio(som);
                for(int k=0; k<nb_comp; k++)
                  secmem(som1,k)+=coeff*volume*ch(face,k);
              }
          }
    }

  for(face=premiere_face_int; face<nb_faces_tot; face++)
    {
      int indvirtface=face-nb_faces;
      // on regarde si la face virtuelle n'a pas deja ete traitee
      if (indvirtface>=0)
        if (virtfait[indvirtface]==1) continue;

      int elem1=face_voisins(face,0);
      int elem2=face_voisins(face,1);
      double volume=zone_VEF.volumes(elem1);
      if(elem2!=-1)
        volume+=zone_VEF.volumes(elem2);
      for(int isom=0; isom<nb_som_face; isom++)
        {
          int som=face_sommets(face,isom);
          int som1=dom.get_renum_som_perio(som);
          for(int k=0; k<nb_comp; k++)
            secmem(som1,k)+=coeff*volume*ch(face,k);
        }
    }
  // ************************************************************************************************
  // ************************************************************************************************
  //
  //                               Pour imposer u_som = u donne par CL_Dirichlet
  ////
  // ************************************************************************************************
  // ************************************************************************************************

  // PQ : on commence par traiter les conditions Dirichlet, PUIS Dirichlet_homogene pour s'assurer
  // de l'"adherence" des points sommets appartenant aux deux types de CL (aretes de bords)

  IntVect test_cl_imposee(nb_som);

  test_cl_imposee = 0;

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      //for n_bord
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = 0;
      int num2 = le_bord.nb_faces_tot();

      //         if ( sub_type(Dirichlet,la_cl.valeur()))
      if (!sub_type(Periodique,la_cl.valeur()))
        {
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              face = le_bord.num_face(ind_face);
              for(int isom=0; isom<nb_som_face; isom++)
                {
                  int som=face_sommets(face,isom);
                  int som1=dom.get_renum_som_perio(som);
                  for(int k=0; k<nb_comp; k++)
                    {
                      if (test_cl_imposee(som1)==0)
                        secmem(som1,k) = 0;

                      secmem(som1,k) += coeff_penalisation * ch(face,k);
                    }
                  test_cl_imposee(som1) += 1;
                }
            }
        }
    }//for n_bord

  test_cl_imposee = 0;

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      //for n_bord
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = 0;
      int num2 = le_bord.nb_faces_tot();

      if (sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          for (int ind_face=num1; ind_face<num2; ind_face++)
            {
              face = le_bord.num_face(ind_face);
              for(int isom=0; isom<nb_som_face; isom++)
                {
                  int som=face_sommets(face,isom);
                  int som1=dom.get_renum_som_perio(som);
                  for(int k=0; k<nb_comp; k++)
                    {
                      if (test_cl_imposee(som1)==0)
                        secmem(som1,k) = 0;

                      secmem(som1,k) += coeff_penalisation * ch(face,k);
                    }
                  test_cl_imposee(som1) += 1;
                }
            }
        }
    }//for n_bord

  // ************************************************************************************************
  // ************************************************************************************************
  secmem.echange_espace_virtuel();
  //  Debog::verifier(" verif secmem_ech " , secmem);

  SolveurSys& solv=ref_cast(SolveurSys, cha.solveur_L2);
  DoubleVect secmemk(cha.ch_som_vect());
  DoubleVect solution(cha.ch_som_vect());

  if(nb_comp==1)
    {
      solution=vit_som;
      secmem.echange_espace_virtuel();
      Debog::verifier(" verif secmem " , secmem);


      //ON UTILISE LA MEME MATRICE EN SEQUENTIEL ET PARALLELE
      //CE QUI PERMET D UTILISER UNE MATRICE BLOC QUAND ON APPELLE LE SOLVEUR NP
      solv.resoudre_systeme(cha.MatP1NC2P1_L2_Parallele.valeur(), secmem, solution);

      for(int som=0; som<nb_som; som++)
        vit_som(som)=solution(dom.get_renum_som_perio(som));
    }
  else
    {
      for(int k=0; k<nb_comp; k++)
        {
          int som;
          for(som=0; som<nb_som; som++)
            {
              solution(som)=vit_som(som,k);
              secmemk(som)=secmem(som,k);
            }

          solution.echange_espace_virtuel();
          secmemk.echange_espace_virtuel();

          //ON UTILISE LA MEME MATRICE EN SEQUENTIEL ET PARALLELE
          //CE QUI PERMET D UTILISER UNE MATRICE BLOC QUAND ON APPELLE LE SOLVEUR NP
          solv.resoudre_systeme(cha.MatP1NC2P1_L2_Parallele.valeur(), secmemk, solution);

          for(som=0; som<nb_som; som++)
            vit_som(som,k)=solution(dom.get_renum_som_perio(som));
        }
    }

  // Debog::verifier(" verif vit_som 0 " , vit_som);
  vit_som.echange_espace_virtuel();
  // Debog::verifier(" verif vit_som 1 " , vit_som);

  //  {
  //    for(int k=0; k<nb_comp; k++)
  //    for(int som=0; som<nb_som; som++)
  //    Cerr << k << " x " << coord(som,0)<< " y " << coord(som,1) << " " << vit_som(som,k) << finl;
  //     double psc=vit_som*vit_som;
  //     psc = Process::mp_sum(psc);
  //     Debog::verifier("norme : ", psc);
  //     Cerr << "############ psc = " << psc << finl;
  //  }
  Debog::verifier(" verif vit_som " , vit_som);
  return vit_som;
}

DoubleTab& valeur_P1_L2(Champ_Fonc_P1NC& cha, const Domaine& dom)
{
  const Zone& mazone = dom.zone(0);
  int nb_elem_tot = mazone.nb_elem_tot();
  int nb_som = mazone.nb_som_tot();
  int nb_som_elem = mazone.nb_som_elem();
  const DoubleTab& ch = cha.valeurs();
  const Zone_VEF& zone_VEF = cha.zone_vef();
  const IntTab& elem_som = mazone.les_elems();
  double mijK, miiK;
  DoubleTab& vit_som=cha.ch_som();

  int nb_comp=vit_som.line_size();
  DoubleTab secmem(cha.ch_som());
  secmem=0.;

  if(Objet_U::dimension==2)
    {
      mijK=1./12.;
      miiK= 1./6.;
    }
  else
    {
      mijK=1./18.;
      miiK= 1./12.;
    }

  // dans le cas d'une zone nulle on doit effectuer le dimensionnement
  // mais une seule fois
  double non_prepare=0;
  if (cha.MatP1NC2P1_L2.nb_lignes() <2)
    non_prepare=1;
  non_prepare=Process::mp_min(non_prepare);
  if (non_prepare==1)
    {
      // Initialisation du solveur
      SolveurSys& solv=ref_cast(SolveurSys, (cha.solveur_L2));
      LecFicDiffuse fic;
      fic.ouvrir("solveur.bar");
      if(fic.good())
        {
          fic >> solv;
          Cerr << Process::me() << "solveur.bar ouvert pour lire: " << solv << finl;
        }
      solv->reinit();

      Cerr<<"On remplit la matrice cha.MatP1NC2P1_L2 pour le champ "<< cha.le_nom() <<finl;
      Matrice_Morse_Sym& mat=ref_cast(Matrice_Morse_Sym, (cha.MatP1NC2P1_L2));
      int rang;
      int nnz=0;
      IntLists voisins(nb_som);
      DoubleLists coeffs(nb_som);
      DoubleVect diag(nb_som);
      int elem;
      for (elem=0; elem<nb_elem_tot; elem++)
        {
          double coeff_ij=mijK*zone_VEF.volumes(elem);
          double coeff_ii=miiK*zone_VEF.volumes(elem);
          for (int isom=0; isom<nb_som_elem; isom++)
            {
              int som=elem_som(elem,isom);
              int ii=dom.get_renum_som_perio(som);
              diag[ii]+=coeff_ii;
              for (int jsom=isom+1; jsom<nb_som_elem; jsom++)
                {
                  int asom=elem_som(elem,jsom);
                  int j=dom.get_renum_som_perio(asom);
                  int i=ii;
                  if(i>j)
                    {
                      int k=j;
                      j=i;
                      i=k;
                    }
                  rang=voisins[i].rang(j);
                  if(rang==-1)
                    {
                      voisins[i].add(j);
                      coeffs[i].add(coeff_ij);
                      nnz++;
                    }
                  else
                    {
                      coeffs[i][rang]+=coeff_ij;
                    }
                }
            }
        }


      {
        for (int i=0; i<nb_som; i++)
          if(diag(i)==0)
            diag(i)=1.;
      }

      mat.dimensionner(nb_som, nnz+nb_som) ;
      mat.remplir(voisins, coeffs, diag) ;
      mat.compacte() ;
      mat.set_est_definie(1);

      // Modif CD 15/04/03 pour paralleliser le filtrage
      // On copie la matrice remplie ci-dessus dans une matrice bloc.
      // Cette modification permet d'utiliser le preconditionnement SSOR

      //On fait aussi la transformation en matrice bloc en sequentiel car sinon
      //probleme avec NP

      cha.dimensionner_Mat_Bloc_Morse_Sym(cha.MatP1NC2P1_L2_Parallele);
      cha.Mat_Morse_to_Mat_Bloc(cha.MatP1NC2P1_L2_Parallele);

      // Fin Modif CD 15/04/03

      //mat.imprimer_formatte(Cerr);
      //      Cerr<<"zone.nb_som_tot() = "<<mazone.nb_som_tot()<<", zone.nb_som() = "<<mazone.nb_som()<<finl;
      //      Cerr << "Matrice de filtrage OK" << finl;
      //       Cout << "Matrice de masse P1 : " << finl;
      //       mat.imprimer(Cout) << finl;
    }

  // Debog::verifier(" verif champ " , ch);

  double coeff;
  if(Objet_U::dimension==2)
    coeff=1./6.;
  else
    coeff=1./12.;

  int nb_faces=zone_VEF.nb_faces();
  int nb_faces_tot=zone_VEF.nb_faces_tot();
  ArrOfInt virtfait(nb_faces_tot-nb_faces);
  int nb_som_face=zone_VEF.nb_som_face();
  const IntTab& face_sommets=zone_VEF.face_sommets();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  int face;

  for(face=0; face<nb_faces_tot; face++)
    {
      int indvirtface=face-nb_faces;
      // on regarde si la face virtuelle n'a pas deja ete traitee
      if (indvirtface>=0)
        if (virtfait[indvirtface]==1) continue;

      int elem1=face_voisins(face,0);
      int elem2=face_voisins(face,1);
      double volume=zone_VEF.volumes(elem1);
      if(elem2!=-1)
        volume+=zone_VEF.volumes(elem2);
      for(int isom=0; isom<nb_som_face; isom++)
        {
          int som=face_sommets(face,isom);
          int som1=dom.get_renum_som_perio(som);
          //Pour un sommet periodique on ne retient qu une seule contribution
          //pour deux faces en vis a vis
          if (som1==som)
            for(int k=0; k<nb_comp; k++)
              secmem(som1,k)+=coeff*volume*ch(face,k);
        }
    }

  secmem.echange_espace_virtuel();
  //  Debog::verifier(" verif secmem_ech " , secmem);

  SolveurSys& solv=ref_cast(SolveurSys, cha.solveur_L2);
  DoubleVect secmemk(cha.ch_som_vect());
  DoubleVect solution(cha.ch_som_vect());

  if(nb_comp==1)
    {
      solution=vit_som;
      secmem.echange_espace_virtuel();
      Debog::verifier(" verif secmem " , secmem);


      //ON UTILISE LA MEME MATRICE EN SEQUENTIEL ET PARALLELE
      //CE QUI PERMET D UTILISER UNE MATRICE BLOC QUAND ON APPELLE LE SOLVEUR NP
      solv.resoudre_systeme(cha.MatP1NC2P1_L2_Parallele.valeur(), secmem, solution);

      for(int som=0; som<nb_som; som++)
        vit_som(som)=solution(dom.get_renum_som_perio(som));
    }
  else
    {
      for(int k=0; k<nb_comp; k++)
        {
          int som;
          for(som=0; som<nb_som; som++)
            {
              solution(som)=vit_som(som,k);
              secmemk(som)=secmem(som,k);
            }

          solution.echange_espace_virtuel();
          secmemk.echange_espace_virtuel();

          //ON UTILISE LA MEME MATRICE EN SEQUENTIEL ET PARALLELE
          //CE QUI PERMET D UTILISER UNE MATRICE BLOC QUAND ON APPELLE LE SOLVEUR NP
          solv.resoudre_systeme(cha.MatP1NC2P1_L2_Parallele.valeur(), secmemk, solution);

          for(som=0; som<nb_som; som++)
            vit_som(som,k)=solution(dom.get_renum_som_perio(som));
        }
    }

  // Debog::verifier(" verif vit_som 0 " , vit_som);
  vit_som.echange_espace_virtuel();
  // Debog::verifier(" verif vit_som 1 " , vit_som);

  //  {
  //    for(int k=0; k<nb_comp; k++)
  //    for(int som=0; som<nb_som; som++)
  //    Cerr << k << " x " << coord(som,0)<< " y " << coord(som,1) << " " << vit_som(som,k) << finl;
  //     double psc=vit_som*vit_som;
  //     psc = Process::mp_sum(psc);
  //     Debog::verifier("norme : ", psc);
  //     Cerr << "############ psc = " << psc << finl;
  //  }
  Debog::verifier(" verif vit_som " , vit_som);
  return vit_som;
}

DoubleTab&
valeur_P1_H1(const Champ_P1NC& cha,
             const Domaine& dom,
             DoubleTab& ch_som)
{
  const Zone& zone = dom.zone(0);
  int nb_elem_tot = zone.nb_elem_tot();
  int nb_som = zone.nb_som();
  int nb_som_elem = zone.nb_som_elem();
  const DoubleTab& ch = cha.valeurs();
  const Zone_VEF& zone_VEF = cha.zone_vef();
  const DoubleTab& normales=zone_VEF.face_normales();
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF, cha.equation().zone_Cl_dis().valeur());
  const IntTab& elem_som = zone.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  double mijK;
  int nb_comp=ch_som.dimension(1);
  int dimension=Objet_U::dimension;

  if(dimension==2)
    {
      mijK=1./4.;
    }
  else
    {
      mijK=1./9.;
    }

  int elem;
  int n_bord;
  static double diag_ref;
  double non_prepare=0;
  if ( cha.MatP1NC2P1_H1.nb_lignes() <2 )
    non_prepare=1;
  non_prepare=Process::mp_min(non_prepare);
  if (non_prepare==1)
    {
      EFichier fic;
      if(fic.ouvrir("solveur.bar"))
        {
          SolveurSys& solv=ref_cast_non_const(SolveurSys,cha.solveur_H1);
          fic >> solv;
        }
      Matrice_Morse_Sym& mat=ref_cast_non_const(Matrice_Morse_Sym,cha.MatP1NC2P1_H1);
      int rang;
      int nnz=0;
      IntLists voisins(nb_som);
      DoubleLists coeffs(nb_som);
      DoubleVect diag(nb_som);
      for (elem=0; elem<nb_elem_tot; elem++)
        {
          double volume=zone_VEF.volumes(elem);
          for (int isom=0; isom<nb_som_elem; isom++)
            {
              int facei=elem_faces(elem,isom);
              int i=dom.get_renum_som_perio(elem_som(elem,isom));
              for (int jsom=isom+1; jsom<nb_som_elem; jsom++)
                {
                  int facej=elem_faces(elem,jsom);
                  int j=dom.get_renum_som_perio(elem_som(elem,jsom));
                  double coeffij=0.0;
                  for(int k=0; k<dimension; k++)
                    coeffij+=normales(facei,k)*normales(facej,k);

                  coeffij*=zone_VEF.oriente_normale(facei,elem)*
                           zone_VEF.oriente_normale(facej,elem);
                  coeffij*=mijK/volume;
                  int ii=i, jj=j;
                  if(ii>jj)
                    {
                      int tmp=ii;
                      ii=jj;
                      jj=tmp;
                    }
                  rang=voisins[ii].rang(jj);
                  if(rang==-1)
                    {
                      voisins[ii].add(jj);
                      coeffs[ii].add(coeffij);
                      nnz++;
                    }
                  else
                    {
                      coeffs[ii][rang]+=coeffij;
                    }
                  diag[ii]-=coeffij;
                  diag[jj]-=coeffij;
                }
            }
        }
      diag_ref=diag(0);
      for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
        {
          //for n_bord
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();

          for (int face=num1; face<num2; face++)
            {
              elem=zone_VEF.face_voisins(face,0);
              for(int isom=0; isom<dimension; isom++)
                {
                  int som=zone_VEF.face_sommets(face,isom);
                  som=dom.get_renum_som_perio(som);
                  diag[som]=1.e1*diag_ref;
                }
            }
        }

      mat.dimensionner(nb_som, nnz+nb_som) ;
      mat.remplir(voisins, coeffs, diag) ;
      mat.compacte() ;
      mat.set_est_definie(1);
      //       Cerr << "Matrice de filtrage OK" << finl;
      //       Cerr << "Matrice de Laplacien P1 : " << finl;
      //       mat.imprimer(Cerr) << finl;
    }


  double coeff=-1./dimension;
  int k;
  for(int comp=0; comp<nb_comp; comp++)
    {
      //for comp
      DoubleVect secmem(nb_som);
      DoubleVect solution(nb_som);
      for (elem=0; elem<nb_elem_tot; elem++)
        {
          //for elem
          double volume=zone_VEF.volumes(elem);
          for (int isom=0; isom<nb_som_elem; isom++)
            {
              int facei=elem_faces(elem,isom);
              int i=dom.get_renum_som_perio(elem_som(elem,isom));
              for (int jsom=0; jsom<nb_som_elem; jsom++)
                {
                  int facej=elem_faces(elem,jsom);
                  double coeffij=0;
                  for(k=0; k<dimension; k++)
                    coeffij+=normales(facei,k)*normales(facej,k);
                  coeffij*=zone_VEF.oriente_normale(facei,elem)*
                           zone_VEF.oriente_normale(facej,elem);
                  coeffij*=coeff/volume;
                  secmem(i)+=coeffij*ch(facej,comp);
                }
            }
        }
      for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
        {
          //for n_bord
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();

          for (int face=num1; face<num2; face++)
            {
              elem=zone_VEF.face_voisins(face,0);
              for(int isom=0; isom<dimension; isom++)
                {
                  int som=zone_VEF.face_sommets(face,isom);
                  som=dom.get_renum_som_perio(som);
                  secmem(som)=1.e1*diag_ref* cha.valeur_a_sommet_compo(som, elem, comp);
                }
            }
        }

      SolveurSys& solv=ref_cast_non_const(SolveurSys,cha.solveur_H1);
      solv.resoudre_systeme(cha.MatP1NC2P1_H1, secmem, solution);

      for(int som=0; som<nb_som; som++)
        ch_som(som,comp)=solution(dom.get_renum_som_perio(som));

    }

  return ch_som;
}

void Champ_P1NC_implementation::filtrer_H1(DoubleTab& valeurs) const
{
  //Cerr << "Champ_P1NC_implementation::filtrer" << finl;
  const Zone_VEF& zone_VEF = zone_vef();
  static int test=1;
  if(!test)
    {
      test=1;
      DoubleTab xv=zone_VEF.xv();
      Cerr << "TEST filtrer_H1 solution = " << mp_norme_vect(xv) << finl;
      filtrer_H1(xv);
      xv-=zone_VEF.xv();
      Cerr << "TEST filtrer_H1 erreur = " << mp_norme_vect(xv) << finl;
      Cerr << "err = " << xv
           << "zone_VEF.xv() " << zone_VEF.xv() << finl;
      xv=1.;
      filtrer_H1(xv);
      xv-=1.;
      Cerr << "erreur sur les constantes : " << mp_norme_vect(xv) << finl;
    }
  int nb_som= zone_VEF.zone().nb_som_tot();
  int nb_faces=valeurs.dimension(0);
  const Champ_P1NC& cha_const =ref_cast(Champ_P1NC,le_champ());
  Champ_P1NC& cha=ref_cast_non_const(Champ_P1NC,cha_const);
  DoubleTab valeurs_fac(cha.valeurs());
  cha.valeurs()=valeurs;
  const int nb_comp=valeurs.line_size();
  DoubleTab vit_som(nb_som, nb_comp);
  valeur_P1_H1(cha, cha.domaine(), vit_som);
  cha.valeurs()=valeurs_fac;
  double coeff= 1./Objet_U::dimension;

  for(int face=0; face<nb_faces; face++)
    {
      int s1=zone_VEF.face_sommets(face,0);
      int s2=zone_VEF.face_sommets(face,1);
      int s3=-1;
      if(Objet_U::dimension==3)
        s3=zone_VEF.face_sommets(face,2);
      for(int comp=0; comp<nb_comp; comp++)
        {
          double somme=0;
          somme+=(vit_som(s1,comp)+vit_som(s2,comp));
          if(Objet_U::dimension==3)
            somme+=vit_som(s3,comp);
          valeurs(face, comp)=coeff*somme;
        }
    }
}

void Champ_P1NC_implementation::filtrer_L2(DoubleTab& valeurs) const
{
  const Zone_VEF& zone_VEF = zone_vef();
  const Domaine& dom=zone_VEF.zone().domaine();
  double coeff= 1./Objet_U::dimension;
  //   int nb_som= zone_VEF.zone().nb_som_tot();
  int nb_faces=valeurs.dimension_tot(0);
  Champ_base& cha = ref_cast_non_const(Champ_base,le_champ());
  if (!sub_type(Champ_P1NC,cha) && (!sub_type(Champ_Fonc_P1NC,cha)))
    {
      Cerr<<"on ne doit pas filtre_L2 des champs qui ne sont pas de type P1NC"<<finl;
      Process::exit();
    }

  const int nb_comp=valeurs.line_size();

  if (sub_type(Champ_P1NC,cha))
    {
      Champ_P1NC& ch_inc_P1NC = ref_cast(Champ_P1NC,cha);

      if (ch_inc_P1NC.equation().le_nom()!="gradient_pression")
        {
          const RefObjU& modele_turbulence = ch_inc_P1NC.equation().get_modele(TURBULENCE);
          if (modele_turbulence.non_nul() && sub_type(Mod_turb_hyd_base,modele_turbulence.valeur()))
            {
              const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
              const Turbulence_paroi& loipar = mod_turb.loi_paroi();
              DoubleTab tau_tan;

              if (loipar.non_nul() && loipar.valeur().use_shear() )
                {
                  tau_tan.ref(loipar->Cisaillement_paroi());

                  //  Interpolation des vitesses a la paroi

                  const Zone_VEF& zone_VE_chF = ch_inc_P1NC.zone_vef();
                  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,ch_inc_P1NC.equation().zone_Cl_dis().valeur());
                  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
                  const Zone& mazone = zone_VE_chF.zone();
                  const IntTab& face_voisins = zone_VE_chF.face_voisins();
                  const IntTab& elem_faces = zone_VE_chF.elem_faces();
                  int nfac = mazone.nb_faces_elem(0);
                  int nb_cl=les_cl.size();
                  IntVect num(nfac);
                  int num_cl,fac,ind_fac;

                  double kappa=0.41;
                  double norm_v,val0,val1,val2,norm_tau,u_tau;

                  for (num_cl=0; num_cl<nb_cl; num_cl++)
                    {
                      //Boucle sur les bords
                      const Cond_lim& la_cl = les_cl[num_cl];
                      const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
                      int ndeb = 0;
                      int nfin = la_front_dis.nb_faces_tot();

                      if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) && tau_tan.size()!=0)
                        {
                          for (ind_fac=ndeb; ind_fac<nfin ; ind_fac++)
                            {
                              fac = la_front_dis.num_face(ind_fac);
                              int elem = face_voisins(fac,0);

                              if (Objet_U::dimension == 2 )
                                {
                                  num[0]=elem_faces(elem,0);
                                  num[1]=elem_faces(elem,1);
                                  if      (num[0]==fac) num[0]=elem_faces(elem,2);
                                  else if (num[1]==fac) num[1]=elem_faces(elem,2);

                                  norm_v=norm_2D_vit1_lp(valeurs,fac,num[0],num[1],zone_VE_chF,val0,val1);
                                  norm_tau=sqrt(tau_tan(fac,0)*tau_tan(fac,0)+tau_tan(fac,1)*tau_tan(fac,1));
                                  u_tau=sqrt(norm_tau);
                                  /*
                                  // PQ : 06/03 : pour un champ de vitesse 1D, l'incompressibilite genere des vitesses (meme faibles) sur toutes
                                  // les composantes, qui se repercutent par la suite au niveau de la CL ici.
                                  //  (se manifestant en cours de calcul en entree par un pic de vitesse en proche paroi).
                                  // Pour assurer des entrees "correctes" (1D) on remedie a ce probleme via l'operation suivante
                                  // (en supposant que les entrees sont alignees avec une des directions d'espace)

                                  if(std::fabs(val0)>0.999) {val0=1.;val1=0.;}
                                  if(std::fabs(val1)>0.999) {val0=0.;val1=1.;}
                                  */

                                  valeurs(fac,0)=(norm_v-u_tau/kappa)*val0;
                                  valeurs(fac,1)=(norm_v-u_tau/kappa)*val1;

                                  //                                for (i=0 ; i<Objet_U::dimension ; i++)
                                  //                                 valeurs(fac,i)=(valeurs(num[0],i)+valeurs(num[1],i))/2.-sqrt(tau_tan(fac,i))/kappa;
                                  //                                 valeurs(fac,0)=(valeurs(num[0],0)+valeurs(num[1],0))/2.-sqrt(tau_tan(fac,0))/kappa;
                                }
                              else
                                {
                                  num[0]=elem_faces(elem,0);
                                  num[1]=elem_faces(elem,1);
                                  num[2]=elem_faces(elem,2);
                                  if      (num[0]==fac) num[0]=elem_faces(elem,3);
                                  else if (num[1]==fac) num[1]=elem_faces(elem,3);
                                  else if (num[2]==fac) num[2]=elem_faces(elem,3);

                                  norm_v=norm_3D_vit1_lp(valeurs,fac,num[0],num[1],num[2],zone_VE_chF,val0,val1,val2);
                                  norm_tau=sqrt(tau_tan(fac,0)*tau_tan(fac,0)+tau_tan(fac,1)*tau_tan(fac,1)+tau_tan(fac,2)*tau_tan(fac,2));
                                  u_tau=sqrt(norm_tau);
                                  /*
                                  // PQ : 06/03 : pour un champ de vitesse 1D, l'incompressibilite genere des vitesses (meme faibles) sur toutes
                                  // les composantes, qui se repercutent par la suite au niveau de la CL ici.
                                  //  (se manifestant en cours de calcul en entree par un pic de vitesse en proche paroi).
                                  // Pour assurer des entrees "correctes" (1D) on remedie a ce probleme via l'operation suivante
                                  // (en supposant que les entrees sont alignees suivant une des directions d'espace)

                                  if(std::fabs(val0)>0.999) {val0=1.;val1=0.;val2=0.;}
                                  if(std::fabs(val1)>0.999) {val0=0.;val1=1.;val2=0.;}
                                  if(std::fabs(val2)>0.999) {val0=0.;val1=0.;val2=1.;}
                                  */
                                  valeurs(fac,0)=(norm_v-u_tau/kappa)*val0;
                                  valeurs(fac,1)=(norm_v-u_tau/kappa)*val1;
                                  valeurs(fac,2)=(norm_v-u_tau/kappa)*val2;


                                  //      DoubleTab xv=zone_VEF.xv();
                                  //      Cerr << fac << " " << val0 << " " << val1 << " " << val2 <<finl;
                                  //      Cerr << fac << " " << xv(fac,2) << " " << valeurs(fac,0) << " " << valeurs(fac,1) << " " << valeurs(fac,2) << " " << u_tau << " " << norm_v <<finl;

                                  //                                for (i=0 ; i<Objet_U::dimension ; i++)
                                  //                                 valeurs(fac,i)=(valeurs(num[0],i)+valeurs(num[1],i)+valeurs(num[2],i))/3.-sqrt(tau_tan(fac,i))/kappa;
                                  //                                 valeurs(fac,0)=(valeurs(num[0],0)+valeurs(num[1],0)+valeurs(num[2],0))/3.-sqrt(tau_tan(fac,0))/kappa;
                                }
                            }//fac
                        }//Dirichlet_paroi
                    } //Boucle sur les bords
                  valeurs.echange_espace_virtuel();
                }//!Paroi_negligeable_VEF
            }
        }
    }

  DoubleTab valeurs_fac(cha.valeurs());
  cha.valeurs()=valeurs;
  Debog::verifier(" verif valeurs " , valeurs);

  if (sub_type(Champ_P1NC,cha))
    {
      Champ_P1NC& ch_inc = ref_cast(Champ_P1NC,cha);
      valeur_P1_L2(ch_inc,dom);
    }
  else if (sub_type(Champ_Fonc_P1NC,cha))
    {
      Champ_Fonc_P1NC& ch_fonc = ref_cast(Champ_Fonc_P1NC,cha);
      valeur_P1_L2(ch_fonc,dom);
    }

  Debog::verifier(" verif valeurs " , valeurs);
  cha.valeurs()=valeurs_fac;
  valeurs_fac=valeurs;
  Debog::verifier(" verif ch_som_ " , ch_som_);
  for(int face=0; face<nb_faces; face++)
    {
      int s1=zone_VEF.face_sommets(face,0);
      s1=dom.get_renum_som_perio(s1);
      int s2=zone_VEF.face_sommets(face,1);
      s2=dom.get_renum_som_perio(s2);
      int s3=-1;
      if(Objet_U::dimension==3)
        s3=dom.get_renum_som_perio(zone_VEF.face_sommets(face,2));
      for(int comp=0; comp<nb_comp; comp++)
        {
          double somme=0;
          somme+=(ch_som_(s1,comp)+ch_som_(s2,comp));
          if(Objet_U::dimension==3)
            somme+=ch_som_(s3,comp);
          valeurs(face, comp)=coeff*somme;
        }
    }
  valeurs.echange_espace_virtuel();
  Debog::verifier(" verif valeurs L2" , valeurs);
}


void Champ_P1NC_implementation::filtrer_resu(DoubleTab& resu) const
{
  const Champ_P1NC& cha_const =ref_cast(Champ_P1NC,le_champ());
  Champ_P1NC& cha=ref_cast_non_const(Champ_P1NC, cha_const);
  double non_prepare=0;
  if (cha.MatP1NC2P1_L2.nb_lignes() <2)
    non_prepare=1;
  non_prepare=Process::mp_min(non_prepare);
  if (non_prepare==1)
    {
      // GF si on n a pas prepare la matrice, on appelle valeur_P1_L2(cha, cha.domaine()); et on remet les anciennes valeurs aux sommets
      DoubleTab vit_som_sa=cha.ch_som();
      valeur_P1_L2(cha, cha.domaine());
      cha.ch_som()=vit_som_sa;
    }

  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,
                                            cha.equation().zone_Cl_dis().valeur());
  const Zone_VEF& zone_VEF = zone_vef();
  const DoubleVect& volumes_entrelaces= zone_VEF.volumes_entrelaces();
  const Zone& mazone =zone_VEF.zone();
  const Domaine& dom=mazone.domaine();
  const IntTab& face_sommets=zone_VEF.face_sommets();
  int nb_faces=resu.dimension(0);
  int nb_faces_tot=resu.dimension_tot(0);
  int n_bord,nb_comp=resu.line_size();

  DoubleTab resu_som(cha.ch_som());
  resu_som = 0;

  ArrOfInt virtfait(nb_faces_tot-nb_faces);

  double coeff= 1./Objet_U::dimension;
  int face,ind_face;
  {
    for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
      {
        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        int nb_faces_tot_bord=le_bord.nb_faces_tot();
        int nb_faces_bord = le_bord.nb_faces();
        int num1 = 0;
        int num2 = nb_faces_tot_bord;

        if (sub_type(Periodique,la_cl.valeur()))
          {
            const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
            int face_associee;
            IntVect fait(nb_faces_tot_bord);
            for (ind_face=num1; ind_face<num2; ind_face++)
              {
                face = le_bord.num_face(ind_face);
                if (fait(ind_face) == 0)
                  {
                    fait(ind_face) = 1;
                    if (ind_face-nb_faces_bord>=0) virtfait[face-nb_faces]=1;
                    face_associee=la_cl_perio.face_associee(ind_face);
                    fait(face_associee) = 1;
                    if (ind_face-nb_faces_bord>=0)
                      {
                        face_associee = le_bord.num_face(face_associee);
                        virtfait[face_associee-nb_faces]=1;
                      }

                    for(int isom=0; isom<Objet_U::dimension; isom++)
                      {
                        int som=face_sommets(face,isom);
                        som=dom.get_renum_som_perio(som);
                        for (int comp=0; comp<nb_comp; comp++)
                          resu_som(som, comp)+=coeff*resu(face, comp);
                      }

                  }
              }
          }
        else
          {
            for (ind_face=num1; ind_face<num2; ind_face++)
              {
                face = le_bord.num_face(ind_face);
                if (ind_face-nb_faces_bord>=0) virtfait[face-nb_faces]=1;
                for(int isom=0; isom<Objet_U::dimension; isom++)
                  {
                    int som=face_sommets(face,isom);
                    som=dom.get_renum_som_perio(som);
                    for (int comp=0; comp<nb_comp; comp++)
                      resu_som(som, comp)+=coeff*resu(face, comp);
                  }
              }
          }
      }


    int deb=zone_VEF.premiere_face_int();


    for(face=deb; face<nb_faces_tot; face++)
      {
        int indvirtface=face-nb_faces;
        // on regarde si la face virtuelle n'a pas deja ete traitee
        if (indvirtface>=0)
          if (virtfait[indvirtface]==1) continue;
        for(int isom=0; isom<Objet_U::dimension; isom++)
          {
            int som=face_sommets(face,isom);
            som=dom.get_renum_som_perio(som);
            for(int comp=0; comp<nb_comp; comp++)
              resu_som(som, comp)+=coeff*resu(face, comp);
          }
      }
  }

  //  resu_som.echange_espace_virtuel();
  //  Debog::verifier("Champ_P1NC_implementation::filtrer_resu 1.5 ",resu);
  //  Debog::verifier("Champ_P1NC_implementation::filtrer_resu_som 1.5 ",resu_som);

  // ************************************************************************************************
  // ************************************************************************************************
  //
  //                           Pour imposer residu_som = residu donne par CL
  //
  // ************************************************************************************************
  // ************************************************************************************************
  IntVect test_cl_imposee(resu_som.dimension_tot(0));
  test_cl_imposee = 0;

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = 0;
      int num2 = le_bord.nb_faces_tot() ;

      //        if (sub_type(Dirichlet,la_cl.valeur()) || sub_type(Dirichlet_homogene,la_cl.valeur()))
      if (sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face = le_bord.num_face(ind_face);
              double vol=volumes_entrelaces(face);
              for(int isom=0; isom<Objet_U::dimension; isom++)
                {
                  int som=face_sommets(face,isom);
                  som=dom.get_renum_som_perio(som);
                  for (int comp=0; comp<nb_comp; comp++)
                    {
                      if (test_cl_imposee(som)==0)
                        resu_som(som,comp) = 0;
                      resu_som(som, comp) += coeff_penalisation * resu(face, comp)/vol;
                    }
                  test_cl_imposee(som) += 1;
                }
            }
        }
    }

  resu_som.echange_espace_virtuel();

  SolveurSys& solv=ref_cast(SolveurSys,cha.solveur_L2);
  int i,nbs = resu_som.dimension(0);

  DoubleVect secmem(cha.ch_som_vect());
  DoubleVect solution(cha.ch_som_vect());

  // Modif CD 15/04/03 pour paralleliser le filtrage
  //  Matrice matrice_tmp;
  //  cha.dimensionner_Mat_Bloc_Morse_Sym(matrice_tmp);
  //  cha.Mat_Morse_to_Mat_Bloc(matrice_tmp);
  // Fin Modif CD 15/04/03

  //  Debog::verifier_Mat_sommet("Champ_P1NC_implementation::filtrer_resu  cha.MatP1NC2P1_L2 ", cha.MatP1NC2P1_L2);

  for(int k=0; k<nb_comp; k++)
    {
      solution=0.;
      for(i=0; i<nbs; i++)
        secmem(i)=resu_som(i,k);

      secmem.echange_espace_virtuel();

      solv.resoudre_systeme(cha.MatP1NC2P1_L2_Parallele.valeur(), secmem, solution);

      for(i=0; i<nbs; i++)
        resu_som(i,k)=solution(i);
    }

  resu=0;
  {
    for(face=0; face<nb_faces; face++)
      {
        for(int isom=0; isom<Objet_U::dimension; isom++)
          {
            int som=dom.get_renum_som_perio(zone_VEF.face_sommets(face,isom));
            for(int comp=0; comp<nb_comp; comp++)
              resu(face, comp)+=coeff*resu_som(som, comp);
          }
      }
  }
  {
    for(face=0; face<nb_faces; face++)
      {
        double vol=volumes_entrelaces(face);
        for(int comp=0; comp<nb_comp; comp++)
          resu(face, comp)*=vol;
      }
  }
}

DoubleVect& Champ_P1NC_implementation::valeur_a_elem(const DoubleVect& position,
                                                     DoubleVect& val,
                                                     int le_poly) const
{
  const Champ_base& cha=le_champ();
  const int N = cha.nb_comp(), D = Objet_U::dimension;
  const Zone& zone_geom = zone();
  const Zone_VEF& zone_VEF = zone_vef();
  const DoubleTab& coord = zone_geom.domaine().coord_sommets();
  const IntTab& sommet_poly = zone_geom.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const DoubleTab& ch = cha.valeurs();
  val = 0;

  if (le_poly != -1)
    {
      const double xs = position(0), ys = position(1),
                   zs = (D == 3) ? position(2) : 0;
      for (int i = 0; i < D + 1; i++)
        {
          const int f = elem_faces(le_poly, i);
          for(int n = 0; n < N; n++)
            val(n) += ch(f, n) * ((D == 2) ? fonction_forme_2D(xs, ys, le_poly, i, sommet_poly, coord) : fonction_forme_3D(xs, ys, zs, le_poly, i, sommet_poly, coord));
        }
    }
  return val;
}

double Champ_P1NC_implementation::
valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const
{
  //Cerr << "Champ_P1NC_implementation::valeur_a_elem_compo" << finl;
  const Champ_base& cha=le_champ();
  const Zone& zone_geom = zone();
  const int D = Objet_U::dimension;
  const Zone_VEF& zone_VEF = zone_vef();
  const DoubleTab& coord = zone_geom.domaine().coord_sommets();
  const IntTab& sommet_poly = zone_geom.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const DoubleTab& ch = cha.valeurs();
  double val = 0.;

  if (le_poly != -1)
    {
      // Calcul d'apres les fonctions de forme sur le triangle ou le tetraedre
      const double xs = position(0), ys = position(1),
                   zs = (D == 3) ? position(2) : 0;
      for (int i = 0; i < D + 1; i++)
        {
          const int f = elem_faces(le_poly,i);
          val += ch(f, ncomp) * ((D == 2) ? fonction_forme_2D(xs, ys, le_poly, i, sommet_poly, coord) : fonction_forme_3D(xs, ys, zs, le_poly, i, sommet_poly, coord));
        }
    }

  return val;
}


DoubleTab& Champ_P1NC_implementation::
valeur_aux_elems(const DoubleTab& positions,
                 const IntVect& les_polys,
                 DoubleTab& val) const
{
  const Champ_base& cha=le_champ();
  int nb_compo_=cha.nb_comp();
  int face;
  double xs,ys,zs;
  const DoubleTab& ch = cha.valeurs();

  const Zone_VEF& zone_VEF = zone_vef();
  const Zone& zone_geom = zone();
  const DoubleTab& coord = zone_geom.domaine().coord_sommets();
  const IntTab& sommet_poly = zone_geom.les_elems();
  if (val.nb_dim() > 2)
    {
      Cerr << "Erreur TRUST dans Champ_P1NC_implementation::valeur_aux_elems()\n";
      Cerr << "Le DoubleTab val a plus de 2 entrees\n";
      Process::exit();
    }

  // TODO : FIXME
  // For FT the resize should be done in its good position and not here ...
  if (val.nb_dim() == 1 ) val.resize(val.dimension_tot(0),1);

  int le_poly, D = Objet_U::dimension;
  const IntTab& elem_faces=zone_VEF.elem_faces();

  for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
    {
      le_poly=les_polys(rang_poly);
      if (le_poly == -1)
        for(int ncomp=0; ncomp<nb_compo_; ncomp++)
          val(rang_poly, ncomp) = 0;
      else
        {
          for(int ncomp=0; ncomp<nb_compo_; ncomp++)
            {
              val(rang_poly, ncomp) = 0;
              xs = positions(rang_poly,0);
              ys = positions(rang_poly,1);
              zs = (D == 3) ? positions(rang_poly,2) : 0;
              for (int i = 0; i < D + 1; i++)
                {
                  face = elem_faces(le_poly, i);
                  val(rang_poly, ncomp) += ch(face, ncomp) *
                                           ((D == 2) ? fonction_forme_2D(xs, ys, le_poly, i, sommet_poly, coord) : fonction_forme_3D(xs, ys, zs, le_poly, i, sommet_poly, coord));
                }
            }
        }
    }
  return val;
}

DoubleVect& Champ_P1NC_implementation::
valeur_aux_elems_compo(const DoubleTab& positions,
                       const IntVect& les_polys,
                       DoubleVect& val,int ncomp) const
{
  int les_polys_size = les_polys.size();
  const Champ_base& cha=le_champ();
  double xs,ys,zs;
  int face;
  const Zone_VEF& zone_VEF = zone_vef();
  const Zone& zone_geom = zone();
  const DoubleTab& coord = zone_geom.domaine().coord_sommets();
  const IntTab& sommet_poly = zone_geom.les_elems();
  // Commenter en attendant de comprendre le dimensionnement de xp apres la sortie de version
  // assert(val.size() == les_polys_size);
  int le_poly, D = Objet_U::dimension;

  const DoubleTab& ch = cha.valeurs();

  for(int rang_poly=0; rang_poly<les_polys_size; rang_poly++)
    {
      le_poly=les_polys(rang_poly);
      if (le_poly == -1) val(rang_poly) = 0;
      else
        {
          // Calcul d'apres les fonctions de forme sur le triangle ou le tetraedre
          val(rang_poly) = 0;
          xs = positions(rang_poly,0);
          ys = positions(rang_poly,1);
          zs = (D == 3) ? positions(rang_poly,2) : 0.;
          for (int i=0; i < D + 1; i++)
            {
              face = zone_VEF.elem_faces(le_poly,i);
              val(rang_poly) += ch(face, ncomp) *
                                ( (D == 2) ? fonction_forme_2D(xs,ys,le_poly,i,sommet_poly,coord) :
                                  fonction_forme_3D(xs,ys,zs,le_poly,i,sommet_poly,coord));
            }
        }
    }

  return val;
}


DoubleTab& Champ_P1NC_implementation::
valeur_aux_elems_smooth(const DoubleTab& positions,
                        const IntVect& les_polys,
                        DoubleTab& val)
{
  if ((!sub_type(Champ_P1NC,le_champ()))&&(!sub_type(Champ_Fonc_P1NC,le_champ())))
    {
      Cerr << "L'option chsom des sondes ne s'applique pour le moment que pour les Champ_P1NC en VEF" << finl;
      Cerr << "or votre champ " << le_champ().le_nom() << " est de type " << le_champ().que_suis_je() << "." << finl;
      //        Je rajoute le message ci-dessous car le message habituel n'apparait pas !
      Cerr << "TRUST a provoque une erreur et va s'arreter" << finl;
      Process::exit();
    }

  Champ_base& cha =ref_cast(Champ_base,le_champ());
  const int les_polys_size = les_polys.size(), nb_compo_=cha.nb_comp(), D = Objet_U::dimension;
  const DoubleTab&  ch_sommet= (ch_som());
  const Zone_VEF& zone_VEF = zone_vef();
  const Zone& zone_geom = zone();
  const Domaine& dom=zone_VEF.zone().domaine();
  const DoubleTab& coord = zone_geom.domaine().coord_sommets();
  const IntTab& sommet_poly = zone_geom.les_elems();
  if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys_size)||(val.dimension_tot(0) == les_polys_size));
      assert(val.line_size() == nb_compo_);
    }
  else
    {
      Cerr << "Erreur TRUST dans Champ_P1NC_implementation::valeur_aux_elems_smooth()\n";
      Cerr << "Le DoubleTab val a plus de 2 entrees\n";
      Process::exit();
    }
  if (!filtrer_L2_deja_appele_)
    {
      // Filtrer L2 ne marche que pour les vecteurs
      // C.MALOD 19/12/2006 : Ce n'est plus vrai, ca marche aussi avec les scalaires.
      if (nb_compo_>0)
        {
          DoubleTab val_sauv=cha.valeurs();
          filtrer_L2(cha.valeurs());
          cha.valeurs()=val_sauv;
        }
      filtrer_L2_deja_appele_=1;
    }

  // calcul de la valeur aux elements suivant les coordonnees barycentriques (repris de Champ_P1)
  val = 0.;
  int p;
  for(int rang_poly=0; rang_poly<les_polys_size; rang_poly++)
    if ((p = les_polys(rang_poly)) != -1)
      {
        const double xs = positions(rang_poly,0), ys = positions(rang_poly,1),
                     zs = (D == 3) ? positions(rang_poly,2) : 0.;
        for(int ncomp=0; ncomp<nb_compo_; ncomp++)
          for (int i = 0; i < D + 1; i++)
            {
              int som = sommet_poly(p, i);
              int som1=dom.get_renum_som_perio(som);
              val(rang_poly, ncomp) += ch_sommet(som1, ncomp) * ((D == 2) ? coord_barycentrique(sommet_poly, coord, xs, ys, p, i) : coord_barycentrique(sommet_poly, coord, xs, ys, zs, p, i));
            }
      }
  return val;
}


DoubleVect& Champ_P1NC_implementation::
valeur_aux_elems_compo_smooth(const DoubleTab& positions,
                              const IntVect& les_polys,
                              DoubleVect& val,int ncomp)
{
  if ((!sub_type(Champ_P1NC,le_champ()))&&(!sub_type(Champ_Fonc_P1NC,le_champ())))
    {
      Cerr << "L'option chsom des sondes ne s'applique pour le moment que pour les Champ_P1NC en VEF" << finl;
      Cerr << "or votre champ " << le_champ().le_nom() << " est de type " << le_champ().que_suis_je() << "." << finl;
      //        Je rajoute le message ci-dessous car le message habituel n'apparait pas !
      Cerr << "TRUST a provoque une erreur et va s'arreter" << finl;
      Process::exit();
    }
  Champ_base& cha =le_champ();
  const int les_polys_size = les_polys.size(), nb_compo_=cha.nb_comp(), D = Objet_U::dimension;
  const DoubleTab& ch_sommet= (ch_som());
  const Zone_VEF& zone_VEF = zone_vef();
  const Domaine& dom=zone_VEF.zone().domaine();
  const Zone& zone_geom = zone();
  const DoubleTab& coord = zone_geom.domaine().coord_sommets();
  const IntTab& sommet_poly = zone_geom.les_elems();
  if (!filtrer_L2_deja_appele_)
    {
      // Filtrer L2 ne marche que pour les vecteurs
      // C.MALOD 19/12/2006 : Ce n'est plus vrai, ca marche aussi avec les scalaires.
      if (nb_compo_>0)
        {
          DoubleTab val_sauv=cha.valeurs();
          filtrer_L2(cha.valeurs());
          cha.valeurs()=val_sauv;
        }
      filtrer_L2_deja_appele_=1;
    }
  assert(val.size() == les_polys_size);
  val = 0.;
  int p;

  // calcul de la valeur aux elements suivant les coordonnees barycentriques (repris de Champ_P1)

  for(int rang_poly=0; rang_poly<les_polys_size; rang_poly++)
    if ((p = les_polys(rang_poly)) != -1)
      {
        const double xs = positions(rang_poly,0), ys = positions(rang_poly,1),
                     zs = (D == 3) ? positions(rang_poly,2) : 0.;
        for (int i = 0; i < D + 1; i++)
          {
            int som = sommet_poly(p, i);
            int som1=dom.get_renum_som_perio(som);
            val(rang_poly) += ch_sommet(som1, ncomp)
                              * ((D == 2) ? coord_barycentrique(sommet_poly, coord, xs, ys, p, i) : coord_barycentrique(sommet_poly, coord, xs, ys, zs, p, i));
          }
      }
  return val;
}

DoubleTab& Champ_P1NC_implementation::
valeur_aux_sommets(const Domaine& dom,
                   DoubleTab& champ_som) const
{
  const Zone& mazone = dom.zone(0);

  int nb_som = mazone.nb_som_tot();
  int nb_som_return = champ_som.dimension_tot(0);

  const Champ_base& cha=le_champ();
  const DoubleTab& ch = cha.valeurs();
  int nb_compo_=cha.nb_comp();
  ArrOfInt compteur(nb_som);

  const Zone_VEF& zone_VEF = zone_vef();
  assert(zone_vef().zone()==mazone);

  if ( methode_calcul_valeurs_sommets>0)
    {
      int nb_elem_tot = mazone.nb_elem_tot();
      int nb_som_elem = mazone.nb_som_elem();

      const IntTab& elem_faces = zone_VEF.elem_faces();

      DoubleTab min_som(nb_som,nb_compo_) ;
      DoubleTab max_som(nb_som,nb_compo_) ;
      int ncomp, face_adj,face_glob;
      int num_elem,num_som,j;

      champ_som = 0;
      compteur = 0;
      for (j=0; j<nb_som; j++)
        for (int k = 0; k < nb_compo_; k++ )
          {
            min_som(j,k) = 1.e+30 ;
            max_som(j,k) = 1.e-30 ;
          }

      for (num_elem=0; num_elem<nb_elem_tot; num_elem++)
        {
          for (j=0; j<nb_som_elem; j++)
            {
              num_som = mazone.sommet_elem(num_elem,j);
              if(num_som < nb_som_return)
                {
                  compteur[num_som]++;
                  for (ncomp=0; ncomp<nb_compo_; ncomp++)
                    {
                      champ_som(num_som,ncomp) += valeur_a_sommet_compo(num_som,num_elem,ncomp);
                      for(face_adj=0; face_adj<nb_som_elem; face_adj ++)
                        {
                          if (face_adj != j )
                            {
                              face_glob = elem_faces(num_elem, face_adj);
                              min_som(num_som,ncomp) = std::min ( ch(face_glob,ncomp),min_som(num_som,ncomp) ) ;
                              max_som(num_som,ncomp) = std::max ( ch(face_glob,ncomp),max_som(num_som,ncomp) ) ;
                            }
                        }
                    }
                }
            }
        }
      for (num_som=0; num_som<nb_som_return; num_som++)
        for (ncomp=0; ncomp<nb_compo_; ncomp++)
          {
            champ_som(num_som,ncomp) /= compteur[num_som];
            if ( methode_calcul_valeurs_sommets>1)
              {
                if ( champ_som(num_som,ncomp) < min_som(num_som,ncomp) )
                  {
                    champ_som(num_som,ncomp) = min_som(num_som,ncomp) ;
                  }
                if ( champ_som(num_som,ncomp) > max_som(num_som,ncomp) )
                  {
                    champ_som(num_som,ncomp) = max_som(num_som,ncomp) ;
                  }
              }
          }
    }
  else
    {
      assert(methode_calcul_valeurs_sommets==-1);
      champ_som = 0;
      compteur = 0;
      int nb_comp=nb_compo_;
      int nb_faces_tot=zone_VEF.nb_faces_tot();
      int nb_som_face=zone_VEF.nb_som_face();
      const IntTab& face_sommets=zone_VEF.face_sommets();
      int face;
      for(face=0; face<nb_faces_tot; face++)
        {

          for(int isom=0; isom<nb_som_face; isom++)
            {
              int som1=face_sommets(face,isom);
              if (som1<nb_som_return)
                {
                  compteur[som1]++;
                  for(int k=0; k<nb_comp; k++)
                    champ_som(som1,k)+=ch(face,k);
                }
            }
        }


      for (int num_som=0; num_som<nb_som_return; num_som++)
        for (int ncomp=0; ncomp<nb_compo_; ncomp++)
          {
            champ_som(num_som,ncomp) /= compteur[num_som];
          }
    }
  return champ_som;
}

double Champ_P1NC_implementation::valeur_a_sommet_compo(int num_som, int le_poly, int ncomp) const
{
  //Cerr << "Champ_P1NC_implementation::valeur_a_sommet_compo" << finl;
  int face=-1;
  const IntTab& elem_faces = zone_vef().elem_faces();
  const IntTab& sommet_poly = zone().les_elems();
  const DoubleTab& ch = le_champ().valeurs();
  double val=0;
  if (le_poly != -1)
    {
      for (int i=0; i< Objet_U::dimension+1; i++)
        {
          face = elem_faces(le_poly,i);
          if (sommet_poly(le_poly,i)==num_som)
            val -= (Objet_U::dimension-1)*ch(face, ncomp);
          else
            val += ch(face, ncomp);
        }
    }
  return val;
}

//DoubleTab& Champ_P1NC_implementation::
//valeur_aux_sommets(const Domaine& dom,
//                   DoubleTab& ch_som) const
//{
//  //Cerr << "Champ_P1NC_implementation::valeur_aux_sommets" << finl;
//  Champ_P1NC& ch=ref_cast(Champ_P1NC, le_champ());
//  valeur_P1_L2(ch, dom);
//  DoubleTab& champ_sommet=ch.ch_som();
//  if (champ_sommet.nb_dim()==1)
//  {
//          for (int k=0;k<champ_sommet.dimension(0);k++)
//           ch_som(k,0)=champ_sommet(k);
//  }
//  else
//  {
//          for (int k=0;k<champ_sommet.dimension(0);k++)
//           for (int ncomp=0;ncomp<champ_sommet.dimension(1);ncomp++)
//           ch_som(k,ncomp)=champ_sommet(k,ncomp);
//  }
//  return ch_som;
//}

DoubleVect& Champ_P1NC_implementation::
valeur_aux_sommets_compo(const Domaine& dom,
                         DoubleVect& champ_som,
                         int ncomp) const
{
  const Zone_dis_base& zone_dis = zone_dis_base();
  const Zone& mazone = zone_dis.zone();
  const DoubleTab& ch = le_champ().valeurs();

  const Zone_VEF& zone_VEF = zone_vef();


  int nb_som = mazone.nb_som();

  IntVect compteur(nb_som);
  if (methode_calcul_valeurs_sommets>0)
    {
      int nb_elem_tot = mazone.nb_elem_tot();
      int nb_som_elem = mazone.nb_som_elem();
      int num_elem,num_som,j;
      champ_som = 0;
      compteur = 0;
      const IntTab& elem_faces = zone_VEF.elem_faces();


      DoubleVect min_som(nb_som) ;
      DoubleVect max_som(nb_som) ;
      int face_adj,face_glob;

      for (j=0; j<nb_som; j++)
        {
          min_som(j) = 1.e+30 ;
          max_som(j) = 1.e-30 ;
        }

      for (num_elem=0; num_elem<nb_elem_tot; num_elem++)
        {
          for (j=0; j<nb_som_elem; j++)
            {
              num_som = mazone.sommet_elem(num_elem,j);
              if(num_som < nb_som)
                {
                  compteur[num_som]++;

                  champ_som(num_som) += valeur_a_sommet_compo(num_som,num_elem,ncomp);

                  for(face_adj=0; face_adj<nb_som_elem; face_adj ++)
                    {
                      if (face_adj != j )
                        {
                          face_glob = elem_faces(num_elem, face_adj);
                          min_som(num_som) = std::min
                                             ( ch(face_glob,ncomp),min_som(num_som) ) ;
                          max_som(num_som) = std::max
                                             ( ch(face_glob,ncomp),max_som(num_som) ) ;
                        }
                    }

                }
            }
        }

      for (num_som=0; num_som<nb_som; num_som++)
        {
          champ_som(num_som) /= compteur[num_som];
          if (methode_calcul_valeurs_sommets>1)
            {
              if ( champ_som(num_som) < min_som(num_som) ) champ_som(num_som) = min_som(num_som) ;
              if ( champ_som(num_som) > max_som(num_som) ) champ_som(num_som) = max_som(num_som) ;
            }

        }
    }
  else
    {
      champ_som = 0;
      compteur = 0;
      int nb_faces_tot=zone_VEF.nb_faces_tot();
      int nb_som_face=zone_VEF.nb_som_face();
      const IntTab& face_sommets=zone_VEF.face_sommets();
      int face;
      for(face=0; face<nb_faces_tot; face++)
        {

          for(int isom=0; isom<nb_som_face; isom++)
            {
              int som1=face_sommets(face,isom);
              if (som1<nb_som)
                {
                  compteur[som1]++;
                  champ_som(som1)+=ch(face,ncomp);
                }
            }
        }


      for (int num_som=0; num_som<nb_som; num_som++)
        {
          champ_som(num_som) /= compteur[num_som];
        }

    }
  return champ_som;
}

DoubleTab& Champ_P1NC_implementation::remplir_coord_noeuds(DoubleTab& noeuds) const
{
  const Zone_VEF& zvef = zone_vef();
  const DoubleTab& xv = zvef.xv();
  int nb_fac = zvef.nb_faces();
  if ( (xv.dimension(0) == nb_fac ) && (xv.dimension(1) == Objet_U::dimension) )
    noeuds.ref(xv);
  else
    {
      Cerr << "Erreur dans Champ_P1NC_implementation::remplir_coord_noeuds()" << finl;
      Cerr << "Les centres de gravite des faces n'ont pas ete calcules" << finl;
      Process::exit();
    }
  return noeuds;
}


int Champ_P1NC_implementation::remplir_coord_noeuds_et_polys(DoubleTab& positions,
                                                             IntVect& polys) const
{
  remplir_coord_noeuds(positions);
  const Zone_VEF& zvef=zone_vef();
  const IntTab& face_voisins=zvef.face_voisins();
  int nb_faces=zvef.nb_faces();
  polys.resize(nb_faces);

  for(int i= 0; i< nb_faces; i++)
    {
      polys(i)=face_voisins(i, 0);
    }
  return 0;
}

int Champ_P1NC_implementation::imprime_P1NC(Sortie& os, int ncomp) const
{
  const Zone_VEF& zone_VEF=zone_vef();
  const DoubleTab& pos_face=zone_VEF.xv();
  const int nb_faces = zone_VEF.nb_faces();
  const Champ_base& cha=le_champ();
  const DoubleTab& val = cha.valeurs();
  int fac;
  os << nb_faces << finl;
  for (fac=0; fac<nb_faces; fac++)
    {
      if (Objet_U::dimension==3)
        os << pos_face(fac,0) << " " << pos_face(fac,1) << " " << pos_face(fac,2) << " " ;
      if (Objet_U::dimension==2)
        os << pos_face(fac,0) << " " << pos_face(fac,1) << " " ;
      os << val(fac,ncomp) << finl;
    }
  os << finl;
  Cout << "Champ_P1NC_implementation::imprime_P1NC FIN >>>>>>>>>> " << finl;
  return 1;
}

int Champ_P1NC_implementation::fixer_nb_valeurs_nodales(int n)
{
  ch_som_.reset();
  const int nb_dim = le_champ().valeurs().nb_dim();
  if (nb_dim > 1)
    ch_som_.resize(0, le_champ().nb_comp());
  const Domaine& dom = zone_vef().zone().domaine();
  dom.creer_tableau_sommets(ch_som_);
  ch_som_vect_.reset();
  dom.creer_tableau_sommets(ch_som_vect_);
  return n;
}
// Ajout de methodes pour le filtrage L2 en //
int Champ_P1NC_implementation::nb_colonnes_tot()
{
  if (sub_type(Champ_P1NC,le_champ()))
    {
      Champ_P1NC& cha =ref_cast(Champ_P1NC,le_champ());

      return cha.ch_som_vect().size_totale();
    }
  else if (sub_type(Champ_Fonc_P1NC,le_champ()))
    {
      Champ_Fonc_P1NC& cha = ref_cast(Champ_Fonc_P1NC,le_champ());
      return cha.ch_som_vect().size_totale();
    }

  return 0;
}

int Champ_P1NC_implementation::nb_colonnes()
{
  if (sub_type(Champ_P1NC,le_champ()))
    {
      Champ_P1NC& cha =ref_cast(Champ_P1NC,le_champ());
      return cha.ch_som_vect().size_reelle();
    }
  else if (sub_type(Champ_Fonc_P1NC,le_champ()))
    {
      Champ_Fonc_P1NC& cha =ref_cast(Champ_Fonc_P1NC,le_champ());
      return cha.ch_som_vect().size_reelle();
    }

  return 0;
}


void Champ_P1NC_implementation::dimensionner_Mat_Bloc_Morse_Sym(Matrice& matrice_tmp)
{
  // modif specific au Champ_P1NC_implementation
  const Matrice_Morse_Sym& la_matrice = get_MatP1NC2P1_L2();
  //
  int n1 = nb_colonnes_tot();
  int n2 = nb_colonnes();
  int iligne;
  const IntVect& tab1=la_matrice.get_tab1();
  const IntVect& tab2=la_matrice.get_tab2();

  matrice_tmp.typer("Matrice_Bloc");
  Matrice_Bloc& matrice=ref_cast(Matrice_Bloc,matrice_tmp.valeur());
  matrice.dimensionner(1,2);
  matrice.get_bloc(0,0).typer("Matrice_Morse_Sym");
  matrice.get_bloc(0,1).typer("Matrice_Morse");

  Matrice_Morse_Sym& MBrr =  ref_cast(Matrice_Morse_Sym,matrice.get_bloc(0,0).valeur());
  Matrice_Morse& MBrv =  ref_cast(Matrice_Morse,matrice.get_bloc(0,1).valeur());
  MBrr.dimensionner(n2,0);
  MBrv.dimensionner(n2,0);

  IntVect& tab1RR=MBrr.get_set_tab1();
  IntVect& tab2RR=MBrr.get_set_tab2();
  IntVect& tab1RV=MBrv.get_set_tab1();
  IntVect& tab2RV=MBrv.get_set_tab2();

  IntVect compteur_MBrr(n2);
  IntVect compteur_MBrv(n2);
  compteur_MBrr=0;
  compteur_MBrv=0;

  // On parcours les lignes de la_matrice pour compter les elements
  // non nuls de chaque ligne
  int jcolonne;
  for (iligne=0; iligne<n2; iligne++)
    {
      int k;
      for ( k=tab1(iligne)-1; k<tab1(iligne+1)-1; k++)
        {
          jcolonne = tab2(k)-1;
          if (jcolonne < n2)
            {
              // l'element correspondant est dans la partie RR de la_matrice
              if ((jcolonne >= iligne) && (jcolonne < n2))
                {
                  // l'element correspondant est  situe au dessus de la diagonale de la_matrice
                  compteur_MBrr(iligne)++;
                }
            }
          else
            {
              // l'element correspondant est dans la partie RV de la_matrice
              compteur_MBrv(iligne)++;
            }
        }
    }


  // On remplie tab1RR et tab1RV
  tab1RR(0)=1;
  tab1RV(0)=1;
  for(int i=0; i<n2; i++)
    {
      tab1RR(i+1)=compteur_MBrr(i)+tab1RR(i);
      tab1RV(i+1)=compteur_MBrv(i)+tab1RV(i);
    }
  // On dimensionne tab2RR et tab2RV
  MBrr.dimensionner(n2,tab1RR(n2)-1);
  MBrv.dimensionner(n2,n1-n2,tab1RV(n2)-1);

  // On remplit tab2RR et tab2RV
  int compteurRR,compteurRV;
  for (iligne=0; iligne<n2; iligne++)
    {
      int k;
      compteurRR = tab1RR(iligne)-1;
      compteurRV = tab1RV(iligne)-1;
      for ( k=tab1(iligne)-1; k<tab1(iligne+1)-1; k++)
        {
          jcolonne = tab2(k)-1;
          if (jcolonne < n2)
            {
              // l'element correspondant est dans la partie RR de la_matrice
              if ((jcolonne >= iligne) && (jcolonne < n2))
                {
                  // l'element correspondant est  situe au dessus de la diagonale de la_matrice
                  tab2RR(compteurRR) = tab2(k);
                  compteurRR++;
                }
            }
          else
            {
              // l'element correspondant est dans la partie RV de la_matrice
              tab2RV(compteurRV) = tab2(k)-n2;
              compteurRV++;
            }
        }
    }
}

void Champ_P1NC_implementation::Mat_Morse_to_Mat_Bloc(Matrice& matrice_tmp)
{
  //
  const Matrice_Morse_Sym& la_matrice = get_MatP1NC2P1_L2();
  //

  int n1 = nb_colonnes_tot();
  int n2 = nb_colonnes();

  Matrice_Bloc& matrice= ref_cast(Matrice_Bloc,matrice_tmp.valeur());
  Matrice_Morse& MBrr =  ref_cast(Matrice_Morse,matrice.get_bloc(0,0).valeur());
  Matrice_Morse& MBrv =  ref_cast(Matrice_Morse,matrice.get_bloc(0,1).valeur());

  IntVect& tab1RR=MBrr.get_set_tab1();
  IntVect& tab2RR=MBrr.get_set_tab2();
  DoubleVect& coeffRR=MBrr.get_set_coeff();
  IntVect& tab1RV=MBrv.get_set_tab1();
  IntVect& tab2RV=MBrv.get_set_tab2();
  DoubleVect& coeffRV=MBrv.get_set_coeff();

  DoubleTab ligne_tmp(n1);
  for(int i=0; i<n2; i++)
    {
      int k;
      // On recopie le premier bloc de la matrice dans un tableau :
      //      ligne_tmp = 0;
      for ( k=la_matrice.get_tab1()(i)-1; k<la_matrice.get_tab1()(i+1)-1; k++)
        ligne_tmp(la_matrice.get_tab2()(k) - 1) = la_matrice.get_coeff()(k);

      // On complete la partie reelle de la matrice
      for ( k=tab1RR(i)-1; k<tab1RR(i+1)-1; k++)
        coeffRR[k] = ligne_tmp(tab2RR[k] - 1);

      // On complete la partie virtuelle
      for ( k=tab1RV(i)-1; k<tab1RV(i+1)-1; k++)
        coeffRV[k] = ligne_tmp(n2 + tab2RV[k] - 1);
    }


  /*  Cerr<<"Impression de la_matrice"<<finl;
      la_matrice.imprimer_formatte(Cerr);
      Cerr<<"Impression de MBrr"<<finl;
      MBrr.imprimer_formatte(Cerr);
      Cerr<<"Impression de MBrv"<<finl;
      MBrv.imprimer_formatte(Cerr);*/

  /*  Debog::verifier_Mat_faces("avant resolution systeme : la_matrice",la_matrice);
      Debog::verifier_Mat_faces("avant resolution systeme : MBrr",MBrr);*/
}

