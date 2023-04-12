#include "AbstractState.h"
#include "CoolProp.h"
#include <iostream>
#include <string>
#include <array>

int main(int argc, char const *argv[])
{
  std::string fluid = "Water";
  std::string backend = "HEOS";

  std::cout << "==========================================================================================================" << std::endl;
  std::cout << "Test simple calls to CoolProp using the low level interface with imposed phases for fluid : " << fluid << std::endl;
  std::cout << "==========================================================================================================" << std::endl;
  std::cout << std::endl;

  shared_ptr<CoolProp::AbstractState> Water(CoolProp::AbstractState::factory(backend, fluid));

  Water->specify_phase(CoolProp::iphase_gas);
  Water->update(CoolProp::PT_INPUTS, 101325, 373.124);  // SI units
  std::cout << "Impose phase CoolProp::iphase_gas " << std::endl;
  std::cout << "rho': " << Water->rhomass() << " kg/m^3" << std::endl;
  std::cout << "rho': " << Water->rhomolar() << " mol/m^3" << std::endl;
  std::cout << "h': " << Water->hmass() << " J/kg" << std::endl;
  std::cout << "h': " << Water->hmolar() << " J/mol" << std::endl;
  std::cout << "s': " << Water->smass() << " J/kg/K" << std::endl;
  std::cout << "s': " << Water->smolar() << " J/mol/K" << std::endl;
  std::cout << std::endl;

  Water->specify_phase(CoolProp::iphase_liquid);
  Water->update(CoolProp::PT_INPUTS, 101325, 373.124);  // SI units
  std::cout << "Impose phase CoolProp::iphase_liquid " << std::endl;
  std::cout << "rho': " << Water->rhomass() << " kg/m^3" << std::endl;
  std::cout << "rho': " << Water->rhomolar() << " mol/m^3" << std::endl;
  std::cout << "h': " << Water->hmass() << " J/kg" << std::endl;
  std::cout << "h': " << Water->hmolar() << " J/mol" << std::endl;
  std::cout << "s': " << Water->smass() << " J/kg/K" << std::endl;
  std::cout << "s': " << Water->smolar() << " J/mol/K" << std::endl;
  std::cout << std::endl;

  Water->specify_phase(CoolProp::iphase_supercritical_liquid);
  Water->update(CoolProp::PT_INPUTS, 101325, 373.124);  // SI units
  std::cout << "Impose phase CoolProp::iphase_supercritical_liquid " << std::endl;
  std::cout << "rho': " << Water->rhomass() << " kg/m^3" << std::endl;
  std::cout << "rho': " << Water->rhomolar() << " mol/m^3" << std::endl;
  std::cout << "h': " << Water->hmass() << " J/kg" << std::endl;
  std::cout << "h': " << Water->hmolar() << " J/mol" << std::endl;
  std::cout << "s': " << Water->smass() << " J/kg/K" << std::endl;
  std::cout << "s': " << Water->smolar() << " J/mol/K" << std::endl;
  std::cout << std::endl;

  Water->specify_phase(CoolProp::iphase_supercritical_gas);
  Water->update(CoolProp::PT_INPUTS, 101325, 373.124);  // SI units
  std::cout << "Impose phase CoolProp::iphase_supercritical_gas " << std::endl;
  std::cout << "rho': " << Water->rhomass() << " kg/m^3" << std::endl;
  std::cout << "rho': " << Water->rhomolar() << " mol/m^3" << std::endl;
  std::cout << "h': " << Water->hmass() << " J/kg" << std::endl;
  std::cout << "h': " << Water->hmolar() << " J/mol" << std::endl;
  std::cout << "s': " << Water->smass() << " J/kg/K" << std::endl;
  std::cout << "s': " << Water->smolar() << " J/mol/K" << std::endl;
  std::cout << std::endl;

  Water->specify_phase(CoolProp::iphase_supercritical);
  Water->update(CoolProp::PT_INPUTS, 101325, 373.124);  // SI units
  std::cout << "Impose phase CoolProp::iphase_supercritical " << std::endl;
  std::cout << "rho': " << Water->rhomass() << " kg/m^3" << std::endl;
  std::cout << "rho': " << Water->rhomolar() << " mol/m^3" << std::endl;
  std::cout << "h': " << Water->hmass() << " J/kg" << std::endl;
  std::cout << "h': " << Water->hmolar() << " J/mol" << std::endl;
  std::cout << "s': " << Water->smass() << " J/kg/K" << std::endl;
  std::cout << "s': " << Water->smolar() << " J/mol/K" << std::endl;
  std::cout << std::endl;

  Water->specify_phase(CoolProp::iphase_not_imposed);
  Water->update(CoolProp::PT_INPUTS, 101325, 373.124);  // SI units
  std::cout << "Impose phase CoolProp::iphase_not_imposed " << std::endl;
  std::cout << "rho': " << Water->rhomass() << " kg/m^3" << std::endl;
  std::cout << "rho': " << Water->rhomolar() << " mol/m^3" << std::endl;
  std::cout << "h': " << Water->hmass() << " J/kg" << std::endl;
  std::cout << "h': " << Water->hmolar() << " J/mol" << std::endl;
  std::cout << "s': " << Water->smass() << " J/kg/K" << std::endl;
  std::cout << "s': " << Water->smolar() << " J/mol/K" << std::endl;
  std::cout << std::endl;

  Water->unspecify_phase();

  return 1;
}
