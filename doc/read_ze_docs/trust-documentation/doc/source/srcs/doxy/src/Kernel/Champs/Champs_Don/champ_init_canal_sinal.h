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

#ifndef champ_init_canal_sinal_included
#define champ_init_canal_sinal_included

#include <TRUSTChamp_Divers_generique.h>

class champ_init_canal_sinal : public TRUSTChamp_Divers_generique<Champ_Divers_Type::CANAL>
{
  Declare_instanciable(champ_init_canal_sinal);
public :
  DoubleVect& valeur_a(const DoubleVect& position, DoubleVect& valeurs) const override;
  double valeur_a_elem_compo(const DoubleVect& position, int le_poly, int ncomp) const override;
  DoubleTab& valeur_aux(const DoubleTab& positions, DoubleTab& valeurs) const override;
  DoubleVect& valeur_aux_compo(const DoubleTab& positions, DoubleVect& valeurs, int ncomp) const override;

protected :
  double Ucent = -1., h = -1., ampli_bruit = -1., omega = -1., ampli_sin = -1.;
  // Initialisation par defaut des minimums et maximums pour generaliser l'initialisation du canal pour n'importe quelle position dans l'espace.
  int dir_wall = 1, dir_flow = 0;
  double min_dir_flow = 0., min_dir_wall = 0.;

  inline double fx(double x) const { return 0.; }

  inline double fx(double x, double y) const
  {
    return Ucent / (h * h) * (y - min_dir_wall) * (2.0 * h - (y - min_dir_wall));
  }

  inline double fy(double x, double y) const
  {
    return ampli_bruit * (-0.5 + drand48()) + ampli_sin * sin(omega * (x - min_dir_flow));
  }

  inline double fx(double x, double y, double z) const
  {
    return Ucent / (h * h) * (y - min_dir_wall) * (2.0 * h - (y - min_dir_wall));
  }

  inline double fy(double x, double y, double z) const
  {
    return ampli_bruit * (-0.5 + drand48()) + ampli_sin * sin(omega * (x - min_dir_flow));
  }

  inline double fz(double x, double y, double z) const
  {
    return ampli_bruit * (-0.5 + drand48());
  }
};

#endif /* champ_init_canal_sinal_included */
