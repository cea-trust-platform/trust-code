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

#ifndef Op_Dift_VEF_Face_Gen_included
#define Op_Dift_VEF_Face_Gen_included

enum class Type_Champ { SCALAIRE, VECTORIEL };
enum class Type_Schema { EXPLICITE, IMPLICITE };

#include <Domaine_Cl_dis.h>
#include <Domaine_Cl_VEF.h>
#include <Domaine_VEF.h>
#include <TRUST_Ref.h>

template <typename DERIVED_T>
class Op_Dift_VEF_Face_Gen
{
public:
  void associer_gen(const Domaine_dis& domaine_dis, const Domaine_Cl_dis& domaine_cl_dis)
  {
    dom_vef = ref_cast(Domaine_VEF,domaine_dis.valeur());
    zcl_vef = ref_cast(Domaine_Cl_VEF,domaine_cl_dis.valeur());
  }

  template <Type_Champ _TYPE_> void fill_grad_Re(const DoubleTab&, const DoubleTab&, const DoubleTab&, const DoubleTab&) const;

  // methodes pour l'explicite
  template <Type_Champ _TYPE_> enable_if_t_<_TYPE_ ==  Type_Champ::VECTORIEL, void>
  ajouter_bord_gen(const DoubleTab& , DoubleTab& , DoubleTab& , const DoubleTab& , const DoubleTab& ) const;

  template <Type_Champ _TYPE_> enable_if_t_<_TYPE_ ==  Type_Champ::VECTORIEL, void>
  ajouter_interne_gen(const DoubleTab& , DoubleTab& , DoubleTab& , const DoubleTab& , const DoubleTab& ) const;

  template <Type_Champ _TYPE_> enable_if_t_<_TYPE_ ==  Type_Champ::SCALAIRE, void>
  ajouter_bord_gen(const DoubleTab& , DoubleTab& , DoubleTab& , const DoubleTab& , const DoubleTab& ) const;

  template <Type_Champ _TYPE_> enable_if_t_<_TYPE_ ==  Type_Champ::SCALAIRE, void>
  ajouter_interne_gen(const DoubleTab& inco, DoubleTab& resu, DoubleTab& , const DoubleTab& nu, const DoubleTab& nu_turb) const
  {
    ajouter_interne_gen__<_TYPE_, Type_Schema::EXPLICITE>(inco, &resu, nullptr, nu, nu_turb, nu_turb /* poubelle */);
  }

  // methodes pour l'implicite
  template <Type_Champ _TYPE_, bool _IS_STAB_ = false>
  void ajouter_contribution_bord_gen(const DoubleTab&, Matrice_Morse&, const DoubleTab&, const DoubleTab&, const DoubleVect&) const;

  template <Type_Champ _TYPE_, bool _IS_STAB_ = false>
  void ajouter_contribution_interne_gen(const DoubleTab& inco, Matrice_Morse& mat, const DoubleTab& nu, const DoubleTab& nu_turb, const DoubleVect& porosite_eventuelle) const
  {
    ajouter_interne_gen__<_TYPE_, Type_Schema::IMPLICITE, _IS_STAB_>(inco, nullptr, &mat, nu, nu_turb, porosite_eventuelle);
  }

protected:
  mutable DoubleTab grad_, Re_;

private:
  REF(Domaine_VEF) dom_vef;
  REF(Domaine_Cl_VEF) zcl_vef;

  template <Type_Champ _TYPE_, Type_Schema _SCHEMA_, bool _IS_STAB_ = false>
  void ajouter_bord_perio_gen__(const int , const DoubleTab&, DoubleTab* /* Si explicite */ , Matrice_Morse* /* Si implicite */, const DoubleTab&, const DoubleTab&, const DoubleVect& , DoubleTab* flux_bord = nullptr /* flux_bords */) const;

  template <Type_Champ _TYPE_, Type_Schema _SCHEMA_, bool _IS_STAB_ = false>
  void ajouter_bord_scalaire_impose_gen__(const int , const DoubleTab&, DoubleTab* /* Si explicite */ , Matrice_Morse* /* Si implicite */, const DoubleTab&, const DoubleTab&, const DoubleVect& , DoubleTab* flux_bord = nullptr /* flux_bords */ ) const;

  template <Type_Champ _TYPE_, Type_Schema _SCHEMA_, bool _IS_STAB_ = false>
  void ajouter_interne_gen__(const DoubleTab&, DoubleTab* /* Si explicite */ , Matrice_Morse* /* Si implicite */, const DoubleTab&, const DoubleTab&, const DoubleVect&) const;
};

#include <Op_Dift_VEF_Face_Gen.tpp>

#endif /* Op_Dift_VEF_Face_Gen_included */
