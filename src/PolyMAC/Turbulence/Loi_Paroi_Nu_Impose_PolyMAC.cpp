/****************************************************************************
* Copyright (c) 2019, CEA
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
// File:        Loi_Paroi_Nu_Impose_PolyMAC.cpp
// Directory:   $TRUST_ROOT/src/PolyMAC/Turbulence
// Version:     /main/19
//
//////////////////////////////////////////////////////////////////////////////

#include <Loi_Paroi_Nu_Impose_PolyMAC.h>
#include <Champ_Uniforme.h>
#include <Champ_Uniforme_Morceaux.h>
#include <Champ_Fonc_Tabule.h>
#include <Zone_Cl_PolyMAC.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Neumann_paroi.h>
#include <Debog.h>
#include <Fluide_Incompressible.h>
#include <Convection_Diffusion_std.h>
#include <Probleme_base.h>
#include <EcrFicPartage.h>
#include <Modele_turbulence_scal_base.h>
#include <Champ_Face_PolyMAC.h>

#include <cmath>

Implemente_instanciable(Loi_Paroi_Nu_Impose_PolyMAC,"Loi_Paroi_Nu_Impose_PolyMAC",Paroi_scal_hyd_base_PolyMAC);


//     printOn()
/////

Sortie& Loi_Paroi_Nu_Impose_PolyMAC::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

//// readOn
//

Entree& Loi_Paroi_Nu_Impose_PolyMAC::readOn(Entree& s)
{
  init_done = 0;
  // Definition des mots-cles
  Motcles les_mots(2);
  les_mots[0] = "nusselt";
  les_mots[1] = "diam_hydr";
  int nusselt_ok=-1;
  // Lecture et interpretation
  Motcle motlu, accolade_fermee="}", accolade_ouverte="{";
  s >> motlu;
  assert(motlu==accolade_ouverte);
  s >> motlu;
  if(motlu == accolade_ouverte)
    {
      // on passe l'accolade ouvrante
      s >> motlu;
    }
  while (motlu != accolade_fermee)
    {
      int rang=les_mots.search(motlu);
      switch(rang)
        {
        case 0 :   // nusselt expression
          {
            nusselt_ok=0;
            Nom tmp;
            s >> tmp;
            Cerr << "Lecture et interpretation de la fonction " << tmp << " ... ";
            nusselt.setNbVar(6+dimension);
            nusselt.setString(tmp);
            nusselt.addVar("Re");
            nusselt.addVar("Pr");
            nusselt.addVar("Pe");
            nusselt.addVar("t");
            nusselt.addVar("x");
            if (dimension>1)
              nusselt.addVar("y");
            if (dimension>2)
              nusselt.addVar("z");
            nusselt.addVar("Dh");
            nusselt.addVar("Lambda");
            nusselt.parseString();
            break;
          }
        case 1: // diam_hydr
          s >> diam_hydr;
          break;
        default : // non compris
          Cerr << "Mot cle \"" << motlu << "\" non compris lors de la lecture d'un "
               << que_suis_je() << endl;
          exit();
        }
      s >> motlu;
    }

  // Verification de la coherence
  if (nusselt_ok==-1)
    {
      Cerr << "Il faut definir l'expression nusselt(Re,Pr,x,y,z,Dh)" << endl;
      exit();
    }

  if (diam_hydr->nb_comp()!=1)
    {
      Cerr << "Il faut definir le champ diam_hydr a une composante" << endl;
      exit();
    }
  return s;
}


/////////////////////////////////////////////////////////////////////////
//
//    Implementation des fonctions de la classe Loi_Paroi_Nu_Impose_PolyMAC
//
////////////////////////////////////////////////////////////////////////

int  Loi_Paroi_Nu_Impose_PolyMAC::calculer_scal(Champ_Fonc_base& diffusivite_turb)
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const IntTab& face_voisins = zone.face_voisins();
  const Equation_base& eqn_hydr = mon_modele_turb_scal->equation().probleme().equation(0);
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible,eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& xv = zone.xv(), &xp = zone.xp(), &nf = zone.face_normales();
  const DoubleVect& fs = zone.face_surfaces();
  DoubleVect pos(dimension);

  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());
  int l_unif;
  double visco=-1;
  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      l_unif = 1;
      visco = max(tab_visco(0,0),DMINFLOAT);
    }
  else
    l_unif = 0;

  if ((!l_unif) && (local_min_vect(tab_visco)<DMINFLOAT))
    //   on ne doit pas changer tab_visco ici !
    {
      Cerr<<" visco <=0 ?"<<finl;
      exit();
    }
  //tab_visco+=DMINFLOAT;

  bool dh_constant=sub_type(Champ_Uniforme,diam_hydr.valeur())?true:false;
  double dh_valeur= dh_constant ? diam_hydr(0,0) : 0;



  int ndeb,nfin;
  int elem;
  double d_visco;

  //  const Convection_Diffusion_std& eqn = mon_modele_turb_scal->equation();
  const Champ_Don& alpha = le_fluide.diffusivite();



  /* vitesses aux elements */
  DoubleTrav ve(zone.nb_elem(), dimension);
  const Champ_Face_PolyMAC& ch = ref_cast(Champ_Face_PolyMAC, eqn_hydr.inconnue().valeur());
  ch.interp_ve(ch.valeurs(), ve);

  // Boucle sur les bords:
  for (int n_bord=0, idx = 0; n_bord<zone.nb_front_Cl(); n_bord++)
    {

      // Pour chaque condition limite on regarde son type
      // On applique les lois de paroi thermiques uniquement
      // aux voisinages des parois ou l'on impose la temperature

      const Cond_lim& la_cl = la_zone_Cl_PolyMAC->les_conditions_limites(n_bord);
      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
           || (sub_type(Dirichlet_paroi_defilante,la_cl.valeur())) )
        {

          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          //find the associated boundary
          int boundary_index=-1;
          if (zone.front_VF(n_bord).le_nom() == le_bord.le_nom())
            boundary_index=n_bord;
          assert(boundary_index >= 0);

          for (int num_face=ndeb; num_face<nfin; num_face++, idx++)
            {
              elem = face_voisins(num_face,0);
              if (elem == -1)
                elem = face_voisins(num_face,1);
              if (l_unif)
                d_visco = visco;
              else
                d_visco = tab_visco[elem];

              double d_alpha=0.;
              if (sub_type(Champ_Uniforme,alpha.valeur()))
                d_alpha = alpha(0,0);
              else
                {
                  if (alpha.nb_comp()==1)
                    d_alpha = alpha(elem);
                  else
                    d_alpha = alpha(elem,0);
                }
              double Pr = d_visco/d_alpha;

              //norme de la vitesse tangentielle a la face de bord
              double vn = zone.dot(&ve(elem, 0), &nf(num_face, 0)) / fs(num_face),
                     Ud = sqrt(max(zone.dot(&ve(elem, 0), &ve(elem, 0)) - vn * vn, 0.));

              // Calcul de la position
              for (int i=0; i<dimension; i++)
                pos[i]=xp(elem,i);

              // Calcul du diametre hydraulique
              if (!dh_constant)
                {
                  dh_valeur=diam_hydr->valeur_a_compo(pos,0);
                }
              if (!dh_valeur)
                dh_valeur = sqrt(zone.dot(&xv(num_face, 0), &xv(num_face, 0), &xp(elem, 0), &xp(elem, 0)));
              double Re=max(Ud*dh_valeur/d_visco, 1e-8);

              nusselt.setVar("Re",Re);
              nusselt.setVar("Pr",Pr);
              nusselt.setVar("Pe",Re * Pr);
              nusselt.setVar("Dh",dh_valeur);
              //nusselt->setVar("Lambda",d_alpha);
              //        nusselt->setVar("t",Pr);
              for (int i=0; i<dimension; i++)
                {
                  nusselt.setVar(4+i,pos[i]);
                }
              double Nu = nusselt.eval();

              // L'expression de d_equiv ne tient pas compte de alpha_t comme en VEF
              // Cela dit, c'est normale car c'est lors du calcul du flux que la
              // turbulence est prise en compte.
              // Ne pas uniformiser l'ecriture avec le VEF, car on tombe sur des problemes
              // au niveau des parois contacts entre plusieurs problemes (alpha_t pas recuperable !).
              int global_face=num_face;
              int local_face=zone.front_VF(boundary_index).num_local_face(global_face);
              assert(dh_valeur/Nu>-DMAXFLOAT && dh_valeur/Nu<DMAXFLOAT);
              equivalent_distance_[boundary_index](local_face)=dh_valeur/Nu;
              //Cout << "pos = " << pos[0] << " " << pos[1] << finl;
              //Cout << "Nu = " << Nu << finl;
              //Cout << "diam = " << dh_valeur << finl;
              //Cout << "tab_d_equiv_ = " << tab_d_equiv_[num_face] << finl;
            }
        }
    }
  return 1;
}



void Loi_Paroi_Nu_Impose_PolyMAC::imprimer_nusselt(Sortie&) const
{
  const Zone_PolyMAC& zone = la_zone_PolyMAC.valeur();
  const IntTab& face_voisins = zone.face_voisins();
  int ndeb,nfin,elem;
  const Convection_Diffusion_std& eqn = mon_modele_turb_scal->equation();
  const Equation_base& eqn_hydr = eqn.probleme().equation(0);
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& conductivite = le_fluide.conductivite();
  const Champ_Don& masse_volumique = le_fluide.masse_volumique();
  const Champ_Don& capacite_calorifique = le_fluide.capacite_calorifique();
  const DoubleTab& temperature = eqn.probleme().equation(1).inconnue().valeurs();

  EcrFicPartage Nusselt;
  ouvrir_fichier_partage(Nusselt,"Nusselt");

  bool dh_constant=sub_type(Champ_Uniforme,diam_hydr.valeur())?true:false;
  double dh_valeur = dh_constant ? diam_hydr(0,0) : 0;
  DoubleVect pos(dimension);

  for (int n_bord=0; n_bord<zone.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = la_zone_Cl_PolyMAC->les_conditions_limites(n_bord);

      if ( (sub_type(Dirichlet_paroi_fixe,la_cl.valeur())) ||
           (sub_type(Dirichlet_paroi_defilante,la_cl.valeur()) ))
        {
          const Zone_Cl_PolyMAC& zone_Cl_PolyMAC_th = ref_cast(Zone_Cl_PolyMAC, eqn.probleme().equation(1).zone_Cl_dis().valeur());
          const Cond_lim& la_cl_th = zone_Cl_PolyMAC_th.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());

          //find the associated boundary
          int boundary_index=-1;
          if (zone.front_VF(n_bord).le_nom() == le_bord.le_nom())
            boundary_index=n_bord;
          assert(boundary_index >= 0);

          if ( (sub_type(Neumann_paroi,la_cl_th.valeur())))
            {
              const Neumann_paroi& la_cl_neum = ref_cast(Neumann_paroi,la_cl_th.valeur());

              if(je_suis_maitre())
                {
                  Nusselt << finl;
                  Nusselt << "Bord " << le_bord.le_nom() << finl;
                  if (dimension == 2)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "\tFace a\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "X\t\t| Y\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)\t| Tparoi equiv.(K)" << finl;
                      Nusselt << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                    }
                  if (dimension == 3)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "\tFace a\t\t\t\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "X\t\t| Y\t\t\t| Z\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)\t| Tparoi equiv.(K)" << finl;
                      Nusselt <<        "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                    }
                }
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  // Calcul de la position
                  for (int i=0; i<dimension; i++)
                    pos[i]=zone.xv(num_face,i);

                  // Calcul du diametre hydraulique
                  if (!dh_constant)
                    dh_valeur=diam_hydr->valeur_a_compo(pos,0);

                  double x=zone.xv(num_face,0);
                  double y=zone.xv(num_face,1);
                  double lambda;
                  double rho,Cp;

                  elem = face_voisins(num_face,0);
                  if (elem == -1)
                    elem = face_voisins(num_face,1);
                  if (sub_type(Champ_Uniforme,conductivite.valeur()))
                    lambda = conductivite(0,0);
                  else
                    {
                      if (conductivite.nb_comp()==1)
                        lambda = conductivite(elem);
                      else
                        lambda = conductivite(elem,0);
                    }
                  if (sub_type(Champ_Uniforme,masse_volumique.valeur()))
                    rho = masse_volumique(0,0);
                  else
                    {
                      if (masse_volumique.nb_comp()==1)
                        rho = masse_volumique(elem);
                      else
                        rho = masse_volumique(elem,0);
                    }
                  if (sub_type(Champ_Uniforme,capacite_calorifique.valeur()))
                    Cp = capacite_calorifique(0,0);
                  else
                    {
                      if (capacite_calorifique.nb_comp()==1)
                        Cp = capacite_calorifique(elem);
                      else
                        Cp = capacite_calorifique(elem,0);
                    }

                  if (dimension == 2)
                    Nusselt << x << "\t| " << y;
                  if (dimension == 3)
                    {
                      double z=zone.xv(num_face,2);
                      Nusselt << x << "\t| " << y << "\t| " << z;
                    }

                  int global_face=num_face;
                  int local_face=zone.front_VF(boundary_index).num_local_face(global_face);

                  double flux = la_cl_neum.flux_impose(num_face-ndeb)*rho*Cp;
                  double tparoi = temperature(elem)+flux/lambda*equivalent_distance_[boundary_index](local_face);

                  Nusselt << "\t| " << equivalent_distance_[boundary_index](local_face) << "\t| " << dh_valeur/equivalent_distance_[boundary_index](local_face) << "\t| "
                          << lambda/equivalent_distance_[boundary_index](local_face) << "\t| " << temperature(elem) << "\t|" << tparoi << finl;
                }
            }
          // fin de condition limite flux impose
          else
            {
              if(je_suis_maitre())
                {
                  Nusselt << finl;
                  Nusselt << "Bord " << le_bord.le_nom() << finl;
                  if (dimension == 2)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "\tFace a\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "X\t\t| Y\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)" << finl;
                      Nusselt << "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                    }
                  if (dimension == 3)
                    {
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "\tFace a\t\t\t\t\t\t\t|" << finl;
                      Nusselt << "----------------------------------------------------------------------------------------------------------------------------------------" << finl;
                      Nusselt << "X\t\t| Y\t\t\t| Z\t\t\t| dist. carac. (m)\t| Nusselt (local)\t| h (Conv. W/m2/K)\t| Tf cote paroi (K)" << finl;
                      Nusselt <<        "----------------|-----------------------|-----------------------|-----------------------|-----------------------|-----------------------" << finl;
                    }
                }
              ndeb = le_bord.num_premiere_face();
              nfin = ndeb + le_bord.nb_faces();
              for (int num_face=ndeb; num_face<nfin; num_face++)
                {
                  // Calcul de la position
                  for (int i=0; i<dimension; i++)
                    pos[i]=zone.xv(num_face,i);

                  // Calcul du diametre hydraulique
                  if (!dh_constant)
                    dh_valeur=diam_hydr->valeur_a_compo(pos,0);

                  double x=zone.xv(num_face,0);
                  double y=zone.xv(num_face,1);
                  double lambda;
                  elem = face_voisins(num_face,0);
                  if (elem == -1)
                    elem = face_voisins(num_face,1);
                  if (sub_type(Champ_Uniforme,conductivite.valeur()))
                    lambda = conductivite(0,0);
                  else
                    {
                      if (conductivite.nb_comp()==1)
                        lambda = conductivite(elem);
                      else
                        lambda = conductivite(elem,0);
                    }
                  if (dimension == 2)
                    Nusselt << x << "\t| " << y;
                  if (dimension == 3)
                    {
                      double z=zone.xv(num_face,2);
                      Nusselt << x << "\t| " << y << "\t| " << z;
                    }
                  int global_face=num_face;
                  int local_face=zone.front_VF(boundary_index).num_local_face(global_face);
                  Nusselt << "\t| " << equivalent_distance_[boundary_index](local_face) << "\t| "
                          << dh_valeur/equivalent_distance_[boundary_index](local_face) << "\t| "
                          << lambda/equivalent_distance_[boundary_index](local_face) << "\t| " << temperature(elem) << finl;
                }
            }
          Nusselt.syncfile();
        }
    }
  if(je_suis_maitre())
    Nusselt << finl << finl;
  Nusselt.syncfile();
}

