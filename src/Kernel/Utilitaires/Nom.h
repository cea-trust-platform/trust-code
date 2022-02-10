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
// File:        Nom.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Nom_included
#define Nom_included

#include <Objet_U.h>
#include <string.h>
#include <string>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     class Nom
//     Une chaine de caractere pour nommer les objets de TRUST
// .SECTION voir aussi
//      Motcle
//////////////////////////////////////////////////////////////////////////////

class Nom : public Objet_U
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Nom);
public:
  Nom();                                         // construit "??"
  Nom(int i);                                // Nom(485)="485"
  Nom(const char* nom);
  Nom(const Nom&);
  Nom(char c);
  Nom(double );
  Nom(double le_reel, const char* format);
  ~Nom() override;
  virtual int find(const char * n) const;

  operator const char*() const;
  Nom&   majuscule();
  int est_egal_a(const Objet_U&) const override;
  int longueur() const ;
  static int selftest();
  const Nom& le_nom() const override;

  Nom& operator=(const char* const);
  Nom& operator=(const Nom&);
  Nom operator +(const Nom&) const;
  Nom& operator +=(const Nom& x);
  Nom& operator +=(const char *);
  Nom& operator +=(char x);
  Nom& suffix(const char* const) ;
  const Nom getSuffix(const char* const) const ;
  Nom& prefix(const char* const) ;
  const Nom getPrefix(const char* const) const ;
  Nom nom_me(int, const char* prefix=0, int without_padding=0) const;
  Nom substr_old(const int, const int) const;
  Nom basename() const;
  virtual int debute_par(const char* const) const;
  friend int operator ==(const Nom& , const char* const) ;
  friend int operator !=(const Nom& , const char* ) ;
  friend int operator ==(const Nom& , const Nom&) ;
  friend int operator !=(const Nom& , const Nom& ) ;
  friend int operator ==(const char* const, const Nom&) ;
  friend int operator !=(const char* const, const Nom&) ;
  virtual int finit_par(const char* const) const;
  inline bool contient(const Nom& nom)
  {
    std::size_t found = getString().find(nom.getString());
    return found!=std::string::npos;
  }
  inline const char* getChar() const
  {
    return nom_.c_str();
  }
  inline const std::string& getString() const
  {
    return nom_;
  }
  inline std::string& getString()
  {
    return nom_;
  }
  static void sed_check_case_non_sensitive(int i) ;
protected :

  static int nb_noms;
  std::string nom_;

  static int check_case_non_sensitive_; // pour descativer dans ==
};

char intochar(int i);

// Conversion en majuscule
inline void convertit_en_majuscule(char* s)
{
  const char decal='a'-'A';
  char c;
  do
    {
      c = *s;
      if (c >= 'a' && c <= 'z')
        *s = c - decal;
      s++;
    }
  while (c);
}
#endif // NOM_H
