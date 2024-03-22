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

#ifndef Op_Diff_VEF_base_included
#define Op_Diff_VEF_base_included

#include <Operateur_Diff_base.h>
#include <Domaine_VEF.h>
#include <Op_VEF_Face.h>
#include <Milieu_base.h>
#include <TRUST_Ref.h>

class Domaine_Cl_VEF;
class Domaine_Cl_dis;
class Champ_Inc_base;
class Domaine_dis;
class Sortie;

/*! @brief class Op_Diff_VEF_base
 *
 *  Classe de base des operateurs de convection VEF
 *
 *
 */

class Op_Diff_VEF_base : public Operateur_Diff_base, public Op_VEF_Face
{
  Declare_base(Op_Diff_VEF_base);
public:

  int impr(Sortie& os) const override;
  void associer(const Domaine_dis& , const Domaine_Cl_dis& ,const Champ_Inc& ) override;

  template <typename _TYPE_>
  double viscA(int face_i, int face_j, int num_elem, const _TYPE_& diffu) const;
  template <typename _TYPE_>
  KOKKOS_INLINE_FUNCTION double viscA(int face_i, int face_j, int num_elem, const _TYPE_& diffu, CIntTabView face_voisins_v, CDoubleTabView face_normales_v, CDoubleArrView inverse_volumes_v) const;

  double calculer_dt_stab() const override;
  void calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const override;
  Motcle get_localisation_pour_post(const Nom& option) const override;
  virtual void remplir_nu(DoubleTab&) const;
  int phi_psi_diffuse(const Equation_base& eq) const;

  const Domaine_VEF& domaine_vef() const { return le_dom_vef.valeur(); }
  const Domaine_Cl_VEF& domaine_cl_vef() const { return la_zcl_vef.valeur(); }

protected:

  REF(Domaine_VEF) le_dom_vef;
  REF(Domaine_Cl_VEF) la_zcl_vef;
  REF(Champ_Inc_base) inconnue_;
  //DoubleVect porosite_face;
  mutable DoubleTab nu_;

private:
  template<typename _TYPE_> std::enable_if_t< std::is_same<_TYPE_, double>::value , double>
  inline diffu__(const int k, const int l, const int num_elem, const _TYPE_ &diffu) const { return diffu; }

  template<typename _TYPE_> std::enable_if_t< std::is_same<_TYPE_, TRUSTTab<double>>::value , double>
  inline diffu__(const int k, const int l, const int num_elem, const _TYPE_ &diffu) const { return diffu(num_elem, k * dimension + l); }

  template<typename _TYPE_> std::enable_if_t< std::is_same<_TYPE_, TRUSTArray<double>>::value , double>
  inline diffu__(const int k, const int l, const int num_elem, const _TYPE_ &diffu) const { return diffu[k * dimension + l]; }
};

// ATTENTION le diffu intervenant dans les fonctions n'est que LOCAL (on appelle d_nu apres)
// Fonction utile viscA
// nu <Si, Sj> / |K|
template<typename _TYPE_>
inline double Op_Diff_VEF_base::viscA(int i, int j, int num_elem, const _TYPE_ &diffu) const
{
  constexpr bool is_double = std::is_same<_TYPE_, double>::value;
  const Domaine_VEF& domaine = le_dom_vef.valeur();
  const IntTab& face_voisins = domaine.face_voisins();
  const DoubleTab& face_normales = domaine.face_normales();
  const DoubleVect& inverse_volumes = domaine.inverse_volumes();

  double DSiSj = 0.;
  if (is_double)
    {
      for (int k = 0; k < dimension; k++)
        DSiSj += diffu__(k, k, num_elem, diffu) * face_normales(i, k) * face_normales(j, k);
    }
  else
    {
      for (int k = 0; k < dimension; k++)
        for (int l = 0; l < dimension; l++)
          DSiSj += diffu__(k, l, num_elem, diffu) * face_normales(i, k) * face_normales(j, l);
    }

  if ((face_voisins(i, 0) == face_voisins(j, 0)) || (face_voisins(i, 1) == face_voisins(j, 1)))
    return -DSiSj * inverse_volumes(num_elem);
  else
    return DSiSj * inverse_volumes(num_elem);
}

template<typename _TYPE_>
KOKKOS_INLINE_FUNCTION double Op_Diff_VEF_base::viscA(int i, int j, int num_elem, const _TYPE_ &diffu, CIntTabView face_voisins_v, CDoubleTabView face_normales_v, CDoubleArrView inverse_volumes_v) const
{
  constexpr bool is_double = std::is_same<_TYPE_, double>::value;
  int dim = (int)face_normales_v.extent(1);
  double DSiSj = 0.;
  if (is_double)
    {
      for (int k = 0; k < dim; k++)
        DSiSj += diffu__(k, k, num_elem, diffu) * face_normales_v(i, k) * face_normales_v(j, k);
    }
  else
    {
      for (int k = 0; k < dim; k++)
        for (int l = 0; l < dim; l++)
          DSiSj += diffu__(k, l, num_elem, diffu) * face_normales_v(i, k) * face_normales_v(j, l);
    }

  if ((face_voisins_v(i, 0) == face_voisins_v(j, 0)) || (face_voisins_v(i, 1) == face_voisins_v(j, 1)))
    return -DSiSj * inverse_volumes_v(num_elem);
  else
    return DSiSj * inverse_volumes_v(num_elem);
}
#endif /* Op_Diff_VEF_base_included */
