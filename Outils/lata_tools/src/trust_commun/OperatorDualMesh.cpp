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

#include <LataFilter.h>
#include <Operator.h>

// Journal level
#define verb_level 4

void build_geometry_(OperatorDualMesh& op, const DomainUnstructured& src, LataDeriv<Domain>& dest_domain)
{
  Journal(verb_level) << "OperatorDualMesh geometry(unstructured) " << src.id_.name_ << endl;
  if (!src.faces_ok())
    {
      Journal() << "Error in OperatorDualMesh::build_geometry: source domain has no faces data" << endl;
      throw;
    }
  const trustIdType nb_som = src.nodes_.dimension(0), nb_elem = src.elem_faces_.dimension(0), nb_faces = src.faces_.dimension(0); // Not elements_, in case elem_faces_ has no virtual data.
  const trustIdType dim = src.dimension();

  DomainUnstructured& dest = dest_domain.instancie(DomainUnstructured);
  dest.id_ = src.id_;
  dest.id_.name_ += "_dual";
  dest.elt_type_ = dim < 3 ? Domain::polygone : Domain::polyedre;

  dest.nodes_ = src.nodes_;
  dest.nodes_.resize(nb_som + nb_elem, int(dim));
  src.compute_cell_center_coordinates(dest.nodes_, nb_som);

  dest.elements_.resize(nb_faces, int(src.faces_.dimension(1)) + 2);
  dest.elem_faces_.resize(nb_faces, 2 * int(src.faces_.dimension(1)));
  dest.faces_.resize(0, int(src.faces_.dimension(1)));

  for (trustIdType f = 0, s, e; f < nb_faces; f++)
    {
      /* dest.elements_ : les sommets de la face + amont/aval */ 
      if (dim < 3) /* 2D : un sommet de la face -> CG elem amont -> l'autre sommet -> CG elem aval */
        {
          int k = 0;
          for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
              if (j ? (s = src.faces_(f, i)) >= 0 : (e = src.face_voisins_(f, i)) >= 0 && e < nb_elem)
                dest.elements_(f, k) = j ? s : nb_som + e, k++;
          for (; k < dest.elements_.dimension(1); k++) dest.elements_(f, k) = -1;
        }
      else
        {
          int i;
          for (i = 0; i < src.faces_.dimension(1) && (s = src.faces_(f, i)) >= 0; i++)
            dest.elements_(f, i) = s;
          for (int j = 0; j < 2; j++)
            if ((e = src.face_voisins_(f, j)) >= 0 && e < nb_elem)
              dest.elements_(f, i) = nb_som + e, i++;
          for ( ; i < dest.elements_.dimension(1); i++)
            dest.elements_(f, i) = -1;
        }
      /* dest.elem_faces_ et dest_faces_ : une face par arete et par cote des faces orginales */
      int j = 0;
      for (int i = 0; i < 2; i++)
        if ((e = src.face_voisins_(f, i)) >= 0 && e < nb_elem) /* un voisin reel -> boucles sur les aretes */
          for (int k = 0; k < src.faces_.dimension(1) && (s = src.faces_(f, k)) >= 0; k++)
            {
              trustIdType sb = dim < 3 ? -1 : src.faces_(f, k + 1 < src.faces_.dimension(1) && src.faces_(f, k + 1) >= 0 ? k + 1 : 0);
              trustIdType l = dest.faces_.dimension(0);
              dest.faces_.resize(l + 1, int(src.faces_.dimension(1)));
              dest.elem_faces_(f, j) = l, j++;
              for (int m = 0; m < src.faces_.dimension(1); m++)
                dest.faces_(l, m) = m < 1 ? nb_som + e : m < 2 ? s : m < dim ? sb : -1;
            }
        else
        {
          trustIdType l = dest.faces_.dimension(0);
          dest.faces_.resize(l + 1, int(src.faces_.dimension(1)));
          dest.elem_faces_(f, j) = l;
          j++;
          for (int k = 0; k < src.faces_.dimension(1); k++) /* sinon -> on insere la face originale */
            dest.faces_(l, k) = src.faces_(f, k);
        }
      for (; j < dest.elem_faces_.dimension(1); j++) dest.elem_faces_(f, j) = -1;
    }
  dest.set_nb_virt_items(LataField_base::ELEM, src.nb_virt_items(LataField_base::FACES));
}

// Builds a field on the dual domain from the field on the source domain.
// Source field must be located at faces.
// (destination field is located at the elements. the value for an element
//  is the value associated to the adjacent face of the source domain).
template<class TabType>
void build_field_(OperatorDualMesh& op, const DomainUnstructured& src_domain, const DomainUnstructured& dest_domain, const Field<TabType>& src, Field<TabType>& dest)
{
  Journal(verb_level) << "OperatorDualMesh field(unstructured) " << src.id_.uname_ << endl;
  dest.component_names_ = src.component_names_;
  dest.localisation_ = LataField_base::ELEM;
  dest.nature_ = src.nature_;
  dest.data_ = src.data_;
}

void build_geometry_(OperatorDualMesh& op, const DomainIJK& src, LataDeriv<Domain>& dest_domain)
{
  Journal(verb_level) << "OperatorDualMesh geometry(ijk) " << src.id_.name_ << endl;
  if (src.elt_type_ != Domain::quadri && src.elt_type_ != Domain::hexa)
    {
      Journal() << "Error in OperatorDualMesh::build_geometry: cannot operate on unstructured mesh with this element type" << endl;
      throw;
    }

  DomainIJK& dest = dest_domain.instancie(DomainIJK);
  dest.elt_type_ = src.elt_type_;
  const int dim = src.dimension();
  for (int i_dim = 0; i_dim < dim; i_dim++)
    {
      const ArrOfFloat& c1 = src.coord_[i_dim];
      ArrOfFloat& c2 = dest.coord_.add(ArrOfFloat());
      const int n = c1.size_array() - 1;
      c2.resize_array(n * 2 + 1);
      for (int i = 0; i < n; i++)
        {
          c2[i * 2] = c1[i];
          c2[i * 2 + 1] = (c1[i] + c1[i + 1]) * 0.5f;
        }
      c2[n * 2] = c1[n];
    }

  if (src.invalid_connections_.size_array() > 0)
    {
      dest.invalid_connections_.resize_array(dest.nb_elements());
      dest.invalid_connections_ = 0;
      int index = 0;

      const int ni = dest.coord_[0].size_array() - 1;
      const int nj = dest.coord_[1].size_array() - 1;
      const int nk = (dim == 3) ? (dest.coord_[2].size_array() - 1) : 1;
      const int ni_src = src.coord_[0].size_array() - 1;
      const int nj_src = src.coord_[1].size_array() - 1;
      for (int k = 0; k < nk; k++)
        {
          const int k_src = k / 2;
          for (int j = 0; j < nj; j++)
            {
              const int j_src = j / 2;
              const trustIdType idx_source = ((trustIdType)k_src * (trustIdType)nj_src + (trustIdType)j_src) * (trustIdType)ni_src;
              for (int i = 0; i < ni; i++)
                {
                  const trustIdType idx = idx_source + i / 2;
                  if (src.invalid_connections_[idx])
                    dest.invalid_connections_.setbit(index);
                  index++;
                }
            }
        }
    }
  dest.virtual_layer_begin_ = 2 * src.virtual_layer_begin_;
  dest.virtual_layer_end_ = 2 * src.virtual_layer_end_;
}

#define IJK(i,j,k) ((trustIdType)k*nj_ni_src + (trustIdType)j*(trustIdType)ni_src + (trustIdType)i)

template<class TabType>
void build_field_(OperatorDualMesh& op, const DomainIJK& src_domain, const DomainIJK& dest_domain, const Field<TabType>& src, Field<TabType>& dest)
{
  Journal(verb_level) << "OperatorDualMesh field(ijk) " << src.id_.uname_ << endl;
  dest.component_names_ = src.component_names_;
  dest.localisation_ = LataField_base::ELEM;
  dest.nature_ = LataDBField::VECTOR;
  const int dim = src_domain.dimension();
  trustIdType index = 0;

  // Loop on destination elements
  const int ni = dest_domain.coord_[0].size_array() - 1;
  const int nj = dest_domain.coord_[1].size_array() - 1;
  const int nk = (dim == 3) ? (dest_domain.coord_[2].size_array() - 1) : 1;
  dest.data_.resize(ni * nj * nk, dim);
  const int ni_src = src_domain.coord_[0].size_array();
  const trustIdType nj_ni_src = (trustIdType)src_domain.coord_[1].size_array() * (trustIdType)ni_src;
  for (int k = 0; k < nk; k++)
    {
      const int k2 = k / 2;
      const int k3 = (k + 1) / 2;
      for (int j = 0; j < nj; j++)
        {
          const int j2 = j / 2;
          const int j3 = (j + 1) / 2;
          for (int i = 0; i < ni; i++)
            {
              const int i2 = i / 2;
              const int i3 = (i + 1) / 2;
              dest.data_(index, 0) = src.data_(IJK(i3, j2, k2), 0);
              dest.data_(index, 1) = src.data_(IJK(i2, j3, k2), 1);
              if (dim == 3)
                dest.data_(index, 2) = src.data_(IJK(i2, j2, k3), 2);
              index++;
            }
        }
    }
}

#undef IJK

void OperatorDualMesh::build_geometry(const Domain& src_domain, LataDeriv<Domain>& dest)
{
  apply_geometry(*this, src_domain, dest);
}

void OperatorDualMesh::build_field(const Domain& src_domain, const LataField_base& src_field, const Domain& dest_domain, LataDeriv<LataField_base>& dest)
{
  if (src_field.localisation_ != LataField_base::FACES)
    {
      Journal() << "Error in OperatorDualMesh::build_field: source field is not located at faces" << endl;
      throw;
    }
  apply_field(*this, src_domain, src_field, dest_domain, dest);
}
#undef level
