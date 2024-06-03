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

#ifndef Precond_base_included
#define Precond_base_included

#include <TRUSTTabs_forward.h>
#include <Objet_U.h>

class Matrice_Base;
class Champ_Inc_base;

class Precond_base : public Objet_U
{
  Declare_base_sans_constructeur(Precond_base);
public:
  // Order is important (we test for example if status_>=REINIT_COEFF)
  enum Init_Status { READY = 0, REINIT_COEFF = 1, REINIT_ALL = 2 };

  Precond_base();

  void   reinit(Init_Status x = REINIT_ALL);
  void   set_echange_espace_virtuel_resultat(int flag);
  int preconditionner(const Matrice_Base&, const DoubleVect& src, DoubleVect& solution);
  void   prepare(const Matrice_Base&, const DoubleVect& src);

  virtual int supporte_matrice_morse_sym();
  Init_Status get_status() { return status_; }
  // Call this to know if "src" must have an updated virtual space before calling preconditionner
  virtual int get_flag_updated_input() const { return 1; }

protected:
  // implementations of preconditionner_ must take care of echange_ev_solution_ and update
  //  the virtual space if needed.
  virtual int preconditionner_(const Matrice_Base&, const DoubleVect& src, DoubleVect& solution) = 0;
  virtual void   prepare_(const Matrice_Base&, const DoubleVect&);

  // Should the preconditionner_ method update the virtual space of the result ?
  int echange_ev_solution_;

private:
  Init_Status status_;
};

#endif
