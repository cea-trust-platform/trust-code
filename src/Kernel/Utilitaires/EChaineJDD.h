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

#ifndef EChaineJDD_included
#define EChaineJDD_included

#include <Entree.h>
#include <sstream>
using std::istringstream;

/*! @brief
 * Same as EChaine except here the input string comes from a datafile.
 * Keeps a track of the lines that have been read in the datafile so far,
 * so in case of a TRUST crash, we can get the line in the datafile where the error occured
 */
class EChaineJDD : public Entree
{
public:

  EChaineJDD();
  EChaineJDD(const char* str);
  ~EChaineJDD() override;
  void init(const char *str);

  using Entree::operator>>;
  using Entree::get;
  Entree& operator>>(int& ob) override;
  Entree& operator>>(double& ob) override;
  int get(char *ob, int bufsize) override;

  void set_track_lines(bool b) { track_lines_ = b ;}

  static int file_cur_line_;

protected:
  istringstream* istrstream_;
  bool track_lines_;

  template <typename _TYPE_>
  Entree& operator_template(_TYPE_& ob);
};


template <typename _TYPE_>
Entree& EChaineJDD::operator_template(_TYPE_& ob)
{
  assert(istrstream_!=0);
  char buffer[100];
  int ok = get(buffer, 100); // get of this class
  if (ok)
    convert_to(buffer, ob);
  return *this;
}

#endif


