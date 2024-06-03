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

#include <Entree_Fichier_base.h>
#include <Process.h>
#include <EntreeSortie.h>
#include <Nom.h>

Implemente_base_sans_constructeur_ni_destructeur(Entree_Fichier_base,"Entree_Fichier_base",Objet_U);

Entree& Entree_Fichier_base::readOn(Entree& s)
{
  throw;
}

Sortie& Entree_Fichier_base::printOn(Sortie& s) const
{
  throw;
}
Entree_Fichier_base::Entree_Fichier_base()
{
  ifstream_=0;
}

Entree_Fichier_base::Entree_Fichier_base(const char* name,IOS_OPEN_MODE mode)
{
  ifstream_ = new ifstream(name, mode);
  if(ifstream_->fail())
    {
      Cerr << "Error while opening the file " << name << finl;
      Process::exit();
    }
  set_istream(ifstream_);
}

ifstream& Entree_Fichier_base::get_ifstream()
{
  return *ifstream_;
}

Entree_Fichier_base::~Entree_Fichier_base()
{
  Entree_Fichier_base::close();
}

int Entree_Fichier_base::ouvrir(const char* name, IOS_OPEN_MODE mode)
{
  if(ifstream_)
    delete ifstream_;
  IOS_OPEN_MODE ios_mod=mode;
  if (bin_)
    {
      ios_mod=ios_mod|ios::binary;
    }
  ifstream_ = new ifstream(name,ios_mod);
  int ok = ifstream_->good();
  set_istream(ifstream_);
#ifdef INT_is_64_
  if (bin_)
    {
      Nom test;
      (*this) >> test;
      if (test!="INT64")
        {
          Cerr<<"Opening " <<name<< " which is an int32 binary file..."<<finl;
          delete ifstream_;
          // on recharge le fichier
          ifstream_ = new ifstream(name,ios_mod);
          is_different_int_size_=true;
          ok = ifstream_->good();
          set_istream(ifstream_);
        }
    }
#else
  if (bin_)
    {
      Nom test;
      (*this) >> test;
      if (test=="INT64")
        {
          Cerr<<"Opening " << name<< " which is an int64 binary file..."<<finl;
          is_different_int_size_=true;
        }
      else
        {
          delete ifstream_;
          // on recharge le fichier
          ifstream_ = new ifstream(name,ios_mod);
          ok = ifstream_->good();
          set_istream(ifstream_);
        }
    }
#endif
  return ok;
}

void Entree_Fichier_base::close()
{
  if(ifstream_)
    {
      ifstream_->close();
      // on ne detruit pas ifstream ca sera fait par entree
      //      delete ifstream_;
    }
}

int Entree_Fichier_base::eof()
{
  if(ifstream_)
    return ifstream_->eof();
  else
    return -1;
}

int Entree_Fichier_base::fail()
{
  if(ifstream_)
    return ifstream_->fail();
  else
    return -1;
}

int Entree_Fichier_base::good()
{
  if(ifstream_)
    return ifstream_->good();
  else
    return -1;
}

void Entree_Fichier_base::precision(int pre)
{
  if(ifstream_)
    ifstream_->precision(pre);
}

void Entree_Fichier_base::setf(IOS_FORMAT code)
{
  if(ifstream_)
    ifstream_->setf(code);
}

/*! @brief Return True if the file can be opened for reading, false otherwise.
 *
 */
bool Entree_Fichier_base::Can_be_read(const char * name)
{
  std::ifstream ifs(name,ios::in);
  return ifs.good();
}

