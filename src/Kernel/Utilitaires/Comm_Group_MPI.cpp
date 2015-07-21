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
// File:        Comm_Group_MPI.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/39
//
//////////////////////////////////////////////////////////////////////////////
#include <Comm_Group_MPI.h>
#include <petsc_for_kernel.h>
#include <PE_Groups.h>
#include <stat_counters.h>
#include <Statistiques.h>

Implemente_instanciable_sans_constructeur_ni_destructeur(Comm_Group_MPI,"Comm_Group_MPI",Comm_Group);


#ifdef MPI_
MPI_Status  * Comm_Group_MPI::mpi_status_ = 0;
MPI_Request * Comm_Group_MPI::mpi_requests_ = 0;
int Comm_Group_MPI::mpi_nrequests_ = -1;
int Comm_Group_MPI::mpi_maxrequests_ = -1;
int Comm_Group_MPI::current_msg_size_;
MPI_Comm Comm_Group_MPI::trio_u_world_ = MPI_COMM_WORLD;
// By default, we initialize mpi at statup (see set_must_mpi_initialize())
int Comm_Group_MPI::must_mpi_initialize_ = 1;
// Description:
// Partie non inline du traitement d'erreur mpi.
// Affichage d'un code d'erreur mpi avec MPI_Error_string.
static void mpi_print_error(int error_code)
{
  Cerr << "mpi_error in Comm_Group_MPI : error_code = " << error_code << finl;
  Process::Journal() << "mpi_error in Comm_Group_MPI : error_code = " << error_code << finl;
  int length = 0;
  char message[MPI_MAX_ERROR_STRING];
  MPI_Error_string(error_code, message, & length);
  if (length > 0)
    {
      Cerr << message << finl;
      Process::Journal() << message << finl;
    }
  // Normalement on aurait pris trio_u_world_, mais on n'y a pas acces ici.
  // Pour faire abort, en l'occurence, c'est pas grave mais merci de ne pas
  // prendre comme exemple...
  MPI_Abort(MPI_COMM_WORLD,-1);
  assert(0);
  Process::exit();
}

// Description:
// Partie inline du traitement d'erreur mpi (on inline le
// test: sauf exception, il n'y a pas d'appel de fonction
// supplementaire.
static inline void mpi_error(int error_code)
{
  if (error_code != MPI_SUCCESS)
    mpi_print_error(error_code);
}

#endif

Sortie& Comm_Group_MPI::printOn(Sortie& os) const
{
  exit();
  return os;
}

Entree& Comm_Group_MPI::readOn(Entree& is)
{
  exit();
  return is;
}

// Description: Constructeur par defaut. Il faut ensuite appeler init_group()
//  ou init_group_trio() pour finir la construction du groupe.
Comm_Group_MPI::Comm_Group_MPI()
{
#ifdef MPI_
  // -1 indique que le groupe n'a pas ete initialise
  must_finalize_ = -1;
  mpi_comm_=MPI_COMM_NULL;
  mpi_group_=MPI_GROUP_NULL;
#endif
}

Comm_Group_MPI::~Comm_Group_MPI()
{
#ifdef MPI_
  // Si groupe non initialise, ne rien faire:
  // Modif par BM (20/08/2012): ne detruire ces membres statiques que si c'est le groupe principal (fin de l'execution)
  if ((mpi_comm_!=MPI_COMM_NULL) && (mpi_comm_ == trio_u_world_))
    {
      delete [] mpi_status_;
      mpi_status_=0;

      for (int r=0; r<mpi_maxrequests_; r++)
        {
          if(mpi_requests_[r]!=MPI_REQUEST_NULL)
            {
              MPI_Request_free(&(mpi_requests_[r]));
            }
        }

      delete [] mpi_requests_;
      mpi_requests_=0;
    }

  else // on detruit les groupes non prinicpaux
    {

      if (mpi_comm_!=MPI_COMM_NULL)
        {
          // on detruit le group puis le mpi_comm
          mpi_error(MPI_Comm_free(&mpi_comm_));
          assert(mpi_comm_==MPI_COMM_NULL);
        }
      if (mpi_group_!=MPI_GROUP_NULL)
        {
          mpi_error(MPI_Group_free( &mpi_group_));
          assert(mpi_group_==MPI_GROUP_NULL);
        }
    }

#endif
}

// Description: appel a MPI_Abort et rend la main
void Comm_Group_MPI::abort() const
{
#ifdef MPI_
  MPI_Abort(trio_u_world_,-1);
#endif
}

void Comm_Group_MPI::mp_collective_op(const double *x, double *resu, int n, Collective_Op op) const
{
#ifdef MPI_
  if (n <= 0)
    return;
  switch(op)
    {
      // Note B.M.: cast en non const a cause de l'interface de MPI,
      // plusieurs posts sur newsgroups incitent a penser que c'est ok
      // (x ne sera jamais modifie par la librairie, sauf bug !)
    case COLL_SUM:
      statistiques().begin_count(mpi_sumdouble_counter_);
      mpi_error(MPI_Allreduce((int*) x, resu, n, MPI_DOUBLE, MPI_SUM, mpi_comm_));
      statistiques().end_count(mpi_sumdouble_counter_);
      break;
    case COLL_MIN:
      statistiques().begin_count(mpi_mindouble_counter_);
      mpi_error(MPI_Allreduce((int*) x, resu, n, MPI_DOUBLE, MPI_MIN, mpi_comm_));
      statistiques().end_count(mpi_mindouble_counter_);
      break;
    case COLL_MAX:
      statistiques().begin_count(mpi_maxdouble_counter_);
      mpi_error(MPI_Allreduce((int*) x, resu, n, MPI_DOUBLE, MPI_MAX, mpi_comm_));
      statistiques().end_count(mpi_maxdouble_counter_);
      break;
    case COLL_PARTIAL_SUM:
      internal_collective(x, resu, n, &op, -1 /* only one operation */, 0 /* recursion level */);
      break;
    }
#endif
}

void Comm_Group_MPI::mp_collective_op(const double *x, double *resu, const Collective_Op *op, int n) const
{
#ifdef MPI_
  if (n <= 0)
    return;
  internal_collective(x, resu, n, op, n /* n different operations */, 0 /* recursion level */);
#endif
}

void Comm_Group_MPI::mp_collective_op(const int *x, int *resu, int n, Collective_Op op) const
{
#ifdef MPI_
  if (n <= 0)
    return;
  switch(op)
    {
      // Note B.M.: cast en non const a cause de l'interface de MPI,
      // plusieurs posts sur newsgroups incitent a penser que c'est ok
      // (x ne sera jamais modifie par la librairie, sauf bug !)
    case COLL_SUM:
      statistiques().begin_count(mpi_sumint_counter_);
      mpi_error(MPI_Allreduce((int*) x, resu, n, MPI_INT, MPI_SUM, mpi_comm_));
      statistiques().end_count(mpi_sumint_counter_);
      break;
    case COLL_MIN:
      statistiques().begin_count(mpi_minint_counter_);
      mpi_error(MPI_Allreduce((int*) x, resu, n, MPI_INT, MPI_MIN, mpi_comm_));
      statistiques().end_count(mpi_minint_counter_);
      break;
    case COLL_MAX:
      statistiques().begin_count(mpi_maxint_counter_);
      mpi_error(MPI_Allreduce((int*) x, resu, n, MPI_INT, MPI_MAX, mpi_comm_));
      statistiques().end_count(mpi_maxint_counter_);
      break;
    case COLL_PARTIAL_SUM:
      internal_collective(x, resu, n, &op, -1 /* only one operation */, 0 /* recursion level */);
      break;
    }
#endif
}

void Comm_Group_MPI::mp_collective_op(const int *x, int *resu, const Collective_Op *op, int n) const
{
#ifdef MPI_
  if (n <= 0)
    return;
  internal_collective(x, resu, n, op, n /* n different operations */, 0 /* recursion level */);
#endif
}

// Description:
// Point de synchronisation de tous les processeurs du groupe (permet
// de verifier que tout le monde est la...). Si check_enabled() est
// non nul, on utilise le tag pour verifier que tous les processeurs
// sont bien en train d'attendre le meme tag, sinon c'est une barriere
// simple. Le tag doit verifier 0 <= tag < max_tag (soit 32).
void Comm_Group_MPI::barrier(int tag) const
{
#ifdef MPI_
  static const int max_tag = 32;
  statistiques().begin_count(mpi_barrier_counter_);
  assert(tag >= 0 && tag < max_tag);
  if (check_enabled())
    {
      // On fait la barriere avec des mpmin et mpmax pour verifier
      // que le tag est le meme sur tous les processeurs :
      // ATTENTION : il faut "int" et pas "entier" !!!
      int tag_complet = get_new_tag() * max_tag + tag;
      int min_tag, amax_tag;
      mpi_error(MPI_Allreduce(& tag_complet, & min_tag, 1, MPI_INT, MPI_MIN, mpi_comm_));
      mpi_error(MPI_Allreduce(& tag_complet, & amax_tag, 1, MPI_INT, MPI_MAX, mpi_comm_));
      if (min_tag != tag_complet || amax_tag != tag_complet)
        {
          Cerr << "Error in Comm_Group_MPI::barrier(int tag)\n";
          Cerr << " the tag is not identical on all the processes.\n";
          Cerr << " (Loss of communications synchronisation)." << finl;
          Process::Journal() << "Comm_Group_MPI::barrier\n Error : tag = " << tag << finl;
          assert(0);
          exit();
        }
    }
  else
    {
      // Barriere simple sans le tag :
      mpi_error(MPI_Barrier(mpi_comm_));
    }
  statistiques().end_count(mpi_barrier_counter_);
#endif
}

// Description:
// Demarre l'envoi et la reception des buffers.
// Les buffers doivent rester valide jusqu'au retour de send_recv_finish().
// Le graphe de communication et la taille des buffers doivent etre corrects !
//
// send_list : liste des processeurs (numerotation sur groupe courant) a qui envoyer
// send_size : taille en octets de chaque message
// send_buffers : adresse des donnees a envoyer.
// recv_...  : idem pour les donnees en reception.
//
// Precondition:
//  Il ne doit pas y avoir d'autre communication en cours (send_recv_finic
void Comm_Group_MPI::send_recv_start(const ArrOfInt& send_list,
                                     const ArrOfInt& send_size,
                                     const char * const * const send_buffers,
                                     const ArrOfInt& recv_list,
                                     const ArrOfInt& recv_size,
                                     char * const * const recv_buffers,
                                     TypeHint typehint) const
{
#ifdef MPI_
  statistiques().begin_count(mpi_sendrecv_counter_);
  assert(mpi_nrequests_ < 0);

  const int tag = get_new_tag();
  int i, n;
  mpi_nrequests_ = 0;
  int msg_size = 0;

  int divisor = 0;
  MPI_Datatype datatype = MPI_CHAR;
  assert(sizeof(int) == sizeof(int)); // Sinon il faut changer MPI_INT !!!
  switch(typehint)
    {
    case CHAR:
      divisor = 1;
      break;
    case INT:
      divisor = sizeof(int);
      datatype = MPI_INT;
      break;
    case DOUBLE:
      divisor = sizeof(double);
      datatype = MPI_DOUBLE;
      break;
    default:
      Process::exit();
    }

  // Astuce pour maximiser les chances que ca marche : on declare
  // la reception d'abord et l'envoi ensuite.
  n = recv_list.size_array();
  for (i = 0; i < n; i++)
    {
      int source = recv_list[i];
      int sz   = recv_size[i];
      msg_size += sz;
      assert(source >= 0 && source < nproc());
      assert(mpi_nrequests_ < mpi_maxrequests_);
      assert(sz % divisor == 0);
      assert(mpi_requests_[mpi_nrequests_]==MPI_REQUEST_NULL);
      mpi_error(MPI_Irecv(recv_buffers[i], sz / divisor,
                          datatype,
                          source, tag, mpi_comm_,
                          & mpi_requests_[mpi_nrequests_]));
      mpi_nrequests_++;
    }

  n = send_list.size_array();
  for (i = 0; i < n; i++)
    {
      int dest = send_list[i];
      int sz   = send_size[i];
      msg_size += sz;
      assert(dest >= 0 && dest < nproc());
      assert(mpi_nrequests_ < mpi_maxrequests_);
      assert(sz % divisor == 0);
      mpi_error(MPI_Isend((char*) send_buffers[i], sz / divisor,
                          datatype,
                          dest, tag, mpi_comm_,
                          & mpi_requests_[mpi_nrequests_]));
      mpi_nrequests_++;
    }
  current_msg_size_ = msg_size;
#endif
}

// Description:
//  Attend que l'ensemble des communications lancees par send_recv_start
//  soient finie.
// Precondition:
//  il faut avoir appele send_recv_start avant.
void Comm_Group_MPI::send_recv_finish() const
{
#ifdef MPI_
  assert(mpi_nrequests_ >= 0);
  mpi_error(MPI_Waitall(mpi_nrequests_, mpi_requests_, mpi_status_));
  statistiques().end_count(mpi_sendrecv_counter_, current_msg_size_, mpi_nrequests_);
  /*
  for (int r=0;r<mpi_nrequests_;r++)
    {
      if ( mpi_requests_[r]!=MPI_REQUEST_NULL)
  {
    MPI_Request_free(&(mpi_requests_[r]));
  }
      mpi_requests_[r]=MPI_REQUEST_NULL;
      }
  */
  mpi_nrequests_ = -1;
#endif
}


// Description:
// Envoi blocant.
// Pour etre bien certain que le code est safe, on force
// une communication synchrone pour forcer le blocage en mode check (voir check_enabled()).
// Sinon, on utilise MPI_Send qui est en general non blocant pour les petits messages
// (donc de meilleures performances).
void Comm_Group_MPI::send(int pe, const void *buffer, int size, int tag) const
{
#ifdef MPI_
  statistiques().begin_count(mpi_send_counter_);
  assert(mpi_nrequests_ < 0);
  int dest = pe;
  assert(dest >= 0 && dest < nproc());
  // Probleme: oblige de faire un cast de (const void*) en (void*) a cause
  // du prototype de MPI_Send
  if (check_enabled())
    mpi_error(MPI_Ssend ((void*)buffer, size, MPI_CHAR, dest, tag, mpi_comm_));
  else
    mpi_error(MPI_Send ((void*)buffer, size, MPI_CHAR, dest, tag, mpi_comm_));
  statistiques().end_count(mpi_send_counter_, size);
#endif
}

// Description:
// Reception blocante d'un message.
void Comm_Group_MPI::recv(int pe, void *buffer, int size, int tag) const
{
#ifdef MPI_
  statistiques().begin_count(mpi_recv_counter_);
  assert(mpi_nrequests_ < 0);
  MPI_Status status;
  int source = pe;
  assert(source >= 0 && source < nproc());
  mpi_error(MPI_Recv (buffer, size, MPI_CHAR, source, tag, mpi_comm_, & status));
  statistiques().end_count(mpi_recv_counter_, size);
#endif
}

void Comm_Group_MPI::broadcast(void *buffer, int size, int pe_source) const
{
#ifdef MPI_
  statistiques().begin_count(mpi_bcast_counter_);
  assert(mpi_nrequests_ < 0);
  mpi_error(MPI_Bcast (buffer, size, MPI_CHAR, pe_source, mpi_comm_));
  statistiques().end_count(mpi_bcast_counter_, size);
#endif
}

void Comm_Group_MPI::all_to_all(const void *src_buffer, void *dest_buffer, int data_size) const
{
#ifdef MPI_
  statistiques().begin_count(mpi_alltoall_counter_);
  assert(src_buffer != dest_buffer);
  void * ptr = (void *) src_buffer; // Cast a cause de l'interface de MPI_Alltoall
  mpi_error(MPI_Alltoall(ptr, data_size, MPI_CHAR, dest_buffer, data_size, MPI_CHAR, mpi_comm_));
  statistiques().end_count(mpi_alltoall_counter_, data_size);
#endif
}

#ifdef MPI_

// Description: constructeur du groupe "tous"
//  Attention, ce constructeur ne doit etre appele qu'une seule fois.
//  Le groupe est associe a trio_u_world_
//  Si must_mpi_initialize_==0, on suppose que MPI_Init a deja ete appele.
//  Apres l'appel a init_group_trio, il faut enregistrer le groupe dans PE_Groups
//  Voir PE_Groups::initialize()
void Comm_Group_MPI::init_group_trio()
{
  must_finalize_ = 0;

  if (mpi_status_ != 0)
    {
      Cerr << "Error : the construction of the global Comm_Group_MPI has already been done." << finl;
      exit();
    }

  if (must_mpi_initialize_)
    {
      if (trio_u_world_ != MPI_COMM_WORLD)
        {
          Cerr << "Error in Comm_Group_MPI::init_group_trio(...) : you cannot ask to initialize MPI\n"
               << " with something else than MPI_COMM_WORLD !" << finl;
          exit();
        }
      must_finalize_ = 1;
      int argc=0;
      char** argv=NULL;
      int errcode = MPI_Init(&argc, &argv);
      //int errcode = MPI_Init(0,0); Message d'erreur sur MPI Voltaire
      if (errcode != MPI_SUCCESS)
        {
          Cerr << "Error in Comm_Group_MPI::init_group_trio()\n"
               << " MPI_Init() failed (forget to run with mpirun ?)" << finl;
          mpi_error(errcode);
        }
    }

  int arank;
  int nbproc;

  mpi_error(MPI_Comm_size (trio_u_world_, & nbproc));
  mpi_error(MPI_Comm_rank (trio_u_world_, & arank));

  Comm_Group::init_group_trio(nbproc, arank);

  mpi_comm_ = trio_u_world_;
  MPI_Comm_group(mpi_comm_, &mpi_group_);

  // Initialisation des variables statiques de la classe
  // Un buffer a envoyer et un a recevoir par processeur
  // d'ou le maximum...
  mpi_maxrequests_ = nbproc * 2;
  mpi_status_ = new MPI_Status[mpi_maxrequests_];
  mpi_requests_ = new MPI_Request[mpi_maxrequests_];

  for (int r=0; r<mpi_maxrequests_; r++)
    {
      mpi_requests_[r]=MPI_REQUEST_NULL;
    }
  if (arank == 0)
    {
      if (trio_u_world_ == MPI_COMM_WORLD)
        {
          Cerr << "Initialized MPI with MPI_COMM_WORLD (using all processors)" << finl;
        }
      else
        {
          Cerr << "Initialized MPI with communicator!=MPI_COMM_WORLD: using " << nbproc << " processors" << finl;
        }
    }
}


// MPI_Group_free should be done before MPI_Finalize so not included into Comm_Group_MPI destructor
void Comm_Group_MPI::free()
{
  if (mpi_maxrequests_!=-1) // Group is created when mpi_maxrequests_>0 (avoid a crash with verifie_pere script)
    mpi_error(MPI_Group_free(& mpi_group_));
}

// Wrapper to MPI_Alltoallv. data type is MPI_CHAR
void Comm_Group_MPI::all_to_allv(const void *src_buffer, int *send_data_size, int *send_data_offset,
                                 void *dest_buffer, int *recv_data_size, int *recv_data_offset) const
{
  statistiques().begin_count(mpi_alltoall_counter_);
  assert(src_buffer != dest_buffer);
  void * ptr = (void *) src_buffer; // Cast a cause de l'interface de MPI_Alltoall
  mpi_error(MPI_Alltoallv(ptr, send_data_size, send_data_offset, MPI_CHAR,
                          dest_buffer, recv_data_size, recv_data_offset, MPI_CHAR, mpi_comm_));
  const int n = nproc()-1;
  const int size = send_data_offset[n] + send_data_size[n] + recv_data_size[n] + recv_data_offset[n];
  statistiques().end_count(mpi_alltoall_counter_, size);
}

// Description: pour que trio_u n'utilise qu'une partie des processeurs
//  de MPI_COMM_WORLD, il faut donner un communicateur a utiliser avant
//  d'appeler init_group_trio.
void Comm_Group_MPI::set_trio_u_world(MPI_Comm world)
{
  if (mpi_status_ != 0)
    {
      Cerr << "Error : the construction of the global Comm_Group_MPI has already been done\n"
           << " set_trio_u_world call is forbidden" << finl;
      exit();
    }
#ifdef __PETSCKSP_H
  PETSC_COMM_WORLD= world;
#endif
  trio_u_world_ = world;
}

void Comm_Group_MPI::set_must_mpi_initialize(int flag)
{
  if (mpi_status_ != 0)
    {
      Cerr << "Error : the construction of the global Comm_Group_MPI has already been done\n"
           << " set_must_mpi_initialize() call is forbidden." << finl;
      exit();
    }
  must_mpi_initialize_ = flag;
}

void Comm_Group_MPI::ptop_send_recv(const void * send_buf, int send_buf_size, int send_proc,
                                    void * recv_buf, int recv_buf_size, int recv_proc) const
{
  statistiques().begin_count(mpi_sendrecv_counter_);
  assert(mpi_nrequests_ < 0);
  int dest = send_proc;
  int src = recv_proc;
  int tag = 1;
  MPI_Status status;
  if (send_proc < 0 && recv_proc < 0)
    {
      // do nothing
    }
  else if (send_proc < 0 && recv_proc >= 0)
    {
      mpi_error(MPI_Recv (recv_buf, recv_buf_size, MPI_CHAR, src, tag, mpi_comm_, &status));
    }
  else if (recv_proc < 0 && send_proc >= 0)
    {
      mpi_error(MPI_Send ((void*)send_buf, send_buf_size, MPI_CHAR, dest, tag, mpi_comm_));
    }
  else
    {
      assert(dest >= 0 && dest < nproc());
      assert(src >= 0 && src < nproc());
      // Probleme: oblige de faire un cast de (const void*) en (void*) a cause
      // du prototype de MPI_Send
      mpi_error(MPI_Sendrecv((void*)send_buf, send_buf_size, MPI_CHAR, dest, tag,
                             recv_buf, recv_buf_size, MPI_CHAR, src, tag, mpi_comm_,
                             &status));
    }
  statistiques().end_count(mpi_sendrecv_counter_, send_buf_size + recv_buf_size);
}

// Description:
// Construction du groupe de processeurs a partir de la liste.
// Voir Comm_Group::init_group(const ArrOfInt &)
// Methode appelee par PE_Groups::create_group()
void Comm_Group_MPI::init_group(const ArrOfInt& pe_list)
{
  must_finalize_ = 0;
  // Le groupe "tous" doit exister
  assert(mpi_status_);

  Comm_Group::init_group(pe_list);

  const Comm_Group_MPI& cg = ref_cast(Comm_Group_MPI, PE_Groups::current_group());
  // On stocke une reference au groupe pere : c'est le groupe courant au moment
  // de l'appel a init_group. Le destructeur devra etre appele simultanement
  // sur tous les processeurs du meme groupe.
  groupe_pere_ = PE_Groups::current_group();
  // Construction du groupe MPI
  const MPI_Group& current_mpi_group = cg.mpi_group_;
  const MPI_Comm& current_mpi_comm  = cg.mpi_comm_;
  // Copie de pe_list au cas ou int != int...
  const int nbproc = this->nproc();
  int *ranks = new int[nbproc];
  for (int i = 0; i < nbproc; i++)
    ranks[i] = pe_list[i];
  assert(mpi_group_==MPI_GROUP_NULL);
  mpi_error(MPI_Group_incl(current_mpi_group, nbproc, ranks, & mpi_group_));
  delete[] ranks;
  // Construction du communicator
  // MPI_Comm_create renvoie MPI_COMM_NULL si le processeur courant
  // n'est pas dans le groupe.
  mpi_error(MPI_Comm_create(current_mpi_comm, mpi_group_, & mpi_comm_));
}

void Comm_Group_MPI::internal_collective(const int *x, int *resu, int nx, const Collective_Op *op, int nop, int level) const
{
  // Pour l'instant algo bourrin, a optimiser...
  for (int i = 0; i < nx; i++)
    {
      int j = (nop < 0) ? 0 : i;
      if (op[j] != COLL_PARTIAL_SUM)
        mp_collective_op(x+i, resu+i, 1, op[j]);
      else
        resu[i] = mppartial_sum(x[i]);
    }
}

void Comm_Group_MPI::internal_collective(const double *x, double *resu, int nx, const Collective_Op *op, int nop, int level) const
{
  // Pour l'instant algo bourrin, a optimiser...
  for (int i = 0; i < nx; i++)
    {
      int j = (nop < 0) ? 0 : i;
      if (op[j] != COLL_PARTIAL_SUM)
        mp_collective_op(x+i, resu+i, 1, op[j]);
      else
        {
          Cerr << "Error in Comm_Group_MPI: COLL_PARTIAL_SUM not coded for double" << finl;
          exit();
        }
    }
}

// Description:
// Renvoie la somme des x sur les processeurs precedents du groupe (moi non
// compris). Le resultat sur le premier processeur du groupe est donc toujours 0.
// Le resultat depend de l'ordre dans lequel les processeurs ont ete
// fournis dans le constructeur.
int Comm_Group_MPI::mppartial_sum(int x) const
{
  statistiques().begin_count(mpi_partialsum_counter_);
  // ATTENTION : il faut "int" et pas "entier" !!!
  int somme = 0;
  MPI_Status status;
  int tag = get_new_tag();
  int rang = rank();
  int np = nproc();

  if (rang > 0)
    {
      // Recoit la somme partielle du precedent
      mpi_error(MPI_Recv(& somme, 1, MPI_INT, rang-1, tag, mpi_comm_, &status));
    }
  if (rang+1 < np)
    {
      // Envoie la somme partielle au suivant
      // ATTENTION : il faut "int" et pas "entier" !!!
      int s = somme + x;
      mpi_error(MPI_Send(& s, 1, MPI_INT, rang+1, tag, mpi_comm_));
    }
  statistiques().end_count(mpi_partialsum_counter_);
  return somme;
}

#endif

