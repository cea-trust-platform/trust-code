#include "AbstractState.h"
#include "CoolProp.h"
#include <iostream>
#include <string>
#include <array>

int main(int argc, char const *argv[])
{
  std::string fluid = "Water";
  std::string backend = "HEOS";

  std::cout << "=================================================================================================================" << std::endl;
  std::cout << "Test simple calls to CoolProp using the low level interface with two phases (liquid/vapor) and fluid : " << fluid  << std::endl;
  std::cout << "=================================================================================================================" << std::endl;
  std::cout << std::endl;

  // Define two points P1 and T1
  double P = 1e5; // Pa
  double T = 300.0; // K

  CoolProp::AbstractState* liquid = CoolProp::AbstractState::factory(backend, fluid);
  CoolProp::AbstractState* vapor = CoolProp::AbstractState::factory(backend, fluid);

  // Set the inputs
  liquid->update(CoolProp::PT_INPUTS, P, T);
  vapor->update(CoolProp::PQ_INPUTS, P, 1); // 1 => 100% vapor

  // Calculate properties for liquid phase at P and T
  double rho_liq = liquid->rhomass(),
      drhodP_liq = liquid->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT),
      drhodT_liq = liquid->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP),
      h_liq = liquid->hmass(),
      dhdp_liq = liquid->first_partial_deriv(CoolProp::iHmass, CoolProp::iP, CoolProp::iT),
      dhdT_liq = liquid->first_partial_deriv(CoolProp::iHmass, CoolProp::iT, CoolProp::iP),
      mu_liq = liquid->viscosity(),
      lambda_liq = liquid->conductivity(),
      beta_liq = liquid->isobaric_expansion_coefficient(),
      cp_liq = liquid->cpmass();

  // Calculate properties for vapor phase at P and T
  double temp = vapor->T(),
      rho_vap = vapor->rhomass(),
      drhodP_vap = vapor->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT),
      drhodT_vap = vapor->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP),
      h_vap = vapor->hmass(),
      dhdp_vap = vapor->first_partial_deriv(CoolProp::iHmass, CoolProp::iP, CoolProp::iT),
      dhdT_vap = vapor->first_partial_deriv(CoolProp::iHmass, CoolProp::iT, CoolProp::iP),
      mu_vap = vapor->viscosity(),
      lambda_vap = vapor->conductivity(),
      beta_vap = vapor->isobaric_expansion_coefficient(),
      cp_vap = vapor->cpmass();

  // Print results
  std::cout << "Liquid phase properties at P = " << P << " Pa and T = " << T << std::endl;
  std::cout << "Density: " << rho_liq << std::endl;
  std::cout << "Partial derivative of density with respect to pressure: " << drhodP_liq << std::endl;
  std::cout << "Partial derivative of density with respect to temperature: " << drhodT_liq << std::endl;
  std::cout << "Enthalpy: " << h_liq << std::endl;
  std::cout << "d Enthalpy P: " << dhdp_liq << std::endl;
  std::cout << "d Enthalpy T: " << dhdT_liq << std::endl;
  std::cout << "Mu: " << mu_liq << std::endl;
  std::cout << "Lambda: " << lambda_liq << std::endl;
  std::cout << "Beta: " << beta_liq << std::endl;
  std::cout << "Cp: " << cp_liq << std::endl;
  std::cout << std::endl;

  // Print results
  std::cout << "Vapor phase properties at P = " << P << std::endl;
  std::cout << "temperature: " << temp << std::endl;
  std::cout << "Density: " << rho_vap << std::endl;
  std::cout << "Partial derivative of density with respect to pressure: " << drhodP_vap << std::endl;
  std::cout << "Partial derivative of density with respect to temperature: " << drhodT_vap << std::endl;
  std::cout << "Enthalpy: " << h_vap << std::endl;
  std::cout << "d Enthalpy P: " << dhdp_vap << std::endl;
  std::cout << "d Enthalpy T: " << dhdT_vap << std::endl;
  std::cout << "Mu: " << mu_vap << std::endl;
  std::cout << "Lambda: " << lambda_vap << std::endl;
  std::cout << "Beta: " << beta_vap << std::endl;
  std::cout << "Cp: " << cp_vap << std::endl;
  std::cout << std::endl;

  delete liquid;
  delete vapor;

  return 1;
}
