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
// File:        Op_Conv_Vort_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/16
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_Vort_VEF_Face.h>
#include <Champ_P1NC.h>
#include <Periodique.h>

Implemente_instanciable(Op_Conv_Vort_VEF_Face,"Op_Conv_Conserve_Ec_VEF_P1NC",Op_Conv_VEF_base);


//// printOn
//

Sortie& Op_Conv_Vort_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Conv_Vort_VEF_Face::readOn(Entree& s )
{
  return s ;
}


DoubleTab& Op_Conv_Vort_VEF_Face::ajouter(const DoubleTab& transporte,
                                          DoubleTab& resu) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const Champ_P1NC& vit = ref_cast(Champ_P1NC,vitesse_.valeur());

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const Zone& zone = zone_VEF.zone();
  const int nb_faces = zone_VEF.nb_faces();
  const int nfa7 = zone_VEF.type_elem().nb_facette();
  const int nb_elem = zone_VEF.nb_elem();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();


  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();
  DoubleVect& fluent_ = fluent;

  int nfac = zone.nb_faces_elem();

  const DoubleVect& volumes = zone_VEF.volumes();
  int comp0;
  double flux;//,flux_int;
  int num_face;
  int elem0,elem1;
  double vol0,vol1;
  double inter,a0,a1,a2,f_int;

  IntVect face(nfac);
  DoubleVect cc(dimension);
  DoubleTab psc(nfac);

  int num_int;
  IntTab autre_num_face(dimension-1);
  IntTab autre_num_face_loc(dimension-1);
  int poly,face_adj,fa7,i,j,n_bord;
  int rang ;
  int num10, num20;
  int nu1, nu2;
  int num_calc;


  // Pour le traitement de la convection on distingue les polyedres
  // standard qui ne "voient" pas les conditions aux limites et les
  // polyedres non standard qui ont au moins une face sur le bord.
  // Un polyedre standard a n facettes sur lesquelles on applique le
  // schema de convection.
  // Pour un polyedre non standard qui porte des conditions aux limites
  // de Dirichlet, une partie des facettes sont portees par les faces.
  // En bref pour un polyedre le traitement de la convection depend
  // du type (triangle, tetraedre ...) et du nombre de faces de Dirichlet.

  int ncomp_ch_transporte;
  if (transporte.nb_dim() == 1)
    ncomp_ch_transporte=1;
  else
    ncomp_ch_transporte= transporte.dimension(1);

  //  Cerr << "ncomp_ch_transporte=" << ncomp_ch_transporte << finl;

  // On remet a zero le tableau qui sert pour
  // le calcul du pas de temps de stabilite
  fluent_ = 0;

  // ATENTION : PBL pour determiner le fluent
  // ********   on le met a 1!!!
  //  fluent_ = 1.;


  // Traitement particulier pour les faces de periodicite

  int nb_faces_perio = 0;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            nb_faces_perio++;
        }
    }

  DoubleTab tab;
  if (ncomp_ch_transporte == 1)
    tab.resize(nb_faces_perio);
  else
    tab.resize(nb_faces_perio,ncomp_ch_transporte);

  nb_faces_perio=0;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          //          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
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

  //  Cerr << "tab=" << tab << finl;

  // Les polyedres non standard sont ranges en 2 groupes dans la Zone_VEF:
  //  - polyedres bords et joints
  //  - polyedres bords et non joints
  // On traite les polyedres en suivant l'ordre dans lequel ils figurent
  // dans la zone

  // boucle sur les polys


  // Boucle pour ajouter la partie : Gradient(U^2/2)
  // ******* boucle sur les elements
  // 06/01/2000 On ne s'occupe pas encore des conditions aux limites (sauf periodique)
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();
  for (poly=0; poly<nb_elem_tot; poly++)
    {

      rang = rang_elem_non_std(poly);

      // calcul des numeros des faces du polyedre
      for (face_adj=0; face_adj<nfac; face_adj++)
        face[face_adj]= elem_faces(poly,face_adj);

      // On cherche les numeros globaux de toutes les faces
      for (fa7=0; fa7<nfa7; fa7++)
        {
          nu1=-1;
          nu2=-1;
          num10 = face[KEL(0,fa7)];
          num20 = face[KEL(1,fa7)];
          // La facette est entouree des faces num1 et num2
          //        Cerr << "num1=" << num1 << "  num2=" << num2 << finl;

          //           i=0;
          //           j=0;
          //           while(i<nfac)
          //             {
          //               num_int = face[i];
          //               if ((num_int!= num1)&&(num_int!= num2))
          //                 {
          //                   autre_num_face(j)=num_int;
          //                   j++;
          //                 }
          //               i++;
          //             }

          // On cherche le numero des autres faces (locaux et globaux)

          i=0;
          j=0;
          //                k=0;
          while(i<nfac)
            {
              num_int = face[i];
              if (num_int == num10)
                {
                  nu1=i;
                }
              else if (num_int == num20)
                {
                  nu2=i;
                }
              else
                {
                  autre_num_face_loc(j)=i;
                  autre_num_face(j)=num_int;
                  j++;
                }
              i++;
            }

          //           Cerr << "num1=" << num1 << "  num2=" << num2 << "  autre_num_face(0)=" << autre_num_face(0) << finl;
          //           if (dimension==3)
          //             Cerr << "autre_num_face(1)=" << autre_num_face(1) << finl;

          if (rang==-1)
            {
              for (i=0; i<dimension; i++)
                cc[i] = facette_normales(poly,fa7,i);
            }
          else
            for (i=0; i<dimension; i++)
              cc[i] = normales_facettes_Cl(rang,fa7,i);

          // On calcule les produits scalaires u(xi).n.S  // >>> calcul de fluent!!
          for (i=0; i<nfac; i ++)
            {
              psc[i] = 0.;
              for (j=0; j<dimension; j++)
                {
                  psc[i]+= la_vitesse(face[i],j)*cc[j];
                }
            }

          // Calcul du flux
          // Boucle sur les composantes : uu+vv+(ww)
          flux = 0.;
          if (dimension == 2)
            {
              f_int =  2.*((psc[nu1]+psc[nu2])- psc[autre_num_face_loc(0)])/3.;
            }
          else
            {
              // (dimension == 3)
              assert(dimension == 3);
              {
                f_int = 3.*(psc[nu1]+psc[nu2]);
                f_int -= (psc[autre_num_face_loc(0)]+psc[autre_num_face_loc(1)]);
                f_int /= 4.;
              }
            }
          if (f_int >= 0.)
            num_calc = num10;
          else
            num_calc = num20;

          flux = 0.;
          for (comp0=0; comp0<dimension; comp0++)
            flux += la_vitesse(num_calc,comp0)*la_vitesse(num_calc,comp0);

          for (comp0=0; comp0<dimension; comp0++)
            {
              resu(num10, comp0) -= 0.5*flux*cc[comp0];
              resu(num20, comp0) += 0.5*flux*cc[comp0];
            }

          // *** ??? : evaluation du fluent
          if (f_int>0.)
            {
              // fluent_[num2] += std::fabs(f_int);
              fluent_[num20] = ( fluent_[num20] > std::fabs(f_int))? fluent_[num20] : std::fabs(f_int);
            }
          else
            {
              fluent_[num10] = ( fluent_[num10] > std::fabs(f_int))? fluent_[num10] : std::fabs(f_int);
              // fluent_[num1] += std::fabs(f_int);
            }

        }
    }

  // FIN DE LA BOUCLE SUR LES ELEMENTS
  ////////// On fait la compensation ici, car la boucle suivante est sur les faces
  ////////// En le faisant a la fin, on aurait deux fois la contribution aux faces
  int voisine;
  nb_faces_perio = 0;
  double diff1,diff2;

  // Dimensionnement du tableau des flux convectifs au bord du domaine
  // de calcul
  DoubleTab& flux_b = flux_bords_;
  flux_b.resize(zone_VEF.nb_faces_bord(),ncomp_ch_transporte);
  flux_b = 0.;

  // Boucle sur les bords pour traiter les conditions aux limites

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          IntVect fait(le_bord.nb_faces());
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
                      flux_b(voisine,0) += diff1;
                      flux_b(num_face,0) += diff2;
                    }
                  else
                    for (int comp=0; comp<ncomp_ch_transporte; comp++)
                      {
                        diff1 = resu(num_face,comp)-tab(nb_faces_perio,comp);
                        diff2 = resu(voisine,comp)-tab(nb_faces_perio+voisine-num_face,comp);
                        resu(voisine,comp)  += diff1;
                        resu(num_face,comp) += diff2;
                        flux_b(voisine,comp) += diff1;
                        flux_b(num_face,comp) += diff2;
                      }

                  fait[num_face-num1]= 1;
                  fait[voisine-num1] = 1;
                }
              nb_faces_perio++;
            }
        }
    }

  /////////////////////////////////////////////////////
  // Boucle pour ajouter la partie avec la vorticite
  // ****** Boucle sur les faces

  // Calcul de la vorticite
  DoubleTab vorticite;
  if (dimension == 2)
    vorticite.resize(nb_elem);
  else if (dimension == 3)
    vorticite.resize(nb_elem,dimension);

  vit.cal_rot_ordre1(vorticite);

  //  Cerr << "vorticite=" << vorticite << finl;

  for (num_face=0; num_face<nb_faces; num_face++)
    {
      vol0=-1;
      vol1=-1;
      elem0 = face_voisins(num_face,0);
      elem1 = face_voisins(num_face,1);

      if (elem0 != -1)
        vol0 = volumes(elem0);

      if (elem1 != -1)
        vol1 = volumes(elem1);

      //      Cerr << "vol0=" << vol0 << "  vol1=" << vol1 << finl;

      if (dimension == 2)
        {
          //           for (comp=0;comp<dimension;comp++)
          //             {
          assert(vol0>0);
          assert(vol1>0);
          inter  = vorticite[elem0]*vol0/3.+vorticite[elem1]*vol1/3.;

          resu(num_face,0) -= -inter*la_vitesse(num_face,1);
          resu(num_face,1) -= inter*la_vitesse(num_face,0);

          // signe - car on est dans le second membre

          // *** PBL : evaluation du fluent
          //               if(psc >= 0)
          //                 fluent_[num2] += psc;
          //               else
          //                 fluent_[num1] -= psc;
        }
      else if (dimension == 3)
        {
          assert(vol0>0);
          assert(vol1>0);
          // vect(a) = vorticite*Vol
          a0 = vorticite(elem0,0)*vol0/4. + vorticite(elem1,0)*vol1/4.;
          a1 = vorticite(elem0,1)*vol0/4. + vorticite(elem1,1)*vol1/4.;
          a2 = vorticite(elem0,2)*vol0/4. + vorticite(elem1,2)*vol1/4.;

          resu(num_face,0) -= a1*la_vitesse(num_face,2)-a2*la_vitesse(num_face,1) ;
          resu(num_face,1) -= a2*la_vitesse(num_face,0)-a0*la_vitesse(num_face,2) ;
          resu(num_face,2) -= a0*la_vitesse(num_face,1)-a1*la_vitesse(num_face,0) ;

          // signe - car on est dans le second membre

          // *** PBL : evaluation du fluent
          //               if(psc >= 0)
          //                 fluent_[num2] += psc;
          //               else
          //                 fluent_[num1] -= psc;
        }
    }

  //******* VERIF PERIO
  Cerr << "DEBUT VERIF PERIO" << finl;
  //  Cerr << "nb_front_Cl=" << zone_VEF.nb_front_Cl() << finl;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          //          Cerr << "num1=" << num1 << "  num2=" << num2 << finl;
          for (num_face=num1; num_face<num2; num_face++)
            {
              voisine = la_cl_perio.face_associee(num_face-num1) + num1;
              for (int ii=0; ii<dimension; ii++)
                {
                  if ( resu(num_face,ii)!=resu(voisine,ii) )
                    {
                      Cerr << "Pbl de periodicite a la face" << num_face << finl;
                      Cerr << "diff = " << resu(num_face,ii)-resu(voisine,ii)  << finl;
                    }
                }
            }
        }
    }
  Cerr << "FIN VERIF PERIO" << finl;
  //******* FIN VERIF PERIO

  modifier_flux(*this);
  return resu;
}
