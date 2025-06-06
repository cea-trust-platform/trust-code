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
#include <Domaine_bord.h>
#include <Hexaedre.h>

Implemente_instanciable_32_64(Domaine_bord_32_64,"Domaine_bord",Domaine_32_64<_T_>);

/*! @brief pour l'instant exit()
 *
 */
template <typename _SIZE_>
Entree& Domaine_bord_32_64<_SIZE_>::readOn(Entree& is)
{
  this->exit();
  return is;
}

/*! @brief pour l'instant exit()
 *
 */
template <typename _SIZE_>
Sortie& Domaine_bord_32_64<_SIZE_>::printOn(Sortie& os) const
{
  this->exit();
  return os;
}

/*! @brief construit le domaine en appelant extraire_domaine_bord()
 *
 */
template <typename _SIZE_>
void Domaine_bord_32_64<_SIZE_>::construire_domaine_bord(const Domaine_bord_32_64<_SIZE_>::Domaine_t& source, const Nom& nom_bord)
{
  domaine_source_ = source;
  bord_source_ = nom_bord;
  extraire_domaine_bord(source, nom_bord, *this, renum_som_);
}

/*! @brief renvoie une reference au domaine source
 *
 */
template <typename _SIZE_>
const Domaine_32_64<_SIZE_>& Domaine_bord_32_64<_SIZE_>::get_domaine_source() const
{
  return domaine_source_;
}

/*! @brief renvoie le nom du bord source
 *
 */
template <typename _SIZE_>
const Nom& Domaine_bord_32_64<_SIZE_>::get_nom_bord_source() const
{
  return bord_source_;
}

/*! @brief renvoie renum_som (pour chaque sommet du domaine_bord, indice du meme sommet dans le domaine)
 *
 */
template <typename _SIZE_>
const ArrOfInt_T<_SIZE_>& Domaine_bord_32_64<_SIZE_>::get_renum_som() const
{
  return renum_som_;
}

/*! @brief methode pour convertir un type de face en type d'element (a deplacer dans la classe Faces ?)
 *
 */
template <typename _SIZE_>
void type_face_to_type_elem(const Elem_geom_base_32_64<_SIZE_>& type_elem, const Type_Face& type_face, Motcle& type_elem_face)
{
  switch(type_face)
    {
    case Type_Face::vide_0D:
      type_elem_face = "??";
      break;
    case Type_Face::point_1D:
      type_elem_face = "??";
      break;
    case Type_Face::segment_2D:
      type_elem_face = "segment";
      break;
    case Type_Face::segment_2D_axi:
      type_elem_face = "segment";
      break;
    case Type_Face::triangle_3D:
      type_elem_face = "triangle";
      break;
    case Type_Face::quadrilatere_2D_axi:
      type_elem_face = "quadrangle_VEF";
      break;
    case Type_Face::quadrangle_3D:
      type_elem_face = (sub_type(Hexaedre,type_elem)?"rectangle":"quadrangle_VEF");
      break;
    case Type_Face::quadrangle_3D_axi:
      type_elem_face = "quadrangle_VEF";
      break;
    default:
      type_elem_face = "??";
    }
}

/*! @brief remplit le domaine "dest" avec les sommets et les faces du bord "nom_bord" du domaine "src".
 *
 * Les sommets du domaine dest sont uniquement les sommets qui sont sur
 *   une face du bord. Le tableau renum_som est dimensionne a dest.nb_som() et rempli comme
 *   suit: renum_som[i] est l'indice dans le domaine "src" du sommet i du domaine "dest".
 *
 */
template <typename _SIZE_>
void Domaine_bord_32_64<_SIZE_>::extraire_domaine_bord(const Domaine_t& src,
                                                       const Nom& nom_bord,
                                                       Domaine_t& dest,
                                                       ArrOfInt_t& renum_som)
{
  if (Process::is_parallel())
    {
      Cerr << "extraire_domaine_bord in parallel: the domain created will not have a distributed structure\n"
           << " (this will be done one day... ask to B.Mathieu)" << finl;
    }

  // Le domaine destination doit etre vide:
  assert(dest.nb_elem() == 0);
  // Initialisation du domaine:
  // On choisit un nom pour le domaine
  dest.nommer(src.le_nom() + Nom("_") + nom_bord);
  // Type des elements du domaine dest:
  Motcle type_elem;
  type_face_to_type_elem(src.type_elem().valeur(), src.type_elem()->type_face(), type_elem);
  dest.type_elem().typer(type_elem);
  dest.type_elem()->associer_domaine(dest);

  const Frontiere_t& front = src.frontiere(nom_bord);
  const int_t nb_faces = front.faces().nb_faces();
  const int nb_som_face = front.faces().nb_som_faces();
  const IntTab_t& faces_src = front.faces().les_sommets();
  IntTab_t& elem_dest = dest.les_elems();
  elem_dest.resize(nb_faces, nb_som_face);
  renum_som.reset();

  // renum_inverse: pour chaque sommet du domaine source, son indice dans le domaine destination:
  ArrOfInt_t renum_inverse(src.nb_som());
  renum_inverse= -1;
  int_t nb_som_dest = 0;
  for (int_t i = 0; i < nb_faces; i++)
    {
      for (int j = 0; j < nb_som_face; j++)
        {
          const int_t som = faces_src(i, j);
          // Si le sommet n'a pas encore ete rencontre, lui donner un indice dans le domaine dest:
          if (renum_inverse[som] < 0)
            {
              renum_som.append_array(som);
              renum_inverse[som] = nb_som_dest++;
            }
          elem_dest(i, j) = renum_inverse[som];
        }
    }
  // Copie des sommets utilises dans le domaine destination
  DoubleTab_t& som_dest = dest.les_sommets();
  const DoubleTab_t& som_src = src.les_sommets();
  const int dim = static_cast<int>(som_src.dimension(1));
  som_dest.resize(nb_som_dest, dim);
  for (int_t i = 0; i < nb_som_dest; i++)
    {
      const int_t som = renum_som[i];
      for (int j = 0; j < dim; j++)
        som_dest(i, j) = som_src(som, j);
    }

  // A faire si besoin: initialiser le joint des sommets pour avoir les items communs,
  //  et autres si necessaire.
}

template class Domaine_bord_32_64<int>;
#if INT_is_64_ == 2
template class Domaine_bord_32_64<trustIdType>;
#endif

