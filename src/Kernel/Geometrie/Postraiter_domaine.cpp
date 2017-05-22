/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
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
// File:        Postraiter_domaine.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////

#include <Postraiter_domaine.h>
#include <Sous_Zone.h>
#include <Format_Post.h>
#include <Param.h>
#include <communications.h>

Implemente_instanciable(Postraiter_domaine,"Postraiter_domaine",Interprete_geometrique_base);

// Description:
//    Simple appel a: Interprete::printOn(Sortie&)
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
Sortie& Postraiter_domaine::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

// Description:
//    Simple appel a: Interprete::readOn(Entree&)
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
Entree& Postraiter_domaine::readOn(Entree& is)
{
  return Interprete::readOn(is);
}
void traite_bord(const Zone& zone,IntVect& ch_som,IntVect& ch_elem3,int num2, Faces& faces,Nom& nom_bord,Nom& fichier,const IntTab& les_elems, Format_Post_base& post, int& compteur,int& compteur0,int& compteur_reel,int& moi,int isjoint=0)
{
  const Domaine& dom=zone.domaine();
  Nom nom_dom=dom.le_nom();
  int num=-num2;
  int nb_elem=les_elems.dimension(0);
  int nb_elem_tot=zone.nb_elem_tot();


  int nb_faces=faces.nb_faces();
  int nb_som_faces=faces.nb_som_faces();
  int j,k,som;

  DoubleTab ch_elem2(nb_elem,1);
  ch_elem2=moi;

  if (isjoint!=2)
    {
      // on a pas encore calcule ch_elem3
      ch_elem3=moi;
      for (j=0; j<nb_faces; j++)
        for (k=0; k<nb_som_faces; k++)
          {
            som=faces.sommet(j,k);
            // cas des polyedres
            if (som==-1)  break;
            if (ch_som(som)==0)
              ch_som(som)=num;
            else
              ch_som(som)=(ch_som(som)+num)/2;
          }

      //num=-Process::nproc();
      num=-1;
      if (nb_faces!=0)
        {
          if (0)
            {
              int taille1=les_elems.dimension(1);
              for (int e=0 ; e < nb_elem ; e++)
                {
                  for (j=0; j<nb_faces; j++)
                    {
                      int m=-1;
                      for (k=0; k<nb_som_faces; k++)
                        {
                          som=faces.sommet(j,k);
                          for (int kk=0; kk<taille1; kk++)
                            {
                              if (les_elems(e,kk)==som)
                                {
                                  m++;;
                                  break;
                                }
                            }
                          if (m!=k) break;
                        }
                      if (m==nb_som_faces-1)
                        ch_elem3[e]=num;
                    }
                }
            }
          else
            {
              const IntTab& faces_voisin=faces.voisins();
              if (faces_voisin.dimension(0) > 0 && local_max_vect(faces_voisin)!=-1)
                {
                  Cerr<<" we use faces_voisins"<<finl;
                  for (j=0; j<nb_faces; j++)
                    {
                      int el1=faces_voisin(j,0);
                      if (el1==-1) el1=faces_voisin(j,1);
                      if (el1==-1)
                        {
                          Cerr << "Error, the face " << j << " of the domain " << dom.le_nom() << " has no neighbours !" << finl;
                          Cerr << "Check that the domain " << dom.le_nom() << " has been discretized." << finl;
                          Process::exit();
                        }
                      ch_elem3[el1]=num;
                    }
                }
              else
                {
                  DoubleTab position(1,Objet_U::dimension);
                  IntVect res;
                  DoubleTab xv;
                  faces.associer_zone(zone);
                  faces.calculer_centres_gravite(xv);
                  for (j=0; j<nb_faces; j++)
                    {
                      for (int dir=0; dir<Objet_U::dimension; dir++)
                        position(0,dir)=xv(j,dir);
                      zone.chercher_elements(position,res);
                      if (res[0]<0)
                        {
                          position+=1e-5;
                          zone.chercher_elements(position,res);
                          if (res[0]<0)
                            {
                              position-=2e-5;
                              zone.chercher_elements(position,res);
                            }
                        }

                      if (res[0]<nb_elem)
                        {
                          if (res[0]<0)
                            {
                              if (isjoint==1)
                                Cerr<<"To properly view the joints, the splitting area must be complete "<<finl;
                              else
                                {
                                  Cerr<<"Problem in Postraiter_domaine.cpp with the boundary "<<nom_bord<<finl;
                                  // abort();
                                }
                            }
                          else
                            {
                              ch_elem3[res[0]]=num;
                            }
                        }
                      else
                        {
                          if (nb_elem>=nb_elem_tot) abort();
                        }
                    }
                }
            }
        }
    }

  for (int s=0; s<nb_elem; s++) ch_elem2(s,0)=(ch_elem3[s]);
  // on ajoute ce bord
  Noms noms_post(1);
  noms_post[0]=nom_bord;
  Noms unites(1);
  unites[0]="1";
  Cerr<<Process::me() <<" ch_elem2 size"<<fichier<< " "<<nom_dom<<" " <<nom_bord<<" "<<ch_elem2.dimension(0)<<finl;
  post.ecrire_champ(dom,unites,noms_post,-1,0.,0.,nom_bord, nom_dom, "ELEM","scalar",ch_elem2);

}
// Description:
//    Fonction principale de l'interprete.
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Postraiter_domaine::interpreter_(Entree& is)
{
  Nom nom_pdb;
  Nom un_dom;
  Param param(que_suis_je());
  param.ajouter("format",&format_post_,Param::REQUIRED);
  format_binaire_=1;
  param.ajouter("binaire",&format_binaire_);
  ecrire_frontiere_=1;
  param.ajouter("ecrire_frontiere",&ecrire_frontiere_);
  nom_pdb="NOM_DU_CAS";
  param.ajouter("fichier|file",&nom_pdb);
  joint_non_ecrit_=1;
  param.ajouter("joints_non_postraites",&joint_non_ecrit_); // desactive l'ecriture des joints pratique pour comparer parallele et sequentielle
  param.ajouter("domaine",&un_dom);
  param.ajouter_non_std("domaines",(this));
  param.ajouter_condition("is_read_domaine_or_is_read_domaines","Vous devez preciser domaine ou domaines dans Postraiter_domaine");
  param.ajouter_condition("is_read_domaine_or_is_read_domaines","Interpreter Postraiter_domaine : one of the keywords domaine or domaines must be specified.");
  param.lire_avec_accolades_depuis(is);
  if (un_dom!=Nom())
    {
      associer_domaine(un_dom);
    }
  nb_domaine_=domaines().size();
  if(nb_domaine_==0)
    {
      Cerr << "You must specify one or several domains" << finl;
      exit();
    }
  if (nom_pdb=="NOM_DU_CAS")
    {
      nom_pdb=nom_du_cas();
      nom_pdb+=".";
      nom_pdb+=format_post_;
    }
  if (format_post_=="lata")
    format_post_+="_V2";

  // On deplace la boucle sur les domaines lus
  ecrire(nom_pdb);
  return is;
}
int Postraiter_domaine::lire_motcle_non_standard(const Motcle& motcle, Entree& is)
{
  if (motcle=="domaines")
    {
      Nom mot;
      is>>mot;
      if (mot!="{")
        {
          Cerr<< "we expected { and not "<<mot<<finl;
          exit();
        }
      is>>mot;
      while (mot!="}")
        {
          associer_domaine(mot);
          is>>mot;
        }
      return 1;
    }
  else
    {
      Cerr<<motcle <<" not understood by Postraiter_domaine::lire_motcle_non_standard"<<finl;
      exit();
    }
  return -1;
}

void Postraiter_domaine::ecrire(Nom& nom_pdb)
{
  Nom fichier=nom_pdb.nom_me(Process::me());
  int compteur=-123;
  int compteur0=-123;
  int compteur_reel=0;
  Nom type("Format_Post_");
  type+=format_post_;
  Format_Post  post_typer;
  post_typer.typer_direct(type);
  Format_Post_base& post=ref_cast(Format_Post_base,post_typer.valeur());

  Nom nom_2(nom_pdb);
  Nom format_post_bis(format_post_);
  if (format_post_bis.debute_par("lata"))
    format_post_bis="lata";
  Nom suffix(".");
  suffix+=format_post_bis;
  nom_2.prefix(suffix);
  int est_le_premie_post=0;
  if (format_post_bis!="lata")
    post.initialize_by_default(nom_2);
  else
    post.initialize(nom_2,format_binaire_,"SIMPLE");
  for (int numero_domaine=0; numero_domaine<nb_domaine_; numero_domaine++)
    {
      if (numero_domaine==0)
        est_le_premie_post=1;
      else
        est_le_premie_post=0;
      post.ecrire_entete(0.,0,est_le_premie_post);

      const Domaine& dom=domaine(numero_domaine);
      int reprise = 0;
      double t_init = 0.;
      post.preparer_post(dom.le_nom(),est_le_premie_post,reprise,t_init);
      post.ecrire_domaine(dom, est_le_premie_post);
    }

  post.ecrire_temps(0.);

  for (int numero_domaine=0; numero_domaine<nb_domaine_; numero_domaine++)
    {
      if (numero_domaine==0)
        est_le_premie_post=1;
      else
        est_le_premie_post=0;
      const Domaine& dom=domaine(numero_domaine);

      compteur=-1;
      post.init_ecriture(0.,-1.,est_le_premie_post,dom);

    }
  int moi=Process::me();
  if (joint_non_ecrit_)
    moi=0;

  for (int numero_domaine=0; numero_domaine<nb_domaine_; numero_domaine++)
    {
      Domaine& dom=domaine(numero_domaine);
      Nom nom_dom(dom.le_nom());
      Zone& zone=dom.zone(0);
      const IntTab& les_elems = zone.les_elems();
      IntVect ch_som(zone.nb_som());
      int nb_elem=zone.nb_elem();
      IntVect ch_elem2(nb_elem);
      int num=0;
      // En format LATA V2, les frontieres et joints
      // sont ecrits par defaut depuis la 1.6.6 lors
      // de l'ecriture des domaines si le domaine a ete discretise <=> indice_domaines_frontieres>0
      if (format_post_!="lata_V2" || dom.domaines_frontieres().size()==0)
        {
          if (dom.domaines_frontieres().size()==0) Cerr << "Warning: it could be faster and nicer to discretize your domain before using Postraiter_domaine." << finl;
          //////////////////////////
          // Ecriture des frontieres
          //////////////////////////
          if (ecrire_frontiere_)
            {
              int nglob=1;
              int nb_bords = zone.nb_bords();
              for (int i=0; i<nb_bords; i++)
                {
                  num=i+nglob;
                  Nom nom_fr=zone.bord(i).le_nom();
                  // le maitre envoit le nom de la frontiere
                  // afin que les processeurs traitent les bords dans le meme ordre
                  if (je_suis_maitre())
                    envoyer(nom_fr,0,-1,11);
                  else
                    recevoir(nom_fr,0,me(),11);
                  Nom nom_bord;
                  if (nb_domaine_==1)
                    nom_bord = nom_fr;
                  else
                    nom_bord = zone.domaine().le_nom()+"_"+nom_fr;
                  Faces& faces=zone.frontiere(zone.rang_frontiere(nom_fr)).faces();
                  traite_bord(zone, ch_som,ch_elem2, num, faces, nom_bord, fichier, les_elems, post,compteur,compteur0,compteur_reel,moi);
                }

              nglob+=zone.nb_bords();
              int nb_raccords = zone.nb_raccords();
              for (int i=0; i<nb_raccords; i++)
                {
                  num=i+nglob;
                  Nom nom_fr=zone.raccord(i).valeur().le_nom();
                  if (je_suis_maitre())
                    envoyer(nom_fr,0,-1,11);
                  else
                    recevoir(nom_fr,0,me(),11);
                  Nom nom_bord;
                  if (nb_domaine_==1)
                    nom_bord = Nom("Raccord_")+nom_fr;
                  else
                    nom_bord = zone.domaine().le_nom()+Nom("_Raccord_")+nom_fr;

                  Faces& faces=zone.frontiere(zone.rang_frontiere(nom_fr)).faces();
                  traite_bord(zone, ch_som,ch_elem2, num, faces, nom_bord, fichier, les_elems, post,compteur,compteur0,compteur_reel,moi);
                }

              nglob+=zone.nb_raccords();

              // On ecrit un champ global
              Noms noms_post(1);
              if (nb_domaine_==1)
                noms_post[0] = "Bord";
              else
                noms_post[0] = zone.domaine().le_nom()+"_Bord";
              Noms unites(1);
              unites[0]="1";
              int nb_som=zone.nb_som();
              DoubleTab ch_som2(nb_som,1);
              for (int j=0; j<nb_som; j++)
                ch_som2(j,0)=(ch_som(j));
              post.ecrire_champ(dom,unites,noms_post,-1,0.,0.,noms_post[0], nom_dom, "SOM","scalar",ch_som2);
            }

          //////////////////////
          // Ecriture des joints
          //////////////////////
          if (!joint_non_ecrit_)
            for (int p=0; p<nproc(); p++)
              {
                int nb_joint=zone.nb_joints();
                envoyer_broadcast(nb_joint, p);
                for (int i=0; i<nb_joint; i++)
                  {
                    Nom nom_bord;
                    if (me()==p)
                      {
                        if (nb_domaine_==1)
                          nom_bord="Joint_Proc_";
                        else
                          nom_bord=zone.domaine().le_nom()+"Joint_Proc_";
                        Nom namep(p);
                        nom_bord+=namep;
                        nom_bord+="_avec_Proc_";
                        nom_bord+=Nom(zone.joint(i).PEvoisin());
                      }
                    // Le processeur p envoie nom_bord a tout le monde
                    envoyer_broadcast(nom_bord, p);
                    Faces toto;
                    Cerr<< me()<<" INFO " << p<<" "<<nom_bord<<finl;
                    Faces& faces=(me()==p?zone.joint(i).faces():toto);
                    barrier();
                    traite_bord(zone, ch_som,ch_elem2, num, faces, nom_bord, fichier, les_elems,post,compteur,compteur0,compteur_reel,moi,1);
                    barrier();
                  }

              }
        }
      //////////////////////////
      // Ecriture des sous_zones
      //////////////////////////
      int nb_ss_zones=dom.nb_ss_zones();
      int nb_elem_tot=zone.nb_elem_tot();
      for (int i=0; i<nb_ss_zones; i++)
        {
          const Sous_Zone& la_ss_zone=dom.ss_zone(i);
          int nbpoly=la_ss_zone.nb_elem_tot();
          IntVect ch_elemb2(nb_elem);
          int cell=0;
          for (int pol=0; pol<nbpoly; pol++)
            {
              int elem=la_ss_zone[pol];
              if (elem<nb_elem)
                {
                  ch_elemb2[elem]=(i+1);
                  cell++;
                }
              else if (elem>=nb_elem_tot)
                {
                  Cerr << "This subzone is poorly defined." << finl;
                  Cerr << "It is composed of elements which do not belong" << finl;
                  Cerr << "to the domain!" << finl;
                  Process::exit();
                }
            }
          int subzone_cells=mp_sum(cell);
          Cerr<<"We handle of the subzone " << i <<" who have " << subzone_cells << " cells." << finl;
          Faces bidon;
          Nom nom_post;
          nom_post="Sous_Zone_";
          nom_post+=la_ss_zone.le_nom();
          traite_bord(zone,ch_som,ch_elemb2,1,bidon,nom_post,fichier,les_elems,post,compteur,compteur0,compteur_reel,moi,2);
        }
    }
  int fin=1;
  post.finir(fin);
}
