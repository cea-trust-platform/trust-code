#include "AbstractState.h"
#include "CoolProp.h"
#include <iostream>
#include <string>
#include <array>

int main(int argc, char const *argv[])
{
  std::string fluid = "Water";

  std::cout << "======================================================================================" << std::endl;
  std::cout << "Test simple calls to CoolProp using the low level interface with fluid : " << fluid  << std::endl;
  std::cout << "======================================================================================" << std::endl;
  std::cout << std::endl;

  double P1 = 1e5, P2 = 2e5, T1 = 300, T2 = 400;

  shared_ptr<CoolProp::AbstractState> water(CoolProp::AbstractState::factory("HEOS", fluid));

  // Set point 1
  water->update(CoolProp::PT_INPUTS, P1, T1);

  // Calculate properties at point 1
  double rho1 = water->rhomass(),
      drhodP1 = water->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT),
      drhodT1 = water->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP),
      h1 = water->hmass(),
      dhdp1 = water->first_partial_deriv(CoolProp::iHmolar, CoolProp::iP, CoolProp::iT),
      dhdT1 = water->first_partial_deriv(CoolProp::iHmolar, CoolProp::iT, CoolProp::iP),
      mu1 = water->viscosity(),
      lambda1 = water->conductivity(),
      beta1 = water->isobaric_expansion_coefficient(),
      cp1 = water->cpmass();

  // Print results
  std::cout << "Point  1 : P = " << P1 << ", T = " << T1 << std::endl;
  std::cout << "rho = " << rho1 << std::endl;
  std::cout << "drhodP = " << drhodP1 << std::endl;
  std::cout << "drhodT = " << drhodT1 << std::endl;
  std::cout << std::endl;

  std::cout << "h = " << h1 << std::endl;
  std::cout << "dh/dP = " << dhdp1 << std::endl;
  std::cout << "dh/dT = " << dhdT1 << std::endl;
  std::cout << std::endl;

  std::cout << "mu = " << mu1 << std::endl;
  std::cout << "lambda = " << lambda1 << std::endl;
  std::cout << "beta = " << beta1 << std::endl;
  std::cout << "cp = " << cp1 << std::endl;
  std::cout << std::endl;

  // Set point 2
  water->update(CoolProp::PT_INPUTS, P2, T2);

  // Calculate properties at point 2
  double rho2 = water->rhomass(),
      drhodP2 = water->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT),
      drhodT2 = water->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP),
      h2 = water->hmass(),
      dhdp2 = water->first_partial_deriv(CoolProp::iHmolar, CoolProp::iP, CoolProp::iT),
      dhdT2 = water->first_partial_deriv(CoolProp::iHmolar, CoolProp::iT, CoolProp::iP),
      mu2 = water->viscosity(),
      lambda2 = water->conductivity(),
      beta2 = water->isobaric_expansion_coefficient(),
      cp2 = water->cpmass();

  // Print results
  std::cout << "Point  1 : P = " << P2 << ", T = " << T2 << std::endl;
  std::cout << "rho = " << rho2 << std::endl;
  std::cout << "drhodP = " << drhodP2 << std::endl;
  std::cout << "drhodT = " << drhodT2 << std::endl;
  std::cout << std::endl;

  std::cout << "h = " << h2 << std::endl;
  std::cout << "dh/dP = " << dhdp2 << std::endl;
  std::cout << "dh/dT = " << dhdT2 << std::endl;
  std::cout << std::endl;

  std::cout << "mu = " << mu2 << std::endl;
  std::cout << "lambda = " << lambda2 << std::endl;
  std::cout << "beta = " << beta2 << std::endl;
  std::cout << "cp = " << cp2 << std::endl;
  std::cout << std::endl;

  return 1;
}
