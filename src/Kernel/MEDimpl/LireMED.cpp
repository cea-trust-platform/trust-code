/****************************************************************************
* Copyright (c) 2024, CEA
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

#include <LireMED.h>
#include <Domaine.h>
#include <Param.h>
#include <EChaine.h>
#include <med++.h>
#include <NettoieNoeuds.h>
#include <SFichier.h>
#include <TRUSTList.h>
#include <TRUSTTabs.h>
#include <TRUSTArrays.h>
#include <Polygone.h>
#include <Polyedre.h>
#include <Scatter.h>
#include <Synonyme_info.h>
#include <RegroupeBord.h>
#include <Char_ptr.h>
#include <fstream>
#include <TRUST_2_MED.h>

#ifdef MEDCOUPLING_
#include <MEDCouplingMemArray.hxx>

using namespace MEDCoupling;
#endif

// XD Read_MED interprete lire_med 1 Keyword to read MED mesh files where 'domain' corresponds to the domain name, 'file' corresponds to the file (written in the MED format) containing the mesh named mesh_name. NL2 Note about naming boundaries: When reading 'file', TRUST will detect boundaries between domains (Raccord) when the name of the boundary begins by 'type_raccord\_'. For example, a boundary named type_raccord_wall in 'file' will be considered by TRUST as a boundary named 'wall' between two domains. NL2 NB: To read several domains from a mesh issued from a MED file, use Read_Med to read the mesh then use Create_domain_from_sub_domain keyword. NL2 NB: If the MED file contains one or several subdomaine defined as a group of volumes, then Read_MED will read it and will create two files domain_name_ssz.geo and domain_name_ssz_par.geo defining the subdomaines for sequential and/or parallel calculations. These subdomaines will be read in sequential in the datafile by including (after Read_Med keyword) something like: NL2 Read_Med .... NL2 Read_file domain_name_ssz.geo ; NL2 During the parallel calculation, you will include something: NL2 Scatter { ... } NL2 Read_file domain_name_ssz_par.geo ;
Implemente_instanciable_32_64(LireMED_32_64,"Lire_MED",Interprete_geometrique_base_32_64<_T_>);
Add_synonym(LireMED,"Read_med");

////
//// Anonymous namespace for local methods to this translation unit:
////
namespace
{

/*! Retrieve connectivity of a mesh in an ArrOfInt_t, casting when necessary.
 * If the type sizes match, a simple ref is taken, otherwise a hard copy is done.
 */
template <typename _SIZE_>
void retrieve_connec(const MEDCouplingUMesh* mesh, ArrOfInt_T<_SIZE_>& conn, ArrOfInt_T<_SIZE_>& connIndex)
{
  _SIZE_ nb_it = static_cast<_SIZE_>(mesh->getNodalConnectivity()->getNbOfElems()),
         nb_it2= static_cast<_SIZE_>(mesh->getNodalConnectivityIndex()->getNbOfElems());
  const mcIdType *c  = mesh->getNodalConnectivity()->begin(),
                  *cI = mesh->getNodalConnectivityIndex()->begin();
  if (std::is_same<_SIZE_, mcIdType>::value)
    {
      // I know, wild cast, discarding const ...:
      conn.ref_data((_SIZE_ *)c, mesh->getNodalConnectivity()->getNbOfElems());
      connIndex.ref_data((_SIZE_ *)cI, mesh->getNodalConnectivityIndex()->getNbOfElems());
    }
  else // Type mismatch - must hardcopy!
    {
      conn.resize(nb_it);
      std::copy(c, c+nb_it, conn.addr());
      connIndex.resize(nb_it2);
      std::copy(cI, cI+nb_it2, connIndex.addr());
    }
}

template <typename _SIZE_>
void verifier_modifier_type_elem(Nom& type_elem,const IntTab_T<_SIZE_>& les_elems,const DoubleTab_T<_SIZE_>& sommets)
{
  using int_t = _SIZE_;

  if ((type_elem=="Rectangle")||(type_elem=="Hexaedre"))
    {
      int dimension=sommets.dimension_int(1);
      int nb_som_elem=les_elems.dimension_int(1);
      bool ok=true;
      ArrOfDouble pos(2);
      for (int_t elem=0; elem<les_elems.dimension(0) && ok; elem++)
        {
          // pour chaque elt on verifie si il est bien a angle droit
          for (int dir=0; dir<dimension && ok; dir++)
            {
              // on compte le nombre de pos de sommmets dans chaque direction
              int n=0;
              for (int s=0; s<nb_som_elem && ok; s++)
                {
                  double npos=sommets(les_elems(elem,s),dir);
                  bool trouve=false;
                  for (int i=0; i<n; i++)
                    if (est_egal(npos,pos[i])) trouve=1;

                  if (!trouve)
                    {
                      if (n==2)
                        {
                          Cerr<<"There is at least the element "<<elem<<" wich seems to not possess a straight angle"<<finl;
                          ok=false;
                        }
                      else
                        {
                          pos[n]=npos;
                          n++;
                        }
                    }
                }
            }
        }
      if (!ok)
        {
          // on change type_elem
          if (type_elem=="Rectangle")
            type_elem="Quadrangle";
          else if  (type_elem=="Hexaedre")
            type_elem="Hexaedre_vef";
        }
    }
}

template <typename _SIZE_>
void recuperer_info_des_joints(Noms& noms_des_joints, const Nom& nom_fic, const Nom& nom_dom,
                               std::vector<ArrOfInt_T<_SIZE_>>& corres_joint,  ArrOfInt& tab_pe_voisin)
{
#ifdef MEDCOUPLING_
  using namespace MEDCoupling;

  Cerr << "  Reading joint informations ... "<<finl;
  MCAuto<MEDFileJoints> jnts(MEDFileJoints::New(nom_fic.getChar(), nom_dom.getChar()));
  int njoint = jnts->getNumberOfJoints();
  corres_joint.resize(njoint);
  noms_des_joints.dimensionner(njoint);
  tab_pe_voisin.resize_array(njoint);

  for (int j=0; j<njoint; j++)
    {
      // Reading joint meta-infos
      const MEDFileJoint *jnt = jnts->getJointAtPos(j);
      std::string jnam = jnt->getJointName(), desc = jnt->getDescription();

      int num_dom = jnt->getDomainNumber();
      Cerr << "  Joint '" << jnam << "' - dom number: " << num_dom << " - '" << desc << "'" << finl;
      tab_pe_voisin[j] = num_dom;
      noms_des_joints[j] = jnam;

      // Multi-steps joints not supported
      int nsteps = jnt->getNumberOfSteps();
      if (nsteps > 1) Process::exit("LireMED_32_64/ScatterMED: Multi-step joints not supported!!");

      // Reading vertex/vertex connection
      const MEDFileJointOneStep* jnt1 = jnt->getStepAtPos(0);
      int nc = jnt1->getNumberOfCorrespondences();
      if (nc <= 0) Process::exit("LireMED_32_64/ScatterMED: joint with no correspondance!!");
      Cerr<<(int)nc <<" connecting vertices " <<finl;

      ArrOfInt_T<_SIZE_>& corres_joint_j = corres_joint[j];
      corres_joint_j.resize_array(nc);

      for (int c=0; c < nc; c++) // lol C++
        {
          const MEDFileJointCorrespondence * corr = jnt1->getCorrespondenceAtPos(c);
          if (!corr->getIsNodal())
            {
              Cerr << "  Skipping joint - it is not nodal!" << finl;
              continue;
            }
          const DataArrayIdType *da = corr->getCorrespondence();
          // TODO check this here
          if (da->getNumberOfTuples() != 1)
            Process::exit("WOOOPS ?");
          const mcIdType *daP = da->begin();
          corres_joint_j[c] = static_cast<_SIZE_>(daP[1]);  // overflow check done before
        }
    }
  Cerr << "  Done reading joint informations ... "<<finl;
#endif
}

} // end anonymous namespace

template <typename _SIZE_>
LireMED_32_64<_SIZE_>::LireMED_32_64(const Nom& file_name, const Nom& mesh_name) :
  nom_fichier_(file_name),
  nom_mesh_(mesh_name)
{ }

/*! @brief Simple appel a: Interprete::printOn(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
template <typename _SIZE_>
Sortie& LireMED_32_64<_SIZE_>::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

/*! @brief Simple appel a: Interprete::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
template <typename _SIZE_>
Entree& LireMED_32_64<_SIZE_>::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

template <typename _SIZE_>
Entree& LireMED_32_64<_SIZE_>::interpreter_(Entree& is)
{
#ifndef MED_
  med_non_installe();
#else
  Nom nom_dom_trio;

  is >> nom_dom_trio;
  if (nom_dom_trio == "{") // New syntax !!
    {
      Nom s = "{ ";
      int cnt = 1;
      while (cnt)
        {
          Nom motlu;
          is >> motlu;
          if (motlu == "{") cnt++;
          if (motlu == "}") cnt --;
          s += Nom(" ") + motlu;
        }
      Param param(this->que_suis_je());
      int convpoly = 0;
      param.ajouter_flag("convertAllToPoly", &convpoly);       // XD_ADD_P flag Option to convert mesh with mixed cells into polyhedral/polygonal cells

      param.ajouter("domain|domaine", &nom_dom_trio, Param::REQUIRED); // XD_ADD_P ref_domaine Corresponds to the domain name.
      param.ajouter("file|fichier", &nom_fichier_, Param::REQUIRED);        // XD_ADD_P chaine File (written in the MED format, with extension '.med') containing the mesh

      param.ajouter("mesh|maillage", &nom_mesh_);                       // XD_ADD_P chaine Name of the mesh in med file. If not specified, the first mesh will be read.

      param.ajouter("exclude_groups|exclure_groupes", &exclude_grps_); // XD_ADD_P listchaine List of face groups to skip in the MED file.
      param.ajouter("include_additional_face_groups|inclure_groupes_faces_additionnels", &internal_face_grps_); // XD_ADD_P listchaine List of face groups to read and register in the MED file.

      EChaine is2(s);
      param.lire_avec_accolades(is2);
      convertAllToPoly_ = (convpoly != 0);
    }
  else // old syntax ?
    {
      Cerr << "ERROR: 'Read_MED': expected opening brace '{' - are you using the new syntax?" << finl;
      Cerr << "If you are still using the old syntax (before TRUST v1.9.3), \nyou can use -convert_data option of your application script:" << finl;
      Cerr << "   trust -convert_data " << Objet_U::nom_du_cas() << ".data" << finl;
      Process::exit();
    }

  // Strip _0000 if there, and create proper file name
  Nom nom_fic2(nom_fichier_);
  nom_fic2.prefix(".med");
  if (nom_fic2==nom_fichier_)
    {
      Cerr<<"Error, the MED file should have .med as extension."<<finl;
      Cerr<<"See the syntax of Read_MED keyword." << finl;
      Process::exit();
    }
  Nom nom_fic3(nom_fic2);
  if (nom_fic3.prefix("_0000") != nom_fic2 )
    {
      nom_fic3+=".med";
      nom_fichier_=nom_fic3.nom_me(this->me());
    }
  this->associer_domaine(nom_dom_trio);
  lire_geom(true);
#endif

  return is;
}


/*! @brief renvoie le type trio a partir du type medocoupling : http://docs.salome-platform.org/6/gui/MED/MEDLoader_8cxx.html
 */
#ifdef MEDCOUPLING_
template <typename _SIZE_>
Nom LireMED_32_64<_SIZE_>::type_medcoupling_to_type_geo_trio(int type_cell, bool cell_from_boundary) const
{
  Nom type_elem;
  // [ABN] : first make sure the axis type is properly set, this will influence choice of the element
  // Set up correctly bidim_axi or axi variable according to MED file data.
  if (axis_type_==MEDCoupling::AX_CYL)
    {
      if (type_cell==INTERP_KERNEL::NORM_QUAD4)
        {
          if (!Objet_U::axi)
            {
              Cerr<<"WARNING, Cylindrical MED coordinates detected - we will use 'axi' keyword."<<finl;
              Objet_U::axi=1;
            }
        }
      else
        {
          Cerr << "Strange error with MED file - should never happen!? MED file with axis type AX_CYL contains elements other than NORM_QUAD4."<< finl;
          Process::exit();
        }
    }
  if (axis_type_==MEDCoupling::AX_SPHER)
    {
      Cerr << "Spherical coordinates read in the MED file - this is unsupported in TRUST!" << finl;
      Process::exit();
    }

  //
  // At this stage 'axi' and 'bidim_axi' are properly set.
  //
  if (type_cell==INTERP_KERNEL::NORM_QUAD4)
    type_elem = cell_from_boundary ? "QUADRANGLE_3D" : (isVEFForce_ ? "Quadrangle" : "Rectangle");
  else if (type_cell==INTERP_KERNEL::NORM_HEXA8)
    type_elem=(isVEFForce_ ? "Hexaedre_vef" : "Hexaedre");
  else if (type_cell==INTERP_KERNEL::NORM_TRI3)
    type_elem= cell_from_boundary ? "TRIANGLE_3D" : "Triangle";
  else if (type_cell==INTERP_KERNEL::NORM_TETRA4)
    type_elem="Tetraedre";
  else if (type_cell==INTERP_KERNEL::NORM_SEG2)
    type_elem= cell_from_boundary ? "SEGMENT_2D" : "Segment";
  else if (type_cell==INTERP_KERNEL::NORM_PENTA6)
    type_elem="Prisme";
  else if (type_cell==INTERP_KERNEL::NORM_POLYHED)
    type_elem="Polyedre";
  else if (type_cell==INTERP_KERNEL::NORM_PYRA5)
    type_elem="Pyramide";
  else if (type_cell==INTERP_KERNEL::NORM_POLYGON)
    type_elem= cell_from_boundary ? "POLYGONE_3D" : "Polygone";
  else if(type_cell==INTERP_KERNEL::NORM_HEXGP12)
    type_elem = "Prisme_hexag";
  else if(type_cell==INTERP_KERNEL::NORM_POINT1)
    type_elem = cell_from_boundary ? "POINT_1D" : "Point_1d";
  else
    {
      Cerr<<"Cell type " << type_cell<< " is not supported yet." <<finl;
      Process::exit();
    }
  if(Objet_U::bidim_axi && !(type_cell == INTERP_KERNEL::NORM_QUAD4 || type_cell == INTERP_KERNEL::NORM_SEG2))
    {
      Cerr<<"Cell type " << type_cell<< " is not supported for 'bidim_axi' mode." <<finl;
      Process::exit();
    }
  if (Objet_U::axi && !(type_cell == INTERP_KERNEL::NORM_HEXA8 || type_cell == INTERP_KERNEL::NORM_QUAD4))
    {
      Cerr<<"Cell type " << type_cell<< " is not supported for 'axi' mode." <<finl;
      Process::exit();
    }
  if (axi1d_ && !(type_cell == INTERP_KERNEL::NORM_SEG2 || type_cell == INTERP_KERNEL::NORM_POINT1))
    {
      Cerr<<"Cell type " << type_cell<< " is not supported for 'axi1d' mode." <<finl;
      Process::exit();
    }
  if (Objet_U::bidim_axi && type_cell == INTERP_KERNEL::NORM_QUAD4)
    type_elem = "Rectangle_2D_axi";
  if (Objet_U::axi)
    type_elem += "_axi";

  if (axi1d_)
    type_elem += "_axi";
  return type_elem;
}
#endif


/*! @brief Load the mesh from the MED file as a MEDCouplingUMesh, and name it as the domain.
 */
#ifdef MEDCOUPLING_

template <typename _SIZE_>
void LireMED_32_64<_SIZE_>::retrieve_MC_objects()
{
  std::string fileName = nom_fichier_.getString();
  std::string meshName = nom_mesh_.getString();

  // Check the mesh name is in the file:
  //const MCAuto<MEDFileMeshes> data(MEDFileMeshes::New(fileName));
  std::vector< std::string > meshes_names = MEDCoupling::GetMeshNames(fileName) ;
  if (std::find(meshes_names.begin(), meshes_names.end(), meshName) == meshes_names.end())
    {
      if (meshName == "--any--") meshName = meshes_names[0]; //magic name -> we take the first mesh
      else
        {
          Cerr << "Mesh " << nom_mesh_ << " not found in the med file " << nom_fichier_ << " !" << finl;
          Cerr << "List of meshes found:" << finl;
          for(unsigned int i = 0; i<meshes_names.size(); i++)
            Cerr << meshes_names[i] << finl;
          Process::exit(-1);
        }
    }

  mfumesh_ = MEDFileUMesh::New(fileName, meshName);
  // Name it correctly here so that all extracted MEDCouplingUMesh will be correctly named:
  mfumesh_->setName( this->domaine().le_nom().getChar());

  axis_type_ = mfumesh_->getAxisType();
  // Some checks:
  std::vector<True_int> nel = mfumesh_->getNonEmptyLevels();
  assert(nel[0] == 0);
  // Get the volume mesh:
  mcumesh_ = mfumesh_->getMeshAtLevel(nel[0]); // ToDo can not make it const because of ArrOfInt
  space_dim_ = mcumesh_->getSpaceDimension();
  Cerr << "Detecting a " << (int)mcumesh_->getMeshDimension() << "D mesh in " << space_dim_ << "D space." << finl;
  if (space_dim_ != Objet_U::dimension)
    {
      Cerr << "The mesh space dimension may be higher than the computation space dimension" << finl;
      Cerr << "as the algorithm will try to detect the useless direction in the mesh." << finl;
      //assert(dim == dimension);
    }

  // Desormais l'option permet de convertir tout type de maillage vers des polyedres:
  if (convertAllToPoly_)
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "Conversion to polyedrons and polygons..." << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      mcumesh_->convertAllToPoly(); // Conversion maillage volumique
    }
}

#endif


/*! Fills in coords and connectivity array from the MC data.
 */
#ifdef MEDCOUPLING_
template <typename _SIZE_>
void LireMED_32_64<_SIZE_>::prepare_som_and_elem(DoubleTab_t& sommets2, IntTab_t& les_elems2)
{
  using Polygone_t = Polygone_32_64<_SIZE_>;
  using Polyedre_t = Polyedre_32_64<_SIZE_>;

  // Get the nodes: size and fill sommets2:
  mcIdType nnodes0 = mcumesh_->getNumberOfNodes();
  if (nnodes0 >= std::numeric_limits<_SIZE_>::max())
    Process::exit("ERROR! You are trying to build a 32b domain with a mesh which is too big (too many nodes). Use Domaine_64.");
  int_t nnodes = static_cast<int_t>(nnodes0);

  const double *coord = mcumesh_->getCoords()->begin();
  Cerr << "Detecting " << nnodes << " nodes." << finl;
  sommets2.resize(nnodes, space_dim_);
  std::copy(coord, coord+sommets2.size_array(), sommets2.addr());

  // Get cell connectivity
  mcIdType ncells0 = mcumesh_->getNumberOfCells();
  if (ncells0 >= std::numeric_limits<_SIZE_>::max())
    Process::exit("ERROR! You are trying to build a 32b domain with a mesh which is too big (too many elements). Use Domaine_64.");
  int_t ncells = static_cast<int_t>(ncells0);

  // Use ArrOfInt to benefit from assert:
  ArrOfInt_t conn, connIndex;
  ::retrieve_connec(mcumesh_, conn, connIndex);

  int mesh_type_cell = static_cast<int>(conn[connIndex[0]]);  // type is always an int.
  Nom type_elem_n = type_medcoupling_to_type_geo_trio(mesh_type_cell, false);
  type_elem_.typer(type_elem_n);
  auto set_of_typs = mcumesh_->getAllGeoTypes();
  if (set_of_typs.size() > 1)
    {
      Cerr << "Elements of kind " << type_elem_n << " have already been read" << finl;
      Cerr << "New elements type(s) found!!" << finl;
      if (!convertAllToPoly_)
        {
          Cerr << "TRUST does not support different element types for the mesh." << finl;
          Cerr << "Either you remesh your domain with a single element type," << finl;
          Cerr << "or convert your cells to polyedrons and polygons by inserting an option in your command line:" << finl;
          Cerr << "   Read_MED { ... convertAllToPoly ... } " << finl;
          Cerr << "After that, you should use a discretization supporting polyedrons!" << finl;
          Process::exit();
        }
    }

  // Fill les_elem2 : Different treatment according type_elem:
  if (sub_type(Polyedre_t, type_elem_.valeur()))
    {
      int_t marker = 0;
      int_t conn_size = static_cast<int_t>(mcumesh_->getNodalConnectivity()->getNbOfElems()); // Overflow check done before
      for (int_t i = 0; i < conn_size; i++)
        if (conn[i]<0) marker++;
      int_t num_nodes = conn_size - ncells - marker;
      int_t nfaces = ncells + marker;
      ArrOfInt_t nodes(num_nodes), facesIndex(nfaces+1), polyhedronIndex(ncells+1);
      int_t face=0, node = 0;
      for (int_t i = 0; i < ncells; i++)
        {
          polyhedronIndex[i] = face; // Index des polyedres

          int_t index = connIndex[i] + 1;
          int nb_som = static_cast<int>(connIndex[i + 1] - index);
          for (int j = 0; j < nb_som; j++)
            {
              if (j==0 || conn[index + j]<0)
                facesIndex[face++] = node; // Index des faces:
              if (conn[index + j]>=0)
                nodes[node++] = conn[index + j]; // Index local des sommets de la face
            }
        }
      facesIndex[nfaces] = node;
      polyhedronIndex[ncells] = face;
      ref_cast(Polyedre_t,type_elem_.valeur()).affecte_connectivite_numero_global(nodes, facesIndex, polyhedronIndex, les_elems2);
    }
  else if (sub_type(Polygone_t, type_elem_.valeur()))
    {
      int_t conn_size = static_cast<int_t>(mcumesh_->getNodalConnectivity()->getNbOfElems()); // Overflow check done before
      int_t facesIndexSize = conn_size - ncells;
      ArrOfInt_t facesIndex(facesIndexSize), polygonIndex(ncells+1);
      int_t face=0;
      for (int_t i = 0; i < ncells; i++)
        {
          polygonIndex[i] = face;   // Index des polygones

          int_t index = connIndex[i] + 1;
          int nb_som = static_cast<int>(connIndex[i + 1] - index);
          for (int j = 0; j < nb_som; j++)
            facesIndex[face++] = conn[index + j];
        }
      polygonIndex[ncells] = face;
      ref_cast(Polygone_t,type_elem_.valeur()).affecte_connectivite_numero_global(facesIndex, polygonIndex, les_elems2);
    }
  else // Tous les autres types
    {
      for (int_t i = 0; i < ncells; i++)
        {
          int_t index = connIndex[i] + 1;
          int nb_som = static_cast<int>(connIndex[i + 1] - index);
          if (i==0) les_elems2.resize(ncells, nb_som); // Size les_elems2
          for (int j = 0; j < nb_som; j++)
            les_elems2(i, j) = conn[index + j];
        }
    }
}
#endif

/*! @brief Fills in sommets in the Domaine, potentially reducing the dimension (flat 3D -> 2D)
 *  by discarding a useless dimension (a flat 2D surface in a 3D space dim for example)
 *  TODO Fixme Adrien : rewrite this in MC style : buildUnique etc ...
 */
template <typename _SIZE_>
void LireMED_32_64<_SIZE_>::finalize_sommets(const DoubleTab_t& sommets2, DoubleTab_t& sommets) const
{
  // affectation des sommets
  if (space_dim_ != Objet_U::dimension)
    {
      Cerr << "One tries to read a meshing written in " << space_dim_ << "D in " << Objet_U::dimension << "D " << finl;
      // determination de la direction inutile
      int_t nbsom=sommets2.dimension(0);
      int dirinut=-1;
      const double epsilon = Objet_U::precision_geom;
      for (int dir=0; dir<space_dim_; dir++)
        {
          bool trouve=true;
          double val1=sommets2(0,dir);
          for (int_t i=0; i<nbsom; i++)
            if (std::abs(val1-sommets2(i,dir))>epsilon)
              {
                trouve=false;
                Cerr<<val1 << " "<<sommets2(i,dir)<<finl;
                break;
              }
          if (trouve)
            {
              dirinut=dir;
              break;
            }
        }
      if (dirinut==-1)
        {
          Cerr<<"No useless direction "<<finl;
          Process::exit();
        }
      Cerr<<"useless direction "<<dirinut<<finl<<finl;

      sommets.resize(nbsom,Objet_U::dimension);
      int d=0;
      for (int dir=0; dir<space_dim_; dir++)
        if (dir!=dirinut)
          {
            for (int_t i=0; i<nbsom; i++)
              sommets(i,d)=sommets2(i,dir);
            d++;
          }
    }
  else
    sommets=sommets2;
}

/* @brief Write datasets for sub-domains (built from MED element groups) into dedicated files.
 */
template <typename _SIZE_>
void LireMED_32_64<_SIZE_>::write_sub_dom_datasets() const
{
#ifdef MEDCOUPLING_

  unsigned nb_volume_groups = (unsigned)mfumesh_->getGroupsOnSpecifiedLev(0).size();
  if (nb_volume_groups>0 && Process::je_suis_maitre())
    {
      Nom nom_dom_trio = this->domaine().le_nom();
      SFichier jdd_seq(nom_dom_trio + "_ssz.geo");
      SFichier jdd_par(nom_dom_trio + "_ssz_par.geo");
      std::vector<std::string> groups = mfumesh_->getGroupsOnSpecifiedLev(0);

      for (const auto& gnam: groups)
        {
          MCAuto<DataArrayIdType> ids(mfumesh_->getGroupArr(0, gnam, false));
          const mcIdType *idP = ids->getConstPointer();
          mcIdType nb_elems0 = ids->getNbOfElems();
          if (nb_elems0 >= std::numeric_limits<_SIZE_>::max())
            Process::exit("ERROR! You are trying to build a 32b domain with a mesh which is too big (too many nodes). Use Domaine_64.");
          mcIdType nb_elems = static_cast<int_t>(nb_elems0);

          const Nom& nom_sous_domaine = gnam; // We take the name of the group for the subzone name
          Cerr << "Detecting a sub-zone (group name="<< nom_sous_domaine << ") with " << nb_elems << " cells." << finl;

          if (nb_elems == 0) continue;

          Nom file_ssz(nom_dom_trio);
          file_ssz += "_";
          file_ssz += nom_sous_domaine;
          file_ssz += Nom(".file");
          jdd_seq << "export Sous_Domaine " << nom_sous_domaine << finl;
          jdd_par << "export Sous_Domaine " << nom_sous_domaine << finl;

          jdd_seq << "Associer " << nom_sous_domaine << " " << nom_dom_trio << finl;
          jdd_par << "Associer " << nom_sous_domaine << " " << nom_dom_trio << finl;

          jdd_seq << "Lire " << nom_sous_domaine << " { fichier " << file_ssz << " }" << finl;
          jdd_par << "Lire " << nom_sous_domaine << " { fichier " << nom_sous_domaine << ".ssz" << " }" << finl;
          SFichier f_ssz(file_ssz);
          f_ssz << nb_elems << finl;
          for (int_t j = 0; j < nb_elems; j++)
            f_ssz << (int)idP[j] << " ";
          f_ssz << finl;
        }
    }
#endif
}

/*! @brief Handles the boundaries found in the MED file.
 *
 * Get the -1 level mesh, and extract boundaries by reading element groups on this mesh.
 */
template <typename _SIZE_>
void LireMED_32_64<_SIZE_>::read_boundaries(BigArrOfInt_& fac_grp_id, IntTab_t& all_faces_bords)
{
#ifdef MEDCOUPLING_

  constexpr bool CELL_FROM_BOUNDARY = true;

  // Get boundary mesh:
  MCAuto<MEDCouplingUMesh> face_mesh(mfumesh_->getMeshAtLevel(-1)); // ToDo can not make it const because of ArrayOfInt
  if (Objet_U::dimension==3 && convertAllToPoly_) // In 2D this will be segments anyway
    face_mesh->convertAllToPoly(); // Conversion maillage frontiere
  int_t nfaces = static_cast<int_t>(face_mesh->getNumberOfCells());  // overflow check done in prepare_som_and_elem()

  // Retrieve connectivity - Use ArrOfInt to benefit from assert:
  ArrOfInt_t conn, connIndex;
  ::retrieve_connec(face_mesh, conn, connIndex);
  DataArrayIdType *cI = face_mesh->getNodalConnectivityIndex();

  // Read type from the first face, and check unique type
  int typ = static_cast<int>(conn[connIndex[0]]);  // type is always int
  type_face_ = type_medcoupling_to_type_geo_trio(typ, CELL_FROM_BOUNDARY);
  if (type_elem_->nb_type_face() != 1)  // should never happen, all is polygon normally.
    Process::exit("Read_MED: unsupported mesh element type! It has more than a single face type (for example a prism can have triangles or quadrangles as boundary faces).");
  // Check unique type
  auto set_of_typs = face_mesh->getAllGeoTypes();
  if (set_of_typs.size() > 1) // same as a above, should never happen
    Process::exit("Read_MED: invalid boundary mesh! More than a single face type.");

  // Get max number of vertices per face - in a dedicated scope to destroy dsi quickly
  int max_som_face;
  {
    MCAuto<DataArrayIdType> dsi = cI->deltaShiftIndex();
    max_som_face = static_cast<int>(dsi->getMaxValueInArray() - 1); // -1 because first slot in connectivity descr is for type
  }

  // Filling face connectivity:
  Cerr << "Detecting " << nfaces << " faces (" << type_face_ << ")." << finl;
  all_faces_bords.resize(nfaces, max_som_face);
  all_faces_bords = -1;
  fac_grp_id.resize_array(nfaces);
  for (int_t i = 0; i < nfaces; i++)
    {
      int_t index = connIndex[i] + 1;  // +1 to skip MEDCoupling type
      int nb_som = static_cast<int>(connIndex[i + 1] - index);
      for (int j = 0; j < nb_som; j++)
        all_faces_bords(i, j) = conn[index + j] ;
    }

  Cerr << "Reading groups at level -1:" << finl;
  auto grp_names = mfumesh_->getGroupsOnSpecifiedLev(-1);
  int zeid = 0;
  std::set<mcIdType> id_check;
  for (const auto& gnam: grp_names)
    {
      if (exclude_grps_.search(Nom(gnam)) != -1)
        {
          Cerr << "    group '" << gnam << "' is skipped, as requested." << finl;
          continue;
        }
      noms_bords_.add(gnam);
      MCAuto<DataArrayIdType> ids(mfumesh_->getGroupArr(-1, gnam, false));
      long bef = (long)id_check.size(), nb_faces = (long)ids->getNumberOfTuples();
      id_check.insert(ids->begin(), ids->begin()+nb_faces);
      if ((long)id_check.size() - bef < nb_faces)
        {
          Cerr << "ERROR: group '" << gnam << "' contains faces which are also in one of the previously read groups." << finl;
          Cerr << "       Fix your mesh, or use the 'exclude_groups' option to prevent reading of this group." << finl;
          Process::exit(-1);
        }
      Cerr << "    group_name=" << gnam << " with " << nb_faces << " faces" << finl;
      for(const auto& id: *ids)
        {
          int_t id2 = static_cast<int_t>(id);
          fac_grp_id[id2] = zeid+1;
        }
      zeid++;
    }

  if (noms_bords_.size()==0)
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "Lire_MED: Warning: no boundary detected for the mesh." << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
    }
#endif
}

/*! @brief Fills in all the information relative to Joints, Raccords and Frontiere
 */
template <typename _SIZE_>
void LireMED_32_64<_SIZE_>::fill_frontieres(const BigArrOfInt_& fac_grp_id, const IntTab_t& all_faces_bords)
{
  using SmallArrOfTID_t = SmallArrOfTID_T<_SIZE_>;

  using Bord_t = Bord_32_64<_SIZE_>;
  using Bords_t = Bords_32_64<_SIZE_>;
  using Groupe_Faces_t = Groupe_Faces_32_64<_SIZE_>;
  using Groupes_Faces_t = Groupes_Faces_32_64<_SIZE_>;
  using Frontiere_t = Frontiere_32_64<_SIZE_>;
  using Raccord_t = OWN_PTR(Raccord_base_32_64<_SIZE_>);
  using Raccords_t = Raccords_32_64<_SIZE_>;
  using Joint_t = Joint_32_64<_SIZE_>;
  using Joints_t = Joints_32_64<_SIZE_>;
  using Faces_t = Faces_32_64<_SIZE_>;

  Domaine_t& dom = this->domaine();
  std::vector<ArrOfInt_t> sommets_joints;
  ArrOfInt tab_pe_voisin;

  int nbord = noms_bords_.size();
  SmallArrOfTID_t nb_face_per_bord(nbord);
  Bords_t& faces_bord=dom.faces_bord();
  faces_bord.vide();
  Groupes_Faces_t& goupes_faces=dom.groupes_faces();
  goupes_faces.vide();
  Raccords_t& faces_raccord=dom.faces_raccord();
  faces_raccord.vide();
  Joints_t& faces_joint=dom.faces_joint();
  faces_joint.vide();

  Noms noms_des_joints;
  recuperer_info_des_joints(noms_des_joints,nom_fichier_,nom_mesh_,sommets_joints,tab_pe_voisin);

  int_t nfacebord=all_faces_bords.dimension(0);
  for (int ib=nbord-1; ib>-1; ib--)
    {
      int indice_bord=ib+1;
      if (noms_bords_[ib]=="elems") continue;
      Bord_t bordprov_;
      Groupe_Faces_t facesgrpprov;
      Raccord_t raccprov;
      Joint_t jointprov;
      bool israccord=false, isjoint=false, isfacesint=false;
      if (noms_bords_[ib].debute_par("type_raccord_"))
        {
          israccord=true;
          noms_bords_[ib].suffix("type_raccord_");
          raccprov.typer("Raccord_local_homogene");
        }
      if (internal_face_grps_.search(noms_bords_[ib]) != -1)
        isfacesint=true;

      int numero_joint=noms_des_joints.search(noms_bords_[ib]);
      if (numero_joint>-1)
        {
          Cerr<<noms_bords_[ib]<<" is considered as a joint "<<finl;
          isjoint=true;
        }
      // on recupere la frontiere  .... que ce soit un Bord,Raccord,ou Joint
      Frontiere_t& bordprov = (isjoint?jointprov:(israccord?ref_cast(Frontiere_t,raccprov.valeur()):(isfacesint?ref_cast(Frontiere_t,facesgrpprov):ref_cast(Frontiere_t,bordprov_))));

      bordprov.nommer(noms_bords_[ib]);
      bordprov.typer_faces(type_face_);
      int_t nb_face_this_bord=0;
      int nsomfa=all_faces_bords.dimension_int(1);
      IntTab_t sommprov(nfacebord,nsomfa);
      for (int_t j=0; j<nfacebord; j++)
        {
          if (fac_grp_id[j]==indice_bord)
            {
              for (int k=0; k<nsomfa; k++)
                sommprov(nb_face_this_bord,k)=all_faces_bords(j,k);
              nb_face_this_bord++;
            }
        }
      Faces_t& facesi=bordprov.faces();
      IntTab_t& sommetsfaces=facesi.les_sommets();
      sommetsfaces.resize(nb_face_this_bord,nsomfa);
      for (int_t jj=0; jj<nb_face_this_bord; jj++)
        for (int k=0; k<nsomfa; k++)
          sommetsfaces(jj,k)=sommprov(jj,k);
      IntTab_t& facesv=facesi.voisins();
      facesv.resize(nb_face_this_bord,2);
      facesv=-1;

      bool vide_0D_a_ecrire=false;
      if (nb_face_this_bord == 0)
        {
          bordprov.typer_faces("vide_0D");
          vide_0D_a_ecrire = true;
        }
      nb_face_per_bord[ib] = nb_face_this_bord;
      // ne marche pas ajoute la famille elem
      if (indice_bord != 0
          && (nb_face_this_bord>0 || vide_0D_a_ecrire))
        {
          if (isjoint)
            {
              int PE_voisin=tab_pe_voisin[numero_joint];
              int epaisseur=1;
              ArrOfInt_t& sommets_joint=sommets_joints[numero_joint];
              jointprov.affecte_PEvoisin(PE_voisin);
              jointprov.affecte_epaisseur(epaisseur);
              ArrOfInt_t& sommets_du_joint=jointprov.set_joint_item(JOINT_ITEM::SOMMET).set_items_communs();
              sommets_du_joint=sommets_joint;
              faces_joint.add(jointprov);
            }
          else if (israccord)
            faces_raccord.add(raccprov);
          else if (isfacesint)
            goupes_faces.add(facesgrpprov);
          else
            faces_bord.add(bordprov_);
        }
    }
  faces_bord.associer_domaine(dom);
  faces_raccord.associer_domaine(dom);
  faces_joint.associer_domaine(dom);
  goupes_faces.associer_domaine(dom);
  dom.fixer_premieres_faces_frontiere();
  int nbfr=dom.nb_front_Cl();
  for (int fr=0; fr<nbfr; fr++)
    {
      dom.frontiere(fr).faces().associer_domaine(dom);
      if (dom.frontiere(fr).faces().type_face() != Type_Face::vide_0D)
        dom.frontiere(fr).faces().reordonner();
    }
}

template <typename _SIZE_>
void LireMED_32_64<_SIZE_>::lire_geom(bool subDom)
{
#ifndef MED_
  med_non_installe();
#else
#ifdef MEDCOUPLING_
  Domaine_t& dom = this->domaine();
  const Nom& nom_dom_trio = dom.le_nom();

  axi1d_ = dom.que_suis_je() == "DomaineAxi1d";
  // pour verif
  if (Objet_U::dimension==0)
    {
      Cerr << "Dimension is not defined. Check your data file." << finl;
      Process::exit();
    }
  Cerr << "Trying to read the domain " << nom_mesh_ << " from the file " << nom_fichier_ << " in order to affect to domain "
       << nom_dom_trio << "..." << finl;

  retrieve_MC_objects();

  // Reading vertices and element descriptions:
  DoubleTab_t sommets2;
  IntTab_t les_elems2;
  prepare_som_and_elem(sommets2, les_elems2);

  // Detect and export sub-domains (based on group of volumes);
  if (subDom)
    write_sub_dom_datasets();

  // Detect boundary meshes:
  BigArrOfInt_ fac_grp_id;
  IntTab_t all_faces_bords;
  std::vector<True_int> nel = mfumesh_->getNonEmptyLevels();
  if (nel.size() > 1)
    {
      assert(nel[1] == -1);
      read_boundaries(fac_grp_id, all_faces_bords);
    }

  // Converting from MED to TRUST connectivity
  Nom type_elem_n = type_elem_->que_suis_je();  // prepare_som_and_elem() has performed the 'typer' operation on type_elem_
  conn_trust_to_med(les_elems2,type_elem_n,false);
  conn_trust_to_med(all_faces_bords,type_face_,false);

  Scatter::uninit_sequential_domain(dom);

  DoubleTab_t& sommets=dom.les_sommets();
  finalize_sommets(sommets2, sommets);

  // Typage des elts du domaine et remplissage des elts
  // Avant de typer on regarde si il ne faut pas transormer les hexa en Hexa_vef
  Nom type_elem_orig = type_elem_n;
  verifier_modifier_type_elem(type_elem_n,les_elems2,sommets);

  dom.type_elem() = type_elem_;
  // si on a modifie type_elem
  if (type_elem_orig != type_elem_n)
    dom.typer(type_elem_n);
  dom.type_elem()->associer_domaine(dom);
  dom.les_elems() = les_elems2;

  fill_frontieres(fac_grp_id, all_faces_bords);

  //  GF au moins en polyedre il faut reordonner
  if (sub_type(Polyedre_32_64<_SIZE_>,dom.type_elem().valeur()) || type_elem_n == "Rectangle" || type_elem_n == "Hexaedre")
    dom.reordonner();

  if (Process::nproc()==1)
    {
      Cerr <<  "   Lire_MED called in sequential => applying NettoieNoeuds" << finl;
      NettoieNoeuds_32_64<_SIZE_>::nettoie(dom);
    }

  Scatter::init_sequential_domain(dom);

  // We might have created several boundaries with the same name
  RegroupeBord r;
  r.rassemble_bords(dom);

  // MCUMesh object will be rebuilt next time it is retreived:
  dom.set_mc_mesh_ready(false);

  Cerr<<"Reading of the MED domain ended."<<finl;
#endif // MEDCOUPLING_
#endif // MED_
}


template class LireMED_32_64<int>;
#if INT_is_64_ == 2
template class LireMED_32_64<trustIdType>;
#endif


