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
// File:        Field_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Field_base_included
#define Field_base_included

#include <Objet_U.h>
#include <Noms.h>

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//    This is a base class from which inherit Champ_base and Champ_front_base.
//    It handles all the generic aspect of a field:
//     - its name and synonyms
//     - its number and names of components
//     - its nature (scalar, vectorial, multi_scalar)
//     - the units associated to each component
//
// .SECTION voir aussi
//    RefField_base
//////////////////////////////////////////////////////////////////////////////

// Definition of the nature of a field:
enum Nature_du_champ { scalaire, multi_scalaire, vectoriel };

class Field_base : public Objet_U
{
  Declare_base_sans_constructeur(Field_base);

public:
  Field_base();
  Field_base(const Field_base& other) =default;

  // Methods reimplemented from Objet_U
  void            nommer(const Nom& ) override;
  const Nom&      le_nom() const override;

  // Synonyms management
  inline const Noms& get_synonyms() const;
  inline void        add_synonymous(const Nom& nom);

  // Component management
  inline int              nb_comp() const ;
  virtual void            fixer_nb_comp(int i);

  const Noms&             noms_compo() const ;
  const Nom&              nom_compo(int ) const ;
  const Nom&              nom_compo() const ;
  virtual const Noms&     fixer_noms_compo(const Noms& ) ;
  virtual const Nom&      fixer_nom_compo(int, const Nom& ) ;
  virtual const Nom&      fixer_nom_compo(const Nom& ) ;

  // Physical unit management
  const Noms&             unites() const ;
  const Nom&              unite() const ;
  const Nom&              unite(int ) const ;
  virtual const Noms&     fixer_unites(const Noms& );
  virtual const Nom&      fixer_unite(const Nom& );
  virtual const Nom&      fixer_unite(int i, const Nom&);

  // Field nature management
  inline Nature_du_champ  nature_du_champ() const ;
  virtual Nature_du_champ fixer_nature_du_champ(Nature_du_champ nat);


protected:
  Nom nom_ ;
  Noms noms_synonymes_;
  Noms noms_compo_;
  Noms unite_;
  int nb_compo_ ;
  Nature_du_champ nature_;
};

inline const Noms& Field_base::get_synonyms() const
{
  return noms_synonymes_;
}

inline void Field_base::add_synonymous(const Nom& nom)
{
  noms_synonymes_.add(nom);
}

// Description:
//    Renvoie le nombre de composantes du champ.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de composantes du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline int Field_base::nb_comp() const
{
  return nb_compo_ ;
}

// Description:
//    Renvoie la nature d'un champ (scalaire, multiscalaire, vectoriel).
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nature_du_champ
//    Signification: la nature du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
inline Nature_du_champ Field_base::nature_du_champ() const
{
  return nature_;
}


#endif


