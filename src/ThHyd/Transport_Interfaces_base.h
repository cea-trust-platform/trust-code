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
// File:        Transport_Interfaces_base.h
// Directory:   $TRUST_ROOT/src/ThHyd
// Version:     /main/8
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Transport_Interfaces_base_included
#define Transport_Interfaces_base_included

#include <Equation_base.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Transport_Interfaces_base
//     Cette classe constitue la classe de base des equations de transport d interfaces
//     Actuellement une seule classe instanciable derivant : Transport_Interfaces_FT_Disc
//
// .SECTION voir aussi
//     Transport_Interfaces_FT_Disc
//     Methodes abstraites:
//     void modifier_vpoint_pour_imposer_vit(...)
//     void integrer_ensemble_lagrange(...)
//////////////////////////////////////////////////////////////////////////////

class Transport_Interfaces_base : public Equation_base
{
  Declare_base(Transport_Interfaces_base);

public:

  //La presence d'une interface solide dans un ecoulement s accompagne d un terme source
  //dans l equation de quantite de mouvement qui ne peut etre considere comme un terme
  //source classique mais plutot comme une modification de vpoint pour imposer au fluide
  //la vitesse de l interface

  //Effectue la modification de vpoint (equation de quantite de mouvement)
  //pour imposer au fluide la vitesse de l interface
  virtual void modifier_vpoint_pour_imposer_vit(const DoubleTab& inco_val,DoubleTab& vpoint0,DoubleTab& vpoint,
                                                const DoubleTab& rho_faces,DoubleTab& source_val,
                                                const double temps, const double dt,
                                                const int is_explicite = 1, const double eta = 1.) = 0;
  // Recupere le tag du maillage de l'interface
  virtual int get_mesh_tag() const = 0;
  // Recupere le champ de l'indicatrice
  virtual const Champ_base& get_update_indicatrice() =0;


  //Effectue l integration de trajectoire de particules ponctuelles
  //destinees a marquer le fluide
  //xn+1 = xn + v_interpolee*dt
  virtual void integrer_ensemble_lagrange(const double temps) = 0;

protected:

  // A terme d autres elements de Transport_Interfaces_FT_Disc sont susceptibles d etre factorises
  // ex : REF(Probleme_base) probleme_base_;
  // Cette factorisation pourra etre realisee quand les developpements recents
  // lies purement au Front-Tracking seront integres

private:

};

#endif
