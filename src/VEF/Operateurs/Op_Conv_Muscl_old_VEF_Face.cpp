/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Op_Conv_Muscl_old_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/32
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_Muscl_old_VEF_Face.h>
#include <Champ_P1NC.h>

Implemente_instanciable_sans_constructeur(Op_Conv_Muscl_old_VEF_Face,"Op_Conv_Muscl_old_VEF_P1NC",Op_Conv_VEF_base);


//// printOn
//
Sortie& Op_Conv_Muscl_old_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//
Entree& Op_Conv_Muscl_old_VEF_Face::readOn(Entree& s )
{
  return s ;
}

//
//   Fonctions de la classe Op_Conv_Muscl_old_VEF_Face
//



//////////////////////////////////////////////////////////////
//   Fonctions de MUSCL
////////////////////////////////////////////////////////////////

#define sgn(x) (x>0) ? 1:-1
inline double minmod(double grad1, double grad2, double gradc)
{
  int s1=sgn(grad1);
  int s2=sgn(grad2);
  int sc=sgn(gradc);
  double gradlim;
  if ((s1==s2) && (s2==sc))
    {
      gradlim=std::min(fabs(grad1), fabs(grad2));
      gradlim=std::min(fabs(gradlim), fabs(gradc));
      return sc*gradlim;
    }
  else
    return 0;
}



////////////////////////////////////////////////////////////////////
//
//                      Implementation des fonctions
//
//                   de la classe Op_Conv_Muscl_old_VEF_Face
//
////////////////////////////////////////////////////////////////////


DoubleTab& Op_Conv_Muscl_old_VEF_Face::ajouter(const DoubleTab& transporte,
                                               DoubleTab& resu) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = ref_cast(Zone_VEF, la_zone_vef.valeur());
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const Zone& zone = zone_VEF.zone();

  const int nfa7 = zone_VEF.type_elem().nb_facette();

  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleVect& volumes = zone_VEF.volumes();

  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();
  int premiere_face_int = zone_VEF.premiere_face_int();
  int nfac = zone.nb_faces_elem();
  int nsom = zone.nb_som_elem();
  int nb_som_facette = zone.type_elem().nb_som_face();
  double inverse_nb_som_facette=1./nb_som_facette;
  DoubleVect& fluent_ = ref_cast(DoubleVect, fluent);
  DoubleTab& vecteur_face_facette = ref_cast_non_const(Zone_VEF,zone_VEF).vecteur_face_facette();

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

  double psc;
  int poly,face_adj,fa7,i,j,n_bord;
  int num_face, rang ,itypcl;
  int num10,num20,num_som;
  int ncomp_ch_transporte;
  if (transporte.nb_dim() == 1)
    ncomp_ch_transporte=1;
  else
    ncomp_ch_transporte= transporte.dimension(1);

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

  int fac=0,elem1,elem2,comp;
  int nb_faces_ = zone_VEF.nb_faces();
  IntVect face(nfac);

  DoubleTab gradient_elem(nb_elem_tot,ncomp_ch_transporte,dimension);
  // (du/dx du/dy dv/dx dv/dy) pour un poly
  DoubleTab gradient(0, ncomp_ch_transporte, dimension);
  zone_VEF.creer_tableau_faces(gradient);

  // (du/dx du/dy dv/dx dv/dy) pour une face
  // gradient_elem=0.; deja fait par le constructeur

  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  Champ_P1NC::calcul_gradient(transporte,gradient_elem,zone_Cl_VEF);

  // On a les gradient_elem par elements

  ////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Limitation minmod
  //
  // Boucle sur les faces
  //
  for (fac=0; fac< premiere_face_int; fac++)
    {
      for (comp=0; comp<ncomp_ch_transporte; comp++)
        for (i=0; i<dimension; i++)
          gradient(fac, comp, i)
          /* amont : */= 0;

    } // fin du for faces

  for (; fac<nb_faces_; fac++)
    {
      elem1=face_voisins(fac,0);
      elem2=face_voisins(fac,1);
      double vol1=volumes(elem1);
      double vol2=volumes(elem2);
      double inverse_voltot=1./(vol1+vol2);
      for (comp=0; comp<ncomp_ch_transporte; comp++)
        for (i=0; i<dimension; i++)
          {
            double grad1=gradient_elem(elem1, comp, i);
            double grad2=gradient_elem(elem2, comp, i);
            double gradc=(vol1*grad1 + vol2*grad2)*inverse_voltot;
            gradient(fac, comp, i) = minmod(grad1, grad2, gradc);
          }
    } // fin du for faces

  gradient.echange_espace_virtuel();


  DoubleVect vs(dimension);
  DoubleVect vc(dimension);
  DoubleTab vsom(nsom,dimension);
  DoubleVect cc(dimension);


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
  const DoubleTab& vitesse_face=la_vitesse.valeurs();

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
          vs(j) = vitesse_face(face(0),j)*porosite_face(face(0));
          for (i=1; i<nfac; i++)
            vs(j)+= vitesse_face(face(i),j)*porosite_face(face(i));
        }
      // calcul de la vitesse aux sommets des polyedres
      // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
      if (istetra==1)
        {
          for (i=0; i<nsom; i++)
            for (j=0; j<dimension; j++)
              vsom(i,j) = vs[j] - dimension*vitesse_face(face[i],j)*porosite_face(face[i]);
        }
      else
        {
          // pour que cela soit valide avec les hexa (c'est + lent a calculer...)
          int ncomp;
          for (j=0; j<nsom; j++)
            {
              num_som = zone.sommet_elem(poly,j);
              for (ncomp=0; ncomp<dimension; ncomp++)
                vsom(j,ncomp) = la_vitesse.valeur_a_sommet_compo(num_som,poly,ncomp);
            }
        }


      // calcul de vc (a l'intersection des 3 facettes)
      type_elemvef.calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);

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

          // On applique le schema de convection a chaque sommet de la facette
          for (i=0; i<nb_som_facette; i++)
            {
              psc =0;
              if (i==nb_som_facette-1)
                {
                  // On traite le sommet confondu avec le centre de gravite du polyedre
                  for (j=0; j<dimension; j++)
                    psc += vc[j]*cc[j];
                }
              else
                {
                  // On traite le ou les sommets qui sont aussi des sommets du polyedre
                  for (j=0; j<dimension; j++)
                    psc+= vsom(KEL(i+2,fa7),j)*cc[j];
                }
              psc *= inverse_nb_som_facette;

              // Calcul de convmuscl (auparavant dans une fonction qui n'etait pas toujours inlinee par le compilateur)
              int comp2,amont,ii;
              double flux;
              if (psc >= 0)
                {
                  amont = num10;
                  fluent_(num20)  += psc;
                }
              else
                {
                  amont = num20;
                  fluent_(num10)  -= psc;
                }
              if (ncomp_ch_transporte == 1)
                {
                  flux = transporte(amont);
                  if (psc >= 0)
                    for (ii=0; ii<dimension; ii++)
                      flux += gradient(amont,0,ii)*vecteur_face_facette(poly,fa7,ii,0);
                  else
                    for (ii=0; ii<dimension; ii++)
                      flux += gradient(amont,0,ii)*vecteur_face_facette(poly,fa7,ii,1);

                  flux*=psc;
                  resu(num10) -= flux;
                  resu(num20) += flux;
                }
              else
                for (comp2=0; comp2<ncomp_ch_transporte; comp2++)
                  {
                    flux = transporte(amont,comp2);
                    if (psc >= 0)
                      for (ii=0; ii<dimension; ii++)
                        flux += gradient(amont,comp2,ii)*vecteur_face_facette(poly,fa7,ii,0);
                    else
                      for (ii=0; ii<dimension; ii++)
                        flux += gradient(amont,comp2,ii)*vecteur_face_facette(poly,fa7,ii,1);
                    flux *= psc;
                    resu(num10,comp2) -= flux;
                    resu(num20,comp2) += flux;
                  }

            } // fin de la boucle sur les sommets de facette
        } // fin de la boucle sur les facettes
    } // fin de la boucle




  int voisine;
  nb_faces_perio = 0;
  double diff1,diff2;

  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement

  // Dimensionnement du tableau des flux convectifs au bord du domaine
  // de calcul
  DoubleTab& flux_b = ref_cast(DoubleTab,flux_bords_);
  flux_b.resize(zone_VEF.nb_faces_bord(),ncomp_ch_transporte);
  flux_b = 0.;

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
                psc += la_vitesse(num_face,i)*face_normales(num_face,i)*porosite_face(num_face);
              if (psc>0)
                if (ncomp_ch_transporte == 1)
                  {
                    resu(num_face) -= psc*transporte(num_face);
                    flux_b(num_face,0) -= psc*transporte(num_face);
                  }
                else
                  for (i=0; i<ncomp_ch_transporte; i++)
                    {
                      resu(num_face,i) -= psc*transporte(num_face,i);
                      flux_b(num_face,i) -= psc*transporte(num_face,i);
                    }
              else
                {
                  if (ncomp_ch_transporte == 1)
                    {
                      resu(num_face) -= psc*la_sortie_libre.val_ext(num_face-num1);
                      flux_b(num_face,0) -= psc*la_sortie_libre.val_ext(num_face-num1);
                    }
                  else
                    for (i=0; i<ncomp_ch_transporte; i++)
                      {
                        resu(num_face,i) -= psc*la_sortie_libre.val_ext(num_face-num1,i);
                        flux_b(num_face,i) -= psc*la_sortie_libre.val_ext(num_face-num1,i);
                      }
                  fluent_(num_face) -= psc;
                }
            }
        }
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
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
                    for (int comp2=0; comp2<ncomp_ch_transporte; comp2++)
                      {
                        diff1 = resu(num_face,comp2)-tab(nb_faces_perio,comp2);
                        diff2 = resu(voisine,comp2)-tab(nb_faces_perio+voisine-num_face,comp2);
                        resu(voisine,comp2)  += diff1;
                        resu(num_face,comp2) += diff2;
                        flux_b(voisine,comp2) += diff1;
                        flux_b(num_face,comp2) += diff2;
                      }

                  fait[num_face-num1]= 1;
                  fait[voisine-num1] = 1;
                }
              nb_faces_perio++;
            }
        }
    }


  modifier_flux(*this);
  return resu;
}
