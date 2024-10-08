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

#ifndef Interprete_geometrique_base_included
#define Interprete_geometrique_base_included

#include <TRUST_List.h>
#include <Interprete.h>
#include <TRUST_Ref.h>
#include <Domaine.h>
#ifdef MICROSOFT
// necessaire pour visual
#include <Octree.h>
#endif

#include <Domaine_forward.h>

/*! @brief classe Interprete_geometrique_base .
 *
 * @sa Classe abstraite dont les interpretes geometriques qui modifient un ou plusieurs domaines doivent deriver., Une reference est faite a chaque domaine. L'invalidation de l'octree apres modification
 * du domaine, (par interpreter_(is)) est factorisee dans la methode interpreter(), Methodes abstraites:, int nombre_d_operateurs() const
 */
template <typename _SIZE_>
class Interprete_geometrique_base_32_64 : public Interprete
{
  Declare_base_32_64(Interprete_geometrique_base_32_64);

public :
  using int_t = _SIZE_;
  using Domaine_t = Domaine_32_64<_SIZE_>;

  Entree& interpreter(Entree& is) override;
  void associer_domaine(Nom& nom_dom);
  void associer_domaine(Entree& is);
  void associer_domaine(Domaine_t& dom);
  inline Domaine_t& domaine(int i=0) { return domains_(i).valeur(); }
  inline const Domaine_t& domaine(int i=0) const { return domains_(i).valeur(); }
  inline LIST(OBS_PTR(Domaine_t))& domaines() { return domains_; }
  void mettre_a_jour_sous_domaine(Domaine_t& domaine, int_t& elem, int_t num_premier_elem, int_t nb_elem) const;

protected :
  virtual Entree& interpreter_(Entree& is)=0;

  LIST(OBS_PTR(Domaine_t)) domains_; // List of reference to domains
};

using Interprete_geometrique_base = Interprete_geometrique_base_32_64<int>;
using Interprete_geometrique_base_64 = Interprete_geometrique_base_32_64<trustIdType>;

#endif

