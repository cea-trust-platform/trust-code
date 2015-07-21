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
// File:        ModPerm_Cte.h
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ModPerm_Cte_included
#define ModPerm_Cte_included
#include <Modele_Permeabilite_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Cette classe represente une permeabilite cte.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class ModPerm_Cte : public Modele_Permeabilite_base
{

  Declare_instanciable_sans_constructeur(ModPerm_Cte);

public:

  ModPerm_Cte();

  /**
   * Renvoie la valeur de la permeabilite en fonction de la porosite (d'une face par exemple).
   * @param porosite la porosite
   */
  inline double  getK(double porosite) const;


private :

  double cte;

};

double ModPerm_Cte::getK(double porosite) const
{
  //assert(porosite != 1.);
  return cte;
}

#endif


