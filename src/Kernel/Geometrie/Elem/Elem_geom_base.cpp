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

#include <Elem_geom_base.h>
#include <Domaine.h>

Implemente_base_32_64(Elem_geom_base_32_64,"Elem_geom_base",Objet_U);

/*! @brief NE FAIT RIEN
 *
 * @param (Sortie& s) un flot de sortie
 * @return (Sortie&) le flot de sortie
 */
template<class _SIZE_>
Sortie& Elem_geom_base_32_64<_SIZE_>::printOn(Sortie& s ) const
{
  return s;
}

/*! @brief NE FAIT RIEN
 *
 * @param (Entree& s) un flot d'entree
 * @return (Entree&) le flot d'entree
 */
template<class _SIZE_>
Entree& Elem_geom_base_32_64<_SIZE_>::readOn(Entree& s )
{
  return s;
}


/*! @brief Renvoie le parametre face si le type de face specifie est le meme que celui de l'element geometrique.
 *
 * @param (int face)
 * @param (Type_Face& type) un type de face
 * @throws mauvais type de face specifie
 */
template <typename _SIZE_>
int Elem_geom_base_32_64<_SIZE_>::num_face(int face, Type_Face& type) const
{
  assert(type==type_face());
  return face;
}

/*! @brief Cree les faces de l'element geometrique specifie du domaine en precisant le type de face a creer.
 *
 * @param (Faces_t& les_faces) les faces a creer
 * @param (int num_elem) le numero de l'element dont on veut creer les faces
 * @param (Type_Face type) le type de face a creer
 */
template <typename _SIZE_>
void Elem_geom_base_32_64<_SIZE_>::creer_faces_elem(Faces_t& les_faces ,
                                                    _SIZE_ num_elem,
                                                    Type_Face type) const
{
  int type_id=0;
  for(; ((type_id<nb_type_face())
         &&(type!=type_face(type_id))); type_id++)
    ;
  const IntTab_t& les_Polys = mon_dom->les_elems();
  assert(les_Polys.dimension_tot(0) > num_elem);
  int face, i;
  les_faces.dimensionner(nb_faces(type_id));
  les_faces.associer_domaine(mon_dom.valeur());

  for(face=0; face<nb_faces(type_id); face++)
    {
      int face_id=num_face(face, type);
      for (i=0; i<nb_som_face(type_id); i++)
        les_faces.sommet(face_id,i) = les_Polys(num_elem,face_sommet(face_id, i));
      les_faces.completer(face_id, num_elem);
    }
}


/*! @brief Calcule les centres de gravites de tous les elements du domaine associe a l'element goemetrique.
 *
 * @param (DoubleTab& xp) le tableau contenant les coordonnees des centres de gravite
 */
template <typename _SIZE_>
void Elem_geom_base_32_64<_SIZE_>::calculer_centres_gravite(DoubleTab_t& tab_xp) const
{
  int_t nb_elem;
  if(tab_xp.dimension(0)==0)
    {
      nb_elem = mon_dom->nb_elem_tot();
      tab_xp.resize(nb_elem,dimension);
    }
  else
    nb_elem=tab_xp.dimension(0);

  int nb_som_elem = nb_som();
  int dim = Objet_U::dimension;
  // ToDo create a type in View_Types.h ?
  ConstView<_SIZE_,2> les_Polys = mon_dom->les_elems().view_ro();
  //CIntTabView les_Polys = mon_dom->les_elems().view_ro();
  CDoubleTabView coord = mon_dom->coord_sommets().view_ro();
  DoubleTabView xp = tab_xp.view_wo();
  Kokkos::parallel_for(start_gpu_timer(__KERNEL_NAME__), nb_elem, KOKKOS_LAMBDA(const int_t num_elem)
  {
    for (int i = 0; i < dim; i++)
      xp(num_elem, i) = 0;
    int nb_som_reel = nb_som_elem;
    while (les_Polys(num_elem, nb_som_reel - 1) == -1) nb_som_reel--;
    for (int s = 0; s < nb_som_reel; s++)
      {
        int_t num_som = les_Polys(num_elem, s);
        for (int i = 0; i < dim; i++)
          xp(num_elem, i) += coord(num_som, i) / nb_som_reel;
      }
  });
  end_gpu_timer(__KERNEL_NAME__);
}

/*! @brief Sort en erreur.
 *
 * Cette methode n'est pas virtuelle pure pour des raisons de commodite
 *
 * @param (DoubleTab& xp) le tableau contenant les coordonnees des centres de gravite
 */
template <typename _SIZE_>
void Elem_geom_base_32_64<_SIZE_>::calculer_normales(const IntTab_t& faces_sommets , DoubleTab_t& face_normales) const
{
  Cerr << "calculer_normales method is not coded for an element " << finl;
  Cerr << "of type que_suis_je() " << finl;
  exit();
}

/*! @brief Renvoie le nombre de type de face de l'element geometrique.
 *
 *     Par exemple un prisme (Classe Prisme)
 *     a 2 types de faces: un triangle ou un quadrangle.
 *
 * @return (int) renvoie toujours 1
 */
template <typename _SIZE_>
int Elem_geom_base_32_64<_SIZE_>::nb_type_face() const
{
  return 1;
}

/*! @brief remplit le tableau faces_som_local(i,j) qui donne pour 0 <= i < nb_faces()  et  0 <= j < nb_som_face(i) le numero local du sommet
 *
 *   sur l'element.
 *   On a  0 <= faces_sommets_locaux(i,j) < nb_som()
 *  Si toutes les faces de l'element n'ont pas le meme nombre de sommets, le nombre
 *  de colonnes du tableau est le plus grand nombre de sommets, et les cases inutilisees
 *  du tableau sont mises a -1
 *  On renvoie 1 si toutes les faces ont le meme nombre d'elements, 0 sinon.
 */
template <typename _SIZE_>
int Elem_geom_base_32_64<_SIZE_>::get_tab_faces_sommets_locaux(IntTab& faces_som_local) const
{
  Cerr << "Elem_geom_base::faces_sommets_locaux : error.\n"
       << " Method not implemented for the object "
       << que_suis_je() << finl;
  exit();
  return 0;
}

/*! @brief idem que Elem_geom_base::get_tab_faces_sommets_locaux mais pour les aretes: aretes_som_local.
 *
 * dimension(0) = nombre d'aretes sur l'element de reference
 *   aretes_som_local.dimension(1) = 2  (nombre de sommets par arete)
 *   aretes_som_local(i,j) = numero d'un sommet de l'element (0 <= n < nb_sommets_par_element)
 *
 */
template <typename _SIZE_>
void Elem_geom_base_32_64<_SIZE_>::get_tab_aretes_sommets_locaux(IntTab& aretes_som_local) const
{
  Cerr << "Elem_geom_base::aretes_sommets_locaux : error.\n"
       << " Method not implemented for the object "
       << que_suis_je() << finl;
  exit();
}


template class Elem_geom_base_32_64<int>;
#if INT_is_64_ == 2
template class Elem_geom_base_32_64<trustIdType>;
#endif

