/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Source_Gravite_Quasi_Compressible_VEF.cpp
// Directory:   $TRUST_ROOT/src/ThHyd/Quasi_Compressible/VEF
// Version:     /main/15
//
//////////////////////////////////////////////////////////////////////////////

#include <Source_Gravite_Quasi_Compressible_VEF.h>
#include <Fluide_Quasi_Compressible.h>
#include <Equation_base.h>
#include <Zone_VEF.h>
#include <Zone_Cl_VEF.h>
#include <Les_Cl.h>
#include <Discretisation_base.h>

Implemente_instanciable(Source_Gravite_Quasi_Compressible_VEF,"Source_Gravite_Quasi_Compressible_VEF",Source_Gravite_Quasi_Compressible_base);


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
Sortie& Source_Gravite_Quasi_Compressible_VEF::printOn(Sortie& os) const
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
Entree& Source_Gravite_Quasi_Compressible_VEF::readOn(Entree& is)
{
  return is;
}

// Description:
//    Remplit le tableau volumes
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
void Source_Gravite_Quasi_Compressible_VEF::associer_zones(const Zone_dis& zone,const Zone_Cl_dis& zone_cl)
{
  la_zone = ref_cast(Zone_VEF,zone.valeur());
  la_zone_Cl = ref_cast(Zone_Cl_VEF,zone_cl.valeur());
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
void Source_Gravite_Quasi_Compressible_VEF::completer()
{
  Source_Gravite_Quasi_Compressible_base::completer();
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
DoubleTab& Source_Gravite_Quasi_Compressible_VEF::ajouter(DoubleTab& resu) const
{
  int i, face, nb_faces = la_zone->nb_faces();
  int premiere_face_interne = la_zone->premiere_face_int();
  const DoubleVect& volumes_entrelaces = la_zone->volumes_entrelaces();
  const DoubleVect& porosite_face = la_zone->porosite_face();
  const IntTab& face_voisins = la_zone->face_voisins();
  const DoubleTab& face_normales = la_zone->face_normales();
  const DoubleTab& xp = la_zone->xp();
  const DoubleTab& xv = la_zone->xv();
  //const DoubleTab& tab_rhoP1 = le_fluide->masse_volumique()->valeurs();
  const DoubleTab& tab_rho = ref_cast(Fluide_Quasi_Compressible,le_fluide.valeur()).rho_discvit();

  int num_cl;
  int elem1,elem2, comp;
  double delta_coord;

  if (mon_equation->discretisation().que_suis_je()=="VEF")
    {
      for (num_cl=0 ; num_cl<la_zone->nb_front_Cl() ; num_cl++)
        {
          const Cond_lim& la_cl = la_zone_Cl->les_conditions_limites(num_cl);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          if (sub_type(Neumann_sortie_libre,la_cl.valeur()))
            {
              for (face=ndeb ; face<nfin ; face++)
                {
                  elem1 = face_voisins(face,0);
                  if (elem1==-1)
                    {
                      elem1 = face_voisins(face,1);
                    }
                  for (comp=0 ; comp<dimension ; comp++)
                    {
                      delta_coord = (xv(face,comp) - xp(elem1,comp));
                      for (i=0 ; i<dimension ; i++)
                        {
                          resu(face,i) += tab_rho(face)*delta_coord*face_normales(face,i)*g(comp);
                        }
                    }
                }
            }
          else if (sub_type(Periodique,la_cl.valeur()))
            {
              for (face=premiere_face_interne ; face<nb_faces; face++)
                {
                  elem1 = face_voisins(face,0);
                  elem2 = face_voisins(face,1);
                  for (comp=0 ; comp<dimension ; comp++)
                    {
                      delta_coord = (xp(elem2,comp) - xp(elem1,comp));
                      for (i=0 ; i<dimension ; i++)
                        {
                          resu(face,i) += tab_rho(face)*delta_coord*face_normales(face,i)*g(comp);
                        }
                    }
                }
            }
        }

      for (face=premiere_face_interne ; face<nb_faces; face++)
        {
          elem1 = face_voisins(face,0);
          elem2 = face_voisins(face,1);
          for (comp=0 ; comp<dimension ; comp++)
            {
              delta_coord = (xp(elem2,comp) - xp(elem1,comp));
              for (i=0 ; i<dimension ; i++)
                {
                  resu(face,i) += tab_rho(face)*delta_coord*face_normales(face,i)*g(comp);
                }
            }
        }
    }
  else if (mon_equation->discretisation().que_suis_je()=="VEFPreP1B")
    {
      double rho_m=0;
      if (le_fluide.valeur().get_traitement_rho_gravite())
        {
          // On calcule rho_moy on le retire de la gravite
          // sensiblement egale a Boussi
          // Le But donner un sens a P=0 quand on a de la gravite
          rho_m=le_fluide.valeur().moyenne_vol(tab_rho);
        }
      for (num_cl=0 ; num_cl<la_zone->nb_front_Cl() ; num_cl++)
        {
          const Cond_lim& la_cl = la_zone_Cl->les_conditions_limites(num_cl);
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = le_bord.num_premiere_face();
          int nfin = ndeb + le_bord.nb_faces();
          if (sub_type(Neumann_sortie_libre,la_cl.valeur())||sub_type(Symetrie,la_cl.valeur())||sub_type(Periodique,la_cl.valeur()))
            {
              for (face=ndeb ; face<nfin ; face++)
                {
                  for (comp=0 ; comp<dimension ; comp++)
                    {
                      resu(face,comp) += (tab_rho(face)-rho_m)*volumes_entrelaces(face)*porosite_face(face)*g(comp);
                    }
                }
            }
        }

      for (face=premiere_face_interne ; face<nb_faces; face++)
        {
          for (comp=0 ; comp<dimension ; comp++)
            {
              resu(face,comp) += (tab_rho(face)-rho_m)*volumes_entrelaces(face)*porosite_face(face)*g(comp);
            }
        }
    }
  else
    {
      Cerr<<"La discretisation "<<mon_equation->discretisation().que_suis_je()<<" n'est pas reconnue dans Source_Gravite_Quasi_Compressible_VEF"<<finl;
      abort();
    }

  return resu;
}

