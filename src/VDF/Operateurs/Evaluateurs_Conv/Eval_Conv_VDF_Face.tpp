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
// File:        Eval_Conv_VDF_Face.tpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Evaluateurs_Conv
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Eval_Conv_VDF_Face_TPP_included
#define Eval_Conv_VDF_Face_TPP_included

/* ************************************** *
 * *********  POUR L'EXPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_fa7(const DoubleTab& inco, int face, const Neumann_sortie_libre& la_cl, int num1, DoubleVect& flux) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  const int elem1 = elem_(face,0), ncomp = flux.size();
  const double psc = dt_vitesse(face)*surface(face);
  if (elem1 != -1)
    {
      if (psc > 0) for (int k = 0; k < flux.size(); k++) flux(k) = -psc*inco(face,k)*porosite(face);
      else for (int k = 0; k < ncomp; k++) flux(k) = is_SCALAIRE ? -psc*la_cl.val_ext(face-num1,orientation(face)) : -psc*la_cl.val_ext(face-num1,k); // TODO : FIXME : Yannick help :/
    }
  else
    {
      if (psc < 0) for (int k = 0; k < flux.size(); k++) flux(k) = -psc*inco(face,k)*porosite(face);
      else for (int k = 0; k < ncomp; k++) flux(k) =  is_SCALAIRE ? -psc*la_cl.val_ext(face-num1,orientation(face)) : -psc*la_cl.val_ext(face-num1,k);
    }
}

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_fa7(const DoubleTab& inco, int num_elem, int fac1, int fac2, DoubleVect& flux) const
{
  const int ncomp = flux.size();
  const double psc = 0.25*(dt_vitesse(fac1)+dt_vitesse(fac2))*(surface(fac1)+surface(fac2));
  if (DERIVED_T::IS_AMONT)
    {
      if (psc>0) for (int k = 0; k < ncomp; k++) flux(k) = -psc*inco(fac1,k)*porosite(fac1);
      else for (int k = 0; k < ncomp; k++) flux(k) = -psc*inco(fac2,k)*porosite(fac2);
    }
  else if (DERIVED_T::IS_CENTRE) for (int k = 0; k < ncomp; k++) flux(k) = -psc*0.5*(inco(fac1,k)*porosite(fac1)+inco(fac2,k)*porosite(fac2));
  else
    {
      const int num0_0 = face_amont_princ_(fac1,0), num1_1 = face_amont_princ_(fac2,1);
      if (DERIVED_T::IS_CENTRE4)
        {
          const int ori = orientation(fac1);
          if  ( (num0_0 == -1) || (num1_1== -1) ) for (int k = 0; k < ncomp; k++) flux(k) = -psc*0.5*(inco(fac1,k)*porosite(fac1)+inco(fac2,k)*porosite(fac2)); // Schema centre 2
          else // Schema centre 4
            {
              ArrOfDouble vit_0(ncomp),vit_0_0(ncomp),vit_1_1(ncomp),vit_1(ncomp);
              const double dx = dim_elem_(num_elem,ori), dxam = dim_elem_(elem_(fac1,0),ori), dxav = dim_elem_(elem_(fac2,1),ori);
              double g1, g2, g3, g4;
              calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
              for (int k = 0; k < ncomp; k++)
                {
                  vit_0_0(k) = inco(num0_0,k)*porosite(num0_0);
                  vit_0(k) = inco(fac1,k)*porosite(fac1);
                  vit_1(k) = inco(fac2,k)*porosite(fac2);
                  vit_1_1(k) = inco(num1_1,k)*porosite(num1_1);
                  flux(k) = -conv_centre_(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
                }
            }
        }
      else // QUICK
        {
          if  (psc > 0)
            {
              if (num0_0 == -1) for (int k=0; k<ncomp; k++) flux(k) = -psc*inco(fac1,k)*porosite(fac1); // Schema amont
              else
                {
                  ArrOfDouble vit_0(ncomp), vit_0_0(ncomp), vit_1(ncomp);
                  const int ori = orientation(fac1), elem_amont = elem_(fac1,0);
                  const double dx = dim_elem_(num_elem,ori), dm = dist_elem_period_(elem_amont,num_elem,ori), dxam = dim_elem_(elem_amont,ori);
                  for (int k = 0; k < ncomp; k++)
                    {
                      vit_0(k) = inco(fac1,k)*porosite(fac1);
                      vit_1(k) = inco(fac2,k)*porosite(fac2);
                      vit_0_0(k) = inco(num0_0,k)*porosite(num0_0);
                      flux(k) = -conv_quick_sharp_plus_(psc,vit_0(k),vit_1(k),vit_0_0(k),dx,dm,dxam);
                    }
                }
            }
          else // (psc < 0)
            {
              if (num1_1 == -1) for (int k = 0; k < ncomp; k++) flux(k) = -psc*inco(fac2,k)*porosite(fac2); // Schema amont
              else
                {
                  ArrOfDouble vit_0(ncomp), vit_1(ncomp), vit_1_1(ncomp);
                  const int ori = orientation(fac2), elem_amont = elem_(fac2,1);
                  const double dx = dim_elem_(num_elem,ori), dm = dist_elem_period_(num_elem,elem_amont,ori), dxam = dim_elem_(elem_amont,ori);
                  for (int k = 0; k < ncomp; k++)
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
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco,int fac1, int fac2,int fac3, int fac4, DoubleVect& flux) const
{
  const int ncomp = flux.size();
  const double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (DERIVED_T::IS_AMONT)
    {
      if (psc>0) for (int k = 0; k < ncomp; k++) flux(k) = -psc*inco(fac3,k);
      else for (int k = 0; k < ncomp; k++) flux(k) = -psc*inco(fac4,k);
    }
  else if (DERIVED_T::IS_CENTRE) for (int k = 0; k < ncomp; k++) flux(k) = -0.5*(inco(fac3,k)+inco(fac4,k))*psc ;
  else
    {
      const int ori = orientation(fac1), num0_0 = face_amont_conj_(fac3,ori,0), num1_1 = face_amont_conj_(fac4,ori,1);
      if (DERIVED_T::IS_CENTRE4)
        {
          if ( (num0_0 == -1)||(num1_1== -1) ) for (int k = 0; k < ncomp; k++) flux(k) = -0.5*(inco(fac3,k)+inco(fac4,k))*psc ; // Schema centre 2 (pas assez de faces)
          else  // Schema Centre 4
            {
              ArrOfDouble vit_0(ncomp), vit_0_0(ncomp), vit_1_1(ncomp), vit_1(ncomp);
              // Inutile de prendre dist_face_period pour dx car fac3 et fac4 ne peuvent etre periodiques (arete interne)
              const double dx = dist_face_(fac3,fac4,ori), dxam = dist_face_period_(num0_0,fac3,ori), dxav = dist_face_period_(fac4,num1_1,ori);
              double g1, g2, g3, g4;
              calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
              for (int k = 0; k < ncomp; k++)
                {
                  vit_0_0(k) = inco(num0_0,k);
                  vit_0(k) = inco(fac3,k);
                  vit_1(k) = inco(fac4,k);
                  vit_1_1(k) = inco(num1_1,k);
                  flux(k) = -conv_centre_(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
                }
            }
        }
      else // IS_QUICK
        {
          if (psc > 0)
            {
              if (num0_0 == -1) for (int k = 0; k < ncomp; k++) flux(k) = -psc*inco(fac3,k); // Schema amont
              else // Schema quick
                {
                  ArrOfDouble vit_0(ncomp), vit_0_0(ncomp), vit_1(ncomp);
                  const double dx = dist_face_period_(fac3,fac4,ori), dm = dim_face_(fac3,ori), dxam = dist_face_period_(num0_0,fac3,ori);
                  for (int k = 0; k < ncomp; k++)
                    {
                      vit_0(k) = inco(fac3,k);
                      vit_1(k) = inco(fac4,k);
                      vit_0_0(k) = inco(num0_0,k);
                      flux(k) = -conv_quick_sharp_plus_(psc,vit_0(k),vit_1(k),vit_0_0(k),dx,dm,dxam);
                    }
                }
            }
          else // (psc <= 0)
            {
              if (num1_1 == -1) for (int k = 0; k < ncomp; k++) flux(k) = -psc*inco(fac4,k); // Schema amont
              else // Schema quick
                {
                  ArrOfDouble vit_0(ncomp), vit_1(ncomp), vit_1_1(ncomp);
                  const double dx = dist_face_period_(fac3,fac4,ori), dm = dim_face_(fac4,ori), dxam = dist_face_period_(fac4,num1_1,ori);
                  for (int k = 0; k < ncomp; k++)
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
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco,int fac1, int fac2,int fac3, int fac4, DoubleVect& flux) const
{
  const double psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (DERIVED_T::IS_CENTRE) for (int k = 0; k < flux.size(); k++) flux(k) = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
  else
    {
      if (psc>0) for (int k = 0; k < flux.size(); k++) flux(k) = -psc*inco(fac3,k);
      else for (int k = 0; k < flux.size(); k++) flux(k) = -psc*inco(fac4,k);
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type>
inline enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco,int fac1, int fac2, int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  constexpr bool is_SYM = (Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE);
  if (DERIVED_T::IS_AXI && is_SYM) return;

  double psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0) for (int k = 0; k < flux3.size(); k++) flux3(k) = -inco(fac3,k)*psc ;
  else
    {
      const int pfb = premiere_face_bord(), ori = orientation(fac3), rang1 = DERIVED_T::IS_QUICK ? fac1 : (fac1-pfb), rang2 = DERIVED_T::IS_QUICK ? fac2 :(fac2-pfb); // TODO : FIXME : euh ? pourquoi ca ?
      for (int k = 0; k < flux3.size(); k++)
        flux3(k) = -0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl())
            +  Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl()))*psc ;
    }

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  if (psc>0) for (int k = 0; k < flux1_2.size(); k++) flux1_2(k) = -psc*inco(fac1,k);
  else for (int k = 0; k < flux1_2.size(); k++) flux1_2(k) = (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4) ? -psc*0.5*(inco(fac1,k)+inco(fac2,k)) : -psc*inco(fac2,k);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2 , int fac3, int fac4, DoubleVect& flux3_4, DoubleVect& flux1_2) const
{
  if (DERIVED_T::IS_QUICK) // XXX : LOL
    {
      if (DERIVED_T::IS_AXI) return;
      flux_arete<Type_Flux_Arete::INTERNE,Field_Type>(inco,fac1,fac2,fac3,fac4,flux3_4);
      flux_arete<Type_Flux_Arete::INTERNE,Field_Type>(inco,fac3,fac4,fac1,fac2,flux1_2);
      return;
    }

  int ncomp = flux3_4.size();
  double psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) +surface(fac2));
  if (DERIVED_T::IS_CENTRE) for (int k = 0; k < ncomp; k++) flux3_4(k) = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
  else if (DERIVED_T::IS_CENTRE4)
    {
      const int ori = orientation(fac1), num0_0 = face_amont_conj_(fac3,ori,0),num1_1 = face_amont_conj_(fac4,ori,1);
      if ( (num0_0 == -1)||(num1_1== -1) ) for (int k = 0; k < ncomp; k++) flux3_4(k) = -psc*0.5*(inco(fac3,k)+inco(fac4,k)); // Schema centre 2 (pas assez de faces)
      else // Schema Centre4
        {
          ArrOfDouble vit_0(ncomp), vit_0_0(ncomp), vit_1_1(ncomp), vit_1(ncomp);
          const double dx = dist_face_period_(fac3,fac4,ori), dxam = dist_face_period_(num0_0,fac3,ori), dxav = dist_face_period_(fac4,num1_1,ori);
          double g1, g2, g3, g4;
          calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
          for (int k = 0; k < ncomp; k++)
            {
              vit_0_0(k) = inco(num0_0,k);
              vit_0(k) = inco(fac3,k);
              vit_1(k) = inco(fac4,k);
              vit_1_1(k) = inco(num1_1,k);
              flux3_4(k) = -conv_centre_(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
            }
        }
    }
  else
    {
      if (psc>0) for (int k = 0; k < ncomp; k++) flux3_4(k) = -psc*inco(fac3,k);
      else for (int k = 0; k < ncomp; k++) flux3_4(k) = -psc*inco(fac4,k);
    }

  ncomp = flux1_2.size();
  psc = 0.25*(dt_vitesse(fac3)*porosite(fac3)+dt_vitesse(fac4)*porosite(fac4))*(surface(fac3)+surface(fac4));
  if (DERIVED_T::IS_CENTRE) for (int k = 0; k < ncomp; k++) flux1_2(k) = -psc*0.5*(inco(fac1,k)+inco(fac2,k));
  else if (DERIVED_T::IS_CENTRE4)
    {
      const int ori = orientation(fac3), num0_0 = face_amont_conj_(fac1,ori,0), num1_1 = face_amont_conj_(fac2,ori,1);

      if ( (num0_0 == -1)||(num1_1== -1) ) for (int k=0; k<ncomp; k++)  flux1_2(k) = -psc*0.5*(inco(fac1,k)+inco(fac2,k)); // Schema centre 2 (pas assez de faces)
      else // Schema Centre4
        {
          ArrOfDouble vit_0(ncomp), vit_0_0(ncomp), vit_1_1(ncomp), vit_1(ncomp);
          const double dx = dist_face_period_(fac1,fac2,ori),dxam = dist_face_period_(num0_0,fac1,ori), dxav = dist_face_period_(fac2,num1_1,ori);
          double g1, g2, g3, g4;
          calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
          for (int k = 0; k < ncomp; k++)
            {
              vit_0_0(k) = inco(num0_0,k);
              vit_0(k) = inco(fac1,k);
              vit_1(k) = inco(fac2,k);
              vit_1_1(k)=inco(num1_1,k);
              flux1_2(k) = -conv_centre_(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
            }
        }
    }
  else
    {
      if (psc>0) for (int k = 0; k < ncomp; k++) flux1_2(k) = -psc*inco(fac1,k);
      else for (int k = 0; k < ncomp; k++) flux1_2(k) = -psc*inco(fac2,k);
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int , int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  if (!DERIVED_T::IS_AMONT)
    {
      Cerr << "Flux_arete with Type_Flux_Arete::COIN_FLUIDE is only coded for amont scheme !" <<finl;
      Process::exit();
    }

  double psc = 0.5 * dt_vitesse(fac1) * porosite(fac1) * surface(fac1);
  if ((psc * signe) > 0) for (int k = 0; k < flux3.size(); k++) flux3(k) = -inco(fac3,k) * psc;
  else
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1 - pfb), ori = orientation(fac3);
      for (int k = 0; k < flux3.size(); k++) flux3(k) = -Champ_Face_get_val_imp_face_bord(inconnue->temps(), rang1, ori, la_zcl()) * psc;
    }

  psc = 0.5 * dt_vitesse(fac3) * surface(fac3) * porosite(fac3);
  if (psc > 0) for (int k = 0; k < flux1_2.size(); k++) flux1_2(k) = -psc * inco(fac1,k);
  else
    {
      const int pfb = premiere_face_bord(), rang3 = (fac3 - pfb), ori = orientation(fac1);
      for (int k = 0; k < flux1_2.size(); k++) flux1_2(k) = -psc * Champ_Face_get_val_imp_face_bord(inconnue->temps(), rang3, ori, la_zcl());
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_fa7(int face,const Neumann_sortie_libre& la_cl, DoubleVect& aii, DoubleVect& ajj) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

  const double psc = dt_vitesse(face)*surface(face)*porosite(face);
  fill_coeffs_proto(psc,psc,aii,ajj);
}

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_fa7(int, int fac1, int fac2, DoubleVect& aii, DoubleVect& ajj ) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

  const double psc = 0.25*(dt_vitesse(fac1)+dt_vitesse(fac2))*(surface(fac1)+surface(fac2)), psc1 = psc*porosite(fac1), psc2 = psc*porosite(fac2);
  fill_coeffs_proto(psc1,psc2,aii,ajj);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline
enable_if_t<Arete_Type == Type_Flux_Arete::INTERNE || Arete_Type == Type_Flux_Arete::MIXTE || Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2 , int fac3, int fac4, DoubleVect& aii, DoubleVect& ajj) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

  const double psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  fill_coeffs_proto(psc,psc,aii,ajj);
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline
enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2,int fac3,int signe,DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

  constexpr bool is_COIN = (Arete_Type == Type_Flux_Arete::COIN_FLUIDE);

  double psc = is_COIN ? 0.5*dt_vitesse(fac1)*porosite(fac1)*surface(fac1) : 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0) for (int k = 0; k < aii3_4.size(); k++) aii3_4(k) = psc ;
  else for (int k = 0; k < aii3_4.size(); k++) aii3_4(k) = 0.;

  psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
  fill_coeffs_proto(psc,psc,aii1_2,ajj1_2);
}

template <typename DERIVED_T>
inline void Eval_Conv_VDF_Face<DERIVED_T>::fill_coeffs_proto(const double psc1, const double psc2, DoubleVect& A, DoubleVect& B) const
{
  if (psc1 > 0)
    {
      for (int k = 0; k < A.size(); k++) A(k) = psc1;
      for (int k = 0; k < B.size(); k++) B(k) = 0.;
    }
  else
    {
      for (int k = 0; k < A.size(); k++) A(k) = 0.;
      for (int k = 0; k < B.size(); k++) B(k) = -psc2;
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
Eval_Conv_VDF_Face<DERIVED_T>::secmem_fa7(int face, const Neumann_sortie_libre& la_cl, int num1,DoubleVect& flux) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);

  const int i = elem_(face,0), ncomp = flux.size();
  const double psc = dt_vitesse(face)*surface(face);
  if (i != -1)
    {
      if (psc < 0) for (int k = 0; k < ncomp; k++) flux(k) = is_SCALAIRE ? -psc*la_cl.val_ext(face-num1,orientation(face)) : -psc*la_cl.val_ext(face-num1,k); // TODO : FIXME : Yannick help :/
      else for (int k = 0; k < ncomp; k++) flux(k) = 0.;
    }
  else // (elem2 != -1)
    {
      if (psc > 0) for (int k = 0; k < ncomp; k++) flux(k) = is_SCALAIRE ? -psc*la_cl.val_ext(face-num1,orientation(face)) : -psc*la_cl.val_ext(face-num1,k);
      else for (int k = 0; k < ncomp; k++) flux(k) = 0.;
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Typ>
inline enable_if_t<Arete_Type == Type_Flux_Arete::FLUIDE || Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE || Arete_Type == Type_Flux_Arete::PAROI_FLUIDE || Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2, int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

  constexpr bool is_COIN = (Arete_Type == Type_Flux_Arete::COIN_FLUIDE);

  const double tps = inconnue->temps(), psc = is_COIN ? 0.5*dt_vitesse(fac1)*porosite(fac1)*surface(fac1) :
      0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));

  if ((psc*signe)<0)
    {
      const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), rang2 = (fac2-pfb), ori = orientation(fac3);
      for (int k = 0; k < flux3.size(); k++)
        flux3(k) = is_COIN ? -Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl()) * psc :
            -0.5*(Champ_Face_get_val_imp_face_bord(tps,rang1,ori,la_zcl())+Champ_Face_get_val_imp_face_bord(tps,rang2,ori,la_zcl()))*psc ;
    }
  else for (int k = 0; k < flux3.size(); k++) flux3(k) = 0.;

  for (int k = 0; k < flux1_2.size(); k++) flux1_2(k) = 0.;
}

#endif /* Eval_Conv_VDF_Face_TPP_included */
