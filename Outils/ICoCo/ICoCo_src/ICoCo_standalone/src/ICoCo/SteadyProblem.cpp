//////////////////////////////////////////////////////////////////////////////
//
// File:        SteadyProblem.cpp
//
//////////////////////////////////////////////////////////////////////////////

#include <SteadyProblem.h>
#include <ICoCoExceptions.h>
#include <Comm_Group_MPI.h>
#include <MAIN.h>
#include <Init_Params.h>

using namespace ICoCo;

SteadyProblem::~SteadyProblem()
{
  //  if(p) delete p;p=0;
  delete (my_params);
}

/////////////////////////////////
//                             //
//   interface SteadyProblem   //
//                             //
/////////////////////////////////
SteadyProblem::SteadyProblem()
{

  my_params=new Init_Params;
  (*my_params).problem_name="default_vvvvv";
  (*my_params).problem_name="pb";
  //my_params.comm=MPI_COMM_WORLD;
  (*my_params).is_mpi=1;
  p=NULL;


}

void SteadyProblem::set_data_file(const string& file)
{
  (*my_params).data_file=file;
  (*my_params).is_mpi=0;
  std::cout<<" fata file "<<(*my_params).data_file<<std::endl;
}

SteadyProblem::SteadyProblem(void* data)
{
  my_params=new Init_Params;
  set_data(data);
}
void SteadyProblem::set_data(void* data)
{
  if (data==0)
    throw WrongArgument("??","Constructor","data","data shoud point to the name of a Probleme_U");
  Init_Params& params=*((Init_Params*)data);
  (*my_params)=params;
}

bool SteadyProblem::initialize()
{

  // exception if I don't belong to comm !
  True_int rank_in_comm=0;
  if (MPI_Comm_rank((*my_params).comm,&rank_in_comm)!=MPI_SUCCESS)
    throw WrongArgument((*my_params).problem_name,"initialize","comm","This process should belong to comm");
  if (rank_in_comm==MPI_UNDEFINED)
    throw WrongArgument((*my_params).problem_name,"initialize","comm","This process should belong to comm");

  Comm_Group_MPI::set_trio_u_world((*my_params).comm);
  Comm_Group_MPI::set_must_mpi_initialize(0); // ???
  int argc=2;
  char** argv=new char*[argc];
  string code="TRUST wrapper";
  // Les copies sont necessaires pour se debarrasser des const...
  argv[0]=new char[code.length()+1];
  strcpy(argv[0],code.c_str());
  argv[1]=new char[(*my_params).data_file.length()+1];
  strcpy(argv[1],(*my_params).data_file.c_str());

  p=NULL;
  int res=main_TRUST(argc,argv,p,1);

  delete [] argv[0];
  delete [] argv[1];
  delete [] argv;

//   pb=&ref_cast(Probleme_U,get_obj((*my_params).problem_name.c_str()));
  if (res /* || !pb */)
    throw WrongArgument((*my_params).problem_name,"initialize","problem_name","No problem of that name found in data file");

//   pb->initialize();
//   pb->postraiter(1);
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
void SteadyProblem::terminate()
{
//   pb->postraiter(1);
//   pb->terminate();
  // Detruire le mon_main
  if (p)
    delete p;
  p=0;
}


///////////////////////////////////
//                               //
//   interface SteadyProblem   //
//                               //
///////////////////////////////////


// Description:
// Calculates the unknown fields.
// Precondition: initialize
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
bool SteadyProblem::solve()
{
  return false;
}


/////////////////////////////////////////////
//                                         //
//   interface IterativeSteadyProblem    //
//                                         //
/////////////////////////////////////////////

// Description:
// In the case solve() uses an iterative process,
// this method executes a single iteration.
// It is thus possible to modify the given fields between iterations.
// converged is set to true if the process has converged, ie if the
// unknown fields are solution to the problem on the next time step.
// Otherwise converged is set to false.
// The return value indicates if the convergence process behaves normally.
// If false, the Problem wishes to abort the resolution.
// Precondition: initialize
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
bool SteadyProblem::iterate(bool& converged)
{
  converged=false;
  return false;
}

// Description:
// This method is used to find the names of input fields understood by the Problem
// Precondition: initialize
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
// Problem unchanged
vector<string> SteadyProblem::getInputFieldsNames() const
{
  vector<string> v;
  return v;
}

// Description:
// This method is used to get a template of a field expected for the given name.
// Precondition: initialize, name is one of getInputFieldsNames
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
// Problem unchanged
void SteadyProblem::getInputFieldTemplate(const string& name, TrioField& afield) const
{
  throw WrongArgument((*my_params).problem_name,"getInputFieldTemplate","name","should be an input field");
}

// Description:
// This method is used to provide the Problem with an input field.
// Precondition: initialize, name is one of getInputFieldsNames, afield is like in getInputFieldTemplate
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
// Values of afield have been used (copied inside the Problem).
void SteadyProblem::setInputField(const string& name, const TrioField& afield)
{
  throw WrongArgument((*my_params).problem_name,"setInputField","name","should be an input field");
}

vector<string> SteadyProblem::getOutputFieldsNames() const
{
  vector<string> output_fields;
  return output_fields;
}

void SteadyProblem::getOutputField(const string& name, TrioField& afield) const
{
  throw WrongArgument((*my_params).problem_name,"getOutputField","name","should be an output field of the Problem");
}
