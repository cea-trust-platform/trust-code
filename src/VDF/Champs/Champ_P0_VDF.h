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
// File:        Champ_P0_VDF.h
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Champ_P0_VDF_included
#define Champ_P0_VDF_included

#include <Champ_Inc_P0_base.h>
#include <Ref_Zone_VDF.h>

//.DESCRIPTION classe Champ_P0_VDF
// Classe qui represente un champ discret P0 par element
// associe a une zone discretisee de type Zone_VDF
//

//.SECTION voir aussi
// Champ_P0

class Champ_P0_VDF: public Champ_Inc_P0_base
{

  Declare_instanciable(Champ_P0_VDF);

public :

  void associer_zone_dis_base(const Zone_dis_base&);
  const Zone_dis_base& zone_dis_base() const;
  DoubleVect moyenne() const;
  double moyenne(int ) const;
  double valeur_au_bord(int face) const;
  virtual DoubleTab& remplir_coord_noeuds(DoubleTab& ) const;
  int imprime(Sortie& os, int nb_compo_) const;
  double integrale_espace(int ncomp) const;

private:

  REF(Zone_VDF) la_zone_VDF;
};

#endif
