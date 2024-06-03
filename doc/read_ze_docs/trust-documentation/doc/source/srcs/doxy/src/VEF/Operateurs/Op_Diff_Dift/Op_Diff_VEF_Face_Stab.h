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

#ifndef Op_Diff_VEF_Face_Stab_included
#define Op_Diff_VEF_Face_Stab_included

#include <Op_Diff_VEF_Face.h>

/*! @brief class Op_Diff_VEF_Face_Stab Cette classe represente l'operateur de diffusion
 *
 *   La discretisation est VEF
 *   Le champ diffuse est scalaire
 *
 */

class Op_Diff_VEF_Face_Stab : public Op_Diff_VEF_Face
{
  Declare_instanciable_sans_constructeur(Op_Diff_VEF_Face_Stab);

public:
  Op_Diff_VEF_Face_Stab();
  DoubleTab& ajouter(const DoubleTab&, DoubleTab&) const override;
  void completer() override;

  void ajouter_contribution(const DoubleTab&, Matrice_Morse&) const;
  void ajouter_contribution_multi_scalaire(const DoubleTab&, Matrice_Morse&) const;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override;

protected :

  void calculer_coefficients(const DoubleTab&, DoubleTab&) const;
  void calculer_min(const DoubleTab&, int&, DoubleTab&) const ;
  void calculer_max(const DoubleTab&, int&, DoubleTab&) const ;
  double calculer_gradients(int, const DoubleTab&) const ;

  void modifie_pour_Cl(const DoubleTab&, DoubleTab&) const;
  void ajouter_operateur_centre(const DoubleTab&, const DoubleTab&, DoubleTab&) const ;
  void ajouter_diffusion(const DoubleTab&, const DoubleTab&, DoubleTab&) const ;
  void ajouter_antidiffusion(const DoubleTab&, const DoubleTab&, DoubleTab&) const ;

private :

  int standard_=0;
  int info_=0;
  int new_jacobian_=0;
  IntTab is_dirichlet_faces_;
};

inline void Op_Diff_VEF_Face_Stab::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  if (!new_jacobian_)
    Op_Diff_VEF_Face::contribuer_a_avec(inco,matrice);
  else
    {
      const Champ_base& inconnue = equation().inconnue().valeur();
      const Nature_du_champ nature_champ = inconnue.nature_du_champ();

      if (nature_champ!=multi_scalaire)
        ajouter_contribution(inco, matrice);
      else
        ajouter_contribution_multi_scalaire(inco, matrice);
    }
}

#endif
