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

#ifndef Solveur_Implicite_Base_included
#define Solveur_Implicite_Base_included

#include <List_Ref_Equation_base.h>
#include <TRUSTTabs_forward.h>
#include <Objet_U.h>
#include <Double.h>
#include <Parametre_implicite.h>

class Equation_base;

class Solveur_Implicite_Base : public  Objet_U
{
  Declare_base(Solveur_Implicite_Base);

public :
  virtual bool iterer_eqn(Equation_base& equation, const DoubleTab& inconnue, DoubleTab& result, double dt, int numero_iteration, int& ok) =0;
  virtual bool iterer_eqs(LIST(REF(Equation_base)) eqs, int n, int& ok);
  virtual bool est_compatible_avec_th_mono() const /* ce solveur est-il  compatible avec une resolution monolithique de la thermique ? */
  {
    return 1; /* par defaut oui */
  }
  virtual int nb_valeurs_temporelles_pression() const /* nombre de valeurs temporelles des champs de pression dont le solveur a besoin */
  {
    return 1; /* par defaut 1 */
  }
  virtual double get_default_facsec_max() const /* facsec_max recommande pour ce schema */
  {
    return DMAXFLOAT; /* par defaut pas de limite : on est en implicite */
  }
  virtual double get_default_growth_factor() const /* taux de croissance du pas de temps */
  {
    return DMAXFLOAT; /* par defaut pas de limite : on est en implicite */
  }
  virtual Parametre_equation& get_and_set_parametre_equation(Equation_base& eqn) /* initialisation de parametre_equation() dans une equation */
  {
    return eqn.parametre_equation(); /* par defaut : ne fait rien */
  }
};

#endif
