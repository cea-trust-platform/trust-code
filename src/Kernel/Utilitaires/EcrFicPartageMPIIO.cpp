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
// File:        EcrFicPartageMPIIO.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/35
//
//////////////////////////////////////////////////////////////////////////////

#include <EcrFicPartageMPIIO.h>
#include <Statistiques.h>
#include <OBuffer.h>
#include <PE_Groups.h>
#include <Comm_Group.h>
#include <communications.h>
#include <Comm_Group_MPI.h>
#include <string>

extern Stat_Counter_Id IO_EcrireFicPartageMPIIO_counter_;

Implemente_instanciable_sans_constructeur_ni_destructeur(EcrFicPartageMPIIO,"EcrFicPartageMPIIO",SFichier);

#ifdef MPI_
static void handle_error(int errcode, const char *str)
{
  char msg[MPI_MAX_ERROR_STRING];
  True_int resultlen;
  MPI_Error_string(errcode, msg, &resultlen);
  Cerr << "======================" << finl;
  Cerr << str << ": " << msg << finl;
  Cerr << "Contact TRUST support." << finl;
  Process::exit();
}
#define MPI_CHECK(fn) { int errcode; errcode = fn;\
     if (errcode != MPI_SUCCESS) handle_error(errcode, #fn); }
#endif

Entree& EcrFicPartageMPIIO::readOn(Entree& s)
{
  throw;
  return s;
}

Sortie& EcrFicPartageMPIIO::printOn(Sortie& s) const
{
  throw;
  return s;
}

EcrFicPartageMPIIO::EcrFicPartageMPIIO() : SFichier()
{
  bin_ = 1; // Binaire par defaut
#ifdef MPI_
  mpi_file_=NULL;
#endif
}
EcrFicPartageMPIIO::~EcrFicPartageMPIIO()
{
  close();
}
#ifdef MPI_
EcrFicPartageMPIIO::EcrFicPartageMPIIO(const char* name,IOS_OPEN_MODE mode)
{
  ouvrir(name, mode);
}
int EcrFicPartageMPIIO::ouvrir(const char* name,IOS_OPEN_MODE mode)
{
  MPI_Comm mpi_comm;
  if (sub_type(Comm_Group_MPI,PE_Groups::current_group()))
    mpi_comm = ref_cast(Comm_Group_MPI,PE_Groups::current_group()).get_mpi_comm();
  else
    mpi_comm = MPI_COMM_WORLD;
  int MPI_OPEN = mode == ios::app ? MPI_MODE_APPEND : MPI_MODE_CREATE;
  int MPI_MODE = (mode == ios::app || ios::out) ? MPI_MODE_WRONLY : MPI_MODE_RDONLY;
  MPI_CHECK(MPI_File_open(mpi_comm, (char*)name, MPI_OPEN|MPI_MODE, MPI_INFO_NULL, &mpi_file_));
  // Set MPI errors fatal:
  MPI_File_set_errhandler(mpi_file_,MPI_ERRORS_ARE_FATAL);
  // Set initial displacement:
  disp_=0;
  // Ecriture d'un entete specifique pour le build int64 ?
  if (mode==ios::out)
    {
      if (je_suis_maitre())
        {
          Nom marq("INT64");
#ifdef INT_is_64_
          (*this)<<marq.getChar();
#endif
        }
    }
  /*
  else
    {
      Cerr<<"not coded in EcrFicPartageMPIIO::ouvrir"<<finl;
      exit();
    }*/
  return 1;
}
void EcrFicPartageMPIIO::close()
{
  if (mpi_file_) MPI_File_close(&mpi_file_);
  disp_=0;
  return;
}

void EcrFicPartageMPIIO::check()
{
  if (Process::me()>0)
    {
      Cerr << "Only master process can call EcrFicPartageMPIIO::operator <<(...)" << finl;
      exit();
    }
  MPI_Offset offset; // Relative position
  MPI_File_get_position(mpi_file_, &offset);
  MPI_Offset disp;   // Absolute position
  MPI_File_get_byte_offset(mpi_file_, offset, &disp);
  if (disp_!=disp)
    {
      Cerr << "Error in EcrFicPartageMPIIO::check()" << finl;
      Cerr << "Decalage : disp_= " << (int)disp_ << " disp=" << (int)disp << finl;
      Cerr << "Contact TRUST support." << finl;
    }
}

// Function write used in all the operator<< to avoid to duplicate the code
void EcrFicPartageMPIIO::write(MPI_Datatype MPI_TYPE, const void* ob)
{
  True_int size;
  MPI_Type_size(MPI_TYPE, &size);
  MPI_File_write(mpi_file_, (void*)ob, 1, MPI_TYPE, &mpi_status_);
  disp_+=size;
  check();
}

Sortie& EcrFicPartageMPIIO::operator <<(const Separateur& ob)
{
  if (bin_)
    {
      // On n'ecrit pas les separateurs. Voir Sortie::operator<<(const Separateur& ob)
      check();
    }
  else
    {
      if (ob.get_type() == ob.ENDL)
        (*this)<<"\n";
      else if (ob.get_type() == ob.SPACE)
        (*this)<<" ";
      else
        {
          Cerr << "Unknown separator in EcrFicPartageMPIIO::operator <<" << finl;
          Process::exit();
        }
    }
  return *this;
}

Sortie& EcrFicPartageMPIIO::operator <<(const char* ob)
{
  int size=strlen(ob) + 1;
  //int size=strlen(ob);
  for (int i=0; i<size; i++)
    write(MPI_CHAR, &ob[i]);
  return *this;
}

Sortie& EcrFicPartageMPIIO::operator <<(const std::string& str)
{
  return (*this) << str.c_str();
}

Sortie& EcrFicPartageMPIIO::operator <<(const int ob)
{
  if (bin_)
    {
#ifdef INT_is_64_
      write(MPI_LONG, &ob);
#else
      write(MPI_INT, &ob);
#endif
    }
  else
    (*this)<<std::to_string(ob).c_str();
  return *this;
}
Sortie& EcrFicPartageMPIIO::operator <<(const unsigned ob)
{
  if (bin_)
    write(MPI_UNSIGNED, &ob);
  else
    (*this)<<std::to_string(ob).c_str();
  return *this;
}

Sortie& EcrFicPartageMPIIO::operator <<(const float ob)
{
  if (bin_)
    write(MPI_FLOAT, &ob);
  else
    (*this)<<std::to_string(ob).c_str();
  return *this;
}
Sortie& EcrFicPartageMPIIO::operator <<(const double ob)
{
  if (bin_)
    write(MPI_DOUBLE, &ob);
  else
    (*this)<<std::to_string(ob).c_str(); // ToDo utiliser sprintf
  return *this;
}
Sortie& EcrFicPartageMPIIO::operator <<(const Objet_U& ob)
{
  ob.printOn(*this);
  check();
  return *this;
}

// Function used for different MPI_Type
// Good info/examples on MPI-IO:
// See page 213: http://www.idris.fr/data/cours/parallel/mpi/IDRIS_MPI_cours_couleurs.pdf
// See http://www.cac.cornell.edu/ranger/mpiadvtopics/fileptroffs.aspx
// Default file view has etype=filetype=MPI_Byte and disp=0
// MPI_File_get_position returns a relative offset in etype units of the current view
// MPI_File_get_byte_offset converts a view relative offset (etype units) into an absolute byte position
// MPI_File_set_view(file,disp,etype,...) disp should be specified in absolute bytes from the start of the file
// sizeof(MPI_DOUBLE)=4 !!! Should use:MPI_Type_size( MPI_DOUBLE, &size ) to return 8 !!!
// Ecriture d'une zone memoire pointee par ob contenant n MPI_TYPE :
int EcrFicPartageMPIIO::put(MPI_Datatype MPI_TYPE, const void* ob, int n)
{
  MPI_Datatype etype;
  etype=MPI_TYPE;
  True_int sizeof_etype;
  MPI_Type_size(etype,&sizeof_etype);

  // filetype is n etype contigous:
  MPI_Datatype filetype;
  MPI_Type_contiguous(n, etype, &filetype);
  MPI_Type_commit(&filetype);

  // Before collecting operations, update disp_ on all processes:
  envoyer_broadcast(disp_, 0);
  MPI_Offset disp_me = disp_ + mppartial_sum(n) * sizeof_etype;
  // ROMIO hints:
  if (Process::nproc()>1024)
    {
      char* ROMIO_HINTS=getenv("ROMIO_HINTS");
      if (ROMIO_HINTS==NULL)
        {
          Cerr << "Warning, no ROMIO_HINTS detected on your massive parallel calculation." << finl;
          Cerr << "Performances of MPI I/O could be improved with ROMIO hints." << finl;
          Cerr << "Contact TRUST support." << finl;
        }
    }

  // Create info for using hints:
  MPI_Info mpi_info;
  MPI_Info_create(&mpi_info);
  // Example (not used cause machine dependant):
  /*
  MPI_Info_set(mpi_info, "romio_cb_read", "enable");
  MPI_Info_set(mpi_info, "romio_cb_write", "enable");
  // Un processus par node s'occupe du buffering:
  MPI_Info_set(mpi_info, "cb_config_list", "*:1");

  // Striping count (max 160 sur LUSTRE):
  Nom striping_factor=(Nom)min(Process::nproc(),160);
  MPI_Info_set(mpi_info, "striping_factor", (char*)striping_factor.getChar());

  // Striping units in bytes (ideal is stripe size = I/O operations):
  Nom striping_unit=(Nom)(n * sizeof_etype);
  MPI_Info_set(mpi_info, "striping_unit", (char*)striping_unit.getChar());
  */

  // Set a new view:
  MPI_File_set_view(mpi_file_, disp_me, etype, filetype, (char*)"native", mpi_info);

  // Write all:
  statistiques().begin_count(IO_EcrireFicPartageMPIIO_counter_);
  MPI_File_write_all(mpi_file_, (void*)ob, n, etype, MPI_STATUS_IGNORE);
  statistiques().end_count(IO_EcrireFicPartageMPIIO_counter_, n * sizeof_etype);

  // Update the position of the pointer file:
  disp_+=mp_sum(n) * sizeof_etype;

  // Reset a simple view at this new position:
  MPI_File_set_view(mpi_file_, disp_, MPI_BYTE, MPI_BYTE, (char*)"native", MPI_INFO_NULL);

  // Free
  MPI_Info_free(&mpi_info);
  MPI_Type_free(&filetype);
  return 1;
}

#endif
