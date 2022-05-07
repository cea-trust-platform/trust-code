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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Table.h
// Directory:   $TRUST_ROOT/src/Kernel/Math
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Table_included
#define Table_included

#include <TRUSTTabs_forward.h>
#include <Parser_Eval.h>
#include <TRUSTVects.h>
#include <Parser_U.h>
#include <vector>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class Table : public Objet_U, public Parser_Eval
{
  Declare_instanciable_sans_constructeur_ni_destructeur(Table);

public :

  Table();
  inline Table(const DoubleVect& param,const DoubleVect& aval);
  void remplir(const DoubleVect& param,const DoubleTab& val);
  void remplir(const DoubleVects& params, const DoubleVect& aval);
  double val_simple(double vals_param) const;
  double val(const double val_param, int ncomp=0) const;
  double val(const std::vector<double>& vals_param, int ncomp) const;
  double val(const DoubleVect& val_param) const;
  DoubleTab& valeurs(const DoubleTab& val_param,const DoubleTab& pos,const double tps,DoubleTab& val) const;
  DoubleVect& valeurs(DoubleVect&, const double val_param) const;
  DoubleVect& valeurs(DoubleVect&, const DoubleVect& val_param) const;
  Entree& lire_f(Entree& is, const int nb_comp);
  Entree& lire_fxyzt(Entree& is,const int dim);
  inline const int& isfonction() const;
  Table(const Table&);
  Table& operator=(const Table& t) = default; // exige par gcc 9 car sinon error: implicitly-declared 'Table& Table::operator=(const Table&)' is deprecated [-Werror=deprecated-copy]

private:

  DoubleTab les_valeurs;
  DoubleVects les_parametres;
  int isf;
};



// Description:
//   Construit un table avec les valeurs val, et les parametres param
// Precondition:
// Parametre: const DoubleVect& param
//    Signification: parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: const DoubleVect& val
//    Signification: valeurs
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Table::Table(const DoubleVect& param,const DoubleVect& aval)
  :  les_parametres(1)
{
  les_valeurs=aval;
  les_parametres[0] = param;
}

inline const int& Table::isfonction() const
{
  return isf;
}

#endif // TABLE_H
