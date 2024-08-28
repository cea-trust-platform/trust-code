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

#include <Option_DG.h>
#include <Motcle.h>
#include <Param.h>
#include <SChaine.h>


int Option_DG::DEFAULT_ORDER = 1;
std::map<std::string, int> Option_DG::ORDER_OVERRIDE = {};

Implemente_instanciable(Option_DG,"Option_DG",Interprete);

Sortie& Option_DG::printOn(Sortie& os) const { return Interprete::printOn(os); }

Entree& Option_DG::readOn(Entree& is) {  return is; }

Entree& Option_DG::interpreter(Entree& is)
{
  int vo=-1, po=-1, to=-1;

  Param param(que_suis_je());
  param.ajouter("order",&DEFAULT_ORDER);
  param.ajouter("velocity_order",&vo);
  param.ajouter("pressure_order",&po);
  param.ajouter("temperature_order",&to);
  param.lire_avec_accolades_depuis(is);

  if (vo != -1)
    ORDER_OVERRIDE["velocity"] = vo;
  if (to != -1)
    ORDER_OVERRIDE["temperature"] = to;
  if (po != -1)
    ORDER_OVERRIDE["pressure"] = to;

  return is;
}


int Option_DG::Get_order_for(const Nom& n)
{
  const std::string& s = n.getString();
  if(ORDER_OVERRIDE.count(s))
    return ORDER_OVERRIDE.at(s);
  return DEFAULT_ORDER;
}

/*! @return the number of columns necessary in the unknown vector for a given
 * method order. For example order 1 and 2D means we deal with the basis {1, X, Y}, so 3 cols.
 */
int Option_DG::Nb_col_from_order(const int order)
{
  int nb_cols = -1;
  // Order
  if (Objet_U::dimension == 2)
    nb_cols = (order + 1)*(order + 2) / 2;
  else // 3D
    nb_cols = (order + 1)*(order + 2)*(order + 3) / 6;

  return nb_cols;
}
