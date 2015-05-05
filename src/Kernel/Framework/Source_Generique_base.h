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
// File:        Source_Generique_base.h
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Source_Generique_base_included
#define Source_Generique_base_included

#include <Source_base.h>
#include <Champ_Generique.h>


//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Source_Generique_base
//     Cette classe est la base de la hierarchie des sources portant
//     un Champ_Generique qui permet d'evaluer une expression dependante
//     de champs du probleme.
// .SECTION voir aussi
//     Source_base
//     Classe abstraite.
//     Methodes abstraites:
//       DoubleTab& ajouter(DoubleTab& ) const
//       void associer_zones(const Zone_dis& ,const Zone_Cl_dis& ) [protected]
//       void associer_pb(const Probleme_base& ) [protected]
//         Nom localisation_source()
//
//  Syntaxe :
//  Sources { Source_Generique "Champ_Generique { ...} " }
//  avec "Champ_Generique" un champ generique a specifier.
//
//  Rq : la discretisation du champ renvoye par le champ generique
//       doit correspondre a celle ou est evaluee le terme source.
//
//////////////////////////////////////////////////////////////////////////////
class Source_Generique_base : public Source_base
{
  Declare_base(Source_Generique_base);

public :

  virtual DoubleTab& ajouter(DoubleTab& ) const=0;
  virtual DoubleTab& calculer(DoubleTab& ) const;
  virtual void associer_zones(const Zone_dis& ,const Zone_Cl_dis&)=0;
  virtual void associer_pb(const Probleme_base& );
  virtual Nom localisation_source() =0;
  virtual void completer();
  virtual void mettre_a_jour(double temps);

protected :

  Champ_Generique ch_source_;

};

#endif
