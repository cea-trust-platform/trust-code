/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Domaine_Cl_dis_base.h>
#include <Equation_base.h>
#include <Frontiere_dis_base.h>
#include <Schema_Temps_base.h>
#include <Champ_front_var_instationnaire.h>
#include <Champ_front_instationnaire_base.h>
#include <Cond_lim_utilisateur_base.h>
#include <Probleme_base.h>

Implemente_base(Domaine_Cl_dis_base,"Domaine_Cl_dis_base",Objet_U);


/*! @brief Surcharge Objet_U::printOn(Sortie&) NE FAIT RIEN
 *
 *     A surcharger dans les classes derivees.
 *     Imprime les conditions aux limites discretisees sur un flot de sortie
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Domaine_Cl_dis_base::printOn(Sortie& os) const
{
  return os;
}


/*! @brief Surcharge Objet_U::readOn(Sortie&) Lit les conditions aux limites discretisees a partir d'un flot d'entree
 *
 *     Le format attendu est le suivant:
 *     {
 *      Nom Cond_lim [REPETER LECTURE AUTANT DE FOIS QUE NECESSAIRE]
 *     }
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws accolade ouvrante attendue
 * @throws nombre de conditions aux limites lues invalide
 */
Entree& Domaine_Cl_dis_base::readOn(Entree& is)
{
  assert(mon_equation.non_nul());
  const Domaine& ledomaine=equation().domaine_dis()->domaine();
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

  int n = ledomaine.nb_front_Cl();
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

      int rang=ledomaine.rang_frontiere(nomlu);

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
      //      const Frontiere& frontiere=domaine.frontiere(rang);
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
      associer_fr_dis_base(domaine_dis().frontiere_dis(rang));

      //Test pour empecher l utilisation de 'Raccord_distant_homogene' en calcul sequentiel
      const Frontiere& frontiere=ledomaine.frontiere(rang);
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
      domaine_dis()->ecrire_noms_bords(Cerr);
      Cerr << "It misses " << n-nb_clim << " boundaries conditions " << finl;
      Cerr << "We read " << nb_clim << " boundaries conditions " << finl;
      Cerr << "We waited " << n << " boundary conditions " << finl;
      exit();
    }
  for (int num_cl=0; num_cl<nb_clim; num_cl++)
    {
      les_conditions_limites(num_cl)->associer_domaine_cl_dis_base(*this);
      les_conditions_limites(num_cl)->verifie_ch_init_nb_comp();
      //      const Frontiere_dis_base& la_fr_dis = les_conditions_limites(num_cl).frontiere_dis();
      //      const Frontiere& frontiere=la_fr_dis.frontiere();
      //      const Domaine& domaine2=frontiere.domaine();
    }

  return is;
}


/*! @brief Renvoie 1 si l'objet contient une condition aux limites du Nom specifie.
 *
 *     Renvoie 0 sinon.
 *
 * @param (Nom& type) le nom de la condition aux limites a chercher
 * @return (int) 1 si la condition aux limites de nom specifie a ete trouve, 0 sinon.
 */
int Domaine_Cl_dis_base::contient_Cl(const Nom& type)
{
  for (auto& itr : les_conditions_limites_)
    if (itr.get_info()->has_base(type)) return 1;
  return 0;
}


/*! @brief Renvoie une reference sur le domaine discretisee associee aux conditions aux limites.
 *
 * Cette Domaine_dis est associee au travers de l'equation
 *     associee et pas directement a l'objet Domaine_Cl_dis_base.
 *
 * @return (Domaine_dis&) le domaine discretisee associee a l'equation associe aux conditions aux limites.
 */
Domaine_dis& Domaine_Cl_dis_base::domaine_dis()
{
  return equation().domaine_dis();
}


/*! @brief Renvoie une reference sur le domaine discretisee associee aux conditions aux limites.
 *
 * Cette Domaine_dis est associee au travers de l'equation
 *     associee et pas directement a l'objet Domaine_Cl_dis_base.
 *     (version const)
 *
 * @return (Domaine_dis&) le domaine discretisee associee a l'equation associe aux conditions aux limites.
 */
const Domaine_dis& Domaine_Cl_dis_base::domaine_dis() const
{
  return equation().domaine_dis();
}


/*! @brief Change le i-eme temps futur de toutes les CLs.
 *
 */
void Domaine_Cl_dis_base::changer_temps_futur(double temps,int i)
{
  for (int j=0; j<nb_cond_lim(); j++)
    les_conditions_limites_[j]->changer_temps_futur(temps,i);
}

/*! @brief Change le i-eme temps futur de toutes les CLs.
 *
 */
void Domaine_Cl_dis_base::set_temps_defaut(double temps)
{
  for (int j=0; j<nb_cond_lim(); j++)
    les_conditions_limites_[j]->set_temps_defaut(temps);
}

/*! @brief Tourne la roue des CLs jusqu'au temps donne
 *
 */
int Domaine_Cl_dis_base::avancer(double temps)
{
  int ok=1;
  for (int j=0; j<nb_cond_lim(); j++)
    ok = ok && les_conditions_limites_[j]->avancer(temps);
  return ok;
}

/*! @brief Tourne la roue des CLsj usqu'au temps donne
 *
 */
int Domaine_Cl_dis_base::reculer(double temps)
{
  int ok=1;
  for (int j=0; j<nb_cond_lim(); j++)
    ok = ok && les_conditions_limites_[j]->reculer(temps);
  return ok;
}

/*! @brief Effectue une mise a jour en temps de toutes les conditions aux limites.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
void Domaine_Cl_dis_base::mettre_a_jour(double temps)
{
  les_conditions_limites_.mettre_a_jour(temps);
}


/*! @brief Effectue une mise a jour pour des sous pas de temps d'un schema en temps (par exemple dans RungeKutta)
 *
 *     pour toutes les Cond Lims renvoyant 1 par le biais de la methode
 *     int Cond_Lim_base::a_mettre_a_jour_ss_pas_dt();
 *
 * @param (double temps) le pas de temps de mise a jour
 */
void Domaine_Cl_dis_base::mettre_a_jour_ss_pas_dt(double temps)
{
  for (auto &itr : les_conditions_limites_)
    {
      Cond_lim_base& la_cl = itr.valeur();
      if (la_cl.a_mettre_a_jour_ss_pas_dt() == 1)
        la_cl.mettre_a_jour(temps);
    }
}


/*! @brief Initialise les CLs Contrairement aux methodes mettre_a_jour, les methodes
 *
 *     initialiser des CLs ne peuvent pas dependre de l'exterieur
 *     (lui-meme peut ne pas etre initialise)
 *
 * @return (int) 1 si OK, 0 sinon
 */
int Domaine_Cl_dis_base::initialiser(double temps)
{
  return les_conditions_limites_.initialiser(temps);
}



/*! @brief Calcul des coefficients d'echange pour les problemes couples thermiques
 *
 * @return (int) renvoie toujours 1
 */
int Domaine_Cl_dis_base::calculer_coeffs_echange(double temps)
{
  les_conditions_limites_.calculer_coeffs_echange(temps);
  return 1;
}

/*! @brief Appel Cond_lim_base::completer() sur chaque condition aux limites
 *
 */
void Domaine_Cl_dis_base::completer()
{
  les_conditions_limites_.completer(domaine_dis());
  completer(domaine_dis());
}

/*! @brief Renvoie la condition limite associee a une face reelle donnee.
 *
 * Met dans face_locale le numero de la face dans la frontiere.
 *  Provoque une erreur si la face ne porte pas de CL.
 *
 */
const Cond_lim_base& Domaine_Cl_dis_base::condition_limite_de_la_face_reelle(int face_globale, int& face_locale) const
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


/*! @brief Renvoie la condition limite associee a une face virtuelle donnee.
 *
 * Met dans face_locale le numero de la face dans la frontiere.
 *  Provoque une erreur si la face ne porte pas de CL.
 *
 */
const Cond_lim_base& Domaine_Cl_dis_base::condition_limite_de_la_face_virtuelle(int face_globale, int& face_locale) const
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

/*! @brief Renvoie la condition limite associee a une frontiere de nom donne.
 *
 * Provoque une erreur si aucune frontiere ne porte ce nom.
 *
 */
Cond_lim_base& Domaine_Cl_dis_base::condition_limite_de_la_frontiere(Nom frontiere)
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

/*! @brief Renvoie la condition limite associee a une frontiere de nom donne.
 *
 * Provoque une erreur si aucune frontiere ne porte ce nom.
 *
 */
const Cond_lim_base& Domaine_Cl_dis_base::condition_limite_de_la_frontiere(Nom frontiere) const
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



/*! @brief Calcule le taux d'accroissement des CLs instationnaires entre t1 et t2.
 *
 */
void Domaine_Cl_dis_base::Gpoint(double t1, double t2)
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


/*! @brief Renvoie la i-ieme condition aux limites.
 *
 * (version const)
 *
 * @param (int i) le rang de la i-ieme condition aux limites
 * @return (Cond_lim&) la i-ieme condition aux limites
 */
const Cond_lim& Domaine_Cl_dis_base::les_conditions_limites(int i) const
{
  return les_conditions_limites_[i];
}


/*! @brief Renvoie la i-ieme condition aux limites.
 *
 * @param (int i) le rang de la i-ieme condition aux limites
 * @return (Cond_lim&) la i-ieme condition aux limites
 */
Cond_lim& Domaine_Cl_dis_base::les_conditions_limites(int i)
{
  return les_conditions_limites_[i];
}


/*! @brief Renvoie le tableaux des conditions aux limites.
 *
 * @return (Conds_lim&) le tableau des conditions aux limites
 */
Conds_lim& Domaine_Cl_dis_base::les_conditions_limites()
{
  return les_conditions_limites_;
}


/*! @brief Renvoie le tableaux des conditions aux limites.
 *
 * (version const)
 *
 * @return (Conds_lim&) le tableau des conditions aux limites
 */
const Conds_lim& Domaine_Cl_dis_base::les_conditions_limites() const
{
  return les_conditions_limites_;
}


/*! @brief Renvoie le nombre de conditions aux limites.
 *
 * @return (int) le nombre de conditions aux limites
 */
int Domaine_Cl_dis_base::nb_cond_lim() const
{
  return les_conditions_limites_.size();
}

int  Domaine_Cl_dis_base::nb_faces_Cl() const
{
  return domaine().nb_faces_frontiere();
}

Domaine& Domaine_Cl_dis_base::domaine()
{
  return domaine_dis().domaine();
}
const Domaine& Domaine_Cl_dis_base::domaine() const
{
  return domaine_dis().domaine();
}

/*! @brief A partir d'un indice de face de bord dans le Domaine_VF, renvoie la condition aux limites a laquelle cette face
 *
 *   appartient, pour 0 <= num_face < nb_faces_Cl().
 *
 */
const Cond_lim& Domaine_Cl_dis_base::la_cl_de_la_face(int num_face) const
{
  //  Algorithme generique: on parcourt les bords jusqu'a trouver
  //  celui qui contient la face.
  //  Les faces du bord 0, puis celle du bord 1, etc... se suivent
  //  dans le Domaine_VF.
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
      Cerr << "Error in Domaine_Cl_dis_base::la_cl_de_la_face(num_face="
           << num_face << ")\n This face is not on a boundary of cond_lim."
           << finl;
      exit();
    }
  return les_conditions_limites_[i];
}

void Domaine_Cl_dis_base::nommer(const Nom& un_nom)
{
  nom_ = un_nom;
}

void Domaine_Cl_dis_base::associer_inconnue(const Champ_Inc& inco)
{
  mon_inconnue=inco;
}

const Champ_Inc& Domaine_Cl_dis_base::inconnue() const
{
  return mon_inconnue;
}

Champ_Inc& Domaine_Cl_dis_base::inconnue()
{
  return mon_inconnue;
}
