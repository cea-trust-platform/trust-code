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

#ifndef Operateur_Diff_base_included
#define Operateur_Diff_base_included

#include <Support_Champ_Masse_Volumique.h>
#include <Correlation_base.h>
#include <Operateur_base.h>
#include <TRUST_Ref.h>

class Champ_base;

/*! @brief classe Operateur_Diff_base Cette classe est la base de la hierarchie des operateurs representant
 *
 *     un terme de diffusion dans une equation. Le choix du terme depend
 *     de la modelisation laminaire ou turbulente de l'ecoulement, de la
 *     discretisation et du type du champ de diffusivite. Ces variantes
 *     donneront lieu a des classes filles de Operateur_Diff_base.
 *
 * @sa Operateur_base Operateur_Diff, Classe abstraite, Methode abstraite, void associer_diffusivite(const Champ_Don_base& ), const Champ_Don_base& diffusivite() const
 */
class Operateur_Diff_base  : public Operateur_base,
  public Support_Champ_Masse_Volumique
{
  Declare_base(Operateur_Diff_base);
public:
  virtual void associer_diffusivite(const Champ_base&) = 0;
  virtual void associer_diffusivite_pour_pas_de_temps(const Champ_base&);
  virtual const Champ_base& diffusivite() const=0;
  inline virtual void calculer_borne_locale(DoubleVect& ,double,double ) const {};

  //liste d'Op_Diff de problemes resolus simultanement (thermique monolithique)
  mutable std::vector<const Operateur_Diff_base *> op_ext;
  virtual void init_op_ext() const { op_ext = { this }; }    //remplissage de op_ext (ne peut pas etre fait dans completer(), trop tot)

  //set to 1 if operator is multiscalar (mixes components together). Only overload in operators where this is implemented!
  virtual void set_multiscalar(int);

  virtual bool is_turb() const { return false; }
  virtual const Correlation_base* correlation_viscosite_turbulente() const { return nullptr; }

protected:
  virtual const Champ_base& diffusivite_pour_pas_de_temps() const;
  int multiscalar_ = 0; //is the operator multiscalar?
  OBS_PTR(Champ_base) diffusivite_pour_pas_de_temps_;
};

#endif
