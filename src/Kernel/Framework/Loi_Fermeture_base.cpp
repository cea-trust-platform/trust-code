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
// File:        Loi_Fermeture_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     1
//
//////////////////////////////////////////////////////////////////////////////


#include <Loi_Fermeture_base.h>
#include <Probleme_base.h>
#include <Param.h>
#include <Champs_compris.h>
#include <Schema_Temps_base.h>

Implemente_base_sans_constructeur(Loi_Fermeture_base, "Loi_Fermeture_base", Objet_U);
// XD loi_fermeture_base objet_u loi_fermeture_base -3 Class for appends fermeture to problem

Loi_Fermeture_base::Loi_Fermeture_base()
{
  status_ = INITIAL;
}

// Description: Cette methode est la premiere appelee par le probleme
//  pour construire l'objet au moment ou on l'associe au probleme.
//  On verifie qu'on est pas encore associe.
void Loi_Fermeture_base::associer_pb_base(const Probleme_base& pb)
{
  if (status_ == PB_ASSOCIE)
    {
      Cerr << "Error associating " << que_suis_je() << " " << le_nom()
           << " to problem " << pb.que_suis_je()
           << ".\n This object is already associated to problem " << mon_probleme().le_nom()
           << finl;
      barrier();
      exit();
    }
  assert(status_ == INITIAL);
  mon_probleme_ = pb;
  status_ = PB_ASSOCIE;
}

// Description: Cette methode est appelee par le probleme apres
//  la discretisation des equations et du milieu et avant
//  l'appel a readOn() pour lecture des parametres.
//  Dans les classes derivees elle doit discretiser au minimum les champs
//  qui seront requis dans le readOn() des equations ou des autres
//  lois de fermeture.
void Loi_Fermeture_base::discretiser(const Discretisation_base&)
{
  assert(status_ == PB_ASSOCIE);
  status_ = DISCRETISE;
}

// Description: Cette methode appelle la methode set_param() pour
//  initialiser les parametres, puis lit les parametres.
//  Dans l'implementation des classes derivees on peut se contenter
//  d'appeler la methode de la classe de base et verifier les parametres.
// Precondition: Il faut avoir discretise avant.
Entree& Loi_Fermeture_base::readOn(Entree& is)
{
  assert(status_ == DISCRETISE);
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  status_ = READON_FAIT;
  return is;
}

// Description: Pour l'instant, exit()
Sortie& Loi_Fermeture_base::printOn(Sortie& os) const
{
  Cerr << "Loi_Fermeture_base::printOn non code" << finl;
  exit();
  return os;
}

// Description: Cette methode est appelee par le readOn de la classe.
//  Elle doit etre reimplementee dans les classes derivees pour
//  ajouter dans "param" les differents parametres a lire
//  dans le jeu de donnees et appeler la methode de l'ancetre.
//  Dans la classe de base: aucun parametre.
void Loi_Fermeture_base::set_param(Param& param)
{
}

// Description: Cette methode est appelee apres avoir lu toutes les equations
//  et les lois de fermeture (tous les champs et les conditions aux limites
//  du probleme sont disponibles)
void Loi_Fermeture_base::completer()
{
  assert(status_ == READON_FAIT);
  status_ = COMPLET;
}

// Description: Renvoie le probleme (j'ai cree cette methode pour ne pas
//  donner acces au probleme en ecriture par la REF)
const Probleme_base& Loi_Fermeture_base::mon_probleme() const
{
  return mon_probleme_.valeur();
}

// Description: Cette methode est appelee par le probleme apres
//  preparer_calcul() des equations et du milieu. Elle doit mettre
//  a jour tous les champs qu'elle gere en fonction des autres
//  champs du probleme.
void Loi_Fermeture_base::preparer_calcul()
{
  assert(status_ == COMPLET);
  const double temps = mon_probleme().schema_temps().temps_courant();
  mettre_a_jour(temps);
}

// Description: Cette methode est appelee par le probleme apres
//  mettre_a_jour() des equations et du milieu. Elle doit mettre
//  a jour tous les champs qu'elle gere en fonction des autres
//  champs du probleme.
void Loi_Fermeture_base::mettre_a_jour(double temps)
{
  assert(status_ == COMPLET);
}

// Description: Cette methode renvoie le champ de nom "nom" s'il est
//  compris par la classe, sinon appelle la methode get_champ de l'ancetre.
//  Dans la classe de base, on leve l'exception Champ_compris_erreur.
const Champ_base& Loi_Fermeture_base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);

}

// Description: La classe de base ne comprend aucun champ supplementaire
void Loi_Fermeture_base::creer_champ(const Motcle& motlu)
{
}

void Loi_Fermeture_base::get_noms_champs_postraitables(Noms& noms, Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr << que_suis_je()<<": " << champs_compris_.liste_noms_compris() << finl;
  else
    noms.add(champs_compris_.liste_noms_compris());

}
