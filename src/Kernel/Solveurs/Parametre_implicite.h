/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Parametre_implicite.h
// Directory:   $TRUST_ROOT/src/Kernel/Solveurs
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Parametre_implicite_included
#define Parametre_implicite_included



#include <Parametre_equation_base.h>
#include <SolveurSys.h>
#include <Parser_U.h>
#include <Equation_base.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Parametre_implicite
//     Un objet Parametre_implicite est un objet regroupant les differentes
//     options specifiques pour piso ou simpler
//
//////////////////////////////////////////////////////////////////////////////
class Parametre_implicite : public Parametre_equation_base
{
  Declare_instanciable(Parametre_implicite);
public:
  inline SolveurSys& solveur()
  {
    return le_solveur_ ;
  };
  inline double& seuil_generation_solveur()
  {
    return  seuil_generation_solveur_ ;
  };
  inline double& seuil_verification_solveur()
  {
    return  seuil_verification_solveur_ ;
  } ;
  inline double& seuil_test_preliminaire_solveur()
  {
    return  seuil_test_preliminaire_solveur_ ;
  };

  inline  double& seuil_convergence_implicite()
  {
    return  seuil_convergence_implicite_ ;
  };
  inline  double& seuil_diffusion_implicite()
  {
    return  seuil_diffusion_implicite_ ;
  };
  inline  int& nb_it_max()
  {
    return  nb_it_max_ ;
  };
  inline int& calcul_explicite()
  {
    return calcul_explicite_ ;
  };
  inline int equation_frequence_resolue(double t)
  {
    equation_frequence_resolue_.setVar("t",t);
    return (int)equation_frequence_resolue_.eval() ;
  };

  void set_seuil_solveur_avec_seuil_convergence_solveur(double );
  int seuil_test_preliminaire_lu() const
  {
    return seuil_test_preliminaire_lu_;
  };
protected:
  SolveurSys le_solveur_;
  double seuil_generation_solveur_,seuil_test_preliminaire_solveur_,seuil_verification_solveur_;
  int seuil_test_preliminaire_lu_;
  double seuil_convergence_implicite_;
  int nb_it_max_,calcul_explicite_;
  double seuil_diffusion_implicite_;
  Parser_U equation_frequence_resolue_;
};
#endif

