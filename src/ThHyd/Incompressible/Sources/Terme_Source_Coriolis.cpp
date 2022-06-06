/****************************************************************************
* Copyright (c) 2021, CEA
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

#include <Terme_Source_Coriolis.h>
#include <Motcle.h>

void Terme_Source_Coriolis::associer_eqn(const Navier_Stokes_std& eq_hyd)
{
  eq_hydraulique_ = eq_hyd;
}

Entree& Terme_Source_Coriolis::lire_donnees(Entree& is)
{
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Motcle motlu;
  int dim_pb=Objet_U::dimension;
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "Erreur a la lecture des parametres de Terme_Source_Coriolis " << finl;
      Cerr << "On attendait " << accolade_ouverte << " a la place de "  << motlu << finl;
      Process::exit();
    }
  is >> motlu;
  if (motlu ==  Motcle("omega") )
    {
      is >> dim;
      if(dim_pb!=dim)
        {
          Cerr << "Error in Terme_Source_Coriolis::lire_donnees" << finl;
          Cerr << "Warning ! Dimension of vector after key-word 'omega'" << finl;
          Cerr << "is not correct : dimension of PB (" << dim_pb << ")!= " << dim << finl;
          Process::exit();
        }
      if (dim == 2)
        {
          omega_.resize(1);
          is >> omega_(0);
        }
      else
        {
          if (dim==3)
            {
              omega_.resize(3);
              is >> omega_(0);
              is >> omega_(1);
              is >> omega_(2);
            }
          else
            {
              Cerr << "Erreur a la lecture des parametres de Terme_Source_Coriolis" << finl;
              Cerr << "omega est de dimension 2 ou 3 et vous avez entre " << dim << finl;
              Process::exit();
            }
        }
    }
  else
    {
      Cerr << "Erreur a la lecture des parametres de Terme_Source_Coriolis " << finl;
      Cerr << "On attendait le mot cle omega a la place de  "  << motlu << finl;
      Process::exit();
    }
  is >> motlu;
  if (motlu != accolade_fermee)
    {
      Cerr << "Erreur a la lecture des parametres de Terme_Source_Coriolis " << finl;
      Cerr << "On attendait " << accolade_fermee << " a la place de "  << motlu << finl;
      Cerr << "Attention! : en 2D, omega est de dimension 1!!!" << finl;
      Process::exit();
    }
  return is;
}
