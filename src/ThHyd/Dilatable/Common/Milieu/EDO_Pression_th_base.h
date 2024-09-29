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

#ifndef EDO_Pression_th_base_included
#define EDO_Pression_th_base_included

#include <Fluide_Dilatable_base.h>
#include <TRUSTTabs_forward.h>
#include <Domaine_Cl_dis.h>
#include <Domaine_dis.h>
#include <TRUST_Ref.h>

class Domaine_VF;

/*! @brief classe EDO_Pression_th_base Cette classe est la base de la hierarchie des EDO sur la pression
 *
 *      associees a la resolution du schema pour les fluides dilatables
 *
 * @sa Fluide_Dilatable_base, Classe abstraite dont toutes les lois d'etat doivent deriver., Methodes abstraites:, void calculer_coeff_T(), void Resoudre_EDO_PT(), void calculer_masse_volumique()
 */

class EDO_Pression_th_base : public Objet_U
{
  Declare_base(EDO_Pression_th_base);
public :
  void associer_fluide(const Fluide_Dilatable_base&);
  void associer_domaines(const Domaine_dis&,const Domaine_Cl_dis&);
  const Fluide_Dilatable_base& le_fluide() const { return le_fluide_.valeur(); };
  inline const double& getM0() const { return M0; }
  virtual void completer();
  virtual void mettre_a_jour_CL(double);

  // Virtuelles pure
  virtual double masse_totale(double P,const DoubleTab& T)=0;
  virtual double resoudre(double) =0;

protected :
  REF(Fluide_Dilatable_base) le_fluide_;
  REF(Domaine_Cl_dis) le_dom_Cl;
  REF(Domaine_VF) le_dom;
  double M0 = -1.;// la masse totale initiale
};

#endif /* EDO_Pression_th_base_included */
