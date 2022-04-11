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
// File:        Lire_Tgrid.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/20
//
//////////////////////////////////////////////////////////////////////////////

#include <Lire_Tgrid.h>
#include <EFichier.h>
#include <Domaine.h>
#include <math.h>
#include <RefFrontiere.h>
#include <NettoieNoeuds.h>
#include <ctype.h>

// Methode a remonter dans une classe au dessus de toutes les classes lire_...
// Ou a placer dans le constructeur d'EFichier avec eventuellement un
// effacement du fichier decompresse dans le desctructeur d'EFichier
inline void decompression(Nom& nom_fichier)
{
  Nom tmp(nom_fichier);
  if (tmp.prefix(".gz")!=nom_fichier)
    {
      Cerr << "Unzipping file " << nom_fichier << " ..." << finl;
      Nom cmd="gunzip -c ";
      cmd+=nom_fichier+" > "+tmp;
      Cerr << (int)system(cmd) << finl;
      nom_fichier=tmp;
    }
}
Implemente_instanciable(Lire_Tgrid,"Lire_Tgrid",Interprete_geometrique_base);
// Description:
//  appel a la methode printOn de la classe Interprete
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Lire_Tgrid::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

// Description:
//  appel a la methode readOn de la classe Interprete
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Lire_Tgrid::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

int chartoint(char c)
{
  if( (0>(c-'0')) || ((c-'0')>9) )
    return -1;
  else
    return (c-'0');
}

// Convertit une chaine au format hexa (00aad22) en une valeur entiere
int htoi(const char * szChaine)
{
  int lResult = 0;
  int iLength = 0;
  // Pointeur null, on renvoi -1
  if (szChaine == NULL)
    return -1;
  // On calcule la longueur de la chaine
  iLength = strlen(szChaine);

  // On met la chaine en majuscule dans une nouvelle chaine (allouee par strdup)
  char * szHexaString = strdup(szChaine);
  // Pour chaque caractere on calcule sa valeur
  for (int i = iLength - 1; i >= 0; i--)
    {
      char cCharacter = szHexaString[i];
      int iValue = 0;
      // C'est un digit, on le convertit en entier
      if (isdigit(cCharacter))
        {
          //iValue = atoi(&cCharacter);
          // atoi a l'air mechamment bugge !
          // Je code un chartoint correct !
          iValue = chartoint(cCharacter);
          if (iValue>9)
            {
              Cerr << "iValue is worth " << iValue << " ! " << finl;
              Process::exit();
            }
        }
      // C'est un caractere, on lui associe une valeur entiere
      else if (isalpha(cCharacter))
        {
          switch(cCharacter)
            {
            case 'A' :
            case 'a' :
              iValue = 10;
              break;
            case 'B' :
            case 'b' :
              iValue = 11;
              break;
            case 'C' :
            case 'c' :
              iValue = 12;
              break;
            case 'D' :
            case 'd' :
              iValue = 13;
              break;
            case 'E' :
            case 'e' :
              iValue = 14;
              break;
            case 'F' :
            case 'f' :
              iValue = 15;
              break;
            default  :
              return -3;              // Caractere invalide en Hexa.
            }
        }
      // Ce n'est ni un caractere ni un digit.
      else
        return -4;   // Caractere non valide.
      //lResult += iValue * pow(16, iLength - i - 1);
      for (int puissance=0; puissance<iLength - i - 1; puissance++)
        iValue *= 16;
      lResult += iValue;
    }
  // Liberation de la chaine allouee
  free (szHexaString);
  //Cerr << lResult << finl;
  return lResult;
}

inline void va_a_la_parenthese_fermante(EFichier& fic)
{
  int parenthese_ouverte=1;
  Nom lu;
  while ((parenthese_ouverte!=0)&&(fic.good()))
    {
      fic >> lu;
      const char* chaine = lu.getChar();
      Process::Journal()<<"|"<<chaine<<"|"<<finl;
      int iLength = strlen(chaine);
      for (int i=0; i<iLength; i++)
        {
          char c = chaine[i];
          switch(c)
            {
            case 40 :
              parenthese_ouverte++;
              break;
            case 41 :
              parenthese_ouverte--;
              break;
            }
        }
    }

  if (parenthese_ouverte!=0)
    {
      Cerr<< "Error in file"<<finl;
      Process::exit();
    }

}
inline void va_a_la_parenthese_ouvrante(EFichier& fic)
{
  int ok=0;
  Nom lu;
  while (ok!=1)
    {
      fic >> lu;
      const char* chaine = lu.getChar();
      int iLength = strlen(chaine);
      for (int i=0; i<iLength; i++)
        {
          char c = chaine[i];
          switch(c)
            {
            case 40 :
              ok=1;
              break;
            }
        }
    }
}

// Description:
//    Lecture d'un fichier
//    Avec 2 arguments nom1 et nom2 , lit l'objet du fichier nom2 dans l'objet nom1
//    Avec un seul argument nom1, interprete le fichier de nom nom1
// Precondition:
// Parametre: Entree& is
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
//
// Derniere modification effectuee par C. MALOD (CS-fontaine : 04-76-85-99-71)
// le 8-02-2006, pour lire egalement le format en 2D.
Entree& Lire_Tgrid::interpreter_(Entree& is)
{
  Cerr << "Reading a mesh which comes from Tgrid" << finl;
  associer_domaine(is);
  Domaine& dom=domaine();
  DoubleTab& coord_sommets=dom.les_sommets();
  // Declaration des variables
  int dim=-1;
  int nb_som;
  int nb_elem;
  int nb_face;
  int nb_som_elem=0;
  int type_elements=0;
  int compteur=0;
  // Tableau de travail
  ArrOfInt nb_som_lu_elem;
  Motcle motlu;
  // Gestion fichier
  Nom nom_fichier;
  is >> nom_fichier;
  decompression(nom_fichier);
  Cerr << "Reading of the file " << nom_fichier << " ..." << finl;
  EFichier lecture(nom_fichier);
  // On teste tout de suite l'existence du fichier car s'il
  // n'existe pas, cela bloque dans le eof();
  if (!lecture.good())
    {
      Cerr << "Problem to open the file " << nom_fichier << finl;
      Cerr << "There is maybe an error in the filename." << finl;
      exit();
    }

  // Premiere lecture du fichier .msh
  //pour trouver le nombre d'element car
  // il est parfois place en fin de fichier !
  while (!lecture.eof())
    {
      lecture >> motlu;
      if (motlu=="(12")
        {
          lecture >> motlu;
          if (motlu=="(0")
            {
              lecture >> motlu;        // Numero premier element
              lecture >> motlu;        // Nombre d'element
              nb_elem=htoi(motlu);
              Cerr << "The total number of elements to read is " << nb_elem << finl;
              lecture >> motlu;        // Type de la zone (0=dead zone; 1=active zone; 32=inactive zone) ou 0))
              if (motlu!="0))") lecture >> motlu; // On saut le type s'il existe
              Zone une_zone;
              Zone& la_zone=dom.add(une_zone);
              la_zone.associer_domaine(dom);
            }
          else if (motlu=="(id") lecture >> motlu;        // evite de lire la description de la balise (12
          else
            {
              lecture >> motlu;        // Numero premier element
              lecture >> motlu;        // Nombre d'element
              lecture >> motlu;        // Type de zone (1:fluid ou 0x11:solid)
              lecture >> motlu;        // Type des elements
              if (motlu=="1))")
                {
                  // On lit des triangles
                  type_elements=1;
                  Cerr << "2D elements of type Triangle" << finl;
                }
              else if (motlu=="3))")
                {
                  // On lit des quadrangles
                  type_elements=3;
                  Cerr << "2D elements of type Quadrangle" << finl;
                }
              else if (motlu=="2))")
                {
                  // On lit des tetraedres
                  type_elements=2;
                  Cerr << "3D elements of type Tetrahedron" << finl;
                }
              else if (motlu=="4))")
                {
                  // On lit des hexaedres
                  type_elements=4;
                  Cerr << "3D elements of type Hexahedron" << finl;
                }
              else
                {
                  // On ne sait pas ce qu'on lit
                  Cerr << "Elements unknown !" << finl;
                  Cerr << "It should probably crashed !!!!!" << finl;
                }
              lecture.close();
            }
        }
    }
  // Deuxieme lecture du fichier .msh
  EFichier fic(nom_fichier);
  while (!fic.eof())
    {
      fic >> motlu;
      if (motlu=="(0")
        {
          Cerr << "Reading a comment:" << finl;
          Cerr << motlu;
          va_a_la_parenthese_fermante(fic);
          Cerr << finl << finl;
        }
      else if (motlu=="(1")
        {
          Cerr << "Reading a header:" << finl;
          Cerr << motlu;
          va_a_la_parenthese_fermante(fic);
          Cerr << finl << finl;
        }
      else if (motlu=="(2")
        {
          Cerr << "Reading of the dimension of the case:" << finl;
          fic >> motlu;
          dim=atoi(motlu.prefix(")"));
          if (dim==3) Cerr << "Dimension 3." << finl;
          else if (dim==2) Cerr << "Dimension 2." << finl;
          else
            {
              Cerr << "Dimension " << dim << " of the mesh not provided." << finl;
              exit();
            }
          Cerr << finl;
        }
      else if (motlu=="(10")
        {
          fic >> motlu;
          if (motlu=="(0")
            {
              fic >> motlu;        // Numero du premier sommet
              fic >> motlu;        // Nombre de sommets
              nb_som=htoi(motlu);
              Cerr << "The total number of nodes to read is " << nb_som << finl;
              // On dimensionne le tableau des sommets
              coord_sommets.resize(nb_som,dim);
              // Depend du format
              fic >> motlu;
              Nom tmp=motlu;
              if (tmp==motlu.prefix("))"))
                fic >> motlu;
            }
          else
            {
              int izone=htoi(motlu.suffix("("));
              fic >> motlu;        // Debut
              int ideb=htoi(motlu);
              fic >> motlu;        // Fin
              int ifin=htoi(motlu);
              Cerr << ifin-ideb+1 << " nodes are read in the area " << izone << finl;
              // Depend du format, on va donc a la paranthese ouvrante
              va_a_la_parenthese_ouvrante(fic);
              /*
                fic >> motlu;        // Type (0: virtual, 1:any, 2:boundary)
                if (motlu.prefix(")"))
                fic >> motlu;        // Dimension
                assert(htoi(motlu.prefix(")"))==dim);
                fic >> motlu;        // ( */

              for (int i=ideb-1; i<ifin; i++)
                for (int j=0; j<dim; j++)
                  fic >> coord_sommets(i,j);
              fic >> motlu;        // ))
            }
          Cerr << finl;
        }
      else if (motlu=="(12")
        {
          fic >> motlu;
          if (motlu=="(0")
            {
              // Informations obtenues lors de la premiere lecture
              fic >> motlu;        // Numero premier element
              fic >> motlu;        // Nombre d'element
              fic >> motlu;        // Type de la zone (0=dead zone; 1=active zone; 32=inactive zone) ou 0))
              if (motlu!="0))") fic >> motlu; // On saut le type s'il existe
            }
          else
            {
              int izone=htoi(motlu.suffix("("));
              dom.zone(0).nommer((Nom)izone);
              fic >> motlu;        // Debut
              //int ideb=htoi(motlu);
              fic >> motlu;        // Fin
              //int ifin=htoi(motlu);
              fic >> motlu;        // Type (1:fluid, Ox11:solid)
              int type=htoi(motlu);
              Cerr << "The type of area " << izone << " is " << type << " (1:fluid, 17:solid)" << finl;
              fic >> motlu;        // Type cell (0:mixed,1:tri,2:tetra,3:quad,4:hexa,5:pyramid,6:wedge)
              if (motlu=="2))")
                {
                  // On lit bien des tetraedres
                  dom.zone(0).type_elem().typer("Tetraedre");
                }
              else if (motlu=="4))")
                {
                  // On lit bien des hexaedres
                  dom.zone(0).type_elem().typer("Hexaedre_VEF");
                }
              else if (motlu=="1))")
                {
                  // On lit bien des triangles
                  dom.zone(0).type_elem().typer("Triangle");
                }
              else if (motlu=="3))")
                {
                  // On lit bien des quadrangles
                  dom.zone(0).type_elem().typer("Quadrangle");
                }
              else
                {
                  Cerr << "Reading the elements is not provided in this interpreter." << finl;
                  Cerr << "Indeed, we read faces to reconstruct the elements." << finl;
                  Cerr << "Contact TRUST support." << finl;
                  exit();
                }
              dom.zone(0).type_elem().associer_zone(dom.zone(0));
            }
          Cerr << finl;
        }
      else if (motlu.debute_par("(13"))
        {
          motlu.suffix("(13");
          if (motlu=="")
            fic >> motlu;
          if (motlu=="(0")
            {
              fic >> motlu;        // Numero de la premiere face
              fic >> motlu;        // Nombre de faces
              nb_face=htoi(motlu);
              Cerr << "The total number of faces to read is " << nb_face << finl;
              fic >> motlu;        // Type de face ou 0))
              if (motlu != "0))") fic >> motlu;        // On saut le type s'il existe
            }
          else
            {
              int izone=htoi(motlu.suffix("("));
              fic >> motlu;        // Debut
              int ideb=htoi(motlu);
              fic >> motlu;        // Fin
              int ifin=htoi(motlu);
              fic >> motlu;        // Type (2: interior, >2: boundary condition): ne semble pas vrai (voir page C-8)!
              int type=htoi(motlu);
              fic >> motlu;        // Type face (0:mixed, 2:linear, 3:triangular, 4:quadrilateral)
              // Depend du format:
              Nom tmp=motlu;
              int nb_som_face,mixte=0;
              if (tmp!=motlu.prefix(")"))
                fic >> tmp;        // (
              else
                motlu.prefix(")(");
              nb_som_face=htoi(motlu);
              if (nb_som_face==0)
                {
                  mixte=1;                // On va lire des elements mixtes en esperant qu'ils sont de meme type
                  fic >> motlu;        // On lit la premiere ligne
                  nb_som_face=htoi(motlu);
                }
              if (nb_som_face==3)
                nb_som_elem=4;         // On va lire des triangles donc les elements sont des tetras (4 sommets)
              else if (nb_som_face==4)
                nb_som_elem=8;        // On va lire des quadrangles donc les elements sont des hexas (8 sommets)
              else if (nb_som_face==2 && type_elements==1)
                nb_som_elem=3;        // On va lire des segments dont les elements sont des triangles (3 sommets)
              else if (nb_som_face==2 && type_elements==3)
                nb_som_elem=4;        // On va lire des segments dont les elements sont des quadrangles (4 sommets)
              else
                {
                  if (nb_som_face==0)
                    Cerr << "It seems that we try to read faces with different types..." << finl;
                  //                        if (nb_som_face==2)                                                        // N'est plus necessaire, vu qu'on sait lire
                  //                           Cerr << "Il semble que l'on essaie de lire un segment..." << finl;        // maintenant des segments
                  Cerr << "The case of faces that are not triangles or quadrangles" << finl;
                  Cerr << "or a mixture of several types of faces" << finl;
                  Cerr << "is not yet provided. Your mesh is not only composed" << finl;
                  Cerr << "of tetrahedra, hexahedra, triangles or quadrangles." << finl;
                  exit();
                }
              // Tableaux de travail
              IntTab& les_elems=dom.zone(0).les_elems();
              if (les_elems.size()==0)
                {
                  // Si les_elems n'est pas dimensionne (1ere lecture de faces)
                  les_elems.resize(nb_elem,nb_som_elem);
                  // On initialise a -1 pour voir les sommets non definis
                  les_elems=-1;
                  nb_som_lu_elem.resize_array(nb_elem);
                  nb_som_lu_elem=0;
                }
              ArrOfInt elem(2),som(nb_som_face);
              int nb_face_lu=ifin-ideb+1;
              REF(Frontiere) nouveau_bord;
              // On lit les sommets de la face et les 2 elements au contact de la face
              for (int i=0; i<nb_face_lu; i++)
                {
                  if (mixte && i>0)
                    {
                      fic >> motlu;
                      if (htoi(motlu)!=nb_som_face)
                        {
                          Cerr << "We read an element to " << htoi(motlu) << " faces." << finl;
                          Cerr << "So it was planned to read elements to " << nb_som_face << " faces."<< finl;
                          exit();
                        }
                    }
                  for (int j=0; j<nb_som_face; j++)
                    {
                      fic >> motlu;
                      som[j]=htoi(motlu)-1;
                    }
                  // Pour les hexaedres a cause de la numerotation TRUST, on inverse les sommets 2 et 3
                  if (nb_som_face==4)
                    {
                      int tmp2=som[2];
                      som[2]=som[3];
                      som[3]=tmp2;
                    }
                  fic >> motlu;        // premier element voisin de la face (nul si frontiere)
                  elem[0]=htoi(motlu)-1;
                  fic >> motlu;        // deuxieme element voisin de la face (nul si frontiere)
                  // Debut Rajout Cyril MALOD : 15-06-2006
                  Nom tmp2=motlu;
                  if (tmp2!=motlu.prefix("))"))
                    {
                      tmp2=motlu.prefix("))");
                      elem[1]=htoi(tmp2)-1;
                      compteur=1; // Ce compteur sert a ne pas lire le "motlu" suivant
                    }
                  else
                    {
                      if (tmp2!=motlu.prefix(")"))
                        {
                          tmp2=motlu.prefix(")");
                          elem[1]=htoi(motlu)-1;
                          compteur=-1; // Ce compteur sert a lire le "motlu" suivant
                        }
                      else
                        {
                          elem[1]=htoi(motlu)-1;
                          compteur=0; // Ce compteur sert a lire le "motlu" suivant
                        }
                    }

                  //                        elem(1)=htoi(motlu)-1;
                  // Fin Rajout Cyril MALOD : 15-06-2006
                  // Premier passage, on verifie bien qu'on lit une frontiere
                  if (i==0)
                    {
                      if (elem[0]<0 || elem[1]<0)
                        {
                          // C'est bien une frontiere donc on dimensionne le necessaire
                          type=3;
                          Cerr << nb_face_lu << " faces are read from the boundary number " << izone << finl;
                          nouveau_bord=dom.zone(0).faces_bord().add(Bord());
                          nouveau_bord->nommer((Nom)izone);
                          if (nb_som_face==3)
                            nouveau_bord->faces().typer(Faces::triangle_3D);
                          else if (nb_som_face==4)
                            nouveau_bord->faces().typer(Faces::quadrangle_3D);
                          else if (nb_som_face==2)
                            nouveau_bord->faces().typer(Faces::segment_2D);
                          else
                            {
                              Cerr << "Type of boundary face not provided for nb_som_face=" << nb_som_face << finl;
                              exit();
                            }
                          nouveau_bord->faces().dimensionner(nb_face_lu);
                        }
                      else
                        {
                          type=2;
                          Cerr << nb_face_lu << " internal faces are read in the area " << izone << finl;
                        }
                    }
                  // Les faces internes sont lues mais pas stockees, les faces frontieres sont stockees
                  if (type!=2)
                    for (int j=0; j<nb_som_face; j++)
                      nouveau_bord->faces().sommet(i,j)=som[j];

                  // On construit le tableau les_elems a partir des faces lues
                  for (int i2=0; i2<2; i2++)
                    {
                      if (elem[i2]>=0) // On ne traite pas les elements -1 voisins des faces frontieres
                        {
                          // Premier remplissage de elem(i)
                          if (nb_som_lu_elem[elem[i2]]==0)
                            {
                              for (int j=0; j<nb_som_face; j++)
                                les_elems(elem[i2],nb_som_lu_elem[elem[i2]]++)=som[j];
                            }
                          else
                            {
                              int face_opposee=1;
                              for (int j=0; j<nb_som_face; j++)
                                {
                                  // On ajoute le sommet s'il n'est pas deja dans les_elems
                                  int k=0,trouve=0;
                                  while (k<nb_som_lu_elem[elem[i2]] && trouve==0)
                                    {
                                      if (les_elems(elem[i2],k)==som[j])
                                        {
                                          face_opposee=0;
                                          trouve=1;
                                        }
                                      else
                                        k++;
                                    }
                                  // On ne complete que pour les tetraedres ou les triangles
                                  if ((trouve==0 && nb_som_face==3) || (trouve==0 && nb_som_face==2 && type_elements==1))
                                    les_elems(elem[i2],nb_som_lu_elem[elem[i2]]++)=som[j];

                                  assert(elem[i2]<nb_elem);
                                  if (nb_som_lu_elem[elem[i2]]>nb_som_elem)
                                    {
                                      Cerr << "Problem on reading the element " << elem[i2] << finl;
                                      Cerr << "There is more than " << nb_som_elem << " nodes !" << finl;
                                      Cerr << "Check that the read file contains only tetrahedra or triangles" << finl;
                                      Cerr << "or contact TRUST support." << finl;
                                      exit();
                                    }
                                }
                              // On complete la face opposee de l'hexaedre ou du quadrangle
                              if ((nb_som_face==4 && face_opposee==1) || (nb_som_face==2 && face_opposee==1 && type_elements==3))
                                {
                                  for (int j=0; j<nb_som_face; j++)
                                    les_elems(elem[i2],nb_som_lu_elem[elem[i2]]++)=som[j];
                                }
                            }
                        }
                    }
                }
              if (compteur==0 || compteur==-1)
                {
                  fic >> motlu;        // ) ou ))
                  if (motlu==")" && compteur==0)
                    fic >> motlu;        // )
                }
            }
          Cerr << finl;
        }
      else if ((motlu=="(45") || (motlu=="(39"))
        {
          Cerr << "Reading of a name:" << finl;
          fic >> motlu;        // Numero de la zone
          // Attention le numero de la zone est en decimal !
          //int izone=htoi(motlu.suffix("("));
          int izone=atoi(motlu.suffix("("));
          fic >> motlu;        // Type de la zone
          Nom Nomzone;
          fic >> Nomzone;        // Nom de la zone)())
          Nom nom_zone=Nomzone;
          nom_zone.prefix(")())");
          if (nom_zone==Nomzone)
            if (1)
              {
                // retour a la ligne ?
                Nom app;
                fic >> app;
                Nomzone+=app;
                nom_zone=Nomzone;
                nom_zone.prefix(")())");

              }
          Cerr << "The area " << izone << " is called " << nom_zone << finl;
          // On parcourt les bords pour renommer
          Bords& les_bords=dom.zone(0).faces_bord();
          les_bords.associer_zone(dom.zone(0));
          if (les_bords.est_vide())
            {
              Cerr << "Reading a name before reading the boundaries..." << finl;
              Cerr << "Case not provided, contact TRUST support." << finl;
              exit();
            }
          LIST_CURSEUR(Bord) curseur=les_bords;
          while (curseur)
            {
              if (curseur->le_nom()==(Nom)izone) curseur->nommer(nom_zone);
              ++curseur;
            }
          // On parcourt les zones pour renommer
          Zones& les_zones=dom.zones();
          LIST_CURSEUR(Zone) curseur2=les_zones;
          while (curseur2)
            {
              if (curseur2->le_nom()==(Nom)izone)
                ref_cast(Zone,curseur2.valeur()).nommer(nom_zone);
              ++curseur2;
            }

          Cerr << finl;
        }
      else
        {
          if (motlu.debute_par("("))
            {
              Cerr << "Reading a tag:" << finl;
              Cerr << motlu;
              va_a_la_parenthese_fermante(fic);
              Cerr << finl << finl;
            }
          else if ( (motlu==" ") || (motlu=="") )
            {
              Cerr << "End of file ?" << finl;
            }
          else if (motlu!="??")
            {
              Cerr << "Tag " << motlu << " unrecognized." << finl;
              exit();
            }
        }
    }
  // Verification si le tableau les_elems a ete rempli completement
  IntTab& les_elems=dom.zone(0).les_elems();
  for (int i=0; i<nb_elem; i++)
    for (int j=0; j<nb_som_elem; j++)
      if (les_elems(i,j)==-1)
        {
          Cerr << "The array of connectivity elements-nodes is wrong filled in Lire_Tgrid::interpreter." << finl;
          Cerr << "Contact TRUST support." << finl;
          exit();
        }

  // On reordonne le domaine (utile surtout pour les hexaedres)
  dom.zone(0).type_elem().reordonner();

  // Nettoie le domaine pour enlever les noeuds inutiles
  // Mettre une methode a Domaine::nettoie
  // Attention: les lignes suivantes pas compatibles avec TRUST < v1.4.6

  if ( (Process::nproc()==1) && (NettoieNoeuds::NettoiePasNoeuds==0) )
    NettoieNoeuds::nettoie(dom);

  return is;
}
