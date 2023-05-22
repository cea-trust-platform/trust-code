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

#include <LecFicDistribue.h>
#include <communications.h>
#include <Synonyme_info.h>
#include <Sous_Domaine.h>
#include <Interprete.h>
#include <Polynome.h>
#include <Parser_U.h>
#include <Domaine.h>

Implemente_instanciable(Sous_Domaine,"Sous_Domaine",Objet_U);
Add_synonym(Sous_Domaine, "Sous_Zone");

//  XD sous_zone objet_u sous_zone 1 It is an object type describing a domain sub-set. NL2 A Sous_Zone (Sub-area) type object must be associated with a Domaine type object. The Read (Lire) interpretor is used to define the items comprising the sub-area. NL2 Caution: The Domain type object nom_domaine must have been meshed (and triangulated or tetrahedralised in VEF) prior to carrying out the Associate (Associer) nom_sous_zone nom_domaine instruction; this instruction must always be preceded by the read instruction.
//  XD attr restriction ref_sous_zone restriction 1 The elements of the sub-area nom_sous_zone must be included into the other sub-area named nom_sous_zone2. This keyword should be used first in the Read keyword.
//  XD attr rectangle bloc_origine_cotes rectangle 1 The sub-area will include all the domain elements whose centre of gravity is within the Rectangle (in dimension 2).
//  XD attr segment bloc_origine_cotes segment 1 not_set
//  XD attr boite bloc_origine_cotes box 1 The sub-area will include all the domain elements whose centre of gravity is within the Box (in dimension 3).
//  XD attr liste listentier liste 1 The sub-area will include n domain items, numbers No. 1 No. i No. n.
//  XD attr fichier chaine filename 1 The sub-area is read into the file filename.
//  XD attr intervalle deuxentiers intervalle 1 The sub-area will include domain items whose number is between n1 and n2 (where n1<=n2).
//  XD attr polynomes bloc_lecture polynomes 1 A REPRENDRE
//  XD attr couronne bloc_couronne couronne 1 In 2D case, to create a couronne.
//  XD attr tube bloc_tube tube 1 In 3D case, to create a tube.
//  XD attr fonction_sous_zone chaine fonction_sous_domaine 1 Keyword to build a sub-area with the the elements included into the area defined by fonction>0.
//  XD attr union bloc_lecture union 1 The elements of the read sub-domains will be added to the sub-area nom_sous_zone. This keyword can be used alone, or last in the Read keyword.
//  XD ref domaine domaine

Sortie& Sous_Domaine::printOn(Sortie& os) const
{
  os << "{" << finl;
  os << "Liste" << finl;
  for (const auto& p : les_polys_) os << p << " ";
  os << "}" << finl;
  return os;
}

template <typename F>
void Sous_Domaine::get_polys_generique(const std::set<int>& les_polys_possibles_, const F& func)
{
  const Domaine& dom = le_dom_.valeur();
  const int nbsom = dom.nb_som_elem();
  DoubleVect x(dimension);
  for (const auto& p : les_polys_possibles_)
    {
      x = 0;
      int k, s;
      for (k = 0; k < nbsom && ((s = dom.sommet_elem(p, k)) >= 0); k++)
        for (int d = 0; d < dimension; d++)
          x(d) += dom.coord(s, d);
      x /= double(k);
      if (func(x) > 0) les_polys_.insert(p);
    }

  Cerr << "Construction of the subarea OK" << finl;
}


void Sous_Domaine::read_polynomials(const std::set<int>& les_polys_possibles_, Entree& is)
{
  Motcle motlu;
  Cerr << "Sous_Domaine::readOn : Reading of the polynomials" << finl;
  LIST(Polynome) les_polynomes;
  is >> motlu;
  if (motlu != Motcle("{"))
    {
      Cerr << "We expected a { " << finl;
      Process::exit();
    }
  while (1)
    {
      Polynome un_poly;
      is >> un_poly;
      les_polynomes.add(un_poly);
      is >> motlu;
      Motcle Et("et");
      if (motlu == Motcle("}"))
        break;

      if (motlu != Motcle("et"))
        {
          Cerr << "We expected " << Et << " or } " << finl;
          Process::exit();
        }
    }

  auto lambda = [&](DoubleVect &x) -> int
  {
    int test = 1;
    for (auto &itr : les_polynomes)
      if ((dimension == 2 && itr(x(0), x(1)) < 0) || (dimension == 3 && itr(x(0), x(1), x(2)) < 0))
        test = -1;
    return test;
  };

  get_polys_generique(les_polys_possibles_, lambda);
}

void Sous_Domaine::check_dimension(int d, const std::string m, const std::string mm)
{
  const int dd = d == 2 ? 3 : 2;
  if (dimension != d)
    {
      Cerr << m << "is not available in " << d << " dimensions!" << finl;
      if (mm != "") Cerr << "In " << dd << " dimensions, you can use '" << mm << "' instead." << finl;
      Process::exit();
    }
}

void Sous_Domaine::read_box(const std::set<int>& les_polys_possibles_, Entree& is)
{
  check_dimension(3, "boite", "rectangle");
  Motcle motlu;
  is >> motlu;
  if (motlu != Motcle("Origine"))
    {
      Cerr << "We expected the keyword \"Origine\"" << finl;
      Process::exit();
    }
  double deux_pi = M_PI * 2.0;
  double ox, oy, oz;
  is >> ox >> oy >> oz;
  is >> motlu;
  if (motlu != Motcle("Cotes"))
    {
      Cerr << "We expected the keyword \"Cotes\"" << finl;
      Process::exit();
    }
  double lx, ly, lz;
  is >> lx >> ly >> lz;
  lx += ox;
  ly += oy;
  lz += oz;
  if (axi)
    {
      oy *= deux_pi;
      ly *= deux_pi;
    }

  auto lambda = [&](DoubleVect &x) -> int
  {
    int test = -1;
    if (sup_strict(x[0], ox) && sup_strict(lx, x[0]) && sup_strict(x[1], oy) && sup_strict(ly, x[1]) && sup_strict(x[2], oz) && sup_strict(lz, x[2])) test = 1;
    return test;
  };
  get_polys_generique(les_polys_possibles_, lambda);
}

void Sous_Domaine::read_rectangle(const std::set<int>& les_polys_possibles_, Entree& is)
{
  check_dimension(2, "rectangle", "boite");
  Motcle motlu;
  is >> motlu;
  if (motlu != Motcle("Origine"))
    {
      Cerr << "We expected the keyword \"Origine\"" << finl;
      Process::exit();
    }
  double ox, oy;
  is >> ox >> oy;
  is >> motlu;
  if (motlu != Motcle("Cotes"))
    {
      Cerr << "We expected the keyword \"Cotes\"" << finl;
      Process::exit();
    }
  double lx, ly;
  is >> lx >> ly;
  lx += ox;
  ly += oy;

  auto lambda = [&](DoubleVect &x) -> int
  {
    int test = -1;
    if (sup_strict(x[0], ox) && sup_strict(lx, x[0]) && sup_strict(x[1], oy) && sup_strict(ly, x[1])) test = 1;
    return test;
  };
  get_polys_generique(les_polys_possibles_, lambda);

}

void Sous_Domaine::read_interval(Entree& is)
{
  if (Process::nproc() > 1)
    {
      Cerr << "You can't use Intervalle option for parallel calculation." << finl;
      Process::exit();
    }
  int prems, nombre;
  is >> prems >> nombre;
  for (int i = 0; i < nombre; i++)
    les_polys_.insert(prems + i);
}

void Sous_Domaine::read_in_file(Entree& is)
{
  // Lecture de la sous-domaine dans un fichier ascii.
  // Le fichier contient, pour chaque processeur dans l'ordre croissant,
  // un IntVect contenant les indices dans le domaine(0) des elements reels
  // qui constituent la sous-domaine.
  Nom nomfic;
  is >> nomfic;
  IntVect les_polys_tab;
  if (je_suis_maitre())
    {
      IntVect tab;
      EFichier fic;
      Cerr << "Reading of a subarea in the file " << nomfic << finl;
      if (!fic.ouvrir(nomfic))
        {
          Cerr << " Error while opening file." << finl;
          Process::exit();
        }
      fic >> les_polys_tab;
      for (int p = 1; p < nproc(); p++)
        {
          fic >> tab;
          envoyer(tab, 0, p, p);
        }
    }
  else
    recevoir(les_polys_tab, 0, me(), me());

  for (int i = 0; i < les_polys_tab.size(); i++) les_polys_.insert(les_polys_tab[i]);

  // Ajout a la liste "les_polys_" des indices des elements virtuels
  // de la sous-domaine.
  // On cree un tableau distribue de marqueurs des elements de la sous-domaine
  const Domaine& dom = le_dom_.valeur();
  const int nb_elem = dom.nb_elem();
  IntVect marqueurs;
  dom.creer_tableau_elements(marqueurs);
  for (const auto& p : les_polys_) marqueurs[p] = 1;

  marqueurs.echange_espace_virtuel();
  for (int i = nb_elem; i < dom.nb_elem_tot(); i++)
    if (marqueurs[i])
      les_polys_.insert(i);
}

void Sous_Domaine::read_plaque(const std::set<int>& les_polys_possibles_, Entree& is)
{
  check_dimension(3, "plaque", "segment");
  Motcle motlu;
  is >> motlu;
  if (motlu != Motcle("Origine"))
    {
      Cerr << "We expected the keyword \"Origine\"" << finl;
      Process::exit();
    }
  double deux_pi = M_PI * 2.0;
  double ox, oy, oz;
  is >> ox >> oy >> oz;
  is >> motlu;
  if (motlu != Motcle("Cotes"))
    {
      Cerr << "We expected the keyword \"Cotes\"" << finl;
      Process::exit();
    }
  double lx, ly, lz;
  is >> lx >> ly >> lz;
  if ((lx != 0) && (ly != 0) && (lz != 0))
    {
      Cerr << "We expected at least one quotation to zero" << finl;
      Process::exit();
    }
  lx += ox;
  ly += oy;
  lz += oz;
  if (axi)
    {
      oy *= deux_pi;
      ly *= deux_pi;
    }

  const Domaine& dom = le_dom_.valeur();
  const int nbsom = dom.nb_som_elem();
  double x, y, z;
  Cerr << "Construction of the subarea " << le_nom() << finl;
  for (const auto& p : les_polys_possibles_)
    {
      int le_som, s, nb_som_poly = 0;
      x = y = z = 0;
      for (le_som = 0; le_som < nbsom && ((s = dom.sommet_elem(p, le_som)) >= 0); le_som++)
        {
          x += dom.coord(s, 0);
          y += dom.coord(s, 1);
          z += dom.coord(s, 2);
          nb_som_poly++;
        }
      le_som = 0;
      double xmin, xmax, ymin, ymax, zmin, zmax;
      xmin = dom.coord(dom.sommet_elem(p, le_som), 0);
      ymin = dom.coord(dom.sommet_elem(p, le_som), 1);
      zmin = dom.coord(dom.sommet_elem(p, le_som), 2);
      xmax = xmin;
      ymax = ymin;
      zmax = zmin;
      for (le_som = 1; le_som < nbsom && ((s = dom.sommet_elem(p, le_som)) >= 0); le_som++)
        {
          xmin = std::min(xmin, dom.coord(s, 0));
          ymin = std::min(ymin, dom.coord(s, 1));
          zmin = std::min(zmin, dom.coord(s, 2));
          xmax = std::max(xmax, dom.coord(s, 0));
          ymax = std::max(ymax, dom.coord(s, 1));
          zmax = std::max(zmax, dom.coord(s, 2));
        }
      x /= ((double) ((nb_som_poly)));
      y /= ((double) ((nb_som_poly)));
      z /= ((double) ((nb_som_poly)));
      if ((sup_strict(x, ox) && sup_strict(lx, x) && sup_strict(y, oy) && sup_strict(ly, y) && sup_strict(oz, zmin) && inf_ou_egal(oz, zmax))
          || (sup_strict(x, ox) && sup_strict(lx, x) && sup_strict(z, oz) && sup_strict(lz, z) && sup_strict(oy, ymin) && inf_ou_egal(oy, ymax))
          || (sup_strict(y, oy) && sup_strict(ly, y) && sup_strict(z, oz) && sup_strict(lz, z) && sup_strict(ox, xmin) && inf_ou_egal(ox, xmax)))
        {
          les_polys_.insert(p);
        }
    }
  Cerr << "Construction of the subarea OK" << finl;
}

void Sous_Domaine::read_segment(const std::set<int>& les_polys_possibles_, Entree& is)
{
  check_dimension(2, "segment", "plaque");
  Motcle motlu;
  is >> motlu;
  if (motlu != Motcle("Origine"))
    {
      Cerr << "We expected the keyword \"Origine\"" << finl;
      Process::exit();
    }
  //double deux_pi=M_PI*2.0 ;
  double ox, oy;
  is >> ox >> oy;
  is >> motlu;
  if (motlu != Motcle("Cotes"))
    {
      Cerr << "We expected the keyword \"Cotes\"" << finl;
      Process::exit();
    }
  double lx, ly;
  is >> lx >> ly;
  if ((lx != 0) && (ly != 0))
    {
      Cerr << "We expected at least one quotation to zero" << finl;
      Process::exit();
    }
  lx += ox;
  ly += oy;

  const Domaine& dom = le_dom_.valeur();
  const int nbsom = dom.nb_som_elem();
  double x, y;
  Cerr << "Construction of the subarea " << le_nom() << finl;
  for (const auto& p : les_polys_possibles_)
    {
      int le_som;
      x = y = 0;
      for (le_som = 0; le_som < nbsom; le_som++)
        {
          x += dom.coord(dom.sommet_elem(p, le_som), 0);
          y += dom.coord(dom.sommet_elem(p, le_som), 1);
        }
      le_som = 0;
      double xmin, xmax, ymin, ymax;
      xmin = dom.coord(dom.sommet_elem(p, le_som), 0);
      ymin = dom.coord(dom.sommet_elem(p, le_som), 1);
      xmax = xmin;
      ymax = ymin; //zmax=zmin;
      for (le_som = 1; le_som < nbsom; le_som++)
        {
          xmin = std::min(xmin, dom.coord(dom.sommet_elem(p, le_som), 0));
          ymin = std::min(ymin, dom.coord(dom.sommet_elem(p, le_som), 1));
          xmax = std::max(xmax, dom.coord(dom.sommet_elem(p, le_som), 0));
          ymax = std::max(ymax, dom.coord(dom.sommet_elem(p, le_som), 1));
        }
      x /= ((double) ((nbsom)));
      y /= ((double) ((nbsom)));
      if ((sup_strict(x, ox) && sup_strict(lx, x) && sup_strict(oy, ymin) && inf_ou_egal(oy, ymax)) || (sup_strict(y, oy) && sup_strict(ly, y) && sup_strict(ox, xmin) && inf_ou_egal(ox, xmax)))
        {
          les_polys_.insert(p);
        }
    }
  Cerr << "Construction of the subarea OK" << finl;
}

void Sous_Domaine::read_couronne(const std::set<int>& les_polys_possibles_, Entree& is)
{
  check_dimension(2, "couronne", "tube");
  Motcle motlu;
  is >> motlu;
  if (motlu != Motcle("Origine"))
    {
      Cerr << "We expected the keyword \"ORIGINE\" " << finl;
      Process::exit();
    }
  double xo, yo;
  is >> xo >> yo;
  is >> motlu;
  if (motlu != Motcle("RI"))
    {
      Cerr << "We expected the internal radius \"RI\" " << finl;
      Process::exit();
    }
  double ri, re;
  is >> ri;
  is >> motlu;
  if (motlu != Motcle("RE"))
    {
      Cerr << "We expected the external radius \"RE\" " << finl;
      Process::exit();
    }
  is >> re;

  double ri2 = ri * ri; // Internal radius^2
  double re2 = re * re; // External radius^2


  auto lambda = [&](DoubleVect &x) -> int
  {
    int test = -1;
    if (sup_strict((x(0) - xo) * (x(0) - xo) + (x(1) - yo) * (x(1) - yo), ri2) && sup_strict(re2, (x(0) - xo) * (x(0) - xo) + (x(1) - yo) * (x(1) - yo))) test = 1;
    return test;
  };
  get_polys_generique(les_polys_possibles_, lambda);
}

void Sous_Domaine::read_tube(const std::set<int>& les_polys_possibles_, Entree& is)
{
  check_dimension(3, "tube", "couronne");
  Motcle motlu;
  is >> motlu;
  if (motlu != Motcle("Origine"))
    {
      Cerr << "We expected the keyword \"ORIGINE\" " << finl;
      Process::exit();
    }
  double xo, yo, zo;
  is >> xo >> yo >> zo;
  is >> motlu;
  if (motlu != Motcle("DIR"))
    {
      Cerr << "We expected the tube direction, keyword \"DIR\" " << finl;
      Process::exit();
    }
  Motcles coords(3);
  {
    coords[0] = "X";
    coords[1] = "Y";
    coords[2] = "Z";
  }
  Nom coord;
  is >> coord;
  motlu = coord;
  int idir = coords.search(motlu);
  int dir[3];
  dir[0] = dir[1] = dir[2] = 0;
  double h0 = 0;
  switch(idir)
    {
    case 0:
      dir[0] = 0;
      dir[1] = dir[2] = 1;
      h0 = xo;
      break;
    case 1:
      dir[1] = 0;
      dir[0] = dir[2] = 1;
      h0 = yo;
      break;
    case 2:
      dir[2] = 0;
      dir[1] = dir[0] = 1;
      h0 = zo;
      break;
    default:
      h0 = -1;
      Cerr << "DIR is equal to X for a tube parallel to OX ; Y for a tube parallel to OY and Z for a tube parallel to OZ" << finl;
      Cerr << "Currently, DIR is equal to " << coord << finl;
      exit();
    }
  is >> motlu;
  if (motlu != Motcle("RI"))
    {
      Cerr << "We expected the internal radius, keyword \"RI\" " << finl;
      Process::exit();
    }
  double ri, re, h;
  is >> ri;
  is >> motlu;
  if (motlu != Motcle("RE"))
    {
      Cerr << "We expected the external radius, keyword \"RE\" " << finl;
      Process::exit();
    }
  is >> re;
  is >> motlu;
  if (motlu != Motcle("Hauteur"))
    {
      Cerr << "We expected the height of tube, keyword \"Hauteur\" " << finl;
      Process::exit();
    }
  is >> h;
  h += h0;

  double ri2 = ri * ri, re2 = re * re;

  auto lambda = [&](DoubleVect &x) -> int
  {
    int test = -1;
    double tmp = dir[0] * (x(0) - xo) * (x(0) - xo) + dir[1] * (x(1) - yo) * (x(1) - yo) + dir[2] * (x(2) - zo) * (x(2) - zo);
    if (sup_ou_egal(tmp, ri2) && inf_ou_egal(tmp, re2) && inf_ou_egal(x(idir), h) && sup_ou_egal(x(idir), h0)) test = 1;
    return test;
  };
  get_polys_generique(les_polys_possibles_, lambda);
}

void Sous_Domaine::read_hexagonal_tube(const std::set<int>& les_polys_possibles_, Entree& is)
{
  check_dimension(3, "tube_hexagonal", "");
  Motcle motlu;
  // PQ : 17/09/08 : on suppose le tube hexagonal centre sur l'origine, porte par l'axe z
  // et pour lequel l'entreplat est entre y=-ep/2 et y=+ep/2
  double ep;
  bool in = 1;
  is >> motlu;
  if (motlu != Motcle("ENTREPLAT"))
    {
      Cerr << "We expected the entreplat of the tube, keyword \"ENTREPLAT\"" << finl;
      Process::exit();
    }
  is >> ep;
  is >> motlu;
  if (motlu == Motcle("IN"))
    in = 1;
  else if (motlu == Motcle("OUT"))
    in = 0;
  else
    {
      Cerr << "We expected the keyword \"IN\" or \"OUT\" " << finl;
      Process::exit();
    }

  auto lambda = [&](DoubleVect &x) -> int
  {
    int test = -1;
    if (sup_ou_egal(x(1), -ep / 2.) && sup_ou_egal(x(1), -ep - x(0) * sqrt(3.)) && sup_ou_egal(x(1), -ep + x(0) * sqrt(3.)) && inf_ou_egal(x(1), ep / 2.)
    && inf_ou_egal(x(1), ep - x(0) * sqrt(3.)) && inf_ou_egal(x(1), ep + x(0) * sqrt(3.)))
      {
        if (in == 1) test = 1;
      }
    else
      {
        if (in == 0) test = 1;
      }
    return test;
  };
  get_polys_generique(les_polys_possibles_, lambda);
}

void Sous_Domaine::read_function(const std::set<int>& les_polys_possibles_, Entree& is)
{
  Parser_U F;
  F.setNbVar(dimension);
  Nom fct;
  is >> fct;
  F.setString(fct);
  std::array<std::string, 3> vars = {"x", "y", "z"};
  for (int i = 0; i < dimension; i++) F.addVar(Nom(vars[i]));

  F.parseString();




  auto lambda = [&](DoubleVect &x) -> int
  {
    for (int i = 0; i < dimension; i++) F.setVar(i, x[i]);
    return int(F.eval());
  };
  get_polys_generique(les_polys_possibles_, lambda);
}

void Sous_Domaine::read_union(Entree& is)
{
  Nom m;
  if (is >> m, m != "{") Cerr << "sous_domaine union : { expected instead of " << m << finl, Process::exit();
  Cerr << "sub-domain " << nom_ << " : union with {";
  for (is >> m; m != "}"; is >> m)
    {
      const Sous_Domaine& ssz = ref_cast(Sous_Domaine, interprete().objet(m));
      Cerr << " " << m;
      for (int i = 0; i < ssz.nb_elem_tot(); i++)
        les_polys_.insert(ssz(i));
    }
  Cerr << " }" << finl;
}

/*! @brief Lit les specifications d'un sous-domaine dans le jeu de donnee a partir d'un flot d'entree.
 *
 *     Format:
 *      { Rectangle Origine x0 y0 Cotes lx ly } en dimension 2
 *      { Boite Origine x0 y0 z0 Cotes lx ly lz} en dimension 3
 *       ou
 *      { Liste n n1  ni   nn }
 *       ou
 *      { Intervalle n1 n2 }
 *       ou
 *      { Polynomes {bloc_lecture_poly1 et bloc_lecture_poly_i
 *                   et bloc_lecture_poly_n}
 *      }
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 * @throws mot clef non reconnu
 * @throws mot clef "et" ou "}" attendu
 * @throws En dimension 2, il faut utiliser Rectangle
 * @throws mot clef "Origine" attendu
 * @throws mot clef "Cotes" attendu
 * @throws En dimension 3, il faut utiliser Boite
 * @throws Erreur TRUST (mot clef reconnu non prevu)
 * @throws accolade fermante attendue
 */
Entree& Sous_Domaine::readOn(Entree& is)
{
  if (!le_dom_.non_nul())
    {
      Cerr << "You have not associated one of the objects of type Sous_Domaine " << finl;
      Cerr << "to the object of type Domain " << finl;
      Process::exit();
    }

  Motcles les_mots(14);
  {
    les_mots[0] = "Liste";
    les_mots[1] = "Polynomes";
    les_mots[2] = "Boite";
    les_mots[3] = "Rectangle";
    les_mots[4] = "Intervalle";
    les_mots[5] = "Fichier";
    les_mots[6] = "Plaque";
    les_mots[7] = "Segment";
    les_mots[8] = "Couronne";
    les_mots[9] = "Tube";
    les_mots[10]= "Tube_Hexagonal";
    les_mots[11]= "fonction_sous_zone";
    les_mots[12]= "fonction_sous_domaine";
    les_mots[13]= "union";
  }

  Motcle motlu;
  is >>  motlu;
  if(motlu != Motcle("{"))
    {
      Cerr << "We expected a { to the reading of the subarea" << finl;
      Cerr << "instead of " << motlu << finl;
      exit();
    }
  is >> motlu;

  std::set<int> les_polys_possibles_;
  if (motlu == "restriction")
    {
      Nom nom_ss_domaine;
      is >> nom_ss_domaine;
      const Sous_Domaine& ssz = ref_cast(Sous_Domaine,interprete().objet(nom_ss_domaine));

      for (int i = 0; i < ssz.nb_elem_tot(); i++)
        les_polys_possibles_.insert(ssz(i));
      is >> motlu;
    }
  else
    for (int i = 0; i < le_dom_->nb_elem_tot(); i++)
      les_polys_possibles_.insert(i);

  int rang = les_mots.search(motlu);
  if (rang == -1)
    {
      int ok = lire_motcle_non_standard(motlu, is);
      if (!ok)
        {
          Cerr << motlu << " is not understood " << finl;
          Cerr << "The understood keywords are " << les_mots;
          Process::exit();
        }
    }
  if ((rang == 0) && (Process::nproc() > 1))
    {
      Cerr << "When the subareas are defined as lists of elements" << finl;
      Cerr << "it is necessary to split them for parallel computing." << finl;
      Process::exit();
    }
  switch(rang)
    {
    case 0 :
      {
        IntVect les_polys_tab;
        is >> les_polys_tab;
        for (int i = 0; i < les_polys_tab.size(); i++) les_polys_.insert(les_polys_tab[i]);
        break;
      }
    case 1 :
      {
        read_polynomials(les_polys_possibles_, is);
        break;
      }
    case 2 :
      {
        read_box(les_polys_possibles_, is);
        break;
      }
    case 3 :
      {
        read_rectangle(les_polys_possibles_, is);
        break;
      }
    case 4 :
      {
        read_interval(is);
        break;
      }
    case 5 :
      {
        read_in_file(is);
        break;
      }
    case 6 :
      {
        read_plaque(les_polys_possibles_, is);
        break;
      }
    case 7:
      {
        read_segment(les_polys_possibles_, is);
        break;
      }
    case 8 :
      {
        read_couronne(les_polys_possibles_, is);
        break;
      }
    case 9 :
      {
        read_tube(les_polys_possibles_, is);
        break;
      }
    case 10 :
      {
        read_hexagonal_tube(les_polys_possibles_, is);
        break;
      }
    case 11:
    case 12:
      {
        read_function(les_polys_possibles_, is);
        break;
      }
    case 13:
      {
        read_union(is);
        break;
      }
    case -1:
      // traite avant
      break;
    default :
      {
        Cerr << "TRUST error" << finl;
        Process::exit();
      }
      break;
    }

  is >> motlu;
  if (motlu == "union")
    {
      read_union(is);
      is >> motlu;
    }

  if(motlu != Motcle("}"))
    {
      Cerr << "We expected a } to the reading of the subarea" << finl;
      Cerr << "instead of " << motlu << finl;
      Process::exit();
    }
  return is;
}

int Sous_Domaine::lire_motcle_non_standard(const Motcle& motlu , Entree& is)
{
  return 0;
}

/*! @brief Ajoute un polyedre au sous-domaine.
 *
 * @return (int)
 */
int Sous_Domaine::add_poly(const int poly)
{
  assert(poly >= 0);
  les_polys_.insert(poly);
  return 1;
}


/*! @brief Enleve un polyedre du sous-domaine.
 *
 * @return (int)
 */
int Sous_Domaine::remove_poly(const int poly)
{
  assert(poly >= 0);
  les_polys_.erase(poly);
  return 1;
}


/*! @brief Associe un sous-domaine au domaine.
 *
 * Le sous-domaine sera un sous-domaine du Domaine specifie.
 *
 * @param (Domaine& domaine) le domaine a associer au sous-domaine
 */
void Sous_Domaine::associer_domaine(const Domaine& un_domaine)
{
  le_dom_=un_domaine;
}


/*! @brief Associe un Objet_U au sous-domaine.
 *
 * On controle le type de l'objet a associer
 *     dynamiquement.
 *
 * @param (Objet_U& ob) objet a associer au sous-domaine.
 * @return (int) renvoie 1 si l'association a reussi 0 sinon.
 */
int Sous_Domaine::associer_(Objet_U& ob)
{
  if( sub_type(Domaine, ob))
    {
      // MONOZONE pour le moment
      if(le_dom_.non_nul()) return 1;
      associer_domaine(ref_cast(Domaine, ob));
      ob.associer_(*this);
      return 1;
    }
  return 0;
}
