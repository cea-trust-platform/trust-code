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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Source_Permeabilite_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Sources
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Source_Permeabilite_VDF_included
#define Source_Permeabilite_VDF_included

#include <Modele_Permeabilite_base.h>

#ifdef DOXYGEN_SHOULD_SKIP_THIS // seulement un truc inutile pour check_source ...
class Source_Permeabilite_VDF
{ };
#endif /* DOXYGEN_SHOULD_SKIP_THIS */

// .DESCRIPTION class ModPerm_Cte
//  Cette classe represente une permeabilite cte.
class ModPerm_Cte : public Modele_Permeabilite_base
{
  Declare_instanciable_sans_constructeur(ModPerm_Cte);
public:
  ModPerm_Cte() : cte(1.) { }
  inline double getK(double ) const { return cte;} // Renvoie la valeur de la permeabilite en fonction de la porosite (d'une face par exemple).

protected:
  double cte;
};

// .DESCRIPTION
//  Cette classe represente la correlation de Carman Kozeny pour la permeabilite d'un lit de particule de diametre donne.
class ModPerm_Carman_Kozeny : public Modele_Permeabilite_base
{
  Declare_instanciable_sans_constructeur(ModPerm_Carman_Kozeny);
public:
  ModPerm_Carman_Kozeny() : diam(1.), C(180.) { }
  inline double getDiametre() const { return diam; }
  inline void setDiametre(double d) { diam = d; }

  inline virtual double getK(double porosite) const // Renvoie la valeur de la permeabilite en fonction de la porosite (d'une face par exemple).
  {
    const double tmp = diam *porosite /(1.-porosite);
    return tmp*tmp*porosite/C;
  }

protected:
  double diam, C;
};

// .DESCRIPTION
//  Cette classe represente la correlation de Ergun pour la permeabilite d'un lit de particule de diametre donne.
class ModPerm_ErgunPourDarcy : public ModPerm_Carman_Kozeny
{
  Declare_instanciable_sans_constructeur(ModPerm_ErgunPourDarcy);
public:
  ModPerm_ErgunPourDarcy() { C = 150.; }
};

// .DESCRIPTION
//  Cette classe represente la correlation de Ergun associe au terme de Forchheimer,
//  pour la permeabilite d'un lit de particule de diametre donne.
class ModPerm_ErgunPourForch : public ModPerm_Carman_Kozeny
{
  Declare_instanciable_sans_constructeur(ModPerm_ErgunPourForch);
public:
  ModPerm_ErgunPourForch() { C = 1.75; }

  inline double getK(double porosite) const // Renvoie la valeur de la permeabilite en fonction de la porosite (d'une face par exemple).
  {
    const double tmp = diam *porosite*porosite*porosite /(1.-porosite)/C;
    return tmp*tmp;
  }
};

#endif /* Source_Permeabilite_VDF_included */
