/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Champ_Fonc_Morceaux.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_Morceaux.h>
#include <Champ_Uniforme.h>
#include <Sous_Zone.h>
#include <Domaine.h>
#include <Interprete.h>
#include <Vect_Parser_U.h>

Implemente_instanciable(Champ_Fonc_Morceaux,"Champ_Fonc_Morceaux",Champ_Don_base);




// Description:
//    Imprime les valeurs du champs sur un flot de sortie
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
Sortie& Champ_Fonc_Morceaux::printOn(Sortie& os) const
{
  return os << valeurs();
}


// Description:
//    Lit les valeurs du champ uniforme par morceaux
//    a partir d'un flot d'entree.
//    On lit le nom du domaine (nom_domaine) le nombre de
//    composantes du champ (nb_comp) la valeur par defaut
//    du champ ainsi que les valeurs sur les sous zones.
//    Format:
//     Champ_Fonc_Morceaux nom_domaine nb_comp
//     { Defaut val_def sous_zone_1 val_1 ... sous_zone_i val_i }
// Precondition: pour utiliser un objet de type Champ_Fonc_Morceaux
//               il faut avoir defini des objets de type Sous_Zone
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le champ d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: mot clef "defaut" attendu
// Effets de bord:
// Postcondition:
Entree& Champ_Fonc_Morceaux::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  int k, poly;

  /* 1. nom du domaine */
  is >> nom;
  mon_domaine = ref_cast(Domaine, Interprete::objet(nom));
  Domaine& le_domaine=mon_domaine.valeur();

  /* 2. dimension et creation des tableaux parser_idx (num de formule pour chaque maille) et valeurs_ */
  dim=lire_dimension(is,que_suis_je());
  fixer_nb_comp(dim);
  parser_idx.resize(0, dim), valeurs_.resize(0, dim);
  le_domaine.creer_tableau_elements(parser_idx);
  le_domaine.creer_tableau_elements(valeurs_);

  /* 3. probleme et champ variable */
  is >> nom;
  ref_pb = ref_cast(Probleme_base, Interprete::objet(nom));
  is >> nom_champ_parametre_;

  /* 4. morceaux : ligne "defaut" */
  is >> motlu;
  if(motlu != Motcle("{") )
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected a { instead of " << nom << finl;
      exit();
    }
  is >> motlu;
  if(motlu != Motcle("defaut") )
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected defaut instead of " << nom << finl;
      exit();
    }

  /* parsers par defaut */
  for(k=0; k< dim; k++)
    {
      Parser_U psr;
      Nom tmp;
      is >> tmp;
      psr.setNbVar(5);
      psr.setString(tmp);
      psr.addVar("val"), psr.addVar("t"), psr.addVar("x"), psr.addVar("y"), psr.addVar("z");
      psr.parseString();
      for (poly=0; poly<le_domaine.zone(0).nb_elem_tot(); poly++) parser_idx(poly, k) = parser.size();
      parser.add(psr);
    }

  is >> nom;
  while (nom != Motcle("}") )
    {
      REF(Sous_Zone) refssz=les_sous_zones.add(le_domaine.ss_zone(nom));
      Sous_Zone& ssz = refssz.valeur();
      for( k=0; k< dim; k++)
        {
          Parser_U psr;
          Nom tmp;
          is >> tmp;
          psr.setNbVar(5);
          psr.setString(tmp);
          psr.addVar("val"), psr.addVar("t"), psr.addVar("x"), psr.addVar("y"), psr.addVar("z");
          psr.parseString();
          for (poly=0; poly < ssz.nb_elem_tot(); poly++) parser_idx(ssz(poly), k) = parser.size();
          parser.add(psr);
        }
      is >> nom;
    }
  return is;
}

// Description:
//    Renvoie une reference sur le domaine associe.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: REF(Domaine)&
//    Signification: reference sur le domaine associe
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const REF(Domaine)& Champ_Fonc_Morceaux::domaine() const
{
  return mon_domaine;
}

// Description:
//    Renvoie une reference sur le domaine associe.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: REF(Domaine)&
//    Signification: reference sur le domaine associe
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
REF(Domaine)& Champ_Fonc_Morceaux::domaine()
{
  return mon_domaine;
}

// Description:
//    Renvoie la liste des sous_zones associees.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: LIST(REF(Sous_Zone))&
//    Signification: la liste des sous_zones associees
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const LIST(REF(Sous_Zone))& Champ_Fonc_Morceaux::sous_zones() const
{
  return les_sous_zones;
}

// Description:
//    Renvoie la liste des sous_zones associees.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: LIST(REF(Sous_Zone))&
//    Signification: la liste des sous_zones associees
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
LIST(REF(Sous_Zone))& Champ_Fonc_Morceaux::sous_zones()
{
  return les_sous_zones;
}


// Description:
//    NE FAIT RIEN
// Precondition:
// Parametre: mp_base& ch
//    Signification:
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: NON ACCEDE
// Retour: Champ_base&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_base& Champ_Fonc_Morceaux::affecter_(const Champ_base& ch)
{
  if(sub_type(Champ_Uniforme, ch))
    {
      const DoubleTab& val=ch.valeurs();
      if(val.nb_dim()==1)
        valeurs()=ch.valeurs()[0];
      else
        valeurs()=ch.valeurs()(0,0);
    }
  else
    {
      valeurs()=ch.valeurs();
    }
  return *this;
}


// Description:
//    Renvoie la valeur du champ au point specifie
//    par ses coordonnees.
// Precondition:
// Parametre: DoubleVect& positions
//    Signification: les coordonnees du point de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& valeurs
//    Signification: la valeur du champ au point specifie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleVect&
//    Signification: la valeur du champ au point specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Fonc_Morceaux::valeur_a(const DoubleVect& positions,
                                          DoubleVect& tab_valeurs) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect le_poly(1);
  la_zone.chercher_elements(positions,le_poly);
  return valeur_a_elem(positions,tab_valeurs,le_poly[0]);
}


// Description:
//    Renvoie la valeur du champ au point specifie
//    par ses coordonnees, en indiquant que ce point est
//    situe dans un element specifie.
// Precondition:
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& val
//    Signification: la valeur du champ au point specifie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: int le_poly
//    Signification: l'element dans lequel est situe le point de calcul
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect&
//    Signification: la valeur du champ au point specifie
//    Contraintes:
// Exception: tableau stockant la valeur du champ de taille incorrecte
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Fonc_Morceaux::valeur_a_elem(const DoubleVect& ,
                                               DoubleVect& val,
                                               int le_poly) const
{
  if (val.size() != nb_compo_)
    {
      Cerr << "Error TRUST in Champ_Fonc_Morceaux::valeur_a_elem()" << finl;
      Cerr << "The DoubleVect val doesn't have the correct size" << finl;
      exit();
    }

  const DoubleTab& ch = valeurs();

  if (nb_compo_ == 1)
    val(0) = ch(le_poly,0);
  else
    for (int k=0; k<nb_compo_; k++)
      val(k) = ch(le_poly,k);

  return val;
}


// Description:
//    Renvoie la valeur d'une composante du champ au point specifie
//    par ses coordonnees, en indiquant que ce point est
//    situe dans un element specifie.
// Precondition:
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int le_poly
//    Signification: l'element dans lequel est situe le point de calcul
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: la valeur de la composante du champ specifiee au
//                   point specifie
//    Contraintes:
// Exception: index de la composante du champ a calculer trop grand
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Champ_Fonc_Morceaux::valeur_a_elem_compo(const DoubleVect& ,
                                                int le_poly,
                                                int ncomp) const
{
  double val;
  if (ncomp > nb_compo_)
    {
      Cerr << "Error TRUST in Champ_Fonc_Morceaux::valeur_a_elem_compo()" << finl;
      Cerr << "the integer ncomp is upper than the number of field components" << finl;
      exit();
    }

  const DoubleTab& ch = valeurs();

  if (nb_compo_ == 1)
    val = ch(le_poly,0);
  else
    val = ch(le_poly,ncomp);

  return val;
}


// Description:
//    Renvoie les valeurs du champ aux points specifies
//    par leurs coordonnees.
// Precondition:
// Parametre: DoubleTab& positions
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& valeurs
//    Signification: le tableau des valeurs du champ aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ aux points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_Fonc_Morceaux::valeur_aux(const DoubleTab& positions,
                                           DoubleTab& tab_valeurs) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect les_polys(la_zone.nb_elem());
  la_zone.chercher_elements(positions,les_polys);
  return valeur_aux_elems(positions,les_polys,tab_valeurs);
}


// Description:
//    Renvoie les valeurs d'une composante du champ aux points specifies
//    par leurs coordonnees.
// Precondition:
// Parametre: DoubleTab& positions
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& valeurs
//    Signification: le tableau des valeurs de la composante du champ
//                   aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect&
//    Signification: le tableau des valeurs de la composante du champ
//                   aux points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Fonc_Morceaux::valeur_aux_compo(const DoubleTab& positions,
                                                  DoubleVect& tab_valeurs, int ncomp) const
{
  const Zone& la_zone = mon_domaine->zone(0);
  IntVect les_polys(la_zone.nb_elem());
  la_zone.chercher_elements(positions,les_polys);
  return valeur_aux_elems_compo(positions,les_polys,tab_valeurs,ncomp);
}


// Description:
//    Renvoie les valeurs du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de
//    calculs sont situes dans les elements indiques.
// Precondition:
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: IntVect& les_polys
//    Signification: le tableau des elements dans lesquels sont
//                   situes les points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& val
//    Signification: le tableau des valeurs du champ aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ aux points specifies
//    Contraintes:
// Exception: le tableau des valeurs a plus de 2 entrees (rang > 2)
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_Fonc_Morceaux::valeur_aux_elems(const DoubleTab& ,
                                                 const IntVect& les_polys,
                                                 DoubleTab& val) const
{
  if (val.nb_dim() == 1)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(nb_compo_ == 1);
    }
  else if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo_);
    }
  else
    {
      Cerr << "Error TRUST in Champ_Fonc_Morceaux::valeur_aux_elems()" << finl;
      Cerr << "The DoubleTab val has more than 2 entries" << finl;
      exit();
    }

  int le_poly;

  if (nb_compo_ == 1)
    {
      const DoubleTab& ch = valeurs();
      if ((ch.nb_dim() == 1) && (val.nb_dim() == 1))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly) = 0;
              else
                val(rang_poly) = ch(le_poly);
            }
        }
      else if ((ch.nb_dim() == 1) && (val.nb_dim() == 2))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly,0) = 0;
              else
                val(rang_poly,0) = ch(le_poly);
            }
        }
      else if ((ch.nb_dim() == 2) && (val.nb_dim() == 1))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly) = 0;
              else
                val(rang_poly) = ch(le_poly,0);
            }
        }
      else if ((ch.nb_dim() == 2) && (val.nb_dim() == 2))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly,0) = 0;
              else
                val(rang_poly,0) = ch(le_poly,0);
            }
        }

    }
  else // nb_compo_ > 1
    {
      const DoubleTab& ch = valeurs();

      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            for(int ncomp=0; ncomp<nb_compo_; ncomp++)
              val(rang_poly, ncomp) = 0;
          else
            for(int ncomp=0; ncomp<nb_compo_; ncomp++)
              val(rang_poly, ncomp) = ch(le_poly,ncomp);
        }

    }
  return val;
}


// Description:
//    Renvoie les valeurs d'une composante du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de
//    calculs sont situes dans les elements indiques.
// Precondition:
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: IntVect& les_polys
//    Signification: le tableau des elements dans lesquels sont
//                   situes les points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& val
//    Signification: le tableau des valeurs de la composante du champ
//                   aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: int ncomp
//    Signification: l'index de la composante du champ a calculer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect&
//    Signification: le tableau des valeurs de la composante du champ
//                   aux points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_Fonc_Morceaux::
valeur_aux_elems_compo(const DoubleTab&,
                       const IntVect& les_polys,
                       DoubleVect& val,
                       int ncomp) const
{
  assert(val.size() == les_polys.size());
  int le_poly;

  const DoubleTab& ch = valeurs();

  for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
    {
      le_poly=les_polys(rang_poly);
      if (le_poly == -1)
        val(rang_poly) = 0;
      else
        val(rang_poly) = ch(le_poly,ncomp);
    }
  return val;
}

void Champ_Fonc_Morceaux::mettre_a_jour(double time)
{
  Domaine& le_domaine=mon_domaine.valeur();
  const IntTab& les_elems=le_domaine.zone(0).les_elems();
  const int nb_som_elem = le_domaine.zone(0).nb_som_elem();

  const Champ_base& ch=ref_pb.valeur().get_champ(nom_champ_parametre_);

  DoubleTab& tab = valeurs();

  for (int i = 0; i < le_domaine.zone(0).nb_elem_tot(); i++)
    {
      /* xs : coordonnees du poly par barycentre des sommets -> pas top */
      double xs[3] = { 0, };
      int nb_som = 0, s, r;
      for (int j = 0; j < nb_som_elem && (s = les_elems(i, j)) >= 0; j++)
        for (r = 0, nb_som++; r < dimension; r++) xs[r] += le_domaine.coord(s, r);
      for (r = 0; r < dimension; r++) xs[r] /= nb_som;

      /* calcul de chaque composante */
      double val = ch.valeurs().nb_dim() > 1 ? ch.valeurs()(i, 0): ch.valeurs()(i);
      for (int k = 0; k < tab.dimension(1); k++)
        {
          Parser_U& psr = parser(parser_idx(i, k));
          psr.setVar("x", xs[0]);
          psr.setVar("y", xs[1]);
          psr.setVar("z", xs[2]);
          psr.setVar("t", time);
          psr.setVar("val", val);
          tab(i, k) = psr.eval();
        }
    }
}

int Champ_Fonc_Morceaux::initialiser(const double& time)
{
  mettre_a_jour(time);
  return 1;
}


