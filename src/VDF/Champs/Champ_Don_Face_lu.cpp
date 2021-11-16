/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Champ_Don_Face_lu.cpp
// Directory:   $TRUST_ROOT/src/VDF/Champs
// Version:     /main/12
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Don_Face_lu.h>
#include <Probleme_base.h>
#include <Zone_VDF.h>
#include <EFichier.h>
#include <Interprete.h>
#include <Motcle.h>
#include <ArrOfBit.h>

// Fonction utilisee par le readOn
int meme_point(const ArrOfInt& x, const DoubleTab& tab, int rang_tab)
{
  int dime = x.size_array();
  assert(tab.dimension(1) == dime);
  int k;
  for (k=0; k<dime; k++)
    if (fabs(x(k)-tab(rang_tab,k)) >= 1.e-12)
      break;
  return (k == dime);
}

Implemente_instanciable(Champ_Don_Face_lu,"Champ_Don_Face_lu",Champ_Fonc_Face);


// Description:
//    Imprime les valeurs du champ sur un flot de sortie
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
Sortie& Champ_Don_Face_lu::printOn(Sortie& os) const
{
  const DoubleTab& tab=valeurs();
  os << tab.size() << " ";
  for(int i=0; i<tab.size(); i++)
    os << tab(0,i);
  return os;
}


// Description:
//    Lit le champ sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: impossible d'ouvrir le fichier
// Exception: pas de valeur pour l'element lu
// Effets de bord:
// Postcondition: le champ est initialise avec les donnees lues
Entree& Champ_Don_Face_lu::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  is >> nom;
  Probleme_base& pb= ref_cast(Probleme_base, Interprete::objet(nom));
  associer_zone_dis_base(pb.domaine_dis().zone_dis(0).valeur());
  const Zone_VDF& zvdf = ref_cast(Zone_VDF,zone_dis_base());
  int nb_faces = zvdf.nb_faces();
  is >> dim;
  dimensionner(nb_faces, dim);
  is >> nom;
  if (Process::je_suis_maitre())
    Cerr << "Champ_Don_Face_lu: lecture du fichier " << nom << finl;
  EFichier fic(nom);
  fic.set_check_types(1);//Remplace UFichier

  // Lecture des valeurs dans le fichier fic
  DoubleTab& mes_val = valeurs();
  const DoubleTab& xv = zvdf.xv();
  ArrOfInt point(dimension);
  ArrOfInt val_lu(dim);
  ArrOfBit flag(nb_faces);
  flag = 0;

  int nb_val_lues = 0;
  fic >> nb_val_lues;
  int num_face = -1;
  int nb_faces_affectees = 0;
  int i;
  for (i = 0; i < nb_val_lues; i++)
    {
      int k;
      for (k=0; k<dimension; k++)
        fic >> point[k];
      for (k=0; k<dim; k++)
        fic >> val_lu[k];

      // On cherche la face correspondante en partant de la derniere trouvee.
      for (int j = 0; j < nb_faces; j++)
        {
          num_face++;
          if (num_face >= nb_faces)
            num_face = 0;

          if (meme_point(point,xv,num_face))
            {
              if (dim == 1)
                mes_val(num_face) = val_lu[0];
              else
                for (k=0; k < dim; k++)
                  mes_val(num_face,k) = val_lu[k];
              if (! flag.testsetbit(num_face))
                nb_faces_affectees++;
              break;
            }
        }
    }

  int erreur = mp_sum(nb_faces - nb_faces_affectees);
  if (erreur)
    {
      if (Process::je_suis_maitre())
        Cerr << "Erreur dans Champ_Don_Face_lu.\n"
             << erreur << " faces n'ont pas ete affectees.\n"
             << "Voir la liste des faces dans les fichiers .log" << finl;
      Journal() << "Champ_Don_Face_lu. Liste des faces non affectees :\n";
      for (i=0; i<nb_faces; i++)
        if (!flag[i])
          Journal() << i << " " << xv(i, 0) << " " << xv(i, 1)
                    << " " << ((dimension == 3) ? xv(i, 3) : 0.) << finl;
      barrier(); // Attend que tout le monde ait fini d'ecrire
      exit();
    }
  return is;
}
