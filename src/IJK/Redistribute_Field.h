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
// File      : Redistribute_Field.h
// Directory : $IJK_ROOT/src/IJK
//
/////////////////////////////////////////////////////////////////////////////
#ifndef Redistribute_Field_included
#define Redistribute_Field_included

#include <IJK_Field.h>
#include <TRUSTTabs.h>
#include <Schema_Comm_Vecteurs.h>

// Classe outil permettant de redistribuer un champ d'un maillage ijk sur un maillage
// distribue differemment (copie maille par maille sans interpolation)
class Redistribute_Field
{
public:
  void initialize(const IJK_Splitting& input,
                  const IJK_Splitting& output,
                  const IJK_Splitting::Localisation loc);
  void initialize(const IJK_Splitting& input,
                  const IJK_Splitting& output,
                  const IJK_Splitting::Localisation loc,
                  const VECT(IntTab) & redistribute_maps);
  void redistribute(const IJK_Field_double& input_field,
                    IJK_Field_double& output_field)
  {
    redistribute_(input_field, output_field, false);
  }
  template<class T,int N>
  void redistribute(const FixedVector<T, N>& input_field,
                    FixedVector<T, N>& output_field)
  {
    for (int i = 0; i < N; i++)
      redistribute_(input_field[i], output_field[i], false);
  }
  void redistribute_add(const IJK_Field_double& input_field,
                        IJK_Field_double& output_field)
  {
    redistribute_(input_field, output_field, true);
  }
  template<class T,int N>
  void redistribute_add(const FixedVector<T, N>& input_field,
                        FixedVector<T, N>& output_field)
  {
    for (int i = 0; i < N; i++)
      redistribute_(input_field[i], output_field[i], true);
  }
  void redistribute(const IJK_Field_float& input_field,
                    IJK_Field_float& output_field)
  {
    redistribute_(input_field, output_field, false);
  }
  void redistribute_add(const IJK_Field_float& input_field,
                        IJK_Field_float& output_field)
  {
    redistribute_(input_field, output_field, true);
  }
protected:
  void redistribute_(const IJK_Field_double& input_field,
                     IJK_Field_double& output_field,
                     bool add);
  void redistribute_(const IJK_Field_float& input_field,
                     IJK_Field_float& output_field,
                     bool add);
  static void intersect(const int s1, const int n1, int& s2, int& n2, int& s3);
  void compute_send_blocs(const IJK_Splitting& input,
                          const IJK_Splitting& output,
                          const IJK_Splitting::Localisation localisation,
                          const int dir,
                          const IntTab& global_index_mapping,
                          IntTab& send_blocs);

  FixedVector<IntTab, 3> send_blocs_;
  FixedVector<IntTab, 3> recv_blocs_;
  Schema_Comm_Vecteurs schema_comm_;
  int size_buffer_for_me_;
};
#endif
