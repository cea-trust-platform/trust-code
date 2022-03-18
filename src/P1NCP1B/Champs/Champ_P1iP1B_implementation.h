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
// File:        Champ_P1iP1B_implementation.h
// Directory:   $TRUST_ROOT/src/P1NCP1B/Champs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_P1iP1B_implementation_included
#define Champ_P1iP1B_implementation_included

#include <Champ_implementation.h>
#include <TRUSTTab.h>
#include <Matrice.h>

class Frontiere_dis_base;
class Zone_Cl_dis_base;
class Zone_VEF_PreP1b;

class Champ_P1iP1B_implementation : public Champ_implementation
{

public:
  Champ_P1iP1B_implementation()
  {
    temps_filtrage_ = -DMAXFLOAT;
    adresse_champ_filtre_ = NULL;
    Condition_Neumann_imposee_ = 0;
  }

  DoubleVect& valeur_a_elem(const DoubleVect& position, DoubleVect& val, int le_poly) const override;

  double valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const override;

  DoubleTab& valeur_aux_elems(const DoubleTab& positions, const IntVect& les_polys, DoubleTab& valeurs) const override;

  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions, const IntVect& les_polys, DoubleVect& valeurs, int ncomp) const override;

  DoubleTab& valeur_aux_sommets(const Domaine&, DoubleTab&) const override;

  DoubleVect& valeur_aux_sommets_compo(const Domaine&, DoubleVect&, int) const override;

  DoubleTab& remplir_coord_noeuds(DoubleTab& positions) const override;

  int remplir_coord_noeuds_et_polys(DoubleTab& positions, IntVect& polys) const override;
  int imprime_P1B(Sortie&, int) const;
  DoubleTab& filtrage(const Zone_VEF_PreP1b&, const Champ_base&) const;        // Methode pour filtrer le champ
  const DoubleTab& champ_filtre() const { return champ_filtre_; }

  int Condition_Neumann_imposee_;        // Drapeau pour savoir s'il y'a des CL de Neumann (influe sur le filtrage)

protected:

  mutable DoubleTab champ_filtre_;                // Contient les valeurs du champ filtre
  mutable Matrice matrice_filtrage_;                // Contient la matrice necessaire au filtrage (evite d'avoir a la recalculer)
  mutable double temps_filtrage_;                // Temps du dernier filtrage         (va servir a ne pas refiltrer inutilement)
  mutable const double *adresse_champ_filtre_;        // Adresse du champ filtre        (va servir a ne pas refiltrer inutilement)

  void completer(const Zone_Cl_dis_base& zcl);

  virtual const Zone_VEF_PreP1b& zone_vef() const =0;
  DoubleTab& trace(const Frontiere_dis_base& fr, const DoubleTab& y, DoubleTab& x, int distant) const;
};
#endif
