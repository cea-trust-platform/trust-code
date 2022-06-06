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

#include <Phases.h>
#include <Pb_Multiphase.h>

Implemente_instanciable(Phases,"Phases",Interprete);

Sortie& Phases::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Phases::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

// Description:
//    Fonction principale de l'interprete Phases
//    Lit la dimension d'espace du probleme.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Phases::interpreter(Entree& is)
{
  Nom nom_pb, mot, nom_phase, nom_milieu;
  is >> nom_pb;
  Pb_Multiphase& pb = ref_cast(Pb_Multiphase, objet(nom_pb));

  Noms noms_phases;
  is >> mot;
  if (mot != "{") Cerr << "Phases : { expected instead of " << mot << finl, Process::exit();
  Noms noms_milieux;
  for (is >> mot; mot != "}"; is >> mot)
    {
      nom_phase = mot;
      if (mot.debute_par("liquide"))  nom_milieu = "liquide";
      else if (mot.debute_par("gaz")) nom_milieu = "gaz";
      else Process::exit("Phases : the names must begin with liquide or gaz");
      is >> mot;
      if (mot != "{") Cerr << "Phases : { expected instead of " << mot << finl, Process::exit();
      for (is >> mot; mot != "}"; is >> mot)
        noms_milieux.add(mot + "_" + nom_milieu), noms_phases.add(nom_phase + "_" + mot);
    }

  pb.set_noms_phases(noms_phases);
  pb.creer_milieu(noms_milieux);
  return is;
}

