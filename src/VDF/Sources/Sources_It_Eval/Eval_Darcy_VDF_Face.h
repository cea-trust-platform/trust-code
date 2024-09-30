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

#ifndef Eval_Darcy_VDF_Face_included
#define Eval_Darcy_VDF_Face_included

#include <Modele_Permeabilite_base.h>
#include <Evaluateur_Source_Face.h>
#include <Champ_Uniforme.h>
#include <Domaine_VDF.h>


#include <TRUST_Ref.h>

class Champ_Don_base;

class Eval_Darcy_VDF_Face: public Evaluateur_Source_Face
{
public:
  Eval_Darcy_VDF_Face() : porosite_(1.) { }
  Eval_Darcy_VDF_Face(const Eval_Darcy_VDF_Face& eval) : Evaluateur_Source_Face(eval), porosite_(1.) { }
  inline double& getPorosite() { return porosite_; }
  template <typename Type_Double> void calculer_terme_source(const int , Type_Double& ) const;
  template <typename Type_Double> void calculer_terme_source_bord(int num_face, Type_Double& source) const { calculer_terme_source(num_face,source); }
  inline void mettre_a_jour() override;
  inline void associer(const Champ_Don_base&);
  inline void associer(const Champ_Inc_base& vit) { vitesse_ = vit; }

  OWN_PTR(Modele_Permeabilite_base) modK_;

protected:
  REF(Champ_Inc_base) vitesse_;
  REF(Champ_Don_base) diffusivite_;
  DoubleTab db_diffusivite_;
  double porosite_;
};

/*! @brief associe le champ de diffusivite
 *
 */
inline void Eval_Darcy_VDF_Face::associer(const Champ_Don_base& diffu)
{
  const int nb_faces_tot = ref_cast(Domaine_VDF,le_dom.valeur()).nb_faces_tot();
  db_diffusivite_.resize(nb_faces_tot,RESIZE_OPTIONS::NOCOPY_NOINIT);
  diffusivite_ = diffu;
  mettre_a_jour();
}

inline void Eval_Darcy_VDF_Face::mettre_a_jour()
{
  const int nb_faces_tot = ref_cast(Domaine_VDF,le_dom.valeur()).nb_faces_tot();
  if (sub_type(Champ_Uniforme,diffusivite_.valeur()))
    db_diffusivite_ = diffusivite_->valeurs()(0,0);
  else
    {
      const DoubleTab& val_diff = diffusivite_->valeurs();
      const IntTab& face_vois = le_dom->face_voisins();
      const ArrOfDouble& volumes = ref_cast(Domaine_VDF,le_dom.valeur()).volumes();
      db_diffusivite_ = 0.;
      // Compute nu(fac)=(nu(elem1).vol(elem1)+nu(elem2).vol(elem2))/(vol(elem1)+vol(elem2))
      // Viscosity at face is the volume weighted average of viscosity on elements
      for (int fac = 0; fac < nb_faces_tot; fac++)
        {
          const int elem1 = face_vois(fac,0), elem2 = face_vois(fac,1);
          double vol = 0;
          if (elem1!=-1)
            {
              db_diffusivite_(fac) += val_diff(elem1)*volumes[elem1];
              vol += volumes[elem1];
            }
          if (elem2!=-1)
            {
              db_diffusivite_(fac) += val_diff(elem2)*volumes[elem2];
              vol += volumes[elem2];
            }
          db_diffusivite_(fac) /= vol;
        }
    }
}

template <typename Type_Double>
void Eval_Darcy_VDF_Face::calculer_terme_source(const int num_face, Type_Double& source) const
{
  const int size = source.size_array();
  for (int i = 0; i < size; i++) source[i] = -db_diffusivite_(num_face)/modK_->getK(porosite_)*volumes_entrelaces(num_face)*porosite_surf(num_face)*(vitesse_->valeurs())(num_face,i); // -mu.vol.psi.U/K
}

#endif /* Eval_Darcy_VDF_Face_included */
