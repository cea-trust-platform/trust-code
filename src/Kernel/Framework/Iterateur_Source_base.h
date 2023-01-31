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

#ifndef Iterateur_Source_base_included
#define Iterateur_Source_base_included

#include <Ref_Domaine_Cl_dis_base.h>
#include <Ref_Domaine_dis_base.h>
#include <TRUSTTabs_forward.h>
#include <Domaine_Cl_dis_base.h>
#include <Ref_Source_base.h>
#include <Equation_base.h>

class Evaluateur_Source;

class Iterateur_Source_base : public Objet_U
{
  Declare_base(Iterateur_Source_base);
public:
  virtual DoubleTab& ajouter(DoubleTab& ) const = 0;
  virtual Evaluateur_Source& evaluateur() = 0;
  virtual void completer_() = 0;

  virtual DoubleTab& calculer(DoubleTab& resu) const
  {
    resu = 0.;
    return ajouter(resu);
  }

  virtual int initialiser(double )
  {
    Process::exit("Iterateur_Source_base::initialiser must be overloaded !!");
    return 0;
  }

  inline void associer(const Source_base& source) { so_base = source; }

  inline int equation_divisee_par_rho() const
  {
    Nom nom_eqn = la_zcl->equation().que_suis_je();
    return (nom_eqn.debute_par("Navier_Stokes")) ? 1 : 0;
  }

  inline void associer_domaines(const Domaine_dis_base& domaine, const Domaine_Cl_dis_base& domaine_cl)
  {
    le_dom = domaine;
    la_zcl = domaine_cl;
  }

  virtual int impr(Sortie&) const
  {
    Process::exit("Iterateur_Source_base::impr must be overloaded !!");
    return 0;
  }

protected:
  REF(Domaine_Cl_dis_base) la_zcl;
  REF(Domaine_dis_base) le_dom;
  REF(Source_base) so_base;
};

#endif /* Iterateur_Source_base_included */
