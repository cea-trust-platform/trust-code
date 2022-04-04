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
// File:        Echange_contact_Correlation_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF/Cond_Lim
// Version:     /main/30
//
//////////////////////////////////////////////////////////////////////////////

#include <Echange_contact_Correlation_VEF.h>
#include <Champ_front_calc.h>
#include <Probleme_base.h>
#include <Zone_VEF.h>
#include <Milieu_base.h>
#include <Champ_front_fonc.h>
#include <Champ_Uniforme.h>
#include <Domaine.h>
#include <Solv_TDMA.h>
#include <communications.h>
#include <Schema_Comm.h>
#include <EFichier.h>
#include <SFichier.h>

Implemente_instanciable(Echange_contact_Correlation_VEF,"Paroi_Echange_contact_Correlation_VEF",Temperature_imposee_paroi);




Sortie& Echange_contact_Correlation_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

Entree& Echange_contact_Correlation_VEF::readOn(Entree& is )
{
  //  Motcles les_mots(16);
  // Rajout Cyril MALOD (14/09/2006)
  Reprise_temperature=0;
  Motcles les_mots(17);
  {
    les_mots[0] = "dir";
    les_mots[1] = "Tinf";
    les_mots[2] = "Tsup";
    les_mots[3] = "lambda";
    les_mots[4] = "rho";
    les_mots[5] = "Cp";
    les_mots[6] = "dt_impr";
    les_mots[7] = "mu";
    les_mots[8] = "debit";
    les_mots[9] = "Dh";
    les_mots[10] = "N";
    les_mots[11] = "surface";
    les_mots[12] = "Nu";
    les_mots[13] = "xinf";
    les_mots[14] = "xsup";
    // Le mot cle suivant n'est pas encore commente car les hypotheses sur ce rayonnement sont restrictives
    les_mots[15] = "emissivite_pour_rayonnement_entre_deux_plaques_quasi_infinies";
    // Rajout Cyril MALOD (14/09/2006)
    les_mots[16] = "Reprise_correlation";
  }

  dt_impr = 1e10;
  avec_rayo=0;
  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  is >> motlu;
  if(motlu!=accolade_ouverte)
    {
      Cerr << "On attendait une { a la lecture d'une " << que_suis_je() << finl;
      Cerr << "et non : " << motlu << finl;
      exit();
    }
  is >> motlu;
  while (motlu != accolade_fermee)
    {

      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :
          {
            is >> dir;
            if (dir<0 ||dir>2)
              {
                Cerr << "La direction doit etre 0, 1 ou 2 dans  " << que_suis_je() << finl;
                exit();
              }
            break;
          }
        case 1 :
          {
            is >> Tinf;
            break;
          }
        case 2 :
          {
            is >> Tsup;
            break;
          }
        case 3 :
          {
            Nom tmp;
            is >> tmp;
            lambda_T.setNbVar(1);
            lambda_T.setString(tmp);
            lambda_T.addVar("T");
            lambda_T.parseString();
            break;
          }
        case 4 :
          {
            Nom tmp;
            is >> tmp;
            rho_T.setNbVar(1);
            rho_T.setString(tmp);
            rho_T.addVar("T");
            rho_T.parseString();
            break;
          }
        case 5 :
          {
            is >> Cp;
            break;
          }
        case 6 :
          {
            is >> dt_impr;
            break;
          }
        case 7 :
          {
            Nom tmp;
            is >> tmp;
            mu_T.setNbVar(1);
            mu_T.setString(tmp);
            mu_T.addVar("T");
            mu_T.parseString();
            break;
          }
        case 8 :
          {
            is >> debit;
            break;
          }
        case 9 :
          {
            Nom tmp;
            is >> tmp;
            fct_Dh.setNbVar(1);
            fct_Dh.setString(tmp);
            fct_Dh.addVar("x");
            fct_Dh.parseString();
            break;
          }
        case 10 :
          {
            is >> N;
            break;
          }
        case 11 :
          {
            Nom tmp;
            is >> tmp;
            fct_vol.setNbVar(2);
            fct_vol.setString(tmp);
            fct_vol.addVar("Dh");
            fct_vol.addVar("x");
            fct_vol.parseString();
            break;
          }
        case 12 :
          {
            Nom tmp;
            is >> tmp;
            fct_Nu.setNbVar(2);
            fct_Nu.setString(tmp);
            fct_Nu.addVar("Re");
            fct_Nu.addVar("Pr");
            fct_Nu.parseString();
            break;
          }
        case 13 :
          {
            is >> xinf;
            break;
          }
        case 14 :
          {
            is >> xsup;
            break;
          }
        case 15 :
          {
            avec_rayo = 1;
            is >> emissivite;
            break;
          }
          // Rajout Cyril MALOD (14/09/2006)
        case 16 :
          {
            Reprise_temperature=1;
            break;
          }
        default :
          {
            Cerr << "On ne comprend le mot : " << motlu << "dans " << que_suis_je() << finl;
            exit();
          }
        }
      is >> motlu;
    }



  champ_front().typer("Champ_front_fonc");
  champ_front()->fixer_nb_comp(1);
  return is ;
}



/**
 * Calcule le coeff d echange local dans la maille solide.
 */
void Echange_contact_Correlation_VEF::calculer_h_solide(DoubleTab& tab)
{
  // forcement local


  const Equation_base& mon_eqn = zone_Cl_dis().equation();
  const Milieu_base& mon_milieu = mon_eqn.milieu();
  const Zone_VEF& zvef = ref_cast(Zone_VEF,zone_Cl_dis().zone_dis().valeur());
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  const IntTab& face_voisins = zvef.face_voisins();

  int nb_comp = mon_milieu.conductivite()->nb_comp();
  const int nb_faces_bord = ma_front_vf.nb_faces();

  const int ndeb = ma_front_vf.num_premiere_face();
  const int nfin = ndeb + nb_faces_bord;



  if(!sub_type(Champ_Uniforme,mon_milieu.conductivite().valeur()))
    {
      const DoubleTab& tab_lambda = mon_milieu.conductivite().valeurs();

      for (int face=ndeb; face<nfin; face++)
        {
          int elem =  face_voisins(face,0);
          if(elem == -1)
            elem =  face_voisins(face,1);

          for(int i=0; i<nb_comp; i++)
            tab(face-ndeb,i) = pdt_scalSqrt(zvef,face,face,elem,dimension,tab_lambda(elem,i)) ;
        }
    }
  else  // la conductivite est un Champ uniforme
    {
      const DoubleTab& tab_lambda = mon_milieu.conductivite().valeurs();

      for (int face=ndeb; face<nfin; face++)
        {
          int elem =  face_voisins(face,0);
          if(elem == -1)
            elem =  face_voisins(face,1);


          for(int i=0; i<nb_comp; i++)
            {
              tab(face-ndeb,i) = pdt_scalSqrt(zvef,face,face,elem,dimension,tab_lambda(0,i));
            }
        }
    }

}



/**
 * Complete et initialise les attributs de la classe
 */

void Echange_contact_Correlation_VEF::completer()
{

  init();

  T=std::min(Tinf,Tsup);
  calculer_prop_physique();
  if (T(0)==Tinf)
    U=debit/rho(0);
  else U = debit/rho(N-1);

  const Equation_base& mon_eqn = zone_Cl_dis().equation();
  const Milieu_base& mon_milieu = mon_eqn.milieu();
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  const int nb_faces_bord = ma_front_vf.nb_faces();
  int nb_comp = mon_milieu.conductivite()->nb_comp();
  h_solide.resize(nb_faces_bord,nb_comp);



  for (int i=0; i<N; i++)
    {
      h_correlation(i) = calculer_coefficient_echange(i);
    }
  calculer_h_solide(h_solide);




  DoubleTab& Tparoi =champ_front().valeur().valeurs();
  Tparoi.resize(nb_faces_bord,nb_comp);
  const DoubleTab& Ts = mon_eqn.inconnue().valeurs();
  const Zone_VEF& zvef = ref_cast(Zone_VEF,zone_Cl_dis().zone_dis().valeur());
  const IntTab& face_voisins = zvef.face_voisins();
  const int ndeb = ma_front_vf.num_premiere_face();
  for (int ii=0; ii<nb_faces_bord; ii++)
    {
      double Tint=0.;
      int face = ndeb+ii;
      int elem =  face_voisins(face,0);
      if(elem == -1)
        elem =  face_voisins(face,1);


      // on recupere les faces 2, 3 et 4 de l'element contenant "face"
      int face2 = zvef.elem_faces(elem,0);
      int face3 = zvef.elem_faces(elem,1);
      if (face2 == face)
        face2 = zvef.elem_faces(elem,dimension);
      else if (face3 == face)
        face3 = zvef.elem_faces(elem,dimension);

      Tint += pdt_scal(zvef,face,face2,elem,dimension,1.)*Ts(face2) + pdt_scal(zvef,face,face3,elem,dimension,1.)*Ts(face3);

      if(dimension == 3)
        {
          int face4 = zvef.elem_faces(elem,2);
          if (face4 == face)
            face4 = zvef.elem_faces(elem,dimension);
          Tint += pdt_scal(zvef,face,face4,elem,dimension,1.)*Ts(face4);
        }

      Tint *= -1./pdt_scal(zvef,face,face,elem,dimension,1.);


      Tparoi(ii,0) = (h_solide(ii,0)*Tint+ h_correlation(correspondance_solide_fluide(ii))*T(correspondance_solide_fluide(ii)))/(h_solide(ii,0)+h_correlation(correspondance_solide_fluide(ii)));
    }

  init_tab_echange();

  //Mise a jour car pas de dependance a des donnees d un autre probleme
  //dans le cas de probleme couple
  //Methode completer() peut etre a remplacer par methode initialiser()
  const double temps = mon_eqn.schema_temps().temps_courant();
  mettre_a_jour(temps);
}

/**
 * Initialise le tab tab_ech pour le parallele
 */
void Echange_contact_Correlation_VEF::init_tab_echange()
{
}
/**
 * Calcule les CL a appliquer sur l'equation d'energie
 */
void Echange_contact_Correlation_VEF::calculer_CL()
{
  Schema_Comm schema_comm;
  const Joints& joints = zone_Cl_dis().zone().faces_joint();
  const int nb_voisins = joints.size();
  ArrOfInt send_pe_list(nb_voisins);
  int i;
  for (i = 0; i < nb_voisins; i++)
    send_pe_list[i] = joints[i].PEvoisin();
  schema_comm.set_send_recv_pe_list(send_pe_list, send_pe_list);

  const double ymin = coord(1);
  const double ymax = coord(N-2);
  const double Tmin = T(1);
  const double Tmax = T(N-2);

  T_CL0 = Tinf;
  T_CL1 = Tsup;

  schema_comm.begin_comm();
  for (i = 0; i < nb_voisins; i++)
    {
      const int pe = send_pe_list[i];
      schema_comm.send_buffer(pe) << ymin << ymax << Tmin << Tmax;
    }
  schema_comm.echange_taille_et_messages();
  for (i = 0; i < nb_voisins; i++)
    {
      const int pe = send_pe_list[i];
      double ymin_voisin, ymax_voisin, Tmin_voisin, Tmax_voisin;
      schema_comm.recv_buffer(pe) >> ymin_voisin >> ymax_voisin >> Tmin_voisin >> Tmax_voisin;
      if (ymax_voisin < ymin)
        T(0) = T_CL0 = 0.5 * (Tmax_voisin + Tmin);
      else if (ymin_voisin > ymax)
        T(N-1) = T_CL1 = 0.5 * (Tmin_voisin + Tmax);
      else
        {
          Cerr << "Erreur dans Echange_contact_Correlation_VEF::calculer_CL() (decoupage incompatible ?)";
          exit();
        }
    }
  schema_comm.end_comm();
}

/**
 * Calcule rho, mu et lambda du fluide pour la temperature courante
 */
void Echange_contact_Correlation_VEF::calculer_prop_physique()
{
  for (int i=0; i<N; i++)
    {
      rho_T.setVar("T",T(i));
      mu_T.setVar("T",T(i));
      lambda_T.setVar("T",T(i));
      rho(i) = rho_T.eval();
      mu(i) = mu_T.eval();
      lambda(i) = lambda_T.eval();
    }
}

/**
 * Calcule le coeff d echange suivant la correlation entree dans le jdd
 */
double Echange_contact_Correlation_VEF::calculer_coefficient_echange(int i)
{
  double Re,Pr;
  Re = std::fabs(getQh()*getDh(i)/getMu(i));
  Pr = getMu(i)*getCp()/getLambda(i);
  fct_Nu.setVar("Re",Re);
  fct_Nu.setVar("Pr",Pr);
  return fct_Nu.eval()*getLambda(i)/getDh(i);
}


/**
 * Calcul du terme source  de puissance volumique dans l'equation d'energie 1D fluide.
 */
void Echange_contact_Correlation_VEF::calculer_Q()
{
  const Zone_VEF& ma_zvef = ref_cast(Zone_VEF,zone_Cl_dis().zone_dis().valeur());
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  const int ndeb = ma_front_vf.num_premiere_face();
  const int nb_faces_bord = ma_front_vf.nb_faces();
  const IntTab& face_voisins = ma_zvef.face_voisins();

  DoubleTab& Tp= champ_front()->valeurs();


  Qvol=0.;
  for(int iface=0; iface<nb_faces_bord; iface++)
    {
      int face = ndeb+iface;
      int corresp = correspondance_solide_fluide(iface);
      int elem =  face_voisins(face,0);
      if(elem == -1)
        elem =  face_voisins(face,1);
      Qvol(corresp)+=h_correlation(corresp)*(Tp(iface,0)-T(corresp))*surfacesVEF(ma_zvef,face,dimension);
    }
  for (int i=0; i<N; i++)
    Qvol(i)/=vol(i);

}



void Echange_contact_Correlation_VEF::init()
{
  const Zone_VEF& ma_zvef = ref_cast(Zone_VEF,zone_Cl_dis().zone_dis().valeur());
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  const DoubleTab& xv = ma_zvef.xv();
  const IntTab& face_sommets = ma_zvef.face_sommets();
  const DoubleTab& coord_som = ma_zvef.domaine_dis().domaine().coord_sommets();

  const int ndeb = ma_front_vf.num_premiere_face();
  const int nb_faces_bord = ma_front_vf.nb_faces();

  // N entre par l'utilisateur = Nombre de mailles 1D
  N=N/Process::nproc()+2;
  // N modifie pour que cela soit le nombre de noeuds 1D par processeur

  // Ici, on suppose que l'utilisateur a decoupe en tranche selon la direction 1D
  double delta=(xsup-xinf)/Process::nproc();
  xinf=xinf+Process::me()*delta;
  xsup=xinf+delta;
  // on dimensionne
  coord.resize(N);
  U.resize(N);
  T.resize(N);
  Qvol.resize(N);
  rho.resize(N);
  mu.resize(N);
  lambda.resize(N);
  vol.resize(N);
  diam.resize(N);
  h_correlation.resize(N);

  // on remplit les coordonnees des noeuds du maillages 1D dans le fluide
  coord(0)=xinf;
  coord(N-1)=xsup;
  double dx = (xsup-xinf)/(N-2);
  for(int i=1; i<N-1; i++)
    {
      coord(i) = xinf+dx*(i-0.5);
    }

  // on remplit les correspondances elements solides -> elements fluides
  correspondance_solide_fluide.resize(nb_faces_bord);
  for(int i=0; i<nb_faces_bord; i++)
    {
      double min=std::fabs(xv(i+ndeb,dir)-coord(1));
      double tmp=0.;
      int jmin=1;
      for (int j=2; j<N-1; j++)
        {
          if ((tmp=std::fabs(xv(i+ndeb,dir)-coord(j))) < min)
            {
              min = tmp;
              jmin = j;
            }
        }
      //Cerr << "min=" << min << " jmin=" << jmin << finl;
      correspondance_solide_fluide(i) = jmin;
    }
  //Cerr << Process::me() << finl;
  //Cerr << correspondance_solide_fluide << finl;

  if (avec_rayo == 1)
    {
      const double precision = 1.e-6;
      const int MAX_FACES_PATCH = 500; //!!! a revoir
      int nb_patches = 2*(N-2);
      patches_rayo.resize(nb_patches,MAX_FACES_PATCH);
      nb_faces_patch.resize(nb_patches);
      IntVect traite(nb_faces_bord);
      correspondance_fluide_patch.resize(N,2); // en dur, deux patches associes a un point fluide
      correspondance_face_patch.resize(nb_faces_bord); // fournit pour chaque face du bord, le numero du patch auquel elle appartient
      traite=0;
      correspondance_fluide_patch=-1;
      int compteur_patches = 0;
      for(int i=0; i<nb_faces_bord; i++)
        {
          int face_corresp = correspondance_solide_fluide(i);
          if (traite(i) == 0)
            {
              int compteur_face=1;
              traite(i)=1;
              if (compteur_patches==nb_patches)
                {
                  Cerr << "Erreur dans Echange_contact_Correlation_VEF::initialiser" << finl;
                  Cerr << "Le nombre de patches est depasse." << finl;
                  Cerr << "Contacter le support TRUST." << finl;
                  exit();
                }
              patches_rayo(compteur_patches,0) = i;
              correspondance_face_patch(i)=compteur_patches;
              if (correspondance_fluide_patch(face_corresp,0) == -1)
                correspondance_fluide_patch(face_corresp,0) = compteur_patches;
              else
                correspondance_fluide_patch(face_corresp,1) = compteur_patches;

              int s0 = face_sommets(ndeb+i,0);
              int s1 = face_sommets(ndeb+i,1);
              int s2 = face_sommets(ndeb+i,2);
              ArrOfDouble v01(dimension);
              ArrOfDouble v02(dimension);
              for (int idim=0; idim<dimension; idim++)
                {
                  v01[idim] = coord_som(s1,idim)-coord_som(s0,idim);
                  v02[idim] = coord_som(s2,idim)-coord_som(s0,idim);
                }
              ArrOfDouble normale(dimension);
              normale[0] = v01[1]*v02[2]-v01[2]*v02[1];
              normale[1] = -v01[0]*v02[2]+v01[2]*v02[0];
              normale[2] = v01[0]*v02[1]-v01[1]*v02[0];

              for(int j=0; j<nb_faces_bord; j++)
                {
                  if ((traite(j) == 0) && (correspondance_solide_fluide(j) == face_corresp))
                    {
                      int sj_0 = face_sommets(ndeb+j,0);
                      double pscal = 0.;
                      for (int idim=0; idim<dimension; idim++)
                        {
                          pscal += normale[idim]*(coord_som(sj_0,idim)-coord_som(s0,idim));
                        }
                      if (std::fabs(pscal) < precision)
                        {
                          traite(j) = 1;
                          if (compteur_face==MAX_FACES_PATCH)
                            {
                              Cerr << "Erreur dans Echange_contact_Correlation_VEF::initialiser" << finl;
                              Cerr << "Le nombre maximal de faces de patches est depasse." << finl;
                              Cerr << "Contacter le support TRUST." << finl;
                              exit();
                            }
                          patches_rayo(compteur_patches,compteur_face++) = j;
                          correspondance_face_patch(j)=compteur_patches;
                        }
                    }
                }
              nb_faces_patch(compteur_patches) = compteur_face;
              compteur_patches++;
            }
        }


    }
  // calcul du diametre hydraulique
  for (int i=0; i<N; i++)
    {
      fct_Dh.setVar("x",coord(i));
      diam(i)=fct_Dh.eval();
    }
  // calcul des volumes des tranches
  vol=0.;
  const double dz = (coord(2)-coord(1)); // pour l'instant dz est constant, le maillage fluide est regulier (hormis pour le premier et dernier point)
  for (int i=1; i<N-1; i++)
    {
      fct_vol.setVar("Dh",getDh(i));
      fct_vol.setVar("x",coord(i));
      vol(i)=fct_vol.eval()*dz;
    }
  vol(0) = vol(N-1) = 1.; // ne sert pas mais evite une division par zero dans le calcul de Qvol.
}


/**
 * Calcule la vitesse par conservation de la masse
 */
void Echange_contact_Correlation_VEF::calculer_Vitesse()
{
  for (int i=0; i<N; i++)
    U(i) = debit/rho(i);
}


/**
 * Calcule la temperature 1D dans le fluide en resolvant la conservation de l'energie
 */
void Echange_contact_Correlation_VEF::calculer_Tfluide()
{
  const Equation_base& mon_eqn = zone_Cl_dis().equation();
  const double dt = mon_eqn.schema_temps().pas_de_temps();

  DoubleVect ma(N); // diagonale
  DoubleVect mb(N-1); // sous-diagonale
  DoubleVect mc(N-1); // sur-diagonale
  DoubleVect sm(N);
  const int sgn = (debit>0) ? 1 : -1; // schema amont pour le transport


  ma(0) = 1.;
  ma(N-1) = 1.;
  sm(0) = T_CL0;
  sm(N-1) = T_CL1;
  mc(0) = 0.;
  mb(N-2) = 0.;
  int i;
  for (i=1; i<N-1; i++)
    {
      const double dtrhoCp = dt/rho(i)/Cp;
      const double dz1 = coord(i+1)-coord(i);
      const double dz2 = coord(i)-coord(i-1);
      const double l1 = 0.5*(lambda(i+1)+lambda(i));
      const double l2 = 0.5*(lambda(i)+lambda(i-1));

      ma(i)=1+dtrhoCp*(sgn*Cp*debit/dz1+2*(l1/dz1+l2/dz2)/(dz1+dz2));

      sm(i) = T(i)+dtrhoCp*Qvol(i);

      mc(i) = dtrhoCp*(0.5*(1-sgn)*Cp*debit/dz1-2*l1/dz1/(dz1+dz2));

      mb(i-1) = dtrhoCp*(-2*l2/dz2/(dz1+dz2));
      mb(i-1) += dtrhoCp*(-0.5*(1+sgn)*Cp*debit/dz1);
    }




  DoubleVect xx(N);
  xx=0.;
  Solv_TDMA::resoudre(ma,mb,mc,sm,xx,N);
  for (i=0; i<N; i++)
    T(i) = xx(i);
}

void Echange_contact_Correlation_VEF_sauvegarder(const Nom& filename, const double temps, const DoubleVect& T)
{
  const int canal = 56;
  const int nproc = Process::nproc();
  if (Process::je_suis_maitre())
    {
      Process::Journal() << "Echange_contact_Correlation_VEF::mettre_a_jour: sauvegarde dans le fichier "
                         << filename << finl;
      SFichier file(filename);
      file << "Temps = " << temps << finl;
      DoubleVect tmp(T); // copie de T
      int count = 0;
      for (int pe = 0; pe < nproc; pe++)
        {
          recevoir(tmp, pe, canal); // ne fait rien si pe==0
          const int sz = tmp.size();
          for (int i = 0; i < sz; i++)
            {
              file << "T(" << count << ")\t = " << tmp[i] << finl;
              count++;
            }
        }
    }
  else
    {
      envoyer(T, 0, canal);
    }
}

void Echange_contact_Correlation_VEF_reprendre(const Nom& filename, const double temps, DoubleVect& T)
{
  const int canal = 56;
  const int nproc = Process::nproc();
  if (Process::je_suis_maitre())
    {
      Cerr << "Echange_contact_Correlation_VEF::mettre_a_jour: reprise dans le fichier "
           << filename << finl;
      EFichier file(filename);
      file.set_error_action(Entree::ERROR_EXIT); // Exit en cas d'erreur de lecture
      file.set_check_types(1); // Verifie que c'est bien des double
      Nom bidon1, bidon2, temps_lu;
      // Le format attendu en entete est "Temps = valeur"
      file >> bidon1 >> bidon2 >> temps_lu;
      if (bidon1 != "Temps" || bidon2 != "=" || temps_lu != temps)
        {
          Cerr << "Erreur dans Echange_contact_Correlation_VEF::mettre_a_jour() :\n"
               << " On attendait l'entet suivante dans le fichier " << filename
               << "\n Temps = " << temps << finl;
          if (temps_lu != temps)
            {
              Cerr << "Le temps indique dans le fichierest different du temps de reprise du calcul " << temps
                   << "Vous ne pouvez pas faire de reprise pour la correlation.\n"
                   << "Supprimez dans la correlation le mot clef \"Reprise\".\n"
                   << "La phase transitoire du calcul sera fausse, mais l'etat stationnaire sera juste." << finl;
            }
          Process::exit();
        }
      DoubleVect tmp(T); // copie de T
      double valeur;
      for (int pe = 0; pe < nproc; pe++)
        {
          recevoir(tmp, pe, canal); // ne fait rien si pe==0
          const int sz = tmp.size();
          for (int i = 0; i < sz; i++)
            {
              // Le format attendu est "T(i) = valeur"
              file >> bidon1 >> bidon2 >> valeur;
              tmp[i] = valeur;
            }
          envoyer(tmp, pe, canal); // ne fait rien si pe==0
          if (pe == 0)
            T = tmp;
        }
    }
  else
    {
      envoyer(T, 0, canal); // j'envoie un tableau de la bonne taille au proc 0
      recevoir(T, 0, canal);// je recois le tableau rempli
    }
}

/**
 * Mise a jour de la vitesse, temperature fluide et coeff d'echange
 */
void Echange_contact_Correlation_VEF::mettre_a_jour(double temps)
{

  // Nom des fichiers de sauvegarde
  Nom Fichier_sauv_nom = zone_Cl_dis().equation().probleme().le_nom();
  Fichier_sauv_nom+="_";
  Fichier_sauv_nom+=frontiere_dis().frontiere().le_nom();
  Fichier_sauv_nom+=".sauv";

  // Operation de reprise du champ de temperature dans le fluide
  if (Reprise_temperature==1)
    {
      Echange_contact_Correlation_VEF_reprendre(Fichier_sauv_nom, temps, T);
      Reprise_temperature=0;
    }

  calculer_CL();
  calculer_prop_physique();
  calculer_Q();
  calculer_Vitesse();
  calculer_Tfluide();



  for (int i=0; i<N; i++)
    h_correlation(i) = calculer_coefficient_echange(i);

  calculer_h_solide(h_solide);

  const int taille = h_solide.dimension(0);
  DoubleTab& Tparoi = champ_front().valeur().valeurs();
  const Equation_base& mon_eqn = zone_Cl_dis().equation();
  const DoubleTab& Ts = mon_eqn.inconnue().valeurs();
  Zone_VEF& zvef = ref_cast(Zone_VEF,zone_Cl_dis().zone_dis().valeur());
  const Front_VF& ma_front_vf = ref_cast(Front_VF,frontiere_dis());
  const IntTab& face_voisins = zvef.face_voisins();
  const int ndeb = ma_front_vf.num_premiere_face();
  const double sigma = 5.67e-8;
  const DoubleVect& face_surfaces = zvef.face_surfaces();
  int nb_patches=2*(N-2);
  flux_radiatif.resize(nb_patches);
  flux_radiatif=0;
  IntVect patch_calcule(nb_patches);
  patch_calcule=0;
  for (int ii=0; ii<taille; ii++)
    {
      double Tint=0.;
      int face = ndeb+ii;
      int elem =  face_voisins(face,0);
      if(elem == -1)
        elem =  face_voisins(face,1);


      // on recupere les faces 2, 3 et 4 de l'element contenant "face"
      int face2 = zvef.elem_faces(elem,0);
      int face3 = zvef.elem_faces(elem,1);
      if (face2 == face)
        face2 = zvef.elem_faces(elem,dimension);
      else if (face3 == face)
        face3 = zvef.elem_faces(elem,dimension);

      Tint += pdt_scal(zvef,face,face2,elem,dimension,1.)*Ts(face2) + pdt_scal(zvef,face,face3,elem,dimension,1.)*Ts(face3);

      if(dimension == 3)
        {
          int face4 = zvef.elem_faces(elem,2);
          if (face4 == face)
            face4 = zvef.elem_faces(elem,dimension);
          Tint += pdt_scal(zvef,face,face4,elem,dimension,1.)*Ts(face4);
        }

      Tint *= -1./pdt_scal(zvef,face,face,elem,dimension,1.);

      int patch_courant = 0;
      if (avec_rayo == 1)
        {
          patch_courant = correspondance_face_patch(ii);
          if (!patch_calcule(patch_courant))
            {
              int patch_en_face = correspondance_fluide_patch(correspondance_solide_fluide(ii),0);
              if (patch_en_face == patch_courant)
                patch_en_face = correspondance_fluide_patch(correspondance_solide_fluide(ii),1);

              double Tp14 = 0.;
              double surf1_tot = 0.;
              for (int face_patch=0; face_patch<nb_faces_patch(patch_courant); face_patch++)
                {
                  int f = ndeb+patches_rayo(patch_courant,face_patch);
                  double surf = face_surfaces(f);
                  Tp14 += surf*pow(Ts(f),4);
                  surf1_tot += surf;
                }
              Tp14 /= surf1_tot;
              double Tp24 = 0.;
              double surf2_tot = 0.;
              for (int face_patch=0; face_patch<nb_faces_patch(patch_en_face); face_patch++)
                {
                  int f = ndeb+patches_rayo(patch_en_face,face_patch);
                  double surf = face_surfaces(f);
                  Tp24 += surf*pow(Ts(f),4);
                  surf2_tot += surf;
                }
              Tp24 /= surf2_tot;

              flux_radiatif(patch_courant) = sigma*emissivite/(2-emissivite)*(Tp24-Tp14);
              patch_calcule(patch_courant) = 1;
              if (limpr(temps,mon_eqn.schema_temps().pas_de_temps()))
                {
                  //Cout << ii << " " << patch_courant << finl;
                  if (Process::nproc()>1) Cout << "Processeur " << Process::me() << " ";
                  Cout << ma_front_vf.le_nom() << " patch " << patch_courant;
                  Cout << " : flux radiatif=sigma*eps/(2-eps)*(TP2^4-TP1^4)= " << flux_radiatif(patch_courant) << " W/m2";
                  Cout << " TP2= " << pow(Tp24,0.25) << " K";
                  Cout << " TP1= " << pow(Tp14,0.25) << " K" << finl;
                }
            }
        }
      Tparoi(ii,0) = (h_solide(ii,0)*Tint+flux_radiatif(patch_courant)+ h_correlation(correspondance_solide_fluide(ii))*T(correspondance_solide_fluide(ii)))/(h_solide(ii,0)+h_correlation(correspondance_solide_fluide(ii)));
    }
  // Mise a jour des CLs pour avoir une impression correcte des resultats
  calculer_CL();
  if (limpr(temps,mon_eqn.schema_temps().pas_de_temps())) imprimer(temps);

  Temperature_imposee_paroi::mettre_a_jour(temps);

  Echange_contact_Correlation_VEF_sauvegarder(Fichier_sauv_nom, temps, T);
}

int Echange_contact_Correlation_VEF::compatible_avec_eqn(const Equation_base& eqn) const
{
  if ( sub_type(Conduction, eqn) )
    return 1;
  else
    {
      err_pas_compatible(eqn);
      return 0;
    }
}


/**
 * Teste si l'impression est demandee
 */
int Echange_contact_Correlation_VEF::limpr(double temps_courant,double dt) const
{
  const Schema_Temps_base& sch = zone_Cl_dis().equation().schema_temps();
  // Test un peu tordu, mais ca fonctionne !!!! (CM 05/07/2007)
  if (dt_impr<=dt || ((sch.temps_max()<=temps_courant || sch.nb_pas_dt_max()<=(sch.nb_pas_dt()+1) || (temps_courant!=sch.temps_courant() && sch.nb_pas_dt()==0)) && dt_impr!=1e10))
    return 1;
  else
    {
      // Voir Schema_Temps_base::limpr pour information sur epsilon et modf
      static const double epsilon = 1.e-9;
      double i, j;
      modf(temps_courant/dt_impr + epsilon, &i);
      modf((temps_courant-dt)/dt_impr + epsilon, &j);
      return ( i>j );
    }
}


/**
 * Imprime les resultats
 */
void Echange_contact_Correlation_VEF::imprimer(double temps) const
{
  const int ME = Process::me();
  const int nbproc = Process::nproc();

  if (nbproc>1)
    {
      envoyer(coord,ME,0,ME);
      envoyer(T,ME,0,ME);
      envoyer(U,ME,0,ME);
      envoyer(h_correlation,ME,0,ME);
      envoyer(rho,ME,0,ME);
      envoyer(mu,ME,0,ME);
      envoyer(lambda,ME,0,ME);
      envoyer(Qvol,ME,0,ME);
      envoyer(vol,ME,0,ME);
      envoyer(flux_radiatif,ME,0,ME);

      if (je_suis_maitre())
        {
          Nom nom_bord=frontiere_dis().frontiere().le_nom();
          nom_bord+="_";
          nom_bord+=Nom(temps);
          nom_bord+=".dat";
          SFichier fic(nom_bord);
          fic.precision(zone_Cl_dis().equation().schema_temps().precision_impr());
          fic.setf(ios::scientific);
          double Qt=0.;
          fic << "# X                T                U                h                rho                mu                lambda                Q[W]";
          if (avec_rayo) fic << "                Flux_radiatif(W/m2)";
          fic << finl;

          for (int p=0; p<nbproc; p++)
            {
              DoubleVect coord_tmp;
              DoubleVect T_tmp;
              DoubleVect U_tmp;
              DoubleVect h_tmp;
              DoubleVect rho_tmp;
              DoubleVect mu_tmp;
              DoubleVect lambda_tmp;
              DoubleVect Qvol_tmp;
              DoubleVect vol_tmp;
              DoubleVect flux_radiatif_tmp;

              recevoir(coord_tmp,p,0,p);
              recevoir(T_tmp,p,0,p);
              recevoir(U_tmp,p,0,p);
              recevoir(h_tmp,p,0,p);
              recevoir(rho_tmp,p,0,p);
              recevoir(mu_tmp,p,0,p);
              recevoir(lambda_tmp,p,0,p);
              recevoir(Qvol_tmp,p,0,p);
              recevoir(vol_tmp,p,0,p);
              recevoir(flux_radiatif_tmp,p,0,p);

              for (int i=0; i<coord_tmp.size(); i++)
                {
                  fic << coord_tmp(i) << " \t" << T_tmp(i) << " \t" << U_tmp(i) << " \t" << h_tmp(i) << " \t" ;
                  fic  << rho_tmp(i) << " \t" << mu_tmp(i) << " \t" << lambda_tmp(i) << " \t" << Qvol_tmp(i)*vol_tmp(i);
                  if (avec_rayo && i>0 && i<coord_tmp.size()-1) fic << " \t" << flux_radiatif_tmp(i-1);
                  fic << finl;
                  Qt+=Qvol_tmp(i)*vol_tmp(i);
                }

            }
          fic << "# Q total[W] = " << Qt << finl;
          fic << finl;
        }
    }
  else
    {
      Nom nom_bord=frontiere_dis().frontiere().le_nom();
      nom_bord+="_";
      nom_bord+=Nom(temps);
      nom_bord+=".dat";
      SFichier fic(nom_bord);
      fic.precision(zone_Cl_dis().equation().schema_temps().precision_impr());
      fic.setf(ios::scientific);
      double Qt=0.;
      fic << "# X                 T                 U                 h                 rho                 mu                 lambda                  Q[W]";
      if (avec_rayo) fic << "                Flux_radiatif(W/m2)";
      fic << finl;

      for (int i =0; i<N; i++)
        {
          fic << coord(i) << " \t" << T(i) << " \t" << U(i) << " \t" << h_correlation(i) << " \t" << rho(i) << " \t" ;
          fic <<    mu(i) << " \t" << lambda(i) << " \t" << Qvol(i)*vol(i);
          if (avec_rayo && i>0 && i<N-1) fic << " \t" << flux_radiatif(i-1);
          fic << finl;
          Qt+=Qvol(i)*vol(i);
        }
      fic << "# Q total[W] = " << Qt << finl;
      fic << finl;

    }

}

// Attention : la normale a la face num_face est suppose sortante
// tandis que celle a la face num2 est reorientee dans la methode
// afin d etre sortante.
double Echange_contact_Correlation_VEF::pdt_scalSqrt(const Zone_VEF& la_zone,int num_face,int num2,
                                                     int num_elem,int dim,double diffu)
{
  double pscal;

  pscal = la_zone.face_normales(num_face,0)*la_zone.face_normales(num2,0)
          + la_zone.face_normales(num_face,1)*la_zone.face_normales(num2,1);
  if (dim == 3)
    pscal += la_zone.face_normales(num_face,2)*la_zone.face_normales(num2,2);
  pscal = sqrt(std::fabs(pscal));

  return (pscal*diffu)/la_zone.volumes(num_elem);
}

// Attention : la normal a la face num_face est suppose sortante
// tandis que celle a la face num2 est reorientee dans la methode
// afin d etre sortante.
double Echange_contact_Correlation_VEF::pdt_scal(const Zone_VEF& la_zone,int num_face,int num2,
                                                 int num_elem,int dim,double diffu)
{
  double pscal;
  const IntTab& face_voisinsF = la_zone.face_voisins();

  pscal = la_zone.face_normales(num_face,0)*la_zone.face_normales(num2,0)
          + la_zone.face_normales(num_face,1)*la_zone.face_normales(num2,1);
  if (dim == 3)
    pscal += la_zone.face_normales(num_face,2)*la_zone.face_normales(num2,2);

  if ( (face_voisinsF(num_face,0) == face_voisinsF(num2,0)) ||
       (face_voisinsF(num_face,1) == face_voisinsF(num2,1))) pscal = -pscal;

  return (pscal*diffu)/la_zone.volumes(num_elem);
}
double Echange_contact_Correlation_VEF::surfacesVEF(const Zone_VEF& la_zone,int num_face,int dim)
{
  double pscal;

  pscal = la_zone.face_normales(num_face,0)*la_zone.face_normales(num_face,0)
          + la_zone.face_normales(num_face,1)*la_zone.face_normales(num_face,1);
  if (dim == 3)
    pscal += la_zone.face_normales(num_face,2)*la_zone.face_normales(num_face,2);
  pscal = sqrt(pscal);

  return pscal;
}
