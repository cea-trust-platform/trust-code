/****************************************************************************
* Copyright (c) 2017, CEA
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
// File:        Champ_P1NC.cpp
// Directory:   $TRUST_ROOT/src/VEF/Champs
// Version:     /main/54
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_P1NC.h>
#include <Periodique.h>
#include <Dirichlet_paroi_fixe.h>
#include <Dirichlet_paroi_defilante.h>
#include <Echange_externe_impose.h>
#include <Neumann_paroi.h>
#include <Neumann_homogene.h>
#include <Fluide_Incompressible.h>
#include <Scalaire_impose_paroi.h>
#include <Champ_Uniforme.h>
#include <Operateur.h>
#include <Operateur_base.h>
#include <Mod_turb_hyd_base.h>
#include <distances_VEF.h>
#include <DoubleTrav.h>
#include <Schema_Temps_base.h>
#include <SFichier.h>

Implemente_instanciable(Champ_P1NC,"Champ_P1NC",Champ_Inc_base);

// printOn


Sortie& Champ_P1NC::
printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}

// readOn


Entree& Champ_P1NC::
readOn(Entree& s)
{
  lire_donnees (s) ;
  return s ;
}

////////////////////////////////////////////////////////////////////
//
//   Implementation des fonctions de la classe Champ_P1NC
//
////////////////////////////////////////////////////////////////////

// PQ : 01/10/04 : mettre_a_jour permet de calculer qu'une seule fois le champ conforme u_bar (defini aux sommets)
//                   pour pouvoir etre utilise dans les differents operateurs ou lors de l'appel
//                   des sondes definies a partir des valeurs "smooth" (dans le cas present : u_bar)

void Champ_P1NC::mettre_a_jour(double un_temps)
{
  Champ_Inc_base::mettre_a_jour(un_temps);
  filtrer_L2_deja_appele_=0;
}
// Description:
void Champ_P1NC::abortTimeStep()
{
  ch_som_=0;
}
// Fonction compo_normale_sortante()
//
// Cette fonction retourne :
//   1 si le fluide est sortant sur la face num_face
//   0 si la face correspond a une reentree de fluide

const Zone_dis_base& Champ_P1NC::zone_dis_base() const
{
  return la_zone_VEF.valeur();
}

void Champ_P1NC::associer_zone_dis_base(const Zone_dis_base& z_dis)
{
  la_zone_VEF=ref_cast(Zone_VEF, z_dis);
}

int Champ_P1NC::compo_normale_sortante(int num_face) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  double vit_norm =0;
  for (int ncomp=0; ncomp<nb_comp(); ncomp++)
    vit_norm += (*this)(num_face,ncomp)*zone_VEF.face_normales(num_face,ncomp);
  return (vit_norm > 0);
}


DoubleTab& Champ_P1NC::trace(const Frontiere_dis_base& fr, DoubleTab& x, double tps,int distant) const
{
  return Champ_P1NC_implementation::trace(fr, valeurs(tps), x,distant);
}

void Champ_P1NC::cal_rot_ordre1(DoubleTab& vorticite) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_elem = zone_VEF.nb_elem();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  DoubleTab gradient_elem(nb_elem_tot,dimension,dimension);

  gradient_elem=0.;
  vorticite=0;
  gradient(gradient_elem);
  int num_elem;

  switch(dimension)
    {
    case 2 :
      {
        for (num_elem=0; num_elem<nb_elem; num_elem++)
          {
            vorticite(num_elem)=gradient_elem(num_elem,1,0)-gradient_elem(num_elem,0,1);
          }
      }
      break;
    case 3 :
      {
        for (num_elem=0; num_elem<nb_elem; num_elem++)
          {
            vorticite(num_elem,0)=gradient_elem(num_elem,2,1)-gradient_elem(num_elem,1,2);
            vorticite(num_elem,1)=gradient_elem(num_elem,0,2)-gradient_elem(num_elem,2,0);
            vorticite(num_elem,2)=gradient_elem(num_elem,1,0)-gradient_elem(num_elem,0,1);
          }
      }
    }

  vorticite.echange_espace_virtuel();
  return ;
}

void calculer_gradientP1NC(const DoubleTab& variable,
                           const Zone_VEF& zone_VEF,
                           const Zone_Cl_VEF& zone_Cl_VEF,
                           DoubleTab& gradient_elem)
{
  const DoubleTab& face_normales = zone_VEF.face_normales();
  const int nb_faces = zone_VEF.nb_faces_tot();
  const int nb_elem = zone_VEF.nb_elem_tot();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  int premiere_face_int = zone_VEF.premiere_face_int();

  int dimension=Objet_U::dimension;
  int nb_comp;
  if (variable.nb_dim() == 1)
    nb_comp=1;
  else
    nb_comp=variable.dimension(1);

  gradient_elem=0.;
  int icomp,fac,i,elem1,elem2,elem,num_face;
  int n_bord;
  int ind_face;


  // Cas du calcul du gradient d'un tableau de vecteurs
  if (nb_comp!=1)
    {
      for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl =
            zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord =
            ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = 0;
          int num2 = le_bord.nb_faces_tot();
          int nb_faces_bord = le_bord.nb_faces();

          if (sub_type(Periodique,la_cl.valeur()))
            {
              for (ind_face=num1; ind_face<num2; ind_face++)
                {
                  num_face = le_bord.num_face(ind_face);
                  elem1=face_voisins(num_face,0);
                  elem2=face_voisins(num_face,1);
                  for (icomp=0; icomp<nb_comp; icomp++)
                    for (i=0; i<dimension; i++)
                      {
                        if (ind_face<nb_faces_bord)
                          {
                            gradient_elem(elem1, icomp, i) +=
                              0.5*face_normales(num_face,i)*variable(num_face,icomp);
                            gradient_elem(elem2, icomp, i) -=
                              0.5*face_normales(num_face,i)*variable(num_face,icomp);
                          }
                        else
                          {
                            //Correction pour prise en compte des faces virtuelles periodiques
                            //On retranche la contribution en exces ajoutee quand on traite
                            //les faces entre nb_faces et nb_faces_tot (dans le cas des faces periodiques)

                            gradient_elem(elem1, icomp, i) -=
                              0.5*face_normales(num_face,i)*variable(num_face,icomp);
                            gradient_elem(elem2, icomp, i) +=
                              0.5*face_normales(num_face,i)*variable(num_face,icomp);
                          }
                      }
                }
            }
          else
            for (ind_face=num1; ind_face<nb_faces_bord; ind_face++)
              {
                fac = le_bord.num_face(ind_face);
                elem1=face_voisins(fac,0);
                for (icomp=0; icomp<nb_comp; icomp++)
                  for (i=0; i<dimension; i++)
                    {
                      gradient_elem(elem1, icomp, i) +=
                        face_normales(fac,i)* variable(fac,icomp);
                    }
              }
        }
      for (fac=premiere_face_int; fac<nb_faces; fac++)
        {
          elem1=face_voisins(fac,0);
          elem2=face_voisins(fac,1);
          // Deroulement de la boucle pour optimiser un peu (-20% sur 12000 mailles):
          if (elem1>=0 && elem2>=0)
            {
              if (dimension==2)
                {
                  for (icomp=0; icomp<nb_comp; icomp++)
                    {
                      double var = variable(fac,icomp);
                      gradient_elem(elem1, icomp, 0) += face_normales(fac,0)*var;
                      gradient_elem(elem2, icomp, 0) -= face_normales(fac,0)*var;
                      gradient_elem(elem1, icomp, 1) += face_normales(fac,1)*var;
                      gradient_elem(elem2, icomp, 1) -= face_normales(fac,1)*var;
                    }
                }
              else
                {
                  for (icomp=0; icomp<nb_comp; icomp++)
                    {
                      double var = variable(fac,icomp);
                      gradient_elem(elem1, icomp, 0) += face_normales(fac,0)*var;
                      gradient_elem(elem2, icomp, 0) -= face_normales(fac,0)*var;
                      gradient_elem(elem1, icomp, 1) += face_normales(fac,1)*var;
                      gradient_elem(elem2, icomp, 1) -= face_normales(fac,1)*var;
                      gradient_elem(elem1, icomp, 2) += face_normales(fac,2)*var;
                      gradient_elem(elem2, icomp, 2) -= face_normales(fac,2)*var;
                    }
                }
            }
          else
            {
              for (icomp=0; icomp<nb_comp; icomp++)
                for (i=0; i<dimension; i++)
                  {
                    if (elem1>=0) gradient_elem(elem1, icomp, i) += face_normales(fac,i)*variable(fac,icomp);
                    if (elem2>=0) gradient_elem(elem2, icomp, i) -= face_normales(fac,i)*variable(fac,icomp);
                  }
            }
        }

    }
  // Cas du calcul du gradient d'un tableau de scalaire dans un tableau gradient_elem(,,)
  else if (gradient_elem.nb_dim()==3)
    {
      for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = 0;
          int num2 = le_bord.nb_faces_tot();
          int nb_faces_bord = le_bord.nb_faces();

          if (sub_type(Periodique,la_cl.valeur()))
            {
              for (ind_face=num1; ind_face<num2; ind_face++)
                {
                  num_face = le_bord.num_face(ind_face);
                  elem1=face_voisins(num_face,0);
                  elem2=face_voisins(num_face,1);
                  for (i=0; i<dimension; i++)
                    {
                      if (ind_face<nb_faces_bord)
                        {
                          gradient_elem(elem1, 0, i) +=
                            0.5*face_normales(num_face,i)*variable(num_face);
                          gradient_elem(elem2, 0, i) -=
                            0.5*face_normales(num_face,i)*variable(num_face);
                        }
                      else
                        {
                          //Correction pour prise en compte des faces virtuelles periodiques
                          //On retranche la contribution en exces ajoutee quand on traite
                          //les faces entre nb_faces et nb_faces_tot (dans le cas des faces periodiques)
                          gradient_elem(elem1, 0, i) -=
                            0.5*face_normales(num_face,i)*variable(num_face);
                          gradient_elem(elem2, 0, i) +=
                            0.5*face_normales(num_face,i)*variable(num_face);

                        }

                    }
                }
            }
          else
            for (ind_face=num1; ind_face<nb_faces_bord; ind_face++)
              {
                fac = le_bord.num_face(ind_face);
                elem1=face_voisins(fac,0);
                for (i=0; i<dimension; i++)
                  {
                    gradient_elem(elem1, 0, i) +=
                      face_normales(fac,i)* variable(fac);
                  }
              }
        }
      for (fac=premiere_face_int; fac<nb_faces; fac++)
        {
          elem1=face_voisins(fac,0);
          elem2=face_voisins(fac,1);
          for (i=0; i<dimension; i++)
            {
              if (elem1>=0)
                gradient_elem(elem1, 0, i) += face_normales(fac,i)*variable(fac);
              if (elem2>=0)
                gradient_elem(elem2, 0, i) -= face_normales(fac,i)*variable(fac);

            }
        }
    }
  // Cas du calcul du gradient d'un tableau de scalaire dans un tableau gradient_elem(,)
  else
    {
      for (n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int num1 = 0;
          int num2 = le_bord.nb_faces_tot();
          int nb_faces_bord = le_bord.nb_faces();

          if (sub_type(Periodique,la_cl.valeur()))
            {
              for (ind_face=num1; ind_face<num2; ind_face++)
                {
                  num_face = le_bord.num_face(ind_face);
                  elem1=face_voisins(num_face,0);
                  elem2=face_voisins(num_face,1);
                  for (i=0; i<dimension; i++)
                    {
                      if (ind_face<nb_faces_bord)
                        {
                          gradient_elem(elem1, i) +=
                            0.5*face_normales(num_face,i)*variable(num_face);
                          gradient_elem(elem2, i) -=
                            0.5*face_normales(num_face,i)*variable(num_face);
                        }
                      else
                        {
                          //Correction pour prise en compte des faces virtuelles periodiques
                          //On retranche la contribution en exces ajoutee quand on traite
                          //les faces entre nb_faces et nb_faces_tot (dans le cas des faces periodiques)
                          gradient_elem(elem1,i) -=
                            0.5*face_normales(num_face,i)*variable(num_face);
                          gradient_elem(elem2,i) +=
                            0.5*face_normales(num_face,i)*variable(num_face);
                        }
                    }
                }
            }
          else
            for (ind_face=num1; ind_face<nb_faces_bord; ind_face++)
              {
                fac = le_bord.num_face(ind_face);
                elem1=face_voisins(fac,0);
                for (i=0; i<dimension; i++)
                  {
                    gradient_elem(elem1, i) +=
                      face_normales(fac,i)* variable(fac);
                  }
              }
        }
      for (fac=premiere_face_int; fac<nb_faces; fac++)
        {
          elem1=face_voisins(fac,0);
          elem2=face_voisins(fac,1);
          for (i=0; i<dimension; i++)
            {
              if (elem1>=0)
                gradient_elem(elem1, i) += face_normales(fac,i)*variable(fac);
              if (elem2>=0)
                gradient_elem(elem2, i) -= face_normales(fac,i)*variable(fac);

            }
        }
    }

  const DoubleVect& inverse_volumes = zone_VEF.inverse_volumes();
  if (gradient_elem.nb_dim()==3)
    for ( elem=0; elem<nb_elem; elem++)
      for (icomp=0; icomp<nb_comp; icomp++)
        for (i=0; i<dimension; i++)
          gradient_elem(elem,icomp,i) *= inverse_volumes(elem);
  else
    for ( elem=0; elem<nb_elem; elem++)
      for (i=0; i<dimension; i++)
        gradient_elem(elem,i) *= inverse_volumes(elem);
}

void Champ_P1NC::gradient(DoubleTab& gradient_elem) const
{
  // Calcul du gradient: par exemple gradient de la vitesse pour le calcul de la vorticite
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,equation().zone_Cl_dis().valeur() );
  calculer_gradientP1NC(valeurs(),zone_VEF,zone_Cl_VEF,
                        gradient_elem);

}

int Champ_P1NC::imprime(Sortie& os, int ncomp) const
{
  imprime_P1NC(os,ncomp);
  return 1;
}

Champ_base&  Champ_P1NC::affecter_(const Champ_base& ch)
{
#ifdef CLEMENT
  DoubleTab& tab = valeurs();
  DoubleTab noeuds;
  remplir_coord_noeuds(noeuds);
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_faces = zone_VEF.nb_faces();
  const IntTab& face_sommets = zone_VEF.face_sommets();
  const DoubleTab& sommets=domaine().coord_sommets();
  int nb_som=sommets.dimension(0);
  Cerr << "Affecter Clement : ... " << finl;
  DoubleTab valf(valeurs());
  ch.valeur_aux(noeuds, valf);
  DoubleTab vals(nb_som, nb_compo_);
  if(nb_compo_==1)
    {
      vals.resize(nb_som);
    }
  ch.valeur_aux(sommets, vals);
  for(int face=0; face<nb_faces; face++)
    {
      for(int comp=0; comp<nb_compo_; comp++)
        {
          double somme=0;
          for (int isom=0; isom<dimension; isom++)
            if(nb_compo_==1)
              somme+=vals(face_sommets(face,isom));
            else
              somme+=vals(face_sommets(face,isom), comp);
          if(nb_compo_==1)
            {
              if(dimension==2)
                tab(face)=1./6.*somme+2./3.*valf(face);
              else
                tab(face)=17./60.*somme+3./20.*valf(face);
            }
          else if(dimension==2)
            tab(face,comp)=1./6.*somme+2./3.*valf(face,comp);
          else
            tab(face,comp)=17./60.*somme+3./20.*valf(face,comp);
        }
    }
#else
  return Champ_Inc_base::affecter_(ch);
#endif
  return *this;
}

//-Cas CL periodique : assure que les valeurs sur des faces periodiques
// en vis a vis sont identiques. Pour cela on prend la demi somme des deux valeurs.
void Champ_P1NC::verifie_valeurs_cl()
{
  const Zone_Cl_dis_base& zcl = equation().zone_Cl_dis().valeur();
  int nb_cl = zcl.nb_cond_lim();
  DoubleTab& ch_tab = valeurs();
  int nb_compo = nb_comp();
  int nb_dimension = ch_tab.nb_dim();
  int ndeb,nfin,num_face;

  for (int i=0; i<nb_cl; i++)
    {
      const Cond_lim_base& la_cl = zcl.les_conditions_limites(i).valeur();
      if (sub_type(Periodique,la_cl))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();
          int voisine;
          double moy;

          if (nb_dimension==1)
            {
              for (num_face=ndeb; num_face<nfin; num_face++)
                {
                  voisine = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
                  if (ch_tab[num_face] != ch_tab[voisine])
                    {
                      moy = 0.5*(ch_tab[num_face] + ch_tab[voisine]);
                      ch_tab[num_face] = moy;
                      ch_tab[voisine] = moy;
                    }
                }
            }
          else
            {
              for (num_face=ndeb; num_face<nfin; num_face++)
                {
                  voisine = la_cl_perio.face_associee(num_face-ndeb) + ndeb;
                  for (int comp=0; comp<nb_compo; comp++)
                    {
                      if (ch_tab(num_face,comp) != ch_tab(voisine,comp))
                        {
                          moy = 0.5*(ch_tab(num_face,comp) + ch_tab(voisine,comp));
                          ch_tab(num_face,comp) = moy;
                          ch_tab(voisine,comp) = moy;
                        }
                    }
                }
            }
        }
    }
  ch_tab.echange_espace_virtuel();
}

void Champ_P1NC::calcul_critere_Q(DoubleVect& Critere_Q) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone_Cl_VEF& zone_Cl_VEF = ref_cast(Zone_Cl_VEF,equation().zone_Cl_dis().valeur() );
  const int nb_elem = zone_VEF.nb_elem();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();
  int num_elem,i,j;
  double crit,deriv1,deriv2;

  DoubleTab gradient_elem(nb_elem_tot,dimension,dimension);
  gradient_elem=0.;

  //Champ_P1NC& vit = *this;
  const DoubleTab& vitesse = valeurs();
  Champ_P1NC::calcul_gradient(vitesse,gradient_elem,zone_Cl_VEF);


  for (num_elem=0; num_elem<nb_elem; num_elem++)
    {
      crit = 0.;
      for (i=0; i<dimension; i++)
        for(j=0; j<dimension; j++)
          {
            deriv1 = gradient_elem(num_elem,i,j);
            deriv2 = gradient_elem(num_elem,j,i);

            crit += -0.25*deriv1*deriv2;
          }
      Critere_Q[num_elem] = crit;
    }
}


void Champ_P1NC::calcul_y_plus(const Zone_Cl_VEF& zone_Cl_VEF, DoubleVect& y_plus) const
{
  // On initialise le champ y_plus avec une valeur negative,
  // comme ca lorsqu'on veut visualiser le champ pres de la paroi,
  // on n'a qu'a supprimer les valeurs negatives et n'apparaissent
  // que les valeurs aux parois.

  int ndeb,nfin,elem,l_unif;
  double norm_tau,u_etoile,norm_v, dist, val1, val2, val3, d_visco, visco=1.;
  IntVect num(dimension);
  y_plus=-1.;

  const Champ_P1NC& vit = *this;
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const IntTab& elem_faces = zone_VEF.elem_faces();
  const Equation_base& eqn_hydr = equation();
  const Fluide_Incompressible& le_fluide = ref_cast(Fluide_Incompressible, eqn_hydr.milieu());
  const Champ_Don& ch_visco_cin = le_fluide.viscosite_cinematique();
  const DoubleTab& tab_visco = ref_cast(DoubleTab,ch_visco_cin->valeurs());

  if (sub_type(Champ_Uniforme,ch_visco_cin.valeur()))
    {
      if(tab_visco(0,0)>DMINFLOAT) visco = tab_visco(0,0);
      else visco = DMINFLOAT;
      l_unif = 1;
    }
  else
    l_unif = 0;
  if ((!l_unif) && (tab_visco.local_min_vect()<DMINFLOAT))
// GF on ne doit pas changer tab_visco ici !
    {
      Cerr<<" visco <=0 ?"<<finl;
      exit();
    }
  // tab_visco+=DMINFLOAT;

  DoubleTab cisaillement(1,1);
  int lp=0;

  const RefObjU& modele_turbulence = eqn_hydr.get_modele(TURBULENCE);
  if (modele_turbulence.non_nul() && sub_type(Mod_turb_hyd_base,modele_turbulence.valeur()))
    {
      const Mod_turb_hyd_base& mod_turb = ref_cast(Mod_turb_hyd_base,modele_turbulence.valeur());
      const Turbulence_paroi_base& loipar = mod_turb.loi_paroi();
      if( !loipar.use_shear() ) lp=0;
      else
        {
          cisaillement.resize(zone_VEF.nb_faces_bord());
          cisaillement.ref(loipar.Cisaillement_paroi());
          lp=1;
        }
    }

  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);

      if ( sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          ndeb = le_bord.num_premiere_face();
          nfin = ndeb + le_bord.nb_faces();

          if (dimension == 2 )
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                elem =face_voisins(num_face,0);

                num[0]=elem_faces(elem,0);
                num[1]=elem_faces(elem,1);

                if (num[0]==num_face) num[0]=elem_faces(elem,2);
                else if (num[1]==num_face) num[1]=elem_faces(elem,2);

                dist = distance_2D(num_face,elem,zone_VEF);
                dist *= 3./2.;// pour se ramener a distance paroi / milieu de num[0]-num[1]
                norm_v=norm_2D_vit1_lp(vit,num_face,num[0],num[1],zone_VEF,val1,val2);

                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                // PQ : 01/10/03 : corrections par rapport a la version premiere

                if(lp)
                  {
                    norm_tau = sqrt(cisaillement(num_face,0)*cisaillement(num_face,0)
                                    +cisaillement(num_face,1)*cisaillement(num_face,1));
                  }
                else norm_tau = d_visco*norm_v/dist;

                u_etoile = sqrt(norm_tau);
                y_plus(elem) = dist*u_etoile/d_visco;

              }

          else if (dimension == 3)
            for (int num_face=ndeb; num_face<nfin; num_face++)
              {
                elem =face_voisins(num_face,0);

                num[0]=elem_faces(elem,0);
                num[1]=elem_faces(elem,1);
                num[2]=elem_faces(elem,2);

                if (num[0]==num_face) num[0]=elem_faces(elem,3);
                else if (num[1]==num_face) num[1]=elem_faces(elem,3);
                else if (num[2]==num_face) num[2]=elem_faces(elem,3);

                dist = distance_3D(num_face,elem,zone_VEF);
                dist *= 4./3.; // pour se ramener a distance paroi / milieu de num[0]-num[1]-num[2]
                norm_v=norm_3D_vit1_lp(vit, num_face, num[0], num[1], num[2], zone_VEF, val1, val2, val3);

                if (l_unif)
                  d_visco = visco;
                else
                  d_visco = tab_visco[elem];

                // PQ : 01/10/03 : corrections par rapport a la version premiere

                if(lp)
                  {
                    norm_tau = sqrt(cisaillement(num_face,0)*cisaillement(num_face,0)
                                    +cisaillement(num_face,1)*cisaillement(num_face,1)
                                    +cisaillement(num_face,2)*cisaillement(num_face,2));
                  }
                else norm_tau = d_visco*norm_v/dist;

                u_etoile = sqrt(norm_tau);
                y_plus(elem) = dist*u_etoile/d_visco;

              }
        } // Fin paroi fixe

    } // Fin boucle sur les bords
}


void Champ_P1NC::calcul_grad_U(const Zone_Cl_VEF& zone_Cl_VEF, DoubleTab& grad_u) const
{
  const DoubleTab& u = valeurs();
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const int nb_elem = zone_VEF.nb_elem();

  const int nb_elem_tot = zone_VEF.nb_elem_tot();

  DoubleTab gradient_elem(nb_elem_tot,dimension,dimension);
  gradient_elem=0.;

  calculer_gradientP1NC(u,zone_VEF,zone_Cl_VEF,gradient_elem);

  for (int elem=0; elem<nb_elem; elem++)
    {
      int comp=0;
      for (int i=0; i<dimension; i++)
        for(int j=0; j<dimension; j++)
          {
            grad_u(elem,comp) = gradient_elem(elem,i,j);
            comp++;
          }
    }


}

void Champ_P1NC::calcul_grad_T(const Zone_Cl_VEF& zone_Cl_VEF, DoubleTab& grad_T) const
{
  const DoubleTab& temp = valeurs();
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  calculer_gradientP1NC(temp,zone_VEF,zone_Cl_VEF,grad_T);

  // *******************************************************************
  //  PQ : 12/12/05 : pour pouvoir visualiser les oscillations a partir
  //                    des changements de signes du gradient de T
  // *******************************************************************
  /*
      const IntTab& face_voisins = zone_VEF.face_voisins();
      const IntTab& elem_faces=zone_VEF.elem_faces();
      int nb_faces = zone_VEF.nb_faces();
      const int nb_elem = zone_VEF.nb_elem_tot();

      int num_elem,num_face,elem0,elem1,i;
      ArrOfInt sign_opp_x(nb_faces);
      ArrOfInt sign_opp_y(nb_faces);
      sign_opp_x=0;
      sign_opp_y=0;

      for (num_face=0;num_face<nb_faces;num_face++)
      {
      elem0 = face_voisins(num_face,0);
      elem1 = face_voisins(num_face,1);
      if (elem1!=-1)
      {
      if  (grad_T(elem0,0)*grad_T(elem1,0)<0.) sign_opp_x(num_face)++;
      if  (grad_T(elem0,1)*grad_T(elem1,1)<0.) sign_opp_y(num_face)++;
      }
      }

      grad_T=0.;

      for (num_elem=0;num_elem<nb_elem;num_elem++)
      {
      for (i=0;i<dimension+1;i++)
      {
      grad_T(num_elem,0) +=sign_opp_x(elem_faces(num_elem,i));
      grad_T(num_elem,1) +=sign_opp_y(elem_faces(num_elem,i));
      }
      }
  */

}

// Calcul du coefficient d'echange
void Champ_P1NC::calcul_h_conv(const Zone_Cl_VEF& zone_Cl_VEF, DoubleTab& h_conv, int temp_ref) const
{
  const DoubleTab& temp = valeurs();
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const IntTab& face_voisins = zone_VEF.face_voisins();
  const DoubleTab& face_normale = zone_VEF.face_normales();

  // On recupere le flux aux frontieres
  DoubleTab Flux;
  Flux=equation().operateur(0).l_op_base().flux_bords();

  // On initialise a -1
  h_conv=-1.;

  double T_ref=double(temp_ref);

  for (int n_bord=0; n_bord<zone_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VEF.les_conditions_limites(n_bord);
      const Frontiere_dis_base& la_fr = la_cl.frontiere_dis();
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();
      double h_moy = 0.;
      // Selon les conditions limites
      if (sub_type(Echange_externe_impose,la_cl.valeur()))
        {
          const Champ_Don& rho = equation().milieu().masse_volumique();
          const Champ_Don& Cp = equation().milieu().capacite_calorifique();
          int rho_uniforme=(sub_type(Champ_Uniforme,rho.valeur()) ? 1:0);
          int cp_uniforme=(sub_type(Champ_Uniforme,Cp.valeur()) ? 1:0);
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              int elem = face_voisins(num_face,0);
              double rho_cp = (rho_uniforme?rho(0,0):(rho.nb_comp()==1?rho(num_face):rho(num_face,0)));
              rho_cp *= (cp_uniforme?Cp(0,0):(Cp.nb_comp()==1?Cp(num_face):Cp(num_face,0)));
              h_conv(elem) = ref_cast(Echange_externe_impose,la_cl.valeur()).h_imp(num_face)*rho_cp;
              h_moy+=h_conv(elem);
            }
        }
      else  if (sub_type(Scalaire_impose_paroi,la_cl.valeur()) ||
                sub_type(Neumann_homogene,la_cl.valeur())  ||
                sub_type(Neumann_paroi,la_cl.valeur()) )
        {
          for (int num_face=ndeb; num_face<nfin; num_face++)
            {
              int elem = face_voisins(num_face,0);

              double norme_flux=0.;
              double surf=0.;

              for(int k=0; k<Flux.dimension(1); k++)
                norme_flux+=Flux(num_face,k)*Flux(num_face,k);

              for(int k=0; k<Objet_U::dimension; k++)
                surf+=face_normale(num_face,k)*face_normale(num_face,k);

              double DT=temp(num_face)-T_ref;

              if (DT==0.)
                h_conv(elem)=0.;
              else
                {
                  //La mise a jour de ce champ est faite quand on demande son postraitement
                  //et par consequent les flux ont ete mis a jour et sont deja multiplies par rho*Cp
                  h_conv(elem)=sqrt(norme_flux/surf)/DT;
                }
              h_moy+=h_conv(elem);
            }
        }
      h_moy=Process::mp_sum(h_moy);
      int nb_faces=Process::mp_sum(le_bord.nb_faces());
      h_moy/=nb_faces;
      if(je_suis_maitre())
        {
          Nom fich_hconv;
          fich_hconv  = "Heat_transfert_Coeff_";
          fich_hconv += la_fr.le_nom();
          fich_hconv += ".dat";

          SFichier fic(fich_hconv,ios::app);
          fic.setf(ios::scientific);

          double le_temps = equation().schema_temps().temps_courant();
          fic << le_temps << " " << h_moy << finl;
        }
    }
}


static double norme_L2(const DoubleTab& u, const Zone_VEF& zone_VEF)
{
  const DoubleVect& volumes = zone_VEF.volumes_entrelaces();
  const int nb_faces = zone_VEF.nb_faces();
  int i=0;
  double norme =0;
  int nb_compo_=u.dimension(1);
  for(; i<nb_faces; i++)
    {
      double s=0;
      for(int j=0; j<nb_compo_; j++)
        s+=u(i,j)*u(i,j);
      norme+=volumes(i)*s;
    }
  return sqrt(norme);
}
double Champ_P1NC::norme_L2(const Domaine& dom) const
{
  Cerr << "Champ_P1NC::norme_L2" << finl;
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  {
    DoubleTab x(valeurs());
    filtrer_L2(x);
    Cerr << "Norme filtree : " << ::norme_L2(x, zone_VEF) << finl;
  }
  const DoubleTab& u = valeurs();
  return ::norme_L2(u, zone_VEF);
}

double Champ_P1NC::norme_H1(const Domaine& dom) const
{
  const Zone_VEF& zone_VEF = la_zone_VEF.valeur();
  const Zone& ma_zone = zone_VEF.zone();

  double dnorme_H1,norme_H1_comp,int_grad_elem,norme_grad_elem;
  int face_globale;

  //On va calculer la norme H1 d'une inconnue P1NC.
  //L'algorithme tient compte des contraintes suivantes:
  //- l'inconnue peut avoir plusieurs composantes
  //  (i.e etre un scalaire ou etre un vecteur)
  //- la dimension du probleme est arbitraire (1, 2 ou 3).
  //ATTENTION: les prismes ne sont pas supportes.

  dnorme_H1=0.;
  for (int composante=0; composante<nb_comp(); composante++) //cas scalaire ou vectoriel
    {
      norme_H1_comp=0.; //pour eviter les accumulations
      for (int K=0; K<ma_zone.nb_elem(); K++) //boucle sur les elements
        {
          norme_grad_elem=0.; //pour eviter les accumulations
          for (int i=0; i<dimension; i++) //boucle sur la dimension du pb
            {
              int_grad_elem=0.; //pour eviter les accumulations
              for (int face=0; face<ma_zone.nb_faces_elem(); face++) //boucle sur les faces d'un "K"
                {
                  face_globale = zone_VEF.elem_faces(K,face);

                  int_grad_elem += (valeurs())(face_globale,composante)*
                                   zone_VEF.face_normales(face_globale,i)*
                                   zone_VEF.oriente_normale(face_globale,K);
                } //fin du for sur "face"

              norme_grad_elem += int_grad_elem*int_grad_elem;
            } //fin du for sur "i"

          norme_H1_comp += norme_grad_elem/zone_VEF.volumes(K);
        } //fin du for sur "K"

      dnorme_H1 += norme_H1_comp;
    } // fin du for sur "composante"

  return sqrt(dnorme_H1);
}

double Champ_P1NC::norme_L2_H1(const Domaine& dom) const
{
  double pas_de_temps = equation().schema_temps().pas_de_temps();

  return ((1./pas_de_temps)*norme_L2(dom))+norme_H1(dom);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////
//Methode qui renvoie gij aux elements a partir du champ aux faces
//(gij represente la derivee partielle dGi/dxj)
//A partir de gij, on peut calculer Sij = 0.5(gij(i,j)+gij(j,i))
////////////////////////////////////////////////////////////////////////////////////////////////////

DoubleTab& Champ_P1NC::calcul_gradient(const DoubleTab& champ, DoubleTab& gij, const Zone_Cl_VEF& zone_Cl_VEF)
{
  const Zone_VEF& zone_VEF = zone_Cl_VEF.zone_VEF();
  calculer_gradientP1NC(champ,zone_VEF,zone_Cl_VEF,gij);
  return gij;

}

///////////////////////////////////////////////////////////////////////////////////
//Modification de duidxj pour tenir compte de laloi de paroi
//Methode recuperee dans Op_Dift_standard_VEF_Face.cpp
//(Contenu a verifier)
//////////////////////////////////////////////////////////////////////////////////

//DoubleTab& Champ_P1NC::calcul_duidxj_paroi(DoubleTab& gij, const DoubleTab& nu, const DoubleTab& nu_turb, const DoubleTab& tau_tan, const int //indic_lp_neg, const int indic_bas_Re, const Zone_Cl_VEF& zone_Cl_VEF)
//{
//
//  const Zone_VEF& zone_VEF = zone_Cl_VEF.zone_VEF();
//  const DoubleTab& face_normale = zone_VEF.face_normales();
//  const IntTab& face_voisins = zone_VEF.face_voisins();
//
//  int i,j,fac,num1;
//
//  // On va modifier Sij pour les faces de Dirichlet (Paroi_fixe) si nous n utilisons pas Paroi_negligeable!!!!
//  // On aura : (grad u)_f = (grad u) - (grad u . n) x n + (grad u . n)_lp x n
//  //
//
//  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
//  int nb_cl=les_cl.size();
////  if (!sub_type(Paroi_negligeable_VEF,loi_par))  Normalement il faudrait ca!!
//  if ((indic_lp_neg!=1)&&(indic_bas_Re!=1))
//      {
//        // Cerr << "On fait une modif aux calculs des gradient a cause de la loi de paroi!!!!" << finl;
//        DoubleTrav part1_int(Objet_U::dimension), part1(Objet_U::dimension,Objet_U::dimension), part2(Objet_U::dimension,Objet_U::dimension);
//
//        for ( int num_cl=0; num_cl<nb_cl; num_cl++)
//          { //Boucle sur les bords
//            const Cond_lim& la_cl = les_cl[num_cl];
//            const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
//            int ndeb = la_front_dis.num_premiere_face();
//            int nfin = ndeb + la_front_dis.nb_faces();
//
//            if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) ||
//                sub_type(Dirichlet_paroi_defilante,la_cl.valeur()))
//              {
//                for (fac=ndeb; fac<nfin ; fac++)
//                  {
//                    // Boucle sur les faces
//
//                    num1 = face_voisins(fac,0);
//
//                    double Surf =0.;
//                    for (j=0 ; j<Objet_U::dimension ; j++)
//                      Surf += face_normale(fac,j)*face_normale(fac,j);
//
//                    part1_int = 0. ;
//                    for (i=0 ; i<Objet_U::dimension ; i++)
//                      for (j=0 ; j<Objet_U::dimension ; j++)
//                        part1_int[i] += gij(num1,i,j)*face_normale(fac,j); // Signe?? hyp : 0 -> 1 donc exterieur!!!!
//
//                    for (i=0 ; i<Objet_U::dimension ; i++)
//                      for (j=0 ; j<Objet_U::dimension ; j++)
//                        part1(i,j) =   part1_int[i] * face_normale(fac,j); // Signe?? hyp : 0 -> 1 donc exterieur!!!!
//
//                    for (i=0 ; i<Objet_U::dimension ; i++)
//                      for (j=0 ; j<Objet_U::dimension ; j++)
//                        part2(i,j) =   tau_tan(fac,i)*face_normale(fac,j);
//
//                    // MODIFS POUR LE SIGNE
//                    // LE SIGNE N EST PAS PRESENT DANS LA LOI DE PAROI : donne u_star^2
//                    // On determine le signe ici puisque c est le meme que celui de Sij x n
//                    double signe;
//
//
//                    // On modifie maintenant Sij
//                    for (i=0 ; i<Objet_U::dimension ; i++)
//                      for (j=0 ; j<Objet_U::dimension ; j++)
//                        {
//                          signe = 1.;
//
//                          if (part1(i,j)<0.) signe=-1;
//
//                              gij(num1,i,j) -= part1(i,j)/Surf;
//
//                              // MODIF SB le 20/08/01 on divise par nu+nu_t
//                              gij(num1,i,j) += signe*dabs(part2(i,j))/(sqrt(Surf)*(nu[num1]+nu_turb[num1])); // ATTENTION car la normale n est pas unitaire!!!
//                        }
//                  }
//              }
//          }
//      }
//
//  return gij;
//}

DoubleTab& Champ_P1NC::calcul_duidxj_paroi(DoubleTab& gij, const DoubleTab& nu, const DoubleTab& nu_turb, const DoubleTab& tau_tan, const int indic_lp_neg, const int indic_bas_Re, const Zone_Cl_VEF& zone_Cl_VEF)
{

  const Zone_VEF& zone_VEF = zone_Cl_VEF.zone_VEF();
  const DoubleTab& face_normale = zone_VEF.face_normales();
  const IntTab& face_voisins = zone_VEF.face_voisins();

  const DoubleVect& porosite_face = zone_VEF.porosite_face();

  int i,j,fac,num1;

  // On va modifier Sij pour les faces de Dirichlet (Paroi_fixe) si nous n utilisons pas Paroi_negligeable!!!!
  // On aura : (grad u)_f = (grad u) - (grad u . n) x n + (grad u . n)_lp x n
  //

  // PQ : 25/01/08 : Mise au propre de la methode pour ne plus etre embete par les histoires de signes notamment
  //                     et savoir reellement ce qu'on fait !!
  //
  // PRINCIPE :
  // (expose en 2D, l'extension au 3D ne pose pas de probleme particulier)
  //
  // On considere 2 reperes : le repere global (x,y) et le repere local (t,n) ou n designe la normale a la paroi
  // Le frottement retourne par la loi de paroi correspond a : || tau_tan || = u*^2 = nu.d(u_t)/dn
  // Soit P la matrice de passage permettant de passer d'un repere a l'autre
  //
  //   P = ( tx  nx )
  //           ( ty  ny )
  //
  // Les matrices des gradients de vitesse dans chacun des reperes :
  //
  //  G_(x,y) =  ( du/dx  du/dy )    et   F_(t,n) = ( du_t/dt   du_t/dn )
  //                 ( dv/dx  dv/dy )                    ( du_n/dt   du_n/dn )
  //
  //                                                                     -1
  // sont reliees l'une a l'autre par :   G_(x,y)  =  P . F_(t,n) . P
  //
  //
  //  Ainsi la correction apportee dans F = ( du_t/dt   du_t/dn + C  )  ou  C = -du_t/dn + tau_tan/nu
  //                                            ( du_n/dt   du_n/dn             )
  //
  //  se reporte dans G de la maniere suivante :
  //
  //                                              -1
  //  G*_(x,y) = G_(x,y) +  P . ( 0  C ). P   =  G_(x,y) + ( C.nx.tx  C.ny.tx )
  //                                ( 0  0 )                   ( C.nx.ty  C.ny.ty )
  //
  //
  // c'est ce dernier terme qu'il s'agit donc de determiner ici



  const Conds_lim& les_cl = zone_Cl_VEF.les_conditions_limites();
  int nb_cl=les_cl.size();
  //  if (!sub_type(Paroi_negligeable_VEF,loi_par))  Normalement il faudrait ca!!
  if ((indic_lp_neg!=1)&&(indic_bas_Re!=1))
    {
      // Cerr << "On fait une modif aux calculs des gradient a cause de la loi de paroi!!!!" << finl;
      //        DoubleTrav part1_int(Objet_U::dimension), part1(Objet_U::dimension,Objet_U::dimension), part2(Objet_U::dimension,Objet_U::dimension);
      DoubleTab P(dimension,dimension);

      for ( int num_cl=0; num_cl<nb_cl; num_cl++)
        {
          //Boucle sur les bords
          const Cond_lim& la_cl = les_cl[num_cl];
          const Front_VF& la_front_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = la_front_dis.num_premiere_face();
          int nfin = ndeb + la_front_dis.nb_faces();

          if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()) ||
              sub_type(Dirichlet_paroi_defilante,la_cl.valeur()))
            {

              for (fac=ndeb; fac<nfin ; fac++)
                {
                  // Boucle sur les faces

                  num1 = face_voisins(fac,0);


                  /////////////////////////////////////////////////////////////////
                  // definition des vecteurs unitaires constituant le repere local
                  // stockes dans la matrice de passage P
                  /////////////////////////////////////////////////////////////////

                  // vecteur tangentiel (porte par la vitesse tangentielle)

                  double norme2_tau_tan = 0.;

                  for(i=0 ; i<Objet_U::dimension ; i++)
                    norme2_tau_tan += tau_tan(fac,i)*tau_tan(fac,i);

                  double norme_tau_tan =  sqrt(norme2_tau_tan);

                  for(i=0 ; i<Objet_U::dimension ; i++)
                    P(i,0) = tau_tan(fac,i)/(norme_tau_tan+DMINFLOAT);

                  // vecteur normal a la paroi

                  double norme2_n = 0.;
                  int signe = -zone_VEF.oriente_normale(fac, num1); // orientation vers l'interieur

                  for (i=0 ; i<Objet_U::dimension ; i++)
                    norme2_n += face_normale(fac,i)*face_normale(fac,i);

                  double norme_n = sqrt(norme2_n);

                  for(i=0 ; i<Objet_U::dimension ; i++)
                    P(i,1) = signe*face_normale(fac,i)/norme_n;


                  // (3D) on complete la base par le deuxieme vecteur tangentiel

                  if(Objet_U::dimension==3)
                    {
                      P(0,2) = P(1,0)*P(2,1) - P(2,0)*P(1,1) ;
                      P(1,2) = P(2,0)*P(0,1) - P(0,0)*P(2,1) ;
                      P(2,2) = P(0,0)*P(1,1) - P(1,0)*P(0,1) ;
                    }

                  /////////////////////////////////////////////////////////////////
                  //         determination du terme d(u_t)/dn a enlever
                  //                                                       -1
                  //         terme identifie a l'aide du produit : F =  P . G . P
                  //
                  /////////////////////////////////////////////////////////////////

                  double dutdn_old = 0.;

                  for (i=0 ; i<Objet_U::dimension ; i++)
                    for (j=0 ; j<Objet_U::dimension ; j++)
                      {
                        dutdn_old += gij(num1,i,j) * P(j,1) * P(i,0) ;
                      }

                  /////////////////////////////////////////////////////////////////
                  //         Correction finale apportee a la matrice G
                  /////////////////////////////////////////////////////////////////

                  double C = -dutdn_old + sqrt(norme2_tau_tan)/(nu[num1]+nu_turb[num1])*porosite_face(fac) ;

                  // la division par (nu[num1]+nu_turb[num1]) s'impose du fait que l'operateur de diffusion
                  // fait intervenir le produit : (nu[num1]+nu_turb[num1])*g(i,j)


                  for (i=0 ; i<Objet_U::dimension ; i++)
                    for (j=0 ; j<Objet_U::dimension ; j++)
                      {
                        gij(num1,i,j) += C * P(j,1) * P(i,0) ;
                      }
                }
            }
        }
    }

  return gij;
}

////////////////////
// Calcul de 2SijSij
////////////////////
DoubleVect& Champ_P1NC::calcul_S_barre(const DoubleTab& la_vitesse,DoubleVect& SMA_barre,const Zone_Cl_VEF& zone_Cl_VEF)
{
  const Zone_VEF& zone_VEF = zone_Cl_VEF.zone_VEF();
  const int nb_elem = zone_VEF.nb_elem();
  const int nb_elem_tot = zone_VEF.nb_elem_tot();

  DoubleTab duidxj(nb_elem_tot,dimension,dimension);

  Champ_P1NC::calcul_gradient(la_vitesse,duidxj,zone_Cl_VEF);

  for (int elem=0 ; elem<nb_elem; elem++)
    {
      double temp = 0.;
      for (int i=0 ; i<dimension ; i++)
        for (int j=0 ; j<dimension ; j++)
          {
            double Sij=0.5*(duidxj(elem,i,j) + duidxj(elem,j,i));
            temp+=Sij*Sij;
          }
      SMA_barre(elem)=2.*temp;
    }
  return SMA_barre;
}

double Champ_P1NC::calculer_integrale_volumique(const Zone_VEF& zone, const DoubleVect& v, Ok_Perio ok)
{
  if (ok != FAUX_EN_PERIO)
    {
      // BM: desole
      Cerr << "Champ_P1NC::calculer_integrale_volumique pas encore code juste en perio !" << finl;
      exit();
    }

  assert(v.line_size() == 1);
  const DoubleVect& volume = zone.volumes_entrelaces();
  const double resu = mp_prodscal(v, volume);
  return resu;
}

int Champ_P1NC::fixer_nb_valeurs_nodales(int nb_noeuds)
{
  assert(nb_noeuds == zone_vef().nb_faces());
  const MD_Vector& md = zone_vef().md_vector_faces();
  creer_tableau_distribue(md);

  Champ_P1NC_implementation::fixer_nb_valeurs_nodales(nb_noeuds);

  return nb_noeuds;
}
