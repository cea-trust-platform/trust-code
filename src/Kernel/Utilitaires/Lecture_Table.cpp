/****************************************************************************
* Copyright (c) 2017 , CEA
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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Lecture_Table.cpp
// Directory : $FLICA5_ROOT/src/Utilitaires
//
/////////////////////////////////////////////////////////////////////////////

#include <Lecture_Table.h>

Implemente_instanciable( Lecture_Table, "Lecture_Table", Objet_U ) ;

Sortie& Lecture_Table::printOn( Sortie& os ) const
{
  Objet_U::printOn( os );
  return os;
}

Entree& Lecture_Table::readOn( Entree& is )
{
  Objet_U::readOn( is );
  return is;
}

Entree& Lecture_Table::lire_table(Entree& is, Table& la_table)

{
  const Motcle accolade_ouverte("{");
  const Motcle accolade_fermee("}");
  int i,nb_val;

  Motcle motlu;

  is >> motlu;

  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading table in "<< que_suis_je() << " \n";
      Cerr << "We expected a " << accolade_ouverte << " instead of \n"
           << motlu;
      Process::exit();
    }

  is >> nb_val;

  DoubleVect param(nb_val);
  DoubleVect tab_valeurs(nb_val);

  for (i=0; i<nb_val; i++)
    is >> param[i];

  for (i=0; i<nb_val; i++)
    is >> tab_valeurs[i];

  la_table.remplir(param,tab_valeurs);

  is >> motlu;

  if (motlu != accolade_fermee)
    {
      Cerr << "Error while reading table in "<< que_suis_je() << " \n";
      Cerr << "We expected a " << accolade_fermee << " instead of \n"
           << motlu;
      Process::exit();
    }
  return is;
}
