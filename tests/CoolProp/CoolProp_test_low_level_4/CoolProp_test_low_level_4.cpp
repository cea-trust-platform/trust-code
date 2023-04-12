#include "AbstractState.h"
#include "CoolProp.h"
#include <iostream>
#include <string>
#include <array>

int main(int argc, char const *argv[])
{
  std::string fluid = "Water";
  std::string backend = "HEOS";

  std::cout << "==================================================================================================================================" << std::endl;
  std::cout << "Test simple calls to CoolProp using the low level interface with imposed phases for fluid and output special values: " << fluid << std::endl;
  std::cout << "==================================================================================================================================" << std::endl;
  std::cout << std::endl;

  shared_ptr<CoolProp::AbstractState> Water(CoolProp::AbstractState::factory(backend, fluid));

  Water->specify_phase(CoolProp::iphase_gas);
  Water->update(CoolProp::PT_INPUTS, 101325, 373.124);  // SI units
  std::cout << "Impose phase CoolProp::iphase_gas " << std::endl;
  std::cout << "P max: " << Water->pmax() << std::endl;
  std::cout << "P min: " << Water->p_triple() << std::endl;
  std::cout << "P critical: " << Water->p_critical() << std::endl;
  std::cout << std::endl;
  std::cout << "T max: " << Water->Tmax() << std::endl;
  std::cout << "T min: " << Water->Tmin() << std::endl;
  std::cout << "T critical: " << Water->T_critical() << std::endl;
  std::cout << "T reducing: " << Water->T_reducing() << std::endl;
  std::cout << std::endl;

  Water->specify_phase(CoolProp::iphase_liquid);
  Water->update(CoolProp::PT_INPUTS, 101325, 373.124);  // SI units
  std::cout << "Impose phase CoolProp::iphase_liquid " << std::endl;
  std::cout << "P max: " << Water->pmax() << std::endl;
  std::cout << "P min: " << Water->p_triple() << std::endl;
  std::cout << "P critical: " << Water->p_critical() << std::endl;
  std::cout << std::endl;
  std::cout << "T max: " << Water->Tmax() << std::endl;
  std::cout << "T min: " << Water->Tmin() << std::endl;
  std::cout << "T critical: " << Water->T_critical() << std::endl;
  std::cout << "T reducing: " << Water->T_reducing() << std::endl;
  std::cout << std::endl;

  Water->unspecify_phase();

  return 1;
}
