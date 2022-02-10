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
//////////////////////////////////////////////////////////////////////////////
//
// File:        EcrFicPartageMPIIO.h
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/17
//
//////////////////////////////////////////////////////////////////////////////

#ifndef EcrFicPartageMPIIO_included
#define EcrFicPartageMPIIO_included

#include <Nom.h>
#include <SFichier.h>
#include <comm_incl.h>
class Objet_U;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Class to use MPI-IO to write in a single file
//
//////////////////////////////////////////////////////////////////////////////

class EcrFicPartageMPIIO : public SFichier
{
  // Parallel write in a shared file with MPIIO
  Declare_instanciable_sans_constructeur_ni_destructeur(EcrFicPartageMPIIO);
public:
  EcrFicPartageMPIIO();
  ~EcrFicPartageMPIIO() override;
#ifdef MPI_
  EcrFicPartageMPIIO(const char* name,IOS_OPEN_MODE mode=ios::out);
  int ouvrir(const char* name,IOS_OPEN_MODE mode=ios::out) override;
  void close();
  // Useless with MPIIO:
  inline Sortie& lockfile() override
  {
    return *this;
  };
  inline Sortie& unlockfile() override
  {
    return *this;
  };
  inline Sortie& syncfile() override
  {
    return *this;
  };
  inline Sortie& flush() override
  {
    return *this;
  };

  // Check method when using some public methods:
  void check();

  Sortie& operator <<(const Separateur& ob) override;
  Sortie& operator <<(const int& ob) override;
  Sortie& operator <<(const unsigned& ob) override;
#ifndef INT_is_64_
  Sortie& operator <<(const long& ob) override
  {
    Cerr << "EcrFicPartageMPIIO::operator<<(...) not implemented." << finl;
    exit();
    return *this;
  };
#endif
  Sortie& operator <<(const float& ob) override;
  Sortie& operator <<(const double& ob) override;
  Sortie& operator <<(const Objet_U& ob) override;
  Sortie& operator <<(const char* ob) override;
  int put(const unsigned* ob, int n, int pas) override
  {
    return put(MPI_UNSIGNED, ob, n);
  }
  int put(const int* ob, int n, int pas) override
  {
#ifdef INT_is_64_
    return put(MPI_LONG, ob, n);
#else
    return put(MPI_INT, ob, n);
#endif
  }
#ifndef INT_is_64_
  int put(const long* ob, int n, int pas) override
  {
    Cerr << "EcrFicPartageMPIIO::put(...) not implemented." << finl;
    exit();
    return 1;
  };
#endif
  int put(const float* ob, int n, int pas) override
  {
    return put(MPI_FLOAT, ob, n);
  }
  int put(const double* ob, int n, int pas) override
  {
    return put(MPI_DOUBLE, ob, n);
  }

private:
  void write(MPI_Datatype, const void*);
  int put(MPI_Datatype, const void*, int);
  MPI_File mpi_file_;
  MPI_Status mpi_status_;
  MPI_Offset disp_; // Displacement of the individual pointers (in bytes)
#endif
};
#endif
