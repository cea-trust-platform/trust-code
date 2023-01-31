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

#include <Domaine_dis_base.h>
#include <Sous_domaine_dis_base.h>
#include <Sous_Domaine.h>

Implemente_base(Sous_domaine_dis_base,"Sous_domaine_dis_base",Objet_U);


Sortie& Sous_domaine_dis_base::printOn(Sortie& os) const
{
  return os ;
}

Entree& Sous_domaine_dis_base::readOn(Entree& is)
{
  return is ;
}

// Not inlined to avoid cyclic includes
const Sous_Domaine& Sous_domaine_dis_base::sous_domaine() const
{
  return la_sous_domaine.valeur();
}

Sous_Domaine& Sous_domaine_dis_base::sous_domaine()
{
  return la_sous_domaine.valeur();
}

/*! @brief Associe une Sous_Domaine a l'objet.
 *
 */
void Sous_domaine_dis_base::associer_sous_domaine(const Sous_Domaine& une_sous_domaine)
{
  la_sous_domaine=une_sous_domaine;
}

/*! @brief Associe une Domaine_dis a l'objet.
 *
 */
void Sous_domaine_dis_base::associer_domaine_dis(const Domaine_dis_base& une_domaine_dis)
{
  le_dom_dis=une_domaine_dis;
}

const Domaine_dis_base& Sous_domaine_dis_base::domaine_dis() const
{
  return le_dom_dis.valeur();
}

Domaine_dis_base& Sous_domaine_dis_base::domaine_dis()
{
  return le_dom_dis.valeur();
}

