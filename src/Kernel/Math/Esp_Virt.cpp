/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Esp_Virt.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#include <Esp_Virt.h>

Implemente_instanciable_sans_constructeur(Esp_Virt,"Esp_Virt",Objet_U);

// Description:
//    Lecture d'un espace virtuel dans un flot d'entree
//    Un espace virtuel est represente par ses 3 attributs PE_voisin_, deb_ et nb_
Entree& Esp_Virt::readOn(Entree& is)
{
  is >> PE_voisin_;
  is >> desc_ev_;
  return is;
}

// Description:
//    Ecriture d'un espace virtuel sur un flot de sortie
//    Un espace virtuel est represente par ses 3 attributs PE_voisin_, deb_ et nb_
Sortie& Esp_Virt::printOn(Sortie& os) const
{
  os << PE_voisin_ << finl;
  os << desc_ev_ << finl;
  return os;
}

// Description:
// Precondition:
//    Constructeur par defaut.
//    Tous les attributs sont fixes a -1
Esp_Virt::Esp_Virt()
{
  PE_voisin_ = -1;
}
