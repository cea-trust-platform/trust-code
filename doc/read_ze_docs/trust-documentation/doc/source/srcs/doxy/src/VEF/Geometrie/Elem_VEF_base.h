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

#ifndef Elem_VEF_base_included
#define Elem_VEF_base_included

#include <TRUSTTab.h>
#include <Objet_U.h>

class Domaine;
class Domaine_VEF;
class Champ_Inc_base;

class Elem_VEF_base: public Objet_U
{
  Declare_base(Elem_VEF_base);
public:
  virtual int nb_facette() const =0;
  inline const IntTab& KEL() const { return KEL_; }
  virtual void creer_facette_normales(const Domaine&, DoubleTab&, const IntVect&) const =0;
  virtual void creer_normales_facettes_Cl(DoubleTab&, int, int, const DoubleTab&, const DoubleVect&, const Domaine&) const =0;
  virtual void modif_volumes_entrelaces(int, int, const Domaine_VEF&, DoubleVect&, int) const =0;
  virtual void modif_volumes_entrelaces_faces_joints(int, int, const Domaine_VEF&, DoubleVect&, int) const =0;
  virtual void modif_normales_facettes_Cl(DoubleTab&, int, int, int, int, int, int) const =0;
  virtual void calcul_vc(const ArrOfInt&, ArrOfDouble&, const ArrOfDouble&, const DoubleTab&, const Champ_Inc_base&, int, const DoubleVect&) const =0;
  virtual void calcul_xg(DoubleVect&, const DoubleTab&, const int, int&, int&, int&, int&) const =0;
  virtual void normale(int, DoubleTab&, const IntTab&, const IntTab&, const IntTab&, const Domaine&) const =0;
protected:
  IntTab KEL_;
};

#endif
