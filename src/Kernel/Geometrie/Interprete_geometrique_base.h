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

#ifndef Interprete_geometrique_base_included
#define Interprete_geometrique_base_included


#include <Interprete.h>
#include <Ref_Domaine.h>
#include <List.h>
#include <Zone.h>
#ifdef MICROSOFT
// necessaire pour visual
#include <Octree.h>
#endif

Declare_liste(REF(Domaine));


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Interprete_geometrique_base
//     .
//
// .SECTION voir aussi
//
//     Classe abstraite dont les interpretes geometriques qui modifient un ou plusieurs domaines doivent deriver.
//     Une reference est faite a chaque domaine. L'invalidation de l'octree apres modification du domaine
//     (par interpreter_(is)) est factorisee dans la methode interpreter()
//     Methodes abstraites:
//       int nombre_d_operateurs() const
//
//////////////////////////////////////////////////////////////////////////////
class Interprete_geometrique_base : public Interprete
{
  Declare_base(Interprete_geometrique_base);

public :

  virtual Entree& interpreter_(Entree& is)=0;
  Entree& interpreter(Entree& is) override;
  void associer_domaine(Nom& nom_dom);
  void associer_domaine(Entree& is);
  inline Domaine& domaine(int i=0)
  {
    return domains_(i).valeur();
  };
  inline const Domaine& domaine(int i=0) const
  {
    return domains_(i).valeur();
  };
  inline LIST(REF(Domaine))& domaines()
  {
    return domains_;
  };
  void mettre_a_jour_sous_zone(Zone& zone, int& elem, int num_premier_elem, int nb_elem) const;

protected :

  LIST(REF(Domaine)) domains_; // List of reference to domains

};

#endif

