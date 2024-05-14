/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Probleme_base_interface_proto_included
#define Probleme_base_interface_proto_included

#include <Schema_Temps_base.h>
#include <TRUST_List.h>
#include <TRUST_Ref.h>

class Champ_Generique_base;
class Probleme_base;
class Field_base;

class Probleme_base_interface_proto
{
public:
  bool initTimeStep_impl(Probleme_base& pb, double dt);
  bool solveTimeStep_impl(Probleme_base& pb);
  bool isStationary_impl(const Probleme_base& pb) const;
  bool iterateTimeStep_impl(Probleme_base& pb, bool& converged);
  bool updateGivenFields_impl(Probleme_base& pb);
  const bool& is_probleme_initialized_impl() const { return initialized; }

  void initialize_impl(Probleme_base& pb);
  void terminate_impl(Probleme_base& pb);
  void validateTimeStep_impl(Probleme_base& pb);
  void abortTimeStep_impl(Probleme_base& pb);
  void getInputFieldsNames_impl(const Probleme_base& pb, Noms& noms) const;
  void getOutputFieldsNames_impl(const Probleme_base& pb, Noms& noms) const;
  void addInputField_impl(Probleme_base& pb, Field_base& f);

  double presentTime_impl(const Probleme_base& pb) const;
  double computeTimeStep_impl(const Probleme_base& pb,bool& stop) const;
  double futureTime_impl(const Probleme_base& pb) const;

  // Not the same signature as ICoCo:
  void resetTimeWithDir_impl(Probleme_base& pb, double time, std::string direname);

  REF(Field_base) findInputField_impl(const Probleme_base& pb,const Nom& name) const;
  REF(Champ_Generique_base) findOutputField_impl(const Probleme_base& pb,const Nom& name) const;

protected:
  LIST(REF(Field_base)) input_fields; // List of input fields inside this problem.
  // Flags used to control the calling order and raise exceptions
  bool initialized = false;  // true if initialize was called
  bool terminated = false;   // true if terminate was called
  bool dt_defined = false;   // true if computation interval is defined
  // set to true by initTimeStep, to false by validateTimeStep & abortTimeStep
  bool dt_validated = false; // true if last computation was validated
  // set to true by validateTimeStep, to false by initTimeStep
};

#endif /* Probleme_base_interface_proto_included */
