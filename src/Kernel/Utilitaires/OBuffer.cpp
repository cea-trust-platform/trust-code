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
//////////////////////////////////////////////////////////////////////////////
//
// File:        OBuffer.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////
#include <OBuffer.h>
#include <cstring>
#include <assert.h>
#include <Process.h>

OBuffer::OBuffer() :
  precision_(8), buf_(0)
{
  new_buffer();
}

OBuffer::~OBuffer()
{
  if(buf_)
    delete buf_;
  buf_=0;
  set_ostream(buf_); // Empeche Sortie::~Sortie de detruire l'objet une deuxieme fois.
}

void OBuffer::new_buffer()
{
  if(buf_)
    delete buf_;
  buf_ = new std::ostringstream();
  set_ostream(buf_);
  buf_->setf(ios::scientific);
  buf_->precision(precision_);
}

void OBuffer::precision(int i)
{
  precision_ = i;
  buf_->precision(i);
}

int OBuffer::get_precision()
{
  return buf_->precision();
}
// Description:
//    Force l'ecriture sur disque des donnees dans le tampon
//    Utilise l'implementation de la classe ofstream
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& OBuffer::flush()
{
  return *this;
}

// Description:
//  ajoute le caractere nul a la fin du buffer pour en faire une chaine
//  de caracteres valide. len() renvoie la longueur du buffer, y compris le \0
//  Voir Ecr_Fic_Par::syncfile()
void OBuffer::put_null_char()
{
  assert(! bin_); // Ca n'a pas de sens en binaire
  buf_->put('\0');
}

// Description:
//  Renvoie un pointeur sur le debut du buffer.
const char* OBuffer::str()
{
// Need a copy:
  string_ = buf_->str();
  return string_.c_str();
}

// Renvoie le nombre d'octets contenus dans le buffer.
int OBuffer::len()
{
  return buf_->tellp();
}

int OBuffer::set_bin(int bin)
{
  assert(bin==0 || bin==1);
  bin_ = bin;
  return bin_;
}
