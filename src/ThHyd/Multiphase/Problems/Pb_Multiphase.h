/****************************************************************************
* Copyright (c) 2024, CEA
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

#ifndef Pb_Multiphase_included
#define Pb_Multiphase_included

#include <Energie_Multiphase.h>
#include <Masse_Multiphase.h>
#include <Correlation_base.h>
#include <Pb_Fluide_base.h>
#include <QDM_Multiphase.h>
#include <TRUST_Deriv.h>
#include <TRUST_List.h>
#include <Interprete.h>
#include <Verif_Cl.h>

/*! @brief classe Pb_Multiphase Cette classe represente un probleme de thermohydraulique multiphase de type "3*N equations" :
 *
 *       - QDM_Multiphase : equations de Navier-Stokes et de continuite pour chaque phase
 *                          inconnues : v_k (vitesses par phase)
 *                          champs auxiliaires : pression p, taux de presence a_k (sum a_k = 1)
 *       - Energie_Multiphase : equations de conservation de l'energie pour chaque phase
 *                              inconnues : T_k (temperatures)
 *     schema de resolution typique :
 *       1) QDM_Multiphase -> prediction des vitesses
 *                            conservation de la masse -> Newton (etape "semi-implicite")
 *       3) Energie_Multiphase -> advection/dffusion implicite de l'energie
 *
 *
 * @sa Pb_Fluide_base QDM_Multiphase Masse_Multiphase Energie_Multiphase
 */

class Pb_Multiphase : public Pb_Fluide_base
{
  Declare_instanciable(Pb_Multiphase);
public:
  void typer_lire_milieu(Entree& is) override;
  int nombre_d_equations() const override;
  const Equation_base& equation(int) const override ;
  Equation_base& equation(int) override;
  void associer_milieu_base(const Milieu_base& ) override;
  Entree& lire_equations(Entree& is, Motcle& dernier_mot) override;
  int verifier() override;
  void preparer_calcul() override;

  /* nombre de phases du probleme */
  int nb_phases() const { return noms_phases_.size(); }
  const Nom& nom_phase(int i) const { return noms_phases_[i]; }
  const Noms& noms_phases() const { return noms_phases_; }

  double calculer_pas_de_temps() const override;

  virtual void typer_lire_correlation_hem() { /* Do nothing */}

  virtual Equation_base& equation_qdm() { return eq_qdm_; }
  virtual const Equation_base& equation_qdm() const { return eq_qdm_; }
  virtual Equation_base& equation_masse() { return eq_masse_; }
  virtual const Equation_base& equation_masse() const { return eq_masse_; }
  virtual Equation_base& equation_energie() { return eq_energie_; }
  virtual const Equation_base& equation_energie() const { return eq_energie_; }

  inline virtual bool resolution_en_T() const { return true; }

protected:
  Noms noms_phases_;
  // equations
  QDM_Multiphase eq_qdm_;
  Energie_Multiphase eq_energie_;
  Masse_Multiphase eq_masse_;
  LIST(OWN_PTR(Equation_base)) eq_opt_; //autres equations (turbulence, aire interfaciale...)
};

#endif /* Pb_Multiphase_included */
