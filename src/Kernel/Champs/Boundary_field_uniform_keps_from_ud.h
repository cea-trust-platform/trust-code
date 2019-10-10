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
// File:        Boundary_field_uniform_keps_from_ud.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Boundary_field_uniform_keps_from_ud_included
#define Boundary_field_uniform_keps_from_ud_included

#include <Champ_front_uniforme.h>



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Boundary_field_uniform_keps_from_ud
//     Classe derivee de Champ_front_uniforme which is for uniform fields
//     in space and time
// .SECTION voir aussi
//     Champ_front_base Champ_front_uniforme
//////////////////////////////////////////////////////////////////////////////
class Boundary_field_uniform_keps_from_ud : public Champ_front_uniforme
{
  Declare_instanciable(Boundary_field_uniform_keps_from_ud);
};

struct pair
{
  double k;
  double eps;
};

inline pair k_eps_from_udi(const double& u, const double& d, const double& I, const int& dimension)
{
  //Now compute associated K and Eps by appling formula given by :
  //http://en.wikipedia.org/wiki/Turbulence_kinetic_energy
  //http://www.cfd-online.com/Wiki/Turbulence_length_
  //http://support.esi-cfd.com/esi-users/turb_parameters/

  //Hypothesis:
  //Cmu=0.09 : k-eps parameter
  //l=0.038 Dh where Dh is the hydraulic diameter
  //Note that Cmu^(3/4)=0.1643


  //in 3D we have
  //k = 3/2*(u*I)^2
  //in 2D we have
  //k = (u*I)^2
  //with u = initial velocity magnitude
  //eps = Cmu^(3/4) * k^(3/2) * l^(-1)
  //where l is the turbulence length scale which can be expressed as 0.038 Dh
  //Dh is the hydraulic diameter
  double l=0.038*d;
  pair val;
  val.k = ( dimension == 2 ? 1 : 3./2. )*(u*I)*(u*I);
  val.eps = 0.1643*pow(val.k,1.5)/l;
  return val;
}
#endif
