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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Parametre_diffusion_implicite.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Solveurs
// Version:     /main/4
//
//////////////////////////////////////////////////////////////////////////////

#include <Parametre_diffusion_implicite.h>
#include <Param.h>

Implemente_instanciable_sans_constructeur(Parametre_diffusion_implicite,"Parametre_diffusion_implicite",Parametre_equation_base);

// Par defaut les seuils sont negatifs (donc pas utilisables!!)
Parametre_diffusion_implicite::Parametre_diffusion_implicite()
{
  preconditionnement_diag_=0;
  crank_=0;
  nb_it_max_=-1;
  seuil_diffusion_implicite_=-1;

}

Sortie& Parametre_diffusion_implicite::printOn(Sortie& os) const
{
  return os;
}
// XD parametre_diffusion_implicite parametre_equation_base parametre_diffusion_implicite -1 To specify additional parameters for the equation when using impliciting diffusion
// XD attr crank entier(into=[0,1]) crank 1 Use (1) or not (0, default) a Crank Nicholson method for the diffusion implicitation algorithm. Setting crank to 1 increases the order of the algorithm from 1 to 2.
// XD attr preconditionnement_diag entier(into=[0,1]) preconditionnement_diag 1 The CG used to solve the implicitation of the equation diffusion operator is not preconditioned by default. If this option is set to 1, a diagonal preconditionning is used. Warning: this option is not necessarily more efficient, depending on the treated case.
// XD attr niter_max_diffusion_implicite entier niter_max_diffusion_implicite 1 Change the maximum number of iterations for the CG (Conjugate Gradient) algorithm when solving the diffusion implicitation of the equation.
// XD attr seuil_diffusion_implicite floattant seuil_diffusion_implicite 1 Change the threshold convergence value used by default for the CG resolution for the diffusion implicitation of this equation.
// XD attr solveur solveur_sys_base solveur 1 Method (different from the default one, Conjugate Gradient) to solve the linear system.
Entree& Parametre_diffusion_implicite::readOn(Entree& is)
{
  Param param(que_suis_je());
  param.ajouter("crank",&crank_);
  param.ajouter("niter_max_diffusion_implicite",&nb_it_max_);
  param.ajouter("preconditionnement_diag",&preconditionnement_diag_);
  param.ajouter("seuil_diffusion_implicite",&seuil_diffusion_implicite_);
  param.ajouter_non_std("solveur",(this));
  param.lire_avec_accolades_depuis(is);
  return is;
}

int Parametre_diffusion_implicite::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Motcle motlu;
  if (mot=="solveur")
    {
      is >> solveur_;
      solveur_.nommer("solveur_diffusion_implicite");
    }
  else
    {
      Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
      exit();
    }
  return -1;
}
