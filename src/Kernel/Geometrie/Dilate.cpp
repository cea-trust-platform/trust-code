/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Dilate.h>
#include <Domaine.h>

Implemente_instanciable(Dilate,"Dilate",Interprete_geometrique_base);


/*! @brief Simple appel a: Interprete::printOn(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Dilate::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


/*! @brief Simple appel a: Interprete::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Dilate::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

/*! @brief Fonction principale de l'interprete Dilate Structure du jeu de donnee (en dimension 2) :
 *
 *     Dilate dom alpha
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree
 * @throws l'objet a mailler n'est pas du type Domaine
 */
Entree& Dilate::interpreter_(Entree& is)
{
  double alpha;
  associer_domaine(is);
  is >> alpha;
  if (Process::is_parallel())
    {
      Cerr << "Dilate can not be use in parallel." << finl;
      Cerr << "Put rather this interpreter in the " << finl;
      Cerr << "data file of the mesh splitter." << finl;
      exit();
    }

  DoubleTab& coord=domaine().les_sommets();
  coord*=alpha;
  Cerr << "Expands the domain " << domaine().le_nom() << " of a factor " << alpha << finl;

  return is;
}

