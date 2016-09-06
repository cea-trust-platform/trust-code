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
// File:        Op_Diff_VEFP1NCP1B_Face.cpp
// Directory:   $TRUST_ROOT/src/P1NCP1B/Operateurs
// Version:     /main/26
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Diff_VEFP1NCP1B_Face.h>
#include <Champ_P1NC.h>
#include <Champ_Don.h>
#include <Les_Cl.h>
#include <Champ_Uniforme.h>
#include <Domaine.h>
#include <Ref_Champ_P1NC.h>
#include <Debog.h>
#include <DoubleLists.h>
#include <Champ_front_txyz.h>
#include <Champ_Don_lu.h>
#include <Champ_Don_Fonc_xyz.h>
#include <Champ_Uniforme_Morceaux.h>
#include <Schema_Temps_base.h>
#include <Porosites_champ.h>
#include <Check_espace_virtuel.h>
#include <fstream>
using std::ofstream;

Implemente_instanciable_sans_constructeur(Op_Diff_VEFP1NCP1B_Face,"Op_Diff_VEFP1NCP1B_const_P1NC",Op_Diff_VEF_Face);


static inline double maximum(const double& x,
                             const double& y)
{
  if(x<y)
    return y;
  return x;
}
/*
static inline double maximum(const double& x,
                             const double& y,
                             const double& z)
{
  return maximum(maximum(x,y),z);
}
*/
Op_Diff_VEFP1NCP1B_Face::Op_Diff_VEFP1NCP1B_Face()
{
  //Initialisation des attributs
  convexite_=1.e-3;
  alphaE=1;
  alphaS=1;
  alphaA=0;
  test_=0;
  coeff_=1.;//alphaE+alphaS;
  decentrage_=1;
  is_laplacian_filled_=0;
}

Sortie& Op_Diff_VEFP1NCP1B_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//// readOn
//

Entree& Op_Diff_VEFP1NCP1B_Face::readOn(Entree& s )
{
  //Les mots a reconnaitre
  Motcle motlu, accouverte = "{" , accfermee = "}" ;
  Motcles les_mots(6);
  {
    les_mots[0] = "alphaE";
    les_mots[1] = "alphaS";
    les_mots[2] = "alphaA";
    les_mots[3] = "test";
    les_mots[4] = "decentrage";
    les_mots[5] = "epsilon";
  }

  //Verification de la syntaxe
  s >> motlu;
  if (motlu!=accouverte)
    if (Process::je_suis_maitre())
      {
        Cerr << "Erreur Op_Diff_VEFP1NCP1B_Face::readOn()" << finl;
        Cerr << "Depuis la 1.5.5, la syntaxe du mot cle P1NCP1B a change."
             << finl;
        Cerr << "Il faut commencer par une accolade ouvrante {" << finl;
        Cerr << "et les options eventuelles sont entre les accolades :"
             << finl;
        Cerr << "Diffusion { P1NCP1B } -> Diffusion { P1NCB { } }" << finl;
        exit();
      }


  //Lecture des parametres
  s >> motlu;
  while(motlu!=accfermee)
    {
      int rang = les_mots.search(motlu);

      switch(rang)
        {
        case 0 :

          s >> alphaE;
          break;

        case 1 :

          s >> alphaS;
          break;

        case 2 :

          if (Objet_U::dimension==3)
            s >> alphaA;
          else if (Process::je_suis_maitre())
            {
              Cerr << "Erreur Op_Diff_VEFP1NCP1B_Face::readOn()" << finl;
              Cerr << "L'option alphaA ne peut etre activee qu'en "
                   << "dimension 3" << finl;
              Cerr << "Sortie du programme" << finl;
              exit();
            }
          break;

        case 3 :

          test_=1;
          break;

        case 4 :

          s >> decentrage_;
          break;

        case 5 :

          s >> convexite_;
          break;

        default :

          if (Process::je_suis_maitre())
            {
              Cerr << "Erreur Op_Diff_VEFP1NCP1B_Face::readOn()" << finl;
              Cerr << "Mot clef " << motlu << " non reconnu" << finl;
              Cerr << "Les mots clef reconnus sont : " << les_mots << finl;
              Cerr << "Sortie du programme" << finl;
              exit();
            }
          break;
        }//fin du switch

      //Suite de la lecture
      s >> motlu;
    }//fin du while

  if (alphaE && !alphaS) convexite_=1.;
  else if (alphaS && !alphaE) convexite_=0.;

  coeff_=1.;//alphaE+alphaS;

  return s;
}

//// associer
//





void Op_Diff_VEFP1NCP1B_Face::associer(const Zone_dis& zone_dis,
                                       const Zone_Cl_dis& zone_cl_dis,
                                       const Champ_Inc& ch_diffuse)
{
  const Zone_VEF_PreP1b& zvef = ref_cast(Zone_VEF_PreP1b,zone_dis.valeur());
  const Zone_Cl_VEFP1B& zclvef = ref_cast(Zone_Cl_VEFP1B,zone_cl_dis.valeur());

  if (sub_type(Champ_P1NC,ch_diffuse.valeur()))
    {
      const Champ_P1NC& inco = ref_cast(Champ_P1NC,ch_diffuse.valeur());
      inconnue_ = inco;
    }

  la_zone_vef = zvef;
  la_zcl_vef = zclvef;
}

void Op_Diff_VEFP1NCP1B_Face::completer()
{
  Op_Diff_VEF_Face::completer();

  initialiser();
}

//ATTENTION : NE TIENT PAS COMPTE DE LA POROSITE 09/04/2009
double Op_Diff_VEFP1NCP1B_Face::calculer_dt_stab() const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();
  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  const int nb_faces=zone_VEF.nb_faces();
  const int nb_faces_tot=zone_VEF.nb_faces_tot();

  const DoubleVect& volumes_entrelaces=zone_VEF.volumes_entrelaces();
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();

  DoubleTab coeffOperateur(nb_faces_tot);
  coeffOperateur=0.;

  DoubleTab nu;

  const int nb_bords=les_cl.size();
  const int marq = phi_psi_diffuse(equation());

  int face=0;
  int ind_face=0;
  int num1=0,num2=0;
  int n_bord=0;

  double dt_stab=DMAXFLOAT;

  //Calcul de la porosite
  remplir_nu(nu_);
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);

  //Calcul : contribution des parties P0, P1 et Pa au dt_stab
  if (alphaE) calculer_dt_stab_elem(nu,coeffOperateur);
  if (alphaS)
    {
      DoubleTab nu_p1;
      zone.domaine().creer_tableau_sommets(nu_p1);
      remplir_nu_p1(nu,nu_p1);
      calculer_dt_stab_som(nu_p1,coeffOperateur);
    }
  if (alphaA)
    {
      DoubleTab nu_pA;
      zone_VEF.creer_tableau_aretes(nu_pA);
      remplir_nu_pA(nu,nu_pA);
      calculer_dt_stab_aretes(nu_pA,coeffOperateur);
    }
  //Calcul : modification pour tenir compte de la matrice de masse
  for (face=0; face<nb_faces; face++)
    {
      coeffOperateur(face)/=volumes_entrelaces(face);
      assert(coeffOperateur(face)>=0.);
      coeffOperateur(face)=1./(coeffOperateur(face)+DMINFLOAT);
    }

  //Calcul : modification pour les faces de Dirichlet
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces();

      if (sub_type(Dirichlet_homogene,la_cl.valeur()) ||
          sub_type(Dirichlet,la_cl.valeur())
         )
        for (ind_face=num1; ind_face<num2; ind_face++)
          {
            face=le_bord.num_face(ind_face);
            coeffOperateur(face)=1.e20;
          }
    }

  //Calcul du pas de temps de stabilite
  //: on en a besoin que sur les faces reelles
  for (face=0; face<nb_faces; face++)
    if (coeffOperateur(face)<dt_stab)
      dt_stab=coeffOperateur(face);

  dt_stab=Process::mp_min(dt_stab);
  return dt_stab;
}

void Op_Diff_VEFP1NCP1B_Face::
calculer_dt_stab_elem(const DoubleTab& nu, DoubleTab& coeffOperateur) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();
  const Zone& zone=zone_VEF.zone();
  const Zone_Cl_VEF& zone_Cl_VEF=la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  const DoubleTab& face_normales=zone_VEF.face_normales();
  const DoubleVect& volumes=zone_VEF.volumes();

  const IntTab& elem_faces=zone_VEF.elem_faces();

  const int nb_elem_tot=zone.nb_elem_tot();
  const int nb_faces_elem=zone.nb_faces_elem();
  const int nb_bords=les_cl.size();

  int elem=0;
  int face=0,face_loc=0;
  int faceAss=0,faceAss_loc=0;
  int dim=0;
  int ind_face=0;
  int num1=0,num2=0;
  int n_bord=0;

  double psc=0.;
  double volume=0.;
  double nu_elem=0.;
  double coeff=0.;

  for (elem=0; elem<nb_elem_tot; elem++)
    {
      volume=volumes(elem);
      nu_elem=nu_(elem);

      for (face_loc=0; face_loc<nb_faces_elem; face_loc++)
        {
          face=elem_faces(elem,face_loc);

          psc=0.;
          for (dim=0; dim<dimension; dim++)
            psc+=face_normales(face,dim)*face_normales(face,dim);

          coeff=nu_elem;
          coeff/=volume;
          coeff*=psc;
          coeff*=convexite_;
          coeffOperateur(face)+=coeff;
        }
    }

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              faceAss_loc=la_cl_perio.face_associee(ind_face);
              faceAss=le_bord.num_face(faceAss_loc);

              if (face<faceAss)
                {
                  coeffOperateur(faceAss)+=coeffOperateur(face);
                  coeffOperateur(face)=coeffOperateur(faceAss);
                }
            }
        }
    }
}

void Op_Diff_VEFP1NCP1B_Face::
calculer_dt_stab_som(const DoubleTab& nu_som, DoubleTab& coeffOperateur) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();

  const int nb_faces=zone_VEF.nb_faces();

  int face=0;
  int face_C=0;

  if (laplacien_p1_.nb_lignes()<2) dimensionner(laplacien_p1_);
  if (!is_laplacian_filled_) calculer_laplacien_som(nu_som);

  //REMARQUE : on multiplie par -1 car laplacien_p1_=+Delta
  for (face=0; face<nb_faces; face++)
    {
      face_C=face*dim_ch_;//pour le cas vectoriel
      coeffOperateur(face)+=-1.*laplacien_p1_(face_C,face_C);
    }
}

void Op_Diff_VEFP1NCP1B_Face::
calculer_dt_stab_aretes(const DoubleTab& nu, DoubleTab& coeffOperateur) const
{
  Cerr<<"Error in Op_Diff_VEFP1NCP1B_Face::calculer_dt_stab_aretes()"<<finl;
  Cerr<<"Function not coded"<<finl;
  Cerr<<"Exit"<<finl;
  exit();
}


DoubleVect& Op_Diff_VEFP1NCP1B_Face::
calculer_gradient_elem(const DoubleVect& inconnue) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();

  const Zone& zone = zone_VEF.zone();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleVect& volumes = zone_VEF.volumes();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();

  const int nb_faces_elem=zone.nb_faces_elem();
  const int nb_elem_tot=zone.nb_elem_tot();
  int elem=0,face_loc=0,face=0,compi=0,compj=0;

  double signe=0.;
  double volume=0.;

  //Valeurs INTEGRALES du gradient_p0_
  for(elem=0; elem<nb_elem_tot; elem++)
    for(face_loc=0; face_loc<nb_faces_elem; face_loc++)
      {
        face=elem_faces(elem,face_loc);

        signe=1;
        if(elem!=face_voisins(face,0)) signe=-1;

        for(compi=0; compi<dim_ch_; compi++)
          for(compj=0; compj<dimension; compj++)
            gradient_p0_(elem,compi,compj)+=signe*
                                            inconnue[face*dim_ch_+compi]*
                                            face_normales(face,compj);
      }

  //Valeurs NODALES du gradient_p0_
  for (elem=0; elem<nb_elem_tot; elem++)
    {
      volume = volumes(elem);

      for (compi=0; compi<dim_ch_; compi++)
        for (compj=0; compj<dimension; compj++)
          gradient_p0_(elem,compi,compj)/=(coeff_*volume);
    }

  //REMARQUE : cet echange_espace_virtuel() est NECESSAIRE
  //dans le cas ou alphaS==1 et/ou alphaA=1
  if (alphaS) gradient_p0_*=convexite_;
  gradient_p0_.echange_espace_virtuel();
  Debog::verifier("OpDifP1NCP1B Gradient P0 : ",gradient_p0_);

  return gradient_p0_;
}

DoubleVect& Op_Diff_VEFP1NCP1B_Face::
calculer_gradient_som(const DoubleVect& inconnue) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone& zone = zone_VEF.zone();
  const Domaine& dom=zone.domaine();

  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  const int nb_faces_elem=zone.nb_faces_elem();
  const int nb_som_face=zone_VEF.nb_som_face();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_som=zone_VEF.nb_som();
  const int nb_som_tot=zone_VEF.nb_som_tot();
  const int nb_bords =les_cl.size();
  int elem=0,face_loc=0,som_loc=0,face=0;
  int compi=0,compj=0,som=0,num1=0,num2=0;
  int i=0,ind_face=0;
  int n_bord=0;

  const double coeff_som=1./(dimension)/(dimension+1);
  double signe=0.;

  const DoubleTab& face_normales = zone_VEF.face_normales();
  const DoubleVect& volume_aux_sommets=zone_VEF.volume_aux_sommets();

  DoubleTab secmem(gradient_p1_);
  ArrOfDouble sigma(dimension);

  DoubleVect secmemij;
  DoubleVect gradij;
  dom.creer_tableau_sommets(secmemij);
  dom.creer_tableau_sommets(gradij);

  const IntTab& som_elem=zone.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& face_sommets=zone_VEF.face_sommets();

  //Le second membre du systeme a inverser
  secmem=0.;
  for(elem=0; elem<nb_elem_tot; elem++)
    {
      sigma = 0;
      for(face_loc=0; face_loc<nb_faces_elem; face_loc++)
        {
          face = elem_faces(elem,face_loc);

          for(compi=0; compi<dim_ch_; compi++)
            sigma[compi]+=inconnue[face*dim_ch_+compi];
        }

      for(face_loc=0; face_loc<nb_faces_elem; face_loc++)
        {
          som = dom.get_renum_som_perio(som_elem(elem,face_loc));
          face = elem_faces(elem,face_loc);

          signe=1;
          if(elem!=face_voisins(face,0)) signe=-1;

          for(compi=0; compi<dim_ch_; compi++)
            for(compj=0; compj<dimension; compj++)
              secmem(som,compi,compj)+=coeff_som*signe*
                                       sigma[compi]*face_normales(face,compj);
        }
    }

  secmem.echange_espace_virtuel();
  Debog::verifier("OpDifP1NCP1B secmem, avant CL : ", secmem);

  //Les conditions aux limites pour le second membre
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces_tot();

      if (sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          //On ne fait rien et c'est normal
        }
      else if (sub_type(Dirichlet,la_cl.valeur()))
        {
          const Dirichlet& dirichlet =
            ref_cast(Dirichlet,la_cl.valeur());

          double x=0.,y=0.,z=0.;
          double inconnue_pt=0.;
          double temps = equation().schema_temps().temps_courant();

          const DoubleTab& coord_sommets = dom.coord_sommets();

          if (sub_type(Champ_front_txyz,dirichlet.champ_front().valeur()))
            {
              const Champ_front_txyz& champ_front =
                ref_cast(Champ_front_txyz,dirichlet.champ_front().valeur());

              for (ind_face=num1; ind_face<num2; ind_face++)
                {
                  face=le_bord.num_face(ind_face);

                  for(som_loc=0; som_loc<nb_som_face; som_loc++)
                    {
                      som=dom.get_renum_som_perio(face_sommets(face,som_loc));

                      //Formule d'integration numerique exacte pour les polynomes de degre 2
                      if (dimension==2) //formule de Simpson
                        {
                          //Coordonnees du sommet "som"
                          x=coord_sommets(som,0);
                          y=coord_sommets(som,1);

                          //Valeur de l'inconnue au point d'integration
                          for (compi=0; compi<dim_ch_; compi++)
                            {
                              inconnue_pt=
                                champ_front.valeur_au_temps_et_au_point(temps,som,x,y,z,compi);

                              for (compj=0; compj<dimension; compj++)
                                secmem(som,compi,compj) +=
                                  1./6*(2*inconnue[face*dim_ch_+compi]+inconnue_pt)
                                  *face_normales(face,compj) ;
                            }
                        }//fin du if sur dimension==2

                      else //formule exacte pour les polynomes de degre 2
                        {
                          //On suppose que l'element considere est un TETRAEDRE
                          for (i=1; i<3; i++)
                            {
                              int som2=face_sommets(face,(som_loc+i)%nb_som_face);
                              som2=dom.get_renum_som_perio(som2);

                              //Coordonnees des points d'integration
                              x=(coord_sommets(som,0)+coord_sommets(som2,0))/2.;
                              y=(coord_sommets(som,1)+coord_sommets(som2,1))/2.;
                              z=(coord_sommets(som,2)+coord_sommets(som2,2))/2.;

                              //Vitesse au point d'integration
                              for (compi=0; compi<dim_ch_; compi++)
                                {
                                  inconnue_pt=
                                    champ_front.valeur_au_temps_et_au_point(temps,som,x,y,z,compi);

                                  for (compj=0; compj<dimension; compj++)
                                    secmem(som, compi, compj) += 1./dimension*
                                                                 1/2.*inconnue_pt*face_normales(face,compj) ;
                                }
                            }
                        }//fin du else sur la dimension

                    }//fin du for sur "som_loc"

                }//fin du for sur "ind_face"

            }//fin du if sur "Champ_front_txyz"
          else
            {
              for (ind_face=num1; ind_face<num2; ind_face++)
                {
                  face = le_bord.num_face(ind_face);

                  for(som_loc=0; som_loc<nb_som_face; som_loc++)
                    {
                      som=dom.get_renum_som_perio(face_sommets(face,som_loc));

                      for (compi=0; compi<dim_ch_; compi++)
                        for (compj=0; compj<dimension; compj++)
                          secmem(som,compi,compj) += 1./dimension*
                                                     inconnue[face*dim_ch_+compi]*face_normales(face,compj) ;

                    }//fin du for sur "som_loc"

                }//fin du for sur "ind_face"
            }

        }//fin du if sur "Dirichlet"

      else if (!sub_type(Periodique,la_cl.valeur()))
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face = le_bord.num_face(ind_face);

              for(som_loc=0; som_loc<nb_som_face; som_loc++)
                {
                  som=dom.get_renum_som_perio(face_sommets(face,som_loc));

                  for (compi=0; compi<dim_ch_; compi++)
                    for (compj=0; compj<dimension; compj++)
                      secmem(som,compi,compj) += 1./dimension*
                                                 inconnue[face*dim_ch_+compi]*face_normales(face,compj) ;

                }//fin du for sur "som_loc"

            }//fin du for sur "ind_face"

        }//fin du if sur "!Periodique"

    }//fin du for sur "n_bord"

  secmem.echange_espace_virtuel();
  Debog::verifier("OpDifP1NCP1B secmem, apres CL : ", secmem);

  //Calcul de la solution du systeme a inverser
  for (compi=0; compi<dim_ch_; compi++)
    for (compj=0; compj<dimension; compj++)
      {
        for(i=0; i<nb_som_tot; i++)
          {
            som=dom.get_renum_som_perio(i);
            secmemij(som)=secmem(som,compi,compj);
          }

        //Resolution du systeme
        for(i=0; i<nb_som; i++)
          {
            som=dom.get_renum_som_perio(i);
            gradij(som)=secmemij(som)/(coeff_*volume_aux_sommets(som));
          }

        for(i=0; i<nb_som_tot; i++)
          {
            som=dom.get_renum_som_perio(i);
            gradient_p1_(som,compi,compj)
              =gradij(som);
          }
      }

  gradient_p1_.echange_espace_virtuel();
  Debog::verifier("OpDifP1NCP1B Gradient P1 : ",gradient_p1_);

  return gradient_p1_;
}

DoubleVect& Op_Diff_VEFP1NCP1B_Face::
calculer_gradient_aretes(const DoubleVect& inconnue) const
{
  return gradient_pa_;
}


DoubleVect& Op_Diff_VEFP1NCP1B_Face::
corriger_div_pour_Cl(const DoubleVect& inconnue,const DoubleTab& nu,
                     DoubleVect& div) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();
  const Zone_Cl_VEF& zone_Cl_VEF=la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
#ifndef NDEBUG
  const IntTab& face_voisins=zone_VEF.face_voisins();
#endif
  const int nb_bords =les_cl.size();
  int n_bord=0, num1=0, num2=0;
  int face=0, face_asso_loc=0, face_associee=0;
  int ind_face=0, comp=0;

  double flux=0.;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      //Reinitialisation de num1 et num2
      num1 = 0;
      num2 = le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          //periodicite
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face = le_bord.num_face(ind_face);
              face_asso_loc=la_cl_perio.face_associee(ind_face);
              face_associee=le_bord.num_face(face_asso_loc);

              if (face<face_associee)
                for (comp=0; comp<dim_ch_; comp++)
                  {
                    div[face*dim_ch_+comp]+=div[face_associee*dim_ch_+comp];
                    div[face_associee*dim_ch_+comp]=div[face*dim_ch_+comp];
                  }

            }//fin du if sur for "ind_face"

        }//fin de la periodicite

      else if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi =
            ref_cast(Neumann_paroi, la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face = le_bord.num_face(ind_face);

              assert(face_voisins(face,0)!=-1);

              for (comp=0; comp<dim_ch_; comp++)
                {
                  flux=la_cl_paroi.flux_impose(ind_face,comp)
                       *zone_VEF.surface(face);

                  div[face*dim_ch_+comp]+=flux;
                }
            }

        }//fin if sur "Neumann"
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi=
            ref_cast(Echange_externe_impose, la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face = le_bord.num_face(ind_face);

              for (comp=0; comp<dim_ch_; comp++)
                {
                  flux=la_cl_paroi.h_imp(ind_face,comp)
                       *zone_VEF.surface(face);
                  flux*=(la_cl_paroi.T_ext(ind_face,comp)-inconnue[face*dim_ch_+comp]);

                  div[face*dim_ch_+comp]+=flux;
                }
            }
        }
    }//fin du for sur "n_bords"

  div.echange_espace_virtuel();
  Debog::verifier("OpDifP1NCP1B divergence apres CL : ", div);
  return div;
}

DoubleVect& Op_Diff_VEFP1NCP1B_Face::
calculer_divergence_elem(DoubleVect& div) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();

  const DoubleTab& face_normales = zone_VEF.face_normales();

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();

  const int nb_faces_elem=zone_VEF.zone().nb_faces_elem();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  int elem=0,face_loc=0,face=0,compi=0,compj=0;

  double signe=0.;

  for(elem=0; elem<nb_elem_tot; elem++)
    for(face_loc=0; face_loc<nb_faces_elem; face_loc++)
      {
        face=elem_faces(elem,face_loc);

        signe=1.;
        if(elem!=face_voisins(face,0)) signe=-1.;

        for(compi=0; compi<dim_ch_; compi++)
          for(compj=0; compj<dimension; compj++)
            div[face*dim_ch_+compi]-=
              gradient_p0_(elem,compi,compj)
              *signe*face_normales(face,compj);
      }

  div.echange_espace_virtuel();
  Debog::verifier("OpDifP1NCP1B divergence P0 : ", div);
  return div;
}

DoubleVect& Op_Diff_VEFP1NCP1B_Face::
calculer_divergence_som(DoubleVect& div) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();
  const Zone& zone = zone_VEF.zone();
  const Domaine& dom=zone.domaine();

  const DoubleTab& face_normales = zone_VEF.face_normales();
  ArrOfDouble sigma(dimension);

  const IntTab& som_elem=zone.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();

  const int nb_faces_elem=zone.nb_faces_elem();
  const int nb_som_face=zone_VEF.nb_som_face();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  int elem=0,face_loc=0,face_loc2=0,face=0;
  int compi=0,compj=0,som=0,ind_face=0;
  int num1=0, num2=0,som_loc=0;

  static double coeff_som=1./(dimension)/(dimension+1);
  double signe=0.;

  //Algorithme sans tenir compte des CL
  for(elem=0; elem<nb_elem_tot; elem++)
    for(face_loc=0; face_loc<nb_faces_elem; face_loc++)
      {
        som=dom.get_renum_som_perio(som_elem(elem,face_loc));
        face=elem_faces(elem,face_loc);

        signe=1;
        if(elem!=face_voisins(face,0)) signe=-1;

        for(compj=0; compj<dimension; compj++)
          sigma[compj]=signe*face_normales(face,compj);

        for(face_loc2=0; face_loc2<nb_faces_elem; face_loc2++)
          for(compi=0; compi<dim_ch_; compi++)
            for(compj=0; compj<dimension; compj++)
              div[elem_faces(elem,face_loc2)*dim_ch_+compi]-=
                coeff_som*gradient_p1_(som,compi,compj)*sigma[compj];
      }

  //Les conditions aux limites
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  const IntTab& face_sommets = zone_VEF.face_sommets();
  const int nb_bords =les_cl.size();

  DoubleTab gradient_bord(dim_ch_,dimension);

  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1 = 0;
      num2 = le_bord.nb_faces();

      if (sub_type(Neumann,la_cl.valeur()) ||
          sub_type(Neumann_homogene,la_cl.valeur()) ||
          sub_type(Symetrie,la_cl.valeur())
         )
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              //Le numero de la face (qui peut etre virtuelle)
              face=le_bord.num_face(ind_face);

              //Le numero de l'element voisin
              elem=face_voisins(face,0);
              assert(elem!=-1);

              //Calcul du gradient au milieu de la face de bord
              //On prend une integration numerique approchee
              gradient_bord=0.;
              for (som_loc=0; som_loc<nb_som_face; som_loc++)
                {
                  som=dom.get_renum_som_perio(face_sommets(face,som_loc));

                  for(compi=0; compi<dim_ch_; compi++)
                    for(compj=0; compj<dimension; compj++)
                      gradient_bord(compi,compj)+=
                        gradient_p1_(som,compi,compj);
                }
              gradient_bord/=dimension;

              //Calcul divergence au bord : formule d'integration numerique
              //exacte pour polynome d'ordre 1
              for(compi=0; compi<dim_ch_; compi++)
                for(compj=0; compj<dimension; compj++)
                  div[face*dim_ch_+compi]-=
                    gradient_bord(compi,compj)
                    *face_normales(face,compj);

            }//fin du for sur "ind_face"

        }//fin du if sur "Neumann_paroi", "Neumann", "Symetrie"

    }//fin du for sur "n_bords"

  div.echange_espace_virtuel();
  Debog::verifier("OpDifP1NCP1B divergence P1 : ", div);
  return div;
}

DoubleVect& Op_Diff_VEFP1NCP1B_Face::
calculer_divergence_aretes(DoubleVect& div) const
{
  return div;
}

void Op_Diff_VEFP1NCP1B_Face::
calculer_laplacien_som(const DoubleTab& nu_som) const
{
  const Champ_base& diffu=diffusivite();

  bool testl=false;
  testl|=sub_type(Champ_Don_Fonc_xyz,diffu);
  testl|=sub_type(Champ_Don_lu,diffu);
  testl|=sub_type(Champ_Uniforme,diffu);
  testl|=sub_type(Champ_Uniforme_Morceaux,diffu);
  testl|=sub_type(Champ_Fonc_base,diffu);
  is_laplacian_filled_=testl;

  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();

  DoubleVect& coeff=laplacien_p1_.get_set_coeff();

  const DoubleTab& inconnue1=equation().inconnue().valeurs();
  const DoubleVect& porosite_face=zone_VEF.porosite_face();

  assert(laplacien_p1_.nb_lignes()>2);

  coeff=0.;
  ajouter_contribution_som(inconnue1,porosite_face,nu_som,laplacien_p1_);
  coeff*=-1;//pour l'explicite
}

DoubleTab& Op_Diff_VEFP1NCP1B_Face::
ajouter(const DoubleTab& inconnue, DoubleTab& resu) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();
  const Zone& zone=zone_VEF.zone();

  const int nb_aretes_tot=zone.nb_aretes_tot();

  //Recuperation de la diffusivite
  remplir_nu(nu_);

  //Pour tenir compte de la porosite
  const int marq = phi_psi_diffuse(equation());
  const DoubleVect& porosite_face = zone_VEF.porosite_face();
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();

  DoubleTab nu,nu_p1,nu_pA;
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);

  DoubleTab inconnue1;
  modif_par_porosite_si_flag(inconnue,inconnue1,marq,porosite_face);

  //Pour des raisons pratiques
  const DoubleVect& inconnue2 = ref_cast(DoubleVect,inconnue1);
  DoubleVect& resu2 = ref_cast(DoubleVect,resu);
  DoubleVect resu3(resu2);
  resu3=0.;

  if (alphaE)
    {
      gradient_p0_=0.;
      calculer_gradient_elem(inconnue2);

      corriger_pour_diffusivite(nu,gradient_p0_);
      gradient_p0_*=convexite_;

      calculer_divergence_elem(resu3);
      calculer_flux_bords_elem(inconnue2);
    }
  if (alphaA)
    {
      gradient_pa_=0.;
      nu_pA.resize(nb_aretes_tot);
      remplir_nu_pA(nu,nu_pA);

      calculer_gradient_aretes(inconnue2);

      corriger_pour_diffusivite(nu_pA,gradient_pa_);

      calculer_divergence_aretes(resu3);
      calculer_flux_bords_aretes(inconnue2);
    }
  corriger_div_pour_Cl(inconnue2,nu,resu3);

  //Le corriger_div_pour_Cl() doit etre fait AVANT le calcul
  //de la partie p1 car la matrice est deja codee pour tenir
  //compte des coefficients periodiques
  //REMARQUE IMPORTANTE : pour des raisons techniques inherentes
  //a TrioU, le calcul du dt_stab a lieu AVANT l'application
  //de la fonction ajouter(). Or le dt_stab a besoin de la matrice
  //pour etre correctement calcule par consequent, la matrice
  //laplacien_p1_ est construite dans la fonction calculer_dt_stab()
  //et est seulement reutilisee ici.
  if (alphaS)
    {
      zone.domaine().creer_tableau_sommets(nu_p1);
      remplir_nu_p1(nu,nu_p1);
      laplacien_p1_.ajouter_multvect(inconnue2,resu3);

      gradient_p1_=0.;
      calculer_gradient_som(inconnue2);
      corriger_pour_diffusivite(nu_p1,gradient_p1_);
      gradient_p1_*=(1.-convexite_);
      calculer_flux_bords_som(inconnue2);
    }

  if (test_) test();

  resu2+=resu3;
  resu.echange_espace_virtuel();
  modifier_flux(*this);
  return resu;
}

DoubleTab& Op_Diff_VEFP1NCP1B_Face::
calculer(const DoubleTab& inconnue, DoubleTab& resu) const
{
  resu = 0;
  return ajouter(inconnue,resu);
}

DoubleTab& Op_Diff_VEFP1NCP1B_Face::
corriger_pour_diffusivite(const DoubleTab& nu,DoubleTab& grad) const
{
  tab_multiply_any_shape(grad, nu);
  return grad;
}


void Op_Diff_VEFP1NCP1B_Face::initialiser()
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();

  const DoubleTab& unknown = equation().inconnue().valeurs();
  const int size = (unknown.nb_dim()==2) ? unknown.dimension(1) : 1;

  //Definition des gradients
  gradient_p0_.resize(0, size, Objet_U::dimension);
  zone_VEF.zone().creer_tableau_elements(gradient_p0_);

  gradient_p1_.resize(0, size, Objet_U::dimension);
  zone_VEF.zone().domaine().creer_tableau_sommets(gradient_p1_);

  if (alphaA)
    {
      gradient_pa_.resize(0, size, Objet_U::dimension);
      zone_VEF.creer_tableau_aretes(gradient_pa_);
    }

  //Initialisation de l'attribut dim_ch_
  dim_ch_=size;

  //Dimensionnemt du tableau flux_bords
  flux_bords_.resize(zone_VEF.nb_faces_bord(),size);
  flux_bords_=0.;
}

//Fonction qui calcule le flux aux bords du domaine
void Op_Diff_VEFP1NCP1B_Face::calculer_flux_bords_elem(const DoubleVect& inconnue) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  const IntTab& face_voisins=zone_VEF.face_voisins();

  const DoubleTab& face_normales=zone_VEF.face_normales();

  const int nb_bords=zone_VEF.nb_front_Cl();

  int face=0;
  int elem=0;
  int n_bord=0,ind_face=0;
  int num1=0,num2=0;
  int compi=0,compj=0;

  double surface=0.;
  double Text=0.;

  double coeff_conv=1.;
  if (alphaS) coeff_conv=convexite_;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces();

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi=
            ref_cast(Neumann_paroi,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              surface=zone_VEF.face_surfaces(face);

              for (compi=0; compi<dim_ch_; compi++)
                flux_bords_(face,compi)=coeff_conv*
                                        la_cl_paroi.flux_impose(ind_face,compi)*
                                        surface;
            }
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi=
            ref_cast(Echange_externe_impose,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              surface=zone_VEF.face_surfaces(face);

              for (compi=0; compi<dim_ch_; compi++)
                {
                  Text=la_cl_paroi.T_ext(ind_face,compi);

                  flux_bords_(face,compi)=coeff_conv*la_cl_paroi.h_imp(ind_face,compi)*surface;
                  flux_bords_(face,compi)*=(Text-inconnue[face*dim_ch_+compi]);
                }
            }
        }
      else if ( sub_type(Neumann_homogene,la_cl.valeur()) ||
                sub_type(Neumann_sortie_libre,la_cl.valeur()) ||
                sub_type(Symetrie,la_cl.valeur())
              )
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);

              for (compi=0; compi<dim_ch_; compi++)
                flux_bords_(face,compi)=0.;
            }
        }
      else
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              elem=face_voisins(face,0);
              assert(elem!=-1);

              //le coefficient de convexite est deja dans gradient_p0_
              for (compi=0; compi<dim_ch_; compi++)
                for (compj=0; compj<dimension; compj++)
                  flux_bords_(face,compi)=gradient_p0_(elem,compi,compj)
                                          *face_normales(face,compj);
            }
        }
    }//fin du for sur n_bord
}

//Fonction qui calcule le flux aux bords du domaine
void Op_Diff_VEFP1NCP1B_Face::calculer_flux_bords_som(const DoubleVect& inconnue) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone& zone = zone_VEF.zone();
  const Domaine& dom=zone.domaine();

  const IntTab& face_sommets=zone_VEF.face_sommets();

  const DoubleTab& face_normales=zone_VEF.face_normales();

  const int nb_som_face=zone_VEF.nb_som_face();
  const int nb_bords=zone_VEF.nb_front_Cl();

  int face=0;
  int som=0,som_loc=0;
  int n_bord=0,ind_face=0;
  int num1=0,num2=0;
  int compi=0,compj=0;

  double surface=0.;
  double Text=0.;
  double coeff_conv=1.;
  if (alphaE) coeff_conv=1.-convexite_;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces();

      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          const Neumann_paroi& la_cl_paroi=
            ref_cast(Neumann_paroi,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              surface=zone_VEF.face_surfaces(face);

              for (compi=0; compi<dim_ch_; compi++)
                flux_bords_(face,compi)=coeff_conv*
                                        la_cl_paroi.flux_impose(ind_face,compi)*
                                        surface;
            }
        }
      else if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Echange_externe_impose& la_cl_paroi=
            ref_cast(Echange_externe_impose,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              surface=zone_VEF.face_surfaces(face);

              for (compi=0; compi<dim_ch_; compi++)
                {
                  Text=la_cl_paroi.T_ext(ind_face,compi);

                  flux_bords_(face,compi)=coeff_conv*la_cl_paroi.h_imp(ind_face,compi)*surface;
                  flux_bords_(face,compi)*=(Text-inconnue[face*dim_ch_+compi]);
                }
            }
        }
      else if ( sub_type(Neumann_homogene,la_cl.valeur()) ||
                sub_type(Neumann_sortie_libre,la_cl.valeur()) ||
                sub_type(Symetrie,la_cl.valeur())
              )
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);

              for (compi=0; compi<dim_ch_; compi++)
                flux_bords_(face,compi)=0.;
            }
        }
      else
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);

              //le coefficient de convexite est deja dans gradient_p1_
              for (som_loc=0; som_loc<nb_som_face; som_loc++)
                {
                  som=face_sommets(face,som_loc);
                  som=dom.get_renum_som_perio(som);

                  for (compi=0; compi<dim_ch_; compi++)
                    for (compj=0; compj<dimension; compj++)
                      flux_bords_(face,compi)+=gradient_p1_(som,compi,compj)*
                                               face_normales(face,compj);
                }

              for (compi=0; compi<dim_ch_; compi++)
                flux_bords_(face,compi)/=nb_som_face;
            }
        }
    }//fin du for sur n_bord
}

//Fonction qui calcule le flux aux bords du domaine
void Op_Diff_VEFP1NCP1B_Face::calculer_flux_bords_aretes(const DoubleVect& inconnue) const
{
  Cerr<<"Op_Dift_VEF_P1NCP1B_Face::calculer_flux_bords_aretes() not coded"<<finl;
  Cerr<<"Exit"<<finl;
  exit();
}


///////////////////////////////////////////////////////////////
//Fonctions pour l'implicite
//////////////////////////////////////////////////////////////
void Op_Diff_VEFP1NCP1B_Face::
ajouter_contribution_elem(const DoubleTab& inconnue,const DoubleVect& porosite_face,
                          const DoubleTab& nu,Matrice_Morse& matrice) const
{
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  //   int nb_faces_tot = zone_VEF.nb_faces_tot();
  int nb_dim = inconnue.nb_dim();
  int nb_faces=zone_VEF.nb_faces();
  int nb_faces_elem = zone_VEF.zone().nb_faces_elem();
  int nb_comp = 1;
  if (nb_dim==2)
    nb_comp=inconnue.dimension(1);

  int i,j,num_face;
  int elem1,elem2;

  double val;
  double coeff=1./coeff_;
  if (alphaS) coeff*=convexite_;

  int nb_bords=zone_VEF.nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int num1 = le_bord.num_premiere_face();
      int num2 = num1 + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int fac_asso;
          for (num_face=num1; num_face<num2; num_face++)
            {
              elem1 = face_voisins(num_face,0);
              fac_asso = la_cl_perio.face_associee(num_face-num1)+num1;

              //A la fin de la boucle :
              //si ok=1, alors num_face appartient bien a elem1
              //si ok=0, alors num_face n'appartient pas a elem1 et
              //         fac_asso appartient a elem1
              int ok=1;
              int fac_loc=0;
              while ((fac_loc<nb_faces_elem) && (elem_faces(elem1,fac_loc)!=num_face)) fac_loc++;
              if (fac_loc==nb_faces_elem) ok=0;

              for (i=0; i<nb_faces_elem; i++)
                if ( ( (j= elem_faces(elem1,i)) > num_face ) && (j != fac_asso ) )
                  {
                    val = viscA(num_face,j,elem1,nu(elem1));

                    //                     int fac_loc=0;
                    //                     while ((fac_loc<nb_faces_elem) && (elem_faces(elem1,fac_loc)!=num_face)) fac_loc++;
                    //                     if (fac_loc==nb_faces_elem) ok=0;

                    for (int nc=0; nc<nb_comp; nc++)
                      {
                        int n0=num_face*nb_comp+nc;
                        int j0=j*nb_comp+nc;

                        matrice(n0,n0)+=val*porosite_face(num_face)*coeff;
                        matrice(n0,j0)-=val*porosite_face(j)*coeff;

                        if (!ok) n0=fac_asso*nb_comp+nc;
                        if (j<nb_faces)
                          {
                            matrice(j0,n0)-=val*porosite_face((n0-nc)/nb_comp)*coeff;
                            matrice(j0,j0)+=val*porosite_face(j)*coeff;
                          }

                      }
                  }

              //Deuxieme element
              elem2 = face_voisins(num_face,1);

              for (i=0; i<nb_faces_elem; i++)
                if ( ( (j= elem_faces(elem2,i)) > num_face ) && (j != fac_asso ) )
                  {
                    val = viscA(num_face,j,elem2,nu(elem2));

                    for (int nc=0; nc<nb_comp; nc++)
                      {
                        int n0=num_face*nb_comp+nc;
                        int j0=j*nb_comp+nc;

                        matrice(n0,n0)+=val*porosite_face(num_face)*coeff;
                        matrice(n0,j0)-=val*porosite_face(j)*coeff;
                      }
                  }

            }//fin du for sur "num_face"
        }
      else
        {
          for (num_face=num1; num_face<num2; num_face++)
            {
              elem1 = face_voisins(num_face,0);

              for (i=0; i<nb_faces_elem; i++)
                if ( (j= elem_faces(elem1,i)) > num_face )
                  {
                    val = viscA(num_face,j,elem1,nu(elem1));
                    for (int nc=0; nc<nb_comp; nc++)
                      {
                        int n0=num_face*nb_comp+nc;
                        int j0=j*nb_comp+nc;

                        matrice(n0,n0)+=val*porosite_face(num_face)*coeff;
                        matrice(n0,j0)-=val*porosite_face(j)*coeff;
                        if (j<nb_faces) //necessaire ????
                          {
                            matrice(j0,n0)-=val*porosite_face(num_face)*coeff;
                            matrice(j0,j0)+=val*porosite_face(j)*coeff;
                          }

                      }
                  }
            }
        }
    }

  //On ne remplit que les lignes reelles
  for (num_face=zone_VEF.premiere_face_int(); num_face<nb_faces; num_face++)
    {
      elem1 = face_voisins(num_face,0);
      elem2 = face_voisins(num_face,1);

      for (i=0; i<nb_faces_elem; i++)
        {
          if ( (j=elem_faces(elem1,i)) > num_face )
            {
              val = viscA(num_face,j,elem1,nu(elem1));
              for (int nc=0; nc<nb_comp; nc++)
                {
                  int n0=num_face*nb_comp+nc;
                  int j0=j*nb_comp+nc;

                  matrice(n0,n0)+=val*porosite_face(num_face)*coeff;
                  matrice(n0,j0)-=val*porosite_face(j)*coeff;
                  if (j<nb_faces)
                    {
                      matrice(j0,n0)-=val*porosite_face(num_face)*coeff;
                      matrice(j0,j0)+=val*porosite_face(j)*coeff;
                    }
                }
            }

          //           if (elem2!=-1) //test non necessaire car la face est reelle
          if ( (j=elem_faces(elem2,i)) > num_face )
            {
              val= viscA(num_face,j,elem2,nu(elem2));
              for (int nc=0; nc<nb_comp; nc++)
                {
                  int n0=num_face*nb_comp+nc;
                  int j0=j*nb_comp+nc;

                  matrice(n0,n0)+=val*porosite_face(num_face)*coeff;
                  matrice(n0,j0)-=val*porosite_face(j)*coeff;
                  if (j<nb_faces)
                    {
                      matrice(j0,n0)-=val*porosite_face(num_face)*coeff;
                      matrice(j0,j0)+=val*porosite_face(j)*coeff;
                    }

                }
            }
        }
    }
}

void Op_Diff_VEFP1NCP1B_Face::
ajouter_contribution_som(const DoubleTab& inconnue,const DoubleVect& porosite_face,
                         const DoubleTab& nu_som,Matrice_Morse& matrice) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  IntVect liste_som(dimension+2);//pour triangles et tetraedres

  DoubleTab gradient0(dimension,dimension+2);
  DoubleTab gradient1(dimension);

  const int premiere_face_int=zone_VEF.premiere_face_int();
  const int nb_faces=zone_VEF.nb_faces();
  const int nb_faces_tot=zone_VEF.nb_faces_tot();
  const int nb_bords=zone_VEF.nb_front_Cl();

  int face=0;
  int ind_face=0;
  int n_bord=0;
  int num1=0,num2=0;

  DoubleTab coeff_perio(nb_faces_tot);
  coeff_perio=1.;
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1=0;
      num2=le_bord.nb_faces_tot();

      if (sub_type(Periodique,la_cl.valeur()))
        for (ind_face=num1; ind_face<num2; ind_face++)
          {
            face=le_bord.num_face(ind_face);
            coeff_perio(face)=0.5;
          }
    }

  //
  //Partie P1 du laplacien discret :
  //on tourne sur les lignes donc nous n'avons
  //besoin que de remplir les lignes reelles.
  //ATTENTION : le remplissage des lignes reelles
  //peut induire le calcul d'un coefficient
  //lie a une colonne virtuelle
  //

  /* Faces internes */
  for (face=premiere_face_int; face<nb_faces; face++)
    {
      coeff_matrice_som(face,liste_som,
                        gradient0,gradient1,
                        porosite_face,nu_som,
                        coeff_perio,matrice);
    }

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1=0;
      num2=le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int faceAss=0;

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              faceAss=le_bord.num_face(la_cl_perio.face_associee(ind_face));

              //On prend la plus petite des faces pour etre sur de n'oublier
              //aucune face de bord
              coeff_matrice_som_perio(face,faceAss,liste_som,
                                      gradient0,gradient1,
                                      porosite_face,nu_som,
                                      coeff_perio,matrice);

            }
        }
      else if (sub_type(Symetrie,la_cl.valeur()))
        for (ind_face=num1; ind_face<num2; ind_face++)
          {
            face=le_bord.num_face(ind_face);

            coeff_matrice_som_symetrie(face,liste_som,
                                       gradient0,gradient1,
                                       porosite_face,nu_som,
                                       coeff_perio,matrice);
          }

      else
        for (ind_face=num1; ind_face<num2; ind_face++)
          {
            face=le_bord.num_face(ind_face);

            coeff_matrice_som_CL(face,liste_som,
                                 gradient0,gradient1,
                                 porosite_face,nu_som,
                                 coeff_perio,matrice);
          }
    }
}

void Op_Diff_VEFP1NCP1B_Face::
ajouter_contribution_aretes(const DoubleTab& inconnue,const DoubleVect& porosite_face,
                            const DoubleTab& nu,Matrice_Morse& matrice) const
{
}

//Fonction qui calcule les coefficients de la matrice pour une face
//INTERNE.
void Op_Diff_VEFP1NCP1B_Face::
coeff_matrice_som(const int& face,IntVect& liste_som,
                  DoubleTab& gradient0, DoubleTab& gradient1,
                  const DoubleVect& porosite_face,const DoubleTab& nu_som,
                  const DoubleTab& coeff_perio,Matrice_Morse& matrice) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();

  const ArrOfInt& tab1=matrice.get_tab1();
  const ArrOfInt& tab2=matrice.get_tab2();

  const DoubleVect& volume_aux_sommets=zone_VEF.volume_aux_sommets();

  const int nb_faces=zone_VEF.nb_faces();

  int face_C=0;
  int face2=0,face2_C=0;
  int som_loc=0,som=0;
  int som_loc0=0,som_loc1=0;
  int debut=0,size=0;
  int compi=0,compj=0;
  int elem0=0,elem1=0;
  int i=0;
  int nnz=0;

  double coeff_som=0.,coeff_mat=0.;
  double coeff_diff=0.;
  double psc=0.;
  double delta=decentrage_;

  double coeff_conv=1.;
  if (alphaE) coeff_conv=(1.-convexite_);

  //Quelques verifications
  assert(gradient0.nb_dim()==2);
  assert(gradient0.dimension(0)==dimension);
  assert(gradient0.dimension(1)==dimension+2);
  assert(gradient1.nb_dim()==1);
  assert(gradient1.dimension(0)==dimension);
  assert(liste_som.size()==dimension+2);

  //
  //Partie P1 du laplacien discret
  //
  liste_som=-1;
  gradient0=0.;
  gradient_som(face,nnz,liste_som,gradient0);


  /* gradient associe a "face" : calcul du coefficient */
  /* diagonal de la matrice associee a l'inconnue */
  coeff_mat=0.;
  for (som_loc=0; som_loc<nnz; som_loc++)
    {
      som=liste_som(som_loc);
      coeff_som=volume_aux_sommets(som)*coeff_;

      psc=0.;
      for (compj=0; compj<dimension; compj++)
        psc+=gradient0(compj,som_loc)
             *gradient0(compj,som_loc);

      psc*=coeff_som;
      psc*=nu_som(som);

      coeff_mat+=psc;
    }
  coeff_mat*=coeff_conv;

  for (compi=0; compi<dim_ch_; compi++)
    {
      face_C=face*dim_ch_+compi;
      matrice(face_C,face_C)+=coeff_mat;
    }


  /* calcul des coefficients extra-diagonaux de la matrice laplacien */
  /* la matrice etant diagonale par bloc, on effectue le calcul du */
  /* produit scalaire une seule fois avant de l'affecter aux differentes */
  /* composantes de la matrice */
  face_C=face*dim_ch_;
  debut=tab1[face_C]-1;
  size=tab1[face_C+1]-tab1[face_C];

  for (i=1; i<size; i++) //i=0 -> face2_C=face_C -> deja rempli
    {
      face2_C=tab2[debut+i]-1;
      face2=face2_C/dim_ch_;//division euclidienne

      if (face2>face)//pour la symetrie de l'operateur
        {
          coeff_mat=0.;
          for (som_loc=0; som_loc<nnz; som_loc++)
            {
              som=liste_som(som_loc);
              coeff_som=volume_aux_sommets(som)*coeff_;
              isInStencil(face2,som,elem0,som_loc0,elem1,som_loc1);

              if (elem0!=-1)//les deux faces se "voient"
                {
                  assert(som_loc0!=-1);
                  gradient1=0.;
                  gradient_som(face2,som,elem0,som_loc0,elem1,som_loc1,gradient1);

                  psc=0.;
                  for (compj=0; compj<dimension; compj++)
                    psc+=gradient0(compj,som_loc)
                         *gradient1(compj);

                  psc*=coeff_som;
                  psc*=nu_som(som);
                  coeff_mat+=psc;
                }
            }
          coeff_mat*=coeff_conv;
          coeff_diff=-1.*delta*maximum(0.,coeff_mat);
          coeff_mat+=coeff_diff;

          for (compi=0; compi<dim_ch_; compi++)
            {
              face_C=face*dim_ch_+compi;
              face2_C=face2*dim_ch_+compi;

              matrice(face_C,face2_C)+=coeff_mat*coeff_perio(face2);
              matrice(face_C,face_C)-=coeff_diff*coeff_perio(face2);
              if (face2<nb_faces)
                {
                  matrice(face2_C,face_C)+=coeff_mat;
                  matrice(face2_C,face2_C)-=coeff_diff;
                }
            }
        }
    }
}

//Fonction qui calcule les coefficients de la matrice pour une face
//de BORD qui n'est ni periodique ni symetrique.
void Op_Diff_VEFP1NCP1B_Face::
coeff_matrice_som_CL(const int& face,IntVect& liste_som,
                     DoubleTab& gradient0, DoubleTab& gradient1,
                     const DoubleVect& porosite_face,const DoubleTab& nu_som,
                     const DoubleTab& coeff_perio,Matrice_Morse& matrice) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();

  const ArrOfInt& tab1=matrice.get_tab1();
  const ArrOfInt& tab2=matrice.get_tab2();

  const DoubleVect& volume_aux_sommets=zone_VEF.volume_aux_sommets();

  const int nb_faces=zone_VEF.nb_faces();

  int face_C=0;
  int face2=0,face2_C=0;
  int som_loc=0,som=0;
  int som_loc0=0,som_loc1=0;
  int debut=0,size=0;
  int compi=0,compj=0;
  int elem0=0,elem1=0;
  int i=0;
  int nnz=0;

  double coeff_som=0.,coeff_mat=0.;
  double coeff_diff=0.;
  double psc=0.;
  double delta=decentrage_;

  double coeff_conv=1.;
  if (alphaE) coeff_conv=(1.-convexite_);

  //Quelques verifications
  assert(gradient0.nb_dim()==2);
  assert(gradient0.dimension(0)==dimension);
  assert(gradient0.dimension(1)==dimension+2);
  assert(gradient1.nb_dim()==1);
  assert(gradient1.dimension(0)==dimension);
  assert(liste_som.size()==dimension+2);

  //
  //Partie P1 du laplacien discret
  //
  liste_som=-1;
  gradient0=0.;
  gradient_som_CL(face,nnz,liste_som,gradient0);


  /* gradient associe a "face" : calcul du coefficient */
  /* diagonal de la matrice associee a l'inconnue */
  coeff_mat=0.;
  for (som_loc=0; som_loc<nnz; som_loc++)
    {
      som=liste_som(som_loc);
      coeff_som=volume_aux_sommets(som)*coeff_;

      psc=0.;
      for (compj=0; compj<dimension; compj++)
        psc+=gradient0(compj,som_loc)
             *gradient0(compj,som_loc);

      psc*=coeff_som;
      psc*=nu_som(som);

      coeff_mat+=psc;
    }
  coeff_mat*=coeff_conv;

  for (compi=0; compi<dim_ch_; compi++)
    {
      face_C=face*dim_ch_+compi;
      matrice(face_C,face_C)+=coeff_mat;
    }


  /* calcul des coefficients extra-diagonaux de la matrice laplacien */
  /* la matrice etant diagonale par bloc, on effectue le calcul du */
  /* produit scalaire une seule fois avant de l'affecter aux differentes */
  /* composantes de la matrice */
  face_C=face*dim_ch_;
  debut=tab1[face_C]-1;
  size=tab1[face_C+1]-tab1[face_C];

  for (i=1; i<size; i++) //i=0 -> face2_C=face_C -> deja rempli
    {
      face2_C=tab2[debut+i]-1;
      face2=face2_C/dim_ch_;//division euclidienne

      if (face2>face)//pour la symetrie de l'operateur
        {
          coeff_mat=0.;
          for (som_loc=0; som_loc<nnz; som_loc++)
            {
              som=liste_som(som_loc);
              coeff_som=volume_aux_sommets(som)*coeff_;
              isInStencil(face2,som,elem0,som_loc0,elem1,som_loc1);

              if (elem0!=-1)//les deux faces se "voient"
                {
                  assert(som_loc0!=-1);
                  gradient1=0.;
                  gradient_som(face2,som,elem0,som_loc0,elem1,som_loc1,gradient1);

                  psc=0.;
                  for (compj=0; compj<dimension; compj++)
                    psc+=gradient0(compj,som_loc)
                         *gradient1(compj);

                  psc*=coeff_som;
                  psc*=nu_som(som);
                  coeff_mat+=psc;
                }
            }
          coeff_mat*=coeff_conv;
          coeff_diff=-1.*delta*maximum(0.,coeff_mat);
          coeff_mat+=coeff_diff;

          for (compi=0; compi<dim_ch_; compi++)
            {
              face_C=face*dim_ch_+compi;
              face2_C=face2*dim_ch_+compi;

              matrice(face_C,face2_C)+=coeff_mat*coeff_perio(face2);
              matrice(face_C,face_C)-=coeff_diff*coeff_perio(face2);
              if (face2<nb_faces)
                {
                  matrice(face2_C,face_C)+=coeff_mat;
                  matrice(face2_C,face2_C)-=coeff_diff;
                }
            }
        }
    }
}

//Fonction qui calcule les coefficients de la matrice pour une face
//de BORD qui est une face de SYMETRIE.
void Op_Diff_VEFP1NCP1B_Face::
coeff_matrice_som_symetrie(const int& face,IntVect& liste_som,
                           DoubleTab& gradient0, DoubleTab& gradient1,
                           const DoubleVect& porosite_face,const DoubleTab& nu_som,
                           const DoubleTab& coeff_perio,Matrice_Morse& matrice) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();

  const ArrOfInt& tab1=matrice.get_tab1();
  const ArrOfInt& tab2=matrice.get_tab2();

  const DoubleVect& volume_aux_sommets=zone_VEF.volume_aux_sommets();

  const int nb_faces=zone_VEF.nb_faces();

  int face_C=0;
  int face2=0,face2_C=0;
  int som_loc=0,som=0;
  int som_loc0=0,som_loc1=0;
  int debut=0,size=0;
  int compi=0,compj=0;
  int elem0=0,elem1=0;
  int i=0;
  int nnz=0;

  double coeff_som=0.,coeff_mat=0.;
  double coeff_diff=0.;
  double psc=0.;
  double delta=decentrage_;

  double coeff_conv=1.;
  if (alphaE) coeff_conv=(1.-convexite_);

  //Quelques verifications
  assert(gradient0.nb_dim()==2);
  assert(gradient0.dimension(0)==dimension);
  assert(gradient0.dimension(1)==dimension+2);
  assert(gradient1.nb_dim()==1);
  assert(gradient1.dimension(0)==dimension);
  assert(liste_som.size()==dimension+2);

  //
  //Partie P1 du laplacien discret
  //
  liste_som=-1;
  gradient0=0.;
  gradient_som_CL(face,nnz,liste_som,gradient0);


  /* gradient associe a "face" : calcul du coefficient */
  /* diagonal de la matrice associee a l'inconnue */
  coeff_mat=0.;
  for (som_loc=0; som_loc<nnz; som_loc++)
    {
      som=liste_som(som_loc);
      coeff_som=volume_aux_sommets(som)*coeff_;

      psc=0.;
      for (compj=0; compj<dimension; compj++)
        psc+=gradient0(compj,som_loc)
             *gradient0(compj,som_loc);

      psc*=coeff_som;
      psc*=nu_som(som);

      coeff_mat+=psc;
    }
  coeff_mat*=coeff_conv;

  for (compi=0; compi<dim_ch_; compi++)
    {
      face_C=face*dim_ch_+compi;
      matrice(face_C,face_C)+=coeff_mat;
    }


  /* calcul des coefficients extra-diagonaux de la matrice laplacien */
  /* la matrice etant diagonale par bloc, on effectue le calcul du */
  /* produit scalaire une seule fois avant de l'affecter aux differentes */
  /* composantes de la matrice */
  /* REMARQUE : la matrice est modifiee pour tenir compte des CL de */
  /* symetrie, mais on ne calcule pas les modifications associees -> */
  /* c'est la fonction Op_VEF_Face::modifier_pour_Cl() qui le fera */
  face_C=face*dim_ch_;
  debut=tab1[face_C]-1;
  size=tab1[face_C+1]-tab1[face_C];
  size-=(dim_ch_-1);//-> pour ne pas calculer les coefficients inutiles

  for (i=1; i<size; i++) //i=0 -> face2_C=face_C -> deja rempli
    {
      face2_C=tab2[debut+i]-1;
      face2=face2_C/dim_ch_;//division euclidienne

      if (face2>face)//pour la symetrie de l'operateur
        {
          coeff_mat=0.;
          for (som_loc=0; som_loc<nnz; som_loc++)
            {
              som=liste_som(som_loc);
              coeff_som=volume_aux_sommets(som)*coeff_;
              isInStencil(face2,som,elem0,som_loc0,elem1,som_loc1);

              if (elem0!=-1)//les deux faces se "voient"
                {
                  assert(som_loc0!=-1);
                  gradient1=0.;
                  gradient_som(face2,som,elem0,som_loc0,elem1,som_loc1,gradient1);

                  psc=0.;
                  for (compj=0; compj<dimension; compj++)
                    psc+=gradient0(compj,som_loc)
                         *gradient1(compj);

                  psc*=coeff_som;
                  psc*=nu_som(som);
                  coeff_mat+=psc;
                }
            }
          coeff_mat*=coeff_conv;
          coeff_diff=-1.*delta*maximum(0.,coeff_mat);
          coeff_mat+=coeff_diff;

          for (compi=0; compi<dim_ch_; compi++)
            {
              face_C=face*dim_ch_+compi;
              face2_C=face2*dim_ch_+compi;

              matrice(face_C,face2_C)+=coeff_mat*coeff_perio(face2);
              matrice(face_C,face_C)-=coeff_diff*coeff_perio(face2);
              if (face2<nb_faces)
                {
                  matrice(face2_C,face_C)+=coeff_mat;
                  matrice(face2_C,face2_C)-=coeff_diff;
                }
            }
        }
    }
}

//Fonction qui calcule les coefficients de la matrice pour une face
//PERIODIQUE.
void Op_Diff_VEFP1NCP1B_Face::
coeff_matrice_som_perio(const int& face,const int& faceAss, IntVect& liste_som,
                        DoubleTab& gradient0, DoubleTab& gradient1,
                        const DoubleVect& porosite_face,const DoubleTab& nu_som,
                        const DoubleTab& coeff_perio,Matrice_Morse& matrice) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();

  const ArrOfInt& tab1=matrice.get_tab1();
  const ArrOfInt& tab2=matrice.get_tab2();

  const DoubleVect& volume_aux_sommets=zone_VEF.volume_aux_sommets();

  const int nb_faces=zone_VEF.nb_faces();

  int face_C=0;
  int face2=0,face2_C=0;
  int som_loc=0,som=0;
  int som_loc0=0,som_loc1=0;
  int debut=0,size=0;
  int compi=0,compj=0;
  int elem0=0,elem1=0;
  int i=0;
  int nnz=0;

  double coeff_som=0.,coeff_mat=0.;
  double coeff_diff=0.;
  double psc=0.;
  double delta=decentrage_;

  double coeff_conv=1.;
  if (alphaE) coeff_conv=(1.-convexite_);

  //Quelques verifications
  assert(gradient0.nb_dim()==2);
  assert(gradient0.dimension(0)==dimension);
  assert(gradient0.dimension(1)==dimension+2);
  assert(gradient1.nb_dim()==1);
  assert(gradient1.dimension(0)==dimension);
  assert(liste_som.size()==dimension+2);

  //
  //Partie P1 du laplacien discret
  //
  liste_som=-1;
  gradient0=0.;
  gradient_som(face,nnz,liste_som,gradient0);


  /* gradient associe a "face" : calcul du coefficient */
  /* diagonal de la matrice associee a l'inconnue */
  coeff_mat=0.;
  for (som_loc=0; som_loc<nnz; som_loc++)
    {
      som=liste_som(som_loc);
      coeff_som=volume_aux_sommets(som)*coeff_;

      psc=0.;
      for (compj=0; compj<dimension; compj++)
        psc+=gradient0(compj,som_loc)
             *gradient0(compj,som_loc);

      psc*=coeff_som;
      psc*=nu_som(som);

      coeff_mat+=psc;
    }
  coeff_mat*=coeff_conv;

  for (compi=0; compi<dim_ch_; compi++)
    {
      face_C=face*dim_ch_+compi;
      matrice(face_C,face_C)+=coeff_mat;
    }


  /* calcul des coefficients extra-diagonaux de la matrice laplacien */
  /* la matrice etant diagonale par bloc, on effectue le calcul du */
  /* produit scalaire une seule fois avant de l'affecter aux differentes */
  /* composantes de la matrice */
  face_C=face*dim_ch_;
  debut=tab1[face_C]-1;
  size=tab1[face_C+1]-tab1[face_C];

  for (i=1; i<size; i++) //i=0 -> face2_C=face_C -> deja rempli
    {
      face2_C=tab2[debut+i]-1;
      face2=face2_C/dim_ch_;//division euclidienne

      if (face2>face)//pour la symetrie de l'operateur
        {
          coeff_mat=0.;
          for (som_loc=0; som_loc<nnz; som_loc++)
            {
              som=liste_som(som_loc);
              coeff_som=volume_aux_sommets(som)*coeff_;
              isInStencil(face2,som,elem0,som_loc0,elem1,som_loc1);

              if (elem0!=-1)//les deux faces se "voient"
                {
                  assert(som_loc0!=-1);
                  gradient1=0.;
                  gradient_som(face2,som,elem0,som_loc0,elem1,som_loc1,gradient1);

                  psc=0.;
                  for (compj=0; compj<dimension; compj++)
                    psc+=gradient0(compj,som_loc)
                         *gradient1(compj);

                  psc*=coeff_som;
                  psc*=nu_som(som);
                  coeff_mat+=psc;
                }
            }
          coeff_mat*=coeff_conv;
          coeff_diff=-1.*delta*maximum(0.,coeff_mat);
          coeff_mat+=coeff_diff;

          for (compi=0; compi<dim_ch_; compi++)
            {
              face_C=face*dim_ch_+compi;
              face2_C=face2*dim_ch_+compi;

              matrice(face_C,face2_C)+=coeff_mat*coeff_perio(face2);
              matrice(face_C,face_C)-=coeff_diff*coeff_perio(face2);
              if (face2<nb_faces)
                {
                  matrice(face2_C,face_C)+=coeff_mat*coeff_perio(face);
                  matrice(face2_C,face2_C)-=coeff_diff*coeff_perio(face);
                }
            }
        }
    }
}

void Op_Diff_VEFP1NCP1B_Face::
ajouter_contribution(const DoubleTab& inconnue,Matrice_Morse& matrice) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();
  const Zone& zone=zone_VEF.zone();

  //Marqueur pour tenir compte de la porosite
  const int marq=phi_psi_diffuse(equation());

  //Lignes pour tenir compte de la porosite
  const DoubleVect& porosite_elem=zone_VEF.porosite_elem();
  DoubleVect porosite_face(zone_VEF.porosite_face());
  if (!marq) porosite_face=1.;

  //Lignes pour tenir compte de la diffusivite
  DoubleTab nu,nu_p1,nu_pA;
  remplir_nu(nu_);
  modif_par_porosite_si_flag(nu_,nu,!marq,porosite_elem);

  //RESTE juste a gerer la porosite a la face
  if (alphaE)
    ajouter_contribution_elem(inconnue,porosite_face,nu,matrice);
  if (alphaS)
    {
      zone.domaine().creer_tableau_sommets(nu_p1);
      remplir_nu_p1(nu,nu_p1);
      ajouter_contribution_som(inconnue,porosite_face,nu_p1,matrice);
    }
  if (alphaA)
    {
      zone_VEF.creer_tableau_aretes(nu_pA);
      remplir_nu_pA(nu,nu_pA);
      ajouter_contribution_aretes(inconnue,porosite_face,nu_pA,matrice);
    }

  if (test_) test();
}

//Description: Calcule la diffusivite "nu_p1" aux sommets du maillage
// en fonction de la diffusivite "nu_elem" aux elements.
// On suppose que nu_elem a son espace virtuel a jour,
// que nu_p1 est dimensionne nb_dim==1 avec la structure domaine.md_vector_sommets()
// En sortie l'espace virtuel de nu_p1 est mis a jour
//
// L'interpolateur calculs pour un sommet la moyenne (non ponderee) des
// diffusivites sur les elements adjacents a ce sommet.
void Op_Diff_VEFP1NCP1B_Face::
remplir_nu_p1(const DoubleTab& nu_elem,DoubleTab& nu_p1) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();
  const Zone& zone=zone_VEF.zone();
  const Domaine& dom=zone.domaine();

  const int nb_som = dom.nb_som();
  const int nb_elem_tot=zone.nb_elem_tot();
  const int nb_som_elem=zone.nb_som_elem();

  int elem=0;
  int som_loc=0,som=0;

  const IntTab& elem_som=zone.les_elems();

  ArrOfInt nb_elem_per_som(nb_som); // Intialise a zero par defaut

  assert(nu_elem.get_md_vector() == zone.md_vector_elements());
  assert(nu_p1.get_md_vector() == dom.md_vector_sommets());
  // On a besoin que l'espace virtuel de nu_elem soit a jour.
  assert_espace_virtuel_vect(nu_elem);

  //Calcul effectif de "nu_som"

  nu_p1=0.;

  for (elem=0; elem<nb_elem_tot; elem++)
    {
      const double nu = nu_elem[elem];
      for (som_loc=0; som_loc<nb_som_elem; som_loc++)
        {
          som=elem_som(elem,som_loc);
          // Ne pas calculer les valeurs pour les sommets virtuels
          // note BM: l'algo precedent ne calculait pas correctement les valeurs non plus
          if (som < nb_som)
            {
              som=dom.get_renum_som_perio(som);
              nu_p1(som) += nu;
              nb_elem_per_som(som)++;
            }
        }
    }

  for (som = 0; som < nb_som; som++)
    {
      const int som_perio = dom.get_renum_som_perio(som);
      int nvoisins = nb_elem_per_som[som_perio];
      if (nvoisins > 0)
        {
          // La premiere fois qu'on tombe sur ce sommet on fait la division.
          // On traite du meme coup le cas ou le sommet n'a pas d'elements voisins.
          nu_p1(som_perio) /= nvoisins;
          nb_elem_per_som[som_perio] = 0;
        }
      if (som != som_perio)
        nu_p1(som) = nu_p1(som_perio);
    }
  // Le codage precedent n'avait apparemment pas besoin d'echange espace virtuel.
  // A mon avis (BM) c'est un miracle du au fait qu'on fait les calculs avec epaisseur2
  // (calcul sur nb_som_tot, donc les sommets des elements d'epaisseur 1 sont ok mais
  //  pas ceux d'epaisseur 2)
  // Je prefere ce codage: (si on enleve, ca fait des ecarts en parallele)
  nu_p1.echange_espace_virtuel();
}

//Fonction qui calcule la diffusivite aux aretes du maillage
//a partir de la diffusivite aux elements
//CONTRAINTE : la diffusivite aux elements "nu_elem" doit
//             deja avoir ete calculee et est consideree
//             comme un parametre d'entree
//CONTRAINTE : la diffusivite aux sommets "nu_som" doit
//            deja avoir ete DIMENSIONNEE a la bonne
//            taille et est consideree comme un parametre
//           de sortie
//CONTRAINTE : l'interpolateur choisi pour le calcul de
//            "nu_som" est tel que pour un sommet s
//            donne nous avons
//            * une liste L des elements possedant le sommet s
//            * la diffusivite en s est la moyenne geometrique
//              des "nu_elem" de L
void Op_Diff_VEFP1NCP1B_Face::
remplir_nu_pA(const DoubleTab& nu_elem,DoubleTab& nu_pA) const
{
  Cerr << "Op_Diff_VEFP1NCP1B_Face::remplir_nu_pA() not coded" << finl;
  Cerr << "Exit" << finl;
  exit();
}


//Creation d'une liste qui a une face donnee associe les faces
//voisines au sens du support de l'operateur de diffusion P1B
//On passe par une liste intermediaire qui donne pour un sommet
//donnee, la liste des faces "voyant" le sommet au sens de l'operateur
//de diffusion P1B
void Op_Diff_VEFP1NCP1B_Face::liste_face(IntLists& liste,int& nnz) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone& zone = zone_VEF.zone();
  const Domaine& dom=zone.domaine();


  const IntTab& som_elem=zone.les_elems();
  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();

  const int firstFaceInt=zone_VEF.premiere_face_int();
  const int nb_som_tot=dom.nb_som_tot();
  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nb_som_elem=zone.nb_som_elem();
  const int nb_faces_tot=zone_VEF.nb_faces_tot();
  const int nb_faces_elem=zone.nb_faces_elem();
  const int nb_bords=zone_VEF.nb_front_Cl();

  int face=0,face2=0;
  int face_loc=0;
  int elem=0,elem_loc=0;
  int som=0,som_loc=0;
  int i=0,size=0;
  int n_bord=0,ind_face=0;
  int num1=0,num2=0;
  int tmp=0;

  IntTab faces_perio(nb_faces_tot);
  ArrOfBit fait(nb_faces_tot);
  IntLists sommets_faces(nb_som_tot);

  //Il faut creer un second tableau travaillant sur les faces periodiques
  //sinon la matrice pourrait ne pas etre homogene a l'operateur explicite
  for (face=0; face<nb_faces_tot; face++)
    faces_perio(face)=face;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces_tot();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int faceAss=0;

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              faceAss=la_cl_perio.face_associee(ind_face);
              faceAss=le_bord.num_face(faceAss);

              //Test afin de ne parcourir que la moitie des faces periodiques
              //sachant que l'algorithme qui suit tient compte de ce choix
              //REMARQUE : ce test marche aussi en parallele ou les faces
              //virtuelles ne sont pas classees
              if (face<faceAss)
                {
                  faces_perio(face)=faceAss;
                  faces_perio(faceAss)=face;
                }
            }
        }
    }

  //Connectivite liee aux sommets
  for (elem=0; elem<nb_elem_tot; elem++)
    for (som_loc=0; som_loc<nb_som_elem; som_loc++)
      {
        som=som_elem(elem,som_loc);
        som=dom.get_renum_som_perio(som);

        for (face_loc=0; face_loc<nb_faces_elem; face_loc++)
          {
            face=elem_faces(elem,face_loc);

            sommets_faces[som].add(face);
            if (faces_perio(face)!=face)
              sommets_faces[som].add(faces_perio(face));
          }
      }

  nnz=0;
  liste.dimensionner(nb_faces_tot);
  //REMARQUE IMPORTANTE : IL FAUT ABSOLUMENT COMMENCER
  //PAR REMPLIR LES FACES PERIODIQUES SINON :
  //-LA MATRICE NE POURRA PAS ETRE PERIODIQUE
  //-ET DES COEFFICIENTS POURRAIENT ETRE OUBLIES
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces_tot();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          int faceAss=0;

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              faceAss=la_cl_perio.face_associee(ind_face);
              faceAss=le_bord.num_face(faceAss);

              //Test afin de ne parcourir que la moitie des faces periodiques
              //sachant que l'algorithme qui suit tient compte de ce choix
              //REMARQUE : ce test marche aussi en parallele ou les faces
              //virtuelles ne sont pas classees
              if (face<faceAss)
                {
                  //RAZ du tableau fait
                  fait=0;

                  //pour la periodicite
                  size=liste[face].size();
                  for (i=0; i<size; i++)
                    fait.setbit(liste[face][i]);

                  //Pour que le premier element de la liste soit "face"
                  //Tient compte de la periodicite
                  if (size!=0)
                    {
                      tmp=liste[face][0];
                      liste[face][0]=face;
                      liste[face].add(tmp);
                      tmp=liste[faceAss][0];
                      liste[faceAss][0]=faceAss;
                      liste[faceAss].add(tmp);
                    }
                  else
                    {
                      liste[face].add(face);
                      liste[faceAss].add(faceAss);
                    }

                  fait.setbit(face);
                  fait.setbit(faceAss);
                  nnz+=2;//car on ajoute 2 coefficients

                  for (elem_loc=0; elem_loc<2; elem_loc++)
                    {
                      elem=face_voisins(face,elem_loc);
                      assert(elem!=-1);

                      for (som_loc=0; som_loc<nb_som_elem; som_loc++)
                        {
                          som=som_elem(elem,som_loc);
                          som=dom.get_renum_som_perio(som);

                          size=sommets_faces[som].size();
                          for (i=0; i<size; i++)
                            {
                              face2=sommets_faces[som][i];

                              if (!fait[face2])
                                {
                                  fait.setbit(face2);
                                  liste[face].add(face2);
                                  liste[faceAss].add(face2);
                                  liste[face2].add(face);
                                  liste[face2].add(faceAss);
                                  nnz+=4;//car on ajoute 4 coefficients
                                }

                            }//fin du for sur "i"
                        }//fin du for sur "som_loc"
                    }//fin du for sur "elem_loc"
                }//fin du if sur "face<faceAss"
            }//fin du for sur "ind_face"
        }//fin Periodique
    }//fin n_bord

  //Autres conditions aux limites
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces_tot();

      if (!sub_type(Periodique,la_cl.valeur()))
        for (ind_face=num1; ind_face<num2; ind_face++)
          {
            face=le_bord.num_face(ind_face);

            //RAZ du tableau fait
            fait=0;

            //pour la periodicite
            size=liste[face].size();
            for (i=0; i<size; i++)
              fait.setbit(liste[face][i]);

            //Pour que le premier element de la liste soit "face"
            //Tient compte de la periodicite
            if (size!=0)
              {
                tmp=liste[face][0];
                liste[face][0]=face;
                liste[face].add(tmp);
              }
            else
              liste[face].add(face);

            fait.setbit(face);
            nnz++;

            elem=face_voisins(face,0);
            assert(elem!=-1);

            for (som_loc=0; som_loc<nb_som_elem; som_loc++)
              {
                som=som_elem(elem,som_loc);
                som=dom.get_renum_som_perio(som);

                size=sommets_faces[som].size();
                for (i=0; i<size; i++)
                  {
                    face2=sommets_faces[som][i];

                    if (!fait[face2])
                      {
                        fait.setbit(face2);
                        liste[face].add(face2);
                        nnz++;
                      }

                  }//fin du for sur "i"
              }//fin du for sur "som_loc"
          }//fin du else sur !Periodique
    }//fin du for sur n_bord

  for (face=firstFaceInt; face<nb_faces_tot; face++)
    if (!zone_VEF.est_une_face_virt_bord(face))
      {
        //RAZ du tableau fait
        fait=0;

        //pour la periodicite
        size=liste[face].size();
        for (i=0; i<size; i++)
          fait.setbit(liste[face][i]);

        //Pour que le premier element de la liste soit "face"
        //Tient compte de la periodicite
        if (size!=0)
          {
            tmp=liste[face][0];
            liste[face][0]=face;
            liste[face].add(tmp);
          }
        else
          liste[face].add(face);

        fait.setbit(face);
        nnz++;

        for (elem_loc=0; elem_loc<2; elem_loc++)
          {
            elem=face_voisins(face,elem_loc);

            if (elem!=-1) //pour face interne de joint
              for (som_loc=0; som_loc<nb_som_elem; som_loc++)
                {
                  som=som_elem(elem,som_loc);
                  som=dom.get_renum_som_perio(som);

                  size=sommets_faces[som].size();
                  for (i=0; i<size; i++)
                    {
                      face2=sommets_faces[som][i];

                      if (!fait[face2])
                        {
                          fait.setbit(face2);
                          liste[face].add(face2);
                          nnz++;
                        }

                    }//fin du for sur "i"
                }//fin du for sur "som_loc"
          }//fin du for sur "elem_loc"
      }//fin du for sur "face"
}

//Fonction qui calcule pour une face "face" donnee et un sommet "som"
//du stencil de "face", le gradient associe a "face" pour le sommet "som"
//ATTENTION : cette fonction ne doit etre utilisee que pour le calcul
//            des gradients pour remplir une COLONNE de la matrice
//CONTRAINTE : "face" est le numero de la face sur laquelle on veut
//             calculer le gradient. La face "face" est une face
//             INTERNE ou PERIODIQUE.
//CONTRAINTE : le sommet "som" en parametre est suppose etre le
//             numero d'un sommet APRES renumerotation periodique
//CONTRAINTE : le sommet "som" DOIT appartenir au stencil de "face"
//CONTRAINTE : "elem0" et "elem1" sont les numeros des elements
//             qui contiennent "face" ET "som". Si l'un des elements
//             est a -1, c'est que "face" et "som" n'appartiennent
//             pas a un meme element. REMARQUE : l'element elem0
//             est toujours suppose different de -1.
//CONTRAINTE : "som_loc0" et "som_loc1" sont les numeros locaux
//             de "sim_glob" dans repectivement "elem0" et "elem1".
//             Si l'un des elements est a -1, alors le numero local
//             de sommet correspondant est a -1
//CONTRAINTE : le parametre de sortie "grad" contenant l'evaluation
//             du gradient associe a "face" au sommet "som" est
//             sense etre CORRECTEMENT dimensionne AVANT l'appel
//             de cette fonction
void Op_Diff_VEFP1NCP1B_Face::
gradient_som(const int& face,const int& som_glob,
             const int& elem0, const int& som_loc0,
             const int& elem1, const int& som_loc1,
             DoubleTab& grad) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();

  const DoubleTab& face_normales=zone_VEF.face_normales();
  const DoubleVect& volume_aux_sommets=zone_VEF.volume_aux_sommets();

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();

  const double coeff=1./dimension;
  const double coeff_som=coeff/(dimension+1);

  int face_opp=0,compj=0;
  int elem=0;
  double signe=0.;

  assert(grad.nb_dim()==1);

  //
  //Calcul du gradient
  //

  /* On regarde le premier element voisin */
  assert(elem0!=-1);
  assert(som_loc0!=-1);

  //Calcul du gradient local
  face_opp = elem_faces(elem0,som_loc0);
  signe=1.;
  if(elem0!=face_voisins(face_opp,0)) signe=-1.;

  for(compj=0; compj<dimension; compj++)
    grad(compj)=coeff_som*signe*
                face_normales(face_opp,compj);

  /* On regarde le deuxieme element voisin */
  if (elem1==-1)
    {
      /* Plusieurs possibilite :
         - "face" est interne reel
         - "face" est interne virtuel
         - "face" est de bord reel
         - "face" est de bord virtuel
         - "face" une face de joint */
      assert(som_loc1==-1);
      elem=face_voisins(face,1);
      assert(face_voisins(face,0)!=-1);

      /* "face" est de bord */
      if (elem==-1 && face_opp!=face)
        for (compj=0; compj<dimension; compj++)
          grad(compj)+=coeff*face_normales(face,compj) ;
    }
  else
    {
      assert(som_loc1!=-1);
      face_opp=elem_faces(elem1,som_loc1);
      signe=1.;
      if(elem1!=face_voisins(face_opp,0)) signe=-1.;

      //Calcul du gradient local
      for(compj=0; compj<dimension; compj++)
        grad(compj)+=coeff_som*signe*
                     face_normales(face_opp,compj);
    }

  grad/=(coeff_*volume_aux_sommets(som_glob));
}

//Pour une face donnee, calcule le gradient associee a cette face
//pour chacun des sommets ou le gradient est non nul
//ATTENTION : cette fonction ne doit etre utilisee que pour le calcul
//            des gradients pour remplir une LIGNE de la matrice
//CONTRAINTE : le tableau "grad" est sense etre correctement
//             dimensionne AVANT l'appel de cette fonction
//ENTREE : "face" est le numero global de la face INTERNE
//         ou PERIODIQUE dont on veut calculer le gradient
//SORTIE : "som_glob" est une liste qui contient tous les sommets
//          inclus dans le stencil de "face"
//SORTIE : "nnz" est le nombre de sommets inclus dans le stencil
//         de face
void Op_Diff_VEFP1NCP1B_Face::
gradient_som(const int& face,int& nnz, IntVect& som_glob,DoubleTab& grad) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();
  const Zone& zone=zone_VEF.zone();
  const Domaine& dom=zone.domaine();

  const DoubleTab& face_normales=zone_VEF.face_normales();
  const DoubleVect& volume_aux_sommets=zone_VEF.volume_aux_sommets();

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& elem_som=zone.les_elems();

  const double coeff=1./dimension;
  const double coeff_som=coeff/(dimension+1);

  const int nb_som_elem=zone.nb_som_elem();

  int face_opp=0,compj=0;
  int som_loc=0,som=0;
  int loc=0;
  int elem=0;

  double signe=0.;
  double volume=0.;

  assert(som_glob.size()==dimension+2);
  assert(grad.nb_dim()==2);
  assert(grad.dimension(1)==dimension+2);

  //
  //Calcul du gradient
  //

  nnz=0;
  /* On regarde le premier element voisin */
  elem=face_voisins(face,0);
  assert(elem!=-1);

  for (som_loc=0; som_loc<nb_som_elem; som_loc++)
    {
      som=elem_som(elem,som_loc);
      som=dom.get_renum_som_perio(som);

      face_opp=elem_faces(elem,som_loc);
      signe=1.;
      if(elem!=face_voisins(face_opp,0)) signe=-1.;

      //ajout a la liste et incrementation du repere
      som_glob(som_loc)=som;
      nnz++;

      //Calcul du gradient local
      for(compj=0; compj<dimension; compj++)
        grad(compj,som_loc)=coeff_som*signe*
                            face_normales(face_opp,compj);
    }
  assert(nnz=nb_som_elem);

  /* On regarde le deuxieme element voisin */
  elem=face_voisins(face,1);
  assert(elem!=-1);

  for (som_loc=0; som_loc<nb_som_elem; som_loc++)
    {
      som=elem_som(elem,som_loc);
      som=dom.get_renum_som_perio(som);

      face_opp=elem_faces(elem,som_loc);
      signe=1.;
      if(elem!=face_voisins(face_opp,0)) signe=-1.;

      //Localisation dans la liste deja construite
      for (loc=0; loc<nnz; loc++)
        if (som_glob[loc]==som)
          break;

      //ajout a la liste et incrementation du repere
      if (loc==nnz)
        {
          som_glob(nnz)=som;
          nnz++;
        }

      //Calcul du gradient local
      for(compj=0; compj<dimension; compj++)
        grad(compj,loc)+=coeff_som*signe*
                         face_normales(face_opp,compj);
    }
  assert(nnz<=(dimension+2));

  for (som_loc=0; som_loc<nnz; som_loc++)
    {
      som=som_glob(som_loc);
      volume=coeff_*volume_aux_sommets(som);

      //On divise par la matrice de masse lumpee
      for(compj=0; compj<dimension; compj++)
        grad(compj,som_loc)/=volume;
    }
}

//Pour une face donnee, calcule le gradient associee a cette face
//pour chacun des sommets ou le gradient est non nul
//ATTENTION : cette fonction ne doit etre utilisee que pour le calcul
//            des gradients pour remplir une LIGNE de la matrice
//CONTRAINTE : le tableau "grad" est sense etre correctement
//             dimensionne AVANT l'appel de cette fonction
//ENTREE : "face" est le numero global de la face de BORD
//         NON PERIODIQUE dont on veut calculer le gradient
//SORTIE : "som_glob" est une liste qui contient tous les sommets
//          inclus dans le stencil de "face"
//SORTIE : "nnz" est le nombre de sommets inclus dans le stencil
//         de face
void Op_Diff_VEFP1NCP1B_Face::
gradient_som_CL(const int& face,int& nnz, IntVect& som_glob,DoubleTab& grad) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();
  const Zone& zone=zone_VEF.zone();
  const Domaine& dom=zone.domaine();

  const DoubleTab& face_normales=zone_VEF.face_normales();
  const DoubleVect& volume_aux_sommets=zone_VEF.volume_aux_sommets();

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& elem_som=zone.les_elems();
  const IntTab& face_sommets=zone_VEF.face_sommets();

  const double coeff=1./dimension;
  const double coeff_som=coeff/(dimension+1);

  const int nb_som_elem=zone.nb_som_elem();
  const int nb_som_face=zone_VEF.nb_som_face();

  int face_opp=0,compj=0;
  int som_loc=0,som=0;
  int loc=0;
  int elem=0;

  double signe=0.;
  double volume=0.;

  assert(som_glob.size()==dimension+2);
  assert(grad.nb_dim()==2);
  assert(grad.dimension(1)==dimension+2);

  //
  //Calcul du gradient
  //

  nnz=0;
  /* On regarde le premier element voisin */
  elem=face_voisins(face,0);
  assert(elem!=-1);

  for (som_loc=0; som_loc<nb_som_elem; som_loc++)
    {
      som=elem_som(elem,som_loc);
      som=dom.get_renum_som_perio(som);

      face_opp=elem_faces(elem,som_loc);
      signe=1.;
      if(elem!=face_voisins(face_opp,0)) signe=-1.;

      //ajout a la liste et incrementation du repere
      som_glob(som_loc)=som;
      nnz++;

      //Calcul du gradient local
      for(compj=0; compj<dimension; compj++)
        grad(compj,som_loc)=coeff_som*signe*
                            face_normales(face_opp,compj);
    }
  assert(nnz=nb_som_elem);

  /* On regarde le deuxieme element voisin */
  assert(face_voisins(face,1)==-1);

  for (som_loc=0; som_loc<nb_som_face; som_loc++) //sommets de "face"
    {
      som=face_sommets(face,som_loc);
      som=dom.get_renum_som_perio(som);

      for (loc=0; loc<nnz; loc++)
        if (som_glob(loc)==som)
          break;
      assert(som_loc<nnz);

      for (compj=0; compj<dimension; compj++)
        grad(compj,loc)+=coeff*face_normales(face,compj) ;
    }
  assert(nnz<=(dimension+2));

  for (som_loc=0; som_loc<nnz; som_loc++)
    {
      som=som_glob(som_loc);
      volume=coeff_*volume_aux_sommets(som);

      //On divise par la matrice de masse lumpee
      for(compj=0; compj<dimension; compj++)
        grad(compj,som_loc)/=volume;
    }
}


//Fonction verifiant si "som_glob" est dans le stencil de "face"
//CONTRAINTE : "som_glob" DOIT etre un numero PERIODIQUE de sommet
//SORTIE : le numero des elements du stencil de "face" qui contiennent
//         le sommet "som_glob". Si le sommet n'est pas contenu
//         dans un des elements du stencil de "face" alors le numero
//         de l'element considere est mis a -1
//SORTIE : som_loc0 et som_loc1 sont les numeros locaux de som_glob
//         dans respectivement elem0 et elem1.
//         Si les sommet n'est pas contenu dans un des elements du
//         stencil de "face" alors le numero local du sommet
//         correspondant est mis a -1
void Op_Diff_VEFP1NCP1B_Face::isInStencil(int face,int som_glob,
                                          int& elem0, int& som_loc0,
                                          int& elem1, int& som_loc1) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();
  const Zone& zone=zone_VEF.zone();
  const Domaine& dom=zone.domaine();

  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& elem_som=zone.les_elems();

  const int nb_som_elem=zone.nb_som_elem();

  int elem00=0,elem11=0;
  int som_loc=0,som=0;

  elem00=face_voisins(face,0);
  assert(elem00!=-1);

  for (som_loc=0; som_loc<nb_som_elem; som_loc++)
    {
      som=elem_som(elem00,som_loc);
      som=dom.get_renum_som_perio(som);

      if (som_glob==som)
        {
          som_loc0=som_loc;
          elem0=elem00;
          break;
        }
    }
  if (som_loc==nb_som_elem)
    {
      elem0=-1;
      som_loc0=-1;
    }

  elem11=face_voisins(face,1);
  if (elem11==-1)
    {
      elem1=-1;
      som_loc1=-1;
    }
  else
    for (som_loc=0; som_loc<nb_som_elem; som_loc++)
      {
        som=elem_som(elem11,som_loc);
        som=dom.get_renum_som_perio(som);

        if (som_glob==som)
          {
            som_loc1=som_loc;
            elem1=elem11;
            break;
          }
      }
  if (som_loc==nb_som_elem)
    {
      elem1=-1;
      som_loc1=-1;
    }

  //On ordonne
  if (elem0==-1 && elem1!=-1)
    {
      elem0=elem1;
      elem1=-1;
      som_loc0=som_loc1;
      som_loc1=-1;
    }
}

void Op_Diff_VEFP1NCP1B_Face::dimensionner(Matrice_Morse& matrice) const
{
  const Zone_VEF_PreP1b& zone_VEF = zone_VEFPreP1B();

  const int nb_faces_tot=zone_VEF.nb_faces_tot();
  const int nb_comp=(inconnue_.valeur().valeurs().nb_dim()==2) ?
                    inconnue_.valeur().valeurs().dimension(1) : 1;

  int face=0;
  int i=0,size=0;
  int comp=0,nnz=0,debut=0,face_f77=0,face_C=0;
  int nb_faces_of_symetry=0;
  int next=0;

  ArrOfBit is_symetry(nb_faces_tot);

  IntVect& tab1 = matrice.get_set_tab1();
  IntVect& tab2 = matrice.get_set_tab2();

  IntLists faces_faces;

  if (alphaE && !alphaS && !alphaA)
    Op_VEF_Face::dimensionner(la_zone_vef.valeur(), la_zcl_vef.valeur(), matrice);

  if (alphaS)
    {
      //Calcul de la liste des faces
      liste_face(faces_faces,nnz);
      isFaceOfSymetry(is_symetry,nb_faces_of_symetry);

      //Dimensionnement des tableaux de la matrice
      //REMARQUE : par essence, ce dimensionnement sera toujours plus GRAND
      //que le dimensionnement par defaut issu de OP_VEF_FACE
      //REMARQUE : pour tenir compte des contraintes dues aux faces de
      //symetrie, le dimensionnement de la matrice doit etre legerement
      //elargie
      size=nnz*nb_comp;//dimensionnement sans face de symetrie
      //      size+=nb_faces_of_symetry*(nb_comp-1)*nb_comp;//dimensionnement avec faces de symetrie
      // pour chaque face de symetrie pour chaque composante on ajoute nb_comp coeffs pour chaque face liee
      for (face=0; face<nb_faces_tot; face++)
        if (is_symetry[face])
          {
            int nb_v=faces_faces[face].size();
            size+=nb_v*(nb_comp-1)*nb_comp;
          }

      matrice.dimensionner(nb_faces_tot*nb_comp,size);

      //Initialisation des grandeurs connues pour tab1
      tab1[nb_faces_tot*nb_comp]=size+1;
      tab1[0]=1;

      //Remplissage de tab1

      /* pour les autres composantes de la face 0 */
      size=faces_faces[0].size();
      if (is_symetry[0]) size*=(nb_comp);
      for (comp=1; comp<nb_comp; comp++)
        {
          face_C=comp;
          tab1[face_C]=tab1[face_C-1]+size;
        }

      /* pour les autres faces */
      for (face=1; face<nb_faces_tot; face++)
        {
          size=faces_faces[face-1].size();
          if (is_symetry[face-1]) size*=(nb_comp);
          for (comp=0; comp<nb_comp; comp++)
            {
              face_C=face*nb_comp+comp;
              tab1[face_C]=tab1[face_C-1]+size;
              size=faces_faces[face].size();
              if (is_symetry[face]) size*=(nb_comp);
            }
        }

      //Remplissage de tab2
      for (face=0; face<nb_faces_tot; face++)
        {
          size=faces_faces[face].size();

          for (comp=0; comp<nb_comp; comp++)
            {
              debut=tab1[face*nb_comp+comp]-1;

              for (i=0; i<size; i++)
                {
                  face_C=faces_faces[face][i]*nb_comp+comp;
                  face_f77=face_C+1;
                  tab2[debut+i]=face_f77;
                }//fin du for sur "i"

            }//fin du for sur "comp"

        }//fin du for sur "face"

      for (face=0; face<nb_faces_tot; face++)
        if (is_symetry[face])
          {
            size=faces_faces[face].size();

            for (comp=0; comp<nb_comp; comp++)
              {
                debut=tab1[face*nb_comp+comp]-1;
                debut+=size;
                for (int voi=0; voi<size; voi++)
                  {
                    int face2=faces_faces[face][voi];
                    for (i=0; i<nb_comp-1; i++)
                      {
                        //reste de la division euclidienne
                        next=(comp+i+1)%nb_comp;
                        face_C=face2*nb_comp+next;
                        face_f77=face_C+1;
                        tab2[debut+i]=face_f77;
                        // Cerr <<face*nb_comp+comp<<" face "<<face << " comp "<< comp<< " face2 "<< face2 << " comp "<<next <<" jface2 "<< face_C<<finl;
                        assert(debut+i<tab1[face*nb_comp+comp+1]-1);
                      }
                    debut+=nb_comp-1;
                  }

              }//fin du for sur "comp"

          }//fin du for sur "face"

    }//fin du if sur "alphaS"

  if (alphaA)
    {
      Cerr << "Erreur Op_Dift_VEFP1NCP1B_Face::dimensionner(Matrice_Morse&)" << finl;
      Cerr << "Le dimensionnement de la matrice implicite avec l'option alphaA"
           << " n'est pas encore codee" << finl;
      Cerr << "Sortie du programme" << finl;
      exit();
    }
}

//Fonction qui initialise le tableau is_symetry passe en argument :
//-si une face "f" est une face de symetrie alors is_symetry(f)=1
//-si une face "f" n'est pas une face de symetrie alors is_symetry(f)=0
//La fonction renvoie egalement le nombre total de faces de symetrie (argument nnz)
//REMARQUE : le tableau "is_symetry" doit etre dimensionne a nb_faces_tot
//AVANT d'utiliser cette fonction
void Op_Diff_VEFP1NCP1B_Face::isFaceOfSymetry(ArrOfBit& is_symetry,int& nnz) const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();
  const Zone_Cl_VEF& zone_Cl_VEF=la_zcl_vef.valeur();

  const int nb_bords=zone_VEF.nb_front_Cl();

  int n_bord=0;
  int num1=0,num2=0;
  int ind_face=0;

  assert(is_symetry.size_array()==zone_VEF.nb_faces_tot());

  //Preinitialisation
  nnz=0;
  is_symetry=0;

  //Modification pour les faces de symetrie
  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces_tot();

      if (sub_type(Symetrie,la_cl.valeur()))
        {
          nnz+=num2;

          for (ind_face=num1; ind_face<num2; ind_face++)
            is_symetry.setbit(le_bord.num_face(ind_face));
        }
    }
}



//////////////////////////////////////////////////////////
//Fonctions de test
/////////////////////////////////////////////////////////

void Op_Diff_VEFP1NCP1B_Face::test() const
{
  const Zone_VEF_PreP1b& zone_VEF=zone_VEFPreP1B();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Zone& zone = zone_VEF.zone();
  const Domaine& dom=zone.domaine();

  const Solveur_Masse& solveur_masse=equation().solv_masse();

  const int nb_bords=zone_VEF.nb_front_Cl();
  const int firstFaceInt=zone_VEF.premiere_face_int();
  const int nb_som_tot=dom.nb_som_tot();

  const DoubleTab& unknown = equation().inconnue().valeurs();
  const DoubleTab& xv=zone_VEF.xv();
  const DoubleTab& xs=dom.les_sommets();

  DoubleTab inco(unknown);
  DoubleVect& incoV=ref_cast(DoubleVect,inco);
  incoV=0.;
  //   DoubleTab inco2(unknown);
  //   DoubleVect& inco2V=ref_cast(DoubleVect,inco2);
  //   inco2V=0.;

  DoubleTab tmp(inco);

  DoubleTab resu(unknown);
  DoubleVect& resuV=ref_cast(DoubleVect,resu);
  DoubleTab resuMat(unknown);
  const DoubleVect& resuMatV=ref_cast(DoubleVect,resuMat);
  DoubleTab gradientMat(dimension,dimension+2);


  IntVect som_glob(dimension+2);

  //Marqueur pour tenir compte de la porosite
  const int marq=phi_psi_diffuse(equation());

  //Lignes pour tenir compte de la porosite
  const DoubleVect& poroE=zone_VEF.porosite_elem();
  DoubleVect poroF(zone_VEF.porosite_face());
  if (!marq) poroF=1.;

  //Lignes pour tenir compte de la diffusivite
  DoubleTab nu;
  remplir_nu(nu_);
  modif_par_porosite_si_flag(nu_,nu,!marq,poroE);

  DoubleTab nu_p1;
  dom.creer_tableau_sommets(nu_p1);
  remplir_nu_p1(nu,nu_p1);

  //Diverses variables utiles
  int face=0;
  int faceAss=0;
  int comp=0;
  int size0=zone_VEF.nb_faces();
  int i=0,j=0;
  int nnz=0;
  int compi=0,compj=0;
  int som=0;
  int n_bord=0;
  int num1=0,num2=0,ind_face=0;
  int ii=0;

  int size1=1;
  if (inco.nb_dim()==2)
    size1=unknown.dimension(1);

  Matrice_Morse matrice;
  dimensionner(matrice);
  if (alphaS) ajouter_contribution_som(inco,poroF,nu_p1,matrice);
  else if (alphaE) ajouter_contribution_elem(inco,poroF,nu,matrice);

  bool test1=false;
  double max=0.;

  DoubleTab gradient1(dimension);

  Motcle type;
  if (Process::nproc()>1)
    type="_PAR";
  else
    type="_SEQ";

  Motcles les_mots(8);
  {
    les_mots[0] = "matrice";
    les_mots[1] = "result";
    les_mots[2] = "res";
    les_mots[3] = "resMat";
    les_mots[4] = "grad";
    les_mots[5] = "gradMat";
    les_mots[6] = "div";
    les_mots[7] = "ligne_mat";
  }
  Motcle proc(Process::me());
  proc+=".txt";

  for (i=0; i<les_mots.size(); i++)
    {
      les_mots[i]+=type;
      les_mots[i]+=proc;
    }

  ofstream mat(les_mots[0].getChar());
  const Matrice_Morse& matConst=matrice;
  for (i=0; i<zone_VEF.nb_faces(); i++)
    {
      for (j=0; j<matConst.nb_colonnes(); j++)
        mat<<matConst(i,j)<<",";
      mat<<endl;
    }

  double coeff_diag=0.;
  double sum_coeff_extra_diag=0.;
  ofstream ligneMat(les_mots[7].getChar());
  for (i=0; i<matConst.nb_lignes(); i++)
    {
      coeff_diag=matConst(i,i);
      ligneMat<<"Ligne : "<<i<<endl;
      ligneMat<<"Coeff diag : "<<coeff_diag<<endl;

      ligneMat<<"Coeff extra diag : ";
      sum_coeff_extra_diag=0.;
      for (j=0; j<matConst.nb_colonnes(); j++)
        if (j!=i) sum_coeff_extra_diag+=matConst(i,j);
      ligneMat<<sum_coeff_extra_diag<<endl;

      ligneMat<<"Coeff extra diag par colonne : ";
      sum_coeff_extra_diag=0.;
      for (j=0; j<matConst.nb_colonnes(); j++)
        if (j!=i) sum_coeff_extra_diag+=matConst(j,i);
      ligneMat<<sum_coeff_extra_diag<<endl;
    }


  Motcle gradi("grad1");
  gradi+=type;
  gradi+=proc;
  int elem=0;
  int elem0=0,elem1=0;
  int som_loc0=1,som_loc1=1;
  int som_loc=0;
  const int nb_som_elem=zone.nb_som_elem();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& elem_som=zone.les_elems();
  ofstream grad1(gradi.getChar());
  for (face=0; face<size0; face++)
    {
      grad1<<"Face : "<<face<<endl;
      grad1<<"(";
      for (i=0; i<dimension; i++)
        grad1<<xv(face,i)<<",";
      grad1<<")"<<endl;

      for (ii=0; ii<2; ii++)
        {
          elem=face_voisins(face,ii);
          if (elem!=-1)
            for (som_loc=0; som_loc<nb_som_elem; som_loc++)
              {
                som=elem_som(elem,som_loc);
                som=dom.get_renum_som_perio(som);
                isInStencil(face,som,elem0,som_loc0,elem1,som_loc1);

                gradient1=0.;
                gradient_som(face,som,elem0,som_loc0,
                             elem1,som_loc1,gradient1);

                grad1<<som<<"(";
                for (i=0; i<dimension; i++)
                  grad1<<xs(som,i)<<",";
                grad1<<"):(";
                for (compj=0; compj<dimension; compj++)
                  grad1<<gradient1(compj)<<",";
                grad1<<")"<<endl;
              }
        }
    }

  ofstream result(les_mots[1].getChar());
  ofstream res(les_mots[2].getChar());
  ofstream resMat(les_mots[3].getChar());
  ofstream grad(les_mots[4].getChar());
  ofstream gradMat(les_mots[5].getChar());
  ofstream div(les_mots[6].getChar());

  for (face=firstFaceInt; face<size0; face++)
    {
      if (Process::je_suis_maitre()) incoV[face*size1+comp]=1.;
      inco.echange_espace_virtuel();

      //Version explicite
      gradient_p1_=0.;
      calculer_gradient_som(inco);

      //Controle des gradients
      grad<<"Face interne : "<<face*size1+comp<<endl;
      grad<<"(";
      for (i=0; i<dimension; i++)
        grad<<xv(face,i)<<",";
      grad<<")"<<endl;
      for (som=0; som<nb_som_tot; som++)
        {
          grad <<som<<"(";
          for (i=0; i<dimension; i++)
            grad<<xs(som,i)<<",";
          grad<<"):(";
          for (compi=0; compi<dim_ch_; compi++)
            for (compj=0; compj<dimension; compj++)
              grad<<gradient_p1_(som,compi,compj)<<",";
          grad<<")"<<endl;
        }

      gradientMat=0.;
      som_glob=-1;
      gradient_som(face,nnz,som_glob,gradientMat);

      gradMat<<"Face interne : "<<face*size1+comp<<endl;
      gradMat<<"(";
      for (i=0; i<dimension; i++)
        gradMat<<xv(face,i)<<",";
      gradMat<<")"<<endl;
      for (i=0; i<nnz; i++)
        {
          gradMat<<som_glob[i]<<"(";
          for (ii=0; ii<dimension; ii++)
            gradMat<<xs(som_glob[i],ii)<<",";
          gradMat<<"):(";
          for (compj=0; compj<dimension; compj++)
            gradMat<<gradientMat(compj,i)<<",";
          gradMat<<")"<<endl;
        }

      resu=0.;
      calculer_divergence_som(resu);
      corriger_Cl_test(resu);
      solveur_masse.appliquer(resu);
      div<<"Face interne : "<<face*size1+comp<<endl;
      div<<"(";
      for (i=0; i<dimension; i++)
        div<<xv(face,i)<<",";
      div<<")"<<endl;
      for (i=0; i<size0; i++)
        {
          div<<i<<"(";
          for (ii=0; ii<dimension; ii++)
            div<<xv(i,ii)<<",";
          div<<"):=(";
          for (j=0; j<size1; j++)
            div<<resu[i*size1+j]<<",";
          div<<")"<<endl;
        }

      resu*=-1.;
      res<<"Face interne : "<<face*size1+comp<<endl;
      res<<"(";
      for (i=0; i<dimension; i++)
        res<<xv(face,i)<<",";
      res<<")"<<endl;
      for (i=0; i<size0; i++)
        {
          res<<i<<"(";
          for (ii=0; ii<dimension; ii++)
            res<<xv(i,ii)<<",";
          res<<"):=(";
          for (j=0; j<size1; j++)
            res<<resu[i*size1+j]<<",";
          res<<")"<<endl;
        }

      //Version matricielle
      resuMat=0.;
      matrice.ajouter_multTab_(inco,resuMat);
      solveur_masse.appliquer(resuMat);
      resuMat.echange_espace_virtuel();
      resMat<<"Face interne :"<<face*size1+comp<<endl;
      resMat<<"(";
      for (i=0; i<dimension; i++)
        resMat<<xv(face,i)<<",";
      resMat<<")"<<endl;
      for (i=0; i<size0; i++)
        {
          resMat<<i<<"(";
          for (ii=0; ii<dimension; ii++)
            resMat<<xv(i,ii)<<",";
          resMat<<"):=(";
          for (j=0; j<size1; j++)
            resMat<<resuMatV[i*size1+j]<<",";
          resMat<<")"<<endl;
        }

      //Difference entre les resultat
      resu-=resuMat;

      //Affichage des differences
      max=resu.local_max_abs_vect();
      if (max>1.e-14)
        {
          result<<"Diff pour face interne : "<<face*size1+comp<<endl;
          result<<"(";
          for (i=0; i<dimension; i++)
            result<<xv(face,i)<<",";
          result<<")"<<endl;
          for (i=0; i<size0; i++)
            {
              test1=false;
              for (j=0; j<size1; j++)
                test1|=(dabs(resuV[i*size1+j])>1.e-14);

              if (test1)
                {
                  result<<i<<"(";
                  for (ii=0; ii<dimension; ii++)
                    result<<xv(i,ii)<<",";
                  result<<"):=(";
                  for (j=0; j<size1; j++)
                    result <<resuV[i*size1+j]<<",";
                  result <<")"<<endl;
                }
            }
        }
      else
        {
          result<<"Diff pour face interne : "<<face*size1+comp<<endl;
          result<<"Maximum : "<<max<<endl;
        }

      if (Process::je_suis_maitre()) incoV[face*size1+comp]=0.;
      inco.echange_espace_virtuel();
    }


  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces_tot();

      //Modif pour tenir compte des conditions de Dirichlet
      //-> la matrice de masse doit tout annuler
      if (sub_type(Dirichlet_homogene,la_cl.valeur()))
        {
          //Il ne sert a rien de faire un test dans ce cas
        }

      else if (sub_type(Dirichlet,la_cl.valeur()))
        for (ind_face=num1; ind_face<num2; ind_face++)
          {
            face=le_bord.num_face(ind_face);

            //Version explicite
            gradient_p1_=0.;
            calculer_gradient_som(inco);

            //Controle des gradients
            grad<<"Face Dirichlet : "<<face*size1+comp<<endl;
            grad<<"(";
            for (i=0; i<dimension; i++)
              grad<<xv(face,i)<<",";
            grad<<")"<<endl;
            for (som=0; som<nb_som_tot; som++)
              {
                grad <<som<<"(";
                for (i=0; i<dimension; i++)
                  grad<<xs(som,i)<<",";
                grad<<"):(";
                for (compi=0; compi<dim_ch_; compi++)
                  for (compj=0; compj<dimension; compj++)
                    grad<<gradient_p1_(som,compi,compj)<<",";
                grad<<")"<<endl;
              }

            gradientMat=0.;
            som_glob=-1;
            gradient_som_CL(face,nnz,som_glob,gradientMat);

            gradMat<<"Face Dirichlet : "<<face*size1+comp<<endl;
            gradMat<<"(";
            for (i=0; i<dimension; i++)
              gradMat<<xv(face,i)<<",";
            gradMat<<")"<<endl;
            for (i=0; i<nnz; i++)
              {
                gradMat<<som_glob[i]<<"(";
                for (ii=0; ii<dimension; ii++)
                  gradMat<<xs(som_glob[i],ii)<<",";
                gradMat<<"):(";
                for (compj=0; compj<dimension; compj++)
                  gradMat<<gradientMat(compj,i)<<",";
                gradMat<<")"<<endl;
              }

            for (i=0; i<size0; i++)
              for (j=0; j<size1; j++)
                resuV[i*size1+j]=0.;
          }

      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

          assert(num2%2==0);
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face=le_bord.num_face(ind_face);
              faceAss=le_bord.num_face(la_cl_perio.face_associee(ind_face));

              if (Process::je_suis_maitre()) incoV[face*size1+comp]=1.;
              if (Process::je_suis_maitre()) incoV[faceAss*size1+comp]=1.;
              inco.echange_espace_virtuel();

              //Version explicite
              gradient_p1_=0.;
              calculer_gradient_som(inco);

              //Controle des gradients
              grad <<"Face perio : "<<face*size1+comp<<endl;
              grad<<"(";
              for (i=0; i<dimension; i++)
                grad<<xv(face,i)<<",";
              grad<<")"<<endl;
              for (som=0; som<nb_som_tot; som++)
                {
                  grad <<som<<"(";
                  for (i=0; i<dimension; i++)
                    grad<<xs(som,i)<<",";
                  grad<<"):(";
                  for (compi=0; compi<dim_ch_; compi++)
                    for (compj=0; compj<dimension; compj++)
                      grad<<gradient_p1_(som,compi,compj)<<",";
                  grad<<")"<<endl;
                }

              gradientMat=0.;
              som_glob=-1;
              gradient_som(face,nnz,som_glob,gradientMat);

              gradMat<<"Face perio : "<<face*size1+comp<<endl;
              gradMat<<"(";
              for (i=0; i<dimension; i++)
                gradMat<<xv(face,i)<<",";
              gradMat<<")"<<endl;
              for (i=0; i<nnz; i++)
                {
                  gradMat<<som_glob[i]<<"(";
                  for (ii=0; ii<dimension; ii++)
                    gradMat<<xs(som_glob[i],ii)<<",";
                  gradMat<<"):(";
                  for (compj=0; compj<dimension; compj++)
                    gradMat<<gradientMat(compj,i)<<",";
                  gradMat<<")"<<endl;
                }

              resu=0.;
              calculer_divergence_som(resu);
              corriger_Cl_test(resu);
              solveur_masse.appliquer(resu);
              div<<"Face perio : "<<face*size1+comp<<endl;
              div<<"(";
              for (i=0; i<dimension; i++)
                div<<xv(face,i)<<",";
              div<<")"<<endl;
              for (i=0; i<size0; i++)
                {
                  div<<i<<"(";
                  for (ii=0; ii<dimension; ii++)
                    div<<xv(i,ii)<<",";
                  div<<"):=(";
                  for (j=0; j<size1; j++)
                    div<<resu[i*size1+j]<<",";
                  div<<")"<<endl;
                }

              resu*=-1.;
              res<<"Face perio : "<<face*size1+comp<<endl;
              res<<"(";
              for (i=0; i<dimension; i++)
                res<<xv(face,i)<<",";
              res<<")"<<endl;
              for (i=0; i<size0; i++)
                {
                  res<<i<<"(";
                  for (ii=0; ii<dimension; ii++)
                    res<<xv(i,ii)<<",";
                  res<<"):=(";
                  for (j=0; j<size1; j++)
                    res<<resu[i*size1+j]<<",";
                  res<<")"<<endl;
                }

              //Version matricielle
              resuMat=0.;
              matrice.ajouter_multTab_(inco,resuMat);
              solveur_masse.appliquer(resuMat);
              resuMat.echange_espace_virtuel();
              resMat<<"Face perio :"<<face*size1+comp<<endl;
              resMat<<"(";
              for (i=0; i<dimension; i++)
                resMat<<xv(face,i)<<",";
              resMat<<")"<<endl;
              for (i=0; i<size0; i++)
                {
                  resMat<<i<<"(";
                  for (ii=0; ii<dimension; ii++)
                    resMat<<xv(i,ii)<<",";
                  resMat<<"):=(";
                  for (j=0; j<size1; j++)
                    resMat<<resuMatV[i*size1+j]<<",";
                  resMat<<")"<<endl;
                }

              //Difference entre les resultat
              resu-=resuMat;

              //Affichage des differences
              max=resu.local_max_abs_vect();
              if (max>1.e-14)
                {
                  result<<"Diff pour face perio : "<<face*size1+comp<<endl;
                  result<<"(";
                  for (i=0; i<dimension; i++)
                    result<<xv(face,i)<<",";
                  result<<")"<<endl;
                  for (i=0; i<size0; i++)
                    {
                      test1=false;
                      for (j=0; j<size1; j++)
                        test1|=(dabs(resuV[i*size1+j])>1.e-14);

                      if (test1)
                        {
                          result<<i<<"(";
                          for (ii=0; ii<dimension; ii++)
                            result<<xv(i,ii)<<",";
                          result<<"):=(";
                          for (j=0; j<size1; j++)
                            result <<resuV[i*size1+j]<<",";
                          result <<")"<<endl;
                        }
                    }
                }
              else
                {
                  result<<"Diff pour face perio : "<<face*size1+comp<<endl;
                  result<<"Maximum : "<<max<<endl;
                }

              if (Process::je_suis_maitre()) incoV[face*size1+comp]=0.;
              if (Process::je_suis_maitre()) incoV[faceAss*size1+comp]=0.;
              inco.echange_espace_virtuel();
            }
        }//fin Perio
      else
        for (ind_face=num1; ind_face<num2; ind_face++)
          {
            face=le_bord.num_face(ind_face);
            if (Process::je_suis_maitre()) incoV[face*size1+comp]=1.;
            inco.echange_espace_virtuel();

            //Version explicite
            gradient_p1_=0.;
            calculer_gradient_som(inco);

            //Controle des gradients
            grad <<"Face CL : "<<face*size1+comp<<endl;
            grad<<"(";
            for (i=0; i<dimension; i++)
              grad<<xv(face,i)<<",";
            grad<<")"<<endl;
            for (som=0; som<nb_som_tot; som++)
              {
                grad <<som<<"(";
                for (i=0; i<dimension; i++)
                  grad<<xs(som,i)<<",";
                grad<<"):(";
                for (compi=0; compi<dim_ch_; compi++)
                  for (compj=0; compj<dimension; compj++)
                    grad<<gradient_p1_(som,compi,compj)<<",";
                grad<<")"<<endl;
              }

            gradientMat=0.;
            som_glob=-1;
            gradient_som_CL(face,nnz,som_glob,gradientMat);

            gradMat<<"Face CL : "<<face*size1+comp<<endl;
            gradMat<<"(";
            for (i=0; i<dimension; i++)
              gradMat<<xv(face,i)<<",";
            gradMat<<")"<<endl;
            for (i=0; i<nnz; i++)
              {
                gradMat<<som_glob[i]<<"(";
                for (ii=0; ii<dimension; ii++)
                  gradMat<<xs(som_glob[i],ii)<<",";
                gradMat<<"):(";
                for (compj=0; compj<dimension; compj++)
                  gradMat<<gradientMat(compj,i)<<",";
                gradMat<<")"<<endl;
              }

            resu=0.;
            calculer_divergence_som(resu);
            corriger_Cl_test(resu);
            solveur_masse.appliquer(resu);
            div<<"Face CL : "<<face*size1+comp<<endl;
            div<<"(";
            for (i=0; i<dimension; i++)
              div<<xv(face,i)<<",";
            div<<")"<<endl;
            for (i=0; i<size0; i++)
              {
                div<<i<<"(";
                for (ii=0; ii<dimension; ii++)
                  div<<xv(i,ii)<<",";
                div<<"):=(";
                for (j=0; j<size1; j++)
                  div<<resu[i*size1+j]<<",";
                div<<")"<<endl;
              }

            resu*=-1.;
            res<<"Face CL : "<<face*size1+comp<<endl;
            res<<"(";
            for (i=0; i<dimension; i++)
              res<<xv(face,i)<<",";
            res<<")"<<endl;
            for (i=0; i<size0; i++)
              {
                test1=false;
                for (j=0; j<size1; j++)
                  test1|=(dabs(resuV[i*size1+j])>max);

                if (test1)
                  {
                    res<<i<<"(";
                    for (ii=0; ii<dimension; ii++)
                      res<<xv(i,ii)<<",";
                    res<<"):=(";
                    for (j=0; j<size1; j++)
                      res<<resu[i*size1+j]<<",";
                    res<<")"<<endl;
                  }
              }

            //Version matricielle
            resuMat=0.;
            matrice.ajouter_multTab_(inco,resuMat);
            solveur_masse.appliquer(resuMat);
            resuMat.echange_espace_virtuel();
            resMat<<"Face CL :"<<face*size1+comp<<endl;
            resMat<<"(";
            for (i=0; i<dimension; i++)
              resMat<<xv(face,i)<<",";
            resMat<<")"<<endl;
            for (i=0; i<size0; i++)
              {
                resMat<<i<<"(";
                for (ii=0; ii<dimension; ii++)
                  resMat<<xv(i,ii)<<",";
                resMat<<"):=(";
                for (j=0; j<size1; j++)
                  resMat<<resuMatV[i*size1+j]<<",";
                resMat<<")"<<endl;
              }

            //Difference entre les resultat
            resu-=resuMat;

            //Affichage des differences
            max=resu.local_max_abs_vect();
            if (max>1.e-14)
              {
                result<<"Diff pour face CL : "<<face*size1+comp<<endl;
                result<<"(";
                for (i=0; i<dimension; i++)
                  result<<xv(face,i)<<",";
                result<<")"<<endl;
                for (i=0; i<size0; i++)
                  {
                    test1=false;
                    for (j=0; j<size1; j++)
                      test1|=(dabs(resuV[i*size1+j])>1.e-14);

                    if (test1)
                      {
                        result<<i<<"(";
                        for (ii=0; ii<dimension; ii++)
                          result<<xv(i,ii)<<",";
                        result<<"):=(";
                        for (j=0; j<size1; j++)
                          result <<resuV[i*size1+j]<<",";
                        result <<")"<<endl;
                      }
                  }
              }
            else
              {
                result<<"Diff pour face CL : "<<face*size1+comp<<endl;
                result<<"Maximum : "<<max<<endl;
              }

            if (Process::je_suis_maitre()) incoV[face*size1+comp]=0.;
            inco.echange_espace_virtuel();
          }//fin autres CL
    }//fin du for sur n_bord
  exit();
}

void Op_Diff_VEFP1NCP1B_Face::corriger_Cl_test(DoubleTab& resu) const
{
  const Zone_Cl_VEF& zone_Cl_VEF=la_zcl_vef.valeur();
  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();

  const int nb_bords =les_cl.size();
  int n_bord=0, num1=0, num2=0;
  int face=0, face_associee=0;
  int ind_face=0, comp=0;

  for (n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

      num1=0;
      num2=le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              face = le_bord.num_face(ind_face);
              face_associee=
                le_bord.num_face(la_cl_perio.face_associee(ind_face));

              if (face<face_associee)
                for (comp=0; comp<dim_ch_; comp++)
                  {
                    resu[face*dim_ch_+comp]+=resu[face_associee*dim_ch_+comp];
                    resu[face_associee*dim_ch_+comp]=resu[face*dim_ch_+comp];
                  }
            }
        }
    }
  resu.echange_espace_virtuel();
}
