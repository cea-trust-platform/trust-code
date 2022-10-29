/****************************************************************************
* Copyright (c) 2022, CEA
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

#ifndef Iterateur_Source_PolyMAC_Elem_included
#define Iterateur_Source_PolyMAC_Elem_included

#include <Iterateur_Source_PolyMAC_base.h>
#include <Champ_Uniforme.h>
#include <Zone_PolyMAC.h>

template <class _TYPE_>
class Iterateur_Source_PolyMAC_Elem: public Iterateur_Source_PolyMAC_base
{
  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    Iterateur_Source_PolyMAC_Elem* xxx = new  Iterateur_Source_PolyMAC_Elem(*this);
    if(!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

public:
  DoubleTab& calculer(DoubleTab&) const override;
  DoubleTab& ajouter(DoubleTab&) const override;
  inline void completer_() override;
  inline Evaluateur_Source_PolyMAC& evaluateur() override;

protected:
  _TYPE_ evaluateur_source_elem;
  int nb_elem_;

  DoubleTab& ajouter_(DoubleTab&) const;
  DoubleTab& ajouter_(DoubleTab&, int) const;
};

template<class _TYPE_>
inline void Iterateur_Source_PolyMAC_Elem<_TYPE_>::completer_()
{
  nb_elem_ = la_zone->nb_elem();
}

template<class _TYPE_>
inline Evaluateur_Source_PolyMAC& Iterateur_Source_PolyMAC_Elem<_TYPE_>::evaluateur()
{
  Evaluateur_Source_PolyMAC& eval = (Evaluateur_Source_PolyMAC&) evaluateur_source_elem;
  return eval;
}

template<class _TYPE_>
DoubleTab& Iterateur_Source_PolyMAC_Elem<_TYPE_>::ajouter(DoubleTab& resu) const
{
  ((_TYPE_&) (evaluateur_source_elem)).mettre_a_jour();
  assert(resu.nb_dim() < 3);
  int ncomp = 1;
  if (resu.nb_dim() == 2)
    ncomp = resu.dimension(1);
  DoubleVect& bilan = so_base->bilan();
  bilan = 0;
  if (ncomp == 1)
    ajouter_(resu);
  else
    ajouter_(resu, ncomp);
  return resu;
}

template<class _TYPE_>
DoubleTab& Iterateur_Source_PolyMAC_Elem<_TYPE_>::ajouter_(DoubleTab& resu) const
{
  DoubleVect& bilan = so_base->bilan();
  for (int num_elem = 0; num_elem < nb_elem_; num_elem++)
    {
      double source = evaluateur_source_elem.calculer_terme_source(num_elem);
      resu[num_elem] += source;
      bilan(0) += source;
    }
  return resu;
}

template<class _TYPE_>
DoubleTab& Iterateur_Source_PolyMAC_Elem<_TYPE_>::ajouter_(DoubleTab& resu, int ncomp) const
{
  DoubleVect source(ncomp);
  DoubleVect& bilan = so_base->bilan();
  for (int num_elem = 0; num_elem < nb_elem_; num_elem++)
    {
      evaluateur_source_elem.calculer_terme_source(num_elem, source);
      for (int k = 0; k < ncomp; k++)
        {
          resu(num_elem, k) += source(k);
          bilan(k) += source(k);
        }
    }
  return resu;
}

template<class _TYPE_>
DoubleTab& Iterateur_Source_PolyMAC_Elem<_TYPE_>::calculer(DoubleTab& resu) const
{
  resu = 0.;
  return ajouter(resu);
}

#endif /* Iterateur_Source_PolyMAC_Elem_included */
