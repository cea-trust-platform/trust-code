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
// File:        Probleme_U.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Probleme_U_included
#define Probleme_U_included

#include <Ref_Field_base.h>
#include <Ref_Champ_Generique_base.h>
class Noms;

namespace ICoCo
{
class TrioField;
}

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Probleme_U
//
//     Cette classe ajoute des specificites TRUST a l'interface
//     generale definie dans la classe Problem.
//
//     De Probleme_U derivent :
//     * Probleme_base pour tous les problemes individuels
//     * Couplage_U pour les couplages de plusieurs Probleme_U
//
//     Toutes ces classes doivent satisfaire l'API de Problem
//     completee par celle de Probleme_U.
//
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class Probleme_U : public Objet_U
{
  Declare_base(Probleme_U);
public :

  // Implementation de l'API de Problem

  virtual void initialize();
  virtual void terminate();
  virtual double presentTime() const;
  virtual double computeTimeStep(bool& stop) const;
  virtual bool initTimeStep(double dt);
  virtual bool solveTimeStep();
  virtual void validateTimeStep();
  virtual bool isStationary() const;
  virtual void setStationary(bool);
  virtual void abortTimeStep();
  virtual bool iterateTimeStep(bool& converged);
  virtual void getInputFieldsNames(Noms& noms) const;
  virtual void getInputFieldTemplate(const Nom& name, ICoCo::TrioField& afield) const;
  virtual void setInputField(const Nom& name, const ICoCo::TrioField& afield);
  virtual void getOutputFieldsNames(Noms& noms) const;
  virtual void getOutputField(const Nom& nameField, ICoCo::TrioField& afield) const;

  

  // Complements a l'API de Problem

  virtual int postraiter(int force=1);
  virtual int limpr() const;
  virtual int lsauv() const;
  virtual void sauver() const;
  virtual bool updateGivenFields();

  virtual double futureTime() const;
  virtual bool run();
  virtual bool runUntil(double time);

  virtual REF(Field_base) findInputField(const Nom& name) const;
  virtual REF(Champ_Generique_base) findOutputField(const Nom& name) const;
  // Methodes d'acces aux membres :

  inline void nommer(const Nom&);
  inline const Nom& le_nom() const;


protected :

  Nom nom;

};


inline void Probleme_U::nommer(const Nom& name)
{
  nom=name;
}

inline const Nom& Probleme_U::le_nom() const
{
  return nom;
}

#endif
