/****************************************************************************
* Copyright (c) 2023, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/

#include <IJK_Test_Multigrille.h>
#include <Statistiques.h>
#include <Interprete_bloc.h>
#include <simd_tools.h>

Implemente_instanciable(IJK_Test_Multigrille, "IJK_Test_Multigrille", Interprete);

Sortie& IJK_Test_Multigrille::printOn(Sortie& s) const
{
  return s;
}

Entree& IJK_Test_Multigrille::readOn(Entree& s)
{
  return s;
}

Entree& IJK_Test_Multigrille::interpreter(Entree& is)
{
  IJK_Splitting split;

  Param param(que_suis_je());
  Nom ijk_splitting_name;
  Nom name_of_dom_;
  param.ajouter("ijk_splitting", &ijk_splitting_name, Param::REQUIRED);
  param.ajouter("multigrid_solver", &poisson_solver_, Param::REQUIRED);
  param.ajouter("fichier_reprise_rhs", &fichier_reprise_rhs_);
  param.ajouter("fichier_reprise_rho", &fichier_reprise_rho_);
  param.ajouter("timestep_reprise_rhs", &timestep_reprise_rhs_);
  param.ajouter("timestep_reprise_rho", &timestep_reprise_rho_);
  param.ajouter("expression_rho", &expression_rho_);
  param.ajouter("expression_rhs", &expression_rhs_);
  param.ajouter("name_of_dom", &name_of_dom_);
  param.lire_avec_accolades(is);

  // Recuperation des donnees de maillage
  split = ref_cast(IJK_Splitting, Interprete_bloc::objet_global(ijk_splitting_name));
  static Stat_Counter_Id count0 = statistiques().new_counter(0, "timing_init");

  statistiques().begin_count(count0);
  rho_.allocate(split, IJK_Splitting::ELEM, 0);
  rhs_.allocate(split, IJK_Splitting::ELEM, 0);
  resu_.allocate(split, IJK_Splitting::ELEM, 0);

  if (expression_rho_!="??")
    {
      set_field_data(rho_, expression_rho_);
      /*   Nom lata_name("rho.lata");
      dumplata_header(lata_name,rho_);
      //dumplata_add_geometry(lata_name,rho_);
      dumplata_newtime(lata_name,0.);
      dumplata_scalar(lata_name,"RHO",rho_,0);
      */
    }
  else if (fichier_reprise_rho_ != "??")
    {
      lire_dans_lata(fichier_reprise_rho_, timestep_reprise_rho_, name_of_dom_, "RHO", rho_);
    }

  Cout<<"End of setting rho"<<finl;
  if (expression_rhs_!="??")
    {
      set_field_data(rhs_, expression_rhs_);
    }
  else if (fichier_reprise_rhs_ != "??")
    {
      lire_dans_lata(fichier_reprise_rhs_, timestep_reprise_rhs_, name_of_dom_, "PRESSURE_RHS", rhs_);
    }
  Cout<<"End of setting rhs"<<finl;
  statistiques().end_count(count0);
  double t0 = statistiques().last_time(count0);
  statistiques().begin_count(count0);
  poisson_solver_.initialize(split);
  statistiques().end_count(count0);
  double t0b = statistiques().last_time(count0);
  Cout <<"initialisation time "<<t0<< " "<<t0b<<finl;
  static Stat_Counter_Id count1 = statistiques().new_counter(0, "timing_set_rho");
  static Stat_Counter_Id count2 = statistiques().new_counter(0, "timing_solve");
  statistiques().begin_count(count1);
  poisson_solver_.set_rho(rho_);
  statistiques().end_count(count1);

  statistiques().begin_count(count2);
  poisson_solver_.resoudre_systeme_IJK(rhs_, resu_);
  statistiques().end_count(count2);

  /*   */
  Nom lata_name("resu.lata");
  dumplata_header(lata_name,resu_);
  dumplata_add_geometry(lata_name,resu_);
  dumplata_newtime(lata_name,0.);
  dumplata_scalar(lata_name,"RESU",resu_,0);
  dumplata_scalar(lata_name,"PRESSURE_RHS",rhs_,0);
  dumplata_scalar(lata_name,"RHO",rho_,0);
  /*  */

  double t1 = statistiques().last_time(count1);
  double t2 = statistiques().last_time(count2);
#if defined WITH_SSE
  Nom optim("SSE");
#elif defined WITH_AVX
  Nom optim("AVX");
#else
  Nom optim("DISABLED");
#endif

  Cout << "Timing for Poisson resolution (vectorization " << optim << " ): set_rho: " << t1 << " solve: " << t2 << finl;

  return is;
}
