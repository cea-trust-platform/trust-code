#define SWIG
#include <Configuration.h>
#undef SWIG

#include "CoolPropTools.h"
#include "AbstractState.h"
#include "CoolPropLib.h"
#include "CoolProp.h"
#include <span.hpp>
#include <iostream>
#include <string>
#include <chrono>
#include <array>

using namespace std::chrono;
using namespace std;

int main(int argc, char const *argv[])
{
  const char *use_refprop = getenv("TRUST_USE_REFPROP");

  if (use_refprop != nullptr && string(use_refprop) == "1")
    {
      const char * refprop_dir = getenv("TRUST_REFPROP_HOME_DIR");
      CoolProp::set_config_string(configuration_keys::ALTERNATIVE_REFPROP_PATH, string(refprop_dir));
    }

  array<string, 1> fluids = { { "Water" } };
  array<string, 6> backends = { { "HEOS" , "BICUBIC&HEOS", "REFPROP" , /* "IF97" , */ "SRK", "PR", "TTSE&HEOS" } };
  const double p = 16415351.434999999;
  const double Rhol = 576.955, Rhov = 112.24, T = 622.586;
  const int N = 10000;

  cout << endl;
  cout << "------------------------------------------------------------------------" << endl;
  for (const auto& itr_fl : fluids )
    for (const auto& itr_bk : backends )
      {
        shared_ptr<CoolProp::AbstractState> Water_l(CoolProp::AbstractState::factory(itr_bk, itr_fl));
        shared_ptr<CoolProp::AbstractState> Water_v(CoolProp::AbstractState::factory(itr_bk, itr_fl));
        Water_l->specify_phase(CoolProp::iphase_liquid);
        Water_v->specify_phase(CoolProp::iphase_gas);
        {
          cout << endl;
          cout << "*** Test PQ_INPUTS with fluid : " << itr_fl << " and backend : " << itr_bk << endl;
          cout << endl;
          auto start = high_resolution_clock::now();
          for (int i = 0; i < N; i++)
            {
              Water_l->update(CoolProp::PQ_INPUTS, p, 0);  // SI units
              Water_v->update(CoolProp::PQ_INPUTS, p, 1);  // SI units

              const double t_s = Water_l->T();
              if (t_s != Water_v->T()) throw;

              const double h_v = Water_v->hmass();
              const double h_l = Water_l->hmass();
              const double cp_v = Water_v->cpmass();
              const double cp_l = Water_l->cpmass();
              const double rho_v = Water_v->rhomass();
              const double rho_l = Water_l->rhomass();
            }
          auto stop = high_resolution_clock::now();
          auto duration = duration_cast<milliseconds>(stop - start);
          cout << "    TIME IN MilliSEC =  " <<  duration.count() << endl;
        }

        cout << endl;

        {
          cout << endl;
          auto start = high_resolution_clock::now();
          for (int i = 0; i < N; i++)
            {
              Water_l->update(CoolProp::PQ_INPUTS, p, 0);  // SI units
              Water_v->update(CoolProp::PQ_INPUTS, p, 1);  // SI units
            }
          auto stop = high_resolution_clock::now();
          auto duration = duration_cast<milliseconds>(stop - start);
          cout << "    TIME IN MilliSEC (just update) =  " <<  duration.count() << endl;
        }

        cout << endl;

        {
          cout << "*** Test DmassT_INPUTS with fluid : " << itr_fl << " and backend : " << itr_bk << endl;
          cout << endl;
          auto start = high_resolution_clock::now();

          for (int i = 0; i < N; i++)
            {
              Water_l->update(CoolProp::DmassT_INPUTS, Rhol, T);  // SI units
              Water_v->update(CoolProp::DmassT_INPUTS, Rhov, T);  // SI units

              const double h_v = Water_v->hmass();
              const double h_l = Water_l->hmass();
              const double cp_v = Water_v->cpmass();
              const double cp_l = Water_l->cpmass();
              const double rho_v = Water_v->rhomass();
              const double rho_l = Water_l->rhomass();
            }

          auto stop = high_resolution_clock::now();
          auto duration = duration_cast<milliseconds>(stop - start);
          cout << "    TIME IN MilliSEC =  " <<  duration.count() << endl;
        }

        cout << endl;

        {
          cout << endl;
          auto start = high_resolution_clock::now();

          for (int i = 0; i < N; i++)
            {
              Water_l->update(CoolProp::DmassT_INPUTS, Rhol, T);  // SI units
              Water_v->update(CoolProp::DmassT_INPUTS, Rhov, T);  // SI units
            }

          auto stop = high_resolution_clock::now();
          auto duration = duration_cast<milliseconds>(stop - start);
          cout << "    TIME IN MilliSEC (just update) =  " <<  duration.count() << endl;
        }

        cout << endl;

        {
          cout << "*** Test PT_INPUTS with fluid : " << itr_fl << " and backend : " << itr_bk << endl;
          cout << endl;
          auto start = high_resolution_clock::now();

          for (int i = 0; i < N; i++)
            {
              Water_l->update(CoolProp::PT_INPUTS, p, T);  // SI units
              Water_v->update(CoolProp::PT_INPUTS, p, T);  // SI units

              const double h_v = Water_v->hmass();
              const double h_l = Water_l->hmass();
              const double cp_v = Water_v->cpmass();
              const double cp_l = Water_l->cpmass();
              const double rho_v = Water_v->rhomass();
              const double rho_l = Water_l->rhomass();
            }

          auto stop = high_resolution_clock::now();
          auto duration = duration_cast<milliseconds>(stop - start);
          cout << "    TIME IN MilliSEC =  " <<  duration.count() << endl;
        }

        cout << endl;

        {
          cout << "*** Test PT_INPUTS (just update) with fluid : " << itr_fl << " and backend : " << itr_bk << endl;
          cout << endl;
          auto start = high_resolution_clock::now();

          for (int i = 0; i < N; i++)
            {
              Water_l->update(CoolProp::PT_INPUTS, p, T);  // SI units
              Water_v->update(CoolProp::PT_INPUTS, p, T);  // SI units
            }

          auto stop = high_resolution_clock::now();
          auto duration = duration_cast<milliseconds>(stop - start);
          cout << "    TIME IN MilliSEC =  " <<  duration.count() << endl;
        }

        cout << endl;
        cout << "------------------------------------------------------------------------" << endl;
      }
  cout << endl;

  return 1;
}
