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

#include<Prepro_IBM_base.h>

Implemente_base(Prepro_IBM_base, "Prepro_IBM_base", Objet_U); // createur

Entree& Prepro_IBM_base::readOn(Entree& s)
{
  Param param(que_suis_je());
  set_param(param);
  return s;
}

void Prepro_IBM_base::set_param(Param& param)
{
  param.ajouter("epsilon_prepro_IBM",&eps_,Param::OPTIONAL);  //
  param.ajouter("constante_c_IBM",&c_prepro_,Param::OPTIONAL);  //
  param.ajouter_non_std("directions_pt_fluid",(this),Param::OPTIONAL);
  param.ajouter("MESH_Euler",&aDomUMesh_, Param::OPTIONAL); // maillage volumique
  param.ajouter("MESH_Lagrange",&aSkinUMesh_, Param::OPTIONAL); // maillage surfacique
  param.ajouter_flag("sauvegarde_resultats_prepro",&save_prepro_);
}

int Prepro_IBM_base::lire_motcle_non_standard(const Motcle& un_mot, Entree& is)
{
  Cout << "Prepro_IBM => " << finl;
  Cout<<"epsilon_prepro_IBM = "<<eps_<<endl;
  Cout<<"constante_prepro_c_IBM = "<<c_prepro_<<endl;
  if (un_mot == "directions_pt_fluid")
    {
      Cout << "reading search directions for reference fluid pt... " << finl;
      int dim_lu;
      is >> dim_lu;
      int dim_geom = Objet_U::dimension;
      if(dim_lu != dim_geom)
        {
          Cerr<<"Prepro_IBM : dim_lu <> dim_geom = "<<dim_geom<<endl;
          abort();
        }
      dimTab_.resize(dim_lu);
      for (int i = 0; i < dim_lu; i++) is >> dimTab_(i);
      for (int i = 0; i < dim_lu; i++)
        {
          if (i == 0) Cout<<"directions : "<<dimTab_(0);
          else Cout<<" "<<dimTab_(i);
          if (i == (dim_lu-1)) Cout<<endl;
        }
    }

  return 1;
}

Sortie& Prepro_IBM_base::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}
