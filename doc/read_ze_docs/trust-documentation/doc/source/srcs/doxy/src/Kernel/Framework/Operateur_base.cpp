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

#include <Operateur_base.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <EcrFicPartage.h>
#include <Periodique.h>
#include <sys/stat.h>
#include <Champ.h>
#include <Front_VF.h>
#include <Domaine_VF.h>
#include <Matrice_Morse.h>
#include <TRUSTTrav.h>
#include <Discretisation_base.h>

Implemente_base_sans_constructeur(Operateur_base,"Operateur_base",Objet_U);

Operateur_base::Operateur_base()
{
  decal_temps=0;
  nb_ss_pas_de_temps=1;
  col_width_ = -1;
}

/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Imprime l'operateur sur un flot de sortie.
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Operateur_base::printOn(Sortie& os) const
{
  return os ;
}


/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Lit un operateur sur un flot d'entree.
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& Operateur_base::readOn(Entree& is)
{
  return is ;
}


/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Mise a jour de l'operateur
 *
 * @param (double) un pas de temps
 */
void Operateur_base::mettre_a_jour(double )
{
}


void Operateur_base::abortTimeStep()
{
}


/*! @brief Associe l'operateur au domaine_dis, le domaine_Cl_dis, et a l'inconnue de son equation.
 *
 * @throws pas d'equation associee
 */
void Operateur_base::completer()
{
  assert(mon_equation.non_nul());
  const Equation_base& eqn = equation();
  const Domaine_dis& zdis= eqn.domaine_dis();

  const Domaine_Cl_dis& zcl = le_champ_inco.non_nul() ? le_champ_inco.valeur()->domaine_Cl_dis() : eqn.domaine_Cl_dis();
  const Champ_Inc& inco = le_champ_inco.non_nul() ? le_champ_inco.valeur() : eqn.inconnue();
  associer(zdis, zcl, inco);
  const Conds_lim& les_cl = zcl->les_conditions_limites();
  for (auto& itr : les_cl)
    {
      const Frontiere_dis_base& la_fr = itr.frontiere_dis();
      col_width_ = std::max(col_width_, la_fr.le_nom().longueur());
    }
  int w_suffix = 3; // pour ajout _Mx (moment)

  // pour les champs a plusieurs composantes, le header des colonnes des fichiers .out prend la forme
  // Time cl1_compo1  cl1_compo2 ...
  // on prend en compte la longueur de compo1, compo2, etc...
  Noms noms_compo_courts(inco->noms_compo());
  if (noms_compo_courts.size() > 1)
    for (int i = 0; i < noms_compo_courts.size(); ++i)
      {
        noms_compo_courts[i] = Motcle(noms_compo_courts[i]).getSuffix(eqn.inconnue().le_nom());
        w_suffix = std::max(w_suffix, noms_compo_courts[i].longueur());
      }
  col_width_ += w_suffix + 1;
}

void Operateur_base::associer_champ(const Champ_Inc& ch, const std::string& nom_ch)
{
  le_champ_inco = ch;
  nom_inco_ = nom_ch;
}

/*! @brief Calcul dt_stab
 *
 * @return (double) renvoie toujours  1.e30
 */
double Operateur_base::calculer_dt_stab() const
{
  return 1.e30;
}

void Operateur_base::calculer_dt_local(DoubleTab& dt) const
{
  Cerr << "You must overload the method " << que_suis_je()
       << "::calculer_dt_local(DoubleVect&)" << finl;
  exit();
}
/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 *     Imprime l'operateur sur un flot de sortie.
 *
 * @param (Sortie& os)
 * @return (int) renvoie toujours 1
 */
int Operateur_base::impr(Sortie& os) const
{
  return 1;
}


/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 * @param (Matrice_Morse&) une matrice au format Morse
 * @throws methode a surcharger
 */
void Operateur_base::dimensionner(Matrice_Morse& mat) const
{
  /* on tente dimensionner_blocs() */
  if (has_interface_blocs()) dimensionner_blocs({{ equation().inconnue().le_nom().getString(), &mat }});
  else Process::exit(que_suis_je() + " : dimensionner() not coded!");
}

void Operateur_base::dimensionner_bloc_vitesse(Matrice_Morse& mat) const
{
  /* on tente dimensionner_blocs() */
  if (has_interface_blocs()) dimensionner_blocs({{ "vitesse", &mat }});
}

void Operateur_base::dimensionner_termes_croises(Matrice_Morse& mat, const Probleme_base& autre_pb, int nl, int nc) const
{
  if (!has_interface_blocs()) return;
  std::string nom_inco = equation().inconnue().le_nom().getString(),
              nom = equation().probleme().le_nom() == autre_pb.le_nom() ? nom_inco : nom_inco + "/" + autre_pb.le_nom().getString(); //nom de bloc croise pour l'interface_blocs
  dimensionner_blocs({{ nom, &mat }}, {});
}

void Operateur_base::ajouter_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, DoubleTab& resu) const
{
  //si on a Interface_blocs, alors ajouter_blocs() est suffisant
  return;
}

void Operateur_base::contribuer_termes_croises(const DoubleTab& inco, const Probleme_base& autre_pb, const DoubleTab& autre_inco, Matrice_Morse& matrice) const
{
  if (!has_interface_blocs()) return;
  DoubleTrav secmem(inco); //on va le jeter
  std::string nom_inco = equation().inconnue().le_nom().getString(),
              nom = equation().probleme().le_nom() == autre_pb.le_nom() ? nom_inco : nom_inco + "/" + autre_pb.le_nom().getString(); //nom de bloc croise pour l'interface_blocs
  ajouter_blocs({{ nom, &matrice}}, secmem, {});
}

void Operateur_base::dimensionner_blocs(matrices_t mats, const tabs_t& semi_impl) const
{
  Process::exit(que_suis_je() + " : dimensionner_blocs() not coded!");
}

void Operateur_base::ajouter_blocs(matrices_t mats, DoubleTab& secmem, const tabs_t& semi_impl) const
{
  Process::exit(que_suis_je() + " : ajouter_blocs() not coded!");
}

/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 * @param (Matrice_Morse&) une matrice au format Morse
 * @param (DoubleTab&) un tableau de valeur (double)
 * @throws methode a surcharger
 */
void Operateur_base::modifier_pour_Cl(Matrice_Morse&, DoubleTab&) const
{
  Cerr << "***********************************************************************************" << finl;
  Cerr << "Sorry, implicit scheme is not available yet for the " << que_suis_je() << " scheme." << finl;
  Cerr << "Please, change this scheme or use another time scheme than " << equation().probleme().schema_temps().que_suis_je() << finl;
  Cerr << "***********************************************************************************" << finl;
  //  Cerr << "You must overload the method " << que_suis_je()
  //     << "::modifier_pour_Cl(Matrice_Morse&, DoubleTab& )" << finl;
  exit();
}

DoubleTab&  Operateur_base::ajouter(const DoubleTab& inco, DoubleTab& secmem) const
{
  /* on tente ajouter_blocs */
  if (has_interface_blocs())
    {
      if (equation().discretisation().is_polymac_family())
        ajouter_blocs({}, secmem);
      else
        ajouter_blocs({}, secmem, {{ equation().inconnue().le_nom().getString(),inco }} ); //pour prise en compte du parametre inco (qui est pas forcement l'inco de l'equation)
    }
  else Process::exit(que_suis_je() + " : ajouter() not coded!");
  return secmem;
}

DoubleTab&  Operateur_base::calculer(const DoubleTab& inco, DoubleTab& secmem) const
{
  secmem = 0;
  return ajouter(inco, secmem);
}

/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 * @param (DoubleTab&) un tableau de valeur (double)
 * @param (Matrice_Morse&) une matrice au format Morse
 * @throws methode a surcharger
 */
void Operateur_base::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  /* on tente ajouter_blocs() */
  DoubleTrav secmem(inco); //on va le jeter
  if (has_interface_blocs()) ajouter_blocs({{ equation().inconnue().le_nom().getString(), &matrice }}, secmem);
  else Process::exit(que_suis_je() + " : contribuer_a_avec() not coded!");
}

void Operateur_base::contribuer_bloc_vitesse(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  /* on tente ajouter_blocs() */
  if (has_interface_blocs())
    {
      DoubleTrav secmem(equation().inconnue().valeurs()); //on va le jeter
      ajouter_blocs({{ "vitesse", &matrice }}, secmem);
    }
}

/*! @brief NE FAIT RIEN A surcharger dans les classes derivees.
 *
 * @param (DoubleTab&) un tableau de valeur (double)
 * @throws methode a surcharger
 */
void Operateur_base::contribuer_au_second_membre(DoubleTab&) const
{
  Cerr << "You must overload the method " << que_suis_je()
       << "::contribuer_au_second_membre(DoubleTab&)" << finl;
  exit();
}

int Operateur_base::systeme_invariant() const
{
  return 1;
}
void Operateur_base::associer_domaine_cl_dis(const Domaine_Cl_dis_base&)
{
  Cerr<<"Operateur_base::associer_domaine_cl_dis must be overloaded "<<finl;
  exit();
}

void Operateur_base::set_fichier(const Nom& nom)
{
  if (nom == "")
    {
      Cerr << "You provided an empty string to name the .out file of operator " << que_suis_je() << " in equation " << equation().que_suis_je() << "." << finl;
      Cerr << "If you don't want to write a .out file for this operator, don't call set_fichier()" << finl;
      Process::exit();
    }
  out_=Objet_U::nom_du_cas();
  out_+="_";
  out_+=equation().probleme().le_nom()+"_"+nom;
}

/*! @brief Ouverture/creation d'un fichier d'impression d'un operateur A surcharger dans les classes derivees.
 *
 * @throws methode a surcharger
 */
void Operateur_base::ouvrir_fichier(SFichier& os,const Nom& type, const int flag) const
{

  // flag nul on n'ouvre pas le fichier
  if (flag==0)
    return ;

  if (!je_suis_maitre())
    {
      Cerr<<"Only master process can open "<<type<<" file. See Operateur_base::ouvrir_fichier()" << finl;
      Process::abort();
    }
  const Equation_base& eqn = equation();
  const Probleme_base& pb=eqn.probleme();
  const Schema_Temps_base& sch=pb.schema_temps();
  const int precision = sch.precision_impr(), wcol = std::max(col_width_, sch.wcol()), gnuplot_header = sch.gnuplot_header();
  os.set_col_width(wcol);
  Nom nomfichier(out_);
  if (type!="") nomfichier+=(Nom)"_"+type;
  nomfichier+=".out";
  struct stat f;
  // On cree le fichier a la premiere impression avec l'en tete ou si le fichier n'existe pas
  if (stat(nomfichier,&f) || (sch.nb_impr()==1 && !pb.reprise_effectuee()))
    {
      os.ouvrir(nomfichier);

      std::vector<std::string> comp_m = {"_Mx", "_My", "_Mz"};
      os << (Nom)"# Printing on the boundaries of the equation "+eqn.que_suis_je()+" of the problem "+eqn.probleme().le_nom() << finl;
      os << "# " << (type=="moment" ? "Moment of " : "") << description() << finl;

      if (!gnuplot_header) os << "#";
      os.set_col_width(wcol - !gnuplot_header);
      os.add_col("Time");
      os.set_col_width(wcol);
      const Conds_lim& les_cls=eqn.inconnue()->domaine_Cl_dis().les_conditions_limites();

      if (flux_bords_.nb_dim()!=2)
        {
          Cerr << "flux_bords_ not dimensioned for the operator " << que_suis_je() << finl;
          Process::exit();
        }

      // s'il y a plusieurs composantes par CL, on se sert des noms de composante de l'inconnue
      int nb_compo = flux_bords_.line_size();
      if (type=="moment" && dimension == 2) nb_compo=1;
      Noms noms_compo_courts(eqn.inconnue()->noms_compo());
      if (nb_compo > 1)
        for (int i = 0; i < noms_compo_courts.size(); ++i)
          noms_compo_courts[i] = Motcle(noms_compo_courts[i]).getSuffix(eqn.inconnue().le_nom());

      // ecriture de l'entete des colonnes de la forme
      // Time cl1  cl2  cl3  ...
      // ou s'il y a plusieurs composantes :
      // Time cl1_compo1  cl1_compo2 cl2 compo1 cl2_compo2 ...
      for (int num_cl=0; num_cl<les_cls.size(); num_cl++)
        {
          const Frontiere_dis_base& la_fr = les_cls[num_cl].frontiere_dis();
          if (type!="sum" || eqn.domaine_dis().domaine().bords_a_imprimer_sum().contient(la_fr.le_nom()))
            {
              Nom ch = la_fr.le_nom();
              if (type=="moment")
                {
                  if (dimension==2)
                    os.add_col((ch + comp_m[2]).getChar());
                  else
                    for (int d = 0; d < dimension; ++d)
                      os.add_col((ch + comp_m[d]).getChar());
                }
              else
                {
                  if (nb_compo > 1)
                    for (int d = 0; d < nb_compo; ++d)
                      {
                        os.add_col((ch + "_" + noms_compo_courts[d]).getChar());
                      }
                  else os.add_col(ch.getChar());
                  // if periodic BC, we write the boundary name twice on the header of .out files
                  if ((sub_type(Periodique,les_cls[num_cl].valeur())) && (Objet_U::nom_du_cas()+"_"+eqn.probleme().le_nom()+"_Force_pression"!=out_))
                    {
                      if (nb_compo > 1)
                        for (int d = 0; d < nb_compo; ++d)
                          {
                            os.add_col((ch + "_" + noms_compo_courts[d]).getChar());
                          }
                      else os.add_col(ch.getChar());
                    }
                }
            }
        }
      if (type!="sum" && type!="moment")
        if (Objet_U::nom_du_cas()+"_"+eqn.probleme().le_nom()+"_Force_pression"!=out_)
          {
            if (nb_compo > 1)
              for (int d = 0; d < nb_compo; ++d)
                os.add_col((Nom("Total_") + noms_compo_courts[d]).getChar());
            else os.add_col("Total");
          }

      os << finl;
    }
  // Sinon on l'ouvre
  else
    {
      os.ouvrir(nomfichier,ios::app);
    }
  os.precision(precision);
  os.setf(ios::scientific);
}

/*! @brief Ouverture/creation d'un fichier d'impression d'un operateur A surcharger dans les classes derivees.
 *
 * @throws methode a surcharger
 */
void Operateur_base::ouvrir_fichier_partage(EcrFicPartage& os,const Nom& type, const int flag) const
{

  // flag nul on n'ouvre pas le fichier
  if (flag==0)
    return ;

  const Probleme_base& pb=equation().probleme();
  const Schema_Temps_base& sch=pb.schema_temps();
  const int precision=sch.precision_impr();
  Nom nomfichier(out_);
  if (type!="") nomfichier+=(Nom)"_"+type;
  nomfichier+=".face";
  // On cree le fichier a la premiere impression avec l'en tete
  if (sch.nb_impr()==1 && !pb.reprise_effectuee())
    {
      os.ouvrir(nomfichier);
    }
  // Sinon on l'ouvre
  else
    {
      os.ouvrir(nomfichier,ios::app);
    }
  os.precision(precision);
  os.setf(ios::scientific);
}

void Operateur_base::ajouter_contribution_explicite_au_second_membre(const Champ_Inc_base& inconnue, DoubleTab& derivee) const
{
  // Methode par defaut pour les operateurs de TRUST:
  ajouter(inconnue.valeurs(), derivee);
}


void Operateur_base::creer_champ(const Motcle& motlu)
{
}

const Champ_base& Operateur_base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);
}
bool Operateur_base::has_champ(const Motcle& nom, REF(Champ_base)& ref_champ) const
{
  return champs_compris_.has_champ(nom, ref_champ);
}

void Operateur_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<que_suis_je()<<" : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());
}

//Calcul du tableau de valeurs d une quantite lie a un operateur pour postraitement
//Options reconnues : "stabilite" pour dt_stab
//                      "flux_bords" ou "flux_surfacique_bords" pour flux_bords_
//
void Operateur_base::calculer_pour_post(Champ& espace_stockage,const Nom& option, int comp) const
{
  if (Motcle(option)=="flux_bords" || Motcle(option)=="flux_surfacique_bords")
    {
      bool surfacique = (Motcle(option)=="flux_surfacique_bords");
      DoubleTab& es_valeurs = espace_stockage->valeurs();
      es_valeurs = 0.;
      const Domaine_Cl_dis_base& zcl_dis = equation().domaine_Cl_dis();
      const Domaine_dis_base& zdis = equation().domaine_dis().valeur();
      int nb_front = zdis.nb_front_Cl();

      if (flux_bords_.nb_dim()==2)
        {
          DoubleVect aire;
          for (int n_bord=0; n_bord<nb_front; n_bord++)
            {
              const Cond_lim& la_cl = zcl_dis.les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              if (surfacique) la_cl.frontiere_dis().frontiere().faces().calculer_surfaces(aire);
              int ndeb = le_bord.num_premiere_face();
              int nfin = ndeb + le_bord.nb_faces();

              for (int face=ndeb; face<nfin; face++)
                es_valeurs(face) = flux_bords_(face,comp) / (surfacique ? aire(face-ndeb) : 1.);
            }
        }
    }
  else
    {
      Cerr<<"The method calculer_pour_post is not recognized for the option "<<option<<finl;
      exit();
    }
}

Motcle Operateur_base::get_localisation_pour_post(const Nom& option) const
{
  Motcle loc;
  if (Motcle(option)=="flux_bords" || Motcle(option)=="flux_surfacique_bords")
    {
      if (!equation().discretisation().is_vdf())
        loc = "face";
      else
        loc = "elem";
    }
  else
    {
      Cerr<<"The method get_localisation_pour_post is not actually coded "<<finl;
      Cerr<<"for the operator "<<que_suis_je()<<" and for the option "<<option<<finl;
      exit();
    }
  return loc;
}

// MODIF ELI LAUCOIN : 6/08/2008
// Je rajoute deux methodes pour le calcul du flux
void Operateur_base::ajouter_flux(const DoubleTab& inconnue, DoubleTab& contribution) const
{
  Cerr << "Warning : 'Operateur_base::ajouter_flux()' must be overloaded" << finl;
  Process::exit();
}

void Operateur_base::calculer_flux(const DoubleTab& inconnue, DoubleTab& flux) const
{
  flux = 0.;
  ajouter_flux(inconnue,flux);
}
// FIN MODIF ELI LAUCOIN : 6/08/2008


// Je rajoute une methode preparer_calcul()
// Ca me permet de coder plus facilement les operateurs de diffusion selon
// que la diffusivite varie ou non.
//
// Par defaut : ne fait rien
void Operateur_base::preparer_calcul(void) { }

// Methode pour tester la methode contribuer_a_avec
// Test active par une variable d'environnement
void Operateur_base::tester_contribuer_a_avec(const DoubleTab& inco, const Matrice_Morse& matrice)
{
  int test_op=0;
  {
    char* theValue = getenv("TRUST_TEST_OPERATEUR_IMPLICITE");
    if (theValue != NULL) test_op=2;
  }
  {
    char* theValue = getenv("TRUST_TEST_OPERATEUR_IMPLICITE_BLOQUANT");
    if (theValue != NULL) test_op=1;
  }
  if (test_op==0) return;

  Matrice_Morse mat_contribuer(matrice), mat_DF(matrice);
  DoubleTrav resu(inco);
  DoubleVect& coeff_contribuer = mat_contribuer.get_set_coeff();
  // A*Inc(n)=Op(Inc(n))+
  coeff_contribuer = 0;
  calculer(inco, resu); // Calcule l'operateur Op(Inc(n)) avec methode ajouter dans resu
  contribuer_a_avec(inco, mat_contribuer); // Construit la matrice de l'operateur (mat=-A)

  // calcul de la matrice par differences finies
  if (inco.dimension(1) == 1)
    {
      const double eps = 1e-6;
      DoubleVect& coeff_DF = mat_DF.get_set_coeff();
      const IntVect& tab1 = mat_DF.get_set_tab1(), &tab2 = mat_DF.get_set_tab2();
      coeff_DF = 0;
      for (int i = 0; i < inco.dimension(0); i++)
        for (int j = tab1[i] - 1; j < tab1[i + 1] - 1; j++)
          {
            const int i2 = tab2[j] - 1;

            DoubleTrav resu_DF(inco);
            DoubleTab inco_pert(inco);
            inco_pert[i2] += eps;
            calculer(inco_pert, resu_DF);
            mat_DF(i, i2) = -(resu_DF[i] - resu[i]) / eps;

          }

      // mat_DF.imprimer_formatte(Cerr);
      // mat_contribuer.imprimer_formatte(Cerr);
      mat_DF += -mat_contribuer;
      // mat_DF.imprimer_formatte(Cerr); // la difference des deux
      Cerr << "Max difference between contribuer_a_avec and finite difference jacobian : " << coeff_DF.mp_max_abs_vect() << finl;
    }
  else Cerr << "nbcomp > 1 : the finite difference jacobian matrix is not compared to the contribuer_a_avec one.";

  mat_contribuer.ajouter_multvect(inco, resu); // Calcule le flux avec la matrice et l'ajoute a resu (resu=Op(Inc(n))-A*Inc(n))
  resu*=-1;
  contribuer_au_second_membre(resu); // Ajoute flux impose
  mon_equation.valeur().solv_masse().appliquer(resu); // M-1*(Op(Inc(n))-A*Inc(n))
  // On multiplie par le volume car les coefficients sont divises par le volume et on ne veut pas
  // qu'un calcul sur des petites mailles semblent disfonctionner;
  DoubleTab un(inco);
  un=1;
  mon_equation.valeur().solv_masse().appliquer(un);
  resu/=mp_max_vect(un);
  double err=mp_max_abs_vect(resu);
  Cerr<<"Test contribuer_a_avec on " << que_suis_je() <<" error: "<<err<<finl;
  if (err>1e-6)
    {
      {
        DoubleVect& resu_=resu;
        Cerr<<" size "<< resu_.size()<<finl;
        for (int i=0; i<resu_.size(); i++)
          if (std::fabs(resu_(i))>1e-10)
            {
              Cerr<<i << " "<< resu_(i)<< " "<<finl;
            }
      }
      if (test_op==1)
        exit();
    }
}
