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
// File:        Traitement_particulier_NS_Brech_VEF.cpp
// Directory:   $TRUST_ROOT/src/VEF
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Traitement_particulier_NS_Brech_VEF.h>
#include <Navier_Stokes_std.h>
#include <Fluide_Incompressible.h>
#include <Probleme_base.h>
#include <DoubleTrav.h>
#include <Periodique.h>
#include <Champ_P1NC.h>

Implemente_instanciable_sans_constructeur(Traitement_particulier_NS_Brech_VEF,"Traitement_particulier_NS_Brech_VEF",Traitement_particulier_NS_VEF);

Traitement_particulier_NS_Brech_VEF::Traitement_particulier_NS_Brech_VEF()
{
  /*
    Noms& nom=champs_compris_.liste_noms_compris();
    nom.dimensionner(2);
    nom[0]="Pression_porosite";
    nom[1]="Richardson";
  */
}
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
Sortie& Traitement_particulier_NS_Brech_VEF::printOn(Sortie& is) const
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
Entree& Traitement_particulier_NS_Brech_VEF::readOn(Entree& is)
{
  return is;
}
/*
  inline int Traitement_particulier_NS_Brech_VEF::comprend_champ(const Motcle& mot) const
  {
  if ( (mot == "Richardson") || (mot == "Pression_porosite") ) return 1 ;
  return 0 ;
  }

  int Traitement_particulier_NS_Brech_VEF::a_pour_Champ_Fonc(const Motcle& mot,
  REF(Champ_base)& ch_ref) const
  {
  if (mot == "Richardson")
  {
  ch_ref = ch_ri;
  return 1 ;
  } else if (mot == "Pression_porosite")
  {
  ch_ref = ch_p;
  return 1 ;
  }
  return 0 ;
  }
*/
Entree& Traitement_particulier_NS_Brech_VEF::lire(Entree& is)
{
  Motcle accouverte = "{" , accfermee = "}" ;
  Motcle motbidon, motlu;

  c_flux = 0 ;
  c_Richardson = 0 ;
  c_pression = 0 ;

  is >> motbidon ;

  Motcles les_mots(3);
  {
    les_mots[0] = "calcul_flux";
    les_mots[1] = "Richardson";
    les_mots[2] = "Pression_porosite";
  }


  if (motbidon == accouverte)
    {
      // is >> motlu;

      while(motlu != accfermee)
        {
          is >> motlu;
          int rang=les_mots.search(motlu);
          switch(rang)
            {
            case 0 :
              {
                is >> motbidon ;
                if (motbidon != accouverte)
                  {
                    Cerr << " on attent { et pas " << motbidon << finl ;
                  }

                is >> nb ;

                C_trans.resize(nb,3) ;
                R_loc.resize(nb,3) ;
                r_int.resize(nb) ;
                r_out.resize(nb) ;
                delta_r.resize(nb) ;
                delta_teta.resize(nb);

                for (int i=0; i<nb; i++)
                  {

                    Cerr << " Lire plan de coupe en x " << finl;

                    is >> R_loc(i,0) ;

                    is >> R_loc(i,1) ;

                    is >> R_loc(i,2) ;

                    is >> C_trans(i,0) ;

                    is >> C_trans(i,1) ;

                    is >> C_trans(i,2) ;

                    is >> r_int(i);

                    is >> r_out(i);

                    is >> delta_r(i);

                    is >> delta_teta(i) ;
                  }

                c_flux = 1 ;

                is >> motbidon ;
                if (motbidon != accfermee)
                  {
                    Cerr << " on attent } et pas " << motbidon << finl ;
                  }
                break ;

              }
            case 1 :
              {
                Cerr << " Lire Richardson " << finl;
                const Zone_dis_base& zdis=mon_equation->inconnue().zone_dis_base();
                const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdis);
                //                  const Probleme_base& pb = mon_equation->probleme();
                const int nb_faces = zone_VEF.nb_faces() ;

                ch_ri.associer_zone_dis_base(zdis);
                ch_ri.nommer("Richardson");
                ch_ri.fixer_nb_comp(1);
                ch_ri.fixer_nb_valeurs_nodales(nb_faces);
                ch_ri.fixer_unite("-");
                c_Richardson = 1 ;
                champs_compris_.ajoute_champ(ch_ri);
                break ;

              }
            case 2 :
              {
                Cerr << " Lire Pression_porosite " << finl;
                const Zone_dis_base& zdis=mon_equation->inconnue().zone_dis_base();
                const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdis);
                //                  const Probleme_base& pb = mon_equation->probleme();
                const int nb_elem = zone_VEF.nb_elem() ;

                ch_p.associer_zone_dis_base(zdis);
                ch_p.nommer("Pression_porosite");
                ch_p.fixer_nb_comp(1);
                ch_p.fixer_nb_valeurs_nodales(nb_elem);
                ch_p.fixer_unite("Pa.m3/kg");
                c_pression = 1 ;
                champs_compris_.ajoute_champ(ch_p);
                break ;
              }
            default :
              {
                if (motlu == accfermee)
                  {
                    break ;
                  }
                else
                  {
                    Cerr << "Erreur dans la lecture de Traitement_particulier_Brech_VEF" << finl;
                    Cerr << "Les mots cles possibles sont : calcul_flux ou Richardson " << finl;
                    Cerr << "Vous avez lu :" << motlu << finl;
                    exit();
                    break;
                  }
              }
            }
        }
      is >> motlu;
      if (motlu != accfermee)
        {
          Cerr << "Erreur dans la lecture de Traitement_particulier_NS_Brech_VEF 1 ";
          Cerr << "On attendait une } et pas " << motlu << finl;
          exit();
        }
      //         is >> motlu;
      //         if (motlu != accfermee)
      //           {
      //             Cerr << "Erreur dans la lecture de Traitement_particulier_NS_Brech_VEF 2 ";
      //             Cerr << "On attendait une } et pas " << motlu << finl;
      //             exit();
      //           }
      //         is >> motlu;
      //         if (motlu != accfermee)
      //           {
      //             Cerr << "Erreur dans la lecture de Traitement_particulier_NS_Brech_VEF 3 ";
      //             Cerr << "On attendait une } et pas " << motlu << finl;
      //             exit();
      //           }
    }

  return is;
}


void Traitement_particulier_NS_Brech_VEF::post_traitement_particulier()
{

  if (c_flux == 1 ) post_traitement_particulier_calcul_flux() ;
  if (c_Richardson == 1 ) post_traitement_particulier_Richardson() ;
  if (c_pression == 1 ) post_traitement_particulier_calcul_pression() ;
}

void Traitement_particulier_NS_Brech_VEF::post_traitement_particulier_calcul_flux()
{

  for (int ii =0; ii<nb; ii++)
    {
      int taille = 1 ;
      DoubleTab coord_trace (taille,3) ;
      DoubleTab Surf_trace(taille,3) ;
      DoubleTab valeurs_(taille,3) ;

      // Modifs VB pour prise en compte rho et calcul du flux enthalpique + Tmoy
      REF(Champ_base) rch1 ;
      REF(Champ_Inc_base) l_inco ;
      const Probleme_base& pb = mon_equation->probleme();
      /*
        pb.a_pour_Champ_Inc("Temperature", rch1 ) ;
        l_inco = ref_cast(Champ_Inc_base, rch1.valeur()) ;
        const Champ_Inc_base& temp = l_inco.valeur();
      */
      rch1 = pb.get_champ("Temperature");
      l_inco = ref_cast(Champ_Inc_base, rch1.valeur()) ;
      const Champ_Inc_base& temp = l_inco.valeur();

      DoubleTab temper_(taille);
      DoubleTab rho_(taille) ;
      DoubleTab cp_(taille) ;
      // Fin Modifs VB
      //
      double rad, teta ;
      double dr    = ( r_out(ii) - r_int(ii) ) /delta_r(ii) ;
      double dteta = 2.*3.14159/delta_teta(ii) ;

      taille = (int)(delta_r(ii)) * (int)(delta_teta(ii)) ;
      coord_trace.resize(taille,3) ;
      Surf_trace.resize(taille,3) ;
      valeurs_.resize(taille,3);

      // Modifs VB pour prise en compte rho et calcul du flux enthalpique + Tmoy
      rho_.resize(taille) ;
      temper_.resize(taille);
      cp_.resize(taille);
      // Fin Modifs VB

      double a,b,c ;

      // Modifs VB pour calculer le flux sur une face inclinee
      // anglex est l'angle que fait la normale a la face par rapport a l'axe des x (angle aigu)
      double anglex = acos(C_trans(ii,0)/sqrt( C_trans(ii,0) * C_trans(ii,0)
                                               + C_trans(ii,1) * C_trans(ii,1)
                                               + C_trans(ii,2) * C_trans(ii,2) )) ;
      // Fin Modifs VB

      int i =-1 ;

      for ( rad = (r_int(ii)+(dr/2.)) ; rad < r_out(ii) ; rad += dr )
        for (teta = dteta/2. ; teta < 2.*3.14159 ; teta += dteta )
          {
            i++ ;
            // Modifs VB pour calculer le flux sur une face inclinee
            //            a = 0. ;
            //            b = rad * cos(teta) ;
            a = - rad * cos(teta) * sin(anglex) ;
            b =   rad * cos(teta) * cos(anglex) ;
            // Fin Modifs VB
            c = rad * sin(teta) ;
            coord_trace(i,0) = a + R_loc(ii,0) ;
            coord_trace(i,1) = b + R_loc(ii,1) ;
            coord_trace(i,2) = c + R_loc(ii,2) ;
            double ra = (dr/2.+rad) * (dr/2.+rad) ;
            double ri = (rad - (dr/2.))* (rad - (dr/2.)) ;
            Surf_trace(i,0) = 3.14159 * (ra -ri) / delta_teta(ii) ;
          }

      double flux_pos = 0. ;
      double flux_neg = 0. ;
      double flux ;
      // Modifs VB pour prise en compte rho et calcul du flux enthalpique + Tmoy
      double tempmoy = 0. ;
      double massvol = 0. ;
      double fluxE_pos = 0. ;
      double fluxE_neg = 0. ;
      double fluxE ;
      // fin modifs VB

      const Zone_dis_base& zdis=mon_equation->inconnue().zone_dis_base();
      const Zone& zone=zdis.zone();
      IntVect les_polys(coord_trace.dimension(0));
      zone.chercher_elements(coord_trace, les_polys);
      mon_equation->inconnue()->valeur_aux_elems(coord_trace, les_polys, valeurs_);

      // Modifs VB pour prise en compte rho et calcul du flux enthalpique + Tmoy
      mon_equation->fluide().masse_volumique()->valeur_aux_elems(coord_trace, les_polys, rho_);
      mon_equation->fluide().capacite_calorifique()->valeur_aux_elems(coord_trace, les_polys, cp_);
      temp.valeur_aux_elems(coord_trace, les_polys, temper_);
      // Fin Modifs VB

      // On annulle la partie virtuelle
      for (i=0 ; i<taille; i++)
        if (les_polys(i)>=zone.nb_elem())
          {
            for (int k=0; k<dimension; k++)
              valeurs_(i,k)=0;
            // Modifs VB pour prise en compte rho et calcul du flux enthalpique + Tmoy
            rho_(i)=0.;
            temper_(i)=0.;
            cp_(i)=0.;
          }
      // Fin Modifs VB

      for (i=0 ; i<taille; i++ )
        {
          // Modifs VB pour prise en compte rho et calcul du flux enthalpique + Tmoy
          //       flux = valeurs_(i,0)*Surf_trace(i,0) ;
          massvol += rho_(i) ;
          tempmoy += temper_(i) ;
          flux  = ( valeurs_(i,0)*cos(anglex) + valeurs_(i,1)*sin(anglex) )
                  *   Surf_trace(i,0)*rho_(i) ;
          fluxE = ( valeurs_(i,0)*cos(anglex) + valeurs_(i,1)*sin(anglex) )
                  *   Surf_trace(i,0)*rho_(i)*cp_(i)*temper_(i) ;
          flux=Process::mp_sum(flux);
          fluxE=Process::mp_sum(fluxE);
          if ( flux > 0. ) flux_pos += flux ;
          if ( flux < 0. ) flux_neg += flux ;
          if ( fluxE > 0. ) fluxE_pos += fluxE ;
          if ( fluxE < 0. ) fluxE_neg += fluxE ;
        }

      tempmoy=Process::mp_sum(tempmoy);
      massvol=Process::mp_sum(massvol);
      tempmoy /= taille ;
      massvol /= taille ;

      if (je_suis_maitre())
        {
          Cout << finl ;
          Cout << " ----------------------- " << finl ;
          Cout << " Traitement_particulier flux en x (Plan Oyz) a: "<<  R_loc(ii,0) << " "
               <<  R_loc(ii,1) << " "
               <<  R_loc(ii,2) << finl ;
          Cout << " angle de la normale du plan par rapport a x = " << anglex/3.1415*180. << " (deg) " << finl ;
          Cout << " flux_pos = " << flux_pos << " flux_neg = " << flux_neg << " (kg/s) " << finl ;
          Cout << " fluxEnthalpique_pos = " << fluxE_pos << " fluxEnthalpique_neg = " << fluxE_neg << " (Watts) " << finl ;
          Cout << " masse volumique moyenne sur le plan de coupe : " << massvol << finl ;
          Cout << " temperature moyenne     sur le plan de coupe : " << tempmoy << finl ;
        }
      // Fin Modifs VB
    }
}

void Traitement_particulier_NS_Brech_VEF::post_traitement_particulier_Richardson()
{
  const Zone_dis& zdis=mon_equation->zone_dis();
  const Zone_VEF& zone_VEF=ref_cast(Zone_VEF, zdis.valeur());
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,mon_equation->zone_Cl_dis().valeur() );

  REF(Champ_base) rch1 ;
  REF(Champ_Inc_base) l_inco ;

  const Probleme_base& pb = mon_equation->probleme();
  /*
    pb.a_pour_Champ_Inc("Temperature", rch1 ) ;

    l_inco = ref_cast(Champ_Inc_base, rch1.valeur()) ;
  */
  rch1 = pb.get_champ("Temperature");
  l_inco = ref_cast(Champ_Inc_base, rch1.valeur()) ;

  const Champ_Inc_base& temp = l_inco.valeur();
  const DoubleTab& temperature = temp.valeurs();

  const DoubleTab&  vitesse = mon_equation->inconnue().valeurs();
  const DoubleVect& gravite = mon_equation->fluide().gravite().valeurs();
  const DoubleVect& beta     = mon_equation->fluide().beta_t().valeurs();

  const int nb_faces = zone_VEF.nb_faces();

  DoubleVect P(nb_faces) ;
  DoubleVect G(nb_faces) ;

  DoubleVect beta_i(nb_faces) ;
  beta_i = beta(0) ;

  calculer_terme_production_K( zone_VEF, zone_Cl_VEF,
                               P, vitesse);

  calculer_terme_destruction_K( zone_VEF, zone_Cl_VEF,
                                G, temperature,
                                beta_i, gravite) ;

  DoubleVect&  richard_loc = ch_ri.valeurs();
  richard_loc = 0. ;

  for ( int i=0; i<nb_faces; i++ )
    {
      if (std::fabs (P[i]) > 1.e-9 )richard_loc [i] = -G[i]/P[i] ;
      else richard_loc [i] = 0. ;
    }

  ch_ri.changer_temps(mon_equation->inconnue().temps());

}

void Traitement_particulier_NS_Brech_VEF::post_traitement_particulier_calcul_pression()
{
  const Zone_VEF& zvef=ref_cast(Zone_VEF, mon_equation->zone_dis().valeur());
  const DoubleVect& porosite_face = zvef.porosite_face();
  int i,comp;
  int nb_face = zvef.nb_faces();
  //Champ_Inc la_pression = mon_equation->pression();
  //Champ_Inc la_vitesse =  mon_equation->vitesse();
  Operateur_Div divergence = mon_equation->operateur_divergence();
  Operateur_Grad gradient = mon_equation->operateur_gradient();
  SolveurSys solveur_pression_ = mon_equation->solveur_pression();

  DoubleTab& pression=mon_equation->pression().valeurs();
  DoubleTab& vitesse=mon_equation->vitesse().valeurs();
  //DoubleTrav gradP(la_vitesse.valeurs());
  //DoubleTrav gradP(vitesse);
  //DoubleTrav inc_pre(pression);
  DoubleTab gradP(vitesse);
  DoubleTab inc_pre(pression);

  //  DoubleVect& inc_pre = ch_p.valeurs();
  inc_pre  = 0. ;
  //DoubleTrav secmem(pression);
  DoubleTab secmem(pression);

  gradient.calculer(mon_equation->pression().valeurs(),gradP);
  //gradient.calculer(la_pression,gradP);
  // Cerr << "gradP " << gradP << finl;
  //on veut BM-1Bt(psi*Pression)
  mon_equation->solv_masse().appliquer(gradP);

  //DoubleTrav grad_temp(la_vitesse.valeurs());
  //DoubleTrav grad_temp(vitesse);
  DoubleTab grad_temp(vitesse);
  for(i=0; i<nb_face; i++)
    {
      for (comp=0; comp<dimension; comp++)
        grad_temp(i,comp) = gradP(i,comp)/porosite_face(i);
    }

  divergence.calculer(grad_temp, secmem);
  secmem *= -1; // car div =-B
  solveur_pression_.resoudre_systeme(mon_equation->matrice_pression().valeur(),secmem, inc_pre);
  // solveur_pression_.resoudre_systeme(mon_equation->matrice_pression().valeur(),secmem, inc_pre, la_pression.valeur());
  // Cerr << "la pression " << pression << finl;
  //Cerr << "inc_pre " << inc_pre << finl;

  DoubleVect& la_pression_porosite = ch_p.valeurs();
  la_pression_porosite = inc_pre ;
  Cerr << "la_pression " << mon_equation->pression().valeurs() << finl;
  Cerr << "ch_p.valeurs() " << ch_p.valeurs() << finl;
}


////////////////////////////////////////////////////////////////////////////
//
//    Implementation des fonctions de la classe Calcul_Production_K_VEF
//
/////////////////////////////////////////////////////////////////////////////


void Traitement_particulier_NS_Brech_VEF::
calculer_terme_production_K(const Zone_VEF& zone_VEF,const Zone_Cl_VEF& zcl_VEF,
                            DoubleVect& P,
                            const DoubleTab& vit) const
{
  // P est discretise comme K et Eps i.e au centre des elements
  //
  // P(elem) = -(2/3)*k(i)*div_U(i) + nu_t(i) * F(u,v,w)
  //
  //                          2          2          2
  //    avec F(u,v,w) = 2[(du/dx)  + (dv/dy)  + (dw/dz) ] +
  //
  //                               2               2               2
  //                  (du/dy+dv/dx) + (du/dz+dw/dx) + (dw/dy+dv/dz)
  //
  // Rqs: On se place dans le cadre incompressible donc on neglige
  //      le terme (2/3)*k(i)*div_U(i)

  P= 0;

  // Calcul de F(u,v,w):

  //  const Zone& zone = zone_VEF.zone();
  int nb_elem = zone_VEF.nb_elem();
  //  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  //  const int nb_faces = zone_VEF.nb_faces();
  const DoubleVect& volumes = zone_VEF.volumes();
  //  int premiere_face_entier = zone_VEF.premiere_face_int();
  //  const IntTab& les_Polys = zone.les_elems();

  int fac=0, poly1, poly2;
  int nb_faces_ = zone_VEF.nb_faces();

  // (du/dx du/dy dv/dx dv/dy ...) pour un poly

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //                        <
  // calcul des gradients;  < [ Ujp*np/vol(j) ]
  //                         j
  ////////////////////////////////////////////////////////////////////////////////////////////////
  int n_bord;

  DoubleTab gradient_elem(nb_elem,dimension,dimension);
  Champ_P1NC::calcul_gradient(vit,gradient_elem,zcl_VEF);

  // On a les gradient_elem par elements
  ////////////////////////////////////////////////////////////////////////////////////

  double du_dx;
  double du_dy;
  double du_dz;
  double dv_dx;
  double dv_dy;
  double dv_dz;
  double dw_dx;
  double dw_dy;
  double dw_dz;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Calcul des du/dx dv/dy et des derivees croisees sur les faces de chaque elements dans le cas 2D
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Boucle sur les bords pour traiter les faces de bord
  // en distinguant le cas periodicite

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zcl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          for (fac=ndeb; fac<nfin; fac++)
            {
              poly1 = face_voisins(fac,0);
              poly2 = face_voisins(fac,1);
              double a=volumes(poly1)/(volumes(poly1)+volumes(poly2));
              double b=volumes(poly2)/(volumes(poly1)+volumes(poly2));

              du_dx =a*gradient_elem(poly1,0,0) + b*gradient_elem(poly2,0,0);
              du_dy =a*gradient_elem(poly1,0,1) + b*gradient_elem(poly2,0,1);
              dv_dx =a*gradient_elem(poly1,1,0) + b*gradient_elem(poly2,1,0);
              dv_dy =a*gradient_elem(poly1,1,1) + b*gradient_elem(poly2,1,1);

              // Determination du terme de production

              P(fac) = ( 2*(du_dx *du_dx  + dv_dy *dv_dy) + ((du_dy+dv_dx)*(du_dy+dv_dx)));

              if (dimension == 3)
                {
                  du_dz=a*gradient_elem(poly1,0,2) + b*gradient_elem(poly2,0,2);
                  dv_dz=a*gradient_elem(poly1,1,2) + b*gradient_elem(poly2,1,2);
                  dw_dx=a*gradient_elem(poly1,2,0) + b*gradient_elem(poly2,2,0);
                  dw_dy=a*gradient_elem(poly1,2,1) + b*gradient_elem(poly2,2,1);
                  dw_dz=a*gradient_elem(poly1,2,2) + b*gradient_elem(poly2,2,2);

                  // Determination du terme de production

                  P(fac) = (2*( du_dx*du_dx + dv_dy*dv_dy + dw_dz*dw_dz )
                            + ( (du_dy+dv_dx)*(du_dy+dv_dx)
                                +   (du_dz+dw_dx)*(du_dz+dw_dx)
                                +   (dw_dy+dv_dz)*(dw_dy+dv_dz)));
                }
            }
        }
      else
        {
          for (fac=ndeb; fac<nfin; fac++)
            {
              poly1 = face_voisins(fac,0);

              du_dx=gradient_elem(poly1,0,0);
              du_dy=gradient_elem(poly1,0,1);
              dv_dx=gradient_elem(poly1,1,0);
              dv_dy=gradient_elem(poly1,1,1);
              // Determination du terme de production
              P(fac) = ( 2*(du_dx*du_dx + dv_dy*dv_dy) + ((du_dy+dv_dx)*(du_dy+dv_dx)));


              if (dimension == 3)
                {
                  du_dz=gradient_elem(poly1,0,2);
                  dv_dz=gradient_elem(poly1,1,2);
                  dw_dx=gradient_elem(poly1,2,0);
                  dw_dy=gradient_elem(poly1,2,1);
                  dw_dz=gradient_elem(poly1,2,2);

                  // Determination du terme de production

                  P(fac) = (2*( du_dx*du_dx + dv_dy*dv_dy + dw_dz*dw_dz )
                            + ( (du_dy+dv_dx)*(du_dy+dv_dx)
                                +   (du_dz+dw_dx)*(du_dz+dw_dx)
                                +   (dw_dy+dv_dz)*(dw_dy+dv_dz)));
                }
            }
        }
    }


  // Traitement des faces internes

  for (; fac<nb_faces_; fac++)
    {
      poly1 = face_voisins(fac,0);
      poly2 = face_voisins(fac,1);
      double a=volumes(poly1)/(volumes(poly1)+volumes(poly2));
      double b=volumes(poly2)/(volumes(poly1)+volumes(poly2));

      du_dx=a*gradient_elem(poly1,0,0) + b*gradient_elem(poly2,0,0);
      du_dy=a*gradient_elem(poly1,0,1) + b*gradient_elem(poly2,0,1);
      dv_dx=a*gradient_elem(poly1,1,0) + b*gradient_elem(poly2,1,0);
      dv_dy=a*gradient_elem(poly1,1,1) + b*gradient_elem(poly2,1,1);

      // Determination du terme de production

      P(fac) = ( 2*(du_dx*du_dx + dv_dy*dv_dy) + ((du_dy+dv_dx)*(du_dy+dv_dx)));

      if (dimension == 3)
        {
          du_dz=a*gradient_elem(poly1,0,2) + b*gradient_elem(poly2,0,2);
          dv_dz=a*gradient_elem(poly1,1,2) + b*gradient_elem(poly2,1,2);
          dw_dx=a*gradient_elem(poly1,2,0) + b*gradient_elem(poly2,2,0);
          dw_dy=a*gradient_elem(poly1,2,1) + b*gradient_elem(poly2,2,1);
          dw_dz=a*gradient_elem(poly1,2,2) + b*gradient_elem(poly2,2,2);

          // Determination du terme de production

          P(fac) = (2*( du_dx*du_dx + dv_dy*dv_dy + dw_dz*dw_dz )
                    + ( (du_dy+dv_dx)*(du_dy+dv_dx)
                        +   (du_dz+dw_dx)*(du_dz+dw_dx)
                        +   (dw_dy+dv_dz)*(dw_dy+dv_dz)));
        }
    }
}


void Traitement_particulier_NS_Brech_VEF::
calculer_terme_destruction_K(const Zone_VEF& zone_VEF,
                             const Zone_Cl_VEF& zcl_VEF,
                             DoubleVect& G,const DoubleTab& temper,
                             const DoubleVect& beta,const DoubleVect& gravite) const
{
  //
  // G est discretise comme K et Eps i.e au centre des faces
  //
  //                                       --> ----->
  // G(face) = beta alpha_t(face) G . gradT(face)
  //
  int nb_elem = zone_VEF.nb_elem();
  int nb_faces= zone_VEF.nb_faces();
  DoubleTrav u_teta(nb_faces,dimension);
  DoubleTrav gradient_elem(nb_elem,dimension);
  u_teta=0;
  gradient_elem=0;

  //  const Zone& la_zone=zone_VEF.zone();
  //  int nb_faces_elem = la_zone.nb_faces_elem();
  //  const IntTab& elem_faces = zone_VEF.elem_faces();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normales = zone_VEF.face_normales();
  int premiere_face_entier = zone_VEF.premiere_face_int();
  const DoubleVect& volumes = zone_VEF.volumes();
  int nb_faces_ = zone_VEF.nb_faces();
  int elem1,elem2,fac;

  DoubleVect coef(Objet_U::dimension);
  //  const IntTab& les_elem_faces = zone_VEF.elem_faces();

  // Calcul du gradient de temperature :

  // On traite les bords
  int n_bord;
  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zcl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          for (fac=ndeb; fac<nfin; fac++)
            {
              elem1=face_voisins(fac,0);
              elem2=face_voisins(fac,1);
              for (int i=0; i<dimension; i++)
                {
                  gradient_elem(elem1,i) += 0.5*face_normales(fac,i)*temper(fac);
                  gradient_elem(elem2,i) -= 0.5*face_normales(fac,i)*temper(fac);
                }
            }
        }
      else
        {
          for (int fac2=ndeb; fac2<nfin; fac2++)
            {
              elem1=face_voisins(fac2,0);
              for (int i=0; i<dimension; i++)
                gradient_elem(elem1,i) += face_normales(fac2,i)*temper(fac2);
            }
        }
    }

  // On traite les faces internes
  for (fac = premiere_face_entier ; fac<nb_faces_; fac++)
    {
      elem1=face_voisins(fac,0);
      elem2=face_voisins(fac,1);
      for (int i=0; i<dimension; i++)
        {
          gradient_elem(elem1,i) += face_normales(fac,i)*temper(fac);
          gradient_elem(elem2,i) -= face_normales(fac,i)*temper(fac);
        }
    }

  for (int elem=0; elem<nb_elem; elem++)
    for (int i=0; i<dimension; i++)
      gradient_elem(elem,i) /= volumes(elem);

  // Calcul de u_teta :

  // On traite les bords

  for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zcl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          for (fac=ndeb; fac<nfin; fac++)
            {
              elem1 = face_voisins(fac,0);
              elem2 = face_voisins(fac,1);
              double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
              double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
              for (int i=0; i<dimension; i++)
                u_teta(fac,i)=a*beta(elem1)*gradient_elem(elem1,i)
                              +b*beta(elem2)*gradient_elem(elem2,i);
            }
        }
      else
        {
          for (fac=ndeb; fac< nfin; fac++)
            {
              elem1 = face_voisins(fac,0);
              for (int i=0; i<dimension; i++)
                u_teta(fac,i)=beta(elem1)*gradient_elem(elem1,i);
            }
        }
    }

  for (; fac<nb_faces_; fac++)
    {
      elem1 = face_voisins(fac,0);
      elem2 = face_voisins(fac,1);
      double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
      double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
      for (int i=0; i<dimension; i++)
        u_teta(fac,i)=a*beta(elem1)*gradient_elem(elem1,i)+b*beta(elem2)*gradient_elem(elem2,i);
    }
  //           ------->  ------>
  // Calcul de gravite . u_teta
  //
  G = 0;
  for (fac=0; fac< nb_faces_; fac++)
    {
      if (dimension == 2)
        G[fac] = gravite(0)*u_teta(fac,0) + gravite(1)*u_teta(fac,1);
      else if (dimension == 3)
        G[fac] = gravite(0)*u_teta(fac,0) + gravite(1)*u_teta(fac,1) + gravite(2)*u_teta(fac,2);
    }
}

