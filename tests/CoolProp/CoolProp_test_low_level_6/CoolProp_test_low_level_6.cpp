#include "CoolPropTools.h"
#include "AbstractState.h"
#include "CoolPropLib.h"
#include "CoolProp.h"
#include <iostream>
#include <span.hpp>
#include <string>
#include <time.h>
#include <array>

int main(int argc, char const *argv[])
{
  std::string fluid = "Water";
  std::string backend = "HEOS";

  std::cout << "===============================================================================================" << std::endl;
  std::cout << "Test simple calls to saturation CoolProp using the low level interface for fluid : " << fluid << std::endl;
  std::cout << "===============================================================================================" << std::endl;
  std::cout << std::endl;

  shared_ptr<CoolProp::AbstractState> Water(CoolProp::AbstractState::factory(backend, fluid));

  Water->specify_phase(CoolProp::iphase_gas);
  std::cout << "Impose phase CoolProp::iphase_gas " << std::endl;
  std::cout << std::endl;

  Water->update(CoolProp::QT_INPUTS, 1, 300);  // SI units
  std::cout << "pl sat: " << Water->saturated_liquid_keyed_output(CoolProp::iP) << std::endl;
  std::cout << "pv sat: " << Water->saturated_vapor_keyed_output(CoolProp::iP) << std::endl;
  std::cout << "pldT sat: " << Water->first_saturation_deriv(CoolProp::iP, CoolProp::iT) << std::endl;
  std::cout << "pvdT sat: " << Water->first_saturation_deriv(CoolProp::iP, CoolProp::iT) << std::endl;
  std::cout << std::endl;

  Water->update(CoolProp::PQ_INPUTS, 101325, 1);  // SI units
  std::cout << "Tl sat: " << Water->saturated_liquid_keyed_output(CoolProp::iT) << std::endl;
  std::cout << "Tv sat: " << Water->saturated_vapor_keyed_output(CoolProp::iT) << std::endl;
  std::cout << "Tldp sat: " << Water->first_saturation_deriv(CoolProp::iT, CoolProp::iP) << std::endl;
  std::cout << "Tvdp sat: " << Water->first_saturation_deriv(CoolProp::iT, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "hl sat: " << Water->saturated_liquid_keyed_output(CoolProp::iHmass) << std::endl;
  std::cout << "hv sat: " << Water->saturated_vapor_keyed_output(CoolProp::iHmass) << std::endl;
  std::cout << "hldp sat: " << Water->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP) << std::endl;
  std::cout << "hvdp sat: " << Water->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "rhol sat: " << Water->saturated_liquid_keyed_output(CoolProp::iDmass) << std::endl;
  std::cout << "rhov sat: " << Water->saturated_vapor_keyed_output(CoolProp::iDmass) << std::endl;
  std::cout << "rholdp sat: " << Water->first_saturation_deriv(CoolProp::iDmass, CoolProp::iP) << std::endl;
  std::cout << "rhovdp sat: " << Water->first_saturation_deriv(CoolProp::iDmass, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "cpl sat: " << Water->saturated_liquid_keyed_output(CoolProp::iCpmass) << std::endl;
  std::cout << "cpv sat: " << Water->saturated_vapor_keyed_output(CoolProp::iCpmass) << std::endl;
  std::cout << "cpldp sat: " << Water->first_saturation_deriv(CoolProp::iCpmass, CoolProp::iP) << std::endl;
  std::cout << "cpvdp sat: " << Water->first_saturation_deriv(CoolProp::iCpmass, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "lambdal sat: " << Water->saturated_liquid_keyed_output(CoolProp::iconductivity) << std::endl;
  std::cout << "lambdav sat: " << Water->saturated_vapor_keyed_output(CoolProp::iconductivity) << std::endl;
//  std::cout << "lambdaldp sat: " << Water->first_saturation_deriv(CoolProp::iconductivity, CoolProp::iP) << std::endl;
//  std::cout << "lambdavdp sat: " << Water->first_saturation_deriv(CoolProp::iconductivity, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "mul sat: " << Water->saturated_liquid_keyed_output(CoolProp::iviscosity) << std::endl;
  std::cout << "muv sat: " << Water->saturated_vapor_keyed_output(CoolProp::iviscosity) << std::endl;
//  std::cout << "muldp sat: " << Water->first_saturation_deriv(CoolProp::iviscosity, CoolProp::iP) << std::endl;
//  std::cout << "muvdp sat: " << Water->first_saturation_deriv(CoolProp::iviscosity, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "Sigma: " << Water->surface_tension() << std::endl;
  std::cout << std::endl;

// -------------------------------------------------------------------------------
  Water->specify_phase(CoolProp::iphase_liquid);
  std::cout << "Impose phase CoolProp::iphase_liquid " << std::endl;
  std::cout << std::endl;

  Water->update(CoolProp::QT_INPUTS, 1, 300);  // SI units
  std::cout << "pl sat: " << Water->saturated_liquid_keyed_output(CoolProp::iP) << std::endl;
  std::cout << "pv sat: " << Water->saturated_vapor_keyed_output(CoolProp::iP) << std::endl;
  std::cout << "pldT sat: " << Water->first_saturation_deriv(CoolProp::iP, CoolProp::iT) << std::endl;
  std::cout << "pvdT sat: " << Water->first_saturation_deriv(CoolProp::iP, CoolProp::iT) << std::endl;
  std::cout << std::endl;

  Water->update(CoolProp::PQ_INPUTS, 101325, 0);  // SI units
  std::cout << "Tl sat: " << Water->saturated_liquid_keyed_output(CoolProp::iT) << std::endl;
  std::cout << "Tv sat: " << Water->saturated_vapor_keyed_output(CoolProp::iT) << std::endl;
  std::cout << "Tldp sat: " << Water->first_saturation_deriv(CoolProp::iT, CoolProp::iP) << std::endl;
  std::cout << "Tvdp sat: " << Water->first_saturation_deriv(CoolProp::iT, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "hl sat: " << Water->saturated_liquid_keyed_output(CoolProp::iHmass) << std::endl;
  std::cout << "hv sat: " << Water->saturated_vapor_keyed_output(CoolProp::iHmass) << std::endl;
  std::cout << "hldp sat: " << Water->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP) << std::endl;
  std::cout << "hvdp sat: " << Water->first_saturation_deriv(CoolProp::iHmass, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "rhol sat: " << Water->saturated_liquid_keyed_output(CoolProp::iDmass) << std::endl;
  std::cout << "rhov sat: " << Water->saturated_vapor_keyed_output(CoolProp::iDmass) << std::endl;
  std::cout << "rholdp sat: " << Water->first_saturation_deriv(CoolProp::iDmass, CoolProp::iP) << std::endl;
  std::cout << "rhovdp sat: " << Water->first_saturation_deriv(CoolProp::iDmass, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "cpl sat: " << Water->saturated_liquid_keyed_output(CoolProp::iCpmass) << std::endl;
  std::cout << "cpv sat: " << Water->saturated_vapor_keyed_output(CoolProp::iCpmass) << std::endl;
  std::cout << "cpldp sat: " << Water->first_saturation_deriv(CoolProp::iCpmass, CoolProp::iP) << std::endl;
  std::cout << "cpvdp sat: " << Water->first_saturation_deriv(CoolProp::iCpmass, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "lambdal sat: " << Water->saturated_liquid_keyed_output(CoolProp::iconductivity) << std::endl;
  std::cout << "lambdav sat: " << Water->saturated_vapor_keyed_output(CoolProp::iconductivity) << std::endl;
//  std::cout << "lambdaldp sat: " << Water->first_saturation_deriv(CoolProp::iconductivity, CoolProp::iP) << std::endl;
//  std::cout << "lambdavdp sat: " << Water->first_saturation_deriv(CoolProp::iconductivity, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "mul sat: " << Water->saturated_liquid_keyed_output(CoolProp::iviscosity) << std::endl;
  std::cout << "muv sat: " << Water->saturated_vapor_keyed_output(CoolProp::iviscosity) << std::endl;
//  std::cout << "muldp sat: " << Water->first_saturation_deriv(CoolProp::iviscosity, CoolProp::iP) << std::endl;
//  std::cout << "muvdp sat: " << Water->first_saturation_deriv(CoolProp::iviscosity, CoolProp::iP) << std::endl;
  std::cout << std::endl;
  std::cout << "Sigma: " << Water->surface_tension() << std::endl;
  std::cout << std::endl;

  return 1;
}


