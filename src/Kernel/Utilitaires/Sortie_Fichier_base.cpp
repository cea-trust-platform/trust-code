/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Sortie_Fichier_base.h>
#include <fstream>
#include <Process.h>
#include <Nom.h>
#include <map>
#include <string>
#include <stdlib.h>
#include <sys/stat.h>

Implemente_base_sans_constructeur_ni_destructeur(Sortie_Fichier_base,"Sortie_Fichier_base",Objet_U);

Entree& Sortie_Fichier_base::readOn(Entree& s)
{
  throw;
}

Sortie& Sortie_Fichier_base::printOn(Sortie& s) const
{
  throw;
}

Sortie_Fichier_base::Sortie_Fichier_base() : Sortie()
{
  set_toFlush();
}

Sortie_Fichier_base::Sortie_Fichier_base(const char* name, IOS_OPEN_MODE mode) : Sortie()
{
  ouvrir(name,mode);
}

Sortie_Fichier_base::~Sortie_Fichier_base()
{
  close();
}

void Sortie_Fichier_base::set_toFlush()
{
  // Les acces disques ont ete optimises depuis
  // donc par defaut on flushe a nouveau car
  // sinon les sondes pas ecrites...
  // Si lent, mettre export TRIOU_FLUSHFILES=0 dans son sub_file
  toFlush_ = 1;
  char* theValue = getenv("TRUST_FLUSHFILES");
  if (theValue != nullptr)
    {
      toFlush_=atoi(theValue);
    }
}

void Sortie_Fichier_base::set_buffer()
{
  if (!toFlush_)
    {
      char* theValue = getenv("TRUST_BUFFSIZE");
      // On fixe une valeur a 3000000 par defaut pour buffSize
      // Elle peut etre changee par la variable d'environnement
      int buffSize = 3000000;
      if (theValue != nullptr)
        {
          buffSize = atoi(theValue);
        }
      if (buffSize>=0)
        {
          size_t internalBuffSize = buffSize;
          internalBuff_ = new char[internalBuffSize];
          ofstream_->rdbuf()->pubsetbuf(internalBuff_,internalBuffSize);
        }
    }
}

ofstream& Sortie_Fichier_base::get_ofstream()
{
  return *ofstream_;
}

void Sortie_Fichier_base::close()
{
  if(ofstream_)
    {
      ofstream_->flush();
      ofstream_->close();
    }
  if (internalBuff_)
    {
      delete[] internalBuff_;
      internalBuff_ = nullptr;
    }
}

void Sortie_Fichier_base::precision(int pre)
{
  if(ofstream_)
    ofstream_->precision(pre);
}

int Sortie_Fichier_base::get_precision()
{
  return (int)ofstream_->precision();
}

void Sortie_Fichier_base::setf(IOS_FORMAT code)
{
  if(ofstream_)
    ofstream_->setf(code);
}

std::string Sortie_Fichier_base::root = "";
static std::map<std::string, int> counters;
int Sortie_Fichier_base::ouvrir(const char* name,IOS_OPEN_MODE mode)
{
  struct stat sb;
  // Create the root directory if it doesn't exit by the master process:
  if (!root.empty() && stat(root.c_str(), &sb) && Process::je_suis_maitre())
    {
      std::string cmd="mkdir -p ";
      cmd+=root;
      int err = system(cmd.c_str());
      if (err)
        {
          Cerr << "Sortie_Fichier_base::ouvrir: Error while creating " << root << " folder!" << finl;
          Process::exit();
        }
    }
  std::string pathname = root;
  if (!pathname.empty()) pathname+="/";
  pathname += name;
  if (++counters[pathname]%100==0) Cerr << "Warning, file " << pathname << " has been opened/closed " << counters[pathname] << " times..." << finl;
  IOS_OPEN_MODE ios_mod=mode;
  int new_bin=0;
  if (bin_)
    {

      if (ios_mod==ios::out)
        new_bin=1;
      else
        assert((ios_mod==ios::app)||(ios_mod==(ios::app|ios::out)));
      ios_mod=ios_mod|ios::binary;
    }
  ostream_ = std::make_unique<ofstream>(pathname,ios_mod); // will delete any existing ostream_ member
  ofstream_ = static_cast<ofstream *>(ostream_.get()); // the typed version of the pointer for this class.
  set_toFlush();
  set_buffer();
  if (!ofstream_->good())
    {
      Cerr << "Error when opening the file " << pathname << ". File was opened " << counters[pathname] << " time(s) ..." << finl;
      Cerr << "Either:\n you don't have write rights,\n or your filesystem is very slow and multiple file open/close." << finl;
      Cerr << "Contact TRUST support team." << finl;
      Process::exception_sur_exit=2;
      Process::exit();
    }

  if (new_bin)
    {
#ifdef INT_is_64_
      Nom marq("INT64");
      (*this)<<marq;
#endif
    }
  return 1;
}

/*! @brief Force l'ecriture sur disque des donnees dans le tampon Utilise l'implementation de la classe ofstream
 *
 * @return (Sortie&) *this
 */
Sortie& Sortie_Fichier_base::flush()
{
  if (toFlush()) ofstream_->flush();
  return *this;
}

bool Sortie_Fichier_base::is_open()
{
  return (ofstream_&&get_ofstream().is_open()) ;
}

void Sortie_Fichier_base::set_root(const std::string dirname)
{
  root=dirname;
  Cerr << "[IO] Setting output directory to: " << root << finl;
}
