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

#ifndef Domaine_VF_inst_included
#define Domaine_VF_inst_included

#include <Domaine_VF.h>
#include <Domaine.h>
class Geometrie;

/*! @brief class Domaine_VF_inst
 *
 *       Classe instanciable qui derive de Domaine_VF. Cette classe ne sait rien faire !!!!
 *       Elle permet juste d'appeler des methodes de type Champ_Inc_base::affecter avec un domaine_dis contenant une reference a un domaine.
 *
 */
class Domaine_VF_inst : public Domaine_VF
{
  Declare_instanciable(Domaine_VF_inst);
public :
  double face_normales(int, int) const override
  {
    Cerr << "face_normales not coded in " << que_suis_je() << finl;
    throw;
  }
  const DoubleTab& face_normales() const override
  {
    Cerr << "face_normales not coded in " << que_suis_je() << finl;
    throw;
  }
  DoubleTab& face_normales() override
  {
    Cerr << "face_normales not coded in " << que_suis_je() << finl;
    throw;
  }

private:
  void remplir_elem_faces() override
  {
    Cerr<<"remplir_elem_faces not coded in "<<que_suis_je()<<finl;
  }
};

#endif /* Domaine_VF_inst_included */
