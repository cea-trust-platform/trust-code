/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Traitement_particulier_NS_THI_VDF.cpp
// Directory:   $TRUST_ROOT/src/VDF/Turbulence
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_THI_VDF.h>
#include <Zone_VDF.h>
#include <Zone_Cl_VDF.h>
#include <Periodique.h>
#include <Champ_Face.h>
#include <Navier_Stokes_std.h>
#include <calcul_spectres.h>
#include <SFichier.h>

Implemente_instanciable(Traitement_particulier_NS_THI_VDF,"Traitement_particulier_NS_THI_VDF",Traitement_particulier_NS_THI);


// Description:
//
// Precondition:
// Parametre: Sortie& is
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
Sortie& Traitement_particulier_NS_THI_VDF::printOn(Sortie& is) const
{
  return is;
}


// Description:
//
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
Entree& Traitement_particulier_NS_THI_VDF::readOn(Entree& is)
{
  return is;
}

void Traitement_particulier_NS_THI_VDF::init_calc_spectre()
{
  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  int nb_som = zone.nb_som();
  calcul_nb_som_dir(zone);
  DoubleTab vit(nb_som,dimension);
  double Ec=0.,D=0;
  double temps_crt = mon_equation->inconnue().temps();

  if (Process::nproc()==1)
    {
      ch_vit_pour_fft(vit);
      calc_sp_nouveau_2(vit,nb_som_dir,temps_crt,Ec,D);
      // Dans Sorties_THI_2, il y a : temps Ecsp : energie cinetique jusqu a kc
      SFichier fic2 ("Sorties_THI_2",ios::app);
      fic2 << temps_crt << " " << Ec << " " << D << finl;
    }
  else
    {
      Cerr << "Traitement_particulier_NS_THI_VDF::init_calc_spectre n'est pas parallelise..." << finl;
    }
}

void Traitement_particulier_NS_THI_VDF::calcul_spectre(void)
{
  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  int nb_som = zone.nb_som();
  DoubleTab vit(nb_som,dimension);
  double temps_crt = mon_equation->inconnue().temps();
  double Eccoup=0.,D=0;

  if (Process::nproc()==1)
    {
      ch_vit_pour_fft(vit);
      calc_sp_nouveau_2(vit,nb_som_dir,temps_crt,Eccoup,D);

      // Dans Sorties_THI_2, il y a : temps Ecsp : energie cinetique jusqu a kc
      SFichier fic2 ("Sorties_THI_2",ios::app);
      fic2 << temps_crt << " " << Eccoup << " " << D << finl;
    }
  else
    {
      Cerr << "Traitement_particulier_NS_THI_VDF::calcul_spectre n'est pas parallelise..." << finl;
    }
}

void Traitement_particulier_NS_THI_VDF::sorties_globales(void)
{
  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  int nb_som = zone.nb_som();
  DoubleTab vit(nb_som,dimension);
  double temps_crt = mon_equation->inconnue().temps();

  // dans ce fichier : il y aura : tps_crt, Ec, D, Skewness
  double skewness=0., Ec=0., D=0.;

  calcul_skewness_ordre_2(skewness);
  calcul_Ec_D(Ec,D);
  if (je_suis_maitre())
    {
      SFichier fic ("Sorties_THI",ios::app);
      fic << temps_crt << " " << Ec << " " << D << " " << skewness << finl;
    }
  Cerr << "temps=" << temps_crt << "  Ec=" << Ec << "  D=" << D << "  skewness=" << skewness  << finl;
}

void Traitement_particulier_NS_THI_VDF::renorm_Ec(void)
{
  double Ec=0,D=0;
  DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  Cerr << "Renormalisation pour les premiers instants de la turb_grille" << finl;
  if (fac_init==0)
    {
      calcul_Ec_D(Ec,D);
    }
  else
    {
      const Zone_dis& zdis = mon_equation->zone_dis();
      const Zone& zone = zdis.zone();
      int nb_som = zone.nb_som();
      DoubleTab vit(nb_som,dimension);
      double temps_crt = mon_equation->inconnue().temps();
      double dD=0;

      ch_vit_pour_fft(vit);
      calc_sp_nouveau_2(vit,nb_som_dir,temps_crt,Ec,dD);
    }
  double nE=pow(Ec/Ec_init,0.5);
  Cerr << "Ec = " << Ec << finl;
  Cerr << "Renormalisation!!" << finl;

  vitesse/=nE;
  //   Ec=vitesse*vitesse;
  //   Ec/=(2.*nb_faces/3.);
  //   Cerr << "Ec = " << Ec << finl;
  return;
}

// Description:
//  Calcul de Ec = 1/2 < u^2 >
//            D  = 1/2 < w^2 > ou w est le rotationnel de u
//  x^2 est le produit scalaire x*x
//  < x > est la moyenne sur le volume
//
//    < x > = INTEGRALE(x_i  d Omega) / INTEGRALE(d Omega)
//            sur Omega                 sur Omega
void Traitement_particulier_NS_THI_VDF::calcul_Ec_D(double& Ec, double& D)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const Zone& zone = zdisbase.zone();
  const int nb_elem=zone.nb_elem();

  const Champ_Face& vit = ref_cast(Champ_Face,mon_equation->inconnue().valeur());
  //  Calcul de l'Energie cinetique et de l enstrophie
  const DoubleVect& vitesse =mon_equation->inconnue().valeurs();
  const int dim = Objet_U::dimension;
  DoubleTab vorticite(nb_elem, dim);
  vit.calculer_rotationnel_ordre2_centre_element(vorticite);

  const IntTab& elem_faces = zone_VDF.elem_faces();
  const DoubleVect& volumes_elem = zone_VDF.volumes();
  const int nb_faces_elem = elem_faces.dimension(1);
  int elem;
  double somme_v2 = 0.;
  double somme_v_elem = 0.;
  double somme_w2 = 0.;
  // Calcul de l'energie cinetique: somme sur les volumes de controle de la pression.
  // Un volume de pression contient deux demi-volumes de controle de vitesse dans
  // chaque direction. On fait donc pour chaque element reel la somme des v^2 des faces.
  // (astuce qui permet de tenir compte naturellement des faces periodiques et
  //  des faces de joint)
  // Somme sur les elements reels
  for (elem = 0; elem < nb_elem; elem++)
    {
      const double volume = volumes_elem[elem];
      somme_v_elem += volume;
      // Calcul de la contribution de l'element a l'integrale de v^2
      {
        int i_face;
        double s = 0.;
        for (i_face = 0; i_face < nb_faces_elem; i_face++)
          {
            const int face = elem_faces(elem, i_face);
            const double v = vitesse(face);
            s += v * v;
          }
        // Chaque face contribue pour la moitie du volume de l'element
        somme_v2 += volume * 0.5 * s;
      }
      // Calcul de la contribution de l'element a l'integrale de w^2
      {
        int i;
        double s = 0.;
        for (i = 0; i < dim; i++)
          {
            const double w_i = vorticite(elem, i);
            s += w_i * w_i;
          }
        somme_w2 += volume * s;
      }
    }
  const double mp_somme_volume = mp_sum(somme_v_elem);
  const double mp_somme_v2 = mp_sum(somme_v2);
  const double mp_somme_w2 = mp_sum(somme_w2);
  if (mp_somme_volume > 0.)
    {
      Ec = mp_somme_v2 * 0.5 / mp_somme_volume;
      D  = mp_somme_w2 * 0.5 / mp_somme_volume;
    }
  else
    {
      Ec = 0.;
      D = 0.;
    }
}

//**********************************************************************//

void Traitement_particulier_NS_THI_VDF::ch_vit_pour_fft(DoubleTab& vit)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, zdisbase);
  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const Zone& zone_geom = zone_vdf.zone();
  const IntVect& orientation = zone_vdf.orientation();
  const IntTab& face_sommets = zone_vdf.face_sommets();
  const int nb_faces_ =  zone_vdf.nb_faces();
  const int nb_som = zone_geom.nb_som();
  double temps_crt = mon_equation->inconnue().temps();
  const char* methode_actuelle="Traitement_particulier_NS_THI_VDF::ch_vit_pour_fft";

  IntTab compteur(nb_som,dimension);
  int num_face,num_som,i,j,k;
  vit = 0;
  compteur = 0;

  vit = -100.0;
  for (num_face=0; num_face<nb_faces_; num_face++)
    {
      j = 0;
      num_som = face_sommets(num_face,j);
      k = orientation(num_face);
      compteur(num_som,k)++;
      vit(num_som,k) = vitesse[num_face];
    }
  i=0;
  j=0;
  k=0;
  IntTab corresp(nb_som_dir+1,nb_som_dir+1,nb_som_dir+1);
  corresp = -1;
  for (num_som = 0; num_som < nb_som; num_som++)
    {
      corresp(i,j,k) = num_som;
      if (i<nb_som_dir)
        i++;
      else
        {
          i=0;
          if (j<nb_som_dir)
            j++;
          else
            {
              j=0;
              if (k<nb_som_dir)
                k++;
            }
        }
    }
  for (j=0; j<nb_som_dir; j++)
    for (i=0; i<=nb_som_dir; i++)
      {
        if (corresp(i,j,nb_som_dir) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(i,j,nb_som_dir),0)=vit(corresp(i,j,0),0);
      }
  for (k=0; k<=nb_som_dir; k++)
    for (i=0; i<=nb_som_dir; i++)
      {
        if (corresp(i,nb_som_dir,k) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(i,nb_som_dir,k),0)=vit(corresp(i,0,k),0);
      }

  for (k=0; k<nb_som_dir; k++)
    for (j=0; j<=nb_som_dir; j++)
      {
        if (corresp(nb_som_dir,j,k) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(nb_som_dir,j,k),1)=vit(corresp(0,j,k),1);
      }
  for (i=0; i<=nb_som_dir; i++)
    for (j=0; j<=nb_som_dir; j++)
      {
        if (corresp(i,j,nb_som_dir) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(i,j,nb_som_dir),1)=vit(corresp(i,j,0),1);
      }

  for (j=0; j<nb_som_dir; j++)
    for (k=0; k<=nb_som_dir; k++)
      {
        if (corresp(nb_som_dir,j,k) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(nb_som_dir,j,k),2)=vit(corresp(0,j,k),2);
      }
  for (i=0; i<=nb_som_dir; i++)
    for (k=0; k<=nb_som_dir; k++)
      {
        if (corresp(i,nb_som_dir,k) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(i,nb_som_dir,k),2)=vit(corresp(i,0,k),2);
      }

  // ALEX
  ////////////////
  // Rajoute pour ecriture du champ des vitesses dans un fichier
  // pour nouveau calcul de spectre (postraitement separe).
  // Ecriture du champ pour un spectre calcule sur la grille a x=cte.
  ////////////////
  Nom fichier_vit_0 = "chp_vit_VDF_0_";
  Nom tps = Nom(temps_crt);
  fichier_vit_0 += tps;
  SFichier fic77 (fichier_vit_0);
  fic77 << nb_som_dir <<finl;
  fic77 << nb_som_dir <<finl;
  fic77 << nb_som_dir <<finl;

  for (i=0; i<nb_som_dir; i++)
    for (j=0; j<nb_som_dir; j++)
      for (k=0; k<nb_som_dir; k++)
        fic77 << vit(corresp(i,j,k),0) <<finl;

  for (i=0; i<nb_som_dir; i++)
    for (j=0; j<nb_som_dir; j++)
      for (k=0; k<nb_som_dir; k++)
        fic77 << vit(corresp(i,j,k),1) <<finl;

  for (i=0; i<nb_som_dir; i++)
    for (j=0; j<nb_som_dir; j++)
      for (k=0; k<nb_som_dir; k++)
        fic77 << vit(corresp(i,j,k),2) <<finl;
  return;
}

void Traitement_particulier_NS_THI_VDF::calcul_skewness_ordre_2(double& Skewness)
{
  // Calcul du skewness : -<(du1/dx1)~3>/(<(du1/dx1)~2>)~(3/2)
  // ce calcul n est valable que pour une periodicite en x
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, zdisbase);

  const Zone_Cl_dis& zcldis = mon_equation->zone_Cl_dis();
  const Zone_Cl_VDF& zone_Cl_VDF = ref_cast(Zone_Cl_VDF,zcldis.valeur());

  const DoubleTab& vitesse = mon_equation->inconnue().valeurs();
  const IntTab& face_voisins = zone_vdf.face_voisins();
  const IntTab& elem_faces = zone_vdf.elem_faces();
  const IntVect& orientation = zone_vdf.orientation();
  const int ndeb_int = zone_vdf.premiere_face_int();

  int num_face,elem,num_face_2,num_face_2_int,compt=0,ori,num_face_int,num_face_asso,elem_2;
  double deriv,Skewness_num=0.,Skewness_den=0.,dist;

  int i_cl=0;
  // i_cl=0 car on veut uniquement u1!!

  const Cond_lim_base& la_cl = zone_Cl_VDF.les_conditions_limites(i_cl).valeur();
  if (sub_type(Periodique,la_cl))
    {
      const Periodique& la_cl_perio = ref_cast(Periodique,la_cl);

      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      for(num_face = ndeb; num_face<nfin; num_face++)
        {
          elem = face_voisins(num_face,1);
          num_face_2_int = elem_faces(elem,dimension);
          elem_2 = face_voisins(num_face_2_int,1);
          num_face_2 = elem_faces(elem_2,dimension);

          num_face_int = num_face;
          deriv = vitesse(num_face_2)-vitesse(num_face_int);
          dist = zone_vdf.dist_face(num_face_int,num_face_2,0);
          if (dist < 0.)
            {
              num_face_asso = la_cl_perio.face_associee(num_face-ndeb)+ndeb;
              dist = zone_vdf.dist_face(num_face_asso,num_face_2,0);
            }
          deriv /=dist;
          Skewness_num += deriv*deriv*deriv;
          Skewness_den += deriv*deriv;
          compt ++;
        }
    }

  num_face = ndeb_int;
  ori = orientation(num_face);

  while(ori == 0)
    {
      // Attention : cela ne va etre vrai qu en maillage regulier (idem seulement o2 en reg)
      elem = face_voisins(num_face,1);
      if (elem != -1)
        {
          num_face_2_int = elem_faces(elem,dimension);
          elem_2 = face_voisins(num_face_2_int,1);
          if (elem_2 != -1)
            {
              num_face_2 = elem_faces(elem_2,dimension);

              deriv = vitesse(num_face_2)-vitesse(num_face);
              deriv /= 2*zone_vdf.dist_face(num_face,num_face_2,0); // on prend 2*delta_x

              Skewness_num += deriv*deriv*deriv;
              Skewness_den += deriv*deriv;
              compt ++;
            }
        }
      num_face ++;
      ori = orientation(num_face);
    }
  compt = mp_sum(compt);
  Skewness_num = mp_sum(Skewness_num);
  Skewness_den = mp_sum(Skewness_den);

  Skewness_num /= compt;
  Skewness_den /= compt;
  Skewness_den = pow(Skewness_den,1.5);
  if (Skewness_den < 1.e-20 )
    {
      Cerr << "ATTENTION : denominateur de Skewness < 1.e-20!!!!!" << finl;
      Skewness = 1.e20;
    }
  else
    Skewness = -Skewness_num/Skewness_den;
  return;
}


void Traitement_particulier_NS_THI_VDF::calcul_spectre_operateur(int nb_op, DoubleTab& u, DoubleTab& u_av, double dt)
{
  const Zone_dis& zdis = mon_equation->zone_dis();
  const Zone& zone = zdis.zone();
  int nb_som = zone.nb_som();
  calcul_nb_som_dir(zone);
  DoubleTab vit(nb_som,dimension);

  double temps_crt = mon_equation->inconnue().temps();

  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_VDF=ref_cast(Zone_VDF, zdisbase);
  const int nb_faces_ = zone_VDF.nb_faces();
  const DoubleVect& volumes_entrelaces = zone_VDF.volumes_entrelaces();

  DoubleTab  u_ap(nb_faces_);
  DoubleVect E_av(nb_som_dir),E_ap(nb_som_dir);

  // il faut d abord diviser par les volumes entrelaces!! pour la diffusion et la convection mais pas pour les autres
  int i,j;
  u *= dt;
  Cerr << "dt=" << dt << finl;
  if ((nb_op == 0)||(nb_op==1))
    for (i=0; i<nb_faces_; i++)
      {
        u_ap[i] = u[i]/volumes_entrelaces[i] +u_av[i];
      }
  else
    for (i=0; i<nb_faces_; i++)
      {
        u_ap[i] = u[i] +u_av[i];
      }

  ch_vit_pour_fft_operateur(u_av,vit);
  calc_sp_nouveau_2_operateur(vit,nb_som_dir,temps_crt,nb_op,dt,E_av);

  ch_vit_pour_fft_operateur(u_ap,vit);
  calc_sp_nouveau_2_operateur(vit,nb_som_dir,temps_crt,nb_op,dt,E_ap);

  Nom fichier = "Transfert_";
  Nom op;
  if (nb_op==0)
    op="diff";
  else if (nb_op==1)
    op="conv";
  else if (nb_op==2)
    op="press";
  else if (nb_op==3)
    op="tot";
  else
    op =Nom(nb_op);


  fichier += op;
  fichier += "_";
  Nom tps = Nom(temps_crt);
  fichier += tps;
  SFichier fic7 (fichier,ios::app);

  for (j=0; j<nb_som_dir; j++)
    if (dabs(E_ap[j]-E_av[j])>1.e-30)
      {
        fic7 << j+1 << "   " << (E_ap[j]-E_av[j])/dt  << finl;
      }
  fic7 << "   " << finl;
  u_av = u_ap;

  return;
}

void Traitement_particulier_NS_THI_VDF::ch_vit_pour_fft_operateur(DoubleTab& u, DoubleTab& vit)
{
  const Zone_dis_base& zdisbase=mon_equation->inconnue().zone_dis_base();
  const Zone_VDF& zone_vdf=ref_cast(Zone_VDF, zdisbase);
  const Zone& zone_geom = zone_vdf.zone();
  const IntVect& orientation = zone_vdf.orientation();
  const IntTab& face_sommets = zone_vdf.face_sommets();
  const int nb_faces_ =  zone_vdf.nb_faces();
  const int nb_som = zone_geom.nb_som();
  const char* methode_actuelle="Traitement_particulier_NS_THI_VDF::ch_vit_pour_fft_operateur";

  IntTab compteur(nb_som,dimension);
  int num_face,num_som,i,j,k;
  vit = 0;
  compteur = 0;

  vit = -100.0;
  for (num_face=0; num_face<nb_faces_; num_face++)
    {
      j = 0;
      num_som = face_sommets(num_face,j);
      k = orientation(num_face);
      compteur(num_som,k)++;
      vit(num_som,k) = u[num_face];
    }
  i=0;
  j=0;
  k=0;
  IntTab corresp(nb_som_dir+1,nb_som_dir+1,nb_som_dir+1);

  for (num_som = 0; num_som < nb_som; num_som++)
    {
      corresp(i,j,k) = num_som;
      if (i<nb_som_dir)
        i++;
      else
        {
          i=0;
          if (j<nb_som_dir)
            j++;
          else
            {
              j=0;
              if (k<nb_som_dir)
                k++;
            }
        }
    }

  for (j=0; j<nb_som_dir; j++)
    for (i=0; i<=nb_som_dir; i++)
      {
        if (corresp(i,j,nb_som_dir) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(i,j,nb_som_dir),0)=vit(corresp(i,j,0),0);
      }
  for (k=0; k<=nb_som_dir; k++)
    for (i=0; i<=nb_som_dir; i++)
      {
        if (corresp(i,nb_som_dir,k) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(i,nb_som_dir,k),0)=vit(corresp(i,0,k),0);
      }

  for (k=0; k<nb_som_dir; k++)
    for (j=0; j<=nb_som_dir; j++)
      {
        if (corresp(nb_som_dir,j,k) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(nb_som_dir,j,k),1)=vit(corresp(0,j,k),1);
      }
  for (i=0; i<=nb_som_dir; i++)
    for (j=0; j<=nb_som_dir; j++)
      {
        if (corresp(i,j,nb_som_dir) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(i,j,nb_som_dir),1)=vit(corresp(i,j,0),1);
      }

  for (j=0; j<nb_som_dir; j++)
    for (k=0; k<=nb_som_dir; k++)
      {
        if (corresp(nb_som_dir,j,k) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(nb_som_dir,j,k),2)=vit(corresp(0,j,k),2);
      }
  for (i=0; i<=nb_som_dir; i++)
    for (k=0; k<=nb_som_dir; k++)
      {
        if (corresp(i,nb_som_dir,k) == -1)
          msg_erreur_maillage(methode_actuelle);
        vit(corresp(i,nb_som_dir,k),2)=vit(corresp(i,0,k),2);
      }
  return ;
}



