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
// File:        MaillerParallel.cpp
// Directory:   $TRUST_ROOT/src/Kernel/Geometrie
// Version:     /main/6
//
//////////////////////////////////////////////////////////////////////////////
#include <MaillerParallel.h>
#include <Domaine.h>
#include <Motcle.h>
#include <Scatter.h>
#include <Statistiques.h>
#include <Param.h>
#include <TRUSTArrays.h>
#include <Parser_U.h>
#include <Reordonner_faces_periodiques.h>
#include <EFichier.h>
#include <Schema_Comm.h>
#include <Octree_Double.h>
#include <Faces_builder.h>
#include <Connectivite_som_elem.h>
#include <Static_Int_Lists.h>
#include <communications.h>
#include <ArrOfBit.h>
#include <Array_tools.h>
#include <Ref_Joint.h>

Implemente_instanciable(MaillerParallel, "MaillerParallel", Interprete);

Entree& MaillerParallel::readOn(Entree& is)
{
  return is;
}

Sortie& MaillerParallel::printOn(Sortie& os) const
{
  return os;
}

struct BlocData
{
  void add_bloc(Domaine& domaine, const VECT(ArrOfDouble) & coord_ijk) const;
  // xmin_tot = 0 implicitement
  // xmax_tot = nombre total de sommets dans le maillage
  // xmax = indice du dernier sommet + 1
  // xmin = indice du premier sommet (donc xmax - xmin = nombre de sommets du bloc courant)
  ArrOfInt xmax_tot_;
  ArrOfInt xmin_, xmax_;
  Noms bord_xmin_, bord_xmax_;
  inline int nb_som(int i)  const
  {
    return xmax_[i] - xmin_[i];
  }
  inline int nb_elem(int i)  const
  {
    return xmax_[i] - xmin_[i] - 1;
  }
  void add_faces(Zone& zone, const Nom& nombord, int offset_sommets, int dir, int cote_max) const;
};

void BlocData::add_bloc(Domaine& domaine, const VECT(ArrOfDouble) & coord_ijk) const
{
  if (nb_elem(0) < 1 || nb_elem(1) < 1 || nb_elem(2) < 1)
    {
      Cerr << "Error : not enough nodes in a block, increase mesh size or decrease processor count" << finl;
      Process::exit();
    }
  const int dim = 3;
  // Les sommets
  const int old_nb_som = domaine.les_sommets().dimension(0);
  {
    DoubleTab& sommets = domaine.les_sommets();
    const int nsom = nb_som(0) * nb_som(1) * nb_som(2);
    sommets.resize(old_nb_som + nsom, dim);

    int index = old_nb_som;
    int i, j, k;
    for (k = xmin_[2]; k < xmax_[2]; k++)
      {
        for (j = xmin_[1]; j < xmax_[1]; j++)
          {
            for (i = xmin_[0]; i < xmax_[0]; i++)
              {
                sommets(index, 0) = coord_ijk[0][i];
                sommets(index, 1) = coord_ijk[1][j];
                if (dim==3)
                  sommets(index, 2) = coord_ijk[2][k];
                index++;
              }
          }
      }
  }
  // Les elements
  {
    assert(dim==3);
    Zone& zone = domaine.zone(0);
    IntTab& elements = zone.les_elems();
    const int nb_som_par_element = 8;

    // stride des indices de sommets:
    const int dx = 1;
    const int dy = nb_som(0);
    const int dz = dy * nb_som(1);
    // nombre d'elements dans chaque direction
    const int nx = nb_elem(0);
    const int ny = nb_elem(1);
    const int nz = nb_elem(2);
    const int nb_elem_old = zone.les_elems().dimension(0);
    const int nbelem = nx * ny * nz;
    elements.resize(nb_elem_old + nbelem, nb_som_par_element);

    int index = nb_elem_old;
    int i, j, k;
    for (k = 0; k < nz; k++)
      {
        for (j = 0; j < ny; j++)
          {
            for (i = 0; i < nx; i++)
              {
                int n = old_nb_som + k * dz + j * dy + i * dx;
                elements(index, 0) = n;
                elements(index, 1) = n + dx;
                elements(index, 2) = n + dy;
                elements(index, 3) = n + dx + dy;
                elements(index, 4) = n + dz;
                elements(index, 5) = n + dz + dx;
                elements(index, 6) = n + dz + dy;
                elements(index, 7) = n + dz + dx + dy;
                index++;
              }
          }
      }
  }
  // Les bords
  Zone& zone = domaine.zone(0);
  for (int dir = 0; dir < 3; dir++)
    {
      if (xmin_[dir] == 0)
        add_faces(zone, bord_xmin_[dir], old_nb_som, dir, 0);
      if (xmax_[dir] == xmax_tot_[dir])
        add_faces(zone, bord_xmax_[dir], old_nb_som, dir, 1);
    }
}

void BlocData::add_faces(Zone& zone, const Nom& nombord, int offset_sommets, int dir, int cote_max) const
{
  int nfaces1, nfaces2;
  int increment1, increment2;
  int increment_dir;
  switch(dir)
    {
    case 0:
      nfaces1 = nb_elem(1);
      nfaces2 = nb_elem(2);
      increment1 = nb_som(0);
      increment2 = nb_som(0) * nb_som(1);
      increment_dir = 1;
      break;
    case 1:
      nfaces1 = nb_elem(0);
      nfaces2 = nb_elem(2);
      increment1 = 1;
      increment2 = nb_som(0) * nb_som(1);
      increment_dir = nb_som(0);
      break;
    default:
      nfaces1 = nb_elem(0);
      nfaces2 = nb_elem(1);
      increment1 = 1;
      increment2 = nb_som(0);
      increment_dir = nb_som(0)*nb_som(1);
      break;
    }
  if (cote_max)
    offset_sommets += increment_dir * (nb_som(dir)-1);

  IntTab& faces = zone.frontiere(nombord).faces().les_sommets();
  int count = faces.dimension(0);
  zone.frontiere(nombord).faces().dimensionner(count + nfaces1 * nfaces2);
  for (int i = 0; i < nfaces2; i++)
    {
      int sommet = offset_sommets;
      for (int j = 0; j < nfaces1; j++)
        {
          faces(count, 0) = sommet;
          faces(count, 1) = sommet + increment1;
          faces(count, 2) = sommet + increment2;
          faces(count, 3) = sommet + increment1 + increment2;
          sommet += increment1;
          count++;
        }
      offset_sommets += increment2;
    }
}

static int bbox_intersection(const DoubleTab& bboxes, int i, int j, double epsilon)
{
  const int dim = bboxes.dimension(1) / 2;
  for (int k = 0; k < dim; k++)
    {
      double min1 = bboxes(i, k) - epsilon;
      double max1 = bboxes(i, dim+k) + epsilon;
      double min2 = bboxes(j, k);
      double max2 = bboxes(j, dim+k);
      if (min1 > max2 || max1 < min2)
        return 0;
    }
  return 1;
}

// Description: for a given set of local coordinates, finds which other processors
//  share the same nodes.
//  match must be sized to Process::nproc() and will be filled with, for each processor,
//  the indexes of coordinates that the corresponding processor shares with me.
//  Nodes are listed in the same order on couples of processors sharing a list of nodes
//  match[Process::me()] will be left empty.
void find_matching_coordinates(const DoubleTab& coords,
                               VECT(ArrOfInt) & match,
                               double epsilon)
{
  int i, j;
  const int dim = coords.dimension(1);
  const int ncoord = coords.dimension(0);
  const int nproc = Process::nproc();
  const int moi = Process::me();
  // Fill nodes coordinates and bounding boxes
  DoubleTab bounding_boxes(nproc, dim*2);
  {
    const double big_val = 1.e36;
    ArrOfDouble min_coord(dim);
    min_coord= big_val;
    ArrOfDouble max_coord(dim);
    max_coord= -big_val;

    for (i = 0; i < ncoord; i++)
      {
        for (j = 0; j < dim; j++)
          {
            const double x = coords(i, j);
            if (x > max_coord[j])
              max_coord[j] = x;
            if (x < min_coord[j])
              min_coord[j] = x;
          }
      }
    // broadcast a tous les processeurs
    for (i = 0; i < nproc; i++)
      {
        for (j = 0; j < dim; j++)
          {
            bounding_boxes(i, j) = min_coord[j];
            bounding_boxes(i, j+dim) = max_coord[j];
          }
      }
    envoyer_all_to_all(bounding_boxes, bounding_boxes);
  }
  // Exchange boundary nodes coordinates with other processors
  ArrOfInt all_pe;
  all_pe.set_smart_resize(1);
  for (i = 0; i < nproc; i++)
    if (i != moi && bbox_intersection(bounding_boxes, moi, i, epsilon))
      all_pe.append_array(i);

  Schema_Comm schema;
  schema.set_send_recv_pe_list(all_pe, all_pe);
  schema.begin_comm();
  Octree_Double octree;
  octree.build_nodes(coords, 0 /* do not include virtual nodes */);

  // Send nodes coordinates to processors with non empty intersection
  // temporary array for octree functions:
  ArrOfInt nodes_list;
  nodes_list.set_smart_resize(1);
  int ipe;
  for (ipe = 0; ipe < all_pe.size_array(); ipe++)
    {
      // Search nodes within the bounding box of the other processor:
      const int pe = all_pe[ipe]; // processor number
      const double xmin = bounding_boxes(pe, 0) - epsilon;
      const double ymin = bounding_boxes(pe, 1) - epsilon;
      const double zmin = (dim == 3) ? bounding_boxes(pe, 2) - epsilon : -epsilon;
      const double xmax = bounding_boxes(pe, dim) + epsilon;
      const double ymax = bounding_boxes(pe, dim+1) + epsilon;
      const double zmax = (dim == 3) ? bounding_boxes(pe, dim+2) + epsilon : epsilon;
      octree.search_elements_box(xmin, ymin, zmin, xmax, ymax, zmax, nodes_list);

      Sortie& buffer = schema.send_buffer(pe);
      const int n = nodes_list.size_array();
      for (i = 0; i < n; i++)
        {
          const int som = nodes_list[i];
          const double x = coords(som, 0);
          const double y = coords(som, 1);
          const double z = (dim==3) ? coords(som, 2) : 0;
          // test again if inside bounding box (octree test gives more nodes)
          if (x>=xmin && x<=xmax && y>=ymin && y<=ymax && z>=zmin && z<=zmax)
            {
              buffer << x << y << z;
            }
        }
    }
  schema.echange_taille_et_messages();
  for (ipe = 0; ipe < all_pe.size_array(); ipe++)
    {
      const int pe = all_pe[ipe]; // processor number
      Entree& buffer = schema.recv_buffer(pe);
      ArrOfInt& resu = match[pe];
      resu.set_smart_resize(1);
      resu.resize_array(0);
      while(1)
        {
          double x, y, z;
          buffer >> x >> y >> z;
          if (buffer.eof())
            break;
          // Seach for this node in my list
          octree.search_elements_box(x-epsilon, y-epsilon, z-epsilon,
                                     x+epsilon, y+epsilon, z+epsilon, nodes_list);
          // Refine search to keep only matching nodes:
          octree.search_nodes_close_to(x, y, z, coords, nodes_list, epsilon);
          const int n = nodes_list.size_array();
          if (n > 1)
            {
              Cerr << "Error in automatic joint builder: more than one node within epsilon tolerance\n"
                   << " node coord = " << x << " " << y << " " << z << " tolerance= " << epsilon << finl;
              Process::exit();
            }
          if (n == 1)
            resu.append_array(nodes_list[0]);
        }
    }
  schema.end_comm();
}

// Description: finds all faces of all elements that
//  - have only one neighbour elements
//  - and do not belong to a boundary
// (these are joint faces)
// Fills "faces" with the local node numbers of the faces that have been found.
static void find_joint_faces(const Zone& zone, IntTab& faces)
{
  Static_Int_Lists som_elem;
  const IntTab& elements = zone.les_elems();
  const int nb_som = zone.domaine().nb_som();
  construire_connectivite_som_elem(nb_som, elements, som_elem, 0 /* do not include virtual elements */);
  const int nb_som_faces = zone.type_elem().valeur().nb_som_face();
  faces.resize(0, nb_som_faces);
  faces.set_smart_resize(1);
  IntTab faces_element_reference;
  zone.type_elem().valeur().get_tab_faces_sommets_locaux(faces_element_reference);
  const int nb_faces_elem = faces_element_reference.dimension(0);
  // Mark faces of elements that are on a boundary
  const int nb_elem = elements.dimension(0);
  ArrOfBit boundary_faces(nb_elem * nb_faces_elem);
  boundary_faces = 0;
  ArrOfInt une_face(nb_som_faces);
  ArrOfInt liste_elements;
  liste_elements.set_smart_resize(1);
  const int nb_boundaries = zone.nb_front_Cl();
  for (int i_boundary = 0; i_boundary < nb_boundaries; i_boundary++)
    {
      const IntTab& faces_front = zone.frontiere(i_boundary).faces().les_sommets();
      const int nb_faces_bord = faces_front.dimension(0);
      for (int j = 0; j < nb_faces_bord; j++)
        {
          for (int k = 0; k < nb_som_faces; k++)
            une_face[k] = faces_front(j, k);
          find_adjacent_elements(som_elem, une_face, liste_elements);
          if (liste_elements.size_array() != 1)
            {
              Cerr << "Error in MaillerParallel::find_joint_faces: boundary face ArrOfInt=" << une_face
                   << " does not have exactly 1 neighbour element: " << liste_elements;
              Process::exit();
            }
          const int elem = liste_elements[0];
          const int face_locale = Faces_builder::chercher_face_element(elements, faces_element_reference, une_face, elem);
          boundary_faces.setbit(elem * nb_faces_elem + face_locale);
        }
    }
  // Browse all faces of all elements, find faces with only one neighbour
  for (int elem = 0; elem < nb_elem; elem++)
    {
      for (int face_locale = 0; face_locale < nb_faces_elem; face_locale++)
        {
          if (boundary_faces[elem * nb_faces_elem + face_locale])
            continue;
          int i;
          for (i = 0; i < nb_som_faces; i++)
            {
              const int sommet_local = faces_element_reference(face_locale, i);
              une_face[i] = elements(elem, sommet_local);
            }
          find_adjacent_elements(som_elem, une_face, liste_elements);
          if (liste_elements.size_array() == 1)
            {
              const int n = faces.dimension(0);
              faces.resize_dim0(n+1);
              for (i = 0; i < nb_som_faces; i++)
                faces(n, i) = une_face[i];
            }
        }
    }
  Process::Journal() << "Domain " << zone.domaine().le_nom() << " has " << faces.dimension(0) << " undeclared boundary faces candidates for joint faces" << finl;
}

static void auto_build_joints(Zone& zone, const int epaisseur_joint)
{
  const double epsilon = 1e-10;
  int i, j;
  // Find joint faces (faces with 1 neighbour element not registered in boundary faces)
  IntTab faces;
  find_joint_faces(zone, faces);

  const DoubleTab& sommets = zone.domaine().les_sommets();
  // List of unique local boundary node numbers
  const int dim = sommets.dimension(1);

  {
    ArrOfInt boundary_nodes_index;
    boundary_nodes_index = faces; // copie du tableau
    array_trier_retirer_doublons(boundary_nodes_index);
    const int ncoord = boundary_nodes_index.size_array();
    // Fill nodes coordinates and bounding boxes
    DoubleTab coord;
    coord.resize(ncoord, dim, Array_base::NOCOPY_NOINIT);
    for (i = 0; i < ncoord; i++)
      {
        const int som = boundary_nodes_index[i];
        for (j = 0; j < dim; j++)
          coord(i, j) = sommets(som, j);
      }
    VECT(ArrOfInt) match(Process::nproc());
    find_matching_coordinates(coord, match, epsilon);

    for (int pe = 0; pe < match.size(); pe++)
      {
        const ArrOfInt& list = match[pe];
        const int n = list.size_array();
        if (n > 0)
          {
            Joint& joint = zone.faces_joint().add(Joint());
            joint.nommer("Joint_i");
            joint.associer_zone(zone);
            joint.affecte_epaisseur(epaisseur_joint);
            joint.affecte_PEvoisin(pe);
            joint.faces().typer(zone.type_elem().valeur().type_face());
            ArrOfInt& sommets_joint = joint.set_joint_item(Joint::SOMMET).set_items_communs();
            sommets_joint.resize_array(n, Array_base::NOCOPY_NOINIT);
            for (i = 0; i < n; i++)
              sommets_joint[i] = boundary_nodes_index[list[i]];
            sommets_joint.ordonne_array();
            Process::Journal() << "Joint sommets with pe " << pe << " has " << n << " nodes" << finl;
          }
      }
  }

  {
    // Find matching faces (match the coordinates of the centers)
    const int nfaces = faces.dimension(0);
    const int nb_som_faces = faces.dimension(1);
    DoubleTab coord(nfaces, dim); // init with 0.
    const double facteur = 1. / (double)nb_som_faces;
    for (i = 0; i < nfaces; i++)
      {
        for (j = 0; j < nb_som_faces; j++)
          {
            const int som = faces(i,j);
            for (int k = 0; k < dim; k++)
              coord(i, k) += sommets(som, k) * facteur;
          }
      }
    VECT(ArrOfInt) match(Process::nproc());
    find_matching_coordinates(coord, match, epsilon);

    for (int pe = 0; pe < match.size(); pe++)
      {
        const ArrOfInt& list = match[pe];
        const int n = list.size_array();
        if (n > 0)
          {
            REF(Joint) ref_joint;
            int ii;
            for (ii = 0; ii < zone.faces_joint().size(); ii++)
              {
                Joint& joint = zone.faces_joint()[ii];
                if (joint.PEvoisin() == pe)
                  {
                    ref_joint = joint;
                    break;
                  }
              }
            if (!ref_joint.non_nul())
              {
                Joint& joint = zone.faces_joint().add(Joint());
                joint.nommer("Joint_i");
                joint.associer_zone(zone);
                joint.affecte_epaisseur(epaisseur_joint);
                joint.affecte_PEvoisin(pe);
                joint.faces().typer(zone.type_elem().valeur().type_face());
                ref_joint = joint;
              }
            Faces& les_faces = ref_joint.valeur().faces();
            les_faces.dimensionner(n);
            IntTab& faces_sommets = les_faces.les_sommets();
            for (ii = 0; ii < n; ii++)
              {
                const int num_face = list[ii];
                for (j = 0; j < nb_som_faces; j++)
                  faces_sommets(ii, j) = faces(num_face, j);
              }
            Process::Journal() << "Joint faces with pe " << pe << " has " << n << " faces" << finl;
          }
      }
  }
  // Tri des joints dans l'ordre croissant des processeurs
  Scatter::trier_les_joints(zone.faces_joint());
}

Entree& MaillerParallel::interpreter(Entree& is)
{
  if (dimension != 3)
    {
      Cerr << "Error: Mailler_Parallel is only coded for dimension 3" << finl;
      barrier();
      exit();
    }

  Nom      nom_domaine;
  ArrOfInt nb_noeuds;
  ArrOfInt decoupage;
  int   epaisseur_joint;
  ArrOfInt perio(3);
  Noms fonctions_coord(3);
  Noms nom_bords_min(3);
  Noms nom_bords_max(3);
  nom_bords_min[0] = "xmin";
  nom_bords_min[1] = "ymin";
  nom_bords_min[2] = "zmin";
  nom_bords_max[0] = "xmax";
  nom_bords_max[1] = "ymax";
  nom_bords_max[2] = "zmax";
  Noms fichier_coord(3);
  IntTab mapping;
  {
    Param param(que_suis_je());
    param.ajouter("domain", &nom_domaine, Param::REQUIRED);
    param.ajouter("nb_nodes", &nb_noeuds, Param::REQUIRED);
    param.ajouter("splitting", &decoupage, Param::REQUIRED);
    param.ajouter("ghost_thickness", &epaisseur_joint, Param::REQUIRED);
    param.ajouter_flag("perio_x", &perio[0]);
    param.ajouter_flag("perio_y", &perio[1]);
    param.ajouter_flag("perio_z", &perio[2]);
    param.ajouter("function_coord_x", &fonctions_coord[0]);
    param.ajouter("function_coord_y", &fonctions_coord[1]);
    param.ajouter("function_coord_z", &fonctions_coord[2]);
    param.ajouter("file_coord_x", &fichier_coord[0]);
    param.ajouter("file_coord_y", &fichier_coord[1]);
    param.ajouter("file_coord_z", &fichier_coord[2]);
    param.ajouter("boundary_xmin", &nom_bords_min[0]);
    param.ajouter("boundary_xmax", &nom_bords_max[0]);
    param.ajouter("boundary_ymin", &nom_bords_min[1]);
    param.ajouter("boundary_ymax", &nom_bords_max[1]);
    param.ajouter("boundary_zmin", &nom_bords_min[2]);
    param.ajouter("boundary_zmax", &nom_bords_max[2]);
    param.ajouter("mapping", &mapping);
    param.lire_avec_accolades(is);
  }

  VECT(ArrOfDouble) coord_ijk(3);
  for (int dir=0; dir<3; dir++)
    {
      const int n = nb_noeuds[dir];
      ArrOfDouble& coord = coord_ijk[dir];
      coord.resize_array(n);
      Cerr << "Building coordinates for direction " << dir << finl;
      if (fonctions_coord[dir] != "??")
        {
          Nom chaine = fonctions_coord[dir];
          Cerr << " Interpreting expression " << chaine << finl;
          Parser_U p;
          Noms noms_dir(3);
          noms_dir[0]="X";
          noms_dir[1]="Y";
          noms_dir[2]="Z";
          p.setNbVar(1);
          p.setString(chaine);
          p.addVar(noms_dir[dir]);
          p.parseString();
          for (int i = 0; i < n; i++)
            {
              p.setVar(0, (double) i / (double) (nb_noeuds[dir]-1));
              coord[i] = p.eval();
              if (i > 0 && coord[i] <= coord[i-1])
                {
                  Cerr << "Error in fonctions_coord parameter " << chaine << " : must be strictly increasing" << finl;
                  Process::exit();
                }
            }
          Cerr << " Build mesh covering [ " << coord[0] << " , " << coord[n-1] << " ]" << finl;
        }
      else if (fichier_coord[dir] != "??")
        {
          Cerr << " Reading file " << fichier_coord[dir] << finl;
          if (Process::je_suis_maitre())
            {
              EFichier f(fichier_coord[dir]);
              for (int i = 0; i < n; i++)
                f >> coord[i];
            }
          envoyer_broadcast(coord, 0);
          Cerr << " Found mesh covering [ " << coord[0] << " , " << coord[n-1] << " ]" << finl;
        }
      else
        {
          Cerr << " Uniform mesh covering [0,1]" << finl;
          for (int i = 0; i < n; i++)
            {
              coord[i] = (double) i / (double)(n-1);
            }
        }
    }

  Objet_U& obj = objet(nom_domaine);
  if(!sub_type(Domaine, obj))
    {
      Cerr << "obj : " << nom_domaine << " is not an object of type Domaine !" << finl;
      exit();
    }

  Domaine& domaine = ref_cast(Domaine, obj);

  const int numproc = Process::me();

  static Stat_Counter_Id stats = statistiques().new_counter(0 /* Level */, "Mailler_Parallel", 0 /* Group */);

  const int dim = Objet_U::dimension;
  if (nb_noeuds.size_array() != dim)
    {
      Cerr << "MaillerParallel::construire_domaine wrong dimension for nb_noeuds : nb_noeuds=" << nb_noeuds << finl;
      barrier();
      exit();
    }

  if (decoupage.size_array() != dim)
    {
      Cerr << "MaillerParallel::construire_domaine wrong dimension for nb_noeuds : nb_noeuds=" << nb_noeuds << finl;
      barrier();
      exit();
    }

  {
    int n = 1;
    for (int i = 0; i < dim; i++)
      n *= decoupage[i];
    if (n != Process::nproc())
      {
        Cerr << "MaillerParallel::construire_domaine decoupage does not match nproc" << finl;
        barrier();
        exit();
      }
  }
  statistiques().begin_count(stats);

  // Position du bloc correspondant a numproc dans le decoupage i,j,k:
  ArrOfInt i_proc(dim);
  if (mapping.dimension(0) == 0)
    {
      // Pas de mapping fourni, on prend decoupe par defaut
      int i;
      int reste = numproc;

      for (i = 0; i < dim; i++)
        {
          // Nombre de parties dans la direction dim_2:
          const int n_parties = decoupage[i];
          // Position du processeur courant dans cette direction:
          i_proc[i] = reste % n_parties;
          reste = reste / n_parties;
        }
    }
  else
    {
      if (mapping.dimension(0) != Process::nproc() || mapping.dimension(1) != 3)
        {
          Cerr << "Error: processor mapping should be of dimension " << Process::nproc() <<  " x 3" << finl;
          Process::exit();
        }
      i_proc[0] = mapping(numproc,0);
      i_proc[1] = mapping(numproc,1);
      i_proc[2] = mapping(numproc,2);
    }
  // Indice du premier element du bloc associe au processeur dans la direction i:
  // Attention, en cas de periodique, i_premier_element sera negatif.
  ArrOfInt i_premier_element(dim);
  // Taille du bloc associe au processeur
  ArrOfInt nb_elements(dim);
  {
    int i;
    for (i = 0; i < dim; i++)
      {
        const int nb_elem_tot = nb_noeuds[i] - 1; // Nb total de noeuds dans la direction i
        i_premier_element[i] = nb_elem_tot * i_proc[i] / decoupage[i];
        const int i_dernier_element = nb_elem_tot * (i_proc[i] + 1) / decoupage[i] - 1;
        nb_elements[i] = i_dernier_element - i_premier_element[i] + 1;

        if (perio[i] && decoupage[i] > 1)
          {
            int decalage = nb_elem_tot / decoupage[i] / 2;
            i_premier_element[i] -= decalage;
          }
      }
  }

  // On cree une zone pour le domaine
  Zone empty_zone;
  Zone& zone = domaine.add(empty_zone);
  // Nom de la zone
  {
    Nom nom_zone("Proc");
    nom_zone += Nom(numproc);
    nom_zone += "sur";
    nom_zone += Nom(Process::nproc());
    zone.nommer(nom_zone);
  }
  zone.associer_domaine(domaine);
  {
    Elem_geom& elem = zone.type_elem();
    switch(dim)
      {
      case 3:
        elem.typer("Hexaedre");
        break;
      default:
        Cerr << "MaillerParallel::construire_domaine  erreur" << finl;
        exit();
      }
    elem.valeur().associer_zone(zone);
  }

  BlocData data;
  Noms liste_bords_perio;
  Bords& bords = zone.faces_bord();
  data.bord_xmin_ = nom_bords_min;
  data.bord_xmax_ = nom_bords_max;
  for (int dir = 0; dir < 3; dir++)
    {
      Nom nomdir;
      if (dir == 0) nomdir = "x";
      else if (dir == 1) nomdir = "y";
      else nomdir = "z";
      if (perio[dir])
        {
          data.bord_xmax_[dir] = data.bord_xmin_[dir];
          bords.add(Bord()).nommer(data.bord_xmin_[dir]);
        }
      else
        {
          bords.add(Bord()).nommer(data.bord_xmin_[dir]);
          bords.add(Bord()).nommer(data.bord_xmax_[dir]);
        }
    }
  data.xmax_tot_ = nb_noeuds;

  ArrOfInt nblocs(3);
  nblocs= 1;
  for (int i = 0; i < 3; i++)
    {
      if (i_premier_element[i] < 0)
        nblocs[i] = 2;
    }

  int num_bord;
  for (num_bord = 0; num_bord < bords.size(); num_bord++)
    {
      Bord& bord = bords[num_bord];
      bord.associer_zone(zone);
      bord.faces().typer(zone.type_elem().valeur().type_face());
      // important pour dimensionner le linesize du tableau des faces pour les frontieres vides
      bord.faces().dimensionner(0);
    }

  ArrOfInt ibloc(3);
  for (ibloc[0] = 0; ibloc[0] < nblocs[0]; ibloc[0]++)
    {
      for (ibloc[1] = 0; ibloc[1]  < nblocs[1]; ibloc[1]++)
        {
          for (ibloc[2] = 0; ibloc[2]  < nblocs[2]; ibloc[2]++)
            {

              data.xmin_.resize_array(3);
              data.xmax_.resize_array(3);
              for (int dir = 0; dir < 3; dir++)
                {
                  data.xmin_[dir] = i_premier_element[dir];
                  data.xmax_[dir] = i_premier_element[dir] + nb_elements[dir] + 1;

                  if (nblocs[dir] > 1)
                    {
                      if (ibloc[dir] == 0)
                        data.xmin_[dir] = 0;
                      else
                        {
                          data.xmin_[dir] += data.xmax_tot_[dir] - 1;
                          data.xmax_[dir] = data.xmax_tot_[dir];
                        }
                    }
                }
              Process::Journal() << "add bloc xmin=" << data.xmin_ << "xmax=" << data.xmax_ << "xtot=" << data.xmax_tot_;
              data.add_bloc(domaine, coord_ijk);
            }
        }
    }

  {
    for (int d = 0; d < 3; d++)
      {
        if (perio[d])
          {
            ArrOfDouble dir(3);
            dir[d] = coord_ijk[d][nb_noeuds[d]-1] - coord_ijk[d][0];
            IntTab& faces = domaine.zone(0).bord(nom_bords_min[d]).faces().les_sommets();
            double epsilon = precision_geom;
            Reordonner_faces_periodiques::reordonner_faces_periodiques(domaine, faces, dir, epsilon);
          }
      }
  }


  auto_build_joints(domaine.zone(0), epaisseur_joint);


  statistiques().end_count(stats);
  double maxtime = mp_max(statistiques().last_time(stats));
  if (Process::je_suis_maitre())
    Cerr << "Ending of the construction of the zones, time:" << maxtime << finl;

  if (nproc() > 1)
    {

      statistiques().begin_count(stats);

      Scatter::construire_correspondance_sommets_par_coordonnees(domaine);
      Scatter::calculer_renum_items_communs(domaine.zone(0).faces_joint(), Joint::SOMMET);

      statistiques().end_count(stats);
      maxtime = mp_max(statistiques().last_time(stats));
      Cerr << "Scatter::construire_correspondance_sommets_par_coordonnees fin, time:"
           << maxtime << finl;

      statistiques().begin_count(stats);

      Scatter::construire_structures_paralleles(domaine, liste_bords_perio);

      statistiques().end_count(stats);
      maxtime = mp_max(statistiques().last_time(stats));
      Cerr << "Scatter::construire_structures_paralleles, time:" << maxtime << finl;
    }
  else
    {
      Scatter::init_sequential_domain(domaine);
    }

  domaine.zone(0).nommer(domaine.le_nom());

  Cerr << "MaillerParallel::construire_domaine : end" << finl;

  return is;
}
