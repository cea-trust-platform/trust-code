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

#ifndef Navier_Stokes_IBM_impl_included
#define Navier_Stokes_IBM_impl_included

#include <TRUSTTab.h>
#include <Ref_Navier_Stokes_std.h>

class Entree;
class Param;

class Navier_Stokes_IBM_impl
{

public:

  void set_param_IBM(Param& param);
  Entree& readOn_IBM(Entree& is, Navier_Stokes_std& eq);
  void modify_initial_velocity_IBM( DoubleTab& );
  void modify_initial_gradP_IBM( DoubleTrav& );
  void reprise_calcul_IBM( DoubleTrav& );
  void pression_initiale_IBM( DoubleTrav& );
  void pression_correction_IBM( DoubleTrav& );
  void matrice_pression_IBM ();
  void preparer_calcul_IBM(const Nom& nom_eq,const Nom& nom_eq2);

  bool initTimeStep_IBM(bool ddt);

  inline const int& get_matrice_pression_penalisee_H1() const { return matrice_pression_penalisee_H1_; }
  inline const int& get_correction_matrice_pression() const { return correction_matrice_pression_; }
  inline const int& get_correction_vitesse_modifie() const { return correction_vitesse_modifie_; }
  inline const int& get_correction_pression_modifie() const { return correction_pression_modifie_; }
  inline const int& get_gradient_pression_qdm_modifie() const { return gradient_pression_qdm_modifie_; }
  inline const int& get_i_source_pdf() const { return i_source_pdf_; }
  inline const DoubleTab& get_champ_coeff_pdf_som() const { return champ_coeff_pdf_som_; }
  inline const int& get_correction_vitesse_projection_initiale() const { return correction_vitesse_projection_initiale_; }
  inline const int& get_correction_calcul_pression_initiale() const { return correction_calcul_pression_initiale_; }
  inline const int& get_correction_matrice_projection_initiale() const { return correction_matrice_projection_initiale_; }

  inline bool is_IBM() { return (i_source_pdf_ != -1) ? true : false ;}

protected:
  int postraiter_gradient_pression_sans_masse_=0;
  int correction_matrice_projection_initiale_=0;
  int correction_calcul_pression_initiale_=0;
  int correction_vitesse_projection_initiale_=1;
  int correction_matrice_pression_=1;
  int matrice_pression_penalisee_H1_=0;
  int correction_vitesse_modifie_=1;
  int correction_pression_modifie_=0;
  int gradient_pression_qdm_modifie_=0;
  int i_source_pdf_=-1;
  DoubleTab champ_coeff_pdf_som_;

  REF(Navier_Stokes_std) eq_NS;
};


#endif
