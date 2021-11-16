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
// File:        Periodique.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/44
//
//////////////////////////////////////////////////////////////////////////////

#include <Periodique.h>
#include <Domaine.h>
#include <Zone_Cl_dis.h>
#include <Zone_VF.h>
#include <Reordonner_faces_periodiques.h>
#include <Scatter.h>

Implemente_instanciable(Periodique,"Periodique",Cond_lim_base);


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
Sortie& Periodique::printOn(Sortie& s ) const
{
  return s << que_suis_je() << finl ;
}



// Description:
Entree& Periodique::readOn(Entree& s)
{
  le_champ_front.typer("Champ_front_uniforme");
  return s;
}

// Description:
//    Renvoie toujours 1
int Periodique::compatible_avec_eqn(const Equation_base&) const
{
  return 1;
}
// Description:
void Periodique::mettre_a_jour(double temps)
{
}

void Periodique::completer()
{
  Frontiere& frontiere = frontiere_dis().frontiere();
  Cerr << "Initialization for periodic on " << frontiere.le_nom() << finl;

  // Recherche de la direction de periodicite:
  ArrOfDouble erreur;
  int ok = Reordonner_faces_periodiques::check_faces_periodiques(frontiere,
                                                                 direction_perio_,
                                                                 erreur,
                                                                 1 /* verbose */);
  if (!ok)
    exit();

  distance_ = norme_array(direction_perio_);
  int i;
  const int dim = direction_perio_.size_array();
  direction_xyz_ = -2;
  for (i = 0; i < dim; i++)
    {
      if (fabs(direction_perio_[i]) > precision_geom)
        {
          if (direction_xyz_ == -2)
            direction_xyz_ = i;
          else
            // Deuxieme coordonnee non nulle, vecteur direction n'est pas aligne sur un axe
            direction_xyz_ = -1;
        }
    }
  if (direction_xyz_ < 0)
    Cerr << "Periodic direction not aligned on an axis" << finl;
  else
    Cerr << "Periodic direction aligned on the axis " << direction_xyz_ << finl;

  const Zone& zone = frontiere.zone();

  // Creation d'un tableau d'indices parallele sur toutes les faces frontieres
  IntTab tab_face_associee;
  const Zone_VF& zonevf = ref_cast(Zone_VF, zone_Cl_dis().zone_dis().valeur());
  zonevf.creer_tableau_faces_bord(tab_face_associee, Array_base::NOCOPY_NOINIT);
  tab_face_associee = -1;

  // Nombre de faces virtuelles de cette frontiere:
  const int nb_faces = frontiere.nb_faces();
  const int nb_faces_2_ = nb_faces / 2;
  const ArrOfInt& faces_virt = frontiere.get_faces_virt();
  const int nb_faces_virt = faces_virt.size_array();

  // On remplit la partie reele du tableau face_associee pour la frontiere qui nous interesse:
  // et la partie reele du tableau "associee"
  const int i_premiere_face = frontiere.num_premiere_face();
  for (i = 0; i < nb_faces_2_; i++)
    {
      const int i1 = i_premiere_face + i;
      const int i2 = i_premiere_face + i + nb_faces_2_;
      tab_face_associee[i1] = i2;
      tab_face_associee[i2] = i1;
    }
  const MD_Vector& md_faces_front = tab_face_associee.get_md_vector();
  // On echange espace virtuel avec traduction des indices:
  Scatter::construire_espace_virtuel_traduction(md_faces_front,
                                                md_faces_front,
                                                tab_face_associee,
                                                1 /* erreurs fatales */);
  // Tableau qui donne pour chaque face virtuelle de la zone, -1 si ce n'est pas une
  // face frontiere, sinon son indice dans les les frontieres.
  const ArrOfInt& ind_faces_virt_bord = zone.ind_faces_virt_bord();
  // Creation d'un tableau qui donne, pour chaque face virtuelle des frontieres (toutes frontieres)
  // l'indice de la face dans la frontiere periodique courante (-1 sinon)
  const int nb_faces_front_tot = tab_face_associee.size_totale();
  ArrOfInt index(nb_faces_front_tot);
  index= -2;
  const int nb_faces_zone = zone_Cl_dis().zone_dis().valeur().face_sommets().dimension(0);
  for (i = 0; i < nb_faces_virt; i++)
    {
      const int face_zone = frontiere.face_virt(i); // Indice d'une face de la zone
      const int face_front = ind_faces_virt_bord[face_zone - nb_faces_zone]; // Indice dans les frontieres
      index[face_front] = nb_faces + i;
    }

  // Remplissage du tableau "face_front_associee_"
  face_front_associee_.resize_array(nb_faces + nb_faces_virt);
  for (i = 0; i < nb_faces + nb_faces_virt; i++)
    {
      int resu = -1;
      if (i < nb_faces_2_)
        resu = i + nb_faces_2_;
      else if (i < nb_faces)
        resu = i - nb_faces_2_;
      else
        {
          // Face virtuelle :
          const int face_zone = frontiere.face_virt(i - nb_faces); // Indice de la face de la zone
          const int face_front = ind_faces_virt_bord[face_zone - nb_faces_zone]; // Indice dans les frontieres
          // Indice de la face associee dans le tableau face_associee:
          const int face_front_associee = tab_face_associee[face_front];
          if (face_front_associee >= 0)
            {
              // Indice de la face associee dans la frontiere:
              const int face_asso = index[face_front_associee];
              assert(face_asso >= 0);
              resu = face_asso;
            }
          else
            {
              // La face virtuelle associee n'est pas dans le domaine
              Cerr << "Error in Periodique::completer()" << finl;
              exit();
            }
        }
      face_front_associee_[i] = resu;
    }
}

double Periodique::distance() const
{
  return distance_;
}

int Periodique::direction_periodicite() const
{
  if (!est_periodique_selon_un_axe())
    {
      Cerr << "Error in Periodique::direction_periodicite():\n"
           << " An algorithm seems to assume that the periodic direction is aligned in X, Y or Z\n"
           << " and this is not the case !" << finl;
      exit();
    }
  return direction_xyz_;
}
