// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found in the TRUST repository:
//
//    https://sourceforge.net/projects/trust/

#include <ICoCoExceptions.hxx>
#include <ICoCoProblem.hxx>
#include <iostream>

namespace ICoCo
{
  ///////////////////////////
  //                       //
  //   section Problem   //
  //                       //
  ///////////////////////////
  Problem::Problem()
  {
  }

  Problem::~Problem()
  {
  }

  void Problem::setDataFile(const std::string& datafile)
  {
    throw NotImplemented("type_of_Problem_not_set", "setDataFile");
  }

  void Problem::setMPIComm(void* mpicomm)
  {
    if (mpicomm != 0)
      throw NotImplemented("type_of_Problem_not_set", "setMPIComm with comm<>0");
  }

  bool Problem::initialize()
  {
    throw NotImplemented("type_of_Problem_not_set", "initialize");
  }

  void Problem::terminate()
  {
    throw NotImplemented("type_of_Problem_not_set", "terminate");
  }

  ///////////////////////////////////
  //                               //
  //   section UnsteadyProblem   //
  //                               //
  ///////////////////////////////////

  double Problem::presentTime() const
  {
    throw NotImplemented("type_of_Problem_not_set", "presentTime");
  }

  double Problem::computeTimeStep(bool& stop) const
  {
    throw NotImplemented("type_of_Problem_not_set", "computeTimeStep");
  }

  bool Problem::initTimeStep(double dt)
  {
    throw NotImplemented("type_of_Problem_not_set", "initTimeStep");
  }

  bool Problem::solveTimeStep()
  {
    throw NotImplemented("type_of_Problem_not_set", "solveTimeStep");
  }

  void Problem::validateTimeStep()
  {
    throw NotImplemented("type_of_Problem_not_set", "terminate");
  }

  void Problem::setStationaryMode(bool stationary)
  {
    throw NotImplemented("type_of_Problem_not_set", "setStationaryMode");
  }

  bool Problem::isStationary() const
  {
    throw NotImplemented("type_of_Problem_not_set", "isStationary");
  }

  void Problem::abortTimeStep()
  {
    throw NotImplemented("type_of_Problem_not_set", "abortTimeStep");
  }

  bool Problem::iterateTimeStep(bool& converged)
  {
    throw NotImplemented("type_of_Problem_not_set", "iterateTimeStep");
  }

  void Problem::resetTime(double time)
  {
    throw NotImplemented("type_of_Problem_not_set", "resetTimeStep");
  }

  void Problem::save(int label, const std::string& method) const
  {
    throw NotImplemented("type_of_Problem_not_set", "save");
  }

  void Problem::restore(int label, const std::string& method)
  {
    throw NotImplemented("type_of_Problem_not_set", "restore");
  }

  void Problem::forget(int label, const std::string& method) const
  {
    throw NotImplemented("type_of_Problem_not_set", "forget");
  }

  std::string Problem::getMeshUnit() const
  {
    throw NotImplemented("type_of_Problem_not_set", "getMeshUnit");
  }

  std::string Problem::getFieldUnit(const std::string& fieldName) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getFieldUnit");
  }

  std::vector<std::string> Problem::getInputFieldsNames() const
  {
    throw NotImplemented("type_of_Problem_not_set", "getInputFieldsNames");
  }

  void Problem::getInputFieldTemplate(const std::string& name, TrioField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getInputFieldTemplate");
  }

  ///////////////////////////////////
  //                               //
  //   section MED fields I/O   //
  //                               //
  ///////////////////////////////////

  void Problem::getInputMEDDoubleFieldTemplate(const std::string& name, MEDDoubleField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getInputMEDDoubleFieldTemplate");
  }

  void Problem::setInputMEDDoubleField(const std::string& name, const MEDDoubleField& afield)
  {
    throw NotImplemented("type_of_Problem_not_set", "setInputMEDDoubleField");
  }

  void Problem::getOutputMEDDoubleField(const std::string& name, MEDDoubleField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputMEDDoubleField");
  }

  void Problem::updateOutputMEDDoubleField(const std::string& name, MEDDoubleField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "updateOutputMEDDoubleField");
  }

  void Problem::getInputMEDIntFieldTemplate(const std::string& name, MEDIntField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getInputMEDIntFieldTemplate");
  }

  void Problem::setInputMEDIntField(const std::string& name, const MEDIntField& afield)
  {
    throw NotImplemented("type_of_Problem_not_set", "setInputMEDIntField");
  }

  void Problem::getOutputMEDIntField(const std::string& name, MEDIntField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputMEDIntField");
  }

  void Problem::updateOutputMEDIntField(const std::string& name, MEDIntField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "updateOutputMEDIntField");
  }

  void Problem::getInputMEDStringFieldTemplate(const std::string& name, MEDStringField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputMEDStringFieldTemplate");
  }

  void Problem::setInputMEDStringField(const std::string& name, const MEDStringField& afield)
  {
    throw NotImplemented("type_of_Problem_not_set", "setInputMEDStringField");
  }

  void Problem::getOutputMEDStringField(const std::string& name, MEDStringField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputMEDStringField");
  }

  void Problem::updateOutputMEDStringField(const std::string& name, MEDStringField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "updateOutputMEDStringField");
  }

  int Problem::getMEDCouplingMajorVersion() const
  {
    throw NotImplemented("type_of_Problem_not_set", "getMEDCouplingMajorVersion");
  }

  bool Problem::isMEDCoupling64Bits() const
  {
    throw NotImplemented("type_of_Problem_not_set", "isMEDCoupling64Bits");
  }

  ///////////////////////////////////
  //                               //
  //   section TrioField fields I/O   //
  //                               //
  ///////////////////////////////////

  void Problem::setInputField(const std::string& name, const TrioField& afield)
  {
    throw NotImplemented("type_of_Problem_not_set", "setInputField");
  }

  std::vector<std::string> Problem::getOutputFieldsNames() const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputFieldsNames");
  }

  ValueType Problem::getFieldType(const std::string& name) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputValuesNames");
  }

  void Problem::getOutputField(const std::string& name, TrioField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputField");
  }

  void Problem::updateOutputField(const std::string& name, TrioField& afield) const
  {
    throw NotImplemented("type_of_Problem_not_set", "updateOutputField");
  }


  ///////////////////////////////////
  //                               //
  //   section Scalar values I/O   //
  //                               //
  ///////////////////////////////////

  std::vector<std::string> Problem::getInputValuesNames() const
  {
    throw NotImplemented("type_of_Problem_not_set", "getInputValuesNames");
  }

  std::vector<std::string> Problem::getOutputValuesNames() const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputValuesNames");
  }

  ValueType Problem::getValueType(const std::string& name) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputValuesNames");
  }

  std::string Problem::getValueUnit(const std::string& varName) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getValueUnit");
  }

  double Problem::getOutputDoubleValue(const std::string& name) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputDoubleValue");
  }

  void Problem::setIntputDoubleValue(const std::string& name, const double& val)
  {
    throw NotImplemented("type_of_Problem_not_set", "setIntputDoubleValue");
  }

  int Problem::getOutputIntValue(const std::string& name) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputIntValue");
  }

  void Problem::setIntputIntValue(const std::string& name, const int& val)
  {
    throw NotImplemented("type_of_Problem_not_set", "setIntputIntValue");
  }

  std::string Problem::getOutputStringValue(const std::string& name) const
  {
    throw NotImplemented("type_of_Problem_not_set", "getOutputStringValue");
  }

  void Problem::setIntputStringValue(const std::string& name, const std::string& val)
  {
    throw NotImplemented("type_of_Problem_not_set", "setIntputStringValue");
  }

}  // end namespace ICoCo
