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

#include <Champ_Uniforme_Morceaux.h>

Implemente_instanciable(Champ_Uniforme_Morceaux,"Champ_Uniforme_Morceaux",Champ_Don_base);

Sortie& Champ_Uniforme_Morceaux::printOn(Sortie& os) const { return os << valeurs(); }

// Description:
//    Lit les valeurs du champ uniforme par morceaux a partir d'un flot d'entree.
//    On lit le nom du domaine (nom_domaine) le nombre de composantes du champ (nb_comp) la valeur par defaut
//    du champ ainsi que les valeurs sur les sous zones.
//    Format:
//     Champ_Uniforme_Morceaux nom_domaine nb_comp
//     { Defaut val_def sous_zone_1 val_1 ... sous_zone_i val_i }
// Precondition: pour utiliser un objet de type Champ_Uniforme_Morceaux
//               il faut avoir defini des objets de type Sous_Zone
Entree& Champ_Uniforme_Morceaux::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  int k, poly;
  is >> nom;
  interprete_get_domaine(nom);
  Domaine& le_domaine=mon_domaine.valeur();
  const IntTab& les_elems=le_domaine.zone(0).les_elems();
  const int nb_som_elem = le_domaine.zone(0).nb_som_elem();
  double x=0,y=0,z=0;

  dim=lire_dimension(is,que_suis_je());
  fixer_nb_comp(dim);
  valeurs_.resize(0, dim);
  le_domaine.creer_tableau_elements(valeurs_);
  is >> nom;
  motlu=nom;
  if(motlu != Motcle("{") )
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected a { instead of " << nom << finl;
      exit();
    }
  is >> nom;
  motlu=nom;
  if(motlu != Motcle("defaut") )
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected defaut instead of " << nom << finl;
      exit();
    }

  VECT(Parser_U) fxyz(dim);
  for( k=0; k< dim; k++)
    {
      Nom tmp;
      is >> tmp;
      fxyz[k].setNbVar(3);
      fxyz[k].setString(tmp);
      fxyz[k].addVar("x");
      fxyz[k].addVar("y");
      fxyz[k].addVar("z");
      fxyz[k].parseString();
    }

  for( poly=0; poly<le_domaine.zone(0).nb_elem(); poly++)
    {
      x = y = z = 0;
      int nsom = 0, e=-1;
      for (int isom = 0; isom<nb_som_elem; isom++)
        if ((e = les_elems(poly,isom)) >= 0)
          {
            x += le_domaine.coord(e,0);
            y += le_domaine.coord(e,1);
            if (dimension == 3) z += le_domaine.coord(e,2);
            nsom++;
          }
      x = x/nsom;
      y = y/nsom;
      z = z/nsom;
      for( k=0; k< dim; k++)
        {
          fxyz[k].setVar("x",x);
          fxyz[k].setVar("y",y);
          fxyz[k].setVar("z",z);
          valeurs_(poly,k)=fxyz[k].eval();
        }
    }

  is >> nom;
  motlu=nom;
  while (motlu != Motcle("}") )
    {
      REF(Sous_Zone) refssz=les_sous_zones.add(le_domaine.ss_zone(nom));
      Sous_Zone& ssz = refssz.valeur();
      for( k=0; k< dim; k++)
        {
          Nom tmp;
          is >> tmp;
          fxyz[k].setString(tmp);
          fxyz[k].parseString();
        }
      for( poly=0; poly<ssz.nb_elem_tot(); poly++)
        {
          x = y = z = 0;
          int nsom = 0, e=-1;
          for (int isom = 0; isom<nb_som_elem; isom++)
            if ((e = les_elems(ssz(poly),isom)) >= 0)
              {
                x += le_domaine.coord(e,0);
                y += le_domaine.coord(e,1);
                if (dimension == 3) z += le_domaine.coord(e,2);
                nsom++;
              }
          x = x/nsom;
          y = y/nsom;
          z = z/nsom;
          for( k=0; k< dim; k++)
            {
              fxyz[k].setVar("x",x);
              fxyz[k].setVar("y",y);
              fxyz[k].setVar("z",z);
              valeurs_(ssz(poly),k)=fxyz[k].eval();
            }
        }
      is >> nom;
      motlu=nom;
    }
  valeurs().echange_espace_virtuel();
  return is;
}

Champ_base& Champ_Uniforme_Morceaux::affecter_(const Champ_base& ch)
{
  if (sub_type(Champ_Uniforme_Morceaux, ch))
    {
      const Champ_Uniforme_Morceaux& chum = ref_cast(Champ_Uniforme_Morceaux, ch);
      les_sous_zones = chum.sous_zones();
      mon_domaine = chum.domaine();
      valeurs_ = chum.valeurs();
    }
  else if (sub_type(Champ_Uniforme, ch))
    {
      const Champ_Uniforme& chu = ref_cast(Champ_Uniforme, ch);
      const int nb_poly = valeurs_.dimension(0), dim = valeurs_.dimension(1);
      const DoubleTab& ch_val = chu.valeurs();
      for (int k = 0; k < dim; k++)
        for (int poly = 0; poly < nb_poly; poly++) valeurs_(poly, k) = ch_val(0, k);
    }
  else
    {
      Cerr << "It is not known to affect a " << ch.que_suis_je() << " at a " << que_suis_je() << finl;
      Process::exit();
    }
  return *this;
}
