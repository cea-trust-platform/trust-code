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
// File:        Perte_Charge_Circulaire_PolyMAC_Face.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Sources
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////

#include <Perte_Charge_Circulaire_PolyMAC_Face.h>
#include <Motcle.h>
#include <Equation_base.h>

#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Sous_zone_VF.h>
#include <Zone_PolyMAC.h>
#include <Matrice_Morse.h>
#include <Schema_Temps_base.h>
#include <TRUSTTrav.h>

Implemente_instanciable(Perte_Charge_Circulaire_PolyMAC_Face,"Perte_Charge_Circulaire_Face_PolyMAC|Perte_Charge_Circulaire_Face_PolyMAC_V2",Perte_Charge_PolyMAC);


Sortie& Perte_Charge_Circulaire_PolyMAC_Face::printOn(Sortie& s ) const
{
  return s << que_suis_je() << finl;
}

////////////////////////////////////////////////////////////////
//                                                            //
//                         readOn                             //
//                                                            //
////////////////////////////////////////////////////////////////

Entree& Perte_Charge_Circulaire_PolyMAC_Face::readOn(Entree& s )
{
  Cerr << "Perte_Charge_Circulaire_PolyMAC_Face::readOn " << finl;
  sous_zone=false;
  int lambda_ortho_ok=0;
  int lambda_ok=0;
  // Definition des mots-cles
  Motcles les_mots(7);
  les_mots[0] = "lambda";
  les_mots[1] = "diam_hydr";
  les_mots[2] = "sous_zone";
  les_mots[3] = "direction";
  les_mots[4] = "lambda_ortho";
  les_mots[5] = "diam_hydr_ortho";
  les_mots[6] = "implicite";

  // Lecture et interpretation
  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  s >> motlu;
  while (motlu != accolade_ouverte)
    {
      Cerr << "On attendait une { a la lecture d'un " << que_suis_je() << finl;
      Cerr << "et non : " << motlu << finl;
      exit();
    }
  s >> motlu;
  while (motlu != accolade_fermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :   // lambda
          {
            lambda_ok=1;
            Nom tmp;
            s >> tmp;
            Cerr << "Lecture et interpretation de la fonction " << tmp << " ... ";
            lambda.setNbVar(3+dimension);
            lambda.setString(tmp);
            lambda.addVar("Re_tot");
            lambda.addVar("Re_long");
            lambda.addVar("t");
            lambda.addVar("x");
            if (dimension>1)
              lambda.addVar("y");
            if (dimension>2)
              lambda.addVar("z");
            lambda.parseString();
            Cerr << " Ok" << finl;
            break;
          }
        case 1: // diam_hydr
          s >> diam_hydr;
          break;
        case 5: // diam_hydr_ortho
          s >> diam_hydr_ortho;
          break;
        case 2: // sous_zone
          s >> nom_sous_zone;
          sous_zone=true;
          break;
        case 3: // direction
          s >> v;
          break;
        case 4:   // lambda_ortho
          {
            lambda_ortho_ok=1;
            Nom tmp;
            s >> tmp;
            Cerr << "Lecture et interpretation de la fonction " << tmp << " ... ";
            lambda_ortho.setNbVar(3+dimension);
            lambda_ortho.setString(tmp);
            lambda_ortho.addVar("Re_tot");
            lambda_ortho.addVar("Re_ortho");
            lambda_ortho.addVar("t");
            lambda_ortho.addVar("x");
            if (dimension>1)
              lambda_ortho.addVar("y");
            if (dimension>2)
              lambda_ortho.addVar("z");
            lambda_ortho.parseString();
            Cerr << " Ok" << finl;
            break;
          }
        case 6:
          {
            s>>implicite_;
            break;
          }
        default : // non compris
          Cerr << "Mot cle \"" << motlu << "\" non compris lors de la lecture d'un "
               << que_suis_je() << finl;
          exit();
        }
      s >> motlu;
    }

  // Verification de la coherence
  if (lambda_ok==0)
    {
      Cerr << "Il faut definir l'expression lamba(Re_long,Re_tot)" << finl;
      exit();
    }

  if (lambda_ortho_ok==0)
    {
      Cerr << "Il faut definir l'expression lamba_ortho(Re_ortho,Re_tot)" << finl;
      exit();
    }

  if  ((!diam_hydr.non_nul())|| (diam_hydr->nb_comp()!=1))
    {
      Cerr << "Il faut definir le champ diam_hydr a une composante" << finl;
      exit();
    }
  if ((!diam_hydr_ortho.non_nul())|| (diam_hydr_ortho->nb_comp()!=1))
    {
      Cerr << "Il faut definir le champ diam_hydr_ortho a une composante" << finl;
      exit();
    }
  if (v->nb_comp()!=dimension)
    {
      Cerr << "Il faut definir le champ direction a " << dimension << " composantes" << finl;
      exit();
    }


  Cerr << "Fin de Perte_Charge_Circulaire_PolyMAC_Face::readOn" << finl;
  return s;
}

////////////////////////////////////////////////////////////////
//                                                            //
//           Fonction principale : perte_charge               //
//                                                            //
////////////////////////////////////////////////////////////////

void  Perte_Charge_Circulaire_PolyMAC_Face::coeffs_perte_charge(const DoubleVect& u, const DoubleVect& pos,
                                                                double t, double norme_u, double dh, double nu, double reynolds,double& coeff_ortho,double& coeff_long,double& u_l,DoubleVect& av_valeur) const
{

  // calcul de dh_ortho
  double dh_ortho=diam_hydr_ortho->valeur_a_compo(pos,0);

  // calcul de u.d/||d||
  // Calcul de v et ||v||^2
  av_valeur.resize(dimension);

  v.valeur().valeur_a(pos,av_valeur);
  // on norme v
  {
    double vcarre=0;
    for (int dim=0; dim<dimension; dim++)
      vcarre+=av_valeur[dim]*av_valeur[dim];
    av_valeur/=sqrt(vcarre);
  }
  // Calcul de u.v/||v||
  u_l=0;

  for (int dim=0; dim<dimension; dim++)
    u_l+=u[dim]*av_valeur[dim];

  double u_ortho=sqrt(norme_u*norme_u-u_l*u_l);
  // calcule de Re_l et Re_ortho
  // Calcul du reynolds
  /* PL: To avoid a possible division by zero, we replace:
     double nu=norme_u*dh/reynolds;
     double Re_l=std::fabs(u_l)*dh/nu; */
  // By:
  double Re_l=dh*std::fabs(u_l)/nu;
  if(Re_l<1e-10) Re_l=1e-10;
  // PL: To avoid a possible division by zero, we replace:
  /* double Re_ortho=u_ortho*dh_ortho/nu; */
  // By:
  double Re_ortho=dh_ortho*u_ortho/nu;
  if (Re_ortho<1e-10) Re_ortho=1e-10;
  // Calcul de lambda
  lambda.setVar(0,reynolds);
  lambda.setVar(1,Re_l);
  lambda.setVar(2,t);
  lambda.setVar(3,pos[0]);
  if (dimension>1)
    lambda.setVar(4,pos[1]);
  if (dimension>2)
    lambda.setVar(5,pos[2]);

  // Calcul de lambda_ortho
  lambda_ortho.setVar(0,reynolds);
  lambda_ortho.setVar(1,Re_ortho);
  lambda_ortho.setVar(2,t);
  lambda_ortho.setVar(3,pos[0]);
  if (dimension>1)
    lambda_ortho.setVar(4,pos[1]);
  if (dimension>2)
    lambda_ortho.setVar(5,pos[2]);
  double l_ortho=lambda_ortho.eval(); // Pour ne pas evaluer 2 fois le parser
  double l_long=lambda.eval();
  coeff_ortho=l_ortho*u_ortho/2./dh_ortho;
  coeff_long=l_long *std::fabs(u_l)    /2./dh    ;
}
/*
  void Perte_Charge_Circulaire_PolyMAC_Face::perte_charge(const DoubleVect& u, const DoubleVect& pos,
  double t, double norme_u, double dh, double reynolds,DoubleVect& p_charge) const
  {
  double coeff_ortho,coeff_long,u_l; //DoubleVect v_valeur(dimension);
  calculer_coeffs(u,pos,t,norme_u,dh,reynolds, coeff_ortho, coeff_long,u_l,v_valeur) ;
  // Calcul du resultat
  for (int dim=0;dim<dimension;dim++)
  p_charge[dim] =
  -coeff_ortho * (u[dim]-u_l*v_valeur[dim])
  -coeff_long * u_l*v_valeur[dim];
  }

  void Perte_Charge_Circulaire_PolyMAC_Face::coef_implicite_perte_charge(const DoubleVect& u, const DoubleVect& pos,
  double t, double norme_u, double dh, double reynolds,DoubleVect& p_charge) const
  {
  double coeff_ortho,coeff_long,u_l; //DoubleVect v_valeur(dimension);
  calculer_coeffs(u,pos,t,norme_u,dh,reynolds, coeff_ortho, coeff_long,u_l,v_valeur) ;
  // Calcul du resultat

  for (int dim=0;dim<dimension;dim++)
  {
  //p_charge[dim] =
  //  -coeff_ortho * (u[dim]-u_l*v_valeur[dim])
  // -coeff_long * u_l*v_valeur[dim];
  // du_l/u[dim]= v_valeur[dim]

  p_charge[dim] =
  coeff_ortho * (1-v_valeur[dim]*v_valeur[dim])
  +coeff_long *v_valeur[dim]*v_valeur[dim];
  }
  }




  void  Perte_Charge_Circulaire_PolyMAC_Face::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
  {
  if (pas_implicite_) return;
  // A factoriser plus tard....
  // copie de Perte_Charge_PolyMAC::ajouter
  // Raccourcis
  const Champ_Don& nu=le_fluide->viscosite_cinematique(); // viscosite cinematique
  const DoubleTab& xv=la_zone_PolyMAC->xv() ;                     // centres de gravite des faces
  const DoubleTab& vit=la_vitesse->valeurs();
  // Sinon segfault a l'initialisation de ssz quand il n'y a pas de sous-zone !
  const Sous_zone_VF& ssz=sous_zone?la_sous_zone_dis.valeur():Sous_zone_VF();
  const Zone_PolyMAC& zvef=la_zone_PolyMAC.valeur();

  // Parametres pour perte_charge()
  static DoubleVect u(dimension);
  double norme_u;
  double dh_valeur;
  double reynolds;
  static DoubleVect p_charge(dimension);
  static DoubleVect pos(dimension);

  // Optimisations pour les cas ou nu ou diam_hydr sont constants
  bool nu_constant=sub_type(Champ_Uniforme,nu.valeur())?true:false;
  double nu_valeur;
  //if (nu_constant)
  ////nu_valeur=nu(0,0);

  bool dh_constant=sub_type(Champ_Uniforme,diam_hydr.valeur())?true:false;
  //if (dh_constant)
  ////dh_valeur=diam_hydr(0,0);

  // Le temps actuel
  double t=equation().schema_temps().temps_courant();

  // Nombre de faces a traiter.
  int max_faces=sous_zone?
  ssz.les_faces().size() :
  zvef.nb_faces_tot();

  for (int face=0;face<max_faces;face++) {

  // indice de la face dans la zone_PolyMAC
  int la_face=sous_zone?
  ssz.les_faces()[face] :
  face;

  // Recup la vitesse a la face, et en calcule le module
  norme_u=0;
  for (int dim=0;dim<dimension;dim++) {
  u[dim]=vit(la_face,dim);
  norme_u+=u[dim]*u[dim];
  }
  norme_u=sqrt(norme_u) ;

  // Calcul de la position
  for (int i=0;i<dimension;i++)
  pos[i]=xv(la_face,i);

  // Calcul de nu
  if (!nu_constant) {
  nu_valeur=nu->valeur_a_compo(pos,0);
  } else nu_valeur=nu(0,0);

  // Calcul du diametre hydraulique
  if (!dh_constant) {
  dh_valeur=diam_hydr->valeur_a_compo(pos,0);
  }
  else dh_valeur=diam_hydr(0,0);

  // Calcul du reynolds
  reynolds=norme_u*dh_valeur/nu_valeur;
  // Lambda est souvent indetermine pour Re->0
  if (reynolds < 1.e-10)
  reynolds=1e-10;

  // Calcul du volume d'integration
  double volume=sous_zone?
  ssz.volumes_entrelaces(face) :
  zvef.volumes_entrelaces(la_face);
  volume*=zvef.porosite_face(la_face);

  // fin de copie....


  coef_implicite_perte_charge (u,pos,t,norme_u,dh_valeur,reynolds,p_charge);
  for (int dim=0;dim<dimension;dim++) {
  int n0=la_face*dimension+dim;
  matrice.coef(n0,n0) += p_charge[dim]*volume;
  }

  }
  // test
  if (0)
  {
  DoubleTrav resu(inco);
  ajouter(resu);
  //Cerr<<" test0 " <<max_abs(resu)<<finl<<resu<<finl;
  matrice.ajouter_multvect(inco,resu);
  equation().solv_masse().appliquer(resu);
  Cerr<<" test " <<mp_max_abs_vect(resu)<<finl;
  }
  }
*/
