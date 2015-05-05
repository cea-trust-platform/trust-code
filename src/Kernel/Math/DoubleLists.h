/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        DoubleLists.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef DoubleLists_included
#define DoubleLists_included

#include <DoubleList.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Un tableau de listes de type DoubleList
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class DoubleLists
{
public :
  DoubleLists();
  Sortie& printOn(Sortie& os) const;
  Entree& readOn(Entree& is);
  inline int size() const;
  DoubleLists(int );
  const DoubleList& operator[](int) const;
  DoubleList& operator[](int);
  DoubleLists(const DoubleLists&);
  virtual ~DoubleLists();
  DoubleLists& operator=(const DoubleLists& );
  int search(const DoubleList&) const;
  void dimensionner(int );
protected :
  int sz;
  DoubleList* data;
};


// Description:
//    retourne le nombre de listes dans le tableau
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: nombre de listes dans le tableau
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline int DoubleLists::size() const
{
  return sz;
}

#endif
