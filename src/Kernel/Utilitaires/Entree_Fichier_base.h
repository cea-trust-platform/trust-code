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
// File:        Entree_Fichier_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Entree_Fichier_base_included
#define Entree_Fichier_base_included


#include <Entree.h>
#include <Objet_U.h>
#include <fstream>
using std::ifstream;
using std::ofstream;
using std::streampos;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Fichier en lecture
//    Cette classe est a la classe C++ ifstream ce que la classe Entree est a la
//    classe C++ istream. Elle redefinit de facon virtuelle les operateurs de lecture dans un fichier.
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////

class Entree_Fichier_base : public Entree, public Objet_U
{
  Declare_base_sans_constructeur(Entree_Fichier_base);
public:
  Entree_Fichier_base();
  Entree_Fichier_base(const char* name,IOS_OPEN_MODE mode=ios::in);
  ifstream& get_ifstream();
  void close();


  virtual int ouvrir(const char* name, IOS_OPEN_MODE mode=ios::in);

  int eof();
  int fail();
  int good();
  void precision(int pre);
  void setf(IOS_FORMAT code);


  ifstream* ifstream_;

private:
  Entree_Fichier_base& operator=(const Entree_Fichier_base&);

};

#endif
