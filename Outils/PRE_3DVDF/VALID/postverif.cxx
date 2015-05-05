// genere par :./traduction_Qt el.trace 
//fichier genere par traduction_Qt
#include <stdio.h>
#include <config.h>
#include <stdlib.h>
//#include <math.h>

#include <BRep_Tool.hxx>
#include <BRepTools.hxx>

#include <BRepPrimAPI_MakePrism.hxx>

#include <TopoDS_Shape.hxx>

#include <BRepClass3d_SolidClassifier.hxx>
#include <TopAbs.hxx>

/*
extern "C" 
{ 
  void read_data_(int *,int* ,int *,int *);
  void read_data_2_(int *,int* ,int *,int *);
  void write_file_data_(float*,float*,float*,int*,int* ,int *,int *);
  void write_file_data_2_(float*,float*,float*,int*,int* ,int *,int *);
}
*/

void read_file_cpp(ifstream& entree,int* indmail,int nx,int ny,int nz)
{
  nx-=1;
  ny-=1;
  nz-=1;
  int n1,n2;
  entree.read((char *)&n1,sizeof(int));
  n1=n1/sizeof(int);
  if (n1!=nx*ny*nz) { cerr<<"pb taille "<<n1<<" "<<nx*ny*nz<<endl;exit(-1); }
  entree.read((char *)indmail,n1*sizeof(int));
  entree.read((char *)&n2,sizeof(int));
  n2=n2/sizeof(int);
  if (n1!=n2) { cerr<<"pb taille final "<<n1<<" "<<n2<<endl;exit(-1); }
}
void read_data_cpp(int* indmail,int nx,int ny,int nz)
{ 
  ifstream entree("avs/model.val");
  read_file_cpp(entree,indmail, nx,ny, nz);
}
void read_data_2_cpp(int* indmail,int nx,int ny,int nz)
{
  ifstream entree("verif.val");
  for (int i=0;i<3;i++)
    {
      int nx1;
      entree.read((char *) &nx1, sizeof(int)) ;
      float* p=new float[nx1/sizeof(float)];
      entree.read((char *) p, nx1);
      int nx2;
      entree.read((char *) &nx2, sizeof(int)) ;
      if (nx1!=nx2) { cerr<<" pb dans verif.val "<<nx1<<" "<<nx2 <<endl;exit(-1);}
      delete [] p;
    }
  read_file_cpp(entree,indmail, nx,ny, nz);
}
void write_file_cpp(const char * name,float* xi,float *yi,float *zi, int* indmail,int nx,int ny,int nz)
{
  
  ofstream sortie(name);
  int nxb=(nx*sizeof(float));
  sortie.write((char *)&nxb,sizeof(int));
  sortie.write((char *)xi,nxb);
  sortie.write((char *)&nxb,sizeof(int));
  int nyb=(ny*sizeof(float));
  sortie.write((char *)&nyb,sizeof(int));
  sortie.write((char *)yi,nyb);
  sortie.write((char *)&nyb,sizeof(int));
  int nzb=(nz*sizeof(float));
  sortie.write((char *)&nzb,sizeof(int));
  sortie.write((char *)zi,nzb);
  sortie.write((char *)&nzb,sizeof(int));

  // indmail
  int nb=((nx-1)*(ny-1)*(nz-1)*sizeof(int));
  sortie.write((char *)&nb,sizeof(int));
  sortie.write((char *)indmail,nb);
  sortie.write((char *)&nb,sizeof(int));

}
void write_file_data_cpp(float* xi,float *yi,float *zi, int* indmail,int nx,int ny,int nz)
{
  write_file_cpp("verif.val",xi,yi,zi, indmail, nx,ny, nz);
}
void write_file_data_2_cpp(float* xi,float *yi,float *zi, int* indmail,int nx,int ny,int nz)
{
  write_file_cpp("verif2.val",xi,yi,zi, indmail, nx,ny, nz);
}


int read_tab(const char* name,float* val,int mode=0)
{
  
  int i=-1;float x;
   if (mode==0)
    {
           ifstream entree(name);

      while (!entree.eof()) { i++;entree>>x;} 
      entree.close();
      //    val=new float[i];
      return i;
      cout<<" fin premiere lecture "<<i<<endl;
      // return read_tab(name,val,i);
    }
 else
    {
      // mode=i;
      ifstream entree(name);
      for (int j=0;j<mode;j++) entree>>val[j];
      entree.close();
    }
   //cout <<"fin deuxieme lecture " <<endl;
  return mode;
}

int imin(int a,int b)
{
  if (a<b) return a; else return b;
}
int imax(int a,int b)
{
  if (a>b) return a; else return b;
}

int get_bornes(int argc, char* argv[], int nx,int& n0x,int& n1x, int ny,int& n0y,int& n1y, int nz , int& n0z, int& n1z, int& min , int& max)
{
  n0x=1;
  n1x=nx-2;
  n0y=1;
  n1y=ny-2;
  n0z=1;
  n1z=nz-2;
  
  int  sztot=(n1x-n0x)*(n1y-n0y)*(n1z-n0z);
  min=0;
  max=(nx-1)*(ny-1)*(nz-1);
  cerr<<" nombre de cases "<<sztot<<endl;
  if (argc==5)
    {
      cerr<<"on change min max des cases"<<endl;
      min=atoi(argv[3]);
      int max_0=max;
      max=atoi(argv[4]);
      if (max>max_0) max=max_0;
      cerr<<"apres "<<min<<" "<<max<<endl;
    }
  sztot=max-min;
  if (argc==9)
    {
      n0x=imax(n0x,atoi(argv[3]));
      n1x=imin(n1x,atoi(argv[4]));
      n0y=imax(n0y,atoi(argv[5]));
      n1y=imin(n1y,atoi(argv[6]));
      n0z=imax(n0z,atoi(argv[7]));
      n1z=imin(n1z,atoi(argv[8]));
      sztot=(n1x-n0x)*(n1y-n0y)*(n1z-n0z);
    }
  return sztot;
}
 
int main_2 (int argc, char* argv[] ) {
  
  TopoDS_Shape shape;
  BRep_Builder B;
  
  //  ifstream entree;
  float* xi;float* yi; float* zi;
  int nx,ny,nz;
  int n0x,n1x,n0y,n1y,n0z,n1z;
  int err=0;
  nx=read_tab("avs/model.xi",xi);
  xi=new float[nx];
  read_tab("avs/model.xi",xi,nx);
  
  //cout<<xi[nx-1]<<endl;;
  ny=read_tab("avs/model.yi",yi);
  yi=new float[ny];
  read_tab("avs/model.yi",yi,ny);
  nz=read_tab("avs/model.zi",zi);
  zi=new float[nz];
  read_tab("avs/model.zi",zi,nz);
  cout<<"nx "<<nx<<" ny "<<ny<<" nz "<<nz <<endl;
  int* indmail;
  int nbm=(nx-1)*(ny-1)*(nz-1);
  indmail =new int[nbm];
    
  for (int k=0;k<nbm;k++)
    indmail[k]=0;
  int* indmailprep;
  indmailprep=new int[nbm];
  read_data_cpp(indmailprep,nx,ny,nz);
   
  int ref=atoi(argv[2]);
  int tot=0;
  int tmp,compt=0,pourcent=0,sztot;
  sztot=(nx-1)*(ny-1)*(nz-1);
  int min=0;
  int max=sztot;
  cerr<<" Nombre de cases "<<sztot<<endl;
  
  sztot=max-min;
  sztot=get_bornes(argc, argv,  nx, n0x, n1x, ny,n0y, n1y,  nz ,  n0z,  n1z,  min ,  max);
  cerr<<  n0x<<" "<< n1x<<" "<<n0y<<" "<< n1y<<" "<<  n0z<<" "<<  n1z<<" "<<  min <<" "<<  max<<endl;
  cout<<"verif "<<ref<<" file "<<argv[1]<<endl;
  BRepTools::Read(shape,argv[1],B);
  cout<<"fin de lecture de "<<argv[1]<<endl;
  BRepClass3d_SolidClassifier classif(shape);

  for (int i=n0x;i<n1x;i++)
    for (int j=n0y;j<n1y;j++)
      for (int k=n0z;k<n1z;k++)
	    
    {
      int sto=i+j*(nx-1)+k*(nx-1)*(ny-1);
      if( (sto>=min)&&(sto<max))
	{
      compt++;
      tmp=(compt*1000)/sztot;
      if (tmp> pourcent)
	{
	  pourcent=tmp;
	  cerr<<"\rOn a traite "<<tmp <<" %% des elements"<<flush;
	}

      // cout <<k<< " " <<zi[k] << " "<<zi[k+1]<<endl;
      float z=(zi[k]+zi[k+1])/2.;
      // cout <<" z "<<z<<endl;
      float y=(yi[j]+yi[j+1])/2.;
      float x=(xi[i]+xi[i+1])/2.;
      gp_Pnt P(x,y,z);
  
      classif.Perform( P,1e-6);
      /*
      if  ((i==3)&&( j==13)&&( k== 0))
	cout<<TopAbs_IN<<"  "<<classif.State()<<"  "<<i<< " "<<j<<" "<<k<<" x"<<x<<" y" <<y<<" z "<<z<<endl;
      */
      int on=0;
      switch(classif.State())
	{
	case   TopAbs_ON:
	  cout<<"\n ON on considere comme IN"<<endl;
	  on=1;
	case  TopAbs_IN: 
	  //cout<<"IN"<<endl;
	  //int elemz=sto;
	  tot++;indmail[sto]=ref;
	  if (indmailprep[sto]!=ref)
	    { err++;
	      cout <<"\n"<<sto<<" Pb maille in for occ "<<i<< " "<<j<<" "<<k;
	      if (on) cout <<" ON ";
	      cout<<endl;;
	      cout<<x<<" "<<y<<" "<<z<<endl;
	    }
	  break;
	case    TopAbs_OUT:
	  if (indmailprep[sto]==ref)
	    { err++;
	      cout <<"\n"<<sto<<" Pb maille out for occ "<<i<< " "<<j<<" "<<k<<endl;
	      cout<<x<<" "<<y<<" "<<z<<endl;
	    }
	  //cout<<"OUT"<<endl;
	  break;
	case   TopAbs_UNKNOWN: 
	  cout<<"???"<<endl;
	  break;
	default: cout<<"gros_pb"<<endl;exit(-1);
	}
    }
    }
  cout <<endl<<" total "<<tot<<endl;
  int err2=0;
  // cout<<indmailprep[1*nz*ny+0*nz+0]<<endl;
   for (int k=min;k<max;k++) 
     {
       //cout <<k<<" k " <<indmailprep[k]<<endl;
       if (((indmailprep[k]==ref) &&(indmail[k]!=ref))||((indmail[k]==ref) &&(indmailprep[k]!=ref)))
       {
	 //	 [sto]
	 int k2=k/((nx-1)*(ny-1));
	 int j2=(k-k2*(nx-1)*(ny-1))/(nx-1);
	 int i2=(k-j2*(nx-1)-k2*(nx-1)*(ny-1));
	 if ((k2<n1z)&&(k2>=n0z)&&(j2<n1y)&&(j2>=n0y)&&(i2<n1x)&&(i2>=n0x))
	 {
	   err2++;
	 cout<<"Pb mail k "<<k<<" prep "<<indmailprep[k]<<" nous "<<indmail[k];
	
	 cout<<" i "<<i2<<" j "<<j2<<" k " << k2<<" pos "<<(xi[i2]+xi[i2+1])/2.<<" "<<(yi[j2]+yi[j2+1])/2.<<" "<<(zi[k2]+zi[k2+1])/2.<<endl;
	 }
       }
	 
     }
 
  // on sauvegarde tout
  ofstream sortie("verif.prep");
  sortie<<"TAILLES 3 "<<nx<<" "<<ny<<" "<<nz<<endl;
  sortie<<"RIEN 0 presence calcule a partir de "<<argv[1]<<" "<<argv[2]<<endl;
  sortie<<"FORT 0 open(UNIT=12,FILE='verif.val',FORM='UNFORMATTED')"<<endl;
  sortie<<"FORT 0 READ(12) XM"<<endl;
  sortie<<"FORT 0 READ(12) YM"<<endl;
  sortie<<"FORT 0 READ(12) ZM"<<endl;
  sortie<<"FORT 0 READ(12) INDMAIL"<<endl;
  sortie<<"FORT 0 CLOSE(12)"<<endl;
  write_file_data_cpp(xi,yi,zi,indmail,nx,ny,nz);
  delete []   xi;
  delete []   yi;
  delete []   zi;
  delete []   indmail ;
  delete []   indmailprep;
  cout<<"\n nomdre d'erreurs "<<err<<endl;
  if (err!=err2) {cerr<<"gros pb nb erreurs "<<err<<" "<<err2<<endl;exit(-1);}
  return err;

}

int main_fin (int argc, char* argv[] ) {

  int err=0;
  //  TopoDS_Shape shape;
  // BRep_Builder B;
  
  //  ifstream entree;
  float* xi;float* yi; float* zi;
  int nx,ny,nz;
 
  nx=read_tab("avs/model.xi",xi);
  xi=new float[nx];
  read_tab("avs/model.xi",xi,nx);
  
  //cout<<xi[nx-1]<<endl;;
  ny=read_tab("avs/model.yi",yi);
  yi=new float[ny];
  read_tab("avs/model.yi",yi,ny);
  nz=read_tab("avs/model.zi",zi);
  zi=new float[nz];
  read_tab("avs/model.zi",zi,nz);
  cout<<"nx "<<nx<<" ny "<<ny<<" nz "<<nz <<endl;
  int* indmail;
  int nbm=(nx-1)*(ny-1)*(nz-1);
  indmail =new int[nbm];
  
  for (int k=0;k<nbm;k++)
    indmail[k]=0;
  int* indmailprep;
  indmailprep=new int[nbm];
  read_data_cpp(indmailprep,nx,ny,nz);
  //read_data_2_(indmail,&nx,&ny,&nz);
  read_data_2_cpp(indmail,nx,ny,nz);
  int ref=atoi(argv[2]);
  ofstream testc3("testc3");
  testc3<<"mu4\npload ALL\nrestore "<<argv[1]<<" A "<<endl; 
  testc3<<"whatis A \n checkshape A\n"<<endl;
  int  sztot=(nx-1)*(ny-1)*(nz-1);
  int min=0;
  int max=sztot;
    int n0x,n1x,n0y,n1y,n0z,n1z;
    sztot=get_bornes(argc, argv,  nx, n0x, n1x, ny,n0y, n1y,  nz ,  n0z,  n1z,  min ,  max);
  for (int k=min;k<max;k++) 
    {
      //cout <<k<<" k " <<indmailprep[k]<<endl;
      if (((indmailprep[k]==ref) &&(indmail[k]!=ref))||((indmail[k]==ref) &&(indmailprep[k]!=ref)))
	{
		  //	 [sto]
	  int k2=k/((nx-1)*ny);
	  int j2=(k-k2*(nx-1)*ny)/(nx-1);
	  int i2=(k-j2*(nx-1)-k2*(nx-1)*ny);
	 if ((k2<n1z)&&(k2>=n0z)&&(j2<n1y)&&(j2>=n0y)&&(i2<n1x)&&(i2>=n0x))
	   { 
	  err++;
	  cout<<"Pb mail k "<<k<<" prep "<<indmailprep[k]<<" nous "<<indmail[k];

	  cout<<" i "<<i2<<" j "<<j2<<" k " << k2<<" pos "<<(xi[i2]+xi[i2+1])/2.<<" "<<(yi[j2]+yi[j2+1])/2.<<" "<<(zi[k2]+zi[k2+1])/2.<<endl;
	 
	  if (indmailprep[k]==ref)
	    { testc3<<"point b";
	    indmail[k]=ref*2;
	    }
	  else
	    {
	      testc3<<"point a";
	    indmail[k]=ref;
	    }
	  testc3<<err<<  " "<<(xi[i2]+xi[i2+1])/2.<<" "<<(yi[j2]+yi[j2+1])/2.<<" "<<(zi[k2]+zi[k2+1])/2. <<endl;
	   if (indmailprep[k]==ref)
		   testc3<<"bclassify A b"<<err<<endl;
	   else
		   testc3<<"bclassify A a"<<err<<endl;
	}
    }
      else indmail[k]=0;
      
    }
  testc3<<"fit"<<endl;
  testc3.close();
  if (err)
    { 
 // on sauvegarde tout
  ofstream sortie("verif2.prep");
  sortie<<"TAILLES 3 "<<nx<<" "<<ny<<" "<<nz<<endl;
  sortie<<"RIEN 0 erreur calculee a partir de "<<argv[1]<<" "<<argv[2]<<endl;
  sortie<<"FORT 0 open(UNIT=12,FILE='verif2.val',FORM='UNFORMATTED')"<<endl;
  sortie<<"FORT 0 READ(12) XM"<<endl;
  sortie<<"FORT 0 READ(12) YM"<<endl;
  sortie<<"FORT 0 READ(12) ZM"<<endl;
  sortie<<"FORT 0 READ(12) INDMAIL"<<endl;
  sortie<<"FORT 0 CLOSE(12)"<<endl;
  write_file_data_2_cpp(xi,yi,zi,indmail,nx,ny,nz);
    }
  cout<<" nombre d'erreur " <<err<<endl;
  delete []   xi;
  delete []   yi;
  delete []   zi;
  delete []   indmail ;
  delete []   indmailprep;

  return err;

}

int main ( int argc, char* argv[] ) 
{
  int err1=main_2(argc,argv);
  if (1)
    {
      int err2=main_fin(argc,argv);
      if (err1!=err2) cerr<<"bizarre " <<endl;
    }
  cerr<<"ok"<<endl;
  return err1;
}

