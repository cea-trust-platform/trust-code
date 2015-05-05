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
// File:        SupprimeBord.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/7
//
//////////////////////////////////////////////////////////////////////////////

#include <SupprimeBord.h>
#include <EFichier.h>
Implemente_instanciable(SupprimeBord,"Supprime_Bord",Interprete_geometrique_base);

Sortie& SupprimeBord::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& SupprimeBord::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& SupprimeBord::interpreter_(Entree& is)
{
  Nom nom_dom;
  LIST(Nom) nlistbord;
  associer_domaine(is);
  Nom mot;
  is >>mot;
  if (mot!="{")
    {
      Cerr<< "we expected { and not "<<mot<<finl;
      exit();
    }
  is >>mot;
  while (mot!="}")
    {
      nlistbord.add(mot);
      is>>mot;
    }
  Domaine& dom=domaine();
  Cout<<"HERE  "<<nlistbord<<finl;
  Zone& zone=dom.zone(0);

  for (int b=0; b<nlistbord.size(); b++)
    {

      {
        // la recup des bords et des raccords est dans la boucle
        // pour pouvoir supprimer ...
        Bords& listbord=zone.faces_bord();
        Raccords& listrac=zone.faces_raccord();
        const Nom& nombord=nlistbord[b];
        int num_b=zone.rang_frontiere(nombord);
        if (num_b<listbord.size())
          listbord.suppr(zone.bord(nombord));
        else
          listrac.suppr(zone.raccord(nombord));
      }
    }

  return is;
}
