/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef PDC_PolyMAC_impl_included
#define PDC_PolyMAC_impl_included

#include <TRUSTTabs_forward.h>
#include <Champ_Don_base.h>
#include <TRUST_Deriv.h>
#include <Parser_U.h>

class Param;

/// \cond DO_NOT_DOCUMENT
class PDC_PolyMAC_impl
{};
/// \endcond

/////////////////////////////////////////////////
class PDC_Anisotrope_PolyMAC
{
protected:
  int lire_motcle_non_standard_impl(const Motcle&, Entree&);

  //! Implemente le calcul effectif de la perte de charge pour un lieu donne
  void coeffs_perte_charge_impl(const DoubleVect&, const DoubleVect&, double, double, double, double, double, double&, double&, double&, DoubleVect&, Parser_U&) const;

  mutable Parser_U lambda_ortho;
  OWN_PTR(Champ_Don_base) v; //!< Vecteur directeur de la perte de charge.
};

/////////////////////////////////////////////////
class PDC_Circulaire_PolyMAC
{
protected:
  int lire_motcle_non_standard_impl(const Motcle&, Entree&);

  //! Implemente le calcul effectif de la perte de charge pour un lieu donne
  void coeffs_perte_charge_impl(const DoubleVect&, const DoubleVect&, double, double, double, double, double, double&, double&, double&, DoubleVect&, Parser_U&) const;

  OWN_PTR(Champ_Don_base) diam_hydr_ortho, v; //!< Vecteur directeur de la perte de charge.
  mutable Parser_U lambda_ortho;
  mutable DoubleVect v_valeur;
};

/////////////////////////////////////////////////
class PDC_Directionnelle_PolyMAC
{
protected:
  //! Implemente le calcul effectif de la perte de charge pour un lieu donne
  void coeffs_perte_charge_impl(const DoubleVect&, const DoubleVect&, double, double, double, double, double, double&, double&, double&, DoubleVect&, Parser_U&) const;

  OWN_PTR(Champ_Don_base) v; //!< Vecteur directeur de la perte de charge.
};

/////////////////////////////////////////////////
class PDC_Isotrope_PolyMAC
{
protected:
  void coeffs_perte_charge_impl(const DoubleVect&, const DoubleVect&, double, double, double, double, double, double&, double&, double&, DoubleVect&, Parser_U&) const;
};

#endif /* PDC_PolyMAC_impl_included */
