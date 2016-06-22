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
// File:        Op_Diff_Fluctu_Temp_Base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/10
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Diff_Fluctu_Temp_Base_included
#define Op_Diff_Fluctu_Temp_Base_included

#include <Operateur_Diff_base.h>
#include <Operateur.h>
#include <Operateur_negligeable.h>
#include <Matrice_Morse.h>

class Champ_Fonc;
class Champ_Don;
class DoubleTab;

//.DESCRIPTION class Op_Diff_Fluctu_Temp_Base
// Sert a modeliser le terme diffusif dans l'equation de transport
// des fluctuations thermiques
// On traite les deux equations de transport en une seule
// equation vectorielle.
// La viscosite_turbulente est a diviser par la constante Prdt_teta
// pour la diffusion de teta^2 et par la constante Prdt_epsteta pour la
// diffusion de Epsteta


class Op_Diff_Fluctu_Temp_Base : public Operateur_base
{
  Declare_base(Op_Diff_Fluctu_Temp_Base);

public:
  virtual void associer_diffusivite_turbulente() =0;

};




class   Op_Diff_Fluctu_Temp_negligeable : public Operateur_negligeable, public Op_Diff_Fluctu_Temp_Base

{
  Declare_instanciable(Op_Diff_Fluctu_Temp_negligeable);

public:

  inline void associer(const Zone_dis&, const Zone_Cl_dis&, const Champ_Inc& );
  inline DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  inline DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  inline void contribuer_au_second_membre(DoubleTab& ) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  inline void dimensionner(Matrice_Morse& ) const;
  inline void associer_diffusivite_turbulente();
};


//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_Fluctu_Temp
//
//////////////////////////////////////////////////////////////////////////////

Declare_deriv(Op_Diff_Fluctu_Temp_Base);

class Op_Diff_Fluctu_Temp :  public Operateur,
  public DERIV(Op_Diff_Fluctu_Temp_Base)
{

  Declare_instanciable(Op_Diff_Fluctu_Temp);

public:

  inline Operateur_base& l_op_base();
  inline const Operateur_base& l_op_base() const;
  inline void associer_diffusivite_turbulente();
  inline DoubleTab& ajouter(const DoubleTab& , DoubleTab& ) const;
  inline DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  void typer();
  virtual inline int op_non_nul() const;


};

///////////////////////////////////////////////////////////////
//   Fonctions inline de la classe Op_Diff_Fluctu_Temp_negligeable
///////////////////////////////////////////////////////////////

inline void Op_Diff_Fluctu_Temp_negligeable::associer(const Zone_dis& zone_dis,
                                                      const Zone_Cl_dis& zone_cl_dis,
                                                      const Champ_Inc& inco)
{
  Operateur_negligeable::associer(zone_dis,zone_cl_dis,inco);
}

inline DoubleTab& Op_Diff_Fluctu_Temp_negligeable::ajouter(const DoubleTab& x,  DoubleTab& y) const
{
  return Operateur_negligeable::ajouter(x,y);
}

inline DoubleTab& Op_Diff_Fluctu_Temp_negligeable::calculer(const DoubleTab& x, DoubleTab& y) const
{
  return Operateur_negligeable::calculer(x,y);
}

//Description:
//on assemble la matrice.

inline void Op_Diff_Fluctu_Temp_negligeable::contribuer_a_avec(const DoubleTab& inco,
                                                               Matrice_Morse& matrice) const
{
  ;
}

//Description:
//on ajoute la contribution du second membre.

inline void Op_Diff_Fluctu_Temp_negligeable::contribuer_au_second_membre(DoubleTab& resu) const
{
  ;
}

// Modification des Cl
inline void  Op_Diff_Fluctu_Temp_negligeable::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& resu) const
{
  ;
}

inline void  Op_Diff_Fluctu_Temp_negligeable::dimensionner(Matrice_Morse& matrice) const
{
  ;
}

inline void Op_Diff_Fluctu_Temp_negligeable::associer_diffusivite_turbulente()
{
  ;
}

///////////////////////////////////////////////////
//  Fonctions inline de la classe Op_Diff_Fluctu_Temp
///////////////////////////////////////////////////


// Description:
inline Operateur_base& Op_Diff_Fluctu_Temp::l_op_base()
{
  if(!non_nul())
    Cerr << "Op_Diff_Fluctu_Temp n'a pas ete typer" << finl;
  return valeur();
}

// Description:
inline const Operateur_base& Op_Diff_Fluctu_Temp::l_op_base() const
{
  if(!non_nul())
    Cerr << "Op_Diff_Fluctu_Temp n'a pas ete typer" << finl;
  return valeur();
}

inline void Op_Diff_Fluctu_Temp::associer_diffusivite_turbulente()
{
  valeur().associer_diffusivite_turbulente();
}

inline DoubleTab& Op_Diff_Fluctu_Temp::ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  return valeur().ajouter(inconnue, resu);
}


inline DoubleTab& Op_Diff_Fluctu_Temp::calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  return valeur().calculer(inconnue, resu);
}

inline int Op_Diff_Fluctu_Temp::op_non_nul() const
{
  if (non_nul())
    return 1;
  else
    return 0;
}
#endif

