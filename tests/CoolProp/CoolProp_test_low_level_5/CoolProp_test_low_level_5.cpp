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
  std::string backend = "BICUBIC&HEOS";

  std::cout << "==========================================================================================================" << std::endl;
  std::cout << "Test simple calls to CoolProp using the low level interface with std vectors for fluid : " << fluid << std::endl;
  std::cout << "==========================================================================================================" << std::endl;
  std::cout << std::endl;

  const long buffer_size = 1000, length = 100000;
  long ierr;
  char herr[buffer_size];

  long input_pair = get_input_pair_index("HmassP_INPUTS");
  long handle = AbstractState_factory(backend.c_str(), fluid.c_str(), &ierr, herr, buffer_size);
  std::vector<double> input1 = linspace(700000.0, 1500000.0, length);
  std::vector<double> input2 = linspace(2.8e6, 3.0e6, length);

  {
    std::vector<double> T(length), p(length), rhomolar(length), hmolar(length), smolar(length);

    // enthalpy & P
    double t1 = clock();

    AbstractState_update_and_common_out(handle, input_pair, &(input1[0]), &(input2[0]), length,
                                        &(T[0]), &(p[0]), &(rhomolar[0]), &(hmolar[0]), &(smolar[0]),
                                        &ierr, herr, buffer_size);

    double t2 = clock();

    //  std::cout << format("value(commons): %g us/call\n", ((double)(t2 - t1)) / CLOCKS_PER_SEC / double(length) * 1e6);
    std::cout << format("value(commons): %g minutes \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC / 60);
    std::cout << format("value(commons): %g seconds \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);
  }

  std::cout << std::endl;

  {
    std::vector<long> outputs(5);
    outputs[0] = get_param_index("T");
    outputs[1] = get_param_index("P");
    outputs[2] = get_param_index("Dmolar");
    outputs[3] = get_param_index("Hmolar");
    outputs[4] = get_param_index("Smolar");

    std::vector<double> out1(length), out2(length), out3(length), out4(length), out5(length);

    double t1 = clock();

    AbstractState_update_and_5_out(handle, input_pair, &(input1[0]), &(input2[0]), length,
                                   &(outputs[0]), &(out1[0]), &(out2[0]), &(out3[0]), &(out4[0]), &(out5[0]),
                                   &ierr, herr, buffer_size);

    double t2 = clock();
    //  std::cout << format("value(user-specified): %g us/call\n", ((double)(t2 - t1)) / CLOCKS_PER_SEC / double(length) * 1e6);
    std::cout << format("value(user-specified): %g minutes \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC / 60);
    std::cout << format("value(user-specified): %g seconds \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);
  }

  std::cout << std::endl;

  // user loop
  {
    shared_ptr<CoolProp::AbstractState> Water(CoolProp::AbstractState::factory(backend, fluid));
    std::vector<double> T(length), p(length), rhomolar(length), hmolar(length), smolar(length);
    std::span<double> T_(T), p_(p), rho_(rhomolar), h_(hmolar), s_(smolar);

    double t1 = clock();

    for (int i = 0; i < length; i++)
      {
        Water->update(CoolProp::HmassP_INPUTS, input1[i], input2[i]);  // SI units
        T_[i] = Water->T();
        p_[i] = Water->p();
        rho_[i] = Water->rhomolar();
        h_[i] = Water->hmolar();
        s_[i] = Water->smolar();
      }

    double t2 = clock();

    std::cout << format("value(My loop / molar): %g minutes \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC / 60);
    std::cout << format("value(My loop / molar): %g seconds \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);
  }

  std::cout << std::endl;

  // user loop
  {
    shared_ptr<CoolProp::AbstractState> Water(CoolProp::AbstractState::factory(backend, fluid));
    std::vector<double> T(length), p(length), rho(length), h(length), s(length);
    std::span<double> T_(T), p_(p), rho_(rho), h_(h), s_(s);

    double t1 = clock();

    for (int i = 0; i < length; i++)
      {
        Water->update(CoolProp::HmassP_INPUTS, input1[i], input2[i]);  // SI units
        T_[i] = Water->T();
        p_[i] = Water->p();
        rho_[i] = Water->rhomass();
        h_[i] = Water->hmass();
        s_[i] = Water->smass();
      }

    double t2 = clock();

    std::cout << format("value(My loop / mass): %g minutes \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC / 60);
    std::cout << format("value(My loop / mass): %g seconds \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);
  }

  std::cout << std::endl;

  // user loop
  {
    std::vector<double> input3 = linspace(1e5, 1.6e5, length);
    std::vector<double> input4 = linspace(300.0, 390.0, length);

    //          CoolProp::AbstractState *Water = CoolProp::AbstractState::factory("HEOS", fluid);
    shared_ptr<CoolProp::AbstractState> Water(CoolProp::AbstractState::factory("BICUBIC&HEOS", fluid));
    std::vector<double> T(length), dTP(length), dTH(length),
        rho(length), drhoP(length), drhoH(length),
        mu(length), dmuP(length), dmuH(length),
        lambda(length), dlambdaP(length), dlambdaH(length),
        cp(length), dcpP(length), dcpH(length);

    std::span<double> T_(T), dTP_(dTP), dTH_(dTH),
        rho_(rho), drhoP_(drhoP), drhoH_(drhoH),
        mu_(mu), dmuP_(dmuP), dmuH_(dmuH),
        lambda_(lambda), dlambdaP_(dlambdaP), dlambdaH_(dlambdaH),
        cp_(cp), dcpP_(dcpP), dcpH_(dcpH);

    double t1 = clock();

    for (int i = 0; i < length; i++)
      {
        Water->update(CoolProp::PT_INPUTS, input3[i], input4[i]);  // SI units

        T_[i] = Water->hmass();
        dTP_[i] = Water->first_partial_deriv(CoolProp::iHmass, CoolProp::iP, CoolProp::iT);
        dTH_[i] = Water->first_partial_deriv(CoolProp::iHmass, CoolProp::iT, CoolProp::iP);

        rho_[i] = Water->rhomass();
        drhoP_[i] = Water->first_partial_deriv(CoolProp::iDmass, CoolProp::iP, CoolProp::iT);
        drhoH_[i] = Water->first_partial_deriv(CoolProp::iDmass, CoolProp::iT, CoolProp::iP);

        mu_[i] = Water->viscosity();
        //              dmuP_[i] = Water->first_partial_deriv(CoolProp::iviscosity, CoolProp::iP, CoolProp::iT);
        //              dmuH_[i] = Water->first_partial_deriv(CoolProp::iviscosity, CoolProp::iT, CoolProp::iP);

        lambda_[i] = Water->conductivity();
        //              dlambdaP_[i] = Water->first_partial_deriv(CoolProp::iconductivity, CoolProp::iP, CoolProp::iT);
        //              dlambdaH_[i] = Water->first_partial_deriv(CoolProp::iconductivity, CoolProp::iT, CoolProp::iP);

        cp_[i] = Water->cpmass();
        //              dcpP_[i] = Water->first_partial_deriv(CoolProp::iCpmass, CoolProp::iP, CoolProp::iT);
        //              dcpH_[i] = Water->first_partial_deriv(CoolProp::iCpmass, CoolProp::iT, CoolProp::iP);
      }

    double t2 = clock();

    std::cout << format("value(My loop / mass): %g minutes \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC / 60);
    std::cout << format("value(My loop / mass): %g seconds \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);

    std::cout << std::endl;

    //          long handle2 = AbstractState_factory("HEOS", fluid.c_str(), &ierr, herr, buffer_size);
    long handle2 = AbstractState_factory("BICUBIC&HEOS", fluid.c_str(), &ierr, herr, buffer_size);

    t1 = clock();

    AbstractState_update_and_common_out(handle2, input_pair, &(input3[0]), &(input4[0]), length,
                                        &(T[0]), &(dTP[0]), &(dTH[0]),
                                        &(rho[0]), &(drhoP[0]), &(drhoH[0]),
                                        &(mu[0]), &(lambda[0]), &(cp[0]),
                                        &ierr, herr, buffer_size);

    t2 = clock();

    std::cout << format("value(My loop coolprop / mass): %g minutes \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC / 60);
    std::cout << format("value(My loop coolprop / mass): %g seconds \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);
  }

  std::cout << std::endl;
  // test 1 output
  {
    std::vector<double> input3 = linspace(1e5, 1.6e5, length);
    std::vector<double> input4 = linspace(300.0, 390.0, length);
    std::vector<double> H(length);

    long input_pair_ = get_input_pair_index("PT_INPUTS");
    long handle_ = AbstractState_factory("HEOS", fluid.c_str(), &ierr, herr, buffer_size);

    // T & P
    double t1 = clock();

    AbstractState_update_and_1_out(handle_ , input_pair_, &(input3[0]), &(input4[0]), length,CoolProp::iDmass,
                                   &(H[0]),&ierr, herr, buffer_size);

    double t2 = clock();

    //  std::cout << format("value(commons): %g us/call\n", ((double)(t2 - t1)) / CLOCKS_PER_SEC / double(length) * 1e6);
    std::cout << format("value(1 output): %g minutes \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC / 60);
    std::cout << format("value(1 output): %g seconds \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);
    std::cout << std::endl;

    shared_ptr<CoolProp::AbstractState> Water(CoolProp::AbstractState::factory("HEOS", fluid));
    t1 = clock();

    for (int i = 0; i < length; i++)
      {
        Water->update(CoolProp::PT_INPUTS, input3[i], input4[i]);  // SI units
        H[i] = Water->hmass();
      }

    t2 = clock();

    std::cout << format("value(My loop 1 output / mass): %g minutes \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC / 60);
    std::cout << format("value(My loop 1 output / mass): %g seconds \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);

    std::cout << std::endl;

    // T & P
    t1 = clock();

    AbstractState_update_and_1_out(/* handle_ */ handle, input_pair_, &(input3[0]), &(input4[0]), length,CoolProp::iDmass,
                                   &(H[0]),&ierr, herr, buffer_size);

    t2 = clock();

    //  std::cout << format("value(commons): %g us/call\n", ((double)(t2 - t1)) / CLOCKS_PER_SEC / double(length) * 1e6);
    std::cout << format("value(1 output): %g minutes \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC / 60);
    std::cout << format("value(1 output): %g seconds \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);
    std::cout << std::endl;

    shared_ptr<CoolProp::AbstractState> Water2(CoolProp::AbstractState::factory("BICUBIC&HEOS", fluid));
    t1 = clock();

    for (int i = 0; i < length; i++)
      {
        Water2->update(CoolProp::PT_INPUTS, input3[i], input4[i]);  // SI units
        H[i] = Water2->hmass();
      }

    t2 = clock();

    std::cout << format("value(My loop 1 output / mass): %g minutes \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC / 60);
    std::cout << format("value(My loop 1 output / mass): %g seconds \n", ((double) (t2 - t1)) / CLOCKS_PER_SEC);
  }

  return 1;
}
