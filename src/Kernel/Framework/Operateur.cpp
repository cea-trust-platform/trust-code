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
// File:        Operateur.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/25
//
//////////////////////////////////////////////////////////////////////////////

#include <Operateur.h>
#include <Schema_Temps_base.h>
#include <Operateur_base.h>
#include <DoubleTrav.h>
#include <Probleme_base.h>
#include <stat_counters.h>

// Description:
//    Ecrit le type de l'operateur sur un flot de Sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie& le flot de sortie modifie
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Operateur::ecrire(Sortie& os) const
{
  os << " { " << typ << " } " << finl;
  return os;
}


// Description:
//    Lit un operateur sur un flot d'entree.
//    Type l'operateur et lui associe son equation.
//    Format:
//      {
//       [UN Motcle REPRESENTANT UN TYPE]
//      }
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree ou lire l'operateur
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: accolade fermante attendue
// Effets de bord: le flot d'entree est modifie
// Postcondition: l'operateur est associe a une equation et il est type
Entree& Operateur::lire(Entree& is)
{
  typ="non defini";
  Motcle motlu;
  is >> motlu ;
  assert(motlu=="{");
  while(motlu!="}")
    {
      is>>motlu;
      if (motlu=="}")
        {
          if(typ=="non defini")
            {
              typ = "";
              typer();
              l_op_base().associer_eqn(equation());
              l_op_base().set_nb_ss_pas_de_temps(1);
              l_op_base().set_decal_temps(0);
            }
        }
      else if (motlu=="nb_sous_pas_de_temps")
        {
          if(typ=="non defini")
            {
              typ = "";
              typer();
              l_op_base().associer_eqn(equation());
            }
          int nb_ss_pas_de_temps;
          is >> nb_ss_pas_de_temps;
          l_op_base().set_nb_ss_pas_de_temps(nb_ss_pas_de_temps);
          l_op_base().set_decal_temps(0);
        }
      else if (motlu=="implicite")
        {
          if(typ=="non defini")
            {
              typ = "";
              typer();
              l_op_base().associer_eqn(equation());
            }
          l_op_base().set_decal_temps(1);
          l_op_base().set_nb_ss_pas_de_temps(1);
          is >> motlu;
          if(motlu != "solveur")
            {
              Cerr << "We expected the keyword \"solveur\" instead of : "
                   << motlu << finl;;
              Process::exit();
            }
          l_op_base().lire_solveur(is);
        }
      else
        {
          if(typ!="non defini")
            {
              Cerr << "We must choose the type of operator in beginning "
                   <<"of reading block (before implicit in particular) "
                   << finl;
              Process::exit();
            }
          typ=motlu;
          typer();
          l_op_base().associer_eqn(equation());
          l_op_base().set_decal_temps(0);
          l_op_base().set_nb_ss_pas_de_temps(1);
          is >> l_op_base();
        }
    }
  return is;
}

// Description:
//    Renvoie le champ representant l'inconnue de l'equation dont
//    l'operateur fait partie.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Inc&
//    Signification: le champ inconuu de l'equation associee
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Champ_Inc& Operateur::mon_inconnue() const
{
  return mon_equation->inconnue();
}

// Description:
//    Renvoie la discretisation de l'equation dont l'operateur
//    fait partie.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Discretisation_base&
//    Signification: la discretisation de l'equation associee
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Discretisation_base& Operateur::discretisation() const
{
  return mon_equation->discretisation();
}

// Description:
//    Met a jour les references des objets associes a l'operateur.
//    Operateur::le_champ_inco, Operateur::champ_inco
//    Appelle Operateur_base::completer()
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
void Operateur::completer()
{
  if (!le_champ_inco.non_nul())
    le_champ_inco=mon_equation->inconnue().valeur();

      l_op_base().completer();
}

void Operateur::associer_champ(const Champ_Inc_base& ch)
{
  le_champ_inco = ch;
}

// Description:
//    Effecttue une mise a jour en temps de l'operateur.
//    Appelle Operateur_base::mettre_a_jour(double)
// Precondition:
// Parametre: double temps
//    Signification: le pas de temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Operateur::mettre_a_jour(double temps)
{
  l_op_base().mettre_a_jour(temps);
}
// Description:
//    Calcule le prochain pas de temps.
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
// Postcondition: la methode ne modifie pas l'objet
double Operateur::calculer_pas_de_temps() const
{
  // Si l'equation de l'operateur n'est pas resolue, on ne calcule pas son pas de temps de stabilite
  if (equation().equation_non_resolue())
    return DMAXFLOAT;
  statistiques().begin_count(dt_counter_);
  double dt_stab = l_op_base().calculer_dt_stab();
  statistiques().end_count(dt_counter_);
  // Verification que l'operateur a bien un mp_min de fait:
  assert(dt_stab==Process::mp_min(dt_stab));
  return dt_stab;
}


// Description:
//    Demande a l'equation si une impression est necessaire
//    Renvoie 1 pour OUI, 0 sinon.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si une impression est necessaire 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int  Operateur::limpr() const
{
  return mon_equation->limpr();
}

// Description:
//    Imprime l'operateur sur un flot de sortie, si
//    c'est necessaire. (voir Schema_Temp_base::limpr())
// Precondition:
// Parametre:Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie peut etre modifie
// Postcondition: la methode ne modifie pas l'objet
void  Operateur::imprimer(Sortie& os) const
{
  if(limpr())
    l_op_base().impr(os);
}


// Description:
//    Imprime l'operateur sur un flot de sortie de facon
//    inconditionnelle.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: int
//    Signification: code de Operateur_base::impr(Sortie&)
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
int Operateur::impr(Sortie& os) const
{
  return l_op_base().impr(os);
}

// Description:
//    Calcule et ajoute la contribution de l'operateur au second
//    membre de l'equation.
//    Appelle Operateur::ajouter(const DoubleTab&, DoubleTab& )
// Precondition:
// Parametre: Champ_Inc& ch
//    Signification: le champ inconnu sur lequel l'operateur agit
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& resu
//    Signification: le tableau stockant les valeurs du second membre
//                   auquel on ajoute la contribution de l'operateur
//    Valeurs par defaut:
//    Contraintes:
//    Acces:entree/sortie
// Retour: DoubleTab&
//    Signification: le second membre auquel on a ajoute la contribution
//                   de l'operateur
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Operateur::ajouter(const Champ_Inc& ch, DoubleTab& resu) const
{
  int i ;
  int nstep=l_op_base().get_nb_ss_pas_de_temps();
  double dt=equation().schema_temps().pas_de_temps();
  dt/=nstep;
  if(nstep==1)
    return ajouter(ch.valeurs(), resu);
  DoubleTrav derivee(resu);
  DoubleTrav inco(ch.valeurs());
  inco=ch.valeurs();
  const Solveur_Masse& solveur_masse=equation().solv_masse();
  double dt_inv=1./(double(nstep));
  for (i=0; i<nstep; i++)
    {
      calculer(inco, derivee);
      derivee.echange_espace_virtuel();
      resu.ajoute(dt_inv, derivee) ;
      solveur_masse.appliquer(derivee);
      inco.ajoute_sans_ech_esp_virt(dt, derivee, VECT_ALL_ITEMS) ;
    }
  return resu;
}

// Description:
//    Renvoie le (nom du) type de l'operateur a creer.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom du type de l'operateur a creer
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Operateur::type() const
{
  return typ;
}


// Description:
//     Calcule la contribution de l'operateur, et renvoie
//     le tableau des valeurs.
// Precondition:
// Parametre: Champ_Inc& ch
//    Signification: le champ inconnu sur lequel l'operateur agit
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Parametre: DoubleTab& resu
//    Signification: le tableau stockant les valeurs resultant de l'application
//                   de l'operateur sur le champ inconnu.
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le resultat de l'application de l'operateur sur le champ inconnu
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Operateur::calculer(const Champ_Inc& ch,DoubleTab& resu) const
{
  return calculer(ch.valeurs(), resu);
}

// Description:
//    Ajoute la contribution de l'operateur au tableau passe
//    en parametre.
//    Appelle Operateur::ajouter(const Champ_Inc&, DoubleTab& )
// Precondition:
// Parametre: DoubleTab& resu
//    Signification: le tableau stockant les valeurs du second membre
//                   auquel on ajoute la contribution de l'operateur
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le second membre auquel on a ajoute la contribution
//                   de l'operateur
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Operateur::ajouter(DoubleTab& resu) const
{
  return ajouter(le_champ_inco.valeur().valeurs(), resu);
}

// Description:
//    Applique l'operateur au champ inconnu et renvoie le resultat.
//    Appelle Operateur::calculer(const Champ_Inc&, DoubleTab& );
// Precondition:
// Parametre: DoubleTab& resu
//    Signification: le tableau stockant les valeurs resultant de l'application
//                   de l'operateur sur le champ inconnu.
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleTab&
//    Signification: le resultat de l'application de l'operateur sur le champ inconnu
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Operateur::calculer(DoubleTab& resu) const
{
  resu=0.;
  return ajouter(le_champ_inco.valeur().valeurs(), resu);
}


void Operateur::set_fichier(const Nom& nom)
{
  l_op_base().set_fichier(nom);
}

void Operateur::set_description(const Nom& nom)
{
  l_op_base().set_description(nom);
}

void Operateur::ajouter_contribution_explicite_au_second_membre (const Champ_Inc_base& linconnue, DoubleTab& derivee) const
{
  l_op_base().ajouter_contribution_explicite_au_second_membre (linconnue, derivee);
}

