/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Domaine_Cl_DG.h>
#include <Domaine_DG.h>
#include <Option_DG.h>
#include <Poly_geom_base.h>
#include <Statistiques.h>
#include <Array_tools.h>
#include <TRUSTLists.h>
#include <TRUSTList.h>
#include <Domaine.h>
#include <Quadrature.h>

Implemente_instanciable(Domaine_DG, "Domaine_DG", Domaine_Poly_base);

Sortie& Domaine_DG::printOn(Sortie& os) const { return Domaine_Poly_base::printOn(os); }

Entree& Domaine_DG::readOn(Entree& is) { return Domaine_Poly_base::readOn(is); }

void Domaine_DG::discretiser()
{
  Domaine_Poly_base::discretiser();

  // fill dof_elem_ !!!
//  calculer_h_carre();
//  discretiser_aretes();
}

void Domaine_DG::calculer_h_carre()
{
  // Calcul de h_carre
  h_carre = 1.e30;
  h_carre_.resize(nb_faces());
  // Calcul des surfaces
  Elem_geom_base& elem_geom = domaine().type_elem().valeur();
  int is_polyedre = sub_type(Poly_geom_base, elem_geom) ? 1 : 0;
  const ArrOfInt PolyIndex = is_polyedre ? ref_cast(Poly_geom_base, domaine().type_elem().valeur()).getElemIndex() : ArrOfInt(0);
  const DoubleVect& surfaces=face_surfaces();
  const int nbe=nb_elem();
  for (int num_elem=0; num_elem<nbe; num_elem++)
    {
      double surf_max = 0;
      const int nb_faces_elem = is_polyedre ? PolyIndex[num_elem+1] - PolyIndex[num_elem] : domaine().nb_faces_elem();
      for (int i=0; i<nb_faces_elem; i++)
        {
          double surf = surfaces(elem_faces(num_elem,i));
          surf_max = (surf > surf_max)? surf : surf_max;
        }
      double vol = volumes(num_elem)/surf_max;
      vol *= vol;
      h_carre_(num_elem) = vol;
      h_carre = ( vol < h_carre )? vol : h_carre;
    }
}

void Domaine_DG::remplir_elem_faces()
{
}

void Domaine_DG::modifier_pour_Cl(const Conds_lim& conds_lim)
{
  Cerr << "Le Domaine_DG a ete rempli avec succes" << finl;
  //      calculer_h_carre();

  Journal() << "Domaine_DG::Modifier_pour_Cl" << finl;
  int nb_cond_lim=conds_lim.size();
  int num_cond_lim=0;
  for (; num_cond_lim<nb_cond_lim; num_cond_lim++)
    {
      //for cl
      const Cond_lim_base& cl = conds_lim[num_cond_lim].valeur();
      if (sub_type(Periodique, cl))
        {
          //if Perio
          const Periodique& la_cl_period = ref_cast(Periodique,cl);
          int nb_faces_elem = domaine().nb_faces_elem();
          const Front_VF& la_front_dis = ref_cast(Front_VF,cl.frontiere_dis());
          int ndeb = 0;
          int nfin = la_front_dis.nb_faces_tot();
#ifndef NDEBUG
          int num_premiere_face = la_front_dis.num_premiere_face();
          int num_derniere_face = num_premiere_face+nfin;
#endif
          int nbr_faces_bord = la_front_dis.nb_faces();
          assert((nb_faces()==0)||(ndeb<nb_faces()));
          assert(nfin>=ndeb);
          int elem1,elem2,k;
          int face;
          // Modification des tableaux face_voisins_ , face_normales_ , volumes_entrelaces_
          // On change l'orientation de certaines normales
          // de sorte que les normales aux faces de periodicite soient orientees
          // de face_voisins(la_face_en_question,0) vers face_voisins(la_face_en_question,1)
          // comme le sont les faces internes d'ailleurs

          DoubleVect C1C2(dimension);
          double vol,psc=0;

          for (int ind_face=ndeb; ind_face<nfin; ind_face++)
            {
              //for ind_face
              face = la_front_dis.num_face(ind_face);
              if  ( (face_voisins_(face,0) == -1) || (face_voisins_(face,1) == -1) )
                {
                  int faassociee = la_front_dis.num_face(la_cl_period.face_associee(ind_face));
                  if (ind_face<nbr_faces_bord)
                    {
                      assert(faassociee>=num_premiere_face);
                      assert(faassociee<num_derniere_face);
                    }

                  elem1 = face_voisins_(face,0);
                  elem2 = face_voisins_(faassociee,0);
                  vol = (volumes_[elem1] + volumes_[elem2])/nb_faces_elem;
                  volumes_entrelaces_[face] = vol;
                  volumes_entrelaces_[faassociee] = vol;
                  face_voisins_(face,1) = elem2;
                  face_voisins_(faassociee,0) = elem1;
                  face_voisins_(faassociee,1) = elem2;
                  psc = 0;
                  for (k=0; k<dimension; k++)
                    {
                      C1C2[k] = xv_(face,k) - xp_(face_voisins_(face,0),k);
                      psc += face_normales_(face,k)*C1C2[k];
                    }

                  if (psc < 0)
                    for (k=0; k<dimension; k++)
                      face_normales_(face,k) *= -1;

                  for (k=0; k<dimension; k++)
                    face_normales_(faassociee,k) = face_normales_(face,k);
                }
            }
        }
    }

  // PQ : 10/10/05 : les faces periodiques etant a double contribution
  //		      l'appel a marquer_faces_double_contrib s'effectue dans cette methode
  //		      afin de pouvoir beneficier de conds_lim.
  Domaine_VF::marquer_faces_double_contrib(conds_lim);
}

void Domaine_DG::set_quadrature(int order, const Quadrature* quad)
{
  assert(quad_map_.count(order) == 0); // fail if a quadrature is already registered for this order
  quad_map_[order] = quad;
}

const Quadrature& Domaine_DG::get_quadrature(int order) const
{
  return *quad_map_.at(order);
}

