#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


using namespace std;
int nbsurf=0;
int nbvol=0;
int nbligne=0;
int nbvolmax=10000;
int* indice_vol;
int* marq;
int nbmarq=0;
char* buf;

class macro
{
public:
  ~macro() { delete [] name;};
  
  int n;
  char* name;
  double pos[3];
  inline macro() {n=-1;name=new char[100];}; 
}; 
int nmacmax=100;
macro* vectmacro;
void error(const char* s, const char* s2 )
{
  cerr << s << " " << s2 << endl;
  exit(-1);
}

istream& getc(istream& entree, char& c)
{
  entree.get(c);
  // cerr <<c;
  return entree;
}

class nom
{
public:
  nom() { str=new char[1];str[0]='\0';}
  ~nom() { delete [] str;}
  void affecte(const char* chaine) { delete [] str; str=new char[strlen(chaine)+1];
    strcpy(str, chaine);}
  char* str;
};
int strcmp(const nom& n,const char*n2) { return strcmp(n.str,n2);}
//istream& get_mot(istream& entree, char** c1)
istream& get_mot(istream& entree, nom& nom, int sans_test=0)
{
  //    char*c =*c1;
  char* chaine=new char[1000];
  chaine[0]='\0';
  entree >> chaine;
  if ((sans_test==0)&&(entree.eof()))
    {
      cerr<<"plus rien a lire"<<endl;
      exit(-1);
    }
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
void addstudy(ostream& sortie,const char* nom,int volc,int mode=0)
{
  sortie<<"addstd("<<nom<<volc<<",\""<<nom<<volc<<"\","<<mode<<")"<<endl;
  /*
    sortie<<"id_"<<nom<<volc<<"=geom.addToStudy("<<nom<<volc<<",\""<<nom<<volc<<"\")"<<endl;
    sortie<<"gg.createAndDisplayGO(id_"<<nom<<volc<<")"<<endl;
    sortie<<"gg.setDisplayMode(id_"<<nom<<volc<<",1)"<<endl;

    sortie<<"print \""<<nom<<volc<<" cree\""<<endl;
    sortie<<"sgPyQt.putInfo( \""<<nom<<volc<<" cree\")"<<endl;
  */
}

void union_2(ostream& sortie,const char * nom,int n1,int deb ,int fin)
{
  // sortie<<nom<<n1<<"=union2(geom,"<<deb<<","<<fin<<")"<<endl;

  int largeur=fin-deb+1;
  // # from math import log
  int  niveau=int(log(largeur)/log(2));
  int ok=int(pow(2,(int(niveau))));
  int m=0;
  int i;
  for (i=0;i<(largeur-ok);i++)
    {
      sortie<<"A0_"<<i<<"=geom.MakeFuse(vol"<<deb+2*i<<",vol"<<deb+2*i+1<<")"<<endl;
      addstudy(sortie,"A0_",i,2);
      sortie<<"del vol"<<deb+2*i<<" , vol"<<deb+2*i+1<<endl;
      //print cmd
      //#exec (cmd)
      m+=1;
    }
  for ( i=largeur-ok;i<ok;i++)
    {
      sortie<<"A0_"<<i<<"=vol"<<deb+i+m<<endl;
      
      sortie<<"del vol"<<deb+i+m<<endl;
    }
  // le niveau 0 est fait
  // ok on a un nombre paire de A
  for (int niv =1;niv<int(niveau)+1;niv++)
    {
      sortie<<"print \"niv "<< niv<<"/"<<niveau+1<<"\""<<endl;
      ok=ok/2;
      for (i=0;i<ok;i++)
	{
	  //Anm1="A"<<niv-1<<"_"
	  sortie<<"dernier=A"<<niv<<"_"<<i<<"=geom.MakeFuse(A"<<niv-1<<"_"<<deb+2*i<<",A"<<niv-1<<"_"<<deb+2*i+1<<")"<<endl;
	  //	  addstudy(sortie,"Aniv_",i,2);
	  sortie<<"addstd(A"<<niv<<"_"<<i<<",\"A"<<niv<<"_"<<i<<"\",2)"<<endl;
	  sortie<<"del A"<<niv-1<<"_"<<deb+2*i<<" , A"<<niv-1<<"_"<<deb+2*i+1<<endl;
	  //print cmd
	  m+=1;
	
	}
    }
      //print "!!!!!!!!!",m,largeur-1
      //return dernier
  
  sortie<<"A"<<n1<<"=dernier"<<endl;
  addstudy(sortie,"A",n1,2);
}

void operation_volume(ostream& sortie,int argc2, char* argv[])
{
  ofstream testc("testc2");
#ifdef STD
  sortie<<"from salome import sg\nsg.EraseAll()"<<endl;
#endif
  int argc=argc2;
  // on bloque l'ancienne option de argc
  argc=2;
  cout<<"argc "<<argc<<endl;
  if (nbmarq==0)
    {
      //int marq[argc];
      for (int kk=1;kk<argc-1;kk++)
	{
	  cout << kk<< " "<<argv[kk+1]<<endl;
	  marq[kk]=atoi(argv[kk+1]);
	  cout << kk<< " "<<marq[kk]<<" "<<argv[kk+1]<<endl; 
	}
      marq[0]=0;
      marq[argc-1]=nbvol;
      nbmarq=argc-1;
    }

  for (int vc=0;vc<nbmarq;vc++)
    {
      int volc=marq[vc];
      int ind1=indice_vol[volc];
      cout <<"on compare les volumes a "<<ind1<<endl;
      //int volc=0;
      int volc2=marq[vc+1];
      testc<<"axo"<<endl; 
      testc<<"restore vol"<<volc<<".brep A0"<<endl;
      int i;
      int tout_pareil=1;
      if (volc2<4) tout_pareil=0;
      for ( i=volc+1;i<volc2;i++) 
	{  
	  if (indice_vol[i]!=ind1) tout_pareil=0;
	}
      if (tout_pareil==0)
      {
      sortie<<"A"<<volc<<"=vol"<<volc<<endl;
      //	sortie<<"vol"<<volc<<"=0"<<endl;
      addstudy(sortie,"A",volc,1);
      //      sortie<<"geom.addToStudy(A"<<volc<<",\"A"<<volc<<"\")"<<endl;
      // on fait d'abord les unions NON
       }
      if (tout_pareil==1)
      {
	   
	union_2(sortie,"A",volc2-1,volc,volc2-1);
	    
      }
      else
      for ( i=volc+1;i<volc2;i++) 
	{  
	  testc<<"restore vol"<<i<<".brep vol"<<i<<endl;
	  testc<<"checkshape  vol"<<i<<endl;   
  
	  if (indice_vol[i]==ind1)
	    {
      
	      sortie<<"A"<<i<<"=geom.MakeFuse(A"<<i-1<<",vol"<<i<<")"<<endl;
	      testc<<"bfuse A"<<i<<" A"<<i-1<<" vol"<<i<<endl;
	      //sortie<<"A"<<i<<"=geom.MakeBoolean(A"<<i-1<<",vol"<<i<<",0)"<<endl;
	    }
	  //  // on fait les intersections	 
	  else
	    {
	      sortie<<"A"<<i<<"=geom.MakeBoolean(A"<<i-1<<",vol"<<i<<",2)"<<endl;
	      testc<<"bcut A"<<i<<" A"<<i-1<<" vol"<<i<<endl;
	    }
	  testc<<"donly A"<<i<<endl;
	  testc<<"checkshape  A"<<i<<endl;   
	  testc<<"fit"<<endl; 
	  if (i!=volc2-1)
	    addstudy(sortie,"A",i,1);
	  else
	    addstudy(sortie,"A",i,2);
	  sortie<<"del A"<<i-1<<endl;
	  sortie<<"del vol"<<i<<endl;
	  testc<<"save  A"<<i<<" A"<<i<<".brep"<<endl;
	  //sortie<<"geom.ExportBREP(A"<<i<<",DIR1+\"A"<<i<<".brep\")"<<endl;

	  //sortie<<"geom.addToStudy(A"<<i<<",\"A"<<i<<"\")"<<endl;
	}
      // sortie<<"id_gg=geom.addToStudy(A"<<volc2-1<<",\"A"<<volc2-1<<"\")"<<endl;
      //sortie<<"gg.createAndDisplayGO(id_gg)"<<endl;
      //sortie<<"geom.ExportBREP(DIR1+\"A"<<volc2-1<<".brep\",A"<<volc2-1<<")"<<endl;
      sortie<<"# marqueur "<<endl;
    }
  // for (int vc=0;vc<argc-1;vc++)
  {
    int vc=0;
    int volc=marq[vc];
    int ind1=indice_vol[volc];
    cout <<"on compare les volumes a "<<ind1<<endl;
    //int volc=0;
    int volc2=marq[vc+1];
    sortie<<"B"<<volc<<"=A"<<volc2-1<<endl;
#ifdef STD
    sortie<<"from salome import sg\nsg.EraseAll()"<<endl;
#endif
    //sortie<<"geom.addToStudy(B"<<volc<<",\"B"<<volc<<"\")"<<endl;
    addstudy(sortie,"B",volc,3);
    sortie<<"geom.ExportBREP(B"<<volc<<",DIR1+\"B"<<volc<<".brep\")"<<endl;
    //sortie<<"# marqueur "<<endl;
    // on fait d'abord les unions NON
    for ( int i1=1;i1<nbmarq;i1++) 
      {  
	sortie<<"# marqueur "<<endl;
	int i2=marq[i1];
	int i=marq[i1+1]-1;
	if (indice_vol[i2]==ind1)
	  {
      
	    sortie<<"B"<<i1<<"=geom.MakeFuse(B"<<i1-1<<",A"<<i<<")"<<endl;
      
	    //sortie<<"B"<<i<<"=geom.MakeBoolean(B"<<i-1<<",vol"<<i<<",0)"<<endl;
	  }
	//  // on fait les intersections	 
	else
	  {
	    sortie<<"B"<<i1<<"=geom.MakeBoolean(B"<<i1-1<<",A"<<i<<",2)"<<endl;
	  }
	addstudy(sortie,"B",i1,3);
	// sortie<<"geom.addToStudy(B"<<i1<<",\"B"<<i1<<"\")"<<endl;
	sortie<<"del A"<<i<<endl;
	sortie<<"geom.ExportBREP(B"<<i1<<",DIR1+\"B"<<i1<<".brep\")"<<endl;

	//sortie<<"# marqueur"<<endl;
      }
  }
  //if (i1==nbmarq)
  {
    int i1=nbmarq-1;if (nbmarq==0) i1=0;
    sortie<<"try : # on essaye d'exporter en SAT"<<endl;
    sortie<<"  geom.Export(B"<<i1<<",DIR1+\"B"<<i1<<".sat\",\"ACIS\")"<<endl;

    sortie<<"except:"<<endl;
    sortie<<"  geom.ExportIGES(B"<<i1<<",DIR1+\"B"<<i1<<".igs\")"<<endl;
    sortie<<"  dil=geom.MakeScaleTransform(B"<<i1<<",geom.MakePointStruct(0,0,0) ,1000)"<<endl;
    sortie<<"  geom.ExportIGES(dil,DIR1+\"dilB"<<i1<<".igs\")"<<endl;
    sortie<<"  geom.ExportBREP(dil,DIR1+\"dilB"<<i1<<".brep\")"<<endl;
    sortie<<"  pass"<<endl;
  }
  sortie<<"# marqueur "<<endl;
}
void cree_cylindre(istream& entree,ostream& sortie)
{
  double xo,yo,r,zo,z1;
  int idir,indice;
  entree>>xo>>yo>>r>>zo>>z1>>idir>>indice;
  sortie<<"# cylindre "<<indice<<endl;
  // pave du bas
  if (idir==3) {
    sortie<<"vol"<<nbvol<<"=geom.MakeCylinder(\n   geom.MakePointStruct("<<xo<<","<<yo<<","<<zo<<"),\n   geom.MakeDirection(geom.MakePointStruct(0,0,1)),\n   "<<r<<","<<z1-zo<<")"<<endl;
  }
  else 
    if (idir==1) {
      //exit(-1);
      sortie<<"vol"<<nbvol<<"=geom.MakeCylinder(\n   geom.MakePointStruct("<<zo<<","<<xo<<","<<yo<<"),\n   geom.MakeDirection(geom.MakePointStruct(1,0,0)),\n "<<r<<","<<z1-zo<<")"<<endl;
    }
    else
      if (idir==-2)
	{
		// GF on essaye de tourner par rotation
	  double Zomin=-z1,Zomax=-zo,rc=r,ri=0,phi=-1.5707964,theta=-1.5707964;
	  int indmat=indice;
	  sortie<<"# tube "<<indmat<<endl;
  //cout<<"cree_tube ne fait rien on attend tubem pour faire"<<endl;
	  sortie<<"volp=geom.MakeCylinder(\n   geom.MakePointStruct(0,0,"<<Zomin<<"),\n   geom.MakeDirection(geom.MakePointStruct(0,0,1)),\n   "<<rc<<","<<Zomax-Zomin<<")"<<endl;
	
	  // on tourne une premiere fois la boite suivant oy angle -phi
	  sortie<<"volp2=geom.MakeRotation(volp,geom.MakeAxisStruct(0,0,0,0,1,0),"<<-phi <<")"<<endl;
	  // on tourne une deuxieme fois la boite suivant oz angle theta
	  sortie<<"volp3=geom.MakeRotation(volp2,geom.MakeAxisStruct(0,0,0,0,0,1),"<<theta  <<")"<<endl;
	  // on translate
	  sortie<<"vol"<<nbvol<<"=geom.MakeTranslation(volp3,0,0,0)"<<endl;
	}
  else
      if (idir==2) {
	sortie<<"vol"<<nbvol<<"=geom.MakeCylinder(\n   geom.MakePointStruct("<<yo<<","<<zo<<","<<xo<<"),\n   geom.MakeDirection(geom.MakePointStruct(0,1,0)),\n   "<<r<<","<<z1-zo<<")"<<endl;
	//	exit(-1);
      }
      else
	{
	  cerr<<"gros pb dans cree_cylindre pour l'instant"<<endl;
	  exit(-1);
	}
   
  nbligne+=6;
  nbsurf+=4;
  //sortie<<"geom.addToStudy(vol"<<nbvol<<",\"vol"<<nbvol<<"\")"<<endl;
  addstudy(sortie,"vol",nbvol);
  if (nbvol>=nbvolmax)
    {
      cerr << "Erreur, nbvol>=nbvolmax, contacter le support TRUST." << endl;
      exit(-1);
    }
  indice_vol[nbvol]=indice;
  nbvol+=1;
 
}
void calcul_p(double* A,double teta,double* org,int IDIR,double r,double z0)
{
  
  A[0]=org[0]+r*cos(teta);
  A[1]=org[1]+r*sin(teta);
  A[2]=org[2]+z0;
  double x,y,z;
  x=A[0];
  y=A[1];
  z=A[2];
  if (IDIR==1)
    {
      // il faut inverser z->x y->z  x->y
      A[0]=z;
      A[2]=y;
      A[1]=x;
    }
  else
    if (IDIR==2)
      {
	// il faut inverser z->y y->x x->z
	A[1]=z;
	A[0]=y;
	A[2]=x;

      }
    else
      if (IDIR!=3) { cerr<<"erreur "<<IDIR <<endl;exit(-1);//il faut inverser xyz
      }
  /*
    cout<<"A[0] "<<A[0]<<endl;
    cout<<"A[1] "<<A[1]<<endl;
    cout<<"A[2] "<<A[2]<<endl;
    cout <<r <<" r zo "<<z0<<endl;
  */
}
void create_arc(double* a0,double* a1, double* a2,ostream& sortie)
{
  return;
  sortie<<"c"<<nbligne<<"=geom.MakeArc( geom.MakePointStruct("<<a0[0]<<","<<a0[1]<<","<<a0[2]<<"),\n";
  sortie<<" geom.MakePointStruct("<<a1[0]<<","<<a1[1]<<","<<a1[2]<<"),\n";
  sortie<<" geom.MakePointStruct("<<a2[0]<<","<<a2[1]<<","<<a2[2]<<"))"<<endl;
    
  sortie<<"geom.addToStudy(c"<<nbligne<<",\"c"<<nbligne<<"\")"<<endl;
  nbligne+=1;
}
void calcul_thore(ostream& sortie,  double AC, double BC, double RMED, double RTORMI, double RTORMA, double RTORII, double RTORIA, double  TETMIN, double TETMAX, double HMED, double HMIN, double HMAX,int IDIR,int INDMAT);

void corps_cree_thore(ostream& sortie,  double AC, double BC, double RMED, double RTORMI, double RTORMA, double RTORII, double RTORIA, double  TETMIN, double TETMAX, double HMED, double HMIN, double HMAX,int IDIR,int INDMAT);
void cree_thore(istream& entree,ostream& sortie)
{
  cout << "cree_thore "<<endl;
  
  double AC,BC,RMED,RTORMI,RTORMA,RTORII,RTORIA, TETMIN,TETMAX,HMED,HMIN,HMAX;
  int IDIR,INDMAT;
  entree>>AC>>BC>>RMED>>RTORMI>>RTORMA>>    
    RTORII>>RTORIA>> 
    TETMIN>>TETMAX>>HMED>>HMIN>>HMAX>>IDIR>>INDMAT;
  calcul_thore(sortie,AC,BC,RMED,RTORMI,RTORMA,RTORII,RTORIA, TETMIN,TETMAX,HMED,HMIN,HMAX,IDIR,INDMAT); 
  addstudy(sortie,"vol",nbvol);
  // sortie<<"geom.addToStudy(vol"<<nbvol<<",\"vol"<<nbvol<<"\")"<<endl;
  indice_vol[nbvol]=INDMAT;
  nbvol+=1;
}
void cree_thore_gen(istream& entree,ostream& sortie)
{
  cout << "cree_thore_gen "<<endl;
  
  double AC,BC,RMED,RTORMI,RTORMA,RTORII,RTORIA, TETMIN,TETMAX,HMED,HMIN,HMAX;
  int IDIR,INDMAT;
  double  phi,teta,xo,yo,zo;
  entree>>AC>>BC>>RMED>>RTORMI>>RTORMA>>    
    RTORII>>RTORIA>> 
    TETMIN>>TETMAX>>HMED>>HMIN>>HMAX>>phi>>teta>>xo>>yo>>zo>>INDMAT;
  // on cree un thore selon x
  IDIR=1;
  calcul_thore(sortie,AC,BC,RMED,RTORMI,RTORMA,RTORII,RTORIA, TETMIN,TETMAX,HMED,HMIN,HMAX,IDIR,INDMAT);
  // puis on le tourne et deplae
  sortie<<"volp=vol"<<nbvol<<endl;
  sortie<<"volp2=geom.MakeRotation(volp,geom.MakeAxisStruct(0,0,0,0,1,0),"<<-phi <<")"<<endl;
    // on tourne une deuxieme fois le cone suivant oz angle teta
  sortie<<"volp3=geom.MakeRotation(volp2,geom.MakeAxisStruct(0,0,0,0,0,1),"<<teta  <<")"<<endl;
  // on translate
  sortie<<"vol"<<nbvol<<"=geom.MakeTranslation(volp3,"<<xo<<","<<yo<<","<<zo<<")"<<endl;
  addstudy(sortie,"vol",nbvol);
  // sortie<<"geom.addToStudy(vol"<<nbvol<<",\"vol"<<nbvol<<"\")"<<endl;
  indice_vol[nbvol]=INDMAT;
  nbvol+=1;
}

void calcul_thore(ostream& sortie,  double AC, double BC, double RMED, double RTORMI, double RTORMA, double RTORII, double RTORIA, double  TETMIN, double TETMAX, double HMED, double HMIN, double HMAX,int IDIR,int INDMAT)
{

  sortie<<"# thore "<<INDMAT<<endl;
  double pi=2.*asin(1);
  int marq;
  // on tolere que tetmax-temin=2Pi a 1e-5 pres
  if (((TETMAX-TETMIN)-2.*pi)>-1e-5)
    {
      // on veut faire un thore
      marq=0;
      if ((RTORMI!=RTORMA)||(RTORII!=RTORIA))
	{ cerr<<"pas code "<<endl;exit(-1);sortie<<"erreur"<<endl;}
      corps_cree_thore(sortie,AC,BC,RMED,RTORMI,RTORMA,RTORII,RTORIA, TETMIN,TETMAX,HMED,HMIN,HMAX,IDIR,marq); 
    }
  else
    {
      // on a une portion de thore est ce un cone ?
      if (RTORMI!=RTORMA)
	{
	  marq=1;
	}
      else marq=2;
      corps_cree_thore(sortie,AC,BC,RMED,RTORMI,RTORMA,RTORII,RTORIA, TETMIN,TETMAX,HMED,HMIN,HMAX,IDIR,marq);
      // a t on un sous thore
    }
  //cerr<<RTORII<<endl;exit(-1);
  if ((RTORII!=0)||(RTORIA!=0))
    {
      
      // on sauvegarde le premier thore
      sortie<<"volsa=vol"<<nbvol<<endl;
      //exit(-1);
      if (marq!=0)
	{
	  if (RTORMI!=RTORMA)
	    {
	      marq=1;
	    }
	  else marq=2;
	}
      corps_cree_thore(sortie,AC,BC,RMED,RTORII,RTORIA,RTORII,RTORIA, TETMIN-0.1,TETMAX+0.1,HMED,HMIN,HMAX,IDIR,marq);
      // on coupe le premier thore avec le nouveau
      sortie<<"vol"<<nbvol<<"=geom.MakeBoolean(volsa,vol"<<nbvol<<",2)"<<endl;
    }
  // le max et le min des quatres rayons
  double rmax=RTORMI;
  if (RTORMA>RTORMI) rmax=RTORMA;
  double rmin=RTORII;
  if (RTORIA>RTORII) rmin=RTORIA;
  if (((HMAX-HMED)<rmax)&&(fabs(fabs(HMAX-HMED)/rmax-1.)>1e-5))
    {
      //il faut couper le thore
      // on cree une boite de HMAX a HMAX+4*rmax
      //                      et selon le plan de -(RMED+4*rmax*cos(Pi/4)) a (RMED+2*rmax)
      cerr<<" ET VOILA"<<(HMAX-HMED)/rmax-1<< endl;
      sortie<<"volsa"<<nbvol<<"=vol"<<nbvol<<endl;
      double org[3],P1[3],P2[3];
      org[0]=AC;org[1]=BC;org[2]=0;
      calcul_p(P1,-3*pi/4.,org,IDIR,4*RMED+4*rmax,HMAX);
      calcul_p(P2,pi/4.,org,IDIR,4*RMED+4*rmax,HMAX+4*rmax);
      sortie <<"vol"<<nbvol<<"=geom.MakeBox("<<P1[0]<<","<<P1[1]<<","<<P1[2]<<","<<P2[0]<<","<<P2[1]<<","<<P2[2]<<")"<<endl;
      //addstudy(sortie,"vol",nbvol);
      //addstudy(sortie,"volsa",nbvol);
      sortie<<"vol"<<nbvol<<"=geom.MakeBoolean(volsa"<<nbvol<<",vol"<<nbvol<<",2)"<<endl;
      //cerr<<"a coder"<<endl;
    }
  if (((HMED-HMIN)<rmax)&&(fabs(fabs(HMIN-HMED)/rmax-1)>1e-5))
    {
      //il faut couper le thore
      // on cree une boite de HMIN-4*RMAX a HMIN
      //                      et selon le plan de -(RMED+4*rmax*cos(Pi/4)) a (RMED+2*rmax)
      sortie<<"volsa"<<nbvol<<"=vol"<<nbvol<<endl;
      double org[3],P1[3],P2[3];
      org[0]=AC;org[1]=BC;org[2]=0;
      calcul_p(P1,-3*pi/4.,org,IDIR,RMED+4*rmax,HMIN-4*rmax);
      calcul_p(P2,pi/4.,org,IDIR,RMED+4*rmax,HMIN);
      sortie <<"vol"<<nbvol<<"=geom.MakeBox("<<P1[0]<<","<<P1[1]<<","<<P1[2]<<","<<P2[0]<<","<<P2[1]<<","<<P2[2]<<")"<<endl;
      //addstudy(sortie,"vol",nbvol);
      //addstudy(sortie,"volsa",nbvol);
      sortie<<"vol"<<nbvol<<"=geom.MakeBoolean(volsa"<<nbvol<<",vol"<<nbvol<<",2)"<<endl;
      //cerr<<"a coder"<<endl;
    }
  // corps_cree_thore(sortie,AC,BC,RMED,RTORMI,RTORMA,RTORII,RTORIA, TETMIN,TETMAX,HMED,HMIN,HMAX,IDIR,marq); 
 
}
void corps_cree_thore(ostream& sortie, double AC, double BC, double RMED, double RTORMI, double RTORMA, double RTORII, double RTORIA, double  TETMIN, double TETMAX, double HMED, double HMIN, double HMAX,int IDIR,int marq)

{

  //   if (  (RTORII!=0.) || (RTORIA!=0.))
  //      {
  //        cout <<"RTORII ou RTORIA non nul : pas code correctement"<<endl;
  //      }
  double pi=2.*asin(1);
  //    if ((TETMAX-TETMIN)>=2.*pi)
  //      {
  //         cout <<"TETMAX-TETMIN > 2*Pi  : pas code correctement"<<endl;
  //         // on fait deux thores
  //         // un de tetmin a temin +pi 
  //  	TETMAX=TETMIN+pi;
  //        // et l'autre de tetmin+pi a tetmin+2*pi	
  //    corps_cree_thore(sortie,AC,BC,RMED,RTORMI,RTORMA,RTORII,RTORIA, TETMAX,TETMAX+pi,HMED,HMIN,HMAX,IDIR,INDMAT); 
  //      }
	
  double A0[3],A1[3],A2[3],A3[3],org[3];
  double B0[3],B1[3],B2[3],B3[3];
  double C0[3],C1[3];
  double A[3],B[3];
 
  org[0]=AC;org[1]=BC;org[2]=HMED;
  calcul_p(A0,TETMIN,org,IDIR,RMED-RTORMI,0.);
  calcul_p(A1,TETMIN,org,IDIR,RMED+RTORMI,0.);
  calcul_p(A2,TETMIN,org,IDIR,RMED,0.+RTORMI);
  calcul_p(A3,TETMIN,org,IDIR,RMED,0.-RTORMI);
  create_arc(A0,A2,A1,sortie);
  create_arc(A0,A3,A1,sortie);
  calcul_p(B0,TETMAX,org,IDIR,RMED-RTORMA,0.);
  calcul_p(B1,TETMAX,org,IDIR,RMED+RTORMA,0.);
  calcul_p(B2,TETMAX,org,IDIR,RMED,0.+RTORMA);
  calcul_p(B3,TETMAX,org,IDIR,RMED,0.-RTORMA);
  create_arc(B0,B2,B1,sortie);
  create_arc(B0,B3,B1,sortie);
  calcul_p(C0,(TETMAX+TETMIN)/2.,org,IDIR,RMED-(RTORMA+RTORMI)/2.,0.);
  calcul_p(C1,(TETMAX+TETMIN)/2.,org,IDIR,RMED+(RTORMA+RTORMI)/2.,0.);
  create_arc(A0,C0,B0,sortie);
  create_arc(A1,C1,B1,sortie);
  //     sortie<<"IorCoupList"<<nbvol<<"=[]"<<endl;
  //     for (int jj=nl;jj<nbligne;jj++)
  //      sortie<<"IorCoupList"<<nbvol<<".append(c"<<jj<<"._get_Name())"<<endl;
  //    sortie<<"Coupe"<<nbligne<<" = geom.MakeCompound(IorCoupList"<<nbvol<<")"<<endl;
  //    sortie<<"geom.addToStudy(Coupe"<<nbligne<<",\"Coupe"<<nbligne<<"\")"<<endl;
  double org2[3],dir[3],org3[3];
 
  org2[0]=0;org2[1]=0;org2[2]=0;
  calcul_p(org3,0,org,IDIR,0,0);
  calcul_p(dir,0,org2,IDIR,0,1);
  calcul_p(B,TETMIN+pi/2,org2,IDIR,1,0.);
  if (marq==0)
    {
      if (RMED==0)
	{
	  cerr<<"on a une sphere"<<endl;
	  sortie<<"vol"<<nbvol<<"=geom.MakeSphere("<<org3[0]<<","<<org3[1]<<","<<org3[2]<<","<<RTORMI<<")"<<endl;
	  return;
	}
      
      sortie<<"vol"<<nbvol<<"=geom.MakeTorus(geom.MakePointStruct("<<org3[0]<<","<<org3[1]<<","<<org3[2]<<"),geom.MakeDirection(geom.MakePointStruct("<<dir[0]<<","<<dir[1]<<","<<dir[2]<<")),"<<RMED<<","<<RTORMI<<")"<<endl;
      return;
      cerr<<"il manque l'interface avec MakeTorus"<<endl;exit(-1);
    }
  if (marq==2)
    {
      if (TETMAX-TETMIN<pi)
	{
	  calcul_p(A,TETMIN,org,IDIR,RMED,0.);
	  calcul_p(B,TETMIN-pi/2,org2,IDIR,1,0.);
 
	  // on cree tout le tor puis on le coupe
	  sortie<<"volp=geom.MakeTorus(geom.MakePointStruct("<<org3[0]<<","<<org3[1]<<","<<org3[2]<<"),geom.MakeDirection(geom.MakePointStruct("<<dir[0]<<","<<dir[1]<<","<<dir[2]<<")),"<<RMED<<","<<RTORMI<<")"<<endl;
	  //sortie<<"addstd(volp,\"volp\",5)"<<endl;
	  sortie<<"p2=geom.MakePointStruct("<<B[0]<<","<<B[1]<<","<<B[2]<<")"<<endl;
	  sortie<<"p3=geom.MakePointStruct("<<A[0]<<","<<A[1]<<","<<A[2]<<")"<<endl;
	  sortie<<"c1=geom.MakeCylinder(p3,geom.MakeDirection(p2),"<<2*(RTORMI+RMED)<<","<< 2*(RTORMI+RMED)<<")"<<endl;
	  calcul_p(A,TETMAX,org,IDIR,RMED,0.);
	  calcul_p(B,TETMAX+pi/2,org2,IDIR,1,0.);
	  sortie<<"p2=geom.MakePointStruct("<<B[0]<<","<<B[1]<<","<<B[2]<<")"<<endl;
	  sortie<<"p3=geom.MakePointStruct("<<A[0]<<","<<A[1]<<","<<A[2]<<")"<<endl;
	  sortie<<"c2=geom.MakeCylinder(p3,geom.MakeDirection(p2),"<<2*(RTORMI+RMED)<<","<< 2*(RTORMI+RMED)<<")"<<endl;
	  // sortie<<"addstd(c1,\"c1\",5)"<<endl;
	  //sortie<<"addstd(c2,\"c2\",5)"<<endl;
	  sortie<<"volp=geom.MakeBoolean(volp,c1,2)"<<endl;
	  sortie<<"vol"<<nbvol<<"=geom.MakeBoolean(volp,c2,2)"<<endl;
	  return;
	}
      else
	{
      //portion de thore
      sortie<<"p2=geom.MakePointStruct("<<B[0]<<","<<B[1]<<","<<B[2]<<")"<<endl;
      //sortie<<"p2b=geom.MakeVertex("<<B[0]<<","<<B[1]<<","<<B[2]<<")"<<endl;
  
      //sortie<<"geom.addToStudy(p2b,\"P"<<nbvol<<"p2\")"<<endl;
      calcul_p(A,TETMIN,org,IDIR,RMED,0.);
      sortie<<"p3=geom.MakePointStruct("<<A[0]<<","<<A[1]<<","<<A[2]<<")"<<endl;
      sortie<<"#geom.addToStudy(p3,\"PA"<<nbvol<<"p2\")"<<endl;
      sortie<<"C1=geom.MakeCircle(p3,geom.MakeDirection(p2),"<<RTORMI<<")"<<endl;
      //sortie<<"geom.addToStudy(C1,\"C1\")"<<endl;
      sortie<<"C12=geom.MakeWire([C1]);"<<endl;
      sortie<<"C13=geom.MakeFace(C12,1);"<<endl;
      //sortie<<"geom.addToStudy(C13,\"C13\")"<<endl;
   
      sortie<<"vol"<<nbvol<<"=geom.MakeRevolution(C13,geom.MakeAxisStruct("<<org3[0]<<","<<org3[1]<<","<<org3[2]<<","<<dir[0]<<","<<dir[1]<<","<<dir[2]<<"),"<<TETMAX-TETMIN<<")"<<endl;
      return;
	}
    }
  //return;
  nbsurf+=4;
  cout <<"cree thore piratee2"<<endl;
  double s=0;
  int kk;
  if (marq==1) 
    { //cone
      for ( kk=0;kk<3;kk++)
	{
	  A[kk]=(A0[kk]+A1[kk])/2.;
	  B[kk]=(B0[kk]+B1[kk])/2.;
	}
      sortie<<"p2=geom.MakeVertex("<<B[0]<<","<<B[1]<<","<<B[2]<<")"<<endl;
      sortie<<"geom.addToStudy(p2,\"vol"<<nbvol<<"p2\")"<<endl;
      sortie<<"p2=geom.MakeVertex("<<A[0]<<","<<A[1]<<","<<A[2]<<")"<<endl;
      sortie<<"geom.addToStudy(p2,\"vol"<<nbvol<<"p1\")"<<endl;
      for ( kk=0;kk<3;kk++)
	{
	  s=s+(B[kk]-A[kk])*(B[kk]-A[kk]);
	  B[kk]-=A[kk];
	}
      s=sqrt(s);
      sortie<<"p1=geom.MakePointStruct("<<A[0]<<","<<A[1]<<","<<A[2]<<")"<<endl;
      sortie<<"p2=geom.MakePointStruct("<<B[0]<<","<<B[1]<<","<<B[2]<<")"<<endl;
      sortie<<"v=geom.MakeDirection(p2)"<<endl;
      sortie <<"vol"<<nbvol<<"=geom.MakeCone(p1,v,"<<RTORMI<<","<<RTORMA<<","<<s<<")"<<endl;
     
      return ;
    }
  // exit(-1);
 
  //cout<<"cree_thore pas encore code"<<endl;
}
void cree_cone(istream& entree,ostream& sortie)
{
  double xo,yo,zo,r1,r2,h;
  int idir,indmat;
  entree>>xo>>yo>>zo>>r1>>r2>>h>>idir>>indmat;
  sortie<<"# cone "<<indmat<<endl;
  if (idir!=3) exit(-1);
  sortie <<"vol"<<nbvol<<"=geom.MakeCone(geom.MakePointStruct("<<xo<<","<<yo<<","<<zo<<"),geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),"<<r1<<","<<r2<<","<<h<<")"<<endl;
  addstudy(sortie,"vol",nbvol);  
  // sortie<<"geom.addToStudy(vol"<<nbvol<<",\"vol"<<nbvol<<"\")"<<endl;
  indice_vol[nbvol]=indmat;
  nbvol+=1;
}
void cree_sphere(istream& entree,ostream& sortie)
{
  double xo,yo,zo,rext,rint,xmin,xmax,ymin,ymax,zmin,zmax;
  int indmat;
  entree>>xo>>yo>>zo>>rext>>rint>>xmin>>xmax>>ymin>>ymax>>zmin>>zmax>>indmat;
  sortie<<"# sphere "<<indmat<<endl;
  sortie<<"v=geom.MakeSphere("<<xo<<","<<yo<<","<<zo<<","<<rext<<")"<<endl;
  if (rint>0.) {
    sortie<<"v1=geom.MakeSphere("<<xo<<","<<yo<<","<<zo<<","<<rint<<")"<<endl;
    //on coupe la sphere ext par la sphere int
    sortie<<"v=geom.MakeBoolean(v,v1,2)"<<endl;
  }
  if ((xo-rext)<xmin || (xo+rext)>xmax
      || (yo-rext)<ymin || (yo+rext)>ymax
      || (zo-rext)<zmin || (zo+rext)>zmax) {
    sortie <<"v1=geom.MakeBox("<<xmin<<","<<ymin<<","<<zmin<<","<<xmax<<","<<ymax<<","<<zmax<<")"<<endl;
    //on recupere la partie commune entre la sphere et la boite min/max
    sortie<<"v=geom.MakeBoolean(v,v1,1)"<<endl;
  }
  sortie <<"vol"<<nbvol<<"=v"<<endl;
  addstudy(sortie,"vol",nbvol);  
  indice_vol[nbvol]=indmat;
  nbvol+=1;
}
void cree_cone_incline(istream& entree,ostream& sortie)
{
  double zmin,r0int,r0ext,zmax,r1int,r1ext,phi,teta,xo,yo,zo;
  int indice;
  entree>>zmin>>r0int>>r0ext>>zmax>>r1int>>r1ext>>phi>>teta>>xo>>yo>>zo>>indice;
  sortie<<"# cone_incline "<<indice<<endl;
  // cout<<"CONE zmin="<<zmin<<"  r0int="<<r0int<<"  r0ext="<<r0ext<<endl;
  // cout<<"  zmax="<<zmax<<"  r1int="<<r1int<<"  r1ext="<<r1ext<<endl;
  //cout<<"  phi="<<phi<<"  teta="<<teta<<endl;
  //cout	<<"  xo="<<xo<<"  =yo"<<yo<<"  zo="<<zo<<endl;
  if (r0int==0. && r1int==0.) {
    if (r0ext==r1ext) {
      // memes rayons -> cylindre :
      sortie<<"volp=geom.MakeCylinder(geom.MakePointStruct("<<0<<","<<0<<","<<zmin<<"),geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),"<<r0ext<<","<<zmax-zmin<<")"<<endl;
    } else {
      // cone de base :
      sortie<<"volp=geom.MakeCone(geom.MakePointStruct("<<0<<","<<0<<","<<zmin<<"),geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),"<<r0ext<<","<<r1ext<<","<<zmax-zmin<<")"<<endl;
    }
    //on tourne une premiere fois le cone suivant oy angle -phi
    sortie<<"volp2=geom.MakeRotation(volp,geom.MakeAxisStruct(0,0,0,0,1,0),"<<-phi <<")"<<endl;
    // on tourne une deuxieme fois le cone suivant oz angle teta
    sortie<<"volp3=geom.MakeRotation(volp2,geom.MakeAxisStruct(0,0,0,0,0,1),"<<teta  <<")"<<endl;
    // on translate
    sortie<<"vol"<<nbvol<<"=geom.MakeTranslation(volp3,"<<xo<<","<<yo<<","<<zo<<")"<<endl;
  } else {
    if (r0ext==r1ext) {
      // memes rayons -> cylindre :
      sortie<<"volp=geom.MakeCylinder(geom.MakePointStruct("<<0<<","<<0<<","<<zmin<<"),geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),"<<r0ext<<","<<zmax-zmin<<")"<<endl;
    } else {
      // cone de base :
      sortie<<"volp=geom.MakeCone(geom.MakePointStruct("<<0<<","<<0<<","<<zmin<<"),geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),"<<r0ext<<","<<r1ext<<","<<zmax-zmin<<")"<<endl;
    }
    //on tourne une premiere fois le cone suivant oy angle -phi
    sortie<<"volp2=geom.MakeRotation(volp,geom.MakeAxisStruct(0,0,0,0,1,0),"<<-phi <<")"<<endl;
    // on tourne une deuxieme fois le cone suivant oz angle teta
    sortie<<"volp3=geom.MakeRotation(volp2,geom.MakeAxisStruct(0,0,0,0,0,1),"<<teta  <<")"<<endl;
    // on translate
    sortie<<"volp4=geom.MakeTranslation(volp3,"<<xo<<","<<yo<<","<<zo<<")"<<endl;

    if (r0int==r1int) {
      // memes rayons -> cylindre :
      sortie<<"volp5=geom.MakeCylinder(geom.MakePointStruct("<<0<<","<<0<<","<<zmin<<"),geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),"<<r0int<<","<<zmax-zmin<<")"<<endl;
    } else {
      // cone de base :
      sortie<<"volp5=geom.MakeCone(geom.MakePointStruct("<<0<<","<<0<<","<<zmin<<"),geom.MakeDirection(geom.MakePointStruct(0.,0.,1.)),"<<r0int<<","<<r1int<<","<<zmax-zmin<<")"<<endl;
    }
    // on tourne une premiere fois le cone suivant oy angle -phi
    sortie<<"volp6=geom.MakeRotation(volp5,geom.MakeAxisStruct(0,0,0,0,1,0),"<<-phi <<")"<<endl;
    // on tourne une deuxieme fois le cone suivant oz angle teta
    sortie<<"volp7=geom.MakeRotation(volp6,geom.MakeAxisStruct(0,0,0,0,0,1),"<<teta  <<")"<<endl;
    // on translate
    sortie<<"volp8=geom.MakeTranslation(volp7,"<<xo<<","<<yo<<","<<zo<<")"<<endl;
  
    sortie<<"vol"<<nbvol<<"=geom.MakeBoolean(volp4,volp8,2)"<<endl;
  }

  addstudy(sortie,"vol",nbvol);
  indice_vol[nbvol]=indice;
  nbvol+=1;
}
void cree_ellipse(istream& entree,ostream& sortie)
{
  double xo,yo,zo,r1,r2,rmin,rmax,zmin,zmax;
  int indmat,iext;
  entree>>xo>>yo>>zo>>r1>>r2>>rmin>>rmax>>zmin>>zmax>>iext>>indmat;
  sortie<<"# allipse "<<indmat<<endl;
  // on cree le tube de depart
  sortie<<"v=geom.MakeCylinder(geom.MakePointStruct("<<xo<<","<<yo<<","<<zmin<<") , geom.MakeDirection(geom.MakePointStruct(0,0,1)),"<<rmax<<","<<zmax-zmin<<")"<<endl;
  if (rmin!=0)
    {
      sortie<<"v1=geom.MakeCylinder(geom.MakePointStruct("<<xo<<","<<yo<<","<<zmin<<"),  geom.MakeDirection(geom.MakePointStruct(0,0,1)),"<<rmin<<","<<zmax-zmin<<")"<<endl;
      sortie<<"v=geom.MakeBoolean(v,v1,2)"<<endl;
    }
  
  if (r1==r2)
    {
      // on cree une sphere
      sortie<<"v2=geom.MakeSphere("<<xo<<","<<yo<<","<<zo<<","<<r1<<")"<<endl;
    }
  else
    { // on cree l'ellipse en 0
      /*
	if (r1<r2)
	sortie<<"rmin="<<r1<<"\nrmaj="<<r2<<endl;
	else
	sortie<<"rmin="<<r2<<"\nrmaj="<<r1<<endl;
	sortie<<"import math\nv2=geom.MakeEllipse( geom.MakePointStruct(0,0,0),geom.MakeDirection(geom.MakePointStruct(1,0,0)),rmaj,rmin)"<<endl;
	if (r1>r2)
	sortie<<"v2=geom.MakeRotation(v2,geom.MakeAxisStruct(0,0,0,1,0,0),math.asin(1))"<<endl;
	sortie<<"C1=[]\nC1.append(v2._get_Name())\nv2=geom.MakeFace(geom.MakeWire(C1),1)\np=geom.MakePlane( geom.MakePointStruct(0,2*rmaj,0),geom.MakeDirection(geom.MakePointStruct(1,0,0)),2*rmaj)\nv2=geom.MakeBoolean(v2,p,2)\nv2=geom.MakeRevolution(v2,geom.MakeAxisStruct(0,0,0,0,0,1),4*math.asin(1))"<<endl;
	sortie<<"v2=geom.MakeTranslation(v2,"<<xo<<","<<yo<<","<<zo<<")"<<endl;
      */
      sortie<<"v2=geom.MakeEllipsoide( "<<xo<<","<<yo<<","<<zo<<","<<r1<<","<<r1<<","<<r2<<")"<<endl;
    

    }
  int modedec=1;
  if (iext==1)
    {
      modedec=2;
    }
  sortie<<"v1=v"<<endl;
  addstudy(sortie,"v",1,2);
  addstudy(sortie,"v",2,2);
  sortie<<"volp=geom.MakeBoolean(v,v2,"<<modedec<<")"<<endl;
  sortie<<"vol"<<nbvol<<"=geom.SubShapeAll(volp,geom.ShapeType[\"SOLID\"])[0]"<<endl;
  addstudy(sortie,"vol",nbvol,2);
  indice_vol[nbvol]=indmat;
  nbvol+=1;
}
void cree_tube(istream& entree,ostream& sortie)
{
  double rc,Zomin,Zomax,phi,theta,xo,yo,zo;
  int indmat;
  double ri;

  entree>> rc>>Zomin>>Zomax>>phi>>theta>>xo>>yo>>zo>>indmat>>ri;
  sortie<<"# tube "<<indmat<<endl;
  //cout<<"cree_tube ne fait rien on attend tubem pour faire"<<endl;
  sortie<<"volp=geom.MakeCylinder(\n   geom.MakePointStruct(0,0,"<<Zomin<<"),\n   geom.MakeDirection(geom.MakePointStruct(0,0,1)),\n   "<<rc<<","<<Zomax-Zomin<<")"<<endl;
  if (ri!=0) 
    {
      sortie<<"volsa=volp"<<endl;
      sortie<<"volprov=geom.MakeCylinder(\n   geom.MakePointStruct(0,0,"<<Zomin<<"),\n   geom.MakeDirection(geom.MakePointStruct(0,0,1)),\n   "<<ri<<","<<Zomax-Zomin<<")"<<endl;

    
      sortie<<"volp=geom.MakeBoolean(volsa,volprov,2)"<<endl;
    }
  // on tourne une premiere fois la boite suivant oy angle -phi
  sortie<<"volp2=geom.MakeRotation(volp,geom.MakeAxisStruct(0,0,0,0,1,0),"<<-phi <<")"<<endl;
  // on tourne une deuxieme fois la boite suivant oz angle theta
  sortie<<"volp3=geom.MakeRotation(volp2,geom.MakeAxisStruct(0,0,0,0,0,1),"<<theta  <<")"<<endl;
  // on translate
  sortie<<"vol"<<nbvol<<"=geom.MakeTranslation(volp3,"<<xo<<","<<yo<<","<<zo<<")"<<endl;
  addstudy(sortie,"vol",nbvol);
  indice_vol[nbvol]=indmat;
  nbvol+=1;
}
void cree_tubem_pir(istream& entree,ostream& sortie)
{
  double rc,hc;
  int indmat;
  double ri;
  double x,y,z; 
  double xnx,xny,xnz;
 
  entree>> x>>y>>z>>xnx>>xny>>xnz>>rc>>hc>>indmat>>ri;
}
void cree_tubem(istream& entree,ostream& sortie)
{
  // ne doit plus servir////

  exit(-1);
  double rc,hc;
  int indmat;
  double ri;
  double x,y,z;
  double xnx,xny,xnz;

  entree>> x>>y>>z>>xnx>>xny>>xnz>>rc>>hc>>indmat>>ri;
  sortie<<"# tubem "<<indmat<<endl;
  // cout<<"cree_tubem non code"<<endl;
 
  sortie<<"vol"<<nbvol<<"=geom.MakeCylinder(\n   geom.MakePointStruct("<<x<<","<<y<<","<<z<<"),\n   geom.MakeDirection(geom.MakePointStruct("<<xnx<<","<<xny<<","<<xnz<<")),\n   "<<rc<<","<<hc<<")"<<endl;
  if (ri!=0) 
    {
      sortie<<"volsa=vol"<<nbvol<<endl;
      sortie<<"volprov=geom.MakeCylinder(\n   geom.MakePointStruct("<<x<<","<<y<<","<<z<<"),\n   geom.MakeDirection(geom.MakePointStruct("<<xnx<<","<<xny<<","<<xnz<<")),\n   "<<ri<<","<<hc<<")"<<endl;
    
      sortie<<"vol"<<nbvol<<"=geom.MakeBoolean(volsa,volprov,2)"<<endl;
    }
  nbligne+=6;
  nbsurf+=4;
  addstudy(sortie,"vol",nbvol);
  //sortie<<"geom.addToStudy(vol"<<nbvol<<",\"vol"<<nbvol<<"\")"<<endl;  
  indice_vol[nbvol]=indmat;
  nbvol+=1;
}

void cree_tube_sect(istream& entree,ostream& sortie)
{

  double rc,Zomin,Zomax,phi,theta,xo,yo,zo;
  int indmat;
  double ri;
 
  entree>> rc>>Zomin>>Zomax>>phi>>theta>>xo>>yo>>zo>>indmat>>ri;
  sortie<<"# tube_secteur "<<indmat<<endl;
  //cree_tube(entree,sortie);
  double angle1,angle2;

  entree>>angle1>>angle2;
  // dans vol nbvol on a le cylindre complet
  // on le coupe par le premier plan
  double rix=ri*cos(angle1);
  double riy=ri*sin(angle1);
  double rcx=rc*cos(angle1);
  double rcy=rc*sin(angle1);
  sortie<<"p1=geom.MakePointStruct("<<rix<<","<<riy<<","<<Zomin<<")"<<endl;
  sortie<<"p2=geom.MakePointStruct("<<rcx<<","<<rcy<<","<<Zomin<<")"<<endl;
  sortie<<"p3=geom.MakePointStruct("<<rcx<<","<<rcy<<","<<Zomax<<")"<<endl;
  sortie<<"p4=geom.MakePointStruct("<<rix<<","<<riy<<","<<Zomax<<")"<<endl;
  sortie<<"w=geom.MakeWire([geom.MakeEdge(p1,p2),geom.MakeEdge(p2,p3),geom.MakeEdge(p3,p4),geom.MakeEdge(p4,p1)])"<<endl;
  sortie<<"p=geom.MakeFace(w,1)"<<endl;
  sortie <<"volp0=geom.MakeRevolution(p,geom.MakeAxisStruct(0,0,0,0,0,1),"<<(angle2-angle1)<<")"<<endl;

  // on tourne une premiere fois la boite suivant oy angle -phi
  sortie<<"volp2=geom.MakeRotation(volp0,geom.MakeAxisStruct(0,0,0,0,1,0),"<<-phi <<")"<<endl;
  // on tourne une deuxieme fois la boite suivant oz angle theta
  sortie<<"volp3=geom.MakeRotation(volp2,geom.MakeAxisStruct(0,0,0,0,0,1),"<<theta  <<")"<<endl;
  // on translate
  sortie<<"vol"<<nbvol<<"=geom.MakeTranslation(volp3,"<<xo<<","<<yo<<","<<zo<<")"<<endl;
  //  addstudy(sortie,"volp",0);

  addstudy(sortie,"vol",nbvol);
  indice_vol[nbvol]=indmat;
  nbvol+=1;
}
void cree_boite(istream& entree,ostream& sortie)
{
  double x[2],y[2],z[2];
  int indice;
  entree>>x[0]>>x[1]>>y[0]>>y[1]>>z[0]>>z[1]>>indice;
  sortie<<"# boite "<<indice<<endl;
  //cerr<<"indice " <<indice<<endl;
  sortie <<"vol"<<nbvol<<"=geom.MakeBox("<<x[0]<<","<<y[0]<<","<<z[0]<<","<<x[1]<<","<<y[1]<<","<<z[1]<<")"<<endl;
  addstudy(sortie,"vol",nbvol);
  indice_vol[nbvol]=indice;
  nbvol+=1;
	  
   
}

void cree_boite_inclinee(istream& entree,ostream& sortie)
{
  double x[2],y[2],z[2];
  int indice;
  entree>>x[0]>>x[1]>>y[0]>>y[1]>>z[0]>>z[1]>>indice;
  sortie<<"# boite_inclinee "<<indice<<endl;
  double phi,theta,xo,yo,zo;
  entree >>phi>>theta>>xo>>yo>>zo;
  //     double dx=x[1]-x[0];
  //     double dy=y[1]-y[0];
  //cerr<<"indice " <<indice<<endl;
  
  sortie <<"volp=geom.MakeBox("<<x[0]<<","<<y[0]<<","<<z[0]<<","<<x[1]<<","<<y[1]<<","<<z[1]<<")"<<endl;
  // on tourne une premiere fois la boite suivant oy angle -phi
  sortie<<"volp2=geom.MakeRotation(volp,geom.MakeAxisStruct(0,0,0,0,1,0),"<<-phi <<")"<<endl;
  // on tourne une deuxieme fois la boite suivant oz angle theta
  sortie<<"volp3=geom.MakeRotation(volp2,geom.MakeAxisStruct(0,0,0,0,0,1),"<<theta  <<")"<<endl;
  // on translate
  sortie<<"vol"<<nbvol<<"=geom.MakeTranslation(volp3,"<<xo<<","<<yo<<","<<zo<<")"<<endl;
  //sortie<<"vol"<<nbvol<<"=volp2"<<endl;

  addstudy(sortie,"vol",nbvol);
  
  indice_vol[nbvol]=indice;
  nbvol+=1;
  
}
void cree_comment(istream& entree,ostream& sortie)
{
  sortie<<"#";
  nom Nomp;
  get_mot(entree,Nomp);
  while (strcmp(Nomp,"end_comment"))  
    {
      sortie<<" "<<Nomp.str;
      get_mot(entree,Nomp);
    }
  sortie<<endl;
}
void cree_macro(istream& entree,ostream& sortie)
{
  double x[3];int numero=-1;nom Name;
  int indice;
  //  name=new char[2];name[0]='\0';
  get_mot(entree,Name);char* name=Name.str;
  entree>>x[0]>>x[1]>>x[2]>>indice;
  int newm=1;
  
  for (int ii=0;ii<nmacmax;ii++)
    if (vectmacro[ii].n!=-1)
      {
	if (!strcmp(vectmacro[ii].name,name)) {newm=0;numero=ii;break;};
      }
    else
      numero=ii;
  if (newm)
    {
      if (numero==-1) { cerr<<" Plus de place pour les macros "<<endl;exit(-1);}
      strcpy(vectmacro[numero].name,name);
      vectmacro[numero].n=1;
      //for (i=0;i<3;i++)
      //listmacro[numero].pos[i]=x[i];
      sortie<<"# on charge la macro"<<endl;
      //     char* buf=new char[801];

      //       int longueur=800;
      //       getcwd(buf, longueur);
      sortie<<"if not os.path.exists(DIR1+\"macro"<<name<<".brep\"):"<<endl;
      sortie <<"  import macro"<<name<<endl;
      sortie<<"pr=geom.ImportBREP(DIR1+\"macro"<<name<<".brep\")"<<endl;
      sortie<<"mac"<<name<<"=geom.MakeTranslation(pr,"<<-x[0]<<","<<-x[1]<<","<<-x[2]<<")"<<endl;
    }
  sortie<<"vol"<<nbvol<<"=geom.MakeTranslation(mac"<<name<<","<< x[0]<<","<<x[1]<<","<<x[2]<<")"<<endl;
  //sortie<<"vol"<<nbvol<<"=volp2"<<endl;

  addstudy(sortie,"vol",nbvol);
  
  indice_vol[nbvol]=indice;
  nbvol+=1;
  nom Nomp;
 
  //  nomp=new char[2];
  get_mot(entree,Nomp);
  char* nomp=Nomp.str;
  
  char* name2= new char[100];
  name2[0]='\0';
  char* mac=new char[100];
  strcpy(mac,"macro");
  //mac[5]='0'+numero;
  strcat(mac,name);
  strcat(mac,".trace");
  ofstream p;
  if (newm)
    {
      p.open(mac);
      p<<nomp;
    }
  while (strcmp(name,name2))
    {
      while (strcmp(Nomp,"end_macro"))  {get_mot(entree,Nomp);if (newm) p<<" "<<Nomp.str;}
  
      entree>> name2;
      Nomp.str[0]='\0';
      if (strcmp(name,name2)) { if (newm) p<<" "<<name2;
	//cerr<<"bizarre dans macro "<<numero<<",  fin :"<<ip<<endl<<name<<" "<<name2<<endl;;
      }
    }
  if (newm) 
    {
      //  p<<endl;
      p.close();
    }
  //  if (ip!=numero) { cerr<<"bizarre dans macro "<<numero<<",  fin :"<<ip<<endl;}
  delete [] name2;
  delete [] mac;
}

void entete(ostream& sortie,int argc,char * argv[])
{
  
  sortie<<"# genere par :";
  for (int i=0;i<argc;i++) sortie<<argv[i]<<" ";
  sortie<<endl;
  sortie<<"# -*- coding: latin-1 -*-"<<endl;

#ifdef STD
  sortie<<"import salome\nimport geom \ngeom=salome.lcc.FindOrLoadComponent(\"FactoryServer\", \"Geometry\")"<<endl;
  sortie<<"gg = salome.ImportComponentGUI(\"Geometry\")\nimport SalomePyQt\nsgPyQt=SalomePyQt.SalomePyQt()"<<endl;
#else
  sortie <<"import mygeompy\ngeom=mygeompy\n";
#endif
  sortie<<"import os"<<endl<<"os.system(\"date\")"<<endl;
  char* buff;
  buff=getenv("DBG_TRAD");
  if (buff)
    sortie<<"moderef="<<buff<<";"<<endl;
  else
    sortie<<"moderef=2;"<<endl;
  buff=getenv("HOME");
  sortie<<"HOME_ORG=\""<<buff<<"\""<<endl;
  //delete [] buff;
  sortie<<"if os.environ.has_key(\"DBG_TRAD\"):"<<endl;
  sortie<<" moderef=eval(os.environ[\"DBG_TRAD\"])"<<endl;
  sortie<<"if os.environ.has_key(\"DIR1\"):"<<endl;
  sortie<<"  DIR1=os.environ[\"DIR1\"]+\"/\""<<endl;
  sortie<<"else:"<<endl;
  sortie<<"  DIR1=\"./\""<<endl;
  sortie<<"pass"<<endl;
  // sortie<<"DIR1=\"/home/fauchet/testOCC/\""<<endl;
  /*
    sortie<<"HO=os.environ[\"HOME\"]"<<endl;
  
    sortie<<"from string import replace"<<endl;
    sortie<<"DIR1=replace(DIR1,HOME_ORG,HO)"<<endl;
    sortie<<"print \"Fichiers resultats dans \",DIR1"<<endl; 
  */
  //delete [] buf;
  sortie<<"def addstd(nom,nom2,mode):"<<endl;

  sortie<<  " if mode>moderef:"<<endl<<"  idd=geom.addToStudy(nom,nom2)"<<endl;
  sortie<<"  try:"<<endl<<"    import salome"<<endl<<"    salome.sg.updateObjBrowser(1)"<<endl<<"  except:"<<endl<<"    pass"<<endl;
	  

#ifdef STD
  sortie<<" gg.createAndDisplayGO(idd)"<<endl;
  sortie<<" #gg.setDisplayMode(idd,1)"<<endl;
#endif

  //sortie<<" geom.addToStudy(nom,nom2)"<<endl;

  sortie<<" if (os.path.exists(DIR1+\"stop\")):"<<endl;
  sortie<<"    print \"stop by user\""<<endl;
  sortie<<"    print 1/0"<<endl;
  //sortie <<" geom.CheckShape(nom)"<<endl;
  sortie <<" geom.ExportBREP(nom,DIR1+nom2+\".brep\")"<<endl ;
 
  // sortie<<"print \""<<nom<<volc<<" cree\""<<endl;
#ifdef STD
  sortie<<" sgPyQt.putInfo(  nom2 + \" cree\")"<<endl;
#endif
  sortie<<" os.system(\"echo \"+ nom2 + \" cree ` date`\")"<<endl;;
}

int main(int argc, char* argv[])
{
  buf=new char[801];

  int longueur=800;
  getcwd(buf, longueur);
  indice_vol=new int[nbvolmax];
  marq=new int[nbvolmax];
  marq[0]=0;
  vectmacro=new macro[nmacmax];
  
  ifstream entree(argv[1]);
  if (!entree)  error(argv[1]," non ouvrable");
  nom nombase; 
  ofstream sortie("es.py");
  sortie.precision(10);
  entete(sortie,argc,argv);
  while (entree)
    {
      get_mot(entree,nombase,1);
      if (!entree.eof())
	{
	  if (strcmp(nombase,"PARALAX")==0) {cree_boite(entree,sortie);
	    //   sortie<<"rior"<<endl;
	  }
	  else  if (strcmp(nombase,"CYLAX")==0) cree_cylindre(entree,sortie);
	  else  if (strcmp(nombase,"REVOLAX")==0) cree_thore(entree,sortie);
	  else  if (strcmp(nombase,"REVOLGEN")==0) cree_thore_gen(entree,sortie);
	  else  if (strcmp(nombase,"tube")==0) cree_tube(entree,sortie);
	  else  if (strcmp(nombase,"tubem_plus_utilise")==0) cree_tubem(entree,sortie);
	  else  if (strcmp(nombase,"tubem")==0) cree_tubem_pir(entree,sortie);
	  else  if (strcmp(nombase,"CONE")==0) cree_cone(entree,sortie);
	  else  if (strcmp(nombase,"conegen")==0) cree_cone_incline(entree,sortie);
	  else  if (strcmp(nombase,"paragen")==0) cree_boite_inclinee(entree,sortie);
	  else  if (strcmp(nombase,"ellipse")==0) cree_ellipse(entree,sortie);
	  else  if (strcmp(nombase,"sphere")==0) cree_sphere(entree,sortie);
	  else  if (strcmp(nombase,"tube_secteur")==0) cree_tube_sect(entree,sortie);
	  else  if (strcmp(nombase,"debut_macro")==0) cree_macro(entree,sortie);
	  else  if (strcmp(nombase,"comment")==0) cree_comment(entree,sortie);
	  else  if (strcmp(nombase,"marqueur")==0) {nbmarq++;marq[nbmarq]=nbvol;sortie<<"# marqueur"<<endl;}
	  else 
	    {
	      cout<<nombase.str<<" pas code "<<nbvol<<endl;
	      exit(-1);
	    }
	}
      //delete [] nombase;
      if (nbvol==1)
	{
	  //cout <<indice_vol[0]<<endl;
	  if (argc>2)
	    if (indice_vol[0]!=atoi(argv[2])) 
	      {
		nbvol=0;
		cout<<"Volume passe " <<indice_vol[0]<<endl;
		sortie.close();
		sortie.open("es.py");
		entete(sortie,argc,argv);
	      }
	  //exit(-1);
	}
    }
  nbmarq++;
  marq[nbmarq]=nbvol;
  sortie<<"# marqueur"<<endl;
  //cerr<<nombase<<endl<<flush;

  //cout <<"nbligne "<<nbligne<<endl;
  // cout <<"nbsurf "<<nbsurf<<endl;
  cout <<"nbvol "<<nbvol<<endl;
  sortie.close();
  ofstream sortie2("intersect.py");
  
  operation_volume(sortie2,argc,argv);
  
  //ofstream sortie3("tout.out");
 
  int min,minsa=-1000000,ok=1;
  while(ok)
    {
      ok=0;
      min=1000000;
      for (int v=0;v<nbvol;v++)
	{
	  if ((indice_vol[v]<min)&& (indice_vol[v]>minsa))
	    {
	      min=indice_vol[v];
	      ok=1;
	      //	      indice_vol[v]=b;
	    }
	}
      if (ok==1) cout<<" list indice vol "<<min<<endl;
      //if (minsa!=min)
      {minsa=min;min+=1;}
    }
  
  delete [] indice_vol;
  delete [] marq;
  delete [] vectmacro;
}

