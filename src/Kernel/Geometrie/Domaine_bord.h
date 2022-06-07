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
#ifndef Domaine_bord_included
#define Domaine_bord_included
#include <Domaine.h>
// .DESCRIPTION
//  Cette classe est un domaine dont les sommets et les elements sont extraits d'un
//  bord d'un domaine source.
//  On construit ce domaine par la methode construire_domaine_bord()
class Domaine_bord : public Domaine
{
  Declare_instanciable(Domaine_bord);
public:
  virtual void construire_domaine_bord(const Domaine& source, const Nom& nom_bord);
  virtual const Domaine&   get_domaine_source() const;
  virtual const Nom&       get_nom_bord_source() const;
  virtual const ArrOfInt& get_renum_som() const;

  static void extraire_domaine_bord(const Domaine& src,
                                    const Nom& nom_bord,
                                    Domaine& dest,
                                    ArrOfInt& renum_som);
protected:
  // Reference au domaine source:
  REF(Domaine) domaine_source_;
  // Nom du bord dans le domaine source:
  Nom bord_source_;
  // Pour chaque sommet de ce domaine, indice du meme sommet dans le domaine source:
  ArrOfInt renum_som_;
};
#endif
