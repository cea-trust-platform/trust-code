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

#ifndef Ch_front_Vortex_included
#define Ch_front_Vortex_included

#include <Champ_front_var_instationnaire.h>
#include <TRUST_Ref.h>

class Domaine;

/*! @brief classe Ch_fr_Vortex Classe derivee de Champ_front_var qui represente les
 *
 *      champs aux frontieres calcules suivant la methode des vortex
 *         nval
 *         xi yi zi val_i
 *
 * @sa Champ_front_base Champ_front_var
 */
class Ch_front_Vortex : public Champ_front_var_instationnaire
{
  Declare_instanciable_sans_destructeur(Ch_front_Vortex);

public:

  ~Ch_front_Vortex() override;
  Champ_front_base& affecter_(const Champ_front_base& ch) override;
  int my_rand();
  void sauvegarder_vortex();
  void reprendre_vortex();
  int initialiser(double temps, const Champ_Inc_base& inco) override;
  void deplacement_vortex(double ,double ,double ,double ,double ,double& ,double& );
  void mettre_a_jour(double ) override;

protected :

  int init = 1;
  int first_rand = 1;
  REF(Domaine) mon_domaine;
  Nom geom;
  double nu = -100., utau = -100.;
  double R = -100., Ox = -100., Oy = -100., Oz = -100., surf = -100.;
  int nb_vortex = -1;
  double temps = -100.;

  double nx = -100., ny = -100., nz = -100.;         // composantes normales du repere associe a la frontiere
  double t1x = -100., t1y = -100., t1z = -100.; // composantes tangentielles : n.t1 = 0
  double t2x = -100., t2y = -100., t2z = -100.; // composantes tangentielles : t2 = n ^ t1 = 0

  DoubleVect xvort;  // coordonnees x des vortex
  DoubleVect yvort;  // coordonnees y des vortex
  DoubleVect zvort;  // coordonnees z des vortex
  DoubleVect tvort;  // durees de vie des vortex
  DoubleVect svort;  // signes de la vorticite des vortex
  ArrOfInt fvort;    // numero faces contenant le centre du vortex
  DoubleVect gamma;  // intensite du vortex
  DoubleVect sigma;  // longeur caracteristique du vortex

  DoubleVect u, v, w, u_moy, dudy, k, eps;

  DoubleVect Wk; // processus de Wiener
};

#endif

