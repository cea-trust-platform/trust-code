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

#ifndef Op_Evanescence_Homogene_Face_base_included
#define Op_Evanescence_Homogene_Face_base_included

#include <Operateur_Evanescence_base.h>
#include <set>

/*! @brief Classe Op_Evanescence_Homogene_Face_base
 *
 *    gestion de l'evanescence dans une equation aux faces (-> QDM)
 *
 * @sa Operateur_Evanescence_base Operateur_base
 */
class Op_Evanescence_Homogene_Face_base: public Operateur_Evanescence_base
{
  Declare_base(Op_Evanescence_Homogene_Face_base);
public :
  int has_interface_blocs() const override { return 1; }
  void dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl = {}) const override;
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl = {}) const override;
  double alpha_res() const override { return alpha_res_; }

protected:
  double alpha_res_ = 0., alpha_res_min_ = 0.; //seuil de declenchement du traitement de l'evanescence
  virtual void calc_grad_alpha_faces(DoubleTab&) const {Process::exit(que_suis_je() + " : calc_grad_alpha_faces is not defined !");};
  virtual void calc_vort_faces(DoubleTab&) const {Process::exit(que_suis_je() + " : calc_grad_alpha_faces is not defined !");};

private:
  virtual void dimensionner_blocs_aux(std::set<int>&, IntTrav& ,  Matrice_Morse& ) const = 0;
  virtual void ajouter_blocs_aux(IntTrav& , DoubleTrav , matrices_t , DoubleTab&) const = 0;
};

#endif /* Op_Evanescence_Homogene_Face_base_included */
