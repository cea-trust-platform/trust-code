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
  static constexpr bool IS_AMONT = false;
  static constexpr bool IS_CENTRE = false;
  static constexpr bool IS_CENTRE4 = false;
  static constexpr bool IS_QUICK = false;

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


  // TODO : all these should have the same name with different attributes
  // so that they become a function template
  inline int calculer_flux_faces_echange_externe_impose() const { return 0; }
  inline int calculer_flux_faces_echange_global_impose() const { return 0; }
  inline int calculer_flux_faces_entree_fluide() const { return 1; }
  inline int calculer_flux_faces_paroi() const { return 0; }
  inline int calculer_flux_faces_paroi_adiabatique() const { return 0; }
  inline int calculer_flux_faces_paroi_defilante() const { return 0; }
  inline int calculer_flux_faces_paroi_fixe() const { return 0; }
  inline int calculer_flux_faces_sortie_libre() const { return 1; }
  inline int calculer_flux_faces_symetrie() const { return 0; }
  inline int calculer_flux_faces_periodique() const { return 1; }
  inline int calculer_flux_faces_NSCBC() const { return DERIVED_T::IS_CENTRE ? 0 : 1; }

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
  inline double flux_face(const DoubleTab&, int , const NSCBC&, int ) const;
  inline double flux_face(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const { return 0; }
  inline double flux_faces_interne(const DoubleTab&, int ) const;

  // Generic return
  template<typename BC>
  inline void coeffs_face(int,int, const BC&, double& aii, double& ajj ) const { /* Do nothing */ }

  // To overload
  inline void coeffs_face(int,int, const Dirichlet_entree_fluide&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Neumann_sortie_libre&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const Periodique&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int, const NSCBC&, double& aii, double& ajj ) const;
  inline void coeffs_face(int,int,int,int, const Echange_externe_impose&, double& aii, double& ajj ) const { /* Do nothing */ }
  inline void coeffs_faces_interne(int, double& aii, double& ajj ) const;

  // contribution de la derivee en vitesse d'une equation scalaire
  // Generic return
  template <typename BC_TYPE>
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int , const BC_TYPE&, int ) const { return 0.; }
  // Overloaded
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Dirichlet_entree_fluide&, int ) const;
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Neumann_sortie_libre&, int ) const;
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int , const Periodique&, int ) const;
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int , const NSCBC&, int ) const;
  inline double coeffs_face_bloc_vitesse(const DoubleTab&, int , int, int, const Echange_externe_impose&, int ) const { return 0.; }
  inline double coeffs_faces_interne_bloc_vitesse(const DoubleTab&, int ) const;

  // Generic return
  template<typename BC>
  inline double secmem_face(int, const BC&, int ) const { return 0; }

  // To overload
  inline double secmem_face(int, const Dirichlet_entree_fluide&, int ) const;
  inline double secmem_face(int, const Neumann_sortie_libre&, int ) const;
  inline double secmem_face(int, const NSCBC&, int ) const;
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

//************************
// CRTP pattern
//************************

template <typename DERIVED_T>
inline int Eval_Conv_VDF_Elem<DERIVED_T>::elem_(int i, int j) const
{
  return static_cast<const DERIVED_T *>(this)->get_elem(i, j);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::dt_vitesse(int face) const
{
  return static_cast<const DERIVED_T *>(this)->get_dt_vitesse(face);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::surface_porosite(int face) const
{
  return static_cast<const DERIVED_T *>(this)->get_surface_porosite(face);
}

template <typename DERIVED_T>
inline int Eval_Conv_VDF_Elem<DERIVED_T>::amont_amont_(int face, int i) const
{
  return static_cast<const DERIVED_T *>(this)->amont_amont(face,i);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::quick_fram_(const double& psc, const int num0, const int num1,
                                                         const int num0_0, const int num1_1, const int face, const DoubleTab& transporte) const
{
  return static_cast<const DERIVED_T *>(this)->quick_fram(psc,num0,num1,num0_0, num1_1, face,  transporte);
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::quick_fram_(const double& psc, const int num0, const int num1,
                                                       const int num0_0, const int num1_1, const int face, const DoubleTab& transporte,ArrOfDouble& flux) const
{
  static_cast<const DERIVED_T *>(this)->quick_fram(psc, num0, num1, num0_0, num1_1, face, transporte, flux);
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::qcentre_(const double& psc, const int num0, const int num1,
                                                      const int num0_0, const int num1_1, const int face, const DoubleTab& transporte) const
{
  return static_cast<const DERIVED_T *>(this)->qcentre(psc,num0,num1,num0_0,num1_1,face,transporte);
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::qcentre_(const double& psc, const int num0, const int num1,
                                                    const int num0_0, const int num1_1, const int face, const DoubleTab& transporte,ArrOfDouble& flux) const
{
  static_cast<const DERIVED_T *>(this)->qcentre(psc,num0,num1,num0_0,num1_1,face,transporte,flux);
}

//************************
// CAS SCALAIRE
//************************

/* Function templates specialization for BC Dirichlet_entree_fluide */
template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                       const Dirichlet_entree_fluide& la_cl, int num1) const
{
  int n0 = elem_(face,0), n1 = elem_(face,1);
  double flux, psc = dt_vitesse(face)*surface_porosite(face), val_imp = la_cl.val_imp(face-num1);

  if (n0 != -1) flux = (psc > 0) ? psc*inco[n0] : psc*val_imp;
  else flux = (psc>0) ? psc*val_imp : psc*inco[n1]; // n1 != -1

  return -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face(int face, int, const Dirichlet_entree_fluide& la_cl,
                                                       double& aii, double& ajj) const
{
  int i = elem_(face,0);
  double psc = dt_vitesse(face)*surface_porosite(face);

  if (i != -1)
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc > 0) aii = -psc;
        }
      else
        {
          if (psc > 0)
            {
              aii = psc;
              ajj = 0;
            }
          else
            {
              aii =  0;
              ajj = 0;
            }
        }
    }
  else   // j != -1
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc<0) ajj = -psc;
        }
      else
        {
          if (psc<0)
            {
              ajj = -psc;
              aii = 0;
            }
          else
            {
              aii = 0;
              ajj = 0;
            }
        }
    }
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face_bloc_vitesse(const DoubleTab& inco, int face,
                                                                      const Dirichlet_entree_fluide& la_cl, int num1) const
{
  if (DERIVED_T::IS_CENTRE4) return 0.;

  int n0 = elem_(face,0), n1 = elem_(face,1);
  double flux, psc = surface_porosite(face), val_imp = la_cl.val_imp(face-num1);

  if (n0 != -1) flux = (dt_vitesse(face) > 0) ? psc*inco[n0] : psc*val_imp;
  else  flux = (dt_vitesse(face)>0) ? psc*val_imp : psc*inco[n1]; // n1 != -1

  return flux;
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::secmem_face(int face, const Dirichlet_entree_fluide& la_cl,
                                                         int num1) const
{
  int i = elem_(face,0);
  double flux, psc = dt_vitesse(face)*surface_porosite(face);

  if (i != -1) flux = (psc < 0) ? psc*la_cl.val_imp(face-num1) : 0.;
  else flux = (psc>0) ? psc*la_cl.val_imp(face-num1) : 0.;  // j != -1

  return -flux;
}

/* Function templates specialization for BC Neumann_sortie_libre */
template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                       const Neumann_sortie_libre& la_cl, int num1) const
{
  int n0 = elem_(face,0), n1 = elem_(face,1);
  double flux, psc = dt_vitesse(face)*surface_porosite(face), val_ext = la_cl.val_ext(face-num1);

  if (n0 != -1) flux = (psc > 0) ? psc*inco[n0] : psc*val_ext;
  else flux = (psc > 0) ? psc*val_ext : psc*inco[n1]; // n1 != -1

  return -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face(int face, int,const Neumann_sortie_libre& la_cl,
                                                       double& aii, double& ajj) const
{
  int i = elem_(face,0);
  double psc = dt_vitesse(face)*surface_porosite(face);

  if (i != -1)
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc > 0) aii = -psc; // euh ????
        }
      else
        {
          if (psc > 0)
            {
              aii = psc;
              ajj = 0;
            }
          else
            {
              aii = 0;
              ajj = 0;
            }
        }
    }
  else   // j != -1
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc < 0) ajj = -psc;
        }
      else
        {
          if (psc<0)
            {
              ajj = -psc;
              aii = 0;
            }
          else
            {
              aii = 0;
              ajj = 0;
            }
        }
    }
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face_bloc_vitesse(const DoubleTab& inco, int face,
                                                                      const Neumann_sortie_libre& la_cl, int num1) const
{
  if (DERIVED_T::IS_CENTRE4) return 0.;

  int n0 = elem_(face,0), n1 = elem_(face,1);
  double flux, psc = surface_porosite(face), val_ext = la_cl.val_ext(face-num1);

  if (n0 != -1) flux = (dt_vitesse(face) > 0) ? psc*inco[n0] : psc*val_ext;
  else flux = (dt_vitesse(face) > 0) ?  psc*val_ext : psc*inco[n1]; // n1 != -1

  return flux;
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::secmem_face(int face, const Neumann_sortie_libre& la_cl, int num1) const
{
  int i = elem_(face,0);
  double flux, psc = dt_vitesse(face)*surface_porosite(face);

  if (i != -1) flux = (psc < 0) ? psc*la_cl.val_ext(face-num1) : 0.;
  else  flux = (psc > 0) ? psc*la_cl.val_ext(face-num1) : 0.; // n1 != -1

  return -flux;
}

/* Function templates specialization for BC Periodique */
template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                       const Periodique& la_cl, int  ) const
{
  double flux, psc = dt_vitesse(face)*surface_porosite(face);
  if (!DERIVED_T::IS_AMONT)
    {
      const int n0 = elem_(face,0), n1 = elem_(face,1), n0_0 = amont_amont_(face,0), n1_1 = amont_amont_(face,1);
      flux =  DERIVED_T::IS_QUICK ? quick_fram_(psc,n0,n1,n0_0,n1_1,face,inco) /* schema Quick_fram */ :
              qcentre_(psc,n0,n1,n0_0,n1_1,face,inco); /* schema centre (2 ou 4 voir CRTP pattern) */
    }
  else // AMONT
    flux = (psc > 0) ? psc*inco(elem_(face,0)) : psc*inco(elem_(face,1));

  return -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face(int face, int,const Periodique& la_cl, double& aii, double& ajj ) const
{
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
    {
      const int i = elem_(face,0),j = elem_(face,1), i0_0 = amont_amont_(face,0),j1_1 = amont_amont_(face,1);
      if (psc > 0) aii = -qcentre_(psc,i,j,i0_0,j1_1,face,inconnue->valeurs());
      else ajj = -qcentre_(psc,i,j,i0_0,j1_1,face,inconnue->valeurs());
    }
  else
    {
      if (psc > 0)
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
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face_bloc_vitesse(const DoubleTab& inco, int face,
                                                                      const Periodique& la_cl, int  ) const
{
  if (DERIVED_T::IS_CENTRE4) return 0.;

  double flux, psc = surface_porosite(face);
  if (DERIVED_T::IS_CENTRE)
    {
      const int n0 = elem_(face,0), n1 = elem_(face,1), n0_0 = amont_amont_(face,0), n1_1 = amont_amont_(face,1);
      flux = qcentre_(psc,n0,n1,n0_0,n1_1,face,inco);
    }
  else flux = (dt_vitesse(face) > 0) ? psc*inco(elem_(face,0)) : psc*inco(elem_(face,1));

  return flux;
}

/* Function templates specialization for BC NSCBC */
template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                       const NSCBC& la_cl, int num1) const
{
  if (DERIVED_T::IS_CENTRE) return 0;
  Cerr << "DERIVED_T::flux_face n'est pas encore codee pour la condition NSCBC" << finl;
  Process::exit();
  return -1e+30;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face(int face, int,const NSCBC& la_cl,
                                                       double& aii, double& ajj) const
{
  if (DERIVED_T::IS_CENTRE) { /* do nothing */ }
  else
    {
      Cerr << "DERIVED_T::coeffs_face n'est pas encore codee pour la condition NSCBC" << finl;
      Process::exit();
    }
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face_bloc_vitesse(const DoubleTab& inco, int face,
                                                                      const NSCBC& la_cl, int num1) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4) return 0.;
  Cerr << "DERIVED_T::coeffs_face_bloc_vitesse n'est pas encore codee pour la condition NSCBC" << finl;
  Process::exit();
  return -1e+30;
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::secmem_face(int face, const NSCBC& la_cl, int num1) const
{
  if (DERIVED_T::IS_CENTRE) return 0;
  Cerr<<"Eval_Amont_VDF_Elem::secmem_face n'est pas encore codee pour la condition NSCBC"<<finl;
  Process::exit();
  return -1e+30;
}

/* Function templates specialization for flux_faces_interne */
template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::flux_faces_interne(const DoubleTab& inco, int face) const
{
  double flux, psc = dt_vitesse(face)*surface_porosite(face);
  if (!DERIVED_T::IS_AMONT)
    {
      const int n0 = elem_(face,0), n1 = elem_(face,1), n0_0 = amont_amont_(face,0), n1_1 = amont_amont_(face,1);
      if (DERIVED_T::IS_CENTRE) flux = qcentre_(psc,n0,n1,n0_0,n1_1,face,inco); /* schema centre2 */
      else if (DERIVED_T::IS_CENTRE4)
        {
          if ( (n0_0 == -1) || (n1_1 == -1) ) flux = psc*(inco(n0)+inco(n1))/2.; /* schema centre2 */
          else flux = qcentre_(psc,n0,n1,n0_0,n1_1,face,inco); /* schema centre4 */
        }
      else
        {
          /* **********************************************************************************************
           * Alexandre C. 19/02/03 : We do not use the first order upwind scheme as it was done before
           * because in LES computations all turbulence is damped and we can not recover a proper behavior
           * of physical turbulent characteristics, especially when using wall-functions.
           * Therefore we use the 2nd order centered scheme.
           * **********************************************************************************************
           * Pierre L. 14/10/04: Correction car le centre explose sur le cas VALIDA On revient au quick en
           * essayant d'ameliorer: on prend le quick si psc est encore favorable pour avoir les 3 points
           * necessaires au calcul du quick. Cela est deja ce qui est fait pour le quick-sharp de
           * l'evaluateur aux faces.
           * **********************************************************************************************/
          if ((n0_0 == -1 && psc >= 0 ) || (n1_1 == -1 && psc <= 0 )) flux = (psc > 0) ? psc*inco(n0) : psc*inco(n1);
          else flux = quick_fram_(psc,n0,n1,n0_0,n1_1,face,inco); // on applique le schema Quick_fram
        }
    }
  else flux = (psc > 0) ? psc*inco(elem_(face,0)) : psc*inco(elem_(face,1)); // AMONT

  return -flux;
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_faces_interne(int face,double& aii, double& ajj ) const
{
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
    {
      const int i = elem_(face,0), j = elem_(face,1), i0_0 = amont_amont_(face,0), j1_1 = amont_amont_(face,1);
      if ( (i0_0 == -1) || (j1_1 == -1) ) // on applique le schema amont
        {
          if (psc > 0) aii = -psc;
          else ajj = -psc;
        }
      else  // on applique le schema centre 2 ou 4
        {
          if (psc > 0) aii = -qcentre_(psc,i,j,i0_0,j1_1,face,inconnue->valeurs());
          else ajj = -qcentre_(psc,i,j,i0_0,j1_1,face,inconnue->valeurs());
        }
    }
  else
    {
      if (psc > 0)
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
}

template <typename DERIVED_T>
inline double Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_faces_interne_bloc_vitesse(const DoubleTab& inco, int face) const
{
  double flux, psc = surface_porosite(face);
  if (DERIVED_T::IS_CENTRE)
    {
      const int n0 = elem_(face,0), n1 = elem_(face,1), n0_0 = amont_amont_(face,0), n1_1 = amont_amont_(face,1);
      flux = qcentre_(psc,n0,n1,n0_0,n1_1,face,inco);
    }
  else flux = (dt_vitesse(face) > 0) ? psc*inco(elem_(face,0)) : psc*inco(elem_(face,1));

  return flux;
}

//************************
// CAS VECTORIEL
//************************

/* Function templates specialization for Periodique */
template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                     const Periodique& la_cl, int num1, DoubleVect& flux) const
{
  int k, elem1 = elem_(face,0), elem2 = elem_(face,1);
  double psc = dt_vitesse(face)*surface_porosite(face);

  if (!DERIVED_T::IS_AMONT)
    {
      const int n0_0 = amont_amont_(face,0),n1_1 = amont_amont_(face,1);
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4) qcentre_(psc,elem1,elem2,n0_0,n1_1,face,inco,flux); // on applique le schema centre 2 ou 4
      else quick_fram_(psc,elem1,elem2,n0_0,n1_1,face,inco,flux); // on applique le schema Quick

      for (k=0; k<flux.size(); k++) flux(k) *= -1;
    }
  else // AMONT
    {
      if (psc > 0) for(k=0; k<flux.size(); k++) flux(k) = -psc*inco(elem1,k);
      else for(k=0; k<flux.size(); k++) flux(k) = -psc*inco(elem2,k);
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face(int face, int,const Periodique& la_cl,
                                                       DoubleVect& aii, DoubleVect& ajj ) const
{
  int k;
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
    {
      const int i = elem_(face,0), j = elem_(face,1), i0_0 = amont_amont_(face,0), j1_1 = amont_amont_(face,1);
      ArrOfDouble flux(aii.size());
      qcentre_(psc,i,j,i0_0,j1_1,face,inconnue->valeurs(),flux); // on applique le schema centre 2 ou 4
      if (psc > 0) for (k=0; k<aii.size(); k++) aii(k) = -flux(k);
      else for (k=0; k<ajj.size(); k++) ajj(k) = -flux(k);
    }
  else
    {
      if (psc > 0)
        {
          for(k=0; k<aii.size(); k++) aii(k) = psc;
          for(k=0; k<ajj.size(); k++) ajj(k) = 0;
        }
      else
        {
          for(k=0; k<ajj.size(); k++) ajj(k) = -psc;
          for(k=0; k<aii.size(); k++) aii(k) = 0;
        }
    }
}

/* Function templates specialization for Neumann_sortie_libre */
template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco,
                                                     int face, const Neumann_sortie_libre& la_cl,
                                                     int num1, DoubleVect& flux) const
{
  int k, n0 = elem_(face,0), n1 = elem_(face,1);
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (n0 != -1)
    {
      if (psc > 0)
        for(k=0; k<flux.size(); k++) flux(k) = -psc*inco(n0,k);
      else
        for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1,k);
    }
  else   // n1 != -1
    {
      if (psc > 0)
        for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1,k);
      else
        for(k=0; k<flux.size(); k++) flux(k) = -psc*inco(n1,k);
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face(int face,int, const Neumann_sortie_libre& la_cl,
                                                       DoubleVect& aii, DoubleVect& ajj) const
{
  int k, i = elem_(face,0);
  double psc = dt_vitesse(face)*surface_porosite(face);

  if (i != -1)
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc > 0) for(k=0; k<aii.size(); k++) aii(k) = -psc;
        }
      else
        {
          if (psc > 0)
            {
              for(k=0; k<aii.size(); k++) aii(k) = psc;
              for(k=0; k<ajj.size(); k++) ajj(k) = 0;
            }
          else
            {
              for(k=0; k<aii.size(); k++) aii(k) = 0;
              for(k=0; k<ajj.size(); k++) ajj(k) = 0;
            }
        }
    }
  else   // j != -1
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc < 0) for(k=0; k<ajj.size(); k++) ajj(k) = -psc;
        }
      else
        {
          if (psc<0)
            {
              for(k=0; k<ajj.size(); k++) ajj(k) = -psc;
              for(k=0; k<aii.size(); k++) aii(k) = 0;
            }
          else
            {
              for(k=0; k<ajj.size(); k++) ajj(k) = 0;
              for(k=0; k<aii.size(); k++) aii(k) = 0;
            }
        }
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::secmem_face(int face, const Neumann_sortie_libre& la_cl, int num1, DoubleVect& flux) const
{
  int k, i = elem_(face,0);
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (i != -1)
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc < 0) for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1); // comprend pas
        }
      else
        {
          if (psc < 0)
            for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1,k);
          else
            for(k=0; k<flux.size(); k++) flux(k) = 0;
        }
    }
  else   // n1 != -1
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc > 0) for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1);
        }
      else
        {
          if (psc > 0)
            for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1,k);
          else
            for(k=0; k<flux.size(); k++) flux(k) = 0;
        }
    }
}

/* Function templates specialization for Dirichlet_entree_fluide */
template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, int face,
                                                     const Dirichlet_entree_fluide& la_cl, int num1, DoubleVect& flux) const
{
  int k, n0 = elem_(face,0), n1 = elem_(face,1);
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (n0 != -1)
    {
      if (psc > 0)
        for(k=0; k<flux.size(); k++) flux(k) = -psc*inco(n0,k);
      else
        for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_imp(face-num1,k);
    }
  else   // n1 != -1
    {
      if (psc>0)
        for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_imp(face-num1,k);
      else
        for(k=0; k<flux.size(); k++) flux(k) = -psc*inco(n1,k);
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face(int face,int, const Dirichlet_entree_fluide& la_cl,
                                                       DoubleVect& aii, DoubleVect& ajj) const
{
  int k, i = elem_(face,0);
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (i != -1)
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc > 0) for(k=0; k<aii.size(); k++) aii(k) = -psc;
        }
      else
        {
          if (psc > 0)
            {
              for(k=0; k<aii.size(); k++) aii(k) = psc;
              for(k=0; k<ajj.size(); k++) ajj(k) = 0;
            }
          else
            {
              for(k=0; k<aii.size(); k++) aii(k) = 0;
              for(k=0; k<ajj.size(); k++) ajj(k) = 0;
            }
        }
    }
  else   // j != -1
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc < 0) for(k=0; k<ajj.size(); k++) ajj(k) = -psc;
        }
      else
        {
          if (psc<0)
            {
              for(k=0; k<ajj.size(); k++) ajj(k) = -psc;
              for(k=0; k<aii.size(); k++) aii(k) = 0;
            }
          else
            {
              for(k=0; k<ajj.size(); k++) ajj(k) = 0;
              for(k=0; k<aii.size(); k++) aii(k) = 0;
            }
        }
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::secmem_face(int face, const Dirichlet_entree_fluide& la_cl, int num1, DoubleVect& flux) const
{
  int k, i = elem_(face,0);
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (i != -1)
    {
      // TODO : FIXME : c'est quoi ca ?
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc < 0) for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_imp(face-num1);
        }
      else
        {
          if (psc < 0)
            for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_imp(face-num1,k);
          else
            for(k=0; k<flux.size(); k++) flux(k) = 0;
        }
    }
  else   // n1 != -1
    {
      // TODO : FIXME : c'est quoi ca ?
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc > 0) for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_imp(face-num1);
        }
      else
        {
          if (psc > 0)
            for(k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_imp(face-num1,k);
          else
            for(k=0; k<flux.size(); k++) flux(k) = 0;
        }
    }
}

/* Function templates specialization for flux_faces_interne */
template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::flux_faces_interne(const DoubleTab& inco,
                                                              int face, DoubleVect& flux) const
{
  int k, n0 = elem_(face,0), n1 = elem_(face,1);
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (!DERIVED_T::IS_AMONT)
    {
      const int n0_0 = amont_amont_(face,0), n1_1 = amont_amont_(face,1);
      if (DERIVED_T::IS_CENTRE)
        {
          qcentre_(psc,n0,n1,n0_0,n1_1,face,inco,flux);
          for (k=0; k<flux.size(); k++) flux(k) *= -1;
        }
      else if (DERIVED_T::IS_CENTRE4)
        {
          if ( (n0_0 == -1) || (n1_1 == -1) )
            {
              for (k=0; k<flux.size(); k++) flux(k) = -psc*(inco(n0,k)+inco(n1,k))/2.; // on applique le schema centre2
            }
          else // on applique le schema centre4
            {
              qcentre_(psc,n0,n1,n0_0,n1_1,face,inco,flux);
              for (k=0; k<flux.size(); k++) flux(k) *= -1;
            }
        }
      else
        {
          if ( (n0_0 == -1 && psc >= 0 ) || (n1_1 == -1 && psc <= 0 ) )
            {
              if (psc > 0)
                for (k=0; k<flux.size(); k++) flux(k) = -psc*inco(n0,k);
              else
                for (k=0; k<flux.size(); k++) flux(k) = -psc*inco(n1,k);
            }
          else // on applique le schema Quick
            {
              quick_fram_(psc,n0,n1,n0_0,n1_1,face,inco,flux);
              for (k=0; k<flux.size(); k++) flux(k) *= -1;
            }
        }
    }
  else // AMONT
    {
      if (psc > 0)
        for(k=0; k<flux.size(); k++) flux(k) = -psc*inco(n0,k);
      else
        for(k=0; k<flux.size(); k++) flux(k) = -psc*inco(n1,k);
    }
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_faces_interne(int face, DoubleVect& aii, DoubleVect& ajj ) const
{
  int k;
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
    {
      const int i = elem_(face,0), j = elem_(face,1), i0_0 = amont_amont_(face,0), j1_1 = amont_amont_(face,1);
      ArrOfDouble flux(aii.size());
      if ( ((i0_0 == -1) || (j1_1 == -1)) &&  DERIVED_T::IS_CENTRE4)
        {
          // on applique le schema amont ... TODO : FIXME
          if (psc > 0) for (k=0; k<aii.size(); k++) aii(k) = -psc;
          else for (k=0; k<ajj.size(); k++) ajj(k) = -psc;
        }
      else // on applique le schema centre4
        {
          qcentre_(psc,i,j,i0_0,j1_1,face,inconnue->valeurs(),flux);
          if (psc > 0) for (k=0; k<aii.size(); k++) aii(k) = -flux(k);
          else for (k=0; k<ajj.size(); k++) ajj(k) = -flux(k);
        }
    }
  else
    {
      if (psc > 0)
        {
          for(k=0; k<aii.size(); k++) aii(k) = psc;
          for(k=0; k<ajj.size(); k++) ajj(k) = 0;
        }
      else
        {
          for(k=0; k<ajj.size(); k++) ajj(k) = -psc;
          for(k=0; k<aii.size(); k++) aii(k) = 0;
        }
    }
}

#endif /* Eval_Conv_VDF_Elem_included */
