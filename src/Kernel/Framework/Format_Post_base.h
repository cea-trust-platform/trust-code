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
#ifndef Format_Post_base_included
#define Format_Post_base_included

#include <TRUSTTabs_forward.h>
#include <Ref_Zone_dis_base.h>
#include <Champ_base.h>
#include <Domaine.h>

class Motcle;
class Zone_dis;
class Param;

//.DESCRIPTION
// Classe de base des formats de postraitements pour les champs
// (lata, med, lml, meshtv).
// Utilisation de la classe par l'interface generique:
// -typer un objet format de postraitement
// -initialiser l objet format de postraitement
// -proceder a des operations de complement et verification
// -ecrire un domaine
// -ecrire_temps
// -ecrire_champ (un type de champ supporte: aux elements ou aux sommets,
//                ou encore aux faces si on a ecrit les faces du domaine
//                et si les faces sont supportees par le postraitement)

// Les parametres intervenant (eventuellement) dans une operation de postraitement
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Nom                    nom_fichier                 : nom du fichier sur lequel on ecrit
// int           champ                  : 1 si postraitement de champ, 0 sinon
// int            stat                          : 1 si postraitement de statistiques, 0 sinon
// double            dt_ch                  : periode de postraitement des champs
// double            dt_stat                  : periode de postraitement des statistiques
// int            reprise                : 1 si reprise effectuee, 0 sinon
// int            axi                        : 1 si calcul axi, 0 sinon
// int            est_le_premier_post         : 1 si premier postraitement pour un fichier d ecriture donne, 0 sinon
// int            est_le_dernier_post  : 1 si dernier postraitement pour un fichier d ecriture donne, 0 sinon
// Domaine            dom                        : domaine de calcul
// Nom                    id_domaine                : nom du domaine
// Nom                   id_zone                : nom de la zone
// IntVect           faces_som                : connectivites faces-sommets
// IntVect           elem_faces                : connectivites elements-faces
// int           nb_som                : nombre de sommets du domaine dom
// int            nb_faces                : nombre de faces de la zone

// double            t_init                : temps initial du calcul
// double            temps_courant          : temps courant du calcul
// double           temps_champ                : temps du champ cible
// double            temps_post                : temps de l objet postraitement (ecriture si temps_post<temps_courant)

// Nom                    id_champ_post        : identifiant du champ generique (nom ou composante)
// Nature_du_champ nature_champ                : scalaire, multi_scalaire, vectoriel
// int            nb_compo                : nombre de composantes du champ
// Noms            nom_compos                : vecteur contenant le nom des composantes du champ
// Noms            unites                : vecteur contenant les unites du champ a ecrire
// Motcle            loc_post                : localisation : ELEM, SOM, FACES
// int            ncomp                : numero de composante (-1 si ce n est pas une composante)
// DoubleTab           data                        : tableau de valeurs a ecrire sur fichier
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Rq: data
// Si on postraite le champ et non pas une composante d un champ
//        Si le tableau de valeurs du champ est a une seule entree, data doit etre dimensionne en data(nb_ddl,1)
// Si on postraite une composante d un champ
//        Si le tableau de valeurs fourni est a plusieurs entrees,  data doit etre dimensionne en data(nb_ddl)


//Description de l utilisation de l interface generique pour realiser une operation de postraitement
//L enchainement d appel des methodes de l interface pour postraiter un tableau de valeurs d un champ discret a un instant donne
//est presente ci-dessous (toute methode n est pas forcement necessaire en fonction du format utilise)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// format_post.initialize_by_default(nom_fichier)
// format_post.test_coherence(champ,stat,dt_ch,dt_stat)
// format_post.ecrire_entete(temps_courant,reprise,est_le_premier_post)
// format_post.preparer_post(id_domaine,est_le_premier_post,reprise,t_init)
// format_post.completer_post(dom,axi,nature_champ,nb_compo,noms_compo,loc_post,id_champ_post)
// format_post.ecrire_domaine(dom,est_le_premier_post)
// format_post.ecrire_item_int("FACES",id_domaine,id_zone,"FACES","SOMMETS",faces_som,nb_som);
// format_post.ecrire_item_int("ELEM_FACES",id_domaine,id_zone,"ELEMENTS","FACES",elem_faces,nb_faces);
// format_post.ecrire_temps(temps_courant)
// format_post.init_ecriture(temps_courant,temps_post,est_le_premier_post,dom)
// format_post.ecrire_champ(dom,unites,noms_compo,ncomp,temps_champ,temps_courant,id_champ_post,id_domaine,loc_post,data)
// format_post.finir_ecriture(temps_courant)
// format_post.finir(est_le_dernier_post)
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Format_Post_base : public Objet_U
{
  Declare_base(Format_Post_base);
public:
  // Remet l'objet dans l'etat initial obtenu apres constructeur par defaut
  virtual void reset() = 0;
  virtual void set_param(Param& param)=0;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  virtual int initialize_by_default(const Nom& file_basename);
  virtual int initialize(const Nom& file_basename, const int format, const Nom& option_para);
  virtual int modify_file_basename(const Nom file_basename, const int a_faire, const double tinit);
  virtual int ecrire_entete(const double temps_courant, const int reprise, const int est_le_premier_post);
  virtual int completer_post(const Domaine& dom, const int axi, const Nature_du_champ& nature, const int nb_compo, const Noms& noms_compo, const Motcle& loc_post, const Nom& le_nom_champ_post);

  virtual int preparer_post(const Nom& id_du_domaine, const int est_le_premier_post, const int reprise, const double t_init);
  virtual int test_coherence(const int champs, const int stat, const double dt_ch, const double dt_stat);

  virtual int init_ecriture(double temps_courant, double temps_post, int est_le_premier_postraitement_pour_nom_fich_, const Domaine& domaine);

  virtual int finir_ecriture(double temps_courant);
  virtual int finir(int& est_le_dernier_post);

  virtual int ecrire_domaine(const Domaine& domaine,const int est_le_premier_post);
  virtual int ecrire_domaine_dis(const Domaine& domaine,const REF(Zone_dis_base)& zone_dis_base,const int est_le_premier_post);
  virtual int ecrire_temps(const double temps);

  virtual int ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo, int ncomp, double temps_, double temps_courant, const Nom& id_du_champ, const Nom& id_du_domaine,
                           const Nom& localisation, const Nom& nature, const DoubleTab& data);

  virtual int ecrire_champ2(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo, int ncomp, double temps_, double temps_courant, const Nom& id_du_champ, const Nom& id_du_domaine,
                            const Nom& localisation, const Nom& nature, const DoubleTab& data, const DoubleTab& coord);

  virtual int ecrire_item_int(const Nom& id_item, const Nom& id_du_domaine, const Nom& id_zone, const Nom& localisation, const Nom& reference, const IntVect& data, const int reference_size);

  //Methode qui postraite les valeurs d un champ discret a un instant donne
  void postraiter_debug_valeurs_un_instant(const Nom& nom_fich, const Domaine& dom, const Zone_dis_base& zone_dis, double temps_champ, double temps_courant, const Nom& id_champ_post,
                                           const Nature_du_champ& nature_champ, const Noms& noms_compos, const Noms& unites, const Motcle& loc_post, const DoubleTab& data, int ncomp = -1, int axi = 0,
                                           int champ = 1, int stat = 0, double dt_ch = 1., double dt_stat = 1., int reprise = 0, int est_le_premier_post = 1, int est_le_dernier_post = 1,
                                           double t_init = 0., double temps_post = -2);


  //Actuellement on commente ces methodes car pas decrites dans l interface commune
  //aux differentes classes dans le CDC.
  /*
    virtual int ecrire_bords(const Nom    & id_du_domaine,
    const Motcle & type_faces,
    const DoubleTab & sommets,
    const IntTab & faces_sommets,
    const IntTab & faces_num_bord,
    const Noms   & bords_nom);

    virtual int ecrire_noms(const Nom  & id_de_la_liste,
    const Nom  & id_du_domaine,
    const Nom  & localisation,
    const Noms & liste_noms);

  */

};
#endif
