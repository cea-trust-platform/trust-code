/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Mkdir.h>
#include <Nom.h>
#include <sys/stat.h>

Implemente_instanciable(Mkdir,"Mkdir",Interprete);
// XD mkdir interprete mkdir 0 equivalent to system mkdir
// XD attr directory chaine directory 0 directory to create

Sortie& Mkdir::printOn(Sortie& os) const { return Interprete::printOn(os); }

Entree& Mkdir::readOn(Entree& is) { return Interprete::readOn(is); }

/*! @brief Lit le nom d'un objet dans is Ecrit l'objet correspondant sur la sortie standard
 *
 * @param (Entree& is)
 * @return (Entree&)
 */
Entree& Mkdir::interpreter(Entree& is)
{
  Nom nom1;
  is >> nom1;
  if (je_suis_maitre())
    {
      int status=mkdir(nom1.getChar(),0777);
      if (status!=0)
        {
          if (errno!=EEXIST)
            {
              Cerr<<"Error on the creation of directory "<<nom1<<" "<<strerror(errno)<<finl;
              exit();
            }
        }
    }
  return is;
}
