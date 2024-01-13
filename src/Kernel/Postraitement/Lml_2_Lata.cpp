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

#include <Lml_2_Lata.h>
#include <LataFilter.h>
#include <LmlReader.h>
#include <Motcle.h>

Implemente_instanciable(Lml_2_Lata, "Lml_to_lata|Lml_2_Lata", Interprete);
// XD lml_to_lata interprete lml_to_lata -1 To convert results file written with LML format to a single LATA file.
// XD attr file_lml chaine file_lml 0 LML file to convert to the new format.
// XD attr file_lata chaine file_lata 0 Name of the single LATA file.

Sortie& Lml_2_Lata::printOn(Sortie& os) const { return Interprete::printOn(os); }
Entree& Lml_2_Lata::readOn(Entree& is) { return Interprete::readOn(is); }

Entree& Lml_2_Lata::interpreter(Entree& is)
{
  Cerr << "Syntax Lml_to_lata nom_lml||NOM_DU_CAS nom_fichier_sortie_lata||NOM_DU_CAS   " << finl;

  Nom nom_lml, nom_lata;
  is >> nom_lml >> nom_lata;

  if (Motcle(nom_lml) == "NOM_DU_CAS") nom_lml = nom_du_cas() + ".lml";
  if (Motcle(nom_lata) == "NOM_DU_CAS") nom_lata = nom_du_cas() + ".lata";

  if (!Motcle(nom_lml).finit_par(".lml")) nom_lml += Nom(".lml");
  if (!Motcle(nom_lata).finit_par(".lata")) nom_lata += Nom(".lata");

  lml_to_lata(nom_lml, nom_lata, 0 /* binaire */, 1 /* fortran_blocs */, 0 /* pas fortran_ordering */, 1 /* fortran_indexing */);

  return is;
}
