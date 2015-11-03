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
// File:        Source_Generique_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////


#include <Source_Generique_base.h>
#include <Equation_base.h>
#include <Postraitement.h>

Implemente_base(Source_Generique_base,"Source_Generique_base",Source_base);


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Imprime la source sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Source_Generique_base::printOn(Sortie& os) const
{
  return os;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Lecture d'un terme source sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot d'entree est modifie
// Postcondition:
Entree& Source_Generique_base::readOn(Entree& is)
{
  is >> ch_source_;
  return is;
}

void Source_Generique_base::associer_pb(const Probleme_base&)
{

}

DoubleTab& Source_Generique_base::calculer(DoubleTab& resu) const
{
  resu=0;
  return ajouter(resu);
}

void Source_Generique_base::completer()
{
  Source_base::completer();

  Postraitement post_bidon;
  ch_source_->completer(post_bidon);
  Entity loc = ch_source_->get_localisation();
  if (((loc==ELEMENT) && (localisation_source()!="elem"))
      || ((loc==FACE) && (localisation_source()!="faces")))
    {
      Nom nom_loc;
      if (loc==ELEMENT)
        nom_loc ="elem";
      else if (loc==FACE)
        nom_loc = "faces";
      Cerr<<"----------------------------------------------------------------"<<finl;
      Cerr<<"Error during TRUST calculation :"<<finl;
      Cerr<<"----------------------------------------------------------------"<<finl;
      Cerr<<"The generic source field associated to the "<<que_suis_je()<<" source term "<<finl;
      Cerr<<"of the equation "<<equation().que_suis_je()<<" will be evaluated at localisation "<<nom_loc<<finl;
      Cerr<<"while the source term itself must be computed at location "<<localisation_source()<<finl;
      Cerr<<"Please contact TRUST support."<<finl;
      exit();
    }
}

void Source_Generique_base::mettre_a_jour(double temps)
{

}
