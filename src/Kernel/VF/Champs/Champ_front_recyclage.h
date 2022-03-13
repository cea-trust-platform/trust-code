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
//////////////////////////////////////////////////////////////////////////////
//
// File:        Champ_front_recyclage.h
// Directory:   $TRUST_ROOT/src/Kernel/VF/Champs
// Version:     /main/14
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Champ_front_recyclage_included
#define Champ_front_recyclage_included

#include <Ch_front_var_instationnaire_dep.h>
#include <TRUSTTabs.h>
#include <TRUSTArrays.h>
#include <Vect_Parser_U.h>
#include <Noms.h>
#include <Ref_Champ_Inc_base.h>

class Equation_base;
class Milieu_base;
class Zone_dis_base;
class Zone_Cl_dis_base;
class Front_dis_base;

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     classe Champ_front_recyclage
//
//                        delt_dist                                             delt_dist
//        pb1           <---------->             pb2                            <-------->    pb
//                                 ch_fr2                                     ch_fr
//     _____________________         ____________________________               ____________________________
//    |                 |   |       |                            |              |        |                  |
//    |         dom1    |   |       |            dom2            |              |        |     dom          |
//    |         ch1     |   |       |bord2                       |              |bord    |     ch           |
//    |_________________|___|       |____________________________|              |________|__________________|
//                    plan1                                                             plan
//
//                             Fig. 1                                                        Fig. 2
//
//     Cette classe a pour objectif d evaluer les valeurs d un champ_front (ch_fr2) sur le bord d un domaine (bord2)
//     en exploitant les valeurs d un champ 1 (ch1 dit champ evaluateur) evaluees dans un plan (plan1) distant de
//     delt_dist de bord2 (Fig. 1).

//     Les problemes pb2 et pb1, les domaines dom2 et dom1 ainsi que les champs 2 et 1 peuvent etre identiques
//     (pb2=pb1=pb dom2=dom1=dom et ch2=ch1=ch)
//     auquel cas les valeurs du champ front (ch_fr) sur le bord (bord) seront construites
//     a partir des valeurs du champ ch (qui devient le champ evaluateur) calculees dans le plan (plan)
//     distant de delt_dist du bord (Fig. 2).

//     L expression des valeurs attribuees au champ front sur le bord2 (ou bord) ont pour expression :
//     val_ch_fr2(dir) = ampli_moy_imposee(dir)*moyenne_imposee(dir)
//                       + ampli_fluct(dir)*(val_evaluateur(dir)-ampli_moy_recyclee(dir)*moyenne_recyclee(dir))
//
//     val_ch_fr2         :  valeurs prises par le_champ_front ch_fr2 (ou ch_fr)
//     moyenne_imposee    :  moyenne de le_champ_front (peut etre impose analytiquement ou lue dans un fichier)
//     val_evaluateur     :  valeurs du champ_evaluateur dans le plan1 (ou plan) evaluees par interpolation
//     moyenne_recylee    :  moyenne du champ_evaluateur (peut etre evaluee par moyenne surfacique ou provenir d un traitement_particulier)
//     ampli_moy_imposee  :  facteur d amplification de la la moyenne imposee
//     ampli_moy_recyclee :  facteur d amplification de la la moyenne recyclee
//     ampli_fluct        :  facteur d amplification de la fluctuation recyclee
//     dir                :  direction

//     Syntaxe utilisateur :
//     Champ_front_recyclage
//     {
//      pb_champ_evaluateur nom_pb1 nom_inco1 nb_compo1
//      [ moyenne_imposee methode_moy [fichier] nom_fich1 (nom_fich2) ]
//      [ moyenne_recyclee methode_recyc [fichier] nom_fich1 (nom_fich2) ]
//      [ direction_anisotrope direction ]
//      [ distance_plan dist0 dist1 (dist2) ]
//      [ ampli_fluctuation nb_comp ampli_fluc0 ampli_fluc1 (ampli_fluc2) ]
//      [ ampli_moyenne_imposee nb_comp ampli_moy0 ampli_moy1 (ampli_moy2) ]
//      [ ampli_moyenne_recyclee nb_comp ampli_recy0 ampli_recy1 (ampli_recy2) ]
//     }
//
//     methode_moy = 1 (keyword profil)
//                     pour imposer un profil analytique
//     methode_moy = 2 (keyword interpolation) :
//                       lecture dans un fichier et construction d un champ moyen
//                     en realisant une interpolation des donnees lues.
//                     La moyenne est construite pour une direction privilegiee
//                     (direction_anisotrope) et vaut 0 pour les autres directions
//     methode_moy = 3 (keyword connexion_approchee)
//                     lecture dans un fichier et on retient la valeur de la
//                     variable lue par connexion avec le point le plus proche
//                     de la face de bord consideree
//     methode_moy = 4 (keyword connexion_exacte)
//                     lecture dans un fichier geometrie des coordonnees de points
//                     situes dans le plan d evaluation et lecture dans un fichier
//                     distinct des valeurs moyennes. Les valeurs moyennes lues
//                     sont stockees quand la correspondance exacte entre les points
//                     en vis a vis est verifiee.
//     methode_moy = 5 (keyword logarithmique)
//                     construction de la moyenne par une loi de paroi (logarithmique)
//
//
//    methode_moy = 2 et methode_moy = 3 :
//                     un fichier unique a lire contennant positions et valeurs de la variable
//    methode_moy = 4 : deux fichiers a lire : le premier contenant les valeurs de la variable
//                                               et le second contenant les positions
//
//
//    methode_recyc = 1 (keyword surfacique)
//                      moyenne surfacique des valeurs recyclees
//                     (la moyenne est faite sur le bord2 ou l on recupere les valeurs)
//    methode_recyc = 2 (keyword interpolation) :
//                       voir methode_moy = 2
//    methode_recyc = 3 (keyword connexion_approchee)
//                     voir methode_moy = 3
//    methode_recyc = 4 (keyword connexion_exacte)
//                     voir methode_moy = 4
//
//    methode_recyc = 2 et methode_moy = 3 :
//                     un fichier unique a lire contennant positions et valeurs de la variable
//    methode_recyc = 4 : deux fichiers a lire : le premier contenant les valeurs de la variable
//                                                 et le second contenant les positions
//
//
// .SECTION voir aussi
//
//////////////////////////////////////////////////////////////////////////////

class Champ_front_recyclage : public Ch_front_var_instationnaire_dep
{

  Declare_instanciable_sans_constructeur(Champ_front_recyclage);

public:

  Champ_front_recyclage();
  int lire_info_moyenne_imposee(Entree& is);
  int lire_info_moyenne_recyclee(Entree& is);
  void calcul_moyenne_imposee(const DoubleTab& tab,double temps);
  void calcul_moyenne_recyclee(const DoubleTab& tab,double temps);
  void initialiser_moyenne_imposee(DoubleTab& moyenne);
  void initialiser_moyenne_recyclee(DoubleTab& moyenne);
  void associer_champ_evaluateur(const Nom&, const Motcle&);
  int initialiser(double temps, const Champ_Inc_base& inco) override;
  void mettre_a_jour(double temps) override;

  static void get_coord_faces(const Frontiere_dis_base& fr_vf,
                              DoubleTab& coords,
                              const DoubleVect& delt_dist);

  void lire_fichier_format1(DoubleTab& moyenne,
                            const Frontiere_dis_base& fr_vf,
                            const Nom& nom_fich);
  void lire_fichier_format2(DoubleTab& moyenne,
                            const Frontiere_dis_base& fr_vf,
                            const Nom& nom_fich);
  void lire_fichier_format3(DoubleTab& moyenne,
                            const Frontiere_dis_base& fr_vf,
                            const Nom& nom_fich1, const Nom& nom_fich2);
  double UPb(double y,Nom nom_fich);

protected :

  REF(Champ_Inc_base) l_inconnue1;  //Reference au champ inconnu (ch1) qui sert d evaluateur
  //dans le plan ou l on recupere les valeurs

  DoubleVect delt_dist;             //vecteur distance entre bord2 et le plan (plan1)
  //de calcul des valeurs de ch1

  Nom nom_pb1;                      //nom du probleme evaluateur (pb1)

  Motcle nom_inco1;                 //nom du champ inconnu evaluateur (ch1)

  DoubleTab moyenne_imposee_;            //Voir description ci dessus
  DoubleTab moyenne_recyclee_;
  DoubleVect ampli_fluct_;
  DoubleVect ampli_moy_imposee_;
  DoubleVect ampli_moy_recyclee_;

  int methode_moy_impos_;            //methode pour evaluer moyenne_imposee_
  int methode_moy_recycl_;       //methode pour evaluer moyenne_recyclee_

  Nom fich_impos_,fich_recycl_;     //Noms de fichiers eventuellemment utilises
  Nom fich_maillage_;                    //pour evaluer moyenne_imposee_ et moyenne_recyclee_

  int ndir;                            //direction d anisotropie

  VECT(Parser_U) profil_2;             //Parser et expressions pour imposer une moyenne analytique
  Noms fcts_xyz;

  double u_tau,diametre,visco_cin;  //parametres pour imposer une moyenne en profil log

  // Ensemble des points ou il faut evaluer inconnue1 (uniquement des coordonnees
  //  incluses dans le domaine1 local), classees en fonction du processeur a qui
  //  il faut envoyer le resultat de l'evaluation. Donc attention, on n'a pas forcement
  //  egalite entre l'inconnue locale sur la face et l'inconnue distante sur la face si
  //  les maillages surfaciques des frontieres locales et distantes ne sont pas identiques...
  DoubleTabs inconnues1_coords_to_eval_;

  // Pour chaque point ou il faut evaluer inconnue1, indice de l'element dans lequel
  //  se trouve ce point (toujours par processeur destination)
  VECT(ArrOfInt) inconnues1_elems_;

  // A la reception des valeurs, indices des faces de bord ou on doit stocker le
  //  resultat recu de chaque processeur
  VECT(ArrOfInt) inconnues2_faces_;
};

#endif
