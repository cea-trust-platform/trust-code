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

#include <Zone_Cl_dis_base.h>
#include <Equation_base.h>
#include <Frontiere_dis_base.h>
#include <Schema_Temps_base.h>
#include <Champ_front_var_instationnaire.h>
#include <Champ_front_instationnaire_base.h>
#include <Cond_lim_utilisateur_base.h>
#include <Probleme_base.h>

Implemente_base(Zone_Cl_dis_base,"Zone_Cl_dis_base",Objet_U);


// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Imprime les conditions aux limites discretisees sur un flot de sortie
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
Sortie& Zone_Cl_dis_base::printOn(Sortie& os) const
{
  return os;
}


// Description:
//    Surcharge Objet_U::readOn(Sortie&)
//    Lit les conditions aux limites discretisees a partir d'un flot d'entree
//    Le format attendu est le suivant:
//    {
//     Nom Cond_lim [REPETER LECTURE AUTANT DE FOIS QUE NECESSAIRE]
//    }
// Precondition: une equation doit avoir ete associee aux conditions aux limites
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Exception: nombre de conditions aux limites lues invalide
// Effets de bord:
// Postcondition: l'objet contient les conditions aux limites lues
Entree& Zone_Cl_dis_base::readOn(Entree& is)
{
  assert(mon_equation.non_nul());
  const Zone& lazone=equation().zone_dis()->zone();
  Motcle accolade_ouverte("{");
  Motcle accolade_fermee("}");
  Nom nomlu;
  Motcle motlu;
  is >> motlu;
  if (motlu != accolade_ouverte)
    {
      Cerr << "Error while reading the boundaries conditions\n";
      Cerr << "We expected a " << accolade_ouverte << " instead of \n"
           << motlu;
      exit();
    }

  int n = lazone.nb_front_Cl();
  IntTab front_deja_lu(n);
  front_deja_lu = 0;
  les_conditions_limites().dimensionner(n);
  int nb_clim=0;

  while(1)
    {

      // lecture d'un nom de bord ou de }
      is >> nomlu;
      motlu=nomlu;
      if (motlu == accolade_fermee)
        break;

      Cerr << "Reading the " << nomlu << " boundary condition of the " << equation().que_suis_je() << " equation from the problem " << equation().probleme().le_nom() << finl;
      Journal()<< "Reading the boundary condition " << nomlu <<finl;

      int rang=lazone.rang_frontiere(nomlu);

      // Test supplementaire sur les conditions aux limites qui ont ete lues :
      // on test si deux frontieres ont le meme nom
      if (front_deja_lu(rang) == 0)
        front_deja_lu(rang) = 1;
      else
        {
          Cerr <<finl;
          Cerr <<"Error in the data set "<<finl;
          Cerr <<"the boundary condition associated"<<finl;
          Cerr <<"to the boundary "<< nomlu <<"is read twice !!"<<finl;
          exit();
        }
      //      const Frontiere& frontiere=zone.frontiere(rang);
      is >> les_conditions_limites(rang);
      // Si on avait une condition_limite_utilisateur
      // Il faut recuperer la cl stockee dans celle ci
      // la copier et detruire celle contenue dans la cond_utilisateur
      if (sub_type(Cond_lim_utilisateur_base,les_conditions_limites(rang).valeur()))
        {
          Cond_lim_utilisateur_base& la_cl=ref_cast(Cond_lim_utilisateur_base,les_conditions_limites(rang).valeur());
          la_cl.lire(is,equation(),nomlu);
          Cond_lim* sa=&(la_cl.la_cl());

          // WEC : La nouvelle formulation evite la copie de l'objet Cond_lim_base
          // Cette copie peut poser probleme aux champs qui s'enregistrent dans un vecteur
          // (Champ_Inputs). Le vecteur pointe sur n'importe quoi...
          // Adopte la nouvelle et detruit l'ancienne (Cond_lim_utilisateur)
          les_conditions_limites(rang).adopt(*sa);
          //les_conditions_limites(rang)=(*sa);
          delete sa;
        }
      les_conditions_limites(rang)->
      associer_fr_dis_base(zone_dis().frontiere_dis(rang));

      //Test pour empecher l utilisation de 'Raccord_distant_homogene' en calcul sequentiel
      const Frontiere& frontiere=lazone.frontiere(rang);
      if ((frontiere.que_suis_je()=="Raccord_distant_homogene") && (Process::nproc()==1))
        {
          Cerr<<"At least one connection (adjacent boundary on two domains) is of type 'Raccord distant homogene'." << finl;
          Cerr<<"Use 'Raccord local homogene' to define the connections in sequential computing"<<finl;
          Cerr<<"And 'Raccord distant homogene' for the connections in parallel computing." << finl;
          exit();
        }

      nb_clim++;
    }
  if (nb_clim!=n)
    {
      zone_dis()->ecrire_noms_bords(Cerr);
      Cerr << "It misses " << n-nb_clim << " boundaries conditions " << finl;
      Cerr << "We read " << nb_clim << " boundaries conditions " << finl;
      Cerr << "We waited " << n << " boundary conditions " << finl;
      exit();
    }
  for (int num_cl=0; num_cl<nb_clim; num_cl++)
    {
      les_conditions_limites(num_cl)->associer_zone_cl_dis_base(*this);
      les_conditions_limites(num_cl)->verifie_ch_init_nb_comp();
      //      const Frontiere_dis_base& la_fr_dis = les_conditions_limites(num_cl).frontiere_dis();
      //      const Frontiere& frontiere=la_fr_dis.frontiere();
      //      const Zone& zone2=frontiere.zone();
    }

  return is;
}


// Description:
//    Renvoie 1 si l'objet contient une condition aux limites
//    du Nom specifie.
//    Renvoie 0 sinon.
// Precondition:
// Parametre: Nom& type
//    Signification: le nom de la condition aux limites a chercher
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: int
//    Signification: 1 si la condition aux limites de nom specifie a ete
//                   trouve, 0 sinon.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Zone_Cl_dis_base::contient_Cl(const Nom& type)
{
  for(int i=0; i< les_conditions_limites_.size(); i++)
    if(les_conditions_limites_[i].get_info()->has_base(type))
      return 1;
  return 0;
}


// Description:
//    Renvoie une reference sur la zone discretisee associee aux conditions
//    aux limites. Cette Zone_dis est associee au travers de l'equation
//    associee et pas directement a l'objet Zone_Cl_dis_base.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone_dis&
//    Signification: la zone discretisee associee a l'equation associe
//                   aux conditions aux limites.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Zone_dis& Zone_Cl_dis_base::zone_dis()
{
  return equation().zone_dis();
}


// Description:
//    Renvoie une reference sur la zone discretisee associee aux conditions
//    aux limites. Cette Zone_dis est associee au travers de l'equation
//    associee et pas directement a l'objet Zone_Cl_dis_base.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Zone_dis&
//    Signification: la zone discretisee associee a l'equation associe
//                   aux conditions aux limites.
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Zone_dis& Zone_Cl_dis_base::zone_dis() const
{
  return equation().zone_dis();
}


// Description:
//    Change le i-eme temps futur de toutes les CLs.
void Zone_Cl_dis_base::changer_temps_futur(double temps,int i)
{
  for (int j=0; j<nb_cond_lim(); j++)
    les_conditions_limites_[j]->changer_temps_futur(temps,i);
}

// Description:
//    Change le i-eme temps futur de toutes les CLs.
void Zone_Cl_dis_base::set_temps_defaut(double temps)
{
  for (int j=0; j<nb_cond_lim(); j++)
    les_conditions_limites_[j]->set_temps_defaut(temps);
}

// Description:
//    Tourne la roue des CLs jusqu'au temps donne
int Zone_Cl_dis_base::avancer(double temps)
{
  int ok=1;
  for (int j=0; j<nb_cond_lim(); j++)
    ok = ok && les_conditions_limites_[j]->avancer(temps);
  return ok;
}

// Description:
//    Tourne la roue des CLsj usqu'au temps donne
int Zone_Cl_dis_base::reculer(double temps)
{
  int ok=1;
  for (int j=0; j<nb_cond_lim(); j++)
    ok = ok && les_conditions_limites_[j]->reculer(temps);
  return ok;
}

// Description:
//    Effectue une mise a jour en temps de toutes les conditions
//    aux limites.
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
void Zone_Cl_dis_base::mettre_a_jour(double temps)
{
  les_conditions_limites_.mettre_a_jour(temps);
}


// Description:
//    Effectue une mise a jour pour des sous pas de temps
//    d'un schema en temps (par exemple dans RungeKutta)
//    pour toutes les Cond Lims renvoyant 1 par le biais de la methode
//    int Cond_Lim_base::a_mettre_a_jour_ss_pas_dt();
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
void Zone_Cl_dis_base::mettre_a_jour_ss_pas_dt(double temps)
{
  const int nb_cl = les_conditions_limites_.size();
  for (int i=0; i<nb_cl; i++)
    {
      Cond_lim_base& la_cl = les_conditions_limites_[i].valeur();
      if (la_cl.a_mettre_a_jour_ss_pas_dt()==1)
        {
          la_cl.mettre_a_jour(temps);
        }
    }
}


// Description:
//    Initialise les CLs
//    Contrairement aux methodes mettre_a_jour, les methodes
//    initialiser des CLs ne peuvent pas dependre de l'exterieur
//    (lui-meme peut ne pas etre initialise)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si OK, 0 sinon
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Zone_Cl_dis_base::initialiser(double temps)
{
  return les_conditions_limites_.initialiser(temps);
}



// Description:
//    Calcul des coefficients d'echange pour les problemes couples thermiques
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Zone_Cl_dis_base::calculer_coeffs_echange(double temps)
{
  les_conditions_limites_.calculer_coeffs_echange(temps);
  return 1;
}

// Description:
//    Appel Cond_lim_base::completer() sur chaque
//    condition aux limites
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
void Zone_Cl_dis_base::completer()
{
  les_conditions_limites_.completer(zone_dis());
  completer(zone_dis());
}

// Description :
// Renvoie la condition limite associee a une face reelle donnee.
// Met dans face_locale le numero de la face dans la frontiere.
// Provoque une erreur si la face ne porte pas de CL.
const Cond_lim_base& Zone_Cl_dis_base::condition_limite_de_la_face_reelle(int face_globale, int& face_locale) const
{
  for (int i=0; i<nb_cond_lim(); i++)
    {
      const Frontiere& fr=les_conditions_limites(i).frontiere_dis().frontiere();
      if (face_globale>=fr.num_premiere_face() && face_globale < fr.num_premiere_face()+fr.nb_faces())
        {
          face_locale=face_globale-fr.num_premiere_face();
          return les_conditions_limites(i).valeur();
        }
    }
  assert(0); // la face ne porte pas de CL
  return les_conditions_limites(0).valeur(); // Pour compilo
}


// Description :
// Renvoie la condition limite associee a une face virtuelle donnee.
// Met dans face_locale le numero de la face dans la frontiere.
// Provoque une erreur si la face ne porte pas de CL.
const Cond_lim_base& Zone_Cl_dis_base::condition_limite_de_la_face_virtuelle(int face_globale, int& face_locale) const
{
  for (int i=0; i<nb_cond_lim(); i++)
    {
      const Frontiere& fr=les_conditions_limites(i).frontiere_dis().frontiere();
      const ArrOfInt& faces_virt=fr.get_faces_virt();
      for (int j=0; j<faces_virt.size_array(); j++)
        if (face_globale==faces_virt[j])
          {
            face_locale=fr.nb_faces()+j;
            return les_conditions_limites(i).valeur();
          }
    }
  assert(0); // la face ne porte pas de CL
  return les_conditions_limites(0).valeur(); // Pour compilo
}

// Description :
// Renvoie la condition limite associee a une frontiere de nom donne.
// Provoque une erreur si aucune frontiere ne porte ce nom.
Cond_lim_base& Zone_Cl_dis_base::condition_limite_de_la_frontiere(Nom frontiere)
{
  for (int i=0; i<nb_cond_lim(); i++)
    {
      const Frontiere& fr=les_conditions_limites(i).frontiere_dis().frontiere();
      if (fr.le_nom()==frontiere)
        return les_conditions_limites(i).valeur();
    }
  assert(0); // Aucune frontiere de ce nom
  exit();
  return les_conditions_limites(0).valeur(); // Pour compilo
}

// Description :
// Renvoie la condition limite associee a une frontiere de nom donne.
// Provoque une erreur si aucune frontiere ne porte ce nom.
const Cond_lim_base& Zone_Cl_dis_base::condition_limite_de_la_frontiere(Nom frontiere) const
{
  for (int i=0; i<nb_cond_lim(); i++)
    {
      const Frontiere& fr=les_conditions_limites(i).frontiere_dis().frontiere();
      if (fr.le_nom()==frontiere)
        return les_conditions_limites(i).valeur();
    }
  assert(0); // Aucune frontiere de ce nom
  exit();
  return les_conditions_limites(0).valeur(); // Pour compilo
}



// Description :
// Calcule le taux d'accroissement des CLs instationnaires entre t1 et t2.
void Zone_Cl_dis_base::Gpoint(double t1, double t2)
{
  for (int i=0; i<nb_cond_lim(); i++)
    {
      Champ_front_base& champ=les_conditions_limites(i)->champ_front().valeur();

      if (sub_type(Champ_front_var_instationnaire,champ))
        {
          Champ_front_var_instationnaire& champ_insta=ref_cast(Champ_front_var_instationnaire,champ);
          champ_insta.Gpoint(t1,t2);
        }

      if (sub_type(Champ_front_instationnaire_base,champ))
        {
          Champ_front_instationnaire_base& champ_insta=ref_cast(Champ_front_instationnaire_base,champ);
          champ_insta.Gpoint(t1,t2);
        }

    }
}


// Description:
//    Renvoie la i-ieme condition aux limites.
//    (version const)
// Precondition:
// Parametre: int i
//    Signification: le rang de la i-ieme condition aux limites
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Cond_lim&
//    Signification: la i-ieme condition aux limites
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Cond_lim& Zone_Cl_dis_base::les_conditions_limites(int i) const
{
  return les_conditions_limites_[i];
}


// Description:
//    Renvoie la i-ieme condition aux limites.
// Precondition:
// Parametre: int i
//    Signification: le rang de la i-ieme condition aux limites
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:  Cond_lim&
//    Signification: la i-ieme condition aux limites
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Cond_lim& Zone_Cl_dis_base::les_conditions_limites(int i)
{
  return les_conditions_limites_[i];
}


// Description:
//    Renvoie le tableaux des conditions aux limites.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Conds_lim&
//    Signification: le tableau des conditions aux limites
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Conds_lim& Zone_Cl_dis_base::les_conditions_limites()
{
  return les_conditions_limites_;
}


// Description:
//    Renvoie le tableaux des conditions aux limites.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Conds_lim&
//    Signification: le tableau des conditions aux limites
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Conds_lim& Zone_Cl_dis_base::les_conditions_limites() const
{
  return les_conditions_limites_;
}


// Description:
//    Renvoie le nombre de conditions aux limites.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de conditions aux limites
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Zone_Cl_dis_base::nb_cond_lim() const
{
  return les_conditions_limites_.size();
}

int  Zone_Cl_dis_base::nb_faces_Cl() const
{
  return zone().nb_faces_frontiere();
}

Zone& Zone_Cl_dis_base::zone()
{
  return zone_dis().zone();
}
const Zone& Zone_Cl_dis_base::zone() const
{
  return zone_dis().zone();
}

// Description:
//  A partir d'un indice de face de bord dans la Zone_VF,
//  renvoie la condition aux limites a laquelle cette face
//  appartient, pour 0 <= num_face < nb_faces_Cl().
const Cond_lim& Zone_Cl_dis_base::la_cl_de_la_face(int num_face) const
{
  //  Algorithme generique: on parcourt les bords jusqu'a trouver
  //  celui qui contient la face.
  //  Les faces du bord 0, puis celle du bord 1, etc... se suivent
  //  dans la Zone_VF.
  assert(num_face >= 0);
  int i = 0;
  const int nb_cl = les_conditions_limites_.size();
  for (i = 0; i < nb_cl; i++)
    {
      const Cond_lim_base& cl = les_conditions_limites_[i].valeur();
      const Frontiere&      fr = cl.frontiere_dis().frontiere();
      int num_premiere_face = fr.num_premiere_face();
      int nb_faces          = fr.nb_faces();

      if (num_face >= num_premiere_face && num_face < num_premiere_face + nb_faces)
        break;
    }
  if (i == nb_cl)
    {
      Cerr << "Error in Zone_Cl_dis_base::la_cl_de_la_face(num_face="
           << num_face << ")\n This face is not on a boundary of cond_lim."
           << finl;
      exit();
    }
  return les_conditions_limites_[i];
}

void Zone_Cl_dis_base::nommer(const Nom& un_nom)
{
  nom_ = un_nom;
}

void Zone_Cl_dis_base::associer_inconnue(const Champ_Inc& inco)
{
  mon_inconnue=inco;
}

const Champ_Inc& Zone_Cl_dis_base::inconnue() const
{
  return mon_inconnue;
}

Champ_Inc& Zone_Cl_dis_base::inconnue()
{
  return mon_inconnue;
}
