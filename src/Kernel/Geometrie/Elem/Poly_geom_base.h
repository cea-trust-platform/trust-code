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

#ifndef Poly_geom_base_included
#define Poly_geom_base_included

#include <Elem_geom_base.h>
#include <Elem_geom.h>

// Useful macros
#ifdef linux
#define BLOQUE Cerr<<__PRETTY_FUNCTION__<< " "<<__FILE__<<":"<<(int)__LINE__<<" not coded" <<finl;Process::exit()
#define WARN Cerr<<"Warning: "<<__PRETTY_FUNCTION__<< " "<<__FILE__<<":"<<(int)__LINE__<<" bad coded" <<finl
#else
#define BLOQUE Cerr<<__FILE__<<":"<<(int)__LINE__<<" not coded" <<finl;Process::exit()
#define WARN Cerr<<"Warning: "<<__FILE__<<":"<<(int)__LINE__<<" bad coded" <<finl
#endif


/*! @brief Base class for polyedrons and polygons.
 * Connectivity is stored in descending mode:
 * - a Poly_geom is made of faces (see members Polygone::PolygonIndex_ and Polyedre::PolyedreIndex_ -> TODO should merge them)
 * - each face is made of nodes (see member FaceIndex_ below)
 */
template <typename _SIZE_>
class Poly_geom_base_32_64 : public Elem_geom_base_32_64<_SIZE_>
{
  Declare_base_32_64( Poly_geom_base_32_64 ) ;

public :
  using int_t = _SIZE_;
  using ArrOfInt_t = ArrOfInt_T<_SIZE_>;

  int get_tab_faces_sommets_locaux(IntTab& faces_som_local) const override =0;
  const ArrOfInt_t& getFacesIndex() const { return FacesIndex_; }

  inline int get_nb_face_elem_max() const { return nb_face_elem_max_ ; }
  int get_nb_som_elem_max() const;
  inline bool est_structure() const override { return false; }

  int nb_type_face() const override { return 1; }

  virtual int get_tab_faces_sommets_locaux(IntTab& faces_som_local,int_t elem) const=0;
  virtual const ArrOfInt_t& getElemIndex() const =0;
  virtual int_t get_somme_nb_faces_elem() const =0;
  virtual void calculer_un_centre_gravite(const int_t elem, DoubleVect& xp) const =0;
  virtual void build_reduced(Elem_geom_32_64<_SIZE_>& type_elem, const ArrOfInt_t& elems_sous_part) const = 0;
  virtual void compute_virtual_index()=0;

protected:
  /*! Description of the faces (resp. edges) of the polyhedron (resp. polygon)
   * FacesIndex_[i:i+s] contains the list of nodes forming a face (resp. edge)
   * Boundaries are given by Polyedre::PolyhedronIndex_ (resp. Polygon::PolygonIndex_)
   */
  ArrOfInt_t FacesIndex_;

  int nb_som_elem_max_;
  int nb_face_elem_max_;
};

using Poly_geom_base = Poly_geom_base_32_64<int>;
using Poly_geom_base_64 = Poly_geom_base_32_64<trustIdType>;


#endif /* Poly_geom_base_included */
