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
// File:        Read_unsupported_ASCII_file_from_ICEM.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Utilitaires
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Read_unsupported_ASCII_file_from_ICEM.h>
#include <Lire_Fichier.h>
#include <SFichierBin.h>
#include <EFichierBin.h>
#include <Interprete.h>
#include <TRUSTTab.h>
#include <Objet_U.h>
#include <Motcle.h>

Implemente_instanciable(Read_unsupported_ASCII_file_from_ICEM,"Read_unsupported_ASCII_file_from_ICEM",Lire_Fichier);

Sortie& Read_unsupported_ASCII_file_from_ICEM::printOn(Sortie& os) const
{
  return os;
}

Entree& Read_unsupported_ASCII_file_from_ICEM::readOn(Entree& is)
{
  return is;
}

// Fonction utilisee
inline char read_write_string_from(EFichierBin& s,SFichierBin& o, const char* first_caracter)
{
  // La regle est :
  // On lit jusqu'a first_caracter s'il est defini
  // On complete par le separateur 00
  // On retourne le dernier caractere (avant l'espace)
  char octet;
  char zero=0;
  char last_caracter;
  // Si first_caracter est defini on lit jusqu'a lui
  if (*first_caracter!=0)
    {
#ifndef NDEBUG
      Cerr << " -> Look for the byte:";
      fprintf(stderr," %02x <=> ",*first_caracter);
      Cerr << (Nom)*first_caracter << finl;
      Cerr << " -> Read the bytes :";
#endif
      //while (s.good() && s.get_istream().read(&octet,1)!=0 && octet!=*first_caracter)
      while (s.good() && s.get_istream().read(&octet,1) && octet!=*first_caracter)
        {
#ifndef NDEBUG
          fprintf(stderr," %02x",octet);
#endif
        };
    }
  else
    {
      // Si first caracter n'est pas defini, on se contente de passer
      // les espaces (32, 20 en hexa) et separateurs (00) et delete (127, 7F en hexa)
#ifndef NDEBUG
      Cerr << " -> Read the bytes :";
#endif
      //while (s.good() && s.get_istream().read(&octet,1)!=0 && (octet==32 || octet==0 || octet==127) )
      while (s.good() && s.get_istream().read(&octet,1) && (octet==32 || octet==0 || octet==127) )
        {
#ifndef NDEBUG
          fprintf(stderr," %02x",octet);
#endif
        };
    }
#ifndef NDEBUG
  fprintf(stderr," %02x",octet);
#endif
  o.get_ostream().write(&octet,1);
  last_caracter = octet;
  Nom chaine(octet);
  // On lit jusqu'a l'espace:
  //while (s.good() && s.get_istream().read(&octet,1)!=0 && octet!=32)
  while (s.good() && s.get_istream().read(&octet,1) && octet!=32)
    {
#ifndef NDEBUG
      fprintf(stderr," %02x",octet);
#endif
      o.get_ostream().write(&octet,1);
      last_caracter = octet;
      chaine+=octet;
    }
#ifndef NDEBUG
  fprintf(stderr," %02x",octet);
#endif
  o.get_ostream().write(&zero,1);
#ifndef NDEBUG
  Cerr << finl;
  Cerr << " -> Read the string: " << chaine << finl;
#endif
  return last_caracter;
}
// Fonction qui verifie si on lit un binaire d'ICEM et si oui
// cree un fichier compatible TRUST
void check_ICEM_binary_file(Nom& filename, const Nom& nom_objet_lu)
{
  // On verifie que l'objet lu est un domaine
  Objet_U& objet_lu = Interprete::objet(nom_objet_lu);
  if (objet_lu.que_suis_je()!="Domaine") return;

  EFichierBin tmp(filename);
  // Un fichier binaire ASCII est reconnaissable par le fait que le nom du
  // domaine est suivi d'un espace donc l'octet est 32 (= 20 en hexa = space)
  // Dans un binaire classique le nom du domaine est suivi par 0
  char octet=-1;
  while(octet!=32 && octet!=0)
    tmp.get_istream().read(&octet,1);
  tmp.close();
  if (octet==32) // C'est un fichier binaire ICEM
    {
      char zero=0;
      Cerr << "==============================================" << finl;
      Cerr << filename << " is an ICEM binary file." << finl;
      Cerr << "To save space, you can now delete this file and use" << finl;
      // Suppression de certains espaces du fichier binaire
      Nom new_filename(filename);
      new_filename+=".cleaned";
      Cerr << "instead, the newly created " << new_filename << " file." << finl;
      Cerr << "==============================================" << finl;
      EFichierBin s(filename);
      SFichierBin o(new_filename);
      s.get_istream().read(&octet,1);
      while(octet!=32)
        {
          o.get_ofstream().write(&octet,1);
          s.get_istream().read(&octet,1);
        }
      // Ecriture de 00 a la place de 32
      o.get_ostream().write(&zero,1);
      // Ensuite on va jusqu'a l'entier 2
      s.get_istream().read(&octet,1);
      while(octet!=2)
        s.get_istream().read(&octet,1);
      // On revient en arriere et on peut commencer a lire le DoubleTab
      s.get_istream().unget();
      // Lecture des sommets
      DoubleTab sommets;
      s >> sommets;
      o << sommets;
      Cerr << "End of the read of the nodes." << finl;
      // "{"
      if (read_write_string_from(s,o,"{")!=123)
        {
          Cerr << "Error in is_a_ICEM_binary_file : { is waited." << finl;
          Process::exit();
        }
      // Nom du domaine
      read_write_string_from(s,o,"");
      // Lecture du type d'element
      read_write_string_from(s,o,"T"); // TETRAEDRE
      // Lecture des elements
      IntTab elems;
      s >> elems;
      o << elems;
      Cerr << "End of the read of the cells." << finl;
      // Boucle sur les frontieres
      //  {  : 7b en hexa, 123 en decimal
      //  ,  : 2C en hexa, 44  en decimal
      //  }  : 7D en haxe, 125 en decimal
      const char* sep="{";
      while(read_write_string_from(s,o,sep)!=125)
        {
          // Nom de frontiere
          read_write_string_from(s,o,"");
          // Type de l'element
          read_write_string_from(s,o,"T"); // TRIANGLE_3D
          // Elements de la frontiere
          s >> elems;
          o << elems;
          // Faces voisines
          IntTab faces_voisins;
          s >> faces_voisins;
          o << faces_voisins;
          Cerr << "End of the read of a boundary." << finl;
          sep=""; // , ou } on ne peut savoir a l'avance...
        }
      // Lecture des 3 "vide"
      for (int i=0; i<3; i++)
        {
          if (read_write_string_from(s,o,"v")!=101)
            {
              Cerr << "Error in is_a_ICEM_binary_file : 'vide' is waited." << finl;
              Process::exit();
            }
        }
      // Lecture de }
      if (read_write_string_from(s,o,"}")!=125)
        {
          Cerr << "Error in is_a_ICEM_binary_file : } is waited." << finl;
          Process::exit();
        }
      // Lecture du dernier "vide"
      read_write_string_from(s,o,"v");
      o.close();
      s.close();
      // On pointe vers le nouveau fichier
      filename=new_filename;
      Cerr << "=============================================" << finl;
      Cerr << "You can use now: " << finl;
      Cerr << "Lire_fichier " << nom_objet_lu << " "<< filename  << finl;
      Cerr << "=============================================" << finl;
    }
}

void check_ICEM_ascii_file(const Nom& filename, const Lire_Fichier& keyword)
{
  Nom ICEM(filename);
  if (ICEM.finit_par(".asc") && !sub_type(Read_unsupported_ASCII_file_from_ICEM,keyword))
    {
      Cerr << "Since the 1.6.7 version, the read of an ASCII file from ICEM mesh tool is not" << finl;
      Cerr << "supported anymore cause insufficient number of digits to define the node coordinates." <<finl;
      Cerr << "Please, use the TRUST binary export from ICEM." << finl;
      Process::exit();
    }
  else if (sub_type(Read_unsupported_ASCII_file_from_ICEM,keyword))
    {
      Cerr << "====================================================" << finl;
      Cerr << "Warning: You are using an obsolete feature of TRUST" << finl;
      Cerr << "with Unsupported_ASCII_file_read_from_ICEM keyword." << finl;
      Cerr << "Use binary format for ICEM file!" << finl;
      Cerr << "====================================================" << finl;
    }
}
