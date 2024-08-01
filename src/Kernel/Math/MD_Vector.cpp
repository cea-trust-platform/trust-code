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

#include <MD_Vector.h>
#include <Memoire.h>

//Implemente_base_sans_constructeur_ni_destructeur(MD_Vector_base,"MD_Vector_base",Objet_U);

/*! @brief construction d'un objet MD_Vector par copie d'un objet existant.
 *
 * C'est la methode recommandee pour creer un objet MD_Vector (autrement que
 * par copie d'un autre MD_Vector)
 */
void MD_Vector::copy(const MD_Vector_base& src)
{
  int num_obj = src.duplique();
  MD_Vector_base * p = dynamic_cast<MD_Vector_base *>(Memoire::Instance().objet_u_ptr(num_obj));
  assert(p!= nullptr);
  ptr_.reset(p);
}

/*! @brief renvoie 1 si les structures sont identiques, 0 sinon
 *
 */
int MD_Vector::operator==(const MD_Vector& md) const
{
  // Pour l'instant, test tres restrictif: les deux structures sont
  //  identiques si et seulement si les pointeurs sont identiques,
  //  (autrement dit la deuxieme a ete creee par copie de la premiere)
  // Si on veut debrancher le systeme de reference multiple et dupliquer
  //  les structures a la copie, il faut faire le test complet d'egalite
  //  sur toute la structure !
  return ptr_ == md.ptr_;
}

/*! @brief reponse inverse de == ...
 */
int MD_Vector::operator!=(const MD_Vector& md) const
{
  return !operator==(md);
}
