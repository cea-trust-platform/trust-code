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
// File:        Op_Conv_EF_VEF_P1NC_Stab.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/47
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_EF_VEF_P1NC_Stab.h>
#include <Champ_P1NC.h>
#include <BilanQdmVEF.h>
#include <Schema_Temps_base.h>
#include <Debog.h>
#include <Porosites_champ.h>
#include <Sous_zone_VF.h>
#include <Probleme_base.h>
#include <ArrOfBit.h>
#include <TRUSTVects.h>
#include <SFichier.h>
#include <TRUSTList.h>
#include <TRUSTTabs.h>
#include <stat_counters.h>

Implemente_instanciable(Op_Conv_EF_VEF_P1NC_Stab,"Op_Conv_EF_Stab_VEF_P1NC",Op_Conv_VEF_Face);


//// printOn
//

Sortie& Op_Conv_EF_VEF_P1NC_Stab::printOn(Sortie& s ) const
{
  return s << que_suis_je();
}

//// readOn
//

Entree& Op_Conv_EF_VEF_P1NC_Stab::readOn(Entree& s )
{
  //Les mots a reconnaitre
  Motcle motlu, accouverte = "{" , accfermee = "}" ;
  Motcles les_mots(9);
  {
    les_mots[0] = "alpha";
    les_mots[1] = "test";
    les_mots[2] = "TdivU";
    les_mots[3] = "old";
    les_mots[4] = "volumes_etendus";
    les_mots[5] = "volumes_non_etendus";
    les_mots[6] = "amont_sous_zone";
    les_mots[7] = "nouvelle_matrice_implicite";
    les_mots[8] = "alpha_sous_zone";
  }

  //Les variables a instancier
  alpha_=1.;
  test_=0;
  is_compressible_=0;
  old_=0;
  volumes_etendus_=1;
  sous_zone=false;
  ssz_alpha=false;
  new_jacobienne_=0;
  s >> motlu;
  if (motlu!=accouverte)
    {
      if (Process::je_suis_maitre())
        {
          Cerr << "Erreur Op_Conv_EF_VEF_P1NC_Stab::readOn()" << finl;
          Cerr << "Depuis la 1.5.3, la syntaxe du mot cle EF_stab a change." << finl;
          Cerr << "Il faut commencer par une accolade ouvrante {" << finl;
          Cerr << "et les options eventuelles sont entre les accolades:" << finl;
          Cerr << "Convection { EF_stab } -> Convection { EF_stab { } }" << finl;
        }
      Process::exit();
    }
  s >> motlu;

  while(motlu!=accfermee)
    {
      int rang=les_mots.search(motlu);

      switch(rang)
        {
        case 0 :

          s >> alpha_;
          break;

        case 1 :

          test_=1;
          break;

        case 2 :

          is_compressible_=1;
          break;

        case 3 :

          old_=1;
          break;

        case 4 :

          volumes_etendus_=1;
          break;

        case 5 :

          volumes_etendus_=0;
          break;

        case 6 :
          sous_zone=true;
          s >> nom_sous_zone;
          break;
        case 7 :
          s >> new_jacobienne_;
          break;
        case 8 :
          ssz_alpha=true;
          s >> nb_ssz_alpha;
          noms_ssz_alpha.dimensionner(nb_ssz_alpha);
          alpha_ssz.resize(nb_ssz_alpha);
          for (int i=0; i<nb_ssz_alpha; i++)
            {
              s>>noms_ssz_alpha[i];
              s>>alpha_ssz(i);
            }
          break;
        default :

          Cerr << "Erreur Op_Conv_EF_VEF_P1NC_Stab::readOn()" << finl;
          Cerr << "Mot clef " << motlu << " non connu." << finl;
          Cerr << "Sortie du programme." << finl;
          Process::exit();
          break;

        }//fin du switch

      //Suite de la lecture
      s >> motlu;

    }//fin du while

  return s ;
}


static inline double maximum(const double x,
                             const double y)
{
  if(x<y)
    return y;
  return x;
}

static inline double maximum(const double x,
                             const double y,
                             const double z)
{
  return maximum(maximum(x,y),z);
}

static inline double minimum(const double x,
                             const double y)
{
  if(x>y)
    return y;
  return x;
}


static inline double Dij(int elem,
                         int face_loc_i,
                         int face_loc_j,
                         const DoubleTab& Kij)
{
  const double kij=Kij(elem,face_loc_i,face_loc_j);
  const double kji=Kij(elem,face_loc_j,face_loc_i);
  return maximum(-kij,-kji,0);
}

static inline double limiteur(double r)
{
  if(r<=0) return 0.;
  return maximum(minimum(2,r),minimum(1,2*r));//SuperBee
}

double formule_Id_2D(int n)
{
  return 1./3;
}

double formule_Id_3D(int n)
{
  return 0.25;
}

double formule_2D(int n)
{
  switch(n)
    {
    case 0:
      return 1./3;
      break;
    case 1:
      return 0.5;
      break;
    case 2:
      return 1.;
      break;
    default:
      Cerr << "Erreur Op_Conv_EF_VEF_P1NC_Stab::formule_2D()" << finl;
      Process::exit();
      break;
    }
  return 0.;
}

double formule_3D(int n)
{
  switch(n)
    {
    case 0:
      return 0.25;
      break;
    case 1:
      return 1./3;
      break;
    case 2:
      return 0.5;
      break;
    case 3:
      return 1.;
      break;
    default:
      Cerr << "Erreur Op_Conv_EF_VEF_P1NC_Stab::formule_3D()" << finl;
      Process::exit();
      break;
    }
  return 0.;
}



////////////////////////////////////////////////////////////////////
//
//                      Implementation des fonctions
//
//                   de la classe Op_Conv_EF_VEF_P1NC_Stab
//
////////////////////////////////////////////////////////////////////

void Op_Conv_EF_VEF_P1NC_Stab::reinit_conv_pour_Cl(const DoubleTab& transporte,const IntList& faces, const DoubleTabs& valeurs_faces, const DoubleTab& tab_vitesse, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF=la_zcl_vef.valeur();
  const DoubleTab& face_normales=zone_VEF.face_normales();
  const int nb_bord=zone_Cl_VEF.nb_cond_lim();
  const int nb_comp=transporte.line_size();
  int n_bord=0, num1=0, num2=0, ind_face=0,facei=0, dim=0;
  double psc=0.;

  const DoubleVect& transporteV= transporte;

  //Boucle pour dimensionner la liste "faces"
  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1 = 0;
      num2 = le_bord.nb_faces_tot();

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre
            = ref_cast(Neumann_sortie_libre, la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei = le_bord.num_face(ind_face);

              psc=0.;
              for (dim=0; dim<dimension; dim++)
                psc-=tab_vitesse(facei,dim)*face_normales(facei,dim);

              if (psc>0)
                for (dim=0; dim<nb_comp; dim++)
                  resu(facei,dim)+=psc*(la_sortie_libre.val_ext(facei-num1,dim)-transporteV[facei*nb_comp+dim]);

            }
        }//fin du if sur "Neumann_sortie_libre"
    }
}

void Op_Conv_EF_VEF_P1NC_Stab::calculer_coefficients_operateur_centre(DoubleTab& Kij,const int nb_comp, const DoubleTab& tab_vitesse) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const DoubleTab& face_normales=zone_VEF.face_normales();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.line_size();

  assert(Kij.nb_dim()==3);
  assert(Kij.dimension(0)==nb_elem_tot);
  assert(Kij.dimension(1)==Kij.dimension(2));
  assert(Kij.dimension(1)==nb_faces_elem);

  //
  //Calcul des coefficients de l'operateur
  //
  for(int elem=0; elem<nb_elem_tot; elem++)
    {
      for(int face_loci=0; face_loci<nb_faces_elem; face_loci++)
        {
          int face_i=elem_faces(elem,face_loci);

          double signei=1.0;
          if(face_voisins(face_i,0)!=elem) signei=-1.0;

          double psci=0;
          for(int comp=0; comp<dimension; comp++)
            psci+=tab_vitesse(face_i,comp)*face_normales(face_i,comp);
          psci*=signei;

          //Kij(elem,face_loci,face_loci)=0.;
          for(int face_locj=face_loci+1; face_locj<nb_faces_elem; face_locj++)
            {
              int face_j=elem_faces(elem,face_locj);
              double signej=1.0;
              if(face_voisins(face_j,0)!=elem)
                signej=-1.0;

              double pscj=0;
              //psci=0;
              for(int comp=0; comp<dimension; comp++)
                pscj+=tab_vitesse(face_j,comp)*face_normales(face_j,comp);
              pscj*=signej;

              Kij(elem,face_loci,face_locj)=-1./nb_faces_elem*pscj;
              Kij(elem,face_loci,face_loci)+=1./nb_faces_elem*pscj;
              Kij(elem,face_locj,face_loci)=-1./nb_faces_elem*psci;
              Kij(elem,face_locj,face_locj)+=1./nb_faces_elem*psci;
            }//fin du for sur "face_locj"
        }//fin du for sur "face_loci"
    }//fin du for sur "elem"

  //
  // Correction des Kij pour Dirichlet !
  //
  {
    int nb_bord=zone_Cl_VEF.nb_cond_lim();
    const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();
    for (int n_bord=0; n_bord<nb_bord; n_bord++)
      {
        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        int nb_faces_tot=le_bord.nb_faces_tot();
        int face;

        if ( (sub_type(Dirichlet,la_cl.valeur()))
             || (sub_type(Dirichlet_homogene,la_cl.valeur()))
           )
          {
            // GF on retire le test sur nb_comp car sinon en scalaire on fait tjs du volume etendu
            if ( volumes_etendus_ )
              {
                //
                //Modification des coefficients de la matrice
                //
                for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
                  {
                    face = le_bord.num_face(ind_face);
                    const int elem=face_voisins(face,0);
                    assert(elem!=-1);

                    const int nb_faces_bord = elem_nb_faces_dirichlet_(elem);

                    //
                    //Calcul du coefficient ponderateur
                    //

                    const double coeff = (Objet_U::dimension==2) ?
                                         nb_faces_bord/2. : nb_faces_bord*nb_faces_bord/6.-nb_faces_bord/3.+1./2;

                    //
                    //Fin du calcul du coefficient ponderateur
                    //

                    for (int face_loc_i=0; face_loc_i<nb_faces_elem; face_loc_i++)
                      {
                        const int face_i = elem_faces(elem,face_loc_i);

                        /* On determine si "face_i" est une face de Dirichlet */
                        bool is_not_on_boundary = true;
                        for (int f_loc=0; f_loc<nb_faces_bord; f_loc++)
                          is_not_on_boundary&=(face_i!=elem_faces_dirichlet_(elem,f_loc));

                        if (is_not_on_boundary) /* si "face_i" n'est pas de Dirichlet */
                          {
                            for (int face_loc_j=0; face_loc_j<nb_faces_elem; face_loc_j++)
                              {
                                /* Modification des coefficients de la matrice */
                                /* selon les fonctions de formes etendues */
                                for (int f_loc=0; f_loc<nb_faces_bord; f_loc++)
                                  {
                                    const int face_bord = elem_faces_dirichlet_(elem,f_loc);
                                    const int face_loc_k = num_fac_loc(face_bord,0);
                                    assert(face_loc_k>=0);
                                    assert(face_loc_k<nb_faces_elem);

                                    double& kij = Kij(elem,face_loc_i,face_loc_j);
                                    const double kkj = Kij(elem,face_loc_k,face_loc_j);
                                    kij+=coeff*kkj;

                                  }//fin du for sur "f_loc"

                              }//fin du for sur "face_loc_k"

                          }//fin du if

                      }//fin du for sur "face_loc_i"


                    //
                    //Remise a zero des coefficients associes aux noeuds
                    //qui se situent sur la frontiere de Dirichlet
                    //
                    {
                      for (int f_loc=0; f_loc<nb_faces_bord; f_loc++)
                        {
                          const int face_bord = elem_faces_dirichlet_(elem,f_loc);
                          const int face_loc_j = num_fac_loc(face_bord,0);
                          assert(face_loc_j>=0);
                          assert(face_loc_j<nb_faces_elem);

                          for (int face_loc_i=0; face_loc_i<nb_faces_elem; face_loc_i++)
                            Kij(elem,face_loc_j,face_loc_i)=0;
                        }//fin du for sur "f_loc"
                    }
                    //
                    //Fin de la remise a zero
                    //

                    //
                    //Pour retrouver le schema LED
                    //
                    {
                      for (int face_loc_i=0; face_loc_i<nb_faces_elem; face_loc_i++)
                        {
                          double sum=0.;
                          for (int face_loc_k=0; face_loc_k<nb_faces_elem; face_loc_k++)
                            {
                              sum+=Kij(elem,face_loc_i,face_loc_k);
                            }
                          //Cerr << "somme apres : " << sum << finl;
                          Kij(elem,face_loc_i,face_loc_i)-=sum;//car div(u)=0!
                        }
                    }
                    //
                    //Fin du schema LED
                    //

                  }// for face
                //
                //Fin de la modification des coefficients de la matrice
                //

              }//fin du if sur "volumes_etendus_"

          }// sub_type Dirichlet


        else if (sub_type(Neumann,la_cl.valeur()) || sub_type(Neumann_homogene,la_cl.valeur()) || sub_type(Neumann_val_ext,la_cl.valeur()))
          {
            //On ne fait rien
          }//fin du if sur Neumann

        else if (sub_type(Symetrie,la_cl.valeur()))
          {
            //On ne fait rien
          }//fin du if sur Symetrie

        else if (sub_type(Periodique,la_cl.valeur()))
          {
            //On ne fait rien
          }//fin du if sur Periodique

        else if (sub_type(Echange_impose_base,la_cl.valeur()))
          {
            //On ne fait rien
          }//fin du if sur Echange_impose_base

        else
          {
            Cerr << "Erreur Op_Conv_EF_VEF_P1NC_Stab::calculer_coefficients_operateur_centre()" << finl;
            Cerr << "Condition aux limites " << la_cl.que_suis_je() << " non codee."   << finl;
            Cerr << "Sortie du programme." << finl;
            Process::exit();
          }//fin du else sur les autres conditions aux limites

      }//fin des conditions aux limites
  }

  //
  // Fin de la correction des Kij
  //
}
void Op_Conv_EF_VEF_P1NC_Stab::remplir_fluent(DoubleVect& fluent_) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const DoubleTab& tab_vitesse=la_vitesse.valeurs();
  const DoubleTab& face_normales=zone_VEF.face_normales();
  const int nb_faces = zone_VEF.nb_faces();
  // calcul de la CFL.
  double psc;
  // On remet a zero le tableau qui sert pour
  // le calcul du pas de temps de stabilite
  fluent_ = 0;

  for(int num_face=0; num_face<nb_faces; num_face++)
    {
      psc=0.;
      for (int i=0; i<dimension; i++)
        psc+=tab_vitesse(num_face,i)*face_normales(num_face,i);
      fluent_(num_face)=std::fabs(psc);
    }
}


DoubleTab& Op_Conv_EF_VEF_P1NC_Stab::ajouter(const DoubleTab& transporte_2,
                                             DoubleTab& resu) const
{
  DoubleTab sauv(resu);
  resu=0;

  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_P1NC& la_vitesse=ref_cast( Champ_P1NC, vitesse_.valeur());
  const DoubleTab& vitesse_2=la_vitesse.valeurs();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();

  const IntTab& elem_faces = zone_VEF.elem_faces();

  const int marq=phi_u_transportant(equation());
  const int nb_faces_elem=elem_faces.dimension(1);
  assert(nb_faces_elem==(dimension+1));
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  int nb_comp=resu.line_size();

  DoubleTab transporte_;
  DoubleTab vitesse_face_;
  // soit on a transporte=phi*transporte_ et vitesse=vitesse_
  // soit transporte=transporte_ et vitesse=phi*vitesse_
  // cela depend si on transporte avec phi u ou avec u.
  const DoubleTab& tab_vitesse=modif_par_porosite_si_flag(vitesse_2,vitesse_face_,marq,porosite_face);

  DoubleTab Kij(nb_elem_tot,nb_faces_elem,nb_faces_elem);

  //Pour tenir compte des conditions de Neumann sortie libre
  IntList NeumannFaces;
  DoubleTabs ValeursNeumannFaces;

  // soit on a transporte=phi*transporte_ et vitesse=vitesse_
  // soit transporte=transporte_ et vitesse=phi*vitesse_
  // cela depend si on transporte avec phi u ou avec u.
  const DoubleTab& transporte=modif_par_porosite_si_flag(transporte_2,transporte_,!marq,porosite_face);

  //Initialisation du tableau flux_bords_ pour le calcul des pertes de charge
  flux_bords_.resize(zone_VEF.nb_faces_bord(),nb_comp);
  calculer_flux_bords(Kij,tab_vitesse,transporte);

  if (!old_)
    {
      //statistiques().begin_count(m1);
      calculer_coefficients_operateur_centre(Kij,nb_comp,tab_vitesse);
      //statistiques().end_count(m1);
      //statistiques().begin_count(m2);
      if (is_compressible_) ajouter_partie_compressible(transporte,resu,tab_vitesse);

      ajouter_operateur_centre(Kij,transporte,resu);

      ajouter_diffusion(Kij,transporte,resu);
      //statistiques().end_count(m2);
      //statistiques().begin_count(m3);
      ajouter_antidiffusion(Kij,transporte,resu);
      //statistiques().end_count(m3);
      mettre_a_jour_pour_periodicite(resu);

    }
  else
    {
      assert(old_==1);
      ajouter_old(transporte,resu,tab_vitesse);
    }

  reinit_conv_pour_Cl(transporte,NeumannFaces,ValeursNeumannFaces,tab_vitesse,resu);

  resu+=sauv;

  if (test_) test(transporte,resu,tab_vitesse);
  modifier_flux(*this);

  return resu;
}

//Correction pour le poreux : on rajoute la partie en  T div(u)
//Variable transportee : T
//Variable transportante : u
//REMARQUE : il ne FAUT SURTOUT PAS utiliser le tableau Kij car par
//construction celui-ci est telle que sum_{j} Kij =0 ce qui revient a
//imposer une vitesse a divergence nulle par element. Ce qui est
//problematique quand on est en compressible
DoubleTab& Op_Conv_EF_VEF_P1NC_Stab::ajouter_partie_compressible(const DoubleTab& transporte,
                                                                 DoubleTab& resu, const DoubleTab& vitesse_2) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normales=zone_VEF.face_normales();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.line_size();

  //Pour tenir compte de la porosite
  const int marq = phi_u_transportant(equation());
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();

  DoubleTab tab_vitesse(vitesse_.valeur().valeurs());
  for (int i=0; i<tab_vitesse.dimension(0); i++)
    for (int j=0; j<tab_vitesse.dimension(1); j++)
      tab_vitesse(i,j)*=porosite_face(i);

  const int nb_comp=transporte.line_size();
  int elem=0,type_elem=0, facei=0,facei_loc=0, ligne=0, dim=0;
  double coeff=0., signe=0., div=0.;

  const DoubleVect& transporteV = transporte;
  DoubleVect& resuV = resu;

  double (*formule)(int);
  if (!volumes_etendus_)
    formule= (dimension==2) ? &formule_Id_2D : &formule_Id_3D;
  else
    formule= (dimension==2) ? &formule_2D : &formule_3D;


  for (elem=0; elem<nb_elem_tot; elem++)
    {
      //Type de l'element : le nombre de faces de Dirichlet
      //qu'il contient
      type_elem=elem_nb_faces_dirichlet_(elem);
      coeff=formule(type_elem);

      //Calcul de la divergence par element
      div=0.;
      for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
        {
          facei=elem_faces(elem,facei_loc);
          signe=(face_voisins(facei,0)==elem)? 1.:-1.;

          for (dim=0; dim<dimension; dim++)
            div+=signe*face_normales(facei,dim)*tab_vitesse(facei,dim);
        }
      div*=coeff;
      if (!marq) div/=porosite_elem(elem);

      //Calcul de la partie compressible
      for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
        {
          facei=elem_faces(elem,facei_loc);
          for (dim=0; dim<nb_comp; dim++)
            {
              ligne=facei*nb_comp+dim;
              resuV[ligne]-=div*transporteV[ligne];
            }
        }
    }

  return resu;
}

void Op_Conv_EF_VEF_P1NC_Stab::calculer_flux_bords(const DoubleTab& Kij, const DoubleTab& tab_vitesse, const DoubleTab& transporte) const
{

  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
#ifndef NDEBUG
  const IntTab& face_voisins = zone_VEF.face_voisins();
#endif
  const DoubleVect& transporteV=transporte;
  const DoubleTab& face_normales=zone_VEF.face_normales();
  const int nb_bord = zone_Cl_VEF.nb_cond_lim();
  const int nb_comp=transporte.line_size();

  //  int elem=0;
  int facei=0, dim=0, n_bord=0, num1=0,num2=0, ind_face=0;
  double psc=0.;

  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1 = 0;
      num2 = le_bord.nb_faces();//il ne faut boucler que sur les faces reelles ici

      if ( sub_type(Dirichlet_homogene,la_cl.valeur()) )
        {
          //On ne calcule pas le flux aux bords Dirichlet_homogene
        }
      else if ( sub_type(Neumann,la_cl.valeur())
                || sub_type(Neumann_val_ext,la_cl.valeur())
                || sub_type(Neumann_homogene,la_cl.valeur())
                || sub_type(Symetrie,la_cl.valeur())
                || sub_type(Echange_impose_base,la_cl.valeur())
                || sub_type(Dirichlet,la_cl.valeur())
                || sub_type(Periodique,la_cl.valeur()) )
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei = le_bord.num_face(ind_face);
              assert(face_voisins(facei,0)!=-1);
              psc=0.;
              for (dim=0; dim<dimension; dim++)
                psc-=tab_vitesse(facei,dim)*face_normales(facei,dim);

              for (dim=0; dim<nb_comp; dim++)
                flux_bords_(facei,dim)=psc*transporteV[facei*nb_comp+dim];
            }

        }//fin du if sur "Neumann", "Neumann_homogene", "Symetrie", "Echange_impose_base"
      else
        {
          Cerr << "Erreur Op_Conv_EF_VEF_P1NC_Stab::calculer_flux_bords()" << finl;
          Cerr << "Condition aux limites " << la_cl.que_suis_je() << " non codee."   << finl;
          Cerr << "Sortie du programme." << finl;
          Process::exit();
        }//fin du else sur les autres conditions aux limites
    }
}

DoubleTab&
Op_Conv_EF_VEF_P1NC_Stab::ajouter_operateur_centre(const DoubleTab& Kij, const DoubleTab& transporte, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.line_size();
  const int nb_comp=transporte.line_size();
  int elem=0,facei=0,facei_loc=0, facej=0,facej_loc=0, ligne=0,colonne=0, dim=0;
  double kij=0.,kji=0.,delta=0.;
  const DoubleVect& transporteV = transporte;
  DoubleVect& resuV = resu;

  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);
        for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
          {
            facej=elem_faces(elem,facej_loc);
            assert(facej!=facei);
            kij=Kij(elem,facei_loc,facej_loc);
            kji=Kij(elem,facej_loc,facei_loc);

            for (dim=0; dim<nb_comp; dim++)
              {
                ligne=facei*nb_comp+dim;
                colonne=facej*nb_comp+dim;
                delta=transporteV[colonne]-transporteV[ligne];
                resuV[ligne]+=kij*delta;
                resuV[colonne]-=kji*delta;
              }
          }
      }

  return resu;
}

DoubleTab&
Op_Conv_EF_VEF_P1NC_Stab::ajouter_diffusion(const DoubleTab& Kij, const DoubleTab& transporte, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.line_size();
  const int nb_comp=transporte.line_size();
  int elem=0, facei=0,facei_loc=0, facej=0,facej_loc=0, ligne=0,colonne=0, dim=0;
  double dij=0., delta=0., coeffij=0.,coeffji=0.;

  const DoubleVect& transporteV = transporte;
  DoubleVect& resuV = resu;

  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);
        for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
          {
            facej=elem_faces(elem,facej_loc);
            assert(facej!=facei);

            dij=Dij(elem,facei_loc,facej_loc,Kij);
            assert(dij>=0);

            coeffij=alpha_tab[facei]*dij;
            coeffji=alpha_tab[facej]*dij;

            for (dim=0; dim<nb_comp; dim++)
              {
                ligne=facei*nb_comp+dim;
                colonne=facej*nb_comp+dim;
                delta=transporteV[colonne]-transporteV[ligne];

                //ATTENTION AU SIGNE : ici on code +div(uT)
                //REMARQUE : on utilise la symetrie de l'operateur
                resuV[ligne]+=coeffij*delta;
                resuV[colonne]-=coeffji*delta;
              }
          }
      }
  return resu;
}

DoubleTab&
Op_Conv_EF_VEF_P1NC_Stab::ajouter_antidiffusion(const DoubleTab& Kij, const DoubleTab& transporte, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.line_size();
  const int nb_comp=transporte.line_size();
  int elem=0, facei=0,facei_loc=0, facej=0,facej_loc=0, ligne=0,colonne=0, dim=0, face_amont=0,face_aval=0;
  double kij=0.,kji=0.,dij=0.,lij=0.,lji=0., limit=0.,daij=0., delta=0.;
  double coeffij=0.,coeffji=0., coeff=0., R=0.;

  //Pour le limiteur
  ArrOfDouble P_plus(nb_comp),P_moins(nb_comp);
  ArrOfDouble Q_plus(nb_comp),Q_moins(nb_comp);
  P_plus=0., P_moins=0.;
  Q_plus=0., Q_moins=0.;

  const DoubleVect& transporteV = transporte;
  DoubleVect& resuV = resu;
  const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();
  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);

        P_plus=0., P_moins=0.;
        Q_plus=0., Q_moins=0.;
        calculer_senseur(Kij,transporteV,nb_comp,facei,elem_faces,face_voisins,num_fac_loc,P_plus,P_moins,Q_plus,Q_moins);

        for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
          if (facej_loc!=facei_loc)
            {
              facej=elem_faces(elem,facej_loc);

              kij = Kij(elem,facei_loc,facej_loc);
              kji = Kij(elem,facej_loc,facei_loc);
              dij = Dij(elem,facei_loc,facej_loc,Kij);
              lij = kij+dij;
              lji = kji+dij;
              assert(lij>=0);
              assert(lji>=0);

              if (lij<=lji) //facei est amont
                {
                  face_amont = facei;
                  face_aval = facej;

                  //Si lij==lji, on passe deux foix dans la boucle
                  //d'ou la presence du coefficient 1/2
                  coeff = 1.*(lij<lji)+0.5*(lij==lji);
                  assert(coeff==1. || coeff==0.5);

                  for (dim=0; dim<nb_comp; dim++)
                    {
                      ligne=face_aval*nb_comp+dim;
                      colonne=face_amont*nb_comp+dim;

                      delta = transporteV[colonne]-transporteV[ligne];

                      //Limiteur de pente
                      // if (delta>=0.) R = (P_plus(dim)==0.) ? 0. : Q_plus(dim)/P_plus(dim);
                      // else R = (P_moins(dim)==0.) ? 0. : Q_moins(dim)/P_moins(dim);

                      // if (delta>=0.) R = (P_plus(dim)==0.) ? 0. : Q_plus(dim)/(P_plus(dim)+DMINFLOAT);
                      // else R = (P_moins(dim)==0.) ? 0. : Q_moins(dim)/(P_moins(dim)+DMINFLOAT);

                      if (delta>=0.) R = (std::fabs(P_plus[dim])<DMINFLOAT) ? 0. : Q_plus[dim]/P_plus[dim];
                      else R = (std::fabs(P_moins[dim])<DMINFLOAT) ? 0. : Q_moins[dim]/P_moins[dim];


                      limit=limiteur(R);
                      // limiteurs_(facei,dim)+=limit;
                      // limiteurs_(facej,dim)+=limit;

                      daij=minimum(limit*dij,lji);
                      assert(daij>=0);
                      assert(daij<=lji);

                      coeffij=alpha_tab[face_amont]*beta[face_amont]*daij;
                      coeffji=alpha_tab[face_aval]*beta[face_aval]*daij;

                      //Calcul de resu
                      resuV[colonne]+=coeffij*coeff*delta;
                      resuV[ligne]-=coeffji*coeff*delta;
                    }
                }
            }
      }

  return resu;
}

//ATTENTION : suppose les parametres P_plus, P_moins, Q_plus, Q_moins nuls en entree
inline void
Op_Conv_EF_VEF_P1NC_Stab::calculer_senseur(const DoubleTab& Kij, const DoubleVect& transporteV,
                                           const int nb_comp, const int face_i,
                                           const IntTab& elem_faces, const IntTab& face_voisins, const IntTab& num_fac_loc,
                                           ArrOfDouble& P_plus, ArrOfDouble& P_moins,
                                           ArrOfDouble& Q_plus, ArrOfDouble& Q_moins) const
{
  assert(P_plus.size_array()==nb_comp);
  assert(Q_plus.size_array()==nb_comp);
  assert(P_moins.size_array()==nb_comp);
  assert(Q_moins.size_array()==nb_comp);
  const int nb_faces_elem=elem_faces.dimension(1);
  for (int elem_voisin=0; elem_voisin<2; elem_voisin++)
    {
      int elem = face_voisins(face_i,elem_voisin);
      if (elem!=-1)
        {
          int face_i_loc = num_fac_loc(face_i,elem_voisin);
          assert(face_i_loc>=0);
          assert(face_i_loc<nb_faces_elem);
          //On travaille sur les faces de "elem"
          for (int face_k_loc=0; face_k_loc<nb_faces_elem; face_k_loc++)
            {
              int face_k=elem_faces(elem,face_k_loc);
              double kik=Kij(elem,face_i_loc,face_k_loc);
              //
              //Calcul des variables intermediaires
              //
              for (int dim=0; dim<nb_comp; dim++)
                {
                  double deltaki = transporteV[face_k*nb_comp+dim]-transporteV[face_i*nb_comp+dim];
                  //Calcul de P_plus et P_moins
                  //Calcul de Q_plus et Q_moins
                  /* Codage initial:
                     P_plus(dim)+=minimum(0.,kik)*minimum(0.,deltaki);
                     P_moins(dim)+=minimum(0.,kik)*maximum(0.,deltaki);
                     Q_plus(dim)+=maximum(0.,kik)*maximum(0.,deltaki);
                     Q_moins(dim)+=maximum(0.,kik)*minimum(0.,deltaki);
                  */
                  // Codage optimise:
                  double tmp = kik*deltaki;
                  if (kik>0)
                    {
                      if (tmp>0) Q_plus[dim]+=tmp;
                      else       Q_moins[dim]+=tmp;
                    }
                  else
                    {
                      if (tmp>0) P_plus[dim]+=tmp;
                      else       P_moins[dim]+=tmp;
                    }
                  assert(P_plus[dim]>=0);
                  assert(Q_plus[dim]>=0);
                  assert(P_moins[dim]<=0);
                  assert(Q_moins[dim]<=0);
                }//fin du for sur "dim"
              //
              //Fin du calcul des variables intermediaires
              //
            }//fin du for sur "face_k_loc"
        }//fin du if sur "elem!=-1"
    }//fin du for sur "elem_voisin"
}

void Op_Conv_EF_VEF_P1NC_Stab::test(const DoubleTab& transporte, const DoubleTab& resu, const DoubleTab& tab_vitesse) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const int nb_faces_elem=elem_faces.dimension(1);
  const int nb_elem_tot = zone_VEF.nb_elem_tot();

  DoubleTab Kij2(nb_elem_tot,nb_faces_elem, nb_faces_elem);
  Kij2=0.;

  DoubleTab Kij_ancien(nb_elem_tot,nb_faces_elem, nb_faces_elem);
  Kij_ancien=0.;

  test_difference_Kij(transporte,Kij2,Kij_ancien,tab_vitesse);
  test_difference_resu(Kij2,Kij_ancien,transporte,resu,tab_vitesse);
}

void Op_Conv_EF_VEF_P1NC_Stab::test_difference_Kij(const DoubleTab& transporte, DoubleTab& Kij, DoubleTab& Kij_ancien, const DoubleTab& tab_vitesse ) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  //  const Champ_P1NC& la_vitesse=ref_cast( Champ_P1NC, vitesse_.valeur());
  //const DoubleTab& vitesse=la_vitesse.valeurs();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normales=zone_VEF.face_normales();
  const int nb_faces_elem=elem_faces.dimension(1);
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  const int nb_comp = (transporte.nb_dim()!=1) ? transporte.dimension(1) : 1;
  int face_i0, face_j0, elem0, comp;

  calculer_coefficients_operateur_centre(Kij,nb_comp,tab_vitesse);

  //
  //   Calcul des Kij_ancien :
  //
  for(elem0=0; elem0<nb_elem_tot; elem0++)
    {
      int face_loci=0;
      int face_locj=0;
      for(; face_loci<nb_faces_elem; face_loci++)
        {
          face_i0=elem_faces(elem0,face_loci);
          double signei=1.0;
          if(face_voisins(face_i0,0)!=elem0)
            signei=-1.0;
          double psci=0;
          for(comp=0; comp<dimension; comp++)
            psci+=tab_vitesse(face_i0,comp)*face_normales(face_i0,comp);
          psci*=signei;
          //Kij_ancien(elem,face_loci,face_loci)=0.;
          for(face_locj=face_loci+1; face_locj<nb_faces_elem; face_locj++)
            {
              face_j0=elem_faces(elem0,face_locj);
              double signej=1.0;
              if(face_voisins(face_j0,0)!=elem0)
                signej=-1.0;

              double pscj=0;
              //psci=0;
              for(comp=0; comp<dimension; comp++)
                pscj+=tab_vitesse(face_j0,comp)*face_normales(face_j0,comp);
              pscj*=signej;
              Kij_ancien(elem0,face_loci,face_locj)=-1./nb_faces_elem*pscj;
              Kij_ancien(elem0,face_loci,face_loci)+=1./nb_faces_elem*pscj;
              Kij_ancien(elem0,face_locj,face_loci)=-1./nb_faces_elem*psci;
              Kij_ancien(elem0,face_locj,face_locj)+=1./nb_faces_elem*psci;
            }
        }
    }
  // Correction des Kij_ancien pour Dirichlet !
  {
    int nb_bord=zone_Cl_VEF.nb_cond_lim();
    int face;
    double coeff=1./dimension;
    for (int n_bord=0; n_bord<nb_bord; n_bord++)
      {
        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        int nb_faces_tot=le_bord.nb_faces_tot();

        if ( (sub_type(Dirichlet,la_cl.valeur()))
             || (sub_type(Dirichlet_homogene,la_cl.valeur()))
           )
          {
            if ( volumes_etendus_ )
              {
                for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
                  {
                    face = le_bord.num_face(ind_face);
                    int elem=face_voisins(face,0);
                    assert(elem!=-1);
                    int face_loc_j;
                    int face_j=-1;
                    for (face_loc_j=0; (face_loc_j<nb_faces_elem && face_j!=face); face_loc_j++)
                      {
                        face_j=elem_faces(elem,face_loc_j);
                      }
                    face_loc_j--;
                    assert(face_loc_j>=0);
                    assert(face_loc_j<nb_faces_elem);
                    assert(elem_faces(elem,face_loc_j)==face);
                    const double kjj=Kij_ancien(elem,face_loc_j,face_loc_j);
                    for (int face_loc_i=0; face_loc_i<nb_faces_elem; face_loc_i++)
                      {
                        int face_i=elem_faces(elem,face_loc_i);
                        if(face_i!=face)
                          {
                            double& kii=Kij_ancien(elem,face_loc_i,face_loc_i);
                            const double kji=Kij_ancien(elem,face_loc_j,face_loc_i);
                            kii+=coeff*kji;
                            double& kij=Kij_ancien(elem,face_loc_i,face_loc_j);
                            kij+=coeff*kjj;
                            for (int face_loc_k=(face_loc_i+1); face_loc_k<nb_faces_elem; face_loc_k++)
                              {
                                int face_k=elem_faces(elem,face_loc_k);
                                if(face_k!=face)
                                  {
                                    double& kik=Kij_ancien(elem,face_loc_i,face_loc_k);
                                    const double kjk=Kij_ancien(elem,face_loc_j,face_loc_k);
                                    double& kki=Kij_ancien(elem,face_loc_k,face_loc_i);
                                    kik+=coeff*kjk;
                                    kki+=coeff*kji;
                                  }
                              }
                          }
                      }
                    {
                      for (int face_loc_i=0; face_loc_i<nb_faces_elem; face_loc_i++)
                        Kij_ancien(elem,face_loc_j,face_loc_i)=0;
                    }
                    {
                      for (int face_loc_i=0; face_loc_i<nb_faces_elem; face_loc_i++)
                        {
                          double sum=0.;
                          for (int face_loc_k=0; face_loc_k<nb_faces_elem; face_loc_k++)
                            {
                              sum+=Kij_ancien(elem,face_loc_i,face_loc_k);
                            }
                          //Cerr << "somme apres : " << sum << finl;
                          Kij_ancien(elem,face_loc_i,face_loc_i)-=sum;//car div(u)=0!
                        }
                    }
                  }// for face

              }//fin du if sur "volumes_etendus_"

          }// sub_type Dirichlet
      }
  }

  Kij-=Kij_ancien;
  const double max_kij = local_max_abs_vect(Kij);
  if (max_kij > 1.e-15)
    {
      Cerr << "Erreur dans le calcul des Kij : " << max_kij << finl;
      Cerr << "Sortie du programme" << finl;
      Process::exit();
    }

  Kij+=Kij_ancien;
}

void Op_Conv_EF_VEF_P1NC_Stab::test_difference_resu(const DoubleTab& Kij, const DoubleTab& Kij_ancien,
                                                    const DoubleTab& transporte,const DoubleTab& resu, const DoubleTab& tab_vitesse) const
{
  DoubleTab resu1(resu);
  resu1=0;

  if (is_compressible_) ajouter_partie_compressible(transporte,resu1,tab_vitesse);
  ajouter_operateur_centre(Kij,transporte,resu1);
  ajouter_diffusion(Kij,transporte,resu1);
  ajouter_antidiffusion(Kij,transporte,resu1);
  mettre_a_jour_pour_periodicite(resu1);

  DoubleTab resu2(resu);
  resu2=0;

  //
  //   Calcul de resu2
  //

  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const int nb_faces_elem=elem_faces.line_size();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const int nb_comp=resu.line_size();

  int face_i0, face_j0, elem, comp0;
  const int nb_faces0 = transporte.dimension(0);
  double sigma_fijd=0., sigma_fija=0.;

  ArrOfDouble pplusi(nb_comp);
  ArrOfDouble qplusi(nb_comp);
  ArrOfDouble pmoinsi(nb_comp);
  ArrOfDouble qmoinsi(nb_comp);

  for(elem=0; elem<nb_elem_tot; elem++)
    {
      int face_loci=0, face_locj=0;

      for(; face_loci<nb_faces_elem; face_loci++)
        {
          face_i0=elem_faces(elem,face_loci);
          for(comp0=0; comp0<nb_comp; comp0++)
            resu2(face_i0,comp0)+=Kij_ancien(elem,face_loci,face_loci)*transporte(face_i0,comp0);

          pplusi=0.;
          qplusi=0.;
          pmoinsi=0.;
          qmoinsi=0.;

          int elem1=face_voisins(face_i0,0), elem2=face_voisins(face_i0,1);
          int face_loc_i=0, face_loc_j=0;
          double dT,min_dT,max_dT, K,min_K,max_K;

          //
          // dans elem1 :
          //
          while((face_loc_i<nb_faces_elem)&&(elem_faces(elem1,face_loc_i)!=face_i0))
            face_loc_i++;
          if(face_loc_i==nb_faces_elem)
            {
              //Periodique!!
              assert(elem2!=-1);
              face_loc_i=0;
              while( (face_loc_i<nb_faces_elem) &&
                     (face_voisins(elem_faces(elem1,face_loc_i),0)!=elem2) &&
                     (face_voisins(elem_faces(elem1,face_loc_i),1)!=elem2) )
                face_loc_i++;
            }

          assert(face_loc_i<nb_faces_elem);
          for(face_loc_j=0; face_loc_j<nb_faces_elem; face_loc_j++)
            {
              int face_j=elem_faces(elem1,face_loc_j);
              if(face_j!=face_i0)
                {
                  K=Kij_ancien(elem1,face_loc_i,face_loc_j);

                  if(K>0.)
                    {
                      max_K=K ;
                      min_K=0.;
                    }
                  else
                    {
                      max_K=0.;
                      min_K=K ;
                    }

                  for(comp0=0; comp0<nb_comp; comp0++)
                    {
                      dT =transporte(face_j,comp0);
                      dT-=transporte(face_i0,comp0);

                      if(dT>0.)
                        {
                          max_dT=dT ;
                          min_dT=0 ;
                        }
                      else
                        {
                          max_dT=0. ;
                          min_dT=dT ;
                        }

                      pplusi[comp0] +=min_K*min_dT;
                      pmoinsi[comp0]+=min_K*max_dT;
                      qplusi[comp0] +=max_K*max_dT;
                      qmoinsi[comp0]+=max_K*min_dT;
                    }
                }
            }

          //
          // dans elem2 :
          //

          if(elem2!=-1)
            {
              face_loc_i=0;
              while((face_loc_i<nb_faces_elem)&&(elem_faces(elem2,face_loc_i)!=face_i0))
                face_loc_i++;
              if(face_loc_i==nb_faces_elem)
                {
                  //Periodique!!
                  face_loc_i=0;
                  while( (face_loc_i<nb_faces_elem) &&
                         (face_voisins(elem_faces(elem2,face_loc_i),0)!=elem1) &&
                         (face_voisins(elem_faces(elem2,face_loc_i),1)!=elem1) )
                    face_loc_i++;
                }
              assert(face_loc_i<nb_faces_elem);
              for(face_loc_j=0; face_loc_j<nb_faces_elem; face_loc_j++)
                {
                  int face_j=elem_faces(elem2,face_loc_j);
                  if(face_j!=face_i0)
                    {
                      K=Kij_ancien(elem2,face_loc_i,face_loc_j);

                      if(K>0.)
                        {
                          max_K=K ;
                          min_K=0.;
                        }
                      else
                        {
                          max_K=0.;
                          min_K=K ;
                        }

                      for(comp0=0; comp0<nb_comp; comp0++)
                        {
                          dT =transporte(face_j,comp0);
                          dT-=transporte(face_i0,comp0);

                          if(dT>0.)
                            {
                              max_dT=dT ;
                              min_dT=0 ;
                            }
                          else
                            {
                              max_dT=0. ;
                              min_dT=dT ;
                            }

                          pplusi[comp0] +=min_K*min_dT;
                          pmoinsi[comp0]+=min_K*max_dT;
                          qplusi[comp0] +=max_K*max_dT;
                          qmoinsi[comp0]+=max_K*min_dT;
                        }
                    }
                }
            }

          for(face_locj=0; face_locj<nb_faces_elem; face_locj++)
            if(face_locj!=face_loci)
              {
                face_j0=elem_faces(elem,face_locj);
                const double kij=Kij_ancien(elem,face_loci,face_locj);
                const double kji=Kij_ancien(elem,face_locj,face_loci);
                double dij=Dij(elem,face_loci,face_locj,Kij_ancien);
                double lji=kji+dij;
                double lij=kij+dij;
                assert(lij>=0);
                assert(lji>=0);

                for(comp0=0; comp0<nb_comp; comp0++)
                  {
                    const double Ti=transporte(face_i0,comp0);
                    const double Tj=transporte(face_j0,comp0);
                    double deltaij=Ti-Tj;
                    double Fij=0;
                    if(lij<=lji)
                      {
                        double coef=1;
                        if (lij==lji)  coef=.5;
                        if(deltaij)
                          {
                            if(Ti >= Tj)
                              {
                                if(pplusi[comp0])
                                  {
                                    double R=qplusi[comp0]/pplusi[comp0];
                                    Fij=minimum(limiteur(R)*dij,lji);
                                  }
                              }
                            else if(pmoinsi[comp0])
                              {
                                double R=qmoinsi[comp0]/pmoinsi[comp0];
                                Fij=minimum(limiteur(R)*dij,lji);
                              }

                            assert(Fij*dij>=0);
                            sigma_fijd+=dij;
                            sigma_fija+=Fij;
                            Fij-=dij;
                            Fij*=deltaij;
                          }
                        resu2(face_i0,comp0)+=coef*(kij*Tj+Fij);
                        resu2(face_j0,comp0)+=coef*(kji*Ti-Fij);
                      }
                  }
              }
        }
    }

  // Pour periodique

  int nb_bord=zone_Cl_VEF.nb_cond_lim();
  int face;
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int nb_faces_b=le_bord.nb_faces();
      int num2 = num1 + nb_faces_b;
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int face_associee;
          IntVect fait(nb_faces_b);
          fait = 0;

          for (face=num1; face<num2; face++)
            {
              if (fait(face-num1) == 0)
                {
                  fait(face-num1) = 1;
                  face_associee=la_cl_perio.face_associee(face-num1);
                  fait(face_associee) = 1;
                  for (int comp=0; comp<nb_comp; comp++)
                    resu2(face_associee+num1, comp)=(resu2(face,comp)+=resu2(face_associee+num1,comp));
                }// if fait
            }// for face
        }// sub_type Perio
    }

  resu1-=resu2;

  //Dans le cas des faces de Dirichlet, notre algorithme ne calcule aucune
  //valeur sur les faces de Dirichlet car de toute facon, elles sont ecrasees
  //par la matrice de masse. Mais ce n'est pas le cas de l'ancien algorithme
  //d'ou la modification apportee ici pour eviter de ne voir que les erreurs
  //commises sur les bords de Dirichlet qui n'ont aucune importance
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int nb_faces=le_bord.nb_faces();
      int num2 = num1 + nb_faces;

      if (sub_type(Dirichlet,la_cl.valeur()) || sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          for (face=num1; face<num2; face++)
            for (int dim=0; dim<nb_comp; dim++)
              resu1(face,dim)=0;
        }//fin du if sur Dirichlet
    }//fin du for sur "n_bord"

  const double max_abs_resu1 = local_max_abs_vect(resu1);
  Journal() << "local_max_abs_vect(resu1) = " << max_abs_resu1
            << " " << equation().schema_temps().temps_courant() << finl;

  if (max_abs_resu1 > 1.e-15)
    {
      Cerr << "Erreur dans le calcul des resu : " << max_abs_resu1 << finl;
      Cerr << "Affichage des faces de bord" << finl;

      /**************************************************/
      Cerr << "Affichage des faces de bord." << finl;

      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int nb_faces=le_bord.nb_faces();
          int num2 = num1 + nb_faces;

          if (sub_type(Periodique,la_cl.valeur()))
            {
              Cerr << "Bord periodique : ";
              for (face=num1; face<num2; face++)
                {
                  Cerr << face << ",";
                }// for face
              Cerr << finl;
            }// sub_type Perio

          else if (sub_type(Dirichlet,la_cl.valeur()) || (sub_type(Dirichlet_homogene,la_cl.valeur())) )
            {
              Cerr << "Bord de Dirichlet : ";
              for (face=num1; face<num2; face++)
                {
                  Cerr << face << ",";
                }// for face
              Cerr << finl;
            }

        }//fin du for sur "nbord"
      /**************************************************/

      /**************************************************/
      Cerr << "Affichage des faces qui posent probleme : " << finl;
      if (nb_comp==1)
        {
          for (int face_i=0; face_i<nb_faces0; face_i++)
            {
              if (resu1(face_i)>1.e-15)
                Cerr << face_i << "(" << face_voisins(face_i,0) << ","
                     << face_voisins(face_i,1) << ") ; ";
            }//fin du for sur "face_i"
        }
      else
        {
          for (int face_i=0; face_i<nb_faces0; face_i++)
            {
              Cerr << face_i << "(" << face_voisins(face_i,0) << ","
                   << face_voisins(face_i,1) << ") ";

              for (int dim=0; dim<nb_comp; dim++)
                {
                  Cerr  << ", resu1("
                        << face_i << "," << dim << ")= "
                        << resu1(face_i,dim);
                }
              Cerr << finl;
            }
        }
      Cerr << finl;
      /**************************************************/

      /**************************************************/
      resu1+=resu2;

      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int nb_faces=le_bord.nb_faces();
          int num2 = num1 + nb_faces;

          if (sub_type(Periodique,la_cl.valeur()))
            {
              Cerr << "Affichage des valeurs des resu au bord perio" << finl;
              for (face=num1; face<num2; face++)
                {
                  Cerr << "resu1(" << face << ") : " << resu1(face) << finl;
                  Cerr << "resu2(" << face << ") : " << resu2(face) << finl;
                }// for face
              Cerr << finl;
            }// sub_type Perio

        }//fin du for sur "nbord"

      /**************************************************/

      static int count = 0;
      count++;
      if (count==2)
        {
          Cerr << "Sortie du programme" << finl;
          Process::exit();
        }
    }
}

void Op_Conv_EF_VEF_P1NC_Stab::mettre_a_jour_pour_periodicite(DoubleTab& resu) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const int nb_bord = zone_Cl_VEF.nb_cond_lim();
  const int nb_comp = (resu.nb_dim()==1) ? 1 : resu.dimension(1);
  DoubleVect& resuV = resu;
  int ligne=0,ligneAss=0,facei=0,faceiAss=0, ind_face=0;
  int num1=0,num2=0, ind_face_associee=0, dim=0;

  //Faces de bord
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1=0;
      num2=le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei=le_bord.num_face(ind_face);
              ind_face_associee=la_cl_perio.face_associee(ind_face);
              faceiAss=le_bord.num_face(ind_face_associee);

              if (facei<faceiAss)
                for (dim=0; dim<nb_comp; dim++)
                  {
                    ligne=facei*nb_comp+dim;
                    ligneAss=faceiAss*nb_comp+dim;

                    resuV[ligneAss]+=resuV[ligne];
                    resuV[ligne]=resuV[ligneAss];
                  }

            }//fin du for sur "face_i"

        }//fin du if sur "Periodique"

    }//fin du for sur "n_bord"
}

void Op_Conv_EF_VEF_P1NC_Stab::ajouter_old(const DoubleTab& transporte, DoubleTab& resu, const DoubleTab& tab_vitesse ) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  //  const Champ_P1NC& la_vitesse=ref_cast( Champ_P1NC, vitesse_.valeur());
  // const DoubleTab& vitesse=la_vitesse.valeurs();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normales=zone_VEF.face_normales();
  const int nb_faces_elem=elem_faces.dimension(1);
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  assert(nb_faces_elem==(dimension+1));

  int nb_comp=resu.line_size();

  int face_i0, face_j0, elem0, comp0;
  DoubleTab Kij(nb_elem_tot,nb_faces_elem, nb_faces_elem);
  //
  //   Calcul des Kij :
  //
  for(elem0=0; elem0<nb_elem_tot; elem0++)
    {
      int face_loci=0;
      int face_locj=0;
      for(; face_loci<nb_faces_elem; face_loci++)
        {
          face_i0=elem_faces(elem0,face_loci);
          double signei=1.0;
          if(face_voisins(face_i0,0)!=elem0)
            signei=-1.0;
          double psci=0;
          for(comp0=0; comp0<dimension; comp0++)
            psci+=tab_vitesse(face_i0,comp0)*face_normales(face_i0,comp0);
          psci*=signei;
          //Kij(elem,face_loci,face_loci)=0.;
          for(face_locj=face_loci+1; face_locj<nb_faces_elem; face_locj++)
            {
              face_j0=elem_faces(elem0,face_locj);
              double signej=1.0;
              if(face_voisins(face_j0,0)!=elem0)
                signej=-1.0;

              double pscj=0;
              //psci=0;
              for(comp0=0; comp0<dimension; comp0++)
                pscj+=tab_vitesse(face_j0,comp0)*face_normales(face_j0,comp0);
              pscj*=signej;
              Kij(elem0,face_loci,face_locj)=-1./nb_faces_elem*pscj;
              Kij(elem0,face_loci,face_loci)+=1./nb_faces_elem*pscj;
              Kij(elem0,face_locj,face_loci)=-1./nb_faces_elem*psci;
              Kij(elem0,face_locj,face_locj)+=1./nb_faces_elem*psci;
            }
        }
    }
  //
  // Correction des Kij pour Dirichlet !
  //
  {
    int nb_bord=zone_Cl_VEF.nb_cond_lim();
    int face;
    double coeff=1./dimension;
    for (int n_bord=0; n_bord<nb_bord; n_bord++)
      {
        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
        const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
        int nb_faces_tot=le_bord.nb_faces_tot();
        if (( (sub_type(Dirichlet,la_cl.valeur())) || (sub_type(Dirichlet_homogene,la_cl.valeur())) )
            && ( volumes_etendus_))
          {
            for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
              {
                face=le_bord.num_face(ind_face);
                int elem=face_voisins(face,0);
                assert(elem!=-1);
                int face_loc_j;
                int face_j=-1;
                for (face_loc_j=0; (face_loc_j<nb_faces_elem && face_j!=face); face_loc_j++)
                  {
                    face_j=elem_faces(elem,face_loc_j);
                  }
                face_loc_j--;
                assert(face_loc_j>=0);
                assert(face_loc_j<nb_faces_elem);
                assert(elem_faces(elem,face_loc_j)==face);
                const double kjj=Kij(elem,face_loc_j,face_loc_j);
                for (int face_loc_i=0; face_loc_i<nb_faces_elem; face_loc_i++)
                  {
                    int face_i=elem_faces(elem,face_loc_i);
                    if(face_i!=face)
                      {
                        double& kii=Kij(elem,face_loc_i,face_loc_i);
                        const double kji=Kij(elem,face_loc_j,face_loc_i);
                        kii+=coeff*kji;
                        double& kij=Kij(elem,face_loc_i,face_loc_j);
                        kij+=coeff*kjj;
                        for (int face_loc_k=(face_loc_i+1); face_loc_k<nb_faces_elem; face_loc_k++)
                          {
                            int face_k=elem_faces(elem,face_loc_k);
                            if(face_k!=face)
                              {
                                double& kik=Kij(elem,face_loc_i,face_loc_k);
                                const double kjk=Kij(elem,face_loc_j,face_loc_k);
                                double& kki=Kij(elem,face_loc_k,face_loc_i);
                                kik+=coeff*kjk;
                                kki+=coeff*kji;
                              }
                          }
                      }
                  }
                {
                  for (int face_loc_i=0; face_loc_i<nb_faces_elem; face_loc_i++)
                    Kij(elem,face_loc_j,face_loc_i)=0;
                }
                {
                  for (int face_loc_i=0; face_loc_i<nb_faces_elem; face_loc_i++)
                    {
                      double sum=0.;
                      for (int face_loc_k=0; face_loc_k<nb_faces_elem; face_loc_k++)
                        {
                          sum+=Kij(elem,face_loc_i,face_loc_k);
                        }
                      //Cerr << "somme apres : " << sum << finl;
                      Kij(elem,face_loc_i,face_loc_i)-=sum;//car div(u)=0!
                    }
                }
              }// for face
          }// sub_type Dirichlet
      }
  }

  //
  //   Calcul de resu
  //

  double sigma_fijd=0;
  double sigma_fija=0;

  ArrOfDouble pplusi(nb_comp);
  ArrOfDouble qplusi(nb_comp);
  ArrOfDouble pmoinsi(nb_comp);
  ArrOfDouble qmoinsi(nb_comp);

  for(elem0=0; elem0<nb_elem_tot; elem0++)
    {
      int face_loci=0;
      int face_locj=0;

      for(; face_loci<nb_faces_elem; face_loci++)
        {
          face_i0=elem_faces(elem0,face_loci);

          for(comp0=0; comp0<nb_comp; comp0++)
            resu(face_i0,comp0)+=Kij(elem0,face_loci,face_loci)*transporte(face_i0,comp0);

          pplusi=0., qplusi=0., pmoinsi=0., qmoinsi=0.;

          int elem1=face_voisins(face_i0,0);
          int elem2=face_voisins(face_i0,1);
          int face_loc_i=0;
          int face_loc_j=0;
          double dT,min_dT,max_dT;
          double K,min_K,max_K;

          //
          // dans elem1 :
          //
          while((face_loc_i<nb_faces_elem)&&(elem_faces(elem1,face_loc_i)!=face_i0))
            face_loc_i++;
          if(face_loc_i==nb_faces_elem)
            {
              //Periodique!!
              assert(elem2!=-1);
              face_loc_i=0;
              while( (face_loc_i<nb_faces_elem) &&
                     (face_voisins(elem_faces(elem1,face_loc_i),0)!=elem2) &&
                     (face_voisins(elem_faces(elem1,face_loc_i),1)!=elem2) )
                face_loc_i++;
            }
          assert(face_loc_i<nb_faces_elem);
          for(face_loc_j=0; face_loc_j<nb_faces_elem; face_loc_j++)
            {
              int face_j=elem_faces(elem1,face_loc_j);
              if(face_j!=face_i0)
                {
                  K=Kij(elem1,face_loc_i,face_loc_j);

                  if(K>0.)
                    {
                      max_K=K ;
                      min_K=0.;
                    }
                  else
                    {
                      max_K=0.;
                      min_K=K ;
                    }

                  for(comp0=0; comp0<nb_comp; comp0++)
                    {
                      dT =transporte(face_j,comp0);
                      dT-=transporte(face_i0,comp0);

                      if(dT>0.)
                        {
                          max_dT=dT ;
                          min_dT=0 ;
                        }
                      else
                        {
                          max_dT=0. ;
                          min_dT=dT ;
                        }

                      pplusi[comp0] +=min_K*min_dT;
                      pmoinsi[comp0]+=min_K*max_dT;
                      qplusi[comp0] +=max_K*max_dT;
                      qmoinsi[comp0]+=max_K*min_dT;
                    }
                }
            }

          //
          // dans elem2 :
          //

          if(elem2!=-1)
            {
              face_loc_i=0;
              while((face_loc_i<nb_faces_elem)&&(elem_faces(elem2,face_loc_i)!=face_i0))
                face_loc_i++;
              if(face_loc_i==nb_faces_elem)
                {
                  //Periodique!!
                  face_loc_i=0;
                  while( (face_loc_i<nb_faces_elem) &&
                         (face_voisins(elem_faces(elem2,face_loc_i),0)!=elem1) &&
                         (face_voisins(elem_faces(elem2,face_loc_i),1)!=elem1) )
                    face_loc_i++;
                }
              assert(face_loc_i<nb_faces_elem);
              for(face_loc_j=0; face_loc_j<nb_faces_elem; face_loc_j++)
                {
                  int face_j=elem_faces(elem2,face_loc_j);
                  if(face_j!=face_i0)
                    {
                      K=Kij(elem2,face_loc_i,face_loc_j);

                      if(K>0.)
                        {
                          max_K=K ;
                          min_K=0.;
                        }
                      else
                        {
                          max_K=0.;
                          min_K=K ;
                        }

                      for(comp0=0; comp0<nb_comp; comp0++)
                        {
                          dT =transporte(face_j,comp0);
                          dT-=transporte(face_i0,comp0);

                          if(dT>0.)
                            {
                              max_dT=dT ;
                              min_dT=0 ;
                            }
                          else
                            {
                              max_dT=0. ;
                              min_dT=dT ;
                            }

                          pplusi[comp0] +=min_K*min_dT;
                          pmoinsi[comp0]+=min_K*max_dT;
                          qplusi[comp0] +=max_K*max_dT;
                          qmoinsi[comp0]+=max_K*min_dT;
                        }
                    }
                }
            }



          for(face_locj=0; face_locj<nb_faces_elem; face_locj++)
            if(face_locj!=face_loci)
              {
                face_j0=elem_faces(elem0,face_locj);
                const double kij=Kij(elem0,face_loci,face_locj);
                const double kji=Kij(elem0,face_locj,face_loci);
                double dij=Dij(elem0,face_loci,face_locj,Kij);
                double lji=kji+dij;
                double lij=kij+dij;
                assert(lij>=0);
                assert(lji>=0);

                for(comp0=0; comp0<nb_comp; comp0++)
                  {
                    const double Ti=transporte(face_i0,comp0);
                    const double Tj=transporte(face_j0,comp0);
                    double deltaij=Ti-Tj;
                    double Fij=0;
                    if(lij<=lji)
                      {
                        double coef=1;
                        if (lij==lji)  coef=.5;
                        if(deltaij)
                          {
                            if(Ti >= Tj)
                              {
                                if(pplusi[comp0])
                                  {
                                    double R=qplusi[comp0]/pplusi[comp0];
                                    Fij=minimum(limiteur(R)*dij,lji);
                                  }
                              }
                            else if(pmoinsi[comp0])
                              {
                                double R=qmoinsi[comp0]/pmoinsi[comp0];
                                Fij=minimum(limiteur(R)*dij,lji);
                              }
                            assert(Fij*dij>=0);
                            sigma_fijd+=dij;
                            sigma_fija+=Fij;
                            Fij-=dij;
                            Fij*=deltaij;
                          }
                        resu(face_i0,comp0)+=coef*(kij*Tj+Fij);
                        resu(face_j0,comp0)+=coef*(kji*Ti-Fij);
                      }
                  }
              }
        }
    }
  //On reprend ici le traitement qui est fait pour la CL de Neumann_sortie_libre dans Op_Conv_VEF_Face
  int nb_bord=zone_Cl_VEF.nb_cond_lim();
  int face;
  const int ncomp_ch_transporte = transporte.line_size();

  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_tot=le_bord.nb_faces_tot();

      double psc;
      int num_face,i;

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
          //const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          DoubleVect& fluent_ = fluent;

          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += tab_vitesse(num_face,i)*face_normales(num_face,i);
              if (psc>0)
                {
                  for (i=0; i<ncomp_ch_transporte; i++)
                    resu(num_face,i) -= psc*transporte(num_face,i);
                }
              else
                {
                  for (i=0; i<ncomp_ch_transporte; i++)
                    resu(num_face,i) -= psc*la_sortie_libre.val_ext(num_face-num1,i);
                  fluent_(num_face) -= psc;
                }
            }
        }
      // Pour periodique
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int face_associee,ind_face_associee;
          IntVect fait(nb_faces_tot);
          fait = 0;

          for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              if (fait(ind_face) == 0)
                {
                  fait(ind_face) = 1;
                  ind_face_associee=la_cl_perio.face_associee(ind_face);
                  fait(ind_face_associee) = 1;
                  face_associee=le_bord.num_face(ind_face_associee);
                  for (int comp=0; comp<nb_comp; comp++)
                    resu(face_associee, comp)=(resu(face,comp)+=resu(face_associee,comp));
                }// if fait
            }// for face
        }// sub_type Perio
    }
  /*
    ArrOfDouble bilan(nb_comp);
    BilanQdmVEF::bilan_qdm(resu, zone_Cl_VEF, bilan);
    if(nb_comp==1)
    Cout << "Scalaire Bilan Convectif : " << bilan[0] << finl;
    else
    for (int comp=0; comp<nb_comp; comp++)
    Cout << "Vecteur Bilan Convectif " << comp << " : " << bilan[comp] << finl;
    bilan=0;
    BilanQdmVEF::bilan_energie(resu, transporte, zone_Cl_VEF, bilan);
    if(nb_comp==1)
    Cout << "Scalaire Bilan Convectif Energie : " << bilan[0] << finl;
    else
    for (int comp=0; comp<nb_comp; comp++)
    Cout << "Vecteur Bilan Convectif Energie " << comp << " : " << bilan[comp] << finl;
    if(nb_comp==1)
    {
    Cout << "min = " << min(transporte);
    Cout << " max = " << std::max(transporte) << finl;
    }
    //  else
    //    for (int comp=0; comp<nb_comp; comp++)
    //    {
    //      Cout << "min("<<comp<<") = " << transporte.min(comp);
    //      Cout << " std::max("<<comp<<") = " << transporte.max(comp) << finl;
    //    }
    Cout << " Ratio Antidiffusion/Diffusion = " << sigma_fija/sigma_fijd << finl;
  */
}

//Fonction qui initialise les attributs "elem_nb_faces_dirichlet_"
//et "elem_faces_dirichlet_"
//REMARQUE : "elem_nb_faces_dirichlet_" contient le nombre de faces de Dirichlet
//pour chaque element du maillage
//REMARQUE : "elem_faces_dirichlet_" le numero global des faces de Dirichlet
//contenu dans un element quelconque du maillage
void Op_Conv_EF_VEF_P1NC_Stab::calculer_data_pour_dirichlet()
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  const IntTab& face_voisins = zone_VEF.face_voisins();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nb_bord=zone_Cl_VEF.nb_cond_lim();

  //Dimensionnement et initialisation des attributs
  //REMARQUE : un element ne peut pas avoir plus de
  //(dimension) faces de Dirichlet
  elem_nb_faces_dirichlet_.resize(nb_elem_tot);
  elem_faces_dirichlet_.resize(nb_elem_tot,Objet_U::dimension);
  elem_nb_faces_dirichlet_=0;
  elem_faces_dirichlet_=-1;


  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int face, nb_faces_tot=le_bord.nb_faces_tot();

      if ( (sub_type(Dirichlet,la_cl.valeur()))
           || (sub_type(Dirichlet_homogene,la_cl.valeur()))
         )
        {
          //
          //Remplissage des tableaux
          //
          for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
            {
              face = le_bord.num_face(ind_face);
              const int elem=face_voisins(face,0);
              assert(elem!=-1);

              elem_nb_faces_dirichlet_(elem)+=1;
              assert(elem_nb_faces_dirichlet_(elem)<=Objet_U::dimension);

              if (elem_faces_dirichlet_(elem,0)==-1) elem_faces_dirichlet_(elem,0)=face;
              else if (elem_faces_dirichlet_(elem,1)==-1) elem_faces_dirichlet_(elem,1)=face;
              else if (Objet_U::dimension==3 && elem_faces_dirichlet_(elem,2)==-1) elem_faces_dirichlet_(elem,2)=face;
              else
                {
                  Cerr << "Erreur Op_Conv_EF_VEF_P1NC_Stab::calculer_data_pour_dirichlet()" << finl;
                  Cerr << "L'element numero " << elem << " contient plus de "
                       << Objet_U::dimension << " faces de Dirichlet" << finl;
                  Cerr << "Sortie du programme." << finl;
                  Process::exit();
                }
            }//fin du for sur "face"

          //
          //Fin du remplissage des tableaux
          //

        }//fin du if sur "Dirichlet"

    }//fin du for sur "n_bord"
}

void Op_Conv_EF_VEF_P1NC_Stab::completer()
{
  Op_Conv_VEF_Face::completer();
  calculer_data_pour_dirichlet();

  //  int nb_comp=1;
  //  if (equation().inconnue().valeurs().nb_dim()>1)
  //  nb_comp=equation().inconnue().valeurs().dimension(1);

  //  limiteurs_.resize(la_zone_vef->nb_faces_tot(),nb_comp);
  // limiteurs_=0.;

  alpha_tab.resize_array(la_zone_vef->nb_faces_tot());
  alpha_tab = alpha_;
  beta.resize_array(la_zone_vef->nb_faces_tot());
  beta=1.;

  if (ssz_alpha)
    {
      for (int i=0; i<nb_ssz_alpha; i++)
        {
          REF(Sous_zone_VF) la_ssz;
          const Sous_Zone& la_sous_zone=equation().probleme().domaine().ss_zone(noms_ssz_alpha[i]);
          const Domaine_dis& le_domaine_dis=la_zone_vef->domaine_dis();
          bool trouve=false;
          for (int ssz=0; ssz<le_domaine_dis.nombre_de_sous_zones_dis(); ssz++)
            {
              if (le_domaine_dis.sous_zone_dis(ssz)->sous_zone().est_egal_a(la_sous_zone))
                {
                  trouve=true;
                  la_ssz=ref_cast(Sous_zone_VF,le_domaine_dis.sous_zone_dis(ssz).valeur());
                }
            }

          if(!trouve)
            {
              Cerr << "On ne trouve pas la sous_zone discretisee associee a " << noms_ssz_alpha[i] << finl;
              Process::exit();
            }
          const Sous_zone_VF& ssz=la_ssz.valeur();
          int nb_faces = ssz.les_faces().size();

          for (int face=0; face<nb_faces; face++)
            {
              int la_face=ssz.les_faces()[face];
              beta[la_face] = 1.;
              alpha_tab[la_face] = alpha_ssz(i);
            }
        }
    }




  if (sous_zone)
    {
      sous_zone=false;
      const Sous_Zone& la_sous_zone=equation().probleme().domaine().ss_zone(nom_sous_zone);
      const Domaine_dis& le_domaine_dis=la_zone_vef->domaine_dis();
      for (int ssz=0; ssz<le_domaine_dis.nombre_de_sous_zones_dis(); ssz++)
        {
          if (le_domaine_dis.sous_zone_dis(ssz)->sous_zone().est_egal_a(la_sous_zone))
            {
              sous_zone=true;
              la_sous_zone_dis=ref_cast(Sous_zone_VF,le_domaine_dis.sous_zone_dis(ssz).valeur());
            }
        }

      if(!sous_zone)
        {
          Cerr << "On ne trouve pas la sous_zone discretisee associee a " << nom_sous_zone << finl;
          Process::exit();
        }

      const Sous_zone_VF& ssz=la_sous_zone_dis.valeur();
      int nb_faces = ssz.les_faces().size();

      for (int face=0; face<nb_faces; face++)
        {
          int la_face=ssz.les_faces()[face];
          beta[la_face] = 0.;
          alpha_tab[la_face] = 1.;
        }
    }

}

void Op_Conv_EF_VEF_P1NC_Stab::ajouter_contribution(const DoubleTab& transporte_2, Matrice_Morse& matrice) const
{
  if (new_jacobienne_==0)
    {
      Op_Conv_VEF_Face::ajouter_contribution(transporte_2, matrice) ;
      return;
    }
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  const int nb_comp=transporte_2.line_size();

  DoubleTab Kij(nb_elem_tot,nb_faces_elem,nb_faces_elem);
  Kij=0.;

  //
  //Pour tenir compte de la porosite
  //
  const Champ_P1NC& la_vitesse=ref_cast( Champ_P1NC, vitesse_.valeur());
  const DoubleTab& vitesse_2=la_vitesse.valeurs();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  DoubleTab transporte_;
  DoubleTab vitesse_face_;

  // soit on a transporte=phi*transporte_ et vitesse=vitesse_
  // soit transporte=transporte_ et vitesse=phi*vitesse_
  // cela depend si on transporte avec phi u ou avec u.
  const int marq=phi_u_transportant(equation());
  const DoubleTab& transporte=modif_par_porosite_si_flag(transporte_2,transporte_,!marq,porosite_face);
  const DoubleTab& tab_vitesse=modif_par_porosite_si_flag(vitesse_2,vitesse_face_,marq,porosite_face);

  calculer_coefficients_operateur_centre(Kij,nb_comp,tab_vitesse);
  if (is_compressible_) ajouter_contribution_partie_compressible(transporte,tab_vitesse,matrice);
  ajouter_contribution_operateur_centre(Kij,transporte,matrice);
  ajouter_contribution_diffusion(Kij,transporte,matrice);

  if (test_) test_implicite();
}

void Op_Conv_EF_VEF_P1NC_Stab::modifier_pour_Cl (Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_Conv_VEF_Face::modifier_pour_Cl(matrice,secmem);
}

void Op_Conv_EF_VEF_P1NC_Stab::ajouter_contribution_operateur_centre(const DoubleTab& Kij, const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.dimension(1);
  const int nb_bord=zone_Cl_VEF.nb_cond_lim();

  const int nb_comp=transporte.line_size();

  int elem=0, facei=0,facei_loc=0, facej=0,facej_loc=0;
  int ligne=0,colonne=0, dim=0, ind_face=0, num1=0,num2=0;
  int faceToComplete=0, elem_loc=0;
  double kij=0.,kji=0.;

  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);

        for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
          {
            facej=elem_faces(elem,facej_loc);
            assert(facej!=facei);

            kij=Kij(elem,facei_loc,facej_loc);
            kji=Kij(elem,facej_loc,facei_loc);

            for (dim=0; dim<nb_comp; dim++)
              {
                ligne=facei*nb_comp+dim;
                colonne=facej*nb_comp+dim;

                //ATTENTION AU SIGNE : ici on code +div(uT)
                matrice(ligne,ligne)+=kij;
                matrice(ligne,colonne)-=kij;
                matrice(colonne,colonne)+=kji;
                matrice(colonne,ligne)-=kji;
              }
          }
      }

  //
  //Pour la periodicite
  //
  const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1 = 0;
      num2=le_bord.nb_faces_tot();//et surtout pas nb_faces sinon on oublie certains coefficients

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int faceiAss=0,ind_faceiAss=0;

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              ind_faceiAss=la_cl_perio.face_associee(ind_face);

              facei=le_bord.num_face(ind_face);
              faceiAss=le_bord.num_face(ind_faceiAss);

              //Pour ne parcourir qu'une seule fois les faces perio
              if (facei<faceiAss)
                for (elem_loc=0; elem_loc<2; elem_loc++)
                  {
                    elem=face_voisins(facei,elem_loc);
                    assert(elem!=-1);

                    //Calcul du numero local de la face dans "elem"
                    facei_loc=num_fac_loc(facei,elem_loc);
                    if (facei_loc!=-1)
                      faceToComplete=faceiAss;
                    else
                      {
                        faceToComplete=facei;
                        facei_loc=num_fac_loc(faceiAss,elem_loc);
                        assert(facei_loc!=-1);
                      }

                    //Calcul des coefficients de la matrice dus a "elem"
                    for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
                      {
                        facej=elem_faces(elem,facej_loc);

                        if (facej_loc!=facei_loc)
                          {
                            kij=Kij(elem,facei_loc,facej_loc);
                            kji=Kij(elem,facej_loc,facei_loc);

                            for (dim=0; dim<nb_comp; dim++)
                              {
                                ligne=faceToComplete*nb_comp+dim;
                                colonne=facej*nb_comp+dim;

                                //ATTENTION AU SIGNE : ici on code +div(uT)
                                matrice(ligne,ligne)+=kij;
                                matrice(ligne,colonne)-=kij;
                              }
                          }
                      }
                  }
            }
        }
    }
}

void Op_Conv_EF_VEF_P1NC_Stab::ajouter_contribution_diffusion(const DoubleTab& Kij, const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.line_size();
  const int nb_bord=zone_Cl_VEF.nb_cond_lim();
  const int nb_comp=transporte.line_size();

  int elem=0, facei=0,facei_loc=0, facej=0,facej_loc=0;
  int ligne=0,colonne=0, dim=0, ind_face=0, num1=0,num2=0;
  int faceToComplete=0, elem_loc=0;
  double dij=0., coeffij=0.,coeffji=0.;

  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);

        for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
          {
            facej=elem_faces(elem,facej_loc);
            assert(facej!=facei);

            dij=Dij(elem,facei_loc,facej_loc,Kij);
            assert(dij>=0);

            coeffij=alpha_tab[facei]*dij;
            coeffji=alpha_tab[facej]*dij;

            for (dim=0; dim<nb_comp; dim++)
              {
                ligne=facei*nb_comp+dim;
                colonne=facej*nb_comp+dim;

                //ATTENTION AU SIGNE : ici on code +div(uT)
                //REMARQUE : on utilise la symetrie de l'operateur
                matrice(ligne,ligne)+=coeffij;
                matrice(ligne,colonne)-=coeffij;
                matrice(colonne,colonne)+=coeffji;
                matrice(colonne,ligne)-=coeffji;
              }
          }
      }

  //
  //Pour la periodicite
  //
  const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1 = 0;
      num2=le_bord.nb_faces_tot();//et surtout pas nb_faces() sinon on oublie certains coefficiens

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int faceiAss=0,ind_faceiAss=0;

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei=le_bord.num_face(ind_face);
              ind_faceiAss=la_cl_perio.face_associee(ind_face);
              faceiAss=le_bord.num_face(ind_faceiAss);

              //Pour ne parcourir qu'une seule fois les faces perio
              if (facei<faceiAss)
                for (elem_loc=0; elem_loc<2; elem_loc++)
                  {
                    elem=face_voisins(facei,elem_loc);
                    assert(elem!=-1);

                    //Calcul du numero local de la face dans "elem"
                    facei_loc=num_fac_loc(facei,elem_loc);
                    if (facei_loc!=-1)
                      faceToComplete=faceiAss;
                    else
                      {
                        faceToComplete=facei;
                        facei_loc=num_fac_loc(faceiAss,elem_loc);
                        assert(facei_loc!=-1);
                      }

                    //Calcul des coefficients de la matrice dus a "elem"
                    for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
                      {
                        facej=elem_faces(elem,facej_loc);

                        if (facej_loc!=facei_loc)
                          {
                            dij=Dij(elem,facei_loc,facej_loc,Kij);
                            assert(dij>=0);

                            coeffij=alpha_tab[faceToComplete]*dij;
                            coeffji=alpha_tab[facej]*dij;

                            for (dim=0; dim<nb_comp; dim++)
                              {
                                ligne=faceToComplete*nb_comp+dim;
                                colonne=facej*nb_comp+dim;

                                //ATTENTION AU SIGNE : ici on code +div(uT)
                                matrice(ligne,ligne)+=coeffij;
                                matrice(ligne,colonne)-=coeffij;
                              }
                          }
                      }
                  }
            }
        }
    }
}

//Correction pour le poreux : on rajoute la partie en  T div(u)
//Variable transportee : T
//Variable transportante : u
//REMARQUE : il ne FAUT SURTOUT PAS utiliser le tableau Kij car par
//construction celui-ci est telle que sum_{j} Kij =0 ce qui revient a
//imposer une vitesse a divergence nulle par element. Ce qui est
//problematique quand on est en compressible
void Op_Conv_EF_VEF_P1NC_Stab::ajouter_contribution_partie_compressible(const DoubleTab& transporte, const DoubleTab& vitesse_2,
                                                                        Matrice_Morse& matrice) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normales=zone_VEF.face_normales();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.line_size();
  const int nb_bord=zone_Cl_VEF.nb_cond_lim();

  //Pour tenir compte de la porosite
  const int marq = phi_u_transportant(equation());
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();

  DoubleTab tab_vitesse(vitesse_.valeur().valeurs());
  for (int i=0; i<tab_vitesse.dimension(0); i++)
    for (int j=0; j<tab_vitesse.line_size(); j++)
      tab_vitesse(i,j)*=porosite_face(i);

  const int nb_comp=transporte.line_size();
  int elem=0,type_elem=0, facei=0,facei_loc=0, faceiAss=0,ind_faceiAss=0;
  int ligne=0, dim=0, ind_face=0, num1=0,num2=0, faceToComplete=0, elem_loc=0, n_bord=0;
  double coeff=0., signe=0., div=0.;

  double (*formule)(int);

  if (!volumes_etendus_)
    formule= (dimension==2) ? &formule_Id_2D : &formule_Id_3D;
  else
    formule= (dimension==2) ? &formule_2D : &formule_3D;

  for (elem=0; elem<nb_elem_tot; elem++)
    {
      //Type de l'element : le nombre de faces de Dirichlet
      //qu'il contient
      type_elem=elem_nb_faces_dirichlet_(elem);
      coeff=formule(type_elem);

      //Calcul de la divergence par element
      div=0.;
      for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
        {
          facei=elem_faces(elem,facei_loc);
          signe=(face_voisins(facei,0)==elem)? 1.:-1.;

          for (dim=0; dim<dimension; dim++)
            div+=signe*face_normales(facei,dim)*tab_vitesse(facei,dim);
        }
      div*=coeff;
      if (!marq) div/=porosite_elem(elem);

      //Calcul de la partie compressible
      for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
        {
          facei=elem_faces(elem,facei_loc);

          for (dim=0; dim<nb_comp; dim++)
            {
              ligne=facei*nb_comp+dim;
              matrice(ligne,ligne)+=div;
            }
        }
    }

  //
  //Pour la periodicite
  //
  const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();
  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1 = 0;
      num2=le_bord.nb_faces();//pour ne parcourir que les faces reelles

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei=le_bord.num_face(ind_face);
              ind_faceiAss=la_cl_perio.face_associee(ind_face);
              faceiAss=le_bord.num_face(ind_faceiAss);

              //Pour ne parcourir qu'une seule fois les faces perio
              if (facei<faceiAss)
                for (elem_loc=0; elem_loc<2; elem_loc++)
                  {
                    elem=face_voisins(facei,elem_loc);
                    assert(elem!=-1);

                    //Calcul du numero local de la face dans "elem"
                    facei_loc=num_fac_loc(facei,elem_loc);
                    if (facei_loc!=-1)
                      faceToComplete=faceiAss;
                    else
                      {
                        faceToComplete=facei;
                        facei_loc=num_fac_loc(faceiAss,elem_loc);
                        assert(facei_loc!=-1);
                      }

                    //Type de l'element : le nombre de faces de Dirichlet
                    //qu'il contient
                    type_elem=elem_nb_faces_dirichlet_(elem);
                    coeff=formule(type_elem);

                    //Calcul de la divergence par element
                    div=0.;
                    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
                      {
                        facei=elem_faces(elem,facei_loc);
                        signe=(face_voisins(facei,0)==elem)? 1.:-1.;

                        for (dim=0; dim<dimension; dim++)
                          div+=signe*face_normales(facei,dim)*tab_vitesse(facei,dim);
                      }
                    div*=coeff;
                    if (!marq) div/=porosite_elem(elem);

                    //Calcul de la partie compressible
                    for (dim=0; dim<nb_comp; dim++)
                      {
                        ligne=faceToComplete*nb_comp+dim;
                        matrice(ligne,ligne)+=div;
                      }
                  }
            }
        }
    }
}

void Op_Conv_EF_VEF_P1NC_Stab::ajouter_contribution_antidiffusion(const DoubleTab& Kij, const DoubleTab& transporte,
                                                                  Matrice_Morse& matrice) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.line_size();
  const int nb_bord=zone_Cl_VEF.nb_cond_lim();
  const int nb_comp=transporte.line_size();

  int elem=0, elem_loc=0, facei=0,facei_loc=0, faceiAss=0, ind_face=0,ind_faceiAss=0;
  int facej=0,facej_loc=0, ligne=0,colonne=0, dim=0, face_amont=0,face_aval=0;
  int faceToComplete=0, num1=0,num2=0, n_bord=0;
  double kij=0.,kji=0.,dij=0., lij=0.,lji=0., daij=0.;
  double delta=0., coeffij=0.,coeffji=0., coeff=0., R=0.;

  //Pour le limiteur
  ArrOfDouble P_plus(nb_comp),P_moins(nb_comp);
  ArrOfDouble Q_plus(nb_comp),Q_moins(nb_comp);
  P_plus=0., P_moins=0., Q_plus=0., Q_moins=0.;

  const DoubleVect& transporteV = transporte;
  const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();
  for (elem=0; elem<nb_elem_tot; elem++)
    for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
      {
        facei=elem_faces(elem,facei_loc);
        P_plus=0., P_moins=0., Q_plus=0., Q_moins=0.;
        calculer_senseur(Kij,transporteV,nb_comp,facei,elem_faces,face_voisins,num_fac_loc,P_plus,P_moins,Q_plus,Q_moins);
        for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
          if (facej_loc!=facei_loc)
            {
              facej=elem_faces(elem,facej_loc);

              kij = Kij(elem,facei_loc,facej_loc);
              kji = Kij(elem,facej_loc,facei_loc);
              dij = Dij(elem,facei_loc,facej_loc,Kij);
              lij = kij+dij;
              lji = kji+dij;
              assert(lij>=0);
              assert(lji>=0);

              if (lij<=lji) //facei est amont
                {
                  face_amont = facei;
                  face_aval = facej;

                  //Si lij==lji, on passe deux foix dans la boucle
                  //d'ou la presence du coefficient 1/2
                  coeff = 1.*(lij<lji)+0.5*(lij==lji);
                  assert(coeff==1. || coeff==0.5);

                  for (dim=0; dim<nb_comp; dim++)
                    {
                      ligne=face_amont*nb_comp+dim;
                      colonne=face_aval*nb_comp+dim;

                      delta=transporteV[ligne]-transporteV[colonne];

                      //Limiteur de pente
                      // if (delta>=0.) R=(P_plus(dim)==0.) ? 0. : Q_plus(dim)/P_plus(dim);
                      // else  R=(P_moins(dim)==0.) ? 0. : Q_moins(dim)/P_moins(dim);

                      // if (delta>=0.) R=(P_plus(dim)==0.) ? 0. : Q_plus(dim)/(P_plus(dim)+DMINFLOAT);
                      // else  R=(P_moins(dim)==0.) ? 0. : Q_moins(dim)/(P_moins(dim)+DMINFLOAT);

                      if (delta>=0.) R=(std::fabs(P_plus[dim])<DMINFLOAT) ? 0. : Q_plus[dim]/P_plus[dim];
                      else  R=(std::fabs(P_moins[dim])<DMINFLOAT) ? 0. : Q_moins[dim]/P_moins[dim];


                      daij=minimum(limiteur(R)*dij,lji);
                      assert(daij>=0);
                      assert(daij<=lji);
                      coeffij=alpha_tab[face_amont]*beta[face_amont]*daij;
                      coeffji=alpha_tab[face_aval]*beta[face_aval]*daij;

                      //Calcul de la matrice
                      matrice(ligne,ligne)-=coeffij*coeff;
                      matrice(ligne,colonne)+=coeffij*coeff;
                      matrice(colonne,colonne)-=coeffji*coeff;
                      matrice(colonne,ligne)+=coeffji*coeff;
                    }
                }
            }
      }

  //
  //Pour la periodicite
  //
  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1 = 0;
      num2=le_bord.nb_faces();//pour ne parcourir que les faces reelles

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

          //Pour le limiteur
          ArrOfDouble Pj_plus(nb_comp),Pj_moins(nb_comp);
          ArrOfDouble Qj_plus(nb_comp),Qj_moins(nb_comp);
          Pj_plus=0., Pj_moins=0.;
          Qj_plus=0., Qj_moins=0.;

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei=le_bord.num_face(ind_face);
              ind_faceiAss=la_cl_perio.face_associee(ind_face);
              faceiAss=le_bord.num_face(ind_faceiAss);

              //Pour ne parcourir qu'une seule fois les faces perio
              if (facei<faceiAss)
                for (elem_loc=0; elem_loc<2; elem_loc++)
                  {
                    elem=face_voisins(facei,elem_loc);
                    assert(elem!=-1);

                    //Calcul du numero local de la face dans "elem"
                    facei_loc=num_fac_loc(facei,elem_loc);
                    if (facei_loc!=-1)
                      faceToComplete=faceiAss;
                    else
                      {
                        faceToComplete=facei;
                        facei_loc=num_fac_loc(faceiAss,elem_loc);
                        assert(facei_loc!=-1);
                      }

                    //Calcul du coefficient a rajouter dans la matrice
                    P_plus=0., P_moins=0.;
                    Q_plus=0., Q_moins=0.;
                    calculer_senseur(Kij,transporteV,nb_comp,faceToComplete,elem_faces,face_voisins,num_fac_loc,P_plus,P_moins,Q_plus,Q_moins);

                    for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
                      if (facej_loc!=facei_loc)
                        {
                          facej=elem_faces(elem,facej_loc);

                          kij = Kij(elem,facei_loc,facej_loc);
                          kji = Kij(elem,facej_loc,facei_loc);
                          dij = Dij(elem,facei_loc,facej_loc,Kij);
                          lij = kij+dij;
                          lji = kji+dij;
                          assert(lij>=0);
                          assert(lji>=0);

                          if (lij<=lji) //faceToComplete est amont
                            {
                              face_amont=faceToComplete;
                              face_aval=facej;

                              //Si lij==lji, on passe deux foix dans la boucle
                              //d'ou la presence du coefficient 1/2
                              coeff = 1.*(lij<lji)+0.5*(lij==lji);
                              assert(coeff==1. || coeff==0.5);

                              for (dim=0; dim<nb_comp; dim++)
                                {
                                  ligne=face_amont*nb_comp+dim;
                                  colonne=face_aval*nb_comp+dim;
                                  delta=transporteV[ligne]-transporteV[colonne];

                                  //Limiteur de pente
                                  // if (delta>=0.) R=(P_plus(dim)==0.) ? 0. : Q_plus(dim)/P_plus(dim);
                                  // else  R=(P_moins(dim)==0.) ? 0. : Q_moins(dim)/P_moins(dim);

                                  // if (delta>=0.) R=(P_plus(dim)==0.) ? 0. : Q_plus(dim)/(P_plus(dim)+DMINFLOAT);
                                  // else  R=(P_moins(dim)==0.) ? 0. : Q_moins(dim)/(P_moins(dim)+DMINFLOAT);

                                  if (delta>=0.) R=(std::fabs(P_plus[dim])<DMINFLOAT) ? 0. : Q_plus[dim]/P_plus[dim];
                                  else  R=(std::fabs(P_moins[dim])<DMINFLOAT) ? 0. : Q_moins[dim]/P_moins[dim];

                                  daij=minimum(limiteur(R)*dij,lji);
                                  assert(daij>=0);
                                  assert(daij<=lji);
                                  coeffij=alpha_tab[face_amont]*beta[face_amont]*daij;

                                  //Calcul de la matrice
                                  matrice(ligne,ligne)-=coeffij*coeff;
                                  matrice(ligne,colonne)+=coeffij*coeff;
                                }
                            }
                          else  //faceToComplete est aval
                            {
                              face_aval=faceToComplete;
                              face_amont=facej;
                              coeff=1.;
                              Pj_plus=0., Pj_moins=0., Qj_plus=0., Qj_moins=0.;
                              calculer_senseur(Kij,transporteV,nb_comp,facej,elem_faces,face_voisins,num_fac_loc,Pj_plus,Pj_moins,Qj_plus,Qj_moins);

                              for (dim=0; dim<nb_comp; dim++)
                                {
                                  ligne=face_amont*nb_comp+dim;
                                  colonne=face_aval*nb_comp+dim;

                                  delta=transporteV[ligne]-transporteV[colonne];

                                  //Limiteur de pente
                                  // if (delta>=0.) R=(Pj_plus(dim)==0.) ? 0. : Qj_plus(dim)/Pj_plus(dim);
                                  // else  R=(Pj_moins(dim)==0.) ? 0. : Qj_moins(dim)/Pj_moins(dim);

                                  // if (delta>=0.) R=(Pj_plus(dim)==0.) ? 0. : Qj_plus(dim)/(Pj_plus(dim)+DMINFLOAT);
                                  // else  R=(Pj_moins(dim)==0.) ? 0. : Qj_moins(dim)/(Pj_moins(dim)+DMINFLOAT);

                                  if (delta>=0.) R=(std::fabs(Pj_plus[dim])<DMINFLOAT) ? 0. : Qj_plus[dim]/Pj_plus[dim];
                                  else  R=(std::fabs(Pj_moins[dim])<DMINFLOAT) ? 0. : Qj_moins[dim]/Pj_moins[dim];

                                  daij=minimum(limiteur(R)*dij,lij);
                                  assert(daij>=0);
                                  assert(daij<=lij);
                                  coeffij=alpha_tab[face_aval]*beta[face_aval]*daij;

                                  //Calcul de la matrice
                                  matrice(colonne,colonne)-=coeffij*coeff;
                                  matrice(colonne,ligne)+=coeffij*coeff;
                                }
                            }
                        }
                  }
            }
        }
    }
}

void Op_Conv_EF_VEF_P1NC_Stab::test_implicite() const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  const DoubleTab& unknown=equation().inconnue().valeurs();
  const DoubleTab& tab_vitesse=vitesse_.valeur().valeurs();

  DoubleTab tab_test(unknown);
  DoubleVect& test2 = tab_test;
  test2 = 0.;

  DoubleTab resuExp(unknown);
  DoubleVect& resu2Exp = resuExp;
  resu2Exp = 0.;

  DoubleTab resuImp(unknown);
  DoubleVect& resu2Imp = resuImp;
  resu2Imp = 0.;

  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  const int nb_faces_tot=zone_VEF.nb_faces_tot();
  const int nb_bord=zone_Cl_VEF.nb_cond_lim();

  DoubleTab Kij(nb_elem_tot,nb_faces_elem,nb_faces_elem);
  Kij=0.;

  const int nb_comp=unknown.line_size();
  int size=unknown.dimension(0);
  int face=0,face2=0, faceAss=0, ind_face=0,ind_faceAss=0, n_bord=0, num1=0,num2=0;

  SFichier testResu("test.txt");
  SFichier testMat("matrice.txt");

  //
  //Pour la periodicite
  //
  IntTab faces_associees(nb_faces_tot);
  for (face=0; face<nb_faces_tot; face++)
    faces_associees(face)=face;

  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1 = 0;
      num2=le_bord.nb_faces_tot();//pour ne pas en oublier

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              ind_faceAss=la_cl_perio.face_associee(ind_face);
              faceAss=le_bord.num_face(ind_faceAss);

              //Pour ne parcourir que la moitie des faces periodiques
              if (face<faceAss)
                {
                  faces_associees(face)=faceAss;
                  faces_associees(faceAss)=face;
                }
            }
        }
    }
  //
  //Fin du traitement pour la periodicite
  //

  calculer_coefficients_operateur_centre(Kij,nb_comp,tab_vitesse);

  //
  //Construction de la matrice
  //
  Matrice_Morse matrice;
  dimensionner(matrice);
  if (is_compressible_)
    ajouter_contribution_partie_compressible(unknown,tab_vitesse,matrice);
  ajouter_contribution_operateur_centre(Kij,unknown,matrice);
  ajouter_contribution_diffusion(Kij,unknown,matrice);
  ajouter_contribution_antidiffusion(Kij,unknown,matrice);
  matrice.imprimer_formatte(testMat);
  //
  //Fin de la construction de la matrice
  //

  //
  //Calcul de l'operateur explicite et comparaison
  //
  for (face=0; face<size; face++)
    {
      test2[face]=1.;
      test2[faces_associees(face)]=1.;

      /* Calcul de l'operateur explicite */
      resuExp=0.;
      if (is_compressible_)
        ajouter_partie_compressible(tab_test,resuExp,tab_vitesse);
      ajouter_operateur_centre(Kij,tab_test,resuExp);
      ajouter_diffusion(Kij,tab_test,resuExp);
      ajouter_antidiffusion(Kij,tab_test,resuExp);
      mettre_a_jour_pour_periodicite(resuExp);

      /* Calcul de l'operateur implicite */
      resuImp=0.;
      matrice.ajouter_multvect_(tab_test,resuImp);

      /* Calcul de la difference */
      resuExp+=resuImp;

      /* Affichage de la difference */
      testResu<<"*************************"<<finl;
      testResu<<"Face test : "<<face<<finl;
      for (face2=0; face2<size; face2++)
        if (resu2Exp[face2]<=1.e-13)
          testResu<<face2<<" OK"<<finl;
        else
          testResu<<face2<<" residu : "<<resu2Exp[face2]<<finl;
      testResu<<"*************************"<<finl;

      test2[face]=0.;
      test2[faces_associees(face)]=0.;
    }
  //
  //Fin du calcul de l'operateur explicite et comparaison
  //

  Process::exit();
}

