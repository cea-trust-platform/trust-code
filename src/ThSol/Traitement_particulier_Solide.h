/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Traitement_particulier_Solide.h
// Directory:   $TRUST_ROOT/src/ThSol
// Version:     /main/9
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Traitement_particulier_Solide_included
#define Traitement_particulier_Solide_included

#include <Traitement_particulier_Solide_base.h>
#include <Deriv.h>

Declare_deriv(Traitement_particulier_Solide_base);
//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Traitement_particulier_Solide
//
// .SECTION voir aussi
//      Conduction, Traitement_particulier_Solide_base
//////////////////////////////////////////////////////////////////////////////
class Traitement_particulier_Solide : public DERIV(Traitement_particulier_Solide_base)
{
  Declare_instanciable(Traitement_particulier_Solide);

public :

  inline void associer_eqn(const Equation_base& );
  inline void preparer_calcul_particulier(void) ;
  inline void post_traitement_particulier(void);
  inline Entree& lire(Entree&);
  inline void en_cours_de_resolution(int , DoubleTab& , DoubleTab& ,double);

protected :

};

inline void Traitement_particulier_Solide::associer_eqn(const Equation_base& eqn)
{
  valeur().associer_eqn(eqn);
}

inline void Traitement_particulier_Solide::preparer_calcul_particulier(void)
{
  valeur().preparer_calcul_particulier();
}

inline void Traitement_particulier_Solide::post_traitement_particulier(void)
{
  valeur().post_traitement_particulier();
}

inline Entree& Traitement_particulier_Solide::lire(Entree& is)
{
  return valeur().lire(is);
}


inline void Traitement_particulier_Solide::en_cours_de_resolution(int nb_op, DoubleTab& u, DoubleTab& u_av, double dt)
{
  valeur().en_cours_de_resolution(nb_op,u,u_av,dt);
}


#endif
