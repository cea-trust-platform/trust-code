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

#include <Ref_.h>
extern const Nom& deriv_vide();

Implemente_base(Ref_,"Ref_",Objet_U_ptr);


/*! @brief Ecriture de l'Objet_U reference sur un flot de sortie
 *
 * @param (Sortie& os) le flot de sortie a utiliser
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Ref_::printOn(Sortie& os)  const
{
  Objet_U_ptr::printOn(os);
  return os;
}


/*! @brief Lecture dans un flot d'entree A surcharger.
 *
 * Ne fait rien
 *
 * @param (Entree& is) le flot d'entree a utiliser
 * @return (Entree&) le flux d'entree non modifie
 */
Entree& Ref_::readOn(Entree& is)
{
  Objet_U_ptr::readOn(is);
  return is;
}

/*! @brief Annule la ref
 *
 */
void Ref_::reset()
{
  set_Objet_U_ptr((Objet_U*) 0);
}
