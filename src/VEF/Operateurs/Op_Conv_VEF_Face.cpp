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
// File:        Op_Conv_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/46
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_VEF_Face.h>
#include <Champ_P1NC.h>
#include <Porosites_champ.h>
#include <Debog.h>
#include <stat_counters.h>
Implemente_instanciable_sans_constructeur(Op_Conv_VEF_Face,"Op_Conv_Generic_VEF_P1NC",Op_Conv_VEF_base);



//////////////////////////////////////////////////////////////
//   Fonctions limiteurs de MUSCL
////////////////////////////////////////////////////////////////
// Inlining inutile car ces fonctions sont appelees dynamiquement
// Pour optimiser, passer par une macro ?

double minmod(double grad1, double grad2)
{
  double gradlim=0.;
  if(grad1*grad2>0.) (dabs(grad1)<dabs(grad2)) ? gradlim=grad1 : gradlim=grad2 ;
  return gradlim;
}

double vanleer(double grad1, double grad2)
{
  double gradlim=0.;
  if(grad1*grad2>0.) gradlim=2.*grad1*grad2/(grad1+grad2) ;
  return gradlim;
}

double vanalbada(double grad1, double grad2)
{
  double gradlim=0.;
  if(grad1*grad2>0.) gradlim=grad1*grad2*(grad1+grad2)/(grad1*grad1+grad2*grad2) ;
  return gradlim;
}


double chakravarthy(double grad1, double grad2)
{
  /*
    Cerr << " limiteur chakavarthy non preconise (non symetrique) " << finl;
    exit();
    return 0;
  */
  double gradlim=0.;
  if ((grad1*grad2)>0)
    {
      gradlim=dmin(grad1/grad2,1.8); // 1<<beta<<2
      gradlim=dmax(gradlim,0.);
      gradlim*=grad2;
    }
  return gradlim;
}

double superbee(double grad1, double grad2)
{
  /*
    Cerr << " limiteur superbee non preconise (source d'instabilites) " << finl;
    exit();
    return 0;
  */
  double gradlim=0.;
  if ((grad1*grad2)>0)
    {
      double gradlim1,gradlim2;
      gradlim1=dmin(2*(grad1/grad2),1);
      gradlim2=dmin(grad1/grad2,2);
      gradlim=dmax(gradlim1,gradlim2);
      gradlim=dmax(gradlim,0.);
      gradlim*=grad2;
    }
  return gradlim;
}

//// printOn
//
Sortie& Op_Conv_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//
Entree& Op_Conv_VEF_Face::readOn(Entree& s )
{
  Motcle type_op_lu;
  s >> type_op_lu;

  if (!(type_op_lu=="amont") && !(type_op_lu=="muscl") && !(type_op_lu=="centre"))
    {
      Cerr << type_op_lu << " n'est pas compris par " << que_suis_je() << finl;
      Cerr << " choisir parmi : amont - muscl - centre " << finl;
      exit();
    }

  if (type_op_lu=="muscl")
    {
      type_op = muscl;
      s >> type_lim;

      if ( !(type_lim=="minmod") && !(type_lim=="vanleer") && !(type_lim=="vanalbada")
           &&  !(type_lim=="chakravarthy") && !(type_lim=="superbee") )
        {
          Cerr << type_lim << " n'est pas compris par " << que_suis_je() << finl;
          Cerr << " choisir parmi : minmod - vanleer - vanalbada - chakravarthy - superbee " << finl;
          exit();
        }

      if (type_lim=="minmod") LIMITEUR=&minmod;
      if (type_lim=="vanleer") LIMITEUR=&vanleer;
      if (type_lim=="vanalbada") LIMITEUR=&vanalbada;
      if (type_lim=="chakravarthy") LIMITEUR=&chakravarthy;
      if (type_lim=="superbee") LIMITEUR=&superbee;

      s >> ordre;

      if (ordre!=1 && ordre!=2 && ordre!=3)
        {
          Cerr << "l'ordre apres " << type_lim << " dans " << que_suis_je() << " doit etre soit 1, soit 2, soit 3" <<  finl;
          exit();
        }
      if (ordre==3)
        {
          // Lecture de alpha_
          s >> alpha_;
        }
    }

  if (type_op_lu=="centre")
    {
      type_op = centre;
      s >> ordre;

      if (ordre!=1 && ordre!=2)
        {
          Cerr << "l'ordre apres " << type_op_lu << " dans " << que_suis_je() << " doit etre soit 1, soit 2 " <<  finl;
          exit();
        }
    }

  if (type_op_lu=="amont")
    {
      type_op = amont;
      ordre = 1;
    }

  return s ;
}

//
//   Fonctions de la classe Op_Conv_VEF_Face
//
////////////////////////////////////////////////////////////////////
//
//                      Implementation des fonctions
//
//                   de la classe Op_Conv_VEF_Face
//
////////////////////////////////////////////////////////////////////




DoubleTab& Op_Conv_VEF_Face::ajouter(const DoubleTab& transporte,
                                     DoubleTab& resu) const
{
  //statistiques().begin_count(m1);
  assert((type_op==amont) || (type_op==muscl) || (type_op==centre));
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = ref_cast(Zone_VEF, la_zone_vef.valeur());
  const Champ_Inc_base& la_vitesse=vitesse();
  const DoubleTab& vitesse_face_absolue=la_vitesse.valeurs();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();

  int marq=phi_u_transportant(equation());
  DoubleTab transporte_face_;
  DoubleTab vitesse_face_;
  // soit on a transporte_face=phi*transporte et vitesse_face=vitesse
  // soit on a transporte_face=transporte et vitesse_face=phi*vitesse
  // cela depend si on transporte avec phi*u ou avec u.
  const DoubleTab& transporte_face = modif_par_porosite_si_flag(transporte,transporte_face_,!marq,porosite_face);
  const DoubleTab& vitesse_face    = modif_par_porosite_si_flag(vitesse_face_absolue,vitesse_face_,marq,porosite_face);

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleTab& facenormales = zone_VEF.face_normales();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const Zone& zone = zone_VEF.zone();
  const int nfa7 = zone_VEF.type_elem().nb_facette();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();
  int premiere_face_int = zone_VEF.premiere_face_int();
  int nfac = zone.nb_faces_elem();
  int nsom = zone.nb_som_elem();
  const IntTab& sommet_elem = zone.les_elems();
  const DoubleTab& vecteur_face_facette = ref_cast_non_const(Zone_VEF,zone_VEF).vecteur_face_facette();
  const  DoubleTab& vecteur_face_facette_Cl = zone_Cl_VEF.vecteur_face_facette_Cl();
  int nb_bord = zone_VEF.nb_front_Cl();
  const IntTab& les_elems=zone.les_elems();

  // Permet d'avoir un flux_bord coherent avec les CLs (mais parfois diverge?)
  // Active uniquement pour ordre 3
  int option_appliquer_cl_dirichlet = (ordre==3 ? 1 : 0);
  int option_calcul_flux_en_un_point = 0;//(ordre==3 ? 1 : 0);
  // Definition d'un tableau pour un traitement special des schemas pres des bords
  if (traitement_pres_bord_.size_array()!=nb_elem_tot)
    {
      traitement_pres_bord_.resize_array(nb_elem_tot);
      traitement_pres_bord_=0;
      // Pour muscl3 on applique le minmod sur les elements ayant une face de Dirichlet
      if (ordre==3)
        {
          for (int n_bord=0; n_bord<nb_bord; n_bord++)
            {
              const Cond_lim_base& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord).valeur();
              if ( sub_type(Dirichlet,la_cl) || sub_type(Dirichlet_homogene,la_cl) )
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  int nb_faces_tot = le_bord.nb_faces_tot();
                  for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
                    {
                      int num_face = le_bord.num_face(ind_face);
                      int elem = face_voisins(num_face,0);
                      traitement_pres_bord_(elem)=1;
                    }
                }
            }
        }
      else
        {
          // Pour le muscl/centre actuels on utilise un calcul de flux a l'ordre 1
          // aux mailles de bord ou aux mailles ayant un sommet de Dirichlet
          ArrOfInt est_un_sommet_de_bord_(zone_VEF.nb_som_tot());
          for (int n_bord=0; n_bord<nb_bord; n_bord++)
            {
              const Cond_lim_base& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord).valeur();
              if ( sub_type(Dirichlet,la_cl) || sub_type(Dirichlet_homogene,la_cl) )
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  int nb_faces_tot = le_bord.nb_faces_tot();
                  int size = zone_VEF.face_sommets().dimension(1);
                  for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
                    for (int som=0; som<size; som++)
                      {
                        int face = le_bord.num_face(ind_face);
                        est_un_sommet_de_bord_(zone_VEF.face_sommets(face,som))=1;
                      }
                }
            }
          for (int elem=0; elem<nb_elem_tot; elem++)
            {
              if (rang_elem_non_std(elem)!=-1)
                traitement_pres_bord_(elem)=1;
              else
                {
                  for (int n_som=0; n_som<nsom; n_som++)
                    if (est_un_sommet_de_bord_(les_elems(elem,n_som)))
                      traitement_pres_bord_(elem)=1;
                }
            }
        }
      // Construction du tableau est_une_face_de_dirichlet_
      est_une_face_de_dirichlet_.resize_array(zone_VEF.nb_faces_tot());
      est_une_face_de_dirichlet_=0;
      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim_base& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord).valeur();
          if ( sub_type(Dirichlet,la_cl) || sub_type(Dirichlet_homogene,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int nb_faces_tot = le_bord.nb_faces_tot();
              for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
                {
                  int num_face = le_bord.num_face(ind_face);
                  est_une_face_de_dirichlet_(num_face) = 1;
                }
            }
        }
    }

  // Pour le traitement de la convection on distingue les polyedres
  // standard qui ne "voient" pas les conditions aux limites et les
  // polyedres non standard qui ont au moins une face sur le bord.
  // Un polyedre standard a n facettes sur lesquelles on applique le
  // schema de convection.
  // Pour un polyedre non standard qui porte des conditions aux limites
  // de Dirichlet, une partie des facettes sont portees par les faces.
  // En bref pour un polyedre le traitement de la convection depend
  // du type (triangle, tetraedre ...) et du nombre de faces de Dirichlet.

  const Elem_VEF_base& type_elemvef= zone_VEF.type_elem().valeur();
  int istetra=0;
  Nom nom_elem=type_elemvef.que_suis_je();
  if ((nom_elem=="Tetra_VEF")||(nom_elem=="Tri_VEF"))
    istetra=1;

  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  double psc;
  int poly,face_adj,fa7,i,j,n_bord;
  int num_face, rang;
  int num10,num20,num_som;
  int ncomp_ch_transporte=(transporte_face.nb_dim() == 1?1:transporte_face.dimension(1));

  // Traitement particulier pour les faces de periodicite
  int nb_faces_perio = 0;
  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          nb_faces_perio+=le_bord.nb_faces();
        }
    }

  DoubleTab tab;
  if (ncomp_ch_transporte == 1)
    tab.resize(nb_faces_perio);
  else
    tab.resize(nb_faces_perio,ncomp_ch_transporte);

  nb_faces_perio=0;
  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              if (ncomp_ch_transporte == 1)
                tab(nb_faces_perio) = resu(num_face);
              else
                for (int comp=0; comp<ncomp_ch_transporte; comp++)
                  tab(nb_faces_perio,comp) = resu(num_face,comp);
              nb_faces_perio++;
            }
        }
    }

  int fac=0,elem1,elem2,comp0;
  int nb_faces_ = zone_VEF.nb_faces();
  ArrOfInt face(nfac);
  //statistiques().end_count(m1);
  //statistiques().begin_count(m2);

  // Tableau gradient base sur gradient_elem selon schema
  DoubleTab gradient_elem(nb_elem_tot,ncomp_ch_transporte,dimension);  // (du/dx du/dy dv/dx dv/dy) pour un poly
  if(type_op==centre || type_op==muscl)
    {
      Champ_P1NC::calcul_gradient(transporte_face,gradient_elem,zone_Cl_VEF);
    }
  DoubleTab gradient;
  if (type_op==centre)
    {
      gradient.ref(gradient_elem);
    }
  else if(type_op==muscl)
    {
      //  application du limiteur
      gradient.resize(0, ncomp_ch_transporte, dimension);     // (du/dx du/dy dv/dx dv/dy) pour une face
      zone_VEF.creer_tableau_faces(gradient);
      for (n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          if (sub_type(Periodique,la_cl.valeur()))
            {
              for (fac=num1; fac<num2; fac++)
                {
                  elem1=face_voisins(fac,0);
                  elem2=face_voisins(fac,1);
                  for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                    for (i=0; i<dimension; i++)
                      {
                        double grad1=gradient_elem(elem1, comp0, i);
                        double grad2=gradient_elem(elem2, comp0, i);
                        gradient(fac, comp0, i) =(*LIMITEUR)(grad1, grad2);
                      }
                }
            }
          else if (sub_type(Symetrie,la_cl.valeur()))
            {
              for (fac=num1; fac<num2; fac++)
                {
                  elem1=face_voisins(fac,0);
                  for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                    for (i=0; i<dimension; i++)
                      gradient(fac, comp0, i) = gradient_elem(elem1, comp0, i);

                  if (ordre==3)
                    {
                      // On enleve la composante normale (on pourrait le faire pour les autres schemas...)
                      // mais pour le moment, on ne veut pas changer le comportement par defaut du muscl...
                      //const DoubleTab& facenormales = zone_VEF.face_normales();
                      for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                        for (i=0; i<dimension; i++)
                          {
                            double carre_surface=0;
                            double tmp=0;
                            for (j=0; j<dimension; j++)
                              {
                                double ndS=facenormales(fac,j);
                                carre_surface += ndS*ndS;
                                tmp += gradient(fac, comp0, j)*ndS;
                              }
                            gradient(fac, comp0, i) -= tmp*facenormales(fac,i)/carre_surface;
                          }
                    }
                }
            }
        }

      for (fac=premiere_face_int; fac<nb_faces_; fac++)
        {
          elem1=face_voisins(fac,0);
          elem2=face_voisins(fac,1);
          int minmod_pres_du_bord = 0;
          if (ordre==3 && (traitement_pres_bord_(elem1) || traitement_pres_bord_(elem2))) minmod_pres_du_bord = 1;
          for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
            for (i=0; i<dimension; i++)
              {
                double grad1=gradient_elem(elem1, comp0, i);
                double grad2=gradient_elem(elem2, comp0, i);
                if (minmod_pres_du_bord)
                  gradient(fac, comp0, i) = minmod(grad1, grad2);
                else
                  gradient(fac, comp0, i) = (*LIMITEUR)(grad1, grad2);
              }
        } // fin du for faces
      gradient.echange_espace_virtuel();
    }// fin if(type_op==muscl)

  ArrOfDouble vs(dimension);
  ArrOfDouble vc(dimension);
  ArrOfDouble cc(dimension);
  DoubleVect xc(dimension);
  DoubleTab vsom(nsom,dimension);
  DoubleTab xsom(nsom,dimension);

  // Dimensionnement du tableau des flux convectifs au bord du domaine de calcul
  DoubleTab& flux_b = ref_cast(DoubleTab,flux_bords_);
  int nb_faces_bord=zone_VEF.nb_faces_bord();
  flux_b.resize(nb_faces_bord,ncomp_ch_transporte);
  flux_b = 0.;

  const IntTab& KEL=type_elemvef.KEL();
  const DoubleTab& xv=zone_VEF.xv();
  const Domaine& domaine=zone.domaine();
  const DoubleTab& coord_sommets=domaine.coord_sommets();

  // Boucle ou non selon la valeur de alpha (uniquement a l'ordre 3 pour le moment)
  // Si alpha=1, la boucle se limite a une simple passe avec le schema choisi (muscl, amont, centre)
  // Si alpha<1, la boucle se compose de 2 passes:
  //                         -la premiere avec le schema choisi et une ponderation de alpha
  //                         -la seconde avec le schema centre et une ponderation de 1-alpha
  double alpha = alpha_;
  int nombre_passes = (alpha==1 ? 1 : 2);
  for (int passe=1; passe<=nombre_passes; passe++)
    {
      type_operateur type_op_boucle = type_op;
      if (passe==2)
        {
          type_op_boucle = centre;
          gradient.ref(gradient_elem);
        }
      // Les polyedres non standard sont ranges en 2 groupes dans la Zone_VEF:
      //  - polyedres bords et joints
      //  - polyedres bords et non joints
      // On traite les polyedres en suivant l'ordre dans lequel ils figurent
      // dans la zone
      // boucle sur les polys
      for (poly=0; poly<nb_elem_tot; poly++)
        {
          int contrib = 0;
          // calcul des numeros des faces du polyedre
          for (face_adj=0; face_adj<nfac; face_adj++)
            {
              int face_ = elem_faces(poly,face_adj);
              face(face_adj)= face_;
              if (face_<nb_faces_) contrib=1; // Une face reelle sur l'element virtuel
            }
          //
          if (contrib)
            {
              int calcul_flux_en_un_point = (ordre != 3) && (ordre==1 || traitement_pres_bord_(poly));
              for (j=0; j<dimension; j++)
                {
                  vs(j) = vitesse_face_absolue(face(0),j)*porosite_face(face(0));
                  for (i=1; i<nfac; i++)
                    vs(j)+= vitesse_face_absolue(face(i),j)*porosite_face(face(i));
                }
              // calcul de la vitesse aux sommets des polyedres
              // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
              if (istetra==1)
                {
                  for (i=0; i<nsom; i++)
                    for (j=0; j<dimension; j++)
                      vsom(i,j) = (vs(j) - dimension*vitesse_face_absolue(face(i),j)*porosite_face(face(i)));
                }
              else
                {
                  // pour que cela soit valide avec les hexa (c'est + lent a calculer...)
                  for (j=0; j<nsom; j++)
                    {
                      num_som = sommet_elem(poly,j);
                      for (int ncomp=0; ncomp<dimension; ncomp++)
                        vsom(j,ncomp) = la_vitesse.valeur_a_sommet_compo(num_som,poly,ncomp);
                    }
                }

              // Determination du type de CL selon le rang
              rang = rang_elem_non_std(poly);
              int itypcl = (rang==-1 ? 0 : zone_Cl_VEF.type_elem_Cl(rang));

              // calcul de vc (a l'intersection des 3 facettes) vc vs vsom proportionnelles a la porosite
              type_elemvef.calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);

              // calcul de xc (a l'intersection des 3 facettes) necessaire pour muscl3
              if (ordre==3)
                {
                  int idirichlet;
                  int n1,n2,n3;
                  for (i=0; i<nsom; i++)
                    for (j=0; j<dimension; j++)
                      xsom(i,j) = coord_sommets(les_elems(poly,i),j);
                  type_elemvef.calcul_xg(xc,xsom,itypcl,idirichlet,n1,n2,n3);
                }

              // Gestion de la porosite
              if (marq==0)
                {
                  double coeff=1./porosite_elem(poly);
                  vsom*=coeff;
                  vc*=coeff;
                }
              // Boucle sur les facettes du polyedre non standard:
              for (fa7=0; fa7<nfa7; fa7++)
                {
                  num10 = face(KEL(0,fa7));
                  num20 = face(KEL(1,fa7));
                  // normales aux facettes
                  if (rang==-1)
                    for (i=0; i<dimension; i++)
                      cc[i] = facette_normales(poly, fa7, i);
                  else
                    for (i=0; i<dimension; i++)
                      cc[i] = normales_facettes_Cl(rang,fa7,i);

                  // Calcul des vitesses en C,S,S2 les 3 extremites de la fa7 et M le centre de la fa7
                  double psc_c=0,psc_s=0,psc_m,psc_s2=0;
                  if (dimension==2)
                    {
                      for (i=0; i<2; i++)
                        {
                          psc_c+=vc[i]*cc[i];
                          psc_s+=vsom(KEL(2,fa7),i)*cc[i];
                        }
                      psc_m=(psc_c+psc_s)/2.;
                    }
                  else
                    {
                      for (i=0; i<3; i++)
                        {
                          psc_c+=vc[i]*cc[i];
                          psc_s+=vsom(KEL(2,fa7),i)*cc[i];
                          psc_s2+=vsom(KEL(3,fa7),i)*cc[i];
                        }
                      psc_m=(psc_c+psc_s+psc_s2)/3.;
                    }
                  // On applique les CL de Dirichlet si num1 ou num2 est une face avec CL de Dirichlet
                  // auquel cas la fa7 coincide avec la face num1 ou num2 -> C est au centre de la face
                  int appliquer_cl_dirichlet=0;
                  if (option_appliquer_cl_dirichlet)
                    if (est_une_face_de_dirichlet_(num10) || est_une_face_de_dirichlet_(num20))
                      {
                        appliquer_cl_dirichlet = 1;
                        psc_m = psc_c;
                      }

                  // Determination de la face amont pour M
                  int face_amont_m,dir;
                  if (psc_m >= 0)
                    {
                      face_amont_m = num10;
                      dir=0;
                    }
                  else
                    {
                      face_amont_m = num20;
                      dir=1;
                    }
                  // Determination des faces amont pour les points C,S,S2
                  int face_amont_c=face_amont_m;
                  int face_amont_s=face_amont_m;
                  int face_amont_s2=face_amont_m;
                  if (type_op_boucle==muscl && ordre==3)
                    {
                      face_amont_c  = (psc_c >= 0)  ? num10 : num20;
                      face_amont_s  = (psc_s >= 0)  ? num10 : num20;
                      face_amont_s2 = (psc_s2 >= 0) ? num10 : num20;
                    }
                  // gradient aux items element (schema centre) ou aux items face (schemas muscl)
                  int item_m=poly;
                  int item_c=poly;
                  int item_s=poly;
                  int item_s2=poly;
                  if (type_op_boucle==muscl)
                    {
                      item_m = face_amont_m;
                      item_c = face_amont_c;
                      item_s = face_amont_s;
                      item_s2 = face_amont_s2;
                    }

                  for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                    {
                      double flux;
                      double inco_m = (ncomp_ch_transporte==1?transporte_face(face_amont_m):transporte_face(face_amont_m,comp0));
                      if (type_op_boucle==amont || appliquer_cl_dirichlet)
                        {
                          flux = inco_m*psc_m;
                        }
                      else // muscl ou centre
                        {
                          // Calcul de l'inconnue au centre M de la fa7
                          if (rang==-1)
                            for (j=0; j<dimension; j++)
                              inco_m+= gradient(item_m,comp0,j)*vecteur_face_facette(poly,fa7,j,dir);
                          else
                            for (j=0; j<dimension; j++)
                              inco_m+= gradient(item_m,comp0,j)*vecteur_face_facette_Cl(rang,fa7,j,dir);

                          // Calcul de l'inconnue au sommet S, une premiere extremite de la fa7
                          double inco_s = (ncomp_ch_transporte==1?transporte_face(face_amont_s):transporte_face(face_amont_s,comp0));
                          int sommet_s = sommet_elem(poly,KEL(2,fa7));
                          for (j=0; j<dimension; j++)
                            inco_s+= gradient(item_s,comp0,j)*(-xv(face_amont_s,j)+coord_sommets(sommet_s,j));

                          // Calcul de l'inconnue au sommet S2, la derniere extremite de la fa7 en 3D
                          double inco_s2=0;
                          if (dimension==3)
                            {
                              inco_s2 = (ncomp_ch_transporte==1?transporte_face(face_amont_s2):transporte_face(face_amont_s2,comp0));
                              int sommet_s2 = sommet_elem(poly,KEL(3,fa7));
                              for (j=0; j<dimension; j++)
                                inco_s2+= gradient(item_s2,comp0,j)*(-xv(face_amont_s2,j)+coord_sommets(sommet_s2,j));
                            }

                          // Calcul de l'inconnue a C, une autre extremite de la fa7, intersection avec les autres fa7
                          // du polyedre. C=G centre du polyedre si volume non etendu
                          // xc donne par elemvef.calcul_xg()
                          double inco_c;
                          if (ordre==3)
                            {
                              inco_c = (ncomp_ch_transporte==1?transporte_face(face_amont_c):transporte_face(face_amont_c,comp0));
                              for (j=0; j<dimension; j++)
                                inco_c+= gradient(item_c,comp0,j)*(-xv(face_amont_c,j)+xc(j));
                            }
                          else
                            {
                              inco_c = dimension*inco_m-inco_s-inco_s2;
                            }

                          // Calcul du flux sur 1 point
                          if (calcul_flux_en_un_point || option_calcul_flux_en_un_point)
                            {
                              flux = inco_m*psc_m;
                            }
                          else
                            {
                              // Calcul du flux sur 3 points
                              flux = (dimension==2) ? (inco_c*psc_c + inco_s*psc_s + 4*inco_m*psc_m)/6
                                     : (inco_c*psc_c + inco_s*psc_s + inco_s2*psc_s2 + 9*inco_m*psc_m)/12;
                            }
                        }

                      // Ponderation par coefficient alpha
                      flux*=alpha;

                      if (ncomp_ch_transporte == 1)
                        {
                          resu(num10) -= flux;
                          resu(num20) += flux;
                          if (num10<nb_faces_bord) flux_b(num10,0) += flux;
                          if (num20<nb_faces_bord) flux_b(num20,0) -= flux;
                        }
                      else
                        {
                          resu(num10,comp0) -= flux;
                          resu(num20,comp0) += flux;
                          if (num10<nb_faces_bord) flux_b(num10,comp0) += flux;
                          if (num20<nb_faces_bord) flux_b(num20,comp0) -= flux;
                        }

                    }// boucle sur comp
                } // fin de la boucle sur les facettes
            }
        } // fin de la boucle
      alpha = 1 - alpha;
    } // fin de la boucle
  //statistiques().end_count(m2);
  //statistiques().begin_count(m3);
  int voisine;
  nb_faces_perio = 0;
  double diff1,diff2;

  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement
  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += vitesse_face(num_face,i)*facenormales(num_face,i);
              if (psc>0)
                if (ncomp_ch_transporte == 1)
                  {
                    resu(num_face) -= psc*transporte_face(num_face);
                    flux_b(num_face,0) = -psc*transporte_face(num_face);
                  }
                else
                  for (i=0; i<ncomp_ch_transporte; i++)
                    {
                      resu(num_face,i) -= psc*transporte_face(num_face,i);
                      flux_b(num_face,i) = -psc*transporte_face(num_face,i);
                    }
              else
                {
                  if (ncomp_ch_transporte == 1)
                    {
                      resu(num_face) -= psc*la_sortie_libre.val_ext(num_face-num1);
                      flux_b(num_face,0) = -psc*la_sortie_libre.val_ext(num_face-num1);
                    }
                  else
                    for (i=0; i<ncomp_ch_transporte; i++)
                      {
                        resu(num_face,i) -= psc*la_sortie_libre.val_ext(num_face-num1,i);
                        flux_b(num_face,i) = -psc*la_sortie_libre.val_ext(num_face-num1,i);
                      }
                }
            }
        }
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          ArrOfInt fait(le_bord.nb_faces());
          fait = 0;
          for (num_face=num1; num_face<num2; num_face++)
            {
              if (fait[num_face-num1] == 0)
                {
                  voisine = la_cl_perio.face_associee(num_face-num1) + num1;

                  if (ncomp_ch_transporte == 1)
                    {
                      diff1 = resu(num_face)-tab(nb_faces_perio);
                      diff2 = resu(voisine)-tab(nb_faces_perio+voisine-num_face);
                      resu(voisine)  += diff1;
                      resu(num_face) += diff2;
                      /* On ne doit pas ajouter a flux_b, c'est deja calcule au dessus
                         flux_b(voisine,0) += diff1;
                         flux_b(num_face,0) += diff2;*/
                    }
                  else
                    for (int comp=0; comp<ncomp_ch_transporte; comp++)
                      {
                        diff1 = resu(num_face,comp)-tab(nb_faces_perio,comp);
                        diff2 = resu(voisine,comp)-tab(nb_faces_perio+voisine-num_face,comp);
                        resu(voisine,comp)  += diff1;
                        resu(num_face,comp) += diff2;
                        /* On ne doit pas ajouter a flux_b, c'est deja calcule au dessus
                           flux_b(voisine,comp) += diff1;
                           flux_b(num_face,comp) += diff2; */
                      }

                  fait[num_face-num1]= 1;
                  fait[voisine-num1] = 1;
                }
              nb_faces_perio++;
            }
        }
    }

  modifier_flux(*this);
  //statistiques().end_count(m3);
  return resu;
}

// methodes recuperes de l'ancien OpVEFFaAmont
inline void convbisimplicite_dec(const double& psc_, const int num1, const int num2,
                                 const DoubleTab& transporte, const int ncomp,
                                 DoubleVect& coeff, Matrice_Morse& matrice,
                                 const DoubleVect& porosite_face, int phi_u_transportant)
{
  double psc=psc_;
  if (psc>=0)
    {
      if (!phi_u_transportant)
        psc*=porosite_face(num1);
      for (int comp=0; comp<ncomp; comp++)
        {
          int n0=num1*ncomp+comp;
          int j0=num2*ncomp+comp;
          matrice(n0,n0)+=psc;
          matrice(j0,n0)-=psc;
        }
    }
  else
    {
      if (!phi_u_transportant)
        psc*=porosite_face(num2);
      for (int comp=0; comp<ncomp; comp++)
        {
          int n0=num1*ncomp+comp;
          int j0=num2*ncomp+comp;
          matrice(n0,j0)+=psc;
          matrice(j0,j0)-=psc;
        }
    }
}

void Op_Conv_VEF_Face::ajouter_contribution(const DoubleTab& transporte, Matrice_Morse& matrice ) const
{
  modifier_matrice_pour_periodique_avant_contribuer(matrice,equation());
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse();
  const DoubleTab& vitesse_face_absolue=la_vitesse.valeurs();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const Zone& zone = zone_VEF.zone();
  const Elem_VEF& type_elem = zone_VEF.type_elem();
  const int nfa7 = type_elem.nb_facette();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();


  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();

  int nfac = zone.nb_faces_elem();
  int nsom = zone.nb_som_elem();
  const IntTab& sommet_elem = zone.les_elems();

  // Pour le traitement de la convection on distingue les polyedres
  // standard qui ne "voient" pas les conditions aux limites et les
  // polyedres non standard qui ont au moins une face sur le bord.
  // Un polyedre standard a n facettes sur lesquelles on applique le
  // schema de convection.
  // Pour un polyedre non standard qui porte des conditions aux limites
  // de Dirichlet, une partie des facettes sont portees par les faces.
  // En bref pour un polyedre le traitement de la convection depend
  // du type (triangle, tetraedre ...) et du nombre de faces de Dirichlet.

  double psc;
  DoubleTab pscl=0;
  int poly,face_adj,fa7,i,j,n_bord;
  int num_face, rang ,itypcl;
  int num10,num20,num_som;
  int ncomp_ch_transporte;
  if (transporte.nb_dim() == 1)
    ncomp_ch_transporte=1;
  else
    ncomp_ch_transporte= transporte.dimension(1);


  int marq=phi_u_transportant(equation());

  DoubleTab vitesse_face_;
  // soit on a transporte=phi*transporte_ et vitesse_face=vitesse_
  // soit transporte=transporte_ et vitesse_face=phi*vitesse_
  // cela depend si on transporte avec phi u ou avec u.
  const DoubleTab& vitesse_face=modif_par_porosite_si_flag(vitesse_face_absolue,vitesse_face_,marq,porosite_face);
  ArrOfInt face(nfac);
  ArrOfDouble vs(dimension);
  ArrOfDouble vc(dimension);
  DoubleTab vsom(nsom,dimension);
  ArrOfDouble cc(dimension);
  DoubleVect& coeff = matrice.get_set_coeff();
  const Elem_VEF_base& type_elemvef= zone_VEF.type_elem().valeur();
  int istetra=0;
  Nom nom_elem=type_elemvef.que_suis_je();
  if ((nom_elem=="Tetra_VEF")||(nom_elem=="Tri_VEF"))
    istetra=1;


  // Les polyedres non standard sont ranges en 2 groupes dans la Zone_VEF:
  //  - polyedres bords et joints
  //  - polyedres bords et non joints
  // On traite les polyedres en suivant l'ordre dans lequel ils figurent
  // dans la zone

  // boucle sur les polys
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();
  int phi_u_transportant_yes=phi_u_transportant(equation());
  for (poly=0; poly<nb_elem_tot; poly++)
    {

      rang = rang_elem_non_std(poly);
      if (rang==-1)
        itypcl=0;
      else
        itypcl=zone_Cl_VEF.type_elem_Cl(rang);

      // calcul des numeros des faces du polyedre
      for (face_adj=0; face_adj<nfac; face_adj++)
        face(face_adj)= elem_faces(poly,face_adj);

      for (j=0; j<dimension; j++)
        {
          vs(j) = vitesse_face_absolue(face(0),j)*porosite_face(face(0));
          for (i=1; i<nfac; i++)
            vs(j)+= vitesse_face_absolue(face(i),j)*porosite_face(face(i));
        }
      // calcul de la vitesse aux sommets des polyedres
      // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
      if (istetra==1)
        {
          for (i=0; i<nsom; i++)
            for (j=0; j<dimension; j++)
              vsom(i,j) = (vs(j) - dimension*vitesse_face_absolue(face(i),j)*porosite_face(face(i)));
        }
      else
        {
          // pour que cela soit valide avec les hexa (c'est + lent a calculer...)
          int ncomp;
          for (j=0; j<nsom; j++)
            {
              num_som = sommet_elem(poly,j);
              for (ncomp=0; ncomp<dimension; ncomp++)
                vsom(j,ncomp) = la_vitesse.valeur_a_sommet_compo(num_som,poly,ncomp);
            }
        }


      // calcul de vc (a l'intersection des 3 facettes) vc vs vsom proportionnelles a la prosite
      type_elemvef.calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);
      if (marq==0)
        {
          double porosite_poly=porosite_elem(poly);
          for (i=0; i<nsom; i++)
            for (j=0; j<dimension; j++)
              vsom(i,j)/=porosite_poly;
          for (j=0; j<dimension; j++)
            {
              vs(j)/= porosite_elem(poly);
              vc(j)/=porosite_elem(poly);
            }
        }
      // Boucle sur les facettes du polyedre non standard:
      for (fa7=0; fa7<nfa7; fa7++)
        {
          num10 = face(KEL(0,fa7));
          num20 = face(KEL(1,fa7));
          // normales aux facettes
          if (rang==-1)
            {
              for (i=0; i<dimension; i++)
                cc[i] = facette_normales(poly, fa7, i);
            }
          else
            {
              for (i=0; i<dimension; i++)
                cc[i] = normales_facettes_Cl(rang,fa7,i);
            }
          // On applique le schema de convection a chaque sommet de la facette


          double psc_c=0,psc_s=0,psc_m,psc_s2=0;
          if (dimension==2)
            {
              for (i=0; i<dimension; i++)
                {
                  psc_c+=vc[i]*cc[i];
                  psc_s+=vsom(KEL(2,fa7),i)*cc[i];
                }
              psc_m=(psc_c+psc_s)/2.;
            }
          else
            {
              for (i=0; i<dimension; i++)
                {
                  psc_c+=vc[i]*cc[i];
                  psc_s+=vsom(KEL(2,fa7),i)*cc[i];
                  psc_s2+=vsom(KEL(3,fa7),i)*cc[i];
                }
              psc_m=(psc_c+psc_s+psc_s2)/3.;
            }
          convbisimplicite_dec(psc_m,num10,num20,transporte,ncomp_ch_transporte,coeff,matrice,porosite_face,phi_u_transportant_yes);
        } // fin de boucle sur les facettes.

    } // fin de boucle sur les polyedres.

  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement
  int nb_bord = zone_VEF.nb_front_Cl();
  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {

      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += vitesse_face(num_face,i)*face_normales(num_face,i);
              if (!phi_u_transportant_yes)
                psc*=porosite_face(num_face);
              if (psc>0)
                {
                  for (j=0; j<ncomp_ch_transporte; j++)
                    {
                      int n0=num_face*ncomp_ch_transporte+j;
                      matrice(n0,n0)+=psc;
                    }
                }
            }
        }
    }
  modifier_matrice_pour_periodique_apres_contribuer(matrice,equation());
}

void Op_Conv_VEF_Face::contribue_au_second_membre(DoubleTab& resu ) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const Zone& zone = zone_VEF.zone();
  int nfac = zone.nb_faces_elem();

  double psc;
  int i,n_bord;
  int num_face;
  int ncomp;
  if (resu.nb_dim() == 1)
    ncomp=1;
  else
    ncomp= resu.dimension(1);

  ArrOfInt face(nfac);


  // Traitement particulier pour les faces de periodicite

  int nb_faces_perio = 0;
  int voisine;
  double diff1,diff2;

  // Boucle pour compter le nombre de faces de periodicite
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          nb_faces_perio += le_bord.nb_faces();
        }
    }

  DoubleTab tab;
  if (ncomp == 1)
    tab.resize(nb_faces_perio);
  else
    tab.resize(nb_faces_perio,ncomp);

  // Boucle pour remplir tab
  nb_faces_perio=0;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              if (ncomp == 1)
                tab(nb_faces_perio) = resu(num_face);
              else
                for (int comp=0; comp<ncomp; comp++)
                  tab(nb_faces_perio,comp) = resu(num_face,comp);
              nb_faces_perio++;
            }
        }
    }
  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement
  nb_faces_perio=0;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += la_vitesse(num_face,i)*face_normales(num_face,i);
              if (psc>0)
                if (ncomp == 1)
                  resu(num_face) += 0;
                else
                  for (i=0; i<ncomp; i++)
                    resu(num_face,i) += 0;
              else
                {
                  if (ncomp == 1)
                    resu(num_face) -= psc*la_sortie_libre.val_ext(num_face-num1);
                  else
                    for (i=0; i<ncomp; i++)
                      resu(num_face,i) -= psc*la_sortie_libre.val_ext(num_face-num1,i);
                }
            }
        }
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          ArrOfInt fait(le_bord.nb_faces());
          fait = 0;
          for (num_face=num1; num_face<num2; num_face++)
            {
              if (fait[num_face-num1] == 0)
                {
                  voisine = la_cl_perio.face_associee(num_face-num1) + num1;

                  if (ncomp == 1)
                    {
                      diff1 = resu(num_face)-tab(nb_faces_perio);
                      diff2 = resu(voisine)-tab(nb_faces_perio+voisine-num_face);
                      resu(voisine)  += diff1;
                      resu(num_face) += diff2;
                    }
                  else
                    for (int comp=0; comp<ncomp; comp++)
                      {
                        diff1 = resu(num_face,comp)-tab(nb_faces_perio,comp);
                        diff2 = resu(voisine,comp)-tab(nb_faces_perio+voisine-num_face,comp);
                        resu(voisine,comp)  += diff1;
                        resu(num_face,comp) += diff2;
                      }

                  fait[num_face-num1]= 1;
                  fait[voisine-num1] = 1;
                }
              nb_faces_perio++;
            }
        }
    }
}



void  Op_Conv_VEF_Face::remplir_fluent(DoubleVect& tab_fluent) const
{

  assert((type_op==amont) || (type_op==muscl) || (type_op==centre));
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = ref_cast(Zone_VEF, la_zone_vef.valeur());
  const Champ_Inc_base& la_vitesse=vitesse();
  const DoubleTab& vitesse_face_absolue=la_vitesse.valeurs();

  int marq=phi_u_transportant(equation());
  // on force a calculer un pas de temps sans "porosite"
  marq=0;

  const DoubleVect& porosite_face = zone_VEF.porosite_face();


  const DoubleTab& vitesse_face=vitesse_face_absolue;

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const Zone& zone = zone_VEF.zone();
  const int nfa7 = zone_VEF.type_elem().nb_facette();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();
  int nfac = zone.nb_faces_elem();
  int nsom = zone.nb_som_elem();
  const IntTab& sommet_elem = zone.les_elems();
  DoubleVect& fluent_ = ref_cast(DoubleVect, tab_fluent);

  // On definit le tableau des sommets:(C MALOD 17/07/2007)

  // Pour le traitement de la convection on distingue les polyedres
  // standard qui ne "voient" pas les conditions aux limites et les
  // polyedres non standard qui ont au moins une face sur le bord.
  // Un polyedre standard a n facettes sur lesquelles on applique le
  // schema de convection.
  // Pour un polyedre non standard qui porte des conditions aux limites
  // de Dirichlet, une partie des facettes sont portees par les faces.
  // En bref pour un polyedre le traitement de la convection depend
  // du type (triangle, tetraedre ...) et du nombre de faces de Dirichlet.

  const Elem_VEF_base& type_elemvef= zone_VEF.type_elem().valeur();
  int istetra=0;
  Nom nom_elem=type_elemvef.que_suis_je();
  if ((nom_elem=="Tetra_VEF")||(nom_elem=="Tri_VEF"))
    istetra=1;
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();

  double psc;
  int poly,face_adj,fa7,i,j,n_bord;
  int num_face, rang ,itypcl;
  int num1,num2,num_som;

  ArrOfInt face(nfac);
  ArrOfDouble vs(dimension);
  ArrOfDouble vc(dimension);
  DoubleTab vsom(nsom,dimension);
  ArrOfDouble cc(dimension);

  // Dimensionnement du tableau des flux convectifs au bord du domaine de calcul
  const IntTab& KEL=type_elemvef.KEL();

  // On remet a zero le tableau qui sert pour
  // le calcul du pas de temps de stabilite
  fluent_ = 0;

  // Les polyedres non standard sont ranges en 2 groupes dans la Zone_VEF:
  //  - polyedres bords et joints
  //  - polyedres bords et non joints
  // On traite les polyedres en suivant l'ordre dans lequel ils figurent
  // dans la zone

  // boucle sur les polys
  for (poly=0; poly<nb_elem_tot; poly++)
    {
      rang = rang_elem_non_std(poly);
      // On cherche, pour un elem qui n'est pas de bord (rang==-1),
      // si un des sommets est sur un bord (tableau des sommets) (C MALOD 17/07/2007)

      if (rang==-1)
        itypcl=0;
      else
        itypcl=zone_Cl_VEF.type_elem_Cl(rang);

      // calcul des numeros des faces du polyedre
      for (face_adj=0; face_adj<nfac; face_adj++)
        face(face_adj)= elem_faces(poly,face_adj);

      for (j=0; j<dimension; j++)
        {
          vs(j) = vitesse_face_absolue(face(0),j)*porosite_face(face(0));
          for (i=1; i<nfac; i++)
            vs(j)+= vitesse_face_absolue(face(i),j)*porosite_face(face(i));
        }
      // calcul de la vitesse aux sommets des polyedres
      // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
      if (istetra==1)
        {
          for (i=0; i<nsom; i++)
            for (j=0; j<dimension; j++)
              vsom(i,j) = (vs(j) - dimension*vitesse_face_absolue(face(i),j)*porosite_face(face(i)));
        }
      else
        {
          // pour que cela soit valide avec les hexa (c'est + lent a calculer...)
          int ncomp;
          for (j=0; j<nsom; j++)
            {
              num_som = sommet_elem(poly,j);
              for (ncomp=0; ncomp<dimension; ncomp++)
                vsom(j,ncomp) = la_vitesse.valeur_a_sommet_compo(num_som,poly,ncomp);
            }
        }

      // calcul de vc (a l'intersection des 3 facettes) vc vs vsom proportionnelles a la prosite
      type_elemvef.calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);
      if (marq==0)
        {
          double porosite_poly=porosite_elem(poly);
          for (i=0; i<nsom; i++)
            for (j=0; j<dimension; j++)
              vsom(i,j)/=porosite_poly;
          for (j=0; j<dimension; j++)
            {
              vs(j)/= porosite_poly;
              vc(j)/=porosite_poly;
            }
        }
      // Boucle sur les facettes du polyedre non standard:
      for (fa7=0; fa7<nfa7; fa7++)
        {
          num1 = face(KEL(0,fa7));
          num2 = face(KEL(1,fa7));
          // normales aux facettes
          if (rang==-1)
            {
              for (i=0; i<dimension; i++)
                cc[i] = facette_normales(poly, fa7, i);
            }
          else
            {
              for (i=0; i<dimension; i++)
                cc[i] = normales_facettes_Cl(rang,fa7,i);
            }
          // On applique le schema de convection a chaque sommet de la facette

          double psc_c=0,psc_s=0,psc_m,psc_s2=0;
          if (dimension==2)
            {
              for (i=0; i<dimension; i++)
                {
                  psc_c+=vc[i]*cc[i];
                  psc_s+=vsom(KEL(2,fa7),i)*cc[i];
                }
              psc_m=(psc_c+psc_s)/2.;
            }
          else
            {
              for (i=0; i<dimension; i++)
                {
                  psc_c+=vc[i]*cc[i];
                  psc_s+=vsom(KEL(2,fa7),i)*cc[i];
                  psc_s2+=vsom(KEL(3,fa7),i)*cc[i];
                }
              psc_m=(psc_c+psc_s+psc_s2)/3.;
            }

          // int amont,dir;
          if (psc_m >= 0)
            {
              // amont = num1;
              fluent_(num2)  += psc_m;
              //dir=0;
            }
          else
            {
              //amont = num2;
              fluent_(num1)  -= psc_m;
              //dir=1;
            }

        } // fin de la boucle sur les facettes
    } // fin de la boucle

  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement
  int nb_bord = zone_VEF.nb_front_Cl();
  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1b = le_bord.num_premiere_face();
          int num2b = num1b + le_bord.nb_faces();
          for (num_face=num1b; num_face<num2b; num_face++)
            {
              psc = 0;
              for (i=0; i<dimension; i++)
                psc += vitesse_face(num_face,i)*face_normales(num_face,i);
              if (psc>0)
                ;
              else
                fluent_(num_face) -= psc;
            }
        }
    }
}
