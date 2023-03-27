#include "CoolProp.h"
#include <iostream>
#include <string>
#include <array>

int main(int argc, char const *argv[])
{
  std::string fluid = "Water";

  std::cout << "======================================================================================" << std::endl;
  std::cout << "Test simple call to CoolProp to compute properties/derivatives with fluid : " << fluid  << std::endl;
  std::cout << "======================================================================================" << std::endl;
  std::cout << std::endl;

  // Set the working pressure and temperature
  double pressure = 101325;  // Pa
  double temperature = 300;  // K

  // Compute all thermodynamic properties and their derivatives using PropsSI
  double density, d_density_dp, d_density_dT;
  double enthalpy, d_enthalpy_dp, d_enthalpy_dT;
  double internal_energy, d_internal_energy_dp, d_internal_energy_dT;
  double isobaric_heat_capacity, d_isobaric_heat_capacity_dp, d_isobaric_heat_capacity_dT;

  density = CoolProp::PropsSI("D", "P", pressure, "T", temperature, fluid);
  d_density_dp = CoolProp::PropsSI("d(D)/d(P)|T", "P", pressure, "T", temperature, fluid) / pressure;
  d_density_dT = CoolProp::PropsSI("d(D)/d(T)|P", "P", pressure, "T", temperature, fluid);

  enthalpy = CoolProp::PropsSI("H", "P", pressure, "T", temperature, fluid);
  d_enthalpy_dp = CoolProp::PropsSI("d(H)/d(P)|T", "P", pressure, "T", temperature, fluid);
  d_enthalpy_dT = CoolProp::PropsSI("d(H)/d(T)|P", "P", pressure, "T", temperature, fluid);

  internal_energy = CoolProp::PropsSI("U", "P", pressure, "T", temperature, fluid);
  d_internal_energy_dp = CoolProp::PropsSI("d(U)/d(P)|T", "P", pressure, "T", temperature, fluid);
  d_internal_energy_dT = CoolProp::PropsSI("d(U)/d(T)|P", "P", pressure, "T", temperature, fluid);

  isobaric_heat_capacity = CoolProp::PropsSI("Cp", "P", pressure, "T", temperature, fluid);
  d_isobaric_heat_capacity_dp = CoolProp::PropsSI("d(Cp)/d(P)|T", "P", pressure, "T", temperature, fluid);
  d_isobaric_heat_capacity_dT = CoolProp::PropsSI("d(Cp)/d(T)|P", "P", pressure, "T", temperature, fluid);

  // Print the computed properties and their derivatives
  std::cout << "density: " << density << std::endl;
  std::cout << "d(density)/dP: " << d_density_dp << std::endl;
  std::cout << "d(density)/dT: " << d_density_dT << std::endl;

  std::cout << std::endl;

  std::cout << "enthalpy: " << enthalpy << std::endl;
  std::cout << "d(enthalpy)/dP: " <<   d_enthalpy_dp << std::endl;
  std::cout << "d(enthalpy)/dT: " <<   d_enthalpy_dT << std::endl;

  std::cout << std::endl;

  std::cout << "internal_energy: " << internal_energy << std::endl;
  std::cout << "d(internal_energy)/dP: " << d_internal_energy_dp << std::endl;
  std::cout << "d(internal_energy)/dT: " << d_internal_energy_dT << std::endl;

  std::cout << std::endl;

  std::cout << "isobaric_heat_capacity: " << isobaric_heat_capacity << std::endl;
  std::cout << "d(isobaric_heat_capacity)/dP: " << d_isobaric_heat_capacity_dp << std::endl;
  std::cout << "d(isobaric_heat_capacity)/dT: " << d_isobaric_heat_capacity_dT << std::endl;

  std::cout << std::endl;

  // Compute viscosity and thermal conductivity using PropsSI
  double viscosity = CoolProp::PropsSI("V", "P", pressure, "T", temperature, fluid);
  double thermal_conductivity = CoolProp::PropsSI("L", "P", pressure, "T", temperature, fluid);

  std::cout << "Viscosity: " << viscosity << std::endl;
  std::cout << "Thermal conductivity: " << thermal_conductivity << std::endl;


  return 1;
}
