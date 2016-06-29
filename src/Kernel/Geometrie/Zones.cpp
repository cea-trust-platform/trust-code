/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Zones.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////

#include <Zones.h>
#include <Domaine.h>

Implemente_liste(Zone);
Implemente_instanciable(Zones,"Zones",LIST(Zone));

// Description:
//    Ecrit toutes les zones de la liste sur un flot de sortie
// Precondition:
// Parametre: Sortie& s
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
Sortie& Zones::printOn(Sortie& s ) const
{
  return LIST(Zone)::printOn(s) ;
}


// Description:
//    Lit les zones a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Zones::readOn(Entree& s)
{
  return LIST(Zone)::readOn(s) ;
}

// Description:
//    Associe toutes les zones de la liste au
//    domaine specifie.
// Precondition:
// Parametre: Domaine& un_domaine
//    Signification: le domaine dont font partie les zones de la liste
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: les zones de la liste sont des zones du domaine specifie
void Zones::associer_domaine(const Domaine& un_domaine)
{
  LIST_CURSEUR(Zone) curseur(*this);;;
  while(curseur)
    {
      curseur->associer_domaine(un_domaine);
      ++curseur;
    }
}


// Description:
//    Reordonne les elements du maillage.
//    On appelle Zone::reordonner() sur chaque
//    zone de la liste.
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
void Zones::reordonner()
{
  LIST_CURSEUR(Zone) curseur(*this);;;
  while(curseur)
    {
      curseur->reordonner();
      ++curseur;
    }
}


// Description:
//    On agglomere les Zones de meme type
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
void Zones::comprimer()
{
  if (Process::nproc() > 1)
    {
      Cerr << "Error in Zones::comprimer() : compression prohibited in parallel mode" << finl;
      exit();
    }
  Cerr << "Compressing objects of type Zone in progress... " << finl;

  IntVect fait(size());
  int rang1=0, rang2=0;
  {
    LIST_CURSEUR(Zone) curseur(*this);;;
    while(curseur)
      {
        curseur->comprimer();
        ++curseur;
      }
  }

  LIST_CURSEUR(Zone) curseur1(*this);;;
  while(curseur1)
    {
      if (!fait(rang1) )
        {
          fait(rang1)=1;
          Zone& zone1=curseur1.valeur();
          rang2=rang1;
          LIST_CURSEUR(Zone) curseur2(curseur1.list());;;
          while(curseur2)
            {
              if (!fait(rang2) )
                {
                  Zone& zone2=curseur2.valeur();
                  if ((zone2.type_elem()->que_suis_je())
                      ==
                      (zone1.type_elem()->que_suis_je()) )
                    {
                      Nom nom1=zone1.le_nom();
                      Nom nom2=zone2.le_nom();
                      Nom nom=nom1;
                      Cerr << "agglomeration of Zones "<< nom1
                           << " and " << nom2 << finl;;
                      fait(rang2)=1;
                      if( (nom1.longueur()+nom2.longueur()) < 20)
                        {
                          nom+="_";
                          nom+=nom2;
                        }
                      zone1.nommer(nom);
                      int sz1=zone1.les_elems().dimension(0);
                      int sz2=zone2.les_elems().dimension(0);
                      int nb_ccord=zone1.les_elems().dimension(1);
                      IntTab& elems1=zone1.les_elems();
                      IntTab& elems2=zone2.les_elems();
                      elems1.resize(sz1+sz2, nb_ccord);
                      for(int i=0; i<sz2; i++)
                        for(int j=0; j<nb_ccord; j++)
                          elems1(sz1+i,j)=elems2(i,j);
                      zone2.faces_bord().associer_zone(zone1);
                      zone1.faces_bord().add(zone2.faces_bord());
                      zone2.faces_joint().associer_zone(zone1);
                      zone1.faces_joint().add(zone2.faces_joint());
                      zone2.faces_raccord().associer_zone(zone1);
                      zone1.faces_raccord().add(zone2.faces_raccord());
                      zone2.faces_int().associer_zone(zone1);
                      zone1.faces_int().add(zone2.faces_int());
                      elems2.resize(0);
                      zone1.comprimer();
                    }
                }
              ++curseur2;
              rang2++;
            }
        }
      ++curseur1;
      ++rang1;
    }
  curseur1=*this;
  while(curseur1)
    {
      if (curseur1->les_elems().size()==0)
        {
          //cerr << &(curseur1.valeur()) << endl;
          suppr(curseur1.valeur());
          curseur1=*this;
        }
      else
        ++curseur1;
    }
  (*this).valeur().invalide_octree();
  Cerr << "Zones::comprimer() - End" << finl;

}

