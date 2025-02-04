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

#ifndef Equation_IBM_proto_included
#define Equation_IBM_proto_included

#include <TRUST_Ref.h>
#include <TRUSTTab.h>

class Equation_base;
class Matrice_Morse;
class Entree;
class Param;

class Equation_IBM_proto
{
public:

  void assembler_proto( Matrice_Morse& mat_morse, const DoubleTab& present, DoubleTab& secmem)  ;


  void set_param_IBM(Param& param);
  Entree& readOn_IBM(Entree& is, Equation_base& eq);
  bool initTimeStep_IBM(double ddt);
  DoubleTab& derivee_en_temps_inco_IBM(DoubleTab& );
  void preparer_calcul_IBM();
  void modify_initial_variable_IBM( DoubleTab& );

  inline const int& get_i_source_pdf() const { return i_source_pdf_; }
  inline const DoubleTab& get_champ_coeff_pdf_som() const { return champ_coeff_pdf_som_; }
  inline void set_champ_coeff_pdf_som(DoubleTab& coeff) { champ_coeff_pdf_som_ = coeff; }
  inline bool is_IBM() { return (i_source_pdf_ != -1) ? true : false ;}
  inline const int& get_correction_variable_initiale() const { return correction_variable_initiale_; }

protected:
  int i_source_pdf_=-1;
  DoubleTab champ_coeff_pdf_som_;
  int correction_variable_initiale_=-1;

  OBS_PTR(Equation_base) eq_IBM_;
};


#endif /* Equation_IBM_proto_included */
