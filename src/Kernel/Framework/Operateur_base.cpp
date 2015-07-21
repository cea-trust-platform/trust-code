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

Implemente_base_sans_constructeur(Operateur_base,"Operateur_base",Objet_U);

Operateur_base::Operateur_base()
{
  decal_temps=0;
  nb_ss_pas_de_temps=1;
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
  Equation_base& eqn = equation();
  Zone_dis& zdis= eqn.zone_dis();
  Zone_Cl_dis& zcl = eqn.zone_Cl_dis();
  Champ_Inc& inco= eqn.inconnue();
  associer(zdis, zcl, inco);
  return ;
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
  const Probleme_base& pb=equation().probleme();
  const Schema_Temps_base& sch=pb.schema_temps();
  const int& precision=sch.precision_impr();
  Nom nomfichier(out_);
  if (type!="") nomfichier+=(Nom)"_"+type;
  nomfichier+=".out";
  struct stat f;
  // On cree le fichier a la premiere impression avec l'en tete ou si le fichier n'existe pas
  if (stat(nomfichier,&f) || (sch.nb_impr()==1 && !pb.reprise_effectuee()))
    {
      os.ouvrir(nomfichier);
      SFichier& fic=os;
      Nom espace="\t\t";
      Nom tab="\t";
      fic << (Nom)"# Printing on the boundaries of the equation "+equation().que_suis_je()+" of the problem "+equation().probleme().le_nom() << finl;
      fic << "# " << (type=="moment" ? "Moment of " : "") << description() << finl;
      fic << "# Boundary:";
      const Conds_lim& les_cls=equation().zone_Cl_dis().les_conditions_limites();
      if (flux_bords_.nb_dim()!=2)
        {
          Cerr << "flux_bords_ not dimensioned for the operator " << que_suis_je() << finl;
          exit();
        }
      int nb_compo=flux_bords_.dimension(1);
      if (type=="moment" && dimension==2) nb_compo=1;
      for (int num_cl=0; num_cl<les_cls.size(); num_cl++)
        {
          const Frontiere_dis_base& la_fr = les_cls[num_cl].frontiere_dis();
          if (type!="sum" || equation().zone_dis().zone().Bords_a_imprimer_sum().contient(la_fr.le_nom()))
            {
              fic         << (num_cl==0?tab:espace) << la_fr.le_nom();
              if (Objet_U::nom_du_cas()+"_"+equation().probleme().le_nom()+"_Force_pression"!=out_)
                {
                  if (sub_type(Periodique, les_cls[num_cl].valeur()))
                    fic << espace << la_fr.le_nom();
                }
              for (int i=1; i<nb_compo; i++)
                fic << espace;
            }
        }
      if (type!="sum")
        fic << espace << "Total";
      fic << finl << "# Time";

      for (int num_cl=0; num_cl<les_cls.size(); num_cl++)
        {
          Nom ch=espace;
          if (type=="moment")
            {
              if (dimension==2) ch+="Mz";
              else
                {
                  ch+="Mx";
                  ch+=espace+"My";
                  ch+=espace+"Mz";
                }
            }
          else
            {
              if (Objet_U::nom_du_cas()+"_"+equation().probleme().le_nom()+"_Force_pression"!=out_)
                {
                  if (sub_type(Periodique,les_cls[num_cl].valeur()))
                    {
                      if (nb_compo>1) ch+="Fx";
                      if (nb_compo>=2) ch+=espace+espace+"Fy"+espace;
                      if (nb_compo>=3) ch+=espace+"Fz"+espace;
                    }
                  else
                    {
                      if (nb_compo>1) ch+="Fx";
                      if (nb_compo>=2) ch+=espace+"Fy";
                      if (nb_compo>=3) ch+=espace+"Fz";
                    }
                }
              else
                {
                  if (nb_compo>1) ch+="Fx";
                  if (nb_compo>=2) ch+=espace+"Fy";
                  if (nb_compo>=3) ch+=espace+"Fz";
                }
            }
          const Frontiere_dis_base& la_fr = les_cls[num_cl].frontiere_dis();
          if (type!="sum" || equation().zone_dis().zone().Bords_a_imprimer_sum().contient(la_fr.le_nom()))
            fic << ch;
        }

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

void Operateur_base::get_noms_champs_postraitables(Noms& nom,Option opt) const
{
  if (opt==DESCRIPTION)
    Cerr<<que_suis_je()<<" : "<<champs_compris_.liste_noms_compris()<<finl;
  else
    nom.add(champs_compris_.liste_noms_compris());
}

//Calcul du tableau de valeurs d une quantite lie a un operateur pour postraitement
//Options reconnues : "stabilite" pour dt_stab
//                      "flux_bords" pour flux_bords_
//
void Operateur_base::calculer_pour_post(Champ& espace_stockage,const Nom& option, int comp) const
{
  if (Motcle(option)=="flux_bords")
    {
      DoubleTab& es_valeurs = espace_stockage->valeurs();
      es_valeurs = 0.;
      const Zone_Cl_dis_base& zcl_dis = equation().zone_Cl_dis();
      const Zone_dis_base& zdis = equation().zone_dis().valeur();
      int nb_front = zdis.nb_front_Cl();

      if (flux_bords_.nb_dim()==2)
        {
          //Methode de distinction horrible mais evite de dupliquer le codage
          //pour les operateurs VEF et idem pour les operateurs VDF
          //Il faudrait une classe de base pour tous les operateurs VEF et idem en VDF

          if (!zdis.que_suis_je().debute_par("Zone_VDF"))
            {
              for (int n_bord=0; n_bord<nb_front; n_bord++)
                {
                  const Cond_lim& la_cl = zcl_dis.les_conditions_limites(n_bord);
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  int ndeb = le_bord.num_premiere_face();
                  int nfin = ndeb + le_bord.nb_faces();

                  for (int face=ndeb; face<nfin; face++)
                    {
                      es_valeurs(face) =  flux_bords_(face,comp);
                    }
                }
            }
          else
            {
              const Zone_VF& zvf = ref_cast(Zone_VF,zdis);
              const IntTab& face_vois = zvf.face_voisins();
              int elem;
              for (int n_bord=0; n_bord<nb_front; n_bord++)
                {
                  const Cond_lim& la_cl = zcl_dis.les_conditions_limites(n_bord);
                  const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
                  int ndeb = le_bord.num_premiere_face();
                  int nfin = ndeb + le_bord.nb_faces();

                  for (int face=ndeb; face<nfin; face++)
                    {
                      elem = face_vois(face,0);
                      if (elem==-1)
                        elem = face_vois(face,1);
                      es_valeurs(elem) =  flux_bords_(face,comp);
                    }
                }
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
  if (Motcle(option)=="flux_bords")
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

