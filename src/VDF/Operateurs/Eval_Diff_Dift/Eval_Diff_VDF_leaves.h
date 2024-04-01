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

#ifndef Eval_Diff_VDF_leaves_included
#define Eval_Diff_VDF_leaves_included

#include <Eval_Diff_VDF_Elem_Gen.h>
#include <Eval_Diff_VDF_Face_Gen.h>
#include <Eval_Diff_VDF.h>

/// \cond DO_NOT_DOCUMENT
class Eval_Diff_VDF_leaves
{};
/// \endcond

/*! @brief class Eval_Diff_VDF_Elem_Axi Evaluateur VDF pour la diffusion en coordonnees cylindriques
 *
 *  Le champ diffuse est scalaire (Champ_P0_VDF).
 *
 * @sa Eval_Diff_VDF
 */
class Eval_Diff_VDF_Elem_Axi :public Eval_Diff_VDF_Elem_Gen<Eval_Diff_VDF_Elem_Axi>, public Eval_Diff_VDF
{
public:
  static constexpr bool IS_AXI = true;
};

/*! @brief class Eval_Diff_VDF_Elem Evaluateur VDF pour la diffusion
 *
 *  Le champ diffuse est scalaire (Champ_P0_VDF).
 *
 * @sa Eval_Diff_VDF
 */
class Eval_Diff_VDF_Elem : public Eval_Diff_VDF_Elem_Gen<Eval_Diff_VDF_Elem>, public Eval_Diff_VDF { };

/*! @brief class Eval_Diff_VDF_Elem_aniso Evaluateur VDF pour la diffusion
 *
 *  Le champ diffuse est scalaire (Champ_P0_VDF).
 *
 * @sa Eval_Diff_VDF
 */
class Eval_Diff_VDF_Elem_aniso : public Eval_Diff_VDF_Elem_Gen<Eval_Diff_VDF_Elem_aniso>, public Eval_Diff_VDF
{
public:
  static constexpr bool IS_ANISO= true;
  inline void associer(const Champ_base& diffu) override
  {
    // ONLY AVAILABLE FOR TRUST : conduction problem : TODO : generalize ME
    if (diffu.le_nom() == "conductivite" && diffu.nb_comp() != Objet_U::dimension)
      {
        Cerr << "Error in Eval_Diff_VDF_aniso::associer (anisotropic diffusion VDF operator) !" << finl;
        Cerr << "Ensure that the dimension of the conductivity field is equal to the dimension of the domain !" << finl;
        Cerr << "A general conductivity tensor with non-zero cross-corellation terms is not yet supported (switch to VEF) !" << finl;
        Process::exit();
      }
    Eval_Diff_VDF::associer(diffu);
  }
};

/*! @brief class Eval_Diff_VDF_Multi_inco_Elem_Axi Evaluateur VDF pour la diffusion en coordonnees cylindriques
 *
 *  Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues. Il y a une diffusivite par inconnue.
 *
 * @sa Eval_Diff_VDF
 */
class Eval_Diff_VDF_Multi_inco_Elem_Axi : public Eval_Diff_VDF_Elem_Gen<Eval_Diff_VDF_Multi_inco_Elem_Axi>, public Eval_Diff_VDF
{
public:
  static constexpr bool IS_AXI = true;
  void mettre_a_jour() override { /* do nothing */ }
};

/*! @brief class Eval_Diff_VDF_Multi_inco_Elem Evaluateur VDF pour la diffusion
 *
 *  Le champ diffuse est scalaire (Champ_P0_VDF) avec plusieurs inconnues. Il y a une diffusivite par inconnue.
 *
 * @sa Eval_Diff_VDF
 */
class Eval_Diff_VDF_Multi_inco_Elem : public Eval_Diff_VDF_Elem_Gen<Eval_Diff_VDF_Multi_inco_Elem>, public Eval_Diff_VDF
{
public:
  void mettre_a_jour() override { /* do nothing */ }
};

/*! @brief class Eval_Diff_VDF_Face Evaluateur VDF pour la diffusion
 *
 *  Le champ diffuse est un Champ_Face_VDF
 *
 * @sa Eval_Diff_VDF
 */
class Eval_Diff_VDF_Face : public Eval_Diff_VDF_Face_Gen<Eval_Diff_VDF_Face>, public Eval_Diff_VDF { };

#endif /* Eval_Diff_VDF_leaves_included */
