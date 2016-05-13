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
// File:        Champ_base.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Framework
// Version:     /main/89
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_base.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Dirichlet.h>
#include <Dirichlet_homogene.h>
#include <Symetrie.h>
#include <strings.h>
#include <Zone_VF.h>
#include <Format_Post_base.h>
#include <communications.h>
#include <Schema_Comm.h>
#include <DoubleTrav.h>
#include <IntVects.h>
#include <Discretisation_base.h>
#include <IntList.h>

Implemente_base_sans_constructeur(Champ_base,"Champ_base",Field_base);

// Description:
//    Surcharge Objet_U::printOn(Sortie&)
//    Imprime le nom du champ sur un flot de sortie.
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
Sortie& Champ_base::printOn(Sortie& os) const
{
  return os << le_nom() << finl;
}


// Description:
//    Lecture du nom d'un champ sur un flot d'entree.
//    Format:
//      nom_du_champ
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Champ_base::readOn(Entree& is)
{
  return is >> nom_;
}

// Description:
//    Constructeur par defaut d'un Champ_base.
//    Mets le champ au temps 0, specifie une unite vide,
//    donne le nom "anonyme" au champ et lui donne une nature
//    scalaire.
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
// Postcondition: un champ anonyme a ete cree
Champ_base::Champ_base()
{
  changer_temps(0.);
  fixer_unite(".");
  nommer("anonyme");
  fixer_nature_du_champ(scalaire);
  nb_compo_ = 1; // Par defaut, champ scalaire
}

// Description:
//  Calcule les "valeurs" du champ au point de coordonnees "pos".
//  Dans cette classe de base, l'implementation appelle
//  valeur_aux(const DoubleTab &, DoubleTab &)
// Parametre: DoubleVect&
//    Signification: les coordonnees du point ou evaluer le champ
// Parametre: DoubleVect& valeurs
//    Signification: En entree: doit avoir la bonne taille (nb_comp),
//                   en sortie contient les composantes du champ.
// Retour: reference a "valeurs"
DoubleVect& Champ_base::valeur_a(const DoubleVect& pos, DoubleVect& les_valeurs) const
{
  assert(les_valeurs.size() == nb_comp());
  DoubleTrav values(1,les_valeurs.size());
  int taille=pos.size();
  DoubleTrav pos2(1,taille);
  for (int dir=0; dir<taille; dir++) pos2(0,dir)=pos(dir);
  valeur_aux(pos2,values);
  for (int comp=0; comp<les_valeurs.size(); comp++)
    les_valeurs(comp)=values(0,comp);
  return les_valeurs;
}

// Description:
//    provoque une erreur !
//    doit etre surchargee par les classes derivees
//    non virtuelle pure par commodite de developpement !
//    Renvoie la valeur du champ au point specifie
//    par ses coordonnees, en indiquant que ce point est
//    situe dans un element specifie.
// Precondition:
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& les_valeurs
//    Signification: la valeur du champ au point specifie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: int
//    Signification: l'element dans lequel est situe le point de calcul
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect&
//    Signification: la valeur du champ au point specifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_base::valeur_a_elem(const DoubleVect& ,
                                      DoubleVect& les_valeurs,
                                      int ) const
{
  Cerr << que_suis_je();
  Cerr << "::valeur_a_elem(...) is not coded " << finl ;
  exit();
  return les_valeurs;
}

// Description: Calcule la valeur ponctuelle de la composante "compo" du champ
//    au point de coordonnees pos.
//    Dans la classe de base, l'implementation appelle
//    valeur_a(const DoubleVect &, DoubleVect &)
// Parametre: DoubleVect&
//    Signification: les coordonnees du point de calcul
// Parametre: int
//    Signification: l'index de la composante du champ a calculer
// Retour: double
double Champ_base::valeur_a_compo(const DoubleVect& pos, int compo) const
{
  DoubleVect values(nb_comp());
  valeur_a(pos, values);
  const double x = values[compo];
  return x;
}

// Description:
// provoque une erreur !
// doit etre surchargee par les classes derivees
// non virtuelle pure par commodite de developpement !
double Champ_base::valeur_a_elem_compo(const DoubleVect&, int ,int ) const
{
  Cerr << que_suis_je();
  Cerr << "::valeur_a_elem_compo(...) is not coded " << finl ;
  exit();
  return 0;
}

// Description:
//    Provoque une erreur !
//    Doit etre surchargee par les classes derivees
//    non virtuelle pure par commodite de developpement !
//    Renvoie les valeurs du champ aux points specifies
//    par leurs coordonnees.
// Precondition:
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& les_valeurs
//    Signification: le tableau des valeurs du champ aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ aux points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_base::valeur_aux(const DoubleTab& ,
                                  DoubleTab& les_valeurs) const
{
  Cerr << que_suis_je();
  Cerr << "::valeur_aux(const DoubleTab& ,DoubleTab& ) is not coded " << finl ;
  exit();
  return les_valeurs;
}

DoubleTab& Champ_base::valeur_aux_centres_de_gravite(const DoubleTab& positions,
                                                     DoubleTab& les_valeurs) const
{
  IntVect les_polys(positions.dimension(0));
  int sz = positions.dimension(0);
  for (int i=0; i<sz; i++)
    les_polys(i) = i;

  return valeur_aux_elems(positions, les_polys, les_valeurs);
}

// Description:
//  Idem que valeur_aux(const DoubleTab &, DoubleTab &), mais calcule
//  uniquement la composante compo du champ.
//  Dans l'implementation de champ_base, on appelle
//  valeur_aux(const DoubleTab &, DoubleTab &)
// Parametre: pos
// Signification: le tableau des coordonnees des points de calcul
//  (on ne traite pas l'espace virtuel du tableau)
// Parametre: les_valeurs
// Signification: tableau destination des valeurs a calculer.
//  Le tableau valeurs doit avoir la bonne taille en entree,
//  soit les_valeurs.size() == pos.dimension(0)
// Parametre: compo
// Signification: l'index de la composante du champ a calculer
// Retour: reference au tableau les_valeurs
DoubleVect& Champ_base::valeur_aux_compo(const DoubleTab& pos ,
                                         DoubleVect& les_valeurs,
                                         int compo) const
{
  // Pas optimal mais fonctionne
  int nb_val=pos.dimension(0);
  // Le tableau les_valeurs doit avoir la bonne taille en entree:
  assert(les_valeurs.size() == nb_val);
  DoubleTrav prov(nb_val,nb_comp());
  valeur_aux(pos,prov);
  for (int i=0; i<nb_val; i++)
    les_valeurs(i)=prov(i,compo);
  return les_valeurs;
}

// Description:
//    provoque une erreur !
//    doit etre surchargee par les classes derivees
//    non virtuelle pure par commodite de developpement !
//    Renvoie les valeurs du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de
//    calculs sont situes dans les elements indiques.
// Precondition:
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: IntVect&
//    Signification: le tableau des elements dans lesquels sont
//                   situes les points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& les_valeurs
//    Signification: le tableau des valeurs du champ aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le tableau des valeurs du champ aux points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_base::valeur_aux_elems(const DoubleTab&,
                                        const IntVect& ,
                                        DoubleTab& les_valeurs) const
{
  Cerr << que_suis_je();
  Cerr << "::valeur_aux_elems(...) is not coded " << finl ;
  exit();
  return les_valeurs;
}

// Description:
//    provoque une erreur !
//    doit etre surchargee par les classes derivees
//    non virtuelle pure par commodite de developpement !
//    Renvoie les valeurs d'une composante du champ aux points specifies
//    par leurs coordonnees, en indiquant que les points de
//    calculs sont situes dans les elements indiques.
// Precondition:
// Parametre: DoubleTab&
//    Signification: le tableau des coordonnees des points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: IntVect&
//    Signification: le tableau des elements dans lesquels sont
//                   situes les points de calcul
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleVect& les_valeurs
//    Signification: le tableau des valeurs de la composante du champ
//                   aux points specifies
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Parametre: int
//    Signification: l'index de la composante du champ a calculer
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: DoubleVect&
//    Signification: le tableau des valeurs de la composante du champ
//                   aux points specifies
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleVect& Champ_base::valeur_aux_elems_compo(const DoubleTab&,
                                               const IntVect&,
                                               DoubleVect& les_valeurs,
                                               int ) const
{
  Cerr << que_suis_je();
  Cerr << "::valeur_aux_elems_compo(...) is not coded " << finl ;
  exit();
  return les_valeurs;
}

DoubleTab& Champ_base::valeur_aux_elems_smooth(const DoubleTab&,
                                               const IntVect& ,
                                               DoubleTab& les_valeurs)
{
  Cerr << que_suis_je();
  Cerr << "::valeur_aux_elems_smooth(...) is not coded " << finl ;
  Cerr << "The chsom option of probes does apply for the moment only for the field of type P1NC in VEF " << finl ;
  exit();
  return les_valeurs;
}

DoubleVect& Champ_base::valeur_aux_elems_compo_smooth(const DoubleTab&,
                                                      const IntVect&,
                                                      DoubleVect& les_valeurs,
                                                      int )
{
  Cerr << que_suis_je();
  Cerr << "::valeur_aux_elems_compo_smooth(...) is not coded " << finl ;
  Cerr << "The chsom option of probes does apply for the moment only for the field of type P1NC in VEF " << finl ;
  exit();
  return les_valeurs;
}

// Description:
//    Mise a jour en temps.
//    NE FAIT RIEN (dans la classe de base)
// Precondition:
// Parametre: double
//    Signification: temps de mise a jour
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
// renvoi les valeurs au sommet sommet
DoubleVect& Champ_base::valeur_a_sommet(int sommet, const Domaine& dom, DoubleVect& val) const
{
  DoubleVect position(dimension);
  for(int i=0; i<dimension; i++)
    {
      position(i)=dom.coord(sommet,i);
    }
  return valeur_a(position, val);
}

// Description:
// renvoi la compo eme corrdonne des valeurs a l'element le_poly au sommet sommet
double Champ_base::valeur_a_sommet_compo(int sommet, int le_poly, int compo) const
{
  Cerr << que_suis_je();
  Cerr << "::valeur_a_sommet_compo(...) is not coded " << finl ;
  exit();
  return -1;
}

// Description:
// renvoie les valeurs aux sommets du Domaine dom
DoubleTab& Champ_base::valeur_aux_sommets(const Domaine& dom, DoubleTab& val) const
{
  const DoubleTab& positions=dom.coord_sommets();
  const Zone& zone=dom.zone(0);
  IntVect les_polys(positions.dimension(0));
  zone.chercher_elements(positions, les_polys);
  return valeur_aux_elems(positions, les_polys, val);
}

// Description:
// renvoie la compo eme valeur aux sommets de dom.
DoubleVect& Champ_base::valeur_aux_sommets_compo(const Domaine& dom,
                                                 DoubleVect& val, int compo) const
{
  const DoubleTab& positions=dom.coord_sommets();
  const Zone& zone=dom.zone(0);
  IntVect les_polys(positions.dimension(0));
  zone.chercher_elements(positions, les_polys);
  return valeur_aux_elems_compo(positions, les_polys, val, compo);
}

// Description:
// mettre_a_jour de la classe de base Champ_base :ne fait rien !
void Champ_base::mettre_a_jour(double)
{

}
void Champ_base::abortTimeStep()
{
}
// Description:
//    Affecter un champ dans un autre.
//    Rebvoie le resultat de l'affectation.
// Precondition:
// Parametre: Champ_base& ch
//    Signification: partie droite de l'affectation
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Champ_base&
//    Signification: le resultat de l'affectation (*this)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Champ_base& Champ_base::affecter(const Champ_base& ch)
{
  affecter_(ch);
  valeurs().echange_espace_virtuel();
  return *this;
}

//Factorise le message d erreur pour les champs
//dont la methode affecter n a pas de sens.
void Champ_base::affecter_erreur()
{
  Nom message;
  message ="The method ";
  message += que_suis_je();
  message += "::affecter has no sense.\n";
  message += "TRUST has caused an error and will stop.\nUnexpected error during TRUST calculation.";
  Process::exit(message);
}

// Description:
// cette methode va fixer les unites et le nom des compos
// elle n'est pas const en realite !!!
void Champ_base::corriger_unite_nom_compo()
{
  if(unite_.size() != nb_compo_)
    {
      Noms& unit=ref_cast_non_const(Noms,unite_);
      Nom nom=unite_[0];
      unit.dimensionner_force(nb_compo_);
      for(int compo=0; compo<nb_compo_; compo++)
        unit[compo] = nom;
    }
  int compo_nommees=(nom_compo(0) != Nom());
  if((!compo_nommees)&&(nb_compo_ != 1))
    {
      Noms ext(nb_compo_);
      if (nb_compo_<=3)
        {
          if(!axi)
            {
              ext[0]=le_nom()+"X";
              if (nb_compo_> 1) ext[1]=le_nom()+"Y";
              if (nb_compo_> 2) ext[2]=le_nom()+"Z";
            }
          else
            {
              ext[0]=le_nom()+"R";
              if (nb_compo_> 1) ext[1]=le_nom()+"teta";
              if (nb_compo_> 2) ext[2]=le_nom()+"Z";
            }
        }
      else for (int c=0; c<nb_compo_; c++)
          ext[c]=le_nom()+"_"+Nom(c);
      Champ_base& ch=ref_cast_non_const(Champ_base,*this);
      ch.fixer_noms_compo(ext);
    }
}


int Champ_base::calculer_valeurs_elem_post(DoubleTab& les_valeurs,int nb_elem,Nom& nom_post,const Domaine& dom) const
{
  const Zone& zone=dom.zone(0);
  //nom_post=le_nom();
  Nom nom_dom=dom.le_nom();
  Nom nom_dom_inc= dom.le_nom();
  if(sub_type(Champ_Inc_base, *this) )
    {
      nom_dom_inc=ref_cast(Champ_Inc_base, *this).domaine().le_nom();
    }
  else if(sub_type(Champ_Fonc_base, *this) )
    {
      nom_dom_inc=ref_cast(Champ_Fonc_base, *this).domaine().le_nom();
    }

  int nb_elem_PE = mppartial_sum(nb_elem);

  DoubleTab centres_de_gravites(nb_elem, dimension);

  les_valeurs.resize(nb_elem, nb_compo_);

  if(nom_dom==nom_dom_inc)
    {

      if(sub_type(Champ_Inc_base, *this) )
        {
          const Zone_VF& zvf = ref_cast(Zone_VF,ref_cast(Champ_Inc_base, *this).zone_dis_base());
          // Pour eviter un resize par nb_elem_tot par appel a xp()
          for (int i=0; i<nb_elem; i++)
            for (int k=0; k<dimension ; k++)
              centres_de_gravites(i,k) = zvf.xp(i,k);
        }
      else
        zone.calculer_centres_gravite(centres_de_gravites);

      valeur_aux_centres_de_gravite(centres_de_gravites,les_valeurs);
    }
  else
    {
      zone.calculer_centres_gravite(centres_de_gravites);
      valeur_aux(centres_de_gravites, les_valeurs);
    }


  if((axi) && (nb_compo_==dimension))
    {
      double teta, vR, vT;
      for (int num_elem=0; num_elem<nb_elem; num_elem++)
        {
          teta = centres_de_gravites(num_elem,1);
          vR=les_valeurs(num_elem, 0);
          vT=les_valeurs(num_elem, 1);
          les_valeurs(num_elem, 0) =vR*cos(teta)-vT*sin(teta);
          les_valeurs(num_elem, 1) =vR*sin(teta)+vT*cos(teta);
        }
    }

  if (nb_compo_==1)
    les_valeurs.resize(nb_elem);

  nom_post+= Nom("_elem_");
  nom_post+= nom_dom;
  return nb_elem_PE;

}
int Champ_base::calculer_valeurs_elem_compo_post(DoubleTab& les_valeurs,int ncomp,int nb_elem,Nom& nom_post,const Domaine& dom) const
{
  const Zone& zone=dom.zone(0);
  //nom_post=nom_compo(ncomp);
  Nom nom_dom=dom.le_nom();
  Nom nom_dom_inc= dom.le_nom();
  if(sub_type(Champ_Inc_base, *this) )
    {
      nom_dom_inc=ref_cast(Champ_Inc_base, *this).domaine().le_nom();
    }
  else if(sub_type(Champ_Fonc_base, *this) )
    {
      nom_dom_inc=ref_cast(Champ_Fonc_base, *this).domaine().le_nom();
    }

  int nb_elem_PE = mppartial_sum(nb_elem);

  DoubleTab centres_de_gravites(nb_elem, dimension);
  les_valeurs.resize(nb_elem);
  if(nom_dom==nom_dom_inc)
    {
      if(sub_type(Champ_Inc_base, *this) )
        {
          const Zone_VF& zvf = ref_cast(Zone_VF,ref_cast(Champ_Inc_base, *this).equation().zone_dis().valeur());
          // Pour eviter un resize par nb_elem_tot par appel a xp()
          for (int i=0; i<nb_elem; i++)
            for (int k=0; k<dimension; k++)
              centres_de_gravites(i,k) = zvf.xp(i,k);
        }
      else
        zone.calculer_centres_gravite(centres_de_gravites);

      IntVect les_polys(nb_elem);
      {
        for(int elem=0; elem<nb_elem; elem++)
          {
            les_polys(elem)=elem;
          }
      }
      valeur_aux_elems_compo(centres_de_gravites, les_polys, les_valeurs, ncomp);
    }
  else
    {
      zone.calculer_centres_gravite(centres_de_gravites);
      valeur_aux_compo(centres_de_gravites, les_valeurs, ncomp);
    }
  nom_post+= Nom("_elem_");
  nom_post+= nom_dom;
  //
  return nb_elem_PE;
}

// Ajoute la contribution des autres processeurs a valeurs et compteur
inline void add_sommets_communs(const Domaine& dom, DoubleTab& les_valeurs, IntTab& compteur, IntList& sommets_dirichlet)
{
//  if (Process::nproc()>9) return;
  int nb_compo_ = 0;
  if (les_valeurs.nb_dim()==2)
    nb_compo_ = les_valeurs.dimension(1);

  // On balaie les joints
  const Joints& joints = dom.zone(0).faces_joint();
  const int nb_joints = joints.size();
  for (int i_joint = 0; i_joint < nb_joints; i_joint++)
    {
      const Joint& joint = joints[i_joint];
      const int PEvoisin = joint.PEvoisin();
      const Joint_Items& joint_item = joint.joint_item(Joint::SOMMET);
      const ArrOfInt& sommets_communs = joint_item.items_communs();

      // Tableaux temporaires
      IntTab envoie_sommets;
      DoubleTab envoie_valeurs;
      IntTab envoie_compteur;
      IntTab recoit_sommets;
      DoubleTab recoit_valeurs;
      IntTab recoit_compteur;
      int size=0;
      // On balaie les sommets communs
      int size_sommets_communs = sommets_communs.size_array();
      for (int j=0; j<size_sommets_communs; j++)
        {
          int sommet = sommets_communs(j);
          // Si ce sommet commun appartient a la liste des sommets Dirichlet:
          if (sommets_dirichlet.contient(sommet))
            {
              // Le numero du sommet voisin distant est:
              int sommet_voisin = joint_item.renum_items_communs()(j,0);
              // On verifie que le sommet local est bien sommet:
              assert(joint_item.renum_items_communs()(j,1)==sommet);
              // On redimensionne les tableaux d'envoi et on les remplit
              size++;
              envoie_sommets.resize(size);
              envoie_sommets(size-1) = sommet_voisin;
              if (nb_compo_)
                {
                  envoie_valeurs.resize(size, nb_compo_);
                  envoie_compteur.resize(size, nb_compo_);
                  for(int compo=0; compo<nb_compo_; compo++)
                    {
                      envoie_valeurs(size-1,compo) = les_valeurs(sommet,compo);
                      envoie_compteur(size-1,compo) = compteur(sommet,compo);
                    }
                }
              else
                {
                  envoie_valeurs.resize(size);
                  envoie_compteur.resize(size);
                  envoie_valeurs(size-1) = les_valeurs(sommet);
                  envoie_compteur(size-1) = compteur(sommet);
                }
            }
        }
      if (Process::me()<PEvoisin)
        {
          // On envoie les tableaux a PEvoisin
          envoyer(envoie_sommets,Process::me(),PEvoisin,Process::me()+1000);
          envoyer(envoie_valeurs,Process::me(),PEvoisin,Process::me()+2000);
          envoyer(envoie_compteur,Process::me(),PEvoisin,Process::me()+3000);
          // On recoit les tableaux de PEvoisin
          recevoir(recoit_sommets,PEvoisin,Process::me(),PEvoisin+1000);
          recevoir(recoit_valeurs,PEvoisin,Process::me(),PEvoisin+2000);
          recevoir(recoit_compteur,PEvoisin,Process::me(),PEvoisin+3000);
        }
      else
        {
          // On recoit les tableaux de PEvoisin
          recevoir(recoit_sommets,PEvoisin,Process::me(),PEvoisin+1000);
          recevoir(recoit_valeurs,PEvoisin,Process::me(),PEvoisin+2000);
          recevoir(recoit_compteur,PEvoisin,Process::me(),PEvoisin+3000);
          // On envoie les tableaux a PEvoisin
          envoyer(envoie_sommets,Process::me(),PEvoisin,Process::me()+1000);
          envoyer(envoie_valeurs,Process::me(),PEvoisin,Process::me()+2000);
          envoyer(envoie_compteur,Process::me(),PEvoisin,Process::me()+3000);

        }
      // On ajoute la contribution de PEvoisin aux tableaux valeurs et compteur
      // si les compteurs de part et d'autre ne sont pas nuls
      int recoit_size = recoit_sommets.size_array();
      for (int i=0; i<recoit_size; i++)
        {
          int sommet = recoit_sommets(i);
          if (nb_compo_)
            {
              for(int compo=0; compo<nb_compo_; compo++)
                {
                  // Contribution recu d'un sommet Dirichlet
                  if (recoit_compteur(i,compo))
                    {
                      // Si le sommet local n'est pas Dirichlet, on annulle valeurs
                      if (compteur(sommet,compo)==0) les_valeurs(sommet,compo)=0;
                      les_valeurs(sommet,compo)+=recoit_valeurs(i,compo);
                      compteur(sommet,compo)+=recoit_compteur(i,compo);
                    }
                }
            }
          else
            {
              // Contribution recu d'un sommet Dirichlet
              if (recoit_compteur(i))
                {
                  // Si le sommet local n'est pas Dirichlet, on annulle valeurs
                  if (compteur(sommet)==0) les_valeurs(sommet)=0;
                  les_valeurs(sommet)+=recoit_valeurs(i);
                  compteur(sommet)+=recoit_compteur(i);
                }
            }
        }
    }
}

int Champ_base::calculer_valeurs_som_post(DoubleTab& les_valeurs,int nb_som,Nom& nom_post,const Domaine& dom) const
{
  Nom nom_dom=dom.le_nom();
  Nom nom_dom_inc= dom.le_nom();
  if(sub_type(Champ_Inc_base, *this) )
    {
      nom_dom_inc=ref_cast(Champ_Inc_base, *this).domaine().le_nom();
    }
  else if(sub_type(Champ_Fonc_base, *this) )
    {
      nom_dom_inc=ref_cast(Champ_Fonc_base, *this).domaine().le_nom();
    }
  int nb_som_PE = mppartial_sum(nb_som);

  const DoubleTab& coord_sommets=dom.coord_sommets() ;

  les_valeurs.resize(nb_som, nb_compo_);

  if(nom_dom==nom_dom_inc)
    {
      valeur_aux_sommets(dom,les_valeurs);
    }
  else
    {
      valeur_aux(coord_sommets, les_valeurs);
    }

  // Liste des sommets de bord au contact d'une face de Dirichlet:
  IntList sommets_dirichlet;
  // Prise en compte des conditions aux limites :
  if (sub_type(Champ_Inc_base, *this))
    {
      const Champ_Inc_base& chi=ref_cast(Champ_Inc_base, *this);
      const Equation_base& eqn=chi.equation();
      // GF on ne veut pas prendre en compte les CL en EF
      if (eqn.discretisation().que_suis_je()!="EF")
        if((eqn.inconnue().le_nom() == le_nom())
            && (sub_type(Champ_Inc_base, *this))
            && (dom==(ref_cast(Champ_Inc_base, *this).equation().probleme().domaine() ) ) )
          {
            IntTab compteur(dom.nb_som(),nb_compo_);
            compteur = 0;

            const Zone_Cl_dis& zcl=eqn.zone_Cl_dis();
            int nb_cond_lim=zcl.nb_cond_lim(),num_cl;
            for (num_cl=0; num_cl<nb_cond_lim; num_cl++)
              {
                const Cond_lim& la_cl=zcl.les_conditions_limites(num_cl);
                const Frontiere_dis_base& frontiere_dis=la_cl->frontiere_dis();
                const Frontiere& frontiere=frontiere_dis.frontiere();
                const Faces& faces=frontiere.faces();
                // modif bm: les faces frontieres ont maintenant des faces virtuelles,
                //  il ne faut pas boucler sur les faces virtuelles (ancien code: nb_faces_tot())
                int nb_faces=faces.nb_faces();
                int nb_som_faces=faces.nb_som_faces();
                if(sub_type(Dirichlet, la_cl.valeur()))
                  {
                    const Dirichlet& diri=ref_cast(Dirichlet, la_cl.valeur());
                    for(int num_face=0; num_face<nb_faces ; num_face++)
                      for(int num_som=0; num_som<nb_som_faces; num_som++)
                        {
                          int sommet=faces.sommet(num_face, num_som);
                          sommets_dirichlet.add_if_not(sommet);
                          for(int compo=0; compo<nb_compo_; compo++)
                            {
                              if (compteur(sommet,compo) == 0)
                                les_valeurs(sommet, compo) = 0;

                              les_valeurs(sommet, compo) += diri.val_imp(num_face, compo);
                              compteur(sommet,compo) += 1;
                            }
                        }
                  }
                else if(sub_type(Dirichlet_homogene, la_cl.valeur()))
                  {
                    for(int num_face=0; num_face<nb_faces ; num_face++)
                      for(int num_som=0; num_som<nb_som_faces; num_som++)
                        {
                          int sommet=faces.sommet(num_face, num_som);
                          sommets_dirichlet.add_if_not(sommet);
                          for(int compo=0; compo<nb_compo_; compo++)
                            {
                              if (compteur(sommet,compo) == 0)
                                les_valeurs(sommet, compo) = 0;

                              les_valeurs(sommet, compo) += 0;
                              compteur(sommet,compo) += 1;
                            }
                        }
                  }
                else if(sub_type(Symetrie, la_cl.valeur()))
                  {
                    if(nb_compo_==dimension)
                      {
                        ArrOfDouble normale(dimension);
                        DoubleTab delta(dimension-1,dimension);
                        int nb_faces_tot=faces.nb_faces_tot();
                        for(int num_face=0; num_face<nb_faces_tot ; num_face++)
                          {
                            // calcul de la normale :
                            int sommet0=faces.sommet(num_face, 0);
                            for (int k=1; k<dimension; k++)
                              {
                                int sommet=faces.sommet(num_face, k);
                                for(int compo=0; compo<dimension; compo++)
                                  delta(k-1,compo)=coord_sommets(sommet,compo)-coord_sommets(sommet0,compo);
                              }
                            if(dimension==2)
                              {
                                normale(0)=-delta(0,1);
                                normale(1)=delta(0,0);
                              }
                            else if(dimension==3)
                              {
                                normale(0)=delta(0,1)*delta(1,2) - delta(0,2)*delta(1,1);
                                normale(1)=delta(0,2)*delta(1,0) - delta(0,0)*delta(1,2);
                                normale(2)=delta(0,0)*delta(1,1) - delta(0,1)*delta(1,0);
                              }
                            else
                              {
                                Cerr << "We do not know treating the dimension : " << dimension << finl;
                                exit();
                              }
                            normale *= 1. / norme_array(normale);
                            for(int num_som=0; num_som<nb_som_faces; num_som++)
                              {
                                int sommet=faces.sommet(num_face, num_som);
                                if (sommet<dom.nb_som()) // Sommet reel
                                  {
                                    double psc=0;
                                    for(int k=0; k< dimension; k++)
                                      psc+=normale(k)*les_valeurs(sommet,k);
                                    for(int compo=0; compo<nb_compo_; compo++)
                                      les_valeurs(sommet, compo) -= psc*normale(compo);
                                  }
                              }
                          }
                      }
                  }
              }
            // Ajoute la contribution des autres processeurs a valeurs et compteur
            add_sommets_communs(dom, les_valeurs, compteur, sommets_dirichlet);

            // On termine le calcul de la moyenne pour les cas qui ont ete modifies
            // par une condition de type Dirichlet
            int nb_som_l = dom.nb_som();
            assert (nb_som_l==nb_som_l); // GF je ne suis pas sur de l'assert c'est pour voir
            for (int sommet = 0; sommet<nb_som_l; sommet++)
              for(int compo=0; compo<nb_compo_; compo++)
                if (compteur(sommet,compo) != 0)
                  les_valeurs(sommet,compo) /= compteur(sommet,compo);

          }
    }

  // Prise en compte de l'axi:
  if((axi) && (nb_compo_==dimension))
    {
      double teta, vR, vT;
      for(int sommet=0; sommet<nb_som; sommet++)
        {
          teta = coord_sommets(sommet,1);
          vR=les_valeurs(sommet, 0);
          vT=les_valeurs(sommet, 1);
          les_valeurs(sommet, 0) =vR*cos(teta)-vT*sin(teta);
          les_valeurs(sommet, 1) =vR*sin(teta)+vT*cos(teta);
        }
    }


  if (nb_compo_==1)
    les_valeurs.resize(nb_som);

  nom_post+= Nom("_som_");
  nom_post+= nom_dom;
  return nb_som_PE;
}
int Champ_base::calculer_valeurs_som_compo_post(DoubleTab& les_valeurs,int ncomp,int nb_som,Nom& nom_post,const Domaine& dom,int appliquer_cl) const
{
  Nom nom_dom=dom.le_nom();
  Nom nom_dom_inc= dom.le_nom();
  int nb_som_PE = mppartial_sum(nb_som);
  if(sub_type(Champ_Inc_base, *this) )
    {
      nom_dom_inc=ref_cast(Champ_Inc_base, *this).domaine().le_nom();
    }
  else if(sub_type(Champ_Fonc_base, *this) )
    {
      nom_dom_inc=ref_cast(Champ_Fonc_base, *this).domaine().le_nom();
    }
  const DoubleTab& coord_sommets=dom.coord_sommets() ;
  les_valeurs.resize(nb_som);
  if(nom_dom==nom_dom_inc)
    {
      valeur_aux_sommets_compo(dom,les_valeurs, ncomp);
    }
  else
    {
      valeur_aux_compo(coord_sommets, les_valeurs, ncomp);
    }
  if (appliquer_cl)
    {
      // Liste des sommets de bord au contact d'une face de Dirichlet:
      IntList sommets_dirichlet;
      // Prise en compte des conditions aux limites :
      if (sub_type(Champ_Inc_base, *this))
        {
          const Champ_Inc_base& chi=ref_cast(Champ_Inc_base, *this);
          const Equation_base& eqn=chi.equation();
          if((eqn.inconnue().le_nom() == le_nom())
              && (sub_type(Champ_Inc_base, *this))
              && (dom==(ref_cast(Champ_Inc_base, *this).equation().probleme().domaine() ) ) )
            {

              IntTab compteur(dom.nb_som());
              compteur = 0;
              int num_cl;
              const Zone_Cl_dis& zcl=eqn.zone_Cl_dis();
              int nb_cond_lim=zcl.nb_cond_lim();
              for (num_cl=0; num_cl<nb_cond_lim; num_cl++)
                {
                  const Cond_lim& la_cl=zcl.les_conditions_limites(num_cl);
                  const Frontiere_dis_base& frontiere_dis=la_cl->frontiere_dis();
                  const Frontiere& frontiere=frontiere_dis.frontiere();
                  const Faces& faces=frontiere.faces();
                  int nb_faces=faces.nb_faces_tot();
                  int nb_som_faces=faces.nb_som_faces();

                  if(sub_type(Dirichlet, la_cl.valeur()))
                    {
                      const Dirichlet& diri=ref_cast(Dirichlet, la_cl.valeur());
                      for(int num_face=0; num_face<nb_faces ; num_face++)
                        {
                          for(int num_som=0; num_som<nb_som_faces; num_som++)
                            {
                              int sommet=faces.sommet(num_face, num_som);
                              sommets_dirichlet.add_if_not(sommet);
                              if (compteur(sommet) == 0)
                                les_valeurs(sommet) = 0;

                              les_valeurs(sommet) += diri.val_imp(num_face,ncomp);
                              compteur(sommet) += 1;
                            }
                        }
                    }
                  else if(sub_type(Dirichlet_homogene, la_cl.valeur()))
                    {
                      const Dirichlet_homogene& diri=ref_cast(Dirichlet_homogene, la_cl.valeur());
                      for(int num_face=0; num_face<nb_faces ; num_face++)
                        {
                          for(int num_som=0; num_som<nb_som_faces; num_som++)
                            {
                              int sommet=faces.sommet(num_face, num_som);
                              sommets_dirichlet.add_if_not(sommet);
                              les_valeurs(sommet) = diri.val_imp(num_face,ncomp);
                              compteur(sommet) += 1;
                            }
                        }
                    }
                }
              // Ajoute la contribution des autres processeurs a les_valeurs et compteur
              add_sommets_communs(dom, les_valeurs, compteur, sommets_dirichlet);

              // On termine le calcul de la moyenne pour les cas qui ont ete modifies
              // par une condition de type Dirichlet
              int nb_som_l = dom.nb_som();
              assert (nb_som==nb_som_l); // GF je ne suis pas sur de l'assert c'est pour voir
              for (int sommet = 0; sommet<nb_som_l; sommet++)
                if (compteur(sommet) != 0)
                  les_valeurs(sommet) /= compteur(sommet);
            }
        }
    }
  nom_post+= Nom("_som_");
  nom_post+= nom_dom;
  return  nb_som_PE;
}


int Champ_base::completer_post_champ(const Domaine& dom,const int is_axi,const Nom& loc_post,
                                     const Nom& le_nom_champ_post,Format_Post_base& format) const
{

  const Nature_du_champ& la_nature = nature_du_champ();
  const int nb_compo = nb_comp();
  const Noms& noms_composante = noms_compo();

  format.completer_post(dom,is_axi,la_nature,nb_compo,noms_composante,loc_post,le_nom_champ_post);

  return 1;
}

//Fixe les conditions de prise en compte des conditions limites
//pour les operations de filtrage-postraitement du champ
void Champ_base::completer(const Zone_Cl_dis_base& zcl)
{

}

// Description:
//    Donne un nom au champ
// Precondition:
// Parametre: Nom& name
//    Signification: le nom a donner au champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Champ_base::nommer(const Nom& name)
{
  nom_ = name ;
}

// Description:
//    Renvoie le nom du champ.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Champ_base::le_nom() const
{
  return nom_;
}

// Description:
//    Fixe le nombre de composantes du champ.
//    Le champ est de vectoriel si il est de meme dimension
//    que l'espace.
// Precondition:
// Parametre: int i
//    Signification: le nombre de composantes du champs
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de composantes du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la nature du champ peut changer, il peut devenir vectoriel
//                si il a la bonne dimension
int Champ_base::fixer_nb_comp(int i)
{
  // Interdiction de changer la nature du champ une fois
  // le nb_valeurs_nodales fixe.
  nb_compo_ = i ;
  noms_compo_.dimensionner(i);
  if(i==dimension)
    fixer_nature_du_champ(vectoriel);
  return i;
}

// Description:
//    Renvoie le nombre de composantes du champ.
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: int
//    Signification: le nombre de composantes du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
int Champ_base::nb_comp() const
{
  return nb_compo_ ;
}

// Description:
//    Fixe le temps auquel se situe le champ
// Precondition:
// Parametre: double& t
//    Signification:  le nouveau temps auquel se situe le champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: double
//    Signification: le nouveau temps auquel se situe le champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le temps du champ a ete change
double Champ_base::changer_temps(const double& t)
{
  return temps_ = t ;
}

// Description:
//    Renvoie le temps du champ
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: double
//    Signification: le temps du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
double Champ_base::temps() const
{
  return temps_ ;
}

// Description:
//    Specifie les unites des composantes du champ.
//    Ces unites sont specifiees grace a un tableau de Nom
//    et peuvent etre differentes pour chaque composante
//    du champ.
// Precondition:
// Parametre: Noms& noms
//    Signification: les noms des unites des composantes du champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Noms&
//    Signification: les noms des unites des composantes du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la nature du champ est changee: le champ est multiscalaire,
const Noms& Champ_base::fixer_unites(const Noms& noms)
{
  unite_.dimensionner_force(nb_comp());
  fixer_nature_du_champ(multi_scalaire);
  return unite_=noms;
}

// Description:
//    Renvoie les unites des composantes du champ
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Noms&
//    Signification: les noms des unites des composantes du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Noms& Champ_base::unites() const
{
  return unite_;
}

// Description:
//    Specifie l'unite de la i-eme composante du champ
// Precondition:
// Parametre: int i
//    Signification: l'index de la composante du champ dont on veut
//                   specifier l'unite
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Parametre: Nom& nom
//    Signification: le type de l'unite a specifier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Nom&
//    Signification: le type de l'unite de la i-eme composante du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: le champ est de nature multi-scalaire.
const Nom& Champ_base::fixer_unite(int i, const Nom& nom)
{
  fixer_nature_du_champ(multi_scalaire);
  return unite_[i]=nom;
}

// Description:
//    Renvoie l'unite de la i-eme composante du champ
// Precondition:
// Parametre: int i
//    Signification: l'index de la composante du champ dont on veut
//                   connaitre l'unite
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: l'unite de la i-eme composante du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Champ_base::unite(int i) const
{
  return unite_[i];
}

// Description:
//    Specifie l'unite d'un champ scalaire ou dont toutes les
//    composantes ont la meme unite
// Precondition:
// Parametre: Nom& nom
//    Signification: l'unite a specifier
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Nom&
//    Signification: l'unite du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la dimension du tableau stockant les unites est force a 1
const Nom& Champ_base::fixer_unite(const Nom& nom)
{
  unite_.dimensionner_force(1);
  return unite_[0]=nom;
}

// Description:
//    Renvoie l'unite d'un champ scalaire dont toutes les
//    composantes ont la meme unite
// Precondition: toutes les composantes du champ doivent avoir
//               la meme unite
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: l'unite (commune) des composantes du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Champ_base::unite() const
{
  assert(unite_.size() == 1);
  return unite_[0];
}

// Description:
//    Fixe le nom des composantes du champ
// Precondition:
// Parametre: Noms& noms
//    Signification: le tableau des noms a donner aux composantes du champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Noms&
//    Signification: le tableau des noms des composantes du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition:
const Noms& Champ_base::fixer_noms_compo(const Noms& noms)
{
  return noms_compo_=noms;
}

// Description:
//    Renvoie le tableau des noms des composantes du champ
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Noms&
//    Signification: les noms des composantes du champ
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Noms& Champ_base::noms_compo() const
{
  return noms_compo_;
}

// Description:
//    Fixe le nom de la i-eme composante du champ
// Precondition:
// Parametre: int i
//    Signification: l'index de la composante du champ dont on veut
//                   specifier le nom
//    Valeurs par defaut:
//    Contraintes: i < nombre de composantes du champ
//    Acces:
// Parametre: Nom& nom
//    Signification: le nom a donner a la i-eme composante du champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Nom&
//    Signification: le nom de i-eme composante du champ
//    Contraintes: reference constante
// Exception: index de la composante du champ invalide
// Effets de bord:
// Postcondition:
const Nom& Champ_base::fixer_nom_compo(int i, const Nom& nom)
{
  assert(i<nb_comp());
  return noms_compo_[i]=nom;
}

// Description:
//    Renvoie le nom de la ieme composante du champ
// Precondition:
// Parametre: int i
//    Signification: l'index de la composante du champ dont on veut
//                   specifier le nom
//    Valeurs par defaut:
//    Contraintes: i < nombre de composantes du champ
//    Acces:
// Retour: Nom& nom
//    Signification: le nom de i-eme composante du champ
//    Contraintes: reference constante
// Exception: index de la composante du champ invalide
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Champ_base::nom_compo(int i) const
{
  assert(i<nb_comp());
  return noms_compo_[i];
}

// Description:
//    Fixe le nom d'un champ scalaire
// Precondition: le champ doit etre scalaire (avoir 1 seule composante)
// Parametre: Nom& nom
//    Signification: le nom a donner au champ (scalaire)
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour: Nom&
//    Signification: le nom du champ scalaire
//    Contraintes: reference constante
// Exception: le champ n'est pas scalaire
// Effets de bord:
// Postcondition:
const Nom& Champ_base::fixer_nom_compo(const Nom& nom)
{
  assert(nb_comp()==1);
  nommer(nom);
  noms_compo_.dimensionner_force(1);
  return noms_compo_[0]=nom;
}

// Description:
//    Renvoie le nom d'un champ scalaire
// Precondition: le champ doit etre scalaire
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nom&
//    Signification: le nom du champ scalaire
//    Contraintes: reference constante
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
const Nom& Champ_base::nom_compo() const
{
  assert(nb_comp()==1);
  return le_nom();
}

// Description:
//    Fixer la nature d'un champ: scalaire, multiscalaire, vectoriel.
//    Le type (enumere) Nature_du_champ est defini dans Ch_base.h.
// Precondition:
// Parametre: Nature_du_champ n
//    Signification: la nature a donner au champ
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Nature_du_champ
//    Signification: la nature du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: le membre Champ_base::nature est modifie
Nature_du_champ Champ_base::fixer_nature_du_champ(Nature_du_champ n)
{
  return nature = n;
}
// Description:
//    Renvoie la nature d'un champ (scalaire, multiscalaire, vectoriel).
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Nature_du_champ
//    Signification: la nature du champ
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Nature_du_champ Champ_base::nature_du_champ() const
{
  return nature;
}

int Champ_base::fixer_nb_valeurs_nodales(int n)
{
  Cerr << "Champ_base::fixer_nb_valeurs_nodales\n ";
  Cerr << que_suis_je() << " does not have nb_valeurs_nodales" << finl;
  exit();
  return 0;
}

// Tous les champs n'ont pas de zone_dis_base. Pour ceux qui
// n'en ont pas, l'appel est invalide.
void Champ_base::associer_zone_dis_base(const Zone_dis_base& zone_dis)
{
  Cerr << "Error in Champ_base::associer_zone_dis_base\n";
  Cerr << " (field name : " << le_nom() << ")\n";
  Cerr << " The method " << que_suis_je();
  Cerr << "::associer_zone_dis_base is not coded\n";
  Cerr << " or the field does not possess a zone_dis_base." << finl;
  exit();
}

// Tous les champs n'ont pas de zone_dis_base. Pour ceux qui
// n'en ont pas, l'appel est invalide.
const Zone_dis_base& Champ_base::zone_dis_base() const
{
  Cerr << "Error in Champ_base::zone_dis_base\n";
  Cerr << " (field name : " << le_nom() << ")\n";
  Cerr << " The method " << que_suis_je();
  Cerr << "::zone_dis_base is not coded\n";
  Cerr << " or the field does not possess a zone_dis_base." << finl;
  exit();
  throw;
  return zone_dis_base();
}

// Description:
//    Calcule la trace d'un champ sur une frontiere au temps du champ
// Precondition: x doit etre dimensionne
// Parametre: Frontiere_dis_base&
//    Signification: frontiere discretisee sur laquelle on veut
//                   calculer la trace du champ
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& x
//    Signification: les valeurs du champ sur la frontiere
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: les valeurs du champ sur la frontiere
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_base::trace(const Frontiere_dis_base& fr, DoubleTab& x) const
{
  return trace(fr,x,temps_);
}

// Description:
//    Calcule la trace d'un champ sur une frontiere au temps tps
//
//    WEC :
//    La frontiere passee en parametre doit faire partie du domaine
//    sur lequel s'appuie le champ
//    Le resultat est calcule sur cette frontiere et la taille du
//    DoubleTab x correspond au nombre de faces de la frontiere.
//    x peut avoir un espace virtuel, la fonction trace apelle
//    echange_espace_virtuel.
//
//    Cas particulier (malheureusement) du Champ_P0_VDF :
//    Si la frontiere est un raccord, le resultat est calcule sur le
//    raccord associe. Dans ce cas, le DoubleTab x doit etre
//    dimensionne sur le raccord associe.
//
// Precondition: x doit etre dimensionne
// Parametre: Frontiere_dis_base&
//    Signification: frontiere discretisee sur laquelle on veut
//                   calculer la trace du champ au temps tps
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: DoubleTab& x , double tps
//    Signification: les valeurs du champ sur la frontiere au temps tps
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: les valeurs du champ sur la frontiere au temps tps
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
DoubleTab& Champ_base::trace(const Frontiere_dis_base& , DoubleTab& x , double tps) const
{
  Cerr << que_suis_je() << "did not overloaded Champ_base::trace" << finl;
  exit();
  return x;
}


