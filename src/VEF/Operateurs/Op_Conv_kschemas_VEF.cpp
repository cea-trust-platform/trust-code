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
// File:        Op_Conv_kschemas_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_kschemas_VEF.h>

Implemente_base(Op_Conv_kschemas_VEF,"Op_Conv_kschemas_VEF_P1NC",Op_Conv_VEF_base);


//// printOn
//

Sortie& Op_Conv_kschemas_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Conv_kschemas_VEF::readOn(Entree& s )
{
  return s ;
}

//
//   Fonctions de la classe Op_Conv_kschemas_VEF
//
void Op_Conv_kschemas_VEF::associer(const Zone_dis& zone_dis,
                                    const Zone_Cl_dis& zone_cl_dis,
                                    const Champ_Inc& ch )
{
  // CCa le 28/05/99 Le schema Kquick ne marche pas en paralle !!
  if(Process::nproc()>1)
    {
      Cerr<<"ATTENTION le kquick ne marche pas en parallele !!!"<<finl;
      exit();
    }

  Op_Conv_VEF_base::associer(zone_dis,zone_cl_dis,ch);
}


//////////////////////////////////////////////////////////////
//   Fonctions pour les kschemas.
////////////////////////////////////////////////////////////////

// convkschemas : fonction utilitaire pour la convection

void convkschemas(const double K, const int ncomp, int dimension, const int poly ,
                  const int poly1, const int poly2,const int jel0,
                  const int jel1,const double& psc ,const DoubleTab& tab1 ,
                  DoubleVect& fluent, DoubleVect& flux,
                  const DoubleVect& rx0, const DoubleTab& gradient_elem )
{

  int comp,amont,i,elem1,elem2;
  double CF,UTC,deltat0,deltat1,deltat;
  DoubleVect rx(dimension);
  deltat0 = 0.;
  deltat1 = 0.;

  ////////////////////////////////////////////////////////////////////////
  // Test sur les bords
  ////////////////////////////////////////////////////////////////////////
  if ((poly1==-1) || (poly2==-1))
    {
      if (psc >= 0)
        {
          amont = jel0;
          fluent[jel1] += psc;
        }
      else
        {
          amont = jel1;
          fluent[jel0] -= psc;
        }

      for (comp=0; comp<ncomp; comp++)
        flux(comp) = tab1(amont,comp);

    }
  else
    {
      if (psc >= 0)
        {
          amont = jel0;
          rx = rx0;
          elem1 = poly;
          elem2 = poly1;
          fluent(jel1)  += psc;
        }
      else
        {
          amont = jel1;
          rx = rx0;
          rx *= -1.;
          elem1 = poly;
          elem2 = poly2;
          fluent(jel0)  -= psc;
        }

      for (comp=0; comp<ncomp; comp++)
        {
          deltat0 = deltat1 = 0.0;
          flux(comp) = tab1(amont,comp);
          //Cerr << " flux(" << comp << ") ie phiamont= " <<  flux(comp) << finl;

          for (i=0; i<dimension; i++)
            {
              deltat0 += gradient_elem(elem1,comp,i)*rx(i);
              deltat1 += gradient_elem(elem2,comp,i)*rx(i);
            }

          if (K == 0.5)
            {
              deltat = deltat0 + deltat1;

              if  (std::fabs(deltat) <= 1.e-5)
                {
                  CF = 0.125;
                }
              else
                {
                  UTC = deltat1 / deltat;

                  if ( (UTC <= -1.) || (UTC >= 1.5) )      CF = 0.125;
                  else if ((UTC > -1.) && (UTC <= 0.))     CF = 0.5 + 0.375*UTC;
                  else if ((UTC > 0.) && (UTC <= 0.25))    CF = 0.5 - 0.625*sqrt(UTC);
                  else if ((UTC > 0.25) && (UTC < 1.5 ))   CF = 0.25* std::fabs(UTC - 1.);
                  else
                    {
                      CF=0.;
                      Process::exit();
                    }
                }
              // Calcul du flux
              flux(comp) += (0.5 - CF)*deltat0 + CF*deltat1 ;
              //Cerr << " flux(" << comp << ")= " <<  flux(comp) << finl;
            }
          else
            {
              // Calcul du flux
              flux(comp) += 0.25*((1.+K)*deltat0 + (1.-K)*deltat1) ;
            }
        }

    }
}

//////////////////////////////////////////////////////////////////////////
// Procedure AJOUTER
/////////////////////////////////////////////////////////////////////////

DoubleTab& Op_Conv_kschemas_VEF::ajouter(const DoubleTab& transporte,
                                         DoubleTab& resu) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const Zone& zone = zone_VEF.zone();
  const int nb_faces = zone_VEF.nb_faces();
  const int nfa7 = zone_VEF.type_elem().nb_facette();
  const int nb_elem = zone_VEF.nb_elem();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleVect& volumes = zone_VEF.volumes();
  const DoubleTab& xv = zone_VEF.xv();
  const DoubleTab& xg = zone_VEF.xp();
  const DoubleTab& coord = zone.domaine().coord_sommets();
  int premiere_face_int = zone_VEF.premiere_face_int();
  const IntTab& les_Polys = zone.les_elems();

  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();

  int nfac = zone.nb_faces_elem();
  int nsom = zone.nb_som_elem();
  int nb_som_facette = zone.type_elem().nb_som_face();
  DoubleVect& fluent_ = static_cast<DoubleVect&>(fluent);

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
  int poly,poly1,poly2,face_adj,fa7,i,j,n_bord;
  int num_face, rang ,itypcl;
  int num10,num20,num3,num_som;

  // MODIF SB su 10/09/03
  // Pour les 3 elements suivants, il y a autant de sommets que de face
  // constituant l'element geometrique
  // PB avec les hexa, 8 sommets et 6 faces, donc l'utilisation du tableau
  // face[i] ne fonctionne plus
  // la methode retenue pour eviter de calculer la vitesse aux sommets sans
  // les fonctions de forme n'est donc pas utilisable,
  // pour l'hexa on n'a pas acces a la face.
  // il existe le tableau Face=>sommets mais pas l'inverse.
  // trop couteux et pour le moment on n'etend pas les porosites aux hexa

  int istetra=0;
  const Elem_VEF_base& type_elemvef= zone_VEF.type_elem().valeur();
  Nom nom_elem=type_elemvef.que_suis_je();
  if ((nom_elem=="Tetra_VEF")||(nom_elem=="Tri_VEF")) istetra=1;

  const int ncomp_ch_transporte= transporte.line_size();
  int fac,elem1,elem2,comp0;
  int nb_faces_ = zone_VEF.nb_faces();
  IntVect face(nfac);

  DoubleVect flux(ncomp_ch_transporte);
  DoubleVect fluxsom(ncomp_ch_transporte);
  DoubleVect fluxg(ncomp_ch_transporte);

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

  DoubleTab tab(nb_faces_perio,ncomp_ch_transporte);

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
              for (int comp=0; comp<ncomp_ch_transporte; comp++)
                tab(nb_faces_perio,comp) = resu(num_face,comp);
              nb_faces_perio++;
            }
        }
    }


  ///////////////////////////////////////////////////////////////////////////////////////////////
  //                        <
  // calcul des gradients;  < [ Ujp*np/vol(j) ]
  //                         j
  ////////////////////////////////////////////////////////////////////////////////////////////////
  DoubleTab gradient_elem(0, ncomp_ch_transporte, dimension);
  zone_VEF.zone().creer_tableau_elements(gradient_elem);
  // Boucle sur les faces


  for (fac=0; fac< premiere_face_int; fac++)
    {
      elem1=face_voisins(fac,0);
      if(ncomp_ch_transporte==1)
        for (i=0; i<dimension; i++)
          {
            gradient_elem(elem1, 0, i) +=
              face_normales(fac,i)*transporte(fac);
          }
      else
        for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
          for (i=0; i<dimension; i++)
            gradient_elem(elem1, comp0, i) +=
              face_normales(fac,i)*transporte(fac,comp0);
      // dUcomp/dXi
    } // fin du for faces

  for (; fac<nb_faces_; fac++)
    {
      elem1=face_voisins(fac,0);
      elem2=face_voisins(fac,1);
      if(ncomp_ch_transporte==1)
        for (i=0; i<dimension; i++)
          {
            gradient_elem(elem1, 0, i) +=
              face_normales(fac,i)*transporte(fac);
            gradient_elem(elem2, 0, i) -=
              face_normales(fac,i)*transporte(fac);
          }
      else
        for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
          for (i=0; i<dimension; i++)
            {
              gradient_elem(elem1, comp0, i) +=
                face_normales(fac,i)*transporte(fac,comp0);
              gradient_elem(elem2, comp0, i) -=
                face_normales(fac,i)*transporte(fac,comp0);
            }
      // dUcomp/dXi
    } // fin du for faces

  for (int elem=0; elem<nb_elem; elem++)
    for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
      for (i=0; i<dimension; i++)
        gradient_elem(elem,comp0,i) /= volumes(elem);

  gradient_elem.echange_espace_virtuel();

  ////////////////////////////////////////////////////////////////////////////////////
  // On a les gradient_elem par elements
  ////////////////////////////////////////////////////////////////////////////////////

  DoubleVect vs(dimension);
  DoubleVect vc(dimension);
  DoubleTab vsom(nsom,dimension);
  DoubleVect cc(dimension);
  double xm;

  // On remet a zero le tableau qui sert pour
  // le calcul du pas de temps de stabilite
  fluent_ = 0;

  // Les polyedres non standard sont ranges en 2 groupes dans la Zone_VEF:
  //  - polyedres bords et joints
  //  - polyedres bords et non joints
  // On traite les polyedres en suivant l'ordre dans lequel ils figurent
  // dans la zone

  //////////////////////////////////////////////////////////////////////////////////////
  // boucle sur les polys
  //////////////////////////////////////////////////////////////////////////////////////
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();
  for (poly=0; poly<nb_elem; poly++)
    {
      rang = rang_elem_non_std(poly);
      if (rang==-1)
        itypcl=0;
      else
        itypcl=zone_Cl_VEF.type_elem_Cl(rang);

      // calcul des numeros des faces du polyedre
      for (face_adj=0; face_adj<nfac; face_adj++)
        face(face_adj)= elem_faces(poly,face_adj);

      int scom;
      DoubleVect rx0(dimension);

      // calcul de la vitesse aux sommets des polyedres
      for (j=0; j<dimension; j++)
        {
          vs(j) = la_vitesse(face(0),j)*porosite_face(face(0));
          for (i=1; i<nfac; i++)
            vs(j)+= la_vitesse(face(i),j)*porosite_face(face(i));
        }

      //int ncomp;
      if (istetra==1)
        {
          for (j=0; j<nsom; j++)
            {
              for (int ncomp=0; ncomp<Objet_U::dimension; ncomp++)
                vsom(j,ncomp) =vs[ncomp] - Objet_U::dimension*la_vitesse(face[j],ncomp)*porosite_face(face[j]);
            }
        }
      else
        {
          // pour que cela soit valide avec les hexa
          // On va utliser les fonctions de forme implementees dans la classe Champs_P1_impl ou Champs_Q1_impl
          //int ncomp;
          for (j=0; j<nsom; j++)
            {
              num_som = zone.sommet_elem(poly,j);
              for (int ncomp=0; ncomp<dimension; ncomp++)
                {
                  vsom(j,ncomp) = la_vitesse.valeur_a_sommet_compo(num_som,poly,ncomp);
                }
            }
        }
      // calcul de la vitesse au centre de gravite
      zone_VEF.type_elem().calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);

      // Boucle sur les facettes du polyedre non standard:
      for (fa7=0; fa7<nfa7; fa7++)
        {
          //Cerr << "la facette etudiee est " << fa7 << finl;
          // fa7 separe num1 et num2. num3 est la troisieme face (2D).

          num10 = face(KEL(0,fa7));
          num20 = face(KEL(1,fa7));
          num3 = face(KEL(2,fa7));

          // Determination des elements voisins aux faces num1 et num2

          poly1 = face_voisins(num10,0);
          if (poly1==poly)
            poly1 = face_voisins(num10,1);

          poly2 = face_voisins(num20,0);
          if (poly2==poly)
            poly2 = face_voisins(num20,1);

          scom = les_Polys(poly,KEL(2,fa7));

          // calcul des rx0, distance entre les milieux des 'num i'

          for (i=0; i<dimension; i++)
            rx0(i) = xv(num20,i)-xv(num10,i);

          // normales aux facettes

          if (rang==-1)
            for (i=0; i<dimension; i++)
              cc[i] = facette_normales(poly, fa7, i);
          else
            for (i=0; i<dimension; i++)
              cc[i] = normales_facettes_Cl(rang,fa7,i);

          /////////////////////////////////////////////////////////////////////////
          // On traite le point pour lequel vitesse = 0.5(vitsommet + vitmilieu)
          /////////////////////////////////////////////////////////////////////////

          for (i=0; i<nb_som_facette-1; i++)
            {
              //////////////////////////////////////////////////////////////////////////
              //Determination de PhiIJ au milieu entre le sommet et le milieu de num3
              /////////////////////////////////////////////////////////////////////////

              psc = 0;
              for (j=0; j<dimension; j++)
                psc+=((vsom(KEL(i+2,fa7),j) + la_vitesse(num3,j) * porosite_face(num3)))*cc[j];
              psc *=0.5;
              convkschemas(K,ncomp_ch_transporte,dimension,poly,poly1,poly2,num10,num20,psc,transporte,
                           fluent_,flux,rx0,gradient_elem);

              ////////////////////////////////////////////////////////////////////////////////////////////////////////
              //Limiteur pour le gradient. Calcul effectue en meme temps que le calcul du flux.
              // gradient(K0) = teta*gradient(K0)+ (1-teta)gradient(K1ou K2)
              ////////////////////////////////////////////////////////////////////////////////////////////////////////

              double teta = 0.5;

              /////////////////////////////////////////////////////////////////////////
              // On traite les sommets qui sont aussi des sommets du polyedre
              /////////////////////////////////////////////////////////////////////////

              // XXX XXX XXX : Attention : we can not factorize more... the code is not the same
              if (ncomp_ch_transporte == 1)
                {
                  for (j=0; j<dimension; j++)
                    {
                      xm = 0.5 *(coord(scom,j)+xv(num3,j));
                      if (psc >= 0)
                        {
                          if (poly1==-1)
                            fluxsom(0) += gradient_elem(poly,0,j)*(coord(scom,j)-xm);
                          else
                            fluxsom(0) += (teta*gradient_elem(poly,0,j) + (1.- teta)*gradient_elem(poly1,0,j))*(coord(scom,j)-xm);
                        }
                      else
                        {
                          if (poly2==-1)
                            fluxsom(0) += gradient_elem(poly,0,j)*(coord(scom,j)-xm);
                          else
                            fluxsom(0) += (teta*gradient_elem(poly,0,j) + (1.- teta)*gradient_elem(poly2,0,j))*(coord(scom,j)-xm);
                        }
                    }
                  fluxsom(0) += flux(0);
                  fluxsom(0) *= psc;
                }
              else
                {
                  for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                    {
                      fluxsom(comp0) = flux(comp0);
                      for (j=0; j<dimension; j++)
                        {
                          xm = 0.5 *(coord(scom,j)+xv(num3,j));
                          if (psc >= 0)
                            {
                              if (poly1==-1)
                                fluxsom(comp0) += gradient_elem(poly,comp0,j)*(coord(scom,j)-xm);
                              else
                                fluxsom(comp0) += (teta*gradient_elem(poly,comp0,j) + (1.-teta)*gradient_elem(poly1,comp0,j))*(coord(scom,j)-xm);
                            }
                          else
                            {
                              if (poly2==-1)
                                fluxsom(comp0) += gradient_elem(poly,comp0,j)*(coord(scom,j)-xm);
                              else
                                fluxsom(comp0) += (teta*gradient_elem(poly,comp0,j) + (1.-teta)*gradient_elem(poly2,comp0,j))*(coord(scom,j)-xm);
                            }
                        }
                      fluxsom(comp0) *= psc;
                    }
                }

              ////////////////////////////////////////////////////////////////////////////
              // on traite le centre de gravite
              ////////////////////////////////////////////////////////////////////////////

              // XXX XXX XXX : Attention : we can not factorize more... the code is not the same
              if (ncomp_ch_transporte == 1)
                {
                  for (j=0; j<dimension; j++)
                    {
                      xm = 0.5 *(coord(scom,j)+xv(num3,j));
                      if (psc >= 0)
                        {
                          if (poly1==-1)
                            fluxg(0) += gradient_elem(poly,0,j)*(xg(poly,j)-xm);
                          else
                            fluxg(0) += (teta*gradient_elem(poly,0,j) + (1.- teta)*gradient_elem(poly1,0,j))*(xg(poly,j)-xm);
                        }

                      else
                        {
                          if (poly2==-1)
                            fluxg(0) += gradient_elem(poly,0,j)*(xg(poly,j)-xm);
                          else
                            fluxg(0) += (teta*gradient_elem(poly,0,j) + (1.- teta)*gradient_elem(poly2,0,j))*(xg(poly,j)-xm);
                        }
                    }

                  fluxg(0) += flux(0);
                  fluxg(0) *= psc;
                }
              else
                {
                  for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                    {
                      fluxg(comp0) = flux(comp0);
                      for (j=0; j<dimension; j++)
                        {
                          xm = 0.5 *(coord(scom,j)+xv(num3,j));
                          if (psc >= 0)
                            {
                              if (poly1==-1)
                                fluxg(comp0) += gradient_elem(poly,comp0,j)*(xg(poly,j)-xm);
                              else
                                fluxg(comp0) += (teta*gradient_elem(poly,comp0,j) + (1.-teta)*gradient_elem(poly1,comp0,j))*(xg(poly,j)-xm);
                            }

                          else
                            {
                              if (poly2==-1)
                                fluxg(comp0) += gradient_elem(poly,comp0,j)*(xg(poly,j)-xm);
                              else
                                fluxg(comp0) += (teta*gradient_elem(poly,comp0,j) + (1.-teta)*gradient_elem(poly2,comp0,j))*(xg(poly,j)-xm);
                            }
                        }
                      fluxg(comp0) *= psc;
                    }
                }
              //////////////////////////////////////////////////////////////////////////////
              // Integration de u.n.flux
              /////////////////////////////////////////////////////////////////////////////
              for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                {
                  resu(num10,comp0) -= ( 0.5*(fluxsom(comp0)+fluxg(comp0)) );
                  resu(num20,comp0) += ( 0.5*(fluxsom(comp0)+fluxg(comp0)) );
                }
            }
        }
    } // fin de la boucle

  // Traitement des elements joints d'epaisseur 1
  for (poly=0; poly<nb_elem_tot; poly++)
    {
      // On regarde si une face du polyedre est une face joint
      for (face_adj=0; face_adj<nfac; face_adj++)
        if(face_adj<nb_faces) break;
      if(face_adj<nfac)
        {
          rang = rang_elem_non_std(poly);
          if (rang==-1)
            itypcl=0;
          else
            itypcl=zone_Cl_VEF.type_elem_Cl(rang);

          // calcul des numeros des faces du polyedre
          for (face_adj=0; face_adj<nfac; face_adj++)
            {
              face(face_adj)= elem_faces(poly,face_adj);
              //Cerr << "les faces de l'elements sont : " << face(face_adj) << finl;
            }

          int scom;
          DoubleVect rx0(dimension);

          // calcul de la vitesse aux sommets des polyedres
          for (j=0; j<dimension; j++)
            {
              vs(j) = la_vitesse(face(0),j)*porosite_face(face(0));
              for (i=1; i<nfac; i++)
                vs(j)+= la_vitesse(face(i),j)*porosite_face(face(j));
            }
          int ncomp;
          for (j=0; j<nsom; j++)
            {
              num_som = zone.sommet_elem(poly,j);
              for (ncomp=0; ncomp<dimension; ncomp++)
                vsom(j,ncomp) = la_vitesse.valeur_a_sommet_compo(num_som,poly,ncomp);
            }
          // calcul de la vitesse au centre de gravite

          zone_VEF.type_elem().calcul_vc(face,vc,vs,vsom,vitesse(),itypcl,porosite_face);


          // Boucle sur les facettes du polyedre non standard:

          for (fa7=0; fa7<nfa7; fa7++)
            {
              //Cerr << "la facette etudiee est " << fa7 << finl;
              // fa7 separe num1 et num2. num3 est la troisieme face (2D).

              num10 = face(KEL(0,fa7));
              num20 = face(KEL(1,fa7));
              num3 = face(KEL(2,fa7));

              // Determination des elements voisins aux faces num1 et num2

              poly1 = face_voisins(num10,0);
              if (poly1==poly)
                {
                  poly1 = face_voisins(num10,1);
                }

              poly2 = face_voisins(num20,0);
              if (poly2==poly)
                {
                  poly2 = face_voisins(num20,1);
                }

              scom = les_Polys(poly,KEL(2,fa7));

              // calcul des rx0, distance entre les milieux des 'num i'

              for (i=0; i<dimension; i++)
                rx0(i) = xv(num20,i)-xv(num10,i);

              // normales aux facettes

              if (rang==-1)
                for (i=0; i<dimension; i++)
                  cc[i] = facette_normales(poly, fa7, i);
              else
                for (i=0; i<dimension; i++)
                  cc[i] = normales_facettes_Cl(rang,fa7,i);

              /////////////////////////////////////////////////////////////////////////
              // On traite le point pour lequel vitesse = 0.5(vitsommet + vitmilieu)
              /////////////////////////////////////////////////////////////////////////

              for (i=0; i<nb_som_facette-1; i++)
                {
                  //////////////////////////////////////////////////////////////////////////
                  //Determination de PhiIJ au milieu entre le sommet et le milieu de num3
                  /////////////////////////////////////////////////////////////////////////

                  psc = 0;
                  for (j=0; j<dimension; j++)
                    psc+=((vsom(KEL(i+2,fa7),j) + la_vitesse(num3,j) * porosite_face(num3)))*cc[j];
                  psc *=0.5;
                  convkschemas(K,ncomp_ch_transporte,dimension,poly,poly1,poly2,num10,num20,psc,transporte,
                               fluent_,flux,rx0,gradient_elem);


                  ////////////////////////////////////////////////////////////////////////////////////////////////////////
                  //Limiteur pour le gradient. Calcul effectue en meme temps que le calcul du flux.
                  // gradient(K0) = teta*gradient(K0)+ (1-teta)gradient(K1ou K2)
                  ////////////////////////////////////////////////////////////////////////////////////////////////////////

                  double teta = 0.5;

                  /////////////////////////////////////////////////////////////////////////
                  // On traite les sommets qui sont aussi des sommets du polyedre
                  /////////////////////////////////////////////////////////////////////////

                  // XXX XXX XXX : Attention : we can not factorize more... the code is not the same
                  if (ncomp_ch_transporte == 1)
                    {
                      for (j=0; j<dimension; j++)
                        {
                          xm = 0.5 *(coord(scom,j)+xv(num3,j));
                          if (psc >= 0)
                            {
                              if (poly1==-1)
                                fluxsom(0) += gradient_elem(poly,0,j)*(coord(scom,j)-xm);
                              else
                                fluxsom(0) += (teta*gradient_elem(poly,0,j) + (1.- teta)*gradient_elem(poly1,0,j))*(coord(scom,j)-xm);
                            }
                          else
                            {
                              if (poly2==-1)
                                fluxsom(0) += gradient_elem(poly,0,j)*(coord(scom,j)-xm);
                              else
                                fluxsom(0) += (teta*gradient_elem(poly,0,j) + (1.- teta)*gradient_elem(poly2,0,j))*(coord(scom,j)-xm);
                            }
                        }
                      fluxsom(0) += flux(0);
                      fluxsom(0) *= psc;
                    }
                  else
                    {
                      for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                        {
                          fluxsom(comp0) = flux(comp0);
                          for (j=0; j<dimension; j++)
                            {
                              xm = 0.5 *(coord(scom,j)+xv(num3,j));
                              if (psc >= 0)
                                {
                                  if (poly1==-1)
                                    fluxsom(comp0) += gradient_elem(poly,comp0,j)*(coord(scom,j)-xm);
                                  else
                                    fluxsom(comp0) += (teta*gradient_elem(poly,comp0,j) + (1.-teta)*gradient_elem(poly1,comp0,j))*(coord(scom,j)-xm);
                                }
                              else
                                {
                                  if (poly2==-1)
                                    fluxsom(comp0) += gradient_elem(poly,comp0,j)*(coord(scom,j)-xm);
                                  else
                                    fluxsom(comp0) += (teta*gradient_elem(poly,comp0,j) + (1.-teta)*gradient_elem(poly2,comp0,j))*(coord(scom,j)-xm);
                                }
                            }
                          fluxsom(comp0) *= psc;
                        }
                    }

                  ////////////////////////////////////////////////////////////////////////////
                  // on traite le centre de gravite
                  ////////////////////////////////////////////////////////////////////////////

                  // XXX XXX XXX : Attention : we can not factorize more... the code is not the same
                  if (ncomp_ch_transporte == 1)
                    {
                      for (j=0; j<dimension; j++)
                        {
                          xm = 0.5 *(coord(scom,j)+xv(num3,j));
                          if (psc >= 0)
                            {
                              if (poly1==-1)
                                fluxg(0) += gradient_elem(poly,0,j)*(xg(poly,j)-xm);
                              else
                                fluxg(0) += (teta*gradient_elem(poly,0,j) + (1.- teta)*gradient_elem(poly1,0,j))*(xg(poly,j)-xm);
                            }
                          else
                            {
                              if (poly2==-1)
                                fluxg(0) += gradient_elem(poly,0,j)*(xg(poly,j)-xm);
                              else
                                fluxg(0) += (teta*gradient_elem(poly,0,j) + (1.- teta)*gradient_elem(poly2,0,j))*(xg(poly,j)-xm);
                            }
                        }
                      fluxg(0) += flux(0);
                      fluxg(0) *= psc;
                    }
                  else
                    {
                      for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                        {
                          fluxg(comp0) = flux(comp0);
                          for (j=0; j<dimension; j++)
                            {
                              xm = 0.5 *(coord(scom,j)+xv(num3,j));
                              if (psc >= 0)
                                {
                                  if (poly1==-1)
                                    fluxg(comp0) += gradient_elem(poly,comp0,j)*(xg(poly,j)-xm);
                                  else
                                    fluxg(comp0) += (teta*gradient_elem(poly,comp0,j) + (1.-teta)*gradient_elem(poly1,comp0,j))*(xg(poly,j)-xm);
                                }
                              else
                                {
                                  if (poly2==-1)
                                    fluxg(comp0) += gradient_elem(poly,comp0,j)*(xg(poly,j)-xm);
                                  else
                                    fluxg(comp0) += (teta*gradient_elem(poly,comp0,j) + (1.-teta)*gradient_elem(poly2,comp0,j))*(xg(poly,j)-xm);
                                }
                            }
                          fluxg(comp0) *= psc;
                        }
                    }

                  //////////////////////////////////////////////////////////////////////////////
                  // Integration de u.n.flux
                  /////////////////////////////////////////////////////////////////////////////
                  for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                    {
                      resu(num10,comp0) -= ( 0.5*(fluxsom(comp0)+fluxg(comp0)) );
                      resu(num20,comp0) += ( 0.5*(fluxsom(comp0)+fluxg(comp0)) );
                    }
                }
            }
        }
    } // fin de la boucle
  int voisine;
  nb_faces_perio = 0;
  double diff1,diff2;

  // Dimensionnement du tableau des flux convectifs au bord du domaine
  // de calcul
  DoubleTab& flux_b = static_cast<DoubleTab&>(flux_bords_);
  flux_b.resize(zone_VEF.nb_faces_bord(),ncomp_ch_transporte);
  flux_b = 0.;

  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += la_vitesse(num_face,i)*face_normales(num_face,i)*porosite_face(num_face);
              if (psc>0)
                for (i=0; i<ncomp_ch_transporte; i++)
                  {
                    resu(num_face,i) -= psc*transporte(num_face,i);
                    flux_b(num_face,i) -= psc*transporte(num_face,i);
                  }
              else
                {
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
  modifier_flux(*this);
  return resu;
}
