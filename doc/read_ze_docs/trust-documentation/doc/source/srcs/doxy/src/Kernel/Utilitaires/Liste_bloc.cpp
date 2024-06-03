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

#include <Liste_bloc.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Liste_bloc,"Liste_bloc",Objet_U);

Entree& Liste_bloc::readOn(Entree& is) { return is; }

Sortie& Liste_bloc::printOn(Sortie& os) const { return os; }

/*! @brief Renvoie le dernier element de la liste
 *
 * @return (Liste_bloc&) le dernier element de la liste
 */
Liste_bloc& Liste_bloc::dernier()
{
  if (est_dernier()) return *this;
  Liste_bloc_curseur curseur(*this);
  while (curseur)
    if (curseur.list().est_dernier()) break;
    else ++curseur;
  return curseur.list();
}

Liste_bloc::~Liste_bloc()
{
  vide();
  suivant_ = nullptr;
}

/*! @brief Vide la liste
 *
 */
void Liste_bloc::vide()
{
  if (!est_vide())
    {
      if (suivant_)
        {
          suivant_->vide();
          delete suivant_;
        }
      data = DerObjU();
    }
  suivant_ = this;
}

/*! @brief Operateur d'acces au ieme element de la liste
 *
 * @param (int i) l'indice de l'element a trouver
 * @return (Objet_U&) le ieme element de la liste
 */
Objet_U& Liste_bloc::operator[](int i)
{
  Liste_bloc_curseur curseur(*this);
  if (!curseur)
    {
      Cerr << que_suis_je() << " : empty list !" << finl;
      Process::exit();
    }
  while (curseur && i--)
    ++curseur;
  if (i != -1)
    {
      Cerr << que_suis_je() << " : overflow of list " << finl;
      assert(i == -1);
      Process::exit();
    }
  return curseur.valeur();
}

/*! @brief Ajout d'un Objet_U a la liste to_add est libere en sortie
 *
 */
Objet_U& Liste_bloc::add_deplace(DerObjU& to_add)
{
  if (est_vide())
    {
      data.deplace(to_add);
      suivant_ = nullptr;
      return valeur();
    }
  else
    {
      if (est_dernier())
        {
          Liste_bloc *liste_ptr = new Liste_bloc;
          if (!liste_ptr)
            {
              Cerr << "Run out of memory " << finl;
              Process::exit();
            }
          liste_ptr->add_deplace(to_add);
          suivant_ = liste_ptr;
          return suivant_->valeur();
        }
      else
        return dernier().add_deplace(to_add);
    }
}
