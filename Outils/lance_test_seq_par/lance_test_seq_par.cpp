/***********************************************************************
 *                                                                      *
 *          Module seq_par  de comparaison de resultats TRUST          *
 *  entre un calcul sequentiel et un calcul parallele.                  *
 *  Pour la compilation :                                               *
 *  cc -o /luna/guy/Trio_u_test/lance_test_seq_par_en_C/seq_par -Aa -D_HPUX_SOURCE seq_par.c -lm  *
 *  Programme principal On doit le lancer avec la commande suivante:    *
 *  seq_par nom_seq nom_par                                             * 
 *  nom_seq   - Nom du fichier Sequentiel avec son chemin en absolu     *
 *  nom_par   - Nom du fichier Parallele avec son chemin en absolu      *
 *                                                                      *
 ***********************************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

typedef char *StringPtr;
typedef char String8[9];
typedef char String10[11];
typedef char String80[81];
typedef char String32[33];


#ifdef MICROSOFT
#define strcasecmp stricmp
#endif
#ifndef TRUE
#define TRUE  ( 0 == 0 )
#endif

#ifndef FALSE
#define FALSE ( 0 == 1 )
#endif

#define ERROR  -1
#define ERROR1  -2
#define OK      0
#define MOINS1 -1

#define MAX_GRILLE 16

/* Structure d'une Topologie      */
typedef struct {
  String80  nom_topo;      /* Nom de la topologie */
  String80  nom_grille;    /* Nom de la grille pour la topologie */
} T_Topo;

/* Structure des coordonnees  */
typedef struct {
  double   *x;             /* Coordonnee suivant X d'un point du maillage */
  double   *y;             /* Coordonnee suivant Y d'un point du maillage */
  double   *z;             /* Coordonnee suivant Z d'un point du maillage */
} T_Lcoor;

#define connect_max 18
typedef struct {
  int   connect[connect_max];             /* Connection pour une maille */
} T_Elem;

typedef struct {
  T_Elem   *lmaille;             /* Pointeur sur une liste de mailles */
} T_Lelem;

typedef struct {
  double     *comp1;               /* Pointeur sur une liste de la composante 1 */
  double     *comp2;               /* Pointeur sur une liste de la composante 2 */
  double     *comp3;               /* Pointeur sur une liste de la composante 3 */
} T_Val;
 
typedef struct {
  String80   *nom_champ;         /* Pointeur sur une liste de champs */
  int        *nb_comp;           /* Nombre de composantes par champ */
  int        *nelem;             /* Nombre de vecteurs par champ    */
  T_Val      *vec;               /* Pointeur sur une liste des vecteurs par champ*/
} T_Champ;

/* Structure du fichier TRIO contenant les informations necessaires a la sauvegarde*
 * des resultats du fichier  pour TRIOU.                              */
typedef struct {
  int      nb_grille;        /* Nombre de grille  du cas*/
  String80 nom_grille[MAX_GRILLE];    /* Nom des grilles */
  int      dim_grille[MAX_GRILLE];    /* Dimension du cas par grille */
  int      nb_noeud[MAX_GRILLE];     /* Nombre de noeuds par grille */
  int      nb_maille[MAX_GRILLE];     /* Nombre de mailles par grille */
  T_Topo   topologie[MAX_GRILLE];    /*  Topologie par grille*/
  T_Lcoor  coord1[MAX_GRILLE];        /* Liste des coordonnees par grille */
  T_Lelem  pu[MAX_GRILLE];            /* Liste des mailles par grille */
  int      nb_champ;         /* Nombre de champs  du cas*/
  T_Champ  champ ;          /* Liste des caracteristiques champs */
  String80 file_name;
} T_Trio;

typedef struct {
  int    *iseq;
  int    *ielem;
} T_Equiv;

/***********************************************************************
 *  MOTS CLEFS ( ou MOTS CLE  ou MOT-CLES ou MOT-CLEFS ... a suivre )
 *  ~~~~~~~~~~
 *
 *
 *                Identificateurs des mots clefs
 *                ******************************
 *
 * ********************************************************************
 * * Les idents utilises portent le meme nom que le texte du mot-cle  *
 * * Si il doit y avoir abandon de ce point, modifier "a la main"     *
 * *  la fonction  init_tab_motcl() .                                 *
 * ********************************************************************
 *
 * Lors d'une mise a jour de cette liste,
 *      NE PAS OUBLIER de mettre a jour NBMOTC et  init_tab_motc()
 *
 *
 ************************************************************************/
#define NBMOTC 5
#define SEQUENTIEL 1
#define PARALLELE 2

enum IDMOTC { /* liste de definition du type IDMOTC*/
  GRILLE,
  TEMPS,
  CHAMPPOINT,
  CHAMPMAILLE,
  FIN

}; /* fin liste de definition du type IDMOTC*/

/* Variables globales */
int          mode_debug = 0;   /* flag de presence d'une demande de debug */
double       gepsilon =1e-05;
double       gerr = 0.0;
int          gnerr = -1;         /* Nombre de differences trouvees */
double       gemax = 0;         /* Erreur maximum rencontree */
String80     g_liste_motcle[NBMOTC];
int          g_nb_motcle = NBMOTC;
double *gvalm;
T_Trio       contenu_res;
T_Trio       contenu_par;
T_Equiv      equiv[MAX_GRILLE];


/*
 *  -----------------------------------------------------------------------
 *           Initialisation de la table des textes des mots clefs
 *
 *  REMARQUE: l'ident numerique du mot-cle est egale a son index
 *            dans cette table.
 *
 *  Le texte du mot cle et le nom de l'identificateur ( const )
 *    correspondant sont identiques.
 *  Pour creer la liste d'initialisation, suivre la procedure:
 *   . recopier la liste des definition des identificateurs des mots clefs
 *       qui se trouve dans  Resultat_ux.typ.h
 *
 *  -----------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------*/
void init_tab_motc(String80 *motc)   
{
  strcpy(motc[       GRILLE], "GRILLE");
  strcpy(motc[       TEMPS], "TEMPS");
  strcpy(motc[       CHAMPPOINT], "CHAMPPOINT");
  strcpy(motc[       CHAMPMAILLE], "CHAMPMAILLE");
  strcpy(motc[       FIN], "FIN");
}
 
/****************************************************************
 *Procedure Ote_blanc( chaine)                                   *
 *---------------------------------------------------------------*
 *                                                               *
 * Procedure de suppression des blancs en fin de chaine          *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * StringPtr  chaine      - Chaine initiale                      *
 * Sorties :                                                     *
 * StringPtr  chaine      - Chaine sans les blancs               *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.DEGENEVE                                          *
 * Creation : 12/02/97                                           *
 ****************************************************************/
void Ote_blanc (StringPtr chaine)

{
  int      len;

  len = strlen(chaine);
  if(mode_debug)
    printf("Longueur %d Valeur de la chaine initiale : <%s>\n", len, chaine);
  for (int i = len-1; i > -1; i--){
    if(chaine[i] == '\n'){
      chaine[i] = '\0';
      break;
    }
    else if(chaine[i] != ' '  && chaine[i] != '\0'){
      chaine[i+1] = '\0';
      break;
    }
  }
  if(mode_debug)
    printf("Longueur %d Valeur de la chaine finale : <%s>\n", len, chaine);
}
/****************************************************************
 * Fonction  Extraire_motcle(chaine, nb_mot, liste)              *
 *---------------------------------------------------------------*
 *                                                               *
 * Fonction d'extraction d'un mot cle de la chaine               *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * StringPtr  chaine      - Pointeur sur la Chaine origine       *
 * int        nb_mot      - Nombre de mots cles dans la liste.   *
 * String80   *liste      - Pointeur sur la liste des mots cles  *
 *---------------------------------------------------------------*
 * Valeur retournee par la fonction :                            *
 *     "" si la chaine ne contient pas de mot cle                *
 *     le mot cle si la chaine contient le mot cle               *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.DEGENEVE                                          *
 * Creation : 11/03/98                                           *
 ****************************************************************/
StringPtr Extraire_motcle_corr(StringPtr chaine, int nb_mot, String80 *liste, String80 motcle)
{
  motcle[0]='\0';
  sscanf(chaine, "%s", motcle);
  for(int i = 0; i < nb_mot; i++){
    if(strcmp(motcle, liste[i]) == 0){
      /* On a trouve un mot cle */
      return (motcle);
    }
  }
  strcpy(motcle, "");
  return (motcle);
}

/****************************************************************
 * Fonction  Cherche_motcle(chaine, nb_mot, liste)               *
 *---------------------------------------------------------------*
 *                                                               *
 * Fonction de recherche du mot cle chaine dans la liste des     *
 * mots cles                                                     *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * StringPtr     chaine     - Pointeur sur le mot cle recherche  *
 * int           nb_mot     - Nombre de mots cles dans la liste. *
 * StringPtr    *liste      - Pointeur sur la liste des mots cles*
 *---------------------------------------------------------------*
 * Valeur retournee par la fonction :                            *
 *     ERROR1 si le mot cle chaine n'a pas ete trouve.           *
 *     la position du mot cle dans la liste.                     *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.DEGENEVE                                          *
 * Creation : 12/03/98                                           *
 ****************************************************************/

int Cherche_motcle (StringPtr chaine,int  nb_mot, String80 *liste)   
{
  for(int i = 0; i < nb_mot; i++){
    if(strcmp(chaine, liste[i]) == 0){
      /* On a trouve le mot cle recherche */
      return (i);
    }
  }
  return (ERROR1);
}
/****************************************************************
 * Fonction  Ecart(x, y, ymax)                                   *
 *---------------------------------------------------------------*
 *                                                               *
 * Calcul l'ecart entre x et y en pourcentage d'une valeur donnee*
 * mots cles                                                     *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * double     x     - Premiere valeur donnee                     *
 * double     y     - Seconde valeur donnee                      *
 * double     ymax  - Valeur de reference pour la comparaison    *
 *---------------------------------------------------------------*
 * Valeur retournee par la fonction :                            *
 *     1 si les 2 valeurs ont un ecart relatif en pourcentage    *
 *          > a epsilon                                          *
 *     0 si les 2 valeurs ont un ecart relatif en pourcentage    *
 *          < ou = a epsilon                                     *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.DEGENEVE                                          *
 * Creation : 12/03/98                                           *
 ****************************************************************/
int Ecart(double x, double y, double ymax)
{
  int flag = 1;
  gerr = 0.0;    
  if(ymax != 0.0) gerr = fabs((x-y)/ymax);
  /* Erreur relative : en %, multiplier par 100; */
  /* Si le max est inferieur a 1.e-14 on ne compare pas */   
  /* GF et si maintenant on compare */
  /* PL on passe a 1.e-12 le 13/12/12 */
  double seuil=1e-12;
  if (ymax<=seuil)
    {
      if( (fabs(x)>seuil)||(fabs(y)>seuil))  
	{
	  gerr=1;
	}	
      else gerr=0;
    }
  if(gerr <= gepsilon) flag = 0;
  if(fabs(gerr) > gemax) gemax = gerr;
  return (flag);
}


/****************************************************************
 * Fonction  Lecture_Geom(nom, contenu)                          *
 *****************************************************************
 *                                                               *
 * Fonction permettant de lire des fichiers de resultats Triou   *
 * en vue de les comparer.                                       *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * char     *nom  -Pointeur sur le nom du fichier                *
 *                      avec son chemin en absolu                *
 *---------------------------------------------------------------*
 * Sorties :                                                     *
 * T_Trio   *contenu   - Pointeur sur la structure contenant les *
 *                       informations contenues dans             *
 *                       un fichier de resultats TRIOU.          *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.DEGENEVE                                          *
 * Creation :  1/08/02                                           *
 ****************************************************************/
int Lecture_Geom(char *nom,T_Trio   *contenu)
  
{

  FILE *fic = NULL;               /* pointeur sur fichier de resultats */
  int igrille;

  char chaine[82];
  String80  motcle;
  char      buff[BUFSIZ + 1];

  igrille = contenu->nb_grille;
  if(contenu->nb_grille == MAX_GRILLE){
    printf("Nombre maximum de grilles autorise: MAX_GRILLE\n"); 
    return (ERROR);
  }
  /******* lecture du fichier    *******/

  /*    printf("Trace 0\n");*/
  if ( (fic = fopen (nom, "r")) == NULL){
    printf ("Erreur a l'ouverture du fichier %s\n", nom);
    return (ERROR);
  }
  // On verifie le format lml du fichier
  /* Non portable sous windows (include <stdio.h>)
  char * line = NULL;
  size_t len = 0;
  getline(&line, &len, fic); 
  */
  // Portable:
  std::ifstream in(nom);
  std::string line;
  std::getline(in,line);
  if (line.compare(0,7,"Trio_U "))
  {
     printf("Error: The format of the file is not lml !\n");  
     return (ERROR);
  }
  /*    printf("Trace 1\n");*/

  /* Boucle de lecture */
  //   char* s;
  while (fgets(buff,BUFSIZ,fic) != NULL){
    /* Lecture d'une ligne du fichier resultats */
    Ote_blanc(buff);

    Extraire_motcle_corr(buff, g_nb_motcle, g_liste_motcle,motcle);
    int imot = Cherche_motcle(motcle, g_nb_motcle, g_liste_motcle);
    switch(imot){
    case ERROR1:
      /* Le mot cle n'a pas ete trouve c'est une ligne de resultat a ne pas exploiter*/
      break;
    case GRILLE:
      /* On recupere le nom de la Grille */
      if (!fgets(buff,BUFSIZ,fic)) exit(-1);
      Ote_blanc(buff);
      if(sscanf(buff,"%s %d %d", contenu->nom_grille[igrille], 
		&contenu->dim_grille[igrille],  &contenu->nb_noeud[igrille]) == EOF){
	printf ("Erreur a la lecture de la grille %s\n", contenu->nom_grille[igrille]);
	return (ERROR);
      }
      /* Lecture des coordonnees des noeuds */
      /* Allouer l'espace memoire necessaire au stockage des coordonnees */
      contenu->coord1[igrille].x = (double *)malloc (sizeof(double) * contenu->nb_noeud[igrille]);
      contenu->coord1[igrille].y =(double *) malloc (sizeof(double) * contenu->nb_noeud[igrille]);
      contenu->coord1[igrille].z =(double *) malloc (sizeof(double) * contenu->nb_noeud[igrille]);
      for (int i = 0; i < contenu->nb_noeud[igrille]; i++){
	if (!fgets(buff,BUFSIZ,fic)) exit(-1);
	Ote_blanc(buff);
	sscanf(buff, "%lf %lf %lf", &contenu->coord1[igrille].x[i], &contenu->coord1[igrille].y[i],
	       &contenu->coord1[igrille].z[i]);
      }
      /* On recupere le nom de la Topologie */
      if (!fgets(buff,BUFSIZ,fic)) exit(-1);
      if (!fgets(buff,BUFSIZ,fic)) exit(-1);
      Ote_blanc(buff);
      sscanf(buff, "%s %s", contenu->topologie[igrille].nom_topo, contenu->topologie[igrille].nom_grille);
      if (!fgets(buff,BUFSIZ,fic)) exit(-1);
      if (!fgets(buff,BUFSIZ,fic)) exit(-1);
      Ote_blanc(buff);
      sscanf(buff, "%d", &contenu->nb_maille[igrille]);
      /* Lecture des mailles */
      /* Allouer l'espace memoire necessaire au stockage des connections */
      contenu->pu[igrille].lmaille =(T_Elem *) malloc (sizeof(T_Elem) * contenu->nb_maille[igrille]);
      for (int i = 0; i < contenu->nb_maille[igrille]; i++)
	for (int j = 0; j < connect_max;j++) contenu->pu[igrille].lmaille[i].connect[j] = 0;
      for (int i = 0; i < contenu->nb_maille[igrille]; i++){
	if (!fgets(buff,BUFSIZ,fic)) exit(-1);
	Ote_blanc(buff);
	sscanf(buff, "%s", chaine);
        if( strcmp(chaine, "TRIANGLE_3D") == 0){
          sscanf(buff, "%s %d %d %d", chaine,
                 &contenu->pu[igrille].lmaille[i].connect[0],
                 &contenu->pu[igrille].lmaille[i].connect[1],
                 &contenu->pu[igrille].lmaille[i].connect[2]);
	  }
          else if(strcmp(chaine, "SEGMENT") == 0 ){
	  sscanf(buff, "%s %d %d ", chaine,
		 &contenu->pu[igrille].lmaille[i].connect[0],
		 &contenu->pu[igrille].lmaille[i].connect[1]);
	}
          else if(strcmp(chaine, "POINT") == 0 ){
	  sscanf(buff, "%s %d ", chaine,
		 &contenu->pu[igrille].lmaille[i].connect[0]);
	}
          else if(strcmp(chaine, "POLY4") == 0 || strcmp(chaine, "TETRA4") == 0 || strcmp(chaine, "QUADRANGLE_3D") == 0){
	  sscanf(buff, "%s %d %d %d %d", chaine,
		 &contenu->pu[igrille].lmaille[i].connect[0],
		 &contenu->pu[igrille].lmaille[i].connect[1],
		 &contenu->pu[igrille].lmaille[i].connect[2],
		 &contenu->pu[igrille].lmaille[i].connect[3]);
	}
	else if(strcmp(chaine, "POLY5") == 0){
	  sscanf(buff, "%s %d %d %d %d %d", chaine,
		 &contenu->pu[igrille].lmaille[i].connect[0],
		 &contenu->pu[igrille].lmaille[i].connect[1],
		 &contenu->pu[igrille].lmaille[i].connect[2],
		 &contenu->pu[igrille].lmaille[i].connect[3],
		 &contenu->pu[igrille].lmaille[i].connect[4]);
	}
	else if(strcmp(chaine, "POLY6") == 0 || strcmp(chaine, "PRISM6") == 0){
	  sscanf(buff, "%s %d %d %d %d %d %d", chaine,
		 &contenu->pu[igrille].lmaille[i].connect[0],
		 &contenu->pu[igrille].lmaille[i].connect[1],
		 &contenu->pu[igrille].lmaille[i].connect[2],
		 &contenu->pu[igrille].lmaille[i].connect[3],
		 &contenu->pu[igrille].lmaille[i].connect[4],
		 &contenu->pu[igrille].lmaille[i].connect[5]);
	}
	else if(strcmp(chaine, "POLY7") == 0){
	  sscanf(buff, "%s %d %d %d %d %d %d %d", chaine,
		 &contenu->pu[igrille].lmaille[i].connect[0],
		 &contenu->pu[igrille].lmaille[i].connect[1],
		 &contenu->pu[igrille].lmaille[i].connect[2],
		 &contenu->pu[igrille].lmaille[i].connect[3],
		 &contenu->pu[igrille].lmaille[i].connect[4],
		 &contenu->pu[igrille].lmaille[i].connect[5],
		 &contenu->pu[igrille].lmaille[i].connect[6]);
	}
	else if(strcmp(chaine, "POLY8") == 0 || strcmp(chaine, "VOXEL8") == 0){
	  sscanf(buff, "%s %d %d %d %d %d %d %d %d", chaine,
		 &contenu->pu[igrille].lmaille[i].connect[0],
		 &contenu->pu[igrille].lmaille[i].connect[1],
		 &contenu->pu[igrille].lmaille[i].connect[2],
		 &contenu->pu[igrille].lmaille[i].connect[3],
		 &contenu->pu[igrille].lmaille[i].connect[4],
		 &contenu->pu[igrille].lmaille[i].connect[5],
		 &contenu->pu[igrille].lmaille[i].connect[6],
		 &contenu->pu[igrille].lmaille[i].connect[7]);
	}
	else if (strstr(chaine,"POLYEDRE")!=NULL) {
				     
	  int nb=-1;
	  sscanf(chaine+strlen("polyedre_"),"%d",&nb);
	  if (nb==-1) abort();
	  if (nb>connect_max)
	    { cerr<< chaine<< " a trop de sommets !!! "<< nb<< ">"<<connect_max<<endl;
	      exit(-1);
	      cerr<<"on ne lit que les premiers!!!!!!"<<endl;
	      nb=connect_max;
	    }
	  //    cerr<<nb<<endl;exit(-1);
	  char* bufe=buff;
	  for (int il=0;il<nb;il++)
	    {
	      bufe=strstr(bufe+1," ");
				       
	      sscanf(bufe,"%d",&contenu->pu[igrille].lmaille[i].connect[il]);
	      // if (contenu->pu[igrille].lmaille[i].connect[il]==0) contenu->pu[igrille].lmaille[i].connect[il]=1;
	      // cerr<<i<< buff << " "<<bufe<<":"<<ii<<endl;
	    }
	  //exit(-1);
	}
	else 
	  {
	    cerr<<chaine<< " elt incompris" <<endl;
	    exit(-1);
	  }
                            
      }
      igrille++;
      break;
    case FIN :
      /* On a trouve la Fin du fichier                     */
      break;
    }
  }/* Fin lecture des lignes du fichier */
  contenu->nb_grille = igrille;

  if(contenu->nb_grille > MAX_GRILLE){
    printf("Nombre de grilles trouvees : %d\n", contenu->nb_grille);
    printf("Nombre maximum de grilles autorise:%d \n", MAX_GRILLE); 
    return (ERROR);
  }
  /* Fermeture des fichiers resultats */
  fclose (fic);
  if(mode_debug){
    printf("Nombre de grilles trouvees : %d\n", contenu->nb_grille);
  }

  return (OK);
}

/****************************************************************
 * Fonction  Recherche_temps(fic, contenu, time)                 *
 *****************************************************************
 *                                                               *
 * Fonction permettant de rechercher un temps                    *
 * en vue de les comparer.                                       *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * FILE   *fic -Pointeur sur le fichier                          *
 * T_Trio   *contenu   - Pointeur sur la structure contenant les *
 *                       informations contenues dans             *
 *                       un fichier de resultats TRIOU.          *
 * double   time       - Valeur du pas de temps recherche.       *
 *---------------------------------------------------------------*
 * Sorties :                                                     *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.DEGENEVE                                          *
 * Creation :  1/08/02                                           *
 ****************************************************************/
int Recherche_temps(ifstream *fic, T_Trio *contenu, double time, int exact=0)
    
{
  String80  motcle;
  char      buff[BUFSIZ + 1];
  double    t;
  int       imot;
  while (*fic)
    { 
      *fic >> buff;
      /* Lecture d'une ligne du fichier resultats */
      Extraire_motcle_corr(buff, g_nb_motcle, g_liste_motcle,motcle);
      imot = Cherche_motcle(motcle, g_nb_motcle, g_liste_motcle);
      switch(imot){
      case ERROR1:
	/* Le mot cle n'a pas ete trouve c'est une ligne de resultat a ne pas exploiter*/
	break;
      case TEMPS:
	*fic >> t;
	/* On a trouve un temps */
	if (exact)
	  {
	    if (t==time)
	      return (OK);
	    else
	      printf("Time read = %10.5e time searched= %10.5e\n", t, time);
	  }
	else {

	  if(t == 0.0 ){
	    if(fabs(t - time) <= gepsilon) return (OK);
	    else{
	      printf("Time read = %10.5e time searched= %10.5e\n", t, time);
	    }
	  }
	  else{
	    if(fabs(t - time)/t <= gepsilon){
	      /* On a trouve le meme temps */
	      return (OK);
	    }
	    else{
	      printf("Time read = %10.5e time searched= %10.5e\n", t, time);
	    }
	  }
	}
	break;
      default :
	break;
      }
    }
  return (ERROR);
}

/****************************************************************
 * Fonction  Lecture_Result(fic, type, contenu, k, time)         *
 *****************************************************************
 *                                                               *
 * Fonction permettant de lire les resultats jusqu'au temps      *
 * suivant ou le motcle FIN.                                     *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * FILE   *fic -Pointeur sur le fichier                          *
 * int      type       - Indicateur si le fichier est issu d'un: *
                         calcul sequentiel.  (SEQUENTIEL)        *
                         calcul parallele.   (PARALLELE)         *
			 * T_Trio   *contenu   - Pointeur sur la structure contenant les *
			 *                       informations contenues dans             *
			 *                       un fichier de resultats TRIOU.          *
			 * int       k         - Numero de l'instant (debute a 1)        *
			 * double    time      - Valeur de l'instant.
			 *---------------------------------------------------------------*
			 * Sorties :                                                     *
			 *---------------------------------------------------------------*
			 * Auteur :                                                      *
			 *           G.FAUCHET                                        *
			 * Creation :  1/08/02                                           *
			 ****************************************************************/
int Lecture_Result(ifstream *fic,int   type, T_Trio   *contenu,int  k, double   time)
    
{


  String80  motcle;
  String80   s1;
  String80   s2;
  String80  nom_champ;
  String80  nom_topo;
  double    t;
  int       igrille;
  int       ic,ic2=-1;
  int       nb_comp;
  int       nelem;
  int       nm=-1;
  int       no=-1,nom=-1;
  int       ival;
  double       val1;
  double       val2;
  double       val3;
  double       em = 0.0;
  String10   type_case;
  streampos      fic_sauve(0);
  double valmin;
  double x=0,y=0;
  
  /* Sauvegarde du pointeur de lecture */
  if(type == PARALLELE) em = 0.0;
  char * bufcpp=new char[BUFSIZ];
  while (*fic)
    {
      fic_sauve = (*fic).tellg();
      *fic >> bufcpp;
      /* Lecture d'une ligne du fichier resultats */      
      Extraire_motcle_corr(bufcpp, g_nb_motcle, g_liste_motcle,motcle);
      int imot = Cherche_motcle(motcle, g_nb_motcle, g_liste_motcle);
      switch(imot)
	{
	case ERROR1:
	  /* Le mot cle n'a pas ete trouve c'est une ligne de resultat a ne pas exploiter*/
	  break;
	case CHAMPMAILLE :
	case CHAMPPOINT :
	  /* On verifie que la topologie existe bien */
	  *fic >> nom_champ>>nom_topo >>t;
	  igrille = -1;
	  for (int i = 0; i < contenu->nb_grille; i++){
	    if(strcmp(nom_topo, contenu->topologie[i].nom_topo) == 0) igrille = i;
	  }

	  if(igrille == -1){
	    printf("Incoherence  topologie %s non definie!\n", nom_topo);
	    return (ERROR);
	  }
	  *fic >> nom_champ>>nb_comp>>s1>>s2>>nelem;
	  if(t == 0.0 ){
	    if (fabs(time - t) > gepsilon){
	      printf("Incoherence  sur l' instant : %10.5e\n", time);
	      if(type == SEQUENTIEL) printf("dans le fichier sequentiel\n");
	      if(type == PARALLELE) printf("dans le fichier comparaison\n");
	      return (ERROR);
	    }
	  }
	  else{
	    if (fabs(time - t)/t > gepsilon){
	      printf("Incoherence  sur l' instant : %10.5e\n", time);
	      if(type == SEQUENTIEL) printf("dans le fichier sequentiel\n");
	      if(type == PARALLELE) printf("dans le fichier comparaison\n");
	      return (ERROR);
	    }
	  }
	  
	  if( k == 1 ){
	    int newc=0;
	    if(contenu->nb_champ == 0){
	      newc=1;
	      contenu->champ.nom_champ = (String80 *) malloc( (contenu->nb_champ+1)*sizeof(String80));
	      contenu->champ.nb_comp = (int *) malloc( (contenu->nb_champ+1)*sizeof(int));
	      contenu->champ.nelem = (int *) malloc( (contenu->nb_champ+1)*sizeof(int));
	      if(type == SEQUENTIEL) gvalm = (double *) malloc( (contenu->nb_champ+1)*sizeof(double));
	      contenu->champ.vec = (T_Val *) malloc( (contenu->nb_champ+1)*sizeof(T_Val));
	    }
	    else{
	      // correction on ajoute le champ que si nom_champ pas trouve!!
	      ic = 0;
	      while((ic<contenu->nb_champ) &&(strcmp(contenu->champ.nom_champ[ic], nom_champ) != 0)) ic++;
	      if( ic == contenu->nb_champ)
		{
		  newc=1;
		  contenu->champ.nom_champ = (String80 *)realloc(contenu->champ.nom_champ, (contenu->nb_champ+1)*sizeof(String80));
		  contenu->champ.nb_comp = (int *)realloc(contenu->champ.nb_comp, (contenu->nb_champ+1)*sizeof(int));
		  contenu->champ.nelem = (int *)realloc(contenu->champ.nelem, (contenu->nb_champ+1)*sizeof(int));
		  if(type == SEQUENTIEL) gvalm = (double *)realloc(gvalm, (contenu->nb_champ+1)*sizeof(double));
		  contenu->champ.vec = (T_Val*)realloc(contenu->champ.vec, (contenu->nb_champ+1)*sizeof(T_Val));
		}
	    }
	    if (newc==1)
	      {
		strcpy(contenu->champ.nom_champ[contenu->nb_champ], nom_champ);
		if(type == SEQUENTIEL) gvalm[contenu->nb_champ] = 0.0;
		contenu->champ.nb_comp[contenu->nb_champ] = nb_comp;
		contenu->champ.nelem[contenu->nb_champ] = nelem;
		contenu->nb_champ++;
	      }
	  }
	  ic = 0;
	  while((ic<contenu->nb_champ) &&(strcmp(contenu->champ.nom_champ[ic], nom_champ) != 0)) ic++;
	  if( ic == contenu->nb_champ){
	    printf("Probleme nom pour %s temps : %f\n", nom_champ, time);
	    return (ERROR);
	  }
	  
	  switch(contenu->champ.nb_comp[ic]){
	  case 1:
	    if( k == 1 ) contenu->champ.vec[ic].comp1 = (double *)malloc (sizeof(double ) * nelem);
	    break;
	  case 2:
	    if( k == 1 ) contenu->champ.vec[ic].comp1 = (double *)malloc (sizeof(double ) * nelem);
	    if( k == 1 ) contenu->champ.vec[ic].comp2 = (double *)malloc (sizeof(double ) * nelem);
	    break;
	  case 3:
	    if( k == 1 ) contenu->champ.vec[ic].comp1 = (double *)malloc (sizeof(double ) * nelem);
	    if( k == 1 ) contenu->champ.vec[ic].comp2 = (double *)malloc (sizeof(double ) * nelem);
	    if( k == 1 ) contenu->champ.vec[ic].comp3 = (double *)malloc (sizeof(double ) * nelem);
	    break;
	  }
	  if( type == SEQUENTIEL){
	    if(contenu->champ.nelem[ic] == contenu->nb_maille[igrille])
	      strcpy(type_case, "Element");
	    else if(contenu->champ.nelem[ic] == contenu->nb_noeud[igrille])
	      strcpy(type_case, "Sommet");
	    else{
	      printf("Probleme numeros pour %s temps : %10.5e\n",
		     nom_champ, time);
	      printf("pas code %d %d\n", contenu->nb_maille[igrille],
		     contenu->nb_noeud[igrille]);
	      return (ERROR);
	    }
	  }
	  for (int n = 0; n < contenu->champ.nelem[ic]; n++){
	    switch(contenu->champ.nb_comp[ic]){
	    case 1: 
	      *fic>>ival>>val1;
	      break;
	    case 2: 
	      *fic>>ival>>val1>>val2;
	      break;
	    case 3: 
	      *fic>>ival>>val1>>val2>>val3;
	      break;
	    default:
	      break;
	    }
	    if(ival != n+1){
	      printf("Probleme numeros pour %s temps : %10.5e\n", nom_champ, time);
	      printf("Nombre de valeurs=%d Nombre de mailles=%d\n",ival,n+1);
	      return (ERROR);
	    }

	    switch(contenu->champ.nb_comp[ic])
	      {
	      case 1:
		contenu->champ.vec[ic].comp1[n] = val1;
		if( type == SEQUENTIEL){
		  if(fabs(val1) >= gvalm[ic]) gvalm[ic] = fabs(val1);
		}
		break;
	      case 2:
		contenu->champ.vec[ic].comp1[n] = val1;
		contenu->champ.vec[ic].comp2[n] = val2;
		if( type == SEQUENTIEL){
		  if(fabs(val1) >= gvalm[ic]) gvalm[ic] = fabs(val1);
		  if(fabs(val2) >= gvalm[ic]) gvalm[ic] = fabs(val2);
		}
		break;
	      case 3:
		contenu->champ.vec[ic].comp1[n] = val1;
		contenu->champ.vec[ic].comp2[n] = val2;
		contenu->champ.vec[ic].comp3[n] = val3;
		if( type == SEQUENTIEL){
		  if(fabs(val1) >= gvalm[ic]) gvalm[ic] = fabs(val1);
		  if(fabs(val2) >= gvalm[ic]) gvalm[ic] = fabs(val2);
		  if(fabs(val3) >= gvalm[ic]) gvalm[ic] = fabs(val3);
		}
		break;
	      default:
		break;
	      }
	    // GF strstr portable ? la modif permet de faire des comparaisons correctes si le champ s-appelle vitesse_ai par exemple
	    if (contenu->champ.nb_comp[ic]==1 && ((strncmp(nom_champ,"vitesseY_elem",13)==0)||((ic>0)&&(strstr(nom_champ,"Y_elem_")!=NULL)&&(strstr(nom_champ,"vitesse")||strstr(nom_champ,"VITESSE")))))
	      {
		/* Cas ou vitesse elem */
		/*gvalm[ic]=gvalm[ic-1];*/
		if (gvalm[ic-1]>gvalm[ic])
		  gvalm[ic]=gvalm[ic-1];
		else
		  gvalm[ic-1]=gvalm[ic];
	      }
	    if (contenu->champ.nb_comp[ic]==1 && ((strncmp(nom_champ,"vitesseZ_elem",13)==0)||((ic>1)&&(strstr(nom_champ,"Z_elem_")!=NULL)&&(strstr(nom_champ,"vitesse")||strstr(nom_champ,"VITESSE")))))
	      {
		if (gvalm[ic-1]>gvalm[ic])
		  gvalm[ic]=gvalm[ic-1];
		else
		  gvalm[ic-1]=gvalm[ic];
		if (gvalm[ic-2]>gvalm[ic])
		  gvalm[ic]=gvalm[ic-2];
		else
		  gvalm[ic-2]=gvalm[ic];
		if (gvalm[ic-2]>gvalm[ic-1])
		  gvalm[ic-1]=gvalm[ic-2];
		else
		  gvalm[ic-2]=gvalm[ic-1];		  		  
	      }
	    
	    if( type == PARALLELE){
	      ic2 = 0;
	      while((ic2<contenu_res.nb_champ) &&(strcasecmp(contenu_res.champ.nom_champ[ic2], nom_champ) != 0)) ic2++;
	      if( ic2 == contenu_res.nb_champ)
		{
		  cerr<<nom_champ<<" non trouve dans le fichier "<< contenu_res.file_name<<  endl;
		  exit(-1);
		}
	      //cerr<<"ici "<<nom_champ<<" "<<contenu_res.champ.nom_champ[ic2]<<contenu->champ.nom_champ[ic]<<" ic "<<ic<<" ic2 "<<ic2<<endl;
	      if(contenu->champ.nelem[ic] == contenu->nb_maille[igrille])
		{  
		  strcpy(type_case, "Element");
		  no = equiv[igrille].ielem[n];
		}
	      else if(contenu->champ.nelem[ic] == contenu->nb_noeud[igrille])
		{	
		  strcpy(type_case, "Sommet");
		  no = equiv[igrille].iseq[n];
		}
	      
	      valmin = -1.e-15;
	      double v1,v2;
	      switch(contenu->champ.nb_comp[ic])
		{
		case 1:
		  v1=contenu->champ.vec[ic].comp1[n];
		  v2=contenu_res.champ.vec[ic2].comp1[no];
		  if((gvalm[ic2]>valmin) && (Ecart(v1,v2,gvalm[ic2]))){
		    if(fabs(gerr) >= em ) {em = gerr;nom=no;nm=n;x=contenu->champ.vec[ic].comp1[n];y=contenu_res.champ.vec[ic2].comp1[no];}
		  }
		  break;
		case 2:
		  if((gvalm[ic2]>valmin) && (Ecart(contenu->champ.vec[ic].comp1[n],
						  contenu_res.champ.vec[ic2].comp1[no], gvalm[ic2]))){
		    if(fabs(gerr) >= em) {em = gerr;nom=no;nm=n;x=contenu->champ.vec[ic].comp1[n];y=contenu_res.champ.vec[ic2].comp1[no];}
		  }
		  if((gvalm[ic2]>valmin) && (Ecart(contenu->champ.vec[ic].comp2[n],
						  contenu_res.champ.vec[ic2].comp2[no],
						  gvalm[ic2]))){
		    if(fabs(gerr) >= em ) {em = gerr;nom=no;nm=n;x=contenu->champ.vec[ic].comp2[n];y=contenu_res.champ.vec[ic2].comp2[no];}
		  }
		  break;
		case 3:
		  if((gvalm[ic2]>valmin) && (Ecart(contenu->champ.vec[ic].comp1[n],
						  contenu_res.champ.vec[ic2].comp1[no],
						  gvalm[ic2]))){
		    if(fabs(gerr) >= em ) {em = gerr;nom=no;nm=n;x=contenu->champ.vec[ic].comp1[n];y=contenu_res.champ.vec[ic2].comp1[no];}
		  }
		  if((gvalm[ic2]>valmin) && (Ecart(contenu->champ.vec[ic].comp2[n],
						  contenu_res.champ.vec[ic2].comp2[no],
						  gvalm[ic2]))){
		    if(fabs(gerr) >= em ) {em = gerr;nom=no;nm=n;x=contenu->champ.vec[ic].comp2[n];y=contenu_res.champ.vec[ic2].comp2[no];}
		  }
		  if((gvalm[ic2]>valmin) && (Ecart(contenu->champ.vec[ic].comp3[n],
						  contenu_res.champ.vec[ic2].comp3[no],
						  gvalm[ic2]))){
		    if(fabs(gerr) >= em ) {em = gerr;nom=no;nm=n;x=contenu->champ.vec[ic].comp3[n];y=contenu_res.champ.vec[ic2].comp3[no];}
		  }
		  break;
		}
	    }
	  }
       
	    
	  if (type == PARALLELE)
	    {
	      if (em>0.)
		{			
		  printf("Ecarts pour %s au temps:%15.8e Erreur max:%10.3e %s seq %d=%15.8e %s par %d=%15.8e gmax %15.8e\n",
			 nom_champ, time, em, type_case, nom, y, type_case, nm, x, gvalm[ic2]);
		  gnerr++;
		  em = 0.0;
		}
	      else if (gnerr!=-1)
		{
		  printf("    OK pour %s au temps:%15.8e gmax %15.8e \n",
			 nom_champ, time,gvalm[ic2]);
		}
	    }

	  fic_sauve = (*fic).tellg();
	  break;
	case TEMPS :
	  /* On a trouve le temps suivant */
	  (*fic).seekg(fic_sauve);
	  delete [] bufcpp;
	  return (OK);
	  break;
	case FIN :
	  /* On a trouve le mot FIN */
	  (*fic).seekg(fic_sauve);;
	  delete [] bufcpp;
	  return (OK);
	  break;
	default :
	  break;
	}
    }
  delete [] bufcpp;
  return (ERROR);
}

double xmax = 0.0;
double ymax = 0.0;
double zmax = 0.0;
double** xp;
int *index_sort;
int compare_elem(const void* ii1,const void* ii2)
{
  const int i1=*(const int*) ii1;
  const int i2=*(const int*) ii2;
  if (xp[i1][0]<xp[i2][0]-gepsilon*xmax)
    return -1;
  else  
    if (xp[i1][0]>xp[i2][0]+gepsilon*xmax)
      return 1;
    else
      if (xp[i1][1]<xp[i2][1]-gepsilon*ymax)
	return -1;
      else  
	if (xp[i1][1]>xp[i2][1]+gepsilon*ymax)
	  return 1;
	else 
	  if (xp[i1][2]<xp[i2][2]-gepsilon*zmax)
	    return -1;
	  else  
	    if (xp[i1][2]>xp[i2][2]+gepsilon*zmax)
	      return 1;
	    else
	      return 0;
}
int compare_elem_seq_par(int i1,double *pos)
{
  if (xp[i1][0]<pos[0]-gepsilon*xmax)
    return -1;
  else  
    if (xp[i1][0]>pos[0]+gepsilon*xmax)
      return 1;
    else
      if (xp[i1][1]<pos[1]-gepsilon*ymax)
	return -1;
      else  
	if (xp[i1][1]>pos[1]+gepsilon*ymax)
	  return 1;
	else 
	  if (xp[i1][2]<pos[2]-gepsilon*zmax)
	    return -1;
	  else  
	    if (xp[i1][2]>pos[2]+gepsilon*zmax)
	      return 1;
	    else
	      return 0;
}

/****************************************************************
 * Fonction  Verif_Geometrie(contenu1, contenu2)                 *
 *****************************************************************
 *                                                               *
 * Fonction permettant de verifier la coherence des structures   *
 * issues des 2 fichiers de resultats  Triou.                    *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * T_Trio   contenu1  - Structure contenant les informations     *
 *                       de la geometrie du premier fichier Triou*
 * T_Trio   contenu2  - Structure contenant les informations     *
 *                       de la geometrie du second fichier Triou *
 *---------------------------------------------------------------*
 * Sorties :                                                     *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.DEGENEVE                                          *
 * Modifcation G Fauchet                                         *
 *       possibilte d'avoir les grilles dans un sens different   *
 *       dans les deux fichiers                                  *
 * Creation : 06/08/02                                           *
 ****************************************************************/
int Verif_Geometrie(T_Trio contenu1, T_Trio contenu2)
  

{
  if(contenu1.nb_grille != contenu2.nb_grille){
    printf("Le premier fichier n'a pas le meme nombre de grilles que le second\n");
    printf("%d %d\n",contenu1.nb_grille,contenu2.nb_grille);
    return (ERROR);
  } 
     
  /* on cree un tableau de correspondance */
  /* corres[i] donne le numero de la grille i du fichier 1 dans le fichier 2 */
  int* corres;
  corres=new int[contenu1.nb_grille];
  for(int i = 0; i < contenu1.nb_grille; i++)
    {
      int marq=0;
      for(int j = 0; j < contenu1.nb_grille; j++)
	{
	  if(strcmp(contenu1.nom_grille[i], contenu2.nom_grille[j]) == 0)
	    {
	      marq++;
	      corres[i]=j;
	    }
	}
      if (marq!=1) 
	{
	  printf("pb pour la grille %s est contenue %d fois dans le deuxieme fichier\n",contenu1.nom_grille[i],marq);
	  //cerr<<"pb pour la grille "<<contenu1.nom_grille[i]<<" est contenue "<<marq<<" fois dnas le deuxieme fichier"<<endl;
	  return (ERROR);
	}
    }
      
  /* On verifie la topologie sur chaque grille */
  for(int i = 0; i < contenu1.nb_grille; i++){
    if(strcmp(contenu1.nom_grille[i], contenu2.nom_grille[corres[i]]) != 0){
      printf("Grille : %s Le premier fichier n'a pas le meme nom de grille que le second\n",
	     contenu1.nom_grille[i]);
      return (ERROR);
    }
  }
  for(int i = 0; i < contenu1.nb_grille; i++){
    if(strcmp(contenu1.nom_grille[i], contenu1.topologie[i].nom_grille) != 0){
      printf("Grille : %s Topologie : %s\nLe nom de la grille relative a cette topologie n'est pas exact\n",
	     contenu1.nom_grille[i], contenu1.topologie[i].nom_topo);
      return (ERROR);
    }
  }
  /* On verifie la dimension du probleme sur chaque grille */
  for(int i = 0; i < contenu1.nb_grille; i++){
    if(contenu1.dim_grille[i] != contenu2.dim_grille[corres[i]]){
      printf("Grille : %s La dimension du probleme n'est pas la meme dans le premier fichier que dans le second\n",
	     contenu1.nom_grille[i]);
      return (ERROR);
    }
  }
  /* On verifie que tous les sommets sont identiques sur chaque grille */
  xmax = 0.0;
  ymax = 0.0;
  zmax = 0.0;
  for(int i = 0; i < contenu1.nb_grille; i++){
    for(int j = 0; j < contenu1.nb_noeud[i]; j++){
      if(fabs(contenu1.coord1[i].x[j]) >= xmax) xmax = fabs(contenu1.coord1[i].x[j]);
      if(fabs(contenu1.coord1[i].y[j]) >= ymax) ymax = fabs(contenu1.coord1[i].y[j]);
      if(fabs(contenu1.coord1[i].z[j]) >= zmax) zmax = fabs(contenu1.coord1[i].z[j]);
    }
  }
  // cherche_element
  for(int i = 0; i < contenu2.nb_grille; i++){
    //isto = 0;
    int i2=corres[i];
        
    xp=new double*[contenu1.nb_maille[i]];
    for (int ii=0;ii<contenu1.nb_maille[i];ii++)
      xp[ii]=new double[3];
    int nb_som_elem=0;
    for (int ii=0;ii<connect_max;ii++)
      if (contenu1.pu[i].lmaille[0].connect[ii] != 0) nb_som_elem=ii;
    nb_som_elem++;
    index_sort=new int[contenu1.nb_maille[i]];
    // calcul du centre de gravite de la grille seq
    for(int k = 0; k < contenu1.nb_maille[i]; k++)
      {
	index_sort[k]=k;
	for (int ii=0;ii<3;ii++)
	  xp[k][ii]=0;
	for (int ii=0;ii<nb_som_elem;ii++)
	  {
	    int som=contenu1.pu[i].lmaille[k].connect[ii]-1;
	    xp[k][0]+=contenu1.coord1[i].x[som];
	    xp[k][1]+=contenu1.coord1[i].y[som];
	    xp[k][2]+=contenu1.coord1[i].z[som];
	  }
	for (int ii=0;ii<3;ii++)
	  xp[k][ii]/=nb_som_elem;
      }
    // tri de la grille seq
    qsort(index_sort,contenu1.nb_maille[i],sizeof(int),compare_elem);
    int nb_elem_1=contenu1.nb_maille[i];
	
    for (int ii=0;ii<contenu1.nb_maille[i]*0;ii++)
      cerr<<index_sort[ii]<<" "<<xp[index_sort[ii]][0]<< " "<<xp[index_sort[ii]][1]<<" "<<xp[index_sort[ii]][2]<<endl;
    equiv[i2].ielem =(int* ) malloc (sizeof(int) * contenu2.nb_maille[i2]);

    equiv[i2].iseq =(int *) malloc (sizeof(int ) * contenu2.nb_noeud[i2]);
    for(int j = 0; j < contenu2.nb_noeud[i2]; j++)
      equiv[i2].iseq[j] = -1;
    // pour chaque element de 2 on fait une dichotomie
	
    for (int j=0; j<contenu2.nb_maille[i2]; j++) {
      //cerr<<((double)j)/contenu2.nb_maille[i2]<<endl;
      equiv[i2].ielem[j]= -1;
      double pos[3];
      for (int ii=0;ii<3;ii++)
	pos[ii]=0;
      for (int ii=0;ii<nb_som_elem;ii++)
	{
	  int som=contenu2.pu[i2].lmaille[j].connect[ii]-1;
	  pos[0]+=contenu2.coord1[i2].x[som];
	  pos[1]+=contenu2.coord1[i2].y[som];
	  pos[2]+=contenu2.coord1[i2].z[som];
	}
      for (int ii=0;ii<3;ii++)
	pos[ii]/=nb_som_elem;
      // equiv[i2].ielem[j]= -1;
	  
      int deb=0;
      int fin=nb_elem_1-1;
      while ((fin-deb)>0)
	{
	  //assert(fin-deb>0); 
	  int mil=(fin+deb)/2;
	     
	  int test=compare_elem_seq_par(index_sort[mil],pos);
	  //cerr<<j<<" test "<<deb<<" "<<mil<<" "<<fin<<" "<<index_sort[mil]<<endl;
	  if (test>0)
	    {
	      fin=mil-1;
	    }
	  else if (test<0)
	    {
	      deb=mil+1;
	    }
	  else
	    {
	      fin=mil;
	      deb=mil;
		 
	    }
	}
	  
      if (fin-deb<0)
	{
	      
	  fin=deb;
	}
      if (fin-deb)
	{
	     
	  int test=compare_elem_seq_par(index_sort[fin],pos);
	  if (test==0) { deb=fin;}
	  else
	    {
	      test=compare_elem_seq_par(index_sort[deb],pos);
	      if (test==0) { fin=deb;}
	      else { cerr<<deb<<" "<<fin<<" pb"<<endl;exit(-1);};
	    }
	}
      int corres_l=index_sort[deb];
	  
      if ((Ecart(xp[corres_l][0], pos[0], xmax))
	  ||(Ecart(xp[corres_l][1], pos[1], ymax))
	  ||(Ecart(xp[corres_l][2], pos[2], zmax)))
	{
	  if (0)
	    {
	      cerr<<deb<<" "<<corres_l<<endl; 
	      for(int ii=0;ii<3;ii++)
		{
		  cerr<<"pos "<<xp[corres_l][ii]<<" "<<pos[ii]<<endl;
		}
	      cerr<<" pb "<<j<<endl;//exit(-1);
	    }
	  corres_l=-1;
	}
      // si on a pas trouve par l amethode optimisee on essaye toutes les mailles , mais avant on essaye le suivant du precedent
      if (corres_l==-1)
	{
	  int k=j;
	  if (j>0)
	    k=equiv[i2].ielem[j-1]+1;
	       
	  if (!((Ecart(xp[k][0], pos[0], xmax))
		||(Ecart(xp[k][1], pos[1], ymax))
		||(Ecart(xp[k][2], pos[2], zmax))))	
	    {
	      corres_l=k;
	    }
	}
      if (corres_l==-1)
	{
	      
	  for (int k=0;k<contenu1.nb_maille[i2];k++)
	    if (!((Ecart(xp[k][0], pos[0], xmax))
		  || (Ecart(xp[k][1], pos[1], ymax))
		  ||(Ecart(xp[k][2], pos[2], zmax))))
	      {
		corres_l=k;
		break;
	      }
	}
      equiv[i2].ielem[j]=corres_l;
      // On a trouve l element sequentiel associe
      // On cherche maintenant les sommets 
      if (corres_l!=-1)
	for (int s_par=0;s_par<nb_som_elem;s_par++)
	  {
	    int som_par=contenu2.pu[i2].lmaille[j].connect[s_par]-1;
	    if ( equiv[i2].iseq[som_par]==-1)
	      for (int s_seq=0;s_seq<nb_som_elem;s_seq++)
		{
		  int som_seq=contenu1.pu[i].lmaille[corres_l].connect[s_seq]-1;
		  if(!Ecart(contenu1.coord1[i].x[som_seq], contenu2.coord1[i2].x[som_par], xmax))
		    if(!Ecart(contenu1.coord1[i].y[som_seq], contenu2.coord1[i2].y[som_par], ymax))
		      if(!Ecart(contenu1.coord1[i].z[som_seq], contenu2.coord1[i2].z[som_par], zmax))
			{
			  equiv[i2].iseq[som_par]=som_seq;
			  break;
			}
		}
	  }
    }
      
    for (int ii=0;ii<contenu1.nb_maille[i];ii++)
      delete [] xp[ii];
    delete [] xp;
    delete [] index_sort;
	
  }
  //cerr<<"ouf"<<endl;
  // exit(-1);
  /* avant iequiv[i] maintenant iqueiv[corrs[i]] */
  /*printf("xmax=%lf ymax=%lf zmax=%lf\n", xmax, ymax, zmax);*/
  for(int i = 0; i < contenu2.nb_grille; i++){
    //isto = 0;
    int i2=corres[i];
    //equiv[i2].iseq =(int *) malloc (sizeof(int ) * contenu2.nb_noeud[i2]);
    for(int j = 0; j < contenu2.nb_noeud[i2]; j++){
      if (equiv[i2].iseq[j] == -1){
	if (0)
	  cerr<< " sommet "<<j<<" pas trouve avec la nouvelle methode"<<endl;
	// le tri sur les xp peut etre un peu faux
	// si on a pas trouve par la methode en log n
	// on essaye la methode en n (mais sur pas trop de case)


	// equiv[i2].iseq[j] = -1;
	int loc=-1;
	for(int k = 0; k < contenu1.nb_noeud[i]; k++)
	  if(!Ecart(contenu1.coord1[i].x[k], contenu2.coord1[i2].x[j], xmax))
	    if(!Ecart(contenu1.coord1[i].y[k], contenu2.coord1[i2].y[j], ymax))
	      if(!Ecart(contenu1.coord1[i].z[k], contenu2.coord1[i2].z[j], zmax)){
		loc = k;
		   
		    
		break;
	      }
	    
	    
	equiv[i2].iseq[j]=loc;
	if(equiv[i2].iseq[j] == -1){
	  printf(" Pas trouve le sommet %d Topologie : %s Grille : %s\n",
		 j+1, contenu2.topologie[i2].nom_topo, contenu2.nom_grille[i2]);
	  return (ERROR);
	}
      }
      /*printf("j i equiv[i2].iseq[j] isto= %d %d %d %d\n", j, i, equiv[i2].iseq[j], isto);*/
    }
    //cerr<<"ouf2"<<endl;
  }
  gemax = 0;         /* Erreur maximum rencontree */
  /* On verifie le nombre de mailles sur chaque grille */
  for(int i = 0; i < contenu1.nb_grille; i++){
    int i2=corres[i];
    if(contenu1.nb_maille[i] != contenu2.nb_maille[i2]){
      printf("Grille : %s Le nombre de mailles n'est pas le meme dans le premier fichier que dans le second\n",
	     contenu1.nom_grille[i]);
      return (ERROR);
    }
  }
  /* On verifie qu'on trouve les memes elements sur chaque grille */
  for(int i = 0; i < contenu2.nb_grille; i++){
    //isto = 0;
    int i2=corres[i];
    //equiv[i2].ielem =(int* ) malloc (sizeof(int) * contenu2.nb_maille[i2]);
    for (int j=0; j<contenu2.nb_maille[i2]; j++) {
      if (equiv[i2].ielem[j]==-1) {
	if (0)
	  cerr<<"element "<<j<<" pas trouve avec la nouvelle methode"<<endl;
	// le tri sur les xp peut etre un peu faux
	// si on a pas trouve par la methode en log n
	// on essaye la methode en n (mais sur pas trop de case)
	    
	//equiv[i2].ielem[j]= -1;
	for(int k = 0; k < contenu1.nb_maille[i]; k++)
	  {
	    int m=1;
	    for (int ii=0;ii<connect_max;ii++){
	      if (contenu1.pu[i].lmaille[k].connect[ii] == 0) break;
	      if (contenu1.pu[i].lmaille[k].connect[ii] !=
		  equiv[i2].iseq[contenu2.pu[i2].lmaille[j].connect[ii]-1]+1){ m=0;break; }
	    }
	    if (m==1) {
	      equiv[i2].ielem[j]=k;
		  
	      break; }
	  }
          
	/*printf("j i equiv[i2].ielem[j] isto= %d %d %d %d\n", j, i, equiv[i2].ielem[j], isto);*/
	if (equiv[i2].ielem[j]==-1) {
	  printf("pas trouve l elt %d\n",j+1);
	  for (int ii=0;ii<connect_max;ii++)
	    printf("%d ", contenu2.pu[i2].lmaille[j].connect[ii]);
	  printf("\n");
	  return (ERROR);
	}
      }
    }
    // cerr<<"ouf3"<<endl;
  }
  delete [] corres;
  return (OK);
}

double trouve_dernier_temps(char* nom1)
{
 
  ifstream fic(nom1);
  if (!fic){
    printf ("Erreur a l'ouverture du fichier %s\n", nom1);
    return (ERROR);
  }
  String80  motcle;
  char      buff[BUFSIZ + 1];
  double    t;
  int       imot;
  while (fic)
    { 
      fic >> buff;
      /* Lecture d'une ligne du fichier resultats */
      //    Ote_blanc(buff);
      Extraire_motcle_corr(buff, g_nb_motcle, g_liste_motcle,motcle);
      imot = Cherche_motcle(motcle, g_nb_motcle, g_liste_motcle);
      /*printf("imot %d %s\n",imot,buff);*/
      switch(imot)
	{
	case ERROR1:
	  /* Le mot cle n'a pas ete trouve c'est une ligne de resultat a ne pas exploiter*/
	  break;
	case TEMPS:
	  fic >> t;		
	  break;
	case FIN:
	  return t;
	  break;
	default :
	  break;
	}
    }
  exit(-1);
  return t;
}

/****************************************************************
 * Fonction  Verif_Result_statio(nom1, nom2, contenu1, contenu2)        *
 *****************************************************************
 *                                                               *
 * Fonction permettant de lire des fichiers de resultats Triou   *
 * en vue de  comparer le dernier temps .                                       *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * char   *nom1 -Pointeur sur le nom du fichier calcul seqentiel *
 *                      avec son chemin en absolu                *
 * char   *nom2 -Pointeur sur le nom du fichier calcul parallele *
 *                      avec son chemin en absolu                *
 * T_Trio   *contenu1  - Pointeur sur la structure contenant les *
 *                       informations contenues dans             *
 *                       un fichier de resultats TRIOU.          *
 * T_Trio   *contenu1  - Pointeur sur la structure contenant les *
 *                       informations contenues dans             *
 *                       un fichier de resultats TRIOU.          *
 *---------------------------------------------------------------*
 * Sorties :                                                     *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.FAUCHET                                         *
 * Creation :  07/2005                                           *
 ****************************************************************/
int Verif_Result_statio(char *nom1, char *nom2, T_Trio *contenu1, T_Trio *contenu2)
{
  /******* lecture du fichier sequentiel   *******/
  ifstream fic1(nom1);
  if (!fic1){
    printf ("Erreur a l'ouverture du fichier %s\n", nom1);
    return (ERROR);
  }
  /******* lecture du fichier parallele   *******/
  ifstream fic2(nom2);
  if (!fic2){
    printf ("Erreur a l'ouverture du fichier %s\n", nom2);
    return (ERROR);
  }
  double temps_seq=trouve_dernier_temps(nom1);
  double temps_par=trouve_dernier_temps(nom2);
  cerr<<" derniers temps "<<temps_seq<<" " <<temps_par<<endl;
  if(Recherche_temps(&fic1, contenu1, temps_seq,1) == ERROR){
    printf("Erreur a la lecture des temps calcul seq temps %10.5e non trouve\n", temps_seq);
    exit(1);
  }
  if(Recherche_temps(&fic2, contenu2, temps_par,1) == ERROR){
    printf("Erreur a la lecture des temps calcul parallele temps %10.5e non trouve\n", temps_par);
    exit(1);
  }
  double time_seq=temps_seq;
  int type_fich = SEQUENTIEL;
  int k1=1;
  if(Lecture_Result(&fic1, type_fich, contenu1, k1, time_seq) == ERROR){
    printf("Erreur a la lecture des resultats calcul sequentiel temps %10.5e\n", time_seq);
    exit(1);
  } 
  time_seq=temps_par;
  type_fich=PARALLELE;
  if(Lecture_Result(&fic2, type_fich, contenu2, k1, time_seq) == ERROR){
    printf("Erreur a la lecture des resultats calcul parallele temps %10.5e\n", time_seq);
    exit(1);
  }
  gnerr++;
  printf("Number of differences : %d\n", gnerr);
  printf("Maximal relative error encountered : %10.5e\n", gemax);
  return gnerr;
}
/****************************************************************
 * Fonction  Verif_Result(nom1, nom2, contenu1, contenu2)        *
 *****************************************************************
 *                                                               *
 * Fonction permettant de lire des fichiers de resultats Triou   *
 * en vue de les comparer.                                       *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * char   *nom1 -Pointeur sur le nom du fichier calcul seqentiel *
 *                      avec son chemin en absolu                *
 * char   *nom2 -Pointeur sur le nom du fichier calcul parallele *
 *                      avec son chemin en absolu                *
 * T_Trio   *contenu1  - Pointeur sur la structure contenant les *
 *                       informations contenues dans             *
 *                       un fichier de resultats TRIOU.          *
 * T_Trio   *contenu1  - Pointeur sur la structure contenant les *
 *                       informations contenues dans             *
 *                       un fichier de resultats TRIOU.          *
 *---------------------------------------------------------------*
 * Sorties :                                                     *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.DEGENEVE                                          *
 * Creation :  1/08/02                                           *
 ****************************************************************/
int Verif_Result(char *nom1,char * nom2, T_Trio *contenu1, T_Trio *contenu2)  
{
  /* pointeur sur fichier de resultats */
  int k1=0;
  String80  motcle;
  int       fin;
  int       imot;
  double    time_seq, time_par;
  int       type_fich;

  fin=0;
  char* bufcpp=new char [BUFSIZ];
  /* Premiere boucle de lecture */
  /* On verifie que l'on a le nombre de temps dans le fichier parallele que sequentiel */
  ifstream tmp1(nom1);
  if (!tmp1){
    printf ("Error when opening file %s\n", nom1);
    return (ERROR);
  }
  ifstream tmp2(nom2);
  if (!tmp2){
    printf ("Error when opening file %s\n", nom2);
    return (ERROR);
  }  
  while (tmp2){
    tmp2 >> bufcpp;
    Extraire_motcle_corr(bufcpp, g_nb_motcle, g_liste_motcle,motcle);
    imot = Cherche_motcle(motcle, g_nb_motcle, g_liste_motcle);
    switch(imot){
    case ERROR1:
      break;
    case TEMPS:
      tmp2 >> time_par;
      if(Recherche_temps(&tmp1, contenu1, time_par) == ERROR){
	printf("Time %10.5e not found in file %s\n", time_par, nom1);
	exit(1);
      }  
      break;
    default :
      break;
    }    
  }
     
  /* Deuxieme boucle de lecture */
  /******* lecture du fichier sequentiel   *******/
  ifstream fic1(nom1);
  if (!fic1){
    printf ("Error when opening file %s\n", nom1);
    return (ERROR);
  }
  /******* lecture du fichier parallele   *******/
  ifstream fic2(nom2);
  if (!fic2){
    printf ("Error when opening file %s\n", nom2);
    return (ERROR);
  }
  while (fic1){
	  
    fic1 >> bufcpp;

    Extraire_motcle_corr(bufcpp, g_nb_motcle, g_liste_motcle,motcle);
    imot = Cherche_motcle(motcle, g_nb_motcle, g_liste_motcle);
    switch(imot){
    case ERROR1:
      /* Le mot cle n'a pas ete trouve c'est une ligne de resultat a ne pas exploiter*/
      break;
    case TEMPS:
      /* On a trouve un nouveau temps dans le fichier resultats calcul sequentiel */
      fic1 >> time_seq;
      k1++;
      /* Lecture des resultats au temps k1 jusqu'au temps suivant ou le motcle FIN */
      type_fich = SEQUENTIEL;
      if(Lecture_Result(&fic1, type_fich, contenu1, k1, time_seq) == ERROR){
	printf("Time %10.5e not found in file %s\n", time_seq, nom1);
	exit(1);
      }
      /* Recherche du meme temps dans le fichier resultats calcul parallele*/
      if(Recherche_temps(&fic2, contenu2, time_seq) == ERROR){
	printf("Time %10.5e not found in file %s\n", time_seq, nom2);
	exit(1);
      }

      /* Lecture des resultats au temps k1 jusqu'au temps suivant ou le motcle FIN */
      /* Traitement des differences pour les variables au meme temps */
      type_fich = PARALLELE;

      if(Lecture_Result(&fic2, type_fich, contenu2, k1, time_seq) == ERROR){
	printf("Error when reading results at time %10.5e in the file %s\n", time_seq, nom2);
	exit(1);
      }
      break;
    case FIN :
      /* On a trouve la Fin du fichier */
      if (!fin)
	{
	  fin=1;
	  gnerr++;
	  printf("Number of differences : %d\n", gnerr);
	  printf("Maximal relative error encountered : %10.5e\n", gemax);
	}
      break;
    default :
      break;
    }
  } /* Fin lecture des lignes du fichier */
  if(gnerr == -1){
    printf("Trio-U stops. May be a .lml file empty or keyword FIN not found inside.\n");
  }
  fic1.close();
  fic2.close();
  if(mode_debug){
  }
  delete [] bufcpp;
  return gnerr;
}

/****************************************************************
 * Fonction  libere_trio(contenu1, contenu1, equi)               *
 *****************************************************************
 *                                                               *
 * Fonction  permettant de liberer les pointeurs alloues a la    *
 * structure contenant les donnees Trio .                        *
 *                                                               *
 *---------------------------------------------------------------*
 * Entrees :                                                     *
 * T_Trio   contenu1  - Structure fichier resultat calcul        *
 *                      sequentiel TRIOU.                        *
 * T_Trio   contenu2  - Structure fichier resultat calcul        *
 *                      parallele  TRIOU.                        *
 * T_Equiv   *equi    - Structure correspondance sommets et      *
 *                      mailles entre calcul parallele et        *
 *                      calcul sequentiel TRIOU.                 *
 *---------------------------------------------------------------*
 * Sorties :                                                     *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.DEGENEVE                                          *
 * Creation : 06/08/02                                           *
 ****************************************************************/
int libere_trio(T_Trio contenu1,T_Trio  contenu2,T_Equiv   *equi)
{

  /* Liberation de la Structure de correspondance sommets et mailles entre calcul parallele et 
     calcul sequentiel TRIOU.*/
  for(int i = 0; i < contenu2.nb_grille; i++){
    if(equi[i].ielem != NULL){
      free(equi[i].ielem);
      equi[i].ielem = NULL;
    }
    if(equi[i].iseq != NULL){
      free(equi[i].iseq);
      equi[i].iseq = NULL;
    }
  }

  /* Liberation de la Structure du fichier resultat calcul sequentiel TRIOU.*/
  for (int i =0; i < contenu1.nb_grille; i++){
    if(contenu1.coord1[i].x != NULL){
      free(contenu1.coord1[i].x);
      contenu1.coord1[i].x = NULL;
    }
    if(contenu1.coord1[i].y != NULL){
      free(contenu1.coord1[i].y);
      contenu1.coord1[i].y = NULL;
    }
    if(contenu1.coord1[i].z != NULL){
      free(contenu1.coord1[i].z);
      contenu1.coord1[i].z = NULL;
    }
    if(contenu1.pu[i].lmaille != NULL){
      free(contenu1.pu[i].lmaille);
      contenu1.pu[i].lmaille = NULL;
    }
  }

  for (int i =0; i < contenu1.nb_champ; i++){
    switch(contenu1.champ.nb_comp[i]){
    case 1:
      if(contenu1.champ.vec[i].comp1 != NULL){
	free(contenu1.champ.vec[i].comp1);
	contenu1.champ.vec[i].comp1 = NULL;
      }
      break;
    case 2:
      if(contenu1.champ.vec[i].comp1 != NULL){
	free(contenu1.champ.vec[i].comp1);
	contenu1.champ.vec[i].comp1 = NULL;
      }
      if(contenu1.champ.vec[i].comp2 != NULL){
	free(contenu1.champ.vec[i].comp2);
	contenu1.champ.vec[i].comp2 = NULL;
      }
      break;
    case 3:
      if(contenu1.champ.vec[i].comp1 != NULL){
	free(contenu1.champ.vec[i].comp1);
	contenu1.champ.vec[i].comp1 = NULL;
      }
      if(contenu1.champ.vec[i].comp2 != NULL){
	free(contenu1.champ.vec[i].comp2);
	contenu1.champ.vec[i].comp2 = NULL;
      }
      if(contenu1.champ.vec[i].comp3 != NULL){
	free(contenu1.champ.vec[i].comp3);
	contenu1.champ.vec[i].comp3 = NULL;
      }
      break;
    }
  }
    
  if(contenu1.champ.nom_champ != NULL){
    free(contenu1.champ.nom_champ);
    contenu1.champ.nom_champ = NULL;
  }
  if(contenu1.champ.nb_comp != NULL){
    free(contenu1.champ.nb_comp);
    contenu1.champ.nb_comp = NULL;
  }
  if(contenu1.champ.nelem != NULL){
    free(contenu1.champ.nelem);
    contenu1.champ.nelem = NULL;
  }
  if(contenu1.champ.vec != NULL){
    free(contenu1.champ.vec);
    contenu1.champ.vec = NULL;
  }

  /* Liberation de la Structure du fichier resultat calcul parallele TRIOU.*/
  for (int i =0; i < contenu2.nb_grille; i++){
    if(contenu2.coord1[i].x != NULL){
      free(contenu2.coord1[i].x);
      contenu2.coord1[i].x = NULL;
    }
    if(contenu2.coord1[i].y != NULL){
      free(contenu2.coord1[i].y);
      contenu2.coord1[i].y = NULL;
    }
    if(contenu2.coord1[i].z != NULL){
      free(contenu2.coord1[i].z);
      contenu2.coord1[i].z = NULL;
    }
    if(contenu2.pu[i].lmaille != NULL){
      free(contenu2.pu[i].lmaille);
      contenu2.pu[i].lmaille = NULL;
    }
  }

  for (int i =0; i < contenu2.nb_champ; i++){
    switch(contenu2.champ.nb_comp[i]){
    case 1:
      if(contenu2.champ.vec[i].comp1 != NULL){
	free(contenu2.champ.vec[i].comp1);
	contenu2.champ.vec[i].comp1 = NULL;
      }
      break;
    case 2:
      if(contenu2.champ.vec[i].comp1 != NULL){
	free(contenu2.champ.vec[i].comp1);
	contenu2.champ.vec[i].comp1 = NULL;
      }
      if(contenu2.champ.vec[i].comp2 != NULL){
	free(contenu2.champ.vec[i].comp2);
	contenu2.champ.vec[i].comp2 = NULL;
      }
      break;
    case 3:
      if(contenu2.champ.vec[i].comp1 != NULL){
	free(contenu2.champ.vec[i].comp1);
	contenu2.champ.vec[i].comp1 = NULL;
      }
      if(contenu2.champ.vec[i].comp2 != NULL){
	free(contenu2.champ.vec[i].comp2);
	contenu2.champ.vec[i].comp2 = NULL;
      }
      if(contenu2.champ.vec[i].comp3 != NULL){
	free(contenu2.champ.vec[i].comp3);
	contenu2.champ.vec[i].comp3 = NULL;
      }
      break;
    }
  }

  if(contenu2.champ.nom_champ != NULL){
    free(contenu2.champ.nom_champ);
    contenu2.champ.nom_champ = NULL;
  }
  if(contenu2.champ.nb_comp != NULL){
    free(contenu2.champ.nb_comp);
    contenu2.champ.nb_comp = NULL;
  }
  if(contenu2.champ.nelem != NULL){
    free(contenu2.champ.nelem);
    contenu2.champ.nelem = NULL;
  }
  if(contenu2.champ.vec != NULL){
    free(contenu2.champ.vec);
    contenu2.champ.vec = NULL;
  }

  return (OK);
}


/****************************************************************
 * Programme  main(argc, argv)                                   *
 *****************************************************************
 *                                                               *
 * Programme principal du module Seq_Par                         *
 *                                                               *
 *---------------------------------------------------------------*
 * Sorties :                                                     *
 * int          argc   - Nombre d'arguments retournÅs.           *
 * char**       argv   - Liste des arguments retournÅs.          *
 *---------------------------------------------------------------*
 * Auteur :                                                      *
 *           G.DEGENEVE                                          *
 * Creation :  1/08/02                                           *
 ****************************************************************/

int main(int argc,char  **argv)
{
  char nom_seq[256], nom_par[256];
  int gnerror;
  if((argc != 3)&&(argc!=4)){
    printf("Nombre d'arguments erronne:\nLa commande est de la forme:\nseq_par fic_seq fic_pari [--dernier ]\n");
    exit(1);
  }
	
  strcpy(nom_seq, argv[1]);
  strcpy(nom_par, argv[2]);
  int statio=0;
  if (argc==4)
    {
      if (strcmp(argv[3],"--dernier")==0)
	{
	  cerr<<"statio"<<endl;
	  statio=1;
	}
      else 
	{
	  printf("Nombre d'arguments erronne:\nLa commande est de la forme:\nseq_par fic_seq fic_pari [--dernier ]\n");
	  exit(1);
	}
    }
  if(mode_debug == TRUE) printf("Arguments: %s %s %s\n", argv[0], argv[1],argv[2]);
  if(mode_debug == TRUE) printf("fichiers : %s  %s\n",nom_seq, nom_par);
  /* Initialisation des mots cles */
  init_tab_motc(g_liste_motcle);
  strncpy(contenu_res.file_name,nom_seq,79);
  contenu_res.file_name[80]='\0';
  contenu_res.nb_champ = 0;
  contenu_par.nb_champ = 0;
  contenu_res.nb_grille = 0;
  /* Appel du module de lecture */
  if(Lecture_Geom(nom_seq, &contenu_res) == ERROR){
    printf("File : %s\n",nom_seq);
    printf("Error when reading the results inside\n");
    exit(1);
  }
  contenu_par.nb_grille = 0;
  if(Lecture_Geom(nom_par, &contenu_par) == ERROR){
    printf("File : %s\n", nom_par);
    printf("Error when reading the results inside\n");
    exit(1);
  }
  /* Verification du maillage et coherence des informations */
  if(Verif_Geometrie(contenu_res, contenu_par) == ERROR){
    printf("Error: the meshes are different into files %s and %s\n",
	   nom_seq, nom_par);
    exit(1);
  }
  /* Verification des resultats */
  if (statio)
    gnerror=Verif_Result_statio(nom_seq, nom_par, &contenu_res, &contenu_par);
  else
    gnerror=Verif_Result(nom_seq, nom_par, &contenu_res, &contenu_par);

  if(gnerror == ERROR){
    printf("Erreur dans la verification des differences dues aux arrondis numeriques\n");
    exit(1);
  }
  /* Appel du module de liberation des structures  */
  if(libere_trio(contenu_res, contenu_par, equiv) == ERROR){
    printf("Error wheen freeing memory\n");
    exit(1);
  }
  free(gvalm);
  exit(gnerror);
}
