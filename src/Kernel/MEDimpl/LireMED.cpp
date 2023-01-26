/****************************************************************************
* Copyright (c) 2023, CEA
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
#include <medcoupling++.h>
#include <trust_med_utils.h>
#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#include <MEDFileMesh.hxx>
#pragma GCC diagnostic ignored "-Wreorder"
#include <MEDFileData.hxx>
using MEDCoupling::MCAuto;
using MEDCoupling::MEDCouplingUMesh;
using MEDCoupling::MEDFileUMesh;
using MEDCoupling::MEDFileMesh;
using MEDCoupling::MEDFileMeshes;
using MEDCoupling::DataArrayInt;
#endif

// XD Read_MED interprete lire_med 1 Keyword to read MED mesh files where 'domain' corresponds to the domain name, 'file' corresponds to the file (written in the MED format) containing the mesh named mesh_name. NL2 Note about naming boundaries: When reading 'file', TRUST will detect boundaries between domains (Raccord) when the name of the boundary begins by type_raccord_. For example, a boundary named type_raccord_wall in 'file' will be considered by TRUST as a boundary named 'wall' between two domains. NL2 NB: To read several domains from a mesh issued from a MED file, use Read_Med to read the mesh then use Create_domain_from_sub_domain keyword. NL2 NB: If the MED file contains one or several subdomaine defined as a group of volumes, then Read_MED will read it and will create two files domain_name_ssz.geo and domain_name_ssz_par.geo defining the subdomaines for sequential and/or parallel calculations. These subdomaines will be read in sequential in the datafile by including (after Read_Med keyword) something like: NL2 Read_Med .... NL2 Read_file domain_name_ssz.geo ; NL2 During the parallel calculation, you will include something: NL2 Scatter { ... } NL2 Read_file domain_name_ssz_par.geo ;
Implemente_instanciable(LireMED,"Lire_MED",Interprete_geometrique_base);
Add_synonym(LireMED,"Read_med");

////
//// Anonymous namespace for local methods to this translation unit:
////
namespace
{

void interp_old_syntax(Entree& is, int& isvefforce, int& convertAllToPoly, int& isfamilyshort, Nom& nom_dom_trio, Nom& nom_mesh, Nom& nom_fic)
{
  //Cerr << "Syntax: Lire_MED [ vef|convertAllToPoly  ] [ family_names_from_group_names | short_family_names ] domaine_name mesh_name filename.med" << finl;

  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
  Cerr << "!!!!!!! WARNING: you're using the old syntax for the keyword 'Lire_MED' / 'Read_MED'" << finl;
  Cerr << "!!!!!!! It will be deprecated in version 1.9.3. Please update your dataset with the following syntax:" << finl;
  Cerr << "    Read_MED {" << finl;
  Cerr << "        domain dom" << finl;
  Cerr << "        file the_file.med" << finl;
  Cerr << "        [mesh my_mesh_in_file]" << finl;
  Cerr << "    }" << finl;
  Cerr << "!!!!!!! See documentation for a comprehensive list of possible options." << finl;
  Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;

  // Warning, in the old syntax, this is set to 0 by default:
  isfamilyshort = 0;

  if (Motcle(nom_dom_trio)=="vef")
    {
      isvefforce=1;
      is>>nom_dom_trio;
    }
  else if (Motcle(nom_dom_trio)=="convertAllToPoly")
    {
      convertAllToPoly=1;
      is>>nom_dom_trio;
    }
  if (Motcle(nom_dom_trio)=="family_names_from_group_names")
    {
      isfamilyshort = 2;
      is>>nom_dom_trio;
    }
  else if (Motcle(nom_dom_trio)=="short_family_names")
    {
      isfamilyshort = 1;
      is>>nom_dom_trio;
    }

  lire_nom_med(nom_mesh,is);
  is >> nom_fic;
}

int verifier_modifier_type_elem(Nom& type_elem,const IntTab& les_elems,const DoubleTab& sommets)
{
  if ((type_elem=="Rectangle")||(type_elem=="Hexaedre"))
    {
      int dimension=sommets.dimension(1);
      int nb_som_elem=les_elems.dimension(1);
      int ok=1;
      ArrOfDouble pos(2);
      for (int elem=0; ((elem<les_elems.dimension(0))&&(ok)); elem++)
        {
          // pour chaque elt on verifie si il est bien a angle droit
          for (int dir=0; ((dir<dimension)&&(ok)); dir++)
            {
              // on compte le nombre de pos de sommmets dans chaque direction
              int n=0;
              for (int s=0; ((s<nb_som_elem)&&(ok)); s++)
                {
                  double npos=sommets(les_elems(elem,s),dir);
                  int trouve=0;
                  for (int i=0; i<n; i++)
                    if (est_egal(npos,pos[i])) trouve=1;

                  if (trouve==0)
                    {
                      if (n==2)
                        {
                          Cerr<<"There is at least the element "<<elem<<" wich seems to not possess a straight angle"<<finl;
                          ok=0;
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
      //ok=0;
      if (ok==0)
        {
          // on change type_elem
          if (type_elem=="Rectangle")
            type_elem="Quadrangle";
          else if  (type_elem=="Hexaedre")
            type_elem="Hexaedre_vef";
        }
      return 0;
    }
  return 0;
}

void recuperer_info_des_joints(Noms& noms_des_joints, const Nom& nom_fic, const Nom& nom_dom, ArrsOfInt& corres_joint,  ArrOfInt& tab_pe_voisin)
{
#ifdef MED_
  Cerr<<"reading of the joint informations "<<finl;
  int njoint=-1;
  med_idt fid=MEDfileOpen(nom_fic,MED_ACC_RDONLY);
  njoint=MEDnSubdomainJoint(fid,nom_dom);
  corres_joint.dimensionner(njoint);
  noms_des_joints.dimensionner(njoint);
  tab_pe_voisin.resize_array(njoint);

  for (int j=0; j<njoint; j++)
    {
      med_int num_dom;

      Char_ptr maa_dist;
      dimensionne_char_ptr_taille(maa_dist,MED_NAME_SIZE);
      Char_ptr name_of_joint(maa_dist);
      Char_ptr desc;
      dimensionne_char_ptr_taille(desc,MED_COMMENT_SIZE);
      med_int nstep;
      med_int nocstpn;
      MEDsubdomainJointInfo(fid, nom_dom, j+1, name_of_joint, desc,
                            &num_dom, maa_dist, &nstep,&nocstpn);
      Cerr<<" ici "<<name_of_joint<<" "<<(int)num_dom<< " "<<desc<<" "<<maa_dist<<finl;
      tab_pe_voisin[j]=num_dom;
      noms_des_joints[j]=name_of_joint;
      // Lecture de la correspondance sommet/sommet
      med_entity_type typ_ent_local;
      med_geometry_type typ_geo_local;
      med_entity_type typ_ent_distant;
      med_geometry_type typ_geo_distant;

      //       med_int corres=1; // faut il faire une boucle
      //       nb=MEDjointTypeCorres (fid, nom_dom, name_of_joint, corres,
      //                             &typ_ent_local,   &typ_geo_local,
      //                             &typ_ent_distant, &typ_geo_distant);
      //       Cerr<<MED_NODE<<finl;
      //       Cerr<< nb<<" LA " <<typ_ent_local<<" "<<typ_geo_local;
      //       Cerr<<" LA " <<typ_ent_distant<<" "<<typ_geo_distant<<finl;


      typ_geo_local=MED_NONE;
      typ_geo_distant=typ_geo_local;
      typ_ent_local=MED_NODE;
      typ_ent_distant=typ_ent_local;
      med_int nc;
      MEDsubdomainCorrespondenceSize(fid, nom_dom, name_of_joint,MED_NO_DT,MED_NO_IT,typ_ent_local,typ_geo_local,typ_ent_distant, typ_geo_distant,&nc);
      Cerr<<(int)nc <<" connecting vertices " <<finl;
      // lecture de la correspondance
      ArrOfInt& corres_joint_j =corres_joint[j];
      corres_joint_j.resize_array(nc);
      if (nc > 0)
        {
          med_int* cortab;

          cortab=new med_int[nc*2];
          med_int ret =MEDsubdomainCorrespondenceRd(fid,nom_dom,name_of_joint,MED_NO_DT,MED_NO_IT, typ_ent_local,typ_geo_local,typ_ent_distant,typ_geo_distant,cortab) ;
          if (ret<0)
            {
              Cerr<<"Error when reading the corresponding informations on the vertices"<<finl;
              Process::exit();
            }
          for (int s=0; s<nc; s++)
            {
              corres_joint_j[s]=cortab[2*s]-1; // -1 pour la numerotation C
            }
          // Cerr<<" sommets du joint "<<corres_joint_j<<finl;
          delete [] cortab;
        }
      else
        {
          Cerr<<" not vertex contained in the joint will leads to trouble" <<finl;
          Process::exit();
        }
    }
  MEDfileClose(fid);
  Cerr<<"End of the reading of the joint informations "<<finl;
#endif
}

// renvoie le type trio a partir du type medocoupling : http://docs.salome-platform.org/6/gui/MED/MEDLoader_8cxx.html
Nom type_medcoupling_to_type_geo_trio(const int type_cell, const int isvef, const int axis_type, const bool& cell_from_boundary, const int axi1d)
{
  Nom type_elem;
#ifdef MEDCOUPLING_
  // [ABN] : first make sure the axis type is properly set, this will influence choice of the element
  // Set up correctly bidim_axi or axi variable according to MED file data.
  if (axis_type==MEDCoupling::AX_CYL)
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
  if (axis_type==MEDCoupling::AX_SPHER)
    {
      Cerr << "Spherical coordinates read in the MED file - this is unsupported in TRUST!" << finl;
      Process::exit();
    }

  //
  // At this stage 'axi' and 'bidim_axi' are properly set.
  //
  if (type_cell==INTERP_KERNEL::NORM_QUAD4)
    type_elem = cell_from_boundary ? "QUADRANGLE_3D" : (isvef == 1 ? "Quadrangle" : "Rectangle");
  else if (type_cell==INTERP_KERNEL::NORM_HEXA8)
    type_elem=(isvef == 1 ? "Hexaedre_vef" : "Hexaedre");
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
  if (axi1d && !(type_cell == INTERP_KERNEL::NORM_SEG2 || type_cell == INTERP_KERNEL::NORM_POINT1))
    {
      Cerr<<"Cell type " << type_cell<< " is not supported for 'axi1d' mode." <<finl;
      Process::exit();
    }
  if (Objet_U::bidim_axi && type_cell == INTERP_KERNEL::NORM_QUAD4)
    type_elem = "Rectangle_2D_axi";
  if (Objet_U::axi)
    type_elem += "_axi";

  if (axi1d)
    type_elem += "_axi";
#endif
  return type_elem;
}

} // end anonymous namespace

/*! @brief Simple appel a: Interprete::printOn(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& LireMED::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

/*! @brief Simple appel a: Interprete::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& LireMED::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

Entree& LireMED::interpreter_(Entree& is)
{
#ifndef MED_
  med_non_installe();
#else
  int isvefforce=0;
  int convertAllToPoly=0;
  int isfamilyshort=0;
  Nom nom_dom_trio,nom_mesh("--any--"), nom_fic;

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
      int nfnfgn = 0;
      Param param(que_suis_je());
      param.ajouter_flag("convertAllToPoly", &convertAllToPoly);       // XD_ADD_P flag Option to convert mesh with mixed cells into polyhedral/polygonal cells

      // Awful option just to keep naked family names from MED file. Rarely used, to be removed very soon.
      // In the new syntax, we implicitely assume that the former option 'family_names_from_group_names' is always set.
      param.ajouter_flag("no_family_names_from_group_names", &nfnfgn); // XD_ADD_P flag Awful option just to keep naked family names from MED file. Rarely used, to be removed very soon.

      param.ajouter("domain|domaine", &nom_dom_trio, Param::REQUIRED); // XD_ADD_P ref_domaine Corresponds to the domain name.
      param.ajouter("file|fichier", &nom_fic, Param::REQUIRED);        // XD_ADD_P chaine File (written in the MED format, with extension '.med') containing the mesh

      param.ajouter("mesh|maillage", &nom_mesh);                       // XD_ADD_P chaine Name of the mesh in med file. If not specified, the first mesh will be read.

      EChaine is2(s);
      param.lire_avec_accolades(is2);
      isfamilyshort = nfnfgn ? 0 : 2;  // will be 2 most of the time in the new syntax.
    }
  else // Boooh: old syntax :-(
    interp_old_syntax(is, isvefforce, convertAllToPoly, isfamilyshort, nom_dom_trio, nom_mesh, nom_fic);

  // Strip _0000 if there, and create proper file name
  Nom nom_fic2(nom_fic);
  nom_fic2.prefix(".med");
  if (nom_fic2==nom_fic)
    {
      Cerr<<"Error, the MED file should have .med as extension."<<finl;
      Cerr<<"See the syntax of Read_MED keyword." << finl;
      exit();
    }
  Nom nom_fic3(nom_fic2);
  if (nom_fic3.prefix("_0000") != nom_fic2 )
    {
      nom_fic3+=".med";
      nom_fic=nom_fic3.nom_me(me());
    }
  associer_domaine(nom_dom_trio);
  Domaine& dom=domaine();
  lire_geom(nom_fic,dom,nom_mesh,nom_dom_trio,isvefforce,convertAllToPoly,isfamilyshort);
#endif

  return is;
}


void LireMED::lire_geom(Nom& nom_fic, Domaine& dom, const Nom& nom_dom, const Nom& nom_dom_trio, int isvef, int convertAllToPoly, int isfamilyshort)
{
#ifndef MED_
  med_non_installe();
#else
  ArrsOfInt sommets_joints;
  ArrOfInt tab_pe_voisin;

  Nom type_elem;
  Noms type_face;
  Noms noms_bords;
  DoubleTab sommets2;
  ArrsOfInt familles;
  IntVect Indices_bord;
  IntTabs all_faces_bord;
  IntTab les_elems2;
  int dim = dimension;

#ifdef MEDCOUPLING_
  int axi1d=0;
  if (dom.que_suis_je()=="DomaineAxi1d")
    axi1d = 1;
  // pour verif
  if (dimension==0)
    {
      Cerr << "Dimension is not defined. Check your data file." << finl;
      Process::exit();
    }
  Cerr << "Trying to read the domain " << nom_dom << " into the file " << nom_fic << " in order to affect to domain "
       << nom_dom_trio << "..." << finl;
  Elem_geom type_ele;
  std::string fileName = nom_fic.getString();
  std::string meshName = nom_dom.getString();
  // Check the mesh name is in the file:
  const MCAuto<MEDFileMeshes> data(MEDFileMeshes::New(fileName));
  std::vector< std::string > meshes_names = data->getMeshesNames();
  if (std::find(meshes_names.begin(), meshes_names.end(), meshName) == meshes_names.end())
    {
      if (meshName == "--any--") meshName = meshes_names[0]; //magic name -> we take the first mesh
      else
        {
          Cerr << "Mesh " << nom_dom << " not found in the med file " << nom_fic << " !" << finl;
          Cerr << "List of meshes found:" << finl;
          for(unsigned int i = 0; i<meshes_names.size(); i++)
            Cerr << meshes_names[i] << finl;
          Process::exit(-1);
        }
    }
  const MEDFileMesh* mfmesh = data->getMeshWithName(meshName);
  const MEDFileUMesh* file = 0;
  if (!(file=dynamic_cast<const MEDFileUMesh*>(mfmesh)))
    {
      Cerr << "Mesh " << nom_dom << " does not have the proper type in the med file " << nom_fic << "!" << finl;
      Process::exit(-1);
    }

  MEDCoupling::MEDCouplingAxisType axis_type = file->getAxisType();
  std::vector<int> nel = file->getNonEmptyLevels();
  if (nel.size() < 1)
    {
      Cerr << "Error! Expecting a multi level mesh named " << nom_fic << " in the " << nom_dom << " file." << finl;
      Process::exit();
    }
  // Some checks:
  assert(nel[0] == 0);
  // Get the volume mesh:
  MCAuto<MEDCouplingUMesh> mesh(file->getMeshAtLevel(nel[0])); // ToDo can not make it const because of ArrOfInt
  dim = mesh->getSpaceDimension();
  Cerr << "Detecting a " << (int)mesh->getMeshDimension() << "D mesh in " << dim << "D space." << finl;
  if (dim != dimension)
    {
      Cerr << "The mesh space dimension may be higher than the computation space dimension" << finl;
      Cerr << "as the algorithm will try to detect the useless direction in the mesh." << finl;
      //assert(dim == dimension);
    }

  // Desormais l'option permet de convertir tout type de maillage vers des polyedres:
  if (convertAllToPoly)
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "Conversion to polyedrons and polygons..." << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      mesh->convertAllToPoly(); // Conversion maillage volumique
    }

  // Get the nodes: size and fill sommets2:
  int nnodes = mesh->getNumberOfNodes();
  const double *coord = mesh->getCoords()->begin();
  Cerr << "Detecting " << nnodes << " nodes." << finl;
  sommets2.resize(nnodes, dim);
  memcpy(sommets2.addr(), coord, sommets2.size_array() * sizeof(double));

  // Get cells:
  int ncells = mesh->getNumberOfCells();
  bool cell_from_boundary = false;
  //const int *conn = mesh->getNodalConnectivity()->begin();
  //const int *connIndex = mesh->getNodalConnectivityIndex()->begin();
  // Use ArrOfInt to benefit from assert:
  ArrOfInt conn, connIndex;
  bool supported_mesh = false;
  while (supported_mesh == false)
    {
      conn.ref_data(mesh->getNodalConnectivity()->getPointer(), mesh->getNodalConnectivity()->getNbOfElems());
      connIndex.ref_data(mesh->getNodalConnectivityIndex()->getPointer(),
                         mesh->getNodalConnectivityIndex()->getNbOfElems());

      int mesh_type_cell = conn[connIndex[0]];
      type_elem = type_medcoupling_to_type_geo_trio(mesh_type_cell, isvef, axis_type, cell_from_boundary, axi1d);
      type_ele.typer(type_elem);
      // Detect a mesh with different cells (not supported):
      for (int i = 0; i < ncells; i++)
        {
          int type_cell = conn[connIndex[i]];
          if (type_cell != mesh_type_cell)
            {
              Cerr << "Elements of kind " << type_elem << " has already been read" << finl;
              Cerr << "New elements of kind "
                   << type_medcoupling_to_type_geo_trio(type_cell, isvef, axis_type, cell_from_boundary, axi1d);
              if (!convertAllToPoly)
                {
                  Cerr << " are not read." << finl;
                  Cerr << "TRUST does not support different element types for the mesh." << finl;
                  Cerr << "Either you remesh your domain with a single element type," << finl;
                  Cerr << "or you convert your cells to polyedrons and polygons by inserting an option in your command line:" << finl;
                  Cerr << "Read_MED convertAllToPoly ... " << finl;
                  Cerr << "After that, you should use a discretization supporting polyedrons !" << finl;
                  Process::exit();
                }
              Cerr << finl;
              break;
            }
          if (i==ncells-1) supported_mesh = true;
        }
    }
  // Stockage du mesh au niveau du domaine, utile pour:
  // Champ_Fonc_MED plus rapide (maillage non relu)
  // Futurs developpements avec MEDCoupling
  dom.setUMesh(mesh);

  // Fill les_elem2 : Different treatment according type_elem:
  if (sub_type(Polyedre, type_ele.valeur()))
    {
      int marker = 0;
      int conn_size = mesh->getNodalConnectivity()->getNbOfElems();
      for (int i = 0; i < conn_size; i++)
        if (conn[i]<0) marker++;
      int NumberOfNodes = conn_size - ncells - marker;
      int nfaces = ncells + marker;
      ArrOfInt Nodes(NumberOfNodes), FacesIndex(nfaces+1), PolyhedronIndex(ncells+1);
      int face=0;
      int node=0;
      for (int i = 0; i < ncells; i++)
        {
          PolyhedronIndex[i] = face; // Index des polyedres

          int index = connIndex[i] + 1;
          int nb_som = connIndex[i + 1] - index;
          for (int j = 0; j < nb_som; j++)
            {
              if (j==0 || conn[index + j]<0)
                FacesIndex[face++] = node; // Index des faces:
              if (conn[index + j]>=0)
                Nodes[node++] = conn[index + j]; // Index local des sommets de la face
            }
        }
      FacesIndex[nfaces] = node;
      PolyhedronIndex[ncells] = face;
      ref_cast(Polyedre,type_ele.valeur()).affecte_connectivite_numero_global(Nodes, FacesIndex, PolyhedronIndex, les_elems2);
    }
  else if (sub_type(Polygone, type_ele.valeur()))
    {
      int conn_size = mesh->getNodalConnectivity()->getNbOfElems();
      int FacesIndexSize = conn_size - ncells;
      ArrOfInt FacesIndex(FacesIndexSize), PolygonIndex(ncells+1);
      int face=0;
      for (int i = 0; i < ncells; i++)
        {
          PolygonIndex[i] = face;   // Index des polygones

          int index = connIndex[i] + 1;
          int nb_som = connIndex[i + 1] - index;
          for (int j = 0; j < nb_som; j++)
            FacesIndex[face++] = conn[index + j];
        }
      PolygonIndex[ncells] = face;
      ref_cast(Polygone,type_ele.valeur()).affecte_connectivite_numero_global(FacesIndex, PolygonIndex, les_elems2);
    }
  else // Tous les autres types
    {
      for (int i = 0; i < ncells; i++)
        {
          int index = connIndex[i] + 1;
          int nb_som = connIndex[i + 1] - index;
          if (i==0) les_elems2.resize(ncells, nb_som); // Size les_elems2
          for (int j = 0; j < nb_som; j++)
            les_elems2(i, j) = conn[index + j];
        }
    }
  les_elems2+=1;  // +1 cause C++ -> Fortran


  // Detect Sub-domains (based on group of volumes);
  unsigned nb_volume_groups = (unsigned)file->getGroupsOnSpecifiedLev(0).size();
  if (nb_volume_groups>0 && Process::je_suis_maitre() && nom_dom_trio!=Nom())
    {
      SFichier jdd_seq(nom_dom_trio + "_ssz.geo");
      SFichier jdd_par(nom_dom_trio + "_ssz_par.geo");
      std::vector<std::string> groups = file->getGroupsOnSpecifiedLev(0);

      int size = nb_volume_groups;
      for (int i = 0; i < size; i++)
        {
          MCAuto<DataArrayInt> ids(file->getGroupArr(0, groups[i], false));
          int nb_elems = (int) ids->getNbOfElems();
          const Nom& nom_sous_domaine = groups[i]; // We take the name of the group for the subzone name
          Cerr << "Detecting a sub-zone (group name="<< nom_sous_domaine << ") with " << nb_elems << " cells." << finl;
          if (nb_elems > 0)
            {
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
              for (int j = 0; j < nb_elems; j++)
                {
                  int elem = ids->getIJ(j, 0);
                  f_ssz << elem << " ";
                }
              f_ssz << finl;
            }
        }
    }

  // Detect boundary meshes:
  if (nel.size() > 1)
    {
      // Size the arrays;
      int nb_type_face = type_ele.nb_type_face();
      familles.dimensionner(nb_type_face);
      all_faces_bord.dimensionner(nb_type_face);
      type_face.dimensionner(nb_type_face);
      for (int i = 0; i < nb_type_face; i++)
        type_face[i] = "";

      // Get boundary mesh:
      assert(nel[1] == -1);
      MCAuto<MEDCouplingUMesh> boundary_mesh(file->getMeshAtLevel(nel[1])); // ToDo can not make it const because of ArrayOfInt
      if (dimension==3 && convertAllToPoly) boundary_mesh->convertAllToPoly(); // Conversion maillage frontiere
      int nfaces = boundary_mesh->getNumberOfCells();
      cell_from_boundary = true;
      //conn = boundary_mesh->getNodalConnectivity()->begin();
      //connIndex = boundary_mesh->getNodalConnectivityIndex()->begin();
      // Use ArrOfInt to benefit from assert:
      conn.ref_data(boundary_mesh->getNodalConnectivity()->getPointer(), boundary_mesh->getNodalConnectivity()->getNbOfElems());
      connIndex.ref_data(boundary_mesh->getNodalConnectivityIndex()->getPointer(), boundary_mesh->getNodalConnectivityIndex()->getNbOfElems());
      int tp = 0;
      int type_cell = -1;
      int max_som_face = 0;
      int nface = 0;
      // First loop to size arrays:
      for (int i = 0; i < nfaces; i++)
        {
          nface++;
          // Detect max number of nodes per face:
          int nb_som_face = connIndex[i + 1] - connIndex[i] - 1;
          if (nb_som_face > max_som_face) max_som_face = nb_som_face;
          // Detect new type of face:
          if (i == nfaces - 1 || conn[connIndex[i]] != conn[connIndex[i + 1]])
            {
              type_cell = conn[connIndex[i]];
              Nom type_face_lu = type_medcoupling_to_type_geo_trio(type_cell, isvef, axis_type, cell_from_boundary, axi1d);
              if (tp>=type_face.size())
                {
                  Cerr << "Error, it does not support another face type: " << type_face_lu << finl;
                  Process::exit();
                }
              type_face[tp] = type_face_lu;
              familles[tp].resize_array(nface);
              all_faces_bord[tp].resize(nface, max_som_face);
              all_faces_bord[tp] = 0;
              Cerr << "Detecting " << nface << " faces (" << type_face[tp] << ")." << finl;
              max_som_face = 0;
              nface = 0;
              tp++;
            }
        }
      assert(tp == nb_type_face);
      // Second loop to fill the connectivity
      tp = 0;
      nface = 0;
      for (int i = 0; i < nfaces; i++)
        {
          int index = connIndex[i] + 1;
          int nb_som = connIndex[i + 1] - index;
          for (int j = 0; j < nb_som; j++)
            all_faces_bord[tp](nface, j) = conn[index + j] + 1; // +1 cause C++ -> Fortran
          nface++;
          // Next type of face ?
          if (i == nfaces - 1 || conn[connIndex[i]] != conn[connIndex[i + 1]])
            {
              nface = 0;
              tp++;
            }
        }
      // Pour debug, lecture des groupes
      bool provisoire = true;
      if (provisoire)
        {
          Cerr << "Reading groups at level -1:" << finl;
          std::vector<std::string> groups = file->getGroupsOnSpecifiedLev(-1);
          size_t size = groups.size();
          for (size_t i=0; i<size; i++)
            {
              MCAuto<DataArrayInt> ids(file->getGroupArr(-1, groups[i], false));
              int nb_faces = (int) ids->getNbOfElems();
              size_t nb_families = file->getFamiliesIdsOnGroup(groups[i]).size();
              Cerr << "group_name=" << groups[i] << " with " << nb_faces << " faces on ";
              Cerr << nb_families << " families (";
              for (size_t j=0; j<nb_families; j++)
                Cerr << file->getFamiliesIdsOnGroup(groups[i])[j] << " ";
              Cerr << ")" << finl;
            }
          Cerr << "Reading families at level -1:" << finl;
          std::vector<std::string> families = file->getFamiliesNames();
          size = families.size();
          for (size_t i = 0; i < size; i++)
            {
              MCAuto<DataArrayInt> ids(file->getFamilyArr(-1 /* faces */, families[i], false));
              int nb_faces = (int) ids->getNbOfElems();
              size_t nb_groups = file->getGroupsOnFamily(families[i]).size();
              int family_id = file->getFamilyId(families[i]);
              Cerr << "family_name=" << families[i] << " (family id=" << family_id << ") with " << nb_faces << " faces on ";
              Cerr << nb_groups << " groups (";
              for (size_t j=0; j<nb_groups; j++)
                Cerr << file->getGroupsOnFamily(families[i])[j] << " ";
              Cerr << ")" << finl;
            }
        }
      Cerr << "Reading boundaries:" << finl;
      // Lecture des familles
      std::vector<std::string> families = file->getFamiliesNames();
      size_t size = families.size();
      for (size_t i = 0; i < size; i++)
        {
          MCAuto<DataArrayInt> ids(file->getFamilyArr(-1 /* faces */, families[i], false));
          int nb_faces = (int) ids->getNbOfElems();
          std::vector<std::string> groups = file->getGroupsOnFamily(families[i]);
          if (nb_faces > 0)
            {
              int family_id = file->getFamilyId(families[i]);
              Nom nom_bord="";
              if (isfamilyshort == 0) // Par defaut, boundary name = family name
                nom_bord = families[i];
              else if (isfamilyshort == 1) // Suppress FAM_*_ from family name
                {
                  Nom family_name = families[i];
                  Nom tmp="FAM_";
                  tmp+=(Nom)family_id;
                  tmp+="_";
                  nom_bord = family_name.suffix(tmp);
                }
              else
                {
                  // Cherche le nom du groupe
                  if (groups.size()==1)
                    nom_bord = groups[0];
                  else
                    for (unsigned long k=0; k<groups.size(); k++)
                      {
                        size_t nb_families = file->getFamiliesIdsOnGroup(groups[k]).size();
                        if (nb_families==1) nom_bord = groups[k];
                      }
                }
              if (nom_bord!="")
                {
                  Cerr << "Detecting a boundary named " << nom_bord << " (family id=" << family_id << ") with "
                       << nb_faces << " faces." << finl;
                  noms_bords.add(nom_bord);
                  for (int j = 0; j < nb_faces; j++)
                    {
                      int face = ids->getIJ(j, 0);
                      // Put the face on the good family
                      tp = 0;
                      int face_famille = face;
                      while (face_famille >= familles[tp].size_array())
                        {
                          face_famille -= familles[tp].size_array();
                          tp++;
                        }
                      familles[tp][face_famille] = family_id;
                    }
                  int nb_bords = noms_bords.size();
                  Indices_bord.resize(nb_bords);
                  Indices_bord[nb_bords - 1] = family_id;
                }
            }
        }
      if (noms_bords.size()==0)
        {
          Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
          Cerr << "Warning: no boundary detected for the mesh." << finl;
          Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
        }
    }
#endif // MEDCOUPLING_

  renum_conn(les_elems2,type_elem,-1);
  for (int j=0; j<type_face.size(); j++)
    renum_conn(all_faces_bord[j],type_face[j],-1);

  Scatter::uninit_sequential_domain(dom);

  DoubleTab& sommets=dom.les_sommets();
  // affectation des sommets
  if (dim!=dimension)
    {
      Cerr<<"One tries to read a meshing written in "<<dim<<"D in "<<dimension<<"D "<<finl;
      // determination de la direction inutile
      int nbsom=sommets2.dimension(0);
      int dirinut=-1;
      const double epsilon = Objet_U::precision_geom;
      for (int dir=0; dir<dim; dir++)
        {
          int trouve=1;
          double val1=sommets2(0,dir);
          for (int i=0; i<nbsom; i++)
            if (std::fabs(val1-sommets2(i,dir))>epsilon)
              {
                trouve=0;
                Cerr<<val1 << " "<<sommets2(i,dir)<<finl;
                break;
              }
          if (trouve==1)
            {
              dirinut=dir;
              break;
            }
        }
      if (dirinut==-1)
        {
          Cerr<<"No useless direction "<<finl;
          exit();
        }
      Cerr<<"useless direction "<<dirinut<<finl<<finl;

      sommets.resize(nbsom,dimension);
      int d=0;
      for (int dir=0; dir<dim; dir++)
        if (dir!=dirinut)
          {
            for (int i=0; i<nbsom; i++)
              sommets(i,d)=sommets2(i,dir);
            d++;
          }
    }
  else
    sommets=sommets2;

  //
  // typage des elts de  le domaine et remplissage des elts
  // Avant de typer on regarde si il ne faut pas transormer les hexa en Hexa_vef
  int err=verifier_modifier_type_elem(type_elem,les_elems2,sommets);
  if (err!=0) exit();

  dom.type_elem()=type_ele;
  // si on a modifier_type_elem
  if (type_ele.valeur().que_suis_je()!=type_elem)
    dom.typer(type_elem);
  dom.type_elem().associer_domaine(dom);
  IntTab& les_elems=dom.les_elems();
  // on revient a une numerotation c
  //les_elems2-=1;
  les_elems=les_elems2;
  int nbord=noms_bords.size();

  // on revient a une numerotation c
  //all_faces_bord-=1;
  // on remet dans le meme sens ne sert a rien mais permet de debugger
  // EcrMed LireMed
  ArrOfInt cor2(Indices_bord);
  {
    ArrOfInt cor(Indices_bord);

    for (int jj=0; jj<nbord; jj++) cor2[jj]=jj;
    for (int kk=0; kk<nbord; kk++)
      {
        int imin=cor[kk];
        int pos=kk;
        for (int ll=kk+1; ll<nbord; ll++)
          {
            if (cor[ll]<imin)
              {
                pos=ll;
                imin=cor[ll];
              }
          }
        cor[pos]=cor[kk];
        cor[kk]=imin;
        int pp=cor2[kk];
        cor2[kk]=cor2[pos];
        cor2[pos]=pp;
      }
  }
  ArrOfInt nb_t(nbord);
  Bords& faces_bord=dom.faces_bord();
  faces_bord.vide();
  Raccords& faces_raccord=dom.faces_raccord();
  faces_raccord.vide();
  Joints& faces_joint=dom.faces_joint();
  faces_joint.vide();

  Noms noms_des_joints;
  recuperer_info_des_joints(noms_des_joints,nom_fic,nom_dom,sommets_joints,tab_pe_voisin);

  //  Cerr<<" FACES BORD VIDE ?"<< faces_bord<<finl;
  //    for (int ib2=0;ib2<nbord;ib2++)
  for (int typef=0; typef<type_face.size(); typef++)
    if (type_face[typef]!=Nom())
      for (int ib2=nbord-1; ib2>-1; ib2--)
        {
          IntTab& all_faces_bord_=all_faces_bord[typef];
          int nfacebord=all_faces_bord_.dimension(0);
          int ib=cor2[ib2];
          int indice_bord=Indices_bord[ib];
          if (indice_bord>=10000) continue;
          if (noms_bords[ib]=="elems") continue;
          Bord bordprov_;
          Raccord raccprov;
          Joint jointprov;
          int israccord=0;
          int isjoint=0;
          if (noms_bords[ib].debute_par("type_raccord_"))
            {
              israccord=1;
              noms_bords[ib].suffix("type_raccord_");
              raccprov.typer("Raccord_local_homogene");
            }

          int numero_joint=noms_des_joints.search(noms_bords[ib]);
          if (numero_joint>-1)
            {
              Cerr<<noms_bords[ib]<<" is considered as a joint "<<finl;
              isjoint=1;
            }
          //modif_nom_bord(noms_bords[ib]);
          // on recupere la frontiere  .... que ce soit un Bord,Raccord,ou Joint
          Frontiere& bordprov=(isjoint?jointprov:(israccord?ref_cast(Frontiere,raccprov.valeur()):ref_cast(Frontiere,bordprov_)));

          bordprov.nommer(noms_bords[ib]);
          bordprov.typer_faces(type_face[typef]);
          int nb=0;
          int nsomfa=all_faces_bord_.dimension(1);
          IntTab sommprov(nfacebord,nsomfa);
          for (int j=0; j<nfacebord; j++)
            {
              if (familles[typef][j]==indice_bord)
                {

                  for (int k=0; k<nsomfa; k++)
                    sommprov(nb,k)=all_faces_bord_(j,k);
                  nb++;
                }
            }
          Faces& facesi=bordprov.faces();
          IntTab& sommetsfaces=facesi.les_sommets();
          sommetsfaces.resize(nb,nsomfa);
          for (int jj=0; jj<nb; jj++)
            for (int k=0; k<nsomfa; k++)
              sommetsfaces(jj,k)=sommprov(jj,k);
          IntTab& facesv=facesi.voisins();
          facesv.resize(nb,2);
          facesv=-1;

          //if ((nb!=0)&&(indice_bord!=0))
          int vide_0D_a_ecrire=0;
          if (typef==type_face.size()-1)
            {
              if ((nb_t[ib]==0)&&(nb==0))
                {
                  bordprov.typer_faces("vide_0D");
                  vide_0D_a_ecrire=1;
                }
            }
          else if (nb>nb_t[ib]) nb_t[ib]=nb;
          // ne marche pas ajoute la famille elem
          if ((indice_bord!=0)&&((nb>0)||(vide_0D_a_ecrire)))
            {
              if (isjoint)
                {
                  int PE_voisin=tab_pe_voisin[numero_joint];
                  int epaisseur=1;
                  ArrOfInt& sommets_joint=sommets_joints[numero_joint];
                  jointprov.affecte_PEvoisin(PE_voisin);
                  jointprov.affecte_epaisseur(epaisseur);
                  ArrOfInt& sommets_du_joint=jointprov.set_joint_item(Joint::SOMMET).set_items_communs();
                  sommets_du_joint=sommets_joint;
                  faces_joint.add(jointprov);
                }
              else if (israccord)
                faces_raccord.add(raccprov);
              else
                faces_bord.add(bordprov_);
            }
        }
//  for (int j=0; j<nbord; j++)
//    {
//      if (Indices_bord(j)==-1000)
//        {
//          // Cerr<<" j "<<j <<noms_bords[j]<<finl;
//          Nom nom_domaine=noms_bords[j];
//          if (nom_domaine.debute_par("cpy_"))
//            nom_domaine.suffix("cpy_");
//          Cerr<<" the domaine is named "<<nom_domaine<<finl;
//          dom.nommer(nom_domaine);
//        }
//    }
  faces_bord.associer_domaine(dom);
  faces_raccord.associer_domaine(dom);
  faces_joint.associer_domaine(dom);
  dom.type_elem().associer_domaine(dom);
  dom.fixer_premieres_faces_frontiere();
  int nbfr=dom.nb_front_Cl();
  for (int fr=0; fr<nbfr; fr++)
    {
      dom.frontiere(fr).faces().associer_domaine(dom);
      if ( dom.frontiere(fr).faces().type_face()!=Faces::vide_0D)
        dom.frontiere(fr).faces().reordonner();
    }
  //  GF au moins en polyedre il faut reordonner
  //  il faut certainement le faire tout le temps
  //  non c'est trop long
  if (sub_type(Polyedre,dom.type_elem().valeur())|| (type_elem=="Rectangle")|| (type_elem=="Hexaedre"))
    dom.reordonner();

  if (nproc()==1)
    NettoieNoeuds::nettoie(dom);

  // +1 car med numero de 1 a n
  // medecrgeom(nom_fic,nom_dom,dimension,sommets,type_elem,les_elems2,type_face,all_faces_bord,familles,noms_bords);
  Scatter::init_sequential_domain(dom);
  if (isfamilyshort==2)
    {
      // il est possible que l'on est cree plusieurs bord de meme nom
      RegroupeBord r;
      r.rassemble_bords(dom);
    }
  Cerr<<"Reading of the domain ended"<<finl;
#endif
}
