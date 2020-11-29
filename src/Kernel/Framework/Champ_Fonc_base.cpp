/****************************************************************************
* Copyright (c) 2020, CEA
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
// File:        Champ_Fonc_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/27
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Fonc_base.h>
#include <Zone_dis.h>
#include <Domaine.h>
#include <EcritureLectureSpecial.h>
#include <MD_Vector_tools.h>

Implemente_base(Champ_Fonc_base,"Champ_Fonc_base",Champ_Don_base);


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: Sortie& s
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& Champ_Fonc_base::printOn(Sortie& s ) const
{
  return s ;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: Entree& s
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
Entree& Champ_Fonc_base::readOn(Entree& s )
{
  return s ;
}


// Description:
//    Mise a jour en temps du champ.
// Precondition:
// Parametre: double temps
//    Signification: le temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_Fonc_base::mettre_a_jour(double un_temps)
{
  changer_temps(un_temps);
  valeurs().echange_espace_virtuel();
}

int Champ_Fonc_base::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  Cerr << "Error in Champ_Fonc_base::fixer_nb_valeurs_nodales: method not reimplemented for the class "
       << que_suis_je() << finl;
  exit();
  return nb_noeuds;
}

void Champ_Fonc_base::creer_tableau_distribue(const MD_Vector& md, Array_base::Resize_Options opt)
{
  // Note B.M.: pour etre symetrique avec Champ_Inc_base, il faudrait tester si le
  // champ est scalaire ou multi-scalaire (voir Champ_Inc_base::creer_tableau_distribue())
  if (valeurs_.size_array() == 0 && (!valeurs_.get_md_vector().non_nul()))
    {
      // Note B.M.: les methodes fixer_nb_valeurs_nodales sont appelees a tort et a travers.
      // Ne rien faire si le tableau a deja la bonne structure
      if(nb_compo_ == 1)
        valeurs_.resize(0);
      else
        valeurs_.resize(0, nb_compo_);
    }
  // Ca va planter si on a attache une autre structure parallele (c'est voulu !)
  if (!(valeurs_.get_md_vector() == md))
    {
      if (valeurs_.get_md_vector().non_nul())
        {
          Cerr << "Internal error in Champ_Fonc_base::creer_tableau_distribue:\n"
               << " array has alreary a (wrong) parallel descriptor" << finl;
          exit();
        }
      MD_Vector_tools::creer_tableau_distribue(md, valeurs_, opt);
    }
}

// Description:
//    Sauvegarde le champ sur un flot de sortie
//    Ecrit le nom, le temps et les valeurs.
// Precondition:
// Parametre: Sortie& fich
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_Fonc_base::sauvegarder(Sortie& fich) const
{
  // en mode ecriture special seul le maitre ecrit l'entete
  int a_faire,special;
  EcritureLectureSpecial::is_ecriture_special(special,a_faire);

  if (a_faire)
    {
      Nom mon_ident(nom_);
      mon_ident += que_suis_je();
      mon_ident += zone_dis_base().zone().domaine().le_nom();
      mon_ident += Nom(temps_,"%e");
      fich << mon_ident << finl;
      fich << que_suis_je() << finl;
      fich << temps_ << finl;
    }
  int bytes = 0;
  if (special)
    bytes = EcritureLectureSpecial::ecriture_special(*this,fich);
  else
    {
      bytes = 8 * valeurs().size_array();
      valeurs().ecrit(fich);
    }
  if (a_faire)
    {
      //fich << flush ; Ne marche pas en binaire, preferer:
      fich.flush();
    }
  if (Process::je_suis_maitre())
    Cerr << "Backup of the field " << nom_ << " performed on time : " << Nom(temps_,"%e") << finl;

  return bytes;
}


// Description:
//    Reprise a partir d'un flot d'entree
//    Lit le temps et les valeurs du champ.
//    Saute un bloc si le nom du champ est "anonyme".
// Precondition:
// Parametre: Entree& fich
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Champ_Fonc_base::reprendre(Entree& fich)
{
  double un_temps;
  int special= EcritureLectureSpecial::is_lecture_special();
  if (nom_ != Nom("anonyme")) // lecture pour reprise
    {
      fich >> un_temps;
      if (special)
        EcritureLectureSpecial::lecture_special(*this,fich);
      else
        valeurs().lit(fich);
      Cerr << "Resume of the field " <<nom_<< " performed " << finl;
    }
  else // lecture pour sauter le bloc
    {
      DoubleTab tempo;
      fich >> un_temps;
      tempo.jump(fich);
    }
  return 1;
}


// Description:
//    Affecte un Champ_base dans un Champ_Fonc_base.
// Precondition:
// Parametre: Champ_base& ch
//    Signification: le champ partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_base&
//    Signification: le resultat de l'affectation (*this) (avec upcast)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_base& Champ_Fonc_base::affecter_(const Champ_base& ch)
{
  DoubleTab noeuds;
  remplir_coord_noeuds(noeuds);
  ch.valeur_aux(noeuds, valeurs());

  return *this;
}


// Description:
//    Affecte une composante d'un Champ_base dans la meme composnate
//    d'un Champ_Fonc_base.
// Precondition:
// Parametre: Champ_base& ch
//    Signification: le champ partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: int compo
//    Signification: l'indice de la composante a affecter
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_base&
//    Signification: le resultat de l'affectation (*this) (avec upcast)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_base& Champ_Fonc_base::affecter_compo(const Champ_base& ch,
                                            int compo)
{
  DoubleTab noeuds;
  IntVect polys;
  if(!remplir_coord_noeuds_et_polys_compo(noeuds, polys, compo))
    {
      remplir_coord_noeuds_compo(noeuds, compo);
      ch.valeur_aux_compo(noeuds, valeurs(), compo);
    }
  else
    ch.valeur_aux_elems_compo(noeuds, polys, valeurs(), compo);
  return *this;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: DoubleTab&
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: IntVect&
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 0
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_Fonc_base::remplir_coord_noeuds_et_polys(DoubleTab&, IntVect&) const
{
  return 0;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: DoubleTab& coord
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: renvoie toujours le parametre coord
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_Fonc_base::remplir_coord_noeuds(DoubleTab& coord) const
{
  // Ne retourne rien
  return coord;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: DoubleTab& coord
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: renvoie toujours le parametre coord
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_Fonc_base::remplir_coord_noeuds_compo(DoubleTab& coord,
                                                       int ) const
{
  // Ne retourne rien
  return coord;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees
// Precondition:
// Parametre: DoubleTab&
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: IntVect&
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: int
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 0
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_Fonc_base::remplir_coord_noeuds_et_polys_compo(DoubleTab& ,
                                                         IntVect& ,
                                                         int ) const
{
  return 0;
}


const Domaine& Champ_Fonc_base::domaine() const
{
  return zone_dis_base().zone().domaine();
}

DoubleTab& Champ_Fonc_base::valeur_aux(const DoubleTab& positions,
                                       DoubleTab& tab_valeurs) const
{
  const Zone& zone=zone_dis_base().zone();
  IntVect les_polys(positions.dimension(0));
  zone.chercher_elements(positions, les_polys);
  return valeur_aux_elems(positions, les_polys, tab_valeurs);
}


