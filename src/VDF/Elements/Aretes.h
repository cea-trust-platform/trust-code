/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Aretes.h
// Directory:   $TRUST_ROOT/src/VDF/Elements
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Aretes_included
#define Aretes_included



//
// .DESCRIPTION class Aretes
//

#include <IntTab.h>

class Zone_VDF;
class DoubleTab;
//////////////////////////////////////////////////////////////////////////////
//
// Classe Aretes : represente un ensemble d' aretes
//
//////////////////////////////////////////////////////////////////////////////

class Aretes : public Objet_U
{

  Declare_instanciable(Aretes);

public:

  inline Aretes(int n);
  void affecter(int&, int, int, int,
                int, int, int, int, const ArrOfInt&);
  inline IntTab& faces();
  void dimensionner(int );
  void trier(int&, int&, int&, int&);
  void trier_pour_debog(int&, int&, int&, int&,const DoubleTab&);
  void calculer_centre_de_gravite(Zone_VDF& zone);

private :
  IntTab faces_;
  IntVect type1_;
  IntVect type2_;
  void swap(int, int);

};

// Description :
// Constructeur : dimensionne les tableaux
inline Aretes::Aretes(int n) : faces_(n,4),type1_(n),type2_(n)
{
}

// Description :
// retourne le tableau (IntTab) des faces
// de dimension nb_aretes,4
// faces(num_arete,i) 0 <= i <= 3:
// numeros des 4 faces qui se partagent l'arete num_arete
// Pour une arete interne les 4 faces existent
// Pour une arete bord un ou deux des numeros valent -1.
inline IntTab& Aretes::faces()
{
  return faces_;
}

#endif  // Aretes_H
