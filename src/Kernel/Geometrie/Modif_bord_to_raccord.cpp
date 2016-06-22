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
// File:        Modif_bord_to_raccord.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Modif_bord_to_raccord.h>
#include <EFichier.h>
Implemente_instanciable(Modif_bord_to_raccord,"Modif_bord_to_raccord",Interprete_geometrique_base);

Sortie& Modif_bord_to_raccord::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Modif_bord_to_raccord::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& Modif_bord_to_raccord::interpreter_(Entree& is)
{
  Nom nom_bord;
  associer_domaine(is);
  is>>nom_bord;
  Domaine& dom=domaine();

  Zone& zone=dom.zone(0);
  Bord& bord=zone.bord(nom_bord);

  Raccords& listrac=zone.faces_raccord();

  Raccord racc_base;
  racc_base.typer("Raccord_local_homogene");
  Raccord_base& racc=racc_base.valeur();

  // on caste en Frontiere pour pouvoir faire la copie ...
  ref_cast(Frontiere,racc)=ref_cast(Frontiere,bord);
  listrac.add(racc_base);

  Bords& listbord=zone.faces_bord();
  listbord.suppr(zone.bord(nom_bord));
  zone.fixer_premieres_faces_frontiere();
  return is;
}
