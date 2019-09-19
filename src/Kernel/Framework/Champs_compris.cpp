/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Champs_compris.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#include <Champs_compris.h>
#include <Champ_base.h>
#include <List_Nom.h>

Implemente_liste(REF(Champ_base));

Implemente_instanciable(Champs_compris,"Champs_compris",Objet_U);


// Description:
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
Sortie& Champs_compris::printOn(Sortie& os) const
{
  return os;
}

// Description:
// Precondition:
// Parametre: Entree& is
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
Entree& Champs_compris::readOn(Entree& is)
{
  return is;
}

static Nom nom; // Optimisation pour ne pas creer a de multiples reprises un objet
const Champ_base& Champs_compris::get_champ(const Motcle& motcle) const
{
  nom = motcle;
  if (nom=="??")
    {
      Cerr<<"Champs_compris::get_champ()"<<finl;
      Cerr<<"No field can be requested using the identifier \'??\'"<<finl;
      exit();
    }

  CONST_LIST_CURSEUR(REF(Champ_base)) curseur = liste_champs_;
  Nom nom_champ;
  while (curseur)
    {
      const Champ_base& ch = curseur.valeur().valeur();
      nom_champ = ch.le_nom();
      if (nom_champ.majuscule()==nom)
        {
          return ch;
        }
      else
        {
          const Noms& syno = ch.get_synonyms();
          int nb_syno=syno.size();
          for (int s=0; s<nb_syno; s++)
            {
              nom_champ = syno[s];
              if (nom_champ.majuscule()==nom)
                return ch;
            }
          int nb_composantes = ch.nb_comp();
          for (int i=0; i<nb_composantes; i++)
            {
              nom_champ = ch.nom_compo(i);
              if (nom_champ.majuscule()==nom)
                {
                  return ch;
                }
            }
        }
      ++curseur;
    }
  throw Champs_compris_erreur();

  REF(Champ_base) ref_champ;
  return ref_champ;

}

int new_liste_add_if_not(LIST(Nom)& new_list,const Nom& nom_champ)
{
  Motcle mot(nom_champ);
  int size=new_list.size();
  for (int i=0; i<size; i++)
    if (mot==new_list(i))
      return 0;
  new_list.add(nom_champ);
  return 1;
}

void rebuild_liste_noms( const  LIST(REF(Champ_base))& liste_champs_, const Noms& liste_noms_,Noms& liste_noms_construits_,int info=0)
{
  if (liste_noms_construits_.size()<liste_noms_.size())
    liste_noms_construits_=liste_noms_;
  LIST(Nom) new_liste;
  for (int i=0; i<liste_noms_.size(); i++)
    new_liste.add(liste_noms_[i]);

  REF(Champ_base) ref_champ;
  CONST_LIST_CURSEUR(REF(Champ_base)) curseur = liste_champs_;
  Nom nom_champ;
  while (curseur)
    {
      nom_champ = (curseur.valeur().valeur().le_nom());
      //Cerr<<" ok "<<nom_champ<<finl;
      if (nom_champ!=Nom())
        new_liste_add_if_not(new_liste,nom_champ);


      const Noms&  syno= curseur.valeur().valeur().get_synonyms();
      int nb_syno=syno.size();
      for (int s=0; s<nb_syno; s++)
        {
          if (syno[s]!=Nom())
            new_liste_add_if_not(new_liste,syno[s]);
        }
      int nb_composantes = curseur.valeur().valeur().nb_comp();
      for (int i=0; i<nb_composantes; i++)
        {
          nom_champ = (curseur.valeur().valeur().nom_compo(i));
          if (nom_champ!=Nom())
            new_liste_add_if_not(new_liste,nom_champ);
        }

      ++curseur;
    }
  int size_new_liste=new_liste.size();
  int prem=1;
  if (size_new_liste!=liste_noms_construits_.size())
    {
      if (info)
        if (prem)
          {
            prem=0;
          }
      Noms nn_liste_noms_construits_(size_new_liste);
      for (int i=0; i<liste_noms_construits_.size(); i++)
        if (i<size_new_liste)
          nn_liste_noms_construits_[i]=liste_noms_construits_[i];
      liste_noms_construits_ =nn_liste_noms_construits_;
    }

  for (int i=0; i<size_new_liste; i++)
    if (liste_noms_construits_[i]!=new_liste(i))
      {
        if (info)
          {
            if (prem)
              {
                prem=0;
              }
            //Cout<<liste_noms_construits_[i] << " va etre remplace par "<< new_liste(i)<<finl;
          }
        liste_noms_construits_[i]=new_liste(i);

      }
  //if ((prem==0) && (info))
  // Cout<<"liste apres modif "<<liste_noms_construits_<<finl;
  // Cerr<<" ici2 "<<finl;
}




void Champs_compris::ajoute_champ(const Champ_base& champ)
{
  REF(Champ_base) champ_ref;
  champ_ref = champ;
  assert(!liste_champs_.contient(champ_ref));
  // GF ne sert a rien le test ne marche pas !!!
  // il faut tester si champ est contenu ( c.a.d si il ya une reference vers celui-ci
  if (liste_champs_.contient(champ_ref)) abort();
  // GF essai brutal ....
  CONST_LIST_CURSEUR(REF(Champ_base)) curseur = liste_champs_;
  Motcle nom_champ_add = champ.le_nom();
  while (curseur)
    {
      const Champ_base& ch= curseur.valeur().valeur();
      const Nom& nom_champ = ch.le_nom();
      if (nom_champ==nom_champ_add)
        {
          Cerr<<champ.le_nom()<<" is already in the list of names of field !!"<<finl;
          //  exit();
        }

      ++curseur;
    }


  liste_champs_.add(champ_ref);
  Cerr<<"Champs_compris::ajoute_champ " <<champ.le_nom()<<finl;
  rebuild_liste_noms(liste_champs_,liste_noms_,liste_noms_construits_);

}

void Champs_compris::ajoute_nom_compris( const Nom& mot)
{
  liste_noms_.add(mot);
  rebuild_liste_noms(liste_champs_,liste_noms_,liste_noms_construits_);
}
const Noms Champs_compris::liste_noms_compris() const
{
  Noms liste_noms_prov(liste_noms_construits_);
  rebuild_liste_noms(liste_champs_,liste_noms_,liste_noms_prov,1);
  return liste_noms_prov;
}
/*Noms& Champs_compris::liste_noms_compris()
  {
  abort();
  return liste_noms_;
  }
*/
