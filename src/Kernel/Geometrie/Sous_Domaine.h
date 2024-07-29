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

#ifndef Sous_Domaine_included
#define Sous_Domaine_included

#include <TRUST_List.h>
#include <TRUSTVect.h>
#include <TRUST_Ref.h>
#include <Domaine_forward.h>

/*! @brief Sous_Domaine represents a volumic sub-domain i.e. a sub set of elements of a Domaine
 *
 * It bears a reference to the Domaine it subdivides
 *
 * @sa Domaine Sous_Domaines
 */
template <typename _SIZE_>
class Sous_Domaine_32_64 : public Objet_U
{

  Declare_instanciable_32_64(Sous_Domaine_32_64);

public :

  using int_t = _SIZE_;
  using ArrOfInt_t = AOInt_T<_SIZE_>;
  using IntVect_t = IVect_T<_SIZE_>;
  using IntTab_t = ITab_T<_SIZE_>;
  using SmallArrOfTID_t = SmallAOTID_T<_SIZE_>;
  using ArrOfDouble_t= AODouble_T<_SIZE_>;
  using DoubleVect_t = DVect_T<_SIZE_>;
  using DoubleTab_t = DTab_T<_SIZE_>;
  using DoubleTabs_t = DoubleTabs_T<_SIZE_>;

  using Domaine_t = Domaine_32_64<_SIZE_>;

  int lire_motcle_non_standard(const Motcle&, Entree&) override { return 0; }
  inline const Nom& le_nom() const override { return nom_; }
  inline int_t operator()(int_t i) const          { return les_elems_[i]; }
  inline int_t operator[](int_t i) const          { return les_elems_[i]; }
  // Returns the subdomaine number of elements (real+virtual elements in parallel)
  inline int_t nb_elem_tot() const              { return les_elems_.size(); }
  void associer_domaine(const Domaine_t& d)  { le_dom_=d; }
  int associer_(Objet_U&) override;
  void nommer(const Nom& nom) override   { nom_=nom; }
  void add_elem(const int_t poly);
  inline const IntVect_t& les_elems() const { return les_elems_; }
  inline IntVect_t& les_elems()             { return les_elems_; }

  Domaine_t& domaine()               { return le_dom_.valeur(); }
  const Domaine_t& domaine() const   { return le_dom_.valeur(); }

protected :

  IntVect_t les_elems_;
  REF(Domaine_t) le_dom_;
  Nom nom_;
};

using Sous_Domaine = Sous_Domaine_32_64<int>;
using Sous_Domaine_64 = Sous_Domaine_32_64<trustIdType>;

#endif

