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

#include <Lire_Fichier.h>
#include <Interprete_bloc.h>
#include <EFichierBin.h>
#include <LecFicDiffuse_JDD.h>
#include <LecFicDiffuse.h>
#include <Read_unsupported_ASCII_file_from_ICEM.h>

Implemente_instanciable(Lire_Fichier,"Lire_Fichier|Read_file",Interprete);

// Description: Pas code: exit()
Sortie& Lire_Fichier::printOn(Sortie& os) const
{
  Cerr << "Error in Lire_Fichier::printOn()" << finl;
  exit();
  return os;
}

// Description: Pas code: exit()
Entree& Lire_Fichier::readOn(Entree& is)
{
  Cerr << "Error in Lire_Fichier::readOn()" << finl;
  exit();
  return is;
}

// Description:
//  Deux syntaxes possibles dans le jeu de donnees:
//   Lire_fichier NOM_OBJET NOM_FICHIER
//    (lit le contenu du fichier dans l'objet avec readOn de l'objet)
//   Lire_fichier NOM_FICHIER ;
//    (interprete le fichier dans un interprete local: les objets declares
//     dans le fichier sont detruits a la fin de la lecture du fichier)
Entree& Lire_Fichier::interpreter(Entree& is)
{
  Nom nom1, nom2;
  is >> nom1 >> nom2;
  if (nom2 != ";")
    {
      if (is_a_binary_file(nom2))
        {
          check_ICEM_binary_file(nom2,nom1);
          Cerr << "Lire_Fichier: reading binary file " << nom2 << " to object " << nom1 << finl;
          Objet_U& ob1=objet(nom1);
          EFichierBin fic(nom2);
          if(!fic.get_ifstream())
            {
              Cerr << "Unable to open the file " << nom2 << finl;
              Cerr << "Enter a different file name please ... ";
              exit();
            }
          fic >> ob1;
        }
      else
        {
          check_ICEM_ascii_file(nom2,*this);
          Cerr << "Lire_Fichier: reading ASCII file " << nom2 << " to object " << nom1 << finl;
          LecFicDiffuse fic(nom2);
          // On ne peut pas activer check_types car certains fichiers geom ont un format louche
          // ou tout est colle comme ceci : -0.12000000E+01-0.25000000E+00-0.25000000E+00
          // (exemple croix.geom)
          fic.set_check_types(0);
          Objet_U& ob1 = objet(nom1);
          fic >> ob1;
        }
    }
  else
    {
      Cerr << "Lire_Fichier: interpreting file " << nom1 << finl;
      LecFicDiffuse_JDD data_file(nom1);
      data_file.set_check_types(1);
      // On cree un nouvel interprete. A la fin de la lecture
      // les objets seront detruits.
      Interprete_bloc interp;
      interp.interpreter_bloc(data_file,
                              Interprete_bloc::BLOC_EOF /* fin du bloc a la fin du fichier */,
                              0 /* verifie_sans_interpreter=0 */);
      Cerr << "Lire_Fichier: end of file " << nom1 << finl;
    }
  Cerr << "Lire_Fichier: end of file " << nom1 << finl;
  return is;
}
