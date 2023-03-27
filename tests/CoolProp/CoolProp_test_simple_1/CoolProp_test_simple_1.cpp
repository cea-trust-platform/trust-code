#include "CoolProp.h"
#include <iostream>
#include <string>
#include <array>

int main(int argc, char const *argv[])
{
  std::array<std::string, 17> fluids =
  { { "Air", "Ammonia", "Carbon dioxide", "Carbon monoxide",
    "Deuterium", "Ethane", "Ethanol", "Helium", "Hydrogen",
    "Isobutane", "Methane", "Neon", "Nitrogen",
    "Oxygen", "Propane", "Propylene", "Water" } };

  for (auto& itr : fluids)
    {
      const std::string& str_ = itr;

      std::cout << "=========================================================" << std::endl;
      std::cout << "Test simple call to CoolProp with fluid : " << str_  << std::endl;
      std::cout << "=========================================================" << std::endl;
      std::cout << CoolProp::PropsSI("T", "P", 101325, "Q", 0, str_) << std::endl;
      std::cout << CoolProp::PropsSI("T", "P", 101325, "Q", 1, str_) << std::endl;

      // Get the density of Water at T = 461.1 K and P = 5.0e6 Pa, imposing the liquid phase
      std::cout << CoolProp::PropsSI("D", "T", 461.1, "P", 5e6, str_) << std::endl;
      std::cout << CoolProp::PropsSI("D", "T|liquid", 461.1, "P", 5e6, str_) << std::endl;
      std::cout << CoolProp::PropsSI("D", "T|gas", 461.1, "P", 5e6, str_) << std::endl;

      // Get the density of Water at T = 597.9 K and P = 5.0e6 Pa, imposing the gas phase
      std::cout << CoolProp::PropsSI("D", "T", 597.9, "P", 5e6, str_) << std::endl;
      std::cout << CoolProp::PropsSI("D", "T", 597.9, "P|gas", 5e6, str_) << std::endl;
      std::cout << CoolProp::PropsSI("D", "T", 597.9, "P|liquid", 5e6, str_) << std::endl;

      std::cout << std::endl;
    }

  return 1;
}
