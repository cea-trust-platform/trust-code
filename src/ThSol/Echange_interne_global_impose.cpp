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

#include <Echange_interne_global_impose.h>
#include <Equation_base.h>
#include <Champ_front_calc_interne.h>
#include <Probleme_base.h>
#include <Zone_Cl_dis_base.h>
#include <Zone_VF.h>
#include <DomaineAxi1d.h>


Implemente_instanciable(Echange_interne_global_impose,"Paroi_echange_interne_global_impose",Echange_global_impose);
// XD Echange_interne_global_impose condlim_base Paroi_echange_interne_global_impose -1 Internal heat exchange boundary condition with global exchange coefficient.
// XD attr h_imp chaine h_imp 0 Global exchange coefficient value. The global exchange coefficient value is expressed in W.m-2.K-1.
// XD attr ch front_field_base ch 0 Boundary field type.

// Description:
//    Ecrit le type de l'objet sur un flot de sortie
// Precondition:
// Parametre: Sortie& s
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
Sortie& Echange_interne_global_impose::printOn(Sortie& s ) const
{
  return s << que_suis_je() << "\n";
}

// Description:
//    Simple appel a Echange_impose_base::readOn(Entree&)
//    Lit les specifications des conditions aux limites
//    a partir d'un flot d'entree.
// Precondition:
// Parametre: Entree& s
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& Echange_interne_global_impose::readOn(Entree& s )
{
  Motcle motlu;
  Motcle motcle("h_gap");

  s >> motlu;
  if (motlu ==motcle)
    s >> h_imp_;
  else
    {
      Cerr << "Error when reading boundary condition with type Echange_interne_global_impose " << finl;
      Cerr << "We expected " << motcle << " instead of " <<  motlu << finl;
      exit();
    }

  init();

  return s;
}

void Echange_interne_global_impose::init()
{
  le_champ_front.typer("Champ_front_calc_interne"); // preparation de T_ext
}

// Override to skip verification - we know we will always be dealing with temperature, and we define ourselves the T_ext with
// a Champ_front_calc_interne.
void Echange_interne_global_impose::verifie_ch_init_nb_comp() const
{
}


// Finish building the champ_front_calc_interne
void Echange_interne_global_impose::completer()
{
  Nom nom_pb(zone_Cl_dis().equation().probleme().le_nom());
  Nom nom_bord(frontiere_dis().le_nom());
  Champ_front_calc_interne& t_ext = ref_cast(Champ_front_calc_interne, T_ext().valeur());

  t_ext.creer(nom_pb, nom_bord, "temperature");

  Echange_global_impose::completer();

  const Front_VF& fvf = ref_cast(Front_VF, frontiere_dis());
  const Zone_VF& zvf = ref_cast(Zone_VF, zone_Cl_dis().zone_dis().valeur());

  frontiere_dis().frontiere().creer_tableau_faces(surface_gap_, Array_base::NOCOPY_NOINIT);


  if (zvf.zone().domaine().Axi1d())
    {
      const DomaineAxi1d& domax = ref_cast(DomaineAxi1d,zvf.zone().domaine());
      const IntTab& face_voisins = zvf.face_voisins();
      const DoubleTab& xv = zvf.xv();
      const IntTab& fmap = t_ext.face_map();
      for (int i=0; i<fvf.nb_faces(); i++)
        {
          int iopp = fmap(i);
          int face = fvf.num_face(i);
          int face_opp = fvf.num_face(iopp);
          int elem = face_voisins(face,0)==-1 ? face_voisins(face,1) : face_voisins(face,0);
          double x0 = domax.origine_repere(elem,0);
          double y0 = domax.origine_repere(elem,1);
          double x1 = xv(face,0);
          double y1 = xv(face,1);
          double x2 = xv(face_opp,0);
          double y2 = xv(face_opp,1);
          double r1 = sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0));
          double r2 = sqrt((x2-x0)*(x2-x0)+(y2-y0)*(y2-y0));
          surface_gap_(i) = M_PI*(r1+r2);
        }
    }
  else
    {
      for (int i=0; i<fvf.nb_faces(); i++)
        {
          int face = fvf.num_face(i);
          surface_gap_(i) = zvf.surface(face);
        }
    }
}
