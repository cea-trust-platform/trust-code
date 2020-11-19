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
// File:        Operateur_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/44
//
//////////////////////////////////////////////////////////////////////////////

#include <Operateur_base.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <EcrFicPartage.h>
#include <Periodique.h>
#include <sys/stat.h>
#include <Champ.h>
#include <Front_VF.h>
#include <Zone_VF.h>
#include <Matrice_Morse.h>
#include <DoubleTrav.h>

Implemente_base_sans_constructeur(Operateur_base,"Operateur_base",Objet_U);

Operateur_base::Operateur_base()
{
  decal_temps=0;
  nb_ss_pas_de_temps=1;
  col_width_ = -1;
}

// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Imprime l'operateur sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Sortie& Operateur_base::printOn(Sortie& os) const
{
  return os ;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Lit un operateur sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Operateur_base::readOn(Entree& is)
{
  return is ;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Mise a jour de l'operateur
// Precondition:
// Parametre: double
//    Signification: un pas de temps
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Operateur_base::mettre_a_jour(double )
{
}


void Operateur_base::abortTimeStep()
{
}


// Description:
//    Associe l'operateur a la zone_dis, la zone_Cl_dis,
//    et a l'inconnue de son equation.
// Precondition: l'equation associee ne doit pas etre nulle (non associee)
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: pas d'equation associee
// Effets de bord:
// Postcondition: l'operateur est pret a etre utilise
void Operateur_base::completer()
{
  assert(mon_equation.non_nul());
  const Equation_base& eqn = equation();
  const Zone_dis& zdis= eqn.zone_dis();

  const Zone_Cl_dis& zcl = le_champ_inco.non_nul() ? le_champ_inco.valeur()->zone_Cl_dis() : eqn.zone_Cl_dis();
  const Champ_Inc& inco = le_champ_inco.non_nul() ? le_champ_inco.valeur() : eqn.inconnue();
  associer(zdis, zcl, inco);
  const Conds_lim& les_cl = zcl->les_conditions_limites();
  for (int i = 0; i < les_cl.size(); i++)
    {
      const Frontiere_dis_base& la_fr = les_cl[i].frontiere_dis();
      col_width_ = max(col_width_, la_fr.le_nom().longueur());
    }
  int w_suffix = 3; // pour ajout _Mx (moment)

  // pour les champs a plusieurs composantes, le header des colonnes des fichiers .out prend la forme
  // Time cl1_compo1  cl1_compo2 ...
  // on prend en compte la longueur de compo1, compo2, etc...
  Noms noms_compo_courts(inco->noms_compo());
  if (noms_compo_courts.size() > 1) for (int i = 0; i < noms_compo_courts.size(); ++i)
      {
        noms_compo_courts[i] = Motcle(noms_compo_courts[i]).getSuffix(eqn.inconnue().le_nom());
        w_suffix = max(w_suffix, noms_compo_courts[i].longueur());
      }
  col_width_ += w_suffix;
}

void Operateur_base::associer_champ(const Champ_Inc& ch)
{
  le_champ_inco = ch;
}

// Description:
//   Calcul dt_stab
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: renvoie toujours  1.e30
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
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
// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
//    Imprime l'operateur sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: renvoie toujours 1
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Operateur_base::impr(Sortie& os) const
{
  return 1;
}


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre: Matrice_Morse&
//    Signification: une matrice au format Morse
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: methode a surcharger
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Operateur_base::dimensionner(Matrice_Morse& ) const
{
  Cerr << "You must overload the method " << que_suis_je()
       << "::dimensionner(Matrice_Morse& )" << finl;
  exit();
}

void Operateur_base::dimensionner_bloc_vitesse(Matrice_Morse& ) const
{
  Cerr << "You must overload the method " << que_suis_je()
       << "::dimensionner_bloc_vitesse(Matrice_Morse& )" << finl;
  exit();
}

// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre: Matrice_Morse&
//    Signification: une matrice au format Morse
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: DoubleTab&
//    Signification: un tableau de valeur (double)
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: methode a surcharger
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
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


// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre: DoubleTab&
//    Signification: un tableau de valeur (double)
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces:
// Parametre: Matrice_Morse&
//    Signification: une matrice au format Morse
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: methode a surcharger
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Operateur_base::contribuer_a_avec(const DoubleTab&, Matrice_Morse&) const
{
  Cerr << "You must overload the method " << que_suis_je()
       << "::contribuer_a_avec(const DoubleTab&, Matrice_Morse&)" << finl;
  exit();
}

void Operateur_base::contribuer_bloc_vitesse(const DoubleTab&, Matrice_Morse&) const
{
  Cerr << "You must overload the method " << que_suis_je()
       << "::contribuer_bloc_vitesse(const DoubleTab&, Matrice_Morse&)" << finl;
  exit();
}

// Description:
//    NE FAIT RIEN
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre: DoubleTab&
//    Signification: un tableau de valeur (double)
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: methode a surcharger
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
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
void Operateur_base::associer_zone_cl_dis(const Zone_Cl_dis_base&)
{
  Cerr<<"Operateur_base::associer_zone_cl_dis must be overloaded "<<finl;
  exit();
}

void Operateur_base::set_fichier(const Nom& nom)
{
  out_=Objet_U::nom_du_cas();
  out_+="_";
  out_+=equation().probleme().le_nom()+"_"+nom;
}

// Description:
//    Ouverture/creation d'un fichier d'impression d'un operateur
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: methode a surcharger
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Operateur_base::ouvrir_fichier(SFichier& os,const Nom& type, const int& flag) const
{

  // flag nul on n'ouvre pas le fichier
  if (flag==0)
    return ;

  if (!je_suis_maitre())
    {
      Cerr<<"Pb with "<<type<<finl;
      ::abort();
    }
  const Equation_base& eqn = equation();
  const Probleme_base& pb=eqn.probleme();
  const Schema_Temps_base& sch=pb.schema_temps();
  const int precision = sch.precision_impr(), wcol = max(col_width_, sch.wcol()), gnuplot_header = sch.gnuplot_header();
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
      const Conds_lim& les_cls=eqn.inconnue()->zone_Cl_dis().les_conditions_limites();

      if (flux_bords_.nb_dim()!=2)
        {
          Cerr << "flux_bords_ not dimensioned for the operator " << que_suis_je() << finl;
          Process::exit();
        }

      // s'il y a plusieurs composantes par CL, on se sert des noms de composante de l'inconnue
      int nb_compo = flux_bords_.dimension(1);
      if (type=="moment" && dimension == 2) nb_compo=1;
      Noms noms_compo_courts(eqn.inconnue()->noms_compo());
      if (nb_compo > 1) for (int i = 0; i < noms_compo_courts.size(); ++i)
          noms_compo_courts[i] = Motcle(noms_compo_courts[i]).getSuffix(eqn.inconnue().le_nom());

      // ecriture de l'entete des colonnes de la forme
      // Time cl1  cl2  cl3  ...
      // ou s'il y a plusieurs composantes :
      // Time cl1_compo1  cl1_compo2 cl2 compo1 cl2_compo2 ...
      for (int num_cl=0; num_cl<les_cls.size(); num_cl++)
        {
          const Frontiere_dis_base& la_fr = les_cls[num_cl].frontiere_dis();
          if (type!="sum" || eqn.zone_dis().zone().Bords_a_imprimer_sum().contient(la_fr.le_nom()))
            {
              Nom ch = la_fr.le_nom();
              if (type=="moment")
                {
                  if (dimension==2)
                    os.add_col((ch + comp_m[2].c_str()).getChar());
                  else
                    for (int d = 0; d < dimension; ++d)
                      os.add_col((ch + comp_m[d].c_str()).getChar());
                }
              else
                {
                  if (nb_compo > 1) for (int d = 0; d < nb_compo; ++d)
                      {
                        os.add_col((ch + "_" + noms_compo_courts[d]).getChar());
                      }
                  else os.add_col(ch.getChar());
                }
            }
        }
      if (type!="sum" && type!="moment") if (Objet_U::nom_du_cas()+"_"+eqn.probleme().le_nom()+"_Force_pression"!=out_)
          {
            if (nb_compo > 1) for (int d = 0; d < nb_compo; ++d)
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

// Description:
//    Ouverture/creation d'un fichier d'impression d'un operateur
//    A surcharger dans les classes derivees.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: methode a surcharger
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
void Operateur_base::ouvrir_fichier_partage(EcrFicPartage& os,const Nom& type, const int& flag) const
{

  // flag nul on n'ouvre pas le fichier
  if (flag==0)
    return ;

  const Probleme_base& pb=equation().probleme();
  const Schema_Temps_base& sch=pb.schema_temps();
  const int& precision=sch.precision_impr();
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
      const Zone_Cl_dis_base& zcl_dis = equation().zone_Cl_dis();
      const Zone_dis_base& zdis = equation().zone_dis().valeur();
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
      const Zone_dis_base& zdis = equation().zone_dis().valeur();
      if (!zdis.que_suis_je().debute_par("Zone_VDF"))
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
// Test activé par une variable d'environnement
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

  Matrice_Morse mat(matrice);
  DoubleTrav resu(inco);
  DoubleVect& coeff = mat.get_set_coeff();
  // A*Inc(n)=Op(Inc(n))+
  coeff=0;
  calculer(inco, resu); // Calcule l'operateur Op(Inc(n)) avec methode ajouter dans resu
  contribuer_a_avec(inco, mat); // Construit la matrice de l'operateur (mat=-A)
  mat.ajouter_multvect(inco, resu); // Calcule le flux avec la matrice et l'ajoute a resu (resu=Op(Inc(n))-A*Inc(n))
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
  Cerr<<"Test contribuer_a_avec on " << que_suis_je() <<" error: "<<err<<finl;;
  if (err>1e-6)
    {
      {
        DoubleVect& resu_=resu;
        Cerr<<" size "<< resu_.size()<<finl;
        for (int i=0; i<resu_.size(); i++)
          if (dabs(resu_(i))>1e-10)
            {
              Cerr<<i << " "<< resu_(i)<< " "<<finl;
            }
      }
      if (test_op==1)
        exit();
    }
}
