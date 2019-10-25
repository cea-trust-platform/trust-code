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
// File:        Domaine.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/69
//
//////////////////////////////////////////////////////////////////////////////

#include <Domaine.h>
#include <math.h>
#include <Sous_Zone.h>
#include <Zone_VF.h>
#include <Periodique.h>
#include <Zone_Cl_dis.h>
#include <Extraire_surface.h>
#include <Frontiere_dis_base.h>
#include <NettoieNoeuds.h>
#include <Reordonner_faces_periodiques.h>
#include <MD_Vector_tools.h>
#include <Scatter.h>
#include <Octree_Double.h>
#include <Interprete_bloc.h>
#include <Domaine_dis.h>
#ifdef MEDCOUPLING_
using MEDCoupling::DataArrayInt;
using MEDCoupling::DataArrayDouble;
#endif

Implemente_instanciable_sans_constructeur(Domaine,"Domaine",Objet_U);
Implemente_liste(Domaine);

// Description:
//    Ecrit le domaine sur un flot de sortie.
//    On ecrit le nom, les sommets, les zones
//    et les sous-zones.
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
Sortie& Domaine::printOn(Sortie& s ) const
{
  Cerr << "Writing of " << nb_som() << " nodes." << finl;
#ifdef SORT_POUR_DEBOG
  s.setf(ios::scientific);
  s.precision(20);
#endif
  s << nom_ << finl;
  s << sommets;
  s << les_zones;
  //s << les_ss_zones;
  return s ;
}


// Description:
//    Lit les specifications d'un domaine
//    a partir d'un flot d'entree. Associe les zones
//    lues au domaine.
//    Format:
//      nom_domaine
//      bloc de lecture des Sommets
//      bloc de lecture des Zones
//      bloc de lecture des Sous_Zones
//    Attention: en parallele, chaque processeur s'attend a lire une strucutre domaine
//     avec des infos de joint, mais il y a des communications globales.
//     Pour lire un domaine sur un seul processeur, il faut mettre le processeur dans
//     un groupe seul
Entree& Domaine::readOn(Entree& s)
{
#ifdef SORT_POUR_DEBOG
  s.setf(ios::scientific);
  s.precision(20);
#endif
  // Ajout BM: reset de la structure (a pour effet de debloquer la structure parallele)
  sommets.reset();
  renum_som_perio.reset();
  // ne pas faire reset du nom (deja lu)
  // pour deformable je ne sais pas...
  // Pour les sous_zones je ne sais pas non plus... les_ss_zones.vide();
  les_zones.vide();

  Nom tmp;
  s >> tmp;
  // Si le domaine n'est pas nomme, on prend celui lu
  if (nom_=="??") nom_=tmp;
  Cerr << "Reading domain " << le_nom() << finl;
  s >> sommets;
  // PL : pas tout a fait exact le nombre affiche de sommets, on compte plusieurs fois les sommets des joints...
  int nbsom = mp_sum(sommets.dimension(0));
  Cerr << " Number of nodes: " << nbsom << finl;
  s >> les_zones;
  //s >> les_ss_zones;
  if ( (Process::nproc()==1) && (NettoieNoeuds::NettoiePasNoeuds==0) )
    {
      NettoieNoeuds::nettoie(*this);
      nbsom = mp_sum(sommets.dimension(0));
      Cerr << " Number of nodes after node-cleanup: " << nbsom << finl;
    }
  les_zones.associer_domaine(*this);

  // On initialise les descripteurs "sequentiels" (attention, cela bloque le resize des tableaux sommets et elements !)
  Scatter::init_sequential_domain(*this);

  return s;
}


// Description:
//    Constructeur par defaut d'un objet Domaine.
//    Ne fait rien sauf fixer la valeur d'epsilon
//    a Objet_U::precision_geom
//    Si 2 points d'un domaine sont separes d'une distance
//    inferieure ou egale a epsilon ils sont consideres egaux.
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
Domaine::Domaine() : epsilon_(Objet_U::precision_geom),deformable_(0)
{
#ifdef MEDCOUPLING_
  mesh_ = NULL;
#endif
}

// Description:
// Renvoie le nombre total de sommets
int Domaine::nb_som_tot() const
{
  return sommets.dimension_tot(0);
}

// Description:
// Renvoie le nombre de sommets
int Domaine::nb_som() const
{
  return sommets.dimension(0);
}

// Description:
//    Ajoute une Zone au domaine.
// Precondition:
// Parametre: Zone& une_zone
//    Signification: la zone a ajouter au domaine
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree
// Retour: Zone&
//    Signification: la zone que l'on vient d'ajouter au domaine
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Zone& Domaine::add(Zone& une_zone)
{
  return les_zones.add(une_zone);
}

// Description:
//    Ajoute des noeuds (ou sommets) au domaine
//    (sans verifier les doublons)
// Precondition:
// Parametre: DoubleTab& soms
//    Signification: le tableau contenant les coordonnees
//                   des noeuds a ajouter au domaine
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Domaine::ajouter(const DoubleTab& soms)
{
  int oldsz=sommets.dimension(0);
  int ajoutsz=soms.dimension(0);
  int dim=soms.dimension(1);
  sommets.resize(oldsz+ajoutsz,dim);
  for(int i=0; i<ajoutsz; i++)
    for(int k=0; k<dim; k++)
      sommets(oldsz+i,k)=soms(i,k) ;
}

// Description:
//    Ajoute des noeuds au domaine avec elimination des noeuds double
//    au retour nums contient les nouveaux numeros des noeuds de soms
//    apres elimination des doublons.
// Precondition:
// Parametre: DoubleTab& soms
//    Signification: le tableau contenant les coordonnees
//                   des noeuds a ajouter au domaine
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Parametre: IntVect& nums
//    Signification: le tableau des nouveaux numeros apres
//                   ajout des nouveaux noeuds et elimination
//                   des doublons.
//    Valeurs par defaut:
//    Contraintes:
//    Acces: sortie
// Retour:
//    Signification:
//    Contraintes:
// Exception: des noeuds double ont ete trouve
// Effets de bord:
// Postcondition:
void Domaine::ajouter(const DoubleTab& soms, IntVect& nums)
{
  int oldsz=sommets.dimension(0);
  int ajoutsz=soms.dimension(0);
  int dim=soms.dimension(1);
  nums.resize(ajoutsz);
  nums=-1;
  if(oldsz!=0)
    {
      assert(dim==sommets.dimension(1));
      Octree_Double octree;
      octree.build_nodes(les_sommets(), 0 /* ne pas inclure les sommets virtuels */);

      int i, k;
      int compteur=0;
      ArrOfDouble tab_coord(dim);
      ArrOfInt liste_sommets;
      liste_sommets.set_smart_resize(1);
      for( i=0; i< ajoutsz; i++)
        {
          for (int j = 0; j < dim; j++)
            tab_coord[j] = soms(i,j);
          octree.search_elements_box(tab_coord, epsilon_, liste_sommets);
          octree.search_nodes_close_to(tab_coord, les_sommets(), liste_sommets, epsilon_);
          const int nb_sommets_proches = liste_sommets.size_array();
          if (nb_sommets_proches == 0)
            {
              // Aucun sommet du premier domaine n'est proche du sommet i.
              // Garder i.
            }
          else if (nb_sommets_proches == 1)
            {
              // Un sommet est confondu avec le sommet i a epsilon_ pres.
              // Ne pas garder le sommet
              nums(i) = liste_sommets[0];
              compteur++;
            }
          else
            {
              // Plusieurs sommets du domaine initial sont dans un rayon de epsilon.
              // epsilon est trop grand.
              Cerr << "Error : several nodes of the domain 1 are within radius epsilon="
                   << epsilon_ << " of point " << tab_coord << ". We must reduce epsilon. " << finl;
              Process::exit();
            }
        }
      Cerr << compteur << " double nodes were found \n";
      sommets.resize(oldsz+ajoutsz-compteur,dim);
      compteur=0;
      for( i=0; i<ajoutsz; i++)
        if(nums(i)==-1)
          {
            nums(i)=oldsz+compteur;
            compteur++;
            for( k=0; k<dim; k++)
              sommets(nums(i),k)=soms(i,k) ;
          }
    }
  else
    {
      sommets=soms;
      // if som has a descriptor, delete it:
      sommets.set_md_vector(MD_Vector());
      for(int i=0; i<ajoutsz; i++)
        {
          nums(i)=i;
        }
    }
}

// Description:
//    Rassemble les Zones de meme type
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
void Domaine::comprimer()
{
  les_zones.comprimer();
}

// Description:
//    Association d'une Sous_Zone au domaine.
//    L'interface permet de passer n'importe quel
//    Objet_U mais ne gere (dynamiquement) que
//    l'association d'un objet derivant Sous_Zone.
// Precondition:
// Parametre: Objet_U& ob
//    Signification: l'objet a associer
//    Valeurs par defaut:
//    Contraintes: doit deriver Sous_Zone
//    Acces:
// Retour: int
//    Signification: 1 si l'association a reussie
//                   0 sinon (l'objet n'etait pas derive Sous_Zone)
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
int Domaine::associer_(Objet_U& ob)
{
  if( sub_type(Sous_Zone, ob))
    {
      add(ref_cast(Sous_Zone, ob));
      ob.associer_(*this);
      return 1;
    }
  return 0;
}


// Description:
//    Ajoute une Sous_Zone au domaine.
// Precondition:
// Parametre: Sous_Zone& ssz
//    Signification: la Sous-zone a ajouter au domaine
//    Valeurs par defaut:
//    Contraintes: reference constante
//    Acces: entree
// Retour:
//    Signification:
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
void Domaine::add(const Sous_Zone& ssz)
{
  les_ss_zones.add(ssz);
}

// Description:
//    Reordonne les mailles du domaine suivant les
//    conventions de Trio-U.
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
// Postcondition: les mailles du domaine sont reordonnees
//                suivant les conventions de Trio-U
void Domaine::reordonner()
{
  les_zones.reordonner();
}

void Domaine::construire_renum_som_perio(const Conds_lim& les_cl,
                                         const Zone_dis& zone_dis)
{
  Noms bords_perio;
  const int nb_bords = les_cl.size();
  for (int n_bord = 0; n_bord < nb_bords; n_bord++)
    {
      if (sub_type(Periodique, les_cl[n_bord].valeur()))
        bords_perio.add(les_cl[n_bord].frontiere_dis().frontiere().le_nom());
    }

  Reordonner_faces_periodiques::renum_som_perio(*this, bords_perio, renum_som_perio,
                                                1 /* Calculer les valeurs pour les sommets virtuels */);
}

// Description: Cree un tableau ayant une "ligne" par sommet du maillage.
//  Voir MD_Vector_tools::creer_tableau_distribue()
void Domaine::creer_tableau_sommets(Array_base& v, Array_base::Resize_Options opt) const
{
  const MD_Vector& md = md_vector_sommets();
  MD_Vector_tools::creer_tableau_distribue(md, v, opt);
}

const MD_Vector& Domaine::md_vector_sommets() const
{
  const MD_Vector& md = les_sommets().get_md_vector();
  return md;
}

// Description: Cree un tableau ayant une "ligne" par element du maillage.
//  Pour l'instant, erreur s'il y a plus d'une zone dans le domaine.
//  Voir MD_Vector_tools::creer_tableau_distribue()
void Domaine::creer_tableau_elements(Array_base& v, Array_base::Resize_Options opt) const
{
  // On pourra faire evoluer le code ci-dessous en creant un
  // tableau associe a un MD_Vector_composite:
  if (nb_zones() != 1)
    {
      Cerr << "Error in Domaine::creer_tableau_elements(Array_base & v):\n"
           << " Domain does not have exactely 1 zone !" << finl;
      Process::exit();
    }
  zone(0).creer_tableau_elements(v, opt);
}



// Creation des domaines frontieres (appele lors de la discretisation)
// Actuellement une liste statique de Domaines ou l'on a besoin pour
// chaque domaine de connaitre le premier element
void Domaine::creer_mes_domaines_frontieres(const Zone_VF& zone_vf)
{
  const Nom expr_elements("1");
  const Nom expr_faces("1");
  int nb_frontieres = zone(0).nb_front_Cl();
  domaines_frontieres_.vide();
  for (int i=0; i<nb_frontieres; i++)
    {
      // Nom de la frontiere
      Noms nom_frontiere(1);
      nom_frontiere[0]=zone(0).frontiere(i).le_nom();
      // Nom du domaine surfacique que l'on va construire
      Nom nom_domaine_surfacique=le_nom();
      nom_domaine_surfacique+="_boundaries_";
      nom_domaine_surfacique+=zone(0).frontiere(i).le_nom();
      // Creation
      Cerr << "Creating a surface domain named " << nom_domaine_surfacique << " for the boundary " << nom_frontiere[0] << " of the domain " << le_nom() << finl;

      Interprete_bloc& interp = Interprete_bloc::interprete_courant();
      if (interp.objet_global_existant(nom_domaine_surfacique))
        {
          Cerr << "Domain " << nom_domaine_surfacique
               << " already exists, writing to this object." << finl;

          Domaine& dom_new = ref_cast(Domaine, interprete().objet(nom_domaine_surfacique));
          Scatter::uninit_sequential_domain(dom_new);
        }
      else
        {
          DerObjU ob;
          ob.typer("Domaine");
          interp.ajouter(nom_domaine_surfacique, ob);
        }
      Domaine& dom_new = ref_cast(Domaine, interprete().objet(nom_domaine_surfacique));

      Extraire_surface::extraire_surface(dom_new,*this,nom_domaine_surfacique,zone_vf,expr_elements,expr_faces,0,nom_frontiere);
      REF(Domaine)& ref_dom_new=domaines_frontieres_.add(REF(Domaine)());
      ref_dom_new=dom_new;
    }
}

void Domaine::imprimer() const
{
  Cerr << "==============================================" << finl;
  Cerr << "The extreme coordinates of the domain " << le_nom() << " are:" << finl;
  // Il n'existe pas de recherche du min et du max dans DoubleTab donc je code:
  int nbsom=coord_sommets().dimension(0);
  for (int j=0; j<dimension; j++)
    {
      double min_=DMAXFLOAT;
      double max_=-DMAXFLOAT;
      for (int i=0; i<nbsom; i++)
        {
          double c = coord_sommets()(i,j);
          min_ = (c < min_ ? c : min_);
          max_ = (c > max_ ? c : max_);
        }
      min_ = mp_min(min_);
      max_ = mp_max(max_);
      if (j==0) Cerr << "x ";
      if (j==1) Cerr << "y ";
      if (j==2) Cerr << "z ";
      Cerr << "is between " << min_ << " and " << max_ << finl;
    }
  Cerr << "==============================================" << finl;
  // We recompute volumes (cause stored in Zone_VF and so not available from Domaine...):
  DoubleVect volumes;
  DoubleVect inverse_volumes;
  zone(0).calculer_volumes(volumes,inverse_volumes);
  Cerr << "==============================================" << finl;
  Cerr << "The volume cells of the domain " << le_nom() << " are:" << finl;
  const int i_vmax = imax_array(volumes);
  const int i_vmin = imin_array(volumes);
  const double vmin_local = (i_vmin < 0) ? 1e40 : volumes[i_vmin];
  const double vmax_local = (i_vmax < 0) ? -1e40 : volumes[i_vmax];
  const double volmin = mp_min(vmin_local);
  const double volmax = mp_max(vmax_local);
  double volume_total = mp_somme_vect(volumes);
  const int nb_elem = zone(0).nb_elem();
  double volmoy = volume_total / Process::mp_sum(nb_elem);
  Cerr << "sum(volume cells)= "  << volume_total << finl;
  Cerr << "mean(volume cells)= " << volmoy << finl;
  Cerr << "min(volume cells)= "  << volmin << finl;
  Cerr << "max(volume cells)= "  << volmax << finl;
  if (volmin*1000<volmoy)
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "Warning, a cell volume is more than 1000 times smaller than the average cell volume. Check your mesh." << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
    }
  Cerr << "==============================================" << finl;
}

// Build the faces mesh:
void Domaine::buildUFacesMesh(const Zone_dis_base& zone_dis_base) const
{
#ifdef MEDCOUPLING_
  MCAuto<DataArrayInt> desc(DataArrayInt::New());
  MCAuto<DataArrayInt> descIndx(DataArrayInt::New());
  MCAuto<DataArrayInt> revDesc(DataArrayInt::New());
  MCAuto<DataArrayInt> revDescIndx(DataArrayInt::New());
  faces_mesh_ = mesh_->buildDescendingConnectivity(desc, descIndx, revDesc, revDescIndx);
  // Renumber faces to have the same numbering than Domaine_dis
  std::size_t size = faces_mesh_->getNumberOfCells();
  IntVect renum(size);
  // Compute Center of Mass
  MCAuto<DataArrayDouble> xv_med = faces_mesh_->computeCellCenterOfMass();
  // On boucle sur les elements des tableaux Zone_VF::elem_faces et desc
  // Ensuite on compare geometriquement les centres des faces de ces tableaux pour trouver les relations
  // Boucle sur les mailles
  const IntTab& elem_faces = ref_cast(Zone_VF, zone_dis_base).elem_faces();
  int nb_elem = elem_faces.dimension(0);
  int nb_face_elem = elem_faces.dimension(1);
  assert(nb_elem == (int)descIndx->getNbOfElems()-1);
  // Centre des faces TRUST:
  const DoubleTab& xv = ref_cast(Zone_VF, zone_dis_base).xv();
  //int nb_faces = xv.dimension_tot(0);
  int nb_comp = xv.dimension_tot(1);
  // xv1, xv2 tableaux temporaires des centres des faces de l'element elem pour comparaison
  MCAuto<DataArrayDouble> xv1(DataArrayDouble::New());
  xv1->alloc(nb_face_elem, nb_comp);
  MCAuto<DataArrayDouble> xv2(DataArrayDouble::New());
  xv2->alloc(nb_face_elem, nb_comp);

  // Boucle sur les elements
  MCAuto<DataArrayInt> renum_local(DataArrayInt::New());
  for (int elem=0; elem<nb_elem; elem++)
    {
      //Cerr << "elem=" << elem << finl;
      for (int i=0; i<nb_face_elem; i++)
        {
          // Face globale TRUST
          int face = elem_faces(elem, i);
          /*
          Cerr << "\tface=" << face;
          for (int j=0; j<nb_comp; j++) Cerr << " " << xv(face, j);
          Cerr << finl; */
          // Face globale MED
          int index = descIndx->getIJ(elem, 0);
          int face_med = desc->getIJ(index + i, 0);
          /*
          Cerr << "\tface_med=" << face_med;
          for (int j=0; j<nb_comp; j++) Cerr << " " << xv_med->getIJ(face_med, j);
          Cerr << finl; */
          // Centre des faces TRUST et MED
          for (int j=0; j<nb_comp; j++)
            {
              xv1->setIJ(i, j, xv(face, j));
              xv2->setIJ(i, j, xv_med->getIJ(face_med, j));
            }
        }
      renum_local = xv2->findClosestTupleId(xv1);
      for (std::size_t i=0; i<renum_local->getNumberOfTuples(); i++)
        {
          int i_med = renum_local->getIJ(i, 0);
          int face_med = desc->getIJ(descIndx->getIJ(elem, 0) + i_med, 0);
          int face = elem_faces(elem, i);
          /*
          Cerr << "Local: " << i_med << " -> " << (int)i << finl;
          Cerr << "Global:" << face_med << " -> " << face << finl;
           */
          renum(face_med) = face;
        }
    }
#ifdef NDEBUG
  bool check = false;
#else
  bool check = true;
#endif
  // Apply the renumbering:
  faces_mesh_->renumberCells(renum.addr(), check);
#ifndef NDEBUG
  faces_mesh_->checkConsistency();
#endif
#endif
}
