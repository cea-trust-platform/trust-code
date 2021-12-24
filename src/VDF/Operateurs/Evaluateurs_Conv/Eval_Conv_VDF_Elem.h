/****************************************************************************
 * Copyright (c) 2021, CEA
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
// File:        Eval_Conv_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Conv
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Conv_VDF_Elem_included
#define Eval_Conv_VDF_Elem_included

#include <CL_Types_include.h>
#include <Eval_VDF_Elem.h>

template <typename DERIVED_T>
class Eval_Conv_VDF_Elem : public Eval_VDF_Elem
{
public:
  static constexpr bool IS_AMONT = false, IS_CENTRE = false, IS_CENTRE4 = false, IS_QUICK = false;
  // Overload Eval_VDF_Elem
  static constexpr bool CALC_FLUX_FACES_ECH_EXT_IMP = false, CALC_FLUX_FACES_ECH_GLOB_IMP = false, CALC_FLUX_FACES_PAR = false,
                        CALC_FLUX_FACES_PAR_FIXE = false, CALC_FLUX_FACES_SORTIE_LIB = true;

  // CRTP pattern to static_cast the appropriate class and get the implementation
  // This is magic !
  inline int elem_(int i, int j) const;
  inline double dt_vitesse(int face) const;
  inline double surface_porosite(int face) const;
  inline int amont_amont_(int face, int i) const;
  inline double quick_fram_(const double&, const int, const int, const int, const int, const int, const DoubleTab& ) const;
  inline void quick_fram_(const double&, const int, const int, const int, const int, const int, const DoubleTab&, ArrOfDouble& ) const;
  inline double qcentre_(const double&, const int, const int, const int, const int, const int, const DoubleTab& ) const;
  inline void qcentre_(const double&, const int, const int, const int, const int, const int, const DoubleTab&, ArrOfDouble& ) const;

  //************************
  // CAS SCALAIRE
  //************************

  // Generic return
  template<typename BC>
  inline double flux_face(const DoubleTab&, int , const BC&, int ) const { return 0; }

  // To overload
  inline double flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const;
  inline double flux_face(const DoubleTab&, int , const Periodique&, int ) const;
  inline double flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const { return 0; }
  inline double flux_faces_interne(const DoubleTab&, int ) const;

  // Generic return
  template<typename BC>
  inline void coeffs_face(int,int, const BC&, double& aii, double& ajj ) const { /* Do nothing */ }

  // To overload
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_sortie_libre&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Periodique&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, double& aii, double& ajj ) const { /* Do nothing */ }
  inline void coeffs_faces_interne(int, double& aii, double& ajj ) const;

  // contribution de la derivee en vitesse d'une equation scalaire
  // Generic return
  template <typename BC>
  inline void coeffs_face_bloc_vitesse(const DoubleTab&, int , const BC&, int , DoubleVect&) const { /* Do nothing */ }
  // Overloaded
  inline void coeffs_face_bloc_vitesse(const DoubleTab&, int , const Dirichlet_entree_fluide&, int , DoubleVect&) const;
  inline void coeffs_face_bloc_vitesse(const DoubleTab&, int , const Neumann_sortie_libre&, int , DoubleVect&) const;
  inline void coeffs_face_bloc_vitesse(const DoubleTab&, int , const Periodique&, int , DoubleVect&) const;
  inline void coeffs_face_bloc_vitesse(const DoubleTab&, int , int, int, const Echange_externe_impose&, int , DoubleVect&) const { /* Do nothing */ }
  inline void coeffs_faces_interne_bloc_vitesse(const DoubleTab&, int , DoubleVect&) const;

  // Generic return
  template<typename BC>
  inline double secmem_face(int, const BC&, int ) const { return 0; }

  // To overload
  inline double secmem_face(int, const Dirichlet_entree_fluide&, int ) const;
  inline double secmem_face(int, const Neumann_sortie_libre&, int ) const;
  inline double secmem_face(int, int, int, const Echange_externe_impose&, int ) const { return 0; }
  inline double secmem_faces_interne(int ) const { return 0; };

  //************************
  // CAS VECTORIEL
  //************************

  // Generic return
  template <typename BC>
  inline void flux_face(const DoubleTab&,int , const BC&,int, DoubleVect& flux) const { /* Do nothing */ }
  // To overload

  inline void flux_face(const DoubleTab&, int , const Periodique&, int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Neumann_sortie_libre&, int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , const Dirichlet_entree_fluide&, int, DoubleVect& flux) const;
  inline void flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void flux_faces_interne(const DoubleTab&, int , DoubleVect& flux) const;

  // Generic return
  template <typename BC>
  inline void coeffs_face(int, int,const BC&, DoubleVect& aii, DoubleVect& ajj ) const { /* Do nothing */ }

  // To overload
  inline void coeffs_face(int,int, const Periodique&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, DoubleVect& aii, DoubleVect& ajj ) const;
  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, DoubleVect& aii, DoubleVect& ajj ) const { /* Do nothing */ }
  inline void coeffs_faces_interne(int, DoubleVect& aii, DoubleVect& ajj ) const;

  // Generic return
  template <typename BC>
  inline void secmem_face(int, const BC&, int, DoubleVect& ) const { /* Do nothing */ }

  // To overload
  inline void secmem_face(int, const Neumann_sortie_libre&, int, DoubleVect& ) const;
  inline void secmem_face(int, const Dirichlet_entree_fluide&, int, DoubleVect& ) const;
  inline void secmem_face(int, int, int, const Echange_externe_impose&, int, DoubleVect& ) const { /* Do nothing */ }
  inline void secmem_faces_interne(int, DoubleVect& ) const { /* Do nothing */ }
};

#include <Eval_Conv_VDF_Elem.tpp> // templates specializations ici ;)

#endif /* Eval_Conv_VDF_Elem_included */
