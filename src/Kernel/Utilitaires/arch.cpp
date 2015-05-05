/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        arch.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#include <arch.h>
#include <iostream>
#include <unistd.h>
using std::cerr;
using std::endl;
#ifdef MICROSOFT
#include <math.h>
#include <direct.h>
void srand48(int s)
{
  srand(s);
  //cerr<< "srand48 not implemented with Visual "<<endl;
  //Process::exit();
}
double drand48()
{
  return double(rand())/RAND_MAX;
  // cerr<< "drand48 not implemented with Visual "<<endl;Process::exit();return 1.;
}
double atanh(const double& x)
{
  // must be x>-1, x<1, if not return Nan (Not a Number)
  if(!(x>-1.0 && x<1.0)) return sqrt(-1.0);
  return log((1.0+x)/(1.0-x))/2.0;
  // cerr<< "atanh not implemented with Visual "<<endl;Process::exit(); return (x);
}
#endif
#include <Process.h>

static char* buf=0;  //new char[801];
// pwd :
// alloue une chaine!
void desalloue_pwd()
{
  // si on a alloue le buf on le detruit ...
#if defined(_CSH_) || defined(linux) || defined(cygwin) || defined(MICROSOFT)
  if (buf)
    delete [] buf;
  buf=0;
#endif
}
char * pwd()
{
#if defined(_CSH_) || defined(linux) || defined(cygwin) || defined(MICROSOFT)
  if (buf==0)
    buf=new char[801];
#endif
#ifdef MICROSOFT
  int longueur=800;
  _getcwd(buf, longueur);
#else
#if defined(_CSH_) || defined(linux) || defined(cygwin)
  int longueur=800;
  if (getcwd(buf, longueur)==NULL)
    {
      cerr << "getcwd returns an error code." << endl;
      Process::exit();
    };
#else
  buf=getenv("PWD");
#endif
#endif
  return buf;
}

