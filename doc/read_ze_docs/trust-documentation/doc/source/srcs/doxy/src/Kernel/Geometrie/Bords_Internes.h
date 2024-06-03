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

#ifndef Bords_Internes_included
#define Bords_Internes_included

#include <TRUST_List.h>
#include <Bord_Interne.h>

/*! @brief Class Bords_Internes Cette classe represente une liste d'objets de type Bords_Interne
 *
 * @sa Bords_Interne
 */
class Bords_Internes : public LIST(Bord_Interne)
{
  Declare_instanciable(Bords_Internes);

public :

  void associer_domaine(const Domaine&);
  int nb_faces() const;
  inline int nb_bords_internes() const;
  //JY 26/08/97
  int nb_faces(Type_Face type) const;
};

/*! @brief Renvoie le nombre d'objet Bord_Interne stockes dans la liste.
 *
 * @return (int) le nombre d'objet Bord_Interne stockes dans la liste
 */
inline int Bords_Internes::nb_bords_internes() const
{
  return size();
}

#endif /* Bords_Internes_included */
