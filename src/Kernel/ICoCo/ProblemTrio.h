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

#ifndef ProblemTrio_included
#define ProblemTrio_included

#include <ICoCoProblem.h>
#include <vector>
#include <string>

class mon_main;
class Objet_U;

class Probleme_U;

namespace ICoCo
{

class TrioField;
class MEDDoubleField;
class MEDIntField;
class Init_Params;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     class ProblemTrio
//
//     This class defines the API a problem has to implement in order to be coupled.
//     For precise specifications and possible use of this API, See NT
//     SSTH/LMDL_2006_001
//     Implements IterativeUnsteadyProblem and FieldIO
//
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class ProblemTrio : public Problem
{

public :
  // interface specifique
  bool initialize_pb(Probleme_U& pb);

  // interface Problem
  ProblemTrio();
  void setDataFile(const std::string& datafile) override;
  void setMPIComm(void*) override;


  ~ProblemTrio() override  ;
  bool initialize() override;

  void terminate() override;

  // interface UnsteadyProblem

  double presentTime() const override;
  double computeTimeStep(bool& stop) const override;
  bool initTimeStep(double dt) override;
  bool solveTimeStep() override;
  void validateTimeStep() override;
  void setStationaryMode(bool stationary) override; // new in V2
  bool getStationaryMode() const override; // new in V2
  bool isStationary() const override;
  void abortTimeStep() override;

  // interface IterativeUnsteadyProblem

  bool iterateTimeStep(bool& converged) override;

  // Field I/O methods. Those methods are **optional**, and not all of them need to be implemented!
  //
  std::vector<std::string> getInputFieldsNames() const override;
  std::vector<std::string> getOutputFieldsNames() const override;
  ValueType getFieldType(const std::string& name) const override;

  //     TrioField fields I/O
  void getInputFieldTemplate(const std::string& name, TrioField& afield) const override;
  void setInputField(const std::string& name, const TrioField& afield) override;
  void getOutputField(const std::string& name, TrioField& afield) const override;
  void updateOutputField(const std::string& name, TrioField& afield) const override;

  //
  //     MED fields I/O: double, int and string.
  //
  void getInputMEDDoubleFieldTemplate(const std::string& name, MEDDoubleField& afield) const override;
  void setInputMEDDoubleField(const std::string& name, const MEDDoubleField& afield) override;
  void getOutputMEDDoubleField(const std::string& name, MEDDoubleField& afield) const override;
  void updateOutputMEDDoubleField(const std::string& name, MEDDoubleField& afield) const override;


  //        Specific to MEDCoupling:
  int getMEDCouplingMajorVersion() const override;
  bool isMEDCoupling64Bits() const override;

  double getOutputDoubleValue(const std::string& name) const override;
  void setInputDoubleValue(const std::string& name, const double& val) override;
protected :

  Init_Params* my_params;
  Probleme_U* pb;
  mon_main* p;

};
} // namespace ICoCo
Objet_U& get_obj(const char *) ;
#endif

