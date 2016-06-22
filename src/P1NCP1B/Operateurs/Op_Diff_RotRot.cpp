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
// File:        Op_Diff_RotRot.cpp
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_RotRot.h>
#include <Zone_Cl_VEF.h>
#include <Front_VF.h>
#include <Les_Cl.h>
#include <Zone_dis.h>
#include <Zone_Cl_dis.h>
#include <Matrice_Morse_Sym.h>
#include <Debog.h>
#include <SFichier.h>
#include <LecFicDistribueBin.h>
#include <Champ_Uniforme.h>
#include <Solv_GCP.h>
#include <SSOR.h>

Implemente_instanciable(Op_Diff_RotRot,"Op_Diff_VEF_ROTROT_const_P1NC",Operateur_Diff_base);

//// Implementation de printOn et readOn
// ATTENTION: verifier ou est implementer la fonction que_suis_je()
Sortie& Op_Diff_RotRot::printOn(Sortie& s) const
{
  return s << que_suis_je() ;
}


Entree& Op_Diff_RotRot::readOn(Entree& s)
{
  return s ;
}

const Zone_VEF_PreP1b& Op_Diff_RotRot::zone_Vef() const
{
  return ref_cast(Zone_VEF_PreP1b, la_zone_vef.valeur());
}

void Op_Diff_RotRot::
associer(const Zone_dis& zone_dis, const Zone_Cl_dis& zone_Cl_dis,
         const Champ_Inc& inco)
{
  const Zone_VEF& zvef = ref_cast(Zone_VEF, zone_dis.valeur());
  const Zone_Cl_VEF& zclvef = ref_cast(Zone_Cl_VEF, zone_Cl_dis.valeur());
  la_zone_vef = zvef;
  la_zcl_vef = zclvef;

  curl_.associer(zone_dis,zone_Cl_dis,inco);
  rot_.associer(zone_dis,zone_Cl_dis,inco);

  //////////////////////////////////////////////
  //On definit le champ vorticite
  vorticite_.typer("Champ_P1_isoP1Bulle");
  Champ_P1_isoP1Bulle& vorticite =
    ref_cast(Champ_P1_isoP1Bulle,vorticite_.valeur());

  vorticite.associer_zone_dis_base(zvef);
  vorticite.nommer("vorticite");

  if (dimension == 2) vorticite.fixer_nb_comp(1);
  else vorticite.fixer_nb_comp(dimension);

  int nb_tot = zvef.nb_elem() + zvef.nb_som() -1;
  vorticite.fixer_nb_valeurs_nodales(nb_tot);

  vorticite.fixer_unite("s-1");

  //////////////////////////////////////////////////

  solveur_.typer("Solv_GCP");
  Precond p;
  p.typer("SSOR");
  ref_cast(Solv_GCP,solveur.valeur()).set_precond(p);
  assembler_matrice(matrice_vorticite_);
  tester();

}



DoubleTab& Op_Diff_RotRot::
calculer(const DoubleTab& vitesse, DoubleTab& diffusion) const
{
  diffusion = 0;
  return ajouter(vitesse,diffusion);
}

//Methode de l'operateur de diffusion sous forme
//rotationnel de la vorticite sans const la seule qui
//doit etre appelee.
DoubleTab& Op_Diff_RotRot::
ajouter(const DoubleTab& vitesse, DoubleTab& diffusion) const
{
  Cerr << "je suis dans OpDiffRotRot" << finl;
  DoubleTab curl(matrice_vorticite_.valeur().ordre());

  //Marche car Champ_Inc est mutable
  curl_.calculer(vitesse,curl);
  //curl=-1*curl;
  calculer_vorticite(vorticite_.valeurs(),curl);
  rot_.calculer(vorticite_.valeurs(),diffusion);

  Cerr << "je sors de OpDiffRotRot" << finl;

  return diffusion;

}

int Op_Diff_RotRot::
calculer_vorticite(DoubleTab& solution,        const DoubleTab& curl) const
{
  const Zone& zone = zone_Vef().zone();
  static int nb_appel2=0;

  // Resolution en vorticite: on ne considere que le cas
  // sequentiel pour le moment.
  // Pour le cas parallele, il faut s'inspirer de la classe N_S.cpp.

  if (Process::nproc()==1)
    {
      //On considere que la matrice de vorticite est une matrice
      //morse par defaut, et on applique la methode inverse()
      //pour resoudre le systeme lineaire.
      const Matrice_Morse_Sym& la_matrice=ref_cast(Matrice_Morse_Sym,
                                                   matrice_vorticite_.valeur());
      //      Matrice_Morse& la_matrice = (Matrice_Morse&) matrice_vorticite_.valeur();
      DoubleTab solution_temporaire(la_matrice.ordre());

      assert(solution_temporaire.size() == solution.size()-1);
      assert(curl.size() == solution_temporaire.size());

      //On resoud un systeme lineaire pour calculer la vorticite
      //La vorticite est alors stockee dans la variable solution
      //      la_matrice.inverse(curl,solution_temporaire,1e-15);
      Solv_GCP& solv = ref_cast_non_const(Solv_GCP,solveur_.valeur());
      solv.set_seuil(1e-17);
      solv.resoudre_systeme(la_matrice,curl,solution_temporaire);

      //On recopie les valeurs de solution temoraire dans solution
      //Une case n'est pas remplie: on le fait plus tard.
      for (int i=0; i<solution_temporaire.size(); i++)
        solution[i]=solution_temporaire[i];

      //On remplit le dernier element de "solution"
      int sommet = zone.nb_som()-1;

      for (int i=0; i<curl_.elem_som_size(sommet); i++)
        solution[sommet] +=
          solution_temporaire[curl_.elements_pour_sommet(sommet,i)];

      nb_appel2++;

    }

  return 1;
}

//////////////////////////////////////////////////////
/* Fonctions permettant l'assemblage de la matrice */
/* de vorticite */
/////////////////////////////////////////////////////

/* On calcule la matrice de vorticite pour le probleme triple:
   / on considere le cas sans condition au limite pour la vorticite
   / pour le moment, et on ne code pas la partie parallele de
   / l'algo.
   / REM: pour l'instant, ne fonctionne qu'en 2D
*/
int Op_Diff_RotRot::assembler_matrice(Matrice& matrice)
{
  const Zone& zone = zone_Vef().zone();

  int colonne_a_remplir_tab2,colonne_a_remplir_coeff;
  int nombre_coeff_non_nuls;

  Cerr << "Assemblage de la matrice de vorticite en cours..." << finl;
  matrice.typer("Matrice_Morse_Sym");
  //Matrice_Morse& la_matrice=(Matrice_Morse&) matrice.valeur();

  // On dimensionne la matrice de maniere convenable.
  // La matrice est carree et de taille (nombre_elem+nombre_som)
  // Le nombre de coeff non nuls n'excedent pas
  // (2* (dimension+1) + 1)* nb_elem + (nb_som-1) * (nb_som-1)
  // ATTENTION: une BASE de mon espace est les fonctions indicatrices
  // des elements + les fonctions chapeaux - 1 de ces fonctions
  // Sinon la somme des toutes les fontions chapeaux - la somme
  // de toutes les fonctions indicatrices = 0
  // Par defaut, on enleve la derniere fonction chapeau pour
  // former notre base.
  // Cf. Papier dans Latex/Vorticity
  nombre_coeff_non_nuls = (2*dimension + 3) * zone.nb_elem()
                          + (zone.nb_som()-1)* (zone.nb_som()-1);
  //la_matrice.dimensionner(zone.nb_elem()+zone.nb_som()-1,nombre_coeff_non_nuls);
  Matrice_Morse la_matrice(zone.nb_elem()+ zone.nb_som()-1,nombre_coeff_non_nuls);

  //Au cas ou la matrice existe deja dans un fichier
  Nom nomfic("Vorticite.sv");
  LecFicDistribueBin vorticite;
  int fic_vorticite_existe;

  if(vorticite.ouvrir(nomfic)) fic_vorticite_existe=1;
  else fic_vorticite_existe=0;

  if(fic_vorticite_existe)
    {
      Cerr << "Relecture de la matrice de vorticite sur le fichier: "
           << nomfic << finl;
      vorticite >> la_matrice;
      vorticite.close();
      Cerr << "Fin de la relecture de la matrice de vorticite." << finl;
      return 1;
    }

  Cerr << "Assemblage de la matrice de vorticite " << nomfic << finl;

  // Maintenant on remplit la matrice ligne par ligne
  // On rappelle que les elements et les sommets sont numerotes
  // a partir de 0.
  // Mais les indices de colonnes et de lignes des tableaux
  // d'une matrice morse FORTRAN commencent a 1.
  // Ex de construction: matrice[numerotation_C++] = numerotation_FORTRAN

  // Parametres importants pour remplir les tableaux
  nombre_coeff_non_nuls = 1;
  colonne_a_remplir_tab2 = 0; //pour le C++
  colonne_a_remplir_coeff = 0; //pour le C++

  // On commence par remplir les lignes de la sous-matrice
  // de taille nb_elem * (nb_elem + nb_som) : cf. structure de la matrice
  for (int numero_elem = 0; numero_elem < zone.nb_elem(); numero_elem++)
    {
      // Pour un element donne "numero_elem", on calcule la liste
      // des sommets qui appartiennent a cet element.
      IntList sommets_pour_elem = sommets_pour_element(numero_elem);
      Tri(sommets_pour_elem); //tableau trie

      // On remplit tab1
      la_matrice.tab1(numero_elem) = nombre_coeff_non_nuls;

      // On remplit tab2 et coeff

      //Les nb_elem premiers elements de la ligne "numero_elem"
      //a cause du FORTRAN
      la_matrice.tab2(colonne_a_remplir_tab2) = numero_elem + 1;
      la_matrice.coeff(colonne_a_remplir_coeff) =
        remplir_elem_elem_EF(numero_elem);

      //On incremente nb_coeff_non_nuls car on vient de remplir
      //une ligne par l'un de ces elements non nul
      nombre_coeff_non_nuls++;

      //On incremente les compteurs puisque l'on vient de remplir
      //des cases du tableau
      colonne_a_remplir_tab2++;
      colonne_a_remplir_coeff++;

      //Les nb_som elements de la ligne "numero_elem"
      for (int i = 0; i < zone.nb_som_elem(); i++)
        {
          //Si ce "sommet_pour_elem[i]" est different du numero
          //du dernier sommet alors on stocke le bon coefficient
          if (sommets_pour_elem[i] != zone.nb_som()-1)
            {
              la_matrice.tab2(colonne_a_remplir_tab2) =
                zone.nb_elem()+sommets_pour_elem[i]+1; //pour FORTRAN
              la_matrice.coeff(colonne_a_remplir_coeff) =
                remplir_elem_som_EF(numero_elem,sommets_pour_elem[i]);

              //On incremente nb_coeff_non_nuls car on vient de remplir
              //une ligne avec l'un de ces elements non nul
              nombre_coeff_non_nuls++;

              //Enfin on incremente les colonne_* pour eviter de reecrire
              //sur des coefficients deja stockes
              colonne_a_remplir_tab2++;
              colonne_a_remplir_coeff++;
            }

        }

      //Pas besoin de modifier les entiers nombre_coeff_non_nuls et
      //colonne_* : ils sont a la bonne valeur.

    }

  // On remplit les lignes de la sous-matrice de taille
  // nb_som *( nb_elem+nb_som) :cf. structure de la matrice
  for (int numero_som = 0; numero_som < zone.nb_som()-1; numero_som++)
    {
      // On stocke les tableaux qui nous sont utiles
      // Ainsi que leur taille respective
      IntList Elem_pour_sommet = elements_pour_sommet(numero_som);
      IntList Sommets_voisins = sommets_voisins(numero_som,Elem_pour_sommet);
      Tri(Elem_pour_sommet); //liste triee
      Tri(Sommets_voisins); //liste triee

      //On remplit tab1
      //pour FORTRAN
      la_matrice.tab1(zone.nb_elem()+numero_som) = nombre_coeff_non_nuls;

      //On remplit tab2 et coeff

      //Les nb_elem premiers elements de la ligne "numero_som"
      for (int i=0; i < Elem_pour_sommet.size(); i++)
        {
          //A cause du FORTRAN
          la_matrice.tab2(colonne_a_remplir_tab2) = Elem_pour_sommet[i]+1;
          la_matrice.coeff(colonne_a_remplir_coeff) =
            remplir_som_elem_EF(Elem_pour_sommet[i],numero_som);

          //On incremente nb_coeff_non_nuls car on vient de remplir
          //une ligne avec l'un de ces elements non nul
          nombre_coeff_non_nuls++;

          //On incremente convenablement les indices de colonnes
          colonne_a_remplir_tab2++;
          colonne_a_remplir_coeff++;
        }

      //Pas besoin de modifier les entiers nb_coeff_non_nuls et
      //colonne_* : ils sont a la bonne valeur.

      //Les nb_som elements de la ligne "numero_som"
      for (int i=0; i < Sommets_voisins.size(); i++)
        {
          //On recupere le numero global du sommet_voisin
          int numero_som_global = Sommets_voisins[i];

          //Et si "numero_som_global" est different du numero
          //du dernier sommet, alors on stocke le bon coefficient

          if (numero_som_global != zone.nb_som()-1)
            {
              //A cause du FORTRAN
              la_matrice.tab2(colonne_a_remplir_tab2) =
                zone.nb_elem()+Sommets_voisins[i]+1;
              la_matrice.coeff(colonne_a_remplir_coeff) =
                remplir_som_som_EF(numero_som,Sommets_voisins[i],Elem_pour_sommet);

              //On incremente nb_coeff_non_nuls car on vient de remplir
              //une ligne avec l'un de ces elements non nul
              nombre_coeff_non_nuls++;

              //On incremente les indices
              colonne_a_remplir_tab2++;
              colonne_a_remplir_coeff++;
            }

        }

      //Pas besoin d'incrementer nombre_coeff_non_nuls et colonne_*
      //: ils sont deja a la bonne valeur
    }

  //Par convention pour les matrices morses, le dernier element
  //de tab1 est tab1[zone.nb_elem()+zone.nb_som()+1] et vaut
  //le nombre total de coefficients non nuls +1
  //soit avec notre algorithme: nombre_coeff_non_nuls
  la_matrice.tab1(zone.nb_elem()+zone.nb_som()-1) = nombre_coeff_non_nuls;

  //   if(Debog::mode_db==2) Debog::save_matrix_seq(la_matrice);
  //   else if(Debog::mode_db==3)
  //     Debog::save_and_distribute_matrix_seq(la_matrice);

  Matrice_Morse_Sym& la_matrice_sym = ref_cast(Matrice_Morse_Sym,
                                               matrice.valeur());
  la_matrice_sym = la_matrice;

  Cerr << "Fin de l'assemblage de la matrice de vorticite. " << finl;

  return 1;
}

/* Pour un element donne "numero_elem" retourne */
/* la liste des sommets appartenant a cet element */
IntList Op_Diff_RotRot::sommets_pour_element(int numero_elem) const
{
  IntList resultat;
  int numero_global_sommet = 0;
  const Zone& zone = zone_Vef().zone();

  // Par precaution mais normalement inutile
  if (! resultat.est_vide() ) resultat.vide();

  for (int i=0 ; i< zone.nb_som_elem() ; i++)
    {
      numero_global_sommet = zone.sommet_elem(numero_elem,i);
      resultat.add_if_not(numero_global_sommet);
    }

  return resultat;
}

/* Pour un sommet donne "numero_sommet" retourne */
/* la liste des elements contenant ce sommet */
IntList
Op_Diff_RotRot::elements_pour_sommet(int numero_sommet) const
{
  IntList resultat;
  int numero_global_som;
  const Zone& zone = zone_Vef().zone();

  // Par precaution mais normalement inutile
  if (! resultat.est_vide() ) resultat.vide();

  // Pas tres efficace mais marche quelle que soit la dimension:
  // on boucle sur les elements, on regarde pour chaque element
  // ses sommets, puis on compare ces sommets au parametre d'entree
  // et si l'un d'eux coincide avec le parametre d'entree, on stocke
  // l'element.
  for (int numero_elem = 0 ; numero_elem < zone.nb_elem() ; numero_elem++)
    for (int numero_som = 0 ; numero_som < zone.nb_som_elem() ; numero_som++)
      {
        numero_global_som = zone.sommet_elem(numero_elem,numero_som);
        if ( numero_sommet == numero_global_som )
          resultat.add_if_not(numero_elem);
      }

  return resultat;

}

/* Pour un sommet donne "numero_sommet" retourne */
/* la liste des sommets voisins de "numero_sommet" */
/* Parametre: la liste des elements contenant "numero_sommet" */
/* Il suffit de chercher dans ces elements pour avoir le resultat */
/* REM: la liste resultat contient le sommet "numero_sommet" */
IntList Op_Diff_RotRot::
sommets_voisins(int numero_sommet,const IntList& liste) const
{
  IntList resultat;
  int numero_global_som;
  const Zone& zone = zone_Vef().zone();

  //Il suffit de recuperer les sommets des elements de "liste"
  //puis de les comparer a "numero_som" et de les conserver
  //sans doublon.
  for (int numero_elem_loc = 0 ; numero_elem_loc<liste.size(); numero_elem_loc++)
    for (int numero_som = 0; numero_som < zone.nb_som_elem() ; numero_som++)
      {
        numero_global_som =
          zone.sommet_elem(liste[numero_elem_loc],numero_som);
        resultat.add_if_not(numero_global_som); //contient "numero_sommet"
      }

  //   Cerr << "Affichage de sommets_voisins pour numero_som " << numero_sommet
  //        << finl;
  //   for (int i=0;i<resultat.size();i++)
  //     Cerr << resultat[i] << finl;

  return resultat;
}

/* Fonction de tri d'une IntList */
/* Le tri s'effectue par ordre croissant */
/* REM: on n'a aucun doublon dans la liste triee */
void Op_Diff_RotRot::Tri(IntList& liste_a_trier) const
{
  if (liste_a_trier.est_vide())
    {
      Cerr << "Erreur dans Op_Diff_RotRot::Tri()." << finl;
      Cerr << "La liste a trier est vide: sortie du programme." << finl;
      Process::exit();
    }

  IntList temporaire;
  int minimum;

  while ( !liste_a_trier.est_vide() )
    {
      minimum = liste_a_trier[0];

      for (int i = 0; i < liste_a_trier.size(); i++)
        minimum = (minimum <= liste_a_trier[i] ? minimum : liste_a_trier[i]);

      temporaire.add_if_not(minimum);
      liste_a_trier.suppr(minimum);
    }

  for (int i = 0 ; i < temporaire.size() ; i++)
    liste_a_trier.add_if_not(temporaire[i]);

}

/* Pour l'element "numero_elem" retourne le coefficient */
/* a placer dans la sous matrice de taille nb_elem * nb_elem */
/* a la ligne "numero_elem" , colonne "numero_elem"*/
/* Matrice EF */
double
Op_Diff_RotRot::remplir_elem_elem_EF(const int& numero_elem) const
{
  return 1.*zone_Vef().volumes(numero_elem);
}

/* Pour l'element "numero_elem" retourne le coefficient */
/* a placer dans la sous matrice de taille nb_elem * nb_som */
/* a la ligne "numero_elem" , colonne "numero_som"*/
/* Matrice EF */
double Op_Diff_RotRot::remplir_elem_som_EF(const int& numero_elem,
                                           const int& numero_som)
const
{
  return (1.*zone_Vef().volumes(numero_elem)/(dimension+1));
}

/* Pour le sommet "numero_som" retourne le coefficient */
/* a placer dans la sous matrice de taille nb_som * nb_elem */
/* a la ligne "numero_som" , colonne "numero_elem"*/
/* Matrice EF */
double
Op_Diff_RotRot::remplir_som_elem_EF(const int& numero_elem,
                                    const int& numero_som) const
{
  return (1.*zone_Vef().volumes(numero_elem)/(dimension+1));
}

/* Pour l'element "numero_som" retourne le coefficient */
/* a placer dans la sous matrice de taille nb_som * nb_som */
/* a la ligne "numero_som" , colonne "sommet_voisin"*/
/* "elem_voisins" est le tableau des elements contenant "numero_som" */
/* Matrice EF */
double
Op_Diff_RotRot::remplir_som_som_EF(const int& numero_som,
                                   const int& sommet_voisin,
                                   const IntList& elem_voisins) const
{
  double resultat = 0.;
  int test = 0;

  //Premier test: si numero_som = sommet_voisin
  //Alors on peut tout de suite retourner le resultat
  if (numero_som == sommet_voisin)
    {
      for (int i=0; i<elem_voisins.size(); i++)
        {
          resultat += zone_Vef().volumes(elem_voisins[i]);
        }

      //On tient compte de la dimension dans nos calculs
      resultat *= 2./((dimension+1)*(dimension+2));

      return resultat;
    }

  //Sinon:
  //On calcule la contribution au resultat

  for (int i=0; i<elem_voisins.size() ; i++)
    {
      if (sommets_pour_element(elem_voisins[i]).contient(sommet_voisin))
        {
          resultat += zone_Vef().volumes(elem_voisins[i]);
          test++;
        }
    }

  //On tient compte de la dimension dans laquelle on evolue
  resultat *= 1./((dimension+1)*(dimension+2));

  //On verifier que sommet_voisin etait bien dans le voisinage de numero_voisin
  if (test == 0)
    {
      Cerr << "Erreur Op_Diff_RotRot::remplir_som_som_EF." << finl;
      Cerr << "sommet_voisin n'est pas dans le voisinage de numero_voisin."
           << finl;
      Cerr << "Sortie du programme." << finl;
      Process::exit();
    }

  return resultat;

}


DoubleTab
Op_Diff_RotRot::vecteur_normal(const int& face, const int& elem) const
{
  assert(dimension == 2);

  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  DoubleTab le_vecteur_normal(dimension);

  for (int composante = 0; composante<dimension; composante++)

    le_vecteur_normal(composante) = zone_VEF.face_normales(face,composante)
                                    * zone_VEF.oriente_normale(face,elem);

  return le_vecteur_normal;
}

int Op_Diff_RotRot::tester() const
{
  //  const Zone& zone = zone_Vef().zone();

  //   if (vorticite_.valeur().nb_valeurs_nodales() !=
  //       zone.nb_elem()+zone.nb_som()-1 )
  //     {
  //       Cerr << "Probleme dans la definition de la vorticite." << finl;
  //       Cerr << "Le nombre de composante enregistree n'est pas le bon." << finl;
  //       Process::exit();
  //     }

  //Test de la matrice de vorticite
  const Matrice_Morse_Sym& la_matrice=ref_cast(Matrice_Morse_Sym,
                                               matrice_vorticite_.valeur());
  Solv_GCP& solv = ref_cast_non_const(Solv_GCP,solveur_.valeur());

  DoubleTab resultat(la_matrice.ordre());
  DoubleTab resultat1(la_matrice.ordre());
  DoubleTab secmem(la_matrice.ordre());
  DoubleTab secmem1(la_matrice.ordre());
  secmem = 0.;
  secmem[0]=-0.25;
  secmem[4]=-0.0833333;
  secmem[5]=-0.0833333;
  secmem1 = 1e-10 + 1e-15;

  SFichier fic("Matrice.test");
  la_matrice.imprimer_formatte(fic);

  resultat = 0.;
  solv.set_seuil(1e-17);
  solv.resoudre_systeme(la_matrice,secmem,resultat);
  Cerr << "Resultat de l'inversion" << finl;
  for (int i=0; i<8; i++)
    Cerr << resultat[i] << " ; ";
  Cerr << finl;

  solv.resoudre_systeme(la_matrice,secmem1,resultat);
  Cerr << "Resultat de l'inversion" << finl;
  for (int i=0; i<8; i++)
    Cerr << resultat[i] << " ; ";
  Cerr << finl;

  return 1;
}

void Op_Diff_RotRot::associer_diffusivite(const Champ_base& diffu)
{
  diffusivite_ = ref_cast(Champ_Uniforme, diffu);
}

const Champ_base& Op_Diff_RotRot::diffusivite() const
{
  return diffusivite_;
}
