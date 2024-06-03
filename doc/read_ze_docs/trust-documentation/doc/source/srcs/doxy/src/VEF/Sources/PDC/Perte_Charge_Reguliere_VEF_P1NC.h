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


#ifndef Perte_Charge_Reguliere_VEF_P1NC_included
#define Perte_Charge_Reguliere_VEF_P1NC_included

#include <Perte_Charge_VEF_Face.h>
#include <Perte_Charge_Reguliere.h>

/*! @brief class Perte_Charge_Reguliere_VEF_P1NC
 *
 *
 *
 * @sa Perte_Charge_VEF_P1NC
 */
class Perte_Charge_Reguliere_VEF_P1NC : public Perte_Charge_VEF_Face,
  public Perte_Charge_Reguliere

{

  Declare_instanciable(Perte_Charge_Reguliere_VEF_P1NC);

public:

  DoubleTab& ajouter(DoubleTab& ) const override;
  DoubleTab& calculer(DoubleTab& ) const override ;
  void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const override ;
  void remplir_num_faces(Nom& );
  void completer() override;
protected:

  IntVect num_faces;
  Nom nom_sous_domaine;
  DoubleVect corr_front_ss;


};

#endif
