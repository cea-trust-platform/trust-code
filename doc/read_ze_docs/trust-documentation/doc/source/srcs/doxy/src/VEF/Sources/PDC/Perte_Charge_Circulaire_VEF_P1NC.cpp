/****************************************************************************
* Copyright (c) 2023, CEA
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

#include <Perte_Charge_Circulaire_VEF_P1NC.h>
#include <Motcle.h>
#include <Equation_base.h>

#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Sous_domaine_VF.h>
#include <Domaine_VEF.h>
#include <Matrice_Morse.h>
#include <Schema_Temps_base.h>
#include <TRUSTTrav.h>
#include <Param.h>

Implemente_instanciable(Perte_Charge_Circulaire_VEF_P1NC,"Perte_Charge_Circulaire_VEF_P1NC",Perte_Charge_VEF);


Sortie& Perte_Charge_Circulaire_VEF_P1NC::printOn(Sortie& s ) const
{
  return s << que_suis_je() << finl;
}

////////////////////////////////////////////////////////////////
//                                                            //
//                         readOn                             //
//                                                            //
////////////////////////////////////////////////////////////////

Entree& Perte_Charge_Circulaire_VEF_P1NC::readOn(Entree& s )
{
  Perte_Charge_VEF::readOn(s);
  if (v->nb_comp()!=dimension)
    {
      Cerr << "Il faut definir le champ direction a " << dimension << " composantes" << finl;
      exit();
    }
  return s;
}

void Perte_Charge_Circulaire_VEF_P1NC::set_param(Param& param)
{
  Perte_Charge_VEF::set_param(param);
  param.ajouter_non_std("lambda_ortho",(this),Param::REQUIRED);
  param.ajouter("diam_hydr_ortho",&diam_hydr_ortho,Param::REQUIRED);
  param.ajouter("direction",&v,Param::REQUIRED);
}

int Perte_Charge_Circulaire_VEF_P1NC::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="lambda")
    {
      lambda.addVar("Re_tot");
      lambda.addVar("Re_long");
      Perte_Charge_VEF::lire_motcle_non_standard(mot,is);
      return 1;
    }
  else if (mot=="lambda_ortho")
    {
      Nom tmp;
      is >> tmp;
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
      return 1;
    }
  else
    {
      return Perte_Charge_VEF::lire_motcle_non_standard(mot,is);
    }
}


////////////////////////////////////////////////////////////////
//                                                            //
//           Fonction principale : perte_charge               //
//                                                            //
////////////////////////////////////////////////////////////////

void  Perte_Charge_Circulaire_VEF_P1NC::coeffs_perte_charge(const DoubleVect& u, const DoubleVect& pos,
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

  double u_ortho=sqrt(std::max(norme_u*norme_u-u_l*u_l,0.0));
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
  lambda.setVar(3,t);
  lambda.setVar(4,pos[0]);
  if (dimension>1)
    lambda.setVar(5,pos[1]);
  if (dimension>2)
    lambda.setVar(6,pos[2]);

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
  void Perte_Charge_Circulaire_VEF_P1NC::perte_charge(const DoubleVect& u, const DoubleVect& pos,
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

  void Perte_Charge_Circulaire_VEF_P1NC::coef_implicite_perte_charge(const DoubleVect& u, const DoubleVect& pos,
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




  void  Perte_Charge_Circulaire_VEF_P1NC::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
  {
  if (pas_implicite_) return;
  // A factoriser plus tard....
  // copie de Perte_Charge_VEF::ajouter
  // Raccourcis
  const Champ_Don& nu=le_fluide->viscosite_cinematique(); // viscosite cinematique
  const DoubleTab& xv=le_dom_VEF->xv() ;                     // centres de gravite des faces
  const DoubleTab& vit=la_vitesse->valeurs();
  // Sinon segfault a l'initialisation de ssz quand il n'y a pas de sous-domaine !
  const Sous_domaine_VF& ssz=sous_domaine?le_sous_domaine_dis.valeur():Sous_domaine_VF();
  const Domaine_VEF& zvef=le_dom_VEF.valeur();

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
  int max_faces=sous_domaine?
  ssz.les_faces().size() :
  zvef.nb_faces_tot();

  for (int face=0;face<max_faces;face++) {

  // indice de la face dans le domaine_VEF
  int la_face=sous_domaine?
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
  double volume=sous_domaine?
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
