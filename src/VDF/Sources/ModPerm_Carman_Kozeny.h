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
// File:        ModPerm_Carman_Kozeny.h
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef ModPerm_Carman_Kozeny_included
#define ModPerm_Carman_Kozeny_included

#include <Modele_Permeabilite_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Cette classe represente la correlation de Carman Kozeny
//    pour la permeabilite d'un lit de particule de diametre donne.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class ModPerm_Carman_Kozeny : public Modele_Permeabilite_base
{

  Declare_instanciable_sans_constructeur(ModPerm_Carman_Kozeny);

public:

  ModPerm_Carman_Kozeny();

  /**
   * Renvoie la valeur de la permeabilite en fonction de la porosite (d'une face par exemple).
   * @param porosite la porosite
   */
  inline double  getK(double porosite) const;

  inline void setDiametre(double d)
  {
    diam = d;
  } ;
  inline double getDiametre() const
  {
    return diam;
  } ;



  double diam;
  double C;
};

double ModPerm_Carman_Kozeny::getK(double porosite) const
{
  //assert(porosite != 1.);
  //Cout << " Dans ModPerm_Carman_Kozeny C = " << C << finl;
  double tmp = diam *porosite /(1.-porosite);
  return tmp*tmp*porosite/C;
}

#endif


