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

#ifndef Remove_elem_included
#define Remove_elem_included

/*! @brief class Remove_elem Enleve du maillage les elements specifies par l'utilisateur dans le jeu de donnees
 *
 *
 *
 * @sa Interprete
 */

#include <Interprete_geometrique_base.h>
#include <TRUSTList.h>
#include <Parser_U.h>
#include <Domaine.h>

class Zone;

class Remove_elem : public Interprete_geometrique_base
{
  Declare_instanciable(Remove_elem);

public :

  Entree& interpreter_(Entree&) override;
  void Remove_elem_(Zone&);
  void recreer_faces(Zone& , Faces&, IntTab&) const;
  void creer_faces(Zone& , Faces&, IntTab&) const;
  void remplir_liste(IntTab&, int, int, int, int) const;

protected :

  IntList listelem;
  Parser_U f;
  int f_ok = -10;
private :
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
};

#endif


