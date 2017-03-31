//////////////////////////////////////////////////////////////////////////////
//
// File:        Problem_V2.h
// Directory:   $TRUST_ROOT/Kernel/ICoCo
// Version:     /main/3
//
//////////////////////////////////////////////////////////////////////////////

// ICoCo file common to several codes
// Problem_V2.h
// version 1.2 10/05/2010

#ifndef _Problem_V2_included_
#define _Problem_V2_included_

#include <vector>
#include <string>

namespace ICoCo
{

class MEDField;
class TrioField;

//     class Problem_V2
//
//     This class defines the API a problem has to implement in order to be coupled.
//     For precise specifications and possible use of this API,
//     see NT SSTH/LMDL_2006_001
//     Implements IterativeUnsteadyProblem_V2, Restorable and FieldIO

class Problem_V2
{

public :

  // interface Problem_V2
  Problem_V2();
  virtual ~Problem_V2();
  virtual void setDataFile(const std::string& datafile);
  virtual void setMPIComm(void* mpicomm);
  virtual bool initialize();
  virtual void terminate();

  // interface UnsteadyProblem_V2

  virtual double presentTime() const;
  virtual double computeTimeStep(bool& stop) const;
  virtual bool initTimeStep(double dt);
  virtual bool solveTimeStep();
  virtual void validateTimeStep();
  virtual bool isStationary() const;
  virtual void abortTimeStep();

  // interface IterativeUnsteadyProblem_V2

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

  virtual void getInputFieldTemplate(const std::string& name, MEDField& afield) const;
  virtual void setInputField(const std::string& name, const MEDField& afield);
  virtual void getOutputField(const std::string& name, MEDField& afield) const;

};

}

extern "C" ICoCo::Problem_V2* getProblem_V2();

#endif

