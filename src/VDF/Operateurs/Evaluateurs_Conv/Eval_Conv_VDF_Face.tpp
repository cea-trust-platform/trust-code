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
Eval_Conv_VDF_Face<DERIVED_T>::flux_fa7(const DoubleTab& inco, int face, const Neumann_sortie_libre& la_cl, int num1,DoubleVect& flux) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  const int elem1 = elem_(face,0);
  const double psc = dt_vitesse(face)*surface(face);
  if (is_SCALAIRE)
    {
      if (DERIVED_T::IS_QUICK || DERIVED_T::IS_CENTRE4) // TODO : FIXME : pareil mais on corrige apres
        {
          if (elem1 != -1) flux(0) = (psc > 0) ? -psc*inco(face) : -psc*la_cl.val_ext(face-num1,orientation(face));
          else flux(0) = (psc < 0) ? -psc*inco(face) : -psc*la_cl.val_ext(face-num1,orientation(face));
        }
      else
        {
          if (elem1 != -1) flux(0) = (psc > 0) ? -psc*inco(face)*porosite(face) : -psc*la_cl.val_ext(face-num1,orientation(face));
          else flux(0) = (psc < 0) ? -psc*inco(face)*porosite(face) : -psc*la_cl.val_ext(face-num1,orientation(face));
        }
    }
  else
    {
      if (elem1 != -1)
        {
          if (psc > 0)
            {
              if (DERIVED_T::IS_QUICK || DERIVED_T::IS_CENTRE4) // porosite !!! TODO : FIXME
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
              if (DERIVED_T::IS_QUICK || DERIVED_T::IS_CENTRE4) // porosite !!! TODO : FIXME
                for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(face,k);
              else
                for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(face,k)*porosite(face);
            }
          else
            for (int k=0; k<flux.size(); k++) flux(k) = -psc*la_cl.val_ext(face-num1,k);
        }
    }
}

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_fa7(const DoubleTab& inco, int num_elem, int fac1, int fac2, DoubleVect& flux) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  double psc = 0.25 * (dt_vitesse(fac1)+dt_vitesse(fac2))*(surface(fac1)+surface(fac2));
  if (is_SCALAIRE)
    {
      if (DERIVED_T::IS_AMONT) flux(0) = (psc>0) ? -psc * inco(fac1)*porosite(fac1) : -psc * inco(fac2)*porosite(fac2);
      else if (DERIVED_T::IS_CENTRE) flux(0) = -psc * 0.5*(inco(fac1)*porosite(fac1)+inco(fac2)*porosite(fac2));
      else
        {
          const int num0_0 = face_amont_princ_(fac1,0), num1_1 = face_amont_princ_(fac2,1);
          if (DERIVED_T::IS_CENTRE4)
            {
              if  ( (num0_0 == -1) || (num1_1== -1) ) flux(0) = -psc * 0.5*(inco(fac1)*porosite(fac1)+inco(fac2)*porosite(fac2)); // Schema centre 2
              else // Schema centre 4
                {
                  const int ori=orientation(fac1);
                  const double dx = dim_elem_(num_elem,ori), dxam = dim_elem_(elem_(fac1,0),ori), dxav = dim_elem_(elem_(fac2,1),ori),
                      vit_0 = inco(fac1)*porosite(fac1), vit_0_0 = inco(num0_0)*porosite(num0_0),
                      vit_1 = inco(fac2)*porosite(fac2), vit_1_1 = inco(num1_1)*porosite(num1_1);
                  double g1,g2,g3,g4;
                  calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
                  flux(0) = -conv_centre_(psc,vit_0_0,vit_0,vit_1,vit_1_1,g1,g2,g3,g4);
                }
            }
          else
            {
              if  (psc > 0)
                {
                  if (num0_0 == -1) flux(0) = -psc*inco[fac1]*porosite(fac1); // Schema amont
                  else
                    {
                      const int ori = orientation(fac1), elem_amont = elem_(fac1,0);
                      const double dx = dim_elem_(num_elem,ori), dm = dist_elem_period_(elem_amont,num_elem,ori);
                      const double dxam = dim_elem_(elem_amont,ori), vit_0 = inco[fac1]*porosite(fac1),
                          vit_0_0 = inco[num0_0]*porosite(num0_0), vit_1 = inco[fac2]*porosite(fac2);
                      flux(0) = -conv_quick_sharp_plus_(psc,vit_0,vit_1,vit_0_0,dx,dm,dxam);
                    }
                }
              else // (psc < 0)
                {
                  if (num1_1 == -1) flux(0) = -psc*inco[fac2]*porosite(fac2); // Schema amont
                  else
                    {
                      const int ori = orientation(fac2), elem_amont = elem_(fac2,1);
                      const double dx = dim_elem_(num_elem,ori), dm = dist_elem_period_(num_elem,elem_amont,ori);
                      const double dxam = dim_elem_(elem_amont,ori), vit_0 = inco[fac1]*porosite(fac1),
                          vit_1 = inco[fac2]*porosite(fac2), vit_1_1 = inco[num1_1]*porosite(num1_1);
                      flux(0) = -conv_quick_sharp_moins_(psc,vit_0,vit_1,vit_1_1,dx,dm,dxam);
                    }
                }
            }
        }
    }
  else
    {
      if (DERIVED_T::IS_AMONT)
        {
          if (psc>0)
            for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac1,k)*porosite(fac1);
          else
            for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac2,k)*porosite(fac2);
        }
      else if (DERIVED_T::IS_CENTRE)
        {
          if (psc>0) for (int k=0; k<flux.size(); k++) flux(k) = -psc*0.5*(inco(fac1,k)*porosite(fac1)+inco(fac2,k)*porosite(fac2));
        }
      else
        {
          const int num0_0 = face_amont_princ_(fac1,0), num1_1 = face_amont_princ_(fac2,1), ncomp = flux.size();
          if (DERIVED_T::IS_CENTRE4)
            {
              const int ori = orientation(fac1);
              //TODO : FIXME : pourquoi porosite ici ?????
              double psc2 = 0.25* (dt_vitesse(fac1)*porosite(fac1) + dt_vitesse(fac2)*porosite(fac2)) * (surface(fac1) + surface(fac2));
              if ( (num0_0 == -1) || (num1_1== -1) ) // Schema centre2
                for (int k=0; k<ncomp; k++) flux(k) = -psc2*0.5*(inco(fac1,k)*porosite(fac1)+inco(fac2,k)*porosite(fac2));
              else // Schema centre4
                {
                  const double dx = dim_elem_(num_elem,ori), dxam = dim_elem_(elem_(fac1,0),ori), dxav = dim_elem_(elem_(fac2,1),ori);
                  ArrOfDouble vit_0(ncomp),vit_0_0(ncomp),vit_1_1(ncomp),vit_1(ncomp);
                  double g1,g2,g3,g4;
                  calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
                  for (int k=0; k<ncomp; k++)
                    {
                      vit_0(k) = inco(fac1,k)* porosite(fac1);
                      vit_0_0(k) = inco(num0_0,k)*porosite(num0_0);
                      vit_1(k) = inco(fac2,k)*porosite(fac2);
                      vit_1_1(k)=inco(num1_1,k)*porosite(num1_1);
                      flux(k) = -conv_centre_(psc2,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
                    }
                }
            }
          else // QUICK
            {
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
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco,int fac1, int fac2,int fac3, int fac4, DoubleVect& flux) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (is_SCALAIRE)
    {
      if (DERIVED_T::IS_AMONT) flux(0) = (psc>0) ? -psc*inco(fac3) : -psc*inco(fac4);
      else if (DERIVED_T::IS_CENTRE) flux(0) = -0.5*(inco(fac3)+inco(fac4))*psc ;
      else
        {
          const int ori = orientation(fac1), num0_0 = face_amont_conj_(fac3,ori,0), num1_1 = face_amont_conj_(fac4,ori,1);

          if (DERIVED_T::IS_CENTRE4)
            {
              if ( (num0_0 == -1)||(num1_1== -1) ) flux(0) = -0.5*(inco[fac3]+inco[fac4])*psc ; // Schema centre 2 (pas assez de faces)
              else  // Schema Centre 4
                {
                  const double dx = dist_face_(fac3,fac4,ori); // Inutile de prendre dist_face_period car fac3 et fac4 ne peuvent etre periodiques (arete interne)
                  const double dxam = dist_face_period_(num0_0,fac3,ori), dxav = dist_face_period_(fac4,num1_1,ori), vit_0 = inco(fac3),
                               vit_0_0 = inco(num0_0), vit_1 = inco(fac4), vit_1_1 = inco(num1_1);
                  double g1,g2,g3,g4;
                  calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
                  flux(0) = -conv_centre_(psc,vit_0_0,vit_0,vit_1,vit_1_1,g1,g2,g3,g4);
                }
            }
          else // IS_QUICK
            {
              if (psc > 0)
                {
                  if (num0_0 == -1) flux(0) = -psc*inco(fac3); // Schema amont
                  else // Schema quick
                    {
                      const double dx = dist_face_period_(fac3,fac4,ori), dm = dim_face_(fac3,ori), dxam = dist_face_period_(num0_0,fac3,ori);
                      const double vit_0 = inco(fac3), vit_0_0 = inco(num0_0), vit_1 = inco(fac4);
                      flux(0) = -conv_quick_sharp_plus_(psc,vit_0,vit_1,vit_0_0,dx,dm,dxam);
                    }
                }
              else // (psc <= 0)
                {
                  if (num1_1 == -1) flux(0) = -psc*inco(fac4); // Schema amont
                  else // Schema quick
                    {
                      const double dx = dist_face_period_(fac3,fac4,ori), dm = dim_face_(fac4,ori), dxam = dist_face_period_(fac4,num1_1,ori);
                      const double vit_0 = inco(fac3), vit_1 = inco(fac4), vit_1_1 = inco(num1_1);
                      flux(0) = -conv_quick_sharp_moins_(psc,vit_0,vit_1,vit_1_1,dx,dm,dxam);
                    }
                }
            }
        }
    }
  else
    {
      if (DERIVED_T::IS_AMONT)
        {
          if (psc>0)
            for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac3,k);
          else
            for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac4,k);
        }
      else if (DERIVED_T::IS_CENTRE)
        {
          for (int k=0; k<flux.size(); k++) flux(k) = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
        }
      else
        {
          const int ncomp = flux.size(), ori = orientation(fac1),
                    num0_0 = face_amont_conj_(fac3,ori,0),num1_1 = face_amont_conj_(fac4,ori,1);
          if (DERIVED_T::IS_CENTRE4)
            {
              if  ( (num0_0 == -1) || (num1_1== -1) ) // Schema centre 2
                for (int k=0; k<ncomp; k++) flux(k) = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
              else   // Schema centre4
                {
                  const double dx = dist_face_(fac3,fac4,ori), dxam = dist_face_period_(num0_0,fac3,ori), dxav = dist_face_period_(fac4,num1_1,ori);
                  ArrOfDouble vit_0(ncomp), vit_0_0(ncomp), vit_1_1(ncomp), vit_1(ncomp);
                  double g1,g2,g3,g4;
                  calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
                  for (int k=0; k<ncomp; k++)
                    {
                      vit_0(k) = inco(fac3,k);
                      vit_0_0(k) = inco(num0_0,k);
                      vit_1(k) = inco(fac4,k);
                      vit_1_1(k)=inco(num1_1,k);
                      flux(k) = -conv_centre_(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
                    }
                }
            }
          else // QUICK
            {
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
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco,int fac1, int fac2,int fac3, int fac4, DoubleVect& flux) const
{
  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if (DERIVED_T::IS_CENTRE)
    {
      for (int k=0; k<flux.size(); k++) flux(k) = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
    }
  else
    {
      if (psc>0)
        for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac3,k);
      else
        for (int k=0; k<flux.size(); k++) flux(k) = -psc*inco(fac4,k);
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco,int fac1, int fac2, int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  double psc;
  if (is_SCALAIRE)
    {
      // Calcul de flux3:
      psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2)) * (surface(fac1)+surface(fac2)));
      if ((psc*signe)>0) flux3(0) = -inco(fac3)*psc ;
      else
        {
          const int pfb = premiere_face_bord(), ori = orientation(fac3);
          // TODO : FIXME : euh ? pourquoi ca ?
          const int rang1 = DERIVED_T::IS_QUICK ? fac1 : (fac1-pfb), rang2 = DERIVED_T::IS_QUICK ? fac2 : (fac2-pfb);
          flux3(0) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl())
                           + Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl()))* psc ;
        }

      // Calcul de flux1_2:
      psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
      flux1_2(0) = (psc>0) ? -psc*inco(fac1) : ( DERIVED_T::IS_CENTRE ? -psc*0.5*(inco(fac1)+inco(fac2)) : -psc*inco(fac2));
    }
  else
    {
      // Calcul de flux3
      psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
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
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco,int fac1, int fac2, int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  if (DERIVED_T::IS_AXI) return;
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  double psc;
  if (is_SCALAIRE)
    {
      // Calcul de flux3:
      psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
      if ((psc*signe)>0) flux3(0) = -inco(fac3)*psc ;
      else
        {
          const int pfb = premiere_face_bord(), ori = orientation(fac3);
          const int rang1 = DERIVED_T::IS_QUICK ? fac1 : (fac1-pfb), rang2 = DERIVED_T::IS_QUICK ? fac2 :(fac2-pfb);
          flux3(0) = -0.5*(Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang1,ori,la_zcl())
                           +  Champ_Face_get_val_imp_face_bord_sym(inco,inconnue->temps(),rang2,ori,la_zcl()))* psc ;
        }
      // Calcul de flux1_2:
      psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
      flux1_2(0) = (psc>0) ? -psc*inco(fac1) : ( (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4) ? -psc*0.5*(inco(fac1)+inco(fac2)) : -psc*inco(fac2));
    }
  else
    {
      psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
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
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int fac2 , int fac3, int fac4, DoubleVect& flux3_4, DoubleVect& flux1_2) const
{
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  double psc;
  if (is_SCALAIRE)
    {
      if (DERIVED_T::IS_QUICK) // XXX : LOL
        {
          if (DERIVED_T::IS_AXI) return;
          flux_arete<Type_Flux_Arete::INTERNE,Field_Type>(inco,fac1,fac2,fac3,fac4,flux3_4);
          flux_arete<Type_Flux_Arete::INTERNE,Field_Type>(inco,fac3,fac4,fac1,fac2,flux1_2);
          return;
        }

      // On calcule le flux convectif entre les volumes de controle associes a fac3 et fac4:
      psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) + surface(fac2));
      if (DERIVED_T::IS_CENTRE4)
        {
          const int ori = orientation(fac1), num0_0 = face_amont_conj_(fac3,ori,0), num1_1 = face_amont_conj_(fac4,ori,1);
          if ( (num0_0 == -1)||(num1_1== -1) ) flux3_4(0) = -psc*0.5*(inco[fac3]+inco[fac4]); // Schema centre 2 (pas assez de faces)
          else // Schema Centre4
            {
              const double dx = dist_face_period_(fac3,fac4,ori), dxam = dist_face_period_(num0_0,fac3,ori),
                           dxav = dist_face_period_(fac4,num1_1,ori), vit_0 = inco(fac3), vit_0_0 = inco(num0_0),
                           vit_1 = inco(fac4), vit_1_1 = inco(num1_1);
              double g1,g2,g3,g4;
              calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
              flux3_4(0) = -conv_centre_(psc,vit_0_0,vit_0,vit_1,vit_1_1,g1,g2,g3,g4);
            }
        }
      else
        {
          if (DERIVED_T::IS_CENTRE) flux3_4(0) = -psc*0.5*(inco(fac3)+inco(fac4));
          else flux3_4 = (psc>0) ? -psc*inco(fac3) : -psc*inco(fac4);
        }

      // On calcule le flux convectif entre les volumes de controle associes a fac1 et fac2:
      psc = 0.25*(dt_vitesse(fac3)*porosite(fac3)+dt_vitesse(fac4)*porosite(fac4))*(surface(fac3)+surface(fac4));
      if (DERIVED_T::IS_CENTRE4)
        {
          const int ori = orientation(fac3), num0_0 = face_amont_conj_(fac1,ori,0), num1_1 = face_amont_conj_(fac2,ori,1);

          if ( (num0_0 == -1)||(num1_1== -1) ) flux1_2(0) = -psc*0.5*(inco[fac1]+inco[fac2]); // Schema centre 2 (pas assez de faces)
          else // Schema Centre4
            {
              const double dx = dist_face_period_(fac1,fac2,ori), dxam = dist_face_period_(num0_0,fac1,ori),
                           dxav = dist_face_period_(fac2,num1_1,ori), vit_0 = inco(fac1), vit_0_0 = inco(num0_0),
                           vit_1 = inco(fac2), vit_1_1 = inco(num1_1);
              double g1,g2,g3,g4;
              calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
              flux1_2(0) = -conv_centre_(psc,vit_0_0,vit_0,vit_1,vit_1_1,g1,g2,g3,g4);
            }
        }
      else
        {
          if (DERIVED_T::IS_CENTRE) flux1_2(0) = -psc*0.5*(inco(fac1)+inco(fac2));
          else flux1_2(0) = (psc>0) ? -psc*inco(fac1) : -psc*inco(fac2);
        }
    }
  else
    {
      if (DERIVED_T::IS_AXI) return;
      // On calcule le flux convectif entre les volumes de controle associes a fac3 et fac4:
      psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
      if (DERIVED_T::IS_CENTRE)
        {
          for (int k=0; k<flux3_4.size(); k++) flux3_4(k) = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
        }
      else if (DERIVED_T::IS_CENTRE4)
        {
          const int ncomp = flux3_4.size(),ori = orientation(fac1),num0_0 = face_amont_conj_(fac3,ori,0),num1_1 = face_amont_conj_(fac4,ori,1);
          if ( (num0_0 == -1) || (num1_1== -1) ) // Schema centre 2
            for (int k=0; k<ncomp; k++) flux3_4(k) = -psc*0.5*(inco(fac3,k)+inco(fac4,k));
          else // Schema centre 4
            {
              const double dx = dist_face_period_(fac3,fac4,ori), dxam = dist_face_period_(num0_0,fac3,ori), dxav = dist_face_period_(fac4,num1_1,ori);
              ArrOfDouble vit_0(ncomp), vit_0_0(ncomp), vit_1_1(ncomp), vit_1(ncomp);
              double g1,g2,g3,g4;
              calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
              for (int k=0; k<ncomp; k++)
                {
                  vit_0(k) = inco(fac3,k);
                  vit_0_0(k) = inco(num0_0,k);
                  vit_1(k) = inco(fac4,k);
                  vit_1_1(k)=inco(num1_1,k);
                  flux3_4(k) = -conv_centre_(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
                }
            }
        }
      else
        {
          if (psc>0)
            for (int k=0; k<flux3_4.size(); k++) flux3_4(k) = -psc*inco(fac3,k);
          else
            for (int k=0; k<flux3_4.size(); k++) flux3_4(k) = -psc*inco(fac4,k);
        }

      // On calcule le flux convectifs entre les volumes de controle associes a fac1 et fac2:
      psc = 0.25 *(dt_vitesse(fac3)*porosite(fac3)+dt_vitesse(fac4)*porosite(fac4))*(surface(fac3) + surface(fac4));
      if (DERIVED_T::IS_CENTRE)
        {
          for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = -psc*0.5*(inco(fac1,k)+inco(fac2,k));
        }
      else if (DERIVED_T::IS_CENTRE4)
        {
          const int ncomp = flux1_2.size(), ori = orientation(fac3), num0_0 = face_amont_conj_(fac1,ori,0), num1_1 = face_amont_conj_(fac2,ori,1);
          if ( (num0_0 == -1) || (num1_1== -1) ) // Schema centre2
            for (int k=0; k<ncomp; k++)
              flux1_2(k) = -psc*0.5*(inco(fac1,k)+inco(fac2,k));
          else // Schema centre4
            {
              const double dx = dist_face_period_(fac1,fac2,ori),dxam = dist_face_period_(num0_0,fac1,ori), dxav = dist_face_period_(fac2,num1_1,ori);
              ArrOfDouble vit_0(ncomp),vit_0_0(ncomp),vit_1_1(ncomp),vit_1(ncomp);
              double g1,g2,g3,g4;
              calcul_g_(dxam,dx,dxav,g1,g2,g3,g4);
              for (int k=0; k<ncomp; k++)
                {
                  vit_0(k) = inco(fac1,k);
                  vit_0_0(k) = inco(num0_0,k);
                  vit_1(k) = inco(fac2,k);
                  vit_1_1(k)=inco(num1_1,k);
                  flux1_2(k) = -conv_centre_(psc,vit_0_0(k),vit_0(k),vit_1(k),vit_1_1(k),g1,g2,g3,g4);
                }
            }
        }
      else
        {
          if (psc>0)
            for (int k=0; k<flux1_2.size(); k++) flux1_2(k) = -psc*inco(fac1,k);
          else
            for (int k=0; k<flux3_4.size(); k++) flux1_2(k) = -psc*inco(fac2,k);
        }
    }
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco,int fac1, int fac2,int fac3,int signe,DoubleVect& flux3,DoubleVect& flux1_2) const
{
  double psc;
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      psc = 0.25*(dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1) + surface(fac2));
      if ((psc*signe)>0) flux3(0) = -inco(fac3)*psc ;
      else
        {
          const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), rang2 = (fac2-pfb), ori = orientation(fac3);
          flux3(0) = -0.5*(Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl())
                           +  Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang2,ori,la_zcl()))* psc ;
        }

      // Calcul de flux1_2:
      psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
      flux1_2(0) = (psc>0) ? -psc*inco(fac1) : ( DERIVED_T::IS_CENTRE ? -psc*0.5*(inco(fac1)+inco(fac2)) : -psc*inco(fac2));
    }
  else
    {
      psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
      // Calcul de flux3
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
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::flux_arete(const DoubleTab& inco, int fac1, int , int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  if (!DERIVED_T::IS_AMONT)
    {
      Cerr << "ToDo: code like amont scheme ! " <<finl;
      Process::exit();
    }
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      double psc;
      // Calcul de flux3:
      psc = 0.5 * dt_vitesse(fac1) * porosite(fac1) * surface(fac1);
      if ((psc * signe) > 0) flux3(0) = -inco(fac3) * psc;
      else
        {
          const int pfb = premiere_face_bord(), rang1 = (fac1 - pfb), ori = orientation(fac3);
          flux3(0) = -Champ_Face_get_val_imp_face_bord(inconnue->temps(), rang1, ori, la_zcl()) * psc;
        }
      // Calcul de flux1_2:
      psc = 0.5 * dt_vitesse(fac3) * surface(fac3) * porosite(fac3);
      if (psc > 0) flux1_2(0) = -psc * inco(fac1);
      else
        {
          const int pfb = premiere_face_bord(), rang3 = (fac3 - pfb), ori = orientation(fac1);
          flux1_2(0) = -psc * Champ_Face_get_val_imp_face_bord(inconnue->temps(), rang3, ori, la_zcl());
        }
    }
  else
    {
      Cerr << "ToDo: code like Eval_Conv_VDF_Face::flux_arete_coin_fluide for a scalar inco" << finl;
      Process::exit();
    }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::SORTIE_LIBRE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_fa7(int face,const Neumann_sortie_libre& la_cl, DoubleVect& aii, DoubleVect& ajj) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

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

template <typename DERIVED_T> template<Type_Flux_Fa7 Fa7_Type, Type_Champ Field_Type> inline enable_if_t< Fa7_Type == Type_Flux_Fa7::ELEM, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_fa7(int, int fac1, int fac2, DoubleVect& aii, DoubleVect& ajj ) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

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

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::INTERNE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2,int fac3, int fac4, DoubleVect& aii, DoubleVect& ajj ) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

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

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::MIXTE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2,int fac3, int fac4, DoubleVect& aii, DoubleVect& ajj) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

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

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PERIODICITE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2 , int fac3, int fac4, DoubleVect& aii, DoubleVect& ajj) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

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

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int signe, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  // Calcul de flux3:
  if ((psc*signe)>0) for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = psc ;
  else for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = 0 ;
  // Calcul de flux1_2:
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

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2, int fac3, int signe, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

  double psc = 0.25*((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0) for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = psc ;
  else for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = 0 ;

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

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int fac2,int fac3,int signe,DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

  double psc = 0.25 * ((dt_vitesse(fac1)*porosite(fac1)+dt_vitesse(fac2)*porosite(fac2))*(surface(fac1)+surface(fac2)));
  if ((psc*signe)>0) for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = psc ;
  else for (int k=0; k<aii3_4.size(); k++) aii3_4(k) = 0;

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

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::coeffs_arete(int fac1, int, int fac3, int signe, DoubleVect& aii1_2, DoubleVect& aii3_4, DoubleVect& ajj1_2) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      // Calcul de flux3:
      double psc = 0.5*dt_vitesse(fac1)*porosite(fac1)*surface(fac1);
      aii3_4(0) = ((psc*signe)>0) ? psc : 0;

      // Calcul de flux1_2:
      psc = 0.5*dt_vitesse(fac3)*surface(fac3)*porosite(fac3);
      if (psc>0)
        {
          aii1_2(0) = psc;
          ajj1_2(0) = 0;
        }
      else
        {
          ajj1_2(0) = -psc;
          aii1_2(0) = 0;
        }
    }
  else
    {
      Cerr << "ToDo: code like Eval_Conv_VDF_Face::coeffs_arete_coin_fluide for a scalar inco" << finl;
      Process::exit();
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
  const int i = elem_(face,0);
  const double psc = dt_vitesse(face)*surface(face);
  if (is_SCALAIRE)
    {
      if (i != -1) flux(0) = (psc < 0) ?  -psc*la_cl.val_ext(face-num1,orientation(face)) : 0;
      else flux(0) = (psc > 0) ? -psc*la_cl.val_ext(face-num1,orientation(face)) : 0;
    }
  else
    {
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
}

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2, int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

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

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Typ> inline enable_if_t< Arete_Type == Type_Flux_Arete::SYMETRIE_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2, int fac3, int signe,DoubleVect& flux3, DoubleVect& flux1_2) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

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

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::PAROI_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int fac2,int fac3,int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;

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

template <typename DERIVED_T> template<Type_Flux_Arete Arete_Type, Type_Champ Field_Type> inline enable_if_t< Arete_Type == Type_Flux_Arete::COIN_FLUIDE, void>
Eval_Conv_VDF_Face<DERIVED_T>::secmem_arete(int fac1, int , int fac3, int signe, DoubleVect& flux3, DoubleVect& flux1_2) const
{
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_AXI || DERIVED_T::IS_CENTRE4) return;
  constexpr bool is_SCALAIRE = (Field_Type == Type_Champ::SCALAIRE);
  if (is_SCALAIRE)
    {
      // Calcul de flux3:
      double psc = 0.5*dt_vitesse(fac1)*porosite(fac1)*surface(fac1);
      if ((psc*signe)<0)
        {
          const int pfb = premiere_face_bord(), rang1 = (fac1-pfb), ori = orientation(fac3);
          flux3(0) = -Champ_Face_get_val_imp_face_bord(inconnue->temps(),rang1,ori,la_zcl()) * psc ;
        }
      else flux3(0) = 0;

      flux1_2(0) = 0;
    }
  else
    {
      Cerr << "ToDo: code like Eval_Conv_VDF_Face::secmem_arete_coin_fluide for a scalar inco" <<finl;
      Process::exit();
    }
}

#endif /* Eval_Conv_VDF_Face_TPP_included */
