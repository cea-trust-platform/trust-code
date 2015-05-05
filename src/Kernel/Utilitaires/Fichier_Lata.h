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
// File:        Fichier_Lata.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////
#ifndef Fichier_Lata_included
#define Fichier_Lata_included

#include <Format_Post_Lata.h>

// .DESCRIPTION        :
//  Cette classe contient un fichier de type EcrFicPartage ou EcrFicPrive,
//  en ASCII ou en BINAIRE selon le format specifie dans le constructeur.
//  Voir la doc du constructeur.
class SFichier;

class Fichier_Lata
{
public:
  enum Mode {ERASE, APPEND};
  Fichier_Lata(const char * basename, const char * extension,
               Mode mode_append,
               Format_Post_Lata::Format format,
               Format_Post_Lata::Options_Para parallel);
  virtual ~Fichier_Lata();
  virtual SFichier& get_SFichier();
  virtual const Nom& get_filename() const;
  virtual int is_master() const;
  virtual void syncfile();
protected:
  Nom filename_;
  SFichier * fichier_;
  int     is_parallel_;
};

// .DESCRIPTION        :
//  Specialisation du Fichier_Lata pour le fichier maitre: toujours en ASCII.
//  On peut utiliser une precision differente si on veut
class Fichier_Lata_maitre : public Fichier_Lata
{
public:
  Fichier_Lata_maitre(const char * basename, const char * extension,
                      Mode mode_append,
                      Format_Post_Lata::Options_Para parallel);
};

#endif
