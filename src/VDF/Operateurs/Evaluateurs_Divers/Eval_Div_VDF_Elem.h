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
// File:        Eval_Div_VDF_Elem.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Divers
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Div_VDF_Elem_included
#define Eval_Div_VDF_Elem_included

#include <CL_Types_include.h>
#include <Eval_VDF_Elem2.h>
#include <Eval_Div_VDF.h>

// .DESCRIPTION class Eval_Div_VDF_Elem
// Evaluateur VDF pour la divergence
class Eval_Div_VDF_Elem : public Eval_Div_VDF, public Eval_VDF_Elem2
{
public:
  inline int calculer_flux_faces_echange_externe_impose() const { return 1; }
  inline int calculer_flux_faces_echange_global_impose() const { return 1; }
  inline int calculer_flux_faces_entree_fluide() const { return 1; }
  inline int calculer_flux_faces_paroi() const { return 0; }
  inline int calculer_flux_faces_paroi_adiabatique() const { return 0; }
  inline int calculer_flux_faces_paroi_defilante() const { return 0; }
  inline int calculer_flux_faces_paroi_fixe() const { return 0; }
  inline int calculer_flux_faces_sortie_libre() const { return 1; }
  inline int calculer_flux_faces_symetrie() const { return 0; }
  inline int calculer_flux_faces_periodique() const { return 1; }
  inline int calculer_flux_faces_NSCBC() const { return 1; }

  //************************
  // CAS SCALAIRE
  //************************

  // Generic return
  template<typename BC>
  inline double flux_face(const DoubleTab& vit, int face, const BC&, int ) const { return vit(face)*surface(face)*porosite(face); }

  inline double flux_face(const DoubleTab&, int , const Symetrie&, int ) const { return 0; }
  inline double flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&, int ) const { return 0; }
  inline double flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int ) const { return 0; }
  inline double flux_face(const DoubleTab& vit, int , int face, int, const Echange_externe_impose&, int ) const { return vit(face)*surface(face)*porosite(face); }
  inline double flux_faces_interne(const DoubleTab& vit, int face) const { return vit(face)*surface(face)*porosite(face); }

  template<typename BC>
  inline void coeffs_face(int,int, const BC&, double& aii, double& ajj ) const { /* Do nothing */ }

  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, double& aii, double& ajj ) const { /* Do nothing */ }
  inline void coeffs_faces_interne(int, double& aii, double& ajj ) const { /* Do nothing */ }

  template<typename BC>
  inline double secmem_face(int, const BC&, int ) const { return 0; }

  inline double secmem_face(int, int, int, const Echange_externe_impose&, int ) const { return 0; }
  inline double secmem_faces_interne(int ) const { return 0; };

  //************************
  // CAS VECTORIEL
  //************************

  // Generic return
  template <typename BC>
  inline void flux_face(const DoubleTab&,int , const BC&,int, DoubleVect& flux) const;

  inline void flux_face(const DoubleTab&, int , const Symetrie&, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_fixe&, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void flux_face(const DoubleTab&, int , const Dirichlet_paroi_defilante&, int, DoubleVect& flux) const { /* Do nothing */ }
  inline void flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int, DoubleVect& flux) const;
  inline void flux_faces_interne(const DoubleTab&, int , DoubleVect& flux) const;

  template <typename BC>
  inline void coeffs_face(int, int,const BC&, DoubleVect& aii, DoubleVect& ajj ) const { /* Do nothing */ }

  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, DoubleVect& aii, DoubleVect& ajj ) const { /* Do nothing */ }
  inline void coeffs_faces_interne(int, DoubleVect& aii, DoubleVect& ajj ) const { /* Do nothing */ }

  template <typename BC>
  inline void secmem_face(int, const BC&, int, DoubleVect& ) const { /* Do nothing */ }

  inline void secmem_face(int, int, int, const Echange_externe_impose&, int, DoubleVect& ) const { /* Do nothing */ }
  inline void secmem_faces_interne(int, DoubleVect& ) const { /* Do nothing */ }
};

// Generic return
template <typename BC>
inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab& inco,int face, const BC&,int, DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++) flux(k) = inco(face,k)*surface(face)*porosite(face);
}

// flux_face avec Echange_externe_impose
inline void Eval_Div_VDF_Elem::flux_face(const DoubleTab& inco, int boundary_index, int face, int local_face, const Echange_externe_impose&, int, DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++) flux(k) = inco(face,k)*surface(face)*porosite(face);
}

// flux_faces_interne
inline void Eval_Div_VDF_Elem::flux_faces_interne(const DoubleTab& inco, int face,DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++) flux(k) = inco(face,k)*surface(face)*porosite(face);
}

#endif /* Eval_Div_VDF_Elem_included */
