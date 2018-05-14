/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Sortie_Fichier_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/22
//
//////////////////////////////////////////////////////////////////////////////

#include <Sortie_Fichier_base.h>
#include <Process.h>
#include <Nom.h>

Implemente_base_sans_constructeur_ni_destructeur(Sortie_Fichier_base,"Sortie_Fichier_base",Objet_U);

Entree& Sortie_Fichier_base::readOn(Entree& s)
{
  throw;
  return s;
}

Sortie& Sortie_Fichier_base::printOn(Sortie& s) const
{
  throw;
  return s;
}
Sortie_Fichier_base::Sortie_Fichier_base()
{
  bin_=0;
  ofstream_=0;
  set_ostream(ofstream_);
  internalBuff_=0;
  set_toFlush();
}

Sortie_Fichier_base::Sortie_Fichier_base(const char* name, IOS_OPEN_MODE mode)
{
  bin_=0;
  internalBuff_=0;
  ofstream_ = 0;
  ouvrir(name,mode);
  /*
  new ofstream(name,mode);
  set_toFlush();
  set_buffer();
  set_ostream(ofstream_);
  #ifdef ver_file
  if (Process::me()!=0)
    {
      Cerr<<Process::me()<<name<<" :"<<__FILE__<<(int)__LINE__<<finl;
      ::abort();
    }
  #endif
  */
}

Sortie_Fichier_base::~Sortie_Fichier_base()
{
  // il est important de ne pas detruire ofstream_
  Sortie_Fichier_base::close();
}

void Sortie_Fichier_base::set_toFlush()
{
  char* theValue = getenv("TRIOU_FLUSHFILES");
  // On fixe une valeur a 0 par defaut pour toFlush_
  // Elle peut etre changee par la variable d'environnement
  // On bufferize sur DEC pour accelerer les ecritures .lml, .lata, ...
#ifdef DECalpha
  toFlush_ = 0;
#else
  toFlush_ = 1;
#endif
  // Les acces disques ont ete optimises depuis
  // donc par defaut on flushe a nouveau car
  // sinon les sondes pas ecrites...
  // Si lent, mettre export TRIOU_FLUSHFILES=0 dans son sub_file
  toFlush_ = 1;
  if (theValue != NULL)
    {
      toFlush_=atoi(theValue);
    }
}

void Sortie_Fichier_base::set_buffer()
{
  if (!toFlush_)
    {
      char* theValue = getenv("TRIOU_BUFFSIZE");
      // On fixe une valeur a 3000000 par defaut pour buffSize
      // Elle peut etre changee par la variable d'environnement
      int buffSize = 3000000;
      if (theValue != NULL)
        {
          buffSize = atoi(theValue);
        }
      if (buffSize>=0)
        {
          size_t internalBuffSize = buffSize;
          internalBuff_ = new char[internalBuffSize];
#if defined(_COMPILE_AVEC_GCC_)
          ofstream_->rdbuf()->setbuf(internalBuff_,internalBuffSize);
#else
          ofstream_->rdbuf()->pubsetbuf(internalBuff_,internalBuffSize);
#endif
          toFlush_ = 0;
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
      //  Destruction faite dans ~Sortie :
      //  delete ofstream_;
      //  ofstream_=0;
    }
  if (internalBuff_)
    {
      delete[] internalBuff_;
      internalBuff_=0;
    }
}

void Sortie_Fichier_base::precision(int pre)
{
  if(ofstream_)
    ofstream_->precision(pre);
}

int Sortie_Fichier_base::get_precision()
{
  return ofstream_->precision();
}

void Sortie_Fichier_base::setf(IOS_FORMAT code)
{
  if(ofstream_)
    ofstream_->setf(code);
}

int Sortie_Fichier_base::ouvrir(const char* name,IOS_OPEN_MODE mode)
{
#ifdef ver_file
  if (Process::me()!=0)
    {

      char* marq=strrchr(name,'_');
      int error=0;
      if (!marq)
        error=1;
      else
        {
          Nom test(name);
          test.prefix(marq);
          Nom suf(name);
          marq=strrchr(name,'.');
          if (!marq)
            error=1;
          else
            {
              int l=strlen(name);
              int deb=l-strlen(marq)+1;
              suf=suf.substr(deb,l);
              test+=suf;
              test=test.nom_me(Process::me());
              if (test!=name) error=1;
            }


        }
      if (error)
        {
          Cerr<<Process::me()<<name<<" strange :"<<__FILE__<<(int)__LINE__<<finl;
          Cerr<<name<<finl;
          ::abort();
        }


    }
#endif
  if(ofstream_)
    delete ofstream_;
  IOS_OPEN_MODE ios_mod=mode;
  int new_bin=0;
  if (bin_)
    {

      if (ios_mod==ios::out)
        {
          new_bin=1;
        }
      else
        assert((ios_mod==ios::app)||(ios_mod==(ios::app|ios::out)));
      ios_mod=ios_mod|ios::binary;
    }
  ofstream_ = new ofstream(name,ios_mod);
  set_toFlush();
  set_buffer();
  set_ostream(ofstream_);
  if (!ofstream_->good())
    {
      Cerr << "Error when opening the file " << name << finl;
      Cerr << "Check if this file can be opened." << finl;
      Process::exit();
    }

  if (new_bin)
    {
      Nom marq("INT64");
#ifdef INT_is_64_
      (*this)<<marq;
#endif
    }
  return 1;
}

// Description:
//    Force l'ecriture sur disque des donnees dans le tampon
//    Utilise l'implementation de la classe ofstream
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: *this
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Sortie_Fichier_base::flush()
{
  if (toFlush()) ofstream_->flush();
  return *this;
}
bool Sortie_Fichier_base::is_open()
{

  return (ofstream_&&get_ofstream().is_open()) ;
}


