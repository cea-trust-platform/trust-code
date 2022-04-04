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
// File:        Champ_Don_Fonc_txyz.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Don_Fonc_txyz.h>
#include <Vect_Parser_U.h>

Implemente_instanciable(Champ_Don_Fonc_txyz,"Champ_Fonc_txyz",TRUSTChamp_Don_generique<Champ_Don_Type::TXYZ>);

Sortie& Champ_Don_Fonc_txyz::printOn(Sortie& os) const { return TRUSTChamp_Don_generique::printOn(os); }

Entree& Champ_Don_Fonc_txyz::readOn(Entree& is)
{
  Nom motlu;
  double t0 = 0., T = 1;
  int dim;
  dim = -1;
  Nom nom_dom;
  is >> motlu;

  if (motlu == "impulsion_t0")
    {
      is >> t0;
      is >> motlu;
      if (motlu == "impulsion_perio")
        {
          is >> T;
          is >> nom_dom;
          dim = lire_dimension(is, que_suis_je());
        }
      else
        {
          nom_dom = motlu;
          dim = lire_dimension(is, que_suis_je());
        }
    }
  else if (motlu == "impulsion_perio")
    {
      is >> T;
      is >> motlu;
      if (motlu == "impulsion_t0")
        {
          is >> t0;
          is >> nom_dom;
          dim = lire_dimension(is, que_suis_je());
        }
      else
        {
          nom_dom = motlu;
          //dim = strtol(motlu.getChar(), (char **)NULL, 10);
          dim = lire_dimension(is, que_suis_je());
        }
    }
  else
    {
      nom_dom = motlu;
      dim = lire_dimension(is, que_suis_je());
    }

  fixer_nb_comp(dim);
  VECT(Parser_U) &fxyz = fonction();
  fxyz.dimensionner(dim);

  for (int i = 0; i < dim; i++)
    {
      Nom tmp;
      is >> tmp;
      Cerr << "Reading and interpretation of the function " << tmp << finl;
      fxyz[i].setNbVar(4);
      fxyz[i].setString(tmp);
      fxyz[i].addVar("t");
      fxyz[i].addVar("x");
      fxyz[i].addVar("y");
      fxyz[i].addVar("z");
      fxyz[i].setImpulsion(t0, T);
      fxyz[i].parseString();
      Cerr << "Interpretation of function " << tmp << " Ok" << finl;
    }

  // On remplit le tableau des valeurs aux elements
  Domaine& domaine = interprete_get_domaine(nom_dom);
  const Zone& ma_zone = domaine.zone(0);
  int nb_elems = ma_zone.nb_elem();
  DoubleTab& mes_val = valeurs();
  dimensionner(nb_elems, dim);
  domaine.creer_tableau_elements(mes_val);
  mettre_a_jour(temps());

  return is;
}
