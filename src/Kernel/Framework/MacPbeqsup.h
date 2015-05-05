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
// File:        MacPbeqsup.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef MacPbeqsup_inclus
#define MacPbeqsup_inclus
#include <List_Equations_Scalaires_Passifs.h>
#define declare_instanciable_pb_eq_sup(NEWCLASS,CLASSMERE)        \
  class NEWCLASS : public CLASSMERE                                \
  {                                                                \
    Declare_instanciable(NEWCLASS);                                \
  public:                                                        \
    int nombre_d_equations() const;                                \
    const Equation_base& equation(int) const ;                \
    Equation_base& equation(int);                                \
    void associer_milieu_base(const Milieu_base& );                \
  protected:                                                        \
    List_Equations_Scalaires_Passifs list_eq_supp;                \
    int is_lecture;                                                \
  };
#define implemente_pb_eq_sup(NEWCLASS,NEWCLASSNAME,CLASSMERE,nbequation_class_mere) \
  Implemente_instanciable_sans_constructeur(NEWCLASS,NEWCLASSNAME,CLASSMERE); \
  NEWCLASS::NEWCLASS()                                                        \
  {                                                                        \
    is_lecture=-1;                                                        \
  }                                                                        \
Sortie& NEWCLASS::printOn(Sortie& os) const\
{\
  return CLASSMERE::printOn(os);\
}\
void NEWCLASS::associer_milieu_base(const Milieu_base& mil) \
{\
CLASSMERE::associer_milieu_base(mil);\
list_eq_supp.associer_milieu_base(mil);\
}\
Entree& NEWCLASS::readOn(Entree& is) \
{\
  is_lecture=1;\
  CLASSMERE::readOn(is);\
  is_lecture=0;\
  return is;\
}\
int NEWCLASS::nombre_d_equations() const\
{\
  if (list_eq_supp.complete())\
    return CLASSMERE::nombre_d_equations()+list_eq_supp.nb_equation();\
else\
  return CLASSMERE::nombre_d_equations()+1;\
}\
const Equation_base& NEWCLASS::equation(int i) const\
{\
  if (i < CLASSMERE::nombre_d_equations())\
    return CLASSMERE::equation(i);\
  else \
    {\
      assert(is_lecture==0);\
      return list_eq_supp.equation(i-CLASSMERE::nombre_d_equations());\
    }\
  }\
Equation_base& NEWCLASS::equation(int i) \
{\
  if (i < CLASSMERE::nombre_d_equations()) \
    return CLASSMERE::equation(i);\
  else if (list_eq_supp.complete()==0)\
    {\
      is_lecture=0;\
      return list_eq_supp;\
    }\
  else if (is_lecture==-1) \
    {\
      return list_eq_supp;\
    }\
  else\
    {\
      return list_eq_supp.equation(i-CLASSMERE::nombre_d_equations());\
    }\
}

#endif
