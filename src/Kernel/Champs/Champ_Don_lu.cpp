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
// File:        Champ_Don_lu.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/24
//
//////////////////////////////////////////////////////////////////////////////

#include <LecFicDiffuse.h>
#include <Champ_Don_lu.h>

Implemente_instanciable(Champ_Don_lu,"Champ_Don_lu",TRUSTChamp_Don_generique<Champ_Don_Type::LU>);

Sortie& Champ_Don_lu::printOn(Sortie& os) const { return TRUSTChamp_Don_generique::printOn(os); }

Entree& Champ_Don_lu::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  is >> nom;

  Domaine& domaine = interprete_get_domaine(nom);
  const Zone& ma_zone = domaine.zone(0);
  int nb_elems = ma_zone.nb_elem();
  dim = lire_dimension(is, que_suis_je());

  dimensionner(nb_elems, dim);
  domaine.creer_tableau_elements(valeurs());

  is >> nom;
  LecFicDiffuse fic(nom);
  Cerr << "Open and read the file : " << nom << finl;

  int nb_val_lues = 0;
  Nom chaine;
  fic >> chaine;
  int star_ccm = 0;
  // Est ce un fichier .csv de STAR-CCM  avec en tete: "champ1 unite" "champ2 unite" ....  "X" "Y" "Z"
  if (chaine.debute_par("\""))
    {
      star_ccm=1;
      Cerr << "STAR-CCM csv file detected..." << finl;
      Cerr << "Find component 1 of the field: " << chaine << finl;
      for (int i=0; i<dim; i++)
        {
          fic >> chaine; // Unite du champ
          fic >> chaine;
          if (i<dim-1) Cerr << "Find component " << i+2 << " of the field: " << chaine << finl;
        }
      if (chaine!="\"X\"")
        {
          Cerr << "Error in the file. There must be one field with " << dim  << " components." << finl;
          Process::exit();
        }
      fic >> chaine; // "Y"
      fic >> chaine; // "Z"
      // Determination du nombre de lignes du fichier STAR-CCM
      LecFicDiffuse tmp(nom);
      while (!tmp.eof())
        {
          nb_val_lues++;
          tmp >> chaine;
        }
      nb_val_lues-=(dim*2+dimension+1);
      nb_val_lues/=(dim+dimension);
    }
  else
    {
      // Sinon nombre de lignes a lire
      nb_val_lues=atoi(chaine);
      Cerr << "The format of this file containing values of a field should be:" << finl;
      Cerr << "N" << finl;
      int nb_lines=4;
      for (int line=1; line<=nb_lines; line++)
        {
          if (line==nb_lines-1) Cerr << "..." << finl;
          else
            {
              Nom l;
              if (line==nb_lines) l="N";
              else l=(Nom)line;

              Cerr << "x["<<l<<"] y["<<l<<"] ";
              if (dimension==3) Cerr << "z["<<l<<"] ";
              for (int k=0; k<dim; k++)
                Cerr << "value["<<l<<","<<k<<"] ";
              Cerr << finl;
            }
        }
    }
  Cerr << finl << "Read number of values (N)=" << nb_val_lues << finl;
  // Lecture des valeurs dans le fichier fic
  DoubleTab& mes_val = valeurs();
  IntTab compteur(nb_elems);
  compteur = 0;
  DoubleVect point(dimension);
  DoubleVect val_lu(dim);
  int elem2;
  double x,y,z=0;
  int pourcent=0,tmp;
  for (int i=0; i<nb_val_lues; i++)
    {
      tmp=(i+1)*10/nb_val_lues;
      if (tmp>pourcent)
        {
          pourcent=tmp;
          Cerr<<"\rProcess "<<Process::me()<<" has read "<<pourcent*10<<"% of values"<<flush;
        }

      // Lecture des coordonnees du centre de l'element et des valeurs du champs
      if (star_ccm)
        {
          for (int k=0; k<dim; k++) fic >> val_lu[k];
          for (int k=0; k<dimension; k++) fic >> point[k];
        }
      else
        {
          for (int k=0; k<dimension; k++) fic >> point[k];
          for (int k=0; k<dim; k++) fic >> val_lu[k];
        }
      x=point[0];
      y=point[1];
      if (Objet_U::dimension==3) z=point[2];

      // Recherche de l'element
      elem2=ma_zone.chercher_elements(x,y,z);
      if ((elem2!=-1) && (elem2<nb_elems))
        {
          for (int k=0; k<dim; k++) mes_val(elem2,k) = val_lu[k];
          compteur[elem2] += 1;
        }
    }
  Cerr << finl;
  // Verification que tous les elements ont ete trouves
  for (int num_elem=0; num_elem<nb_elems; num_elem++)
    if (compteur[num_elem] == 0)
      {
        Cerr << "Error when using Champ_Don_lu keyword:" << finl;
        Cerr << "There is no value found for the cell number " << num_elem << finl;
        Cerr << "Check your mesh and/or the file " << nom << finl;
        Process::exit();
      }

  mes_val.echange_espace_virtuel();

  return is;
}
