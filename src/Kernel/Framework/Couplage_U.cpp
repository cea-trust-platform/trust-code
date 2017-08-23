/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Couplage_U.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Couplage_U.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>

Implemente_instanciable(Couplage_U,"Couplage_U",Probleme_U);

//////////////////////////////////////////////////
//                                              //
// Implementation de l'interface de Probleme_U  //
//                                              //
//////////////////////////////////////////////////


void Couplage_U::initialize()
{
  for(int i=0; i<nb_problemes(); i++)
    probleme(i).initialize();
}

void Couplage_U::terminate()
{
  for(int i=0; i< nb_problemes(); i++)
    probleme(i).terminate();
}

double Couplage_U::presentTime() const
{
  assert(nb_problemes()>0);
  return probleme(0).presentTime();
}

bool Couplage_U::initTimeStep(double dt)
{
  bool ok=true;
  for(int i=0; i<nb_problemes(); i++)
    ok = ok && probleme(i).initTimeStep(dt);
  return ok;
}

// bool Couplage_U::solveTimeStep() {
//   bool converged;
//   bool ok=iterateTimeStep(converged);

//   while (!converged && ok) {
//     updateGivenFields(); // Field exchange between iterations.
//     ok = iterateTimeStep(converged);
//   }

//   return ok;
// }

void Couplage_U::validateTimeStep()
{
  for(int i=0; i<nb_problemes(); i++)
    probleme(i).validateTimeStep();
}

bool Couplage_U::isStationary() const
{
  bool stat=true;
  for(int i=0; i<nb_problemes(); i++)
    stat = stat && probleme(i).isStationary();
  return stat;
}

void Couplage_U::setStationary(bool flag)
{
  for(int i=0; i<nb_problemes(); i++)
    probleme(i).setStationary(flag);
}

void Couplage_U::abortTimeStep()
{
  for(int i=0; i<nb_problemes(); i++)
    probleme(i).abortTimeStep();
}


bool Couplage_U::iterateTimeStep(bool& converged)
{
  bool ok=true;
  converged=true;
  for(int i=0; i<nb_problemes(); i++)
    {
      bool cv;
      ok = ok && probleme(i).iterateTimeStep(cv);
      converged = converged && cv;
    }
  return ok;
}

// Adds the input fields names this pb understands to noms
void Couplage_U::getInputFieldsNames(Noms& noms) const
{
  for (int i=0; i<nb_problemes(); i++)
    probleme(i).getInputFieldsNames(noms);
}
// Adds the output fields names this pb understands to noms
void Couplage_U::getOutputFieldsNames(Noms& noms) const
{
  for (int i=0; i<nb_problemes(); i++)
    probleme(i).getOutputFieldsNames(noms);
}


int Couplage_U::postraiter(int force)
{
  int ok=1;
  for(int i=0; i<nb_problemes(); i++)
    ok = ok && probleme(i).postraiter(force);
  return ok;
}

int Couplage_U::limpr() const
{
  bool ok=false;
  for(int i=0; i<nb_problemes(); i++)
    ok = ok || probleme(i).limpr();
  return ok;
}

int Couplage_U::lsauv() const
{
  bool ok=false;
  for(int i=0; i<nb_problemes(); i++)
    ok = ok || probleme(i).lsauv();
  return ok;
}

void Couplage_U::sauver() const
{
  for(int i=0; i<nb_problemes(); i++)
    probleme(i).sauver();
}

bool Couplage_U::updateGivenFields()
{
  bool ok=true;
  for(int i=0; i<nb_problemes(); i++)
    ok = ok && probleme(i).updateGivenFields();
  return ok;
}

double Couplage_U::futureTime() const
{
  assert(nb_problemes()>0);
  return probleme(0).futureTime();
}

REF(Field_base) Couplage_U::findInputField(const Nom& name) const
{
  REF(Field_base) ch;
  for (int i=0; i<nb_problemes(); i++)
    {
      ch=probleme(i).findInputField(name);
      if (ch.non_nul())
        return ch;
    }
  return ch;
}
REF(Champ_Generique_base) Couplage_U::findOutputField(const Nom& name) const
{
  REF(Champ_Generique_base) ch;
  for (int i=0; i<nb_problemes(); i++)
    {
      ch=probleme(i).findOutputField(name);
      if (ch.non_nul())
        return ch;
    }
  return ch;
}

///////////////////////////////////////////////////////////
//                                                       //
// Fin de l'implementation de l'interface de Probleme_U  //
//                                                       //
///////////////////////////////////////////////////////////



// PrintOn and ReadOn

Sortie& Couplage_U::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Couplage_U::readOn(Entree& s )
{
  return s ;
}


int Couplage_U::indice_probleme(const Nom& nom_pb) const
{
  int i=0;
  while ((i< nb_problemes()))
    {
      if(probleme(i).le_nom()==nom_pb)
        {
          return i;
        }
      i++;
    }
  Cerr << "Problem " << nom_pb
       << " not found in Couplage_U " << le_nom() << finl;
  exit();
  // Avoid compiler warning
  return 0;
}
