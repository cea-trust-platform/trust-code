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

#include <Perte_Charge_Directionnelle_PolyMAC_P0P1NC_Face.h>
#include <Motcle.h>
#include <Equation_base.h>
#include <Param.h>

Implemente_instanciable(Perte_Charge_Directionnelle_PolyMAC_P0P1NC_Face,"Perte_Charge_Directionnelle_Face_PolyMAC_P0P1NC|Perte_Charge_Directionnelle_Face_PolyMAC_P0",Perte_Charge_PolyMAC_P0P1NC);


// printOn
//

Sortie& Perte_Charge_Directionnelle_PolyMAC_P0P1NC_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() << finl;
}

////////////////////////////////////////////////////////////////
//                                                            //
//                         readOn                             //
//                                                            //
////////////////////////////////////////////////////////////////

Entree& Perte_Charge_Directionnelle_PolyMAC_P0P1NC_Face::readOn(Entree& s )
{
  Perte_Charge_PolyMAC_P0P1NC::readOn(s);
  if (v->nb_comp()!=dimension)
    {
      Cerr << "Il faut definir le champ direction a " << dimension << " composantes" << finl;
      exit();
    }
  return s;
}

void Perte_Charge_Directionnelle_PolyMAC_P0P1NC_Face::set_param(Param& param)
{
  Perte_Charge_PolyMAC_P0P1NC::set_param(param);
  param.ajouter("direction",&v,Param::REQUIRED);
}

////////////////////////////////////////////////////////////////
//                                                            //
//           Fonction principale : perte_charge               //
//                                                            //
////////////////////////////////////////////////////////////////

void Perte_Charge_Directionnelle_PolyMAC_P0P1NC_Face::coeffs_perte_charge(const DoubleVect& u, const DoubleVect& pos,
                                                                          double t, double norme_u, double dh, double nu, double reynolds,double& coeff_ortho, double& coeff_long,double& u_l, DoubleVect& v_valeur) const
{

  // Calcul de lambda
  lambda.setVar(0,reynolds);
  lambda.setVar(1,t);
  lambda.setVar(2,pos[0]);
  if (dimension>1)
    lambda.setVar(3,pos[1]);
  if (dimension>2)
    lambda.setVar(4,pos[2]);

  // Calcul de v et ||v||^2
  //  DoubleVect v_valeur(dimension);
  double vcarre=0;
  v.valeur().valeur_a(pos,v_valeur);
  for (int dim=0; dim<dimension; dim++)
    vcarre+=v_valeur[dim]*v_valeur[dim];
  v_valeur/=sqrt(vcarre);
  // Calcul de u.v
  double scal=0;
  for (int dim=0; dim<dimension; dim++)
    scal+=u[dim]*v_valeur[dim];
  /*
  // Calcul du resultat
  for (int dim=0;dim<dimension;dim++)
  p_charge[dim] = -lambda.eval()*scal*v_valeur[dim]*norme_u/2./dh;
  */
  u_l=scal;
  coeff_long=lambda.eval()*norme_u/2./dh;
  coeff_ortho=0;
}
