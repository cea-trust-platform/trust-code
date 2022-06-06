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

#include <Champ_Don_Fonc_xyz.h>
#include <Vect_Parser_U.h>

Implemente_instanciable(Champ_Don_Fonc_xyz,"Champ_Fonc_xyz",TRUSTChamp_Don_generique<Champ_Don_Type::XYZ>);

Sortie& Champ_Don_Fonc_xyz::printOn(Sortie& os) const { return TRUSTChamp_Don_generique::printOn(os); }

Entree& Champ_Don_Fonc_xyz::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  is >> nom;

  Domaine& domaine = interprete_get_domaine(nom);
  int nb_elems = domaine.zone(0).nb_elem();
  dim = lire_dimension(is, que_suis_je());
  dimensionner(nb_elems, dim);
  domaine.creer_tableau_elements(valeurs());
  DoubleTab& mes_val = valeurs();

  VECT(Parser_U) &fxyz = fonction();
  fxyz.dimensionner(dim);

  for (int i = 0; i < dim; i++)
    {
      Nom tmp;
      is >> tmp;
      Cerr << "Reading and interpretation of the function " << tmp << finl;
      fxyz[i].setNbVar(3);
      fxyz[i].setString(tmp);
      fxyz[i].addVar("x");
      fxyz[i].addVar("y");
      if (dimension > 2) fxyz[i].addVar("z");
      fxyz[i].parseString();
      Cerr << "Interpretation of function " << tmp << " Ok" << finl;
    }

  DoubleTab positions(nb_elems,dimension);
  mettre_a_jour_positions(positions);
  eval_fct(positions,mes_val);
  mes_val.echange_espace_virtuel();

  return is;
}
