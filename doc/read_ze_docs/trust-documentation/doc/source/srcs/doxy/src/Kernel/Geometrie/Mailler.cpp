/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <NettoieNoeuds.h>
#include <TRUST_Deriv.h>
#include <Mailler.h>
#include <Domaine.h>
#include <Scatter.h>
#include <Domaine.h>

#include <list>
#include <memory>

Implemente_instanciable(Mailler,"Mailler",Interprete_geometrique_base);

Sortie& Mailler::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

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
/*! @brief Fonction principale de l'interprete Mailler Structure du jeu de donnee (en dimension 2) :
 *
 *     Mailler dom
 *     {
 *     [Epsilon eps]
 *     [objet1]
 *     ,
 *     [objet2]
 *     ...
 *     }
 *     Deux points seront confondus des que la distance entre eux est
 *     inferieure a Epsilon.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree
 * @throws l'objet a mailler n'est pas du type Domaine
 * @throws accolade ouvrante attendue
 * @throws accolade fermante ou virgule attendue
 */
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
  Nom typ_domaine;
  std::list<Domaine*> dom_lst;  // the list of domains we will merge later when calling comprimer()
  std::list<DERIV(Domaine)> dom_lst2; // just for memory management
  do
    {
      is >> typ_domaine;
      motlu = typ_domaine;
      if(motlu=="epsilon")
        {
          double eps;
          is >> eps;
          dom.fixer_epsilon(eps);
          // GF le eps du domaine ne sert plus quand on cherche les sommets doubles;
          precision_geom=eps;
          is >> motlu;
          verifie_syntaxe(motlu);
        }
      else if(motlu=="Domain")
        {
          Nom nom_dom;
          is >> nom_dom;
          Cerr << "Adding a domain " << nom_dom << finl;
          Domaine& added_dom=ref_cast(Domaine, objet(nom_dom));
          dom_lst.push_back(&added_dom);
          is >> motlu;
          verifie_syntaxe(motlu);
        }
      else
        {
          dom_lst2.push_back(DERIV(Domaine)()); // to keep them alive till the end of the method
          DERIV(Domaine)& un_domaine = dom_lst2.back();
          un_domaine.typer(typ_domaine); // Most likely a Pave ...
          Domaine& ze_domaine = un_domaine.valeur();
          is >> ze_domaine;
          dom_lst.push_back(&ze_domaine);
          is >> motlu;
          verifie_syntaxe(motlu);
        }
    }
  while(motlu != "}");

  // Dans le cas ou on a modifie precision geom, on le remet a la valeur d'origine
  precision_geom=precision_geom_sa;
  dom.fill_from_list(dom_lst);
  dom.fixer_premieres_faces_frontiere();

  NettoieNoeuds::nettoie(dom);

  Scatter::init_sequential_domain(dom);
  return is;
}
