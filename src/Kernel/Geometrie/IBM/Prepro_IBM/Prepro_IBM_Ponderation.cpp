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

#include <Prepro_IBM_Ponderation.h>

Implemente_instanciable( Prepro_IBM_Ponderation," Prepro_IBM_Ponderation|methode_IBM_ponderation",Prepro_IBM_base ) ;

Sortie& Prepro_IBM_Ponderation::printOn(Sortie& os) const
{
  Prepro_IBM_base::printOn(os);
  return os;
}

void Prepro_IBM_Ponderation::set_param(Param& param)
{
  Prepro_IBM_base::set_param(param);
  param.ajouter("type_de_ponderation",&pond_,Param::OPTIONAL); // choix de la methode de ponderation
}

Entree& Prepro_IBM_Ponderation::readOn(Entree& is)
{
  Prepro_IBM_base::readOn(is);
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);

  if(pond_==1)
    {
      Cout<<"Weighting method = arimethic weight"<<endl;
    }
  else if(pond_==2)
    {
      Cout<<"Weighting method = area weight"<<endl;
    }
  else if(pond_==3)
    {
      Cout<<"Weighting method =  inverse distance weight"<<endl;
    }
  else if(pond_==4)
    {
      Cout<<"Weighting method = area and inverse distance weight"<<endl;
    }
  else
    {
      Cerr<<"Prepro_IBM_ponderation : Type_de_ponderation : invalide argument = "<<pond_<<endl;
      abort();
    }
  return is;
}

void intersect_ponderation( )
{
}
