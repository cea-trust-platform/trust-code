#define SWIG
#include <Configuration.h>
#undef SWIG

#include "CoolPropTools.h"
#include "AbstractState.h"
#include "CoolPropLib.h"
#include "CoolProp.h"
#include <iostream>
#include <string>
#include <time.h>
#include <array>
#include <span.hpp>

int main(int argc, char const *argv[])
{
  const char *use_refprop = std::getenv("TRUST_USE_REFPROP");

  if (use_refprop != nullptr && std::string(use_refprop) == "1")
    {
      const char * refprop_dir = std::getenv("TRUST_REFPROP_HOME_DIR");
      CoolProp::set_config_string(configuration_keys::ALTERNATIVE_REFPROP_PATH, std::string(refprop_dir));
    }

  std::array<std::string, 1> fluids = { { "Water" } };
  std::array<std::string, 3> backends = { { "REFPROP", "HEOS" , "BICUBIC&HEOS" } };

  for (const auto& itr_fl : fluids )
    for (const auto& itr_bk : backends )
      {
        std::cout << "==================================================================================================================================" << std::endl;
        std::cout << "Test simple calls to CoolProp using the low level interface with fluid : " << itr_fl << " and backend : " << itr_bk << std::endl;
        std::cout << "==================================================================================================================================" << std::endl;
        std::cout << std::endl;

        CoolProp::AbstractState *Water = CoolProp::AbstractState::factory(itr_bk, itr_fl);

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
//        std::cout << "T reducing: " << Water->T_reducing() << std::endl;
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
//        std::cout << "T reducing: " << Water->T_reducing() << std::endl;
        std::cout << std::endl;

        Water->unspecify_phase();

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

        double P1 = 1e5, P2 = 2e5, T1 = 300, T2 = 400;

        // Set point 1
        Water->specify_phase(CoolProp::iphase_liquid);
        Water->update(CoolProp::PT_INPUTS, P1, T1);

        // Calculate properties at point 1
        double rho1 = Water->rhomass(),
            drhodP1 = Water->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT),
            drhodT1 = Water->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP),
            h1 = Water->hmass(),
            dhdp1 = Water->first_partial_deriv(CoolProp::iHmolar, CoolProp::iP, CoolProp::iT),
            dhdT1 = Water->first_partial_deriv(CoolProp::iHmolar, CoolProp::iT, CoolProp::iP),
            mu1 = Water->viscosity(),
            lambda1 = Water->conductivity(),
            beta1 = Water->isobaric_expansion_coefficient(),
            cp1 = Water->cpmass();

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
        Water->update(CoolProp::PT_INPUTS, P2, T2);

        // Calculate properties at point 2
        double rho2 = Water->rhomass(),
            drhodP2 = Water->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT),
            drhodT2 = Water->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP),
            h2 = Water->hmass(),
            dhdp2 = Water->first_partial_deriv(CoolProp::iHmolar, CoolProp::iP, CoolProp::iT),
            dhdT2 = Water->first_partial_deriv(CoolProp::iHmolar, CoolProp::iT, CoolProp::iP),
            mu2 = Water->viscosity(),
            lambda2 = Water->conductivity(),
            beta2 = Water->isobaric_expansion_coefficient(),
            cp2 = Water->cpmass();

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

        delete Water;
      }
  return 1;
}


