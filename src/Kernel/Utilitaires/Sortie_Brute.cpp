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
#include <Sortie_Brute.h>
#include <Process.h>
#include <EntreeSortie.h>
#include <sstream>

using std::ostringstream;

// In its implementation use an ostringstream. This is by far the simplest method.
Sortie_Brute::Sortie_Brute() :
  Sortie()
{
  set_bin(1);
  ostringstream *os = new ostringstream();  // deletion in Sortie destructor
  set_ostream(os);
}

Sortie_Brute::~Sortie_Brute()
{
}

// Description:
//   returns a pointer to the internal data. The data is valid as long as no other write operation is done on
//   the Sortie_Brute object.
const char* Sortie_Brute::get_data() const
{
  const ostringstream& os = static_cast< const ostringstream& >(get_ostream());
  string_ = std::string(os.str());
  return string_.c_str();
}

unsigned Sortie_Brute::get_size() const
{
  const ostringstream& os = static_cast< const ostringstream& >(get_ostream());
  return os.str().size();
}

int Sortie_Brute::set_bin(int bin)
{
  if (bin != 1)
    {
      Cerr << "Error in Sortie_Brute::set_bin(int bin) : only binary format supported. Use SChaine otherwise." << finl;
      Process::exit();
    }
  Sortie::set_bin(bin);
  return bin;
}
