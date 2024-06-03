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

#ifndef SFichier_included
#define SFichier_included

#include <Sortie_Fichier_base.h>

/*! @brief Cette classe est a la classe C++ ofstream ce que la classe Sortie est a la classe C++ ostream Elle redefinit de facon virtuelle les operateurs d'ecriture dans un fichier
 *
 * @sa EFichier
 */

class SFichier :  public Sortie_Fichier_base
{
  Declare_instanciable(SFichier);
public:
  inline SFichier(const char* name, IOS_OPEN_MODE mode=ios::out):Sortie_Fichier_base(name,mode)
  {
    if (Process::me()>1)
      {
        Cerr << "Error! You can't use SFichier to open the file " << name << " on several processes! Only on the master process." << finl;
        Process::exit();
      }
  };

  SFichier(const SFichier& ) = default;
  SFichier& operator=(const SFichier& f) = default;
};

#endif /* SFichier_included */
