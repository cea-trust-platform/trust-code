/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Loi_horaire.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Loi_horaire_included
#define Loi_horaire_included

#include <Champ_Fonc_t.h>
class ArrOfDouble;
class DoubleTab;
class Schema_Temps_base;

////////////////////////////////////////////////////////////////////////////////
// .DESCRIPTION
// .SECTION voir aussi
//////////////////////////////////////////////////////////////////////////////
class Loi_horaire : public Objet_U
{

  Declare_instanciable(Loi_horaire);

public:
  ArrOfDouble position(const double& t, const double& t0, const ArrOfDouble& position_a_t0);
  ArrOfDouble vitesse(const double& t, const ArrOfDouble& position_a_t);
  void imprimer(const Schema_Temps_base&, const ArrOfDouble&);
  void nommer(const Nom& un_nom)
  {
    nom_ = un_nom;
  };
  const Nom& le_nom() const
  {
    return nom_;
  };
private:
  void verifier_derivee(const double& t);
  void tester(const Schema_Temps_base&);

  Champ_Fonc_t position_;
  Champ_Fonc_t vitesse_;
  Champ_Fonc_t rotation_;
  Champ_Fonc_t derivee_rotation_;
  int verification_derivee_;
  int impr_;
  Nom nom_;
};

#endif

