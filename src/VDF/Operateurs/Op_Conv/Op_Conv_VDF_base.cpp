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
// File:        Op_Conv_VDF_base.cpp
// Directory:   $TRUST_ROOT/src/VDF/Operateurs/Op_Conv
// Version:     /main/31
//
//////////////////////////////////////////////////////////////////////////////

#include <Modifier_pour_fluide_dilatable.h>
#include <Discretisation_base.h>
#include <Op_Conv_VDF_base.h>
#include <Probleme_base.h>
#include <TRUSTTrav.h>
#include <Champ.h>
#include <Debog.h>

Implemente_base(Op_Conv_VDF_base,"Op_Conv_VDF_base",Operateur_Conv_base);

Sortie& Op_Conv_VDF_base::printOn(Sortie& s ) const { return s << que_suis_je() ; }
Entree& Op_Conv_VDF_base::readOn(Entree& s ) { return s ; }

inline void eval_fluent(const double& psc,const int num1,const int num2, DoubleVect& fluent)
{
  if (psc >= 0) fluent[num2] += psc;
  else fluent[num1] -= psc;
}

///////////////////////////////////////////////////////////////////////////////////
//
//    Implementation de fonctions de la classe Op_Conv_VDF_base
//
///////////////////////////////////////////////////////////////////////////////////

double Op_Conv_VDF_base::calculer_dt_stab() const
{
  const Zone_VDF& zone_VDF = iter.zone();
  const Zone_Cl_VDF& zone_Cl_VDF = iter.zone_Cl();
  const IntTab& face_voisins = zone_VDF.face_voisins();
  const DoubleVect& volumes = zone_VDF.volumes();
  const DoubleVect& face_surfaces = zone_VDF.face_surfaces();
  const DoubleVect& vit_associe = vitesse().valeurs();
  const DoubleVect& vit= (vitesse_pour_pas_de_temps_.non_nul()?vitesse_pour_pas_de_temps_.valeur().valeurs(): vit_associe);
  DoubleTab fluent;
  // fluent est initialise a zero par defaut:
  zone_VDF.zone().creer_tableau_elements(fluent);

  // Remplissage du tableau fluent
  double psc;
  int num1, num2, face, elem1;

  // On traite les bords
  for (int n_bord = 0; n_bord < zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);
      if ( sub_type(Dirichlet_entree_fluide,la_cl.valeur()) || sub_type(Neumann_sortie_libre,la_cl.valeur()) )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          num1 = le_bord.num_premiere_face();
          num2 = num1 + le_bord.nb_faces();
          for (face=num1; face<num2; face++)
            {
              psc = vit[face]*face_surfaces(face);
              if ( (elem1 = face_voisins(face,0)) != -1)
                {
                  if (psc < 0) fluent[elem1] -= psc;
                }
              else // (elem2 != -1)
                if (psc > 0) fluent[face_voisins(face,1)] += psc;
            }
        }
    }

  // Boucle sur les faces internes pour remplir fluent
  const int zone_VDF_nb_faces = zone_VDF.nb_faces(), premiere_face = zone_VDF.premiere_face_int();
  for (face = premiere_face; face < zone_VDF_nb_faces; face++)
    {
      psc = vit[face]*face_surfaces(face);
      eval_fluent(psc,face_voisins(face,0),face_voisins(face,1),fluent);
    }

  // Calcul du pas de temps de stabilite a partir du tableau fluent
  if (vitesse().le_nom()=="rho_u" && equation().probleme().is_dilatable())
    diviser_par_rho_si_dilatable(fluent,equation().milieu());

  double dt_stab = 1.e30;
  int zone_VDF_nb_elem=zone_VDF.nb_elem();
  // dt_stab = min ( 1 / ( |U|/dx + |V|/dy + |W|/dz ) )
  for (int num_poly=0; num_poly<zone_VDF_nb_elem; num_poly++)
    {
      double dt_elem = volumes(num_poly)/(fluent[num_poly]+DMINFLOAT);
      if (dt_elem<dt_stab) dt_stab = dt_elem;
    }
  dt_stab = Process::mp_min(dt_stab);

  // astuce pour contourner le type const de la methode
  Op_Conv_VDF_base& op =ref_cast_non_const(Op_Conv_VDF_base, *this);
  op.fixer_dt_stab_conv(dt_stab);
  return dt_stab;
}

//Description
// Calculation of local time: Vect of size number of faces of the domain
// This is the equivalent of "Op_Conv_VDF_base :: calculer_dt_stab ()"
void Op_Conv_VDF_base::calculer_dt_local(DoubleTab& dt_face) const
{
  const Zone_VDF& zone_VDF = iter.zone();
  const Zone_Cl_VDF& zone_Cl_VDF = iter.zone_Cl();
  const DoubleVect& volumes_entrelaces= zone_VDF.volumes_entrelaces();
  const DoubleVect& face_surfaces = zone_VDF.face_surfaces();
  //const DoubleVect& vit= vitesse_pour_pas_de_temps_.valeur().valeurs();
  const DoubleVect& vit=equation().inconnue().valeurs();
  DoubleTrav fluent(volumes_entrelaces);

  // Remplissage du tableau fluent
  // On traite les bords
  for (int n_bord = 0; n_bord < zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);

      if ( sub_type(Dirichlet_entree_fluide,la_cl.valeur()) || sub_type(Neumann_sortie_libre,la_cl.valeur())  )
        {
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          const int num1 = le_bord.num_premiere_face(), num2 = num1 + le_bord.nb_faces();
          for (int face = num1; face < num2; face++)
            {
              double value = vit[face]*face_surfaces(face);
              if (value >0) fluent[face] = value;
              else fluent[face] -= value;
            }
        }
    }

  // Boucle sur les faces internes pour remplir fluent
  const int zone_VDF_nb_faces = zone_VDF.nb_faces(), premiere_face = zone_VDF.premiere_face_int();
  for (int face = premiere_face; face < zone_VDF_nb_faces; face++)
    {
      const double value = vit[face]*face_surfaces(face);
      if (value >0) fluent[face] = value;
      else fluent[face] -= value;
    }


  // Calcul du pas de temps de stabilite a partir du tableau fluent
  if (vitesse().le_nom()=="rho_u" && equation().probleme().is_dilatable())
    diviser_par_rho_si_dilatable(fluent,equation().milieu());

  dt_face=(volumes_entrelaces);
  // Boucle sur les faces de bords
  for (int n_bord=0; n_bord<zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();
      for (int num_face = ndeb; num_face < nfin; num_face++)
        {
          if( sup_strict(fluent[num_face], 1.e-16) ) dt_face(num_face)= volumes_entrelaces(num_face)/fluent[num_face];
          else dt_face(num_face) = -1.;
        }
    }

  // Boucle sur les faces internes
  for (int num_face = premiere_face; num_face<zone_VDF_nb_faces; num_face++)
    {
      if( sup_strict(fluent[num_face], 1.e-16) ) dt_face(num_face)= volumes_entrelaces(num_face)/fluent[num_face];
      else dt_face(num_face) = -1.;
    }

  double max_dt_local= dt_face.mp_max_abs_vect();
  const int taille = dt_face.size();
  for(int i = 0; i < taille; i++)
    if(! sup_strict(dt_face(i), 1.e-16)) dt_face(i) = max_dt_local;

  dt_face.echange_espace_virtuel();

  for (int n_bord = 0; n_bord < zone_VDF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& la_cl_perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          const int nb_faces_bord = le_bord.nb_faces();
          for (int ind_face = 0; ind_face < nb_faces_bord; ind_face++)
            {
              int ind_face_associee = la_cl_perio.face_associee(ind_face);
              int face = le_bord.num_face(ind_face), face_associee = le_bord.num_face(ind_face_associee);
              if (!est_egal(dt_face(face),dt_face(face_associee),1.e-8))
                {
                  dt_face(face) = std::min(dt_face(face),dt_face(face_associee));
                }
            }
        }
    }
  dt_face.echange_espace_virtuel();
//  dt_conv_locaux=dt_face;
}

// cf Op_Conv_VDF_base::calculer_dt_stab() pour choix de calcul de dt_stab
void Op_Conv_VDF_base::calculer_pour_post(Champ& espace_stockage,const Nom& option,int comp) const
{
  if (Motcle(option)=="stabilite")
    {
      DoubleTab& es_valeurs = espace_stockage->valeurs();
      es_valeurs = 1.e30;

      const Zone_VDF& zone_VDF = iter.zone();
      const Zone_Cl_VDF& zone_Cl_VDF = iter.zone_Cl();
      const IntTab& face_voisins = zone_VDF.face_voisins();
      const DoubleVect& volumes = zone_VDF.volumes();
      const DoubleVect& face_surfaces = zone_VDF.face_surfaces();
      const DoubleVect& vit = vitesse().valeurs();
      DoubleTrav fluent(zone_VDF.zone().nb_elem_tot());

      // Remplissage du tableau fluent
      fluent = 0;
      double psc;
      int num1, num2, face, elem1;

      // On traite les bords
      for (int n_bord = 0; n_bord < zone_VDF.nb_front_Cl(); n_bord++)
        {
          const Cond_lim& la_cl = zone_Cl_VDF.les_conditions_limites(n_bord);
          if ( sub_type(Dirichlet_entree_fluide,la_cl.valeur()) || sub_type(Neumann_sortie_libre,la_cl.valeur())  )
            {
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              num1 = le_bord.num_premiere_face();
              num2 = num1 + le_bord.nb_faces();
              for (face = num1; face < num2; face++)
                {
                  psc = vit[face]*face_surfaces(face);
                  if ( (elem1 = face_voisins(face,0)) != -1)
                    {
                      if (psc < 0) fluent[elem1] -= psc;
                    }
                  else // (elem2 != -1)
                    if (psc > 0) fluent[face_voisins(face,1)] += psc;
                }
            }
        }

      // Boucle sur les faces internes pour remplir fluent
      const int zone_VDF_nb_faces = zone_VDF.nb_faces();
      for (face = zone_VDF.premiere_face_int(); face < zone_VDF_nb_faces; face++)
        {
          psc = vit[face]*face_surfaces(face);
          eval_fluent(psc,face_voisins(face,0),face_voisins(face,1),fluent);
        }
      //fluent.echange_espace_virtuel();
      if (vitesse().le_nom()=="rho_u" && equation().probleme().is_dilatable())
        diviser_par_rho_si_dilatable(fluent,equation().milieu());

      const int zone_VDF_nb_elem=zone_VDF.nb_elem();
      for (int num_poly=0; num_poly<zone_VDF_nb_elem; num_poly++)
        es_valeurs(num_poly) = volumes(num_poly)/(fluent[num_poly]+1.e-30);

      //double dt_min = mp_min_vect(es_valeurs);
      //assert(dt_min==calculer_dt_stab());
    }
  else
    Operateur_Conv_base::calculer_pour_post(espace_stockage,option,comp);
}

Motcle Op_Conv_VDF_base::get_localisation_pour_post(const Nom& option) const
{
  Motcle loc;
  if (Motcle(option)=="stabilite") loc = "elem";
  else return Operateur_Conv_base::get_localisation_pour_post(option);
  return loc;
}

void Op_Conv_VDF_base::completer()
{
  Operateur_base::completer();
  iter.completer_();
}

void Op_Conv_VDF_base::associer_zone_cl_dis(const Zone_Cl_dis_base& zcl)
{
  iter.valeur().associer_zone_cl_dis(zcl);
}

int Op_Conv_VDF_base::impr(Sortie& os) const
{
  return iter.impr(os);
}
