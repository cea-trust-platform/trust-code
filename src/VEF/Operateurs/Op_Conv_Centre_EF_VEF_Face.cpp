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
// File:        Op_Conv_Centre_EF_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_Centre_EF_VEF_Face.h>

Implemente_instanciable(Op_Conv_Centre_EF_VEF_Face,"Op_Conv_Centre_EF_VEF_P1NC",Op_Conv_VEF_base);


//// printOn
//

Sortie& Op_Conv_Centre_EF_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Conv_Centre_EF_VEF_Face::readOn(Entree& s )
{
  return s ;
}

// ATTENTION!!!!!! les modifs concernant fluent_ et autre_num_face_loc sont faites qu en 3D!!!!
// C.A. 30/06/99

////////////////////////////////////////////////////////////////////
//
//                      Implementation des fonctions
//
//                   de la classe Op_Conv_Centre_EF_VEF_Face
//
////////////////////////////////////////////////////////////////////


DoubleTab& Op_Conv_Centre_EF_VEF_Face::ajouter(const DoubleTab& transporte,
                                               DoubleTab& resu) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();

  const DoubleVect& porosite_face = zone_VEF.porosite_face();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const Zone& zone = zone_VEF.zone();
  const int nfa7 = zone_VEF.type_elem().nb_facette();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();


  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();
  DoubleVect& fluent_ = ref_cast(DoubleVect, fluent);

  int nfac = zone.nb_faces_elem();
  int nsom = zone.nb_som_elem();


  // Pour le traitement de la convection on distingue les polyedres
  // standard qui ne "voient" pas les conditions aux limites et les
  // polyedres non standard qui ont au moins une face sur le bord.
  // Un polyedre standard a n facettes sur lesquelles on applique le
  // schema de convection.
  // Pour un polyedre non standard qui porte des conditions aux limites
  // de Dirichlet, une partie des facettes sont portees par les faces.
  // En bref pour un polyedre le traitement de la convection depend
  // du type (triangle, tetraedre ...) et du nombre de faces de Dirichlet.

  double flux;
  int poly,face_adj,fa7,i,j,comp0,n_bord;
  int num_face, rang ,itypcl;
  int num10, num20;

  int ncomp_ch_transporte;
  if (transporte.nb_dim() == 1)
    ncomp_ch_transporte=1;
  else
    ncomp_ch_transporte= transporte.dimension(1);

  IntVect face(nfac);
  DoubleVect cc(dimension);

  //////////////////////////////
  DoubleTab psc(nfac);
  int num_int;
  IntTab autre_num_face(dimension-1);
  IntTab autre_num_face_loc(dimension-1);
  double coef1=0.,coef2=0.,coef3=0.;
  //  double psc1;
  int nu1,nu2;
  //  int k;
  double f_int;

  DoubleVect vs(dimension);
  DoubleVect vc(dimension);
  DoubleTab vsom(nsom,dimension);

  // On remet a zero le tableau qui sert pour
  // le calcul du pas de temps de stabilite
  fluent_ = 0;

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


  // Les polyedres non standard sont ranges en 2 groupes dans la Zone_VEF:
  //  - polyedres bords et joints
  //  - polyedres bords et non joints
  // On traite les polyedres en suivant l'ordre dans lequel ils figurent
  // dans la zone

  // boucle sur les polys
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();
  for (poly=0; poly<nb_elem_tot; poly++)
    {

      rang = rang_elem_non_std(poly);
      if (rang==-1)
        itypcl=0;
      else
        itypcl=zone_Cl_VEF.type_elem_Cl(rang);

      // calcul des numeros des faces du polyedre
      for (face_adj=0; face_adj<nfac; face_adj++)
        face[face_adj]= elem_faces(poly,face_adj);

      // On cherche les numeros locaux de tpoutes les faces
      for (fa7=0; fa7<nfa7; fa7++)
        {
          nu1=-1;
          nu2=-1;
          num10 = face[KEL(0,fa7)];
          num20 = face[KEL(1,fa7)];
          // La facette est entouree des faces num1 et num2
          //        Cerr << "num1=" << num1 << "  num2=" << num2 << finl;

          // On cherche le numero des autres faces

          i=0;
          j=0;
          //                k=0;
          while(i<nfac)
            {
              num_int = face[i];
              if (num_int == num10)
                {
                  nu1=i;
                  //                Cerr << "nu1 (dans les boucles)=" << nu1 << finl;
                }
              else if (num_int == num20)
                {
                  nu2=i;
                  //                  Cerr << "nu2 (dans les boucles)=" << nu2 << finl;
                }
              else
                {
                  autre_num_face_loc(j)=i;
                  autre_num_face(j)=num_int;
                  //                  Cerr << "autre_num_face (dans les boucles)=" << autre_num_face(j) << finl;
                  j++;
                  //                          k++;
                }
              i++;
            }

          if (rang==-1)
            {
              for (i=0; i<dimension; i++)
                cc[i] = facette_normales(poly,fa7,i);
            }
          else
            for (i=0; i<dimension; i++)
              cc[i] = normales_facettes_Cl(rang,fa7,i);

          // On calcule les produits scalaires u(xi).n.S
          for (i=0; i<nfac; i ++)
            {
              psc[i] = 0.;
              for (j=0; j<dimension; j++)
                {
                  //                   Cerr << "cc[j]=" << cc[j] << finl;
                  //                   Cerr << "la_vitesse(face[i],j)=" << la_vitesse(face[i],j) << finl;

                  psc[i]+= la_vitesse(face[i],j)*cc[j]*porosite_face(face[i]);
                }
              //            Cerr << "psc[" << i << "]=" <<  psc[i] << finl;
            }

          //                assert(ncomp_ch_transporte==dimension);
          // Ce schema est valable (enfin je crois...) uniquement quand ch_transporte = ch_tranportant = vitesse??


          if (dimension == 2)
            {
              switch(itypcl)
                {
                case 0:
                  {
                    coef1  = 13.0*( psc[nu1]+psc[nu2] ) ;
                    coef1 -=  8.0*psc[autre_num_face_loc(0)];

                    coef2  =  8.0*( psc[nu1]+psc[nu2] ) ;
                    coef2 -=  7.0*psc[autre_num_face_loc(0)];

                    if (ncomp_ch_transporte == 1)
                      {
                        flux  = (transporte(num10)+transporte(num20))*coef1;
                        flux -=  transporte(autre_num_face(0))*coef2;
                        flux /= 27.;
                        resu(num10) -= flux;
                      }
                    else
                      {
                        for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                          {
                            flux  = (transporte(num10,comp0)+transporte(num20,comp0))*coef1;
                            flux -=  transporte(autre_num_face(0),comp0)*coef2;
                            flux /= 27.;
                            resu(num10, comp0) -= flux;
                            resu(num20, comp0) += flux;
                          }
                      }
                    // Pour la calcul du pas de temps de stabilite
                    fluent_[num10] += 2.*((psc[nu1]+psc[nu2])- psc[autre_num_face_loc(0)])/3.;
                    fluent_[num10] -= 2.*((psc[nu1]+psc[nu2])- psc[autre_num_face_loc(0)])/3.;
                    break;
                  }
                default :
                  {
                    int numfa7;
                    //  !!!!!! on a traite que le cas ou le champ transporte est vecteur!!!
                    if ((itypcl==1)||(itypcl==2)||(itypcl==4))     // 1 face de Dirichlet!!
                      {
                        switch(itypcl)
                          {
                          case 1 :
                            {
                              numfa7 = 2;
                              break;
                            }
                          case 2 :
                            {
                              numfa7 = 1;
                              break;
                            }
                          case 4 :
                            {
                              numfa7 = 0;
                              break;
                            }
                          default :
                            {
                              numfa7=-1;
                              Cerr << "C est pas possible!!!" << finl;
                              exit();
                              break;
                            }
                          }

                        //                    if (fa7 == itypcl)
                        if (fa7 == numfa7)  // On est sur la fa7 non confondu avec la face de Dirichlet
                          {
                            coef1  = 2.*( psc[nu1]+psc[nu2] ) ;
                            coef1 -= psc[numfa7];

                            coef2  =   psc[nu1]+psc[nu2] ;
                            coef2 -=  2.*psc[numfa7];

                            for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                              {
                                flux  = (transporte(num10,comp0)+transporte(num20,comp0))*coef1;
                                flux -=  transporte(numfa7,comp0)*coef2;
                                flux /= 6.;
                                resu(num10, comp0) -= flux;
                                resu(num20, comp0) += flux;
                              }
                            // Pour la calcul du pas de temps de stabilite
                            fluent_[num10] += 0.5*(psc[nu1]+psc[nu2]);
                            fluent_[num20] -= 0.5*(psc[nu1]+psc[nu2]);
                            //                            fluent_[num1] = ( fluent_[num1] > f_int) ? fluent_[num1] : f_int ;
                          }
                        else
                          {
                            // Pour les fa7 confondus
                            if (fa7 == nu2)
                              {
                                coef1  = 2.*( psc[nu1]-psc[nu2] ) ;
                                coef1 += 3.*psc[numfa7];

                                coef2  =  3.*(psc[nu1]-psc[nu2]) ;
                                coef2 +=  6.*psc[numfa7];


                                for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                                  {
                                    flux  = (transporte(face[nu1],comp0)-transporte(face[nu2],comp0))*coef1;
                                    flux +=  transporte(face[numfa7],comp0)*coef2;
                                    flux /= 6.;
                                    resu(face[nu1],comp0) -= flux;  // est ce le bon signe??????????????????????
                                    // Cerr << "num1=" << num1 << "  num2=" << num2 << finl;
                                  }
                                // Pour la calcul du pas de temps de stabilite
                                fluent_[face[nu1]] -=  0.5*(psc[nu1]-psc[nu2])+psc[numfa7]; // signe????????

                                //////////////////////////////////////////////
                              }
                            else
                              {
                                // fa7 == nu1
                                coef1  = 2.*( psc[nu2]-psc[nu1] ) ;
                                coef1 += 3.*psc[numfa7];

                                coef2  =  3.*(psc[nu2]-psc[nu1]) ;
                                coef2 +=  6.*psc[numfa7];  // est ce la bonne numerotation?


                                for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                                  {
                                    flux  = (transporte(face[nu2],comp0)-transporte(face[nu1],comp0))*coef1;
                                    flux +=  transporte(face[numfa7],comp0)*coef2;
                                    flux /= 6.;
                                    resu(face[nu1],comp0) -= flux;  // est ce le bon signe?????????????????
                                    // Cerr << "num1=" << num1 << "  num2=" << num2 << finl;
                                  }
                                // Pour la calcul du pas de temps de stabilite
                                fluent_[face[nu1]] -=  0.5*(psc[nu2]-psc[nu1])+psc[numfa7];   // signe?????????????????

                                //////////////////////////////////////////////
                              }
                          }
                      }
                    else
                      {
                        // 2 faces de Dirichlet!!!!!
                        // meme expression pour les 3 fa7 (confondues avec les faces de bord)
                        // On suppose que la num fa7 = num face

                        // Recuperation des numeros des autres faces
                        switch(fa7)
                          {
                          case 0 :
                            {
                              nu1 = 1;
                              nu2 = 2;
                              break;
                            }
                          case 1 :
                            {
                              nu1 = 0;
                              nu2 = 2;
                              break;
                            }
                          case 2 :
                            {
                              nu1 = 1;
                              nu2 = 0;
                              break;
                            }
                          default :
                            {
                              nu1=-1;
                              nu2=-1;
                              Cerr << "On arrete tout, c est pas possible!!!!" << finl;
                              Cerr << "sinon c est que je n ai rien compris!!!" << finl;
                              exit();
                            }
                          }
                        // Cerr << "fa7=" << fa7 << "   nu1=" << nu1 << "   nu2=" << nu2 << finl;
                        coef1  = psc[fa7];
                        coef2  = (psc[nu1]-psc[nu2])/3.;
                        for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                          {
                            flux  = transporte(face[fa7],comp0)*coef1;
                            flux += (transporte(face[nu1],comp0)-transporte(face[nu2],comp0))*coef2;
                            flux /= 6.;
                            resu(face[nu1],comp0) -= flux;  // est ce le bon signe?
                          }
                        // Pour la calcul du pas de temps de stabilite
                        fluent_[face[fa7]] -=  psc[fa7];
                        //                             f_int =  psc[fa7];
                        //                             fluent_[face[fa7]] = ( fluent_[face[fa7]] > f_int) ? fluent_[face[fa7]]  : f_int ;
                        //////////////////////////////////////////////
                      }
                    break;
                  }
                }
            }          // FIN de if(dimension == 2)
          else if (dimension == 3)
            {
              switch(itypcl)
                {
                case 0:
                  {
                    coef1  =  19.0*( psc[nu1]+psc[nu2] ) ;
                    coef1 -=   7.0*(psc[autre_num_face_loc(0)]+psc[autre_num_face_loc(1)]);
                    //                      Cerr << "coef1=" << coef1 << finl;

                    coef2  =   7.0*( psc[nu1]+psc[nu2] ) ;
                    coef2 -=  15.0*psc[autre_num_face_loc(0)];
                    coef2 +=   9.0*psc[autre_num_face_loc(1)];
                    //                      Cerr << "coef2=" << coef2 << finl;

                    coef3  =   7.0*( psc[nu1]+psc[nu2] ) ;
                    coef3 +=   9.0*psc[autre_num_face_loc(0)];
                    coef3 -=  15.0*psc[autre_num_face_loc(1)];
                    //                      Cerr << "coef3=" << coef3 << finl;

                    for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                      {
                        flux  = (transporte(num10,comp0)+transporte(num20,comp0))*coef1;
                        flux -=  transporte(autre_num_face(0),comp0)*coef2;
                        flux -=  transporte(autre_num_face(1),comp0)*coef3;
                        flux /= 32.;
                        resu(num10, comp0) -= flux;
                        resu(num20, comp0) += flux;
                        // Cerr << "transporte(num1,comp)=" << transporte(num1,comp) << finl;
                        //                           Cerr << "transporte(num2,comp)=" << transporte(num2,comp) << finl;
                        //                           Cerr << "transporte(autre_num_face(0),comp)=" << transporte(autre_num_face(0),comp) << finl;
                        //                           Cerr << "transporte(autre_num_face(1),comp)=" << transporte(autre_num_face(1),comp) << finl;
                        //                           Cerr << "flux=" << flux << finl;
                        //************Calcul de fluent pour le pas de temps
                        // f_int = 0.5*cc[comp]*(la_vitesse(num1,comp)+la_vitesse(num2,comp));

                        //  if (f_int>=0.)
                        //                             fluent_[num2] += f_int ;
                        //                           else
                        //                             fluent_[num1] -= f_int ;
                      }
                    f_int = 3.*(psc[nu1]+psc[nu2]);
                    f_int -= (psc[autre_num_face_loc(0)]+psc[autre_num_face_loc(1)]);
                    f_int /= 4.;
                    if (f_int>=0.)
                      {
                        //fluent_[num2] += f_int ;
                        fluent_[num20] = ( fluent_[num20] > std::fabs(f_int))? fluent_[num20] : std::fabs(f_int);
                      }
                    else
                      {
                        //fluent_[num1] -= f_int ;
                        fluent_[num10] = ( fluent_[num10] > std::fabs(f_int))? fluent_[num10] : std::fabs(f_int);
                      }



                    break;
                  }
                default:
                  {
                    Cerr << "Dans le default!!" << finl;
                    int nu3=-1;
                    if (fa7 == itypcl)
                      {
                        i=0;
                        while(i<nfac) // on cherche le 4ieme point!!
                          {
                            nu3 = i;
                            if (nu3 == fa7)
                              i++;
                            else if (nu3 == nu1)
                              i++;
                            else if (nu3 == nu2)
                              i++;
                            else
                              i=nfac;
                          }
                        coef1  =  6.*( psc[nu1]+psc[nu2] ) ;
                        coef1 -=  3.*psc[nu3]+psc[fa7];

                        coef2  =   3.0*( psc[nu1]+psc[nu2] ) ;
                        coef2 -=   7.0*psc[nu3];
                        coef2 +=   4.0*psc[fa7];

                        coef3  =   psc[nu1]+psc[nu2];
                        coef3 +=   psc[nu3];
                        coef3 -=   7.*psc[fa7];

                        for (comp0=0; comp0<ncomp_ch_transporte; comp0++)
                          {
                            flux  = (transporte(num10,comp0)+transporte(num20,comp0))*coef1;
                            flux -=  transporte(face[nu3],comp0)*coef2;
                            flux -=  transporte(face[fa7],comp0)*coef3;
                            flux /= 12.;
                            resu(num10, comp0) -= flux;
                            resu(num20, comp0) += flux;
                            // Pour la calcul du pas de temps de stabilite
                            // SIGNE????????????
                            //                                  f_int = std::fabs(flux/transporte(num1,comp));
                            f_int = 3.*(psc[nu1]+psc[nu2]);
                            f_int -= (psc[autre_num_face_loc(0)]+psc[autre_num_face_loc(1)]);
                            f_int /= 4.;
                            // fluent_[num1] -= f_int ;
                            //                               fluent_[num2] += f_int ;
                            if (f_int >=0.)
                              fluent_[num20] += f_int ;
                            else
                              fluent_[num10] -= f_int ;

                            //                              fluent_[num1] = ( fluent_[num1] > f_int) ? fluent_[num1]  : f_int ;
                            //////////////////////////////////////////////
                            //                          Cerr << "flux=" << flux << finl;
                          }
                      }
                    else
                      {
                        // J ai l impression que ce n est pas fini???!!!!!!!!!!
                      }
                    break;
                  }
                }
            }
        }
    }
  //  Cerr << "vitesse=" << la_vitesse.valeurs() << finl;


  int voisine;
  nb_faces_perio = 0;
  double diff1,diff2;
  double pscav;

  // Dimensionnement du tableau des flux convectifs au bord du domaine
  // de calcul
  DoubleTab& flux_b = ref_cast(DoubleTab,flux_bords_);
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
          ////////////ATTENTION!!!!!!!!!!! Cela correspond a l ancien schema et pas au EF!!
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              pscav =0;
              for (i=0; i<dimension; i++)
                pscav += la_vitesse(num_face,i)*face_normales(num_face,i)*porosite_face(num_face);
              if (pscav>0)
                if (ncomp_ch_transporte == 1)
                  {
                    resu(num_face) -= pscav*transporte(num_face);
                    flux_b(num_face,0) -= pscav*transporte(num_face);
                  }
                else
                  for (i=0; i<ncomp_ch_transporte; i++)
                    {
                      resu(num_face,i) -= pscav*transporte(num_face,i);
                      flux_b(num_face,i) -= pscav*transporte(num_face,i);
                    }
              else
                {
                  if (ncomp_ch_transporte == 1)
                    {
                      resu(num_face) -= pscav*la_sortie_libre.val_ext(num_face-num1);
                      flux_b(num_face,0) -= pscav*la_sortie_libre.val_ext(num_face-num1);
                    }
                  else
                    for (i=0; i<ncomp_ch_transporte; i++)
                      {
                        resu(num_face,i) -= pscav*la_sortie_libre.val_ext(num_face-num1,i);
                        flux_b(num_face,i) -= pscav*la_sortie_libre.val_ext(num_face-num1,i);
                      }
                  fluent_[num_face] -= pscav;
                }
            }
          //        Cerr << "Pour l instant Neumann_sortie_libre pas possible!!!" << finl;
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

                  if (ncomp_ch_transporte == 1)
                    {
                      diff1 = resu(num_face)-tab(nb_faces_perio);
                      diff2 = resu(voisine)-tab(nb_faces_perio+voisine-num_face);
                      resu(voisine)  += diff1;
                      resu(num_face) += diff2;
                      flux_b(voisine,0) += diff1;
                      flux_b(num_face,0) += diff1;
                      // Pour la calcul du pas de temps de stabilite
                      // RIEN en periodique ??? (faces deja traites avant??????)
                      //////////////////////////////////////////////
                    }
                  else
                    for (int comp=0; comp<ncomp_ch_transporte; comp++)
                      {
                        diff1 = resu(num_face,comp)-tab(nb_faces_perio,comp);
                        diff2 = resu(voisine,comp)-tab(nb_faces_perio+voisine-num_face,comp);
                        //                       Cerr << "num_face=" << num_face << "  diff1=" << diff1 << finl;
                        //                       Cerr << "voisine=" << voisine << "  diff2=" << diff2 << finl;
                        resu(voisine,comp)  += diff1;
                        resu(num_face,comp) += diff2;
                        flux_b(voisine,comp) += diff1;
                        flux_b(num_face,comp) += diff1;
                        // Pour la calcul du pas de temps de stabilite
                        // RIEN en periodique ??? (faces deja traites avant??????)
                        //////////////////////////////////////////////
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
