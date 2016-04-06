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
// File:        Champ_front_lu.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Champs
// Version:     /main/28
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_front_lu.h>
#include <Domaine.h>
#include <LecFicDiffuse.h>
#include <Interprete.h>
#include <Zone_VF.h>

Implemente_instanciable(Champ_front_lu,"Champ_front_lu",Ch_front_var_stationnaire);



// Description:
//    Imprime le champ sur flot de sortie.
//    Imprime la taille du champ et la valeur (constante) sur
//    la frontiere.
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
Sortie& Champ_front_lu::printOn(Sortie& os) const
{
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}


// Description:
//    Lit le champ a partir d'un flot d'entree.
//    Format:
//      Champ_front_lu nb_compo vrel_1 ... [vrel_i]
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree& is
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le champ analytique a la valeur lue
Entree& Champ_front_lu::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  is >> nom;

  mon_domaine = ref_cast(Domaine, Interprete::objet(nom));



  dim=lire_dimension(is,que_suis_je());
  fixer_nb_comp(dim);
  is >> nom_fic;
  // Lecture des valeurs dans le fichier fic
  return is;
}

void erreur_chfront(const Nom& nom_fic, const int& dim, const Nom& nom_domaine)
{
  Cerr << "****************************************************************************************************************" << finl;
  Cerr << "Error! It seems that several faces into the file " << nom_fic << " does not match any faces of the current mesh." << finl;
  Cerr << finl;
  Cerr << "First, check that the file " << nom_fic << " can be used to define a boundary field on the specified boundary of the current mesh." << finl;
  Cerr << "Second, try to switch to a different algorithm in your data file (find the nearest geometric face of the mesh for each face defined in the " << nom_fic << " file):" << finl;
  Cerr << "Replace:" <<finl;
  Cerr << "Champ_front_lu " << nom_domaine << " " << dim << " " << nom_fic << finl;
  Nom un((Nom)dim),zero((Nom)dim);
  for (int i=0; i<dim; i++)
    {
      un+=" 1";
      zero+=" 0";
    }
  Cerr << "By:"<<finl;
  Nom chaine="moyenne_imposee connexion_approchee fichier";
  Cerr << "Champ_front_recyclage { pb_champ_evaluateur problem_name field_name "<<dim<<finl;
  Cerr << "                        "<<chaine<<" "<<nom_fic<<finl;
  Cerr << "                        ampli_moyenne_imposee "<<un<<finl;
  Cerr << "                        ampli_moyenne_recyclee "<<zero<<finl;
  Cerr << "                        ampli_fluctuation "<<zero<<" }" <<finl;
  Process::exit();
}

int Champ_front_lu::initialiser(double temps, const Champ_Inc_base& inco)
{

  if (!Ch_front_var_stationnaire::initialiser(temps,inco))
    return 0;

  int dim=nb_comp();
  Domaine& domaine=mon_domaine.valeur();
  const Frontiere_dis_base& fr_dis=frontiere_dis();
  const Frontiere& frontiere=fr_dis.frontiere();
  const int nb_faces=frontiere.nb_faces();
  const int ndeb=frontiere.num_premiere_face();
  DoubleTab& les_val=valeurs();

  LecFicDiffuse fic;
  int bin=is_bin(nom_fic);
  fic.set_bin(bin);
  fic.ouvrir(nom_fic);
  Nom format;
  if (bin)
    fic >> format;
  else
    format="ascii";

  Cerr << "Reading of the " << format << " file : " << nom_fic << finl;

  DoubleVect point(dimension);
  DoubleVect val_lu(dim);
  int nb_val_lues;
  fic >> nb_val_lues;

  // Verification (il se peut que le fichier contienne PLUS de faces que la frontiere, exemple en periodique)
  int nb_faces_tot = mp_sum(nb_faces);
  if (nb_val_lues<nb_faces_tot)
    {
      Cerr << "This file contains " << nb_val_lues << " faces which is not enough compare to" << finl;
      Cerr << "the faces number (" << nb_faces_tot << ") of the boundary " << frontiere.le_nom() << finl;
      exit();
    }

  // PQ 20/06/06 : pour permettre une recherche "rapide" des faces
  //                 on s'appuie sur ma_zone.chercher_elements(x,y,z)
  //                 (methode inspiree de EcritureLectureSpecial::lecture_special)
  //
  //                Restriction --> champs aux faces

  const Zone& ma_zone = domaine.zone(0);
  int nb_elems = ma_zone.nb_elem();
  int nbfacelem=ma_zone.nb_faces_elem();
  const Zone_VF& zvf = ref_cast(Zone_VF, zone_dis());
  const IntTab& elem_faces=zvf.elem_faces();
  const DoubleTab& xv=zvf.xv();
  IntTab compteur(nb_faces);
  compteur=0;

  int k;
  double x,y,z=0;
  int pourcent=0,tmp;

  for (int p=0; p<nb_val_lues; p++)
    {
      tmp=(p*10)/nb_val_lues;
      if (tmp>pourcent)
        {
          pourcent=tmp;
          Cerr<<"\rProcess "<<Process::me()<<" has read "<<pourcent*10<<"% of values"<<flush;
        }
      for (k=0; k<dimension; k++)
        fic >> point[k];
      for (k=0; k<dim; k++)
        fic >>val_lu[k];

      x=point[0];
      y=point[1];
      if (Objet_U::dimension==3) z=point[2];
      int Case=-1;
      // Premier algorithme (le plus rapide) cherchant l'element
      // contenant le centre de gravite de la face
      int num_elem=ma_zone.chercher_elements(x,y,z);
      if ((num_elem!=-1) && (num_elem<nb_elems))
        {
          // On cherche la face de l'element
          for (int fac=0; fac<nbfacelem; fac++)
            {
              int face=elem_faces(num_elem,fac);
              int ok=1;
              for (int dir=0; dir<dimension; dir++)
                {
                  ok=ok&&(est_egal(point[dir],xv(face,dir)));
                }
              if (ok)
                {
                  Case=face;
                  break;
                }
            }
        }
      else
        {
          // Deuxieme algorithme (plus lent) utilise dans le cas ou le point se trouve a l'exterieur du domaine
          // Dans ce cas la, on parcours les faces de la frontiere
          for (int face=0; face<nb_faces; face++)
            if (compteur[face] == 0)
              {
                int ok=1;
                for (int dir=0; dir<dimension; dir++)
                  ok=ok&&(est_egal(point[dir],xv(ndeb+face,dir)));
                if (ok)
                  {
                    Case=ndeb+face;
                    break;
                  }
              }
        }
      // Face trouvee
      if (Case!=-1)
        {
          int face_loc=Case-ndeb;
          if ((face_loc<0) || (face_loc>=nb_faces))
            {
              Cerr << " The face " << ndeb << " found doesn't belong to the boundary sought " << finl;
              exit();
            }
          compteur[face_loc] += 1;
          for (k=0; k<dim; k++)
            les_val(face_loc,k) = val_lu[k];
        }
    }
  Cerr<<"\rProcess "<<Process::me()<<" has read 100% of values"<<finl;

  // On verifie que toutes les faces de la frontiere ont ete affectees
  int err=0;
  for (int face=0; face<nb_faces; face++)
    if (compteur[face] != 1)
      {
        Cerr.precision(12);
        if (compteur[face] == 0)
          Cerr << "No value found for the face " << face << "/" << nb_faces-1 << " of the boundary " << frontiere.le_nom() << " on the processor " << Process::me() << ".";
        else
          Cerr << "Several valeurs found for the face " << face << "/" << nb_faces-1 << " of the boundary " << frontiere.le_nom() << " on the processor " << Process::me() << ".";
        Cerr << " Coordinates: " << xv(ndeb+face,0) << " " << xv(ndeb+face,1);
        if (dimension==3) Cerr << " " << xv(ndeb+face,2);
        Cerr << finl;
        err=1;
      }
  if (Process::mp_sum(err))
    {
      erreur_chfront(nom_fic, dim, mon_domaine.le_nom());
    }
  les_val.echange_espace_virtuel();
  return 1;
}

// Description:
//    Renvoie le format du fichier lu
// Precondition:
// Parametre: Nom name
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_front_lu::is_bin(Nom name)
{
  EFichier fichier(name);
  Nom format;
  fichier >> format;

#ifdef INT_is_64_
  // In case of INT_64, first Nom can be INT64 ...
  if(format=="INT64")
    format="binary";
#endif

  if(format=="binary")
    return 1;
  else
    return 0;
}
