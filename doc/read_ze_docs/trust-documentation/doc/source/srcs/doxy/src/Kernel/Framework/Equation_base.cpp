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

#include <Parametre_diffusion_implicite.h>
#include <Solveur_Implicite_Base.h>
#include <Schema_Euler_explicite.h>
#include <Schema_Implicite_base.h>
#include <Source_dep_inco_base.h>
#include <Operateur_Conv_base.h>
#include <Op_Conv_negligeable.h>
#include <Operateur_Diff_base.h>
#include <Discretisation_base.h>
#include <Matrice_Morse_Diag.h>
#include <Frontiere_dis_base.h>
#include <Matrice_Morse_Sym.h>
#include <Operateur_base.h>
#include <TRUSTTab_parts.h>
#include <EcrFicPartage.h>
#include <Postraitement.h>
#include <Equation_base.h>
#include <Statistiques.h>
#include <Milieu_base.h>
#include <SolveurSys.h>
#include <Operateur.h>
#include <EChaine.h>
#include <Domaine_VF.h>
#include <Avanc.h>
#include <Debog.h>
#include <Param.h>

extern Stat_Counter_Id assemblage_sys_counter_;
extern Stat_Counter_Id diffusion_implicite_counter_;
extern Stat_Counter_Id source_counter_;

Implemente_base_sans_constructeur(Equation_base,"Equation_base",Objet_U);
// XD eqn_base mor_eqn eqn_base -3 Basic class for equations.

Equation_base::Equation_base()
{
  nombre_champ_xyz=0;
  ecrit_champ_xyz_bin=0;
  sys_invariant_=1;
  implicite_=-1;
  has_time_factor_= false;
  champs_compris_.ajoute_nom_compris("volume_maille");
  Nom expr_equation_non_resolue="0";
  equation_non_resolue_.setNbVar(1);
  equation_non_resolue_.setString(expr_equation_non_resolue); // Valeur par defaut, equation resolue
  equation_non_resolue_.addVar("t");
  equation_non_resolue_.parseString();
  set_calculate_time_derivative(0);

}

int Equation_base::equation_non_resolue() const
{
  std::string str= equation_non_resolue_.getString();
  std::string str2("INPUT_INT_VALUE");
  if (str.compare(str2)==0)
    {
      Nom name = eq_non_resolue_input_.getName();
      int eq_non_resolue_int = mon_probleme.valeur().getOutputIntValue(name);
      return eq_non_resolue_int;
    }
  else
    {
      double t = le_schema_en_temps->temps_courant();
      equation_non_resolue_.setVar("t",t);
      return (int)equation_non_resolue_.eval();
    }
}

/*! @brief Renvoie le domaine discretisee associee a l'equation.
 *
 * @return (Domaine_dis&) le domaine discretisee asscoiee a l'equation
 * @throws l'objet domaine discretisee (Domaine_dis) est invalide,
 * probleme associe non discretise.
 */
Domaine_dis& Equation_base::domaine_dis()
{
  if (!le_dom_dis.non_nul())
    {
      Cerr << "There is no object of type Domaine_dis yet associated to the equation " << que_suis_je() << finl;
      Cerr << "This means that the problem has not been discretized or" << finl;
      Cerr << "that instruction Discretiser is misplaced" << finl;
      exit();
    }
  return le_dom_dis.valeur();
}

/*! @brief Renvoie le domaine discretisee associee a l'equation.
 *
 * (version const)
 *
 * @return (Domaine_dis&) le domaine discretisee asssociee a l'equation
 * @throws l'objet domaine discretisee (Domaine_dis) est invalide,
 * probleme associe non discretise.
 */
const Domaine_dis& Equation_base::domaine_dis() const
{
  if (!le_dom_dis.non_nul())
    {
      Cerr << "There is no object of type Domaine_dis associated to the equation " << que_suis_je() << finl;
      Cerr << "This means that the problem has not been discretized" << finl;
      exit();
    }
  assert(le_dom_dis.non_nul());
  return le_dom_dis.valeur();
}


/*! @brief Complete la construction (initialisation) des objets associes a l'equation.
 *
 *     Complete les sources, associe l'equation a l'inconnue complete les
 *     operateurs, complete les conditions aux limites discretisees.
 *     Voir les methodes Source_base::completer(),
 *                       Operateur_base::completer()
 *                       Domaine_Cl_dis_base::completer()
 *                       Domaine_Cl_dis_base::completer(const Domaine_dis& )
 *
 */
void Equation_base::completer()
{
  inconnue().associer_eqn(*this);
  if (le_dom_Cl_dis.non_nul())
    le_dom_Cl_dis->completer();

  inconnue()->associer_domaine_cl_dis(le_dom_Cl_dis);

  if (mon_probleme.valeur().is_coupled())
    {
      bool err = false;
      int nb_cl = le_dom_Cl_dis->nb_cond_lim();
      for (int i=0; i<nb_cl; i++)
        {
          const Cond_lim_base& la_cl = le_dom_Cl_dis.valeur().les_conditions_limites(i);
          const Frontiere& la_frontiere = la_cl.frontiere_dis().frontiere();

          bool raccord_found = false;

          // VDF and PolyMAC_P0P1NC
          if (la_cl.que_suis_je().debute_par("Paroi_Echange_contact") || la_cl.que_suis_je().debute_par("Echange_contact_Rayo"))
            raccord_found = true;

          // VEF
          if (la_cl.que_suis_je().debute_par("Scalaire_impose_paroi") || la_cl.que_suis_je().debute_par("paroi_temperature_imposee_rayo") || la_cl.que_suis_je().debute_par("temperature_imposee_paroi"))
            raccord_found = true;

          if ( raccord_found && !sub_type(Raccord_base,la_frontiere))
            {
              Cerr << "====================================================================" << finl;
              Cerr << "Boundary " << la_frontiere.le_nom() << " should be a raccord" << finl;
              Cerr << "Add in your data file between the definition and the partition of the domain " << mon_probleme.valeur().domaine().le_nom() << " : " << finl;
              Cerr << "Modif_bord_to_raccord " << mon_probleme.valeur().domaine().le_nom() << " " << la_frontiere.le_nom() << finl;
              err = true;
            }
        }
      if(err)
        exit();
    }

  //Ajout d un element vide qui sera renvoye si pas de modele trouve
  RefObjU mod;
  if (liste_modeles_.size()==0)
    liste_modeles_.add(mod);

  // pour les eqns n'appelant pas preparer_calcul
  initialise_residu();
  int nb_op=nombre_d_operateurs();
  Nom msg="Equation_base::completer(), nb_op = " ;
  Debog::verifier(msg, nb_op);
  for(int i=0; i<nb_op; i++)
    operateur(i).completer();

  if (solv_masse().non_nul())  // [ABN]: In case of Front-Tracking, mass solver mass might be uninitialized ...
    solv_masse().valeur().completer();

  les_sources.completer();
  schema_temps().completer();

  // Determine implicite_ and sys_invariant_ variables:
  implicite_=0;
  for(int i=0; i<nombre_d_operateurs(); i++)
    {
      if(operateur(i).l_op_base().get_decal_temps()==1)
        {
          ++implicite_;
          sys_invariant_*=operateur(i).l_op_base().systeme_invariant();
        }
    }
}


/*! @brief Surcharge Objet_U::printOn Imprime l'equation et ses composants sur un flot de sortie.
 *
 *     Imprime le nom de l'equation, le solveur masse, les termes sources
 *     les conditions aux limites discretisees, les inconnues et les operateurs.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Equation_base::printOn(Sortie& os) const
{
  os << nom_ << finl;
  os << solveur_masse;
  os << les_sources;
  os << le_dom_Cl_dis;
  os << inconnue() ;
  for(int i=0; i<nombre_d_operateurs(); i++)
    {
      os << operateur(i).l_op_base();
    }
  return os;
}

/*! @brief Lecture d'une equation sur un flot d'entree.
 *
 * Le format est le suivant:
 *       {
 *         [Source { [sou_1], [sour_2], ...} ]
 *          Conditions_limites { [cl_1] [cl_2] ... }
 *          Conditions_initiales { [cl_init] }
 *       }
 *
 * @param (Entree& is) un flot d'entree pour lire l'equation
 * @return (Entree&) le flot d'entree modifie
 * @throws mauvais format de lecture, accolade ouvrante attendue
 * @throws pas de conditions initiales, il faut initialiser l'inconnue
 * @throws pas de conditions aux limites, il faut donner des conditions aux limites
 */
Entree& Equation_base::readOn(Entree& is)
{
  Cerr<<"Reading of data for a "<<que_suis_je()<<" equation"<<finl;
  Param param(que_suis_je());
  set_param(param);
  Nom expr_equation_non_resolue="0";
  param.ajouter("disable_equation_residual",&disable_equation_residual_); // XD attr disable_equation_residual chaine disable_equation_residual 1 The equation residual will not be used for the problem residual used when checking time convergence or computing dynamic time-step
  equation_non_resolue_.setString(expr_equation_non_resolue);
  param.lire_avec_accolades_depuis(is);
  matrice_init = 0;
  return is;
}

// XD attr convection bloc_convection convection 1 Keyword to alter the convection scheme.
// XD attr diffusion bloc_diffusion diffusion 1 Keyword to specify the diffusion operator.
void Equation_base::set_param(Param& param)
{
  param.ajouter_non_std("conditions_limites|boundary_conditions",(this),Param::REQUIRED);  // XD attr conditions_limites|boundary_conditions condlims conditions_limites 1 Boundary conditions.
  param.ajouter_non_std("conditions_initiales|initial_conditions",(this),Param::REQUIRED); // XD attr conditions_initiales|initial_conditions condinits conditions_initiales 1 Initial conditions.
  param.ajouter_non_std("sources",(this)); // XD attr sources sources sources 1 To introduce a source term into an equation (in case of several source terms into the same equation, the blocks corresponding to the various terms need to be separated by a comma)
  param.ajouter_non_std("ecrire_fichier_xyz_valeur_bin",(this)); // XD attr ecrire_fichier_xyz_valeur_bin ecrire_fichier_xyz_valeur_param ecrire_fichier_xyz_valeur_bin 1 This keyword is used to write the values of a field only for some boundaries in a binary file with the following format: n_valeur NL2 x_1 y_1 [z_1] val_1 NL2 ... NL2 x_n y_n [z_n] val_n NL2 The created files are named : pbname_fieldname_[boundaryname]_time.dat
  param.ajouter_non_std("ecrire_fichier_xyz_valeur",(this)); // XD attr ecrire_fichier_xyz_valeur ecrire_fichier_xyz_valeur_param ecrire_fichier_xyz_valeur 1 This keyword is used to write the values of a field only for some boundaries in a text file with the following format: n_valeur NL2 x_1 y_1 [z_1] val_1 NL2 ... NL2 x_n y_n [z_n] val_n NL2 The created files are named : pbname_fieldname_[boundaryname]_time.dat
  param.ajouter_non_std("bords",(this));
  param.ajouter("parametre_equation",&parametre_equation_); // XD attr parametre_equation parametre_equation_base parametre_equation 1 Keyword used to specify additional parameters for the equation
  param.ajouter_non_std("equation_non_resolue",(this)); // XD attr equation_non_resolue chaine equation_non_resolue 1 The equation will not be solved while condition(t) is verified if equation_non_resolue keyword is used. Exemple: The Navier-Stokes equations are not solved between time t0 and t1. NL2 Navier_Sokes_Standard NL2 { equation_non_resolue (t>t0)*(t<t1) }
}

int Equation_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="sources")
    {
      lire_sources(is);
      return 1;
    }
  else if (mot=="conditions_limites|boundary_conditions")
    {
      lire_cl(is);
      verif_Cl();
      return 1;
    }
  else if (mot=="conditions_initiales|initial_conditions")
    {
      lire_cond_init(is);
      return 1;
    }
  else if ((mot=="ecrire_fichier_xyz_valeur") || (mot=="ecrire_fichier_xyz_valeur_bin"))
    {
      if (mot=="ecrire_fichier_xyz_valeur_bin")
        ecrit_champ_xyz_bin=1;
      dt_ecrire_fic_xyz.resize_array(nombre_champ_xyz+1);
      nb_bords_post_xyz.resize(nombre_champ_xyz+1);
      nb_bords_post_xyz[nombre_champ_xyz]=0;
      Motcle nom_champ;
      is >> nom_champ;
      // il faut creer les champs associes au mot clef ecrire_fichier_xyz_valeur ICI
      // car dans methode ecrire_fichier_xyz()
      //seuls les champs definis via equation ou post-traiment sont creer
      mon_probleme.valeur().creer_champ(nom_champ);

      nom_champ_xyz.add(nom_champ);
      is >> dt_ecrire_fic_xyz[nombre_champ_xyz];
      nombre_champ_xyz++;
      return 1;
    }
  else if (mot=="bords")
    {
      int nb_bords_post;
      is >> nb_bords_post;
      nb_bords_post_xyz[nombre_champ_xyz-1] = nb_bords_post;
      if (nb_bords_post<=0)
        {
          Cerr << " ecrire_fichier_xyz_valeur - bords : the number of boundary on which you want to postprocess must be positive !! " << finl;
          exit();
        }
      Noms noms_bord;
      noms_bord.dimensionner(nb_bords_post);
      for (int i=0; i<nb_bords_post; i++)
        {
          is >> noms_bord[i];
        }
      noms_bord_xyz.add(noms_bord);
      return 1;
    }
  else if (mot=="equation_non_resolue")
    {
      // differentiate an entry with an expression from an entry with the keyword eq_non_resoue_input_
      // On complete:
      Motcle expr_equation_non_resolue;
      is >> expr_equation_non_resolue;
      equation_non_resolue_.setString(expr_equation_non_resolue);
      if (expr_equation_non_resolue=="INPUT_INT_VALUE")
        is >> eq_non_resolue_input_;
      else
        equation_non_resolue_.parseString();
      return 1;
    }
  return -1;
}

/*! @brief Ecrit dans un fichier les valeurs du champ specifie par le mot cle "ecrire_fichier_xyz_valeur"
 *
 * @return le flot d'entree modifie
 */
void Equation_base::ecrire_fichier_xyz() const
{
  for (int numero_champ_xyz=0; numero_champ_xyz<nombre_champ_xyz; numero_champ_xyz++)
    {
      Noms vide;
      const double dt_ecrire_fic = dt_ecrire_fic_xyz[numero_champ_xyz];
      const Motcle& nom_champ = nom_champ_xyz[numero_champ_xyz];
      const int nb_bords_post = nb_bords_post_xyz[numero_champ_xyz];
      const Noms& noms_bord = (nb_bords_post?noms_bord_xyz[numero_champ_xyz]:vide);

      REF(Champ_base) champ_a_ecrire;
      const double temps_courant = le_schema_en_temps->temps_courant();
      const double dt = le_schema_en_temps->pas_de_temps();
      const double tmax = le_schema_en_temps->temps_max();
      const int nb_pas_dt_max = le_schema_en_temps->nb_pas_dt_max();
      const int nb_pas_dt = le_schema_en_temps->nb_pas_dt();
      const int stationnaire_atteint = le_schema_en_temps->stationnaire_atteint();
      int ok;
      if (dt_ecrire_fic<=dt || tmax<=temps_courant || nb_pas_dt_max<=nb_pas_dt || nb_pas_dt<=1 || stationnaire_atteint || le_schema_en_temps.valeur().stop_lu())
        ok=1;
      else
        {
          // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
          double i, j, epsilon = 1.e-8;
          modf(temps_courant/dt_ecrire_fic + epsilon, &i);
          modf((temps_courant-dt)/dt_ecrire_fic + epsilon, &j);
          ok = (i>j);
        }
      int champ_ok = 0;
      int champ_stat = 0;
      REF(Operateur_Statistique_tps_base) op_stat;
      if (ok && dt_ecrire_fic>0)
        {
          // On recherche le champ dans le probleme contenant l'equation, et les postraitements
          // dans les postraitements ?
          for (auto &itr :  mon_probleme->postraitements())
            if (!champ_ok)
              if (sub_type(Postraitement, itr.valeur()))
                {
                  const Postraitement& post = ref_cast(Postraitement, itr.valeur());
                  if (champ_ok == 0)
                    {
                      Motcle nom_test = nom_champ;
                      //La recherche est faite sur les champs statistiques a partir d un identifiant
                      //Si le nom indique dans le jeux de donnes est celui d un champ statistiques
                      //il doit correspondre au nom du champ de postraitement

                      post.champ_fonc(nom_test, champ_a_ecrire, op_stat);
                      if (champ_a_ecrire.non_nul())
                        {
                          champ_stat = 1;
                          champ_ok = 1;
                        }
                    }
                }

          if (champ_ok==0)
            //L identifiant correspond ici a un Champ_base
            champ_a_ecrire = mon_probleme.valeur().get_champ(nom_champ);

          int nb_compo = champ_a_ecrire->nb_comp();
          if (nb_bords_post>0) // on ne souhaite postraiter que sur certains bords
            {
              int nb_cl = le_dom_Cl_dis->nb_cond_lim();
              for (int j=0; j<nb_bords_post; j++)
                {
                  int count = 0 ; // int servant a tester si le nom du bord correspond bien au nom d'une frontiere
                  for (int i=0; i<nb_cl; i++)
                    {
                      const Cond_lim_base& la_cl = le_dom_Cl_dis.valeur().les_conditions_limites(i);
                      const Frontiere& la_frontiere = la_cl.frontiere_dis().frontiere();
                      if (la_frontiere.le_nom() == noms_bord[j])
                        {
                          // Construction du nom du fichier
                          Nom nom_fic(probleme().le_nom());
                          nom_fic+="_";
                          nom_fic+=nom_champ;
                          nom_fic+="_";
                          nom_fic+=noms_bord[j];
                          nom_fic+="_";
                          nom_fic+=Nom(temps_courant);
                          nom_fic+=".dat";
                          EcrFicPartage fic;
                          fic.set_bin(ecrit_champ_xyz_bin);
                          fic.ouvrir(nom_fic);
                          fic.setf(ios::scientific);
                          fic.precision(format_precision_geom);

                          // Construction du tableau pos contenant les centres des faces frontiere
                          const int nb_val = la_frontiere.nb_faces();
                          DoubleTab pos;
                          la_frontiere.faces().calculer_centres_gravite(pos);

                          // Construction du tableau val contenant les valeurs aux centres des faces
                          DoubleTab val(nb_val,nb_compo);
                          val = 0.;
                          if (champ_stat == 1)
                            {
                              DoubleTab copie(champ_a_ecrire->valeurs());
                              champ_a_ecrire->valeurs() = op_stat->calculer_valeurs();
                              champ_a_ecrire->valeur_aux(pos, val);
                              champ_a_ecrire->valeurs() = copie;
                            }
                          else
                            champ_a_ecrire->valeur_aux(pos, val);

                          // Ecriture du fichier
                          int nb_val_tot = Process::mp_sum(nb_val);
                          if (Process::je_suis_maitre())
                            {
                              if(ecrit_champ_xyz_bin)
                                fic << "binary" << finl;
                              fic  << nb_val_tot << finl;
                            }
                          barrier();
                          fic.lockfile();
                          for (int i2=0; i2<nb_val; i2++)
                            {
                              // Ecriture des coordonees
                              for (int j2=0; j2<dimension; j2++)
                                fic << pos(i2,j2) << " ";
                              // Ecriture des valeurs
                              for (int nb=0; nb<nb_compo; nb++)
                                fic << val(i2,nb) << " " ;
                              fic << finl;
                            }
                          fic.unlockfile();
                          barrier();
                          fic.syncfile();
                          fic.close();
                        }
                      else
                        {
                          count++;
                        }
                    }
                  if (count==nb_cl)
                    {
                      Cerr << "You try to use the method Ecrire_fichier_xyz_valeur with an unknow name boundary" << finl;
                      Cerr << "The boundary named " << noms_bord[j] << " is not recognized"<< finl;
                      exit();
                    }

                }
            }
          else // on souhaite postraiter sur tout le domaine
            {

              Cerr << "The option of post processing the entire domain with Ecrire_fichier_xyz_valeur is now obsolete." <<finl;
              exit();
            }
        }
    }
}



/*! @brief Lecture des termes sources dans un flot d'entree.
 *
 * @param (Entree& is) flot d'entree pour lire les termes sources
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Equation_base::lire_sources(Entree& is)
{
  Cerr << "Reading of source terms" << finl ;
  is >> les_sources;
  return is;
}

/*! @brief Renvoie les termes sources asssocies a l'equation
 *
 * @return (Sources&) la liste des termes sources associees a l'equation
 */
Sources& Equation_base::sources()
{
  return les_sources;
}

/*! @brief Renvoie les termes sources asssocies a l'equation (version const)
 *
 * @return (Sources&) la liste des termes sources associees a l'equation
 */
const Sources& Equation_base::sources() const
{
  return les_sources;
}

/*! @brief Lecture des conditions initiales dans un flot d'entree.
 *
 * Le format de lecture est le suivant:
 *      {
 *       Nom [DOIT ETRE LE NOM DE L'INCONNUE]
 *       [LIRE UN CHAMP DONNE]
 *      }
 *
 * @param (Entree& is) le flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws erreur de format, accolade ouvrante attendue
 * @throws mauvais nom pour l'inconnue
 * @throws erreur de format, accolade fermante attendue
 */
Entree& Equation_base::lire_cond_init(Entree& is)
{
  Cerr << "Reading of initial conditions\n";
  Nom nom;
  Motcle motlu;
  is >> nom;
  motlu = nom;
  if(motlu!=Motcle("{"))
    {
      Cerr << "We expected a { while reading " << que_suis_je() << finl;
      Cerr << "and not : " << nom << finl;
      exit();
    }
  is >> nom;
  motlu = nom;
  if (motlu != Motcle(inconnue()->le_nom()))
    {
      Cerr << nom << " is not the name of the unknown "
           << inconnue()->le_nom() << finl;
      exit();
    }
  Champ_Don ch_init;
  is >> ch_init;
  const int nb_comp = ch_init.nb_comp();
  verifie_ch_init_nb_comp(inconnue(),nb_comp);

  //Cerr<<"inconnue().valeur().que_suis_je() = "<<inconnue().valeur().que_suis_je()<<finl;

  inconnue()->affecter(ch_init.valeur());
  is >> nom;
  motlu = nom;
  if(motlu!=Motcle("}"))
    {
      Cerr << "We expected a } while reading " << que_suis_je() << finl;
      Cerr << "and not : " << nom << finl;
      exit();
    }
  return is;
}

/*! @brief Lecture des conditions limites sur un flot d'entree.
 *
 * voir Domaine_Cl_dis_base::readOn
 *
 * @param (Entree& is) le flot d'entree
 * @return (Entree&) le flot d'entree modifie
 * @throws le domaine des conditions aux limites discretisee est vide
 */
Entree& Equation_base::lire_cl(Entree& is)
{
  Cerr << "Reading of boundaries conditions\n";
  if(!le_dom_Cl_dis.non_nul())
    {
      Cerr << "Error while reading boundaries conditions : " <<
           que_suis_je() << finl;
      Cerr << "The Domaine_Cl_dis is empty ..." << finl;
      exit();
    }
  is >> le_dom_Cl_dis.valeur();
  return is;
}

/*! @brief On sauvegarde l'inconnue de l'equation sur un flot de sortie.
 *
 * @param (Sortie& os)
 * @return (int) le code de retour de Champ_Inc::sauvegarder()
 */
int Equation_base::sauvegarder(Sortie& os) const
{
  return inconnue().sauvegarder(os);
}

/*! @brief On reprend l'inconnue a partir d'un flot d'entree.
 *
 * [ON CHERCHE L'INCONNUE PAR SON NOM]
 *      [ON LIT L'INCONNUE]
 *      Voir Champ_Inc::reprendre(Entree&)
 *
 * @param (Entree& fich) le flot d'entree (fichier) a lire
 * @return (int) renvoie toujours 1
 * @throws erreur de reprise, fin de fichier atteinte sans trouver l'inconnue
 */
int Equation_base::reprendre(Entree& fich)
{
  double temps = schema_temps().temps_courant();
  Nom field_tag(inconnue().le_nom());
  field_tag += inconnue().valeur().que_suis_je();
  field_tag += probleme().domaine().le_nom();
  field_tag += Nom(temps,probleme().reprise_format_temps());
  avancer_fichier(fich, field_tag);
  inconnue().reprendre(fich);
  return 1;
}

/*! @brief Demande au schema en temps si il faut effectuer une impression.
 *
 * Renvoie 1 si il faut effectuer une impression.
 *      Appel simple a Schema_Temps_base::limpr()
 *
 * @return (int) 1 si il faut effectuer une impression, 0 sinon.
 */
int Equation_base::limpr() const
{
  return le_schema_en_temps->limpr();
}

/*! @brief Imprime les operateurs de l'equation sur un flot de sortie, de facon inconditionnelle.
 *
 *     appelle Operateur_base::impr(os)
 *
 * @param (Sortie& os) le flot de sortie
 * @return (int) renvoie toujours 1
 */
int Equation_base::impr(Sortie& os) const
{
  for(int i=0; i<nombre_d_operateurs(); i++)
    operateur(i).impr(os);
  //if (je_suis_maitre() && les_sources.size()>0) os << finl << "Impression des termes sources pour l'equation " << que_suis_je() << " : " << finl;
  les_sources.impr(os);
  return 1;
}

/*! @brief Imprime les operateurs de l'equation si le schema en temps indique que c'est necessaire.
 *
 *      [SI limpr() ALORS impr(os)]
 *
 * @param (Sortie& os) le flot de sortie
 */
void Equation_base::imprimer(Sortie& os) const
{
  ecrire_fichier_xyz();
  if (limpr() )
    impr(os);
}

/*! @brief Returns the time derivative of the unknown I of the equation: dI/dt = M-1*(sum(operators(I) + sources))
 *
 *  In : derivee contains I (but immediatly set to 0)
 *  Out: derivee contains dI/dt
 *
 */
DoubleTab& Equation_base::derivee_en_temps_inco(DoubleTab& derivee)
{
  derivee = 0.;
  DoubleTrav secmem(derivee);
  // secmem = sum(operators) + sources + equation specific terms
  const double time_factor = get_time_factor();

  bool calcul_explicite = false;
  if (parametre_equation_.non_nul() && sub_type(Parametre_implicite, parametre_equation_.valeur()))
    {
      Parametre_implicite& param2 = ref_cast(Parametre_implicite, parametre_equation_.valeur());
      calcul_explicite = param2.calcul_explicite();
    }

  if (schema_temps().diffusion_implicite() && !calcul_explicite)
    {
      // Add convection operator only if equation has one
      derivee=inconnue().valeurs();
      if (nombre_d_operateurs()>1)
        {
          derivee_en_temps_conv(secmem, derivee);
          if (has_time_factor_)
            {
              secmem *= time_factor;
            }
        }
    }
  else
    {
      // Add all explicit operators
      for(int i=0; i<nombre_d_operateurs(); i++)
        if(operateur(i).l_op_base().get_decal_temps()!=1)
          {
            if (has_time_factor_)
              {
                DoubleTrav secmem_tmp(secmem);
                operateur(i).ajouter(secmem_tmp);
                if (i == 1) secmem_tmp *= time_factor;
                secmem += secmem_tmp;
              }
            else
              {
                operateur(i).ajouter(secmem);
              }
          }
    }
  les_sources.ajouter(secmem);
  if (calculate_time_derivative())
    {
      // Store dI/dt(n) = M-1 secmem :
      derivee_en_temps().valeurs()=secmem;
      solveur_masse.appliquer(derivee_en_temps().valeurs());
      schema_temps().modifier_second_membre((*this),secmem); // Change secmem for some schemes (eg: Adams_Bashforth)
    }

  corriger_derivee_expl(secmem); // Add specific term for an equation (eg: -gradP for Navier Stokes)

  if (implicite_==0)
    {
      solveur_masse.appliquer(secmem); // M-1 * secmem
      if (schema_temps().diffusion_implicite() && !calcul_explicite)
        {
          // Solve: (1/dt + M-1*A)*dI = M-1 * secmem
          // where A is the diffusion
          Equation_base::Gradient_conjugue_diff_impl(secmem, derivee);
        }
      else
        {
          derivee = secmem;
          derivee.echange_espace_virtuel();
        }
      corriger_derivee_impl(derivee);  // Solve specific implicit term for an equation (eg: pressure for Navier Stokes)

    }
  else if (implicite_>0)
    {
      // TRUST support notices that this part has never been covered...
      //implicite
      // M dU/dt + AU* = f -BUn;
      // U* = Un+dt dU/dt
      // (M/dt + A) U* = f -BUn + M/dt Un
      //
      double dt=schema_temps().pas_de_temps();
      for(int i=0; i<nombre_d_operateurs(); i++)
        {
          //boucle sur les operateurs
          Operateur_base& op=operateur(i).l_op_base();
          if(op.get_matrice().est_nul())
            op.set_matrice().typer("Matrice_Morse");
          if(op.get_decal_temps()==1)
            {
              //if (op.set_matrice().valeur().nb_lignes()<2)
              {
                Matrice_Morse& matrice=ref_cast(Matrice_Morse,op.set_matrice().valeur());
                op.dimensionner(matrice);
                sys_invariant_=0;
              }
              if(!sys_invariant_)
                {
                  Matrice_Morse& matrice=ref_cast(Matrice_Morse, op.set_matrice().valeur());
                  op.contribuer_a_avec(inconnue().valeurs(), matrice);
                  solv_masse().ajouter_masse(dt, op.set_matrice().valeur());

                  if(
                    (op.get_solveur()->que_suis_je()=="Solv_Cholesky")
                    ||
                    (op.get_solveur()->que_suis_je()=="Solv_GCP")
                  )
                    {
                      Matrice_Morse_Sym new_mat(matrice);
                      new_mat.set_est_definie(1);
                      op.set_matrice()=new_mat;
                      ref_cast_non_const(SolveurSys,op.get_solveur())->reinit();
                    }
                }
            }
        }
      if(implicite_==1)
        {
          // Un seul operateur implicite.
          // On suppose que c'est le premier (la diffusion !!)
          Operateur_base& op=operateur(0).l_op_base();
          Matrice_Base& matrice=op.set_matrice().valeur();
          // DoubleTrav secmem(derivee);
          secmem=derivee;
          solv_masse().ajouter_masse(dt, secmem, inconnue().valeurs());
          op.contribuer_au_second_membre(secmem );
          op.set_solveur().resoudre_systeme(matrice,
                                            secmem,
                                            derivee
                                           );
          solv_masse().corriger_solution(derivee,inconnue().valeurs());

          derivee-=inconnue().valeurs();
          derivee/=dt;

          //Sert uniquement a calculer les flux sur les bords quand la diffusion est implicitee !
          DoubleTab resu;
          resu=derivee;
          operateur(0).calculer(inconnue().valeurs(), resu);
        }
      else
        {
          // plusieurs operateurs implicites ...
          Cerr << "Must be coded ... " << finl;
          exit();
        }
    }
  else
    {
      Cerr << "Error in Equation_base::derivee_en_temps_inco" << finl;
      Cerr << "implicite_ = " << implicite_ << " has not been initialized!" << finl;
      Cerr << "May be " << que_suis_je() << "::completer() method doesn't call Equation_base::completer()" << finl;
      Cerr << "Contact TRUST support." << finl;
      Process::exit();
    }
  return derivee;
}

/*! @brief Renvoie le probleme associe a l'equation.
 *
 * @return (Probleme_base&) le probleme associe a l'equation
 */
Probleme_base& Equation_base::probleme()
{
  return mon_probleme.valeur();
}

/*! @brief Renvoie le probleme associe a l'equation.
 *
 * (version const)
 *
 * @return (Probleme_base&) le probleme associe a l'equation
 */
const Probleme_base& Equation_base::probleme() const
{
  return mon_probleme.valeur();
}

/*! @brief Methode appelee lorsqu'on cree l'instance de l'objet dans le jeu de donnees (Interprete::ajouter)
 *
 */
void Equation_base::nommer(const Nom& un_nom)
{
  nom_ = un_nom;
}

/*! @brief S'associe au Probleme passe en parametre.
 *
 * Associe egalement les sources, les operateurs et le solveur
 *     de masse a l'equation.
 *
 * @param (Probleme_base& pb) le probleme auquel l'equation doit s'associer
 */
void Equation_base::associer_pb_base(const Probleme_base& pb)
{
  mon_probleme=pb;
  // Modif B. Mathieu : pour le front_tracking, le nom de l'equation
  // est donne dans le jeu de donnees lors de l'instanciation de l'objet
  // (la methode "nommer" est appelee a ce moment).
  // Si l'equation n'est pas instanciee, le nom est vide, on donne
  // le nom par defaut qui est defini ci-dessous :
  Nom nom_vide;
  if (nom_ == nom_vide)
    {
      nom_ = pb.le_nom();
      nom_ += que_suis_je();
    }
  // fin modif B.M.
  les_sources.associer_eqn(*this);
  int nb_op = nombre_d_operateurs();
  for(int i=0; i<nb_op; i++)
    operateur(i).associer_eqn(*this);
  solveur_masse.associer_eqn(*this);
}

/*! @brief Discretise l'equation.
 *
 * Type le domaine_Cl_dis, la formatte, l'associe a l'equation.
 *     Type le solveur masse, lui associe le domaine discretisee et
 *     le domaine des conditions aux limites discretisees.
 *
 */
void Equation_base::discretiser()
{
  discretisation().domaine_Cl_dis(domaine_dis(), le_dom_Cl_dis);
  le_dom_Cl_dis->associer_eqn(*this);
  le_dom_Cl_dis->associer_inconnue(inconnue());

  solveur_masse.typer();
  solveur_masse->associer_domaine_dis_base(domaine_dis().valeur());
  solveur_masse->associer_domaine_cl_dis_base(le_dom_Cl_dis.valeur());

  if (calculate_time_derivative())
    {
      Motcle directive("temperature");
      if (inconnue().valeur().nature_du_champ()==vectoriel)
        directive="vitesse";

      Nom nom("derivee_en_temps_");
      nom += inconnue().valeur().le_nom();

      Nom unite(inconnue().valeur().unites()[0]);
      unite += "/s";

      discretisation().discretiser_champ(directive,domaine_dis().valeur(),nom,unite,
                                         inconnue().valeur().nb_comp(),
                                         schema_temps().nb_valeurs_temporelles(),
                                         schema_temps().temps_courant(),derivee_en_temps());

      champs_compris_.ajoute_champ(derivee_en_temps());
    }
}
void Equation_base::associer_milieu_equation()
{
  milieu().associer_equation(this);
}

/*! @brief S'associe au schema_en_temps.
 *
 * @param (Schema_Temps_base& un_schema_en_temps) le schema en temps a associer a l'equation
 */
void Equation_base::associer_sch_tps_base(const Schema_Temps_base& un_schema_en_temps)
{
  le_schema_en_temps=un_schema_en_temps;
}

/*! @brief Renvoie le schema en temps associe a l'equation.
 *
 * @return (Schema_Temps_base&) le schema en temps associe a l'equation
 * @throws pas de schema en temps associe a l'equation
 */
Schema_Temps_base& Equation_base::schema_temps()
{
  if(!le_schema_en_temps.non_nul())
    {
      Cerr << "Error : " << que_suis_je()
           << "has not been associated to a time scheme " << finl;
      exit();
    }
  return le_schema_en_temps.valeur();
}

/*! @brief Renvoie le schema en temps associe a l'equation.
 *
 * @return (Schema_Temps_base&) le schema en temps associe a l'equation
 * @throws pas de schema en temps associe a l'equation
 */
const Schema_Temps_base& Equation_base::schema_temps() const
{
  if(!le_schema_en_temps.non_nul())
    {
      Cerr << "Error : " << que_suis_je()
           << "has not been associated to a time scheme " << finl;
      exit();
    }
  return le_schema_en_temps.valeur();
}

/*! @brief Associe le domaine discretisee a l'equation.
 *
 * @param (Domaine_dis& z) le domaine discretisee a associee
 */
void Equation_base::associer_domaine_dis(const Domaine_dis& z)
{
  le_dom_dis=z;
}

/*! @brief La valeur de l'inconnue sur le pas de temps a ete calculee.
 *
 * Cette methode avance le present jusqu'au temps passe en parametre.
 *    Elle met aussi a jour les proprietes du milieu.
 *
 * @param (double temps) le pas de temps de mise a jour
 */
void Equation_base::mettre_a_jour(double temps)
{
  // On tourne la roue de l'inconnue
  // Update the unknown:
  inconnue().mettre_a_jour(temps);
  if (calculate_time_derivative()) derivee_en_temps().mettre_a_jour(temps);

  les_sources.mettre_a_jour(temps);

  // On tourne la roue des CLs
  // Update the boundary condition:
  if (temps > schema_temps().temps_courant()) domaine_Cl_dis()->avancer(temps);
}

//mise a jour de champ_conserve / champ_convecte : appele par Probleme_base::mettre_a_jour() apres avoir mis a jour le milieu
void Equation_base::mettre_a_jour_champs_conserves(double temps, int reset)
{
  if (reset && champ_conserve_.non_nul()) champ_conserve_->reset_champ_calcule(); //force le calcul de toutes les cases
  if (reset && champ_convecte_.non_nul()) champ_convecte_->reset_champ_calcule();
  if (champ_conserve_.non_nul()) champ_conserve_->mettre_a_jour(temps);
  if (champ_convecte_.non_nul()) champ_convecte_->mettre_a_jour(temps);
}

/*! @brief Reinitialiser ce qui doit l'etre.
 *
 * Cette methode est appelee lorsqu'un pas de temps est abandonne,
 *     par exemple parce que le calcul a diverge.
 *     Il faut donc nettoyer ce qui pourrait polluer la nouvelle resolution.
 *
 * @throws WrongContext
 */
void Equation_base::abortTimeStep()
{
  for (int i=0; i<nombre_d_operateurs(); i++)
    operateur(i).l_op_base().abortTimeStep();
  inconnue()->abortTimeStep();
  if (champ_conserve_.non_nul()) champ_conserve_->abortTimeStep();
  if (champ_convecte_.non_nul()) champ_convecte_->abortTimeStep();
}

/*! @brief methode virtuelle permettant de corriger l'onconnue lors d'iterations implicites par exemple K-eps doivent rester positifs
 *
 *  les fractions massqiues entre 0 et 1
 *
 */
void Equation_base::valider_iteration()
{
}

/*! @brief Tout ce qui ne depend pas des autres problemes eventuels.
 *
 * @return (int) renvoie toujours 1
 */
int Equation_base::preparer_calcul()
{
  solveur_masse.valeur().preparer_calcul();
  int nb_op=nombre_d_operateurs();
  for(int i=0; i<nb_op; i++)
    {
      operateur(i).l_op_base().preparer_calcul();
      dt_op_bak.add(0.);
    }
  initialise_residu();
  double temps=schema_temps().temps_courant();
  inconnue()->verifie_valeurs_cl();
  inconnue().changer_temps(temps);
  if (calculate_time_derivative()) derivee_en_temps().changer_temps(temps);
  le_dom_Cl_dis->initialiser(temps);

  Nom msg=que_suis_je();
  msg+=" dans Equation_base::preparer_calcul 1";
  Debog::verifier(msg ,inconnue());

  les_sources.initialiser(temps);
  domaine_Cl_dis().imposer_cond_lim(inconnue(),temps);
  inconnue().valeurs().echange_espace_virtuel();

  /* initialisation de parametre_equation() par le schema en temps si celui-ci le permet */
  if (sub_type(Schema_Implicite_base, schema_temps()))
    ref_cast(Schema_Implicite_base, schema_temps()).solveur()->get_and_set_parametre_equation(*this);

  msg=que_suis_je();
  msg+=" dans Equation_base::preparer_calcul ";
  Debog::verifier(msg ,inconnue());

  return 1;
}


/*! @brief Allocation et initialisation de l'inconnue et des CLs jusqu'a present+dt.
 *
 *   + autres initialisations pour les calculs sur le prochain
 *     pas de temps : operateurs, solveur_masse.
 *
 * @return (0 en cas d'erreur, 1 sinon)
 */
bool Equation_base::initTimeStep(double dt)
{
  Schema_Temps_base& sch=schema_temps();
  double temps_present=sch.temps_courant();

  // Le pas de temps doit etre celui du schema !!
  assert(sch.pas_de_temps()==dt);

  // Pour chaque temps futur
  for (int i=1; i<=sch.nb_valeurs_futures(); i++)
    {
      double tps=sch.temps_futur(i);
      // Mise a jour du temps dans l'inconnue
      inconnue()->changer_temps_futur(tps,i);
      if (champ_conserve_.non_nul()) champ_conserve_->changer_temps_futur(tps,i);
      if (champ_convecte_.non_nul()) champ_convecte_->changer_temps_futur(tps,i);
      if (calculate_time_derivative()) derivee_en_temps()->changer_temps_futur(tps,i);

      inconnue()->futur(i)=inconnue()->valeurs();
      if (champ_conserve_.non_nul()) champ_conserve_->futur(i) = champ_conserve().valeurs();
      if (champ_convecte_.non_nul()) champ_convecte_->futur(i) = champ_convecte().valeurs();
      if (calculate_time_derivative()) derivee_en_temps()->futur(i)=derivee_en_temps()->valeurs();

      // Mise a jour du temps dans les CL
      domaine_Cl_dis()->changer_temps_futur(tps,i);
    }

  // Mise a jour du temps par defaut des CLs
  domaine_Cl_dis()->set_temps_defaut(sch.temps_defaut());

  // Mise a jour des operateurs
  for(int i=0; i<nombre_d_operateurs(); i++)
    operateur(i).mettre_a_jour(temps_present);

  // Mise a jour du solveur masse au temps present
  if (solveur_masse.non_nul())
    solveur_masse.mettre_a_jour(temps_present);

  return true;
}

bool Equation_base::updateGivenFields()
{
  Schema_Temps_base& sch=schema_temps();
  double temps_present=sch.temps_courant();
  double temps_futur=temps_present+sch.pas_de_temps();

  // Pour chaque temps futur
  for (int i=1; i<=sch.nb_valeurs_futures(); i++)
    {
      double tps=sch.temps_futur(i);
      // Calcul des CLs a ce temps.
      domaine_Cl_dis().mettre_a_jour(tps);
    }
  // Calcul du taux d'accroissement des CLs entre les temps present et futur.
  domaine_Cl_dis()->Gpoint(temps_present,temps_futur);

  //MaJ des operateurs
  for (int i = 0; i < nombre_d_operateurs(); i++)
    if (operateur(i).op_non_nul())
      operateur(i).l_op_base().mettre_a_jour(temps_present);

  // Mise a jour des sources au temps present
  les_sources.mettre_a_jour(temps_present);

  return true;
}


/*! @brief Renvoie la discretisation associee a l'equation.
 *
 * @return (Discretisation_base&) a discretisation associee a l'equation
 * @throws pas de probleme associe
 */
const Discretisation_base& Equation_base::discretisation() const
{
  if(!mon_probleme.non_nul())
    {
      Cerr << "Error : " << que_suis_je()
           << "has not been associated to a problem ! " << finl;
      exit();
    }
  return mon_probleme->discretisation();
}

void Equation_base::creer_champ(const Motcle& motlu)
{
  if (motlu == "volume_maille")
    {
      if (!volume_maille.non_nul())
        {
          discretisation().volume_maille(schema_temps(),domaine_dis(),volume_maille);
          champs_compris_.ajoute_champ(volume_maille);
        }
    }
// pour recuperer une equation const !!!!!

  const Equation_base& me_const =(*this);
  const Nom& nom_inco=me_const.inconnue()->le_nom();
  Nom inco(nom_inco);
  inco += "_residu";
  if (motlu == Motcle(inco))
    {
      if (!field_residu_.non_nul())
        {
          discretisation().residu(domaine_dis(),inconnue(),field_residu_);
          champs_compris_.ajoute_champ(field_residu_);
        }
    }

  int nb_op = nombre_d_operateurs();
  for (int i=0; i<nb_op; i++)
    {
      if (operateur(i).op_non_nul())
        operateur(i).l_op_base().creer_champ(motlu);
    }

  for (auto& itr : les_sources)
    if (itr.non_nul())
      itr->creer_champ(motlu);
}

const Champ_base& Equation_base::get_champ(const Motcle& nom) const
{
  Nom inco_residu (inconnue()->le_nom());
  inco_residu += "_residu";
  if (nom=="volume_maille")
    {
      Champ_Fonc_base& ch_vol_maille=ref_cast_non_const(Champ_Fonc_base,volume_maille.valeur());
      if (est_different(ch_vol_maille.temps(),inconnue()->temps()))
        ch_vol_maille.mettre_a_jour(inconnue()->temps());
    }
  else if(nom == Motcle(inco_residu))
    {
      Champ_Fonc_base& ch=ref_cast_non_const(Champ_Fonc_base,field_residu_.valeur());
      double temps_init = schema_temps().temps_init();
      if (((ch.temps()!=inconnue()->temps()) || (ch.temps()==temps_init)) && (inconnue()->mon_equation_non_nul()))
        {
          ch.mettre_a_jour(inconnue()->temps());
        }
    }
  else
    {
      for (const auto& itr : list_champ_combi)
        {
          const Champ_Fonc& ch = itr;
          if (ch.le_nom()==nom && ch.temps()!=inconnue()->temps())
            ref_cast_non_const( Champ_Fonc,ch).mettre_a_jour(inconnue()->temps());
        }
    }
  try
    {
      return champs_compris_.get_champ(nom);
    }
  catch (Champs_compris_erreur&)
    {

    }

  try
    {
      return milieu().get_champ(nom);
    }
  catch (Champs_compris_erreur&)
    {
    }
  int nb_op = nombre_d_operateurs();
  for (int i=0; i<nb_op; i++)
    {
      if (operateur(i).op_non_nul())
        try
          {
            return operateur(i).l_op_base().get_champ(nom);
          }
        catch (Champs_compris_erreur&)
          {

          }
    }

  for (const auto& itr : les_sources)
    {
      if (itr.non_nul())
        try
          {
            return itr->get_champ(nom);
          }
        catch (Champs_compris_erreur&)
          {

          }
    }
  throw Champs_compris_erreur();
}

void Equation_base::get_noms_champs_postraitables(Noms& noms,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<que_suis_je()<<" : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    noms.add(champs_compris_.liste_noms_compris());
  milieu().get_noms_champs_postraitables(noms,opt);
  int nb_op = nombre_d_operateurs();
  for (int i=0; i<nb_op; i++)
    {
      if (operateur(i).op_non_nul())
        operateur(i).l_op_base().get_noms_champs_postraitables(noms,opt);
    }

  for (const auto& itr : les_sources)
    if (itr.non_nul())
      itr->get_noms_champs_postraitables(noms,opt);
}


/*! @brief Calcul du prochain pas de temps.
 *
 * Renvoie l'inverse de la somme des inverses des
 *     pas de temps calcules par les operateurs.
 *     Ces pas de temps sont ceux pour le schema d'Euler explicite.
 *     Le pas de temps n'est pas majore par dt_max, ceci est fait dans corriger_dt_calcule
 *
 * @return (double) inverse de la somme des inverses des pas de temps calcules par les operateurs
 */
double Equation_base::calculer_pas_de_temps() const
{
  bool harmonic_calculation = true;
  double dt_op;
  double dt;
  if (harmonic_calculation)
    dt = 0;
  else
    dt = DMAXFLOAT;

  int nb_op = nombre_d_operateurs();
  for(int i=0; i<nb_op; i++)
    {
      if(operateur(i).l_op_base().get_decal_temps()!=1)
        dt_op = operateur(i).calculer_pas_de_temps();
      else
        dt_op = DMAXFLOAT;

      Debog::verifier("Equation_base::calculer_pas_de_temps dt_op 0 ",dt_op);

      Debog::verifier("Equation_base::calculer_pas_de_temps dt ",dt);
      const Operateur_base& op=operateur(i).l_op_base();
      if (sub_type(Operateur_Diff_base,op) && le_schema_en_temps->diffusion_implicite())
        {
          // On ne compte pas le pas de temps de diffusion lorsque la diffusion est implicitee
        }
      else if (dt_op>0)
        {
          // Une demie-moyenne harmonique est justifiee par le fait que diffusion et convection sont deux phenonomenes qui se cumulent
          // L'information a chaque pas de temps ne peut traverser plus d'une maille de calcul donc dt*U + dt*vitesse_diffusion(~alpha/dx) < dx
          // donc dt < dx/(U+alpha/dx) = 1/(1/(dx/U)+1/(dx^2/alpha)) : c'est bien une demie moyenne harmonique...
          if (harmonic_calculation)
            dt = dt + 1./dt_op;
          else
            // https://en.wikipedia.org/wiki/Numerical_solution_of_the_convection%E2%80%93diffusion_equation
            // On pourrait prendre le min de tous les operateurs. Mais attention parfois peut diverger:
            // Un cas typique (rare?) Convection.data ecoulement en travers par rapport au maillage VDF et Re(maille)~1
            dt = (dt_op < dt ? dt_op : dt);
        }

      Debog::verifier("Equation_base::calculer_pas_de_temps dt_op 1 ",dt_op);
      if (le_schema_en_temps->limpr())
        {
          if (i == 0)
            {
              Cout << " " << finl;
              Cout << "Printing of the next provisional time steps for the equation: " << que_suis_je() << finl;
            }
          if (sub_type(Operateur_Conv_base,op))
            Cout << "   convective";
          else if (sub_type(Operateur_Diff_base,op))
            Cout << "   diffusive";
          else
            Cout << "   operator ";
          Cout<<" time step : "<< dt_op_bak[i] << finl;
        }
      dt_op_bak[i]=dt_op;
    }
  if (harmonic_calculation)
    {
      if (dt==0.)
        dt = DMAXFLOAT;
      else
        dt = 1./dt;
    }
  // Warning sur les premiers pas de temps si l'implicitation de la diffusion
  // dans un schema explicite vaut le coup (dt_diff<<min(dt_max,dt_conv))
  // On fait le test apres les 10 premiers pas de temps en cas de demarrage avec vitesse nulle
  int nw=100;
  if (le_schema_en_temps->nb_pas_dt()>10 && le_schema_en_temps->nb_pas_dt()<nw && le_schema_en_temps->limpr())
    if (nb_op>1 && dt_op_bak[0]<0.01*std::min(le_schema_en_temps->pas_temps_max(),dt_op_bak[1]) && sub_type(Schema_Euler_explicite,le_schema_en_temps.valeur()) && !le_schema_en_temps->diffusion_implicite())
      {
        Cerr << finl << "**** Advice (printed only on the first " << nw << " time steps) ****" << finl;
        Cerr << "You could use diffusion_implicite option into the Euler scheme to increase the stability time step of the " << this->que_suis_je() << " equation by impliciting the diffusive operator." << finl;
      }
  Debog::verifier("Equation_base::calculer_pas_de_temps dt ",dt);
  return dt;
}

// Calculation of local time: Vect of size number of faces of the domain
// Calculate whether the "steady" option is used in the "Euler implicit"
// The local time step is a convection temp step
void Equation_base::calculer_pas_de_temps_locaux(DoubleTab& dt_op) const
{
  int nb_op=nombre_d_operateurs();
  for(int i=0; i<nb_op; i++)
    {
      const Operateur_base& op=operateur(i).l_op_base();
      if (sub_type(Operateur_Conv_base,op))
        {
          if(sub_type(Op_Conv_negligeable, op))
            {
              Cerr<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<finl;
              Cerr<<"STEADY option is not compatible with the 'CONVECTION { NEGLIGEABLE }' model!"<<finl;
              Cerr << "Please, contact TRUST support." << finl;
              Cerr<<"!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<finl;
              exit();
            }
          else
            operateur(i).calculer_pas_de_temps_locaux(dt_op);

        }
    }

}

/*! @brief Verifie la compatibilite des conditions limites avec l'equation.
 *
 * voir Conds_lim::compatible_avec_eqn().
 *
 * @return (int)
 */
int Equation_base::verif_Cl() const
{
  const Conds_lim& les_cl=le_dom_Cl_dis->les_conditions_limites();
  les_cl.compatible_avec_eqn(*this);
  return les_cl.compatible_avec_discr(discretisation());
}

/*! @brief Renvoie "indetermine" Navier_Stokes_standard par exemple surcharge cette methode
 *
 *     pour renvoyer "Hydraulique"
 *
 * @return (Motcle&) le nom du domaine d'aplication
 */
const Motcle& Equation_base::domaine_application() const
{
  static Motcle defaut="indetermine";
  return defaut;
}

/*! @brief Verification du nombre de composantes lues pour la specification d un champ.
 *
 * Actuellement utilise pour la lecture d un condition initiale ou limite.
 *
 * @param (ch_ref  : un champ inconnu de l equation consideree)
 */
void Equation_base::verifie_ch_init_nb_comp(const Champ_Inc_base& ch_ref, const int nb_comp) const
{
  const Nature_du_champ nature = ch_ref.nature_du_champ();
  const int nb_composantes = ch_ref.nb_comp();
  const Nom& nom = ch_ref.le_nom();

  if (nature==scalaire)
    {
      if (nb_comp!=1)
        {
          Cerr<<"The nature of the field "<<nom<<" unknown to the equation name "<<le_nom()<<" is scalar."<<finl;
          Cerr<<"The number of components readed for this field "<<nb_comp<<" is not compatible with its nature."<<finl;
          Cerr<<"We must read only one component for this field."<<finl;
          exit();
        }
    }
  else if (nature==vectoriel)
    {
      if (nb_comp!=Objet_U::dimension)
        {
          Cerr<<"The nature of the field "<<nom<<" unknown to the equation name "<<le_nom()<<" is vector."<<finl;
          Cerr<<"The number of components readed for this field "<<nb_comp<<" is not compatible with its nature."<<finl;
          Cerr<<"It should read "<<Objet_U::dimension<<" components for this field."<<finl;
          exit();
        }
    }
  else if (nature==multi_scalaire)
    {
      if (nb_comp!=nb_composantes)
        {
          Cerr<<"The nature of the field "<<nom<<" unknown to the equation name "<<le_nom()<<" is multiscalar."<<finl;
          Cerr<<"The number of components readed for this field "<<nb_comp<<" does not match the number expected."<<finl;
          Cerr<<"It should read "<<nb_composantes<<" components for this field."<<finl;
          exit();
        }
    }
  else
    {
      Cerr<<"The nature of the field is not recognized"<<finl;
      exit();
    }
}

/*! @brief Add convection term In:  solution is the unknown I
 *
 *  Out: secmem is increased by convection(I)
 *
 */
DoubleTab& Equation_base::derivee_en_temps_conv(DoubleTab& secmem, const DoubleTab& solution)
{
  double dt = le_schema_en_temps->pas_de_temps();
  double dt_convection;
  if (le_schema_en_temps->no_conv_subiteration_diffusion_implicite())
    {
      // faux mais permet de ne pas sous iterer sur la convection
      dt_convection=1e38;
    }
  else
    dt_convection= operateur(1).calculer_pas_de_temps();
  if(inf_strict(dt_convection,dt))
    {
      // Several steps (nstep) are done if the convective time step
      // is smaller than the time step (dt) :
      double epsilon = 1.e-10;
      int nstep = (int)((dt / dt_convection)*(1+epsilon)) + 1 ;
      if (( nstep % 2 ) != 0 ) nstep += 1 ;
      double dt_loc = dt/double(nstep) ;
      Cout << "Convection Semi Implicite: Number of Sub-Cycles : " << nstep << " with dt_loc = " << dt_loc << " dt =" << dt << finl ;

      secmem = 0;
      DoubleTab solution_loc(solution) ;
      DoubleTab derivee;
      derivee.copy(secmem, Array_base::NOCOPY_NOINIT);
      for (int i=0; i<nstep; i++)
        {
          derivee = 0. ;
          operateur(1).ajouter(solution_loc, derivee); // derivee=conv(solution_loc)
          derivee.echange_espace_virtuel();
          secmem += derivee ;

          // Si ce n'est pas la derniere iteration:
          if (i+1 < nstep)
            {
              solveur_masse.appliquer(derivee);
              solution_loc.ajoute(dt_loc, derivee, VECT_REAL_ITEMS);
              solution_loc.echange_espace_virtuel();
            }
        }
      secmem /= double(nstep) ;
      return secmem;
    }
  else
    {
      operateur(1).ajouter(solution, secmem); // secmem+=conv(solution)
      return secmem;
    }
}

/*! @brief Solve: (1/dt + M-1*L)*dI = M-1 * secmem with a Conjugate Gradient matrix-free algorithm by default
 *
 *  L is the diffusion
 *  M is the mass
 *  In : solution=I(n)
 *  Out: solution=dI/dt
 *
 */
void Equation_base::Gradient_conjugue_diff_impl(DoubleTrav& secmem, DoubleTab& solution, int size_terme_mul, const DoubleTab& terme_mul)
{
  if (le_schema_en_temps->impr_diffusion_implicite())
    Cout << "Implicited diffusion algorithm applied on " << que_suis_je() << " equation:" << finl;
  int marq_tot = 0;
  int size_s = sources().size();
  ArrOfInt marq(size_s);
  for (int i = 0; i < size_s; i++)
    {
      if (sub_type(Source_dep_inco_base, sources()(i).valeur()))
        {
          marq[i] = 1;
          marq_tot = 1;
        }
    }
  bool has_diffusion_implicit_solver = parametre_equation().non_nul() &&
                                       sub_type(Parametre_diffusion_implicite, parametre_equation().valeur()) &&
                                       ref_cast(Parametre_diffusion_implicite, parametre_equation().valeur()).solveur().non_nul();
  if (has_diffusion_implicit_solver)
    {
      // PL: Solve (M/dt + L)*dI = Secmem(sans diffusion) with a matrix build to use more solvers
      if (marq_tot)
        {
          Cerr << "solveur_dffusion_implicite can't be used yet with source terms depending from the unknowns. " << finl;
          Process::exit();
        }
      if (size_terme_mul)
        {
          Cerr << "solveur_dffusion_implicite can't be used yet with penalization. " << finl;
          Process::exit();
        }
      statistiques().begin_count(assemblage_sys_counter_);
      DoubleTab present(solution); // I(n)
      // On multiplie secmem par M (qui etait divise par M avant l'appel...)
      DoubleTab copie(secmem);
      secmem = 0;
      solveur_masse.ajouter_masse(1, secmem, copie);
      // Build matrix A:
      Matrice_Morse& matrice = ref_cast(Parametre_diffusion_implicite, parametre_equation().valeur()).matrice();
      if (matrice.ordre()==0) dimensionner_matrice(matrice);
      matrice.clean(); // A=0
      // Add diffusion matrix L into matrix
      operateur(0).l_op_base().contribuer_a_avec(present, matrice); // A=L
      statistiques().end_count(assemblage_sys_counter_);
      operateur(0).ajouter(present, secmem);
      statistiques().begin_count(assemblage_sys_counter_);
      matrice.ajouter_multvect(present, secmem);
      // Add M/dt into matrix
      schema_temps().ajouter_inertie(matrice,secmem,(*this)); // A=M/dt+L
      modifier_pour_Cl(matrice,secmem);
      statistiques().end_count(assemblage_sys_counter_,0,0);
      // Solve to get I(n+1):
      SolveurSys& solveur = ref_cast(Parametre_diffusion_implicite, parametre_equation().valeur()).solveur();
      solveur->reinit(); // New matrix but same sparsity
      int niter = solveur.resoudre_systeme(matrice, secmem, solution);
      Cout << "Diffusion operator implicited for the equation " << que_suis_je()
           << " : Conjugate gradient converged in " << niter << " iterations." << finl;
      // CHD 230501 : Call to diffusive operator to update flux_bords (boundary fluxes):
      operateur(0).ajouter(inconnue(), secmem);

      solution-=present; // dI=I(n+1)-I(n)
      solution/=schema_temps().pas_de_temps(); // dI/dt

    }
  else
    {
      statistiques().begin_count(diffusion_implicite_counter_);
      // on retire les sources dependantes de l inco ; on les rajoutera apres
      if (marq_tot)
        {
          DoubleTrav toto(secmem);
          statistiques().end_count(diffusion_implicite_counter_,0,0);
          for (int i = 0; i < size_s; i++)
            if (marq[i])
              sources()(i).ajouter(toto);
          statistiques().begin_count(diffusion_implicite_counter_);
          solv_masse().appliquer(toto);
          secmem.ajoute(-1., toto); // ,VECT_REAL_ITEMS);
        }
      int n = secmem.size_totale();
      if (solution.size_totale() != n)
        {
          Cerr << "the size of the solution and the second member does not match";
          exit();
        }

      // Le nombre maximal d'iteration peut etre desormais borne par niter_max_diff_impl
      int nmax = le_schema_en_temps->niter_max_diffusion_implicite();

      DoubleTab p(solution);
      p = 0;
      DoubleTab phiB(p); // la partie Bord de l'operateur.
      DoubleTab resu(p);
      DoubleTab merk(solution);
      double aCKN = 1; // Crank - Nicholson -> 0.5

      // Calcul de la matrice Diagonale

      int precond_diag = 0;
      // On preconditionne par la diagonale si on penalise
      // car sinon residu initial trop grand
      if (size_terme_mul) precond_diag = 1;

      double seuil_diffusion_implicite = le_schema_en_temps->seuil_diffusion_implicite();
      // Recuperation eventuelle d'options de Parametre_diffusion_implicite
      if (parametre_equation().non_nul() && (sub_type(Parametre_diffusion_implicite, parametre_equation().valeur())))
        {
          const Parametre_diffusion_implicite& param = ref_cast(Parametre_diffusion_implicite,
                                                                parametre_equation().valeur());
          if (param.crank())
            aCKN = 0.5;
          precond_diag = param.precoditionnement_diag();
          // Bug fixed : Diagonal preconditionning is fixed with periodic BC (it is OK for a parallel calculation)
          //if (precond_diag==1 && Process::nproc()>1)
          //  {
          //    Cerr << "Error with the value of preconditionnement_diag option which is set to " << precond_diag << "." << finl;
          //    Cerr << "The diagonal preconditionning is unavailable for a parallel calculation." << finl;
          //    Cerr << "The CG used to solve the implicitation of the equation diffusion operator can not preconditioned." << finl;
          //    Cerr << "So edit your .data file with preconditionnement_diag = 0 and run your case." << finl;
          //    exit();
          //  }
          if (param.seuil_diffusion_implicite() > 0)
            seuil_diffusion_implicite = param.seuil_diffusion_implicite();
          if (param.nb_it_max() > 0)
            nmax = param.nb_it_max();
        }
      statistiques().end_count(diffusion_implicite_counter_,0,0);

      /////////////////////
      // Preconditionnement
      /////////////////////
      double dt = le_schema_en_temps->pas_de_temps();
      Matrice_Morse_Diag diag;
      if (precond_diag)
        {
          statistiques().begin_count(assemblage_sys_counter_);
          const int nb_case = inconnue().valeurs().dimension_tot(0);
          const int nb_comp = inconnue().valeurs().line_size();
          if (nb_comp * nb_case != n)
            {
              Cerr << "the size of the unknown and the second member does not match" << finl;
              Cerr << "dimension_tot nbdim1 nb_comp = " << nb_case << " " << nb_comp << finl;
              Cerr << "size_totale = " << n << finl;
              exit();
            }
          diag.dimensionne_diag(n);
          operateur(0).l_op_base().contribuer_a_avec(inconnue().valeurs(), diag);
          for (int i = 0; i < size_s; i++)
            if (marq[i])
              sources()(i).valeur().contribuer_a_avec(inconnue().valeurs(), diag);
          // La diagonale est proportionnelle au volume de controle....
          // Il faut appliquer le solveur_masse
          DoubleTab tempo(inconnue().valeurs());
          for (int ca = 0; ca < nb_case; ca++)
            for (int ncp = 0; ncp < nb_comp; ncp++)
              tempo(ca, ncp) = diag(ca * nb_comp + ncp, ca * nb_comp + ncp);
          solveur_masse.appliquer(tempo);
          tempo.echange_espace_virtuel();
          // On inverse... // Crank - Nicholson
          // La matrice correspond a - la jacobienne (pour avoir un plus justement, GF)
          for (int ca = 0; ca < nb_case; ca++)
            {
              double tmp = (size_terme_mul ? terme_mul(ca) : 1) / dt;
              for (int ncpa = 0; ncpa < nb_comp; ncpa++)
                diag(ca * nb_comp + ncpa, ca * nb_comp + ncpa) = 1. / (tmp + tempo(ca, ncpa) * aCKN);
            }
          statistiques().end_count(assemblage_sys_counter_);
        }
      // On utilise p pour calculer phiB :
      operateur(0).ajouter(p, phiB);
      if (marq_tot)
        {
          for (int i = 0; i < size_s; i++)
            if (marq[i])
              ref_cast(Source_dep_inco_base, sources()(i).valeur()).ajouter_(p, phiB);
        }
      statistiques().begin_count(diffusion_implicite_counter_);
      solveur_masse.appliquer(phiB);
      // phiB *= aCKN;  // Crank - Nicholson
      // fait maintenant avant l'appel
      //solveur_masse.appliquer(secmem);

      DoubleTab sol;
      if (size_terme_mul)
        {
          sol = solution;
          for (int i = 0; i < size_terme_mul; i++)
            sol(i) *= terme_mul(i);
        }
      else
        sol.ref(solution);

      secmem.ajoute(1. / dt, sol, VECT_REAL_ITEMS);

      // Stop the counter because operator diffusion is also counted
      statistiques().end_count(diffusion_implicite_counter_, 0, 0);
      operateur(0).ajouter(solution, resu);
      if (marq_tot)
        {
          for (int i = 0; i < size_s; i++)
            if (marq[i])
              ref_cast(Source_dep_inco_base, sources()(i).valeur()).ajouter_(solution, resu);
        }
      statistiques().begin_count(diffusion_implicite_counter_);
      solveur_masse.appliquer(resu);
      resu.echange_espace_virtuel();
      resu *= -1.;
      resu.ajoute(1. / dt, sol, VECT_REAL_ITEMS);

      DoubleTab residu(resu); // residu = Ax
      residu -= secmem;      // residu = Ax-B
      DoubleTab z;
      if (precond_diag)
        {
          z = residu;
          diag.multvect(residu, z); // z = D-1(Ax-B)
        }
      else
        z.ref(residu); // z = Ax-B
      p -= z;

      double initial_residual = mp_carre_norme_vect(z); // =||Ax(0)-B|| ou ||D-1.(Ax(0)-B)||
      /* Calcul different de initial_residual:
         DoubleTab tmp(secmem);
         if (precond_diag)
         diag.multvect(secmem, tmp);
         else
         tmp.ref(secmem);
         double initial_residual = mp_carre_norme_vect(tmp) ; // =||B|| ou ||D-1.B|| */

      if (le_schema_en_temps->impr_diffusion_implicite()) Cout << "Residu(0)=" << initial_residual << finl;
      double seuil = seuil_diffusion_implicite * seuil_diffusion_implicite;
      // On calcule un seuil relatif (PL: 18/07/12, j'ai l'impression qu'il y'a une erreur, c'est seuil*=initial_residual;
      // En outre, il serait bon de faire residual = sqrt(mp_carre_norme_vect(tmp)) a plusieurs endroits...
      if (initial_residual > seuil)
        seuil = seuil_diffusion_implicite * initial_residual;

      double residual = seuil; // Il est mieux d'utiliser cette valeur pour entrer dans la boucle car seuil peut etre tres grand si le calcul diverge avant
      double prodrz_old = mp_prodscal(residu, z);
      DoubleTrav pp(p);
      int niter = 0;
      if (initial_residual != 0)
        while ((residual >= seuil) && (niter++ <= nmax))
          {
            resu = 0.;

            p.echange_espace_virtuel();

            // Stop the counter during diffusive operator:
            statistiques().end_count(diffusion_implicite_counter_, 0, 0);
            operateur(0).ajouter(p, resu);

            if (marq_tot)
              {
                for (int i = 0; i < size_s; i++)
                  if (marq[i])
                    ref_cast(Source_dep_inco_base, sources()(i).valeur()).ajouter_(p, resu);
              }
            statistiques().begin_count(diffusion_implicite_counter_);
            solveur_masse.appliquer(resu);
            //resu *= aCKN ;  // Crank - Nicholson
            resu.echange_espace_virtuel();

            resu -= phiB;// On retire la contribution des bords.
            resu *= -aCKN;
            if (size_terme_mul)
              {
                for (int i = 0; i < size_terme_mul; i++)
                  pp(i) = p(i) * terme_mul(i);
                resu.ajoute(1. / dt, pp, VECT_REAL_ITEMS);
              }
            else
              resu.ajoute(1. / dt, p, VECT_REAL_ITEMS);
            double alfa = prodrz_old / mp_prodscal(resu, p);
            // Inutile de faire un echange espace virtuel:
            solution.ajoute(alfa, p, VECT_REAL_ITEMS);
            residu.ajoute(alfa, resu, VECT_REAL_ITEMS);

            if (precond_diag)
              diag.multvect(residu, z); // preconditionnement par diag^(-1)
            // sinon z=residu

            residual = mp_carre_norme_vect(z);
            if (le_schema_en_temps->impr_diffusion_implicite())
              Cout << "Iteration n=" << niter << " Residu(n)/Residu(0)=" << residual / initial_residual << finl;

            double prodrz_new = mp_prodscal(residu, z);
            p *= (prodrz_new / prodrz_old);
            p -= z;
            prodrz_old = prodrz_new;
            // On previent si convergence anormalement longue sur de tres gros cas
            if (niter == 100)
              {
                Cout
                    << "Already 100 iterations of the conjugate gradient solver used for diffusion implicited algorithm."
                    << finl;
                Cout
                    << "The calculation is diverging, may be because of incorrect initial and/or boundary conditions for the equation "
                    << que_suis_je() << finl;
                Cout
                    << "You may also try to stop the calculation and rerun it with a lower facsec, say 0.9 or may be less: 0.5."
                    << finl;
              }
          }
      if ((le_schema_en_temps->no_error_if_not_converged_diffusion_implicite() == 0) && (niter > nmax))
        {
          Cerr << "No convergence of the implicited diffusion algorithm for the equation " << que_suis_je() << " in "
               << nmax << " iterations." << finl;
          Cerr << "Residue : " << residual << " Threshold : " << seuil << finl;
          Cerr << "======================================================================================" << finl;
          Cerr << "The problem is post processed to help you to see where the non convergence is located." << finl;
          schema_temps().set_stationnaire_atteint()=0;
          probleme().postraiter(1);
          probleme().mettre_a_jour(schema_temps().temps_courant());
          probleme().finir();
          Cerr << "The problem " << probleme().le_nom() << " has been saved too." << finl;
          exit();
        }
      else
        Cout << "Diffusion operator implicited for the equation " << que_suis_je()
             << " : Conjugate gradient converged in " << niter << " iterations." << finl;
      solution -= merk;
      solution.echange_espace_virtuel();

      // End the counter
      statistiques().end_count(diffusion_implicite_counter_);

      // CHD 230501 : Call to diffusive operator to update flux_bords (boundary fluxes):
      resu = 0.;
      operateur(0).ajouter(inconnue(), resu);
      if (marq_tot)
        {
          for (int i = 0; i < size_s; i++)
            if (marq[i])
              ref_cast(Source_dep_inco_base, sources()(i).valeur()).ajouter_(inconnue()->valeurs(), resu);
        }
      // Since 1.6.8 returns dI/dt:
      solution/=dt;
    }
}

DoubleTab& Equation_base::corriger_derivee_expl(DoubleTab& d)
{
  return d;
}
DoubleTab& Equation_base::corriger_derivee_impl(DoubleTab& d)
{
  return d;
}

const RefObjU& Equation_base::get_modele(Type_modele type) const
{
  return liste_modeles_(0);

}
// MODIF ELI LAUCOIN (22/11/2007) : je rajoute un avancer et un reculer
// par defaut, cela ne fait qu'avancer ou reculer l'inconnue (et champ_conserve_ si initialise)
void Equation_base::avancer(int i)
{
  inconnue().avancer(i);
  if (champ_conserve_.non_nul()) champ_conserve_->avancer(i);
  if (champ_convecte_.non_nul()) champ_convecte_->avancer(i);
}

void Equation_base::reculer(int i)
{
  inconnue().reculer(i);
  if (champ_conserve_.non_nul()) champ_conserve_->reculer(i);
  if (champ_convecte_.non_nul()) champ_convecte_->reculer(i);
}
// FIN MODIF ELI LAUCOIN (22/11/2007)


#define BLOQUE Cerr<<__PRETTY_FUNCTION__<< " "<<__FILE__<<":"<<(int)__LINE__<<" not coded, retrieves coding simpler" <<finl;exit()

// methodes pour l'implicite

/* peut utiliser une memoization (discretisations PolyMAC_P0P1NC)*/
void Equation_base::dimensionner_matrice(Matrice_Morse& matrice)
{
  if (matrice_init)
    {
      matrice.get_set_tab1().ref_array(matrice_stockee.get_set_tab1());
      matrice.get_set_tab2().ref_array(matrice_stockee.get_set_tab2());
      matrice.get_set_coeff().ref_array(matrice_stockee.get_set_coeff());
      matrice.set_nb_columns(matrice_stockee.nb_colonnes());
      matrice.sorted_ = matrice_stockee.sorted_;
      return;
    }

  dimensionner_matrice_sans_mem(matrice); //le vrai appel

  matrice.get_set_coeff() = 0.0;  // just to be sure ...

  if (probleme().discretisation().is_polymac_family())
    {
      matrice.sort_stencil();
      matrice_stockee.get_set_tab1().ref_array(matrice.get_set_tab1());
      matrice_stockee.get_set_tab2().ref_array(matrice.get_set_tab2());
      matrice_stockee.get_set_coeff().ref_array(matrice.get_set_coeff());
      matrice_stockee.set_nb_columns(matrice.nb_colonnes());
      matrice_stockee.sorted_ = 1;
      matrice_init = 1;
    }
}

void Equation_base::dimensionner_matrice_sans_mem(Matrice_Morse& matrice)
{
  // [ABN]: dimensioning of the implicit matrix: it can receive input from:
  //  - the operators
  //  - the mass solver
  //  - the sources
  // We proceed in this order, with the idea that operators should take most of the slots in the Morse
  // structure. Having the mass solver first would lead (in VDF, EF and VEF) to diagonal slots
  // being reserved before sub-diagonal terms. In the Morse structure coefficients would then
  // not be ordered by increasing columns number. This is not desirable, notably for EF which seem to
  // be sensitive to this ordering (although they should not ....). Some Baltiks too (FLICA5 - thermic part)
  // rely on this.
  if (matrice_init) // memoization
    {
      matrice.get_set_tab1().ref_array(matrice_stockee.get_set_tab1());
      matrice.get_set_tab2().ref_array(matrice_stockee.get_set_tab2());
      matrice.get_set_coeff().ref_array(matrice_stockee.get_set_coeff());
      matrice.set_nb_columns(matrice_stockee.nb_colonnes());
      return;
    }

  bool isInit = false;
  // Operators first (they're likely to take most of the slots in the Morse structure)
  for(int opp=0; opp < nombre_d_operateurs(); opp++)
    {
      if (!isInit)
        operateur(opp).l_op_base().dimensionner(matrice);
      else
        {
          Matrice_Morse mat2;
          operateur(opp).l_op_base().dimensionner(mat2);
          if (mat2.nb_colonnes())
            matrice += mat2;  // this only works if the matrix has been given its overall size first
        }
      isInit = isInit || (matrice.nb_colonnes() != 0);

    }

  //  ... then the mass solver ...
  if (!isInit)
    solv_masse().valeur().dimensionner(matrice);
  else
    {
      Matrice_Morse mat2;
      solv_masse().valeur().dimensionner(mat2);
      matrice += mat2; // this only works if the matrix has been given its overall size first
    }

  // and finally sources (mass solver has surely done something already so no need for "isInit" test)
  les_sources.dimensionner(matrice);
}

void Equation_base::dimensionner_termes_croises(Matrice_Morse& matrice, const Probleme_base& autre_pb, int nl, int nc)
{
  matrice.dimensionner(nl, nc, 0);
  for(int i = 0; i < nombre_d_operateurs(); i++)
    {
      Matrice_Morse mat2;
      operateur(i).l_op_base().dimensionner_termes_croises(mat2, autre_pb, nl, nc);
      if (mat2.nb_colonnes()) matrice += mat2;
    }
}

void Equation_base::ajouter_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, DoubleTab& resu) const
{
  for (int i = 0; i < nombre_d_operateurs(); i++)
    operateur(i).l_op_base().ajouter_termes_croises(inco, autre_pb, autre_inco, resu);
}

void Equation_base::contribuer_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, Matrice_Morse& matrice) const
{
  for (int i = 0; i < nombre_d_operateurs(); i++)
    operateur(i).l_op_base().contribuer_termes_croises(inco, autre_pb, autre_inco, matrice);
}

// ajoute les contributions des operateurs et des sources
void Equation_base::assembler(Matrice_Morse& matrice, const DoubleTab& inco, DoubleTab& resu)
{
  // Test de verification de la methode contribuer_a_avec
  for (int op=0; op<nombre_d_operateurs(); op++)
    operateur(op).l_op_base().tester_contribuer_a_avec(inco, matrice);

  // Contribution des operateurs et des sources:
  // [Vol/dt+A]Inco(n+1)=somme(residu)+Vol/dt*Inco(n)
  // Typiquement: si Op=flux(Inco) alors la matrice implicite A contient une contribution -dflux/dInco
  // Exemple: Op flux convectif en VDF:
  // Op=T*u*S et A=-d(T*u*S)/dT=-u*S
  const Discretisation_base::type_calcul_du_residu& type_codage=probleme().discretisation().codage_du_calcul_du_residu();
  if (type_codage==Discretisation_base::VIA_CONTRIBUER_AU_SECOND_MEMBRE)
    {
      // On calcule somme(residu) par contribuer_au_second_membre (typiquement CL non implicitees)
      // Cette approche necessite de coder 3 methodes (contribuer_a_avec, contribuer_au_second_membre et ajouter pour l'explicite)
      sources().contribuer_a_avec(inco,matrice);
      statistiques().end_count(assemblage_sys_counter_,0,0);
      sources().ajouter(resu);
      statistiques().begin_count(assemblage_sys_counter_);
      matrice.ajouter_multvect(inco, resu); // Add source residual first
      for (int op = 0; op < nombre_d_operateurs(); op++)
        {
          operateur(op).l_op_base().contribuer_a_avec(inco, matrice);
          operateur(op).l_op_base().contribuer_au_second_membre(resu);
        }
    }
  else if (type_codage==Discretisation_base::VIA_AJOUTER)
    {
      // On calcule somme(residu) par somme(operateur)+sources+A*Inco(n)
      // Cette approche necessite de coder seulement deux methodes (contribuer_a_avec et ajouter)
      // Donc un peu plus couteux en temps de calcul mais moins de code a ecrire/maintenir
      for (int op=0; op<nombre_d_operateurs(); op++)
        {
          operateur(op).l_op_base().contribuer_a_avec(inco, matrice);
          statistiques().end_count(assemblage_sys_counter_, 0, 0);
          operateur(op).ajouter(resu);
          statistiques().begin_count(assemblage_sys_counter_);
        }
      sources().contribuer_a_avec(inco,matrice);
      statistiques().end_count(assemblage_sys_counter_,0,0);
      sources().ajouter(resu);
      statistiques().begin_count(assemblage_sys_counter_);
      matrice.ajouter_multvect(inco, resu); // Ajout de A*Inco(n)
      // PL (11/04/2018): On aimerait bien calculer la contribution des sources en premier
      // comme dans le cas VIA_CONTRIBUER_AU_SECOND_MEMBRE mais le cas Canal_perio_3D (keps
      // periodique plante: il y'a une erreur de periodicite dans les termes sources du modele KEps...
    }
  else
    {
      Cerr << "Unknown value in Equation_base::assembler for " << (int)type_codage << finl;
      Process::exit();
    }
}

// modifie la matrice et le second mmebre en fonction des CL
void Equation_base::modifier_pour_Cl(Matrice_Morse& mat_morse, DoubleTab& secmem) const
{
  for (int op=0; op<nombre_d_operateurs(); op++)
    {
      operateur(op).l_op_base().modifier_pour_Cl(mat_morse,secmem);
    }
}
// assemble, ajoute linertie,et modifie_pour_cl.
void Equation_base::assembler_avec_inertie( Matrice_Morse& mat_morse,const DoubleTab& present, DoubleTab& secmem)
{
  statistiques().begin_count(assemblage_sys_counter_);
  assembler(mat_morse,present,secmem);
  schema_temps().ajouter_inertie(mat_morse,secmem,(*this));
  modifier_pour_Cl(mat_morse,secmem);
  statistiques().end_count(assemblage_sys_counter_);
}

/* verifie que tous les termes sont compatibles */
int Equation_base::has_interface_blocs() const
{
  int ok = 1;
  /* operateurs, masse, sources */
  for (int op = 0; op < nombre_d_operateurs(); op++) ok &= operateur(op).l_op_base().has_interface_blocs();
  ok &= solv_masse().valeur().has_interface_blocs();
  for (int i = 0; i < les_sources.size(); i++) ok &= les_sources(i).valeur().has_interface_blocs();
  return ok;
}

/* comme dimmensionner_matrice(), mais sans memoization */
void Equation_base::dimensionner_blocs(matrices_t matrices, const tabs_t& semi_impl) const
{
  /* operateurs, masse, sources */
  int nb = nombre_d_operateurs();
  for (int op = 0; op < nb; op++) operateur(op).l_op_base().dimensionner_blocs(matrices, semi_impl);
  solv_masse().valeur().dimensionner_blocs(matrices);
  for (int i = 0; i < les_sources.size(); i++) les_sources(i).valeur().dimensionner_blocs(matrices, semi_impl);
}

void Equation_base::assembler_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  /* mise a zero */
  secmem = 0;
  for (auto && i_m : matrices) i_m.second->get_set_coeff() = 0;
  /* operateurs, sources, masse */
  for (int i = 0; i < nombre_d_operateurs(); i++)
    operateur(i).l_op_base().ajouter_blocs(matrices, secmem, semi_impl);

  statistiques().end_count(assemblage_sys_counter_, 0, 0);

  statistiques().begin_count(source_counter_);
  for (int i = 0; i < les_sources.size(); i++)
    les_sources(i).valeur().ajouter_blocs(matrices, secmem, semi_impl);

  statistiques().end_count(source_counter_);

  statistiques().begin_count(assemblage_sys_counter_);
  if (!(discretisation().is_polymac_family() || probleme().que_suis_je() == "Pb_Multiphase"))
    {
      const std::string& nom_inco = inconnue().le_nom().getString();
      Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;
      if(mat) mat->ajouter_multvect(inconnue().valeurs(), secmem);
    }
}

void Equation_base::assembler_blocs_avec_inertie(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl)
{
  statistiques().begin_count(assemblage_sys_counter_);
  assembler_blocs(matrices, secmem, semi_impl);
  solv_masse().valeur().set_penalisation_flag(0);
  schema_temps().ajouter_blocs(matrices, secmem, *this);

  if (!discretisation().is_polymac_family())
    {
      const std::string& nom_inco = inconnue().le_nom().getString();
      Matrice_Morse *mat = matrices.count(nom_inco) ? matrices.at(nom_inco) : NULL;
      modifier_pour_Cl(*mat,secmem);
    }
  statistiques().end_count(assemblage_sys_counter_);
}

/* creation de champ_conserve_, cl_champ_conserve_ */
void Equation_base::init_champ_conserve() const
{
  if (champ_conserve_.non_nul()) return; //deja fait
  int Nt = inconnue()->nb_valeurs_temporelles(), Nl = inconnue().valeurs().size_reelle_ok() ? inconnue().valeurs().dimension(0) : -1, Nc = inconnue().valeurs().line_size();
  //champ_conserve_ : meme type / support que l'inconnue
  discretisation().creer_champ(champ_conserve_, domaine_dis().valeur(), inconnue().valeur().que_suis_je(), "N/A", "N/A", Nc, Nl, Nt, schema_temps().temps_courant());
  champ_conserve_->associer_eqn(*this);
  auto nom_fonc = get_fonc_champ_conserve();
  champ_conserve_->nommer(nom_fonc.first);
  champ_conserve_->init_champ_calcule(*this, nom_fonc.second);
}

/* methode de calcul par defaut de champ_conserve : produit coefficient_temporel * inconnue */
void Equation_base::calculer_champ_conserve(const Objet_U& obj, DoubleTab& val, DoubleTab& bval, tabs_t& deriv)
{
  const Equation_base& eqn = ref_cast(Equation_base, obj);
  const Champ_base *coeff = eqn.solv_masse().valeur().has_coefficient_temporel() ? &eqn.get_champ(eqn.solv_masse().valeur().get_name_of_coefficient_temporel()) : NULL; //coeff temporel
  const Champ_Inc_base& inco = eqn.inconnue();
  ConstDoubleTab_parts part(inco.valeurs());
  //valeur du champ lui-meme
  val = part[0];
  if (coeff) tab_multiply_any_shape(val, coeff->valeurs(), VECT_ALL_ITEMS);

  bval = inco.valeur_aux_bords();
  if (coeff) tab_multiply_any_shape(bval, coeff->valeur_aux_bords(), VECT_ALL_ITEMS);

  DoubleTab& der = deriv[inco.le_nom().getString()];
  if (coeff) der = coeff->valeurs();
  else der.resize(val.dimension_tot(0), val.line_size()), der = 1;
}

// Impression du residu dans fic (generalement dt_ev)
// Cette methode peut etre surchargee par des equations
// imprimant des residus particuliers (K-Eps, Concentrations,...)
void Equation_base::imprime_residu(SFichier& fic)
{
  int size = residu_.size_array();
  for (int i=0; i<size; i++)
    {
      Nom tab = schema_temps().norm_residu() == "max" ? "\t " : "\t\t ";
      fic << residu_(i) << tab;
      int nb_unknowns = inconnue().valeurs().dimension_tot(0);
      double residual_limit = ( nb_unknowns == 0 ? DMAXFLOAT : DMAXFLOAT/nb_unknowns);
      if (residu_(i)>residual_limit)
        {
          fic << finl;
          Cerr << "Equation " << le_nom() << " is diverging:" << finl;
          Cerr << "A residual ||dI/dt|| for this equation is bigger than " << residual_limit << " !" << finl;
          Cerr << "Have a look at the " << nom_du_cas() << ".dt_ev file." << finl;
          Process::exit();
        }
    }
  // Affichage min/max
  if (schema_temps().impr_extremums() && limpr())
    {
      double vmax = mp_max_vect(inconnue().valeurs());
      double vmin = mp_min_vect(inconnue().valeurs());
      Cout << finl << inconnue().le_nom() << " field [min/max]: ";
      if (je_suis_maitre()) printf("[ %.2e / %.2e ]",vmin,vmax);
      Cout << finl;
    }
}

// Retourne l'expression du residu (de meme peut etre surcharge)
Nom Equation_base::expression_residu()
{
  int size = residu_.size_array();
  Nom tmp(""),ajout("");
  if (probleme().is_coupled())
    {
      ajout="_";
      ajout+=probleme().le_nom();
    }
  for (int i=0; i<size; i++)
    {
      Nom norm = schema_temps().norm_residu();
      tmp+="Ri=";
      tmp+=norm;
      if(norm != "max") tmp+="-norm";
      tmp+=ajout;
      tmp+="|d";
      // Comment nommer de facon claire ET concise
      // le champ de l'expression:
      if (size==1)
        {
          // Champ scalaire
          tmp+=(Motcle)inconnue().le_nom()[0];
        }
      else
        {
          // Champ multiscalaire
          if (inconnue()->nom_compo(0)[0]!=inconnue()->nom_compo(size-1)[0])
            {
              // Exemple, champ K_Eps
              tmp+=(Motcle)inconnue()->nom_compo(i)[0];
            }
          else
            {
              // Exemple, champ concentration
              tmp+=(Motcle)inconnue().le_nom()[0];
              tmp+=(Nom)i;
            }
        }
      tmp+="/dt|\t ";
    }
  return tmp;
}

// Dimension les tableaux des residus
void Equation_base::initialise_residu(int size)
{
  if (size==0)
    size = inconnue().valeurs().line_size();
  residu_.resize(size);
  residu_initial_.resize(size);
  residu_=0;
  residu_initial_=0;
}

// Remplit le champ field_residu_ si existant
void Equation_base::set_residuals(const DoubleTab& residual)
{
  if(field_residu_.non_nul())
    {
      DoubleTab& tab = field_residu_.valeur().valeurs();
      if (tab.dimension_tot(0) == residual.dimension_tot(0))
        tab = residual;
      else
        {
          ConstDoubleTab_parts parts(residual);
          if (parts[0].dimension_tot(0) == tab.dimension_tot(0))
            tab = parts[0];
        }
    }
}

const Operateur& Equation_base::operateur_fonctionnel(int i) const
{
  Cerr<<"The method operateur_fonctionnel is not coded for equation "<<que_suis_je()<<finl;
  exit();
  throw;
}

Operateur& Equation_base::operateur_fonctionnel(int i)
{
  Cerr<<"The method operateur_fonctionnel is not coded for equation "<<que_suis_je()<<finl;
  exit();
  throw;
}

int Equation_base::nombre_d_operateurs_tot() const
{
  return nombre_d_operateurs();
}
