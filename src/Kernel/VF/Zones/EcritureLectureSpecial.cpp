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
// File:        EcritureLectureSpecial.cpp
// Directory:   $TRUST_ROOT/src/Kernel/VF/Zones
// Version:     /main/38
//
//////////////////////////////////////////////////////////////////////////////

#include <EcritureLectureSpecial.h>
#include <Champ_Inc_base.h>
#include <Champ_Fonc_base.h>
#include <Zone_VF.h>
#include <Domaine.h>
#include <Scatter.h>
#include <MD_Vector_tools.h>
#include <Octree_Double.h>
#include <MD_Vector_composite.h>
#include <ConstDoubleTab_parts.h>

int EcritureLectureSpecial::mode_ecr=-1;
int EcritureLectureSpecial::mode_lec=0;
int EcritureLectureSpecial::Active=1;

#ifdef MPI_
Nom EcritureLectureSpecial::Input="LecFicDiffuseBin";    // "EFichierBin" was the past (<=1.6.9) and "LecFicPartageMPIIO" is may be the future
Nom EcritureLectureSpecial::Output="EcrFicPartageMPIIO"; // "EcrFicPartageBin" was the past (<=1.6.9) and "EcrFicPartageMPIIO" is after 1.7.0
#else
Nom EcritureLectureSpecial::Input="EFichierBin";
Nom EcritureLectureSpecial::Output="EcrFicPartageBin";
#endif

Implemente_instanciable(EcritureLectureSpecial,"EcritureLectureSpecial",Interprete);

const DoubleTab& get_ref_coordinates_items(const Zone_VF& zvf, const MD_Vector& md)
{
  // j'aurais pris xv mais il n'a pas de structure parallele !!!
  if (md == zvf.face_sommets().get_md_vector())
    return zvf.xv(); // Descripteur des face
  // j'aurais pris xp mais il n'a pas de structure parallele !!!!!!$&
  else if (md == zvf.zone().les_elems().get_md_vector())
    return zvf.xp(); // Descripteur des elements
  else if (md == zvf.xa().get_md_vector())
    return zvf.xa(); // Descripteur des aretes
  else if (md == zvf.zone().domaine().les_sommets().get_md_vector())
    return zvf.zone().domaine().les_sommets();
  else
    {
      Cerr << "Error in get_ref_coordinates_items\n"
           << " descriptor not found in this zone" << finl;
      Process::exit();
    }
  return zvf.xv(); // for compiler
}
Sortie& EcritureLectureSpecial::printOn(Sortie& os) const
{
  return os;
}

Entree& EcritureLectureSpecial::readOn(Entree& is)
{
  return is;
}

Entree& EcritureLectureSpecial::interpreter(Entree& is)
{
  Nom option;
  is >> option;
  if (option=="EFichierBin" || option=="LecFicDiffuseBin" || option=="LecFicPartageMPIIO")
    {
      Input=option;
      Cerr << "EcritureLectureSpecial::Input set to " << option << finl;
    }
  else if (option=="EcrFicPartageMPIIO" || option=="EcrFicPartageBin")
    {
      Active=1;
      Output=option;
      Cerr << "EcritureLectureSpecial::Output set to " << option << finl;
    }
  else if (option=="0")
    Active=0;
  else if (option=="1")
    Active=1;
  else
    {
      Cerr << "\"EcritureLectureSpecial " << option << "\" is unknown." << finl;
      Process::exit();
    }
  Cerr << "EcritureLectureSpecial::Active set to " << Active << finl;
  return is;
}

// Description:
//     indique si le format special a ete demande en lecture
//     active par reprise xyz ....
int EcritureLectureSpecial::is_lecture_special()
{
  return mode_lec;
}

// Description:
//     indique si le format special a ete demande en lecture
//     active par sauvegarde xyz ....
//  si le mode ecriture est special c.a.d si format de sauvegarde xyz
//    alors special =1 a_faire=je_suis_maitre
//    sinon special=0 , a_faire=1
//
int EcritureLectureSpecial::is_ecriture_special(int& special,int& a_faire)
{
  special=0;
  a_faire=1;
  assert(mode_ecr>=0); // mode_ecr n'est pas positionne
  if (mode_ecr)
    {
      special=1;
      a_faire=Process::je_suis_maitre();
    }
  return mode_ecr;
}

// Description:
//    simple appel a EcritureLectureSpecial::ecriture_special
//          (const Zone_VF& zvf,Sortie& fich,int nbval,const DoubleTab& val)
//    apres avoir recupere le tableau val
// Precondition:
// Parametre:
// Retour:
// Exception:
// Effets de bord:
// Postcondition:
void EcritureLectureSpecial::ecriture_special(const Champ_base& ch, Sortie& fich)
{
  const Zone_VF& zvf = ref_cast(Zone_VF, ch.zone_dis_base());
  const DoubleTab& val = ch.valeurs();
  ecriture_special(zvf, fich, val);
}

void ecrit(Sortie& fich, const ArrOfBit& items_to_write, const DoubleTab& pos, const DoubleTab& val)
{
  const int nb_dim = val.nb_dim();
  const int nb_comp = (nb_dim == 2) ? val.dimension(1) : 1;
  const int nb_val = items_to_write.size_array();
  const int dim = pos.dimension(1);

  if (EcritureLectureSpecial::get_Output().finit_par("MPIIO"))
    {
      // No bufferisation needed for Parallel IO
      int jmax = (dim + nb_comp) * nb_val;
      ArrOfDouble tmp(jmax);
      int j=0;
      for (int p = 0; p < nb_val; p++)
        {
          // On n'ecrit que les items reels non communs afin d'avoir un fichier .xyz
          // de meme taille quelque soit le decoupage et surtout de pouvoir le relire
          // quelque soit le decoupage et les supports
          if (items_to_write[p])
            {
              for (int k = 0; k < dim; k++)
                tmp[j++] = pos(p, k);
              if (nb_dim == 1)
                tmp[j++] = val(p);
              else
                for (int k = 0; k < nb_comp; k++)
                  tmp[j++] = val(p, k);
            }
        }
      fich.put(tmp.addr(), j, dim + nb_comp /* nb colonnes en ascii */);
    }
  else
    {
      // Bufferisation needed for EcrFicPartage
      int jmax = (dim + nb_comp) * 128;
      ArrOfDouble tmp(jmax);
      int j = 0;
      for (int p = 0; p < nb_val; p++)
        {
          // On n'ecrit que les items reels non communs afin d'avoir un fichier .xyz
          // de meme taille quelque soit le decoupage et surtout de pouvoir le relire
          // quelque soit le decoupage et les supports
          if (items_to_write[p])
            {
              for (int k = 0; k < dim; k++)
                tmp[j++] = pos(p, k);
              if (nb_dim == 1)
                tmp[j++] = val(p);
              else
                for (int k = 0; k < nb_comp; k++)
                  tmp[j++] = val(p, k);
              if (j == jmax)
                {
                  fich.put(tmp.addr(), j, dim + nb_comp /* nb colonnes en ascii */);
                  // On flushe regulierement en sequentiel car sur certains tres gros maillages
                  // stack overflow possible...
                  if (Process::nproc()==1) fich.syncfile();
                  j = 0;
                }
            }
        }
      if (j)
        fich.put(tmp.addr(), j, dim + nb_comp /* nb colonnes en ascii */);
    }
}

// Description:
//  Partie "interieure" de l'ecriture, appellee par la methode en dessous.
//  Methode recursive, si le tableau a ecrire a un descripteur MD_Vector_composite
static void ecriture_special_part2(const Zone_VF& zvf, Sortie& fich, const DoubleTab& val)
{
  const MD_Vector& md = val.get_md_vector();

  if (sub_type(MD_Vector_composite, md.valeur()))
    {
      // Champs p1bulles et autres: appel recursif pour les differents sous-tableaux:
      ConstDoubleTab_parts parts(val);
      const int n = parts.size();
      for (int i = 0; i < n; i++)
        ecriture_special_part2(zvf, fich, parts[i]);
    }
  else if (sub_type(MD_Vector_std, md.valeur()))
    {
      ArrOfBit items_to_write;
      MD_Vector_tools::get_sequential_items_flags(md, items_to_write);
      const DoubleTab& coords = get_ref_coordinates_items(zvf, md);
      ecrit(fich, items_to_write, coords, val);
      fich.syncfile();
    }
  else
    {
      Cerr << "EcritureLectureSpecial::ecriture_special_part: Error, unknown Metadata vector type : "
           << md.valeur().que_suis_je() << finl;
      Process::exit();
    }
}

// Description:
//    codage de l'ecriture des positions et des valeurs de val
// Precondition:
// Parametre:
// Retour:
// Exception:
// Effets de bord:
// Postcondition:
void EcritureLectureSpecial::ecriture_special(const Zone_VF& zvf, Sortie& fich, const DoubleTab& val)
{
  const MD_Vector& md = val.get_md_vector();
  if (!md.non_nul())
    {
      Cerr << "EcritureLectureSpecial::ecriture_special: error, cannot save an array with no metadata" << finl;
      Process::exit();
    }
  const int nb_items_seq = md.valeur().nb_items_seq_tot();
  if (nb_items_seq == 0)
    return;

  const int nb_dim = val.nb_dim();
  const int nb_comp = (nb_dim == 2) ? val.dimension(1) : 1;
  const int dim = Objet_U::dimension;
  const int n = nb_items_seq * (nb_comp + dim);


  if (Process::je_suis_maitre())
    fich << 1 << finl << n << finl << 1 << finl << n << finl << n <<finl;

  ecriture_special_part2(zvf, fich, val);

  if (Process::je_suis_maitre())
    fich << n << finl << 0 << finl << 0 << finl << 0 << finl << 1 << finl << 0 << finl << n << finl << finl << 1 << finl << 0 << finl << 0 <<finl;

  fich.syncfile();

}

// Description:
//    simple appel a EcritureLectureSpecial::lecture_special
//          (const Zone_VF& zvf,Entree& fich,int nbval, DoubleTab& val )
// Precondition:
// Parametre:
// Retour:
// Exception:
// Effets de bord:
// Postcondition:
void EcritureLectureSpecial::lecture_special(Champ_base& ch, Entree& fich)
{
  const Zone_VF& zvf=ref_cast(Zone_VF,ch.zone_dis_base());
  DoubleTab& val = ch.valeurs();
  lecture_special(zvf, fich, val);
}


// Description: Reciproque de la methode ecrit(...), lit uniquement les items sequentiels
//   (donc pas les items communs recus d'un autre processeur)
//   On verifie a la fin qu'on a bien lu exactement le nombre d'items attendus, s'il en manque
//   c'est que le epsilon n'est pas bon (ou qu'on a change le maillage...)
// Valeur de retour: nombre total d'items sequentiels lus (sur tous les procs)
static int lire_special(Entree& fich, const DoubleTab& coords, DoubleTab& val, const double epsilon)
{
  const int dim = coords.dimension(1);
  const int nb_dim = val.nb_dim();
  const int nb_comp = (nb_dim == 1) ? 1 : val.dimension(1);

  const MD_Vector& md_vect = val.get_md_vector();
  // Dans un premier temps, 1 si l'item est a lire, 0 s'il est lu par un autre processeur.
  // Une fois que l'item est lu, on met le flag a 2.
  ArrOfInt items_to_read;
  const int n_to_read = MD_Vector_tools::get_sequential_items_flags(md_vect, items_to_read);
  Octree_Double octree;
  // Build an octree with "thick" nodes (epsilon size)
  octree.build_nodes(coords, 0 /* do not include virtual elements */, epsilon);
  const ArrOfInt& floor_elements = octree.floor_elements();

  // Le fichier contient ce nombre de lignes pour cette partie du tableau (nombre total d'items sequentiels)
  const int ntot = Process::mp_sum(n_to_read);

  // On lit dans le fichier par blocs de buflines_max parce qu'il y a
  //  un broadcast reseau a chaque comm:
  const int buflines_max = 2048; // pas trop, histoire d'avoir plusieurs blocs dans les cas tests
  DoubleTab buffer(buflines_max, dim + nb_comp);
  int bufptr = buflines_max;
  ArrOfInt items;
  items.set_smart_resize(1);

  double max_epsilon_needed = epsilon;
  // Combien de fois on a trouve plusieurs candidats a moins de epsilon ?
  int error_too_many_matches = 0;
  // Combien de fois on est tombe plusieurs fois sur le meme sommet a lire ?
  int error_duplicate_read = 0;
  // Combien d'items a-t-on lu ?
  int count_items_read = 0;

  // Boucle sur les items sequentiels du fichier:
  int pourcent=0;
  for (int i = 0; i < ntot; i++)
    {
      int tmp=(i*10)/(ntot-1);
      if (tmp>pourcent || i==0)
        {
          pourcent=tmp;
          Cerr<<"\r"<<pourcent*10<<"% of data has been found."<<flush;
        }
      if (bufptr == buflines_max)
        {
          bufptr = 0;
          int n = min(buflines_max, ntot - i) * (dim + nb_comp);
          assert(n <= buffer.size_array());
          fich.get(buffer.addr(), n);
        }
      const double x = buffer(bufptr, 0);
      const double y = buffer(bufptr, 1);
      const double z = (dim == 3) ? buffer(bufptr, 2) : 0.;
      // Recherche des items correspondant potentiellement au point (x,y,z)
      int index = -1;
      int nb_items_proches = octree.search_elements(x, y, z, index);
      if (nb_items_proches > 0)
        {
          items.resize_array(nb_items_proches, ArrOfInt::NOCOPY_NOINIT);
          // Voir doc de Octree_Double::search_elements: on copie les indices des items proches dans items:
          for (int j = 0; j < nb_items_proches; j++)
            items[j] = floor_elements[index++];
          // On reduit la liste pour avoir uniquement les items a moins de epsilon
          const int item_le_plus_proche = octree.search_nodes_close_to(x, y, z, coords, items, epsilon);
          nb_items_proches = items.size_array();
          if (nb_items_proches == 1)
            {
              const int flag = items_to_read[item_le_plus_proche];
              if (flag == 1)
                {
                  // Ok, il faut lire cette valeur
                  items_to_read[item_le_plus_proche] = 2;
                  count_items_read++;
                  if (nb_dim == 1)
                    {
                      val(item_le_plus_proche) = buffer(bufptr, dim);
                    }
                  else
                    {
                      for (int j = 0; j < nb_comp; j++)
                        val(item_le_plus_proche, j) = buffer(bufptr, dim + j);
                    }
                }
              else if (flag == 0)
                {
                  // Cet item n'est pas a moi, ne pas le lire
                }
              else
                {
                  // Erreur, on a deja lu cet item !!! epsilon est trop grand (ou erreur a la sauvegarde ???)
                  error_duplicate_read++;
                }
            }
          else if (nb_items_proches == 0)
            {
              // ok, le sommet est sur un autre processeur (ou epsilon trop petit ??)
            }
          else
            {
              // Erreur: epsilon est trop grand, on a plusieurs candidats a moins de epsilon
              // Calcul de la distance avec le deuxieme plus proche pour afficher un message d'erreur a la fin:
              for (int ii = 0; ii < nb_items_proches; ii++)
                {
                  const int i_coord = items[ii];
                  if (i_coord == item_le_plus_proche)
                    continue; // celui-la est sans doute le bon, il faut un epsilon superieur a cette valeur la...
                  double xx = 0;
                  for (int j = 0; j < dim; j++)
                    {
                      double yy = coords(i_coord, j) - buffer(bufptr, j);
                      xx += yy * yy;
                    }
                  // On propose de mettre un epsilon au maximum egal a 1/10 de la distance avec le deuxieme point le plus proche:
                  xx = sqrt(xx) * 0.1;
                  if (max_epsilon_needed > xx)
                    max_epsilon_needed = xx;
                }
              error_too_many_matches++;
            }
        }
      bufptr++;
    }
  Cerr << finl;
  // Erreurs ?
  int err = (count_items_read != n_to_read) || (error_too_many_matches > 0) || (error_duplicate_read > 0);
  err = Process::mp_sum(err);
  if (err)
    {
      error_too_many_matches = Process::mp_sum(error_too_many_matches);
      error_duplicate_read = Process::mp_sum(error_duplicate_read);
      max_epsilon_needed = Process::mp_min(max_epsilon_needed);
      if (Process::je_suis_maitre())
        {
          if (error_too_many_matches)
            {
              Cerr << "Error in EcritureLectureSpecial: error_too_many_matches = " << error_too_many_matches
                   << ", epsilon is too large. Suggested value: " << max_epsilon_needed << finl;
              if (max_epsilon_needed==0)
                {
                  Cerr << "It could be because your mesh has two boundaries which match exactly." << finl;
                  Cerr << "It is possible to do calculation with this property but xyz restart process" << finl;
                  Cerr << "is impossible because it can't detect the differences between faces of the two boundaries..." << finl;
                  Cerr << "Try to do a classic restart with .sauv files." << finl;
                }
            }
          else if (error_duplicate_read)
            {
              Cerr << "Error in EcritureLectureSpecial: error_duplicate_read = " << error_duplicate_read
                   << ", probably epsilon too large. " << finl;
            }
          else
            {
              Cerr << "Error in EcritureLectureSpecial: Some items were not found: epsilon too small (or changed the mesh ?)" << finl;
            }
        }
      Process::barrier();
      Process::exit();
    }
  return ntot;
}

// Valeur de retour: nombre total d'items sequentiels lus (sur tous les procs)
static int lecture_special_part2(const Zone_VF& zvf, Entree& fich, DoubleTab& val)
{
  const MD_Vector& md = val.get_md_vector();

  int ntot = 0;
  if (sub_type(MD_Vector_composite, md.valeur()))
    {
      // Champs p1bulles et autres: appel recursif pour les differents sous-tableaux:
      DoubleTab_parts parts(val);
      const int n = parts.size();
      for (int i = 0; i < n; i++)
        ntot += lecture_special_part2(zvf, fich, parts[i]);
    }
  else if (sub_type(MD_Vector_std, md.valeur()))
    {
      const DoubleTab& coords = get_ref_coordinates_items(zvf, md);
      const double epsilon = zvf.zone().domaine().epsilon();
      ntot += lire_special(fich, coords, val, epsilon);
    }
  else
    {
      Cerr << "EcritureLectureSpecial::lecture_special_part2: Error, unknown Metadata vector type : "
           << md.valeur().que_suis_je() << finl;
      Process::exit();
    }
  return ntot;
}

// Description:
//    codage de la relecture d'un champ a partir d'un fichier special
//          positions,valeurs
// Precondition:
// Parametre:
// Retour:
// Exception:
// Effets de bord:
// Postcondition:
void EcritureLectureSpecial::lecture_special(const Zone_VF& zvf, Entree& fich, DoubleTab& val)
{

  const MD_Vector& md_vect = val.get_md_vector();
  if (!md_vect.non_nul())
    {
      Cerr << "EcritureLectureSpecial::ecriture_special: error, cannot save an array with no metadata" << finl;
      Process::exit();
    }
  const int nb_items_seq = md_vect.valeur().nb_items_seq_tot();
  if (nb_items_seq == 0)
    return;

  int bidon;
  fich >> bidon >> bidon >> bidon >> bidon >> bidon;

  int ntot = lecture_special_part2(zvf, fich, val);
  if (ntot != nb_items_seq)
    {
      Cerr << "Internal error in EcritureLectureSpecial::lecture_special" << finl;
      exit();
    }

  fich >> bidon >> bidon >> bidon >> bidon >> bidon >> bidon >> bidon >> bidon >> bidon >> bidon;

  // On met a jour les parties virtuelles du tableau val
  val.echange_espace_virtuel();
}

// Description:
//  Renvoie le mode d'ecriture utilise
//  (pour pouvoir le modifier).
//  Cette methode est statique.
Nom& EcritureLectureSpecial::get_Output()
{
  static Nom option=Output;

  // disable MPIIO in sequential mode
  if (Output=="EcrFicPartageMPIIO" && Process::nproc()==1) option="EcrFicPartageBin";

  // disable MPIIO if TRUST_DISABLE_MPIIO=1
  char* theValue = getenv("TRUST_DISABLE_MPIIO");
  if (theValue != NULL)
    {
      if (option=="EcrFicPartageMPIIO" && strcmp(theValue,"1")==0) option="EcrFicPartageBin";
    }

  return option;
}
