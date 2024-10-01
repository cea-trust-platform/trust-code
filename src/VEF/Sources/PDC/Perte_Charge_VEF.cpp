/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <Perte_Charge_VEF.h>
#include <Schema_Temps_base.h>
#include <Domaine_VEF.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <Domaine.h>
#include <Champ_Uniforme.h>
#include <Sous_Domaine.h>
#include <Sous_domaine_VF.h>
#include <Sous_domaine_dis_base.h>
#include <Champ_P1NC.h>
#include <Check_espace_virtuel.h>
#include <Champ_Don_Fonc_xyz.h>
#include <Champ_Don_Fonc_txyz.h>
#include <Param.h>

Implemente_base_sans_constructeur(Perte_Charge_VEF,"Perte_Charge_VEF",Source_base);

Perte_Charge_VEF::Perte_Charge_VEF():implicite_(1) { }

Sortie& Perte_Charge_VEF::printOn(Sortie& s ) const
{
  return s << que_suis_je() << finl;
}

Entree& Perte_Charge_VEF::readOn(Entree& is )
{
  Param param(que_suis_je());
  Cerr << que_suis_je() << "::readOn " << finl;
  lambda.setNbVar(4+dimension);
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  Cerr << "Interpretation de la fonction " << lambda.getString() << " ... ";
  lambda.parseString();
  Cerr << " Ok" << finl;
  if (diam_hydr->nb_comp()!=1)
    {
      Cerr << "Il faut definir le champ diam_hydr a une composante" << finl;
      exit();
    }
  return is;
}

void Perte_Charge_VEF::set_param(Param& param)
{
  param.ajouter_non_std("lambda",(this),Param::REQUIRED);
  param.ajouter("diam_hydr",&diam_hydr,Param::REQUIRED);
  param.ajouter_non_std("sous_domaine|sous_zone",(this));
  param.ajouter("implicite",&implicite_);
}

int Perte_Charge_VEF::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  if (mot=="lambda")
    {
      Nom tmp;
      is >> tmp;
      lambda.setString(tmp);
      lambda.addVar("Re");
      lambda.addVar("t");
      lambda.addVar("x");
      if (dimension>1)
        lambda.addVar("y");
      if (dimension>2)
        lambda.addVar("z");
      return 1;
    }
  else if (mot=="sous_domaine")
    {
      is >> nom_sous_domaine;
      sous_domaine=true;
      return 1;
    }
  else // non compris
    {
      Cerr << "Mot cle \"" << mot << "\" non compris lors de la lecture d'un "
           << que_suis_je() << finl;
      exit();
    }
  return -1;
}

////////////////////////////////////////////////////////////////
//                                                            //
//             Fonction principale : ajouter                  //
//                                                            //
////////////////////////////////////////////////////////////////

namespace
{
double valeur_a_la_face(const Champ_Don_base& le_ch, const IntTab& f_e, int f)
{
  assert(le_ch.valeurs().line_size() == 1);
  double s = 0.0, val = 0.0;
  for (int i = 0, e; i < 2; i++)
    if ((e = f_e(f, i)) > -1)
      {
        val += le_ch.valeurs()(e, 0);
        s += 1.0;
      }
  val /= s;
  return val;
}
}

DoubleTab& Perte_Charge_VEF::ajouter(DoubleTab& resu) const
{
  const Domaine_VEF& zvef=le_dom_VEF.valeur();
  const Champ_Don_base& nu=le_fluide->viscosite_cinematique(); // viscosite cinematique
  const DoubleTab& xv=zvef.xv() ;                 // centres de gravite des faces
  const DoubleTab& vit=la_vitesse->valeurs();
  // Sinon segfault a l'initialisation de ssz quand il n'y a pas de sous-domaine !
  const Sous_domaine_VF& ssz = sous_domaine ? le_sous_domaine_dis.valeur() : Sous_domaine_VF();
  const IntTab& f_e = zvef.face_voisins();

  // Parametres pour perte_charge()
  DoubleVect u(dimension);
  DoubleVect pos(dimension);
  DoubleVect v_valeur(dimension);
  // Optimisations pour les cas ou nu ou diam_hydr sont constants
  const bool nu_constant = sub_type(Champ_Uniforme, nu), dh_constant = sub_type(Champ_Uniforme, diam_hydr.valeur()),
             nu_xyz = sub_type(Champ_Don_Fonc_xyz, nu) || sub_type(Champ_Don_Fonc_txyz, nu),
             dh_xyz = sub_type(Champ_Don_Fonc_xyz, diam_hydr.valeur()) || sub_type(Champ_Don_Fonc_txyz, diam_hydr.valeur());

  // Le temps actuel
  double t=equation().schema_temps().temps_courant();

  // Nombre de faces a traiter.
  int max_faces = sous_domaine ? ssz.les_faces().size() : zvef.nb_faces();

  for (int face=0; face<max_faces; face++)
    {
      // indice de la face dans le domaine_VEF
      int la_face = sous_domaine ? ssz.les_faces()[face] : face;

      if (la_face<zvef.nb_faces())
        {
          // Recup la vitesse a la face, et en calcule le module
          double norme_u=0;
          for (int dim=0; dim<dimension; dim++)
            {
              u[dim]=vit(la_face,dim);
              norme_u+=u[dim]*u[dim];
            }
          norme_u=sqrt(norme_u) ;

          // Calcul de la position
          for (int i=0; i<dimension; i++)
            pos[i]=xv(la_face,i);

          const double nu_valeur = nu_constant ? nu.valeurs()(0,0) : (nu_xyz ? nu.valeur_a_compo(pos,0) : ::valeur_a_la_face(nu, f_e, la_face));
          const double dh_valeur = dh_constant ? diam_hydr->valeurs()(0,0) : (dh_xyz ? diam_hydr->valeur_a_compo(pos,0) : ::valeur_a_la_face(diam_hydr, f_e, la_face));

          // Calcul du reynolds
          double reynolds=norme_u*dh_valeur/nu_valeur;
          // Lambda est souvent indetermine pour Re->0
          if (reynolds < 1.e-10)
            reynolds=1e-10;

          // Calcul du volume d'integration
          double volume=sous_domaine?
                        ssz.volumes_entrelaces(face) :
                        zvef.volumes_entrelaces(la_face);
          volume*=equation().milieu().porosite_face(la_face);

          // Calcul du resultat final (ouf)
          double coeff_ortho,coeff_long,u_l;
          coeffs_perte_charge(u,pos,t,norme_u,dh_valeur,nu_valeur,reynolds,coeff_ortho,coeff_long,u_l,v_valeur);
          for (int dim=0; dim<dimension; dim++)
            {
              // La perte de charge vaut -coeff_long*u_l*v_valeur[dim] -coeff_ortho(u[dim] -u_v*v_valeur[dim])
              // soit -coeff_ortho* u[dim] - (coeff_long-coeff_ortho)* u_l*v_valeur[dim]
              resu(la_face,dim)-=(coeff_ortho* u[dim] + (coeff_long-coeff_ortho)* u_l*v_valeur[dim])*volume;
            }
        }
    }
  return resu;
}

/*! @brief copie de ajouter sauf la derniere ligne
 *
 */
void Perte_Charge_VEF::contribuer_a_avec(const DoubleTab& inco, Matrice_Morse& matrice) const
{
  if (implicite_==0) return; // La perte de charge n'est pas implicitee, on quitte

  // Raccourcis
  const Champ_Don_base& nu=le_fluide->viscosite_cinematique(); // viscosite cinematique
  const DoubleTab& xv=le_dom_VEF->xv() ;                     // centres de gravite des faces
  const DoubleTab& vit=la_vitesse->valeurs();
  // Sinon segfault a l'initialisation de ssz quand il n'y a pas de sous-domaine !
  const Sous_domaine_VF& ssz=sous_domaine?le_sous_domaine_dis.valeur():Sous_domaine_VF();
  const Domaine_VEF& zvef=le_dom_VEF.valeur();
  const IntTab& f_e = zvef.face_voisins();

  // Parametres pour perte_charge()
  DoubleVect u(dimension);
  double norme_u;
  double reynolds;
  DoubleVect pos(dimension);
  DoubleVect v_valeur(dimension);
  // Optimisations pour les cas ou nu ou diam_hydr sont constants
  const bool nu_constant = sub_type(Champ_Uniforme, nu), dh_constant = sub_type(Champ_Uniforme, diam_hydr.valeur()),
             nu_xyz = sub_type(Champ_Don_Fonc_xyz, nu) || sub_type(Champ_Don_Fonc_txyz, nu),
             dh_xyz = sub_type(Champ_Don_Fonc_xyz, diam_hydr.valeur()) || sub_type(Champ_Don_Fonc_txyz, diam_hydr.valeur());

  // Le temps actuel
  double t=equation().schema_temps().temps_courant();

  // Nombre de faces a traiter.
  int max_faces=sous_domaine?
                ssz.les_faces().size() :

                zvef.nb_faces();

  // To ensure that nu_valeur will not be zero
  // and not have a division by zero for the calculation of Reynolds
  assert_espace_virtuel_vect(nu.valeurs());

  for (int face=0; face<max_faces; face++)
    {

      // indice de la face dans le domaine_VEF
      int la_face=sous_domaine?
                  ssz.les_faces()[face] :
                  face;

      // Fixed bug by GF: si la face est une face virtuelle on passe, car nu mal calcule et coef(no,no) invalide
      if (la_face>=zvef.nb_faces())
        continue;

      // Recup la vitesse a la face, et en calcule le module
      norme_u=0;
      for (int dim=0; dim<dimension; dim++)
        {
          u[dim]=vit(la_face,dim);
          norme_u+=u[dim]*u[dim];
        }
      norme_u=sqrt(norme_u) ;

      // Calcul de la position
      for (int i=0; i<dimension; i++)
        pos[i]=xv(la_face,i);

      const double nu_valeur = nu_constant ? nu.valeurs()(0,0) : (nu_xyz ? nu.valeur_a_compo(pos,0) : ::valeur_a_la_face(nu, f_e, la_face));
      const double dh_valeur = dh_constant ? diam_hydr->valeurs()(0,0) : (dh_xyz ? diam_hydr->valeur_a_compo(pos,0) : ::valeur_a_la_face(diam_hydr, f_e, la_face));

      // Calcul du reynolds
      reynolds=norme_u*dh_valeur/nu_valeur;
      // Lambda est souvent indetermine pour Re->0
      if (reynolds < 1.e-10)
        reynolds=1e-10;

      // Calcul du volume d'integration
      double volume=sous_domaine?
                    ssz.volumes_entrelaces(face) :
                    zvef.volumes_entrelaces(la_face);
      volume*=equation().milieu().porosite_face(la_face);

      // Calcul du resultat final (ouf)
      double coeff_ortho,coeff_long,u_l;
      coeffs_perte_charge(u,pos,t,norme_u,dh_valeur,nu_valeur,reynolds,coeff_ortho,coeff_long,u_l,v_valeur);
      for (int dim=0; dim<dimension; dim++)
        {
          int n0=la_face*dimension+dim;
          // matrice.coef(n0,n0) += (coeff_ortho + (coeff_long-coeff_ortho)* v_valeur[dim]*v_valeur[dim]); // Fixed bug
          matrice.coef(n0,n0) += (coeff_ortho + (coeff_long-coeff_ortho)* v_valeur[dim]*v_valeur[dim])*volume;
          // La perte de charge vaut -coeff_long*u_l*v_valeur[dim] -coeff_ortho(u[dim] -u_v*v_valeur[dim])
          // soit -coeff_ortho* u[dim] - (coeff_long-coeff_orho)* u_l*v_valeur[dim]
          //      resu(la_face,dim)-=(coeff_ortho* u[dim] + (coeff_long-coeff_orho)* u_l*v_valeur[dim]);
        }
    }
}

DoubleTab& Perte_Charge_VEF::calculer(DoubleTab& resu) const
{
  resu=0;
  return ajouter(resu);
}


void Perte_Charge_VEF::completer()
{
  Source_base::completer();

  if (sous_domaine)
    {
      sous_domaine=false;
      const Sous_Domaine& le_sous_domaine=equation().probleme().domaine().ss_domaine(nom_sous_domaine);
      const Domaine_dis_base& le_domaine_dis=le_dom_VEF.valeur();
      for (int ssz=0; ssz<le_domaine_dis.nombre_de_sous_domaines_dis(); ssz++)
        {
          if (le_domaine_dis.sous_domaine_dis(ssz).sous_domaine().est_egal_a(le_sous_domaine))
            {
              sous_domaine=true;
              le_sous_domaine_dis=ref_cast(Sous_domaine_VF,le_domaine_dis.sous_domaine_dis(ssz));
            }
        }

      if(!sous_domaine)
        {
          Cerr << "On ne trouve pas le sous_domaine discretise associe a " << nom_sous_domaine << finl;
          exit();
        }
    }
}

////////////////////////////////////////////////////////////////
//                                                            //
//         Fonctions virtuelles pures de Source_base          //
//                                                            //
////////////////////////////////////////////////////////////////


void Perte_Charge_VEF::associer_pb(const Probleme_base& pb)
{
  la_vitesse = ref_cast(Champ_P1NC,equation().inconnue());
  le_fluide = ref_cast(Fluide_base,equation().milieu());
}

void Perte_Charge_VEF::associer_domaines(const Domaine_dis_base& domaine_dis,
                                         const Domaine_Cl_dis_base& domaine_Cl_dis)
{
  le_dom_VEF = ref_cast(Domaine_VEF, domaine_dis);
  le_dom_Cl_VEF = ref_cast(Domaine_Cl_VEF, domaine_Cl_dis);
}
