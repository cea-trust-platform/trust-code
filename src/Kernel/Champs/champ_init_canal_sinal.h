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
// File:        champ_init_canal_sinal.h
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef champ_init_canal_sinal_included
#define champ_init_canal_sinal_included

#include <Champ_Don_base.h>
#include <Motcle.h>

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: champ_init_canal_sinal
//
//////////////////////////////////////////////////////////////////////////////

class champ_init_canal_sinal : public Champ_Don_base
{
  Declare_instanciable(champ_init_canal_sinal);
public :
  //    champ_init_canal_sinal();
  Champ_base& affecter(const Champ_base& ch);
  DoubleVect& valeur_a(const DoubleVect& position,
                       DoubleVect& valeurs) const override;
  DoubleVect& valeur_a_elem(const DoubleVect& position,
                            DoubleVect& valeurs,
                            int poly) const override ;
  double valeur_a_elem_compo(const DoubleVect& position,
                             int le_poly,int ncomp) const override;

  DoubleTab& valeur_aux(const DoubleTab& positions,
                        DoubleTab& valeurs) const override;
  DoubleVect& valeur_aux_compo(const DoubleTab& positions,
                               DoubleVect& valeurs, int ncomp) const override;

  DoubleTab& valeur_aux_elems(const DoubleTab& positions,
                              const IntVect& les_polys,
                              DoubleTab& valeurs) const override ;
  DoubleVect& valeur_aux_elems_compo(const DoubleTab& positions,
                                     const IntVect& les_polys,
                                     DoubleVect& valeurs,
                                     int ncomp) const override;
protected :
  double Ucent, h, ampli_bruit, omega, ampli_sin;
  int dir_wall, dir_flow;
  double min_dir_flow, min_dir_wall;

  inline double fx(double x) const;
  inline double fx(double x, double y) const;
  inline double fy(double x, double y) const;
  inline double fx(double x, double y, double z) const;
  inline double fy(double x, double y, double z) const;
  inline double fz(double x, double y, double z) const;
};

//
// Methodes inline :
//

// Description:
//
// 3
inline double champ_init_canal_sinal::fx(double x) const
{
  // A CODER return f(x) si champ_init_canal_sinal a un sens en 1D;
  return 0;
}

// Description:
//
inline double champ_init_canal_sinal::fx(double x, double y) const
{
  return Ucent/(h*h)*(y-min_dir_wall)*(2.0*h-(y-min_dir_wall));
}

// Description:
//
inline double champ_init_canal_sinal::fy(double x, double y) const
{
  return  ampli_bruit*(-0.5+drand48())+ampli_sin*sin(omega*(x-min_dir_flow));
}

// Description:
//
inline double champ_init_canal_sinal::fx(double x, double y, double z) const
{
  return Ucent/(h*h)*(y-min_dir_wall)*(2.0*h-(y-min_dir_wall));
}

// Description:
//
inline double champ_init_canal_sinal::fy(double x, double y, double z) const
{
  return ampli_bruit*(-0.5+drand48())+ampli_sin*sin(omega*(x-min_dir_flow));
}

// Description:
//
inline double champ_init_canal_sinal::fz(double x, double y, double z) const
{
  return ampli_bruit*(-0.5+drand48());
}

#endif
