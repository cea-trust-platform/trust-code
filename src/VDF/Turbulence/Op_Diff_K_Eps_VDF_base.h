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
// File:        Op_Diff_K_Eps_VDF_base.h
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/13
//
//////////////////////////////////////////////////////////////////////////////



#ifndef Op_Diff_K_Eps_VDF_base_included
#define Op_Diff_K_Eps_VDF_base_included

#include <Op_Diff_K_Eps_base.h>
#include <ItVDFEl.h>
#include <Eval_Diff_K_Eps_VDF_Elem.h>
#include <Op_VDF_Elem.h>

class Zone_dis;
class Zone_Cl_dis;
class Champ_Inc;

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_K_Eps_VDF_base
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_K_Eps_VDF_base : public Op_Diff_K_Eps_base, public Op_VDF_Elem
{

  Declare_base(Op_Diff_K_Eps_VDF_base);

public:

  inline Op_Diff_K_Eps_VDF_base(const Iterateur_VDF_base& );
  inline DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  inline DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  inline void contribuer_au_second_membre(DoubleTab& ) const;
  virtual void mettre_a_jour_diffusivite() const;
  void completer();
  void associer_diffusivite(const Champ_base& ch_diff);
  void associer_diffusivite_turbulente();
  // void associer_diffusivite(const Champ_Don& );
  // void associer_mvolumique();
  inline  void dimensionner(Matrice_Morse& ) const;
  void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  const Champ_Fonc& diffusivite_turbulente() const;
  const Champ_base& diffusivite() const ;

protected:

  //Champ_Fonc diff_tot;
  Iterateur_VDF iter;

};

declare_It_VDF_Elem(Eval_Diff_K_Eps_VDF_Elem)

//////////////////////////////////////////////////////////////////////////////
//
// CLASS: Op_Diff_K_Eps_VDF_Elem
//
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_K_Eps_VDF_Elem : public Op_Diff_K_Eps_VDF_base
{

  Declare_instanciable_sans_constructeur(Op_Diff_K_Eps_VDF_Elem);

public:

  Op_Diff_K_Eps_VDF_Elem();
  void associer(const Zone_dis& , const Zone_Cl_dis& ,
                const Champ_Inc& );
};


//
// Fonctions inline de la classe Op_Diff_K_Eps_VDF_base
//

// Description:
// constructeur
inline Op_Diff_K_Eps_VDF_base::Op_Diff_K_Eps_VDF_base(const Iterateur_VDF_base& iter_base) :
  iter(iter_base)
{}


// Description:
// ajoute la contribution de la diffusion au second membre resu
// renvoie resu
inline DoubleTab& Op_Diff_K_Eps_VDF_base::ajouter(const DoubleTab& inco,  DoubleTab& resu) const
{
  mettre_a_jour_diffusivite();
  return iter.ajouter(inco, resu);
}

//Description:
//on assemble la matrice.

inline void Op_Diff_K_Eps_VDF_base::contribuer_a_avec(const DoubleTab& inco,
                                                      Matrice_Morse& matrice) const
{
  Cerr << "dans Op_Diff_K_Eps_VDF_base::contribuer_a_avec" << finl;
  iter.ajouter_contribution(inco, matrice);
}

//Description:
//on ajoute la contribution du second membre.

inline void Op_Diff_K_Eps_VDF_base::contribuer_au_second_membre(DoubleTab& resu) const
{
  Cerr << "dans Op_Diff_K_Eps_VDF_base::contribuer_au_second_membre "<< finl;
  iter.contribuer_au_second_membre(resu);
}


// Description:
// calcule la contribution de la diffusion, la range dans resu
// renvoie resu
inline DoubleTab& Op_Diff_K_Eps_VDF_base::calculer(const DoubleTab& inco,  DoubleTab& resu) const
{
  mettre_a_jour_diffusivite();
  return iter.calculer(inco, resu);
}

// Description:
// on dimensionne notre matrice.
inline  void Op_Diff_K_Eps_VDF_base::dimensionner(Matrice_Morse& matrice) const
{
  Op_VDF_Elem::dimensionner(iter.zone(), iter.zone_Cl(), matrice);
}




#endif
