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
// File:        Init_par_partie.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs/Champs_Don
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#include <Init_par_partie.h>

Implemente_instanciable_sans_constructeur(Init_par_partie,"Init_par_partie",TRUSTChamp_Divers_generique<Champ_Divers_Type::INUTILE>);

Sortie& Init_par_partie::printOn(Sortie& os) const { return os; }

Entree& Init_par_partie::readOn(Entree& is)
{
  nbcomp = lire_dimension(is, que_suis_je());
  fixer_nb_comp(nbcomp);
  val_don.resize(2 * nbcomp);
  R_loc.resize(nbcomp);

  for (int i = 0; i < nbcomp; i++) is >> R_loc(i);

  for (int i = 0; i < 2 * nbcomp; i++) is >> val_don(i);

  return is;
}

DoubleTab& Init_par_partie::valeur_aux(const DoubleTab& positions, DoubleTab& tab_valeurs) const
{
  assert(tab_valeurs.size_totale() * dimension / nbcomp == positions.size_totale());

  for (int i = 0; i < tab_valeurs.dimension(0); i++)
    {
      if (nbcomp == 1)
        {
          if (positions(i, 0) > R_loc(0)) tab_valeurs(i) = val_don(0);
          else tab_valeurs(i) = val_don(1);
        }
      else if (nbcomp == 2)
        {
          if (positions(i, 0) > R_loc(0)) tab_valeurs(i, 0) = val_don(0);
          else tab_valeurs(i, 0) = val_don(1);

          if (positions(i, 1) > R_loc(1)) tab_valeurs(i, 1) = val_don(2);
          else tab_valeurs(i, 1) = val_don(3);
        }
      else if (nbcomp == 3)
        {
          if (positions(i, 0) > R_loc(0)) tab_valeurs(i, 0) = val_don(0);
          else tab_valeurs(i, 0) = val_don(1);

          if (positions(i, 1) > R_loc(1)) tab_valeurs(i, 1) = val_don(2);
          else tab_valeurs(i, 1) = val_don(3);

          if (positions(i, 2) > R_loc(2)) tab_valeurs(i, 2) = val_don(4);
          else tab_valeurs(i, 2) = val_don(5);
        }
      else Cerr << " error nbcomp " << finl;
    }

  return tab_valeurs;
}
