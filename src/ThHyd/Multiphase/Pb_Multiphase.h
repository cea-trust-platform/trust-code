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
// File:        Pb_Multiphase.h
// Directory:   $TRUST_ROOT/src/ThHyd/Multiphase
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Pb_Multiphase_included
#define Pb_Multiphase_included

#include <Interprete.h>
#include <Pb_Fluide_base.h>
#include <QDM_Multiphase.h>
#include <Masse_Multiphase.h>
#include <Energie_Multiphase.h>
#include <Verif_Cl.h>
#include <Correlation.h>
#include <List_Equation.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Pb_Multiphase
//     Cette classe represente un probleme de thermohydraulique multiphase de type "3*N equations" :
//      - QDM_Multiphase : equations de Navier-Stokes et de continuite pour chaque phase
//                         inconnues : v_k (vitesses par phase)
//                         champs auxiliaires : pression p, taux de presence a_k (sum a_k = 1)
//      - Energie_Multiphase : equations de conservation de l'energie pour chaque phase
//                             inconnues : T_k (temperatures)
//    schema de resolution typique :
//      1) QDM_Multiphase -> prediction des vitesses
//                           conservation de la masse -> Newton (etape "semi-implicite")
//      3) Energie_Multiphase -> advection/dffusion implicite de l'energie
//
// .SECTION voir aussi
//     Pb_Fluide_base QDM_Multiphase Masse_Multiphase Energie_Multiphase
//////////////////////////////////////////////////////////////////////////////
class Pb_Multiphase : public Pb_Fluide_base
{

  Declare_instanciable(Pb_Multiphase);

public:
  void discretiser(const Discretisation_base&) override;
  int nombre_d_equations() const override;
  const Equation_base& equation(int) const override ;
  Equation_base& equation(int) override;
  void associer_milieu_base(const Milieu_base& ) override;
  void creer_milieu(const Noms);
  virtual Entree& lire_equations(Entree& is, Motcle& dernier_mot) override;
  int verifier() override;
  virtual Entree& lire_correlations(Entree& is);
  void preparer_calcul() override;

  /* nombre de phases du probleme */
  int nb_phases() const { return noms_phases_.size(); }
  const Nom& nom_phase(int i) const { return noms_phases_[i]; }
  const Noms& noms_phases() const { return noms_phases_; }
  void set_noms_phases(Noms& noms) { noms_phases_ = noms; }

  QDM_Multiphase eq_qdm;
  Energie_Multiphase eq_energie;
  Masse_Multiphase eq_masse;
  LIST(Equation) eq_opt; //autres equations (turbulence, aire interfaciale...)

  const Correlation& get_correlation(std::string nom_correlation) const
  {
    Motcle mot(nom_correlation);
    return correlations.at(mot.getString());
  }

  int has_correlation(std::string nom_correlation) const
  {
    Motcle mot(nom_correlation);
    return correlations.count(mot.getString());
  }

protected:

  Noms noms_phases_;
  std::map<std::string, Correlation> correlations;
};



#endif
