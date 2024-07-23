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

#include <Perte_Charge_Circulaire_PolyMAC_Face.h>
#include <Motcle.h>
#include <Equation_base.h>

#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Sous_domaine_VF.h>
#include <Domaine_PolyMAC_P0P1NC.h>
#include <Matrice_Morse.h>
#include <Schema_Temps_base.h>
#include <TRUSTTrav.h>
#include <Param.h>

Implemente_instanciable(Perte_Charge_Circulaire_PolyMAC_Face, "Perte_Charge_Circulaire_Face_PolyMAC", Perte_Charge_PolyMAC);
Implemente_instanciable(Perte_Charge_Circulaire_PolyMAC_P0P1NC_Face, "Perte_Charge_Circulaire_Face_PolyMAC_P0P1NC|Perte_Charge_Circulaire_Face_PolyMAC_P0", Perte_Charge_Circulaire_PolyMAC_Face);

Sortie& Perte_Charge_Circulaire_PolyMAC_Face::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Sortie& Perte_Charge_Circulaire_PolyMAC_P0P1NC_Face::printOn(Sortie& s) const { return s << que_suis_je() << finl; }
Entree& Perte_Charge_Circulaire_PolyMAC_P0P1NC_Face::readOn(Entree& s) { return Perte_Charge_Circulaire_PolyMAC_Face::readOn(s); }

Entree& Perte_Charge_Circulaire_PolyMAC_Face::readOn(Entree& s)
{
  Perte_Charge_PolyMAC::readOn(s);
  if (v->nb_comp() != dimension)
    {
      Cerr << "Il faut definir le champ direction a " << dimension << " composantes" << finl;
      Process::exit();
    }
  return s;
}

void Perte_Charge_Circulaire_PolyMAC_Face::set_param(Param& param)
{
  Perte_Charge_PolyMAC::set_param(param);
  param.ajouter_non_std("lambda_ortho", (this), Param::REQUIRED);
  param.ajouter("diam_hydr_ortho", &diam_hydr_ortho, Param::REQUIRED);
  param.ajouter("direction", &v, Param::REQUIRED);
}

int Perte_Charge_Circulaire_PolyMAC_Face::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot == "lambda")
    {
      lambda.addVar("Re_tot");
      lambda.addVar("Re_long");
      return Perte_Charge_PolyMAC::lire_motcle_non_standard(mot, is);
    }
  else if (mot == "lambda_ortho")
    {
      Nom tmp;
      is >> tmp;
      Cerr << "Lecture et interpretation de la fonction " << tmp << " ... ";
      lambda_ortho.setNbVar(3 + dimension);
      lambda_ortho.setString(tmp);
      lambda_ortho.addVar("Re_tot");
      lambda_ortho.addVar("Re_ortho");
      lambda_ortho.addVar("t");
      lambda_ortho.addVar("x");
      if (dimension > 1)
        lambda_ortho.addVar("y");
      if (dimension > 2)
        lambda_ortho.addVar("z");
      lambda_ortho.parseString();
      Cerr << " Ok" << finl;
      return 1;
    }
  else
    {
      return Perte_Charge_PolyMAC::lire_motcle_non_standard(mot, is);
    }
}

void Perte_Charge_Circulaire_PolyMAC_Face::coeffs_perte_charge(const DoubleVect& u, const DoubleVect& pos, double t, double norme_u, double dh, double nu, double reynolds, double& coeff_ortho,
                                                               double& coeff_long, double& u_l, DoubleVect& av_valeur) const
{

  // calcul de dh_ortho
  double dh_ortho = diam_hydr_ortho->valeur_a_compo(pos, 0);

  // calcul de u.d/||d||
  // Calcul de v et ||v||^2
  av_valeur.resize(dimension);

  v->valeur_a(pos, av_valeur);
  // on norme v
  {
    double vcarre = 0;
    for (int dim = 0; dim < dimension; dim++)
      vcarre += av_valeur[dim] * av_valeur[dim];
    av_valeur /= sqrt(vcarre);
  }
  // Calcul de u.v/||v||
  u_l = 0;

  for (int dim = 0; dim < dimension; dim++)
    u_l += u[dim] * av_valeur[dim];

  double u_ortho = sqrt(norme_u * norme_u - u_l * u_l);
  // calcule de Re_l et Re_ortho
  // Calcul du reynolds
  /* PL: To avoid a possible division by zero, we replace:
   double nu=norme_u*dh/reynolds;
   double Re_l=std::fabs(u_l)*dh/nu; */
  // By:
  double Re_l = dh * std::fabs(u_l) / nu;
  if (Re_l < 1e-10)
    Re_l = 1e-10;
  // PL: To avoid a possible division by zero, we replace:
  /* double Re_ortho=u_ortho*dh_ortho/nu; */
  // By:
  double Re_ortho = dh_ortho * u_ortho / nu;
  if (Re_ortho < 1e-10)
    Re_ortho = 1e-10;
  // Calcul de lambda
  lambda.setVar(0, reynolds);
  lambda.setVar(1, Re_l);
  lambda.setVar(2, t);
  lambda.setVar(3, pos[0]);
  if (dimension > 1)
    lambda.setVar(4, pos[1]);
  if (dimension > 2)
    lambda.setVar(5, pos[2]);

  // Calcul de lambda_ortho
  lambda_ortho.setVar(0, reynolds);
  lambda_ortho.setVar(1, Re_ortho);
  lambda_ortho.setVar(2, t);
  lambda_ortho.setVar(3, pos[0]);
  if (dimension > 1)
    lambda_ortho.setVar(4, pos[1]);
  if (dimension > 2)
    lambda_ortho.setVar(5, pos[2]);
  double l_ortho = lambda_ortho.eval(); // Pour ne pas evaluer 2 fois le parser
  double l_long = lambda.eval();
  coeff_ortho = l_ortho * u_ortho / 2. / dh_ortho;
  coeff_long = l_long * std::fabs(u_l) / 2. / dh;
}
