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

#ifndef Eval_Conv_VDF_Elem_TPP_included
#define Eval_Conv_VDF_Elem_TPP_included

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const DoubleTab& val_b, const int f, const Dirichlet_entree_fluide& la_cl, const int num1, Type_Double& flux) const
{
  for (int n = 0; n < flux.size_array(); n++)
    {
      const int ind = (tab_vitesse().line_size() == flux.size_array()) ? n : 0;
      const double psc = dt_vitesse(f,ind) * surface_porosite(f);
      for (int i = 0, e; i < 2; i++)
        if ((e = elem_(f, i)) > -1)
          flux[n] = -psc * (((psc > 0 && !i) || (psc <= 0 && i)) ? inco(e, n) : val_b(f, n));
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const DoubleTab& val_b, const int f, const Neumann_sortie_libre& la_cl, const int num1, Type_Double& flux) const
{
  for (int n = 0; n < flux.size_array(); n++)
    {
      const int ind = (tab_vitesse().line_size() == flux.size_array()) ? n : 0;
      const double psc = dt_vitesse(f, ind) * surface_porosite(f);
      for (int i = 0, e; i < 2; i++)
        if ((e = elem_(f, i)) > -1)
          flux[n] = -psc * (((psc > 0 && !i) || (psc <= 0 && i)) ? inco(e, n) : val_b(f, n));
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::flux_face(const DoubleTab& inco, const DoubleTab&, const int face, const Periodique& la_cl, const int num1, Type_Double& flux) const
{
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size_array();

  if (!DERIVED_T::IS_AMONT)
    {
      const double psc = dt_vitesse(face) * surface_porosite(face);
      const int i_0 = amont_amont_(face, 0), j_1 = amont_amont_(face, 1);

      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        qcentre_ < Type_Double > (psc, i, j, i_0, j_1, face, inco, flux); // on applique le schema centre 2 ou 4
      else
        quick_fram_(psc, i, j, i_0, j_1, face, inco, flux); // on applique le schema Quick

      for (int k = 0; k < ncomp; k++) flux[k] *= -1;
    }
  else
    for (int k = 0; k < ncomp; k++)
      {
        const int ind = (tab_vitesse().line_size() == ncomp) ? k : 0;
        const double psc = dt_vitesse(face, ind) * surface_porosite(face);
        flux[k] = (psc > 0) ? -psc * inco(i, k) : -psc * inco(j, k); /* AMONT */
      }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::flux_faces_interne(const DoubleTab& inco, const int face, Type_Double& flux) const
{
  const int i = elem_(face,0), j = elem_(face,1), ncomp = flux.size_array();
  if (!DERIVED_T::IS_AMONT)
    {
      const double psc = dt_vitesse(face)*surface_porosite(face);
      const int i_0 = amont_amont_(face,0), j_1 = amont_amont_(face,1);
      if (DERIVED_T::IS_CENTRE)
        {
          qcentre_<Type_Double>(psc,i,j,i_0,j_1,face,inco,flux);
          for (int k=0; k<ncomp; k++) flux[k] *= -1;
        }
      else if (DERIVED_T::IS_CENTRE4)
        {
          if ( (i_0 == -1) || (j_1 == -1) )
            for (int k=0; k<ncomp; k++) flux[k] = -psc*(inco(i,k)+inco(j,k))/2.; // on applique le schema centre2
          else // on applique le schema centre4
            {
              qcentre_<Type_Double>(psc,i,j,i_0,j_1,face,inco,flux);
              for (int k=0; k<ncomp; k++) flux[k] *= -1;
            }
        }
      else
        {
          /* *****************************************************************************************************************************************************
           * Alexandre C. 19/02/03 : We do not use the first order upwind scheme as it was done before because in LES computations all turbulence is damped
           * and we can not recover a proper behavior of physical turbulent characteristics, especially when using wall-functions.
           * Therefore we use the 2nd order centered scheme.
           * *****************************************************************************************************************************************************
           * Pierre L. 14/10/04: Correction car le centre explose sur le cas VALIDA On revient au quick en essayant d'ameliorer: on prend le quick si psc est
           * encore favorable pour avoir les 3 points necessaires au calcul du quick. Cela est deja ce qui est fait pour le quick-sharp de l'evaluateur aux faces.
           * *****************************************************************************************************************************************************/
          if ( (i_0 == -1 && psc >= 0 ) || (j_1 == -1 && psc <= 0 ) )
            for (int k=0; k<ncomp; k++) flux[k] = (psc > 0) ? -psc*inco(i,k) : -psc*inco(j,k);
          else // on applique le schema Quick
            {
              quick_fram_(psc,i,j,i_0,j_1,face,inco,flux);
              for (int k=0; k<ncomp; k++) flux[k] *= -1;
            }
        }
    }
  else
    for(int k = 0; k < ncomp; k++)
      {
        const int ind = (tab_vitesse().line_size() == ncomp) ? k : 0;
        const double psc = dt_vitesse(face, ind)*surface_porosite(face);
        flux[k] = (psc > 0) ? -psc * inco(i, k) : -psc * inco(j, k); /* AMONT */
      }
}

/* ************************************** *
 * *********  POUR L'IMPLICITE ********** *
 * ************************************** */

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face_common(const int face, Type_Double& aii, Type_Double& ajj) const
{
  assert (aii.size_array() == ajj.size_array());
  const int i = elem_(face,0), ncomp = aii.size_array();
  double psc = dt_vitesse(face)*surface_porosite(face);
  if (i != -1)
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc > 0)
            for (int k = 0; k < ncomp; k++) aii[k] = -psc; // TODO : FIXME : Yannick help :/
        }
      else
        for (int k = 0; k < ncomp; k++)
          {
            const int ind = (tab_vitesse().line_size() == ncomp) ? k : 0;
            psc = dt_vitesse(face, ind)*surface_porosite(face);
            aii[k] = (psc > 0) ? psc : 0.;
            ajj[k] = 0.;
          }
    }
  else // j != -1
    {
      if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
        {
          if (psc < 0)
            for (int k = 0; k < ncomp; k++) ajj[k] = -psc;
        }
      else
        for (int k = 0; k < ncomp; k++)
          {
            const int ind = (tab_vitesse().line_size() == ncomp) ? k : 0;
            psc = dt_vitesse(face, ind)*surface_porosite(face);
            ajj[k] = (psc < 0) ? -psc : 0.;
            aii[k] = 0.;
          }
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int, const Dirichlet_entree_fluide& la_cl, Type_Double& aii, Type_Double& ajj) const
{
  coeffs_face_common(face,aii,ajj);
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int, const Neumann_sortie_libre& la_cl, Type_Double& aii, Type_Double& ajj) const
{
  coeffs_face_common(face,aii,ajj);
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face(const int face, const int, const Periodique& la_cl, Type_Double& aii, Type_Double& ajj ) const
{
  assert (aii.size_array() == ajj.size_array());
  const int ncomp = aii.size_array();
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
    {
      const double psc = dt_vitesse(face)*surface_porosite(face);
      Type_Double flux(ncomp);
      const int i = elem_(face,0), j = elem_(face,1), i_0 = amont_amont_(face,0), j_1 = amont_amont_(face,1);
      qcentre_<Type_Double>(psc,i,j,i_0,j_1,face,inconnue->valeurs(),flux); // on applique le schema centre 2 ou 4
      if (psc > 0)
        for (int k = 0; k < ncomp; k++) aii[k] = -flux[k];
      else for (int k = 0; k < ncomp; k++) ajj[k] = -flux[k];
    }
  else
    for (int k = 0; k < ncomp; k++)
      {
        const int ind = (tab_vitesse().line_size() == ncomp) ? k : 0;
        const double psc = dt_vitesse(face, ind)*surface_porosite(face);
        aii[k] = (psc > 0) ? psc : 0.;
        ajj[k] = (psc > 0) ? 0. : -psc;
      }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_faces_interne(const int face, Type_Double& aii, Type_Double& ajj ) const
{
  assert (aii.size_array() == ajj.size_array());
  const int ncomp = aii.size_array();
  if (DERIVED_T::IS_CENTRE || DERIVED_T::IS_CENTRE4)
    {
      const double psc = dt_vitesse(face)*surface_porosite(face);
      const int i = elem_(face,0), j = elem_(face,1), i_0 = amont_amont_(face,0), j_1 = amont_amont_(face,1);
      if ( ((i_0 == -1) || (j_1 == -1)) &&  DERIVED_T::IS_CENTRE4) // TODO : FIXME : Yannick help :/ pourquoi pas pour centre2 egalement ?
        {
          if (psc > 0)
            for (int k = 0; k < ncomp; k++) aii[k] = -psc;
          else for (int k = 0; k < ncomp; k++) ajj[k] = -psc;
        }
      else
        {
          Type_Double flux(ncomp);
          qcentre_<Type_Double>(psc,i,j,i_0,j_1,face,inconnue->valeurs(),flux); // on applique le schema centre 2 ou 4
          if (psc > 0)
            for (int k = 0; k < ncomp; k++) aii[k] = -flux[k];
          else for (int k = 0; k < ncomp; k++) ajj[k] = -flux[k];
        }
    }
  else
    for (int k = 0; k < ncomp; k++)
      {
        const int ind = (tab_vitesse().line_size() == ncomp) ? k : 0;
        const double psc = dt_vitesse(face, ind)*surface_porosite(face);
        aii[k] = (psc > 0) ? psc : 0.;
        ajj[k] = (psc > 0) ? 0. : -psc;
      }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face_bloc_vitesse(const DoubleTab& inco, const DoubleTab& val_b, const int f, const Dirichlet_entree_fluide& la_cl, const int num1, Type_Double& flux) const
{
  if (DERIVED_T::IS_CENTRE4) return ;
  else
    {
      for (int n = 0; n < flux.size_array(); n++)
        for (int i = 0, e; i < 2; i++)
          if ((e = elem_(f, i)) > -1)
            {
              const int ind = (tab_vitesse().line_size() == flux.size_array()) ? n : 0;
              flux[n] = surface_porosite(f) *
                        (((dt_vitesse(f, ind) > 0 && !i) || (dt_vitesse(f, ind) <= 0 && i)) ? inco(e, n) : val_b(f,
                                                                                                                 n));
            }
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face_bloc_vitesse(const DoubleTab& inco, const DoubleTab& val_b, const int f, const Neumann_sortie_libre& la_cl, const int num1, Type_Double& flux) const
{
  if (DERIVED_T::IS_CENTRE4) return;
  else
    {
      for (int n = 0; n < flux.size_array(); n++)
        for (int i = 0, e; i < 2; i++)
          if ((e = elem_(f, i)) > -1)
            {
              const int ind = (tab_vitesse().line_size() == flux.size_array()) ? n : 0;
              flux[n] = surface_porosite(f) *
                        (((dt_vitesse(f, ind) > 0 && !i) || (dt_vitesse(f, ind) <= 0 && i)) ? inco(e, n) : val_b(f,
                                                                                                                 n));
            }
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face_bloc_vitesse_common(const DoubleTab& inco, const int face, Type_Double& flux ) const
{
  if (DERIVED_T::IS_CENTRE4) return;
  else
    {
      const int i = elem_(face, 0), j = elem_(face, 1), ncomp = flux.size_array();
      const double psc = surface_porosite(face);
      if (DERIVED_T::IS_CENTRE)
        {
          const int i_0 = amont_amont_(face, 0), j_1 = amont_amont_(face, 1);
          qcentre_<Type_Double>(psc, i, j, i_0, j_1, face, inco, flux);
        }
      else
        for (int k = 0; k < ncomp; k++)
          {
            const int ind = (tab_vitesse().line_size() == ncomp) ? k : 0;
            flux[k] = (dt_vitesse(face, ind) > 0) ? psc * inco(i, k) : psc * inco(j, k);
          }
    }
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_face_bloc_vitesse(const DoubleTab& inco, const DoubleTab&, const int face, const Periodique& la_cl, const int, Type_Double& flux ) const
{
  coeffs_face_bloc_vitesse_common(inco, face, flux);
}

template <typename DERIVED_T> template <typename Type_Double>
inline void Eval_Conv_VDF_Elem<DERIVED_T>::coeffs_faces_interne_bloc_vitesse(const DoubleTab& inco, const int face , Type_Double& flux) const
{
  coeffs_face_bloc_vitesse_common(inco, face, flux);
}

#endif /* Eval_Conv_VDF_Elem_TPP_included */
