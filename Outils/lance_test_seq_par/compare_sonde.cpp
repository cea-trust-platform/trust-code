#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <fstream>
using namespace std;

istream& get_c(istream& entree, char& c)
{
  // on verifie que l'on n'est pas au bout
  if (entree.eof())
    {
      cerr<<" fin de fichier!!!"<<endl;
      throw;
    }
 
  entree.get(c);
  // on ignore le br de dos
  if (c=='\r') return entree.get(c);
  return entree;
}
static int exit_=0;
static double seuil_erreur=1.e-5;
class nom
{
public:
  nom() { str=new char[1];str[0]='\0';}
  ~nom() { delete [] str;}
  void affecte(const char* chaine) { delete [] str; str=new char[strlen(chaine)+1];
    strcpy(str, chaine);}
  char* str;
};

void error(const nom& s, const char* s2 )
{
  cerr << s.str << " " << s2 << endl;
  exit(-1);
}
int strcmp(const nom& n,const char*n2) { return strcmp(n.str,n2);}
//istream& get_mot(istream& entree, char** c1)
istream& get_mot(istream& entree, nom& nom)
{
  //    char*c =*c1;
  char* chaine=new char[1000];
  chaine[0]='\0';
  entree >> chaine;

  //cerr<<"ici "<< chaine <<endl<<flush;
  /*
    char* p=*c1;
    cerr<<"p "<<p;
    cerr<<" "<<strlen(p)<<endl<<flush;
    // if (p)
    //  delete  [] p;
    (*c1)=new char[strlen(chaine)+1];
    strcpy(*c1, chaine);
  */
  nom.affecte(chaine);
  delete[] chaine;

  return entree;
}
void passe_entete(ifstream& entree)
{
  char c; 
  get_c(entree,c);
  // si premiere ligne blanche on passe
  if (c!='\n') entree.putback(c);
  while ((c=='#') || (c=='@'))
    {
      
      while(c!='\n') {
	get_c(entree,c);
	//cerr<<"ici "<<c<<endl;
      }
      get_c(entree,c);
      if ((c!='#') && (c!='@')) entree.putback(c);
    }
  
}
int trouve_nb_champ(const nom& nom_f,int& nl)
{
  ifstream entree(nom_f.str);
  if (!entree) error(nom_f," non ouvrable");
  passe_entete(entree);
  nom titi;
  int i=0;
  char c='a';
  while (c!='\n' && !entree.eof())
    {
      get_mot(entree,titi);
      //cerr<<"|"<<titi.str<<"|" << i << " " << strlen(titi.str) << endl;
      if (strlen(titi.str)) i++;
      get_c(entree,c);
      while ((c==' ')||(c=='\t')) get_c(entree,c);
      //      cerr<<"c"<<c<<":"<<(int)c<<" "<<(int)' '<<endl;
      if (c!='\n') entree.putback(c);
    }
  
  get_mot(entree,titi);
  nl=1;
  //cerr<<"ci "<<entree.eof()<<" "<<titi.str<<" "<<c<<"l"<<endl;
  c='\n';
  while(!entree.eof())
    {
      //nl++;
      //cerr<<nl<<endl;
      get_c(entree,c);
      //if (nl > 100) exit(-1);
      int m=0;
      while (c!='\n')  { m=1;get_c(entree,c);}
      if (m) 
	nl++;
      else 
	cerr<<"ligne vide ?"<<endl;
      //c=0;
      //cerr<<"c"<<c<<":"<<(int)c<<endl;
    }
  
  
  entree.close();
  return i;
}

//if absolute_gap = true that means that the function called "ecart" will return the absolute gap and not the relative gap
bool absolute_gap = false;

double valmin=1e-15;
double ecart(const double& x,const double& y, const double& ymax1,const double& ymax2)
{
  double gerr = 0.0;
  double ymax=0.5*(ymax1+ymax2); // pour retrouver l'ancien compare_sonde faire ymax=ymax1
  if( absolute_gap )
    {
      gerr = fabs((x-y));
    }
  else
    {
      if(ymax != 0.0) gerr = fabs((x-y)/ymax);
    }
  if (ymax<=valmin)
    {
      if( (fabs(x)>valmin)||(fabs(y)>valmin))
  	{
  	  gerr=1;
  	}
      else 
  	gerr=0;
    }
  return gerr;
}

void traite_erreur(ifstream& entree,ifstream& entree2,double* val_max,double* val_max2,int nbc,int nl1,int nl2,const nom& option)
{
  int mode;
  if (!strcmp(option,"evol"))
    mode=1;
  else if (!strcmp(option,"statio"))
    mode=2;
  else if (!strcmp(option,"evol_inter"))
    mode=3;
  
  else
    {
      cerr<<option.str<<" pas compris"<<endl;
      exit(-1);
    }
  cerr<<" mode de comparaison "<<mode<<endl;
  double* err=new double[nbc];
  
  int el;
  for (el=0;el<nbc;el++)
    err[el]=0;
  if (mode==1)
    {
      if (nl1!=nl2)
	{
	  cerr<<"pas le meme nombre de lignes..."<<endl;
	  if (nl2<nl1) nl1=nl2;
	  cerr<<"on ne compare que les "<<nl1<<" premieres lignes"<<endl;
	  err[0]=1;
	}
      for (int l=0;l<nl1;l++)
	{
	  for (el=0;el<nbc;el++)
	    {
	      double f,f2;
	      entree >>f;
	      entree2>>f2;
	      {
		double err1=ecart(f,f2,val_max[el],val_max2[el]);
		if (err1>err[el]) err[el]=err1;
	      }
	    }
	}
    }
  if (mode==2)
    {
      int l;
      // on passe les premieres lignes de entree
      for ( l=0;l<nl1-1;l++)
	{
	  for (el=0;el<nbc;el++)
	    {
	      double f;
	      entree>>f;
	    }
	}
      // on passe les premieres lignes de entree2
      for ( l=0;l<nl2-1;l++)
	{
	  for (el=0;el<nbc;el++)
	    {
	      double f;
	      entree2>>f;
	    }
	}
      // on est sur la derniere ligne
      for (el=0;el<nbc;el++)
	{
	  double f,f2;
	  entree >>f;
	  entree2>>f2;
	  if (el>0)
	    {
	      double err1=ecart(f,f2,val_max[el],val_max2[el]);
	      if (err1>err[el]) err[el]=err1;
	    }
	}
    }
  if (mode==3)
    {
      // on va interpoler les valeurs du fichier1 en fonction de la colonne 0
      double* valmoins1=new double[nbc];
      double* valplus1=new double[nbc];
      double* val=new double[nbc];
      {
	for (el=0;el<nbc;el++)
	  valplus1[el]=-1e10;
      }
      int l=0;
      for (int l2=0;l2<nl2;l2++)
	{
	  // lecture de la l2 ligne du fichier2
	  for (el=0;el<nbc;el++)
	    entree2>>val[el];
	  while ((val[0]>valplus1[0])&&(l<nl1))
	    {
	      // on lit la ligne suivante
	      for (el=0;el<nbc;el++)
		{
		  valmoins1[el]=valplus1[el];
		  entree>>valplus1[el];
		}
	      // on initialisz valmoins1 avec valplus1
	      if (l==0)
		for (el=1;el<nbc;el++)
		  valmoins1[el]=valplus1[el];
	      l++;
	    }
	  // pour l'instant on ne traite pas le cas l=0 ou l=nl1
	  
	  //if ((l!=0)&&(l!=nl1))
	  {
	    if (val[0]>valplus1[0])
	      {
		cerr<<"interp vers le futur? "<<endl;
		assert(l==nl1);
		valplus1[0]=1e10;
	      }
	    assert((val[0]>valmoins1[0])&&(val[0]<=valplus1[0]));
	    //
	    cerr<< " on s'occupe de la ligne "<<l2<<endl;
	    for (el=0;el<nbc;el++)
	      {
		double f;
		// f interpolation lineaire
		f=valmoins1[el]+(valplus1[el]-valmoins1[el])*(val[0]-valmoins1[0])/(valplus1[0]-valmoins1[0]);
		double err1=ecart(f,val[el],val_max[el],val_max2[el]);
		if (err1>err[el]) err[el]=err1;
	      }
	  }
	}
      delete[] val;
      delete[] valplus1;  
      delete[] valmoins1;	
    }
  double max_err=0;
  for (el=0;el<nbc;el++)
    {
      if (err[el]>max_err)
	max_err=err[el];
      if (err[el]>seuil_erreur)
      {
	
	exit_=1;
	cout<<"Erreur max colonne "<<el<<" : "<<err[el]*100<<" % (>"<<seuil_erreur*100<<"%), val_max "<<val_max[el]<<" "<<val_max2[el]<<endl;
      }
    }
  cout<<" Erreur max sur toutes les colonnes "<< max_err<<endl;
  delete[] err;
}
void recupere_max(ifstream& entree,double* val_max,int nbc,int nl1, int max_val_all_comp,int max_delta)
{
  double* val_min=new double[nbc];
  
  for (int el=0;el<nbc;el++)
    {
      val_max[el]=-1e38;
      val_min[el]=1e38;
    }
  for (int l=0;l<nl1;l++)
    {
      for (int el=0;el<nbc;el++)
	{
	  double f;
	  entree >>f;
	  //f=fabs(f);
	  if (f>val_max[el]) val_max[el]=f;
	  if (f<val_min[el]) val_min[el]=f;
	}
    }
  if (max_val_all_comp)
    {
      double max_val_max=-1e37;
      double min_val_min=1e37;
      // Calcul de la valeur max du fichier
      for (int el=1;el<nbc;el++)
	{
	  //cout<<"val max colonne "<<el<<" : "<<val_max[el]<<endl;
	  // PL pour GF: je prefere supposer que le fichier est rempli
	  // de valeurs du meme champ et je prefere prendre le max
	  // de toutes les colonnes que le max d'une colonne
	  // En effet, par exemple pour les forces de pression
	  // j'ai des ecarts sur Fz et pas sur Fx et Fy alors que Fx~0
	  if (val_max[el]>max_val_max) 
	    max_val_max=val_max[el];
	  if (val_min[el]<min_val_min) 
	    min_val_min=val_min[el];
	} 
      // On ne touche pas a la premiere colonne qui contient le temps
      for (int el=1;el<nbc;el++)
	{
	  val_max[el]=max_val_max; 
	  val_min[el]=min_val_min; 
	}
    }
   for (int el=0;el<nbc;el++)
     if (max_delta) {
       val_max[el]+=(-val_min[el]+1e-2);
     }
     else
       {
	 if (fabs(val_max[el])>fabs(val_min[el]))
	   val_max[el]=fabs(val_max[el]);
	 else
	   val_max[el]=fabs(val_min[el]);
	     
       }
  
}
void usage()
{
  cerr<<"mauvais params: file1 file2 [-type evol|statio|evol_inter] [-seuil_erreur val] [-valmin val] [-max_par_compo] [ -max_delta ] [-absolute_gap] "<<endl;
  exit(-1);	
}
int main(int argc, char* argv[])
{
  int max_val_all_comp=1;
  int max_delta=0;
  //char c,c2;
  nom file1,file2,option;
  if (argc<3) usage();
  file1.affecte(argv[1]);
  file2.affecte(argv[2]);
  //cout<<"comparaison "<<argv[1] << " "<<argv[2]<<endl;
  for (int  arg=0;arg<argc;arg++)
    cout<<argv[arg]<<" ";
  cout << endl;
  // evol par defaut 
  option.affecte("evol");
  for (int arg=3;arg<argc;arg++)
    {
      if (!strcmp(argv[arg],"-type")) 
	option.affecte(argv[++arg]);
      else 
	if (!strcmp(argv[arg],"-seuil_erreur"))
	  seuil_erreur=atof(argv[++arg]);
	else 
	  if (!strcmp(argv[arg],"-max_par_compo"))
	    max_val_all_comp=0;
	  else
	    if (!strcmp(argv[arg],"-max_delta"))
	      max_delta=1;
	    else
	      if (!strcmp(argv[arg],"-valmin"))
		valmin=atof(argv[++arg]);
	      else
		if (!strcmp(argv[arg],"-absolute_gap"))
		  absolute_gap = true;
		else usage();
    }

  ifstream entree(file1.str);
  if (!entree) error(file1," non ouvrable");

  ifstream entree2(file2.str);
  if (!entree2) error(file2," non ouvrable");
  int nl1;
  int nb_champ=trouve_nb_champ(file1,nl1);
  cerr<<"nb_champ "<<nb_champ<<" nb_lignes "<<nl1<<endl;
  int nl2;
  int nb_champ2=trouve_nb_champ(file2,nl2);
  if (nb_champ2!=nb_champ)
    {
      cerr<<"les deux fichiers non pas le meme nombre de colonnes"<<endl;
      exit(-1);
    }
  double* val_max=new double[nb_champ];
  double* val_max2=new double[nb_champ2];
  //double max_val_max=0;
  
  passe_entete(entree);
  recupere_max(entree,val_max,nb_champ,nl1,max_val_all_comp,max_delta);
  entree.close();

  
  passe_entete(entree2);
  recupere_max(entree2,val_max2,nb_champ,nl2,max_val_all_comp,max_delta);
  entree2.close();
    
  entree.open(file1.str);
  entree2.open(file2.str);
 
  passe_entete(entree);
  passe_entete(entree2);

  traite_erreur(entree,entree2,val_max,val_max2,nb_champ,nl1,nl2,option);

  //nom mot;
  //get_mot(entree,mot);
  //cerr<<mot.str<<endl;
  // ofstream sortie("es.py");
  // sortie<<"escape escape escape escape files new n escape escape escape escape"<<endl;
  // sortie<<"escape escape escape escape utilities variables CreateAlwaysNewPoint -1 escape escape"<<endl;
 

  delete[] val_max;
  delete[] val_max2;
  return exit_;
} 

