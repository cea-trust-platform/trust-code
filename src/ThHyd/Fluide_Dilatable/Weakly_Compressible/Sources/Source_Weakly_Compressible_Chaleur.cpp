/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Source_Weakly_Compressible_Chaleur.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Fluide_Dilatable/Weakly_Compressible/Sources
// Version:     /main/11
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Weakly_Compressible_Chaleur.h>
#include <Fluide_Weakly_Compressible.h>
#include <Equation_base.h>
#include <Probleme_base.h>
#include <Schema_Temps_base.h>
#include <Navier_Stokes_WC.h>
#include <Zone_VF.h>
#include <Neumann_sortie_libre.h>

Implemente_base(Source_Weakly_Compressible_Chaleur,"Source_Weakly_Compressible_Chaleur",Source_base);


// Description:
//    Imprime la source sur un flot de sortie.
// Precondition:
// Parametre: Sortie& os
//    Signification: le flot de sortie pour l'impression
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord: le flot de sortie est modifie
// Postcondition: la methode ne modifie pas l'objet
Sortie& Source_Weakly_Compressible_Chaleur::printOn(Sortie& os) const
{
  os <<que_suis_je()<< finl;
  return os;
}

// Description:
//    Lecture de la source sur un flot d'entree.
// Precondition:
// Parametre: Entree& is
//    Signification: le flot d'entree pour la lecture des parametres
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Source_Weakly_Compressible_Chaleur::readOn(Entree& is)
{
  return is;
}

// Description:
//    Complete la source : rempli la ref sur le fluide
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Source_Weakly_Compressible_Chaleur::completer()
{
  Source_base::completer();
  le_fluide = ref_cast(Fluide_Weakly_Compressible,mon_equation->milieu());
}

// Description:
//    Calcule la contrinution de cette source
// Precondition:
// Parametre: DoubleTab& resu
//    Signification: flux
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: DoubleTab&
//    Signification: le flux
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Source_Weakly_Compressible_Chaleur::calculer(DoubleTab& resu) const
{
  return ajouter(resu);
}

// Description:
//    Ajoute les termes sources
// Precondition:
// Parametre:
//    Signification:
//    Valeurs par defaut:
//    Contraintes:
//    Acces:
// Retour: Entree&
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
DoubleTab& Source_Weakly_Compressible_Chaleur::ajouter(DoubleTab& resu) const
{
  double dt_ = mon_equation->schema_temps().temps_courant() - mon_equation->schema_temps().temps_precedent();

  if (dt_<=0) return resu; // On calcul pas ce terme source si dt<=0

  /*
   * The source term corresponds to :
   * d P_tot / d t = del P / del t + u.grad(P_tot)
   */

  Fluide_Weakly_Compressible& FWC = ref_cast_non_const(Fluide_Weakly_Compressible,le_fluide.valeur());
  const DoubleTab& Ptot = FWC.pression_th_tab(), Ptot_n = FWC.pression_thn_tab(); // present & passe

  // compute the grad
  const Navier_Stokes_WC& eqHyd = ref_cast(Navier_Stokes_WC,mon_equation->probleme().equation(0));
  const DoubleTab& la_vitesse = eqHyd.vitesse().valeurs();
  DoubleTab grad_Ptot(eqHyd.grad_P().valeurs()); // initialize as grad(P) of NS
  const Operateur_Grad& gradient = eqHyd.operateur_gradient();
  gradient.calculer(Ptot,grad_Ptot); // compute grad(P_tot)

  // XXX : very important : sinon we have values * V !
  const Solveur_Masse& solv_mass = eqHyd.solv_masse();
  solv_mass.appliquer(grad_Ptot);

  /*
   * XXX XXX XXX
   * READ CAREFULLY : We use the grad operator from Navier_Stokes_std
   *    => created for the pressure normally
   * There is a special treatement for Neumann bc (Neumann_sortie_libre)
   * because we use explicitly the prescribed pressure.
   * So if we calculate the grad of Ptot or rho for example, we will use
   * P_ext defined in data file which is wrong !
   *
   * Now we do the following : we assume open bd as a wall => null gradient
   */

  // We use that of NS because we test the CL too (attention mon_equation is Chaleur...)
  const Zone_dis_base& zone_dis = eqHyd.inconnue().zone_dis_base();
  const Zone_VF& zone = ref_cast(Zone_VF, zone_dis);
  const Zone_Cl_dis& zone_cl = eqHyd.zone_Cl_dis();

  if (zone_dis.que_suis_je() != "Zone_VDF")
    {
      Cerr <<  "Source_Weakly_Compressible_Chaleur::ajouter Not coded yet for VEF !!" << finl;
      Process::exit();
    }

  for (int n_bord=0; n_bord<zone.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zone_cl.les_conditions_limites(n_bord);
      // corrige si Neumann_sortie_libre
      if ( sub_type(Neumann_sortie_libre,la_cl.valeur()) )
        {
          // recuperer face et remplace gradient par 0
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          const int ndeb = le_bord.num_premiere_face(), nfin = ndeb + le_bord.nb_faces();

          for (int num_face=ndeb; num_face<nfin; num_face++) grad_Ptot(num_face,0) = 0.;
        }
    }

  // We compute u*grad(P_tot) on each face
  DoubleTab UgradP(grad_Ptot); // field on faces
  const int n = la_vitesse.dimension(0);
  assert ( n == zone.nb_faces() );

  for (int i=0 ; i <n ; i++) UgradP(i) = la_vitesse(i) * grad_Ptot(i);

  const IntTab& elem_faces = zone.elem_faces();
  assert ( Ptot.dimension(0) == zone.nb_elem() );

  DoubleTab UgradP_elem(Ptot); // field on elem
  int face_x_0,face_x_1,face_y_0,face_y_1,face_z_0,face_z_1;
  double xx,yy,zz;
  // u.grad(P_tot) on each elem
  for (int num_elem=0; num_elem < zone.nb_elem(); num_elem++)
    {
      face_x_0 = elem_faces(num_elem,0);
      face_x_1 = elem_faces(num_elem,dimension);
      face_y_0 = elem_faces(num_elem,1);
      face_y_1 = elem_faces(num_elem,1+dimension);
      face_z_0 = (dimension == 3) ? elem_faces(num_elem,2) : 0;
      face_z_1 = (dimension == 3) ? elem_faces(num_elem,2+dimension) : 0;

      xx = UgradP(face_x_0,0) + UgradP(face_x_1,0);
      yy = UgradP(face_y_0,0) + UgradP(face_y_1,0);
      zz = (dimension == 3) ? UgradP(face_z_0,0) + UgradP(face_z_1,0) : 0.0;

      UgradP_elem(num_elem,0) = 0.5 * ( xx + yy + zz ); // mean at elem
    }

  assert ( zone.nb_elem() == resu.dimension(0));
  for (int i=0 ; i< zone.nb_elem() ; i++)
    {
      double dpth = ( Ptot(i,0) - Ptot_n(i,0) ) / dt_ +  UgradP_elem(i,0);
      resu(i) += dpth * volumes(i)*porosites(i) ;
    }

  return resu;
}
