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
// File:        DescStructure.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <DescStructure.h>

Implemente_instanciable(DescStructure,"DescStructure",Objet_U);

#include <Esp_Virt.h>
#include <Esp_Dist.h>
Implemente_vect(Esp_Virt);
Implemente_vect(Esp_Dist);

// Description:
//    Lecture d'un descripteur de structure
//    Un descripteur de structure est constitue d'un
//    identificateur, d'un vecteur d'espaces virtuels,
//    d'un vecteur d'espaces distants et d'un descripteur de
//    structure interne
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& DescStructure::readOn(Entree& is)
{
//  VectEsp_Virt virt_data_;
//  VectEsp_Dist dist_data_;
  VECT(Descripteur) desc_;
  //  is >> identificateur_;
  // is >> virt_data_;
  VECT(Esp_Virt) virt;
  virt.lit(is);
  VECT(Esp_Dist) dist;
  dist.lit(is);
  {
    int i;
    is >> i;
    desc_.dimensionner_force(i);
    while(i--)
      is>>desc_[i];
    //    assert(desc_.size()>=0); if(desc_.size()) assert(desc_!=0);
  }
  return is;
}


// Description:
//    Ecriture d'un descripteur de structure
//    Un descripteur de structure est constitue d'un
//    identificateur, d'un vecteur d'espaces virtuels,
//    d'un vecteur d'espaces distants et d'un descripteur de
//    structure interne
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie a utiliser
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& DescStructure::printOn(Sortie& os) const
{
  Cerr<<"DescStructure::printOn no more used" <<finl;
  exit();
  return os;
}

