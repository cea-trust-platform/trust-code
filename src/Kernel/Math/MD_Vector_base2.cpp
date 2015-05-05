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
// File:        MD_Vector_base2.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/1
//
//////////////////////////////////////////////////////////////////////////////

#include <MD_Vector_base2.h>
#include <Param.h>

Implemente_base_sans_constructeur(MD_Vector_base2,"MD_Vector_base2",MD_Vector_base);

MD_Vector_base2::MD_Vector_base2()
{
  nb_items_tot_ = -1;
  nb_items_reels_ = -1;
  nb_items_seq_tot_ = -1;
  nb_items_seq_local_ = -1;
}

// Description: method used to dump/restore a descriptor in a file
//  Each process writes a different descriptor. See MD_Vector_tools::restore_vector_with_md()
Entree& MD_Vector_base2::readOn(Entree& is)
{
  Param p("MD_Vector_base2");
  p.ajouter("nb_items_tot", &nb_items_tot_);
  p.ajouter("nb_items_reels", &nb_items_reels_);
  p.ajouter("nb_items_seq_tot", &nb_items_seq_tot_);
  p.ajouter("nb_items_seq_local", &nb_items_seq_local_);
  p.ajouter("blocs_items_to_sum", &blocs_items_to_sum_);
  p.ajouter("blocs_items_to_compute", &blocs_items_to_compute_);
  p.lire_avec_accolades(is);
  return is;
}

// Description: method used to dump/restore a descriptor in a file
//  Each process writes a different descriptor. See MD_Vector_tools::dump_vector_with_md()
Sortie& MD_Vector_base2::printOn(Sortie& os) const
{
  os << "{" << finl;
  os << "nb_items_tot" << space << nb_items_tot_ << finl;
  os << "nb_items_reels" << space << nb_items_reels_ << finl;
  os << "nb_items_seq_tot" << space << nb_items_seq_tot_ << finl;
  os << "nb_items_seq_local" << space << nb_items_seq_local_ << finl;
  os << "blocs_items_to_sum" << space << blocs_items_to_sum_;
  os << "blocs_items_to_compute" << space << blocs_items_to_compute_;
  os << "}" << finl;
  return os;
}
