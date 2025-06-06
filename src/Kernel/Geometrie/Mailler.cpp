/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <list>
#include <memory>

Implemente_instanciable_32_64(Mailler_32_64,"Mailler",Interprete_geometrique_base_32_64<_T_>);
// XD mailler interprete mailler -1 The Mailler (Mesh) interpretor allows a Domain type object domaine to be meshed with objects objet_1, objet_2, etc...
// XD attr domaine ref_domaine domaine 0 Name of domain.
// XD attr bloc list_bloc_mailler bloc 0 Instructions to mesh.

template <typename _SIZE_>
Sortie& Mailler_32_64<_SIZE_>::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

template <typename _SIZE_>
Entree& Mailler_32_64<_SIZE_>::readOn(Entree& is)
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
 * @throws l'objet a mailler n'est pas du type Domaine_t
 * @throws accolade ouvrante attendue
 * @throws accolade fermante ou virgule attendue
 */
template <typename _SIZE_>
Entree& Mailler_32_64<_SIZE_>::interpreter_(Entree& is)
{
  if (Process::is_parallel())
    {
      Cerr << "Mailler keyword can't be used in parallel calculation!" << finl;
      Cerr << "Use only Scatter keyword to read partitioned mesh." << finl;
      Process::exit();
    }
  double precision_geom_sa = this->precision_geom;
  this->associer_domaine(is);
  Domaine_t& dom = this->domaine();

  // On debloque les structures pour modifier le domaine
  Scatter::uninit_sequential_domain(dom);

  Motcle motlu;
  is >> motlu;
  if (motlu != "{")
    {
      Cerr << "We expected a {" << finl;
      Process::exit();
    }
  Nom typ_domaine;
  std::list<Domaine_t*> dom_lst;  // the list of domains we will merge later when calling comprimer()
  std::list<OWN_PTR(Domaine_t)> dom_lst2; // just for memory management
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
          this->precision_geom=eps;
          is >> motlu;
          verifie_syntaxe(motlu);
        }
      else if(motlu=="Domain")
        {
          Nom nom_dom;
          is >> nom_dom;
          Cerr << "Adding a domain " << nom_dom << finl;
          Domaine_t& added_dom=ref_cast(Domaine_t, this->objet(nom_dom));
          dom_lst.push_back(&added_dom);
          is >> motlu;
          verifie_syntaxe(motlu);
        }
      else
        {
          dom_lst2.push_back(OWN_PTR(Domaine_t)()); // to keep them alive till the end of the method
          OWN_PTR(Domaine_t)& un_domaine = dom_lst2.back();
          un_domaine.typer(typ_domaine); // Most likely a Pave ...
          Domaine_t& ze_domaine = un_domaine.valeur();
          is >> ze_domaine;
          dom_lst.push_back(&ze_domaine);
          is >> motlu;
          verifie_syntaxe(motlu);
        }
    }
  while(motlu != "}");

  // Dans le cas ou on a modifie precision geom, on le remet a la valeur d'origine
  this->precision_geom=precision_geom_sa;
  dom.fill_from_list(dom_lst);
  dom.fixer_premieres_faces_frontiere();

  NettoieNoeuds_32_64<_SIZE_>::nettoie(dom);

  Scatter::init_sequential_domain(dom);
  return is;
}

template class Mailler_32_64<int>;
#if INT_is_64_ == 2
template class Mailler_32_64<trustIdType>;
#endif
