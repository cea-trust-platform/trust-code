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
// File:        Discretiser.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Discretiser.h>
#include <Probleme_Couple.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>

Implemente_instanciable(Discretiser,"Discretiser|Discretize",Interprete);

int Discretiser::is_deja_appele=0;


// Description:
//    Simple appel a:
//      Interprete::printOn(Sortie&)
//    Imprime l'interprete sur un flot de sortie
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Discretiser::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


// Description:
//    Simple appel a:
//      Interprete::readOn(Entree&)
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
Entree& Discretiser::readOn(Entree& is)
{
  return Interprete::readOn(is);
}


// Description:
//    Fonction principale de l'interprete Discretiser:
//      discretiser un probleme avec une discretisation
//    On controle dynamiquement le type du probleme et
//    de la discretisation que l'on a lu, on verifie
//    que le schema en temps du probleme a ete lu et on
//    discretise le probleme avec la discretisation.
// Precondition: le schema en temps du probleme doit avoir ete lu pour discretiser
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: discretisation non reconnue
// Exception: on ne sait pas discretiser car le probleme n'est pas d'un
//            sous-type reconnu (Probleme,Probleme_base,Probleme_Couple)
// Effets de bord:
// Postcondition: le probleme est discretise avec la discretisation specifiee
Entree& Discretiser::interpreter(Entree& is)
{
saisie :
  Nom nom1, nom2;
  is >> nom1 >> nom2;
  Objet_U& ob1=objet(nom1);
  Objet_U& ob2=objet(nom2);

  if (is_deja_appele==0)
    is_deja_appele = 1;
  else
    {
      Cerr<<"Be careful, you call several times the interpreter Discretiser"<<finl;
      Cerr<<"there is probably an error in your data set"<<finl;
      //      exit();
    }

  if( !( sub_type(Discretisation_base, ob2) ) )
    {
      Cerr << nom2 << "is not a recognized discretization" << finl;
      exit();
    }
  Discretisation_base& typ=ref_cast(Discretisation_base, ob2);
  Cerr << "The chosen discretization is of type " << typ.que_suis_je() << finl;
  if (typ.que_suis_je()=="VEF")
    {
      Cerr << "Since 1.6 version, the keyword VEF is obsolete." << finl;
      Cerr << "Use keyword VEFPreP1B and P0 support to match the old VEF keyword." << finl;
      exit();
    }
  // Test on bidim_axi
  if ( dimension == 2 && !(typ.que_suis_je()=="VDF") && bidim_axi == 1 )
    {
      Cerr << "The feature 'bidim_axi' is only available for 2D calculation with VDF discretisation." << finl;
      exit();
    }
  //
  if( sub_type(Probleme_base, ob1) )
    {
      Probleme_base& pb=ref_cast(Probleme_base,ob1);
      if(!pb.schema_temps().lu())
        {
          Cerr << "it must have read the time scheme before to discretize" << finl;
          exit();
        }
      Cerr << "The chosen time scheme is of type " << pb.schema_temps().que_suis_je() << finl;
      Cerr << "We treat the problem " << pb.le_nom() << " of type " << pb.que_suis_je() << finl;
      Cerr << "Discretization of " << pb.le_nom() << " in progress..." << finl;
      pb.discretiser(typ);
    }
  else if( sub_type(Probleme_Couple, ob1) )
    {
      Probleme_Couple& pbc=ref_cast(Probleme_Couple,ob1);
      if(!pbc.schema_temps().lu())
        {
          Cerr << "it must have read the time scheme before to discretize" << finl;
          exit();
        }
      Cerr << "The chosen time scheme is of type " << pbc.schema_temps().que_suis_je() << finl;
      Cerr << "We treat the problem " << pbc.le_nom() << " of type " << pbc.que_suis_je() << finl;
      Cerr << "It corresponds to the coupling of the following problems: " << finl;
      for(int i=0; i< pbc.nb_problemes(); i++)
        Cerr << " " <<  pbc.probleme(i).le_nom() << " of type " << pbc.probleme(i).que_suis_je() << finl;
      Cerr << "Discretization of " << pbc.le_nom() << " in progress..." << finl;
      pbc.discretiser(typ);
    }
  else
    {
      Cerr << "It is not known discretize a : " << ob1.que_suis_je()        << finl;
      Cerr << "return at the seizure of the arguments" << finl;
      goto saisie;
    }
  return is;
}
