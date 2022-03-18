/****************************************************************************
* Copyright (c) 2022, CEA
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
// File:        Sch_CN_iteratif.h
// Directory:   $TRUST_ROOT/src/ThHyd/Schemas_Temps
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Sch_CN_iteratif_included
#define Sch_CN_iteratif_included

#include <Schema_Temps_base.h>
#include <TRUSTTabs_forward.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Sch_CN_iteratif
//     Schema en temps alternant un demi-pas de temps d'Euler implicite et un demi-pas de temps de LeapFrog.
//     La resolution implicite est iterative (point fixe).
//     Le pas de temps est calcule comme le produit du pas de temps de stabilite explicite par un facsec.
//     Le facsec est ajuste automatiquement pour que la resolution converge en un nombre d'iterations predefini.
//     Les caracteristiques de chaque iteration sont ecrites dans le fichier dt_CN.
//
//     La resolution est gouvernee par 4 parametres (valeurs par defauts entre parentheses) :
//     * seuil (1e-3) : le seuil de convergence. Plus il est bas, plus la resolution est precise.
//     * facsec_max (2) : la valeur du facsec que l'on ne veut pas depasser (eviter les instabilites et capter les phenomenes physiques)
//     * niter_min (2) : le nombre minimum d'iterations. En deca, on continue a iterer meme si on semble avoir atteint la convergence.
//     * niter_avg (3) : le nombre d'iterations que l'on souhaite faire pour arriver a convergence.
//     * niter_max (6) : le nombre d'iteration au-dela duquel on reessaye avec un facsec plus petit.
//
//     Conseil pour le choix des parametres d'ajustement du facsec :
//     * Choisir seuil en fonction de la precision desiree.
//     * Choisir niter_min : 2 garantit un schema d'ordre 2 en temps.
//     * Si on cherche un stationnaire, choisir seuil_statio >= seuil.
//     * Choisir facsec_max en fonction des phenomenes physiques a capter.
//     * Commencer par tester avec une grande valeur de niter_avg. Observer le comportement du nombre d'iterations.
//       Il bute sur une valeur maximum avant de retomber.
//     * Choisir niter_avg aux 2/3 de cette valeur maximum, et niter_max aux 4/3 ou au double environ.
//
// .SECTION voir aussi
//     Schema_Temps_base
//////////////////////////////////////////////////////////////////////////////
class Sch_CN_iteratif : public Schema_Temps_base
{
  Declare_instanciable(Sch_CN_iteratif);

public :

  ////////////////////////////////
  //                            //
  // Caracteristiques du schema //
  //                            //
  ////////////////////////////////

  int nb_valeurs_temporelles() const override;
  int nb_valeurs_futures() const override;
  double temps_futur(int i) const override;
  double temps_defaut() const override;

  /////////////////////////////////////////
  //                                     //
  // Fin des caracteristiques du schema  //
  //                                     //
  /////////////////////////////////////////

  bool initTimeStep(double dt) override;
  bool iterateTimeStep(bool& converged) override;

  int faire_un_pas_de_temps_eqn_base(Equation_base&) override;

  void completer() override {}
  void set_param(Param& titi) override;

protected :

  enum type_convergence {DIVERGENCE, NON_CONVERGENCE, CONVERGENCE_LENTE, CONVERGENCE_RAPIDE, CONVERGENCE_OK};

  virtual bool convergence(const DoubleTab& u0, const DoubleTab& up1, const DoubleTab& delta, int p) const;
  virtual bool divergence(const DoubleTab& u0, const DoubleTab& up1, const DoubleTab& delta, int p) const;
  virtual void ajuster_facsec(type_convergence cv);

  virtual bool iterateTimeStepOnEquation(int i,bool& converged);

  double seuil; // To determine convergence
  int niter_min; // Minimum number of iterations (before, continue to iterate)
  int niter_max; // Maximum number of iterations (after, considered as not convergent)
  int niter_avg; // Average number of iterations wanted (facsec adjusted to fit that number)
  double facsec_max; // Maximum facsec (not to miss the physics)

  // Used internally.
  int iteration; // Number of iterations done for the current time step.
  double last_facsec; // facsec at the beginning of time step resolution,
  // to avoid changing facsec several times for the same time step.
};

#endif
