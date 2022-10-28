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
/////////////////////////////////////////////////////////////////////////////
//
// File      : IJK_discretization.h
// Directory : $IJK_ROOT/src/IJK/OpVDF
//
/////////////////////////////////////////////////////////////////////////////
#ifndef IJK_discretization_included
#define IJK_discretization_included
#include <IJK_Splitting.h>
#include <VDF_to_IJK.h>
#include <Objet_U.h>

// This class holds an IJK split mesh converted from a vdf source mesh and splitting
class IJK_discretization : public Objet_U
{
  Declare_instanciable(IJK_discretization);
public:
  const IJK_Splitting& get_IJK_splitting() const;
  const VDF_to_IJK& get_vdf_to_ijk(IJK_Splitting::Localisation) const;
  void nommer(const Nom& n) override
  {
    object_name_ = n;
  }
  const Nom& le_nom() const override
  {
    return object_name_;
  }
  static const char *get_conventional_name();
protected:
  Nom object_name_;

  IJK_Splitting splitting_;

  // For faces data:
  VDF_to_IJK vdf_to_ijk_i_;
  VDF_to_IJK vdf_to_ijk_j_;
  VDF_to_IJK vdf_to_ijk_k_;
  // For cell centered data:
  VDF_to_IJK vdf_to_ijk_elem_;
  // For nodes data
  VDF_to_IJK vdf_to_ijk_nodes_;
};
#endif
