//////////////////////////////////////////////////////////////////////////////
//
// File:        SteadyProblem.h
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SteadyProblem_included
#define SteadyProblem_included

#include <mpi.h>
#include <mon_main.h>
#include <vector>
#include <string>
#include <Problem.h>

using std::string;
using std::vector;

namespace ICoCo {

  class TrioField;

  class Init_Params ;
//////////////////////////////////////////////////////////////////////////////
//
//     class SteadyProblem
//
//     This class defines the API a problem has to implement in order to be coupled.
//     For precise specifications and possible use of this API, See NT
//     SSTH/LMDL_2006_001
//     Implements SteadyProblem and FieldIO
//
//////////////////////////////////////////////////////////////////////////////
//class SteadyProblem : public Problem {
class SteadyProblem  {

  public :

     // interface Problem
    SteadyProblem();

    SteadyProblem(void* data);
    void set_data(void* data);
    void set_data_file(const std::string& file);

    virtual ~SteadyProblem() ;
    virtual bool initialize();
    virtual void terminate();

    // interface SteadyProblem

    virtual bool solve();

    // interface IterativeSteadyProblem

    virtual bool iterate(bool& converged);

    // interface FieldIO

    virtual vector<string> getInputFieldsNames() const;
    virtual void getInputFieldTemplate(const string& name, TrioField& afield) const;
    virtual void setInputField(const string& name, const TrioField& afield);
    virtual vector<string> getOutputFieldsNames() const;
    virtual void getOutputField(const string& name, TrioField& afield) const;

  protected :

    Init_Params* my_params;
    //     Probleme_U* pb;
    mon_main* p;

  };

}
#endif
