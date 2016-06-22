/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Neumann_paroi_QC.h
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Neumann_paroi_QC_included
#define Neumann_paroi_QC_included



#include <Neumann_paroi.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Neumann_paroi_QC
//    Cette condition limite correspond a un flux impose pour l'equation de
//    transport d'un scalaire. Ex: une paroi chauffante pour l'equation de
//    transport de la temperature.
//    Le flux impose est uniforme. Version Quasi compressible
// .SECTION voir aussi
//    Neumann_paroi
//////////////////////////////////////////////////////////////////////////////
class Neumann_paroi_QC : public Neumann_paroi
{

  Declare_instanciable(Neumann_paroi_QC);

public :
  int compatible_avec_eqn(const Equation_base&) const;
  virtual double flux_impose(int i) const;
  virtual double flux_impose(int i,int j) const;
};



#endif
