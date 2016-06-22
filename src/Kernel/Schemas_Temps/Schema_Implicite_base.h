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
// File:        Schema_Implicite_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Schemas_Temps
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Schema_Implicite_base_included
#define Schema_Implicite_base_included



//
// .DESCRIPTION class Schema_Implicite_base
//  Classe de base pour tous les schemas en temps implicite
//  Il porte un objet de type Solveur ( ex le Simpler)
// .SECTION voir aussi
//

#include <Schema_Temps_base.h>
#include <Solveur_Implicite.h>


class Schema_Implicite_base : public Schema_Temps_base
{
  Declare_base(Schema_Implicite_base);
public:

  inline Solveur_Implicite& solveur();
  inline const  Solveur_Implicite& solveur() const;
  virtual void set_param(Param& param);

protected:
  Solveur_Implicite le_solveur;
};


// Description
inline Solveur_Implicite& Schema_Implicite_base::solveur()
{
  return le_solveur;
}

// Description
inline const Solveur_Implicite& Schema_Implicite_base::solveur() const
{
  return le_solveur;
}

#endif

