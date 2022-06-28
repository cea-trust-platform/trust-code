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

#ifndef Interpolation_IBM_base_included
#define Interpolation_IBM_base_included

#include <Objet_U.h>
#include <Discretisation_base.h>
#include <Zone_dis_base.h>
#include <Champ_Don.h>
#include <Param.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Interpolation_IBM_base
//
// <Description of class Interpolation_IBM_base>
//
/////////////////////////////////////////////////////////////////////////////

class Interpolation_IBM_base : public Objet_U
{

  Declare_base( Interpolation_IBM_base ) ;

public:
  virtual void discretise(const Discretisation_base&, Zone_dis_base&);
  inline int get_impr()
  {
    return impr_;
  };
  void set_param(Param&);
protected:

  Champ_Don solid_points_lu_;
  Champ_Don solid_points_;

  Champ_Don corresp_elems_lu_;
  Champ_Don corresp_elems_;
  int impr_=0;  // Default value
};

#endif /* Interpolation_IBM_base_included */
