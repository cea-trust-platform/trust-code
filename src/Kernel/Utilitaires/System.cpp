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

#include <System.h>

Implemente_instanciable(System,"System",Interprete);


// printOn et readOn

Sortie& System::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& System::readOn(Entree& is )
{
  //
  //
  return is;
}

Entree& System::interpreter(Entree& is)
{
  Nom instruction,suite;
  is >> instruction;
  if (instruction[0]!='"')
    {
      Cerr<<"In System, instruction should begin by \""<<finl;
      Cerr<<"instruction beginning "<<instruction;
      exit();
    }
  instruction.suffix("\"");
  while(instruction[instruction.longueur()-2]!='"')
    {
      is >> suite;
      assert(is.good());
      instruction+=" ";
      instruction+=suite;
    }
  instruction.prefix("\"");


  if (je_suis_maitre())
    {
      Cerr<<"Shell command executed: " << instruction<<finl;
      int error = system(instruction);
      if (error)
        {
          Cerr << "The shell command returns an error: " << error << finl;
          Cerr << "Check your data file." << finl;
          exit();
        }
    }
  barrier();
  return is;
}
