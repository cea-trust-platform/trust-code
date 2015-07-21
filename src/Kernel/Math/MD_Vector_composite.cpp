/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        MD_Vector_composite.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////
#include <MD_Vector_composite.h>
#include <Array_tools.h>
#include <Param.h>

implemente_vect_no_obj(MD_Vector)

Implemente_instanciable(MD_Vector_composite,"MD_Vector_composite",MD_Vector_base2);

#if 0
// .DESCRIPTION
//  Metadata for a distributed vector built as a subset of another vector
class MD_Vector_subset : public MD_Vector_std
{
  eeclare_instanciable(MD_Vector_subset)
public:
  virtual void build(const ArrOfBit& selection, const MD_Vector_Ptr& src);
  virtual void copy_vector_to_subset(const DoubleVect& src, DoubleVect& dest) const;
  virtual void copy_subset_to_vector(const DoubleVect& src, DoubleVect& dest) const;
  // The metadata of the source vector
  MD_Vector_Ptr src_data_;
  // The subset selection (size = src_data_.valeur().get_nb_items_tot())
  ArrOfBit selection_;
};
#endif

// Description: method used to dump/restore a descriptor in a file
//  Each process writes a different descriptor. See MD_Vector_tools::dump_vector_with_md()
Sortie& MD_Vector_composite::printOn(Sortie& os) const
{
  MD_Vector_base2::printOn(os);
  os << "{" << finl;
  os << "global_md" << finl << global_md_ << finl;
  os << "parts_offsets" << space << parts_offsets_ << finl;
  os << "shapes" << space << shapes_ << finl;
  os << "}" << finl;
  return os;
}

// Description: method used to dump/restore a descriptor in a file
//  Each process writes a different descriptor. See MD_Vector_tools::restore_vector_with_md()
Entree& MD_Vector_composite::readOn(Entree& is)
{
  MD_Vector_base2::readOn(is);
  Param p(que_suis_je());
  p.ajouter("global_md", &global_md_);
  p.ajouter("parts_offsets", &parts_offsets_);
  p.ajouter("shapes", &shapes_);
  p.lire_avec_accolades(is);
  return is;
}

static void append_items(ArrOfInt& dest, const ArrOfInt& src, int offset = 0, int multiplier = 1)
{
  int i = dest.size_array();
  const int n = src.size_array();
  dest.resize_array(i + n * multiplier);
  for (int j = 0; j < n; j++)
    {
      int x = offset + src[j] * multiplier;
      for (int k = 0; k < multiplier; k++)
        dest[i++] = x++;
    }
}

static void append_blocs(ArrOfInt& dest, const ArrOfInt& src, int offset = 0, int multiplier = 1)
{
  int i = dest.size_array();
  const int n = src.size_array();
  dest.resize_array(i + n);
  for (int j = 0; j < n; j++, i++)
    dest[i] = offset + src[j] * multiplier;
}

// Description: returns i such that a[i]==x, or -1 if x is not found
static int find_in_array(const ArrOfInt& a, int x)
{
  int n;
  for (n = a.size_array() - 1; n >= 0; n--)
    if (a[n] == x)
      break;
  return n;
}

// Append all items in "src" to the "dest" descriptor, duplicating src items "multiplier" times
//  and adding the "offset" to item indexes. Also merge pe_voisins_ lists.
static void append_global_md(MD_Vector_std& dest, const MD_Vector_std& src, int offset, int multiplier)
{
  // List sizes for items_to_send_, items_to_recv_ and blocs_to_recv_ of new descriptor
  ArrOfInt x_sz, y_sz, z_sz;
  // Data of these lists:
  ArrOfInt x_data, y_data, z_data;
  ArrOfInt new_blocs_items_count, new_nb_items_to_items;
  x_sz.set_smart_resize(1);
  y_sz.set_smart_resize(1);
  z_sz.set_smart_resize(1);
  x_data.set_smart_resize(1);
  y_data.set_smart_resize(1);
  z_data.set_smart_resize(1);
  new_blocs_items_count.set_smart_resize(1);
  new_nb_items_to_items.set_smart_resize(1);

  ArrOfInt tmp;
  tmp.set_smart_resize(1);

  ArrOfInt pe_list(dest.pe_voisins_);
  pe_list.set_smart_resize(1);
  append_blocs(pe_list, src.pe_voisins_);
  array_trier_retirer_doublons(pe_list);
  const int np = pe_list.size_array();
  for (int i = 0; i < np; i++)
    {
      // Number of items to recv, send and blocs to recv for this processor:
      int nx = 0, ny = 0, nz = 0;
      int blocs_count = 0;
      // Is processor pe_list[i] in initial "dest" descriptor ?
      int j;

      for (j = dest.pe_voisins_.size_array()-1; j >= 0; j--)
        if (dest.pe_voisins_[j] == pe_list[i])
          break;
      const int pe = pe_list[i];

      // Insert items to send and items to recv from dest
      j = find_in_array(dest.pe_voisins_, pe);
      if (j >= 0)
        {
          const int count_items_single = dest.nb_items_to_items_[j];
          nx += count_items_single;
          // take only single items to send
          tmp.resize_array(0);
          for (int k = 0; k < count_items_single; k++)
            tmp.append_array(dest.items_to_send_(j, k));
          append_blocs(x_data, tmp);

          ny += dest.items_to_recv_.get_list_size(j);
          dest.items_to_recv_.copy_list_to_array(j, tmp);
          append_blocs(y_data, tmp);
        }

      // Insert items to send and items to recv from src, with multiplier
      j = find_in_array(src.pe_voisins_, pe);
      if (j >= 0)
        {
          const int count_items_single = src.nb_items_to_items_[j];
          nx += count_items_single * multiplier;
          // take only single items to send
          tmp.resize_array(0);
          for (int k = 0; k < count_items_single; k++)
            tmp.append_array(src.items_to_send_(j, k));
          append_items(x_data, tmp, offset, multiplier);

          ny += src.items_to_recv_.get_list_size(j) * multiplier;
          src.items_to_recv_.copy_list_to_array(j, tmp);
          append_items(y_data, tmp, offset, multiplier);
        }
      const int single_items_count = nx;
      // Insert blocs to send and blocs to recv from dest
      j = find_in_array(dest.pe_voisins_, pe);
      if (j >= 0)
        {
          const int count_items_tot = dest.items_to_send_.get_list_size(j);
          const int count_items_single = dest.nb_items_to_items_[j];
          nx += count_items_tot - count_items_single;
          tmp.resize_array(0);
          for (int k = count_items_single; k < count_items_tot; k++)
            tmp.append_array(dest.items_to_send_(j, k));
          append_blocs(x_data, tmp);

          nz += dest.blocs_to_recv_.get_list_size(j);
          dest.blocs_to_recv_.copy_list_to_array(j, tmp);
          append_blocs(z_data, tmp);

          blocs_count += dest.blocs_items_count_[j];
        }
      // Insert blocs to send and blocs to recv from src, with multiplier
      j = find_in_array(src.pe_voisins_, pe);
      if (j >= 0)
        {
          const int count_items_tot = src.items_to_send_.get_list_size(j);
          const int count_items_single = src.nb_items_to_items_[j];
          nx += (count_items_tot - count_items_single) * multiplier;
          tmp.resize_array(0);
          for (int k = count_items_single; k < count_items_tot; k++)
            tmp.append_array(src.items_to_send_(j, k));
          append_items(x_data, tmp, offset, multiplier);

          nz += src.blocs_to_recv_.get_list_size(j);
          src.blocs_to_recv_.copy_list_to_array(j, tmp);
          append_blocs(z_data, tmp, offset, multiplier);

          blocs_count += src.blocs_items_count_[j] * multiplier;
        }

      x_sz.append_array(nx);
      y_sz.append_array(ny);
      z_sz.append_array(nz);
      new_blocs_items_count.append_array(blocs_count);
      new_nb_items_to_items.append_array(single_items_count);
    }

  dest.items_to_send_.set_list_sizes(x_sz);
  dest.items_to_send_.set_data(x_data);
  dest.items_to_recv_.set_list_sizes(y_sz);
  dest.items_to_recv_.set_data(y_data);
  dest.blocs_to_recv_.set_list_sizes(z_sz);
  dest.blocs_to_recv_.set_data(z_data);
  dest.pe_voisins_ = pe_list;
  dest.blocs_items_count_ = new_blocs_items_count;
  dest.nb_items_to_items_ = new_nb_items_to_items;
}

// Description:
//   Append the "part" descriptor to the composite vector.
//   By default, shape=0 says that for each item in "part", one item will
//   be appended to the composite descriptor.
//   If shape=n, each items in "part" will be duplicated n times in the
//   composite descriptor.
//   The difference between 0 and 1 is when we create parts with DoubleTab_parts
//   (see class DoubleTab_parts)
//  Exemples:
//   1) P1Bubble pressure fields have one, two or three parts with shape=1.
//       (see Zone_VEF_PreP1b::discretiser_suite())
//   2) One can create a composite field containing
//    - velocity degrees of freedom at the faces
//    - pressure d.o.f. at the elements
//    - one multiscalar d.o.f at nodes
//        MD_Vector_composite mdc;
//        mdc.add_part(zonevf.md_vector_faces(), dimension);
//        mdc.add_part(zone.md_vector_elements(), 0 /* scalar */);
//        mdc.add_part(domaine.md_vector_sommets(), 1 /* multiscalar with 1 component */);
//        MD_Vector md;
//        md.copy(mdc);
//    I will create a DoubleVect of this form by attaching this descriptor:
//              DoubleVect v;
//         MD_Vector_tools::creer_tableau_distribue(md, v);
//       DoubleTab_Parts parts(v);
//       // parts[0] has nb_dim()==2, dimensions are [ zonevf.nb_faces_tot, 3 ]
//       // parts[1] has nb_dim()==1, dimension [ zone.nb_elem_tot ] /* speciel case shape==0 */
//       // parts[2] has nb_dim()==2, dimension [ domaine.nb_som_tot, 1 ]
//    I can duplicate items in the descriptor by creating DoubleTab objects:
//    The following code will create an array containing 12 d.o.f for each face
//    followed by 4 d.o.f for each element and 4 d.o.f for each node:
//       DoubleTab tab(0, 4);
//       MD_Vector_tools::creer_tableau_distribue(md, tab);
//       DoubleTab_Parts parts(tab);
//       // parts[0] has nb_dim()==3, dimensions are [ zonevf.nb_faces_tot, 3, 4 ]
//       // parts[1] has nb_dim()==2, dimension [ zone.nb_elem_tot, 4 ] /* speciel case shape==0 */
//       // parts[2] has nb_dim()==3, dimension [ domaine.nb_som_tot, 1, 4 ]
void MD_Vector_composite::add_part(const MD_Vector& part, int shape)
{
  assert(part.non_nul());
  assert(shape >= 0);
  if (data_.size() == 0)
    {
      nb_items_tot_ = 0;
      nb_items_seq_tot_ = 0;
      nb_items_seq_local_ = 0;
    }
  const int multiplier = (shape == 0) ? 1 : shape;
  data_.add(part);
  const int offset = nb_items_tot_;
  parts_offsets_.set_smart_resize(1);
  parts_offsets_.append_array(offset);
  shapes_.set_smart_resize(1);
  shapes_.append_array(shape);

  nb_items_tot_ += part.valeur().get_nb_items_tot() * multiplier;
  if (data_.size() > 1 || part.valeur().get_nb_items_reels() < 0)
    nb_items_reels_ = -1; // size() will be invalid for arrays with this descriptor
  else
    nb_items_reels_ = part.valeur().get_nb_items_reels() * multiplier;
  nb_items_seq_tot_ += part.valeur().nb_items_seq_tot() * multiplier;
  nb_items_seq_local_ += part.valeur().nb_items_seq_local() * multiplier;

  append_blocs(blocs_items_to_sum_, part.valeur().get_items_to_sum(), offset, multiplier);
  append_blocs(blocs_items_to_compute_, part.valeur().get_items_to_compute(), offset, multiplier);

  if (sub_type(MD_Vector_std, part.valeur()))
    append_global_md(global_md_, ref_cast(MD_Vector_std, part.valeur()), offset, multiplier);
  else if (sub_type(MD_Vector_composite, part.valeur()))
    {
      append_global_md(global_md_, ref_cast(MD_Vector_composite, part.valeur()).global_md_, offset, multiplier);
    }
  else
    {
      Cerr << "Internal error in MD_Vector_composite::add_part: unknown part type " << part.valeur().que_suis_je() << finl;
      exit();
    }
}

void MD_Vector_composite::initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& comm, DoubleVect& v) const
{
  global_md_.initialize_comm(opt, comm, v);
}

void MD_Vector_composite::prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& comm, DoubleVect& v) const
{
  global_md_.prepare_send_data(opt, comm, v);
}

void MD_Vector_composite::process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& comm, DoubleVect& v) const
{
  global_md_.process_recv_data(opt, comm, v);
}


void MD_Vector_composite::initialize_comm(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& comm, IntVect& v) const
{
  global_md_.initialize_comm(opt, comm, v);
}

void MD_Vector_composite::prepare_send_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& comm, IntVect& v) const
{
  global_md_.prepare_send_data(opt, comm, v);
}

void MD_Vector_composite::process_recv_data(const Echange_EV_Options& opt, Schema_Comm_Vecteurs& comm, IntVect& v) const
{
  global_md_.process_recv_data(opt, comm, v);
}

