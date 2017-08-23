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
// File:        Op_Conv_Muscl_New_VEF_Face.cpp
// Directory:   $TRUST_ROOT/src/VEF/Operateurs
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Op_Conv_Muscl_New_VEF_Face.h>
#include <Champ_P1NC.h>
#include <BilanQdmVEF.h>
#include <Schema_Temps_base.h>
#include <Debog.h>
#include <Porosites_champ.h>
#include <Sous_zone_VF.h>
#include <Probleme_base.h>
#include <ArrOfBit.h>
#include <IntVects.h>
#include <SFichier.h>
#include <IntList.h>
#include <DoubleTabs.h>
#include <stat_counters.h>

/*
  static inline double vanleer(double r)
{
  if(r<=0.) return 0.;
  return 2.*r/(1.+r);//VanLeer
}
*/
static inline double minmod(double r)
{
  if (r<=0.) return 0.;
  if (r<=1.) return r;
  else return 1.;
}
/*
static inline double superbee(double r)
{
  if (r<=0.) return 0.;
  if (r<=.5) return 2.*r;
  if (r<=1.) return 1;
  if (r<=2) return r;
  else return 2.;
}
*/
static inline double optimum(double a, double b)
{
  if (a>=0.)
    if (b>=0.)
      return (a<=b)?a:b;
    else
      return 0.;
  else if (b>=0.)
    return 0.;
  else
    return (a>=b)?a:b;
}

Implemente_instanciable(Op_Conv_Muscl_New_VEF_Face,"Op_Conv_Muscl_New_VEF_P1NC",Op_Conv_VEF_Face);


//// printOn
//

Sortie& Op_Conv_Muscl_New_VEF_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je();
}

//// readOn
//

Entree& Op_Conv_Muscl_New_VEF_Face::readOn(Entree& s )
{
  //Les mots a reconnaitre
  Motcle motlu, accouverte = "{" , accfermee = "}" ;
  Motcle limiteur;
  Motcles les_mots(8);
  {
    les_mots[0] = "centre";
    les_mots[1] = "amont";
    les_mots[2] = "stab";
    les_mots[3] = "alpha";
    les_mots[4] = "centre_old";
    les_mots[5] = "version";
    les_mots[6] = "limiteur";
    les_mots[7] = "facsec_auto";
  }
  Motcles les_mots2(3);
  {
    les_mots2[0] = "vanleer";
    les_mots2[1] = "minmod";
    les_mots2[2] = "superbee";
  }

  //Les variables a instancier
  alpha_=1.;
  centered_=1;
  upwind_=1;
  stabilized_=1;
  old_centered_=0;
  version_=2;
  limiteur_=&minmod;
  facsec_auto_=0;
  max_limiteur_=1.;

  //   Temporaire
  //   s >> motlu;
  //   if (motlu!=accouverte)
  //     {
  //       if (Process::je_suis_maitre())
  //       {
  //          Cerr << "Erreur Op_Conv_Muscl_New_VEF_Face::readOn()" << finl;
  //          Cerr << "Depuis la 1.6.1, la syntaxe du mot cle MUSCL a change." << finl;
  //          Cerr << "Il faut commencer par une accolade ouvrante {" << finl;
  //          Cerr << "et les options eventuelles sont entre les accolades:" << finl;
  //          Cerr << "Convection { muscl } -> Convection { muscl { } }" << finl;

  //       }
  //       Process::exit();
  //     }
  //   s >> motlu;

  //   while(motlu!=accfermee)
  //     {
  //       int rang=les_mots.search(motlu);

  //       switch(rang)
  //         {
  //         case 0 :

  //           centered_=1;
  //           upwind_=0;
  //           stabilized_=0;
  //           break;

  //         case 1 :

  //           centered_=1;
  //           upwind_=1;
  //           stabilized_=0;
  //           break;

  //         case 2 :

  //           centered_=1;
  //           upwind_=1;
  //           stabilized_=1;
  //           break;

  //         case 3 :

  //           s >> alpha_;
  //           break;

  //         case 4 :

  //           centered_=1;
  //           upwind_=0;
  //           stabilized_=0;
  //           old_centered_=1;
  //           break;

  //         case 5 :

  //           s>>version_;
  //           break;

  //         case 6 :

  //           s>>limiteur;

  //           int rang2=les_mots2.search(limiteur);

  //           switch(rang2)
  //             {
  //             case 0 :

  //               limiteur_=&vanleer;
  //            max_limiteur_=2.;
  //               break;

  //             case 1 :

  //               limiteur_=&minmod;
  //            max_limiteur_=1.;
  //               break;

  //             case 2 :

  //               limiteur_=&superbee;
  //            max_limiteur_=2.;
  //               break;

  //             default :

  //               if (Process::je_suis_maitre())
  //                 {
  //                   Cerr << "Erreur Op_Conv_Muscl_New_VEF_Face::readOn()" << finl;
  //                   Cerr << "Limiteur "<<limiteur<<" non code" << finl;
  //                   Cerr << "Sortie du programme" << finl;
  //                 }
  //               Process::exit();
  //               break;
  //             }

  //           break;

  //         case 7 :

  //         s>>facsec_auto_;
  //         break;

  //         default :

  //           if (Process::je_suis_maitre())
  //             {
  //               Cerr << "Erreur Op_Conv_Muscl_New_VEF_Face::readOn()" << finl;
  //               Cerr << "Mot clef " << motlu << " non connu." << finl;
  //               Cerr << "Sortie du programme." << finl;
  //             }
  //           Process::exit();
  //           break;

  //         }//fin du switch

  //       //Suite de la lecture
  //       s >> motlu;

  //     }//fin du while
  //   Fin du temporaire


  return s;
}

/*
 static inline int maximum(const int& i, const int& j)
{
  if (i<j) return j;
  return i;
}
*/

/*
 static inline double maximum(const double& x,
                             const double& y)
{
  if(x<y)
    return y;
  return x;
}
*/
/*
static inline double maximum(const double& x,
                             const double& y,
                             const double& z)
{
  return maximum(maximum(x,y),z);
}
*/
/*
static inline double minimum(const double& x,
                             const double& y)
{
  if(x>y)
    return y;
  return x;
}
*/
/*
static inline double minimum(const double& x,
                             const double& y,
                             const double& z)
{
  return minimum(minimum(x,y),z);
}
*/
/*
  static inline double Dij(int elem,
                         int face_loc_i,
                         int face_loc_j,
                         const DoubleTab& Kij)
{
  const double& kij=Kij(elem,face_loc_i,face_loc_j);
  const double& kji=Kij(elem,face_loc_j,face_loc_i);
  return maximum(-kij,-kji,0);
}
*/

////////////////////////////////////////////////////////////////////
//
//                      Implementation des fonctions
//
//                   de la classe Op_Conv_Muscl_New_VEF_Face
//
////////////////////////////////////////////////////////////////////

void Op_Conv_Muscl_New_VEF_Face::calculer_coefficients_operateur_centre(DoubleTab& Kij, DoubleTab& Cij, DoubleTab& Sij, DoubleTab& Sij2, const int& nb_comp, const DoubleTab& velocity) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  const DoubleTab& facette_normales = zone_VEF.facette_normales();
  const DoubleTab& facette_normales_Cl = zone_Cl_VEF.normales_facettes_Cl();
  const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();

  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.dimension(1);
  const int nfa7 = zone_VEF.type_elem().nb_facette();
  const int nb_som_elem = zone_VEF.zone().nb_som_elem();
  const int marq = phi_u_transportant(equation());

  int rang=-2;
  int fa7=0;
  int dim=-1;
  int itypcl=-2;
  int elem=0;
  int facei_loc=-1;
  int facej_loc=-1;

  double psc=0.;
  double porosite=0.;

  ArrOfDouble vc(Objet_U::dimension);
  ArrOfDouble vs(Objet_U::dimension);
  DoubleTab vsom(nb_som_elem,Objet_U::dimension);

  ArrOfInt face(Objet_U::dimension+1);

  assert(Kij.nb_dim()==3);
  assert(Kij.dimension(0)==nb_elem_tot);
  assert(Kij.dimension(1)==Kij.dimension(2));
  assert(Kij.dimension(1)==nb_faces_elem);

  assert(Cij.nb_dim()==2);
  assert(Cij.dimension(0)==nb_elem_tot);
  assert(Cij.dimension(1)==nfa7);

  assert(Sij.nb_dim()==2);
  assert(Sij.dimension(0)==nb_elem_tot);
  assert(Sij.dimension(1)==nfa7);

  if (Objet_U::dimension==3)
    {
      assert(Sij2.nb_dim()==2);
      assert(Sij2.dimension(0)==nb_elem_tot);
      assert(Sij2.dimension(1)==nfa7);
    }

  //
  //Calcul des coefficients de l'operateur
  //
  for(elem=0; elem<nb_elem_tot; elem++)
    {
      rang=rang_elem_non_std(elem);
      itypcl=(rang==-1)?0:zone_Cl_VEF.type_elem_Cl(rang);

      for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
        face(facei_loc)=elem_faces(elem,facei_loc);

      vs=0.;
      for (dim=0; dim<Objet_U::dimension; dim++)
        for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
          vs(dim)+=velocity(face(facei_loc),dim);

      vsom=0;
      for (dim=0; dim<Objet_U::dimension; dim++)
        for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
          vsom(facei_loc,dim)=vs(dim)-Objet_U::dimension*velocity(face(facei_loc),dim);

      vc=0.;
      zone_VEF.type_elem().valeur().calcul_vc(face,vc,vs,vsom,(*this).vitesse(),itypcl,porosite_face);
      if (marq==0)
        {
          porosite=1./porosite_elem(elem);

          vsom*=porosite;
          vs*=porosite;
          vc*=porosite;
        }

      if (Objet_U::dimension==2)
        {
          if (rang==-1)
            for (fa7=0; fa7<nfa7; fa7++)
              {
                facei_loc=KEL(0,fa7);
                facej_loc=KEL(1,fa7);

                Cij(elem,fa7)=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  Cij(elem,fa7)+=vc(dim)*facette_normales(elem,fa7,dim);

                Sij(elem,fa7)=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  Sij(elem,fa7)+=vsom(KEL(2,fa7),dim)*facette_normales(elem,fa7,dim);

                psc=Cij(elem,fa7)+Sij(elem,fa7);
                psc*=0.5;

                Kij(elem,facei_loc,facej_loc)=psc;
                Kij(elem,facei_loc,facei_loc)-=psc;//pour l'aspect LED
                Kij(elem,facej_loc,facei_loc)=-psc;
                Kij(elem,facej_loc,facej_loc)+=psc;//pour l'aspect LED
              }
          else //rang!=-1
            for (fa7=0; fa7<nfa7; fa7++)
              {
                facei_loc=KEL(0,fa7);
                facej_loc=KEL(1,fa7);

                Cij(elem,fa7)=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  Cij(elem,fa7)+=vc(dim)*facette_normales_Cl(rang,fa7,dim);

                Sij(elem,fa7)=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  Sij(elem,fa7)+=vsom(KEL(2,fa7),dim)*facette_normales_Cl(rang,fa7,dim);

                psc=Cij(elem,fa7)+Sij(elem,fa7);
                psc*=0.5;

                Kij(elem,facei_loc,facej_loc)=psc;
                Kij(elem,facei_loc,facei_loc)-=psc;//pour l'aspect LED
                Kij(elem,facej_loc,facei_loc)=-psc;
                Kij(elem,facej_loc,facej_loc)+=psc;//pour l'aspect LED
              }
        }//fin Objet_U::dimension==2
      else //Objet_U::dimension==3
        {
          if (rang==-1)
            for (fa7=0; fa7<nfa7; fa7++)
              {
                facei_loc=KEL(0,fa7);
                facej_loc=KEL(1,fa7);

                Cij(elem,fa7)=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  Cij(elem,fa7)+=vc(dim)*facette_normales(elem,fa7,dim);

                Sij(elem,fa7)=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  Sij(elem,fa7)+=vsom(KEL(2,fa7),dim)*facette_normales(elem,fa7,dim);

                Sij2(elem,fa7)=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  Sij2(elem,fa7)+=vsom(KEL(3,fa7),dim)*facette_normales(elem,fa7,dim);

                psc=Cij(elem,fa7)+Sij(elem,fa7)+Sij2(elem,fa7);
                psc/=3.;

                Kij(elem,facei_loc,facej_loc)=psc;
                Kij(elem,facei_loc,facei_loc)-=psc;//pour l'aspect LED
                Kij(elem,facej_loc,facei_loc)=-psc;
                Kij(elem,facej_loc,facej_loc)+=psc;//pour l'aspect LED
              }
          else //rang!=-1
            for (fa7=0; fa7<nfa7; fa7++)
              {
                facei_loc=KEL(0,fa7);
                facej_loc=KEL(1,fa7);

                Cij(elem,fa7)=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  Cij(elem,fa7)+=vc(dim)*facette_normales_Cl(rang,fa7,dim);

                Sij(elem,fa7)=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  Sij(elem,fa7)+=vsom(KEL(2,fa7),dim)*facette_normales_Cl(rang,fa7,dim);

                Sij2(elem,fa7)=0.;
                for (dim=0; dim<Objet_U::dimension; dim++)
                  Sij2(elem,fa7)+=vsom(KEL(3,fa7),dim)*facette_normales_Cl(rang,fa7,dim);

                psc=Cij(elem,fa7)+Sij(elem,fa7)+Sij2(elem,fa7);
                psc/=3.;

                Kij(elem,facei_loc,facej_loc)=psc;
                Kij(elem,facei_loc,facei_loc)-=psc;//pour l'aspect LED
                Kij(elem,facej_loc,facei_loc)=-psc;
                Kij(elem,facej_loc,facej_loc)+=psc;//pour l'aspect LED
              }
        }//fin Objet_U::dimension==3
    }

  //
  // Correction des Kij pour Dirichlet !
  //
  {
    int nb_bord=zone_Cl_VEF.nb_cond_lim();

    for (int n_bord=0; n_bord<nb_bord; n_bord++)
      {
        const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

        if ( (sub_type(Dirichlet,la_cl.valeur()))
             || (sub_type(Dirichlet_homogene,la_cl.valeur()))
           )
          {
            //On ne fait rien
          }// sub_type Dirichlet


        else if (sub_type(Neumann,la_cl.valeur()) || sub_type(Neumann_homogene,la_cl.valeur()))
          {
            //On ne fait rien
          }//fin du if sur Neumann

        else if (sub_type(Symetrie,la_cl.valeur()))
          {
            //On ne fait rien
          }//fin du if sur Symetrie

        else if (sub_type(Periodique,la_cl.valeur()))
          {
            //On ne fait rien
          }//fin du if sur Periodique

        else if (sub_type(Echange_impose_base,la_cl.valeur()))
          {
            //On ne fait rien
          }//fin du if sur Echange_impose_base

        else
          {
            if (Process::je_suis_maitre())
              {
                Cerr << "Erreur Op_Conv_Muscl_New_VEF_Face::calculer_coefficients_operateur_centre()" << finl;
                Cerr << "Condition aux limites " << la_cl.que_suis_je() << " non codee."   << finl;
                Cerr << "Sortie du programme." << finl;
              }
            Process::exit();
          }//fin du else sur les autres conditions aux limites

      }//fin des conditions aux limites
  }

  //
  // Fin de la correction des Kij
  //
}


void Op_Conv_Muscl_New_VEF_Face::
calculer_flux_operateur_centre(DoubleTab& Fij,const DoubleTab& Kij,const DoubleTab& Cij, const DoubleTab& Sij, const DoubleTab& Sij2, const int& nb_comp, const DoubleTab& velocity, const DoubleTab& transporte) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
  const Domaine& domaine = zone_VEF.zone().domaine();

  const DoubleTab& vecteur_face_facette = ref_cast_non_const(Zone_VEF,zone_VEF).vecteur_face_facette();
  const DoubleTab& vecteur_face_facette_Cl = zone_Cl_VEF.vecteur_face_facette_Cl();
  const DoubleTab& coord_sommets = domaine.coord_sommets();
  const DoubleTab& xv = zone_VEF.xv();

  const DoubleVect& transporteVect=ref_cast(DoubleVect,transporte);

  const IntTab& elem_faces = zone_VEF.elem_faces();
  //   const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();

  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.dimension(1);
  const int nfa7 = zone_VEF.type_elem().nb_facette();

  int rang=-2;
  int fa7=0;
  int dim=-1;
  int elem=0;
  int face_amont=-1;
  int dir=-1;
  int comp=-1;
  int s=-1;
  int s2=-1;
  int facei=-1;
  int facej=-1;
  int facei_loc=-1;
  int facej_loc=-1;

  double inco_m=0.;
  double inco_s=0.;
  double inco_s2=0.;
  double inco_c=0.;
  double flux=0.;

  DoubleTab gradient_elem(nb_elem_tot,nb_comp,Objet_U::dimension);  //!< (du/dx du/dy dv/dx dv/dy) pour un poly  gradient_elem=0.;
  IntTab face(Objet_U::dimension+1);

  assert(Fij.nb_dim()==4);
  assert(Fij.dimension(0)==nb_elem_tot);
  assert(Fij.dimension(1)==Fij.dimension(2));
  assert(Fij.dimension(1)==nb_faces_elem);
  assert(Fij.dimension(3)==nb_comp);

  //
  //Calcul des flux de l'operateur
  //
  Champ_P1NC::calcul_gradient(transporte,gradient_elem,zone_Cl_VEF);

  for(elem=0; elem<nb_elem_tot; elem++)
    {
      rang=rang_elem_non_std(elem);

      for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
        face(facei_loc)=elem_faces(elem,facei_loc);

      if (Objet_U::dimension==2)
        {
          if (rang==-1)
            for (fa7=0; fa7<nfa7; fa7++)
              {
                facei_loc=KEL(0,fa7);
                facej_loc=KEL(1,fa7);

                facei=face(facei_loc);
                facej=face(facej_loc);

                //REMARQUE : ON N'OUBLIE PAS QUE LES NORMALES FA7 SONT ORIENTeES DE FACE_LOCI VERS FACE_LOCJ PAR DEFAUT
                //LES PRODUITS SCALAIRES PSC_M, PSC_C, PSC_S SONT DONC CALCULeS POUR OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCI
                //ET EMIS PAR LA FACE_LOCJ
                //SI L'ON VEUT OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCJ ET EMIS PAR LA FACE_LOCI, IL FAUT MULTIPLIER LES
                //PRODUITS SCALAIRES PSC_M, PSC_C, PSC_S PAR -1 POUR IMPLICITEMENT ReORIENTER LES NORMALES FA7 DE FACE_LOCJ VERS
                //FACE_LOCI
                //MEME REMARQUE POUR LE SCALAIRE "FLUX"
                const double& psc_m = Kij(elem,facei_loc,facej_loc);
                const double& psc_c = Cij(elem,fa7);
                const double& psc_s = Sij(elem,fa7);

                if (psc_m>=0.)
                  face_amont=facei,dir=0;
                else
                  face_amont=facej,dir=1;

                s=zone_VEF.zone().sommet_elem(elem,KEL(2,fa7));

                for (comp=0; comp<nb_comp; comp++)
                  {
                    //Calcul de la valeur de l'inconnue aux points d'integration de la formule de Simspon
                    inco_m=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_m+=gradient_elem(elem,comp,dim)*vecteur_face_facette(elem,fa7,dim,dir);

                    inco_s=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_s+=gradient_elem(elem,comp,dim)*(coord_sommets(s,dim)-xv(face_amont,dim));

                    inco_c=2.*inco_m-inco_s;

                    //Calcul du flux final : formule d'integration de Simpson
                    flux=inco_c*psc_c;
                    flux+=4*inco_m*psc_m;
                    flux+=inco_s*psc_s;
                    flux/=6.;

                    //Fij(elem,facei_loc,facej_loc,comp) est le flux recu par la facei_loc dans elem
                    //envoye par la face facej_loc dans elem pour la composante comp de l'inconnue
                    Fij(elem,facei_loc,facej_loc,comp)=flux-psc_m*transporteVect[facei*nb_comp+comp];
                    Fij(elem,facej_loc,facei_loc,comp)=psc_m*transporteVect[facej*nb_comp+comp]-flux;
                  }
              }
          else //rang!=-1
            for (fa7=0; fa7<nfa7; fa7++)
              {
                facei_loc=KEL(0,fa7);
                facej_loc=KEL(1,fa7);

                facei=face(facei_loc);
                facej=face(facej_loc);

                //REMARQUE : ON N'OUBLIE PAS QUE LES NORMALES FA7 SONT ORIENTeES DE FACE_LOCI VERS FACE_LOCJ PAR DEFAUT
                //LES PRODUITS SCALAIRES PSC_M, PSC_C, PSC_S SONT DONC CALCULeS POUR OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCI
                //ET EMIS PAR LA FACE_LOCJ
                //SI L'ON VEUT OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCJ ET EMIS PAR LA FACE_LOCI, IL FAUT MULTIPLIER LES
                //PRODUITS SCALAIRES PSC_M, PSC_C, PSC_S PAR -1 POUR IMPLICITEMENT ReORIENTER LES NORMALES FA7 DE FACE_LOCJ VERS
                //FACE_LOCI
                //MEME REMARQUE POUR LE SCALAIRE "FLUX"
                const double& psc_m = Kij(elem,facei_loc,facej_loc);
                const double& psc_c = Cij(elem,fa7);
                const double& psc_s = Sij(elem,fa7);

                if (psc_m>=0.)
                  face_amont=facei,dir=0;
                else
                  face_amont=facej,dir=1;

                s=zone_VEF.zone().sommet_elem(elem,KEL(2,fa7));

                for (comp=0; comp<nb_comp; comp++)
                  {
                    //Calcul de la valeur de l'inconnue aux points d'integration de la formule de Simspon
                    inco_m=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_m+=gradient_elem(elem,comp,dim)*vecteur_face_facette_Cl(rang,fa7,dim,dir);

                    inco_s=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_s+=gradient_elem(elem,comp,dim)*(coord_sommets(s,dim)-xv(face_amont,dim));

                    inco_c=2.*inco_m-inco_s;

                    //Calcul du flux final : formule d'integration de Simpson
                    flux=inco_c*psc_c;
                    flux+=4*inco_m*psc_m;
                    flux+=inco_s*psc_s;
                    flux/=6.;

                    //Fij(elem,facei_loc,facej_loc,comp) est le flux recu par la facei_loc dans elem
                    //envoye par la face facej_loc dans elem pour la composante comp de l'inconnue
                    Fij(elem,facei_loc,facej_loc,comp)=flux-psc_m*transporteVect[facei*nb_comp+comp];
                    Fij(elem,facej_loc,facei_loc,comp)=psc_m*transporteVect[facej*nb_comp+comp]-flux;
                  }
              }
        }//fin Objet_U::dimension==2
      else //Objet_U::dimension==3
        {
          if (rang==-1)
            for (fa7=0; fa7<nfa7; fa7++)
              {
                facei_loc=KEL(0,fa7);
                facej_loc=KEL(1,fa7);

                facei=face(facei_loc);
                facej=face(facej_loc);

                //REMARQUE : ON N'OUBLIE PAS QUE LES NORMALES FA7 SONT ORIENTeES DE FACE_LOCI VERS FACE_LOCJ PAR DEFAUT
                //LES PRODUITS SCALAIRES PSC_M, PSC_C, PSC_S, PSC_S2 SONT DONC CALCULeS POUR OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCI
                //ET EMIS PAR LA FACE_LOCJ
                //SI L'ON VEUT OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCJ ET EMIS PAR LA FACE_LOCI, IL FAUT MULTIPLIER LES
                //PRODUITS SCALAIRES PSC_M, PSC_C, PSC_S, PSC_S2 PAR -1 POUR IMPLICITEMENT ReORIENTER LES NORMALES FA7 DE FACE_LOCJ VERS
                //FACE_LOCI
                //MEME REMARQUE POUR LE SCALAIRE "FLUX"
                const double& psc_c = Cij(elem,fa7);
                const double& psc_s = Sij(elem,fa7);
                const double& psc_s2 = Sij2(elem,fa7);
                const double& psc_m = Kij(elem,facei_loc,facej_loc);

                if (psc_m>=0.)
                  face_amont=facei,dir=0;
                else
                  face_amont=facej,dir=1;

                s=zone_VEF.zone().sommet_elem(elem,KEL(2,fa7));
                s2=zone_VEF.zone().sommet_elem(elem,KEL(3,fa7));

                for (comp=0; comp<nb_comp; comp++)
                  {
                    //Calcul de la valeur de l'inconnue aux points d'integration de la formule 3D
                    inco_m=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_m+=gradient_elem(elem,comp,dim)*vecteur_face_facette(elem,fa7,dim,dir);

                    inco_s=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_s+=gradient_elem(elem,comp,dim)*(coord_sommets(s,dim)-xv(face_amont,dim));

                    inco_s2=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_s2+=gradient_elem(elem,comp,dim)*(coord_sommets(s2,dim)-xv(face_amont,dim));

                    inco_c=3.*inco_m-inco_s-inco_s2;

                    //Calcul du flux final : formule d'integration 3D exacte pour les polynomes de degre 2
                    flux=(inco_s+inco_s2)*(psc_s+psc_s2);
                    flux+=(inco_s+inco_c)*(psc_s+psc_c);
                    flux+=(inco_s2+inco_c)*(psc_s2+psc_c);
                    flux/=12.;

                    //Fij(elem,facei_loc,facej_loc,comp) est le flux recu par la facei_loc dans elem
                    //envoye par la face facej_loc dans elem pour la composante comp de l'inconnue
                    Fij(elem,facei_loc,facej_loc,comp)=flux-psc_m*transporteVect[facei*nb_comp+comp];
                    Fij(elem,facej_loc,facei_loc,comp)=psc_m*transporteVect[facej*nb_comp+comp]-flux;
                  }
              }
          else //rang!=-1
            for (fa7=0; fa7<nfa7; fa7++)
              {
                facei_loc=KEL(0,fa7);
                facej_loc=KEL(1,fa7);

                facei=face(facei_loc);
                facej=face(facej_loc);

                //REMARQUE : ON N'OUBLIE PAS QUE LES NORMALES FA7 SONT ORIENTeES DE FACE_LOCI VERS FACE_LOCJ PAR DEFAUT
                //LES PRODUITS SCALAIRES PSC_M, PSC_C, PSC_S, PSC_S2 SONT DONC CALCULeS POUR OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCI
                //ET EMIS PAR LA FACE_LOCJ
                //SI L'ON VEUT OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCJ ET EMIS PAR LA FACE_LOCI, IL FAUT MULTIPLIER LES
                //PRODUITS SCALAIRES PSC_M, PSC_C, PSC_S, PSC_S2 PAR -1 POUR IMPLICITEMENT ReORIENTER LES NORMALES FA7 DE FACE_LOCJ VERS
                //FACE_LOCI
                //MEME REMARQUE POUR LE SCALAIRE "FLUX"
                const double& psc_c = Cij(elem,fa7);
                const double& psc_s = Sij(elem,fa7);
                const double& psc_s2 = Sij2(elem,fa7);
                const double& psc_m = Kij(elem,facei_loc,facej_loc);

                if (psc_m>=0.)
                  face_amont=facei,dir=0;
                else
                  face_amont=facej,dir=1;

                s=zone_VEF.zone().sommet_elem(elem,KEL(2,fa7));
                s2=zone_VEF.zone().sommet_elem(elem,KEL(3,fa7));

                for (comp=0; comp<nb_comp; comp++)
                  {
                    //Calcul de la valeur de l'inconnue aux points d'integration de la formule 3D
                    inco_m=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_m+=gradient_elem(elem,comp,dim)*vecteur_face_facette_Cl(rang,fa7,dim,dir);

                    inco_s=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_s+=gradient_elem(elem,comp,dim)*(coord_sommets(s,dim)-xv(face_amont,dim));

                    inco_s2=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_s2+=gradient_elem(elem,comp,dim)*(coord_sommets(s2,dim)-xv(face_amont,dim));

                    inco_c=3.*inco_m-inco_s-inco_s2;

                    //Calcul du flux final : formule d'integration 3D exacte pour les polynomes de degre 2
                    flux=(inco_s+inco_s2)*(psc_s+psc_s2);
                    flux+=(inco_s+inco_c)*(psc_s+psc_c);
                    flux+=(inco_s2+inco_c)*(psc_s2+psc_c);
                    flux/=12.;

                    //Fij(elem,facei_loc,facej_loc,comp) est le flux recu par la facei_loc dans elem
                    //envoye par la face facej_loc dans elem pour la composante comp de l'inconnue
                    Fij(elem,facei_loc,facej_loc,comp)=flux-psc_m*transporteVect[facei*nb_comp+comp];
                    Fij(elem,facej_loc,facei_loc,comp)=psc_m*transporteVect[facej*nb_comp+comp]-flux;
                  }
              }
        }//fin Objet_U::dimension==3
    }
}

void Op_Conv_Muscl_New_VEF_Face::
modifier_flux_operateur_centre(DoubleTab& Fij,const DoubleTab& Kij,const DoubleTab& Cij, const DoubleTab& Sij, const DoubleTab& Sij2, const int& nb_comp, const DoubleTab& velocity, const DoubleTab& transporte) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  const DoubleTab& vecteur_face_facette = ref_cast_non_const(Zone_VEF,zone_VEF).vecteur_face_facette();
  const DoubleTab& vecteur_face_facette_Cl = zone_Cl_VEF.vecteur_face_facette_Cl();

  const DoubleVect& transporteVect=ref_cast(DoubleVect,transporte);

  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();

  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nb_faces_elem=elem_faces.dimension(1);
  const int nfa7 = zone_VEF.type_elem().nb_facette();

  int rang=-2;
  int fa7=0;
  int dim=-1;
  int elem=0;
  int face_amont=-1;
  int dir=-1;
  int comp=-1;
  int facei=-1;
  int facej=-1;
  int facei_loc=-1;
  int facej_loc=-1;

  double inco_m=0.;
  double flux=0.;

  DoubleTab gradient_elem(nb_elem_tot,nb_comp,Objet_U::dimension);  //!< (du/dx du/dy dv/dx dv/dy) pour un poly  gradient_elem=0.;
  IntTab face(Objet_U::dimension+1);

  assert(Fij.nb_dim()==4);
  assert(Fij.dimension(0)==nb_elem_tot);
  assert(Fij.dimension(1)==Fij.dimension(2));
  assert(Fij.dimension(1)==nb_faces_elem);
  assert(Fij.dimension(3)==nb_comp);

  //
  //Calcul des flux de l'operateur
  //
  Champ_P1NC::calcul_gradient(transporte,gradient_elem,zone_Cl_VEF);

  for(elem=0; elem<nb_elem_tot; elem++)
    {
      rang=rang_elem_non_std(elem);

      for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
        face(facei_loc)=elem_faces(elem,facei_loc);

      //RAZ des flux des elements a modifier
      if (is_element_for_upwinding_(elem))
        for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
          for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
            for (comp=0; comp<nb_comp; comp++)
              Fij(elem,facei_loc,facej_loc,comp)=0.;

      if (is_element_for_upwinding_(elem))
        {
          if (rang==-1)
            for (fa7=0; fa7<nfa7; fa7++)
              {
                facei_loc=KEL(0,fa7);
                facej_loc=KEL(1,fa7);

                facei=face(facei_loc);
                facej=face(facej_loc);

                //REMARQUE : ON N'OUBLIE PAS QUE LES NORMALES FA7 SONT ORIENTeES DE FACE_LOCI VERS FACE_LOCJ PAR DEFAUT
                //LE PRODUIT SCALAIRE PSC_M EST DONC CALCULe POUR OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCI
                //ET EMIS PAR LA FACE_LOCJ
                //SI L'ON VEUT OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCJ ET EMIS PAR LA FACE_LOCI, IL FAUT MULTIPLIER LE
                //PRODUIT SCALAIRE PSC_M PAR -1 POUR IMPLICITEMENT ReORIENTER LES NORMALES FA7 DE FACE_LOCJ VERS
                //FACE_LOCI
                //MEME REMARQUE POUR LE SCALAIRE "FLUX"
                const double& psc_m = Kij(elem,facei_loc,facej_loc);

                if (psc_m>=0.)
                  face_amont=facei,dir=0;
                else
                  face_amont=facej,dir=1;

                for (comp=0; comp<nb_comp; comp++)
                  {
                    //Calcul de la valeur de l'inconnue aux points d'integration de la formule du point milieu
                    inco_m=transporteVect[face_amont*nb_comp+comp];
                    for(dim=0; dim<Objet_U::dimension; dim++)
                      inco_m+=gradient_elem(elem,comp,dim)*vecteur_face_facette(elem,fa7,dim,dir);

                    //Calcul du flux final : formule du point milieu
                    flux=inco_m*psc_m;

                    //Fij(elem,facei_loc,facej_loc,comp) est le flux recu par la facei_loc dans elem
                    //envoye par la face facej_loc dans elem pour la composante comp de l'inconnue
                    Fij(elem,facei_loc,facej_loc,comp)=flux-psc_m*transporteVect[facei*nb_comp+comp];
                    Fij(elem,facej_loc,facei_loc,comp)=psc_m*transporteVect[facej*nb_comp+comp]-flux;
                  }
              }
        }
      else //rang!=-1
        {
          for (fa7=0; fa7<nfa7; fa7++)
            {
              facei_loc=KEL(0,fa7);
              facej_loc=KEL(1,fa7);

              facei=face(facei_loc);
              facej=face(facej_loc);

              //REMARQUE : ON N'OUBLIE PAS QUE LES NORMALES FA7 SONT ORIENTeES DE FACE_LOCI VERS FACE_LOCJ PAR DEFAUT
              //LE PRODUIT SCALAIRE PSC_M EST DONC CALCULe POUR OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCI
              //ET EMIS PAR LA FACE_LOCJ
              //SI L'ON VEUT OBTENIR LE FLUX REcU PAR LA FACE FACE_LOCJ ET EMIS PAR LA FACE_LOCI, IL FAUT MULTIPLIER LE
              //PRODUIT SCALAIRE PSC_M PAR -1 POUR IMPLICITEMENT ReORIENTER LES NORMALES FA7 DE FACE_LOCJ VERS
              //FACE_LOCI
              //MEME REMARQUE POUR LE SCALAIRE "FLUX"
              const double& psc_m = Kij(elem,facei_loc,facej_loc);

              if (psc_m>=0.)
                face_amont=facei,dir=0;
              else
                face_amont=facej,dir=1;

              for (comp=0; comp<nb_comp; comp++)
                {
                  //Calcul de la valeur de l'inconnue aux points d'integration de la formule du point milieu
                  inco_m=transporteVect[face_amont*nb_comp+comp];
                  for(dim=0; dim<Objet_U::dimension; dim++)
                    inco_m+=gradient_elem(elem,comp,dim)*vecteur_face_facette_Cl(rang,fa7,dim,dir);

                  //Calcul du flux final : formule d'integration du point milieu
                  flux=inco_m*psc_m;

                  //Fij(elem,facei_loc,facej_loc,comp) est le flux recu par la facei_loc dans elem
                  //envoye par la face facej_loc dans elem pour la composante comp de l'inconnue
                  Fij(elem,facei_loc,facej_loc,comp)=flux-psc_m*transporteVect[facei*nb_comp+comp];
                  Fij(elem,facej_loc,facei_loc,comp)=psc_m*transporteVect[facej*nb_comp+comp]-flux;
                }
            }
        }
    }//for sur elem
}

void Op_Conv_Muscl_New_VEF_Face::remplir_fluent(DoubleVect& fluent_) const
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_Inc_base& la_vitesse=vitesse_.valeur();
  const DoubleTab& velocity=la_vitesse.valeurs();
  const DoubleTab& face_normales=zone_VEF.face_normales();
  const int nb_faces = zone_VEF.nb_faces();
  // calcul de la CFL.
  double psc;
  // On remet a zero le tableau qui sert pour
  // le calcul du pas de temps de stabilite
  fluent_ = 0;

  for(int num_face=0; num_face<nb_faces; num_face++)
    {
      psc=0.;
      for (int i=0; i<dimension; i++)
        psc+=velocity(num_face,i)*face_normales(num_face,i);
      fluent_(num_face)=dabs(psc);
    }
}


DoubleTab& Op_Conv_Muscl_New_VEF_Face::ajouter(const DoubleTab& transporte_2,
                                               DoubleTab& resu) const
{
  DoubleTab sauv(resu);
  resu=0;

  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Champ_P1NC& la_vitesse=ref_cast( Champ_P1NC, vitesse_.valeur());
  const DoubleTab& vitesse_2=la_vitesse.valeurs();
  const DoubleVect& porosite_face = zone_VEF.porosite_face();

  const IntTab& elem_faces = zone_VEF.elem_faces();

  const int marq=phi_u_transportant(equation());
  const int nb_faces_elem=elem_faces.dimension(1);
  assert(nb_faces_elem==(dimension+1));
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nfa7 = zone_VEF.type_elem().nb_facette();

  int nb_comp=1;
  if(resu.nb_dim()!=1)
    nb_comp=resu.dimension(1);

  DoubleTab transporte_;
  DoubleTab vitesse_face_;
  // soit on a transporte=phi*transporte_ et vitesse=vitesse_
  // soit transporte=transporte_ et vitesse=phi*vitesse_
  // cela depend si on transporte avec phi u ou avec u.
  const DoubleTab& velocity=modif_par_porosite_si_flag(vitesse_2,vitesse_face_,marq,porosite_face);

  DoubleTab Kij(nb_elem_tot,nb_faces_elem,nb_faces_elem);
  DoubleTab Fij(nb_elem_tot,nb_faces_elem,nb_faces_elem,nb_comp);
  DoubleTab Cij(nb_elem_tot,nfa7);
  DoubleTab Sij(nb_elem_tot,nfa7);
  DoubleTab Sij2;
  if (Objet_U::dimension==3)
    {
      Sij2.resize(nb_elem_tot,nfa7);
      Sij2=0.;
    }

  //Pour tenir compte des conditions de Neumann sortie libre

  // soit on a transporte=phi*transporte_ et vitesse=vitesse_
  // soit transporte=transporte_ et vitesse=phi*vitesse_
  // cela depend si on transporte avec phi u ou avec u.
  const DoubleTab& transporte=modif_par_porosite_si_flag(transporte_2,transporte_,!marq,porosite_face);

  //Initialisation du tableau flux_bords_ pour le calcul des pertes de charge
  flux_bords_.resize(zone_VEF.nb_faces_bord(),nb_comp);
  calculer_flux_bords(Kij,velocity,transporte);


  //statistiques().begin_count(m1);
  calculer_coefficients_operateur_centre(Kij,Cij,Sij,Sij2,nb_comp,velocity);
  calculer_flux_operateur_centre(Fij,Kij,Cij,Sij,Sij2,nb_comp,velocity,transporte);
  if (old_centered_) modifier_flux_operateur_centre(Fij,Kij,Cij,Sij,Sij2,nb_comp,velocity,transporte);
  //statistiques().end_count(m1);
  //statistiques().begin_count(m2);

  if (centered_) ajouter_operateur_centre(Kij,Fij,transporte,resu);
  if (upwind_) ajouter_diffusion(Kij,Fij,transporte,resu);
  //statistiques().end_count(m2);
  //statistiques().begin_count(m3);
  if (stabilized_) ajouter_antidiffusion(Kij,Fij,transporte,resu);

  //statistiques().end_count(m3);
  mettre_a_jour_pour_periodicite(Kij,transporte,resu);

  resu*=-1.;//car l'operateur de convection est construit en tant que terme source
  resu+=sauv;

  modifier_flux(*this);

  return resu;
}

//ALGO TRES GROSSIER MAIS FONCTIONNE FACILEMENT
double Op_Conv_Muscl_New_VEF_Face::calculer_dt_stab() const
{
  if (!facsec_auto_)
    return Op_Conv_VEF_Face::calculer_dt_stab();
  else
    {
      const Zone_VEF& zone_VEF = la_zone_vef.valeur();
      const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();
      const Champ_P1NC& la_vitesse=ref_cast( Champ_P1NC, vitesse_.valeur());
      const DoubleTab& vitesse_2=la_vitesse.valeurs();
      const DoubleVect& porosite_face = zone_VEF.porosite_face();

      const IntTab& elem_faces = zone_VEF.elem_faces();

      const int marq=phi_u_transportant(equation());
      const int nb_faces_elem=elem_faces.dimension(1);
      const int nb_elem_tot = zone_VEF.nb_elem_tot();
      const int nfa7 = zone_VEF.type_elem().nb_facette();
      const int nb_faces_tot = zone_VEF.nb_faces_tot();
      const int nb_bord = zone_Cl_VEF.nb_cond_lim();

      int nb_comp=1;
      if(equation().inconnue().valeurs().nb_dim()!=1)
        nb_comp=equation().inconnue().valeurs().dimension(1);

      int elem=-1;
      int facei_loc=-1;
      int facei=-1;
      int faceiAss=-1;
      int facej_loc=-1;
      int facej=-1;
      int n_bord=-1;
      int num1=-1,num2=-1;
      int ind_face=-1;
      int ind_face_associee=-1;
      int max_int=-1;

      double dt_stab = Op_Conv_VEF_Face::calculer_dt_stab();
      double security_coeff=0.95;
      double dt_corrector=-1.;
      double kij=0.;

      // soit on a transporte=phi*transporte_ et vitesse=vitesse_
      // soit transporte=transporte_ et vitesse=phi*vitesse_
      // cela depend si on transporte avec phi u ou avec u.
      DoubleTab vitesse_face_;
      const DoubleTab& velocity=modif_par_porosite_si_flag(vitesse_2,vitesse_face_,marq,porosite_face);

      //statistiques().begin_count(m1);
      DoubleTab Kij(nb_elem_tot,nb_faces_elem,nb_faces_elem);
      DoubleTab Cij(nb_elem_tot,nfa7);
      DoubleTab Sij(nb_elem_tot,nfa7);
      DoubleTab Sij2;
      if (Objet_U::dimension==3)
        {
          Sij2.resize(nb_elem_tot,nfa7);
          Sij2=0.;
        }
      calculer_coefficients_operateur_centre(Kij,Cij,Sij,Sij2,nb_comp,velocity);

      //Debut du calcul
      IntTab plus_tab(nb_faces_tot);
      for (elem=0; elem<nb_elem_tot; elem++)
        for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
          {
            facei=elem_faces(elem,facei_loc);
            for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
              {
                kij=Kij(elem,facei_loc,facej_loc);
                facej=elem_faces(elem,facej_loc);

                if (kij>=0.)
                  plus_tab(facei)+=1;
                else
                  plus_tab(facej)+=1;
              }
          }
      for (n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          num1=0;
          num2=le_bord.nb_faces_tot();

          if (sub_type(Periodique,la_cl.valeur()))
            {
              const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

              for (ind_face=num1; ind_face<num2; ind_face++)
                {
                  facei=le_bord.num_face(ind_face);
                  ind_face_associee=la_cl_perio.face_associee(ind_face);
                  faceiAss=le_bord.num_face(ind_face_associee);

                  if (facei<faceiAss)
                    {
                      plus_tab[faceiAss]+=plus_tab[facei];
                      plus_tab[facei]=plus_tab[faceiAss];
                    }

                }//fin du for sur "ind_face"

            }//fin du if sur "Periodique"

        }//fin du for sur "n_bord"
      max_int=plus_tab.mp_max_vect();
      max_int=(int)Process::mp_max(max_int);

      dt_corrector=1./(1+max_limiteur_*max_int);
      dt_corrector*=security_coeff;
      dt_stab*=dt_corrector;

      Op_Conv_VEF_base& op = ref_cast_non_const(Op_Conv_VEF_base,*this);
      op.fixer_dt_stab_conv(dt_stab);

      return dt_stab;
    }
}

//Correction pour le poreux : on rajoute la partie en  T div(u)
//Variable transportee : T
//Variable transportante : u
//REMARQUE : il ne FAUT SURTOUT PAS utiliser le tableau Kij car par
//construction celui-ci est telle que sum_{j} Kij =0 ce qui revient a
//imposer une vitesse a divergence nulle par element. Ce qui est
//problematique quand on est en compressible
DoubleTab& Op_Conv_Muscl_New_VEF_Face::ajouter_partie_compressible(const DoubleTab& transporte,
                                                                   DoubleTab& resu, const DoubleTab& vitesse_2) const
{
  //   const Zone_VEF& zone_VEF=la_zone_vef.valeur();

  //   const IntTab& elem_faces=zone_VEF.elem_faces();
  //   const IntTab& face_voisins = zone_VEF.face_voisins();

  //   const DoubleTab& face_normales=zone_VEF.face_normales();

  //   const int nb_elem_tot=zone_VEF.nb_elem_tot();
  //   const int nb_faces_elem=elem_faces.dimension(1);

  //   //Pour tenir compte de la porosite
  //   const int marq = phi_u_transportant(equation());

  //   const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  //   const DoubleVect& porosite_face = zone_VEF.porosite_face();

  //   DoubleTab vitesse(vitesse_.valeur().valeurs());
  //   for (int i=0; i<vitesse.dimension(0); i++)
  //     for (int j=0; j<vitesse.dimension(1); j++)
  //       vitesse(i,j)*=porosite_face(i);

  //   int nb_comp=1;
  //   if (transporte.nb_dim()!=1) nb_comp=transporte.dimension(1);

  //   int elem=0,type_elem=0;
  //   int facei=0,facei_loc=0;
  //   int ligne=0;
  //   int dim=0;

  //   double coeff=0.;
  //   double signe=0.;
  //   double div=0.;

  //   const DoubleVect& transporteV=ref_cast(DoubleVect,transporte);
  //   DoubleVect& resuV=ref_cast(DoubleVect,resu);

  //   double (*formule)(int);
  //   if (!volumes_etendus_)
  //     if (dimension==2) formule=&formule_Id_2D;
  //     else formule=&formule_Id_3D;
  //   else
  //     if (dimension==2) formule=&formule_2D;
  //     else formule=&formule_3D;

  //   for (elem=0; elem<nb_elem_tot; elem++)
  //     {
  //       //Type de l'element : le nombre de faces de Dirichlet
  //       //qu'il contient
  //       type_elem=elem_nb_faces_dirichlet_(elem);
  //       coeff=formule(type_elem);

  //       //Calcul de la divergence par element
  //       div=0.;
  //       for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
  //         {
  //           facei=elem_faces(elem,facei_loc);
  //           signe=(face_voisins(facei,0)==elem)? 1.:-1.;

  //           for (dim=0; dim<dimension; dim++)
  //             div+=signe*face_normales(facei,dim)*vitesse(facei,dim);
  //         }
  //       div*=coeff;
  //       if (!marq) div/=porosite_elem(elem);

  //       //Calcul de la partie compressible
  //       for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
  //         {
  //           facei=elem_faces(elem,facei_loc);

  //           for (dim=0; dim<nb_comp; dim++)
  //             {
  //               ligne=facei*nb_comp+dim;
  //               resuV[ligne]-=div*transporteV[ligne];
  //             }
  //         }
  //     }

  return resu;
}

void Op_Conv_Muscl_New_VEF_Face::calculer_flux_bords(const DoubleTab& Kij, const DoubleTab& velocity, const DoubleTab& transporte) const
{

  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();


  const DoubleVect& transporteV=ref_cast(DoubleVect,transporte);
  const DoubleTab& face_normales=zone_VEF.face_normales();

  const int nb_bord = zone_Cl_VEF.nb_cond_lim();

  int nb_comp=1;
  if (transporte.nb_dim()!=1) nb_comp=transporte.dimension(1);

  int facei=0;
  int dim=0;
  int n_bord=0;
  int num1=0,num2=0;
  int ind_face=0;

  double psc=0.;

  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1 = 0;
      num2 = le_bord.nb_faces();//il ne faut boucler que sur les faces reelles ici

      if ( sub_type(Dirichlet_homogene,la_cl.valeur()) )
        {
          //On ne calcule pas le flux aux bords de Dirichlet_homogene
        }//fin du if sur "Dirichlet"
      else if ( sub_type(Dirichlet,la_cl.valeur() ))
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei = le_bord.num_face(ind_face);

              psc=0.;
              for (dim=0; dim<dimension; dim++)
                psc-=velocity(facei,dim)*face_normales(facei,dim);

              for (dim=0; dim<nb_comp; dim++)
                flux_bords_(facei,dim)=psc*transporteV[facei*nb_comp+dim];
            }
        }
      else if ( ( sub_type(Neumann,la_cl.valeur()) &&
                  !( sub_type(Neumann_sortie_libre,la_cl.valeur())) )
                || sub_type(Neumann_homogene,la_cl.valeur())
                || sub_type(Symetrie,la_cl.valeur())
                || sub_type(Echange_impose_base,la_cl.valeur())
              )
        {
          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei = le_bord.num_face(ind_face);

              psc=0.;
              for (dim=0; dim<dimension; dim++)
                psc-=velocity(facei,dim)*face_normales(facei,dim);

              for (dim=0; dim<nb_comp; dim++)
                flux_bords_(facei,dim)=psc*transporteV[facei*nb_comp+dim];
            }

        }//fin du if sur "Neumann", "Neumann_homogene", "Symetrie", "Echange_impose_base"
      else if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei = le_bord.num_face(ind_face);

              psc=0.;
              for (dim=0; dim<dimension; dim++)
                psc-=velocity(facei,dim)*face_normales(facei,dim);

              if (psc<0.)
                {
                  for (dim=0; dim<nb_comp; dim++)
                    {
                      flux_bords_(facei,dim) = psc*transporteV[facei*nb_comp+dim];
                    }
                }
              else
                {
                  if (nb_comp == 1)
                    {
                      flux_bords_(facei,0) = psc*la_sortie_libre.val_ext(ind_face);
                    }
                  else
                    {
                      for (dim=0; dim<nb_comp; dim++)
                        {
                          flux_bords_(facei,dim) = psc*la_sortie_libre.val_ext(ind_face,dim);
                        }
                    }
                }
            }
        }
      else if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique, la_cl.valeur());

          ArrOfInt fait(num2);
          fait = 0;

          int ind_face_voisine = -1;
          int facei_voisine    = -1;
          double flux             = 0.;

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              if (fait[ind_face] == 0)
                {
                  facei            = le_bord.num_face(ind_face);
                  ind_face_voisine = la_cl_perio.face_associee(ind_face);
                  facei_voisine    = le_bord.num_face(ind_face_voisine);

                  psc=0.;
                  for (dim=0; dim<dimension; dim++)
                    psc-=velocity(facei,dim)*face_normales(facei,dim);

                  for (dim=0; dim<nb_comp; dim++)
                    {
                      flux                           = psc*transporteV[facei*nb_comp+dim];
                      flux_bords_(facei,dim)         =  flux;
                      flux_bords_(facei_voisine,dim) = -flux;
                    }

                  fait[ind_face]         = 1;
                  fait[ind_face_voisine] = 1;
                }
            }
        }
      else
        {
          if (Process::je_suis_maitre())
            {
              Cerr << "Erreur Op_Conv_Muscl_New_VEF_Face::calculer_flux_bords()" << finl;
              Cerr << "Condition aux limites " << la_cl.que_suis_je() << " non codee."   << finl;
              Cerr << "Sortie du programme." << finl;
            }
          Process::exit();
        }//fin du else sur les autres conditions aux limites
    }
}

DoubleTab&
Op_Conv_Muscl_New_VEF_Face::ajouter_operateur_centre(const DoubleTab& Kij, const DoubleTab& Fij, const DoubleTab& transporte, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nfa7 = zone_VEF.type_elem().nb_facette();
  const int premiere_face_int = zone_VEF.premiere_face_int();
  const int nb_faces = zone_VEF.nb_faces();

  int nb_comp=1;
  if (transporte.nb_dim()!=1) nb_comp=transporte.dimension(1);

  int elem=0;
  int facei=0,facei_loc=0;
  int facej=0,facej_loc=0;
  int ligne=0,colonne=0;
  int dim=0;
  int fa7=0;

  double fij=0.;
  double fji=0.;

  const DoubleVect& transporteV = ref_cast(DoubleVect,transporte);
  DoubleVect& resuV=ref_cast(DoubleVect,resu);

  //Faces internes
  for (elem=0; elem<nb_elem_tot; elem++)
    for (fa7=0; fa7<nfa7; fa7++)
      {
        facei_loc=KEL(0,fa7);
        facej_loc=KEL(1,fa7);

        facei=elem_faces(elem,facei_loc);
        facej=elem_faces(elem,facej_loc);

        for (dim=0; dim<nb_comp; dim++)
          {
            fij=Fij(elem,facei_loc,facej_loc,dim);
            fji=Fij(elem,facej_loc,facei_loc,dim);

            ligne=facei*nb_comp+dim;
            colonne=facej*nb_comp+dim;

            resuV[ligne]+=fij;
            resuV[colonne]+=fji;
          }
      }

  if (old_centered_)
    for (facei=premiere_face_int; facei<nb_faces; facei++)
      {
        //Premier voisin
        elem=face_voisins(facei,0);
        facei_loc=num_fac_loc(facei,0);
        assert(elem_faces(elem,facei_loc)==facei);

        fij=Kij(elem,facei_loc,facei_loc);

        //Deuxieme voisin
        elem=face_voisins(facei,1);
        facei_loc=num_fac_loc(facei,1);
        assert(elem_faces(elem,facei_loc)==facei);

        fij+=Kij(elem,facei_loc,facei_loc);

        for (dim=0; dim<nb_comp; dim++)
          {
            ligne=facei*nb_comp+dim;
            resuV[ligne]-=fij*transporteV[ligne];
          }
      }


  //Faces de bord
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  const int nb_bord = zone_Cl_VEF.nb_cond_lim();

  int n_bord=-1;
  int num1=0;
  int num2=0;
  int ind_face=-1;

  double psc=0.;

  for (n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      num1=0;
      num2=le_bord.nb_faces();

      if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
        {
          const Neumann_sortie_libre& la_sortie_libre = ref_cast(Neumann_sortie_libre, la_cl.valeur());

          for (ind_face=num1; ind_face<num2; ind_face++)
            {
              facei=le_bord.num_face(ind_face);
              facei_loc=num_fac_loc(facei,0);
              elem=face_voisins(facei,0);
              assert(elem_faces(elem,facei_loc)==facei);


              psc=Kij(elem,facei_loc,facei_loc);//ATTENTION : SUPPOSE QU'ON EST a DIVERGENCE NULLE

              if (psc>=0.)
                {
                  //On ne fait rien car on en a deja tenu compte dans l'operateur centre
                }
              else
                for (dim=0; dim<nb_comp; dim++)
                  {
                    //On modifie car on a tenu compte dans l'operateur centre de psc*tansporteV[ligne]
                    ligne=facei*nb_comp+dim;
                    resuV[ligne]+=psc*(la_sortie_libre.val_ext(facei,dim)-transporteV[ligne]);
                  }

            }//fin du for sur "face_i"

        }//fin du if sur "Neumann"

    }//fin du for sur "n_bord"


  //Retour du resultat
  return resu;
}

DoubleTab&
Op_Conv_Muscl_New_VEF_Face::ajouter_diffusion(const DoubleTab& Kij,const DoubleTab& Fij, const DoubleTab& transporte, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nfa7 = zone_VEF.type_elem().nb_facette();

  int nb_comp=1;
  if (transporte.nb_dim()!=1) nb_comp=transporte.dimension(1);

  int elem=0;
  int facei=0,facei_loc=0;
  int facej=0,facej_loc=0;
  int ligne=0,colonne=0;
  int dim=0;
  int fa7=0;

  //   double coeffij=0.,coeffji=0.;
  double fij=0.,fji=0.;
  double psc=0.;

  DoubleVect& resuV=ref_cast(DoubleVect,resu);

  //Pour les faces internes
  for (elem=0; elem<nb_elem_tot; elem++)
    for (fa7=0; fa7<nfa7; fa7++)
      {
        facei_loc=KEL(0,fa7);
        facej_loc=KEL(1,fa7);

        facei=elem_faces(elem,facei_loc);
        facej=elem_faces(elem,facej_loc);

        psc=Kij(elem,facei_loc,facej_loc);

        for (dim=0; dim<nb_comp; dim++)
          {
            fij=alpha_*Fij(elem,facei_loc,facej_loc,dim);
            fji=alpha_*Fij(elem,facej_loc,facei_loc,dim);

            ligne=facei*nb_comp+dim;
            colonne=facej*nb_comp+dim;

            if (psc>=0.)
              {
                resuV[ligne]-=fij;
                resuV[colonne]+=fij;
              }
            else
              {
                resuV[ligne]+=fji;
                resuV[colonne]-=fji;
              }
          }
      }

  //Pour les faces de bord :
  //ON N'A RIEN a FAIRE

  //Retour du resultat
  return resu;
}

DoubleTab&
Op_Conv_Muscl_New_VEF_Face::ajouter_antidiffusion(const DoubleTab& Kij, const DoubleTab& Fij,const DoubleTab& transporte, DoubleTab& resu) const
{
  switch(version_)
    {
    case 1 :

      return ajouter_antidiffusion_v1(Kij,Fij,transporte,resu);
      break;

    case 2 :

      return ajouter_antidiffusion_v2(Kij,Fij,transporte,resu);
      break;

    default :

      if (Process::je_suis_maitre())
        {
          Cerr<<"Error in Op_Conv_Muscl_New_VEF_Face::ajouter_antidiffusion()"<<finl;
          Cerr<<"Version number "<<version_<<" of antidiffusive operator does not exist"<<finl;
        }
      Process::exit();
      break;
    }

  return resu;
}

DoubleTab&
Op_Conv_Muscl_New_VEF_Face::ajouter_antidiffusion_v2(const DoubleTab& Kij, const DoubleTab& Fij,const DoubleTab& transporte, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();

  const DoubleVect& transporteV=ref_cast(DoubleVect,transporte);

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nfa7 = zone_VEF.type_elem().nb_facette();

  int nb_comp=1;
  if (transporte.nb_dim()!=1) nb_comp=transporte.dimension(1);

  int elem=0;
  int facei=0,facei_loc=0;
  int facej=0,facej_loc=0;
  int ligne=0,colonne=0;
  int dim=0;
  int fa7=0;

  //Pour le limiteur
  ArrOfDouble P_plus(nb_comp),P_moins(nb_comp);
  ArrOfDouble Q_plus(nb_comp),Q_moins(nb_comp);
  P_plus=0., P_moins=0.;
  Q_plus=0., Q_moins=0.;

  //   double coeffij=0.,coeffji=0.;
  double fij=0.,fji=0.;
  double kij=0.;
  double R=0.;
  double tmp=0.;
  double fij_low=0.,fji_low=0.;

  DoubleVect& resuV=ref_cast(DoubleVect,resu);

  //Pour les faces internes
  for (elem=0; elem<nb_elem_tot; elem++)
    for (fa7=0; fa7<nfa7; fa7++)
      {
        facei_loc=KEL(0,fa7);
        facej_loc=KEL(1,fa7);

        facei=elem_faces(elem,facei_loc);
        facej=elem_faces(elem,facej_loc);

        kij=Kij(elem,facei_loc,facej_loc);

        P_plus=0., P_moins=0.;
        Q_plus=0., Q_moins=0.;
        if (kij>=0.) //facei amont
          calculer_senseur_v2(Kij,Fij,transporteV,nb_comp,facei,elem_faces,face_voisins,num_fac_loc,P_plus,P_moins,Q_plus,Q_moins);//La face i est amont
        else //facej amont
          calculer_senseur_v2(Kij,Fij,transporteV,nb_comp,facej,elem_faces,face_voisins,num_fac_loc,P_plus,P_moins,Q_plus,Q_moins);//La face j est amont

        for (dim=0; dim<nb_comp; dim++)
          {
            fij=alpha_*Fij(elem,facei_loc,facej_loc,dim);
            fji=alpha_*Fij(elem,facej_loc,facei_loc,dim);

            ligne=facei*nb_comp+dim;
            colonne=facej*nb_comp+dim;

            fij_low=transporteV[colonne]-transporteV[ligne];
            fij_low*=kij;
            fji_low=fij_low;

            if (kij>=0.) //facei amont
              {
                // if (fij>=0.) R=(P_plus(dim)==0.)?0.:Q_plus(dim)/P_plus(dim);
                // else R=(P_moins(dim)==0.)?0.:Q_moins(dim)/P_moins(dim);

                // if (fij>=0.) R=(P_plus(dim)==0.)?0.:Q_plus(dim)/(P_plus(dim)+DMINFLOAT);
                // else R=(P_moins(dim)==0.)?0.:Q_moins(dim)/(P_moins(dim)+DMINFLOAT);

                if (fij>=0.) R=(dabs(P_plus(dim))<DMINFLOAT)?0.:Q_plus(dim)/P_plus(dim);
                else R=(dabs(P_moins(dim))<DMINFLOAT)?0.:Q_moins(dim)/P_moins(dim);


                R=(*limiteur_)(R);
                R*=fij;

                if (!is_dirichlet_faces_(facej))
                  tmp=optimum(R,fji_low);
                else
                  tmp=R;

                resuV[ligne]+=tmp;
                resuV[colonne]-=tmp;
              }
            else //facej amont
              {
                // if (fji<=0.) R=(P_moins(dim)==0.)?0.:Q_moins(dim)/P_moins(dim);
                // else R=(P_plus(dim)==0.)?0.:Q_plus(dim)/P_plus(dim);

                // if (fji<=0.) R=(P_moins(dim)==0.)?0.:Q_moins(dim)/(P_moins(dim)+DMINFLOAT);
                // else R=(P_plus(dim)==0.)?0.:Q_plus(dim)/(P_plus(dim)+DMINFLOAT);

                if (fji<=0.) R=(dabs(P_moins(dim))<DMINFLOAT)?0.:Q_moins(dim)/P_moins(dim);
                else R=(dabs(P_plus(dim))<DMINFLOAT)?0.:Q_plus(dim)/P_plus(dim);


                R=(*limiteur_)(R);
                R*=fji;

                if (!is_dirichlet_faces_(facei))
                  tmp=optimum(R,fij_low);
                else
                  tmp=R;

                resuV[ligne]-=tmp;
                resuV[colonne]+=tmp;
              }
          }
      }


  //Pour les faces de bord
  //IL N'Y A RIEN a FAIRE TOUT EST FAIT DANS LA FONCTION AJOUTER_DIFFUSION
  //QUI EST PARFAITEMENT COMPLeTER PAR LA FONCTION AJOUTER_ANTIDIFFUSION

  //Retour du resultat
  return resu;
}

DoubleTab&
Op_Conv_Muscl_New_VEF_Face::ajouter_antidiffusion_v1(const DoubleTab& Kij, const DoubleTab& Fij,const DoubleTab& transporte, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();

  const DoubleVect& transporteV=ref_cast(DoubleVect,transporte);

  const IntTab& elem_faces=zone_VEF.elem_faces();
  const IntTab& KEL=zone_VEF.type_elem().valeur().KEL();
  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();

  const int nb_elem_tot=zone_VEF.nb_elem_tot();
  const int nfa7 = zone_VEF.type_elem().nb_facette();

  int nb_comp=1;
  if (transporte.nb_dim()!=1) nb_comp=transporte.dimension(1);

  int elem=0;
  int facei=0,facei_loc=0;
  int facej=0,facej_loc=0;
  int ligne=0,colonne=0;
  int dim=0;
  int fa7=0;

  //Pour le limiteur
  ArrOfDouble Pi_plus(nb_comp),Pi_moins(nb_comp);
  ArrOfDouble Qi_plus(nb_comp),Qi_moins(nb_comp);
  Pi_plus=0., Pi_moins=0.;
  Qi_plus=0., Qi_moins=0.;

  //Pour le limiteur
  ArrOfDouble Pj_plus(nb_comp),Pj_moins(nb_comp);
  ArrOfDouble Qj_plus(nb_comp),Qj_moins(nb_comp);
  Pj_plus=0., Pj_moins=0.;
  Qj_plus=0., Qj_moins=0.;

  //   double coeffij=0.,coeffji=0.;
  double fij=0.,fji=0.;
  double kij=0.;
  double R=0.;
  double Ri=0.;
  double Rj=0.;
  double tmp=0.;

  DoubleVect& resuV=ref_cast(DoubleVect,resu);

  //Pour les faces internes
  for (elem=0; elem<nb_elem_tot; elem++)
    for (fa7=0; fa7<nfa7; fa7++)
      {
        facei_loc=KEL(0,fa7);
        facej_loc=KEL(1,fa7);

        facei=elem_faces(elem,facei_loc);
        facej=elem_faces(elem,facej_loc);

        kij=Kij(elem,facei_loc,facej_loc);

        Pi_plus=0., Pi_moins=0.;
        Qi_plus=0., Qi_moins=0.;
        calculer_senseur_v1(Kij,Fij,transporteV,nb_comp,facei,elem_faces,face_voisins,num_fac_loc,Pi_plus,Pi_moins,Qi_plus,Qi_moins);

        Pj_plus=0., Pj_moins=0.;
        Qj_plus=0., Qj_moins=0.;
        calculer_senseur_v1(Kij,Fij,transporteV,nb_comp,facej,elem_faces,face_voisins,num_fac_loc,Pj_plus,Pj_moins,Qj_plus,Qj_moins);

        for (dim=0; dim<nb_comp; dim++)
          {
            fij=alpha_*Fij(elem,facei_loc,facej_loc,dim);
            fji=alpha_*Fij(elem,facej_loc,facei_loc,dim);

            ligne=facei*nb_comp+dim;
            colonne=facej*nb_comp+dim;

            if (kij>=0.)
              {
                //Face amont : facei
                if (fij>=0.)
                  {
                    // Ri=(Pi_plus(dim)==0.)?0.:Qi_plus(dim)/Pi_plus(dim);
                    // Rj=(Pj_moins(dim)==0.)?0.:Qj_moins(dim)/Pj_moins(dim);//car fji=-fij

                    // Ri=(Pi_plus(dim)==0.)?0.:Qi_plus(dim)/(Pi_plus(dim)+DMINFLOAT);
                    // Rj=(Pj_moins(dim)==0.)?0.:Qj_moins(dim)/(Pj_moins(dim)+DMINFLOAT);//car fji=-fij

                    Ri=(dabs(Pi_plus(dim))<DMINFLOAT)?0.:Qi_plus(dim)/Pi_plus(dim);
                    Rj=(dabs(Pj_moins(dim))<DMINFLOAT)?0.:Qj_moins(dim)/Pj_moins(dim);//car fji=-fij
                  }
                else
                  {
                    // Ri=(Pi_moins(dim)==0.)?0.:Qi_moins(dim)/Pi_moins(dim);
                    // Rj=(Pj_plus(dim)==0.)?0.:Qj_plus(dim)/Pj_plus(dim);//car fji=-fij

                    // Ri=(Pi_moins(dim)==0.)?0.:Qi_moins(dim)/(Pi_moins(dim)+DMINFLOAT);
                    // Rj=(Pj_plus(dim)==0.)?0.:Qj_plus(dim)/(Pj_plus(dim)+DMINFLOAT);//car fji=-fij

                    Ri=(dabs(Pi_moins(dim))<DMINFLOAT)?0.:Qi_moins(dim)/Pi_moins(dim);
                    Rj=(dabs(Pj_plus(dim))<DMINFLOAT)?0.:Qj_plus(dim)/Pj_plus(dim);//car fji=-fij
                  }

                if (is_dirichlet_faces_(facej)) Rj=DMAXFLOAT;//on n'a pas besoin de prendre le min quand il y a une face de Dirichlet
                R=(Ri<=Rj)?Ri:Rj;
                R=(*limiteur_)(R);

                tmp=R*fij;
                resuV[ligne]+=tmp;
                resuV[colonne]-=tmp;
              }
            else
              {
                //Face amont : facej
                if (fji<=0.)
                  {
                    // Rj=(Pj_moins(dim)==0.)?0.:Qj_moins(dim)/Pj_moins(dim);
                    // Ri=(Pi_plus(dim)==0.)?0.:Qi_plus(dim)/Pi_plus(dim);

                    // Rj=(Pj_moins(dim)==0.)?0.:Qj_moins(dim)/(Pj_moins(dim)+DMINFLOAT);
                    // Ri=(Pi_plus(dim)==0.)?0.:Qi_plus(dim)/(Pi_plus(dim)+DMINFLOAT);

                    Rj=(dabs(Pj_moins(dim))<DMINFLOAT)?0.:Qj_moins(dim)/Pj_moins(dim);
                    Ri=(dabs(Pi_plus(dim))<DMINFLOAT)?0.:Qi_plus(dim)/Pi_plus(dim);
                  }
                else
                  {
                    // Rj=(Pj_plus(dim)==0.)?0.:Qj_plus(dim)/Pj_plus(dim);
                    // Ri=(Pi_moins(dim)==0.)?0.:Qi_moins(dim)/Pi_moins(dim);

                    // Rj=(Pj_plus(dim)==0.)?0.:Qj_plus(dim)/(Pj_plus(dim)+DMINFLOAT);
                    // Ri=(Pi_moins(dim)==0.)?0.:Qi_moins(dim)/(Pi_moins(dim)+DMINFLOAT);

                    Rj=(dabs(Pj_plus(dim))<DMINFLOAT)?0.:Qj_plus(dim)/Pj_plus(dim);
                    Ri=(dabs(Pi_moins(dim))<DMINFLOAT)?0.:Qi_moins(dim)/Pi_moins(dim);
                  }

                if (is_dirichlet_faces_(facei)) Ri=DMAXFLOAT;//on n'a pas besoin de prendre le min quand il y a une face de Dirichlet
                R=(Ri<=Rj)?Ri:Rj;
                R=(*limiteur_)(R);

                tmp=R*fji;
                resuV[ligne]-=tmp;
                resuV[colonne]+=tmp;
              }
          }
      }

  //Pour les faces de bord
  //IL N'Y A RIEN a FAIRE TOUT EST FAIT DANS LA FONCTION AJOUTER_DIFFUSION
  //QUI EST PARFAITEMENT COMPLeTER PAR LA FONCTION AJOUTER_ANTIDIFFUSION

  //Retour du resultat
  //     resuV+=antidiff;
  return resu;
}

//ATTENTION : suppose les parametres P_plus, P_moins, Q_plus, Q_moins nuls en entree
inline void
Op_Conv_Muscl_New_VEF_Face::calculer_senseur_v2(const DoubleTab& Kij, const DoubleTab& Fij, const DoubleVect& transporteV,
                                                const int& nb_comp, const int& face_i,
                                                const IntTab& elem_faces, const IntTab& face_voisins, const IntTab& num_fac_loc,
                                                ArrOfDouble& P_plus, ArrOfDouble& P_moins,
                                                ArrOfDouble& Q_plus, ArrOfDouble& Q_moins) const
{
  calculer_senseur_v1(Kij,Fij,transporteV,nb_comp,face_i,elem_faces,face_voisins,num_fac_loc,P_plus,P_moins,Q_plus,Q_moins);

  //   assert(P_plus.size_array()==nb_comp);
  //   assert(Q_plus.size_array()==nb_comp);
  //   assert(P_moins.size_array()==nb_comp);
  //   assert(Q_moins.size_array()==nb_comp);

  //   const int nb_faces_elem=elem_faces.dimension(1);
  //   for (int elem_voisin=0; elem_voisin<2; elem_voisin++)
  //     {
  //       int elem = face_voisins(face_i,elem_voisin);
  //       if (elem!=-1)
  //         {
  //           int face_i_loc = num_fac_loc(face_i,elem_voisin);
  //           assert(face_i_loc>=0);
  //           assert(face_i_loc<nb_faces_elem);
  //           //On travaille sur les faces de "elem"
  //           for (int face_k_loc=0; face_k_loc<nb_faces_elem; face_k_loc++)
  //             {
  //               double kik=Kij(elem,face_i_loc,face_k_loc);
  //               //
  //               //Calcul des variables intermediaires
  //               //
  //               for (int dim=0; dim<nb_comp; dim++)
  //                 {
  //                   double fik=Fij(elem,face_i_loc,face_k_loc,dim);

  //                   // Codage optimise:
  //                   if (kik<0)
  //                   {
  //                      if (fik>0) Q_plus(dim)+=fik;
  //                      else       Q_moins(dim)+=fik;
  //                   }
  //                   else
  //                   {
  //                      if (fik>0) P_plus(dim)+=fik;
  //                      else       P_moins(dim)+=fik;
  //                   }
  //                   assert(P_plus(dim)>=0);
  //                   assert(Q_plus(dim)>=0);
  //                   assert(P_moins(dim)<=0);
  //                   assert(Q_moins(dim)<=0);
  //                 }//fin du for sur "dim"
  //               //
  //               //Fin du calcul des variables intermediaires
  //               //
  //             }//fin du for sur "face_k_loc"
  //         }//fin du if sur "elem!=-1"
  //     }//fin du for sur "elem_voisin"
}

//ATTENTION : suppose les parametres P_plus, P_moins, Q_plus, Q_moins nuls en entree
inline void
Op_Conv_Muscl_New_VEF_Face::calculer_senseur_v1(const DoubleTab& Kij, const DoubleTab& Fij, const DoubleVect& transporteV,
                                                const int& nb_comp, const int& face_i,
                                                const IntTab& elem_faces, const IntTab& face_voisins, const IntTab& num_fac_loc,
                                                ArrOfDouble& P_plus, ArrOfDouble& P_moins,
                                                ArrOfDouble& Q_plus, ArrOfDouble& Q_moins) const
{
  assert(P_plus.size_array()==nb_comp);
  assert(Q_plus.size_array()==nb_comp);
  assert(P_moins.size_array()==nb_comp);
  assert(Q_moins.size_array()==nb_comp);

  double inci=0.;
  double inck=0.;

  const int nb_faces_elem=elem_faces.dimension(1);
  for (int elem_voisin=0; elem_voisin<2; elem_voisin++)
    {
      int elem = face_voisins(face_i,elem_voisin);
      if (elem!=-1)
        {
          int face_i_loc = num_fac_loc(face_i,elem_voisin);
          assert(face_i_loc>=0);
          assert(face_i_loc<nb_faces_elem);

          //On travaille sur les faces de "elem"
          for (int face_k_loc=0; face_k_loc<nb_faces_elem; face_k_loc++)
            {
              int face_k=elem_faces(elem,face_k_loc);
              double kik=Kij(elem,face_i_loc,face_k_loc);
              //
              //Calcul des variables intermediaires
              //
              for (int dim=0; dim<nb_comp; dim++)
                {
                  inci=transporteV[face_i*nb_comp+dim];
                  inck=transporteV[face_k*nb_comp+dim];

                  double fik_low=kik*(inck-inci);
                  double fik_high=Fij(elem,face_i_loc,face_k_loc,dim);

                  // Codage optimise:
                  if (kik<0)
                    {
                      if (fik_low>0) Q_plus(dim)+=fik_low;
                      else       Q_moins(dim)+=fik_low;
                    }
                  else
                    {
                      if (fik_high>0) P_plus(dim)+=fik_high;
                      else       P_moins(dim)+=fik_high;
                    }
                  assert(P_plus(dim)>=0);
                  assert(Q_plus(dim)>=0);
                  assert(P_moins(dim)<=0);
                  assert(Q_moins(dim)<=0);
                }//fin du for sur "dim"
              //
              //Fin du calcul des variables intermediaires
              //
            }//fin du for sur "face_k_loc"
        }//fin du if sur "elem!=-1"
    }//fin du for sur "elem_voisin"
}

void Op_Conv_Muscl_New_VEF_Face::test(const DoubleTab& transporte, const DoubleTab& resu, const DoubleTab& velocity) const
{
}

void Op_Conv_Muscl_New_VEF_Face::test_difference_Kij(const DoubleTab& transporte, DoubleTab& Kij, DoubleTab& Kij_ancien, const DoubleTab& velocity ) const
{
}

void Op_Conv_Muscl_New_VEF_Face::test_difference_resu(const DoubleTab& Kij, const DoubleTab& Kij_ancien,
                                                      const DoubleTab& transporte,const DoubleTab& resu, const DoubleTab& velocity) const
{
  //   DoubleTab resu1(resu);
  //   resu1=0;

  //   if (is_compressible_) ajouter_partie_compressible(transporte,resu1,velocity);
  //   ajouter_operateur_centre(Kij,transporte,resu1);
  //   ajouter_diffusion(Kij,transporte,resu1);
  //   ajouter_antidiffusion(Kij,transporte,resu1);
  //   mettre_a_jour_pour_periodicite(resu1);
}

void Op_Conv_Muscl_New_VEF_Face::mettre_a_jour_pour_periodicite(const DoubleTab& Kij, const DoubleTab& transporte, DoubleTab& resu) const
{
  const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  const int nb_bord = zone_Cl_VEF.nb_cond_lim();
  const int nb_comp = (resu.nb_dim()==1) ? 1 : resu.dimension(1);

  const DoubleVect& transporteV=ref_cast(DoubleVect,transporte);
  DoubleVect& resuV=ref_cast(DoubleVect,resu);

  const IntTab& face_voisins=zone_VEF.face_voisins();
  const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();

  int ligne=0,ligneAss=0;
  int facei=0,faceiAss=0;
  int ind_face=0;
  int num1=0,num2=0;
  int ind_face_associee=0;
  int dim=0;
  int elem=-1;
  int facei_loc=-1;

  double kii=0.;
  double tmp=0.;

  //Faces de bord
  for (int n_bord=0; n_bord<nb_bord; n_bord++)
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
              facei=le_bord.num_face(ind_face);
              ind_face_associee=la_cl_perio.face_associee(ind_face);
              faceiAss=le_bord.num_face(ind_face_associee);

              if (facei<faceiAss)
                for (dim=0; dim<nb_comp; dim++)
                  {
                    ligne=facei*nb_comp+dim;
                    ligneAss=faceiAss*nb_comp+dim;

                    resuV[ligneAss]+=resuV[ligne];
                    resuV[ligne]=resuV[ligneAss];
                  }

              if (old_centered_)
                {
                  //Pour le 1er element voisin
                  elem=face_voisins(facei,0);
                  facei_loc=-1;
                  facei_loc=num_fac_loc(facei,0);
                  assert(facei_loc!=-1);
                  kii=Kij(elem,facei_loc,facei_loc);

                  //Pour le 2eme element voisin
                  elem=face_voisins(facei,1);
                  facei_loc=-1;
                  facei_loc=num_fac_loc(facei,1);
                  assert(facei_loc!=-1);
                  kii+=Kij(elem,facei_loc,facei_loc);

                  for (dim=0; dim<nb_comp; dim++)
                    {
                      ligne=facei*nb_comp+dim;
                      ligneAss=faceiAss*nb_comp+dim;

                      tmp=kii*transporteV[ligneAss];
                      resuV[ligneAss]-=tmp;
                      resuV[ligne]-=tmp;
                    }
                }

            }//fin du for sur "face_i"

        }//fin du if sur "Periodique"

    }//fin du for sur "n_bord"
}

void Op_Conv_Muscl_New_VEF_Face::ajouter_old(const DoubleTab& transporte, DoubleTab& resu, const DoubleTab& velocity ) const
{
}

//Fonction qui initialise les attributs "elem_nb_faces_dirichlet_"
//et "elem_faces_dirichlet_"
//REMARQUE : "elem_nb_faces_dirichlet_" contient le nombre de faces de Dirichlet
//pour chaque element du maillage
//REMARQUE : "elem_faces_dirichlet_" le numero global des faces de Dirichlet
//contenu dans un element quelconque du maillage
void Op_Conv_Muscl_New_VEF_Face::calculer_data_pour_dirichlet()
{
  const Zone_VEF& zone_VEF = la_zone_vef.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  const IntTab& face_sommets = zone_VEF.face_sommets();
  const IntVect& rang_elem_non_std = zone_VEF.rang_elem_non_std();
  const IntTab& elem_sommets = zone_VEF.zone().les_elems();

  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  const int nb_bord = zone_Cl_VEF.nb_cond_lim();
  const int nb_som_faces = zone_VEF.nb_som_face();
  const int nb_som_elem = zone_VEF.zone().nb_som_elem();

  int som_loc=-1;
  int som=-1;
  int ind_face=-1;
  int elem=-1;
  int rang=-2;

  //On rajoute un flag aux elements qui ont au moins un sommet qui appartient
  //une face de Dirichlet ou Dirichlet_homogene
  if (old_centered_)
    {
      is_element_for_upwinding_.resize(nb_elem_tot);
      is_element_for_upwinding_=0;

      IntTab est_un_sommet_de_bord(zone_VEF.nb_som_tot());
      est_un_sommet_de_bord=0;

      for (int n_bord=0; n_bord<nb_bord; n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int nb_faces_tot=le_bord.nb_faces_tot();
          int face=-1;

          if ( (sub_type(Dirichlet,la_cl.valeur()))
               || (sub_type(Dirichlet_homogene,la_cl.valeur()))
             )
            for (ind_face=0; ind_face<nb_faces_tot; ind_face++)
              {
                face = le_bord.num_face(ind_face);
                for (som_loc=0; som_loc<nb_som_faces; som_loc++)
                  {
                    som=face_sommets(face,som_loc);
                    est_un_sommet_de_bord(som)=1;
                  }
              }
        }

      for (elem=0; elem<nb_elem_tot; elem++)
        {
          rang=rang_elem_non_std(elem);
          if (rang!=-1) is_element_for_upwinding_(elem)=1;
          else
            for (som_loc=0; som_loc<nb_som_elem; som_loc++)
              {
                som=elem_sommets(elem,som_loc);
                if (est_un_sommet_de_bord(som))
                  is_element_for_upwinding_(elem)=1;
              }
        }
    }
  is_dirichlet_faces_.resize(zone_VEF.nb_faces_tot());
  is_dirichlet_faces_=0;

  for (int n_bord=0; n_bord<nb_bord; n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int nb_faces_tot=le_bord.nb_faces_tot();
      int face=-1;

      if ( (sub_type(Dirichlet,la_cl.valeur()))
           || (sub_type(Dirichlet_homogene,la_cl.valeur()))
         )
        for (ind_face=0; ind_face<nb_faces_tot; ind_face++)
          {
            face = le_bord.num_face(ind_face);
            is_dirichlet_faces_(face)=1;
          }
    }
}

void Op_Conv_Muscl_New_VEF_Face::completer()
{
  Op_Conv_VEF_Face::completer();
  calculer_data_pour_dirichlet();

  //int nb_comp=1;
  //if (equation().inconnue().valeurs().nb_dim()>1)
  //  nb_comp=equation().inconnue().valeurs().dimension(1);

  //  limiteurs_.resize(la_zone_vef->nb_faces_tot(),nb_comp);
  // limiteurs_=0.;

  alpha_tab.resize_array(la_zone_vef->nb_faces_tot());
  alpha_tab = alpha_;

  beta.resize_array(la_zone_vef->nb_faces_tot());
  beta=1.;

  //   if (sous_zone) {
  //     sous_zone=false;
  //     const Sous_Zone& la_sous_zone=equation().probleme().domaine().ss_zone(nom_sous_zone);
  //     const Domaine_dis& le_domaine_dis=la_zone_vef->domaine_dis();
  //     for (int ssz=0;ssz<le_domaine_dis.nombre_de_sous_zones_dis();ssz++) {
  //       if (le_domaine_dis.sous_zone_dis(ssz)->sous_zone().est_egal_a(la_sous_zone)) {
  //       sous_zone=true;
  //       la_sous_zone_dis=ref_cast(Sous_zone_VF,le_domaine_dis.sous_zone_dis(ssz).valeur());
  //       }
  //     }

  //     if(!sous_zone) {
  //       Cerr << "On ne trouve pas la sous_zone discretisee associee a " << nom_sous_zone << finl;
  //       Process::exit();
  //     }

  //   const Sous_zone_VF& ssz=la_sous_zone_dis.valeur();
  //   int nb_faces = ssz.les_faces().size();

  //   for (int face=0;face<nb_faces;face++) {
  //     int la_face=ssz.les_faces()[face];
  //     beta(la_face) = 0.;
  //     alpha_tab(la_face) = 1.;
  //      }
  //   }

}

void Op_Conv_Muscl_New_VEF_Face::ajouter_contribution(const DoubleTab& transporte_2, Matrice_Morse& matrice) const
{
}

void Op_Conv_Muscl_New_VEF_Face::modifier_pour_Cl (Matrice_Morse& matrice, DoubleTab& secmem) const
{
  Op_Conv_VEF_Face::modifier_pour_Cl(matrice,secmem);
}

void Op_Conv_Muscl_New_VEF_Face::ajouter_contribution_operateur_centre(const DoubleTab& Kij, const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  //   const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  //   const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  //   const IntTab& elem_faces=zone_VEF.elem_faces();
  //   const IntTab& face_voisins = zone_VEF.face_voisins();

  //   const int nb_elem_tot=zone_VEF.nb_elem_tot();
  //   const int nb_faces_elem=elem_faces.dimension(1);
  //   const int nb_bord=zone_Cl_VEF.nb_cond_lim();

  //   int nb_comp=1;
  //   if (transporte.nb_dim()!=1) nb_comp=transporte.dimension(1);

  //   int elem=0;
  //   int facei=0,facei_loc=0;
  //   int facej=0,facej_loc=0;
  //   int ligne=0,colonne=0;
  //   int dim=0;
  //   int ind_face=0;
  //   int num1=0,num2=0;
  //   int faceToComplete=0;
  //   int elem_loc=0;

  //   double kij=0.,kji=0.;

  //   for (elem=0; elem<nb_elem_tot; elem++)
  //     for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
  //       {
  //         facei=elem_faces(elem,facei_loc);

  //         for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
  //           {
  //             facej=elem_faces(elem,facej_loc);
  //             assert(facej!=facei);

  //             kij=Kij(elem,facei_loc,facej_loc);
  //             kji=Kij(elem,facej_loc,facei_loc);

  //             for (dim=0; dim<nb_comp; dim++)
  //               {
  //                 ligne=facei*nb_comp+dim;
  //                 colonne=facej*nb_comp+dim;

  //                 //ATTENTION AU SIGNE : ici on code +div(uT)
  //                 matrice(ligne,ligne)+=kij;
  //                 matrice(ligne,colonne)-=kij;
  //                 matrice(colonne,colonne)+=kji;
  //                 matrice(colonne,ligne)-=kji;
  //               }
  //           }
  //       }

  //   //
  //   //Pour la periodicite
  //   //
  //   const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();
  //   for (int n_bord=0; n_bord<nb_bord; n_bord++)
  //     {
  //       const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
  //       const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
  //       num1 = 0;
  //       num2=le_bord.nb_faces_tot();//et surtout pas nb_faces sinon on oublie certains coefficients

  //       if (sub_type(Periodique,la_cl.valeur()))
  //         {
  //           const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
  //           int faceiAss=0,ind_faceiAss=0;

  //           for (ind_face=num1; ind_face<num2; ind_face++)
  //               {
  //                 ind_faceiAss=la_cl_perio.face_associee(ind_face);

  //                 facei=le_bord.num_face(ind_face);
  //                 faceiAss=le_bord.num_face(ind_faceiAss);

  //                 //Pour ne parcourir qu'une seule fois les faces perio
  //                 if (facei<faceiAss)
  //                   for (elem_loc=0;elem_loc<2;elem_loc++)
  //                     {
  //                       elem=face_voisins(facei,elem_loc);
  //                       assert(elem!=-1);

  //                       //Calcul du numero local de la face dans "elem"
  //                       facei_loc=num_fac_loc(facei,elem_loc);
  //                       if (facei_loc!=-1)
  //                         faceToComplete=faceiAss;
  //                       else
  //                         {
  //                           faceToComplete=facei;
  //                           facei_loc=num_fac_loc(faceiAss,elem_loc);
  //                           assert(facei_loc!=-1);
  //                         }

  //                       //Calcul des coefficients de la matrice dus a "elem"
  //                       for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
  //                         {
  //                           facej=elem_faces(elem,facej_loc);

  //                           if (facej_loc!=facei_loc)
  //                             {
  //                               kij=Kij(elem,facei_loc,facej_loc);
  //                               kji=Kij(elem,facej_loc,facei_loc);

  //                               for (dim=0; dim<nb_comp; dim++)
  //                                 {
  //                                   ligne=faceToComplete*nb_comp+dim;
  //                                    colonne=facej*nb_comp+dim;

  //                                   //ATTENTION AU SIGNE : ici on code +div(uT)
  //                                   matrice(ligne,ligne)+=kij;
  //                                    matrice(ligne,colonne)-=kij;
  //                                 }
  //                             }
  //                         }
  //                     }
  //               }
  //         }
  //     }
}

void Op_Conv_Muscl_New_VEF_Face::ajouter_contribution_diffusion(const DoubleTab& Kij, const DoubleTab& transporte, Matrice_Morse& matrice) const
{
  //   const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  //   const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  //   const IntTab& elem_faces=zone_VEF.elem_faces();
  //   const IntTab& face_voisins = zone_VEF.face_voisins();

  //   const int nb_elem_tot=zone_VEF.nb_elem_tot();
  //   const int nb_faces_elem=elem_faces.dimension(1);
  //   const int nb_bord=zone_Cl_VEF.nb_cond_lim();

  //   int nb_comp=1;
  //   if (transporte.nb_dim()!=1) nb_comp=transporte.dimension(1);

  //   int elem=0;
  //   int facei=0,facei_loc=0;
  //   int facej=0,facej_loc=0;
  //   int ligne=0,colonne=0;
  //   int dim=0;
  //   int ind_face=0;
  //   int num1=0,num2=0;
  //   int faceToComplete=0;
  //   int elem_loc=0;

  //   double dij=0.;
  //   double coeffij=0.,coeffji=0.;

  //   for (elem=0; elem<nb_elem_tot; elem++)
  //     for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
  //       {
  //         facei=elem_faces(elem,facei_loc);

  //         for (facej_loc=facei_loc+1; facej_loc<nb_faces_elem; facej_loc++)
  //           {
  //             facej=elem_faces(elem,facej_loc);
  //             assert(facej!=facei);

  //             dij=Dij(elem,facei_loc,facej_loc,Kij);
  //             assert(dij>=0);

  //             coeffij=alpha_tab(facei)*dij;
  //             coeffji=alpha_tab(facej)*dij;

  //             for (dim=0; dim<nb_comp; dim++)
  //               {
  //                 ligne=facei*nb_comp+dim;
  //                 colonne=facej*nb_comp+dim;

  //                 //ATTENTION AU SIGNE : ici on code +div(uT)
  //                 //REMARQUE : on utilise la symetrie de l'operateur
  //                 matrice(ligne,ligne)+=coeffij;
  //                 matrice(ligne,colonne)-=coeffij;
  //                 matrice(colonne,colonne)+=coeffji;
  //                 matrice(colonne,ligne)-=coeffji;
  //               }
  //           }
  //       }

  //   //
  //   //Pour la periodicite
  //   //
  //   const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();
  //   for (int n_bord=0; n_bord<nb_bord; n_bord++)
  //     {
  //       const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
  //       const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
  //       num1 = 0;
  //       num2=le_bord.nb_faces_tot();//et surtout pas nb_faces() sinon on oublie certains coefficiens

  //       if (sub_type(Periodique,la_cl.valeur()))
  //         {
  //           const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
  //           int faceiAss=0,ind_faceiAss=0;

  //           for (ind_face=num1; ind_face<num2; ind_face++)
  //               {
  //                 facei=le_bord.num_face(ind_face);
  //                 ind_faceiAss=la_cl_perio.face_associee(ind_face);
  //                 faceiAss=le_bord.num_face(ind_faceiAss);

  //                 //Pour ne parcourir qu'une seule fois les faces perio
  //                 if (facei<faceiAss)
  //                   for (elem_loc=0;elem_loc<2;elem_loc++)
  //                     {
  //                       elem=face_voisins(facei,elem_loc);
  //                       assert(elem!=-1);

  //                       //Calcul du numero local de la face dans "elem"
  //                       facei_loc=num_fac_loc(facei,elem_loc);
  //                       if (facei_loc!=-1)
  //                         faceToComplete=faceiAss;
  //                       else
  //                         {
  //                           faceToComplete=facei;
  //                           facei_loc=num_fac_loc(faceiAss,elem_loc);
  //                           assert(facei_loc!=-1);
  //                         }

  //                       //Calcul des coefficients de la matrice dus a "elem"
  //                       for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
  //                         {
  //                           facej=elem_faces(elem,facej_loc);

  //                           if (facej_loc!=facei_loc)
  //                             {
  //                               dij=Dij(elem,facei_loc,facej_loc,Kij);
  //                               assert(dij>=0);

  //                               coeffij=alpha_tab(faceToComplete)*dij;
  //                               coeffji=alpha_tab(facej)*dij;

  //                               for (dim=0; dim<nb_comp; dim++)
  //                                 {
  //                                   ligne=faceToComplete*nb_comp+dim;
  //                                    colonne=facej*nb_comp+dim;

  //                                   //ATTENTION AU SIGNE : ici on code +div(uT)
  //                                   matrice(ligne,ligne)+=coeffij;
  //                                    matrice(ligne,colonne)-=coeffij;
  //                                 }
  //                             }
  //                         }
  //                     }
  //               }
  //         }
  //     }
}

//Correction pour le poreux : on rajoute la partie en  T div(u)
//Variable transportee : T
//Variable transportante : u
//REMARQUE : il ne FAUT SURTOUT PAS utiliser le tableau Kij car par
//construction celui-ci est telle que sum_{j} Kij =0 ce qui revient a
//imposer une vitesse a divergence nulle par element. Ce qui est
//problematique quand on est en compressible
void Op_Conv_Muscl_New_VEF_Face::ajouter_contribution_partie_compressible(const DoubleTab& transporte, const DoubleTab& vitesse_2,
                                                                          Matrice_Morse& matrice) const
{
  //   const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  //   const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  //   const IntTab& elem_faces=zone_VEF.elem_faces();
  //   const IntTab& face_voisins = zone_VEF.face_voisins();

  //   const DoubleTab& face_normales=zone_VEF.face_normales();

  //   const int nb_elem_tot=zone_VEF.nb_elem_tot();
  //   const int nb_faces_elem=elem_faces.dimension(1);
  //   const int nb_bord=zone_Cl_VEF.nb_cond_lim();

  //   //Pour tenir compte de la porosite
  //   const int marq = phi_u_transportant(equation());

  //   const DoubleVect& porosite_elem = zone_VEF.porosite_elem();
  //   const DoubleVect& porosite_face = zone_VEF.porosite_face();

  //   DoubleTab vitesse(vitesse_.valeur().valeurs());
  //   for (int i=0; i<vitesse.dimension(0); i++)
  //     for (int j=0; j<vitesse.dimension(1); j++)
  //       vitesse(i,j)*=porosite_face(i);

  //   int nb_comp=1;
  //   if (transporte.nb_dim()!=1) nb_comp=transporte.dimension(1);

  //   int elem=0,type_elem=0;
  //   int facei=0,facei_loc=0;
  //   int faceiAss=0,ind_faceiAss=0;
  //   int ligne=0;
  //   int dim=0;
  //   int ind_face=0;
  //   int num1=0,num2=0;
  //   int faceToComplete=0;
  //   int elem_loc=0;
  //   int n_bord=0;

  //   double coeff=0.;
  //   double signe=0.;
  //   double div=0.;

  //   double (*formule)(int);
  //   if (!volumes_etendus_)
  //     if (dimension==2) formule=&formule_Id_2D;
  //     else formule=&formule_Id_3D;
  //   else
  //     if (dimension==2) formule=&formule_2D;
  //     else formule=&formule_3D;

  //   for (elem=0; elem<nb_elem_tot; elem++)
  //     {
  //       //Type de l'element : le nombre de faces de Dirichlet
  //       //qu'il contient
  //       type_elem=elem_nb_faces_dirichlet_(elem);
  //       coeff=formule(type_elem);

  //       //Calcul de la divergence par element
  //       div=0.;
  //       for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
  //         {
  //           facei=elem_faces(elem,facei_loc);
  //           signe=(face_voisins(facei,0)==elem)? 1.:-1.;

  //           for (dim=0; dim<dimension; dim++)
  //             div+=signe*face_normales(facei,dim)*vitesse(facei,dim);
  //         }
  //       div*=coeff;
  //       if (!marq) div/=porosite_elem(elem);

  //       //Calcul de la partie compressible
  //       for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
  //         {
  //           facei=elem_faces(elem,facei_loc);

  //           for (dim=0; dim<nb_comp; dim++)
  //             {
  //               ligne=facei*nb_comp+dim;
  //               matrice(ligne,ligne)+=div;
  //             }
  //         }
  //     }

  //   //
  //   //Pour la periodicite
  //   //
  //   const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();
  //   for (n_bord=0; n_bord<nb_bord; n_bord++)
  //     {
  //       const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
  //       const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
  //       num1 = 0;
  //       num2=le_bord.nb_faces();//pour ne parcourir que les faces reelles

  //       if (sub_type(Periodique,la_cl.valeur()))
  //         {
  //           const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

  //           for (ind_face=num1; ind_face<num2; ind_face++)
  //               {
  //                 facei=le_bord.num_face(ind_face);
  //                 ind_faceiAss=la_cl_perio.face_associee(ind_face);
  //                 faceiAss=le_bord.num_face(ind_faceiAss);

  //                 //Pour ne parcourir qu'une seule fois les faces perio
  //                 if (facei<faceiAss)
  //                   for (elem_loc=0;elem_loc<2;elem_loc++)
  //                     {
  //                       elem=face_voisins(facei,elem_loc);
  //                       assert(elem!=-1);

  //                       //Calcul du numero local de la face dans "elem"
  //                       facei_loc=num_fac_loc(facei,elem_loc);
  //                       if (facei_loc!=-1)
  //                         faceToComplete=faceiAss;
  //                       else
  //                         {
  //                           faceToComplete=facei;
  //                           facei_loc=num_fac_loc(faceiAss,elem_loc);
  //                           assert(facei_loc!=-1);
  //                         }

  //                       //Type de l'element : le nombre de faces de Dirichlet
  //                       //qu'il contient
  //                       type_elem=elem_nb_faces_dirichlet_(elem);
  //                       coeff=formule(type_elem);

  //                       //Calcul de la divergence par element
  //                       div=0.;
  //                       for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
  //                         {
  //                           facei=elem_faces(elem,facei_loc);
  //                           signe=(face_voisins(facei,0)==elem)? 1.:-1.;

  //                           for (dim=0; dim<dimension; dim++)
  //                             div+=signe*face_normales(facei,dim)*vitesse(facei,dim);
  //                         }
  //                       div*=coeff;
  //                       if (!marq) div/=porosite_elem(elem);

  //                       //Calcul de la partie compressible
  //                       for (dim=0; dim<nb_comp; dim++)
  //                         {
  //                           ligne=faceToComplete*nb_comp+dim;
  //                           matrice(ligne,ligne)+=div;
  //                         }
  //                     }
  //               }
  //         }
  //     }
}

void Op_Conv_Muscl_New_VEF_Face::ajouter_contribution_antidiffusion(const DoubleTab& Kij, const DoubleTab& transporte,
                                                                    Matrice_Morse& matrice) const
{
  //   const Zone_VEF& zone_VEF=la_zone_vef.valeur();
  //   const Zone_Cl_VEF& zone_Cl_VEF = la_zcl_vef.valeur();

  //   const IntTab& elem_faces=zone_VEF.elem_faces();
  //   const IntTab& face_voisins = zone_VEF.face_voisins();

  //   const int nb_elem_tot=zone_VEF.nb_elem_tot();
  //   const int nb_faces_elem=elem_faces.dimension(1);
  //   const int nb_bord=zone_Cl_VEF.nb_cond_lim();

  //   int nb_comp=1;
  //   if (transporte.nb_dim()!=1) nb_comp=transporte.dimension(1);

  //   int elem=0;
  //   int elem_loc=0;
  //   int facei=0,facei_loc=0;
  //   int faceiAss=0;
  //   int ind_face=0,ind_faceiAss=0;
  //   int facej=0,facej_loc=0;
  //   int ligne=0,colonne=0;
  //   int dim=0;
  //   int face_amont=0,face_aval=0;
  //   int faceToComplete=0;
  //   int num1=0,num2=0;
  //   int n_bord=0;

  //   double kij=0.,kji=0.,dij=0.;
  //   double lij=0.,lji=0.;
  //   double daij=0.;
  //   double delta=0.;
  //   double coeffij=0.,coeffji=0.;
  //   double coeff=0.;
  //   double R=0.;

  //   //Pour le limiteur
  //   ArrOfDouble P_plus(nb_comp),P_moins(nb_comp);
  //   ArrOfDouble Q_plus(nb_comp),Q_moins(nb_comp);
  //   P_plus=0., P_moins=0.;
  //   Q_plus=0., Q_moins=0.;

  //   const DoubleVect& transporteV=ref_cast(DoubleVect,transporte);
  //   const IntTab& num_fac_loc = zone_VEF.get_num_fac_loc();
  //   for (elem=0; elem<nb_elem_tot; elem++)
  //     for (facei_loc=0; facei_loc<nb_faces_elem; facei_loc++)
  //       {
  //         facei=elem_faces(elem,facei_loc);

  //         P_plus=0., P_moins=0.;
  //         Q_plus=0., Q_moins=0.;
  //         calculer_senseur(Kij,transporteV,nb_comp,facei,elem_faces,face_voisins,num_fac_loc,P_plus,P_moins,Q_plus,Q_moins);
  //         for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
  //           if (facej_loc!=facei_loc)
  //             {
  //               facej=elem_faces(elem,facej_loc);

  //               kij = Kij(elem,facei_loc,facej_loc);
  //               kji = Kij(elem,facej_loc,facei_loc);
  //               dij = Dij(elem,facei_loc,facej_loc,Kij);
  //               lij = kij+dij;
  //               lji = kji+dij;
  //               assert(lij>=0);
  //               assert(lji>=0);

  //               if (lij<=lji) //facei est amont
  //                 {
  //                   face_amont = facei;
  //                   face_aval = facej;

  //                   //Si lij==lji, on passe deux foix dans la boucle
  //                   //d'ou la presence du coefficient 1/2
  //                   coeff = 1.*(lij<lji)+0.5*(lij==lji);
  //                   assert(coeff==1. || coeff==0.5);

  //                   for (dim=0; dim<nb_comp; dim++)
  //                     {
  //                       ligne=face_amont*nb_comp+dim;
  //                       colonne=face_aval*nb_comp+dim;

  //                       delta=transporteV[ligne]-transporteV[colonne];

  //                       //Limiteur de pente
  //                       if (delta>=0.) R=(P_plus(dim)==0.) ? 0. : Q_plus(dim)/P_plus(dim);
  //                       else  R=(P_moins(dim)==0.) ? 0. : Q_moins(dim)/P_moins(dim);

  //                       daij=minimum(limiteur(R)*dij,lji);
  //                       assert(daij>=0);
  //                       assert(daij<=lji);
  //                       coeffij=alpha_tab(face_amont)*beta(face_amont)*daij;
  //                       coeffji=alpha_tab(face_aval)*beta(face_aval)*daij;

  //                       //Calcul de la matrice
  //                       matrice(ligne,ligne)-=coeffij*coeff;
  //                       matrice(ligne,colonne)+=coeffij*coeff;
  //                       matrice(colonne,colonne)-=coeffji*coeff;
  //                       matrice(colonne,ligne)+=coeffji*coeff;
  //                     }
  //                 }
  //             }
  //       }

  //   //
  //   //Pour la periodicite
  //   //
  //   for (n_bord=0; n_bord<nb_bord; n_bord++)
  //     {
  //       const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
  //       const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
  //       num1 = 0;
  //       num2=le_bord.nb_faces();//pour ne parcourir que les faces reelles

  //       if (sub_type(Periodique,la_cl.valeur()))
  //         {
  //           const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());

  //           //Pour le limiteur
  //           ArrOfDouble Pj_plus(nb_comp),Pj_moins(nb_comp);
  //           ArrOfDouble Qj_plus(nb_comp),Qj_moins(nb_comp);
  //           Pj_plus=0., Pj_moins=0.;
  //           Qj_plus=0., Qj_moins=0.;

  //           for (ind_face=num1; ind_face<num2; ind_face++)
  //               {
  //                 facei=le_bord.num_face(ind_face);
  //                 ind_faceiAss=la_cl_perio.face_associee(ind_face);
  //                 faceiAss=le_bord.num_face(ind_faceiAss);

  //                 //Pour ne parcourir qu'une seule fois les faces perio
  //                 if (facei<faceiAss)
  //                   for (elem_loc=0;elem_loc<2;elem_loc++)
  //                     {
  //                       elem=face_voisins(facei,elem_loc);
  //                       assert(elem!=-1);

  //                       //Calcul du numero local de la face dans "elem"
  //                       facei_loc=num_fac_loc(facei,elem_loc);
  //                       if (facei_loc!=-1)
  //                         faceToComplete=faceiAss;
  //                       else
  //                         {
  //                           faceToComplete=facei;
  //                           facei_loc=num_fac_loc(faceiAss,elem_loc);
  //                           assert(facei_loc!=-1);
  //                         }

  //                       //Calcul du coefficient a rajouter dans la matrice
  //                       P_plus=0., P_moins=0.;
  //                       Q_plus=0., Q_moins=0.;
  //                       calculer_senseur(Kij,transporteV,nb_comp,faceToComplete,elem_faces,face_voisins,num_fac_loc,P_plus,P_moins,Q_plus,Q_moins);

  //                       for (facej_loc=0; facej_loc<nb_faces_elem; facej_loc++)
  //                         if (facej_loc!=facei_loc)
  //                           {
  //                             facej=elem_faces(elem,facej_loc);

  //                             kij = Kij(elem,facei_loc,facej_loc);
  //                             kji = Kij(elem,facej_loc,facei_loc);
  //                             dij = Dij(elem,facei_loc,facej_loc,Kij);
  //                             lij = kij+dij;
  //                             lji = kji+dij;
  //                             assert(lij>=0);
  //                             assert(lji>=0);

  //                             if (lij<=lji) //faceToComplete est amont
  //                               {
  //                                 face_amont=faceToComplete;
  //                                 face_aval=facej;

  //                                 //Si lij==lji, on passe deux foix dans la boucle
  //                                 //d'ou la presence du coefficient 1/2
  //                                 coeff = 1.*(lij<lji)+0.5*(lij==lji);
  //                                 assert(coeff==1. || coeff==0.5);

  //                                 for (dim=0; dim<nb_comp; dim++)
  //                                   {
  //                                     ligne=face_amont*nb_comp+dim;
  //                                     colonne=face_aval*nb_comp+dim;

  //                                     delta=transporteV[ligne]-transporteV[colonne];

  //                                     //Limiteur de pente
  //                                     if (delta>=0.) R=(P_plus(dim)==0.) ? 0. : Q_plus(dim)/P_plus(dim);
  //                                     else  R=(P_moins(dim)==0.) ? 0. : Q_moins(dim)/P_moins(dim);

  //                                     daij=minimum(limiteur(R)*dij,lji);
  //                                     assert(daij>=0);
  //                                     assert(daij<=lji);
  //                                     coeffij=alpha_tab(face_amont)*beta(face_amont)*daij;

  //                                     //Calcul de la matrice
  //                                     matrice(ligne,ligne)-=coeffij*coeff;
  //                                     matrice(ligne,colonne)+=coeffij*coeff;
  //                                   }
  //                               }
  //                             else  //faceToComplete est aval
  //                               {
  //                                 face_aval=faceToComplete;
  //                                 face_amont=facej;

  //                                 coeff=1.;

  //                                 Pj_plus=0., Pj_moins=0.;
  //                                 Qj_plus=0., Qj_moins=0.;
  //                                 calculer_senseur(Kij,transporteV,nb_comp,facej,elem_faces,face_voisins,num_fac_loc,Pj_plus,Pj_moins,Qj_plus,Qj_moins);

  //                                 for (dim=0; dim<nb_comp; dim++)
  //                                   {
  //                                     ligne=face_amont*nb_comp+dim;
  //                                     colonne=face_aval*nb_comp+dim;

  //                                     delta=transporteV[ligne]-transporteV[colonne];

  //                                     //Limiteur de pente
  //                                     if (delta>=0.) R=(Pj_plus(dim)==0.) ? 0. : Qj_plus(dim)/Pj_plus(dim);
  //                                     else  R=(Pj_moins(dim)==0.) ? 0. : Qj_moins(dim)/Pj_moins(dim);

  //                                     daij=minimum(limiteur(R)*dij,lij);
  //                                     assert(daij>=0);
  //                                     assert(daij<=lij);
  //                                     coeffij=alpha_tab(face_aval)*beta(face_aval)*daij;

  //                                     //Calcul de la matrice
  //                                     matrice(colonne,colonne)-=coeffij*coeff;
  //                                     matrice(colonne,ligne)+=coeffij*coeff;
  //                                   }
  //                               }
  //                           }
  //                     }
  //               }
  //         }
  //     }
}

void Op_Conv_Muscl_New_VEF_Face::test_implicite() const
{
}

