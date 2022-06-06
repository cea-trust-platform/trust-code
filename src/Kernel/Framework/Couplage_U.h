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


#ifndef Couplage_U_included
#define Couplage_U_included


#include <Probleme_U.h>
#include <List_Ref_Probleme_U.h>

//////////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     class Couplage_U
//
//     Cette classe implemente Probleme_U dans le cas d'un probleme composite
//     couplant plusieurs Probleme_U.
//
// .SECTION voir aussi
/////////////////////////////////////////////////////////////////////////////////
class Couplage_U : public Probleme_U
{
  Declare_instanciable(Couplage_U);

public:

  //////////////////////////////////////////////////
  //                                              //
  // Implementation de l'interface de Probleme_U  //
  //                                              //
  //////////////////////////////////////////////////

  // interface Problem

  void initialize() override;
  void terminate() override;

  // interface UnsteadyProblem

  double presentTime() const override;
  bool initTimeStep(double dt) override;
  //  virtual bool solveTimeStep();
  void validateTimeStep() override;
  bool isStationary() const override;
  void setStationary(bool) override;
  void abortTimeStep() override;

  // interface IterativeUnsteadyProblem

  bool iterateTimeStep(bool& converged) override;

  // interface FieldIO

  void getInputFieldsNames(Noms& noms) const override;
  void getOutputFieldsNames(Noms& noms) const override;

  // interface Probleme_U

  int postraiter(int force=1) override;
  int limpr()const override ;
  int lsauv() const override;
  void sauver() const override;
  bool updateGivenFields() override;
  double futureTime()const override;

  REF(Field_base) findInputField(const Nom& name) const override;
  REF(Champ_Generique_base) findOutputField(const Nom& name) const override;

  ///////////////////////////////////////////////////////////
  //                                                       //
  // Fin de l'implementation de l'interface de Probleme_U  //
  //                                                       //
  ///////////////////////////////////////////////////////////


  // Access methods to the problem list

  void addProblem(Probleme_U&);                        // Append a problem to the list
  int nb_problemes() const;                            // Number of problems

  const Probleme_U& probleme(int i) const;             // Access by index (const)
  Probleme_U& probleme(int i);                         // Access by index
  const Probleme_U& probleme(const Nom& nom_pb) const; // Access by name (const)
  Probleme_U& probleme(const Nom& nom_pb);             // Access by name
  int indice_probleme(const Nom& nom_pb) const;        // Index from name

private:

  LIST(REF(Probleme_U)) problems;

};

// inline access methods

inline void Couplage_U::addProblem(Probleme_U& pb)
{
  problems.add(pb);
}

inline int Couplage_U::nb_problemes() const
{
  return problems.size();
}

inline Probleme_U& Couplage_U::probleme(int i)
{
  return problems(i);
}

inline const Probleme_U& Couplage_U::probleme(int i) const
{
  return problems(i);
}


inline const Probleme_U& Couplage_U::probleme(const Nom& nom_pb) const
{
  return probleme(indice_probleme(nom_pb));
}

inline Probleme_U& Couplage_U::probleme(const Nom& nom_pb)
{
  return probleme(indice_probleme(nom_pb));
}

#endif
