/****************************************************************************
* Copyright (c) 2023, CEA
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

#ifndef Domaine_PolyMAC_P0P1NC_included
#define Domaine_PolyMAC_P0P1NC_included

#include <Domaine_PolyMAC.h>
#include <TRUSTLists.h>
#include <Conds_lim.h>

class Domaine_PolyMAC_P0P1NC : public Domaine_PolyMAC
{
  Declare_instanciable(Domaine_PolyMAC_P0P1NC);
public :
  void discretiser() override;
  void calculer_volumes_entrelaces() override;

  //pour chaque element, normale * surface duale liee a chacune de ses aretes (orientee comme l'arete)
  const DoubleTab& surf_elem_arete() const;

  //matrices locales par elements (operateurs de Hodge) permettant de faire des interpolations :
  void M1(const DoubleTab *nu, int e, DoubleTab& m1) const; //normales aux aretes duales -> tangentes aux aretes : (nu|a|t_a.v)   = m1 (S_ea.v)
  void W1(const DoubleTab *nu, int e, DoubleTab& w1, DoubleTab& v_e, DoubleTab& v_ea) const; //tangentes aux aretes -> normales aux aretes duales : (nu S_ea.v)    = w1 (|a|t_a.v)
  //possibilite pour le tenseur nu :
  //nul -> nu = Id ci-dessous
  //isotrope -> nu(n_e, N) avec n_e = 1 (tenseur constant) / nb_elem_tot() (tenseur par element), et N un nombre de composantes
  //anisotrope -> nu(n_e, N, D) (anisotrope diagonal) ou nu(n_e, N, D, D) (anisotrope complet)
  //la matrice en sortie est de taile (n_f, n_f, N) (pour M2/W2) ou (n_a, n_a, N) (pour M1 / W1)

private:
  mutable DoubleTab surf_elem_arete_; // vecteur normale * surface duale
};

#endif /* Domaine_PolyMAC_P0P1NC_included */
