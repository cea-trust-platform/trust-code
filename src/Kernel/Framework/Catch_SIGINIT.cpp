/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Catch_SIGINIT.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <Catch_SIGINIT.h>

Nom Catch_SIGINIT::NOM_DU_CAS_ = "";
int Catch_SIGINIT::NB_CAUGHT_SIGNALS_=0;

void Catch_SIGINIT::signal_callback_handler(True_int signum)
{
  if (NB_CAUGHT_SIGNALS_ < 2)
    {
      if (Process::je_suis_maitre())
        {
          std::cerr << " " << std::endl;
          std::cerr << "=======================================================" << std::endl;
          std::cerr << " " << std::endl;
          std::cerr << "Caught signal " << signum << std::endl;
          std::cerr << "We are trying to finish the simulation correctly ... " << std::endl;
          std::cerr << " " << std::endl;
          std::cerr << "=======================================================" << std::endl;
          std::cerr << " " << std::endl;
        }
    }
  else // force an exit
    {
      if (Process::je_suis_maitre())
        {
          std::cerr << " " << std::endl;
          std::cerr << "=======================================================" << std::endl;
          std::cerr << " " << std::endl;
          std::cerr << "Multiple signals " << signum <<" are caught !" << std::endl;
          std::cerr << "We are forcing the exit of the processors ... " << std::endl;
          std::cerr << " " << std::endl;
          std::cerr << "=======================================================" << std::endl;
          std::cerr << " " << std::endl;
        }
      Process::exit();
    }

  // Terminate TRUST correctly => print 1 in stop file
  Nom command = "echo '1' > ";
  command += NOM_DU_CAS_;
  command += ".stop";

  NB_CAUGHT_SIGNALS_++; // force an exit when nb_caught_signals = 2 (3 tries)

  int err = system(command);
  if (err)
    {
      if (Process::je_suis_maitre())
        std::cerr << "We can not finish the simulation correctly because the stop file " << NOM_DU_CAS_<<".stop is not in the directory !" << std::endl;
      Process::exit();
    }
}
