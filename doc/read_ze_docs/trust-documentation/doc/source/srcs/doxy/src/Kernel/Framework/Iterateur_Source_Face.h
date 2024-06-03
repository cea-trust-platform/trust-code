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

#ifndef Iterateur_Source_Face_included
#define Iterateur_Source_Face_included

#include <Iterateur_Source_base.h>
#include <Dirichlet_homogene.h>
#include <Champ_Uniforme.h>
#include <TRUSTSingle.h>
#include <Milieu_base.h>
#include <Dirichlet.h>
#include <Domaine_VF.h>

template <class _TYPE_>
class Iterateur_Source_Face : public Iterateur_Source_base
{
  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    Iterateur_Source_Face* xxx = new  Iterateur_Source_Face(*this);
    if(!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

public:
  Iterateur_Source_Face() : nb_faces(-1), premiere_face_interne(-1) { }
  Iterateur_Source_Face(const Iterateur_Source_Face<_TYPE_>& iter) : Iterateur_Source_base(iter),
    evaluateur_source_face(iter.evaluateur_source_face), nb_faces(iter.nb_faces), premiere_face_interne(iter.premiere_face_interne) { }

  DoubleTab& ajouter(DoubleTab& ) const override;

  void completer_() override
  {
    nb_faces = ref_cast(Domaine_VF,le_dom.valeur()).nb_faces();
    premiere_face_interne = ref_cast(Domaine_VF,le_dom.valeur()).premiere_face_int();
  }

  inline Evaluateur_Source& evaluateur() override
  {
    Evaluateur_Source& eval = static_cast<Evaluateur_Source&>(evaluateur_source_face);
    return eval;
  }

protected:
  _TYPE_ evaluateur_source_face;
  int nb_faces, premiere_face_interne;
  mutable DoubleTab coef;

  template <typename Type_Double> DoubleTab& ajouter_faces_internes(const int, DoubleTab& ) const;
  template <typename Type_Double> DoubleTab& ajouter_faces_bords(const int, DoubleTab& ) const;
  inline const int& faces_doubles(int num_face) const { return ref_cast(Domaine_VF,le_dom.valeur()).faces_doubles()[num_face]; }
};

template<class _TYPE_>
DoubleTab& Iterateur_Source_Face<_TYPE_>::ajouter(DoubleTab& resu) const
{
  ((_TYPE_&) (evaluateur_source_face)).mettre_a_jour();

  assert(resu.nb_dim() < 3);
  const int ncomp = resu.line_size();

  DoubleVect& bilan = so_base->bilan();
  bilan = 0;
  const int nb_faces_tot = ref_cast(Domaine_VF,le_dom.valeur()).nb_faces_tot();
  coef.resize(nb_faces_tot, Array_base::NOCOPY_NOINIT);
  coef = 1;
  if (equation_divisee_par_rho())
    {
      const Milieu_base& milieu = la_zcl->equation().milieu();
      const Champ_base& rho = milieu.masse_volumique().valeur();
      if (sub_type(Champ_Uniforme, rho))
        coef = rho(0, 0);
      else
        {
          const DoubleTab& val_rho = rho.valeurs();
          const IntTab& face_vois = le_dom.valeur().face_voisins();
          const DoubleVect& volumes = ref_cast(Domaine_VF,le_dom.valeur()).volumes();
          coef = 0.;
          for (int fac = 0; fac < nb_faces_tot; fac++)
            {
              const int elem1 = face_vois(fac, 0), elem2 = face_vois(fac, 1);
              double vol = 0.;
              if (elem1 != -1)
                {
                  coef(fac) += val_rho(elem1) * volumes(elem1);
                  vol += volumes(elem1);
                }
              if (elem2 != -1)
                {
                  coef(fac) += val_rho(elem2) * volumes(elem2);
                  vol += volumes(elem2);
                }
              coef(fac) /= vol;
            }
        }
    }

  if (ncomp == 1)
    {
      ajouter_faces_bords<SingleDouble>(ncomp, resu);
      ajouter_faces_internes<SingleDouble>(ncomp, resu);
    }
  else
    {
      ajouter_faces_bords<ArrOfDouble>(ncomp, resu);
      ajouter_faces_internes<ArrOfDouble>(ncomp, resu);
    }
  return resu;
}

template<class _TYPE_> template<typename Type_Double>
DoubleTab& Iterateur_Source_Face<_TYPE_>::ajouter_faces_bords(const int ncomp, DoubleTab& resu) const
{
  Type_Double source(ncomp);
  DoubleVect& bilan = so_base->bilan();
  for (int num_cl = 0; num_cl < le_dom->nb_front_Cl(); num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl.frontiere_dis());
      const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
      if ((sub_type(Dirichlet, la_cl.valeur())) || (sub_type(Dirichlet_homogene, la_cl.valeur()))) { /* Do nothing */ }
      else
        for (int num_face = ndeb; num_face < nfin; num_face++)
          {
            evaluateur_source_face.calculer_terme_source_bord(num_face, source);
            for (int k = 0; k < ncomp; k++)
              {
                resu(num_face, k) += source[k];
                double contribution = (faces_doubles(num_face) == 1) ? 0.5 : 1;
                bilan(k) += contribution * coef(num_face) * source[k];
              }
          }
    }
  return resu;
}

template<class _TYPE_> template<typename Type_Double>
DoubleTab& Iterateur_Source_Face<_TYPE_>::ajouter_faces_internes(const int ncomp, DoubleTab& resu) const
{
  Type_Double source(ncomp);
  DoubleVect& bilan = so_base->bilan();
  for (int num_face = premiere_face_interne; num_face < nb_faces; num_face++)
    {
      evaluateur_source_face.calculer_terme_source(num_face, source);
      for (int k = 0; k < ncomp; k++)
        {
          resu(num_face, k) += source[k];
          double contribution = (faces_doubles(num_face) == 1) ? 0.5 : 1;
          bilan(k) += contribution * coef(num_face) * source[k];
        }
    }
  return resu;
}

#endif /* Iterateur_Source_Face_included */
