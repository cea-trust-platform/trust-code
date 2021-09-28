/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        List_Equations_Scalaires_Passifs_Especes.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef List_Equations_Scalaires_Passifs_Especes_included
#define List_Equations_Scalaires_Passifs_Especes_included

#include <Ref_Milieu_base.h>
#include <List_Equation.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe List_Equations_Scalaires_Passifs_Especes
//     Represente une liste d'equations de scalaires passifs ou especes
//     List_Equations_Scalaires_Passifs_Especes comprend une liste d'equations
//     de scalaires passifs ou especes et est vue comme une equation
//     elle porte une vraie liste d'equations
// .SECTION voir aussi
//
//////////////////////////////////////////////////////////////////////////////

class List_Equations_Scalaires_Passifs_Especes : public Equation_base
{
  Declare_instanciable(List_Equations_Scalaires_Passifs_Especes);

public :
  const  Equation_base& equation(int i) const { return list_eq(i).valeur(); }
  int nb_equation() { return list_eq.size(); }
  int nb_equation() const { return list_eq.size(); }
  int complete() { return complet; }
  int complete() const { return complet; }
  virtual void discretiser() { };
  virtual void associer_milieu_equation();
  virtual void associer_milieu_base(const Milieu_base& mil1) { mil=mil1; }
  virtual const Milieu_base& milieu() const { return mil.valeur(); }
  virtual Milieu_base& milieu() { return mil.valeur(); }
  virtual int nombre_d_operateurs() const { return 0; }

  Equation_base& equation(int i)
  {
    if (nb_equation()==0)
      {
        Cerr << "\nError in List_Equations_Scalaires_Passifs_Especes::equation() : The equation list is empty !" << finl;
        Process::exit();
      }
    return list_eq(i).valeur();
  };

  virtual const Operateur& operateur(int) const
  {
    Process::exit();
    Equation bidon;;
    return bidon.operateur(0);
  }

  virtual Operateur& operateur(int)
  {
    Process::exit();
    Equation bidon;
    return bidon.operateur(0);
  }

  virtual const Champ_Inc& inconnue() const
  {
    Process::exit(); ;
    Equation bidon;
    return bidon.inconnue();
  }

  virtual Champ_Inc& inconnue()
  {
    Process::exit();
    Equation bidon ;
    return bidon.inconnue();
  }

protected :
  LIST(Equation) list_eq;
  REF(Milieu_base) mil;
  int complet;
};

#endif /* List_Equations_Scalaires_Passifs_Especes_included */
