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
// File:        Champ_Tabule_Morceaux.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_Tabule_Morceaux_included
#define Champ_Tabule_Morceaux_included

#include <Champ_Don_base.h>
#include <Ref_Domaine.h>
#include <List_Ref_Sous_Zone.h>
#include <List_Champ_Fonc_Tabule.h>
#include <IntTab.h>

/////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION : class Champ_Tabule_Morceaux
//
// <Description of class Champ_Tabule_Morceaux>
//
/////////////////////////////////////////////////////////////////////////////

class Champ_Tabule_Morceaux : public Champ_Don_base
{

  Declare_instanciable( Champ_Tabule_Morceaux ) ;

public :

  int initialiser(const double& un_temps) override;
  void mettre_a_jour(double un_temps) override;

  int nb_champs_tabules()const
  {
    return champs_tabules.size();
  }

  Champ_Fonc_Tabule& champ_tabule(int i)
  {
    return champs_tabules[i] ;
  }
  const Champ_Fonc_Tabule& champ_tabule(int i) const
  {
    return champs_tabules[i];
  }

  DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                              const IntVect& les_polys,
                              DoubleTab& valeurs) const override ;
  double valeur_a_elem_compo(const DoubleVect& position,
                             int le_poly, int ncomp) const override;

protected:

  LIST(Champ_Fonc_Tabule) champs_tabules;

  LIST(REF(Sous_Zone)) les_sous_zones;
  REF(Domaine) mon_domaine;

  void calculer_champ_tabule_morceaux();
  IntTab num_zone_;
};

#endif /* Champ_Tabule_Morceaux_included */
