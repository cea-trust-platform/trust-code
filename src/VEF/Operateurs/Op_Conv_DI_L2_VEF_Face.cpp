/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Op_Conv_DI_L2_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/21
//
//////////////////////////////////////////////////////////////////////////////


#include <Op_Conv_DI_L2_VEF_Face.h>
#include <Champ_P1NC.h>
#include <Schema_Temps_base.h>


Implemente_instanciable(Op_Conv_DI_L2_VEF_Face,"Op_Conv_DI_L2_VEF_P1NC",Op_Conv_VEF_base);


//// printOn
//

Sortie& Op_Conv_DI_L2_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Conv_DI_L2_VEF_Face::readOn(Entree& s )
{
  return s ;
}

//
//   Fonctions de la classe Op_Conv_DI_L2_VEF_Face
//

void Op_Conv_DI_L2_VEF_Face::associer_vitesse(const Champ_base& vit)
{
  const Champ_P1NC& inco = ref_cast(Champ_P1NC,vit);
  vitesse_= inco;
}


void flora(DoubleTab A, int& N , DoubleVect B, DoubleVect& U, int& test_flora)
{
  //cette procedure correspond a la methode d'elimination de Gauss
  test_flora = 0; //test pour savoir si la matrice est inversible(0) ou non(1)
  int N1 = N-1;
  int m, m1, i, i1, k, l;
  double quo,SU;
  for(m=0; m<N1; m++)
    {
      m1 = m+1;
      if(fabs(A(m,m)) > 1e-20)
        {
          for(i=m1; i<N; i++)
            {
              quo = A(i,m)/A(m,m);
              for(k=m; k<N; k++)
                A(i,k) =A(i,k)-quo*A(m,k);
              B(i) = B(i)-quo*B(m);
            }
        }
      else
        {
          Cerr<<"Erreur flora: matrice non inversible a l'indice "<<m<<finl;
          test_flora = 1;
        }

    }
  if(fabs(A(N-1,N-1)) >= 1e-20)
    {
      U(N-1) = B(N-1)/A(N-1,N-1);
      for(l=0; l<N1; l++)
        {
          i = N1-l-1;
          SU = 0;
          i1 = i+1;
          for(k=i1; k<N; k++)
            SU = SU+A(i,k)*U(k);
          U(i) = (B(i)-SU)/A(i,i);
        }
    }
  else
    {
      Cerr<<"Erreur flora: matrice non inversible a l'indice"<<N-1<<finl;
      test_flora = 1;
    }
}


void flora_p(DoubleTab& A, int& N, DoubleVect& B, DoubleVect& U, int& test_flora)
{
  //cette procedure correspond a la methode d'elimination de Gauss
  test_flora = 1;//test pour savoir si la matrice est inversible(1) ou non(0)
  int N1 = N-1;
  int m, m1, i, j, i1, k, l;
  int test=0;
  double quo, SU, x, y;
  for(m=0; m<N1; m++)
    {
      m1 = m+1;
      if(fabs(A(m,m))  >= 1.e-10)
        {
          for(i=m1; i<N; i++)
            {
              quo = A(i,m)/A(m,m);
              for(k=m; k<N; k++)
                A(i,k) =A(i,k)-quo*A(m,k);
              B(i) = B(i)-quo*B(m);
            }
        }
      else
        {
          j=m+1;
          while(j<N && test == 0)
            {
              if(fabs(A(m,j)) >= 1.e-10) test =1;
              j++;
            }
          if(j == N)
            {
              Cerr<<"Erreur flora: matrice non inversible a l'indice "<<m<<finl;
              test_flora = 1;
            }
          else         //echange des colonnes m et j
            {
              for(i=0; i<N; i++)
                {
                  x=A(i,m);
                  A(i,m)=A(i,j);
                  A(i,j)=x;
                  y=B(m);
                  B(m)=B(j);
                  B(j)=y;
                }
            }
          test_flora = 0;

        }

    }
  if(fabs(A(N-1,N-1)) >= 1.e-10)
    {
      U(N-1) = B(N-1)/A(N-1,N-1);
      for(l=0; l<N1; l++)
        {
          i = N1-l-1;
          SU = 0;
          i1 = i+1;
          for(k=i1; k<N; k++)
            SU = SU+A(i,k)*U(k);
          U(i) = (B(i)-SU)/A(i,i);
        }
    }
  else
    {
      //Cerr<<"Erreur flora: matrice non inversible a l'indice"<<N-1<<finl;
      test_flora = 0;
    }
}


void qrdcmp(DoubleTab& A, int& N, DoubleVect& C, DoubleVect& D, int& sing)
{
  //construit la decomposition QR de A. Le triangle superieur R est stockee dans le triangle superieur de A,
  //exepte les elements de la diagonale, qui sont stockes dans D. La matrice orthogonale Q est representee
  //comme un produit de N-1 matrice Q(1), ..., Q(N-1) ou Q(j) = Id-(uj*ujt)/cj. La ieme composante de uj est 0
  //pour i=1, ...,j-1 et A(i,j) pour i=j, ..., N. sing retourne 0 si la decomposition est possible et 1 sinon.

  int i, j, k;
  double scale, sigma, sum, tau;
  sing =0;
  for(k=0; k<N-1; k++)
    {
      scale = 0.;
      for(i=k; i<N; i++)
        if(scale < fabs(A(i,k)))                scale = fabs(A(i,k));
      if(fabs(scale)<1.e-15)
        {
          //cas singulier
          Cerr << " huhu " << finl ;
          sing = 1;
          C(k) = 0;
          D(k) = 0;
          return ;
        }
      else
        {
          for(i=k; i<N; i++)        A(i,k) /= scale;
          for(sum=0.0,i=k; i<N; i++)        sum += (A(i,k) * A(i,k));
          if(A(k,k)>0) sigma = sqrt(sum);
          else        sigma = -1*sqrt(sum);
          A(k,k) += sigma;
          C(k) = sigma*A(k,k);
          D(k) = -1*scale*sigma;
          for(j=k+1; j<N; j++)
            {
              for(sum=0.0,i=k; i<N; i++)        sum += A(i,k)*A(i,j);
              tau = sum/C(k);
              for(i=k; i<N; i++)        A(i,j) -= tau*A(i,k);
            }
        }
    }
  D(N-1) = A(N-1,N-1);
  if(fabs(D(N-1)) <1.e-12)
    {
      Cerr << " hoho " << finl ;
      sing =1;
    }
}


void rsolv(DoubleTab& A, int& N, DoubleVect& D, DoubleVect& B)
{
  //resout le systeme Rx=B, ou R est triangulaire superieure stockee dans A et D, provenant de qrdcmp
  //le resultat est stocke dans B.
  int i, j;
  double sum;
  B(N-1) /= D(N-1);
  for(i=N-2; i>=0; i--)
    {
      for(sum=0.0,j=i+1; j<N; j++)        sum += A(i,j)*B(j);
      B(i) = (B(i)-sum)/D(i);
    }
}

void qrsolv( DoubleTab& A, int& N, DoubleVect& B, DoubleVect& X, int& sing,
             int& ncomp, DoubleVect& C, DoubleVect& D)
{
  //resout le systeme lineaire Ax=B
  // DoubleVect C(N), D(N);
  int i, j;
  double sum, tau;
  if(ncomp == 0 ) qrdcmp(A, N, C, D, sing);
  if(sing == 0)
    {
      for(j=0; j<N-1; j++)
        {
          for(sum=0.0,i=j; i<N; i++)        sum += A(i,j)*B(i);
          tau = sum/C(j);
          for(i=j; i<N; i++)        B(i) -= tau*A(i,j);
        }
      rsolv(A, N, D, B);//resout Rx=QtB
      for(i=0; i<N; i++)        X(i) = B(i);
    }
  //else        Cerr<<"erreur"<<finl;
}




//methode du gradient biconjugue
void gradient_biconjugue(DoubleTab A, int n, DoubleVect b, DoubleVect& x, int& sing, int& niter)
{
  if (Process::nproc() > 1)
    {
      Cerr << "OpVEF_DI_L2.cpp: gradient_biconjugue() n'est pas parallele" << finl;
      assert(0);
      Process::exit();
    }

  double seuil ;
  double dnew  = 1. ;

  double dold, alfa, beta ;
  niter = 0 ;
  sing = 0 ;

  DoubleVect r(n) ;
  DoubleVect r_tilda(n);
  DoubleVect p(n) ;
  DoubleVect p_tilda(n);
  DoubleVect q(n) ;
  DoubleVect u(n);
  double r_norme, b_norme = norme_array(b);

  int i,j;

  if(b_norme > 1.e-7 )
    seuil = 1.e-5/b_norme;
  else seuil = 1.e-10;

  r = 0. ;
  p = 0. ;
  q = 0. ;

  for(i=0; i<n; i++)
    p(i) = b(i);

  int nmax = 50 ;

  for(i=0; i<n; i++)
    for(j=0; j<n; j++)
      {
        r(i) = p(i)-A(i,j)*x(j) ;
        r_tilda(i) = r(i) ;
      }

  p = 0 ;
  dold = dotproduct_array(r_tilda, r);
  r_norme = norme_array(r);

  if(sqrt(dold) > seuil)
    {
      while ( ( r_norme > seuil ) && (niter++ < nmax) )
        {
          assert(Process::nproc() == 1); // B.M. code visiblement faux en parallele
          dnew = dotproduct_array(r_tilda, r);

          if(dold == 0.)        niter = nmax ;
          else
            {
              beta = dnew/dold ;
              for(i=0; i<n; i++)
                {
                  p(i) = r(i)+beta*p(i) ;
                  p_tilda(i) = r_tilda(i)+beta*p_tilda(i) ;
                }

              q = 0. ;
              for(i=0; i<n; i++)
                for(j=0; j<n; j++)
                  q(i) += A(i,j)*p(j) ;

              beta = dotproduct_array(p_tilda, q) ;
              //Cerr<<"beta :"<<beta<<finl ;
              if(beta == 0.)        niter = nmax ;
              else
                {
                  alfa = dnew / beta ;

                  x.ajoute_sans_ech_esp_virt(alfa, p);
                  r.ajoute_sans_ech_esp_virt(-alfa, q);

                  q = 0. ;
                  for(i=0; i<n; i++)
                    for(j=0; j<n; j++)
                      q(i) += A(j,i)*p_tilda(j) ;

                  r_tilda.ajoute_sans_ech_esp_virt(-alfa, q);

                  dold = dnew;
                  //assert(dnew >= 0.);
                  r_norme = norme_array(r);
                  //Cerr << " niter " << niter << " dnew " << dnew <<finl ;
                  //Cerr << " r " << r.norme() << finl;
                  //Cerr << " x " << x << finl;
                }
            }
        }
    }
  if ( niter >= nmax) sing = 1 ;
  //Cerr<<"niter :"<<niter<<finl;
}


// convbis correspond au calcul de -1*terme_convection

void convbis(double psc,int num1,int num2,
             const DoubleTab& transporte, int ncomp,
             DoubleTab& resu, DoubleVect& fluent)
{
  int comp,amont;
  double flux;

  if (psc >= 0)
    {
      amont = num1;
      fluent[num2] += psc;
    }
  else
    {
      amont = num2;
      fluent[num1] -= psc;
    }

  if (ncomp == 1)
    {
      flux = transporte(amont)*psc;
      resu(num1) -= flux;
      resu(num2) += flux;
    }
  else
    for (comp=0; comp<ncomp; comp++)
      {
        flux = transporte(amont,comp)*psc;
        resu(num1,comp) -= flux;
        resu(num2,comp) += flux;
      }
}


DoubleTab& Op_Conv_DI_L2_VEF_Face::ajouter(const DoubleTab& transporte,
                                           DoubleTab& resu) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  //  const DoubleVect& volumes_entrelaces = zone_VEF.volumes_entrelaces();
  const Zone& zone = zone_VEF.zone();
  //  const int nb_faces = zone_VEF.nb_faces();
  const int nfa7 = zone_VEF.type_elem().nb_facette();
  //  const int nb_elem = zone_VEF.nb_elem();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();

  /*  const IntTab& face_voisins = zone_VEF.face_voisins();
      int jjj;
      for(jjj = 0;jjj<zone_VEF.nb_faces_tot();jjj++)
      Cerr<<"face_voisins : face = "<<jjj<<" : "<<face_voisins(jjj,0)<<", "<<face_voisins(jjj,1)<<finl;*/


  const DoubleTab& normales_facettes_Cl = zone_Cl_VEF.normales_facettes_Cl();
  //  const DoubleVect& volumes_entrelaces_Cl = zone_Cl_VEF.volumes_entrelaces_Cl();
  DoubleVect& fluent_ = ref_cast(DoubleVect, fluent);

  int nfac = zone.nb_faces_elem();
  int nsom = zone.nb_som_elem();
  int nb_som_facette = zone.type_elem().nb_som_face();

  //  int premiere_face_int = zone_VEF.premiere_face_int();

  // Pour le traitement de la convection on distingue les polyedres
  // standard qui ne "voient" pas les conditions aux limites et les
  // polyedres non standard qui ont au moins une face sur le bord.
  // Un polyedre standard a n facettes sur lesquelles on applique le
  // schema de convection.
  // Pour un polyedre non standard qui porte des conditions aux limites
  // de Dirichlet, une partie des facettes sont portees par les faces.
  // En bref pour un polyedre le traitement de la convection depend
  // du type (triangle, tetraedre ...) et du nombre de faces de Dirichlet.

  double psc;
  int poly,face_adj,fa7,i,j,n_bord;
  int num_face, rang ,itypcl;
  int num10,num20;
  int ncomp_ch_transporte, first;
  if (transporte.nb_dim() == 1)
    ncomp_ch_transporte=1;
  else
    ncomp_ch_transporte= transporte.dimension(1);

  IntVect face(nfac);
  DoubleVect vs(dimension);
  DoubleVect vc(dimension);
  DoubleTab  vsom(nsom,dimension);
  DoubleVect cc(dimension);
  DoubleTab derive(1,1) ;
  int N ,M , sing , cal_amont;
  DoubleVect trans(ncomp_ch_transporte) ;
  if (dimension == 2)
    {
      N = 5 ;
      M = 8 ;
      derive.resize(N,ncomp_ch_transporte) ;
    }
  else // (dimension == 3)
    {
      N = 9 ;
      M = 15 ;
      derive.resize(N,ncomp_ch_transporte) ;
    }
  // On remet a zero le tableau qui sert pour
  // le calcul du pas de temps de stabilite
  fluent_ = 0;

  // Traitement particulier pour les faces de periodicite

  int nb_faces_perio = 0;
  // Boucle pour compter le nombre de faces de periodicite
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          nb_faces_perio += le_bord.nb_faces();
        }
    }

  DoubleTab tab;
  if (ncomp_ch_transporte == 1)
    tab.resize(nb_faces_perio);
  else
    tab.resize(nb_faces_perio,ncomp_ch_transporte);

  // Boucle pour remplir tab
  nb_faces_perio=0;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          //          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              if (ncomp_ch_transporte == 1)
                tab(nb_faces_perio) = resu(num_face);
              else
                for (int comp=0; comp<ncomp_ch_transporte; comp++)
                  tab(nb_faces_perio,comp) = resu(num_face,comp);
              nb_faces_perio++;
            }
        }
    }


  // Les polyedres non standard sont ranges en 2 groupes dans la Zone_VEF:
  //  - polyedres bords et joints
  //  - polyedres bords et non joints
  // On traite les polyedres en suivant l'ordre dans lequel ils figurent
  // dans la zone

  // boucle sur les polys

  int nlim = -1 ;
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();
  for (poly=0; poly<nb_elem_tot; poly++)
    {

      rang = rang_elem_non_std(poly);
      if (rang==-1)
        itypcl=0;
      else
        itypcl=zone_Cl_VEF.type_elem_Cl(rang);

      // calcul des numeros des faces du polyedre
      for (face_adj=0; face_adj<nfac; face_adj++)
        face[face_adj]= elem_faces(poly,face_adj);

      // calcul de la vitesse aux sommets des polyedres
      for (j=0; j<dimension; j++)
        {
          vs[j] = la_vitesse(face[0],j);
          for (i=1; i<nfac; i++)
            vs[j]+= la_vitesse(face[i],j);
        }
      for (i=0; i<nsom; i++)
        for (j=0; j<dimension; j++)
          vsom(i,j) = vs[j] - dimension*la_vitesse(face[i],j);

      // calcul de vc
      zone_VEF.type_elem().calcul_vc(face,vc,vs,vsom,vitesse(),
                                     itypcl,porosite_face);


      /*    Cerr<<"premiere_face_int = "<<premiere_face_int<<", zone_VEF.nb_faces_joint() = "<<zone_VEF.nb_faces_joint()<<finl;
            Cerr<<"premiere_face_std() = "<<zone_VEF.premiere_face_std()<<finl;
            Cerr<<"nb_elem() = "<<zone_VEF.nb_elem()<<", nb_elem_tot() = "<<zone_VEF.nb_elem_tot()<<finl;
            Cerr<<"nb_faces() = "<<zone_VEF.nb_faces()<<", nb_faces_tot() = "<<zone_VEF.nb_faces_tot()<<finl;
            Cerr<<"nb_faces_bord() = "<<zone_VEF.nb_faces_bord()<<", premiere_face_bord() = "<<zone_VEF.premiere_face_bord()<<finl;*/

      cal_amont = 0 ;

      int elem0,elem1,face_adj_glob;
      for (face_adj=0; face_adj<nfac; face_adj++)
        {
          face_adj_glob = face[face_adj];
          elem0 = face_voisins(face_adj_glob,0);
          elem1 = face_voisins(face_adj_glob,1);
          if ((elem0 == -1) || (elem1 == -1))
            cal_amont++ ;
        }

      // calcul polynom de reconstruction
      if(dimension == 2 )
        {
          if ( cal_amont == 0) poly_DI_L2_2d(N,M,derive,poly,ncomp_ch_transporte,transporte,sing);
        }
      else
        {
          if ( cal_amont == 0) poly_DI_L2_3d(N,M,derive,poly,ncomp_ch_transporte,transporte,sing);
        }

      // Boucle sur les facettes du polyedre

      for (fa7=0; fa7<nfa7; fa7++)
        {
          if (rang==-1)
            for (i=0; i<dimension; i++)
              cc[i] = facette_normales(poly,fa7,i);
          else
            for (i=0; i<dimension; i++)
              cc[i] = normales_facettes_Cl(rang,fa7,i);

          // On applique le schema de convection a chaque sommet de la facette

          // reconstruction seulement wenn first = 0

          first = -1 ;

          // On traite le ou les sommets qui sont aussi des sommets du polyedre
          for (i=0; i<nb_som_facette-1; i++)
            {
              first++ ;

              psc =0;
              for (j=0; j<dimension; j++)
                psc+= (vc(j)/double(nb_som_facette-1)+vsom(KEL(i+2,fa7),j))*cc[j];
              psc /= nb_som_facette;

              num10 = face[KEL(0,fa7)];
              num20 = face[KEL(1,fa7)];

              if ( cal_amont > 0)
                {
                  convbis(psc,num10,num20,transporte,ncomp_ch_transporte,resu,fluent_);
                }
              else
                {
                  if(dimension == 2 )
                    {
                      reconst_DI_L2_2d(derive,poly,psc,num10,num20,transporte,ncomp_ch_transporte,resu,fluent_,sing,
                                       nlim );
                    }
                  else
                    {
                      reconst_DI_L2_3d(derive,poly,psc,num10,num20,transporte,ncomp_ch_transporte,resu,fluent_,sing,
                                       first,trans,nlim);
                    }
                }
            }

        }

    } // fin de la boucle

  Cerr << " limitiert in " << nlim << " valeurs " << finl ;

  int voisine;
  nb_faces_perio = 0;
  double diff1,diff2;

  // Dimensionnement du tableau des flux convectifs au bord du domaine
  // de calcul
  DoubleTab& flux_b = ref_cast(DoubleTab,flux_bords_);
  flux_b.resize(zone_VEF.nb_faces_bord(),ncomp_ch_transporte);
  flux_b = 0.;

  // Boucle sur les bords pour traiter les conditions aux limites
  // il y a prise en compte d'un terme de convection pour les
  // conditions aux limites de Neumann_sortie_libre seulement

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {

      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          for (num_face=num1; num_face<num2; num_face++)
            {
              psc =0;
              for (i=0; i<dimension; i++)
                psc += la_vitesse(num_face,i)*face_normales(num_face,i);
              if (psc>0)
                if (ncomp_ch_transporte == 1)
                  {
                    resu(num_face) -= psc*transporte(num_face);
                    flux_b(num_face,0) -= psc*transporte(num_face);
                  }
                else
                  for (i=0; i<ncomp_ch_transporte; i++)
                    {
                      resu(num_face,i) -= psc*transporte(num_face,i);
                      flux_b(num_face,i) -= psc*transporte(num_face,i);
                    }
              else
                {
                  if (ncomp_ch_transporte == 1)
                    {
                      resu(num_face) -= psc*la_sortie_libre.val_ext(num_face-num1);
                      flux_b(num_face,0) -= psc*la_sortie_libre.val_ext(num_face-num1);
                    }
                  else
                    for (i=0; i<ncomp_ch_transporte; i++)
                      {
                        resu(num_face,i) -= psc*la_sortie_libre.val_ext(num_face-num1,i);
                        flux_b(num_face,i) -= psc*la_sortie_libre.val_ext(num_face-num1);
                      }
                  fluent_[num_face] -= psc;
                }
            }
        }
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = le_bord.num_premiere_face();
          int num2 = num1 + le_bord.nb_faces();
          IntVect fait(le_bord.nb_faces());
          fait = 0;
          for (num_face=num1; num_face<num2; num_face++)
            {
              if (fait[num_face-num1] == 0)
                {
                  voisine = la_cl_perio.face_associee(num_face-num1) + num1;

                  if (ncomp_ch_transporte == 1)
                    {
                      diff1 = resu(num_face)-tab(nb_faces_perio);
                      diff2 = resu(voisine)-tab(nb_faces_perio+voisine-num_face);
                      resu(voisine)  += diff1;
                      resu(num_face) += diff2;
                      flux_b(voisine,1) += diff1;
                      flux_b(num_face,0) += diff2;
                    }
                  else
                    for (int comp=0; comp<ncomp_ch_transporte; comp++)
                      {
                        diff1 = resu(num_face,comp)-tab(nb_faces_perio,comp);
                        diff2 = resu(voisine,comp)-tab(nb_faces_perio+voisine-num_face,comp);
                        resu(voisine,comp)  += diff1;
                        resu(num_face,comp) += diff2;
                        flux_b(voisine,comp) += diff1;
                        flux_b(num_face,comp) += diff2;
                      }

                  fait[num_face-num1]= 1;
                  fait[voisine-num1] = 1;
                }
              nb_faces_perio++;
            }
        }
    }
  modifier_flux(*this);
  return resu;

}


void Op_Conv_DI_L2_VEF_Face::reconst_DI_L2_2d(DoubleTab& derive ,int poly,
                                              double psc,int num1,int num2,
                                              const DoubleTab& transporte,
                                              int ncomp,
                                              DoubleTab& resu,
                                              DoubleVect& tab_fluent, int sing, int& nlim) const
{

  //Cerr << " in reconst_DI_L2_2d " << sing << finl ;

  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  //  const IntTab& face_voisins = zone_VEF.face_voisins();

  const Zone& zone = zone_VEF.zone();

  int nfac = zone.nb_faces_elem();

  const DoubleTab& xv = zone_VEF.xv();
  const DoubleTab& xp = zone_VEF.xp();

  double dt=equation().schema_temps().pas_de_temps();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();

  int i, j, face_adj , face_glob, numg=-1 ;
  DoubleVect dist(dimension) ;
  DoubleVect trans_c_g(ncomp) ;
  DoubleVect vs(ncomp) ;
  DoubleVect trans(ncomp) ;
  DoubleTab coor_trans(dimension,dimension) ;

  trans_c_g = 0. ;

  for (j=0; j<ncomp; j++)
    {
      for(face_adj=0; face_adj<nfac; face_adj ++)
        {
          face_glob = elem_faces(poly, face_adj);

          if (ncomp == 1) trans_c_g(0) += transporte(face_glob)/double(nfac) ;
          else trans_c_g(j) += transporte(face_glob,j)/double(nfac) ;

          if ((face_glob != num1) && (face_glob != num2)) numg = face_glob ;

        }
    }
  int num3 = elem_faces(poly, 0 ) ;

  //coor_trans(0,0) = cos(3.14159/4.) ;
  //coor_trans(0,1) = cos(3.14159/4.) ;
  //coor_trans(1,0) = cos(3.14159/2.-3.14159/4.) ;
  //coor_trans(1,1) = cos(3.14159/2.+3.14159/4.) ;
  coor_trans(0,0) = 1. ;
  coor_trans(0,1) = 0. ;
  coor_trans(1,0) = 0. ;
  coor_trans(1,1) = 1. ;

  for (i=0; i<dimension; i++) vs(i)= (la_vitesse(num1,i)+la_vitesse(num2,i)) / 2.;

  dist = 0. ;

  for (i=0; i<dimension; i++)
    for (j=0; j<dimension; j++)
      dist(i) += (2.*xp(poly,j) - xv(numg,j)  -  xv(num3,j) - vs(j)*dt/2. ) * coor_trans(i,j) ;

  double dtrans_max, dtrans_min, dtrans_cen ;

  int amont, aval ;

  if (psc >= 0)
    {
      amont = num1;
      aval  = num2;
    }
  else
    {
      amont = num2;
      aval  = num1;
    }

  for (j=0; j<ncomp; j++)
    {

      if (sing == 0 )
        {
          if(ncomp == 1)
            {
              trans(0) = transporte(num3) + derive(4)*dist(0) * coor_trans(0,0)
                         + derive(3)*dist(1) * coor_trans(1,1)
                         + 1./2.*( derive(2)*dist(0)*dist(0) + derive(1)*dist(1)*dist(1) )
                         + derive(0)*dist(0)*dist(1)    ;

              dtrans_cen =  transporte(amont) + transporte(aval) - trans_c_g(0) ;
              dtrans_max =  std::max( transporte(amont), dtrans_cen ) ;
              dtrans_min =  std::min( transporte(amont), dtrans_cen ) ;
            }
          else
            {
              trans(j) = transporte(num3,j) + derive(4,j)*dist(0) * coor_trans(0,0)
                         + derive(3,j)*dist(1) * coor_trans(1,1)
                         + 1./2.*( derive(2,j)*dist(0)*dist(0) + derive(1,j)*dist(1)*dist(1) )
                         + derive(0,j)*dist(0)*dist(1)    ;


              //dtrans_cen =  transporte(num1,j) + transporte(num2,j) - trans_c_g(j) ;
              //dtrans_cen =  1./2.*(transporte(amont,j) + transporte(aval,j)) ;
              dtrans_cen =  transporte(num1,j) + transporte(num2,j) - trans_c_g(j) ;

              //dtrans_max =  std::max( transporte(amont,j), transporte(aval,j)) ;
              dtrans_max =  std::max( transporte(amont,j) , dtrans_cen) ;
              dtrans_min =  std::min( transporte(amont,j) , dtrans_cen) ;
              //dtrans_min =  std::min( dtrans_min, dtrans_cen) ;

            }

          if (trans(j) > dtrans_max )
            {
              //Cerr << "lim_max "<< finl ;
              nlim++ ;
              trans(j) = dtrans_max ;
            }
          if (trans(j) < dtrans_min )
            {
              //Cerr << "lim_min "<< finl ;
              nlim++ ;
              trans(j) = dtrans_min ;
            }
        }
      else
        {
          if(ncomp == 1) trans(0) = ( transporte(num1) + transporte(num2) ) - trans_c_g(0) ;
          else           trans(j) = ( transporte(num1,j) + transporte(num2,j) ) - trans_c_g(j) ;
          Cerr << " singx != 0 " << finl ;
        }
    }

  //    double kwave = 1. ;
  //    double lo_x = xv(num1,0)+xv(num2,0)-xp(poly,0) ;
  //    double lo_y = xv(num1,1)+xv(num2,1)-xp(poly,1) ;

  //    double exact0= -sin(kwave*lo_x)*cos(kwave*lo_y);
  //    double exact1=  cos(kwave*lo_x)*sin(kwave*lo_y);
  //    double exact0 = kwave*lo_x*lo_x ;
  //    double exact1 = kwave*lo_y*lo_y ;

  //    double moyenne0 = ( transporte(num1,0) + transporte(num2,0) ) - trans_c_g(0)  ;
  //    double moyenne1 = ( transporte(num1,1) + transporte(num2,1) ) - trans_c_g(1)  ;

  //    double xv1 = (xv(num1,0)+xv(num2,0)-xp(poly,0)) - (xv(num3,0) + dist(0) - coor_trans(0) ) ;
  //    double xv2 = (xv(num1,1)+xv(num2,1)-xp(poly,1)) - (xv(num3,1) + dist(1) - coor_trans(1) ) ;

  //      double moyenne0 = transporte(numg,0) ;
  //      double moyenne1 = transporte(numg,1) ;

  //    if (sing == 0 ) Cerr << exact0 << " " << moyenne0 << " " << trans(0)  << " "
  //                         << exact1 << " " << moyenne1 << " " << trans(1) << finl ;

  //    if (sing == 0 ) Cerr << moyenne0 << " " << trans(0)  << " "
  //                         << moyenne1 << " " << trans(1) << finl ;
  //      Cerr << numg <<  " " << num3 << finl ;

  double flux;

  if (psc >= 0)
    {
      tab_fluent[num2] += psc;
    }
  else
    {
      tab_fluent[num1] -= psc;
    }

  if (ncomp == 1)
    {
      flux = trans(0)*psc;
      resu(num1) -= flux;
      resu(num2) += flux;
    }
  else
    {
      for (i=0; i<ncomp; i++)
        {
          flux = trans(i)*psc;
          resu(num1,i) -= flux;
          resu(num2,i) += flux;
        }
    }

}

void Op_Conv_DI_L2_VEF_Face::reconst_DI_L2_3d(DoubleTab& derive, int poly,
                                              double psc,int num1,int num2,
                                              const DoubleTab& transporte,
                                              int ncomp,
                                              DoubleTab& resu,
                                              DoubleVect& tab_fluent ,
                                              int sing, int first, DoubleVect& trans,
                                              int& nlim) const
{

  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  //  const IntTab& face_voisins = zone_VEF.face_voisins();

  //  const int nb_faces = zone_VEF.nb_faces();
  //  const int nfa7 = zone_VEF.type_elem().nb_facette();
  //  const int nb_elem = zone_VEF.nb_elem();
  //  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  //  int premiere_face_std = zone_VEF.premiere_face_std();

  const Zone& zone = zone_VEF.zone();

  int nfac = zone.nb_faces_elem();
  //  int nsom = zone.nb_som_elem();
  //  int nb_som_facette = zone.type_elem().nb_som_face();

  const DoubleTab& xv = zone_VEF.xv();
  const DoubleTab& xp = zone_VEF.xp();


  double dt=equation().schema_temps().pas_de_temps();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();

  double dtrans_max, dtrans_min, dtrans_cen;//, dtrans_aval ;

  int i, j, face_adj , face_glob ;

  DoubleVect dist(dimension) ;
  DoubleTab coor_trans(dimension,dimension) ;

  DoubleVect trans_c_g(ncomp) ;
  double flux;
  IntVect face(nfac) ;

  DoubleVect vs(dimension) ;

  int amont, aval ;

  if (psc >= 0)
    {
      amont = num1;
      aval  = num2;
    }
  else
    {
      amont = num2;
      aval  = num1;
    }

  if(first == 0)
    {
      trans_c_g = 0. ;

      for(face_adj=0; face_adj<nfac; face_adj ++)
        {
          face_glob = elem_faces(poly, face_adj );
          face(face_adj) = face_glob ;
          for (i=0; i<ncomp; i++)
            trans_c_g(i) += transporte(face_glob,i)/double(nfac) ;

        }
      int num3 = elem_faces(poly, 0 ) ;

      coor_trans(0,0) = cos(3.14159/4.) ;
      coor_trans(0,1) = cos(3.14159/4.) ;
      coor_trans(0,2) = cos(3.14159/2.) ;

      coor_trans(1,0) = cos(3.14159/2.-3.14159/4.) ;
      coor_trans(1,1) = cos(3.14159/2.+3.14159/4.) ;
      coor_trans(1,2) = cos(3.14159/2.) ;

      coor_trans(2,0) = cos(3.14159/2.) ;
      coor_trans(2,1) = cos(3.14159/2.) ;
      coor_trans(2,2) = cos(0.) ;

      //coor_trans = 0.;
      //coor_trans(0,0) = 1. ;
      //coor_trans(1,1) = 1. ;
      //coor_trans(2,2) = 1. ;

      vs =0. ;
      for(face_adj=0; face_adj<nfac; face_adj ++)
        {
          face_glob = elem_faces(poly, face_adj );
          face(face_adj) = face_glob ;
          for (i=0; i<dimension; i++)
            vs(i) -= la_vitesse(face_glob,i)/double(nfac) ;

        }
      for (i=0; i<dimension; i++) vs(i) += la_vitesse(num1,i) + la_vitesse(num2,i) ;

      dist = 0. ;

      for(face_adj=0; face_adj<nfac; face_adj ++)
        {
          face_glob = elem_faces(poly, face_adj);
          if ((face_glob == num1) || (face_glob == num2 ))
            for (i=0; i<dimension; i++)
              for (j=0; j<dimension; j++)
                dist(i) += xv(face_glob,j) * coor_trans(i,j) ;
        }

      for(i=0; i<dimension; i++)
        for (j=0; j<dimension; j++)
          dist(i) -= (xp(poly,j) + xv(num3,j) + vs(j)*dt/2.) * coor_trans(i,j) ;

      for (j=0; j<ncomp; j++)
        {
          if (sing == 0 )
            {
              if(ncomp == 1)
                {
                  trans(j) = transporte(num3,j) + derive(8)*dist(0) * coor_trans(0,0)
                             + derive(7)*dist(1) * coor_trans(1,1)
                             + derive(6)*dist(2) * coor_trans(2,2)
                             + 1./2.*( derive(5)*dist(0)*dist(0) + derive(4)*dist(1)*dist(1)
                                       + derive(3)*dist(2)*dist(2) )
                             + derive(2)*dist(0)*dist(1) + derive(1)*dist(0)*dist(2)
                             + derive(0)*dist(1)*dist(2) ;

                  dtrans_cen =  transporte(amont) + transporte(aval) - trans_c_g(0) ;
                  //dtrans_max =  std::max( transporte(amont), transporte(aval) ) ;
                  dtrans_max =  std::max( transporte(amont), dtrans_cen ) ;
                  dtrans_min =  std::min( transporte(amont), dtrans_cen ) ;
                  //dtrans_min =  std::min( dtrans_min, dtrans_cen ) ;
                }
              else
                {
                  trans(j) = transporte(num3,j) + derive(8,j)*dist(0) * coor_trans(0,0)
                             + derive(7,j)*dist(1) * coor_trans(1,1)
                             + derive(6,j)*dist(2) * coor_trans(2,2)
                             + 1./2.*( derive(5,j)*dist(0)*dist(0) + derive(4,j)*dist(1)*dist(1)
                                       + derive(3,j)*dist(2)*dist(2) )
                             +         derive(2,j)*dist(0)*dist(1) + derive(1,j)*dist(0)*dist(2)
                             +         derive(0,j)*dist(1)*dist(2) ;

                  dtrans_cen  =  transporte(amont,j) + transporte(aval,j) - trans_c_g(j) ;
                  dtrans_max =  std::max( transporte(amont,j), dtrans_cen ) ;

                  dtrans_min =  std::min( transporte(amont,j), dtrans_cen ) ;

                }

              if (trans(j) > dtrans_max )
                {
                  //Cerr << " lim max in element " << poly << finl ;
                  trans(j) = dtrans_max ;
                  nlim++ ;
                }
              if (trans(j) < dtrans_min )
                {
                  //Cerr << " lim min in element " << poly << finl ;
                  trans(j) = dtrans_min ;
                  nlim++ ;
                }

            }
          else
            {
              if(ncomp == 1)
                {
                  trans(0) =  transporte(num1) + transporte(num2)  - trans_c_g(0) ;

                }
              else
                {
                  trans(j) =  transporte(num1,j) + transporte(num2,j)  - trans_c_g(j) ;
                  //Cerr << " singx != 0 " << finl ;
                }
            }
        }
      //double lo_x = xv(num1,0)+xv(num2,0)-xp(poly,0) ;
      //double lo_y = xv(num1,1)+xv(num2,1)-xp(poly,1) ;
      //double lo_z = xv(num1,2)+xv(num2,2)-xp(poly,2) ;

      //double exact0= -sin(kwave*lo_x)*cos(kwave*lo_y);
      //double exact1=  cos(kwave*lo_x)*sin(kwave*lo_y);

      //double exact0 = lo_x*lo_x   ;
      //double exact1 = lo_y*lo_y   ;
      //double exact2 = lo_z*lo_z   ;

      //double moyenne0 =  ( transporte(num1,0) + transporte(num2,0) ) - trans_c_g(0) ;
      //double moyenne1 =  ( transporte(num1,1) + transporte(num2,1) ) - trans_c_g(1) ;
      //double moyenne2 =  ( transporte(num1,2) + transporte(num2,2) ) - trans_c_g(2) ;
      //double moyenne0 =   transporte(num1,0) ;
      //double moyenne1 =   transporte(num1,1) ;
      //double moyenne2 =   transporte(num1,2) ;   ;

      //double x0 =  ( xv(num1,0) + xv(num2,0) ) - xp(poly,0) ;
      //double x1 =  ( xv(num1,1) + xv(num2,1) ) - xp(poly,1) ;
      //double x2 =  ( xv(num1,2) + xv(num2,2) ) - xp(poly,2) ;
      //double x0 =   xv(num1,0) ;
      //double x1 =   xv(num1,1) ;
      //double x2 =   xv(num1,2) ;

      //double y0 =   xv(num3,0) + dist(0) ;
      //double y1 =   xv(num3,1) + dist(1) ;
      //double y2 =   xv(num3,2) + dist(2) ;

      //if (sing == 0 ) Cerr << moyenne0 <<  "  " <<trans(0) << "  " << moyenne1 <<
      //                                     "  " << trans(1) << "  " << moyenne2 << "  " << trans(2) <<finl ;
      //Cerr << " exact0" <<  " " << exact0 << "  " << "exact1" <<  " " << exact1
      //     << " exact2" <<  " " << exact2 << finl ;

      // Cerr << x0<<  " " << y0 <<  " "<<x1 <<  " "<< y1<<  " " << x2 <<  " "<< y2 << finl ;
    } // ende first = 0 und normal weiter

  if (psc >= 0)
    {
      tab_fluent[num2] += psc;
    }
  else
    {
      tab_fluent[num1] -= psc;
    }

  if (ncomp == 1)
    {
      flux = trans(0)*psc;
      resu(num1) -= flux;
      resu(num2) += flux;
    }
  else
    {
      for (i=0; i<ncomp; i++)
        {
          flux = trans(i)*psc;
          resu(num1,i) -= flux;
          resu(num2,i) += flux;
        }
    }

}
void Op_Conv_DI_L2_VEF_Face::poly_DI_L2_2d(int N, int M, DoubleTab& derive ,int poly, int ncomp,
                                           const DoubleTab& transporte, int& sing) const
{
  //Cerr << " in           poly_DI_L2_2d " << finl ;
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  const Zone& zone = zone_VEF.zone();

  int nfac = zone.nb_faces_elem();

  const DoubleTab& xv = zone_VEF.xv();
  const DoubleTab& xp = zone_VEF.xp();

  int i, j, face_adj , face_glob , poly1 ;
  DoubleVect dist(dimension) ;
  DoubleTab coor_trans(dimension,dimension) ;
  IntVect face(nfac) ;

  DoubleTab L(M,N) ;
  DoubleTab B(M,ncomp) ;
  DoubleVect dTransp_x(N) ;
  DoubleVect  C(N) ;
  DoubleVect D(N) ;
  DoubleVect SM(N) ;
  double poid ;

  for(face_adj=0; face_adj<nfac; face_adj ++)
    {
      face_glob = elem_faces(poly, face_adj );
      face(face_adj) = face_glob ;
    }

  int num3 = elem_faces(poly, 0);

  //coor_trans(0) =  cos(    (xv(num3,1) - xp(poly,1)) / (xv(num3,0) - xp(poly,0))) ;
  //coor_trans(1) =  cos(1.- (xv(num3,1) - xp(poly,1)) / (xv(num3,0) - xp(poly,0))) ;

  //coor_trans(0,0) = cos(3.14159/4.) ;
  //coor_trans(0,1) = cos(3.14159/4.) ;
  //coor_trans(1,0) = cos(3.14159/2.-3.14159/4.) ;
  //coor_trans(1,1) = cos(3.14159/2.+3.14159/4.) ;
  coor_trans(0,0) = 1. ;
  coor_trans(0,1) = 0. ;
  coor_trans(1,0) = 0. ;
  coor_trans(1,1) = 1. ;
  int row = -1 ;

  for(int face_adj_poly =0; face_adj_poly < nfac; face_adj_poly ++)
    {
      poly1 = face_voisins(face[face_adj_poly],0);
      if (poly1 == poly ) poly1 = face_voisins(face[face_adj_poly], 1);

      for(face_adj=0; face_adj<nfac; face_adj ++)
        {
          face_glob = elem_faces(poly1, face_adj);
          if (face_glob != num3 )
            {
              row++ ;
              dist = 0.;
              for (i=0; i<dimension; i++)
                for (j=0; j<dimension; j++)
                  dist(i) += (xv(face_glob,j) - xv(num3,j)) * coor_trans(i,j) ;

              poid = 0. ;
              for (i=0; i<dimension; i++)
                poid += ( (xv(face_glob,i)-xp(poly,i)) * (xv(face_glob,i)-xp(poly,i)) );
              poid = 1./sqrt(poid) ;

              L(row,4) = poid * dist(0) * coor_trans(0,0) ;
              L(row,3) = poid * dist(1) * coor_trans(1,1) ;
              L(row,2) = poid * 1./2.*dist(0)*dist(0) ;
              L(row,1) = poid * 1./2.*dist(1)*dist(1) ;
              L(row,0) = poid * dist(0)*dist(1) ;

              if (ncomp == 1)
                B(row,0) = poid * (transporte(face_glob)  - transporte(num3) )  ;
              else for (j=0; j<ncomp; j++)
                  B(row,j) = poid * (transporte(face_glob,j) - transporte(num3,j)) ;

            }
        }

    }

  DoubleTab Lij(N,N) ;
  DoubleTab Bij(N,ncomp) ;

  for (j=0; j<N; j++)
    for (i=0; i<ncomp; i++)
      for (int k=0; k<M; k++)       Bij(j,i) += L(k,j) * B(k,i) ;

  for (i=0; i<N; i++)
    for (j=0; j<N; j++)
      for (int k=0; k<M; k++)       Lij(i,j) += L(k,i) * L(k,j) ;


  for (j=0; j<ncomp; j++)
    {
      for (i=0; i<N; i++)   SM(i)=Bij(i,j) ;

      qrsolv(Lij, N, SM, dTransp_x, sing, j, C, D);

      if (sing == 0 )
        {
          if(ncomp == 1)
            {
              for (int val_N= 0; val_N < N ; val_N++ )
                derive(val_N) = dTransp_x(val_N) ;
            }
          else
            {
              for (int val_N= 0; val_N < N ; val_N++ )
                derive(val_N,j) = dTransp_x(val_N) ;
            }
        }
    }
}

void Op_Conv_DI_L2_VEF_Face::poly_DI_L2_3d(int N, int M, DoubleTab& derive ,int poly, int ncomp,
                                           const DoubleTab& transporte ,int& sing) const
{
  //  Cerr<<"Op_Conv_DI_L2_VEF_Face::poly_DI_L2_3d 0 "<<finl;
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  const Zone& zone = zone_VEF.zone();

  int nfac = zone.nb_faces_elem();

  const DoubleTab& xv = zone_VEF.xv();
  const DoubleTab& xp = zone_VEF.xp();

  int i, j, face_adj , face_glob , poly1 ;

  DoubleVect dist(dimension) ;
  DoubleTab coor_trans(dimension,dimension) ;
  IntVect face(nfac) ;

  DoubleTab L(M,N) ;
  DoubleTab B(M,ncomp) ;
  DoubleVect dTransp_x(N) ;
  DoubleVect  C(N) ;
  DoubleVect D(N) ;
  DoubleVect SM(N) ;
  double poid ;
  //  Cerr<<"Op_Conv_DI_L2_VEF_Face::poly_DI_L2_3d 1 "<<finl;

  for(face_adj=0; face_adj<nfac; face_adj ++)
    {
      face_glob = elem_faces(poly, face_adj );
      face(face_adj) = face_glob ;
    }
  //  Cerr<<"Op_Conv_DI_L2_VEF_Face::poly_DI_L2_3d 2 "<<finl;

  int num3 = elem_faces(poly, 0);

  coor_trans(0,0) = cos(3.14159/4.) ;
  coor_trans(0,1) = cos(3.14159/4.) ;
  coor_trans(0,2) = cos(3.14159/2.) ;

  coor_trans(1,0) = cos(3.14159/2.-3.14159/4.) ;
  coor_trans(1,1) = cos(3.14159/2.+3.14159/4.) ;
  coor_trans(1,2) = cos(3.14159/2.) ;

  coor_trans(2,0) = cos(3.14159/2.) ;
  coor_trans(2,1) = cos(3.14159/2.) ;
  coor_trans(2,2) = cos(0.) ;

  //coor_trans = 0.;
  //coor_trans(0,0) = 1. ;
  //coor_trans(1,1) = 1. ;
  //coor_trans(2,2) = 1. ;

  int row = -1 ;
  //  Cerr<<"Op_Conv_DI_L2_VEF_Face::poly_DI_L2_3d 3 "<<finl;

  for(int face_adj_poly =0; face_adj_poly < nfac; face_adj_poly ++)
    {
      poly1 = face_voisins(face[face_adj_poly],0);
      //     Cerr<<"avant poly = "<<poly<<", poly1 = "<<poly1<<", proc = "<<me()<<finl;
      //     Cerr<<"face[face_adj_poly] = "<<face[face_adj_poly]<<finl;
      if (poly1 == poly ) poly1 = face_voisins(face[face_adj_poly], 1);
      //     Cerr<<"apres poly = "<<poly<<", poly1 = "<<poly1<<", proc = "<<me()<<finl;
      for(face_adj=0; face_adj<nfac; face_adj ++)
        {
          face_glob = elem_faces(poly1, face_adj);
          if (face_glob != num3 )
            {
              row++ ;
              dist=0 ;
              for (i=0; i<dimension; i++)
                for (j=0; j<dimension; j++)
                  dist(i) += (xv(face_glob,j) - xv(num3,j)) * coor_trans(i,j) ;

              poid = 0. ;
              for (i=0; i<dimension; i++)
                poid += ( (xv(face_glob,i)-xp(poly,i)) * (xv(face_glob,i)-xp(poly,i)) );
              poid = 1./(poid) ;


              L(row,8) = poid*dist(0) * coor_trans(0,0) ;
              L(row,7) = poid*dist(1) * coor_trans(1,1) ;
              L(row,6) = poid*dist(2) * coor_trans(2,2) ;
              L(row,5) = poid*1./2.*dist(0)*dist(0) ;
              L(row,4) = poid*1./2.*dist(1)*dist(1) ;
              L(row,3) = poid*1./2.*dist(2)*dist(2) ;
              L(row,2) = poid*dist(0)*dist(1) ;
              L(row,1) = poid*dist(0)*dist(2) ;
              L(row,0) = poid*dist(1)*dist(2) ;

              if (ncomp == 1)
                B(row,0) = poid * (transporte(face_glob)  - transporte(num3) )  ;
              else for (j=0; j<ncomp; j++)
                  B(row,j) = poid * (transporte(face_glob,j) - transporte(num3,j)) ;
            }
        }
    }

  //  Cerr<<"Op_Conv_DI_L2_VEF_Face::poly_DI_L2_3d 4 "<<finl;

  DoubleTab Lij(N,N) ;
  DoubleTab Bij(N,ncomp) ;

  for (j=0; j<N; j++)
    for (i=0; i<ncomp; i++)
      for (int k=0; k<M; k++)       Bij(j,i) += L(k,j) * B(k,i) ;

  //  Cerr<<"Op_Conv_DI_L2_VEF_Face::poly_DI_L2_3d 5 "<<finl;

  for (i=0; i<N; i++)
    for (j=0; j<N; j++)
      for (int k=0; k<M; k++)       Lij(i,j) += L(k,i) * L(k,j) ;
  //  Cerr<<"Op_Conv_DI_L2_VEF_Face::poly_DI_L2_3d 6 "<<finl;


  for (j=0; j<ncomp; j++)
    {
      for (i=0; i<N; i++) SM(i)=Bij(i,j) ;

      qrsolv(Lij, N, SM, dTransp_x, sing, j, C, D);

      if (sing == 0 )
        {
          if(ncomp == 1)
            {
              for (int val_N= 0; val_N < N ; val_N++ )
                derive(val_N) = dTransp_x(val_N) ;
            }
          else
            {
              for (int val_N= 0; val_N < N ; val_N++ )
                derive(val_N,j) = dTransp_x(val_N) ;
            }
        }
    }

  //  Cerr<<"Op_Conv_DI_L2_VEF_Face::poly_DI_L2_3d 7 "<<finl;

}

