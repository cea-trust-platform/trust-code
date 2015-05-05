//////////////////////////////////////////////////////////////////////////////
//
// File:        Problem.h
// Directory:   $TRUST_ROOT/Kernel/ICoCo
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////

// ICoCo file common to several codes
// Problem.h
// version 1.2 10/05/2010

#ifndef _Problem_included_
#define _Problem_included_

#include <vector>
#include <string>

namespace ParaMEDMEM
{
class MEDCouplingFieldDouble;
}

namespace ICoCo
{

class TrioField;

//     class Problem
//
//     This class defines the API a problem has to implement in order to be coupled.
//     For precise specifications and possible use of this API,
//     see NT SSTH/LMDL_2006_001
//     Implements IterativeUnsteadyProblem, Restorable and FieldIO

class Problem
{

public :

  // interface Problem
  Problem();
  virtual ~Problem();
  virtual void setDataFile(const std::string& datafile);
  virtual void setMPIComm(void* mpicomm);
  virtual bool initialize();
  virtual void terminate();

  // interface UnsteadyProblem

  virtual double presentTime() const;
  virtual double computeTimeStep(bool& stop) const;
  virtual bool initTimeStep(double dt);
  virtual bool solveTimeStep();
  virtual void validateTimeStep();
  virtual bool isStationary() const;
  virtual void abortTimeStep();

  // interface IterativeUnsteadyProblem

  virtual bool iterateTimeStep(bool& converged);

  // interface Restorable

  virtual void save(int label, const std::string& method) const;
  virtual void restore(int label, const std::string& method);
  virtual void forget(int label, const std::string& method) const;

  // interface FieldIO

  virtual std::vector<std::string> getInputFieldsNames() const;
  virtual void getInputFieldTemplate(const std::string& name, TrioField& afield) const;
  virtual void setInputField(const std::string& name, const TrioField& afield);
  virtual std::vector<std::string> getOutputFieldsNames() const;
  virtual void getOutputField(const std::string& name, TrioField& afield) const;

  virtual ParaMEDMEM::MEDCouplingFieldDouble* getInputMEDFieldTemplate(const std::string& name) const;
  virtual void setInputMEDField(const std::string& name, const ParaMEDMEM::MEDCouplingFieldDouble* afield);
  virtual ParaMEDMEM::MEDCouplingFieldDouble* getOutputMEDField(const std::string& name) const;

};

}

extern "C" ICoCo::Problem* getProblem();

#endif

