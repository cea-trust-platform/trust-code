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

#include <MD_Vector_composite.h>
#include <MD_Vector_seq.h>
#include <Array_tools.h>
#include <Param.h>

Implemente_instanciable(MD_Vector_composite,"MD_Vector_composite",MD_Vector_base);

/*! @brief method used to dump/restore a descriptor in a file Each process writes a different descriptor.
 *
 * See MD_Vector_tools::restore_vector_with_md()
 */
Entree& MD_Vector_composite::readOn(Entree& is)
{
  MD_Vector_base::readOn(is);

  int np;
  is >> np ;
  for (int p=0; p<np; p++)
    {
      OWN_PTR(MD_Vector_base) md_ptr;
      Nom md_type;
      is >> md_type;
      md_ptr.typer(md_type);
      is >> md_ptr.valeur();
      if (1)
        {
          // Creation du MD_Vector attache au tableau
          data_.add(MD_Vector());
          MD_Vector& mdV =data_[p];
          mdV.copy(md_ptr.valeur());
        }
    }
  Param p(que_suis_je());
  p.ajouter("is_seq", &is_seq_);
  p.ajouter_non_std("global_md", this);
  p.ajouter("parts_offsets", &parts_offsets_);
  p.ajouter("shapes", &shapes_);
  p.ajouter("names", &names_);
  p.lire_avec_accolades(is);
  return is;
}

int MD_Vector_composite::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "global_md")
    {
      assert(is_seq_ != -1);
      is_seq_ ? instanciate_seq() : instanciate_std();
      is >> *global_md_;
      return 1;
    }
  return -1;
}

/*! @brief method used to dump/restore a descriptor in a file Each process writes a different descriptor.
 *
 * See MD_Vector_tools::dump_vector_with_md()
 */
Sortie& MD_Vector_composite::printOn(Sortie& os) const
{
  MD_Vector_base::printOn(os);
  int np=data_.size();
  os << np<<finl;
  for (int p=0; p<np; p++)
    {
      const MD_Vector_base& md = get_desc_part(p).valeur();
      os << md.que_suis_je() << finl;
      os << md << finl;
    }

  os << "{" << finl;
  os << "is_seq" << tspace << is_seq_ << finl;
  os << "global_md" << finl << *global_md_ << finl;
  os << "parts_offsets" << tspace << parts_offsets_ << finl;
  os << "shapes" << tspace << shapes_ << finl;
  os << "names" << tspace << names_ << finl;
  os << "}" << finl;
  return os;
}

// [ABN] TODO the 2 methods below could be made more OO by using Objet_U::duplique() or sth like that ... can't be bothered ...
void MD_Vector_composite::instanciate_std()
{
  if(!global_md_)
    {
      global_md_ = std::make_shared<MD_Vector_std>();
      is_seq_ = 0;
    }
}

void MD_Vector_composite::instanciate_seq()
{
  if(!global_md_)
    {
      // important: initialize with a number of 0 items - will be incremented by add_part() below:
      global_md_ = std::make_shared<MD_Vector_seq>(0);
      is_seq_ = 1;
    }
}


/*! @brief Append the "part" descriptor to the composite vector.
 *
 * By default, shape=0 says that for each item in "part", one item will
 *    be appended to the composite descriptor.
 *    If shape=n, each items in "part" will be duplicated n times in the
 *    composite descriptor.
 *    The difference between 0 and 1 is when we create parts with DoubleTab_parts
 *    (see class DoubleTab_parts)
 *   Exemples:
 *    1) P1Bubble pressure fields have one, two or three parts with shape=1.
 *        (see Domaine_VEF::discretiser_suite())
 *    2) One can create a composite field containing
 *     - velocity degrees of freedom at the faces
 *     - pressure d.o.f. at the elements
 *     - one multiscalar d.o.f at nodes
 *         MD_Vector_composite mdc;
 *         mdc.add_part(domainevf.md_vector_faces(), dimension);
 *         mdc.add_part(domaine.md_vector_elements(), 0 // scalar //);
 *         mdc.add_part(domaine.md_vector_sommets(), 1 // multiscalar with 1 component //);
 *         MD_Vector md;
 *         md.copy(mdc);
 *     I will create a DoubleVect of this form by attaching this descriptor:
 *               DoubleVect v;
 *          MD_Vector_tools::creer_tableau_distribue(md, v);
 *        DoubleTab_Parts parts(v);
 *        // parts[0] has nb_dim()==2, dimensions are [ domainevf.nb_faces_tot, 3 ]
 *        // parts[1] has nb_dim()==1, dimension [ domaine.nb_elem_tot ] // speciel case shape==0 //
 *        // parts[2] has nb_dim()==2, dimension [ domaine.nb_som_tot, 1 ]
 *     I can duplicate items in the descriptor by creating DoubleTab objects:
 *     The following code will create an array containing 12 d.o.f for each face
 *     followed by 4 d.o.f for each element and 4 d.o.f for each node:
 *        DoubleTab tab(0, 4);
 *        MD_Vector_tools::creer_tableau_distribue(md, tab);
 *        DoubleTab_Parts parts(tab);
 *        // parts[0] has nb_dim()==3, dimensions are [ domainevf.nb_faces_tot, 3, 4 ]
 *        // parts[1] has nb_dim()==2, dimension [ domaine.nb_elem_tot, 4 ] // speciel case shape==0 //
 *        // parts[2] has nb_dim()==3, dimension [ domaine.nb_som_tot, 1, 4 ]
 *
 */
void MD_Vector_composite::add_part(const MD_Vector& part, int shape, Nom name)
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

  parts_offsets_.append_array(offset);

  shapes_.append_array(shape);
  names_.add(name);

  nb_items_tot_ += part->get_nb_items_tot() * multiplier;
  if (data_.size() > 1 || part->get_nb_items_reels() < 0)
    nb_items_reels_ = -1; // size() will be invalid for arrays with this descriptor
  else
    nb_items_reels_ = part->get_nb_items_reels() * multiplier;
  nb_items_seq_tot_ += part->nb_items_seq_tot() * multiplier;
  nb_items_seq_local_ += part->nb_items_seq_local() * multiplier;

  // Double dynamic dispatch here:
  //  - global_md_ can be either MD_Vector_std or MD_Vector_seq
  //  - and part.valeur() can be any child of MD_Vector_base
  // First dispatch is made with C++ polymorphism ... and for the second, no choice, we must sub_type:
  if (sub_type(MD_Vector_std, part.valeur()))
    {
      assert(Process::is_parallel());
      instanciate_std();
      global_md_->append_from_other_std(ref_cast(MD_Vector_std, part.valeur()), offset, multiplier);
    }
  else if (sub_type(MD_Vector_seq, part.valeur()))
    {
      assert(Process::is_sequential());
      instanciate_seq();
      global_md_->append_from_other_seq(ref_cast(MD_Vector_seq, part.valeur()), offset, multiplier);
    }
  else if (sub_type(MD_Vector_composite, part.valeur()))
    {
      const MD_Vector_mono& oth = * ( ref_cast(MD_Vector_composite, part.valeur()).global_md_ );
      if (sub_type(MD_Vector_std, oth))
        {
          instanciate_std();
          global_md_->append_from_other_std(ref_cast(MD_Vector_std, oth), offset, multiplier);
        }
      else if (sub_type(MD_Vector_seq, oth))
        {
          instanciate_seq();
          global_md_->append_from_other_seq(ref_cast(MD_Vector_seq, oth), offset, multiplier);
        }
      else
        Process::exit("Internal error in MD_Vector_composite::add_part: should not happen. ");
    }
  else
    {
      Cerr << "Internal error in MD_Vector_composite::add_part: unknown part type " << part->que_suis_je() << finl;
      Process::exit();
    }

  // Update the block information - in sequential (MD_Vector_seq) nothing will be changed (since underlying arrays
  // remain empty), but this is OK because all algorithsm using this data (determine_blocks() in TRUSTVect_tools for
  // example, or get_sequential_items_flags()) are overriden for sequential cases.
  append_blocs(global_md_->blocs_items_to_sum_, part->get_items_to_sum(), offset, multiplier);
  append_blocs(global_md_->blocs_items_to_compute_, part->get_items_to_compute(), offset, multiplier);
}

void MD_Vector_composite::fill_md_vect_renum(const IntVect& renum, MD_Vector& md_vect) const
{
  global_md_->fill_md_vect_renum(renum, md_vect);
}

/*! Override, to avoid using the standard implementation of the method that scans the blocks 'bloc_items_to_sum_'
 * in case of a sequential situation.
 */
int MD_Vector_composite::get_seq_flags_impl(ArrOfBit& flags, int line_size) const
{
  assert(data_.size() > 0);
  assert(is_seq_ != -1);
  if (is_seq_)
    {
      assert(dynamic_cast<MD_Vector_seq *>(global_md_.get()) != nullptr);
      return global_md_->get_seq_flags_impl(flags, line_size);
    }
  else  // bloc_* members are up-to-date, we can use the base implementation:
    {
      assert(dynamic_cast<MD_Vector_std *>(global_md_.get()) != nullptr);
      return MD_Vector_base::get_seq_flags_impl(flags, line_size);
    }
}

