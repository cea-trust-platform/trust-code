
/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Interpolation_IBM_power_law_tbl.h
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Interpolation_IBM
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Interpolation_IBM_power_law_tbl_included
#define Interpolation_IBM_power_law_tbl_included

#include <Interpolation_IBM_elem_fluid.h>
#include <Champ_Don.h>
#include <Zone.h>
#include <Param.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Interpolation_IBM_power_law_tbl
//
// <Description of class Interpolation_IBM_power_law_tbl>
//
/////////////////////////////////////////////////////////////////////////////

class Interpolation_IBM_power_law_tbl : public Interpolation_IBM_elem_fluid
{

  Declare_instanciable( Interpolation_IBM_power_law_tbl ) ;

public :
  inline double get_A_pwl()
  {
    return  A_pwl_;
  };
  inline double get_B_pwl()
  {
    return  B_pwl_;
  };
  inline double get_y_c_p_pwl()
  {
    return y_c_p_pwl_;
  };

protected :
  double A_pwl_ = 8.3;
  double B_pwl_ = 1./7. ;
  double y_c_p_pwl_ = 11.81;
};

#endif /* Interpolation_IBM_power_law_tbl_included */
