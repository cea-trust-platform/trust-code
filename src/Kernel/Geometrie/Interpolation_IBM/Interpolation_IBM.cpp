/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Interpolation_IBM.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie/Interpolation_IBM
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Interpolation_IBM.h>
#include <Nom.h>

Implemente_deriv(Interpolation_IBM_base);
Implemente_instanciable( Interpolation_IBM, "Interpolation_IBM", Interpolation_IBM_base ) ;
// XD interpolation_ibm interpolation_ibm_base interpolation_ibm 0 Generic class for the interpolation method available in the Immersed Boundary Method (IBM).
// XD    attr inter_typ chaine(into=["aucune","element_fluide","gradient_moyen","hybride"]) inter_typ 1 Type of interpolation.
// XD    attr interp bloc_lecture interp 1 Interpolation parameters

Sortie& Interpolation_IBM::printOn( Sortie& os ) const
{
  DERIV(Interpolation_IBM_base)::printOn( os );
  return os;
}

Entree& Interpolation_IBM::readOn( Entree& is )
{
  Nom nom_modele("Interpolation_IBM_");
  Nom type_mod;
  is >> type_mod;
  nom_modele+=type_mod;
  typer(nom_modele);
  is >> valeur();
  return is;
}
