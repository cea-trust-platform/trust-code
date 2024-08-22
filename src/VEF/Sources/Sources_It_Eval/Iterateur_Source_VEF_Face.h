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

#ifndef Iterateur_Source_VEF_Face_included
#define Iterateur_Source_VEF_Face_included

#include <Evaluateur_Source_VEF_Face.h>
#include <Iterateur_Source_base.h>
#include <Champ_Uniforme.h>
#include <TRUSTSingle.h>
#include <Milieu_base.h>
#include <Domaine_Cl_VEF.h>
#include <Dirichlet.h>
#include <Domaine_VEF.h>

template<typename _TYPE_>
class Iterateur_Source_VEF_Face: public Iterateur_Source_base
{
  inline unsigned taille_memoire() const override { throw; }

  inline int duplique() const override
  {
    Iterateur_Source_VEF_Face* xxx = new  Iterateur_Source_VEF_Face(*this);
    if(!xxx) Process::exit("Not enough memory !");
    return xxx->numero();
  }

public:
  Iterateur_Source_VEF_Face() : nb_faces(-1), nb_faces_elem(-1), premiere_face_std(-1) { }
  Iterateur_Source_VEF_Face(const Iterateur_Source_VEF_Face<_TYPE_>& iter) : Iterateur_Source_base(iter), evaluateur_source_face(iter.evaluateur_source_face),
    nb_faces(iter.nb_faces), nb_faces_elem(iter.nb_faces_elem), premiere_face_std(iter.premiere_face_std) { }

  DoubleTab& ajouter(DoubleTab&) const override;

  void completer_() override
  {
    nb_faces = ref_cast(Domaine_VEF,le_dom.valeur()).nb_faces();
    nb_faces_elem = le_dom->domaine().nb_faces_elem();
    premiere_face_std = ref_cast(Domaine_VEF,le_dom.valeur()).premiere_face_std();
  }

  inline Evaluateur_Source_VEF_Face& evaluateur() override
  {
    Evaluateur_Source_VEF_Face& eval = static_cast<Evaluateur_Source_VEF_Face&> (evaluateur_source_face);
    return eval;
  }

protected:
  int initialiser(double tps) override;
  void remplir_volumes_cl_dirichlet();

  inline const int& faces_doubles(int num_face) const { return ref_cast(Domaine_VEF,le_dom.valeur()).faces_doubles()[num_face]; }
  inline double volumes_entrelaces_Cl(int& num_face) const { return volumes_cl_dirichlet_[num_face]; }
  inline double volumes_entrelaces(int& num_face) const { return ref_cast(Domaine_VEF,le_dom.valeur()).volumes_entrelaces()[num_face]; }
  inline int face_voisins(int num_face, int dir) const { return ref_cast(Domaine_VEF,le_dom.valeur()).face_voisins(num_face, dir); }
  inline int elem_faces(int num_elem, int i) const { return ref_cast(Domaine_VEF,le_dom.valeur()).elem_faces(num_elem, i); }

private:
  _TYPE_ evaluateur_source_face;
  int nb_faces, nb_faces_elem, premiere_face_std;
  mutable DoubleTab coef;
  DoubleVect volumes_cl_dirichlet_;

  template <typename Type_Double>  DoubleTab& ajouter_faces_standard(const int, DoubleTab& ) const;
  template <typename Type_Double> DoubleTab& ajouter_faces_non_standard(const int, DoubleTab& ) const;
};

template<typename _TYPE_>
inline int Iterateur_Source_VEF_Face<_TYPE_>::initialiser(double tps)
{
  remplir_volumes_cl_dirichlet();
  evaluateur().changer_volumes_entrelaces_Cl(volumes_cl_dirichlet_);
  return 1;
}

template<typename _TYPE_>
DoubleTab& Iterateur_Source_VEF_Face<_TYPE_>::ajouter(DoubleTab& resu) const
{
  ((_TYPE_&) (evaluateur_source_face)).mettre_a_jour();

  assert(resu.nb_dim() < 3);
  int ncomp = resu.line_size();

  DoubleVect& bilan = so_base->bilan();
  bilan.resize(ncomp);
  bilan = 0;
  int nb_faces_tot = ref_cast(Domaine_VEF,le_dom.valeur()).nb_faces_tot();
  coef.resize(nb_faces_tot, RESIZE_OPTIONS::NOCOPY_NOINIT);
  coef = 1;
  if (equation_divisee_par_rho())
    {
      const Milieu_base& milieu = la_zcl->equation().milieu();
      const Champ& rho = milieu.masse_volumique();
      if (sub_type(Champ_Uniforme, rho.valeur()))
        coef = rho->valeurs()(0, 0);
      else
        {
          const DoubleTab& val_rho = rho->valeurs();
          const IntTab& face_vois = le_dom->face_voisins();
          const DoubleVect& volumes = ref_cast(Domaine_VEF,le_dom.valeur()).volumes();
          coef = 0.;
          for (int fac = 0; fac < nb_faces_tot; fac++)
            {
              int elem1 = face_vois(fac, 0);
              int elem2 = face_vois(fac, 1);
              double vol = 0;
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
      ajouter_faces_non_standard<SingleDouble>(ncomp,resu);
      ajouter_faces_standard<SingleDouble>(ncomp,resu);
    }
  else
    {
      ajouter_faces_non_standard<ArrOfDouble>(ncomp,resu);
      ajouter_faces_standard<ArrOfDouble>(ncomp,resu);
    }
  return resu;
}

template<typename _TYPE_>
void Iterateur_Source_VEF_Face<_TYPE_>::remplir_volumes_cl_dirichlet()
{
  Domaine_VEF& zvef = ref_cast(Domaine_VEF,le_dom.valeur());
  Domaine_Cl_VEF& zclvef = ref_cast(Domaine_Cl_VEF,la_zcl.valeur());
  zvef.creer_tableau_faces(volumes_cl_dirichlet_);
  DoubleVect& marq = volumes_cl_dirichlet_;
  for (int f = 0; f < zvef.premiere_face_std(); f++)
    marq[f] = zclvef.volumes_entrelaces_Cl()[f];
  for (int f = zvef.premiere_face_std(); f < zvef.nb_faces(); f++)
    marq[f] = volumes_entrelaces(f);
  for (int num_cl = 0; num_cl < zvef.nb_front_Cl() * 0; num_cl++)
    {
      const Cond_lim& la_cl = zclvef.les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
      int nf = le_bord.nb_faces();
      if (sub_type(Dirichlet, la_cl.valeur()))
        for (int ind_face = 0; ind_face < nf; ind_face++)
          {
            int num_face = le_bord.num_face(ind_face);
            if (volumes_entrelaces_Cl(num_face) == 0)
              marq[num_face] = 1;
          }
    }
  marq.echange_espace_virtuel();
}

template<typename _TYPE_> template<typename Type_Double>
DoubleTab& Iterateur_Source_VEF_Face<_TYPE_>::ajouter_faces_non_standard(const int ncomp, DoubleTab& resu) const
{
  int num_cl;
  int num_face, nfin = 0;
  Type_Double source(ncomp);
  DoubleVect& bilan = so_base->bilan();
  for (num_cl = 0; num_cl < le_dom->nb_front_Cl(); num_cl++)
    {
      const Cond_lim& la_cl = la_zcl->les_conditions_limites(num_cl);
      const Front_VF& le_bord = ref_cast(Front_VF, la_cl->frontiere_dis());
      int nf = le_bord.nb_faces_tot();
      nfin = le_bord.num_premiere_face() + le_bord.nb_faces();
      int type_CL = 0;
      if (sub_type(Dirichlet, la_cl.valeur()))
        type_CL = 1;
      for (int ind_face = 0; ind_face < nf; ind_face++)
        {
          num_face = le_bord.num_face(ind_face);
          evaluateur_source_face.calculer_terme_source_non_standard(num_face, source);
          if (volumes_entrelaces_Cl(num_face) == 0)
            {
              int faces_dirichlet = 0;
              int face_voisine;
              for (int i = 0; i < nb_faces_elem; i++)
                {
                  face_voisine = elem_faces(face_voisins(num_face, 0), i);
                  if (volumes_entrelaces_Cl(face_voisine) == 0)
                    faces_dirichlet += 1;
                }
              for (int i = 0; i < nb_faces_elem; i++)
                {
                  face_voisine = elem_faces(face_voisins(num_face, 0), i);
                  if (volumes_entrelaces_Cl(face_voisine) != 0)
                    for (int k = 0; k < ncomp; k++)
                      {
                        resu(face_voisine, k) += source(k) / (nb_faces_elem - faces_dirichlet);
                        if (face_voisine < ref_cast(Domaine_VEF,le_dom.valeur()).nb_faces())
                          {
                            double contribution = (faces_doubles(face_voisine) == 1) ? 0.5 : 1;
                            bilan(k) += contribution * coef(face_voisine) * source(k) / (nb_faces_elem - faces_dirichlet);
                          }
                      }
                }
            }
          else
            {
              for (int k = 0; k < ncomp; k++)
                {
                  resu(num_face, k) += source(k);
                  if (num_face < ref_cast(Domaine_VEF,le_dom.valeur()).nb_faces())
                    {
                      double contribution = (faces_doubles(num_face) == 1) ? 0.5 : 1;
                      bilan(k) += (1. - type_CL) * contribution * coef(num_face) * source(k);
                    }
                }
            }
        }
    }
  for (num_face = nfin; num_face < premiere_face_std; num_face++)
    {
      evaluateur_source_face.calculer_terme_source_non_standard(num_face, source);
      for (int k = 0; k < ncomp; k++)
        {
          resu(num_face, k) += source(k);
          double contribution = (faces_doubles(num_face) == 1) ? 0.5 : 1;
          bilan(k) += contribution * coef(num_face) * source(k);
        }
    }
  return resu;
}

template<typename _TYPE_> template<typename Type_Double>
DoubleTab& Iterateur_Source_VEF_Face<_TYPE_>::ajouter_faces_standard(const int ncomp, DoubleTab& resu) const
{
  Type_Double source(ncomp);
  DoubleVect& bilan = so_base->bilan();
  for (int num_face = premiere_face_std; num_face < nb_faces; num_face++)
    {
      evaluateur_source_face.calculer_terme_source_standard(num_face, source);
      for (int k = 0; k < ncomp; k++)
        {
          resu(num_face, k) += source(k);
          double contribution = (faces_doubles(num_face) == 1) ? 0.5 : 1;
          bilan(k) += contribution * coef(num_face) * source(k);
        }
    }
  return resu;
}

#endif /* Iterateur_Source_VEF_Face_included */
