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
// File:        Mailler.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <Mailler.h>
#include <Domaine.h>
#include <Deriv_Zone.h>
#include <NettoieNoeuds.h>
#include <Scatter.h>

Implemente_instanciable(Mailler,"Mailler",Interprete_geometrique_base);

// Description:
//    Simple appel a: Interprete::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Mailler::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


// Description:
//    Simple appel a: Interprete::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Mailler::readOn(Entree& is)
{
  return Interprete::readOn(is);
}
inline void verifie_syntaxe(Motcle& motlu)
{
  if (motlu!="}" && motlu!=",")
    {
      Cerr << "We expected , or }" << finl;
      Process::exit();
    }
}
// Description:
//    Fonction principale de l'interprete Mailler
//    Structure du jeu de donnee (en dimension 2) :
//    Mailler dom
//    {
//    [Epsilon eps]
//    [objet1]
//    ,
//    [objet2]
//    ...
//    }
//    Deux points seront confondus des que la distance entre eux est
//    inferieure a Epsilon.
// Precondition:
//    Les seuls objets actuellement reconnus par Trio-U pour
//    mailler un domaine sont les objets de type Pave.
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception: l'objet a mailler n'est pas du type Domaine
// Exception: accolade ouvrante attendue
// Exception: accolade fermante ou virgule attendue
// Effets de bord:
// Postcondition:
Entree& Mailler::interpreter_(Entree& is)
{
  if (Process::nproc()>1)
    {
      Cerr << "Mailler keyword can't be used in parallel calculation!" << finl;
      Cerr << "Use only Scatter keyword to read partitioned mesh." << finl;
      exit();
    }
  double precision_geom_sa=precision_geom;
  associer_domaine(is);
  Domaine& dom=domaine();

  // On debloque les structures pour modifier le domaine
  Scatter::uninit_sequential_domain(dom);

  Motcle motlu;
  is >> motlu;
  if (motlu != "{")
    {
      Cerr << "We expected a {" << finl;
      exit();
    }
  // Valeur par defaut: precision_geom
  dom.fixer_epsilon(Objet_U::precision_geom);
  Nom typ_zone;
  do
    {
      is >> typ_zone;
      motlu = typ_zone;
      if(motlu=="epsilon")
        {
          double eps;
          is >> eps;
          dom.fixer_epsilon(eps);
          // GF le eps du domaine ne sert plus quand on cherche les sommets
          // doubles;
          precision_geom=eps;
          is >> motlu;
          if ((motlu!="}") && (motlu!=",") )
            {
              Cerr<<"The syntax of \"Mailler\" has changed. We expected \",\" after the definition of epsilon and not \"" <<motlu << "\"" <<finl;
              exit();
            }
          verifie_syntaxe(motlu);
        }
      else if (motlu=="Domaine")
        {
          Cerr<<"The syntax of \"Mailler\" has changed. The keyword \"Domaine\" has been replaced by \"Domain\"" <<finl;
          exit();
        }
      else if(motlu=="Domain")
        {
          Nom nom_dom;
          is >> nom_dom;
          Cerr << "Adding a domain " << nom_dom << finl;
          Domaine& added_dom=ref_cast(Domaine, objet(nom_dom));
          IntVect nums;
          Zone& zone=dom.add(added_dom.zone(0));
          zone.associer_domaine(dom);
          dom.ajouter(added_dom.coord_sommets(), nums);
          Scatter::uninit_sequential_domain(dom);
          zone.renum(nums);
          zone.associer_domaine(dom);
          dom.comprimer();
          dom.zone(0).fixer_premieres_faces_frontiere();
          dom.zone(0).type_elem().associer_zone(dom.zone(0));
          is >> motlu;
          verifie_syntaxe(motlu);
        }
      else
        {
          DERIV(Zone) une_zone;
          une_zone.typer(typ_zone);
          Zone& ajoutee = dom.add(une_zone.valeur());
          ajoutee.associer_domaine(dom);
          is >> ajoutee;
          ajoutee.associer_domaine(dom); // On renumerote!
          is >> motlu;
          verifie_syntaxe(motlu);
        }
    }
  while(motlu != "}");
  // Dans le cas ou on a modifie precision geom, on le remet a la valeur
  // d'origine
  precision_geom=precision_geom_sa;
  dom.comprimer();
  dom.zone(0).fixer_premieres_faces_frontiere();


  NettoieNoeuds::nettoie(dom);

  Scatter::init_sequential_domain(dom);
  return is;
}
