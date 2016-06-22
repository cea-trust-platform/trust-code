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
// File:        Traitement_particulier_NS_temperature.cpp
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_temperature.h>

Implemente_base_sans_constructeur_ni_destructeur(Traitement_particulier_NS_temperature,"Traitement_particulier_NS_temperature",Traitement_particulier_NS_base);


// Description:
//
// Precondition:
// Parametre: Sortie& is
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
Sortie& Traitement_particulier_NS_temperature::printOn(Sortie& is) const
{
  return is;
}


// Description:
//
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
Entree& Traitement_particulier_NS_temperature::readOn(Entree& is)
{
  return is;
}

Entree& Traitement_particulier_NS_temperature::lire(Entree& is)
{
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle motbidon, motlu;
  is >> motbidon ;
  if (motbidon == accouverte)
    {
      Motcles les_mots(2);
      les_mots[0] = "Bord";
      les_mots[1] = "Direction";

      is >> motlu;
      while(motlu != accfermee)
        {
          int rang=les_mots.search(motlu);
          switch(rang)
            {
            case 0 :
              {
                is >> nom_cl;
                break;
              }
            case 1 :
              {
                is >> dir;      // intervalle de temps de sorties des moyennes temporelles
                break;
              }
            default :
              {
                Cerr << motlu << " is not a keyword for Traitement_particulier_NS_canal." << finl;
                Cerr << "The keywords are: " << les_mots << finl;
                exit();
              }
            }
          is >> motlu;
        }
      is >> motlu;
      if (motlu != accfermee)
        {
          Cerr << "Error while reading in Traitement_particulier_NS_temperature" << finl;;
          Cerr << "We were expecting a }" << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Error while reading in Traitement_particulier_NS_temperature" << finl;
      Cerr << "We were expecting a {" << finl;
      exit();
    }

  return is;
}


void Traitement_particulier_NS_temperature::preparer_calcul_particulier()
{
  // maintenant code en 2D  au moins en vef
  /*
    if(Objet_U::dimension!=3)
    {
    Cerr << " Traitement_particulier_NS_temperature : non prevu pour des calculs autres que 3D " << finl;
    exit();
    }
  */
  calcul_temperature();
}

void Traitement_particulier_NS_temperature::post_traitement_particulier()
{
  calcul_temperature();
}


