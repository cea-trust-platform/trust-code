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

#ifndef Eval_Conv_VDF_Face_TPP_included
#define Eval_Conv_VDF_Face_TPP_included

/* ************************************** *
 * *********  POUR L'EXPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline std::enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_fa7(const DoubleTab& inco, const DoubleTab* a_r, int face, const Neumann_sortie_libre& la_cl, int num1, Type_Double& flux) const
{
  const int elem1 = elem_(face, 0), elem2 = elem_(face,1);
  for (int k = 0; k < flux.size_array(); k++)
    {
      double psc = dt_vitesse(face, k) * surface(face);
      if (a_r && DERIVED_T::IS_AMONT) psc *= (*a_r)((elem1 != -1) ? elem1 : elem2, k);
      flux[k] = -psc * inco(face, k) * porosite(face);
    }
}

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline std::enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_fa7(const DoubleTab& inco, const DoubleTab* a_r, int num_elem, int fac1, int fac2, Type_Double& flux) const
{
  const int ncomp = flux.size_array();
  double psc = 0.25*(dt_vitesse(fac1)+dt_vitesse(fac2))*(surface(fac1)+surface(fac2));
  if (DERIVED_T::IS_AMONT)
    {
      for (int k = 0; k < ncomp; k++)
        {
          psc = 0.25*(dt_vitesse(fac1,k)+dt_vitesse(fac2,k))*(surface(fac1)+surface(fac2));
          const int f = psc > 0 ? fac1 : fac2;

          if (a_r)
            {
              const int elem = elem_(f, 0), elem2 = elem_(f, 1);
              const int e = dt_vitesse(f,k) > 0 ? (elem > -1 ? elem : elem2) : (elem2 > -1 ? elem2 : elem);
              psc *= (*a_r)(e, k);
            }

          flux[k] = -psc * inco(f, k) * porosite(f);
        }
    }
  else if (DERIVED_T::IS_CENTRE)
    for (int k = 0; k < ncomp; k++) flux[k] = -psc*0.5*(inco(fac1,k)*porosite(fac1)+inco(fac2,k)*porosite(fac2));
  else
    {
      const int num0_0 = face_amont_princ_(fac1,0), num1_1 = face_amont_princ_(fac2,1);
      if (DERIVED_T::IS_CENTRE4)
        {
          const int ori = orientation(fac1);
          if  ( (num0_0 == -1) || (num1_1== -1) )
            for (int k = 0; k < ncomp; k++) flux[k] = -psc*0.5*(inco(fac1,k)*porosite(fac1)+inco(fac2,k)*porosite(fac2)); // Schema centre 2
          else // Schema centre 4
            {
              Type_Double vit_0(ncomp),vit_0_0(ncomp),vit_1_1(ncomp),vit_1(ncomp);
              const double dx = dim_elem_(num_elem,ori), dxam = dim_elem_(elem_(fac1,0),ori), dxav = dim_elem_(elem_(fac2,1),ori);
              double g1, g2, g3, g4;
              calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
              for (int k = 0; k < ncomp; k++)
                {
                  vit_0_0[k] = inco(num0_0,k)*porosite(num0_0);
                  vit_0[k] = inco(fac1,k)*porosite(fac1);
                  vit_1[k] = inco(fac2,k)*porosite(fac2);
                  vit_1_1[k] = inco(num1_1,k)*porosite(num1_1);
                  flux[k] = -conv_centre_(psc,vit_0_0[k],vit_0[k],vit_1[k],vit_1_1[k],g1,g2,g3,g4);
                }
            }
        }
      else // QUICK
        {
          if  (psc > 0)
            {
              if (num0_0 == -1)
                for (int k=0; k<ncomp; k++) flux[k] = -psc*inco(fac1,k)*porosite(fac1); // Schema amont
              else
                {
                  Type_Double vit_0(ncomp), vit_0_0(ncomp), vit_1(ncomp);
                  const int ori = orientation(fac1), elem_amont = elem_(fac1,0);
                  const double dx = dim_elem_(num_elem,ori), dm = dist_elem_period_(elem_amont,num_elem,ori), dxam = dim_elem_(elem_amont,ori);
                  for (int k = 0; k < ncomp; k++)
                    {
                      vit_0[k] = inco(fac1,k)*porosite(fac1);
                      vit_1[k] = inco(fac2,k)*porosite(fac2);
                      vit_0_0[k] = inco(num0_0,k)*porosite(num0_0);
                      flux[k] = -conv_quick_sharp_plus_(psc,vit_0[k],vit_1[k],vit_0_0[k],dx,dm,dxam);
                    }
                }
            }
          else // (psc < 0)
            {
              if (num1_1 == -1)
                for (int k = 0; k < ncomp; k++) flux[k] = -psc*inco(fac2,k)*porosite(fac2); // Schema amont
              else
                {
                  Type_Double vit_0(ncomp), vit_1(ncomp), vit_1_1(ncomp);
                  const int ori = orientation(fac2), elem_amont = elem_(fac2,1);
                  const double dx = dim_elem_(num_elem,ori), dm = dist_elem_period_(num_elem,elem_amont,ori), dxam = dim_elem_(elem_amont,ori);
                  for (int k = 0; k < ncomp; k++)
                    {
                      vit_0[k] = inco(fac1,k)*porosite(fac1);
                      vit_1[k] = inco(fac2,k)*porosite(fac2);
                      vit_1_1[k] = inco(num1_1,k)*porosite(num1_1);
                      flux[k] = -conv_quick_sharp_moins_(psc,vit_0[k],vit_1[k],vit_1_1[k],dx,dm,dxam);
                    }
                }
            }
        }
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline std::enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, const DoubleTab* a_r, int fac1, int fac2,int fac3, int fac4, Type_Double& flux) const
{
  const int ncomp = flux.size_array();
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (DERIVED_T::IS_AMONT)
    {
      for (int k = 0; k < ncomp; k++)
        {
          psc = 0.25 * ((dt_vitesse(fac1,k)*porosite(fac1)+dt_vitesse(fac2,k)*porosite(fac2))*(surface(fac1)+surface(fac2)));
          const int f = psc > 0 ? fac3 : fac4;

          if (a_r)
            {
              const int elem = elem_(f, 0), elem2 = elem_(f, 1);
              const int e = dt_vitesse(f, k) > 0 ? (elem > -1 ? elem : elem2) : (elem2 > -1 ? elem2 : elem);
              psc *= (*a_r)(e, k);
            }

          flux[k] = -psc * inco(f, k);
        }
    }
  else if (DERIVED_T::IS_CENTRE)
    for (int k = 0; k < ncomp; k++) flux[k] = -0.5*(inco(fac3,k)+inco(fac4,k))*psc ;
  else
    {
      const int ori = orientation(fac1), num0_0 = face_amont_conj_(fac3,ori,0), num1_1 = face_amont_conj_(fac4,ori,1);
      if (DERIVED_T::IS_CENTRE4)
        {
          if ( (num0_0 == -1)||(num1_1== -1) )
            for (int k = 0; k < ncomp; k++) flux[k] = -0.5*(inco(fac3,k)+inco(fac4,k))*psc ; // Schema centre 2 (pas assez de faces)
          else  // Schema Centre 4
            {
              Type_Double vit_0(ncomp), vit_0_0(ncomp), vit_1_1(ncomp), vit_1(ncomp);
              // Inutile de prendre dist_face_period pour dx car fac3 et fac4 ne peuvent etre periodiques (arete interne)
              const double dx = dist_face_(fac3,fac4,ori), dxam = dist_face_period_(num0_0,fac3,ori), dxav = dist_face_period_(fac4,num1_1,ori);
              double g1, g2, g3, g4;
              calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
              for (int k = 0; k < ncomp; k++)
                {
                  vit_0_0[k] = inco(num0_0,k);
                  vit_0[k] = inco(fac3,k);
                  vit_1[k] = inco(fac4,k);
                  vit_1_1[k] = inco(num1_1,k);
                  flux[k] = -conv_centre_(psc,vit_0_0[k],vit_0[k],vit_1[k],vit_1_1[k],g1,g2,g3,g4);
                }
            }
        }
      else // IS_QUICK
        {
          if (psc > 0)
            {
              if (num0_0 == -1)
                for (int k = 0; k < ncomp; k++) flux[k] = -psc*inco(fac3,k); // Schema amont
              else // Schema quick
                {
                  Type_Double vit_0(ncomp), vit_0_0(ncomp), vit_1(ncomp);
                  const double dx = dist_face_period_(fac3,fac4,ori), dm = dim_face_(fac3,ori), dxam = dist_face_period_(num0_0,fac3,ori);
                  for (int k = 0; k < ncomp; k++)
                    {
                      vit_0[k] = inco(fac3,k);
                      vit_1[k] = inco(fac4,k);
                      vit_0_0[k] = inco(num0_0,k);
                      flux[k] = -conv_quick_sharp_plus_(psc,vit_0[k],vit_1[k],vit_0_0[k],dx,dm,dxam);
                    }
                }
            }
          else // (psc <= 0)
            {
              if (num1_1 == -1)
                for (int k = 0; k < ncomp; k++) flux[k] = -psc*inco(fac4,k); // Schema amont
              else // Schema quick
                {
                  Type_Double vit_0(ncomp), vit_1(ncomp), vit_1_1(ncomp);
                  const double dx = dist_face_period_(fac3,fac4,ori), dm = dim_face_(fac4,ori), dxam = dist_face_period_(fac4,num1_1,ori);
                  for (int k = 0; k < ncomp; k++)
                    {
                      vit_0[k] = inco(fac3,k);
                      vit_1[k] = inco(fac4,k);
                      vit_1_1[k] = inco(num1_1,k);
                      flux[k] = -conv_quick_sharp_moins_(psc,vit_0[k],vit_1[k],vit_1_1[k],dx,dm,dxam);
                    }
                }
            }
        }
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline std::enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, const DoubleTab* a_r, int fac1, int fac2,int fac3, int fac4, Type_Double& flux) const
{
  double psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (DERIVED_T::IS_CENTRE)
    for (int k = 0; k < flux.size_array(); k++) flux[k] = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
  else
    {
      for (int k = 0; k < flux.size_array(); k++)
        {
          psc = 0.25 * ((dt_vitesse(fac1, k) * porosite(fac1) + dt_vitesse(fac2, k) * porosite(fac2)) * (surface(fac1) + surface(fac2)));
          if (psc > 0)
            {
              if (a_r) psc *= (*a_r)(elem_(fac3,0),k);
              flux[k] = -psc * inco(fac3, k);
            }
          else
            {
              if (a_r) psc *= (*a_r)(elem_(fac4,0),k);
              flux[k] = -psc * inco(fac4, k);
            }
        }
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double>
inline std::enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::NAVIER_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, const DoubleTab* a_r, int fac1, int fac2, int fac3, int signe, Type_Double& flux3, Type_Double& flux1_2) const
{
  assert(flux3.size_array() == flux1_2.size_array());
  constexpr bool is_SYM = (Arete_Type == Type_Flux_Arete::NAVIER_FLUIDE);
  if (DERIVED_T::IS_AXI && is_SYM) return;
  const int ncomp = flux3.size_array();

  double psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0)
    for (int k = 0; k < ncomp; k++)
      {
        const int elem = elem_(fac3, 0), elem2 = elem_(fac3, 1);
        const int e = dt_vitesse(fac3, k) > 0 ? (elem > -1 ? elem : elem2) : (elem2 > -1 ? elem2 : elem);
        const double aa_r = (a_r && DERIVED_T::IS_AMONT) ? (*a_r)(e, k) : 1.0;
        flux3[k] = -aa_r*inco(fac3,k)*psc ;
      }
  else
    {
      const int pfb = premiere_face_bord(), ori = orientation(fac3), rang1 = DERIVED_T::IS_QUICK ? fac1 : (fac1-pfb), rang2 = DERIVED_T::IS_QUICK ? fac2 :(fac2-pfb); // TODO : FIXME : euh ? pourquoi ca ?
      for (int k = 0; k < ncomp; k++)
        {
          const double vf1 = Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl());
          const double vf2 = Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl());
          flux3[k] = -0.5 * (vf1 + vf2) * psc ;
        }
    }

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0)
    for (int k = 0; k < ncomp; k++)
      {
        const int elem = elem_(fac1, 0), elem2 = elem_(fac1, 1);
        const int e = dt_vitesse(fac1, k) > 0 ? (elem > -1 ? elem : elem2) : (elem2 > -1 ? elem2 : elem);
        const double aa_r = (a_r && DERIVED_T::IS_AMONT) ? (*a_r)(e, k) : 1.0;
        flux1_2[k] = -aa_r * psc * inco(fac1, k);
      }
  else for (int k = 0; k < ncomp; k++)
      {
        const int elem = elem_(fac2, 0), elem2 = elem_(fac2, 1);
        const int e = dt_vitesse(fac2, k) > 0 ? (elem > -1 ? elem : elem2) : (elem2 > -1 ? elem2 : elem);
        const double aa_r = (a_r && DERIVED_T::IS_AMONT) ? (*a_r)(e, k) : 1.0;
        flux1_2[k] = (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4) ? -psc*0.5*(inco(fac1,k)+inco(fac2,k)) : -aa_r * psc * inco(fac2, k);
      }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline std::enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, const DoubleTab* a_r, int fac1, int fac2 , int fac3, int fac4, Type_Double& flux3_4, Type_Double& flux1_2) const
{
  assert(flux3_4.size_array() == flux1_2.size_array());
  if (DERIVED_T::IS_QUICK) // XXX : LOL
    {
      if (DERIVED_T::IS_AXI) return;
      else
        {
          flux_arete < Type_Flux_Arete::INTERNE > (inco, a_r, fac1, fac2, fac3, fac4, flux3_4);
          flux_arete < Type_Flux_Arete::INTERNE > (inco, a_r, fac3, fac4, fac1, fac2, flux1_2);
          return;
        }
    }
  const int ncomp = flux3_4.size_array();

  // FIXME : pb_multi !
  if (ncomp > 1) throw;

  double psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) +surface(fac2));
  if (DERIVED_T::IS_CENTRE)
    for (int k = 0; k < ncomp; k++) flux3_4[k] = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
  else if (DERIVED_T::IS_CENTRE4)
    {
      const int ori = orientation(fac1), num0_0 = face_amont_conj_(fac3,ori,0),num1_1 = face_amont_conj_(fac4,ori,1);
      if ( (num0_0 == -1)||(num1_1== -1) )
        for (int k = 0; k < ncomp; k++) flux3_4[k] = -psc*0.5*(inco(fac3,k)+inco(fac4,k)); // Schema centre 2 (pas assez de faces)
      else // Schema Centre4
        {
          Type_Double vit_0(ncomp), vit_0_0(ncomp), vit_1_1(ncomp), vit_1(ncomp);
          const double dx = dist_face_period_(fac3,fac4,ori), dxam = dist_face_period_(num0_0,fac3,ori), dxav = dist_face_period_(fac4,num1_1,ori);
          double g1, g2, g3, g4;
          calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
          for (int k = 0; k < ncomp; k++)
            {
              vit_0_0[k] = inco(num0_0,k);
              vit_0[k] = inco(fac3,k);
              vit_1[k] = inco(fac4,k);
              vit_1_1[k] = inco(num1_1,k);
              flux3_4[k] = -conv_centre_(psc,vit_0_0[k],vit_0[k],vit_1[k],vit_1_1[k],g1,g2,g3,g4);
            }
        }
    }
  else
    {
      if (psc>0)
        for (int k = 0; k < ncomp; k++)
          {
//            if (a_r && DERIVED_T::IS_AMONT) psc *= (*a_r)(elem_(fac3,0),0); // FIXME
            flux3_4[k] = -psc*inco(fac3,k);
          }
      else for (int k = 0; k < ncomp; k++)
          {
//            if (a_r && DERIVED_T::IS_AMONT) psc *= (*a_r)(elem_(fac4,0),0); // FIXME
            flux3_4[k] = -psc*inco(fac4,k);
          }
    }

  psc = 0.25*(dt_vitesse(fac3)*porosite(fac3)+dt_vitesse(fac4)*porosite(fac4))*(surface(fac3)+surface(fac4));
  if (DERIVED_T::IS_CENTRE)
    for (int k = 0; k < ncomp; k++) flux1_2[k] = -psc*0.5*(inco(fac1,k)+inco(fac2,k));
  else if (DERIVED_T::IS_CENTRE4)
    {
      const int ori = orientation(fac3), num0_0 = face_amont_conj_(fac1,ori,0), num1_1 = face_amont_conj_(fac2,ori,1);

      if ( (num0_0 == -1)||(num1_1== -1) )
        for (int k=0; k<ncomp; k++)  flux1_2[k] = -psc*0.5*(inco(fac1,k)+inco(fac2,k)); // Schema centre 2 (pas assez de faces)
      else // Schema Centre4
        {
          Type_Double vit_0(ncomp), vit_0_0(ncomp), vit_1_1(ncomp), vit_1(ncomp);
          const double dx = dist_face_period_(fac1,fac2,ori),dxam = dist_face_period_(num0_0,fac1,ori), dxav = dist_face_period_(fac2,num1_1,ori);
          double g1, g2, g3, g4;
          calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
          for (int k = 0; k < ncomp; k++)
            {
              vit_0_0[k] = inco(num0_0,k);
              vit_0[k] = inco(fac1,k);
              vit_1[k] = inco(fac2,k);
              vit_1_1[k]=inco(num1_1,k);
              flux1_2[k] = -conv_centre_(psc,vit_0_0[k],vit_0[k],vit_1[k],vit_1_1[k],g1,g2,g3,g4);
            }
        }
    }
  else
    {
      if (psc>0)
        for (int k = 0; k < ncomp; k++)
          {
//            if (a_r && DERIVED_T::IS_AMONT) psc *= (*a_r)(elem_(fac1,0),0); // FIXME
            flux1_2[k] = -psc*inco(fac1,k);
          }
      else for (int k = 0; k < ncomp; k++)
          {
//            if (a_r && DERIVED_T::IS_AMONT) psc *= (*a_r)(elem_(fac2,0),0); // FIXME
            flux1_2[k] = -psc*inco(fac2,k);
          }
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline std::enable_if_t< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, const DoubleTab* a_r, int fac1, int , int fac3, int signe, Type_Double& flux3, Type_Double& flux1_2) const
{
  assert(flux3.size_array() == flux1_2.size_array());
  if (!DERIVED_T::IS_AMONT)
    {
      Cerr << "Flux_arete with Type_Flux_Arete::COIN_FLUIDE is only coded for amont scheme !" <<finl;
      Process::exit();
    }

  const int ncomp = flux3.size_array();

  // FIXME : pb_multi !
  if (ncomp > 1) throw;

  double psc = 0.5 * dt_vitesse(fac1) * porosite(fac1) * surface(fac1);
  if ((psc * signe) > 0)
    for (int k = 0; k < ncomp; k++)
      {
//        if (a_r) psc *= (*a_r)(elem_(fac3,0),0); // FIXME
        flux3[k] = -inco(fac3,k) * psc;
      }
  else
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1 - pfb), ori = orientation(fac3);
      for (int k = 0; k < ncomp; k++) flux3[k] = -Champ_Face_get_val_imp_face_bord(inconnue->temps(), rang1, ori, la_zcl()) * psc;
    }

  psc = 0.5 * dt_vitesse(fac3) * surface(fac3) * porosite(fac3);
  if (psc > 0)
    for (int k = 0; k < ncomp; k++)
      {
//        if (a_r) psc *= (*a_r)(elem_(fac1,0),0); // FIXME
        flux1_2[k] = -psc * inco(fac1,k);
      }
  else
    {
      const int pfb = premiere_face_bord(), rang3 = (fac3 - pfb), ori = orientation(fac1);
      for (int k = 0; k < ncomp; k++) flux1_2[k] = -psc * Champ_Face_get_val_imp_face_bord(inconnue->temps(), rang3, ori, la_zcl());
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline std::enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_fa7(const DoubleTab* a_r, int face,const Neumann_sortie_libre& la_cl, Type_Double& aii, Type_Double& ajj) const
{
  assert(aii.size_array() == ajj.size_array());
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;
  const int elem1 = elem_(face,0), elem2 = elem_(face,1);

  for (int k = 0; k < aii.size_array(); k++)
    {
      double psc = dt_vitesse(face,k) * surface(face) * porosite(face);
      if (a_r) psc *= (*a_r)((elem1 != -1) ? elem1 : elem2, k);
      fill_coeffs_proto < Type_Double > (k, psc, psc, aii, ajj);
    }
}

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, typename Type_Double> inline std::enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_fa7(const DoubleTab* a_r, int num_elem, int fac1, int fac2, Type_Double& aii, Type_Double& ajj ) const
{
  assert(aii.size_array() == ajj.size_array());
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;
  else
    {
      for (int k = 0; k < aii.size_array(); k++)
        {
          double psc = 0.25 * (dt_vitesse(fac1, k) + dt_vitesse(fac2, k)) * (surface(fac1) + surface(fac2));
          const int f = psc > 0 ? fac1 : fac2;

          if (a_r)
            {
              const int elem = elem_(f, 0), elem2 = elem_(f, 1);
              const int e = dt_vitesse(f, k) > 0 ? (elem > -1 ? elem : elem2) : (elem2 > -1 ? elem2 : elem);
              psc *= (*a_r)(e, k);
            }

          const double psc1 = psc * porosite(fac1), psc2 = psc * porosite(fac2);
          fill_coeffs_proto<Type_Double>(k, psc1, psc2, aii, ajj);
        }
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline
std::enable_if_t<Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE || Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(const DoubleTab* a_r, int fac1, int fac2 , int fac3, int fac4, Type_Double& aii, Type_Double& ajj) const
{
  assert(aii.size_array() == ajj.size_array());
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;
  else
    {
      for (int k = 0; k < aii.size_array(); k++)
        {
          double psc = 0.25 * ((dt_vitesse(fac1, k) * porosite(fac1) + dt_vitesse(fac2, k) * porosite(fac2)) *
                               (surface(fac1) + surface(fac2)));
          if (a_r)
            {
              if (psc > 0) psc *= (*a_r)(elem_(fac3, 0), k);
              else psc *= (*a_r)(elem_(fac4, 0), k);
            }
          fill_coeffs_proto<Type_Double>(k, psc, psc, aii, ajj);
        }
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, typename Type_Double> inline
std::enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::NAVIER_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(const DoubleTab* a_r, int fac1, int fac2,int fac3,int signe,Type_Double& aii1_2, Type_Double& aii3_4, Type_Double& ajj1_2) const
{
  assert(aii1_2.size_array() == aii3_4.size_array() && aii1_2.size_array() == ajj1_2.size_array());
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

  constexpr bool is_COIN = (Arete_Type == Type_Flux_Arete::COIN_FLUIDE);
  const int ncomp = aii1_2.size_array();

  for (int k = 0; k < ncomp; k++)
    {
      double psc = is_COIN ? 0.5 * dt_vitesse(fac1,k) * porosite(fac1) * surface(fac1) : 0.25 * ((dt_vitesse(fac1,k) * porosite(fac1) + dt_vitesse(fac2,k) * porosite(fac2)) * (surface(fac1) + surface(fac2)));
      if ((psc * signe) > 0)
        {
          const int elem = elem_(fac3, 0), elem2 = elem_(fac3, 1);
          const int e = elem > -1 ? elem : elem2;
          const double aa_r = (a_r && DERIVED_T::IS_AMONT) ? (*a_r)(e, k) : 1.0;
          aii3_4[k] = aa_r * psc;
        }
      else
        aii3_4[k] = 0.;

      int elem = elem_(fac1, 0), elem2 = elem_(fac1, 1);
      int e = elem > -1 ? elem : elem2;
      double aa_r = (a_r && DERIVED_T::IS_AMONT) ? (*a_r)(e, k) : 1.0;
      const double psc1 = aa_r * 0.5 * dt_vitesse(fac3,k) * surface(fac3) * porosite(fac3);

      elem = elem_(fac2, 0), elem2 = elem_(fac2, 1);
      e = elem > -1 ? elem : elem2;
      aa_r = (a_r && DERIVED_T::IS_AMONT) ? (*a_r)(e, k) : 1.0;
      const double psc2 = aa_r * 0.5 * dt_vitesse(fac3,k) * surface(fac3) * porosite(fac3);
      fill_coeffs_proto < Type_Double > (k, psc1, psc2, aii1_2, ajj1_2);
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Face<DERIVED_T>::fill_coeffs_proto(const int k, const double psc1, const double psc2, Type_Double& A, Type_Double& B) const
{
  if (psc1 > 0)
    {
      A[k] = psc1;
      B[k] = 0.;
    }
  else
    {
      A[k] = 0.;
      B[k] = -psc2;
    }
}

#endif /* Eval_Conv_VDF_Face_TPP_included */
