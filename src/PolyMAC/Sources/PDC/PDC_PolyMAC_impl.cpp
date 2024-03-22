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

#include <PDC_PolyMAC_impl.h>
#include <Equation_base.h>
#include <Motcle.h>
#include <Param.h>

int PDC_Anisotrope_PolyMAC::lire_motcle_non_standard_impl(const Motcle& mot, Entree& is)
{
  Nom tmp;
  is >> tmp;
  Cerr << "Lecture et interpretation de la fonction " << tmp << " ... ";
  lambda_ortho.setNbVar(2 + Objet_U::dimension);
  lambda_ortho.setString(tmp);
  lambda_ortho.addVar("Re");
  lambda_ortho.addVar("t");
  lambda_ortho.addVar("x");
  if (Objet_U::dimension > 1)
    lambda_ortho.addVar("y");
  if (Objet_U::dimension > 2)
    lambda_ortho.addVar("z");
  lambda_ortho.parseString();
  Cerr << " Ok" << finl;
  return 1;
}

void PDC_Anisotrope_PolyMAC::coeffs_perte_charge_impl(const DoubleVect& u, const DoubleVect& pos, double t, double norme_u,
                                                      double dh, double nu, double reynolds, double K, double& coeff_ortho,
                                                      double& coeff_long, double& u_l, DoubleVect& v_valeur, Parser_U& lambda) const
{
  // Calcul de lambda
  lambda.setVar(0, reynolds);
  lambda.setVar(1, t);
  lambda.setVar(2, pos[0]);
  if (Objet_U::dimension > 1)
    lambda.setVar(3, pos[1]);
  if (Objet_U::dimension > 2)
    lambda.setVar(4, pos[2]);

  // Calcul de lambda_ortho
  lambda_ortho.setVar(0, reynolds);
  lambda_ortho.setVar(1, t);
  lambda_ortho.setVar(2, pos[0]);
  if (Objet_U::dimension > 1)
    lambda_ortho.setVar(3, pos[1]);
  if (Objet_U::dimension > 2)
    lambda_ortho.setVar(4, pos[2]);
  double l_ortho = lambda_ortho.eval(); // Pour ne pas evaluer 2 fois le parser

  // Calcul de v et ||v||^2
  //  DoubleVect v_valeur(dimension);
  double vcarre = 0;
  v->valeur_a(pos, v_valeur);
  for (int dim = 0; dim < Objet_U::dimension; dim++)
    vcarre += v_valeur[dim] * v_valeur[dim];
  v_valeur /= sqrt(vcarre);
  // Calcul de u.v
  double scal = 0;
  for (int dim = 0; dim < Objet_U::dimension; dim++)
    scal += u[dim] * v_valeur[dim];

  // Calcul du resultat
  /*
   for (int dim=0;dim<dimension;dim++)
   p_charge[dim] = -l_ortho*norme_u/2./dh*u[dim]
   -(lambda.eval()-l_ortho)*scal*v_valeur[dim]*norme_u/2./dh;
   */
  coeff_ortho = K * l_ortho * norme_u / 2. / dh;
  coeff_long = K * lambda.eval() * norme_u / 2. / dh;
  u_l = scal;
}

int PDC_Circulaire_PolyMAC::lire_motcle_non_standard_impl(const Motcle& mot, Entree& is)
{
  Nom tmp;
  is >> tmp;
  Cerr << "Lecture et interpretation de la fonction " << tmp << " ... ";
  lambda_ortho.setNbVar(3 + Objet_U::dimension);
  lambda_ortho.setString(tmp);
  lambda_ortho.addVar("Re_tot");
  lambda_ortho.addVar("Re_ortho");
  lambda_ortho.addVar("t");
  lambda_ortho.addVar("x");
  if (Objet_U::dimension > 1)
    lambda_ortho.addVar("y");
  if (Objet_U::dimension > 2)
    lambda_ortho.addVar("z");
  lambda_ortho.parseString();
  Cerr << " Ok" << finl;
  return 1;
}

void PDC_Circulaire_PolyMAC::coeffs_perte_charge_impl(const DoubleVect& u, const DoubleVect& pos, double t,
                                                      double norme_u, double dh, double nu, double reynolds, double K, double& coeff_ortho,
                                                      double& coeff_long, double& u_l, DoubleVect& av_valeur, Parser_U& lambda) const
{
  // calcul de dh_ortho
  double dh_ortho = diam_hydr_ortho->valeur_a_compo(pos, 0);

  // calcul de u.d/||d||
  // Calcul de v et ||v||^2
  av_valeur.resize(Objet_U::dimension);

  v->valeur_a(pos, av_valeur);
  // on norme v
  {
    double vcarre = 0;
    for (int dim = 0; dim < Objet_U::dimension; dim++)
      vcarre += av_valeur[dim] * av_valeur[dim];
    av_valeur /= sqrt(vcarre);
  }
  // Calcul de u.v/||v||
  u_l = 0;

  for (int dim = 0; dim < Objet_U::dimension; dim++)
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
  if (Objet_U::dimension > 1)
    lambda.setVar(4, pos[1]);
  if (Objet_U::dimension > 2)
    lambda.setVar(5, pos[2]);

  // Calcul de lambda_ortho
  lambda_ortho.setVar(0, reynolds);
  lambda_ortho.setVar(1, Re_ortho);
  lambda_ortho.setVar(2, t);
  lambda_ortho.setVar(3, pos[0]);
  if (Objet_U::dimension > 1)
    lambda_ortho.setVar(4, pos[1]);
  if (Objet_U::dimension > 2)
    lambda_ortho.setVar(5, pos[2]);
  double l_ortho = lambda_ortho.eval(); // Pour ne pas evaluer 2 fois le parser
  double l_long = lambda.eval();
  coeff_ortho = K * l_ortho * u_ortho / 2. / dh_ortho;
  coeff_long = K * l_long * std::fabs(u_l) / 2. / dh;
}

void PDC_Directionnelle_PolyMAC::coeffs_perte_charge_impl(const DoubleVect& u, const DoubleVect& pos, double t, double norme_u,
                                                          double dh, double nu, double reynolds, double K, double& coeff_ortho,
                                                          double& coeff_long, double& u_l, DoubleVect& v_valeur, Parser_U& lambda) const
{
  // Calcul de lambda
  lambda.setVar(0, reynolds);
  lambda.setVar(1, t);
  lambda.setVar(2, pos[0]);
  if (Objet_U::dimension > 1)
    lambda.setVar(3, pos[1]);
  if (Objet_U::dimension > 2)
    lambda.setVar(4, pos[2]);

  // Calcul de v et ||v||^2
  //  DoubleVect v_valeur(dimension);
  double vcarre = 0;
  v->valeur_a(pos, v_valeur);
  for (int dim = 0; dim < Objet_U::dimension; dim++)
    vcarre += v_valeur[dim] * v_valeur[dim];
  v_valeur /= sqrt(vcarre);
  // Calcul de u.v
  double scal = 0;
  for (int dim = 0; dim < Objet_U::dimension; dim++)
    scal += u[dim] * v_valeur[dim];
  /*
   // Calcul du resultat
   for (int dim=0;dim<dimension;dim++)
   p_charge[dim] = -lambda.eval()*scal*v_valeur[dim]*norme_u/2./dh;
   */
  u_l = scal;
  coeff_long = K * lambda.eval() * norme_u / 2. / dh;
  coeff_ortho = 0;
}

void PDC_Isotrope_PolyMAC::coeffs_perte_charge_impl(const DoubleVect& u, const DoubleVect& pos, double t, double norme_u,
                                                    double dh, double nu, double reynolds, double K, double& coeff_ortho,
                                                    double& coeff_long, double& u_l, DoubleVect& v_valeur, Parser_U& lambda) const
{
  // Calcul de lambda
  lambda.setVar(0, reynolds);
  lambda.setVar(1, t);
  lambda.setVar(2, pos[0]);
  if (Objet_U::dimension > 1)
    lambda.setVar(3, pos[1]);
  if (Objet_U::dimension > 2)
    lambda.setVar(4, pos[2]);

  // Calcul du resultat
  coeff_ortho = K * lambda.eval() * norme_u / 2. / dh;
  coeff_long = coeff_ortho;
  // v ne sert pas, car coeff_ortho=coeff_long
  //  for (int dim=0;dim<dimension;dim++)
  //  p_charge[dim] = -lambda.eval()*norme_u/2./dh*u[dim];
  u_l = 0;
}
