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

#include <Schema_Temps_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <communications.h>
#include <Source_base.h>
#include <TRUSTTrav.h>
#include <TRUSTTabs.h>
#include <SFichier.h>
#include <map>

Implemente_base(Source_base,"Source_base",Objet_U);


/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Imprime la source sur un flot de sortie.
 *
 * @param (Sortie& os) le flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Source_base::printOn(Sortie& os) const
{
  return os;
}


/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Lecture d'un terme source sur un flot d'entree.
 *
 * @param (Entree& is) le flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Source_base::readOn(Entree& is)
{
  return is;
}


/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Mise a jour en temps du terme source.
 *
 * @param (double) le pas de temps de mise a jour
 */
void Source_base::mettre_a_jour(double )
{
  Cerr << finl;
  Cerr << "You must to overload the method Source_base::mettre_a_jour() because there's big" << finl;
  Cerr << "chances that the source term " <<  que_suis_je() << " must be updated," << finl;
  Cerr << "especially if your source has a field, you must update it..." << finl << finl;
  exit();
}


/*! @brief Met a jour les references internes a l'objet Source_base.
 *
 * Appelle 2 methodes virtuelles pures protegees:
 *        Source_base::associer_domaines(const Domaine_dis& ,const Domaine_Cl_dis&)
 *        Source_base::associer_pb(const Probleme_base&)
 *
 */
void Source_base::completer()
{
  const Equation_base& eqn = equation();
  const Domaine_dis& zdis= eqn.domaine_dis();
  const Domaine_Cl_dis& zcldis = eqn.domaine_Cl_dis();
  associer_domaines(zdis, zcldis);
  associer_pb(eqn.probleme());
  // Initialize the bilan_ array:
  bilan_.resize(eqn.inconnue().valeur().nb_comp());
  bilan_=0;
}

/*! @brief Cette methode (ou la methode de la classe derivee) est appelee par Sources::associer_champ_rho pour chaque source de la liste
 *
 *   (par exemple, a l'initialisation d'un calcul front-tracking).
 *   Cette methode doit etre reimplementee dans les classes derivees
 *   utilisees dans les problemes a rho variable.
 *
 *   La methode ajouter calcule le terme suivant:
 *     INTEGRALE            (terme source)
 *     sur volume entrelace
 *
 *   Dans les problemes ou rho est variable, "terme source" homogene a rho*v.
 *   Sinon, "terme source" est homogene a v.
 *
 */
void Source_base::associer_champ_rho(const Champ_base& champ_rho)
{
  Cerr << "In Source_base::associer_champ_rho" << finl;
  Cerr << " field : " << champ_rho.le_nom() << finl;
  Cerr << " Source of type : " << que_suis_je() << finl;
  Cerr << " This source does not support the association of a field rho." << finl;
  Cerr << " (method associer_champ_rho must be coded for this source term)." << finl;
  assert(0);
  exit();
}

/*! @brief Si la source comprend le motcle "mot", elle remplit la reference a ch_ref et renvoie 1, sinon renvoie 0 (voir Source_Translation par ex.
 *
 * )
 *
 */
int Source_base::a_pour_Champ_Fonc(const Motcle& mot,
                                   REF(Champ_base)& ch_ref) const
{
  // La classe de base ne comprend aucun motcle
  return 0;
}

void Source_base::creer_champ(const Motcle& motlu)
{
}

const Champ_base& Source_base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);
}
bool Source_base::has_champ(const Motcle& nom, REF(Champ_base)& ref_champ) const
{
  return champs_compris_.has_champ(nom, ref_champ);
}

void Source_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<que_suis_je()<<" : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());
}


/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Mise a jour en temps du terme source.
 *
 * @param (double) le pas de temps de mise a jour
 */
static std::map<std::string, int> counters; // ToDo provisoire
int Source_base::impr(Sortie& os) const
{
  if (out_=="??")
    {
      //Cerr << "No balance printed for " << que_suis_je() << finl;
    }
  else
    {
      int nb_compo=bilan_.size();
      if (nb_compo==0)
        {
          Cerr << "No balance printed for " << que_suis_je() << finl;
          Cerr << "cause bilan_ array is not filled." << finl;
        }
      else
        {
          int flag=je_suis_maitre();
          //SFichier Flux;
          if (!Flux.is_open()) ouvrir_fichier(Flux,"",flag);
          const Probleme_base& pb=equation().probleme();
          const Schema_Temps_base& sch=pb.schema_temps();
          double temps=sch.temps_courant();

          if(Process::je_suis_maitre())
            Flux.add_col(temps);
          DoubleVect bilan_p(bilan_);
          mp_sum_for_each_item(bilan_p);
          /*
          for(int k=0; k<nb_compo; k++)
               bilan_(k)=Process::mp_sum(bilan_(k)); */
          // Replaced by:
          // mp_sum_for_each_item(bilan_); // Fixed bug: double line!

          if (Process::je_suis_maitre())
            {
              for(int k=0; k<nb_compo; k++)
                Flux.add_col(bilan_p(k));
              Flux << finl;
            }
        }
    }
  return 1;
}
/*! @brief Dimensionnement de la matrice implicite des termes sources.
 *
 * Par defaut ne fait rien.
 *
 */
void Source_base::dimensionner(Matrice_Morse& mat) const
{
  if (has_interface_blocs()) dimensionner_blocs({{ equation().inconnue().le_nom().getString(), &mat }});
}

void Source_base::dimensionner_bloc_vitesse(Matrice_Morse& mat) const
{
  if (has_interface_blocs()) dimensionner_blocs({{ "vitesse", &mat }});
}

DoubleTab& Source_base::ajouter(DoubleTab& secmem) const
{
  if (has_interface_blocs()) ajouter_blocs({}, secmem, {});
  else Process::exit(que_suis_je() + " : ajouter() not coded!");
  return secmem;
}

DoubleTab& Source_base::calculer(DoubleTab& secmem) const
{
  secmem = 0;
  return ajouter(secmem);
}

/*! @brief contribution a la matrice implicite des termes sources par defaut pas de contribution
 *
 */
void Source_base::contribuer_a_avec(const DoubleTab&, Matrice_Morse& mat) const
{
  if (!has_interface_blocs()) return;
  DoubleTrav secmem(equation().inconnue().valeurs()); //sera jete
  ajouter_blocs({{ equation().inconnue().le_nom().getString(), &mat}}, secmem, {});
}

/*! @brief contribution au second membres des termes sources en implicite par defaut erreur
 *
 *  methode presente par coherence avec Operateur_base
 *
 */
void Source_base::contribuer_au_second_membre(DoubleTab& ) const
{
  Cerr<<"Source_base::contribuer_au_second_membre(DoubleTab& ) const uncoded"<<finl;
  exit();
}

/* par defaut erreur */
void Source_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  Process::exit(que_suis_je() + " : dimensionner_blocs() not coded!");
}
void Source_base::ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  Process::exit(que_suis_je() + " : ajouter_blocs() not coded!");
}

/*! @brief Contrairement aux methodes mettre_a_jour, les methodes initialiser des sources ne peuvent pas dependre de l'exterieur
 *
 *     (lui-meme peut ne pas etre initialise)
 *     Par defaut, mettre_a_jour
 *
 */
int Source_base::initialiser(double temps)
{
  mettre_a_jour(temps);
  return 1;
}


/*! @brief Ouverture/creation d'un fichier d'impression d'un terme source A surcharger dans les classes derivees.
 *
 * @throws methode a surcharger
 */
void Source_base::ouvrir_fichier(SFichier& os,const Nom& type, const int flag) const
{
  // flag nul on n'ouvre pas le fichier
  if (flag==0)
    return ;
  // ToDo provisoire:
  counters[type.getString()]++;
  if (counters[type.getString()]>1 && type!="")
    {
      Cerr << "Code should be rewritten to have only one call to Source_base::ouvrir_fichier for " << type << " source and not " << counters[type.getString()] << " times." << finl;
      Process::exit();
    }
  const Probleme_base& pb=equation().probleme();
  const Schema_Temps_base& sch=pb.schema_temps();
  const int precision = sch.precision_impr(), wcol = std::max(col_width_, sch.wcol()), gnuplot_header = sch.gnuplot_header();
  os.set_col_width(wcol);

  Nom nomfichier(out_);
  if (type!="") nomfichier+=(Nom)"_"+type;
  nomfichier+=".out";

  // On cree le fichier a la premiere impression avec l'en tete
  if (sch.nb_impr()==1 && !pb.reprise_effectuee())
    {
      os.ouvrir(nomfichier);
      SFichier& fic=os;
      //Nom espace="\t\t";
      fic << (Nom)"# Printing of the source term "+que_suis_je()+" of the equation "+equation().que_suis_je()+" of the problem "+equation().probleme().le_nom() << finl;
      fic << "# " << description() << finl;
      if (!gnuplot_header) fic << "#";
      os.set_col_width(wcol - !gnuplot_header);
      fic.add_col("Time");
      os.set_col_width(wcol);
      for (int i = 0; i < col_names_.size(); i++)
        fic.add_col(col_names_[i]);
      fic << finl;
    }
  // Sinon on l'ouvre
  else
    {
      os.ouvrir(nomfichier,ios::app);
    }
  os.precision(precision);
  os.setf(ios::scientific);
}

void Source_base::set_fichier(const Nom& nom)
{
  out_=Objet_U::nom_du_cas();
  out_+="_";
  out_+=equation().probleme().le_nom()+"_"+nom;
}
