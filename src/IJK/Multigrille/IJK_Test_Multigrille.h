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
// File      : IJK_Test_Multigrille.h
// Directory : $IJK_ROOT/src
//
/////////////////////////////////////////////////////////////////////////////
#ifndef IJK_Test_Multigrille_included
#define IJK_Test_Multigrille_included
#include <Interprete.h>
#include <Multigrille_Adrien.h>
#include <IJK_Splitting.h>
#include <IJK_Field.h>
#include <IJK_Lata_writer.h>
#include <IJK_Navier_Stokes_tools.h>

// This class reads a Right Hand Side and a Poisson Coefficient from a lata file
// and runs the multigrid algorithm to solve the problem, providing convergence and timing information
class IJK_Test_Multigrille : public Interprete
{
  Declare_instanciable(IJK_Test_Multigrille);
public:
  Entree& interpreter(Entree&) override;

  IJK_Splitting splitting_;
  IJK_Field_double rho_;
  IJK_Field_double rhs_;
  IJK_Field_double resu_;
  Multigrille_Adrien poisson_solver_;
  Nom fichier_reprise_rho_;
  int timestep_reprise_rho_;
  Nom fichier_reprise_rhs_;
  int timestep_reprise_rhs_;
  Nom expression_rho_;
  Nom expression_rhs_;
};

#endif
