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

#ifndef Iterateur_Source_EF_Som_included
#define Iterateur_Source_EF_Som_included

#include <Iterateur_Source_base.h>
#include <Champ_Uniforme.h>
#include <TRUSTSingle.h>
#include <Milieu_base.h>
#include <Domaine_EF.h>

template<typename _TYPE_>
class Iterateur_Source_EF_Som: public Iterateur_Source_base
{
  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    Iterateur_Source_EF_Som *xxx = new Iterateur_Source_EF_Som(*this);
    if (!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

public:
  Iterateur_Source_EF_Som() : nb_elems(-1) { }
  Iterateur_Source_EF_Som(const Iterateur_Source_EF_Som& iter) : Iterateur_Source_base(iter), evaluateur_source_elem(iter.evaluateur_source_elem), nb_elems(iter.nb_elems) { }

  inline Evaluateur_Source& evaluateur() override
  {
    Evaluateur_Source& eval = static_cast<Evaluateur_Source&> (evaluateur_source_elem);
    return eval;
  }

  void completer_() override { nb_elems = le_dom->domaine().nb_elem_tot(); }

  DoubleTab& ajouter(DoubleTab&) const override;
  int impr(Sortie&) const override;

protected:
  _TYPE_ evaluateur_source_elem;
  int nb_elems;
  mutable double coef = -1.;
  mutable DoubleVect bilan;

  template <typename Type_Double>
  DoubleTab& ajouter_elems_standard(const int , DoubleTab& ) const;
};

template<typename _TYPE_>
DoubleTab& Iterateur_Source_EF_Som<_TYPE_>::ajouter(DoubleTab& resu) const
{
  ((_TYPE_&) (evaluateur_source_elem)).mettre_a_jour();

  assert(resu.nb_dim() < 3);
  const int ncomp = resu.line_size();

  bilan.resize(ncomp);
  bilan = 0;
  coef = 1;
  if (equation_divisee_par_rho())
    {
      const Milieu_base& milieu = la_zcl->equation().milieu();
      const Champ_base& rho = milieu.masse_volumique();
      if (sub_type(Champ_Uniforme, rho))
        coef = rho.valeurs()(0, 0);
      else
        {
          Cerr << "Cas non prevu dans Iterateur_Source_EF_Som<_TYPE_>::ajouter(DoubleTab& resu) const" << finl;
          Process::exit();
        }
    }

  (ncomp == 1) ? ajouter_elems_standard<SingleDouble>(ncomp, resu) : ajouter_elems_standard<ArrOfDouble>(ncomp, resu);

  return resu;
}

template<typename _TYPE_> template<typename Type_Double>
DoubleTab& Iterateur_Source_EF_Som<_TYPE_>::ajouter_elems_standard(const int ncomp, DoubleTab& resu) const
{
  Type_Double source(ncomp);
  const IntTab& elems = le_dom->domaine().les_elems();
  const DoubleTab& IPhi_thilde = ref_cast(Domaine_EF,le_dom.valeur()).IPhi_thilde();
  int nb_som_elem = le_dom->domaine().nb_som_elem();
  for (int num_elem = 0; num_elem < nb_elems; num_elem++)
    {
      evaluateur_source_elem.calculer_terme_source(num_elem, source);
      for (int k = 0; k < ncomp; k++)
        for (int i = 0; i < nb_som_elem; i++)
          {
            double src = source[k]; // XXX : ATTENTION : ON CHANGE PAS source[k] !!!
            src *= IPhi_thilde(num_elem, i);
            bilan(k) += coef * src;
            resu(elems(num_elem, i),k) += src;
          }
    }
  return resu;
}

template<typename _TYPE_>
int Iterateur_Source_EF_Som<_TYPE_>::impr(Sortie& os) const
{
  for (int k = 0; k < bilan.size(); k++)
    {
      bilan(k) = Process::mp_sum(bilan(k));
      if (je_suis_maitre())
        os << " : " << bilan(k) << finl;
    }
  return 1;
}

#endif /* Iterateur_Source_EF_Som_included */
