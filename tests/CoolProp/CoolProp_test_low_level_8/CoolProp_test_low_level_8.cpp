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

#include "EOS/API/EOS.hxx"
#include "EOS/API/EOS_Fields.hxx"
#include "EOS/API/EOS_Error_Field.hxx"
#include "EOS/API/EOS_Std_Error_Handler.hxx"
#include <TPPI_tools.h>

using namespace NEPTUNE;

enum class params { MU, LAMBDA, SIGMA , T, P, HL, HV, RHOL, RHOV, CPL, CPV };

static constexpr double eps = 1.e-6;

static int test_counter = 0;

double FD_derivative_ph(const params prm, const double P, const double H, CoolProp::AbstractState * fluide_cool, bool wrt_p = true)
{
  double der = 1e30, plus_ = eps, minus_ = eps;

  wrt_p ? fluide_cool->update(CoolProp::HmassP_INPUTS, H, P * (1. + eps)) : fluide_cool->update(CoolProp::HmassP_INPUTS, H * (1. + eps), P);
  if (prm == params::MU) plus_ = fluide_cool->viscosity();
  if (prm == params::LAMBDA)  plus_ = fluide_cool->conductivity();
  if (prm == params::SIGMA) plus_ = fluide_cool->surface_tension();

  wrt_p ? fluide_cool->update(CoolProp::HmassP_INPUTS, H, P * (1. - eps)) : fluide_cool->update(CoolProp::HmassP_INPUTS, H * (1. - eps), P);
  if (prm == params::MU) minus_ = fluide_cool->viscosity();
  if (prm == params::LAMBDA)  minus_ = fluide_cool->conductivity();
  if (prm == params::SIGMA) minus_ = fluide_cool->surface_tension();

  der = wrt_p ? ((plus_ - minus_) / ( 2 * eps * P)) : ((plus_ - minus_) / ( 2 * eps * H));

  return der;
}

double FD_derivative_pT(const params prm, const double P, const double T, CoolProp::AbstractState * fluide_cool, bool wrt_p = true)
{
  double der = 1e30, plus_ = eps, minus_ = eps;

  wrt_p ? fluide_cool->update(CoolProp::PT_INPUTS, P * (1. + eps), T) : fluide_cool->update(CoolProp::PT_INPUTS, P, T * (1. + eps));
  if (prm == params::MU) plus_ = fluide_cool->viscosity();
  if (prm == params::LAMBDA)  plus_ = fluide_cool->conductivity();
  if (prm == params::SIGMA) plus_ = fluide_cool->surface_tension();

  wrt_p ? fluide_cool->update(CoolProp::PT_INPUTS, P * (1. - eps), T) : fluide_cool->update(CoolProp::PT_INPUTS, P, T * (1. - eps));
  if (prm == params::MU) minus_ = fluide_cool->viscosity();
  if (prm == params::LAMBDA)  minus_ = fluide_cool->conductivity();
  if (prm == params::SIGMA) minus_ = fluide_cool->surface_tension();

  der = wrt_p ? ((plus_ - minus_) / ( 2 * eps * P)) : ((plus_ - minus_) / ( 2 * eps * T));

  return der;
}

double FD_derivative_p(const params prm, const double P, CoolProp::AbstractState * fluide_cool, bool is_liq = true)
{
  double der = 1e30, plus_ = eps, minus_ = eps;

  if (prm == params::P)
    {
      fluide_cool->update(CoolProp::QT_INPUTS,0,  P /* this is T */ * (1. + eps));
      plus_ = fluide_cool->p();

      fluide_cool->update(CoolProp::QT_INPUTS,0, P /* this is T */ * (1. - eps));
      minus_ = fluide_cool->p();

      der = (plus_ - minus_) / ( 2 * eps * P);
      return der;
    }

  is_liq ? fluide_cool->update(CoolProp::PQ_INPUTS, P * (1. + eps), 0) : fluide_cool->update(CoolProp::PQ_INPUTS, P * (1. + eps), 1);
  if (prm == params::T) plus_ = fluide_cool->T();
  if (prm == params::HL || prm == params::HV) plus_ = fluide_cool->hmass();
  if (prm == params::RHOL || prm == params::RHOV) plus_ = fluide_cool->rhomass();
  if (prm == params::CPL || prm == params::CPV) plus_ = fluide_cool->cpmass();

  is_liq ? fluide_cool->update(CoolProp::PQ_INPUTS, P * (1. - eps), 0) : fluide_cool->update(CoolProp::PQ_INPUTS, P * (1. - eps), 1);
  if (prm == params::T) minus_ = fluide_cool->T();
  if (prm == params::HL || prm == params::HV) minus_ = fluide_cool->hmass();
  if (prm == params::RHOL || prm == params::RHOV) minus_ = fluide_cool->rhomass();
  if (prm == params::CPL || prm == params::CPV) minus_ = fluide_cool->cpmass();

  der = (plus_ - minus_) / ( 2 * eps * P);

  return der;
}

void test_fluids_ph(const std::string fld,const double P, const double H, EOS& fluide_eos, CoolProp::AbstractState * fluide_cool)
{
  test_counter++;

  std::cout << std::endl;
  std::cout << "*************** TEST : " << test_counter << std::endl;
  std::cout << std::endl;
  std::cout << "    TEST EN ENTHALPIE" << std::endl;
  std::cout << "    FLUID : " << fld << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;

  double double_eos_ = -1000. , double_cool_ = -1000.;

  // rho
  fluide_eos.compute_rho_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->rhomass();
  std::cout << "  - Rho EOS = " << double_eos_ << " , Rho CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_rho_d_p_h_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iHmass);
  std::cout << "  - RhoDP EOS = " << double_eos_ << " , RhoDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_rho_d_h_p_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iDmass,  CoolProp::iHmass, CoolProp::iP);
  std::cout << "  - RhoDH EOS = " << double_eos_ << " , RhoDH CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // T
  fluide_eos.compute_T_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->T();
  std::cout << "  - T EOS = " << double_eos_ << " , T CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_T_d_p_h_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iT, CoolProp::iP, CoolProp::iHmass);
  std::cout << "  - TDP EOS = " << double_eos_ << " , TDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_T_d_h_p_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iT,  CoolProp::iHmass, CoolProp::iP);
  std::cout << "  - TDH EOS = " << double_eos_ << " , TDH CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // cp
  fluide_eos.compute_cp_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->cpmass();
  std::cout << "  - Cp EOS = " << double_eos_ << " , Cp CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_cp_d_p_h_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iCpmass, CoolProp::iP, CoolProp::iHmass);
  std::cout << "  - CpDP EOS = " << double_eos_ << " , CpDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_cp_d_h_p_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iCpmass,  CoolProp::iHmass, CoolProp::iP);
  std::cout << "  - CpDH EOS = " << double_eos_ << " , CpDH CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // mu
  fluide_eos.compute_mu_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->viscosity();
  std::cout << "  - Mu EOS = " << double_eos_ << " , Mu CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_mu_d_p_h_ph(P,H,double_eos_);
  double_cool_ = FD_derivative_ph(params::MU, P, H, fluide_cool); //fluide_cool->first_partial_deriv(CoolProp::iviscosity, CoolProp::iP, CoolProp::iHmass);
  std::cout << "  - MuDP EOS = " << double_eos_ << " , MuDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_mu_d_h_p_ph(P,H,double_eos_);
  double_cool_ = FD_derivative_ph(params::MU, P, H, fluide_cool, false); //fluide_cool->first_partial_deriv(CoolProp::iviscosity,  CoolProp::iHmass, CoolProp::iP);
  std::cout << "  - MuDH EOS = " << double_eos_ << " , MuDH CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // lambda
  fluide_eos.compute_lambda_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->conductivity();
  std::cout << "  - Lambda EOS = " << double_eos_ << " , Lambda CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_lambda_d_p_h_ph(P,H,double_eos_);
  double_cool_ = FD_derivative_ph(params::LAMBDA, P, H, fluide_cool); //fluide_cool->first_partial_deriv(CoolProp::iconductivity, CoolProp::iP, CoolProp::iHmass);
  std::cout << "  - LambdaDP EOS = " << double_eos_ << " , LambdaDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_lambda_d_h_p_ph(P,H,double_eos_);
  double_cool_ = FD_derivative_ph(params::LAMBDA, P, H, fluide_cool, false); //fluide_cool->first_partial_deriv(CoolProp::iconductivity,  CoolProp::iHmass, CoolProp::iP);
  std::cout << "  - LambdaDH EOS = " << double_eos_ << " , LambdaDH CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // sigma
  fluide_eos.compute_sigma_ph(P,H,double_eos_);
  double_cool_ = fluide_cool->surface_tension();
  std::cout << "  - Sigma EOS = " << double_eos_ << " , Sigma CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_sigma_d_p_h_ph(P,H,double_eos_);
  double_cool_ = FD_derivative_ph(params::SIGMA, P, H, fluide_cool); //fluide_cool->first_partial_deriv(CoolProp::isurface_tension, CoolProp::iP, CoolProp::iHmass);
  std::cout << "  - SigmaDP EOS = " << double_eos_ << " , SigmaDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_sigma_d_h_p_ph(P,H,double_eos_);
  double_cool_ = FD_derivative_ph(params::SIGMA, P, H, fluide_cool, false); //fluide_cool->first_partial_deriv(CoolProp::isurface_tension,  CoolProp::iHmass, CoolProp::iP);
  std::cout << "  - SigmaDH EOS = " << double_eos_ << " , SigmaDH CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;
}

void test_fluids_pt(const std::string fld,const double P, const double T, EOS& fluide_eos, CoolProp::AbstractState * fluide_cool)
{
  test_counter++;

  std::cout << std::endl;
  std::cout << "*************** TEST : " << test_counter << std::endl;
  std::cout << std::endl;
  std::cout << "    TEST EN TEMPERATURE" << std::endl;
  std::cout << "    FLUID : " << fld << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;

  double double_eos_ = -1000. , double_cool_ = -1000.;

  // rho
  fluide_eos.compute_rho_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->rhomass();
  std::cout << "  - Rho EOS = " << double_eos_ << " , Rho CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_rho_d_p_T_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT);
  std::cout << "  - RhoDP EOS = " << double_eos_ << " , RhoDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_rho_d_T_p_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iDmass,  CoolProp::iT, CoolProp::iP);
  std::cout << "  - RhoDT EOS = " << double_eos_ << " , RhoDT CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // H
  fluide_eos.compute_h_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->hmass();
  std::cout << "  - H EOS = " << double_eos_ << " , H CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_h_d_p_T_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iHmass, CoolProp::iP, CoolProp::iT);
  std::cout << "  - HDP EOS = " << double_eos_ << " , HDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_h_d_T_p_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iHmass,  CoolProp::iT, CoolProp::iP);
  std::cout << "  - HDT EOS = " << double_eos_ << " , HDT CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // cp
  fluide_eos.compute_cp_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->cpmass();
  std::cout << "  - Cp EOS = " << double_eos_ << " , Cp CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_cp_d_p_T_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iCpmass, CoolProp::iP, CoolProp::iT);
  std::cout << "  - CpDP EOS = " << double_eos_ << " , CpDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_cp_d_T_p_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->first_partial_deriv(CoolProp::iCpmass,  CoolProp::iT, CoolProp::iP);
  std::cout << "  - CpDT EOS = " << double_eos_ << " , CpDT CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // mu
  fluide_eos.compute_mu_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->viscosity();
  std::cout << "  - Mu EOS = " << double_eos_ << " , Mu CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_mu_d_p_T_pT(P,T,double_eos_);
  double_cool_ = FD_derivative_pT(params::MU, P, T, fluide_cool); //fluide_cool->first_partial_deriv(CoolProp::iviscosity, CoolProp::iP, CoolProp::iHmass);
  std::cout << "  - MuDP EOS = " << double_eos_ << " , MuDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_mu_d_T_p_pT(P,T,double_eos_);
  double_cool_ = FD_derivative_pT(params::MU, P, T, fluide_cool, false); //fluide_cool->first_partial_deriv(CoolProp::iviscosity,  CoolProp::iHmass, CoolProp::iP);
  std::cout << "  - MuDT EOS = " << double_eos_ << " , MuDT CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // lambda
  fluide_eos.compute_lambda_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->conductivity();
  std::cout << "  - Lambda EOS = " << double_eos_ << " , Lambda CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_lambda_d_p_T_pT(P,T,double_eos_);
  double_cool_ = FD_derivative_pT(params::LAMBDA, P, T, fluide_cool); //fluide_cool->first_partial_deriv(CoolProp::iconductivity, CoolProp::iP, CoolProp::iHmass);
  std::cout << "  - LambdaDP EOS = " << double_eos_ << " , LambdaDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_lambda_d_T_p_pT(P,T,double_eos_);
  double_cool_ = FD_derivative_pT(params::LAMBDA, P, T, fluide_cool, false); //fluide_cool->first_partial_deriv(CoolProp::iconductivity,  CoolProp::iHmass, CoolProp::iP);
  std::cout << "  - LambdaDT EOS = " << double_eos_ << " , LambdaDT CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // sigma
  fluide_eos.compute_sigma_pT(P,T,double_eos_);
  double_cool_ = fluide_cool->surface_tension();
  std::cout << "  - Sigma EOS = " << double_eos_ << " , Sigma CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_sigma_d_p_T_pT(P,T,double_eos_);
  double_cool_ = FD_derivative_pT(params::SIGMA, P, T, fluide_cool); //fluide_cool->first_partial_deriv(CoolProp::isurface_tension, CoolProp::iP, CoolProp::iHmass);
  std::cout << "  - SigmaDP EOS = " << double_eos_ << " , SigmaDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_sigma_d_T_p_pT(P,T,double_eos_);
  double_cool_ = FD_derivative_pT(params::SIGMA, P, T, fluide_cool, false); //fluide_cool->first_partial_deriv(CoolProp::isurface_tension,  CoolProp::iHmass, CoolProp::iP);
  std::cout << "  - SigmaDH EOS = " << double_eos_ << " , SigmaDH CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;
}

void test_sats_p(const std::string fld,const double P, const double T, EOS& fluide_eos, CoolProp::AbstractState * fluide_cool)
{
  test_counter++;

  std::cout << std::endl;
  std::cout << "*************** TEST : " << test_counter << std::endl;
  std::cout << std::endl;
  std::cout << "    TEST SATURATION" << std::endl;
  std::cout << "    FLUID : " << fld << std::endl;
  std::cout << std::endl;
  std::cout << std::endl;

  double double_eos_ = -1000. , double_cool_ = -1000.;

  // Tsat
  fluide_eos.compute_T_sat_p(P,double_eos_);
  fluide_cool->update(CoolProp::PQ_INPUTS,  P, 0);  // SI units
  double_cool_ = fluide_cool->T();
  std::cout << "  - Tsat EOS = " << double_eos_ << " , Tsat CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_T_sat_d_p_p(P,double_eos_);
  double_cool_ = FD_derivative_p(params::T, P, fluide_cool); //fluide_cool->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
  std::cout << "  - TsatDP EOS = " << double_eos_ << " , TsatDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // P sat
  fluide_eos.compute_p_sat_T(T,double_eos_);
  fluide_cool->update(CoolProp::QT_INPUTS, 0, T);  // SI units
  double_cool_ = fluide_cool->p();
  std::cout << "  - Psat EOS = " << double_eos_ << " , Psat CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_p_sat_d_T_T(T,double_eos_);
  double_cool_ = FD_derivative_p(params::P, T, fluide_cool); //fluide_cool->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
  std::cout << "  - TsatDP EOS = " << double_eos_ << " , TsatDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // hl sat
  fluide_eos.compute_h_l_sat_p(P,double_eos_);
  fluide_cool->update(CoolProp::PQ_INPUTS,  P, 0);  // SI units
  double_cool_ = fluide_cool->hmass();
  std::cout << "  - Hlsat EOS = " << double_eos_ << " , Hlsat CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_h_l_sat_d_p_p(P,double_eos_);
  double_cool_ = FD_derivative_p(params::HL, P, fluide_cool); //fluide_cool->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
  std::cout << "  - HlsatDP EOS = " << double_eos_ << " , HlsatDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // hv sat
  fluide_eos.compute_h_v_sat_p(P,double_eos_);
  fluide_cool->update(CoolProp::PQ_INPUTS,  P, 1);  // SI units
  double_cool_ = fluide_cool->hmass();
  std::cout << "  - Hvsat EOS = " << double_eos_ << " , Hvsat CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_h_v_sat_d_p_p(P,double_eos_);
  double_cool_ = FD_derivative_p(params::HV, P, fluide_cool, false); //fluide_cool->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
  std::cout << "  - HvsatDP EOS = " << double_eos_ << " , HvsatDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // rhol sat
  fluide_eos.compute_rho_l_sat_p(P,double_eos_);
  fluide_cool->update(CoolProp::PQ_INPUTS,  P, 0);  // SI units
  double_cool_ = fluide_cool->rhomass();
  std::cout << "  - Rholsat EOS = " << double_eos_ << " , Rholsat CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_rho_l_sat_d_p_p(P,double_eos_);
  double_cool_ = FD_derivative_p(params::RHOL, P, fluide_cool); //fluide_cool->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
  std::cout << "  - RholsatDP EOS = " << double_eos_ << " , RholsatDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // rhov sat
  fluide_eos.compute_rho_v_sat_p(P,double_eos_);
  fluide_cool->update(CoolProp::PQ_INPUTS,  P, 1);  // SI units
  double_cool_ = fluide_cool->rhomass();
  std::cout << "  - Rhovsat EOS = " << double_eos_ << " , Rhovsat CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_rho_v_sat_d_p_p(P,double_eos_);
  double_cool_ = FD_derivative_p(params::RHOV, P, fluide_cool, false); //fluide_cool->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
  std::cout << "  - RhovsatDP EOS = " << double_eos_ << " , RhovsatDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // cpl sat
  fluide_eos.compute_cp_l_sat_p(P,double_eos_);
  fluide_cool->update(CoolProp::PQ_INPUTS,  P, 0);  // SI units
  double_cool_ = fluide_cool->cpmass();
  std::cout << "  - Cplsat EOS = " << double_eos_ << " , Cplsat CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_cp_l_sat_d_p_p(P,double_eos_);
  double_cool_ = FD_derivative_p(params::CPL, P, fluide_cool); //fluide_cool->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
  std::cout << "  - CplsatDP EOS = " << double_eos_ << " , CplsatDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;

  // cpv sat
  fluide_eos.compute_cp_v_sat_p(P,double_eos_);
  fluide_cool->update(CoolProp::PQ_INPUTS,  P, 1);  // SI units
  double_cool_ = fluide_cool->cpmass();
  std::cout << "  - Cpvsat EOS = " << double_eos_ << " , Cpvsat CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  fluide_eos.compute_d_cp_v_sat_d_p_p(P,double_eos_);
  double_cool_ = FD_derivative_p(params::CPV, P, fluide_cool, false); //fluide_cool->first_saturation_deriv(CoolProp::iT, CoolProp::iP);
  std::cout << "  - CpvsatDP EOS = " << double_eos_ << " , CpvsatDP CoolProp = " <<  double_cool_ << " ======> RELATIVE ERROR (%) = " << std::abs((double_eos_ - double_cool_) / double_eos_) * 100 << std::endl;

  std::cout << std::endl;
}

/*
 * The main !
 */
int main(int argc, char const *argv[])
{
  const char* have_eos = std::getenv("TRUST_USE_EOS");
  if (have_eos != nullptr && std::string(have_eos) == "0") return 1;

  const char *use_refprop = std::getenv("TRUST_USE_REFPROP");
  if (use_refprop != nullptr && std::string(use_refprop) == "1")
    {
      const char * refprop_dir = std::getenv("TRUST_REFPROP_HOME_DIR");
      CoolProp::set_config_string(configuration_keys::ALTERNATIVE_REFPROP_PATH, std::string(refprop_dir));
    }

  CoolProp::AbstractState * fluide_cool = CoolProp::AbstractState::factory("REFPROP", "Water");

  EOS_Std_Error_Handler handler;
  handler.set_exit_on_error(EOS_Std_Error_Handler::disable_feature);
  handler.set_throw_on_error(EOS_Std_Error_Handler::disable_feature);
  handler.set_dump_on_error(EOS_Std_Error_Handler::disable_feature);

  {
    // test liquid phase
    const double P = 1.35e7, H = 7.e5, T = 300.;

    fluide_cool->specify_phase(CoolProp::iphase_liquid);
    fluide_cool->update(CoolProp::HmassP_INPUTS, H, P);

    EOS fluide_eos("EOS_Refprop10", "WaterLiquid");
    fluide_eos.set_error_handler(handler);

    test_fluids_ph("Water_liquid", P, H, fluide_eos, fluide_cool);

    fluide_cool->specify_phase(CoolProp::iphase_liquid);
    fluide_cool->update(CoolProp::PT_INPUTS, P, T);
    test_fluids_pt("Water_liquid", P, T, fluide_eos, fluide_cool);

    fluide_cool->unspecify_phase();
  }

  {
    // test vapor phase
    const double P = 10.e6, H = 3.e6, T = 600.;

    fluide_cool->specify_phase(CoolProp::iphase_gas);
    fluide_cool->update(CoolProp::HmassP_INPUTS, H, P);

    EOS fluide_eos("EOS_Refprop10", "WaterVapor");
    fluide_eos.set_error_handler(handler);

    test_fluids_ph("Water_vapor", P, H, fluide_eos, fluide_cool);

    fluide_cool->specify_phase(CoolProp::iphase_gas);
    fluide_cool->update(CoolProp::PT_INPUTS, P, T);
    test_fluids_pt("Water_vapor", P, T, fluide_eos, fluide_cool);

    fluide_cool->unspecify_phase();
  }

  {
    // test saturation
    const double P = 1.35e7, T = 500.;
    fluide_cool->specify_phase(CoolProp::iphase_liquid);
    EOS fluide_eos("EOS_Refprop10", "WaterLiquid");
    fluide_eos.set_error_handler(handler);

    test_sats_p("Water_liquid", P, T, fluide_eos, fluide_cool);
  }

  delete fluide_cool;
  return 1;
}
