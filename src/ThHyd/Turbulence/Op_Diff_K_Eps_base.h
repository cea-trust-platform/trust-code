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
// File:        Op_Diff_K_Eps_base.h
// Directory:   $TRUST_ROOT/src/ThHyd/Turbulence
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Op_Diff_K_Eps_base_included
#define Op_Diff_K_Eps_base_included

#include <Operateur_Diff_base.h>
#include <Operateur.h>
#include <Operateur_negligeable.h>
#include <Matrice_Morse.h>
#include <Ref_Champ_Don.h>

class Champ_Fonc;
class Champ_base;
class DoubleTab;



//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    classe Op_Diff_K_Eps_base
//    Classe de base de la hierarchie des operateurs de diffusion
//    pour l'equation de transport K-epsilon.
//    Sert a modeliser le terme diffusif dans l'equation de transport
//    de K_Eps. On traite les deux equations de transport en une seule
//    equation vectorielle.
//    La viscosite_turbulente est a diviser par la constante Prdt_K
//    pour la diffusion de K et par la constante Prdt_Eps pour la
//    diffusion de Eps
// .SECTION voir aussi
//    Operateur_base Op_Diff_K_Eps Op_Diff_K_Eps_negligeable
//    Classe abstraite
//    Methode abstraite:
//      void associer_diffusivite_turbulente()
//////////////////////////////////////////////////////////////////////////////
class Op_Diff_K_Eps_base : public Operateur_base
{

  Declare_base(Op_Diff_K_Eps_base);

public:

  virtual void associer_diffusivite_turbulente() =0;
  inline virtual void associer_diffusivite(const Champ_base& )
  {
    cerr<<get_info()->name()<<" doit coder associer_diffusivite"<<endl;
    Process::exit();
  }

  inline  virtual const Champ_base& diffusivite() const
  {
    cerr<<get_info()->name()<<" doit coder diffusivite()"<<endl;
    throw;

  }

};


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Op_Diff_K_Eps_negligeable
//    Cette classe represente un operateur de diffusion pour
//    l'equation de transport K-epsilon negligeable.
//    Lorsqu'un operateur de ce type est utilise dans une equation
//    transport K-epsilon cela revient a negliger le terme de diffusion.
// .SECTION voir aussi
//    Op_Diff_K_Eps_base Operateur_negligeable
//////////////////////////////////////////////////////////////////////////////
class Op_Diff_K_Eps_negligeable : public Operateur_negligeable,
  public Op_Diff_K_Eps_base

{
  Declare_instanciable(Op_Diff_K_Eps_negligeable);

public:

  inline void associer(const Zone_dis&, const Zone_Cl_dis&, const Champ_Inc& );
  inline DoubleTab& ajouter(const DoubleTab& ,  DoubleTab& ) const;
  inline DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  inline void contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const;
  inline void contribuer_au_second_membre(DoubleTab& ) const;
  inline void modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const;
  inline void dimensionner(Matrice_Morse& ) const;
  inline void associer_diffusivite_turbulente();

protected:
  REF(Champ_base) la_diffusivite;
};

Declare_deriv(Op_Diff_K_Eps_base);


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    Classe Op_Diff_K_Eps
//    Classe generique de la hierarchie des operateurs de diffusion dans
//    l'equation de transport K-epsilon. Un objet Op_Diff_K_Eps peut
//    referencer n'importe quel objet derivant de Op_Diff_K_Eps_base.
// .SECTION voir aussi
//    Op_Diff_K_Eps_base
//////////////////////////////////////////////////////////////////////////////

class Op_Diff_K_Eps : public Operateur,
  public DERIV(Op_Diff_K_Eps_base)
{

  Declare_instanciable(Op_Diff_K_Eps);

public:

  inline Operateur_base& l_op_base();
  inline const Operateur_base& l_op_base() const;
  inline void associer_diffusivite_turbulente();
  DoubleTab& ajouter(const DoubleTab& , DoubleTab& ) const;
  DoubleTab& calculer(const DoubleTab& , DoubleTab& ) const;
  inline Champ_base& associer_diffusivite(const Champ_base&);
  inline const Champ_base& diffusivite() const;
  void typer();
  virtual inline int op_non_nul() const;

protected :

  REF(Champ_base) la_diffusivite;
};


///////////////////////////////////////////////////////////////
// Description:
//    Associe divers objets a un operateurs negligeable: NE FAIT RIEN
//    Simple appel a Operateur_negligeable::associer(const Zone_dis&,
//                                                     const Zone_Cl_dis&,
//                                                     const Champ_Inc&)
// Precondition:
// Parametre: Zone_dis& zone_dis
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Parametre: Zone_Cl_dis& zone_cl_dis
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Parametre: Champ_Inc& inco
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
///////////////////////////////////////////////////////////////

inline void Op_Diff_K_Eps_negligeable::associer(const Zone_dis& zone_dis,
                                                const Zone_Cl_dis& zone_cl_dis,
                                                const Champ_Inc& inco)
{
  Operateur_negligeable::associer(zone_dis,zone_cl_dis,inco);
}



// Description:
//    Ajoute la contribution de l'operateur a un tableau passe en parametre.
//    Simple appel a Operateur_negligeable::ajouter(const DoubleTab&,DoubleTab&)
// Precondition:
// Parametre: DoubleTab& x
//    Signification: le tableau sur lequel on applique l'operateur
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Parametre: DoubleTab& y
//    Signification: tableau auquel on ajoute la contribution de l'operateur
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: DoubleTab&
//    Signification: le parametre d'entree y non modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline DoubleTab& Op_Diff_K_Eps_negligeable::ajouter(const DoubleTab& x,  DoubleTab& y) const
{
  return Operateur_negligeable::ajouter(x,y);
}


// Description:
//    Initialise le parametre tableau avec la contribution
//    de l'operateur negligeable: initialise le tableau a ZERO.
//    Simple appel a Operateur_negligeable::(calculer(const DoubleTab&, DoubleTab&)
// Precondition:
// Parametre: DoubleTab& x
//    Signification: le tableau sur lequel on applique l'operateur
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Parametre: DoubleTab& y
//    Signification: tableau dans lequel stocke la contribution de l'operateur
//    Valeurs par defaut:
//    Contraintes: l'ancien contenu est ecrase
//    Acces: sortie
// Retour: DoubleTab&
//    Signification: le tableau d'entree y mis a zero
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline DoubleTab& Op_Diff_K_Eps_negligeable::calculer(const DoubleTab& x, DoubleTab& y) const
{
  return Operateur_negligeable::calculer(x,y);
}


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
//Description:
//on assemble la matrice.

inline void Op_Diff_K_Eps_negligeable::contribuer_a_avec(const DoubleTab& inco,
                                                         Matrice_Morse& matrice) const
{
  ;
}

//Description:
//on ajoute la contribution du second membre.

inline void Op_Diff_K_Eps_negligeable::contribuer_au_second_membre(DoubleTab& resu) const
{
  ;
}

// Modification des Cl
inline void  Op_Diff_K_Eps_negligeable::modifier_pour_Cl(Matrice_Morse& matrice, DoubleTab& resu) const
{
  ;
}

inline void  Op_Diff_K_Eps_negligeable::dimensionner(Matrice_Morse& matrice) const
{
  ;
}

inline void Op_Diff_K_Eps_negligeable::associer_diffusivite_turbulente()
{
  ;
}

// Description:
//    Renvoie l'objet sous-jacent upcaste en Operateur_base
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Operateur_base&
//    Signification: l'objet sous-jacent upcaste en Operateur_base
//    Contraintes:
// Exception: l'operateur n'a pas ete type
// Effets de bord:
// Postcondition:
//  Fonctions inline de la classe Op_Diff_K_Eps
///////////////////////////////////////////////////
inline Operateur_base& Op_Diff_K_Eps::l_op_base()
{
  if(!non_nul())
    Cerr << "Op_Diff_K_Eps n'a pas ete typer" << finl;
  return valeur();
}

// Description:
//    Renvoie l'objet sous-jacent upcaste en Operateur_base
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Operateur_base&
//    Signification: l'objet sous-jacent upcaste en Operateur_base
//    Contraintes: reference constante
// Exception: l'operateur n'a pas ete type
// Effets de bord:
// Postcondition:
inline const Operateur_base& Op_Diff_K_Eps::l_op_base() const
{
  if(!non_nul())
    Cerr << "Op_Diff_K_Eps n'a pas ete typer" << finl;
  return valeur();
}


// Description:
//    Appel a l'objet sous-jacent.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline void Op_Diff_K_Eps::associer_diffusivite_turbulente()
{
  valeur().associer_diffusivite_turbulente();
}



inline int Op_Diff_K_Eps::op_non_nul() const
{
  if (non_nul())
    return 1;
  else
    return 0;
}


// Description:
//    Appel a l'objet sous-jacent.
//    Initialise le tableau passe en parametre avec la contribution
//    de l'operateur.
// Precondition:
// Parametre: DoubleTab& inconnue
//    Signification: tableau contenant les donnees sur lesquelles on applique
//                   l'operateur.
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& resu
//    Signification: tableau dans lequel stocke la contribution de l'operateur
//    Valeurs par defaut:
//    Contraintes: l'ancien contenu est ecrase
//    Acces: sortie
// Retour: DoubleTab&
//    Signification: le tableau contenant le resultat
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline const Champ_base& Op_Diff_K_Eps::diffusivite() const
{
  return la_diffusivite.valeur();
}


// Description:
//    Associe la diffusivite a l'operateur.
// Precondition:
// Parametre: Champ_Don& nu
//    Signification: le champ representant la diffusivite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_Don&
//    Signification: le champ representant la diffusivite
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
inline Champ_base& Op_Diff_K_Eps::associer_diffusivite(const Champ_base& nu)
{
  la_diffusivite=nu;
  return la_diffusivite.valeur();
}




#endif




