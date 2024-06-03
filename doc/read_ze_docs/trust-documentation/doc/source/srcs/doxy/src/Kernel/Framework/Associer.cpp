/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Associer.h>
#include <Nom.h>

Implemente_instanciable(Associer,"Associer|Associate",Interprete);

Sortie& Associer::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

Entree& Associer::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

/*! @brief Fonction principale de l'interprete Associer: associer deux objets.
 *
 *     On essaye d'associer 1 a 2 et 2 a 1, provoque une
 *     erreur si cela echoue.
 *
 * @param (Entree& is) un flot d'entree, a partir duquel on lit les noms des objets a associer
 * @return (Entree&) le flot d'entree modifie
 * @throws on ne sait pas associer obj1 et obj2
 */
Entree& Associer::interpreter(Entree& is)
{
  // Acquisition des parametres :
  Nom nom1, nom2;
  is >> nom1 >> nom2;
  if(is.eof())
    {
      Cerr << "Reading problem in Associer" << finl;
      exit();
    }
  Objet_U& ob1=objet(nom1);
  Objet_U& ob2=objet(nom2);
  int association12,association21;
  association12=ob1.associer_(ob2);
  association21=0;
  if (!association12)
    {
      association21=ob2.associer_(ob1);
      if(!association21)
        {
          Cerr << "It is not known associate " << ob1.que_suis_je()
               << " and " << ob2.que_suis_je()  << finl;
          exit();
        }
    }

  if (association12==2)
    {
      Cerr << finl;
      Cerr << "The medium " << ob2.que_suis_je() << " \"" << nom2
           << "\" can not be associated to the problem" << finl;
      Cerr << ob1.que_suis_je() << " \"" << nom1
           << "\" because it is already associated with another problem." << finl;
      exit();
    }
  if (association21==2)
    {
      Cerr << finl;
      Cerr << "The medium " << ob1.que_suis_je() << " \"" << nom1
           << "\" can not be associated to the problem" << finl;
      Cerr << ob2.que_suis_je() << " \"" << nom2
           << "\" because it is already associated with another problem." << finl;
      exit();
    }

  return is;
}
