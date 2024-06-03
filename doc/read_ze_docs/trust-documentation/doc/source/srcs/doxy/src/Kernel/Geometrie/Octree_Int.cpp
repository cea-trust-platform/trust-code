/****************************************************************************
* Copyright (c) 2023, CEA
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
#include <Octree_Int.h>
#include <ArrOfBit.h>

static const int max_levels_ = 32; // 1 de plus que le nombre de bits=1 dans coords_max
// La valeur suivante doit etre une puissance de deux
const int Octree_Int::root_octree_half_width_ = 1073741824; /* 2^30   = 0100 0000  0000 0000  0000 0000  0000 0000b */
// La valeur suivante doit etre egale a (root_octree_half_width_ * 2 - 1)
const int Octree_Int::coord_max_ = 2147483647;              /* 2^31-1 = 0111 1111  1111 1111  1111 1111  1111 1111b */

/*! @brief construction d'un octree_id (voir octree_structure_) Si type==EMPTY, on l'octree_id est 0
 *
 *   Si type==OCTREE, on suppose que index est un indice dans octree_structure_
 *   Si type==FLOOR, on suppose que index est un indice dans floor_elements_
 *
 */
inline int Octree_Int::octree_id(int index, Octree_Type type)
{
  switch(type)
    {
    case EMPTY:
      return 0;
    case OCTREE:
      return index + 1;
    case FLOOR:
      return - index - 1;
    }
  return -1;
}

/*! @brief calcul de l'index de l'octree dans octree_structure ou floor_elements en fonction du type de l'octree et de son octree_id.
 *
 *   En general on a deja determine le type avant, on le passe en parametre pour optimiser.
 *
 */
inline int Octree_Int::octree_index(int octree_id, Octree_Type type)
{
  assert(type==octree_type(octree_id));
  switch(type)
    {
    case EMPTY:
      return -1;
    case OCTREE:
      return octree_id - 1;
    case FLOOR:
      return - octree_id - 1;
    }
  return -1;
}

/*! @brief Renvoie le type d'un octree en fonction de son octree_id.
 *
 */
inline Octree_Int::Octree_Type Octree_Int::octree_type(int octree_id)
{
  if (octree_id > 0)
    return OCTREE;
  else if (octree_id == 0)
    return EMPTY;
  else
    return FLOOR;
}

/*! @brief construction de l'octree.
 *
 * On donne la dimension (1, 2 ou 3) et un tableau d'elements a stocker dans l'octree. Deux possibilites:
 *   1) les elements sont ponctuels si elements_boxes.dimension(1) == dimension.
 *      Dans ce cas, chaque element se trouve dans un et un seul octree_floor
 *   2) les elements sont des parallelipipedes, si elements_boxes.dimension(1) == dimension*2
 *      Les "dimension" premieres colonnes sont les coordonnees inferieures,
 *      les "dimension" suivantes sont les coordonnees superieures.
 *      Un parallelipipede peut etre affecte a plusieurs octree_floor.
 *   Les coordonnees stockees dans elements_boxes peuvent aller de 0 a coord_max_ inclus.
 *   Il vaut mieux utiliser toute la plage des entiers en multipliant par un facteur adequat.
 *
 */
void Octree_Int::build(const int dimension, const IntTab& elements_boxes)
{
  assert(dimension >= 1 && dimension <= 3);
  assert(elements_boxes.dimension(1) == dimension
         || elements_boxes.dimension(1) == dimension * 2 );

  const int nb_elems = elements_boxes.dimension(0);
  nb_elements_ = nb_elems;

  octree_structure_.set_smart_resize(1);
  floor_elements_.set_smart_resize(1);
  floor_elements_.resize_array(0);
  const int nb_octrees = 1 << dimension;
  octree_structure_.resize(0, nb_octrees);

  assert(elements_boxes.size_array() == 0
         || (min_array(elements_boxes) >= 0 && max_array(elements_boxes) <= coord_max_));

  ArrsOfInt tmp_elem_flags(max_levels_);
  ArrsOfInt tmp_elem_list(max_levels_);
  for (int i = 0; i < max_levels_; i++)
    {
      tmp_elem_flags[i].set_smart_resize(1);
      tmp_elem_list[i].set_smart_resize(1);
    }
  ArrOfInt& elements_list = tmp_elem_list[0];
  elements_list.resize_array(nb_elems, ArrOfInt::NOCOPY_NOINIT);
  for (int i = 0; i < nb_elems; i++)
    elements_list[i] = i;

  root_octree_id_ = build_octree_recursively(root_octree_half_width_,root_octree_half_width_,root_octree_half_width_,
                                             root_octree_half_width_,
                                             elements_boxes,
                                             tmp_elem_list,
                                             0,
                                             tmp_elem_flags);
}

/*! @brief renvoie la liste des elements contenant potentiellement le point (x,y,z) On renvoie n=nombre d'elements de la liste et les elements sont dans
 *
 *   floor_elements()[index+i] pour 0 <= i < n.
 *   En realite on renvoie tous les elements qui ont une intersection non vide avec l'octree_floor
 *   contenant le point (x,y,z)
 *
 */
int Octree_Int::search_elements(int x, int y, int z, int& index) const
{
  const int nb_octrees = octree_structure_.dimension(1);
  if (nb_octrees == 2)
    y = 0; // important pour ne pas tomber sur des cubes inexistants
  if (nb_octrees <= 4)
    z = 0; // idem
  assert(x >= 0 && x <= coord_max_);
  assert(y >= 0 && y <= coord_max_);
  assert(z >= 0 && z <= coord_max_);

  const int my_octree_id = search_octree_floor(x, y, z);

  if (octree_type(my_octree_id) == EMPTY)
    {
      return 0;
    }
  const int idx = octree_index(my_octree_id, FLOOR);
  const int n = floor_elements_[idx];
  index = idx + 1;
  return n;
}

struct IntBoxData
{
  IntBoxData(int xmin, int ymin, int zmin,
             int xmax, int ymax, int zmax,
             ArrOfInt& elements,
             ArrOfBit *markers) :
    xmin_(xmin), ymin_(ymin), zmin_(zmin),
    xmax_(xmax), ymax_(ymax), zmax_(zmax),
    elements_(elements),
    markers_(markers) { };
  int xmin_, ymin_, zmin_;
  int xmax_, ymax_, zmax_;
  ArrOfInt& elements_;
  ArrOfBit *markers_;
};

/*! @brief cherche les elements ayant potentiellement une intersection non vide avec la boite xmin.
 *
 * .zmax. Le tableau elements doit etre de type smart_resize(1).
 *   Les elements peuvent apparaitre plusieurs fois dans le tableau "elements"
 *
 */
int Octree_Int::search_elements_box(int xmin, int ymin, int zmin,
                                    int xmax, int ymax, int zmax,
                                    ArrOfInt& elements) const
{
  const int nb_octrees = octree_structure_.dimension(1);
  if (nb_octrees == 2)
    ymin = ymax = 0; // important pour ne pas tomber sur des cubes inexistants
  if (nb_octrees <= 4)
    zmin = zmax = 0; // idem
  assert(xmin >= 0 && xmin <= coord_max_);
  assert(ymin >= 0 && ymin <= coord_max_);
  assert(zmin >= 0 && zmin <= coord_max_);
  assert(xmax >= 0 && xmax <= coord_max_);
  assert(ymax >= 0 && ymax <= coord_max_);
  assert(zmax >= 0 && zmax <= coord_max_);

  elements.resize_array(0);
  IntBoxData boxdata(xmin, ymin, zmin, xmax, ymax, zmax, elements, 0);
  switch(octree_type(root_octree_id_))
    {
    case FLOOR:
      search_elements_box_floor(boxdata, root_octree_id_);
      break;
    case OCTREE:
      search_elements_box_recursively(boxdata, root_octree_id_,
                                      root_octree_half_width_,root_octree_half_width_,root_octree_half_width_,
                                      root_octree_half_width_);
      break;
    case EMPTY:
      break;
    }
  const int n = elements.size_array();
  return n;
}

/*! @brief ajoute des elements de l'octree_floor a boxdata.
 *
 * elements_
 *
 */
void Octree_Int::search_elements_box_floor(IntBoxData& boxdata,
                                           int octree_floor_id) const
{
  const int idx = octree_index(octree_floor_id, FLOOR);
  const int n = floor_elements_[idx];
  if (boxdata.markers_)
    for (int i = 0; i < n; i++)
      {
        const int elem = floor_elements_[idx+1+i];
        if (!boxdata.markers_->testsetbit(elem))
          boxdata.elements_.append_array(elem);
      }
  else
    for (int i = 0; i < n; i++)
      {
        const int elem = floor_elements_[idx+1+i];
        boxdata.elements_.append_array(elem);
      }
}

// Pour chaque direction, drapeaux des cubes de la rangee inferieure
static int sub_cube_flags_min[3] = { 1+4+16+64, /* drapeaux des cubes 0,2,4,6 */
                                     1+2+16+32, /* drapeaux des cubes 0,1,4,5 */
                                     1+2+4+8    /* drapeaux des cubes 0,1,2,3 */
                                   };
static int sub_cube_flags_max[3] = { 2+8+32+128, /* drapeaux des cubes 1,3,5,7 */
                                     4+8+64+128, /* drapeaux des cubes 2,3,7,8 */
                                     16+32+64+128 /* drapeaux des cubes 4,5,6,7 */
                                   };

/*! @brief cherche recursivement les elements inclus dans la boite boxdata pour l'octree_id donne, de centre cx, cy, cz.
 *
 */

void Octree_Int::search_elements_box_recursively(IntBoxData& boxdata,
                                                 int the_octree_id,
                                                 int cx, int cy, int cz,
                                                 int half_width) const
{
  int flags = 255;
  if (cx > boxdata.xmax_) // les cubes superieurs en x ne sont pas dedans
    flags &= sub_cube_flags_min[0];
  if (cx <= boxdata.xmin_) // les cubes inferieurs ne sont pas dedans
    flags &= sub_cube_flags_max[0];
  if (cy > boxdata.ymax_)
    flags &= sub_cube_flags_min[1];
  if (cy <= boxdata.ymin_)
    flags &= sub_cube_flags_max[1];
  if (cz > boxdata.zmax_)
    flags &= sub_cube_flags_min[2];
  if (cz <= boxdata.zmin_)
    flags &= sub_cube_flags_max[2];
  int test_flag = 1;
  const int idx = octree_index(the_octree_id, OCTREE);
  const int half_width_2 = half_width >> 1;
  const int mhalf_width = - half_width_2;
  int cx2, cy2, cz2;
  for (int i = 0; i < 8; i++, test_flag <<= 1)
    {
      if ((flags & test_flag) != 0)
        {
          const int id = octree_structure_(idx, i);
          switch(octree_type(id))
            {
            case FLOOR:
              search_elements_box_floor(boxdata, id);
              break;
            case OCTREE:
              cx2 = cx + ((i & 1) ? half_width_2 : mhalf_width);
              cy2 = cy + ((i & 2) ? half_width_2 : mhalf_width);
              cz2 = cz + ((i & 4) ? half_width_2 : mhalf_width);
              search_elements_box_recursively(boxdata, id,
                                              cx2, cy2, cz2,
                                              half_width_2);
              break;
            case EMPTY:
              break;
            }
        }
    }
}

void Octree_Int::reset()
{
  root_octree_id_ = octree_id(0, EMPTY);
  nb_elements_ = 0;
  octree_structure_.reset();
  floor_elements_.reset();
}

/*! @brief construit un octree_floor avec la liste d'elements donnee et renvoie l'octree_id de cet octree_floor
 *
 */
int Octree_Int::build_octree_floor(const ArrOfInt& elements_list)
{
  const int nb_elems = elements_list.size_array();
  const int index = floor_elements_.size_array();
  floor_elements_.resize_array(index + nb_elems + 1, ArrOfInt::COPY_NOINIT);
  floor_elements_[index] = nb_elems;
  for (int i = 0; i < nb_elems; i++)
    floor_elements_[index + 1 + i] = elements_list[i];
  return octree_id(index, FLOOR);
}

/*! @brief octree_center_i est le premier int de la moitie superieure de l'octree dans la direction i.
 *
 * octree_half_width est une puissance de 2 egale a octree_center_i-octree_min_i (octree_min_i
 *    est le premier int inclu dans cet octree dans la direction i)
 *  Valeur de retour: octree_id de l'octree construit (void octree_structure_)
 *
 */
int Octree_Int::build_octree_recursively(const int octree_center_x,
                                         const int octree_center_y,
                                         const int octree_center_z,
                                         const int octree_half_width,
                                         const IntTab& elements_boxes,
                                         ArrsOfInt& vect_elements_list,
                                         const int level,
                                         ArrsOfInt& tmp_elem_flags)
{
  // Criteres d'arret de la subdivision:
  // Nombre maximal d'elements dans un sous-cube floor
  static const int octree_floor_max_elems = 8;
  // S'il y a beaucoup d'elements dupliques, mais pas trop, et que le nombre d'elements
  //  dans l'octree est superieur a cette valeur, on subdivise quand-meme
  static const int octree_duplicate_elements_limit = 64;
  const ArrOfInt& elements_list = vect_elements_list[level];
  // Si le nombre d'elements est inferieur a la limite, on cree un floor_element,
  // sinon on subdivise
  const int nb_elems = elements_list.size_array();
  if (nb_elems == 0)
    return octree_id(0, EMPTY);

  if (nb_elems < octree_floor_max_elems || octree_half_width == 1 /* dernier niveau */)
    {
      const int the_octree_id = build_octree_floor(elements_list);
      return the_octree_id;
    }

  ArrOfInt& elem_flags = tmp_elem_flags[level];
  elem_flags.resize_array(nb_elems, ArrOfInt::NOCOPY_NOINIT);

  const int nb_octrees = octree_structure_.dimension(1);
  assert(nb_octrees == 2 || nb_octrees == 4 || nb_octrees == 8);
  const int elem_box_dim = elements_boxes.dimension(1);
  // Soit elements_boxes contient dimension colonnes, soit dimension*2
  const int box_delta = (elem_box_dim > 3) ? (elem_box_dim >> 1) : 0;
  // Nombre d'elements stockes en double dans l'octree (a cause des elements a cheval
  //  sur plusieurs sous-octrees)
  int nb_duplicate_elements = 0;
  // On range les elements de la liste dans 8 sous-cubes (remplissage de elem_flags)
  for (int i_elem = 0; i_elem < nb_elems; i_elem++)
    {
      const int elem = elements_list[i_elem];
      // dir_flag vaut 1 pour la direction x, 2 pour y et 4 pour z
      int dir_flag = 1;
      // sub_cube_flags contient 2^dim drapeaux binaires (1 par sous-cube),
      // et indique les sous-cubes coupes par l'element
      int octree_flags = 255;
      // dans combien de sous-octree cet element est-il stocke ?
      int nb_duplicates = 1;

      for (int direction = 0; direction < 3; direction++)
        {
          const int elem_min = elements_boxes(elem, direction);
          const int elem_max = elements_boxes(elem, box_delta+direction);
          assert(elem_max >= elem_min);
          // coordonnee du centre du cube dans la direction j:
          const int center = (direction==0) ? octree_center_x : ((direction==1) ? octree_center_y : octree_center_z);
          // L'element coupe-t-il la partie inferieure et la partie superieure du cube dans la "direction" ?
          if (elem_min >= center) // non -> on retire les flags des cubes de la partie inferieure
            octree_flags &= sub_cube_flags_max[direction];
          else if (elem_max < center) // non -> on retire les flags des cubes de la partie superieure
            octree_flags &= sub_cube_flags_min[direction];
          else
            nb_duplicates <<= 1; // l'element coupe les deux parties !
          dir_flag = dir_flag << 1;
          if (dir_flag == nb_octrees)
            break;
        }
      elem_flags[i_elem] = octree_flags;
      nb_duplicate_elements += nb_duplicates - 1;
    }

  // Critere un peu complique : s'il y a vraiment beaucoup d'elements
  //  dans cet octree, on autorise jusqu'a dupliquer tous les elements,
  //  ce qui permet de ranger des elements tres alonges qui sont forcement
  //  dupliques dans une direction (>octree_duplicate_elements_limit).
  if ((nb_duplicate_elements * 2 >= nb_elems && nb_elems < octree_duplicate_elements_limit)
      || nb_duplicate_elements > nb_elems)
    {
      const int the_octree_id = build_octree_floor(elements_list);
      // On renvoie un index d'octreefloor
      return the_octree_id;
    }

  // On reserve une case a la fin de octree_structure pour stocker cet octree:
  const int index_octree = octree_structure_.dimension(0);
  octree_structure_.resize_dim0(index_octree + 1, ArrOfInt::COPY_NOINIT);
  ArrOfInt& new_liste_elems = vect_elements_list[level+1];
  new_liste_elems.resize_array(0);
  const int width = octree_half_width >> 1;
  const int m_width = - width;
  // Traitement recursif des sous-cubes de l'octree:
  int i_cube;
  for (i_cube = 0; i_cube < nb_octrees; i_cube++)
    {
      const int octree_flag = 1 << i_cube;
      new_liste_elems.resize_array(nb_elems, ArrOfInt::NOCOPY_NOINIT);
      int count = 0;
      // Liste des elements inclus dans le sous-cube:
      for (int i_elem = 0; i_elem < nb_elems; i_elem++)
        if ((elem_flags[i_elem] & octree_flag) != 0)
          new_liste_elems[count++] = elements_list[i_elem];
      new_liste_elems.resize_array(count);

      int sub_octree_id;
      if (new_liste_elems.size_array() == 0)
        {
          sub_octree_id = octree_id(-1, EMPTY);
        }
      else
        {
          // Coordonnees du nouveau sous-cube
          const int cx = octree_center_x + ((i_cube&1) ? width : m_width);
          const int cy = octree_center_y + ((i_cube&2) ? width : m_width);
          const int cz = octree_center_z + ((i_cube&4) ? width : m_width);
          sub_octree_id = build_octree_recursively(cx, cy, cz, width,
                                                   elements_boxes,
                                                   vect_elements_list,
                                                   level+1,
                                                   tmp_elem_flags);
        }
      octree_structure_(index_octree, i_cube) = sub_octree_id;
    }

  return octree_id(index_octree, OCTREE);
}

/*! @brief renvoie l'octree_id de l'octree_floor contenant le sommet (x,y,z) (peut renvoyer l'octree EMPTY)
 *
 */
int Octree_Int::search_octree_floor(int x, int y, int z) const
{
  if (octree_type(root_octree_id_) != OCTREE)
    return root_octree_id_;
  // Le test pour savoir si on est dans la partie superieure ou
  // inferieure d'un octree au niveau i consiste simplement a tester
  // le i-ieme bit de la position.
  int flag = root_octree_half_width_;

  int index = octree_index(root_octree_id_, OCTREE);

  // Descendre dans la hierarchie d'octree subdivises jusqu'au cube
  //  le plus petit
  while (1)
    {
      // Numero du sous-cube dans lequel se trouve le sommet x,y,z
      const int ix = (x & flag) ? 1 : 0;
      const int iy = (y & flag) ? 2 : 0;
      const int iz = (z & flag) ? 4 : 0;
      int i_sous_cube = ix + iy + iz;
      // On entre dans le sous-cube :
      const int the_octree_id = octree_structure_(index, i_sous_cube);
      if (octree_type(the_octree_id) != OCTREE)
        return the_octree_id;

      index = octree_index(the_octree_id, OCTREE);
      flag >>= 1;
    }
  //return -1; // On n'arrive jamais ici !
}
