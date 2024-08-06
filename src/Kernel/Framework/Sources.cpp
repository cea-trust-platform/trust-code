/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Matrice_Morse.h>
#include <TRUSTTab.h>
#include <Sources.h>

Implemente_instanciable(Sources, "Sources", LIST(Source));
// XD sources listobj sources -1 source_base -1 The sources.

Sortie& Sources::printOn(Sortie& os) const { return LIST(Source)::printOn(os); }

/*! @brief Lecture d'une liste de sources sur un flot d'entree.
 *
 * Lit une liste de sources separees par des virgules
 *     et le sajoute a la liste.
 *     format:
 *     {
 *      bloc de lecture d'une source
 *      [, bloc de lecture d'une source]
 *      ...
 *     }
 *
 * @param (Entree& is) le flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 * @throws accolade fermante ou virgule attendue
 */
Entree& Sources::readOn(Entree& is)
{
  Nom accouverte="{";
  Nom accfermee="}";
  Nom virgule=",";
  Nom typ;
  is >> typ;
  if (typ!=accouverte)
    {
      Cerr << "We were waiting for { before " << typ << finl;
      exit();
    }
  Source t;
  is >> typ;
  if(typ==accfermee)
    return is;
  while(1)
    {
      Source& so=add(t);
      Cerr << "Reading of term " << typ << finl;
      Cerr << "and typing: ";
      // Cout << ">>>>>>>>>>>>>>>>>>>> Type de Source = " << typ << finl;
      so.typer(typ,mon_equation.valeur());
      so->associer_eqn(mon_equation.valeur());
      is >> so.valeur();
      is >> typ;
      if(typ==accfermee)
        return is;
      if(typ!=virgule)
        {
          Cerr << typ << " : we expected a ',' or a '}'" << finl;
          exit();
        }
      assert (typ==virgule);
      is >> typ;
    }
}


/*! @brief Ajoute la contribution de toutes les sources de la liste au tableau passe en parametre, et renvoie ce tableau.
 *
 * @param (DoubleTab& xx) le tableau dans lequel on doit accumuler la contribution des termes sources
 * @return (DoubleTab&) le parametre xx modifie
 */
DoubleTab& Sources::ajouter(DoubleTab& xx) const
{
  for (const auto& itr : *this) itr.ajouter(xx);
  return xx;
}

/*! @brief Calcule la contribution de toutes les sources de la liste stocke le resultat dans le tableau passe en parametre,
 *
 *     et renvoie  ce tableau.
 *
 * @param (DoubleTab& xx) le tableau dans lequel on doit stocker la somme des contributions des sources
 * @return (DoubleTab&) le parametre xx modifie
 */
DoubleTab& Sources::calculer(DoubleTab& xx) const
{
  xx = 0.;
  for (const auto& itr : *this) itr.ajouter(xx);
  return xx;
}

/*! @brief Mise a jour en temps, de toute les sources de la liste
 *
 * @param (double temps) le pas de temps de mise a jour
 */
void Sources::mettre_a_jour(double temps)
{
  for (auto& itr : *this) itr->mettre_a_jour(temps);
}

/*! @brief Rest all sources to a given time
 *  See ProblemeTrio::resetTime()
 */
void Sources::resetTime(double temps)
{
  for (auto& itr : *this) itr->resetTime(temps);
}

/*! @brief Appelle Source::completer() sur toutes les sources de la liste.
 *
 * voir Source_base::completer().
 *
 */
void Sources::completer()
{
  for (auto& itr : *this) itr->completer();
}

/*! @brief Pour chaque source de la liste, appel a associer_champ_rho de la source.
 *
 *   Si la masse volumique est variable, il faut declarer le
 *   champ de rho aux sources avec cette methode (front-tracking)
 *   Sinon, par defaut, les calculs sont faits avec rho=1
 *
 */
void Sources::associer_champ_rho(const Champ_base& champ_rho)
{
  for (auto& itr : *this)
    {
      Source& src = itr;
      Source_base& src_base = src.valeur();
      src_base.associer_champ_rho(champ_rho);
    }
}

/*! @brief Pour chaque source de la liste, appel a a_pour_Champ_Fonc(mot,ch_ref).
 *
 * Cette methode est appelee par Equation_base::a_pour_Champ_Fonc.
 *
 */
int Sources::a_pour_Champ_Fonc(const Motcle& mot,
                               REF(Champ_base)& ch_ref) const
{
  int ok = 0;
  for (const auto& itr : *this)
    {
      const Source& src = itr;
      const Source_base& src_base = src.valeur();
      if (src_base.a_pour_Champ_Fonc(mot, ch_ref))
        {
          ok = 1;
          break;
        }
    }
  return ok;
}

/*! @brief Appelle Source::impr() sur toutes les sources de la liste.
 *
 * voir Source_base::impr().
 *
 */
int Sources::impr(Sortie& os) const
{
  for (const auto& itr : *this) itr->impr(os);
  return 1;
}

/*! @brief Dimensionnement de la matrice implicite des termes sources.
 *
 * Parcours toutes les sources de la liste pour dimensionner.
 *
 */
void Sources::dimensionner(Matrice_Morse& matrice) const
{
  for (const auto& itr : *this)
    {
      const Source& src = itr;
      const Source_base& src_base = src.valeur();
      Matrice_Morse mat;
      src_base.dimensionner(mat);
      if (mat.nb_colonnes()) matrice += mat;
    }
}
/*! @brief contribution a la matrice implicite des termes sources par defaut pas de contribution
 *
 */
void Sources::contribuer_a_avec(const DoubleTab& a, Matrice_Morse& matrice) const
{
  for (const auto& itr : *this)
    {
      const Source& src = itr;
      const Source_base& src_base = src.valeur();
      src_base.contribuer_a_avec(a,matrice);
    }
}

void Sources::contribuer_jacobienne(Matrice_Bloc& matrice, int n) const
{
  for (const auto& itr : *this)
    {
      const Source& src = itr;
      const Source_base& src_base = src.valeur();
      src_base.contribuer_jacobienne(matrice, n);
    }
}

/*! @brief Appelle Source::initialiser(temps) sur toutes les sources de la liste.
 *
 * voir Source_base::initialiser(double temps).
 *
 */
int Sources::initialiser(double temps)
{
  int ok=1;
  for (auto& itr : *this)
    ok = ok && itr->initialiser(temps);
  return ok;
}

void Sources::check_multiphase_compatibility() const
{
  for (const auto& itr : *this)
    {
      const Source& src = itr;
      const Source_base& src_base = src.valeur();
      src_base.check_multiphase_compatibility();
    }
}
