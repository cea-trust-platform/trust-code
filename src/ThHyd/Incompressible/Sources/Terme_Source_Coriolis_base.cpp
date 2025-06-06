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

#include <Terme_Source_Coriolis_base.h>
#include <Motcle.h>
#include <Param.h>

Implemente_base(Terme_Source_Coriolis_base,"Terme_Source_Coriolis_base",Source_base);
// XD coriolis source_base coriolis 1 Keyword for a Coriolis term in hydraulic equation. Warning: Only available in VDF.
// XD   attr omega list omega 0 Value of omega.

Sortie& Terme_Source_Coriolis_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }

Entree& Terme_Source_Coriolis_base::readOn(Entree& is )
{
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Terme_Source_Coriolis_base::set_param(Param& param)
{
  param.ajouter_non_std("omega",(this),Param::REQUIRED);
}

int Terme_Source_Coriolis_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  int dim_pb=Objet_U::dimension;
  int sz_lst = -1;
  if (mot ==  Motcle("omega") )
    {
      is >> sz_lst;
      if((dim_pb == 2 && sz_lst != 1) || (dim_pb == 3 && sz_lst != 3))
        {
          Cerr << "Error in Terme_Source_Coriolis_base::lire_donnees" << finl;
          Cerr << "Warning ! Dimension of vector after key-word 'omega'" << finl;
          Cerr << "is not correct - it should be 1 in 2D and 3 in 3D."<< finl;
          Process::exit();
        }
      if (sz_lst == 1)
        {
          omega_.resize(1);
          is >> omega_(0);
          return 1;
        }
      else
        {
          assert(sz_lst==3);
          omega_.resize(3);
          is >> omega_(0);
          is >> omega_(1);
          is >> omega_(2);
          return 1;
        }
    }
  else
    {
      Cerr << "Erreur a la lecture des parametres de Terme_Source_Coriolis_base " << finl;
      Cerr << "On attendait le mot cle omega a la place de  "  << mot << finl;
      Process::exit();
    }
  return -1;
}
