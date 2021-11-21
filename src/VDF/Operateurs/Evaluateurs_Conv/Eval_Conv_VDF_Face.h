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
// File:        Eval_Conv_VDF_Face.h
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Conv
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Conv_VDF_Face_included
#define Eval_Conv_VDF_Face_included

#include <Neumann_sortie_libre.h>
#include <Eval_VDF_Face2.h>

template <typename DERIVED_T>
class Eval_Conv_VDF_Face : public Eval_VDF_Face2
{
public:
  static constexpr bool IS_AMONT = false;
  static constexpr bool IS_CENTRE = false;
  static constexpr bool IS_CENTRE4 = false;
  static constexpr bool IS_QUICK = false;

  // CRTP pattern to static_cast the appropriate class and get the implementation
  // This is magic !
  inline int premiere_face_bord() const;
  inline int orientation(int face) const;
  inline int elem_(int i, int j) const;
  inline int face_amont_princ_(int num_face,int i) const;
  inline int face_amont_conj_(int num_face,int i,int k) const;
  inline double dt_vitesse(int face) const;
  inline double surface_porosite(int face) const;
  inline double surface(int face) const;
  inline double porosite(int face) const;
  inline double dim_face_(int n1,int k) const;
  inline double dim_elem_(int n1,int k) const;
  inline double dist_face_(int n1,int n2,int k) const;
  inline double dist_face_period_(int n1,int n2,int k) const;
  inline double dist_elem_period_(int n1, int n2, int k) const;
  inline double conv_quick_sharp_plus_(const double& ,const double& , const double& , const double& , const double& , const double& , const double& )const ;
  inline double conv_quick_sharp_moins_(const double& ,const double& ,const double& , const double& ,const double& , const double& ,const double& ) const;
  inline const Zone_Cl_VDF& la_zcl() const;



  inline int calculer_arete_fluide() const { return 1; }
  inline int calculer_arete_paroi() const { return 0; }
  inline int calculer_arete_paroi_fluide() const { return 1; }
  inline int calculer_arete_coin_fluide() const { return 1; }
  inline int calculer_arete_symetrie() const { return 0; }
  inline int calculer_arete_interne() const { return 1; }
  inline int calculer_arete_mixte() const { return 1; }
  inline int calculer_fa7_sortie_libre() const { return 1; }
  inline int calculer_arete_periodicite() const { return 1; }
  inline int calculer_arete_symetrie_paroi() const { return 0; }
  inline int calculer_arete_symetrie_fluide() const { return 1; }

  //************************
  // CAS SCALAIRE
  //************************

  inline double flux_fa7_elem(const DoubleTab&, int, int, int) const ;
  inline double flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const;
  inline double flux_arete_interne(const DoubleTab&, int, int, int, int ) const;
  inline double flux_arete_mixte(const DoubleTab&, int, int, int, int ) const;
  inline double flux_arete_paroi(const DoubleTab&, int, int, int, int ) const { return 0; }
  inline double flux_arete_symetrie(const DoubleTab&, int, int, int, int ) const { return 0; }
  inline double flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int) const { return 0; }
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline void flux_arete_coin_fluide(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int, double&, double&) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int, int, double&, double&) const ;

  inline void coeffs_fa7_elem(int, int, int, double& aii, double& ajj) const;
  inline void coeffs_fa7_sortie_libre(int, const Neumann_sortie_libre&, double& aii, double& ajj) const;
  inline void coeffs_arete_interne(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_paroi_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_coin_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_periodicite(int, int, int, int, double& aii, double& ajj) const;
  inline void coeffs_arete_symetrie_fluide(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const;
  inline void coeffs_arete_paroi(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const { /* do nothing */ }
  inline void coeffs_arete_symetrie(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const { /* do nothing */ }
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, double& aii1_2, double& aii3_4, double& ajj1_2) const { /* do nothing */ }

  inline double secmem_fa7_sortie_libre(int, const Neumann_sortie_libre&, int ) const;
  inline double secmem_fa7_elem( int, int, int) const { return 0; }
  inline double secmem_arete_interne(int, int, int, int) const { return 0; }
  inline double secmem_arete_mixte(int, int, int, int) const { return 0; }
  inline double secmem_arete_symetrie(int, int, int, int) const { return 0; }
  inline double secmem_arete_paroi(int, int, int, int ) const { return 0; }
  inline double secmem_arete_symetrie_paroi(int, int, int, int ) const { return 0; }
  inline void secmem_arete_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_paroi_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_coin_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_symetrie_fluide(int, int, int, int, double&, double&) const;
  inline void secmem_arete_periodicite(int, int, int, int, double&, double&) const { /* do nothing */ }

  //************************
  // CAS VECTORIEL
  //************************

  inline void flux_fa7_elem(const DoubleTab&, int, int, int, DoubleVect& flux) const;
  inline void flux_fa7_sortie_libre(const DoubleTab&, int , const Neumann_sortie_libre&, int, DoubleVect& flux) const;
  inline void flux_arete_interne(const DoubleTab&, int, int, int, int, DoubleVect& flux) const ;
  inline void flux_arete_mixte(const DoubleTab&, int, int, int, int, DoubleVect& flux) const ;
  inline void flux_arete_fluide(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect& ) const;
  inline void flux_arete_paroi_fluide(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect& ) const ;
  inline void flux_arete_periodicite(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect& ) const ;
  inline void flux_arete_symetrie_fluide(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect& ) const;
  inline void flux_arete_paroi(const DoubleTab&, int, int, int, int, DoubleVect& ) const { /* do nothing */ }
  inline void flux_arete_symetrie(const DoubleTab&, int, int, int, int, DoubleVect& ) const { /* do nothing */ }
  inline void flux_arete_symetrie_paroi(const DoubleTab&, int, int, int, int, DoubleVect& flux) const { /* do nothing */ }
  inline void flux_arete_coin_fluide(const DoubleTab&, int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    Cerr << "ToDo: code like Eval_Conv_VDF_Face::flux_arete_coin_fluide for a scalar inco" << finl;
    Process::exit();
  }

  inline void coeffs_fa7_elem(int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_fa7_sortie_libre(int , const Neumann_sortie_libre&, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_interne(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_mixte(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_paroi_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_periodicite(int, int, int, int, DoubleVect& aii, DoubleVect& ajj) const;
  inline void coeffs_arete_symetrie_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const;
  inline void coeffs_arete_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const { /* do nothing */ }
  inline void coeffs_arete_symetrie(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const { /* do nothing */ }
  inline void coeffs_arete_symetrie_paroi(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const { /* do nothing */ }
  inline void coeffs_arete_coin_fluide(int, int, int, int, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
  {
    Cerr << "ToDo: code like Eval_Conv_VDF_Face::coeffs_arete_coin_fluide for a scalar inco" << finl;
    Process::exit();
  }

  inline void secmem_arete_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_paroi_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_arete_symetrie_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const;
  inline void secmem_fa7_sortie_libre(int , const Neumann_sortie_libre&, int, DoubleVect& flux) const;
  inline void secmem_fa7_elem(int, int, int, DoubleVect& flux) const { /* do nothing */ }
  inline void secmem_arete_interne(int, int, int, int, DoubleVect& flux) const { /* do nothing */ }
  inline void secmem_arete_mixte(int, int, int, int, DoubleVect& flux) const { /* do nothing */ }
  inline void secmem_arete_symetrie(int, int, int, int, DoubleVect& ) const { /* do nothing */ }
  inline void secmem_arete_paroi(int, int, int, int, DoubleVect& ) const { /* do nothing */ }
  inline void secmem_arete_periodicite(int, int, int, int, DoubleVect&, DoubleVect&) const { /* do nothing */ }
  inline void secmem_arete_symetrie_paroi(int, int, int, int, DoubleVect& ) const { /* do nothing */ }
  inline void secmem_arete_coin_fluide(int, int, int, int, DoubleVect&, DoubleVect&) const
  {
    Cerr << "ToDo: code like Eval_Conv_VDF_Face::secmem_arete_coin_fluide for a scalar inco" <<finl;
    Process::exit();
  }
};

//************************
// CRTP pattern
//************************

template <typename DERIVED_T>
inline int Eval_Conv_VDF_Face<DERIVED_T>::premiere_face_bord() const
{
  return static_cast<const DERIVED_T *>(this)->get_premiere_face_bord();
}

template <typename DERIVED_T>
inline int Eval_Conv_VDF_Face<DERIVED_T>::orientation(int face) const
{
  return static_cast<const DERIVED_T *>(this)->get_orientation(face);
}

template <typename DERIVED_T>
inline int Eval_Conv_VDF_Face<DERIVED_T>::elem_(int i, int j) const
{
  return static_cast<const DERIVED_T *>(this)->get_elem(i, j);
}

template <typename DERIVED_T>
inline int Eval_Conv_VDF_Face<DERIVED_T>::face_amont_princ_(int num_face,int i) const
{
  return static_cast<const DERIVED_T *>(this)->face_amont_princ(num_face,i);
}

template <typename DERIVED_T>
inline int Eval_Conv_VDF_Face<DERIVED_T>::face_amont_conj_(int num_face,int i,int k) const
{
  return static_cast<const DERIVED_T *>(this)->face_amont_conj(num_face,i,k);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::dt_vitesse(int face) const
{
  return static_cast<const DERIVED_T *>(this)->get_dt_vitesse(face);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::surface_porosite(int face) const
{
  return static_cast<const DERIVED_T *>(this)->get_surface_porosite(face);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::surface(int face) const
{
  return static_cast<const DERIVED_T *>(this)->get_surface(face);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::porosite(int face) const
{
  return static_cast<const DERIVED_T *>(this)->get_porosite(face);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::dim_face_(int n1,int k) const
{
  return static_cast<const DERIVED_T *>(this)->dim_face(n1,k);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::dim_elem_(int n1,int k) const
{
  return static_cast<const DERIVED_T *>(this)->dim_elem(n1,k);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::dist_face_(int n1,int n2,int k) const
{
  return static_cast<const DERIVED_T *>(this)->dist_face(n1,n2,k);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::dist_face_period_(int n1,int n2,int k) const
{
  return static_cast<const DERIVED_T *>(this)->dist_face_period(n1,n2,k);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::dist_elem_period_(int n1, int n2, int k) const
{
  return static_cast<const DERIVED_T *>(this)->dist_elem_period(n1,n2,k);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::conv_quick_sharp_plus_(const double& psc,const double& vit_0, const double& vit_1,
                                                                    const double& vit_0_0, const double& dx, const double& dm, const double& dxam) const
{
  return static_cast<const DERIVED_T *>(this)->conv_quick_sharp_plus(psc,vit_0,vit_1,vit_0_0,dx,dm,dxam);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::conv_quick_sharp_moins_(const double& psc,const double& vit_0,const double& vit_1,
                                                                     const double& vit_1_1,const double& dx, const double& dm,const double& dxam) const
{
  return static_cast<const DERIVED_T *>(this)->conv_quick_sharp_moins(psc,vit_0,vit_1,vit_1_1,dx,dm,dxam);
}

template <typename DERIVED_T>
inline const Zone_Cl_VDF& Eval_Conv_VDF_Face<DERIVED_T>::la_zcl() const
{
  return static_cast<const DERIVED_T *>(this)->get_la_zcl();
}

//************************
// CAS SCALAIRE
//************************

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_fluide(const DoubleTab& inco,int fac1, int fac2, int fac3, int signe,double& flux3, double& flux1_2) const
{
  double flux, psc;

  // Calcul de flux3:
  psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2)) * (surface(fac1)+surface(fac2)));

  if ((psc*signe)>0) flux=inco(fac3)*psc ;
  else
    {
      const int pfb = premiere_face_bord(), ori = orientation(fac3);
      // TODO : FIXME : euh ? pourquoi ca ?
      const int rang1 = DERIVED_T::IS_QUICK ? fac1 : (fac1-pfb), rang2 = DERIVED_T::IS_QUICK ? fac2 : (fac2-pfb);
      flux = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl())
                  + Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl()))* psc ;
    }
  flux3 = -flux;

  // Calcul de flux1_2:
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  flux = (psc>0) ? psc*inco(fac1) : psc*inco(fac2);
  flux1_2 = -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_coin_fluide(const DoubleTab& inco, int fac1, int , int fac3, int signe, double& flux3, double& flux1_2) const
{
  double flux, psc;
  // Calcul de flux3:
  psc = 0.5 * dt_vitesse(fac1) * porosite(fac1) * surface(fac1);

  if ((psc * signe) > 0) flux = inco(fac3) * psc;
  else
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1 - pfb), ori = orientation(fac3);
      flux = Champ_Face_get_val_imp_face_bord(inconnue->temps(), rang1, ori, la_zcl()) * psc;
    }
  flux3 = -flux;

  // Calcul de flux1_2:
  psc = 0.5 * dt_vitesse(fac3) * surface(fac3) * porosite(fac3);
  if (psc > 0)
    flux = psc * inco(fac1);
  else
    {
      const int pfb = premiere_face_bord(), rang3 = (fac3 - pfb), ori = orientation(fac1);
      flux = psc * Champ_Face_get_val_imp_face_bord(inconnue->temps(), rang3, ori, la_zcl());
    }
  flux1_2 = -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_fluide(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4, double& ajj1_2) const
{
  double psc;
  // Calcul de flux3:
  psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+ dt_vitesse(fac2)*porosite(fac2)) * (surface(fac1)+surface(fac2)));
  aii3_4 = ((psc*signe)>0) ? psc : 0;

  // Calcul de flux1_2:
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    {
      aii1_2 = psc;
      ajj1_2 = 0;
    }
  else
    {
      ajj1_2 = -psc;
      aii1_2 = 0;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_coin_fluide(int fac1, int, int fac3, int signe, double& aii1_2, double& aii3_4, double& ajj1_2) const
{
  double psc;
  // Calcul de flux3:
  psc = 0.5*dt_vitesse(fac1)*porosite(fac1)*surface(fac1);
  aii3_4 = ((psc*signe)>0) ? psc : 0;

  // Calcul de flux1_2:
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    {
      aii1_2 = psc;
      ajj1_2 = 0;
    }
  else
    {
      ajj1_2 = -psc;
      aii1_2 = 0;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete_fluide(int fac1, int fac2, int fac3, int signe,  double& flux3, double& flux1_2) const
{
  double psc;
  // Calcul de flux3:
  psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)<0)
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), rang2 = (fac2-pfb), ori = orientation(fac3);
      flux3 = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl())
                    + Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl()))* psc ;
    }
  else flux3 = 0;

  flux1_2 = 0;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete_coin_fluide(int fac1, int , int fac3, int signe, double& flux3, double& flux1_2) const
{
  double psc;
  // Calcul de flux3:
  psc = 0.5*dt_vitesse(fac1)*porosite(fac1)*surface(fac1);
  if ((psc*signe)<0)
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), ori = orientation(fac3);
      flux3 = - Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl()) * psc ;
    }
  else flux3 = 0;

  flux1_2 = 0;
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_interne(const DoubleTab& inco , int fac1, int fac2, int fac3, int fac4) const
{
  double flux;
  double psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2));
  if (DERIVED_T::IS_AMONT) flux = (psc>0) ? psc*inco(fac3) : psc*inco(fac4);
  else
    {
      const int ori = orientation(fac1), num0_0 = face_amont_conj_(fac3,ori,0), num1_1 = face_amont_conj_(fac4,ori,1);
      if (psc > 0)
        {
          if (num0_0 == -1) flux = psc*inco(fac3); // Schema amont
          else // Schema quick
            {
              const double dx = dist_face_period_(fac3,fac4,ori), dm = dim_face_(fac3,ori), dxam = dist_face_period_(num0_0,fac3,ori);
              const double vit_0 = inco(fac3), vit_0_0 = inco[num0_0], vit_1 = inco(fac4);
              flux = conv_quick_sharp_plus_(psc,vit_0,vit_1,vit_0_0,dx,dm,dxam);
            }
        }
      else // (psc <= 0)
        {
          if (num1_1 == -1) flux = psc*inco(fac4); // Schema amont
          else // Schema quick
            {
              const double dx = dist_face_period_(fac3,fac4,ori), dm = dim_face_(fac4,ori), dxam = dist_face_period_(fac4,num1_1,ori);
              const double vit_0 = inco(fac3), vit_1 = inco(fac4), vit_1_1 = inco(num1_1);
              flux = conv_quick_sharp_moins_(psc,vit_0,vit_1,vit_1_1,dx,dm,dxam);
            }
        }
    }
  return -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_interne(int fac1, int fac2, int fac3, int fac4, double& aii, double& ajj) const
{
  double psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2));
  if (psc>0)
    {
      aii = psc;
      ajj=0;
    }
  else
    {
      ajj = -psc;
      aii=0;
    }
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_mixte(const DoubleTab& inco , int fac1, int fac2, int fac3, int fac4) const
{
  double flux;
  double psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) + surface(fac2));
  flux = (psc>0) ? psc*inco(fac3) : psc*inco(fac4);

  return -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_mixte(int fac1, int fac2, int fac3, int fac4, double& aii, double& ajj) const
{
  double psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) + surface(fac2));
  if (psc>0)
    {
      aii = psc;
      ajj = 0;
    }
  else
    {
      ajj = -psc;
      aii = 0;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_paroi_fluide(const DoubleTab& inco, int fac1, int fac2, int fac3, int signe, double& flux3 , double& flux1_2) const
{
  double psc,flux;
  // Calcul de flux3:
  psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) + surface(fac2));
  if ((psc*signe)>0) flux=inco(fac3)*psc ;
  else
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), rang2 = (fac2-pfb), ori = orientation(fac3);
      flux = 0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl())
                  +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl()))* psc ;
    }
  flux3 = -flux;

  // Calcul de flux1_2:
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  flux = (psc>0) ? psc*inco(fac1) : psc*inco(fac2);
  flux1_2 = -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4, double& ajj1_2) const
{
  double psc;
  psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) + surface(fac2));
  aii3_4 = ((psc*signe)>0) ? psc : 0;

  // Calcul des coefficients entre les faces 1 et 2.
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    {
      aii1_2 = psc;
      ajj1_2 = 0;
    }
  else
    {
      ajj1_2 = -psc;
      aii1_2 = 0;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete_paroi_fluide(int fac1, int fac2, int fac3, int signe, double& flux3 , double& flux1_2) const
{
  double psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) + surface(fac2));
  if ((psc*signe)<0)
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), rang2 = (fac2-pfb), ori = orientation(fac3);
      flux3 = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl())
                    +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl()))* psc ;
    }
  else flux3 = 0;
  flux1_2 = 0;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_periodicite(const DoubleTab& inco, int fac1, int fac2, int fac3, int fac4, double& flux3_4 , double& flux1_2) const
{
  if (DERIVED_T::IS_QUICK) // XXX : LOL
    {
      flux3_4 = flux_arete_interne(inco,fac1, fac2,fac3, fac4) ;
      flux1_2 = flux_arete_interne(inco,fac3, fac4,fac1, fac2) ;
      return;
    }

  double flux, psc;
  // On calcule le flux convectif entre les volumes de controle associes a fac3 et fac4:
  psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) + surface(fac2));
  flux = (psc>0) ? psc*inco(fac3) : psc*inco(fac4);
  flux3_4 = -flux;

  // On calcule le flux convectif entre les volumes de controle associes a fac1 et fac2:
  psc = 0.25*(dt_vitesse(fac3)*porosite(fac3)+dt_vitesse(fac4)*porosite(fac4))*(surface(fac3)+surface(fac4));
  flux = (psc>0) ? psc*inco(fac1) : psc*inco(fac2);
  flux1_2 = -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_periodicite(int fac1, int fac2, int fac3, int fac4, double& aii , double& ajj) const
{
  double psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) + surface(fac2));
  if (psc>0)
    {
      aii = psc;
      ajj = 0;
    }
  else
    {
      ajj = -psc;
      aii = 0;
    }
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::flux_fa7_elem(const DoubleTab& inco, int num_elem, int fac1, int fac2) const
{
  double flux, psc = 0.25 * (dt_vitesse(fac1)+dt_vitesse(fac2))*(surface(fac1)+surface(fac2));
  if (DERIVED_T::IS_AMONT) flux = (psc>0) ? psc * inco(fac1)*porosite(fac1) : psc * inco(fac2)*porosite(fac2);
  else
    {
      const int num0_0 = face_amont_princ_(fac1,0), num1_1 = face_amont_princ_(fac2,1);
      if  (psc > 0)
        {
          if (num0_0 == -1) flux = psc*inco[fac1]*porosite(fac1); // Schema amont
          else
            {
              const int ori = orientation(fac1), elem_amont = elem_(fac1,0);
              const double dx = dim_elem_(num_elem,ori), dm = dist_elem_period_(elem_amont,num_elem,ori);
              const double dxam = dim_elem_(elem_amont,ori), vit_0 = inco[fac1]*porosite(fac1),
                           vit_0_0 = inco[num0_0]*porosite(num0_0), vit_1 = inco[fac2]*porosite(fac2);
              flux = conv_quick_sharp_plus_(psc,vit_0,vit_1,vit_0_0,dx,dm,dxam);
            }
        }
      else // (psc < 0)
        {
          if (num1_1 == -1) flux = psc*inco[fac2]*porosite(fac2); // Schema amont
          else
            {
              const int ori = orientation(fac2), elem_amont = elem_(fac2,1);
              const double dx = dim_elem_(num_elem,ori), dm = dist_elem_period_(num_elem,elem_amont,ori);
              const double dxam = dim_elem_(elem_amont,ori), vit_0 = inco[fac1]*porosite(fac1),
                           vit_1 = inco[fac2]*porosite(fac2), vit_1_1 = inco[num1_1]*porosite(num1_1);
              flux = conv_quick_sharp_moins_(psc,vit_0,vit_1,vit_1_1,dx,dm,dxam);
            }
        }
    }
  return -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_fa7_elem(int, int fac1, int fac2, double& aii, double& ajj) const
{
  double psc = 0.25 * (dt_vitesse(fac1)+dt_vitesse(fac2)) * (surface(fac1)+surface(fac2));
  if (psc>0)
    {
      aii = psc*porosite(fac1);
      ajj = 0;
    }
  else
    {
      ajj = -psc*porosite(fac2);
      aii = 0;
    }
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::flux_fa7_sortie_libre(const DoubleTab& inco, int face, const Neumann_sortie_libre& la_cl, int num1) const
{
  const int elem1 = elem_(face,0);
  double flux, psc = dt_vitesse(face)*surface(face);
  if (DERIVED_T::IS_QUICK) // TODO : FIXME : pareil mais on corrige apres
    {
      if (elem1 != -1) flux = (psc > 0) ? psc*inco(face) : psc*la_cl.val_ext(face-num1,orientation(face));
      else flux = (psc < 0) ? psc*inco(face) : psc*la_cl.val_ext(face-num1,orientation(face));
    }
  else
    {
      if (elem1 != -1) flux = (psc > 0) ? psc*inco(face)*porosite(face) : psc*la_cl.val_ext(face-num1,orientation(face));
      else flux = (psc < 0) ? psc*inco(face)*porosite(face) : psc*la_cl.val_ext(face-num1,orientation(face));
    }
  return -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_fa7_sortie_libre(int face, const Neumann_sortie_libre& la_cl, double& aii, double& ajj) const
{
  double psc = dt_vitesse(face)*surface(face);
  if (psc > 0)
    {
      aii = psc*porosite(face);
      ajj = 0;
    }
  else
    {
      ajj = -psc*porosite(face);
      aii = 0;
    }
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Face<DERIVED_T>::secmem_fa7_sortie_libre(int face, const Neumann_sortie_libre& la_cl, int num1) const
{
  const int i = elem_(face,0);
  double flux, psc = dt_vitesse(face)*surface(face);

  if (i != -1) flux = (psc < 0) ?  psc*la_cl.val_ext(face-num1,orientation(face)) : 0;
  else flux = (psc > 0) ? psc*la_cl.val_ext(face-num1,orientation(face)) : 0;

  return -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_symetrie_fluide(const DoubleTab& inco, int fac1, int fac2, int fac3, int signe, double& flux3, double& flux1_2) const
{
  double flux,psc;
  // Calcul de flux3:
  psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0) flux=inco(fac3)*psc ;
  else
    {
      const int pfb = premiere_face_bord(), ori = orientation(fac3);
      const int rang1 = DERIVED_T::IS_QUICK ? fac1 : (fac1-pfb), rang2 = DERIVED_T::IS_QUICK ? fac2 :(fac2-pfb);
      flux = 0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl())
                  +  Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl()))* psc ;
    }
  flux3 =  -flux;

  // Calcul de flux1_2:
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  flux = (psc>0) ? psc*inco(fac1) : psc*inco(fac2);
  flux1_2 = -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_symetrie_fluide(int fac1, int fac2, int fac3, int signe, double& aii1_2, double& aii3_4,double& ajj1_2) const
{
  double psc;
  // Calcul de flux3:
  psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  aii3_4 = ((psc*signe)>0) ? psc : 0;

  // Calcul de flux1_2:
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    {
      aii1_2 = psc;
      ajj1_2 = 0;
    }
  else
    {
      ajj1_2 = -psc;
      aii1_2 = 0;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete_symetrie_fluide(int fac1, int fac2, int fac3, int signe, double& flux3, double& flux1_2) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)<0)
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), rang2 = (fac2-pfb), ori = orientation(fac3);
      flux3 = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl())
                    + Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl()))* psc ;
    }
  else flux3 = 0;

  flux1_2 = 0;
}

//************************
// CAS VECTORIEL
//************************

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_fluide(const DoubleTab& inco,int fac1, int fac2, int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  // Calcul de flux3
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0)
    for (int k=0; k<flux3.size(); k++) flux3(k) = -inco(fac3,k)*psc ;
  else
    {
      const int pfb = premiere_face_bord() , ori = orientation(fac3);
      const int rang1 = DERIVED_T::IS_QUICK ? fac1 : (fac1-pfb), rang2 = DERIVED_T::IS_QUICK ? fac2 : (fac2-pfb);
      for (int k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,ori,la_zcl())
                         +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,ori,la_zcl()))* psc ;
    }
  // Calcul de flux1_2:
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = -psc*inco(fac1,k);
  else
    for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = -psc*inco(fac2,k);
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_fluide(int fac1, int fac2, int fac3, int signe, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0)
    for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = psc ;
  else
    for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = 0 ;

  // Calcul des coefficients entre 1 et 2:
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    {
      for (int k=0; k<aii1_2.size(); k++) aii1_2(k) = psc;
      for (int k=0; k<ajj1_2.size(); k++) ajj1_2(k) = 0;
    }
  else
    {
      for (int k=0; k<aii1_2.size(); k++) aii1_2(k) = 0;
      for (int k=0; k<ajj1_2.size(); k++) ajj1_2(k) = -psc;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete_fluide(int fac1, int fac2, int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)<0)
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), rang2 = (fac2-pfb), ori = orientation(fac3);
      for (int k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,ori,la_zcl())
                         + Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,ori,la_zcl()))* psc ;
    }
  else
    for (int k=0; k<flux3.size(); k++) flux3(k) = 0;

  for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = 0;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_interne(const DoubleTab& inco,int fac1, int fac2,int fac3, int fac4, DoubleVect& flux) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (DERIVED_T::IS_AMONT)
    {
      if (psc>0)
        for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac3,k);
      else
        for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac4,k);
    }
  else
    {
      const int ncomp = flux.size(), ori = orientation(fac1),
                num0_0 = face_amont_conj_(fac3,ori,0),num1_1 = face_amont_conj_(fac4,ori,1);
      if (psc > 0)
        {
          if (num0_0 == -1) // Schema amont
            for (int k=0; k<ncomp; k++) flux(k) = -psc*inco(fac3,k);
          else // Schema quick
            {
              const double dx = dist_face_(fac3,fac4,ori), dm = dim_face_(fac3,ori), dxam = dist_face_period_(num0_0,fac3,ori);
              ArrOfDouble vit_0(ncomp), vit_0_0(ncomp), vit_1(ncomp);
              for (int k=0; k<ncomp; k++)
                {
                  vit_0(k) = inco(fac3,k);
                  vit_0_0(k) = inco(num0_0,k);
                  vit_1(k) = inco(fac4,k);
                  flux(k) = -conv_quick_sharp_plus_(psc,vit_0(k),vit_1(k),vit_0_0(k),dx,dm,dxam);
                }
            }
        }
      else // (psc <= 0)
        {
          if (num1_1 == -1) // Schema amont
            for (int k=0; k<ncomp; k++) flux(k) = -psc*inco(fac4,k);
          else // Schema quick
            {
              const double dx = dist_face_(fac3,fac4,ori), dm = dim_face_(fac4,ori), dxam = dist_face_period_(fac4,num1_1,ori);
              ArrOfDouble vit_0(ncomp), vit_1(ncomp), vit_1_1(ncomp);
              for (int k=0; k<ncomp; k++)
                {
                  vit_0(k) = inco(fac3,k);
                  vit_1(k) = inco(fac4,k);
                  vit_1_1(k) = inco(num1_1,k);
                  flux(k) = -conv_quick_sharp_moins_(psc,vit_0(k),vit_1(k),vit_1_1(k),dx,dm,dxam);
                }
            }
        }
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_interne(int fac1, int fac2,int fac3, int fac4, DoubleVect& aii, DoubleVect& ajj ) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (psc>0)
    {
      for (int k=0; k<aii.size(); k++)  aii(k) = psc;
      for (int k=0; k<ajj.size(); k++) ajj(k) = 0;
    }
  else
    {
      for (int k=0; k<ajj.size(); k++) ajj(k) = -psc;
      for (int k=0; k<aii.size(); k++) aii(k) = 0;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_mixte(const DoubleTab& inco,int fac1, int fac2,int fac3, int fac4, DoubleVect& flux) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (psc>0)
    for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac3,k);
  else
    for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac4,k);
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_mixte(int fac1, int fac2,int fac3, int fac4, DoubleVect& aii, DoubleVect& ajj) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (psc>0)
    {
      for (int k=0; k<aii.size(); k++) aii(k) = psc;
      for (int k=0; k<ajj.size(); k++) ajj(k) = 0;
    }
  else
    {
      for (int k=0; k<ajj.size(); k++) ajj(k) = -psc;
      for (int k=0; k<aii.size(); k++) aii(k) = 0;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_paroi_fluide(const DoubleTab& inco,int fac1, int fac2,int fac3,int signe,DoubleVect& flux3,DoubleVect& flux1_2) const
{
  double psc;
  // Calcul de flux3
  psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0)
    for (int k=0; k<flux3.size(); k++) flux3(k) = -inco(fac3,k)*psc ;
  else
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), rang2 = (fac2-pfb), ori = orientation(fac3);
      for (int k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,ori,la_zcl())
                         +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,ori,la_zcl()))* psc ;
    }

  // Calcul de flux1_2:
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = -psc*inco(fac1,k);
  else
    for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = -psc*inco(fac2,k);
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_paroi_fluide(int fac1, int fac2,int fac3,int signe,DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0)
    for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = psc ;
  else
    for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = 0;

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    {
      for (int k=0; k<aii1_2.size(); k++) aii1_2(k) = psc;
      for (int k=0; k<ajj1_2.size(); k++) ajj1_2(k) = 0;
    }
  else
    {
      for (int k=0; k<aii1_2.size(); k++) aii1_2(k) = 0;
      for (int k=0; k<ajj1_2.size(); k++) ajj1_2(k) = -psc;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete_paroi_fluide(int fac1, int fac2,int fac3,int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)<0)
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), rang2 = (fac2-pfb), ori = orientation(fac3);
      for (int k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,ori,la_zcl())
                         +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,ori,la_zcl()))* psc ;
    }
  else
    for (int k=0; k<flux3.size(); k++) flux3(k) = 0;

  for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = 0;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_periodicite(const DoubleTab& inco, int fac1, int fac2 , int fac3, int fac4, DoubleVect& flux3_4, DoubleVect& flux1_2) const
{
  double psc;
  // On calcule le flux convectif entre les volumes de controle associes a fac3 et fac4:
  psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (psc>0)
    for (int k=0; k<flux3_4.size(); k++) flux3_4(k) = -psc*inco(fac3,k);
  else
    for (int k=0; k<flux3_4.size(); k++) flux3_4(k) = -psc*inco(fac4,k);

  // On calcule le flux convectifs entre les volumes de controle associes a fac1 et fac2:
  psc = 0.25 *(dt_vitesse(fac3)*porosite(fac3)+dt_vitesse(fac4)*porosite(fac4))*(surface(fac3) + surface(fac4));
  if (psc>0)
    for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = -psc*inco(fac1,k);
  else
    for (int k=0; k<flux3_4.size(); k++) flux1_2(k) = -psc*inco(fac2,k);
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_periodicite(int fac1, int fac2 , int fac3, int fac4, DoubleVect& aii, DoubleVect& ajj) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (psc>0)
    {
      for (int k=0; k<aii.size(); k++) aii(k) = psc;
      for (int k=0; k<ajj.size(); k++) ajj(k) = 0;
    }
  else
    {
      for (int k=0; k<ajj.size(); k++) ajj(k) = -psc;
      for (int k=0; k<aii.size(); k++) aii(k) = 0;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_fa7_elem(const DoubleTab& inco, int num_elem, int fac1, int fac2, DoubleVect& flux) const
{
  double psc = 0.25 * (dt_vitesse(fac1)+dt_vitesse(fac2))* (surface(fac1)+surface(fac2));
  if (DERIVED_T::IS_AMONT)
    {
      if (psc>0)
        for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac1,k)*porosite(fac1);
      else
        for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac2,k)*porosite(fac2);
    }
  else
    {
      const int num0_0 = face_amont_princ_(fac1,0), num1_1 = face_amont_princ_(fac2,1), ncomp = flux.size();
      if  (psc > 0)
        {
          if (num0_0 == -1) // Schema amont
            for (int k=0; k<ncomp; k++) flux(k) = -psc*inco(fac1,k)*porosite(fac1);
          else
            {
              const int ori = orientation(fac1), elem_amont = elem_(fac1,0);
              const double dx = dim_elem_(num_elem,ori), dm = dist_elem_period_(elem_amont,num_elem,ori), dxam = dim_elem_(elem_amont,ori);
              ArrOfDouble vit_0(ncomp), vit_0_0(ncomp), vit_1(ncomp);
              for (int k=0; k<ncomp; k++)
                {
                  vit_0(k) = inco(fac1,k)*porosite(fac1);
                  vit_0_0(k) = inco(num0_0,k)*porosite(num0_0);
                  vit_1(k) = inco(fac2,k)*porosite(fac2);
                  flux(k) = -conv_quick_sharp_plus_(psc,vit_0(k),vit_1(k),vit_0_0(k),dx,dm,dxam);
                }
            }
        }
      else // (psc < 0)
        {
          if (num1_1 == -1) // Schema amont
            for (int k=0; k<ncomp; k++) flux(k) = -psc*inco(fac2,k)*porosite(fac2);
          else
            {
              const int ori = orientation(fac2), elem_amont = elem_(fac2,1);
              const double dx = dim_elem_(num_elem,ori), dm = dist_elem_period_(num_elem,elem_amont,ori), dxam = dim_elem_(elem_amont,ori);
              ArrOfDouble vit_0(ncomp), vit_1(ncomp), vit_1_1(ncomp);
              for (int k=0; k<ncomp; k++)
                {
                  vit_0(k) = inco(fac1,k)*porosite(fac1);
                  vit_1(k) = inco(fac2,k)*porosite(fac2);
                  vit_1_1(k) = inco(num1_1,k)*porosite(num1_1);
                  flux(k) = -conv_quick_sharp_moins_(psc,vit_0(k),vit_1(k),vit_1_1(k),dx,dm,dxam);
                }
            }
        }
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_fa7_elem(int, int fac1, int fac2, DoubleVect& aii, DoubleVect& ajj ) const
{
  double psc = 0.25 * (dt_vitesse(fac1)+dt_vitesse(fac2))* (surface(fac1)+surface(fac2));
  if (psc>0)
    {
      for (int k=0; k<aii.size(); k++) aii(k) = psc*porosite(fac1);
      for (int k=0; k<ajj.size(); k++) ajj(k) = 0;
    }
  else
    {
      for (int k=0; k<aii.size(); k++) aii(k) = 0;
      for (int k=0; k<ajj.size(); k++) ajj(k) = -psc*porosite(fac2);
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_fa7_sortie_libre(const DoubleTab& inco, int face, const Neumann_sortie_libre& la_cl, int num1,DoubleVect& flux) const
{
  const int elem1 = elem_(face,0);
  const double psc = dt_vitesse(face)*surface(face);
  if (elem1 != -1)
    {
      if (psc > 0)
        {
          if (DERIVED_T::IS_QUICK) // porosite !!! TODO : FIXME
            for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(face,k);
          else
            for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(face,k)*porosite(face);
        }
      else
        for (int k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1,k);
    }
  else // (elem2 != -1)
    {
      if (psc < 0)
        {
          if (DERIVED_T::IS_QUICK) // porosite !!! TODO : FIXME
            for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(face,k);
          else
            for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(face,k)*porosite(face);
        }
      else
        for (int k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1,k);
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_fa7_sortie_libre(int face,const Neumann_sortie_libre& la_cl, DoubleVect& aii, DoubleVect& ajj) const
{
  const double psc = dt_vitesse(face)*surface(face);
  if (psc>0)
    {
      for (int k=0; k<aii.size(); k++) aii(k) = psc*porosite(face);
      for (int k=0; k<ajj.size(); k++) ajj(k) = 0;
    }
  else
    {
      for (int k=0; k<ajj.size(); k++) ajj(k) = -psc*porosite(face);
      for (int k=0; k<aii.size(); k++) aii(k) = 0;
    }
}
//// secmem_fa7_sortie_libre
//
template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::secmem_fa7_sortie_libre(int face, const Neumann_sortie_libre& la_cl, int num1,DoubleVect& flux) const
{
  const int i = elem_(face,0);
  const double psc = dt_vitesse(face)*surface(face);
  if (i != -1)
    {
      if (psc < 0)
        for (int k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1,k);
      else
        for (int k=0; k<flux.size(); k++) flux(k) = 0;
    }
  else   // (elem2 != -1)
    {
      if (psc > 0)
        for (int k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1,k);
      else
        for (int k=0; k<flux.size(); k++) flux(k) = 0;
    }
}

//// flux_arete_symetrie_fluide
//
template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::flux_arete_symetrie_fluide(const DoubleTab& inco,int fac1, int fac2, int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  double psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0)
    for (int k=0; k<flux3.size(); k++) flux3(k) = -inco(fac3,k)*psc ;
  else
    {
      const int pfb = premiere_face_bord(), ori = orientation(fac3);
      const int rang1 = DERIVED_T::IS_QUICK ? fac1 : (fac1-pfb), rang2 = DERIVED_T::IS_QUICK ? fac2 : (fac2-pfb);
      for (int k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,ori,la_zcl())
                         +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,ori,la_zcl()))* psc ;
    }

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = -psc*inco(fac1,k);
  else
    for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = -psc*inco(fac2,k);
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete_symetrie_fluide(int fac1, int fac2, int fac3, int signe, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  double psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0)
    for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = psc ;
  else
    for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = 0 ;

  // Calcul des coefficients entre 1 et 2:
  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    {
      for (int k=0; k<aii1_2.size(); k++) aii1_2(k) = psc;
      for (int k=0; k<ajj1_2.size(); k++) ajj1_2(k) = 0;
    }
  else
    {
      for (int k=0; k<aii1_2.size(); k++) aii1_2(k) = 0;
      for (int k=0; k<ajj1_2.size(); k++) ajj1_2(k) = -psc;
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete_symetrie_fluide(int fac1, int fac2, int fac3, int signe,DoubleVect& flux3, DoubleVect& flux1_2) const
{
  double psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)<0)
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), rang2 = (fac2-pfb), ori = orientation(fac3);
      for (int k=0; k<flux3.size(); k++)
        flux3(k) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,k,ori,la_zcl())
                         + Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,k,ori,la_zcl()))* psc ;
    }
  else
    for (int k=0; k<flux3.size(); k++) flux3(k) = 0;

  for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = 0;
}

#endif /* Eval_Conv_VDF_Face_included */
