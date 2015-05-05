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
// File:        Terme_Source_inc_th.cpp
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Terme_Source_inc_th.h>
#include <Motcle.h>
//

void Terme_Source_inc_th::associer_eqn(const Navier_Stokes_std& eq_hyd)
{
  eq_hydraulique_ = eq_hyd;
}

void Terme_Source_inc_th::associer_eqn(const Convection_Diffusion_Temperature& eq_thm)
{
  eq_thermique_ = eq_thm;
}


Entree& Terme_Source_inc_th::lire_donnees(Entree& is)
{
  Cerr << "Source_inc_th::lire_donnees" << finl;
  // Initialisation
  //   u_etoile = 1.;
  //   h = 1.;
  //   coeff = 1;
  //   dir_source = 1;
  impr = 0;
  // Fin Init
  Motcle motlu, accolade_fermee="}", accolade_ouverte="{", mot="impr";
  is >> motlu;
  if(motlu != accolade_ouverte)
    {
      Cerr << "On attendait { a la place de " << motlu
           << " lors de la lecture de inc " << finl;
    }
  is >> motlu;
  while (motlu != accolade_fermee)
    {
      if (motlu==mot)
        {
          impr = 1;
          is >> motlu;
        }
      else
        {
          Cerr << "Le mot lu : " <<  motlu << " n'est pas compris dans Source_inc_th" << finl;
          Cerr << "Le mot compris est :  impr " << finl;
          Process::exit();
        }
    }
  return is;
}
