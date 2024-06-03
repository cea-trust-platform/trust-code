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

#ifndef DomaineCutter_Correspondance_included
#define DomaineCutter_Correspondance_included

#include <TRUSTArray.h>

/*! @brief Classe outil contenant la correspondance entre les indices de sommets et d'elements du domaine global et d'un sous-domaine.
 *
 *   Les champs de cette classe sont remplis par DomaineCutter::construire_sous_domaine()
 *
 */
class DomaineCutter_Correspondance
{
  DomaineCutter_Correspondance();
  void reset();
  const ArrOfInt& get_liste_sommets() const;
  const ArrOfInt& get_liste_inverse_sommets() const;
  const ArrOfInt& get_liste_inverse_elements() const;

private:
  // Le numero du sous-domaine construit
  int partie_;
  // Correspondance sommets local/global:
  //   indice_som_global = liste_sommets[indice_local]
  ArrOfInt liste_sommets_;
  // Correspondance sommets global/local:
  //   indice_local = liste_inverse_sommets_[indice_som_global]
  //   egal -1 si le sommet n'est pas dans la partie
  ArrOfInt liste_inverse_sommets_;
  // Correspondance elements global/local:
  //   indice_local = liste_inverse_elements_[indice_elem_global]
  ArrOfInt liste_inverse_elements_;

  // La seule classe qui construit cet objet. Pour l'instant...
  friend class DomaineCutter;
};
#endif
