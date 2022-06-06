/****************************************************************************
* Copyright (c) 2021, CEA
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

#include <Parametre_implicite.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Parametre_implicite,"Parametre_implicite",Parametre_equation_base);

// Par defaut les seuils sont negatifs (donc pas utilisables!!)
Parametre_implicite::Parametre_implicite()
{
  seuil_convergence_implicite()=-1.;
  //  seuil_convergence_solveur()=-1.;
  seuil_generation_solveur_=-1;
  seuil_verification_solveur_=-1;
  seuil_test_preliminaire_solveur_=-1;
  seuil_test_preliminaire_lu_=0;
  seuil_diffusion_implicite_=-1;
  nb_it_max_=1000000;
  calcul_explicite_=0;
  Nom eq_freq_resolue="0";
  equation_frequence_resolue_.setNbVar(1);
  equation_frequence_resolue_.setString(eq_freq_resolue);
  equation_frequence_resolue_.addVar("t");
  equation_frequence_resolue_.parseString();
}

Sortie& Parametre_implicite::printOn(Sortie& os) const
{
  return os;
}

// XD parametre_implicite parametre_equation_base parametre_implicite -1 Keyword to change for this equation only the parameter of the implicit scheme used to solve the problem.
// XD attr seuil_convergence_implicite floattant seuil_convergence_implicite 1 Keyword to change for this equation only the value of seuil_convergence_implicite used in the implicit scheme.
// XD attr seuil_convergence_solveur floattant seuil_convergence_solveur 1 Keyword to change for this equation only the value of seuil_convergence_solveur used in the implicit scheme
// XD attr solveur solveur_sys_base solveur 1 Keyword to change for this equation only the solver used in the implicit scheme
// XD attr resolution_explicite rien resolution_explicite 1 To solve explicitly the equation whereas the scheme is an implicit scheme.
// XD attr equation_non_resolue rien equation_non_resolue 1 Keyword to specify that the equation is not solved.
// XD attr equation_frequence_resolue chaine equation_frequence_resolue 1 Keyword to specify that the equation is solved only every n time steps (n is an integer or given by a time-dependent function f(t)).
Entree& Parametre_implicite::readOn(Entree& is)
{
  Nom eq_freq_resolue="0";
  double seuil_convergence_solveur_prov=-1;
  Param param(que_suis_je());
  param.ajouter("solveur",&le_solveur_);
  param.ajouter( "seuil_convergence_implicite",&seuil_convergence_implicite_);
  param.ajouter( "seuil_convergence_solveur",&seuil_convergence_solveur_prov);

  param.ajouter("seuil_generation_solveur",&seuil_generation_solveur_);
  param.ajouter("seuil_test_preliminaire_solveur",&seuil_test_preliminaire_solveur_);
  param.ajouter("seuil_verification_solveur",&seuil_verification_solveur_);

  param.ajouter( "resolution_explicite_diffusion_implicite",&seuil_diffusion_implicite_);
  param.ajouter_flag( "resolution_explicite",&calcul_explicite_);
  param.ajouter( "equation_frequence_resolue",&eq_freq_resolue);
  param.lire_avec_accolades_depuis(is);
  if (param.get_list_mots_lus().rang("seuil_test_preliminaire_solveur")>0)
    seuil_test_preliminaire_lu_=1;
  if (seuil_convergence_solveur_prov>0)
    set_seuil_solveur_avec_seuil_convergence_solveur(seuil_convergence_solveur_prov);
  if (seuil_diffusion_implicite_>0)
    calcul_explicite_=1;
  equation_frequence_resolue_.setString(eq_freq_resolue);
  equation_frequence_resolue_.parseString();
  le_solveur_.nommer("solveur_implicite");
  return is;
}

// Description: permet de mettre les seuils equivalents a ceux de la 161
// pour l'instant warning ensuite ereur.
void  Parametre_implicite::set_seuil_solveur_avec_seuil_convergence_solveur(double seuil_convergence_solveur)
{
  if (seuil_convergence_solveur<0)
    return;
  Cerr<< "Warning: using of seuil_convergence_solveur is obsolete, use seuil_generation_solveur,seuil_test_preliminaire_solveur,seuil_verification_solveur "<<finl;
  /*
    #ifndef NDEBUG
    Cerr << "This warning is an error when using a debug TRUST version." << finl;
    Process::exit();
    #endif
  */
  if (seuil_generation_solveur_<0)
    seuil_generation_solveur_=seuil_convergence_solveur;
  else
    {
      Cerr<< "You can not use seuil_generation_solveur and seuil_convergence_solveur in the same block" <<finl;
      exit();
    }
  if (seuil_verification_solveur_<0)
    seuil_verification_solveur_=seuil_convergence_solveur;
  else
    {
      Cerr<< "You can not use seuil_verification_solveur and seuil_convergence_solveur in the same block" <<finl;
      exit();
    }
  if (seuil_test_preliminaire_solveur_<0)
    seuil_test_preliminaire_solveur_=seuil_convergence_solveur;
  else
    {
      Cerr<< "You can not use seuil_test_preliminaire_solveur and seuil_convergence_solveur in the same block" <<finl;
      exit();
    }
}

