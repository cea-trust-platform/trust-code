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
// File:        ProblemTrio.cpp
// Directory:   $TRUST_ROOT/src/Kernel/ICoCo
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#include <ProblemTrio.h>
#include <Probleme_U.h>
#include <Probleme_base.h>
#include <ICoCoExceptions.h>
#include <Noms.h>
#include <stdlib.h>
#include <Comm_Group_MPI.h>
#include <MAIN.h>
#include <mon_main.h>
#include <ICoCoTrioField.h>
#include <Schema_Temps_base.h>
#include <Catch_SIGINIT.h>
#include <Init_Params.h>

#include <Motcle.h>

#include <ICoCoTrioField.h>
#include <medcoupling++.h>
#ifndef NO_MEDFIELD
#include <ICoCoMEDDoubleField.h>
#include <MEDCouplingFieldDouble.hxx>
#include <MEDCoupling_version.h>
#endif

#include <Convert_ICoCoTrioField.h>
#include <stat_counters.h>
#include <Field_base.h>

using ICoCo::Problem;
using ICoCo::ProblemTrio;
using ICoCo::TrioField;
using std::string;
using std::vector;


ProblemTrio::~ProblemTrio()
{
  if(p) delete p;
  p=0;
  delete (my_params);
}

////////////////////////////
//                        //
//   interface ProblemTrio    //
//                        //
////////////////////////////

// When the dynamic library is loaded via dlopen(), getting a handle
// on this function is the only way to create a Problem object.

extern "C" Problem* getProblem()
{
  //Cerr<<"coucou getProblem"<<finl;
  Problem* T=new ProblemTrio;
  return T;
}

// Description:
// As initialize doesn't have any arguments, they can be passed to the Problem
// at the time of instantiation.
// They can include data file name, MPI communicator,..
// In this implementation (for use outside TRUST), only the name of an
// underlying Probleme_U needs to be provided.
// Precondition: None.
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// Problem instantiated

ProblemTrio::ProblemTrio()
{
  my_params=new Init_Params;
  (*my_params).problem_name="default_vvvvv";
  (*my_params).problem_name="pb";
  //my_params.comm=MPI_COMM_WORLD;
  (*my_params).is_mpi=0;
  pb=NULL;
  p=NULL;
}


void ProblemTrio::setDataFile(const string& file)
{
  (*my_params).data_file=file;
}
void ProblemTrio::setMPIComm(void* mpicomm)
{
#ifdef MPI_
  if (mpicomm)
    {
      (*my_params).is_mpi=1;
      (*my_params).comm=*((MPI_Comm*)(mpicomm));
    }
#endif
}
// Description:
// This method is called once at the beginning, before any other one of
// the interface Problem.
// Precondition: Problem is instantiated, not initialized
// (*my_params) have been filled by constructor.
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// Problem initialized.
// Unknown and given fields are initialized at initial time
bool ProblemTrio::initialize()
{
  Process::exception_sur_exit=1;
  if (((*my_params).problem_name=="default_vvvvv") || ((*my_params).data_file=="default_vvvvv"))
    throw WrongArgument("??","Constructor","data","data shoud point to the name of a Probleme_U");

#ifdef MPI_
  // exception if I don't belong to comm !
  True_int rank_in_comm=0;
  if ((*my_params).is_mpi!=0)
    {
      if (MPI_Comm_rank((*my_params).comm,&rank_in_comm)!=MPI_SUCCESS)
        throw WrongArgument((*my_params).problem_name,"initialize","comm","This process should belong to comm");
      if (rank_in_comm==MPI_UNDEFINED)
        throw WrongArgument((*my_params).problem_name,"initialize","comm","This process should belong to comm");

      Comm_Group_MPI::set_trio_u_world((*my_params).comm);

    }
  Comm_Group_MPI::set_must_mpi_initialize(0); // ???
#endif
  int argc=2;
  char** argv=new char*[argc];
  string code="TRUST wrapper";
  // Les copies sont necessaires pour se debarrasser des const...
  argv[0]=new char[code.length()+1];
  strcpy(argv[0],code.c_str());
  argv[1]=new char[(*my_params).data_file.length()+1];
  strcpy(argv[1],(*my_params).data_file.c_str());
  // pour salome
  if (p) delete p;
  p=NULL;
  int res;
  // on lance avec ou sans mpi
  res=main_TRUST(argc,argv,p,(*my_params).is_mpi);


  delete [] argv[0];
  delete [] argv[1];
  delete [] argv;

  Nom nom("ICoCoProblemName"),nom_pb;
  if (nom.interprete().objet_existant(nom))
    {
      nom_pb=ref_cast(Nom,get_obj(nom));
      Cout<<finl<<" ICoCoProblemName from data file "<<nom_pb<< finl;
    }
  else
    {
      nom_pb="pb";
      Cout<<finl<<" ICoCoProblemName not found in data file, we try "<<nom_pb<< finl;
    }

  (*my_params).problem_name=nom_pb;
  pb=&ref_cast(Probleme_U,get_obj(nom_pb));
  if (res || !pb)
    throw WrongArgument((*my_params).problem_name,"initialize","problem_name","No problem of that name found in data file");
  initialize_pb(*pb);

  // Register signal and signal handler (SIGINT) if user presses ctrl-c during exec
  if (!Objet_U::DEACTIVATE_SIGINT_CATCH)
    {
      Catch_SIGINIT sig;
      sig.set_nom_cas_pour_signal(pb->nom_du_cas());
      signal(SIGINT, Catch_SIGINIT::signal_callback_handler);
    }

  // Print the initialization CPU statistics
  if (!Objet_U::disable_TU)
    {
      statistiques().dump("Statistiques d'initialisation du calcul", 0);
      print_statistics_analyse("Statistiques d'initialisation du calcul", 0);
    }
  statistiques().reset_counters();
  statistiques().begin_count(temps_total_execution_counter_);
  return true;
}
bool ProblemTrio::initialize_pb(Probleme_U& pb_to_solve)
{
  if (pb==NULL)
    pb=&pb_to_solve;
  pb_to_solve.initialize();
  pb_to_solve.postraiter(1);
  return true;
}

// Description:
// This method is called once at the end, after any other one.
// It frees the memory and saves anything that needs to be saved.
// Precondition: initialize, but not yet terminate
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// The object is ready to be destroyed.
void ProblemTrio::terminate()
{
  pb->postraiter(1);
  pb->terminate();
  int mode_append=1;
  if (!Objet_U::disable_TU)
    {
      statistiques().dump("Statistiques Resolution", mode_append);
      print_statistics_analyse("Statistiques Resolution", 1);
    }
  if(p)
    {
      delete p;
      p=0;
      // fait dans mon_main maintenant // PE_Groups::finalize();
    }
}


///////////////////////////////////
//                               //
//   interface UnsteadyProblem   //
//                               //
///////////////////////////////////


// Description:
// Returns the present time.
// This value may change only at the call of validateTimeStep.
// A surcharger
// Precondition: initialize, not yet terminate
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: present time
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// ProblemTrio unchanged
double ProblemTrio::presentTime() const
{
  return pb->presentTime();
}

// Description:
// Compute the value the Problem would like for the next time step.
// This value will not necessarily be used at the call of initTimeStep,
// but it is a hint.
// This method may use all the internal state of the Problem.
// Precondition: initialize, not yet terminate
// Parametre: stop
//    Signification: Does the Problem want to stop ?
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: The desired time step
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// ProblemTrio unchanged
double ProblemTrio::computeTimeStep(bool& stop) const
{
  return pb->computeTimeStep(stop);
}


// Description:
// This method allocates and initializes the unknown and given fields
// for the future time step.
// The value of the interval is imposed through the parameter dt.
// In case of error, returns false.
// Precondition: initialize, not yet terminate, timestep not yet initialized, dt>0
// Parametre: double dt
//    Signification: the time interval to allocate
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: bool
//    Signification: true=OK, false=error, not able to tackle this dt
//    Contraintes:
// Exception: WrongContext, WrongArgument
// Effets de bord:
// Postcondition:
// Enables the call to several methods for the next time step
bool ProblemTrio::initTimeStep(double dt)
{
  return pb->initTimeStep(dt);
}


// Description:
// Calculates the unknown fields for the next time step.
// The default implementation uses iterations.
// Precondition: initTimeStep
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: bool
//    Signification: true=OK, false=unable to find the solution.
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// The unknowns are updated over the next time step.
bool ProblemTrio::solveTimeStep()
{
  statistiques().begin_count(timestep_counter_);
  if (pb->lsauv())
    pb->sauver();
  bool res=pb->solveTimeStep();

  return res;
}


// Description:
// Validates the calculated unknown by moving the present time
// at the end of the time step.
// This method is allowed to free past values of the unknown and given
// fields.
// Precondition: initTimeStep
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// The present time has moved forward.
void ProblemTrio::validateTimeStep()
{
  pb->validateTimeStep();
  if(sub_type(Probleme_base,*pb))
    {
      const Probleme_base& pb_base = ref_cast(Probleme_base,*pb);
      const Schema_Temps_base& sch = pb_base.schema_temps();
      bool stop_ = sch.stop();
      if (!stop_) pb->postraiter(0);
    }
  else
    {
      // If *pb is a coupled problem, we may get the last line duplicated in post-processing files.
      pb->postraiter(0);
    }
  statistiques().end_count(timestep_counter_);
}

// Description:
// Tells if the Problem unknowns have changed during the last time step.
// Precondition: validateTimeStep
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: bool
//    Signification: true=stationary, false=not stationary
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// ProblemTrio unchanged
bool ProblemTrio::isStationary() const
{
  return pb->isStationary();
}

void ProblemTrio::setStationaryMode(bool stationary)
{
  // TODO TODO FIXME
  Cerr << "ProblemTrio::setStationaryMode() not impl!" << finl;
  Process::exit(-1);
}

bool ProblemTrio::getStationaryMode() const
{
  // TODO TODO FIXME
  Cerr << "ProblemTrio::getStationaryMode() not impl!" << finl;
  Process::exit(-1);
  return true;
}


// Description:
// Aborts the resolution of the current time step.
// Precondition: initTimeStep
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// Can call again initTimeStep with a new dt.
void ProblemTrio::abortTimeStep()
{
  pb->abortTimeStep();
}

/////////////////////////////////////////////
//                                         //
//   interface IterativeUnsteadyProblem    //
//                                         //
/////////////////////////////////////////////

// Description:
// In the case solveTimeStep uses an iterative process,
// this method executes a single iteration.
// It is thus possible to modify the given fields between iterations.
// converged is set to true if the process has converged, ie if the
// unknown fields are solution to the problem on the next time step.
// Otherwise converged is set to false.
// The return value indicates if the convergence process behaves normally.
// If false, the ProblemTrio wishes to abort the time step resolution.
// Precondition: initTimeStep
// Parametre: bool& converged
//    Signification: It is a return value :
//                   true if the process has converged, false otherwise.
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: bool
//    Signification: true=OK, false=unable to converge
//    Contraintes:
// Exception: WrongContext
// Effets de bord:
// Postcondition:
// The unknowns are updated over the next time step.
bool ProblemTrio::iterateTimeStep(bool& converged)
{
  return pb->iterateTimeStep(converged);
}

// Description:
// This method is used to find the names of input fields understood by the Problem
// Precondition: initTimeStep
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: vector<string>
//    Signification: list of names usable with getInputFieldTemplate and setInputField
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
// ProblemTrio unchanged
vector<string> ProblemTrio::getInputFieldsNames() const
{
  vector<string> v;
  Noms noms;
  pb->getInputFieldsNames(noms);
  for (int i=0; i<noms.size(); i++)
    v.push_back(noms[i].getChar());
  return v;
}

// Description:
// This method is used to get a template of a field expected for the given name.
// Precondition: initTimeStep, name is one of getInputFieldsNames
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
// afield contains a field with values neither allocated nor filled, describing the
// field expected by setInputField for that name.
// ProblemTrio unchanged
void ProblemTrio::getInputFieldTemplate(const std::string& name, TrioField& afield) const
{
  Nom nom(name.c_str());
  pb->getInputFieldTemplate(nom,afield);
}

// Description:
// This method is used to provide the Problem with an input field.
// Precondition: initTimeStep, name is one of getInputFieldsNames, afield is like in getInputFieldTemplate
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
// Values of afield have been used (copied inside the ProblemTrio).
void ProblemTrio::setInputField(const std::string& name, const TrioField& afield)
{
  Nom nom(name.c_str());
  pb->setInputField(nom,afield);
}


vector<string> ProblemTrio::getOutputFieldsNames() const
{

  // const Probleme_base& pb_base=ref_cast(Probleme_base,*pb);
  Noms my_names;
  pb->getOutputFieldsNames(my_names);
  vector<string> output_names;
  for (int i=0; i<my_names.size(); i++)
    output_names.push_back(my_names(i).getChar());
  return  output_names;

}

ICoCo::ValueType ProblemTrio::getFieldType(const std::string& name) const
{
  vector<string> fld = getOutputFieldsNames();
  if (std::find(fld.begin(), fld.end(), name) == fld.end())
    {
      fld = getInputFieldsNames();
      if (std::find(fld.begin(), fld.end(), name) == fld.end())
        throw WrongArgument((*my_params).problem_name,"getFieldType","name","invalid field name!!");
    }
  // All fields thus far are Double field in TRUST:
  return ICoCo::ValueType::Double;
}

void ProblemTrio::getOutputField(const std::string& name_, TrioField& afield) const
{
  Motcle name(name_.c_str());
  pb->getOutputField(name,afield);
}

// TODO: provide a more efficient version of this:
void ProblemTrio::updateOutputField(const std::string& name, TrioField& afield) const
{
  getOutputField(name, afield);
}

void ProblemTrio::getOutputMEDDoubleField(const std::string& name,MEDDoubleField& medfield) const
{
#ifndef NO_MEDFIELD
  TrioField  triofield;
  getOutputField(name,triofield);
  medfield= build_medfield(triofield);

#else
  throw NotImplemented("No ParaMEDMEM","getInputMEDDoubleField");
#endif
}

void ProblemTrio::getInputMEDDoubleFieldTemplate(const std::string& name, MEDDoubleField& medfield) const
{
#ifndef NO_MEDFIELD
  TrioField  triofield;
  getInputFieldTemplate(name,triofield);
  medfield=build_medfield(triofield);
#else
  throw NotImplemented("No ParaMEDMEM","getInputMEDDoubleFieldTemplate");
#endif
}

void ProblemTrio::setInputMEDDoubleField(const std::string& name, const MEDDoubleField& afield)
{
#ifndef NO_MEDFIELD
  // bof en attendant mieux
  TrioField  triofield;
  getInputFieldTemplate(name,triofield);
#ifdef OLD_MEDCOUPLING
  const ParaMEDMEM::DataArrayDouble *fieldArr=afield.getField()->getArray();
#else
  const MEDCoupling::DataArrayDouble *fieldArr=afield.getMCField()->getArray();
#endif
  triofield._field=const_cast<double*> (fieldArr->getConstPointer());
  // il faut copier les valeurs
  setInputField(name,triofield);
  triofield._field=0;
  //fieldArr->decrRef();

  // Assigning proper component names
  const unsigned int nbcomp = fieldArr->getNumberOfComponents();
  std::vector<std::string> compo_names = fieldArr->getInfoOnComponents();

  REF(Field_base) ch = pb->findInputField(Nom(name.c_str()));
  assert(nbcomp == (unsigned)ch->nb_comp());
  for (unsigned int i = 0; i < nbcomp; i++)
    {
      Nom compo_name(compo_names[i].c_str());
      ch->fixer_nom_compo((int)i, compo_name);
    }
#else
  throw NotImplemented("No ParaMEDMEM","setInputMEDDoubleField");
#endif
}

// TODO TODO FIXME do this more cleverly
void ProblemTrio::updateOutputMEDDoubleField(const std::string& name, MEDDoubleField& medfield) const
{
#ifndef NO_MEDFIELD
  TrioField  triofield;
  getOutputField(name,triofield);
  medfield= build_medfield(triofield);

#else
  throw NotImplemented("No ParaMEDMEM","getInputMEDDoubleField");
#endif
}


int ProblemTrio::getMEDCouplingMajorVersion() const
{
#ifdef OLD_MEDCOUPLING
  return 7;
#else
  return MEDCOUPLING_VERSION_MAJOR;
#endif
}

bool ProblemTrio::isMEDCoupling64Bits() const
{
#ifdef OLD_MEDCOUPLING
  return false;
#else
  return sizeof(mcIdType) == 8;
#endif
}

// Miscellaneous
Objet_U& get_obj(const char* chr)
{
  Nom nom(chr);
  Objet_U& obj=nom.interprete().objet(nom);
  return obj;
}


