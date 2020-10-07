/****************************************************************************
* Copyright (c) 2020, CEA
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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Interpolation_IBM_mean_gradient.cpp
// Directory : $TRUST_ROOT/src/EF/Interpolation_IBM
//
/////////////////////////////////////////////////////////////////////////////

#include <Interpolation_IBM_mean_gradient.h>
#include <Param.h>
#include <DoubleTab.h>
#include <DoubleTrav.h>
#include <IntTab.h>
#include <Domaine.h>

Implemente_instanciable( Interpolation_IBM_mean_gradient, "Interpolation_IBM_gradient_moyen", Interpolation_IBM_base ) ;

Sortie& Interpolation_IBM_mean_gradient::printOn( Sortie& os ) const
{
  Objet_U::printOn( os );
  return os;
}

Entree& Interpolation_IBM_mean_gradient::readOn( Entree& is )
{
  Param param(que_suis_je());
  param.ajouter("points_solides",&solid_points_lu_,Param::REQUIRED);
  param.ajouter("est_dirichlet",&is_dirichlet_lu_,Param::REQUIRED);
  param.ajouter("correspondance_elements",&corresp_elems_lu_,Param::REQUIRED);
  param.ajouter("elements_solides",&solid_elems_lu_,Param::REQUIRED);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Interpolation_IBM_mean_gradient::discretise(const Discretisation_base& dis, Zone_dis_base& la_zone_EF)
{
  int nb_comp = Objet_U::dimension;
  Noms units(nb_comp);
  Noms c_nam(nb_comp);

  if ((&corresp_elems_lu_)->non_nul())
    {
      dis.discretiser_champ("champ_elem",la_zone_EF,"corresp_elems","none",1,0., corresp_elems_);
      corresp_elems_.valeur().affecter(corresp_elems_lu_);
    }
  dis.discretiser_champ("champ_sommets",la_zone_EF,"solid_elems","none",1,0., solid_elems_);
  solid_elems_.valeur().affecter(solid_elems_lu_);
  dis.discretiser_champ("champ_sommets",la_zone_EF,"is_dirichlet","none",1,0., is_dirichlet_);
  is_dirichlet_.valeur().affecter(is_dirichlet_lu_);
  dis.discretiser_champ("vitesse",la_zone_EF,vectoriel,c_nam,units,nb_comp,0., solid_points_);
  solid_points_.valeur().affecter(solid_points_lu_);
  computeSommetsVoisins(la_zone_EF);
}

void Interpolation_IBM_mean_gradient::computeSommetsVoisins(Zone_dis_base& la_zone_EF)
{
  int nb_som = la_zone_EF.nb_som();
  int nb_som_tot = la_zone_EF.nb_som_tot();
  int nb_elem = la_zone_EF.nb_elem();
  int nb_elem_tot = la_zone_EF.nb_elem_tot();
  int nb_som_elem = la_zone_EF.zone().nb_som_elem();
  DoubleTab& elems_solid_ref = solid_elems_.valeur().valeurs();
  const IntTab& elems = la_zone_EF.zone().les_elems();
  const DoubleTab& coordsDom = la_zone_EF.zone().domaine().coord_sommets();
  const DoubleTab& solidPointsCoords = solid_points_.valeur().valeurs();

  //Cerr << "nb_som_elem = " << nb_som_elem << finl;

  DoubleTab& is_dirichlet = is_dirichlet_.valeur().valeurs();

  // On cree un indicateur nodal reperant si un noeud appartient a un ou des elements
  // etant totalement fluide (sans noeuds CL Dirichelt immergee)
  DoubleTrav is_elem_fluide(nb_elem_tot);
  DoubleTrav is_node_voisin_elem_fluide(is_dirichlet);
  is_node_voisin_elem_fluide = 0.;
  for (int num_elem_tf = 0; num_elem_tf < nb_elem_tot; num_elem_tf++)
    {
      is_elem_fluide(num_elem_tf) = 1.;
      // Est-ce vrai ?
      for (int j_tf = 0; j_tf < nb_som_elem; j_tf++)
        {
          int num_som_tf = elems(num_elem_tf,j_tf);
          if ((num_som_tf < nb_som) && (is_dirichlet(num_som_tf) > 0.0)) is_elem_fluide(num_elem_tf) = 0.;
        }
      if (is_elem_fluide(num_elem_tf) == 1.)
        {
          for (int j_tf = 0; j_tf < nb_som_elem; j_tf++)
            {
              int num_som_tf = elems(num_elem_tf,j_tf);
              is_node_voisin_elem_fluide(num_som_tf) = 1.;
            }
        }
    }

  // La numerotation des elements peut avoir changee entre l'etape pre_pro et
  // l'etape calcul (cas du pre_pro Salome)
  // On utilise un champ d'etiquette pour solid_elems_ (par exemple le no elem Salome)
  // et un champ d'element reprenant ces etiquettes
  if ((&corresp_elems_)->non_nul())
    {
      DoubleTab& corresp_elems_ref = corresp_elems_.valeur().valeurs();
      int nb_tag_max = -1;
      for (int i = 0 ; i < nb_elem_tot ; i++)
        {
          int int_cor_elem = lrint(corresp_elems_ref(i));
          if (int_cor_elem > nb_tag_max) nb_tag_max = int_cor_elem;
        }
      int dimtag = nb_tag_max+1;
      DoubleTrav elems_solid_trust(dimtag);
      elems_solid_trust = -1.e+50;
      for (int i = 0 ; i < nb_elem_tot ; i++)
        {
          int indextag = lrint(corresp_elems_ref(i)) ;
          if (indextag < dimtag && indextag >= 0)
            {
              elems_solid_trust(indextag) = i*1.0;
            }
          else
            {
              Cerr<<"erreur in computeSommetsVoisins : elem corresp_elems_ref(elem) = "<<i<<" "<<corresp_elems_ref(i)<<" ; indextag = "<<indextag<<" < 0 ou >= "<<dimtag<<finl;
              exit();
            }
        }
      for (int i = 0 ; i < nb_som_tot ; i++)
        {
          if (elems_solid_ref(i) >= 0.0)
            {
              int indexr = lrint(elems_solid_ref(i));
              if (indexr < dimtag && indexr >= 0)
                {
                  if (elems_solid_trust(indexr) >= 0.) elems_solid_ref(i) = elems_solid_trust(indexr);
                }
            }
        }
    }

  sommets_voisins_ = new IntList[nb_som];
  int nb_nodes_in_list = 0;

  for (int num_elem = 0; num_elem < nb_elem_tot; num_elem++)
    {
      for (int j = 0; j < nb_som_elem; j++)
        {
          int num_som = elems(num_elem,j);
          double x1 = coordsDom(num_som,0);
          double x2 = coordsDom(num_som,1);
          double x3 = coordsDom(num_som,2);
          double xp1 = solidPointsCoords(num_som,0);
          double xp2 = solidPointsCoords(num_som,1);
          double xp3 = solidPointsCoords(num_som,2);

          if ((num_som < nb_som) && (is_dirichlet(num_som) > 0.0))
            {
              for (int k = 0; k < nb_som_elem; k++)
                {
                  if (k != j)
                    {
                      int num_som_2 = elems(num_elem,k);
                      double xf1 = coordsDom(num_som_2,0);
                      double xf2 = coordsDom(num_som_2,1);
                      double xf3 = coordsDom(num_som_2,2);
                      double xpf1 = solidPointsCoords(num_som_2,0);
                      double xpf2 = solidPointsCoords(num_som_2,1);
                      double xpf3 = solidPointsCoords(num_som_2,2);

                      double d = (x1-xp1)*(xf1-xpf1)+(x2-xp2)*(xf2-xpf2)+(x3-xp3)*(xf3-xpf3);
                      double d1 = (xf1-xpf1)*(xf1-xpf1)+(xf2-xpf2)*(xf2-xpf2)+(xf3-xpf3)*(xf3-xpf3);
                      double d2 = (x1-xp1)*(x1-xp1)+(x2-xp2)*(x2-xp2)+(x3-xp3)*(x3-xp3);

                      // On demande que :
                      // *) le point voisin soit de type fluide (pas Dirichlet)
                      // *) le projete du point fluide voisin soit du meme cote de la frontiere
                      //    immergee que le point Dirichlet considere
                      // *) le projete du point fluide soit dans un element traverse par la
                      //    frontiere (on considere la vitesse de la frontiere immergee en ce point)
                      // *) le projete du point fluide voisin soit a une distance carre d1 de la
                      //    frontiere immergee plus grande que celle d2 du point Dirichlet
                      //    considere (on interpole; on n'extrapole pas; critere de
                      //    distance : d1 = 2.0^2 fois d2)
                      // *) le point fluide voisin appartienne a au moins un element totalement fluide
                      if (is_dirichlet(num_som_2) < 0.0 && d > 0.0 && (4.0*d2) < d1 && is_node_voisin_elem_fluide(num_som_2) == 1.0)
                        {
                          // Element contenant le projete du point fluide
                          int elems_xpf = lrint(elems_solid_ref(num_som_2));
                          bool flag_xpf = true;
                          bool flag_prt_nodes = false;
                          int elem_found = la_zone_EF.zone().chercher_elements(xpf1,xpf2,xpf3);
                          if (elems_xpf >= 0)
                            {
                              // test de verification prepro Salome/Trust :
                              if ((elems_xpf != elem_found) && (elem_found != -1))
                                {
                                  Cerr << __FILE__ << __LINE__ << "Interpolation_IBM_mean_gradient::computeSommetsVoisins : ERROR : elem solid prepro != elem solid Trust" << finl;
                                  flag_xpf = false;
                                  flag_prt_nodes = true;
                                }
                            }
                          else
                            {
                              if (elems_xpf == -1.e+50)
                                {
                                  flag_xpf = false;
                                }
                              else if (elem_found != -1)
                                {
                                  Cerr<<"node_fluide = "<<num_som_2<<" On utilise chercher_elements(xpf,ypf,zpf) = "<<elem_found<<" pour supleer au prepro Salome = "<<elems_xpf<<finl;
                                  Cerr<<"coords_point(node_fluide) : xf yf zf = "<<xf1<<" "<<xf2<<" "<<xf3<<finl;
                                  Cerr<<"solid_points(node_fluide) : xpf ypf zpf = "<<xpf1<<" "<<xpf2<<" "<<xpf3<<finl;
                                  elems_xpf = elem_found ;
                                }
                            }
                          if (!flag_xpf)
                            {
                              // Traitement des erreurs
                              Cerr << __FILE__ << __LINE__ << "Interpolation_IBM_mean_gradient::computeSommetsVoisins : ERROR : joint width too low?" << finl;
                              Cerr<<"Nb elem; Nb elem tot = "<<nb_elem<<" "<<nb_elem_tot<<finl;
                              Cerr<<"Nb som;  Nb som tot  = "<<nb_som<<" "<<nb_som_tot<<finl;
                              Cerr<<"element; node_Dirichlet = "<<num_elem<<" "<<num_som<<finl;
                              Cerr<<"coords_point(node_Dirichlet) : x y z    = "<<x1<<" "<<x2<<" "<<x3<<finl;
                              Cerr<<"solid_points(node_Dirichlet) : xp yp zp = "<<xp1<<" "<<xp2<<" "<<xp3<<finl;
                              Cerr<<"node_fluide elems_solid_ref(node_fluide) elems_xpf = "<<num_som_2<<" "<<elems_solid_ref(num_som_2)<<" "<<elems_xpf<<finl;
                              Cerr<<"coords_point(node_fluide) : xf yf zf    = "<<xf1<<" "<<xf2<<" "<<xf3<<finl;
                              Cerr<<"solid_points(node_fluide) : xpf ypf zpf = "<<xpf1<<" "<<xpf2<<" "<<xpf3<<finl;
                              Cerr<<"chercher_elements(xpf,ypf,zpf) = "<<elem_found<<finl;
                              if (flag_prt_nodes)
                                {
                                  double x1kf = 0.0;
                                  double x2kf = 0.0;
                                  double x3kf = 0.0;
                                  double x1kf2 = 0.0;
                                  double x2kf2 = 0.0;
                                  double x3kf2 = 0.0;
                                  for (int kf = 0; kf < nb_som_elem; kf++)
                                    {
                                      int num_som_kf = elems(elems_xpf,kf);
                                      int num_som_kf2 = elems(elem_found,kf);
                                      x1kf += coordsDom(num_som_kf,0)/nb_som_elem;
                                      x2kf += coordsDom(num_som_kf,1)/nb_som_elem;
                                      x3kf += coordsDom(num_som_kf,2)/nb_som_elem;
                                      x1kf2 += coordsDom(num_som_kf2,0)/nb_som_elem;
                                      x2kf2 += coordsDom(num_som_kf2,1)/nb_som_elem;
                                      x3kf2 += coordsDom(num_som_kf2,2)/nb_som_elem;
                                      Cerr<<"x elems_solid_ref   = "<<coordsDom(num_som_kf,0)<<" "<<coordsDom(num_som_kf,1)<<" "<<coordsDom(num_som_kf,2)<<finl;
                                      Cerr<<"x chercher_elements = "<<coordsDom(num_som_kf2,0)<<" "<<coordsDom(num_som_kf2,1)<<" "<<coordsDom(num_som_kf2,2)<<finl;
                                    }
                                  Cerr<<"bary elems_solid_ref   = "<<x1kf<<" "<<x2kf<<" "<<x3kf<<finl;
                                  Cerr<<"bary chercher_elements = "<<x1kf2<<" "<<x2kf2<<" "<<x3kf2<<finl;
                                }
                              exit();
                            }
                          // On demande que le projete du point fluide soit dans un element
                          // traverse par la frontiere ou borde d'elements traverses par la frontiere
                          if (elems_xpf >= 0)
                            {
                              for (int kxpf = 0; kxpf < nb_som_elem; kxpf++)
                                {
                                  int num_som_2xpf = elems(elems_xpf,kxpf);
                                  if (is_dirichlet(num_som_2xpf) < 0.0) flag_xpf = false;
                                }
                              if (flag_xpf)
                                {
                                  sommets_voisins_[num_som].add_if_not(num_som_2);
                                  nb_nodes_in_list += 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
  Cerr << __FILE__ << __LINE__ << "Interpolation_IBM_mean_gradient::computeSommetsVoisins : nb nodes for interpolation = " <<nb_nodes_in_list<<finl;
}

