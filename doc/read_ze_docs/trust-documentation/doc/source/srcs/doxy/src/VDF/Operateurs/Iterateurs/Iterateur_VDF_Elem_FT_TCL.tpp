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

#ifndef Iterateur_VDF_Elem_FT_TCL_TPP_included
#define Iterateur_VDF_Elem_FT_TCL_TPP_included

/*
 * XXX XXX XXX
 * Elie Saikali : NOTA BENE : fichier surcharger dans trio pour le FT, TCL model
 */

template<class _TYPE_> template<typename Type_Double>
inline void Iterateur_VDF_Elem<_TYPE_>::fill_flux_tables_(const int face, const int ncomp, const double coeff, const Type_Double& flux, DoubleTab& resu) const
{
  DoubleTab& flux_bords = op_base->flux_bords();
  const int elem1 = elem(face, 0), elem2 = elem(face, 1);
  if (elem1 > -1)
    for (int k = 0; k < ncomp; k++)
      {
        resu(elem1, k) += coeff * flux[k];
        flux_bords(face, k) += coeff * flux[k];
      }
  if (elem2 > -1)
    for (int k = 0; k < ncomp; k++)
      {
        resu(elem2, k) -= coeff * flux[k];
        flux_bords(face, k) -= coeff * flux[k];
      }
}

template<class _TYPE_> template<typename Type_Double>
bool Iterateur_VDF_Elem<_TYPE_>::ajouter_blocs_bords_echange_ext_FT_TCL(const Echange_externe_impose& cl, const int ndeb, const int nfin, const int num_cl, const int N, const Front_VF& frontiere_dis,
                                                                        matrices_t mats, DoubleTab& resu, const tabs_t& semi_impl) const
{
  return false; /* true dans trio ... */
}

#endif /* Iterateur_VDF_Elem_FT_TCL_TPP_included */
