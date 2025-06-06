/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Op_Conv_VEF_Face.h>
#include <Champ_P1NC.h>
#include <Porosites_champ.h>

#include <stat_counters.h>
#include <Convection_tools.h>
#include <Dirichlet_homogene.h>
#include <Periodique.h>
#include <Symetrie.h>
#include <Neumann_sortie_libre.h>
#include <TRUSTVect.h>
#include <Device.h>
#include <Tetra_VEF.h>

Implemente_instanciable_sans_constructeur(Op_Conv_VEF_Face,"Op_Conv_Generic_VEF_P1NC",Op_Conv_VEF_base);
// XD convection_generic convection_deriv generic 0 Keyword for generic calling of upwind and muscl convective scheme in VEF discretization. For muscl scheme, limiters and order for fluxes calculations have to be specified. The available limiters are : minmod - vanleer -vanalbada - chakravarthy - superbee, and the order of accuracy is 1 or 2. Note that chakravarthy is a non-symmetric limiter and superbee may engender results out of physical limits. By consequence, these two limiters are not recommended. NL2 Examples: NL2 convection { generic amont }NL2 convection { generic muscl minmod 1 }NL2 convection { generic muscl vanleer 2 }NL2 NL2 In case of results out of physical limits with muscl scheme (due for instance to strong non-conformal velocity flow field), user can redefine in data file a lower order and a smoother limiter, as : convection { generic muscl minmod 1 }
// XD   attr type chaine(into=["amont","muscl","centre"]) type 0 type of scheme
// XD   attr limiteur chaine(into=["minmod","vanleer","vanalbada","chakravarthy","superbee"]) limiteur 1 type of limiter
// XD   attr ordre entier(into=[1,2,3]) ordre 1 order of accuracy
// XD   attr alpha floattant alpha 1 alpha

Sortie& Op_Conv_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

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

      if (type_lim=="minmod")
        {
          LIMITEUR=&minmod;
          type_lim_int = type_lim_minmod;
        }
      if (type_lim=="vanleer")
        {
          LIMITEUR=&vanleer;
          type_lim_int = type_lim_vanleer;
        }
      if (type_lim=="vanalbada")
        {
          LIMITEUR=&vanalbada;
          type_lim_int = type_lim_vanalbada;
        }
      if (type_lim=="chakravarthy")
        {
          LIMITEUR=&chakravarthy;
          type_lim_int = type_lim_chakravarthy;
        }
      if (type_lim=="superbee")
        {
          LIMITEUR=&superbee;
          type_lim_int = type_lim_superbee;
        }
      s >> ordre_;

      if (ordre_!=1 && ordre_!=2 && ordre_!=3)
        {
          Cerr << "l'ordre apres " << type_lim << " dans " << que_suis_je() << " doit etre soit 1, soit 2, soit 3" <<  finl;
          exit();
        }
      if (ordre_==3)
        {
          // Lecture de alpha_
          s >> alpha_;
        }
    }

  if (type_op_lu=="centre")
    {
      type_op = centre;
      s >> ordre_;

      if (ordre_!=1 && ordre_!=2)
        {
          Cerr << "l'ordre apres " << type_op_lu << " dans " << que_suis_je() << " doit etre soit 1, soit 2 " <<  finl;
          exit();
        }
    }

  if (type_op_lu=="amont")
    {
      type_op = amont;
      ordre_ = 1;
    }

  return s ;
}

void Op_Conv_VEF_Face::completer()
{
  Op_Conv_VEF_base::completer();
  // On cherche, pour un elem qui n'est pas de bord (rang==-1),
  // si un des sommets est sur un bord (tableau des sommets) (C MALOD 17/07/2007)
  if (type_elem_Cl_.size_array() == 0)
    {
      const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, le_dom_vef.valeur());
      const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
      const int nb_elem_tot = domaine_VEF.nb_elem_tot();
      const IntVect& rang_elem_non_std = domaine_VEF.rang_elem_non_std();
      type_elem_Cl_.resize(nb_elem_tot);
      for (int poly = 0; poly < nb_elem_tot; poly++)
        {
          int rang = rang_elem_non_std(poly);
          type_elem_Cl_[poly] = rang == -1 ? 0 : domaine_Cl_VEF.type_elem_Cl(rang);
        }
      // Appel a vecteur_face_facette() des le completer:
      Cerr << "Build of vecteur_face_facette() size:" << ref_cast_non_const(Domaine_VEF,domaine_VEF).vecteur_face_facette().size_array() << finl;
    }
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
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, le_dom_vef.valeur());
  const Champ_Inc_base& la_vitesse=vitesse();
  const DoubleTab& vitesse_face_absolue=la_vitesse.valeurs();
  const DoubleVect& porosite_face = equation().milieu().porosite_face();

  int marq=phi_u_transportant(equation());
  DoubleTab transporte_face_;
  DoubleTab vitesse_face_;
  // soit on a transporte_face=phi*transporte et vitesse_face=vitesse
  // soit on a transporte_face=transporte et vitesse_face=phi*vitesse
  // cela depend si on transporte avec phi*u ou avec u.
  const DoubleTab& transporte_face = modif_par_porosite_si_flag(transporte,transporte_face_,!marq,porosite_face);
  const DoubleTab& vitesse_face    = modif_par_porosite_si_flag(la_vitesse.valeurs(),vitesse_face_,marq,porosite_face);

  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const DoubleTab& facette_normales = domaine_VEF.facette_normales();
  const Domaine& domaine = domaine_VEF.domaine();
  const int nfa7 = domaine_VEF.type_elem().nb_facette();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = domaine_VEF.rang_elem_non_std();
  const DoubleTab& normales_facettes_Cl = domaine_Cl_VEF.normales_facettes_Cl();
  int premiere_face_int = domaine_VEF.premiere_face_int();
  int nb_faces = domaine_VEF.nb_faces();
  int nfac = domaine.nb_faces_elem();
  int nsom = domaine.nb_som_elem();
  const DoubleTab& vecteur_face_facette = ref_cast_non_const(Domaine_VEF,domaine_VEF).vecteur_face_facette();
  const DoubleTab& vecteur_face_facette_Cl = domaine_Cl_VEF.vecteur_face_facette_Cl();
  int nb_bord = domaine_VEF.nb_front_Cl();
  const IntTab& les_elems=domaine.les_elems();

  // Permet d'avoir un flux_bord coherent avec les CLs (mais parfois diverge?)
  // Active uniquement pour ordre 3
  int option_appliquer_cl_dirichlet = (ordre_==3 ? 1 : 0);
  int option_calcul_flux_en_un_point = 0;//(ordre==3 ? 1 : 0);
  // Definition d'un tableau pour un traitement special des schemas pres des bords
  if (traitement_pres_bord_.size_array()!=nb_elem_tot)
    {
      traitement_pres_bord_.resize_array(nb_elem_tot);
      traitement_pres_bord_=0;
      // Pour muscl3 on applique le minmod sur les elements ayant une face de Dirichlet
      if (ordre_==3)
        {
          for (int n_bord=0; n_bord<nb_bord; n_bord++)
            {
              const Cond_lim_base& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord).valeur();
              if ( sub_type(Dirichlet,la_cl) || sub_type(Dirichlet_homogene,la_cl) )
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  int nb_faces_tot = le_bord.nb_faces_tot();
                  const IntTab& face_voisins = domaine_VEF.face_voisins();
                  for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
                    {
                      int num_face = le_bord.num_face(ind_face);
                      int elem = face_voisins(num_face,0);
                      traitement_pres_bord_[elem]=1;
                    }
                }
            }
        }
      else
        {
          // Pour le muscl/centre actuels on utilise un calcul de flux a l'ordre 1
          // aux mailles de bord ou aux mailles ayant un sommet de Dirichlet
          ArrOfInt est_un_sommet_de_bord_(domaine_VEF.nb_som_tot());
          for (int n_bord=0; n_bord<nb_bord; n_bord++)
            {
              const Cond_lim_base& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord).valeur();
              if ( sub_type(Dirichlet,la_cl) || sub_type(Dirichlet_homogene,la_cl) )
                {
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  int nb_faces_tot = le_bord.nb_faces_tot();
                  int size = domaine_VEF.face_sommets().dimension(1);
                  for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
                    for (int som=0; som<size; som++)
                      {
                        int face = le_bord.num_face(ind_face);
                        est_un_sommet_de_bord_[domaine_VEF.face_sommets(face,som)]=1;
                      }
                }
            }
          for (int elem=0; elem<nb_elem_tot; elem++)
            {
              if (rang_elem_non_std(elem)!=-1)
                traitement_pres_bord_[elem]=1;
              else
                {
                  for (int n_som=0; n_som<nsom; n_som++)
                    if (est_un_sommet_de_bord_[les_elems(elem,n_som)])
                      traitement_pres_bord_[elem]=1;
                }
            }
        }
      // Construction du tableau est_une_face_de_dirichlet_
      est_une_face_de_dirichlet_.resize_array(domaine_VEF.nb_faces_tot());
      est_une_face_de_dirichlet_=0;
      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim_base& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord).valeur();
          if ( sub_type(Dirichlet,la_cl) || sub_type(Dirichlet_homogene,la_cl) )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int nb_faces_tot = le_bord.nb_faces_tot();
              for (int ind_face=0; ind_face<nb_faces_tot; ind_face++)
                {
                  int num_face = le_bord.num_face(ind_face);
                  est_une_face_de_dirichlet_[num_face] = 1;
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

  const Elem_VEF_base& type_elemvef= domaine_VEF.type_elem();
  int istetra=0;
  Nom nom_elem=type_elemvef.que_suis_je();
  if ((nom_elem=="Tetra_VEF")||(nom_elem=="Tri_VEF"))
    istetra=1;

  const DoubleVect& porosite_elem = equation().milieu().porosite_elem();
  int ncomp_ch_transporte=(transporte_face.nb_dim() == 1?1:transporte_face.dimension(1));

  // Traitement particulier pour les faces de periodicite
  DoubleTrav resu_perio;
  int nb_faces_perio = 0;
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          nb_faces_perio+=le_bord.nb_faces();
        }
    }
  if (nb_faces_perio>0)
    {
      resu_perio.resize(nb_faces_perio,ncomp_ch_transporte);
      nb_faces_perio=0;
      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
          if (sub_type(Periodique,la_cl.valeur()))
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              CDoubleTabView resu_v = resu.view_ro();
              DoubleTabView resu_perio_v = resu_perio.view_wo();
              Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(num1, num2), KOKKOS_LAMBDA(const int num_face)
              {
                int ind_face = nb_faces_perio + num_face - num1;
                for (int comp = 0; comp < ncomp_ch_transporte; comp++)
                  resu_perio_v(ind_face, comp) = resu_v(num_face, comp);
              });
              end_gpu_timer(__KERNEL_NAME__);
              nb_faces_perio+=num2-num1;
            }
        }
    }

  DoubleTab tab_gradient; // Peut pointer vers gradient_elem ou gradient_face selon schema
  if(type_op==centre || type_op==muscl)
    {
      // Tableau gradient base sur gradient_elem selon schema
      if (gradient_elem_.size_array() == 0) gradient_elem_.resize(nb_elem_tot, ncomp_ch_transporte, dimension);  // (du/dx du/dy dv/dx dv/dy) pour un poly
      Champ_P1NC::calcul_gradient(transporte_face, gradient_elem_, domaine_Cl_VEF);

      if (type_op == centre)
        {
          tab_gradient.ref(gradient_elem_);
        }
      else if (type_op == muscl)
        {
          int cas = 1;
          if (type_lim_int == type_lim_minmod) cas = 1;
          if (type_lim_int == type_lim_vanleer) cas = 2;
          if (type_lim_int == type_lim_vanalbada) cas = 3;
          if (type_lim_int == type_lim_chakravarthy) cas = 4;
          if (type_lim_int == type_lim_superbee) cas = 5;
          //  application du limiteur
          if (!gradient_face_.get_md_vector().non_nul())
            {
              gradient_face_.resize(0, ncomp_ch_transporte, dimension);     // (du/dx du/dy dv/dx dv/dy) pour une face
              domaine_VEF.creer_tableau_faces(gradient_face_);
            }
          for (int n_bord = 0; n_bord < nb_bord; n_bord++)
            {
              const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
              int num1 = le_bord.num_premiere_face();
              int num2 = num1 + le_bord.nb_faces();
              if (sub_type(Symetrie, la_cl.valeur()))
                {
                  int ordre = ordre_;
                  int dim = dimension;
                  CIntTabView face_voisins = domaine_VEF.face_voisins().view_ro();
                  CDoubleTabView face_normale = domaine_VEF.face_normales().view_ro();
                  CDoubleTabView3 gradient_elem = gradient_elem_.view_ro<3>();
                  DoubleTabView3 gradient_face = gradient_face_.view_wo<3>();
                  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), range_1D(num1, num2), KOKKOS_LAMBDA(const int fac)
                  {
                    int elem1 = face_voisins(fac, 0);
                    for (int comp0 = 0; comp0 < ncomp_ch_transporte; comp0++)
                      for (int i = 0; i < dim; i++)
                        gradient_face(fac, comp0, i) = gradient_elem(elem1, comp0, i);

                    if (ordre == 3)
                      {
                        // On enleve la composante normale (on pourrait le faire pour les autres schemas...)
                        // mais pour le moment, on ne veut pas changer le comportement par defaut du muscl...
                        for (int comp0 = 0; comp0 < ncomp_ch_transporte; comp0++)
                          for (int i = 0; i < dim; i++)
                            {
                              double carre_surface = 0;
                              double tmp = 0;
                              for (int j = 0; j < dim; j++)
                                {
                                  double ndS = face_normale(fac, j);
                                  carre_surface += ndS * ndS;
                                  tmp += gradient_face(fac, comp0, j) * ndS;
                                }
                              gradient_face(fac, comp0, i) -= tmp * face_normale(fac, i) / carre_surface;
                            }
                      }
                  });
                  end_gpu_timer(__KERNEL_NAME__);
                }
            }
          tab_gradient.ref(gradient_face_);
          int dim = dimension;
          int ordre = ordre_;
          CIntTabView face_voisins = domaine_VEF.face_voisins().view_ro();
          CIntArrView traitement_pres_bord = traitement_pres_bord_.view_ro();
          CIntArrView faces_doubles = domaine_VEF.faces_doubles().view_ro();
          CDoubleTabView3 gradient_elem = gradient_elem_.view_ro<3>();
          DoubleTabView3 gradient = tab_gradient.view_wo<3>();
#ifdef TRUST_USE_GPU
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), range_2D({0,0}, {nb_faces, ncomp_ch_transporte}), KOKKOS_LAMBDA(const int fac, const int comp0)
#else
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), range_1D(0, nb_faces), KOKKOS_LAMBDA(const int fac)
#endif
          {
            if (faces_doubles[fac] /* face perio */ || fac>=premiere_face_int /* face interne */)
              {
                int elem1 = face_voisins(fac, 0);
                int elem2 = face_voisins(fac, 1);
                int limiteur = cas;
                if (ordre == 3 && (traitement_pres_bord[elem1] || traitement_pres_bord[elem2]))
                  limiteur = 1;
#ifndef TRUST_USE_GPU
                for (int comp0 = 0; comp0 < ncomp_ch_transporte; comp0++)
#endif
                  for (int i = 0; i < dim; i++)
                    {
                      double grad1 = gradient_elem(elem1, comp0, i);
                      double grad2 = gradient_elem(elem2, comp0, i);
                      gradient(fac, comp0, i) = FCT_LIMITEUR(grad1, grad2, limiteur);
                    }
              }
          });
          end_gpu_timer(__KERNEL_NAME__);
          tab_gradient.echange_espace_virtuel(); // Pas possible de supprimer. Garder le Kernel sur le CPU n'apporte pas.
        }// fin if(type_op==muscl)
    }

  // Dimensionnement du tableau des flux convectifs au bord du domaine de calcul
  DoubleTab& flux_b = flux_bords_;
  int nb_faces_bord=domaine_VEF.nb_faces_bord();
  flux_b.resize(nb_faces_bord,ncomp_ch_transporte);
  // No, lock with DomainFlow test case:
  //if (flux_b.dimension(0)!=nb_faces_bord) flux_b.resize(nb_faces_bord,ncomp_ch_transporte);
  flux_b = 0.;

  const IntTab& KEL=type_elemvef.KEL();
  const DoubleTab& xv=domaine_VEF.xv();
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
          tab_gradient.ref(gradient_elem_);
        }
      // Les polyedres non standard sont ranges en 2 groupes dans le Domaine_VEF:
      //  - polyedres bords et joints
      //  - polyedres bords et non joints
      // On traite les polyedres en suivant l'ordre dans lequel ils figurent
      // dans le domaine
      // boucle sur les polys
      if(nom_elem=="Tetra_VEF")
        {
          CIntArrView rang_elem_non_std_v = rang_elem_non_std.view_ro();
          CIntTabView elem_faces_v = elem_faces.view_ro();
          CDoubleArrView porosite_face_v = porosite_face.view_ro();
          CDoubleArrView porosite_elem_v = porosite_elem.view_ro();
          CDoubleTabView coord_sommets_v = coord_sommets.view_ro();
          CIntTabView les_elems_v = les_elems.view_ro();
          CDoubleTabView3 facette_normales_v = facette_normales.view_ro<3>();
          CIntArrView est_une_face_de_dirichlet_v = est_une_face_de_dirichlet_.view_ro();
          // ToDo suppress one day: Domaine_VEF::vecteur_face_facette() -20% in RAM
          // and implement a vecteur_face_fa7(poly, face, dir) method
          //CDoubleTabView4 vecteur_face_facette_v = vecteur_face_facette.view_ro<4>();
          CDoubleTabView xp_v = domaine_VEF.xp().view_ro();
          CDoubleTabView xv_v = xv.view_ro();
          CIntArrView type_elem_Cl_v = type_elem_Cl_.view_ro();
          CIntArrView traitement_pres_bord_v = traitement_pres_bord_.view_ro();
          CIntTabView KEL_v = type_elemvef.KEL().view_ro();
          CDoubleTabView3 normales_facettes_Cl_v = normales_facettes_Cl.view_ro<3>();
          CDoubleTabView4 vecteur_face_facette_Cl_v = vecteur_face_facette_Cl.view_ro<4>();

          CDoubleTabView vitesse_v = la_vitesse.valeurs().view_ro();
          CDoubleTabView vitesse_face_absolue_v = vitesse_face_absolue.view_ro();
          CDoubleTabView transporte_face_v = transporte_face.view_ro();
          CDoubleTabView3 gradient_v = tab_gradient.view_ro<3>();

          DoubleTabView resu_v = resu.view_rw();
          DoubleTabView flux_b_v = flux_b.view_rw();

          const int dim = Objet_U::dimension;
          const int nb_som_facette = dim;
          const bool isMuscl = type_op_boucle == muscl;
          const bool isAmont = type_op_boucle == amont;
          const int ordre = ordre_;

          if (getenv("TRUST_USE_SCRATCH_MEMORY")==nullptr)
            {
              // Example where MDRangePolicy should be used only on GPU ! RangePolicy should be used in serial else +40% overhead on Cx test case...
#ifdef TRUST_USE_GPU
              Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::MDRangePolicy<Kokkos::Rank<2>>({0,0}, {nb_elem_tot, nfa7}), KOKKOS_LAMBDA(const int poly, const int fa7)
#else
              Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(0, nb_elem_tot), KOKKOS_LAMBDA(const int poly)
#endif
              {
                int rang = rang_elem_non_std_v(poly);
                int contrib = 0;
                // calcul des numeros des faces du polyedre
                int face[4] {};
                for (int face_adj = 0; face_adj < nfac; face_adj++)
                  {
                    int fac = elem_faces_v(poly, face_adj);
                    face[face_adj] = fac;
                    if (fac < nb_faces) contrib = 1; // Une face reelle sur l'element virtuel
                  }
                //
                if (contrib)
                  {
                    int calcul_flux_en_un_point = (ordre != 3) && (ordre == 1 || traitement_pres_bord_v(poly));
                    bool flux_en_un_point = calcul_flux_en_un_point || option_calcul_flux_en_un_point;
                    double vs[3] {0.0};
                    for (int j = 0; j < dim; j++)
                      for (int i = 0; i < nfac; i++)
                        vs[j] += vitesse_face_absolue_v(face[i], j) * porosite_face_v(face[i]);
                    // calcul de la vitesse aux sommets des tetraedres
                    // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
                    double vsom[12] {};
                    for (int i = 0; i < nsom; i++)
                      for (int j = 0; j < dim; j++)
                        vsom[i * 3 + j] = vs[j] - dim * vitesse_face_absolue_v(face[i], j) * porosite_face_v(face[i]);

                    // Determination du type de CL selon le rang
                    rang = rang_elem_non_std_v(poly);
                    True_int itypcl = type_elem_Cl_v(poly);

                    double vc[3] {};
                    calcul_vc_tetra_views(face, vc, vs, vsom, vitesse_v, itypcl, porosite_face_v);
                    // calcul de xc (a l'intersection des 3 facettes) necessaire pour muscl3
                    double xc[3] {};
                    if (ordre == 3) // A optimiser! Risque de mauvais resultats en parallel si ordre=3
                      {
                        double xsom[12] {};
                        for (int i = 0; i < nsom; i++)
                          for (int j = 0; j < dim; j++)
                            xsom[i * 3 + j] = coord_sommets_v(les_elems_v(poly, i), j);
                        int idirichlet, n1, n2, n3;
                        calcul_xg_tetra(xc, xsom, itypcl, idirichlet, n1, n2, n3);
                      }

                    // Gestion de la porosite
                    if (marq == 0)
                      {
                        double coeff = 1. / porosite_elem_v(poly);
                        for (int l = 0; l < nsom * dim; l++) vsom[l] *= coeff;
                        for (int l = 0; l < dim; l++) vc[l] *= coeff;
                      }
                    // Boucle sur les facettes du polyedre:
                    double centre_fa7[3] {};
                    double cc[3] {};
#ifndef TRUST_USE_GPU
                    for (int fa7 = 0; fa7 < nfa7; fa7++)
#endif
                      {
                        int num10 = face[KEL_v(0, fa7)];
                        int num20 = face[KEL_v(1, fa7)];
                        // normales aux facettes
                        if (rang == -1)
                          for (int i = 0; i < dim; i++)
                            cc[i] = facette_normales_v(poly, fa7, i);
                        else
                          for (int i = 0; i < dim; i++)
                            cc[i] = normales_facettes_Cl_v(rang, fa7, i);

                        // Calcul des vitesses en C,S,S2 les 3 extremites de la fa7 et M le centre de la fa7
                        double psc_c = 0, psc_s = 0, psc_s2 = 0;
                        for (int i = 0; i < dim; i++)
                          {
                            psc_c += vc[i] * cc[i];
                            psc_s += vsom[KEL_v(2, fa7) * dim + i] * cc[i];
                            psc_s2 += (dim==2 ? 0 : vsom[KEL_v(3, fa7) * dim + i] * cc[i]);
                          }
                        double psc_m = (psc_c + psc_s + psc_s2) / dim;
                        // On applique les CL de Dirichlet si num1 ou num2 est une face avec CL de Dirichlet
                        // auquel cas la fa7 coincide avec la face num1 ou num2 -> C est au centre de la face
                        int appliquer_cl_dirichlet = 0;
                        if (option_appliquer_cl_dirichlet)
                          if (est_une_face_de_dirichlet_v(num10) || est_une_face_de_dirichlet_v(num20))
                            {
                              appliquer_cl_dirichlet = 1;
                              psc_m = psc_c;
                            }

                        // Determination des faces amont pour les points M,C,S,S2
                        int face_amont_m = (psc_m >= 0) ? num10 : num20;
                        int face_amont_c = (isMuscl && ordre == 3) ? ((psc_c >= 0) ? num10 : num20) : face_amont_m;
                        int face_amont_s = (isMuscl && ordre == 3) ? ((psc_s >= 0) ? num10 : num20) : face_amont_m;
                        int face_amont_s2 = (isMuscl && ordre == 3) ? ((psc_s2 >= 0) ? num10 : num20) : face_amont_m;

                        // gradient aux items element (schema centre) ou aux items face (schemas muscl)
                        int item_m = isMuscl ? face_amont_m : poly;
                        int item_c = isMuscl ? face_amont_c : poly;
                        int item_s = isMuscl ? face_amont_s : poly;
                        int item_s2 = isMuscl ? face_amont_s2 : poly;

                        int dir = (psc_m >= 0) ? 0 : 1;
                        int num_face = elem_faces_v(poly, KEL_v(dir, fa7));
                        if (rang==-1)
                          {
                            for (int j = 0; j < dim; j++)
                              {
                                centre_fa7[j] = xp_v(poly, j);
                                for (int num_som_fa7 = 0; num_som_fa7 < nb_som_facette - 1; num_som_fa7++)
                                  {
                                    int isom_loc = KEL_v(num_som_fa7 + 2, fa7);
                                    int isom_glob = les_elems_v(poly, isom_loc);
                                    centre_fa7[j] += coord_sommets_v(isom_glob, j);
                                  }
                                centre_fa7[j] /= nb_som_facette;
                              }
                          }
                        bool flux_avec_m = isAmont || appliquer_cl_dirichlet;
                        for (int comp0 = 0; comp0 < ncomp_ch_transporte; comp0++)
                          {
                            double inco_m = transporte_face_v(face_amont_m, comp0);
                            double flux;
                            if (flux_avec_m) // amont
                              flux = inco_m * psc_m;
                            else // muscl ou centre
                              {
                                // PL: data locality matters ! Try to access arrays one by one and store into registers
                                double inco_s = transporte_face_v(face_amont_s, comp0);
                                double inco_s2 = dim == 3 ? transporte_face_v(face_amont_s2, comp0) : 0;
                                int sommet_s = les_elems_v(poly, KEL_v(2, fa7));
                                int sommet_s2 = dim == 3 ? les_elems_v(poly, KEL_v(3, fa7)) : -1;
                                for (int j = 0; j < dim; j++)
                                  {
                                    double gm = gradient_v(item_m, comp0, j);
                                    double gs = gradient_v(item_s, comp0, j);
                                    double gs2 = (dim == 3 ? gradient_v(item_s2, comp0, j) : 0);
                                    double xv_m = xv_v(num_face, j);
                                    double xv_s = xv_v(face_amont_s, j);
                                    double xv_s2 = xv_v(face_amont_s2, j);
                                    double cs_s = coord_sommets_v(sommet_s, j);
                                    double cs_s2 = (dim == 3 ? coord_sommets_v(sommet_s2, j) : 0);
                                    // Calcul de l'inconnue au centre M de la fa7
                                    inco_m += gm * (rang == -1 ? centre_fa7[j] - xv_m : vecteur_face_facette_Cl_v(rang, fa7, j,  dir));
                                    // Calcul de l'inconnue au sommet S, une premiere extremite de la fa7
                                    inco_s += gs * (cs_s - xv_s);
                                    // Calcul de l'inconnue au sommet S2, la derniere extremite de la fa7 en 3D
                                    inco_s2 += gs2 * (cs_s2 - xv_s2);
                                  }
                                // Calcul de l'inconnue a C, une autre extremite de la fa7, intersection avec les autres fa7
                                // du polyedre. C=G centre du polyedre si volume non etendu
                                // xc donne par elemvef.calcul_xg()
                                double inco_c;
                                if (ordre == 3)
                                  {
                                    inco_c = transporte_face_v(face_amont_c, comp0);
                                    for (int j = 0; j < dim; j++)
                                      inco_c += gradient_v(item_c, comp0, j) * (-xv_v(face_amont_c, j) + xc[j]);
                                  }
                                else
                                  inco_c = dim * inco_m - inco_s - inco_s2;

                                if (flux_en_un_point)
                                  // Calcul du flux sur 1 point
                                  flux = inco_m * psc_m;
                                else
                                  // Calcul du flux sur 3 points
                                  flux = (inco_c * psc_c + inco_s * psc_s + inco_s2 * psc_s2 + dim * dim * inco_m * psc_m) / (dim + dim * dim);
                              }

                            // Ponderation par coefficient alpha
                            flux *= alpha;

                            int compo = ncomp_ch_transporte == 1 ? 0 : comp0;
                            Kokkos::atomic_sub(&resu_v(num10, compo), flux);
                            Kokkos::atomic_add(&resu_v(num20, compo), flux);
                            if (num10 < nb_faces_bord)
                              Kokkos::atomic_add(&flux_b_v(num10, compo), flux);
                            if (num20 < nb_faces_bord)
                              Kokkos::atomic_sub(&flux_b_v(num20, compo), flux);

                          }// boucle sur comp
                      } // fin de la boucle sur les facettes
                  } // fin de la boucle
              });
              end_gpu_timer(__KERNEL_NAME__);
            }
          else
            {
              // Example of use of scratch memory on previous kernel thanks
              // to Kokkos hierarchical parallelism
              using TeamPolicy = Kokkos::TeamPolicy<>;
              using TeamMember = TeamPolicy::member_type;
              using ScratchPadInt = Kokkos::View<int*, Kokkos::DefaultExecutionSpace::scratch_memory_space, Kokkos::MemoryTraits<Kokkos::Unmanaged>>;
              using ScratchPadDouble = Kokkos::View<double*, Kokkos::DefaultExecutionSpace::scratch_memory_space, Kokkos::MemoryTraits<Kokkos::Unmanaged>>;

              // scratch memory required for temporary arrays
              const std::size_t scratch_size = ScratchPadInt::shmem_size(
                                                 4 // face
                                               ) + ScratchPadDouble::shmem_size(
                                                 3 // vs
                                                 + 12 // vsom
                                                 + 3 // vc
                                                 + 3 // xc
                                                 + 12 // xsom
                                                 + 3 // centre_fa7
                                                 + 3 // cc
                                               );

              // size of a team
              constexpr int warp_size = 32;

              auto kern_conv_aj = KOKKOS_LAMBDA (TeamMember const& teamMember)
              {
                // this parallel for is artificial, as it makes 1 thread work on 1 cell, just like with RangePolicy
                Kokkos::parallel_for(Kokkos::TeamVectorRange(teamMember, warp_size), [=] (const int vector_id)
                {
                  // recompute polyhedra id
                  int const poly = teamMember.league_rank() * warp_size + vector_id;

                  // exit early if all polyhedra computed
                  if (poly >= nb_elem_tot)
                    {
                      return;
                    }

                  int rang = rang_elem_non_std_v(poly);
                  int contrib = 0;
                  // calcul des numeros des faces du polyedre
                  ScratchPadInt face(teamMember.thread_scratch(0), 4);
                  for (int face_adj = 0; face_adj < nfac; face_adj++)
                    {
                      int fac = elem_faces_v(poly, face_adj);
                      face(face_adj) = fac;
                      if (fac < nb_faces) contrib = 1; // Une face reelle sur l'element virtuel
                    }
                  //
                  if (contrib)
                    {
                      int calcul_flux_en_un_point = (ordre != 3) && (ordre == 1 || traitement_pres_bord_v(poly));
                      bool flux_en_un_point = calcul_flux_en_un_point || option_calcul_flux_en_un_point;
                      ScratchPadDouble vs(teamMember.thread_scratch(0), 3);
                      for (int j = 0; j < dim; j++)
                        {
                          vs(j) = 0; // initialize vs
                          for (int i = 0; i < nfac; i++)
                            vs(j) += vitesse_face_absolue_v(face(i), j) * porosite_face_v(face(i));
                        }
                      // calcul de la vitesse aux sommets des tetraedres
                      // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
                      ScratchPadDouble vsom(teamMember.thread_scratch(0), 12);
                      for (int i = 0; i < nsom; i++)
                        for (int j = 0; j < dim; j++)
                          vsom(i * 3 + j) = vs(j) - dim * vitesse_face_absolue_v(face(i), j) * porosite_face_v(face(i));

                      // Determination du type de CL selon le rang
                      rang = rang_elem_non_std_v(poly);
                      True_int itypcl = type_elem_Cl_v(poly);

                      ScratchPadDouble vc(teamMember.thread_scratch(0), 3);
                      calcul_vc_tetra_views(face.data(), vc.data(), vs.data(), vsom.data(), vitesse_v, itypcl, porosite_face_v);
                      // calcul de xc (a l'intersection des 3 facettes) necessaire pour muscl3
                      ScratchPadDouble xc(teamMember.thread_scratch(0), 3);
                      if (ordre == 3) // A optimiser! Risque de mauvais resultats en parallel si ordre=3
                        {
                          ScratchPadDouble xsom(teamMember.thread_scratch(0), 12);
                          for (int i = 0; i < nsom; i++)
                            for (int j = 0; j < dim; j++)
                              xsom(i * 3 + j) = coord_sommets_v(les_elems_v(poly, i), j);
                          int idirichlet, n1, n2, n3;
                          calcul_xg_tetra(xc.data(), xsom.data(), itypcl, idirichlet, n1, n2, n3);
                        }

                      // Gestion de la porosite
                      if (marq == 0)
                        {
                          double coeff = 1. / porosite_elem_v(poly);
                          for (int l = 0; l < nsom * dim; l++) vsom(l) *= coeff;
                          for (int l = 0; l < dim; l++) vc(l) *= coeff;
                        }
                      // Boucle sur les facettes du polyedre:
                      ScratchPadDouble centre_fa7(teamMember.thread_scratch(0), 3);
                      ScratchPadDouble cc(teamMember.thread_scratch(0), 3);
                      for (int fa7 = 0; fa7 < nfa7; fa7++)
                        {
                          int num10 = face(KEL_v(0, fa7));
                          int num20 = face(KEL_v(1, fa7));
                          // normales aux facettes
                          if (rang == -1)
                            for (int i = 0; i < dim; i++)
                              cc(i) = facette_normales_v(poly, fa7, i);
                          else
                            for (int i = 0; i < dim; i++)
                              cc(i) = normales_facettes_Cl_v(rang, fa7, i);

                          // Calcul des vitesses en C,S,S2 les 3 extremites de la fa7 et M le centre de la fa7
                          double psc_c = 0, psc_s = 0, psc_s2 = 0;
                          for (int i = 0; i < dim; i++)
                            {
                              psc_c += vc(i) * cc(i);
                              psc_s += vsom(KEL_v(2, fa7) * dim + i) * cc(i);
                              psc_s2 += (dim==2 ? 0 : vsom(KEL_v(3, fa7) * dim + i) * cc(i));
                            }
                          double psc_m = (psc_c + psc_s + psc_s2) / dim;
                          // On applique les CL de Dirichlet si num1 ou num2 est une face avec CL de Dirichlet
                          // auquel cas la fa7 coincide avec la face num1 ou num2 -> C est au centre de la face
                          int appliquer_cl_dirichlet = 0;
                          if (option_appliquer_cl_dirichlet)
                            if (est_une_face_de_dirichlet_v(num10) || est_une_face_de_dirichlet_v(num20))
                              {
                                appliquer_cl_dirichlet = 1;
                                psc_m = psc_c;
                              }

                          // Determination des faces amont pour les points M,C,S,S2
                          int face_amont_m = (psc_m >= 0) ? num10 : num20;
                          int face_amont_c = (isMuscl && ordre == 3) ? ((psc_c >= 0) ? num10 : num20) : face_amont_m;
                          int face_amont_s = (isMuscl && ordre == 3) ? ((psc_s >= 0) ? num10 : num20) : face_amont_m;
                          int face_amont_s2 = (isMuscl && ordre == 3) ? ((psc_s2 >= 0) ? num10 : num20) : face_amont_m;

                          // gradient aux items element (schema centre) ou aux items face (schemas muscl)
                          int item_m = isMuscl ? face_amont_m : poly;
                          int item_c = isMuscl ? face_amont_c : poly;
                          int item_s = isMuscl ? face_amont_s : poly;
                          int item_s2 = isMuscl ? face_amont_s2 : poly;

                          int dir = (psc_m >= 0) ? 0 : 1;
                          int num_face = elem_faces_v(poly, KEL_v(dir, fa7));
                          if (rang==-1)
                            {
                              for (int j = 0; j < dim; j++)
                                {
                                  centre_fa7(j) = xp_v(poly, j);
                                  for (int num_som_fa7 = 0; num_som_fa7 < nb_som_facette - 1; num_som_fa7++)
                                    {
                                      int isom_loc = KEL_v(num_som_fa7 + 2, fa7);
                                      int isom_glob = les_elems_v(poly, isom_loc);
                                      centre_fa7(j) += coord_sommets_v(isom_glob, j);
                                    }
                                  centre_fa7(j) /= nb_som_facette;
                                }
                            }
                          else
                            {
                              for (int j = 0; j < dim; j++)
                                {
                                  centre_fa7(j) = 0;
                                }
                            }
                          bool flux_avec_m = isAmont || appliquer_cl_dirichlet;
                          for (int comp0 = 0; comp0 < ncomp_ch_transporte; comp0++)
                            {
                              double inco_m = transporte_face_v(face_amont_m, comp0);
                              double flux;
                              if (flux_avec_m) // amont
                                flux = inco_m * psc_m;
                              else // muscl ou centre
                                {
                                  // PL: data locality matters ! Try to access arrays one by one and store into registers
                                  double inco_s = transporte_face_v(face_amont_s, comp0);
                                  double inco_s2 = dim == 3 ? transporte_face_v(face_amont_s2, comp0) : 0;
                                  int sommet_s = les_elems_v(poly, KEL_v(2, fa7));
                                  int sommet_s2 = dim == 3 ? les_elems_v(poly, KEL_v(3, fa7)) : -1;
                                  for (int j = 0; j < dim; j++)
                                    {
                                      double gm = gradient_v(item_m, comp0, j);
                                      double gs = gradient_v(item_s, comp0, j);
                                      double gs2 = (dim == 3 ? gradient_v(item_s2, comp0, j) : 0);
                                      double xv_m = xv_v(num_face, j);
                                      double xv_s = xv_v(face_amont_s, j);
                                      double xv_s2 = xv_v(face_amont_s2, j);
                                      double cs_s = coord_sommets_v(sommet_s, j);
                                      double cs_s2 = (dim == 3 ? coord_sommets_v(sommet_s2, j) : 0);
                                      // Calcul de l'inconnue au centre M de la fa7
                                      inco_m += gm * (rang == -1 ? centre_fa7(j) - xv_m : vecteur_face_facette_Cl_v(rang, fa7, j,  dir));
                                      // Calcul de l'inconnue au sommet S, une premiere extremite de la fa7
                                      inco_s += gs * (cs_s - xv_s);
                                      // Calcul de l'inconnue au sommet S2, la derniere extremite de la fa7 en 3D
                                      inco_s2 += gs2 * (cs_s2 - xv_s2);
                                    }
                                  // Calcul de l'inconnue a C, une autre extremite de la fa7, intersection avec les autres fa7
                                  // du polyedre. C=G centre du polyedre si volume non etendu
                                  // xc donne par elemvef.calcul_xg()
                                  double inco_c;
                                  if (ordre == 3)
                                    {
                                      inco_c = transporte_face_v(face_amont_c, comp0);
                                      for (int j = 0; j < dim; j++)
                                        inco_c += gradient_v(item_c, comp0, j) * (-xv_v(face_amont_c, j) + xc(j));
                                    }
                                  else
                                    inco_c = dim * inco_m - inco_s - inco_s2;

                                  if (flux_en_un_point)
                                    // Calcul du flux sur 1 point
                                    flux = inco_m * psc_m;
                                  else
                                    // Calcul du flux sur 3 points
                                    flux = (inco_c * psc_c + inco_s * psc_s + inco_s2 * psc_s2 + dim * dim * inco_m * psc_m) / (dim + dim * dim);
                                }

                              // Ponderation par coefficient alpha
                              flux *= alpha;

                              int compo = ncomp_ch_transporte == 1 ? 0 : comp0;
                              Kokkos::atomic_sub(&resu_v(num10, compo), flux);
                              Kokkos::atomic_add(&resu_v(num20, compo), flux);
                              if (num10 < nb_faces_bord)
                                Kokkos::atomic_add(&flux_b_v(num10, compo), flux);
                              if (num20 < nb_faces_bord)
                                Kokkos::atomic_sub(&flux_b_v(num20, compo), flux);

                            }// boucle sur comp
                        } // fin de la boucle sur les facettes
                    } // fin de la boucle
                }); // end nested parallel for
              };
              // Use artificial hierarchical parallelism just to use sratch memory.
              // We iterate over "vectors" of `warp_size` threads (i.e. 32 cells).
              // In other words, we still have 1 thread for 1 cell.
              Kokkos::parallel_for(
                start_gpu_timer(__KERNEL_NAME__),
                TeamPolicy(nb_elem_tot / warp_size + 1, warp_size) // artificial hierarchical parallelism
                .set_scratch_size(0, Kokkos::PerThread(scratch_size)), // amount of required scratch memory
                kern_conv_aj
              );
              end_gpu_timer(__KERNEL_NAME__);
            }
        }
      else
        {
          // Non tetra (tri, quad, hexa)
          ArrOfInt face(nfac);
          ArrOfDouble vs(dimension);
          ArrOfDouble vc(dimension);
          ArrOfDouble cc(dimension);
          DoubleVect xc(dimension);
          DoubleTab vsom(nsom,dimension);
          DoubleTab xsom(nsom,dimension);
          for (int poly=0; poly<nb_elem_tot; poly++)
            {
              int contrib = 0;
              // calcul des numeros des faces du polyedre
              for (int face_adj=0; face_adj<nfac; face_adj++)
                {
                  int face_ = elem_faces(poly,face_adj);
                  face(face_adj)= face_;
                  if (face_<nb_faces) contrib=1; // Une face reelle sur l'element virtuel
                }
              //
              if (contrib)
                {
                  int calcul_flux_en_un_point = (ordre_ != 3) && (ordre_==1 || traitement_pres_bord_(poly));
                  for (int j=0; j<dimension; j++)
                    {
                      vs(j) = vitesse_face_absolue(face(0),j)*porosite_face(face(0));
                      for (int i=1; i<nfac; i++)
                        vs(j)+= vitesse_face_absolue(face(i),j)*porosite_face(face(i));
                    }
                  // calcul de la vitesse aux sommets des polyedres
                  // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
                  if (istetra==1)
                    {
                      for (int i=0; i<nsom; i++)
                        for (int j=0; j<dimension; j++)
                          vsom(i,j) = (vs(j) - dimension*vitesse_face_absolue(face(i),j)*porosite_face(face(i)));
                    }
                  else
                    {
                      // pour que cela soit valide avec les hexa (c'est + lent a calculer...)
                      for (int j=0; j<nsom; j++)
                        {
                          int num_som = les_elems(poly,j);
                          for (int ncomp=0; ncomp<dimension; ncomp++)
                            vsom(j,ncomp) = la_vitesse.valeur_a_sommet_compo(num_som,poly,ncomp);
                        }
                    }

                  // Determination du type de CL selon le rang
                  int rang = rang_elem_non_std(poly);
                  int itypcl = (rang==-1 ? 0 : domaine_Cl_VEF.type_elem_Cl(rang));

                  // calcul de vc (a l'intersection des 3 facettes) vc vs vsom proportionnelles a la porosite
                  type_elemvef.calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);

                  // calcul de xc (a l'intersection des 3 facettes) necessaire pour muscl3
                  if (ordre_==3)
                    {
                      int idirichlet;
                      int n1,n2,n3;
                      for (int i=0; i<nsom; i++)
                        for (int j=0; j<dimension; j++)
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
                  for (int fa7=0; fa7<nfa7; fa7++)
                    {
                      int num10 = face(KEL(0,fa7));
                      int num20 = face(KEL(1,fa7));
                      // normales aux facettes
                      if (rang==-1)
                        for (int i=0; i<dimension; i++)
                          cc[i] = facette_normales(poly, fa7, i);
                      else
                        for (int i=0; i<dimension; i++)
                          cc[i] = normales_facettes_Cl(rang,fa7,i);

                      // Calcul des vitesses en C,S,S2 les 3 extremites de la fa7 et M le centre de la fa7
                      double psc_c=0,psc_s=0,psc_m,psc_s2=0;
                      if (dimension==2)
                        {
                          for (int i=0; i<2; i++)
                            {
                              psc_c+=vc[i]*cc[i];
                              psc_s+=vsom(KEL(2,fa7),i)*cc[i];
                            }
                          psc_m=(psc_c+psc_s)/2.;
                        }
                      else
                        {
                          for (int i=0; i<3; i++)
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
                      if (type_op_boucle==muscl && ordre_==3)
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

                      for (int comp0=0; comp0<ncomp_ch_transporte; comp0++)
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
                                for (int j=0; j<dimension; j++)
                                  inco_m+= tab_gradient(item_m,comp0,j)*vecteur_face_facette(poly,fa7,j,dir);
                              else
                                for (int j=0; j<dimension; j++)
                                  inco_m+= tab_gradient(item_m,comp0,j)*vecteur_face_facette_Cl(rang,fa7,j,dir);

                              // Calcul de l'inconnue au sommet S, une premiere extremite de la fa7
                              double inco_s = (ncomp_ch_transporte==1?transporte_face(face_amont_s):transporte_face(face_amont_s,comp0));
                              int sommet_s = les_elems(poly,KEL(2,fa7));
                              for (int j=0; j<dimension; j++)
                                inco_s+= tab_gradient(item_s,comp0,j)*(-xv(face_amont_s,j)+coord_sommets(sommet_s,j));

                              // Calcul de l'inconnue au sommet S2, la derniere extremite de la fa7 en 3D
                              double inco_s2=0;
                              if (dimension==3)
                                {
                                  inco_s2 = (ncomp_ch_transporte==1?transporte_face(face_amont_s2):transporte_face(face_amont_s2,comp0));
                                  int sommet_s2 = les_elems(poly,KEL(3,fa7));
                                  for (int j=0; j<dimension; j++)
                                    inco_s2+= tab_gradient(item_s2,comp0,j)*(-xv(face_amont_s2,j)+coord_sommets(sommet_s2,j));
                                }

                              // Calcul de l'inconnue a C, une autre extremite de la fa7, intersection avec les autres fa7
                              // du polyedre. C=G centre du polyedre si volume non etendu
                              // xc donne par elemvef.calcul_xg()
                              double inco_c;
                              if (ordre_==3)
                                {
                                  inco_c = (ncomp_ch_transporte==1?transporte_face(face_amont_c):transporte_face(face_amont_c,comp0));
                                  for (int j=0; j<dimension; j++)
                                    inco_c+= tab_gradient(item_c,comp0,j)*(-xv(face_amont_c,j)+xc(j));
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
        }
      alpha = 1 - alpha;
    } // fin de la boucle

  nb_faces_perio = 0;
  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          int dim = Objet_U::dimension;
          CDoubleTabView face_normale = domaine_VEF.face_normales().view_ro();
          CDoubleTabView val_ext = la_sortie_libre.val_ext().view_ro();
          CDoubleTabView transporte_face_v = transporte_face.view_ro();
          CDoubleTabView vitesse_face_v = vitesse_face.view_ro();
          DoubleTabView flux_b_v = flux_b.view_wo();
          DoubleTabView resu_v = resu.view_rw();
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(num1, num2), KOKKOS_LAMBDA(const int num_face)
          {
            double psc =0;
            for (int i=0; i<dim; i++)
              psc += vitesse_face_v(num_face,i)*face_normale(num_face,i);
            for (int i=0; i<ncomp_ch_transporte; i++)
              {
                double val = psc > 0 ? transporte_face_v(num_face,i) : val_ext(num_face-num1,i);
                resu_v(num_face,i) -= psc*val;
                flux_b_v(num_face,i) = -psc*val;
              }
          });
          end_gpu_timer(__KERNEL_NAME__);
        }
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          CIntArrView face_associee = la_cl_perio.face_associee().view_ro();
          CDoubleTabView resu_perio_v = resu_perio.view_ro();
          DoubleTabView resu_v = resu.view_rw();
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), Kokkos::RangePolicy<>(num1, num1+le_bord.nb_faces()/2), KOKKOS_LAMBDA(const int num_face)
          {
            int ind_face = num_face - num1;
            int voisine = face_associee(ind_face) + num1;
            for (int comp=0; comp<ncomp_ch_transporte; comp++)
              {
                double diff1 = resu_v(num_face,comp) - resu_perio_v(nb_faces_perio+ind_face,comp);
                double diff2 = resu_v(voisine,comp)  - resu_perio_v(nb_faces_perio+ind_face+voisine-num_face,comp);
                resu_v(voisine,comp)  += diff1;
                resu_v(num_face,comp) += diff2;
                /* On ne doit pas ajouter a flux_b, c'est deja calcule au dessus
                   flux_b(voisine,comp) += diff1;
                   flux_b(num_face,comp) += diff2; */
              }
          });
          end_gpu_timer(__KERNEL_NAME__);
          nb_faces_perio+=num2-num1;
        }
    }
  modifier_flux(*this);
  return resu;
}

// methodes recuperes de l'ancien OpVEFFaAmont
inline void convbisimplicite_dec(const double psc_, const int num1, const int num2,
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
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse();
  const DoubleTab& vitesse_face_absolue=la_vitesse.valeurs();
  const IntTab& elem_faces = domaine_VEF.elem_faces();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  const DoubleTab& facette_normales = domaine_VEF.facette_normales();
  const Domaine& domaine = domaine_VEF.domaine();
  const Elem_VEF_base& type_elem = domaine_VEF.type_elem();
  const int nfa7 = type_elem.nb_facette();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = domaine_VEF.rang_elem_non_std();
  const IntTab& les_elems=domaine.les_elems();

  const DoubleVect& porosite_face = equation().milieu().porosite_face();
  const DoubleVect& porosite_elem = equation().milieu().porosite_elem();
  const DoubleTab& normales_facettes_Cl = domaine_Cl_VEF.normales_facettes_Cl();

  int nfac = domaine.nb_faces_elem();
  int nsom = domaine.nb_som_elem();

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
  //DoubleTab pscl=0;
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
  const Elem_VEF_base& type_elemvef= domaine_VEF.type_elem();
  int istetra=0;
  Nom nom_elem=type_elemvef.que_suis_je();
  if ((nom_elem=="Tetra_VEF")||(nom_elem=="Tri_VEF"))
    istetra=1;


  // Les polyedres non standard sont ranges en 2 groupes dans le Domaine_VEF:
  //  - polyedres bords et joints
  //  - polyedres bords et non joints
  // On traite les polyedres en suivant l'ordre dans lequel ils figurent
  // dans le domaine

  // boucle sur les polys
  const IntTab& KEL=domaine_VEF.type_elem().KEL();
  int phi_u_transportant_yes=phi_u_transportant(equation());
  for (poly=0; poly<nb_elem_tot; poly++)
    {

      rang = rang_elem_non_std(poly);
      if (rang==-1)
        itypcl=0;
      else
        itypcl=domaine_Cl_VEF.type_elem_Cl(rang);

      // calcul des numeros des faces du polyedre
      for (face_adj=0; face_adj<nfac; face_adj++)
        face[face_adj]= elem_faces(poly,face_adj);

      for (j=0; j<dimension; j++)
        {
          vs[j] = vitesse_face_absolue(face[0],j)*porosite_face[face[0]];
          for (i=1; i<nfac; i++)
            vs[j]+= vitesse_face_absolue(face[i],j)*porosite_face[face[i]];
        }
      // calcul de la vitesse aux sommets des polyedres
      // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
      if (istetra==1)
        {
          for (i=0; i<nsom; i++)
            for (j=0; j<dimension; j++)
              vsom(i,j) = (vs[j] - dimension*vitesse_face_absolue(face[i],j)*porosite_face[face[i]]);
        }
      else
        {
          // pour que cela soit valide avec les hexa (c'est + lent a calculer...)
          int ncomp;
          for (j=0; j<nsom; j++)
            {
              num_som = les_elems(poly,j);
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
              vs[j]/= porosite_elem(poly);
              vc[j]/=porosite_elem(poly);
            }
        }
      // Boucle sur les facettes du polyedre non standard:
      for (fa7=0; fa7<nfa7; fa7++)
        {
          num10 = face[KEL(0,fa7)];
          num20 = face[KEL(1,fa7)];
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
  int nb_bord = domaine_VEF.nb_front_Cl();
  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {

      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
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
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = le_dom_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse();
  const DoubleTab& face_normales = domaine_VEF.face_normales();
  //const Domaine& domaine = domaine_VEF.domaine();
  //int nfac = domaine.nb_faces_elem();

  double psc;
  int i,n_bord;
  int num_face;
  int ncomp;
  if (resu.nb_dim() == 1)
    ncomp=1;
  else
    ncomp= resu.dimension(1);

  //ArrOfInt face(nfac);


  // Traitement particulier pour les faces de periodicite

  int nb_faces_perio = 0;
  int voisine;
  double diff1,diff2;

  // Boucle pour compter le nombre de faces de periodicite
  for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
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
  for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
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
  for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += la_vitesse.valeurs()(num_face,i)*face_normales(num_face,i);
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
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
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

void Op_Conv_VEF_Face::remplir_fluent() const
{
  const Domaine_Cl_VEF& domaine_Cl_VEF = la_zcl_vef.valeur();
  const Domaine_VEF& domaine_VEF = ref_cast(Domaine_VEF, le_dom_vef.valeur());
  int marq=phi_u_transportant(equation());
  // on force a calculer un pas de temps sans "porosite"
  marq=0;
  const Domaine& domaine = domaine_VEF.domaine();
  int nfac = domaine.nb_faces_elem();
  int nsom = domaine.nb_som_elem();
  const int nfa7 = domaine_VEF.type_elem().nb_facette();
  const int nb_elem_tot = domaine_VEF.nb_elem_tot();

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

  const Elem_VEF_base& type_elemvef= domaine_VEF.type_elem();
  int istetra=0;
  Nom nom_elem=type_elemvef.que_suis_je();
  if ((nom_elem=="Tetra_VEF")||(nom_elem=="Tri_VEF"))
    istetra=1;

  // On remet a zero le tableau qui sert pour
  // le calcul du pas de temps de stabilite
  fluent_ = 0;

  // Les polyedres non standard sont ranges en 2 groupes dans le Domaine_VEF:
  //  - polyedres bords et joints
  //  - polyedres bords et non joints
  // On traite les polyedres en suivant l'ordre dans lequel ils figurent
  // dans le domaine
  if (nom_elem=="Tetra_VEF")
    {
      assert(dimension==3);
      int dim = dimension;
      CIntArrView rang_elem_non_std = domaine_VEF.rang_elem_non_std().view_ro();
      CIntTabView elem_faces = domaine_VEF.elem_faces().view_ro();
      CDoubleTabView3 facette_normales = domaine_VEF.facette_normales().view_ro<3>();
      CDoubleTabView3 normales_facettes_Cl = domaine_Cl_VEF.normales_facettes_Cl().view_ro<3>();
      CIntTabView KEL = type_elemvef.KEL().view_ro();
      CDoubleArrView porosite_face = equation().milieu().porosite_face().view_ro();
      CDoubleArrView porosite_elem = equation().milieu().porosite_elem().view_ro();
      CDoubleTabView vitesse_face = vitesse().valeurs().view_ro();
      CIntArrView type_elem_Cl = type_elem_Cl_.view_ro();
      DoubleArrView fluent = fluent_.view_rw();
      // boucle sur les polys
      // Warning MDRangePolicy slowdown here on GPU...
      Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem_tot, KOKKOS_LAMBDA(const int poly)
      {
        int face[4] {};
        double vs[3] {};
        double vc[3] {};
        double cc[3] {};
        double vsom[12] {};

        // calcul des numeros des faces du polyedre
        for (int face_adj = 0; face_adj < nfac; face_adj++)
          face[face_adj] = elem_faces(poly, face_adj);

        for (int j = 0; j < dim; j++)
          {
            vs[j] = 0;
            for (int i = 0; i < nfac; i++)
              vs[j] += vitesse_face(face[i], j) * porosite_face(face[i]);
          }

        // calcul de la vitesse aux sommets des tetradres
        for (int i = 0; i < 4; i++)
          for (int j = 0; j < 3; j++)
            vsom[i * 3 + j] = (vs[j] - 3 * vitesse_face(face[i], j) * porosite_face(face[i]));

        int itypcl = type_elem_Cl(poly);
        // calcul de vc (a l'intersection des 3 facettes) vc vs vsom proportionnelles a la prosite
        calcul_vc_tetra_views(face, vc, vs, vsom, vitesse_face, itypcl, porosite_face);
        if (marq == 0)
          {
            double inv_porosite_poly = 1.0 / porosite_elem(poly);
            for (int j = 0; j < dim; j++)
              {
                vs[j] *= inv_porosite_poly;
                vc[j] *= inv_porosite_poly;
                for (int i = 0; i < nsom; i++)
                  vsom[i * dim + j] *= inv_porosite_poly;
              }
          }
        int rang = rang_elem_non_std(poly);
        // Boucle sur les facettes du polyedre non standard:
        for (int fa7 = 0; fa7 < nfa7; fa7++)
          {
            int num1 = face[KEL(0, fa7)];
            int num2 = face[KEL(1, fa7)];
            // normales aux facettes
            for (int i = 0; i < dim; i++)
              {
                int elem = (rang == -1 ? poly : rang);
                cc[i] = rang == -1 ? facette_normales(elem, fa7, i) : normales_facettes_Cl(elem, fa7, i);
              }

            // On applique le schema de convection a chaque sommet de la facette
            double psc_c = 0, psc_s = 0, psc_s2 = 0;
            for (int i = 0; i < dim; i++)
              {
                psc_c  += vc[i] * cc[i];
                psc_s  += vsom[KEL(2, fa7) * dim + i] * cc[i];
                psc_s2 += vsom[KEL(3, fa7) * dim + i] * cc[i];
              }
            double psc_m = (psc_c + psc_s + psc_s2) / dim;

            int num = (psc_m >= 0 ? num2 : num1);
            Kokkos::atomic_add(&fluent[num], std::abs(psc_m));
          } // fin de la boucle sur les facettes
      }); // fin de la boucle
      end_gpu_timer(__KERNEL_NAME__);
    }
  else
    {
      const DoubleVect& porosite_elem = equation().milieu().porosite_elem();
      const IntTab& KEL = type_elemvef.KEL();
      const DoubleVect& porosite_face = equation().milieu().porosite_face();
      const IntTab& elem_faces = domaine_VEF.elem_faces();
      const DoubleTab& facette_normales = domaine_VEF.facette_normales();
      const IntTab& les_elems = domaine.les_elems();
      const IntVect& rang_elem_non_std = domaine_VEF.rang_elem_non_std();
      const DoubleTab& normales_facettes_Cl = domaine_Cl_VEF.normales_facettes_Cl();
      ArrOfInt face(nfac);
      ArrOfDouble vs(dimension);
      ArrOfDouble vc(dimension);
      DoubleTab vsom(nsom,dimension);
      ArrOfDouble cc(dimension);
      const DoubleTab& vitesse_face = vitesse().valeurs();
      // boucle sur les polys
      for (int poly=0; poly<nb_elem_tot; poly++)
        {
          int rang = rang_elem_non_std(poly);
          // On cherche, pour un elem qui n'est pas de bord (rang==-1),
          // si un des sommets est sur un bord (tableau des sommets) (C MALOD 17/07/2007)
          int itypcl;
          if (rang==-1)
            itypcl=0;
          else
            itypcl=domaine_Cl_VEF.type_elem_Cl(rang);

          // calcul des numeros des faces du polyedre
          for (int face_adj=0; face_adj<nfac; face_adj++)
            face[face_adj]= elem_faces(poly,face_adj);

          for (int j=0; j<dimension; j++)
            {
              vs[j] = vitesse_face(face[0],j)*porosite_face[face[0]];
              for (int i=1; i<nfac; i++)
                vs[j]+= vitesse_face(face[i],j)*porosite_face[face[i]];
            }
          // calcul de la vitesse aux sommets des polyedres
          // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
          if (istetra==1)
            {
              for (int i=0; i<nsom; i++)
                for (int j=0; j<dimension; j++)
                  vsom(i,j) = (vs[j] - dimension*vitesse_face(face[i],j)*porosite_face[face[i]]);
            }
          else
            {
              // pour que cela soit valide avec les hexa (c'est + lent a calculer...)
              int ncomp;
              for (int j=0; j<nsom; j++)
                {
                  int num_som = les_elems(poly,j);
                  for (ncomp=0; ncomp<dimension; ncomp++)
                    vsom(j,ncomp) = vitesse().valeur_a_sommet_compo(num_som,poly,ncomp);
                }
            }

          // calcul de vc (a l'intersection des 3 facettes) vc vs vsom proportionnelles a la prosite
          type_elemvef.calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);
          if (marq==0)
            {
              double porosite_poly=porosite_elem(poly);
              for (int i=0; i<nsom; i++)
                for (int j=0; j<dimension; j++)
                  vsom(i,j)/=porosite_poly;
              for (int j=0; j<dimension; j++)
                {
                  vs[j]/= porosite_poly;
                  vc[j]/=porosite_poly;
                }
            }
          // Boucle sur les facettes du polyedre non standard:
          for (int fa7=0; fa7<nfa7; fa7++)
            {
              int num1 = face[KEL(0,fa7)];
              int num2 = face[KEL(1,fa7)];
              // normales aux facettes
              if (rang==-1)
                {
                  for (int i=0; i<dimension; i++)
                    cc[i] = facette_normales(poly, fa7, i);
                }
              else
                {
                  for (int i=0; i<dimension; i++)
                    cc[i] = normales_facettes_Cl(rang,fa7,i);
                }
              // On applique le schema de convection a chaque sommet de la facette

              double psc_c=0,psc_s=0,psc_m,psc_s2=0;
              if (dimension==2)
                {
                  for (int i=0; i<dimension; i++)
                    {
                      psc_c+=vc[i]*cc[i];
                      psc_s+=vsom(KEL(2,fa7),i)*cc[i];
                    }
                  psc_m=(psc_c+psc_s)/2.;
                }
              else
                {
                  for (int i=0; i<dimension; i++)
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
    }

  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement
  int nb_bord = domaine_VEF.nb_front_Cl();
  int nb_comp = Objet_U::dimension;
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl->frontiere_dis());
          int num1b = le_bord.num_premiere_face();
          int num2b = num1b + le_bord.nb_faces();
          CDoubleTabView face_normales = domaine_VEF.face_normales().view_ro();
          CDoubleTabView vitesse_face = vitesse().valeurs().view_ro();
          DoubleArrView fluent = fluent_.view_rw();
          Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__),
                               Kokkos::RangePolicy<>(num1b, num2b), KOKKOS_LAMBDA(
                                 const int num_face)
          {
            double psc = 0;
            for (int i=0; i<nb_comp; i++)
              psc += vitesse_face(num_face,i)*face_normales(num_face,i);
            if (psc>0) { /* Do nothing */}
            else
              fluent(num_face) -= psc;
          });
          end_gpu_timer(__KERNEL_NAME__);
        }
    }
}

void Op_Conv_VEF_Face::get_ordre(int& ord) const
{
  ord=ordre_;
}
void Op_Conv_VEF_Face::get_type_lim(Motcle& typelim) const
{
  typelim=type_lim;
}
void Op_Conv_VEF_Face::get_alpha(double& alp) const
{
  alp=alpha_;
}

void Op_Conv_VEF_Face::get_type_op(int& typeop) const
{
  typeop=type_op;
}
