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
// File:        Eval_Grad_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Divers
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Grad_VDF_Face_included
#define Eval_Grad_VDF_Face_included

#include <Neumann_sortie_libre.h>
#include <Eval_VDF_Face.h>
#include <Eval_Grad_VDF.h>

// .DESCRIPTION class Eval_Grad_VDF_Face
// Evaluateur VDF pour le gradient
// .SECTION voir aussi Eval_Grad_VDF
class Eval_Grad_VDF_Face : public Eval_Grad_VDF, public Eval_VDF_Face
{
public:
  inline int calculer_arete_bord() const { return 0; }
  inline int calculer_arete_fluide() const { return 0; }
  inline int calculer_arete_paroi() const { return 0; }
  inline int calculer_arete_paroi_fluide() const { return 0; }
  inline int calculer_arete_symetrie() const { return 0; }
  inline int calculer_arete_interne() const { return 0; }
  inline int calculer_arete_mixte() const { return 0; }
  inline int calculer_fa7_sortie_libre() const { return 1; }
  inline int calculer_arete_periodicite() const { return 0; }
  inline int calculer_arete_symetrie_paroi() const { return 0; }
  inline int calculer_arete_symetrie_fluide() const { return 0; }
  inline int calculer_arete_coin_fluide() const { return 0; }

  //************************
  // CAS SCALAIRE
  //************************

  inline double flux_fa7_sortie_libre(const DoubleTab&, int face, const Neumann_sortie_libre& la_cl, int num1) const { return la_cl.flux_impose(face-num1)*surface(face); }
  inline double flux_fa7_elem(const DoubleTab& inco, int elem, int fac1, int) const { return inco(elem)*surface(fac1); }
  inline double flux_arete_interne(const DoubleTab&, int, int, int, int) const { return 0; }
  inline double flux_arete_mixte(const DoubleTab&, int, int, int, int) const { return 0; }
  inline double flux_arete_paroi(const DoubleTab&, int, int, int, int) const { return 0; }
  inline double flux_arete_symetrie(const DoubleTab&, int, int, int, int) const { return 0; }
  inline double flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int) const { return 0; }
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int, double&, double&) const { /* do nothing */ }
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int, double&, double&) const { /* do nothing */ }
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int, double&, double&) const { /* do nothing */ }
  inline void flux_arete_coin_fluide(const DoubleTab&, int, int, int, int, double&, double&) const  { /* do nothing */ }
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int, int, double&, double&) const { /* do nothing */ }

  inline void coeffs_fa7_elem(int, int, int, double& , double& ) const { /* do nothing */ }
  inline void coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&, double& , double& ) const { /* do nothing */ }
  inline void coeffs_arete_interne(int, int, int, int, double& , double& ) const { /* do nothing */ }
  inline void coeffs_arete_mixte(int, int, int, int, double& , double& ) const { /* do nothing */ }
  inline void coeffs_arete_fluide(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }
  inline void coeffs_arete_paroi_fluide(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }
  inline void coeffs_arete_coin_fluide(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }
  inline void coeffs_arete_periodicite(int, int, int, int, double& , double& ) const { /* do nothing */ }
  inline void coeffs_arete_symetrie_fluide(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }
  inline void coeffs_arete_paroi(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }
  inline void coeffs_arete_symetrie(int, int, int, int, double& , double& , double& ) const { /* do nothing */ }
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, double& aii1_2, double& , double& ) const { /* do nothing */ }

  inline double secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&, int ) const { return 0; }
  inline double secmem_fa7_elem( int, int, int) const { return 0; }
  inline double secmem_arete_interne(int, int, int, int) const { return 0; }
  inline double secmem_arete_mixte(int, int, int, int) const { return 0; }
  inline double secmem_arete_symetrie(int, int, int, int) const { return 0; }
  inline double secmem_arete_paroi(int, int, int, int ) const { return 0; }
  inline double secmem_arete_symetrie_paroi(int, int, int, int ) const { return 0; }
  inline void secmem_arete_fluide(int, int, int, int, double&, double&) const { /* do nothing */ }
  inline void secmem_arete_paroi_fluide(int, int, int, int, double&, double&) const { /* do nothing */ }
  inline void secmem_arete_coin_fluide(int, int, int, int, double&, double&) const { /* do nothing */ }
  inline void secmem_arete_symetrie_fluide(int, int, int, int, double&, double&) const { /* do nothing */ }
  inline void secmem_arete_periodicite(int, int, int, int, double&, double&) const { /* do nothing */ }

  //************************
  // CAS VECTORIEL
  //************************

  inline void flux_fa7_elem(const DoubleTab&, int, int, int, DoubleVect& flux) const;
  inline void flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&, int, DoubleVect& flux) const;
  inline void flux_arete_interne(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }
  inline void flux_arete_mixte(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }
  inline void flux_arete_symetrie(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }
  inline void flux_arete_paroi(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int, DoubleVect& ,DoubleVect&) const { /* do nothing */ }
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int, DoubleVect& ,DoubleVect&) const { /* do nothing */ }
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect& ) const { /* do nothing */ }
  inline void flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }
  inline void flux_arete_coin_fluide(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }

  inline void coeffs_fa7_elem(int, int, int, DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&, DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_arete_interne(int, int, int, int, DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_arete_mixte(int, int, int, int, DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_arete_fluide(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_arete_paroi_fluide(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_arete_periodicite(int, int, int, int, DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_arete_symetrie_fluide(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_arete_paroi(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_arete_symetrie(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }
  inline void coeffs_arete_coin_fluide(int, int, int, int, DoubleVect& , DoubleVect& , DoubleVect& ) const { /* do nothing */ }

  inline void secmem_arete_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }
  inline void secmem_arete_paroi_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }
  inline void secmem_arete_symetrie_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }
  inline void secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&, int, DoubleVect&) const { /* do nothing */ }
  inline void secmem_fa7_elem(int, int, int, DoubleVect& flux) const { /* do nothing */ }
  inline void secmem_arete_interne(int, int, int, int, DoubleVect& ) const { /* do nothing */ }
  inline void secmem_arete_mixte(int, int, int, int, DoubleVect& ) const { /* do nothing */ }
  inline void secmem_arete_symetrie(int, int, int, int, DoubleVect& ) const { /* do nothing */ }
  inline void secmem_arete_paroi(int, int, int, int, DoubleVect& ) const { /* do nothing */ }
  inline void secmem_arete_periodicite(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }
  inline void secmem_arete_symetrie_paroi(int, int, int, int, DoubleVect& ) const { /* do nothing */ }
  inline void secmem_arete_coin_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }
};

// flux_fa7_elem
inline void Eval_Grad_VDF_Face::flux_fa7_elem(const DoubleTab& inco, int elem, int fac1, int, DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++) flux(k) = inco(elem,k)*surface(fac1);
}

// flux_fa7_sortie_libre
inline void Eval_Grad_VDF_Face::flux_fa7_sortie_libre(const DoubleTab& , int face, const Neumann_sortie_libre& la_cl, int num1,DoubleVect& flux) const
{
  for (int k=0; k<flux.size(); k++) flux(k) = la_cl.flux_impose(face-num1,k)*surface(face);
}

#endif /* Eval_Grad_VDF_Face_included */
