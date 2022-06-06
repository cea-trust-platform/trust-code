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

#include <Milieu_base.h>
#include <Champ.h>
#include <Champ_Fonc_Tabule.h>
#include <Champ_Uniforme.h>
#include <Discretisation_base.h>
#include <Probleme_base.h>
#include <Equation_base.h>
#include <Param.h>
#include <Champ_Fonc_MED.h>
#include <Discretisation_tools.h>
#include <Schema_Temps_base.h>
#include <EChaine.h>

Implemente_base_sans_constructeur(Milieu_base,"Milieu_base",Objet_U);
// XD milieu_base objet_u milieu_base -1 Basic class for medium (physics properties of medium).
// XD ref gravite field_base

Milieu_base::Milieu_base()
{
  indic_rayo_ = NONRAYO;
  deja_associe=0;
}

// Description:
//    Surcharge Objet_U::printOn
//    Imprime un mileu sur un flot de sortie.
//    Appelle Milieu_base::ecrire
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Milieu_base::printOn(Sortie& os) const
{
  os << "{" << finl;
  ecrire(os);
  os << "}" << finl;
  return os;
}

// Description:
//    Ecrit un objet milieu sur un flot de sortie.
//    Ecrit les caracteristiques du milieu:
//        - masse volumique
//        - conductivite
//        - capacite calorifique
//        - beta_th
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'ecriture
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:  le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
void  Milieu_base::ecrire(Sortie& os) const
{
  os << "rho " << rho<< finl;
  os << "lambda " << lambda << finl;
  os << "Cp " << Cp << finl;
  os << "beta_th " << beta_th << finl;
}

void Milieu_base::discretiser(const Probleme_base& pb, const  Discretisation_base& dis)
{
  Cerr << "Medium discretization." << finl;
  Champ_Don& ch_lambda = conductivite();
  Champ_Don& ch_alpha = diffusivite();
  Champ_Don& ch_beta_th = beta_t();

  const Zone_dis_base& zone_dis=pb.equation(0).zone_dis();
  // PL: pas le temps de faire plus propre, je fais comme dans Fluide_Incompressible::discretiser
  // pour gerer une conductivite lue dans un fichier MED. Test: Reprise_grossier_fin_VEF
  // ToDo: reecrire ces deux methodes discretiser

  // E. Saikali
  // The thermal conductivity and diffusivity fields are considered as
  // multi_scalaire fields, sure if the number of components read
  // in the data file for lambda > 1, i.e: case of anisotropic diffusion !

  int lambda_nb_comp = 0;

  if(ch_lambda.non_nul())
    {
      // Returns number of components of lambda field
      lambda_nb_comp = ch_lambda.nb_comp( );
      if (sub_type(Champ_Fonc_MED,ch_lambda.valeur()))
        {
          double temps=ch_lambda.valeur().temps();
          Cerr<<"Convert Champ_fonc_MED lambda in Champ_Don..."<<finl;
          Champ_Don ch_lambda_prov;
          dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",lambda_nb_comp,temps,ch_lambda_prov);
          ch_lambda_prov.affecter_(ch_lambda.valeur());
          ch_lambda.detach();
          ch_alpha.detach();
          dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",lambda_nb_comp,temps,ch_lambda);
          ch_lambda.valeur().valeurs()=ch_lambda_prov.valeur().valeurs();
        }

      if(lambda_nb_comp >1) // Pour anisotrope
        ch_lambda.valeur().fixer_nature_du_champ(multi_scalaire);

      dis.nommer_completer_champ_physique(zone_dis,"conductivite","W/m/K",ch_lambda.valeur(),pb);

      // le vrai nom sera donne plus tard
      if (sub_type(Champ_Fonc_Tabule,ch_lambda.valeur()))
        {
          double temps=ch_lambda.valeur().temps();
          dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",lambda_nb_comp,temps,ch_alpha);
        }
      champs_compris_.ajoute_champ(ch_lambda.valeur());
    }
  if (!ch_alpha.non_nul()&&(ch_lambda.non_nul()))
    {
      double temps=ch_lambda.valeur().temps();
      // ch_alpha (i.e. diffusivite_thermique) will have same component number as ch_lambda
      dis.discretiser_champ("champ_elem",zone_dis,"neant","neant",lambda_nb_comp,temps,ch_alpha);
    }
  if (ch_alpha.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"diffusivite_thermique","m2/s",ch_alpha.valeur(),pb);
      champs_compris_.ajoute_champ(ch_alpha.valeur());
    }
  if (ch_beta_th.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"dilatabilite","K-1",ch_beta_th.valeur(),pb);
      champs_compris_.ajoute_champ(ch_beta_th.valeur());
    }
  if  (Cp.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"capacite_calorifique","J/kg/K",Cp.valeur(),pb);
      champs_compris_.ajoute_champ(Cp.valeur());
    }
  if  (rho.non_nul())
    {
      dis.nommer_completer_champ_physique(zone_dis,"masse_volumique","kg/m^3",rho.valeur(),pb);
      champs_compris_.ajoute_champ(rho.valeur());
    }
  if (rho.non_nul() && Cp.non_nul())
    {
      assert (rho.nb_comp() == Cp.nb_comp());
      if(!rho_cp_comme_T_.non_nul())
        {
          const double temps = pb.schema_temps().temps_courant();
          // E. Saikali
          // XXX : j'ai remis nb_comp = 1, sinon ca bloque dans Solveur_Masse_base => tab_divide_any_shape
          // parce qu'on a pas line_size % line_size_vx == 0 (cas nb_comp >1 pour rho et cp
          // TODO : FIXME : faut coder un cas generique dans DoubleVect::tab_divide_any_shape... bon courage
          dis.discretiser_champ("temperature", zone_dis, "rho_cp_comme_T", "J/m^3/K", 1 /* rho.nb_comp() */, temps, rho_cp_comme_T_);
          dis.discretiser_champ( "champ_elem", zone_dis,    "rho_cp_elem", "J/m^3/K", 1 /* rho.nb_comp() */, temps,    rho_cp_elem_);
        }
      champs_compris_.ajoute_champ(rho_cp_comme_T_.valeur());
      champs_compris_.ajoute_champ(rho_cp_elem_.valeur());
    }
}
// Description:
//    Lecture d'un milieu sur un flot d'entree.
//    Format:
//    {
//     grandeur_physique type_champ bloc de lecture du champ
//    }
// cf set_param method to know the understood keywords
// cf Param class to know possible options reading
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree pour la lecture des parametres du milieu
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception: accolade ouvrante attendue
// Effets de bord:
// Postcondition: l'objet milieu est construit avec les parametres lus
Entree& Milieu_base::readOn(Entree& is)
{
  Cerr<<"Reading of data for a "<<que_suis_je()<<" medium"<<finl;
  Param param(que_suis_je());
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  creer_champs_non_lus();
  return is;
}

void Milieu_base::set_param(Param& param)
{
  param.ajouter("rho",&rho);
  param.ajouter("lambda",&lambda);
  param.ajouter("Cp",&Cp);
  param.ajouter("beta_th",&beta_th);
}
int Milieu_base::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  Cerr << mot << " is not a keyword understood by " << que_suis_je() << " in lire_motcle_non_standard"<< finl;
  exit();
  return -1;
}

void Milieu_base::verifier_coherence_champs(int& err,Nom& msg)
{
  if (err==1)
    {
      Cerr<<"Error while reading the physical properties of a "<<que_suis_je()<<" medium."<<finl;
      Cerr<<msg<<finl;
      exit();
    }
  else if (err==2)
    {
      Cerr<<"Warning while reading the physical properties of a "<<que_suis_je()<<" medium."<<finl;
      Cerr<<msg<<finl;
    }
  else
    Cerr<<"The physical properties of a "<<que_suis_je()<<" medium have been successfully checked."<<finl;
}

void Milieu_base::preparer_calcul()
{
  int err=0;
  Nom msg;
  verifier_coherence_champs(err,msg);
}

void Milieu_base::creer_champs_non_lus()
{
  if (rho.non_nul() && lambda.non_nul() && Cp.non_nul())
    creer_alpha();
}

// Description:
//    Associe la gravite en controlant dynamiquement le
//    type de l'objet a associer.
//    Si l'objet est du type Champ_Don ou Champ_Don_base
//    l'association reussit, sinon elle echoue.
// Precondition:
// Parametre: Objet_U& ob
//    Signification: un objet TRUST devant representer un champ de gravite
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: int
//    Signification: 1 si l'association a reussie, 0 sinon.
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Milieu_base::associer_(Objet_U& ob)
{
  if (sub_type(Champ_Don_base,ob))
    {
      associer_gravite(ref_cast(Champ_Don_base, ob));
      return 1;
    }
  if (sub_type(Champ_Don,ob))
    {
      associer_gravite(ref_cast(Champ_Don, ob).valeur());
      return 1;
    }
  return 0;
}

// Description:
//    Associe (affecte) un champ de gravite au milieu.
// Precondition:
// Parametre: Champ_Don_base& gravite
//    Signification: champ donne representant la gravite
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Milieu_base::associer_gravite(const Champ_Don_base& la_gravite)
{
  // On verifie que la gravite est de la bonne dimension
  if (Objet_U::dimension!=la_gravite.nb_comp())
    {
      Cerr << "The dimension is " << Objet_U::dimension << " and you create a gravity vector with " << la_gravite.nb_comp() << " components." << finl;
      exit();
    }
  g = la_gravite;
}

// Description:
//    Calcul de alpha=lambda/(rho*Cp).
//    Suivant lambda, rho et Cp alpha peut-etre type comme
//    un champ uniforme ou un champ uniforme par morceau.
// Precondition: Conductivite (lambda) non nulle
// Precondition: Masse volumique (rho) non nulle
// Precondition: Capacite calorifique (Cp) non nulle
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception: violation d'une precondition
// Exception: impossible de calculer alpha car le type des champs
//            lambda,rho,Cp n'est pas compatible ou pas gere.
// Effets de bord:
// Postcondition: la diffusivite (alpha) est calculee
void Milieu_base::calculer_alpha()
{
  if(lambda.non_nul())
    {
      DoubleTab& tabalpha = alpha.valeurs();
      tabalpha = lambda.valeurs();

      // [ABN]: allows variable rho, Cp at this level (will be used by Solide_Milieu_Variable for instance).
      if (sub_type(Champ_Uniforme,rho.valeur()))
        tabalpha /= rho.valeurs()(0,0);
      else
        tab_divide_any_shape(tabalpha,rho.valeurs());

      if (sub_type(Champ_Uniforme,Cp.valeur()))
        tabalpha /= Cp.valeurs()(0,0);
      else
        tab_divide_any_shape(tabalpha,Cp.valeurs());
    }
  else
    {
      Cerr << "alpha calculation is not possible since lambda is not known." << finl;
    }
}
void Milieu_base::mettre_a_jour(double temps)
{
  //Cerr << que_suis_je() << "Milieu_base::mettre_a_jour" << finl;
  if (rho.non_nul())
    rho.mettre_a_jour(temps);
  if (g.non_nul())
    g.valeur().mettre_a_jour(temps);
  if (lambda.non_nul())
    lambda.mettre_a_jour(temps);
  if (Cp.non_nul())
    Cp.mettre_a_jour(temps);
  if (beta_th.non_nul())
    beta_th.mettre_a_jour(temps);

  if ( (lambda.non_nul()) && (Cp.non_nul()) && (rho.non_nul()) )
    {
      calculer_alpha();
      alpha.valeur().changer_temps(temps);
    }
  if (rho_cp_comme_T_.non_nul())
    update_rho_cp(temps);

}

void Milieu_base::update_rho_cp(double temps)
{
  rho_cp_elem_.changer_temps(temps);
  rho_cp_elem_.valeur().changer_temps(temps);
  DoubleTab& rho_cp=rho_cp_elem_.valeurs();
  if (sub_type(Champ_Uniforme,rho.valeur()))
    rho_cp=rho.valeurs()(0,0);
  else
    {
      // AB: rho_cp = rho.valeurs() turns rho_cp into a 2 dimensional array with 1 compo. We want to stay mono-dim:
      rho_cp = 1.;
      tab_multiply_any_shape(rho_cp, rho.valeurs());
    }
  if (sub_type(Champ_Uniforme,Cp.valeur()))
    rho_cp*=Cp.valeurs()(0,0);
  else
    tab_multiply_any_shape(rho_cp,Cp.valeurs());
  rho_cp_comme_T_.changer_temps(temps);
  rho_cp_comme_T_.valeur().changer_temps(temps);
  const MD_Vector& md_som = rho_cp_elem_.zone_dis_base().domaine_dis().domaine().md_vector_sommets(),
                   &md_faces = ref_cast(Zone_VF,rho_cp_elem_.zone_dis_base()).md_vector_faces();
  if (rho_cp_comme_T_.valeurs().get_md_vector() == rho_cp_elem_.valeurs().get_md_vector())
    rho_cp_comme_T_.valeurs() = rho_cp;
  else if (rho_cp_comme_T_.valeurs().get_md_vector() == md_som)
    Discretisation_tools::cells_to_nodes(rho_cp_elem_,rho_cp_comme_T_);
  else if (rho_cp_comme_T_.valeurs().get_md_vector() == md_faces)
    Discretisation_tools::cells_to_faces(rho_cp_elem_,rho_cp_comme_T_);
  else
    {
      Cerr<< que_suis_je()<<(int)__LINE__<<finl;
      Process::exit();
    }
}

void Milieu_base::abortTimeStep()
{
  if (rho.non_nul()) rho->abortTimeStep();
}

bool Milieu_base::initTimeStep(double dt)
{
  return true;
}

void Milieu_base::creer_alpha()
{
  if (Process::je_suis_maitre())
    Cerr << que_suis_je() << "Milieu_base::creer_alpha" << finl;
  assert(lambda.non_nul());
  assert(rho.non_nul());
  assert(Cp.non_nul());
  alpha=lambda;
  alpha->nommer("alpha");
}

// Description:
//    Renvoie la gravite du milieu si elle a ete associe
//    provoque une erreur sinon.
//    (version const)
// Precondition: la gravite doit avoir ete associee
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don_base&
//    Signification: le champ representant la gravite du milieu
//    Contraintes: reference constante
// Exception: pas de gravite associee au milieu
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Champ_Don_base& Milieu_base::gravite() const
{
  if (g.non_nul())
    return g.valeur();
  else
    {
      Cerr << "The gravity has not been associated with the medium" << finl;
      exit();
    }
  return g.valeur(); // pour les compilateurs
}

// Description:
//    Renvoie la gravite du milieu si elle a ete associe
//    provoque une erreur sinon.
// Precondition: la gravite doit avoir ete associee
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don_base&
//    Signification: le champ representant la gravite du milieu
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Don_base& Milieu_base::gravite()
{
  if (g.non_nul())
    return g.valeur();
  else
    {
      Cerr << "The gravity has not been associated with the medium" << finl;
      exit();
    }
  return g.valeur(); // pour les compilateurs
}

int Milieu_base::initialiser(const double temps)
{
  Cerr << que_suis_je() << "Milieu_base:::initialiser" << finl;
  if (sub_type(Champ_Don_base, rho.valeur())) ref_cast(Champ_Don_base, rho.valeur()).initialiser(temps);
  if (g.non_nul())
    g.valeur().initialiser(temps);
  if (lambda.non_nul())
    lambda.initialiser(temps);
  if (Cp.non_nul())
    Cp.initialiser(temps);
  if (beta_th.non_nul())
    beta_th.initialiser(temps);

  if ( (lambda.non_nul()) && (Cp.non_nul()) && (rho.non_nul()) )
    {
      calculer_alpha();
      alpha.valeur().changer_temps(temps);
    }
  if (rho_cp_comme_T_.non_nul())
    update_rho_cp(temps);
  return 1;
}

// Description:
//    Renvoie la masse volumique du milieu.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_base&
//    Signification: le champ donne representant la masse volumique
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Champ& Milieu_base::masse_volumique() const
{
  return rho;
}

// Description:
//    Renvoie la masse volumique du milieu.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_base&
//    Signification: le champ donne representant la masse volumique
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ& Milieu_base::masse_volumique()
{
  return rho;
}

// Description:
//    Renvoie la diffusivite du milieu.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:Champ_Don&
//    Signification: le champ donne representant la diffusivite
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Champ_Don& Milieu_base::diffusivite() const
{
  return alpha;
}

// Description:
//    Renvoie la diffusivite du milieu.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: le champ donne representant la diffusivite
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Don& Milieu_base::diffusivite()
{
  return alpha;
}

// Description:
//    Renvoie la conductivite du milieu.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: le champ donne representant la conductivite
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Champ_Don& Milieu_base::conductivite() const
{
  return lambda;
}

// Description:
//    Renvoie la conductivite du milieu.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: le champ donne representant la conductivite
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Don& Milieu_base::conductivite()
{
  return lambda;
}

// Description:
//    Renvoie la capacite calorifique du milieu.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: le champ donne representant la capacite calorifique
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Champ_Don& Milieu_base::capacite_calorifique() const
{
  return Cp;
}

// Description:
//    Renvoie la capacite calorifique du milieu.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: le champ donne representant la capacite calorifique
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Don& Milieu_base::capacite_calorifique()
{
  return Cp;
}

// Description:
//    Renvoie beta_t du milieu.
//    (version const)
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: le champ donne representant beta_t
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Champ_Don& Milieu_base::beta_t() const
{
  return beta_th;
}

// Description:
//    Renvoie beta_t du milieu.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Champ_Don&
//    Signification: le champ donne representant beta_t
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_Don& Milieu_base::beta_t()
{
  return beta_th;
}

// Description:
//    Renvoie 1 si la gravite a ete initialisee
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: 1 si g.non_nul
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Milieu_base::a_gravite() const
{
  if (g.non_nul())
    {
      return 1;
    }
  return 0;
}

int Milieu_base::is_rayo_semi_transp() const
{
  return 0;
}

int Milieu_base::is_rayo_transp() const
{
  return 0;
}

void Milieu_base::creer_champ(const Motcle& motlu)
{

}

const Champ_base& Milieu_base::get_champ(const Motcle& nom) const
{
  return champs_compris_.get_champ(nom);
}
void Milieu_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<que_suis_je()<<" : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());
}


// Description:
//    Renvoie 0 si le milieu est deja associe a un probleme, 1 sinon
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Milieu_base::est_deja_associe()
{
  if (deja_associe==1)
    return 0;
  deja_associe=1;
  return 1;
}

void Milieu_base::associer_equation(const Equation_base *eqn) const
{
  std::string nom_inco(eqn->inconnue().le_nom().getString());
  // E. Saikali
  // At the initialization step, FT problem can have several equations with same unknown name "concentration"
  if (equation_.count(nom_inco) && eqn->probleme().que_suis_je() != "Probleme_FT_Disc_gen")
    Cerr << que_suis_je() << " multiple equations solve the unknown " << eqn->inconnue().le_nom() << " !" << finl, Process::exit();
  equation_[nom_inco] = eqn;
}

const Equation_base& Milieu_base::equation(const std::string& inco) const
{
  return *equation_.at(inco);
}

void Milieu_base::set_id_composite(const int i)
{
  id_composite = i;
}

void Milieu_base::nommer(const Nom& nom)
{
  nom_ = nom;
}
const Nom& Milieu_base::le_nom() const
{
  return nom_;
}
