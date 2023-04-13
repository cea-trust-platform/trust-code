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

#include <EcrMED.h>
#include <Domaine.h>
#include <med++.h>
#include <Domaine_VF.h>
#include <TRUSTTabs.h>
#include <Char_ptr.h>
#include <medcoupling++.h>
#include <ctime>
#include <trust_med_utils.h>
#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#include <MEDFileMesh.hxx>
#include <MEDCouplingFieldDouble.hxx>
#pragma GCC diagnostic ignored "-Wreorder"
#include <MEDFileField.hxx>
using MEDCoupling::MCAuto;
using MEDCoupling::MEDCouplingUMesh;
using MEDCoupling::DataArrayDouble;
using MEDCoupling::DataArrayInt;
using MEDCoupling::MEDFileField1TS;
using MEDCoupling::MEDFileUMesh;
using MEDCoupling::MEDCouplingFieldDouble;
using MEDCoupling::GetAllFieldNames;
using MEDCoupling::GetAllFieldIterations;
using MEDCoupling::MEDFileMesh;
#endif

Implemente_instanciable(EcrMED,"Ecrire_MED",Interprete);

// Anonymous namespace for local functions:
namespace
{

/*! @brief Loop on bords,raccords,joints
 */
const Frontiere& mes_faces_fr(const Domaine& domaine, int i)
{
  int nb_std = domaine.nb_front_Cl();
  return i<nb_std ? domaine.frontiere(i) : domaine.joint(i-nb_std);
}

} // namespace

/*! @brief Simple appel a: Interprete::printOn(Sortie&)
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& EcrMED::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

/*! @brief Simple appel a: Interprete::readOn(Entree&)
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le flot d'entree modifie
 */
Entree& EcrMED::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

EcrMED::EcrMED(const Nom& file_name, const Domaine& dom):
  major_mode_(false),
  nom_fichier_(file_name),
  dom_(dom),
  mcumesh_(nullptr)
{
}

void EcrMED::set_file_name_and_dom(const Nom& file_name, const Domaine& dom)
{
  nom_fichier_ = file_name;
  dom_ = dom;
}

// XD writemed interprete ecrire_med -1 Write a domain to MED format into a file.
// XD attr nom_dom ref_domaine nom_dom 0 Name of domain.
// XD attr file chaine file 0 Name of file.
Entree& EcrMED::interpreter(Entree& is)
{
  Cerr<<"syntax : EcrMED [ append ] nom_dom nom_fic "<<finl;
  bool append=false;
  Nom nom_dom;
  is >> nom_dom ;
  Motcle app("append");
  if (app==nom_dom)
    {
      append=true;
      is >> nom_dom;
      Cerr<<" Adding "<<nom_dom<<finl;
    }
  is >> nom_fichier_;
  if(! sub_type(Domaine, objet(nom_dom)))
    {
      Cerr << nom_dom << " type is " << objet(nom_dom).que_suis_je() << finl;
      Cerr << "Only Domaine type objects can be meshed" << finl;
      exit();
    }
  dom_ = ref_cast(Domaine, objet(nom_dom));
  ecrire_domaine(append);
  return is;
}

#ifndef MED_
void EcrMED::ecrire_champ(const Nom& type,const Nom& nom_cha1,const DoubleTab& val,const Noms& unite, const Noms& noms_compo, const Nom& type_elem,double time)
{
  med_non_installe();
}
void EcrMED::ecrire_domaine(const Nom& nom_dom,int mode)
{
  med_non_installe();
}
#else

/*! @brief A partir d'un domaine extrait le type de face, la connectivite des faces de bords, le nom des bords et cree les familles
 */
void EcrMED::creer_all_faces_bord_OLD(Noms& type_face,IntTabs& all_faces_bord, Noms& noms_bords,ArrsOfInt& familles)
{
  const Domaine& dom = dom_.valeur();
  int nb_type_face=dom.type_elem().nb_type_face();
  type_face.dimensionner(nb_type_face);
  all_faces_bord.dimensionner(nb_type_face);
  familles.dimensionner(nb_type_face);


  int nb_faces_bord=dom.nb_faces_bord()+dom.nb_faces_raccord()+dom.nb_faces_int()+dom.nb_faces_joint();
  int nb_bords=dom.nb_front_Cl()+dom.nb_joints();
  if (nb_bords==0)
    {
      // on n'a pas les bords
      for (int j=0; j<nb_type_face; j++)
        all_faces_bord[j].resize(0,0);
      return;
    }
  // on cherche un bord non vide pour recuperer le type de faces
  int nb_type_trouve=0;
  for (int ii=0; ii<nb_bords; ii++)
    {
      if (::mes_faces_fr(dom,ii).faces().nb_faces()!=0)
        {
          Nom type_face_b;
          type_face_b=::mes_faces_fr(dom,ii).faces().type(::mes_faces_fr(dom,ii).faces().type_face());
          int existe=0;
          for (int j=0; j<nb_type_trouve; j++)
            if (type_face_b==type_face[j])
              existe=1;
          if (!existe)
            {
              type_face[nb_type_trouve]=type_face_b;
              nb_type_trouve++;
            }
        }
    }
  if (nb_type_trouve!=nb_type_face)
    {
      Cerr<<"all boundaries type have not been found" <<finl;
    }
  //Cerr<<"type_face "<<type_face<<finl;

  int nb_som_face_max=dom.type_elem().nb_som_face(0);
  for (int j=0; j<nb_type_face; j++)
    {
      all_faces_bord[j].resize(nb_faces_bord, nb_som_face_max);
      all_faces_bord[j]=0;
      familles[j].resize_array(nb_faces_bord);
    }
  noms_bords.dimensionner(nb_bords);

  // enfin on recupere le nom des faces de bord et leur connectivite
  ArrOfInt cpt(nb_type_face);
  ArrOfInt nb_som(nb_type_face);
  for(int i=0; i<nb_bords; i++)
    {
      if (sub_type(Raccord_base,::mes_faces_fr(dom,i)))
        {
          noms_bords[i]="type_raccord_";
          noms_bords[i]+=::mes_faces_fr(dom,i).le_nom();
        }
      else
        noms_bords[i]=::mes_faces_fr(dom,i).le_nom();
      const IntTab& les_sommets_des_faces=::mes_faces_fr(dom,i).les_sommets_des_faces();
      int nb_fac=les_sommets_des_faces.dimension(0);
      // on cherche de quel type est le bord
      int ref=0;
      if (nb_fac>0)
        {
          Nom type_face_b=::mes_faces_fr(dom,i).faces().type(::mes_faces_fr(dom,i).faces().type_face());
          ref=type_face.search(type_face_b);
        }
      for(int j=0; j<nb_fac; j++)
        {
          int nb_som_face=les_sommets_des_faces.dimension(1);
          familles[ref][cpt[ref]]=-(i+1);
          for(int k=0; k<nb_som_face; k++)
            all_faces_bord[ref](cpt[ref], k)=les_sommets_des_faces(j, k);
          cpt[ref]++;
          nb_som[ref]=nb_som_face;
        }
    }
  for (int j=0; j<nb_type_face; j++)
    {
      familles[j].resize_array(cpt[j]);
      if (nb_type_face>1)
        {
          IntTab prov=all_faces_bord[j];
          int nb_som_face=nb_som[j];
          int nb_faces=cpt[j];
          IntTab& all_faces_bord_=all_faces_bord[j];
          all_faces_bord_.resize(nb_faces,nb_som_face);
          for (int f=0; f<nb_faces; f++)
            for (int c=0; c<nb_som_face; c++)
              all_faces_bord_(f,c)=prov(f,c);
        }
    }
}

/*! @brief For each bord get starting and ending index (by construction in TRUST, face indices at the
 * boundary are grouped)
 */
void EcrMED::get_bords_infos(Noms& noms_bords_and_jnts, ArrOfInt& sz_bords_and_jnts) const
{
  const Domaine& dom = dom_.valeur();
  int nb_bords = dom.nb_front_Cl(), nb_jnts = dom.nb_joints();

  noms_bords_and_jnts.dimensionner(nb_bords+nb_jnts);
  sz_bords_and_jnts.resize_array(nb_bords+nb_jnts);

  // Get border names and nb of faces:
  for(int i=0; i<nb_bords; i++)
    {
      const Frontiere& front = dom.frontiere(i);
      if (sub_type(Raccord_base,front))
        {
          noms_bords_and_jnts[i] = "type_raccord_";
          noms_bords_and_jnts[i] += front.le_nom();
        }
      else
        noms_bords_and_jnts[i] = front.le_nom();
      sz_bords_and_jnts[i] = front.nb_faces();
    }
  for(int i=0; i<nb_jnts; i++)
    {
      const Joint& jnt = dom.joint(i);
      noms_bords_and_jnts[nb_bords+i] = jnt.le_nom();
      sz_bords_and_jnts[nb_bords+i] = jnt.nb_faces();
    }
}

void EcrMED::ecrire_domaine(bool append)
{
  REF(Domaine_dis_base) domaine_dis_base; // Pas de domaine discretisee
  ecrire_domaine_dis(domaine_dis_base, append);
}

/*! @brief Fill face and groups in the MEDCoupling object
 *
 * Two cases:
 * - either we want the full face mesh, in which case we garantee that the face numbering will be identical to TRUST in the
 * final MED file
 * - or, we only write boundary faces. In this case we can still preserve face numbering for all 'classical' borders (i.e. not
 * joints) since by construction TRUST places those faces first, but faces of **joints** are renumbered.
 */
void EcrMED::fill_faces_and_boundaries(const REF(Domaine_dis_base)& domaine_dis_base)
{
  // Fill arrays all_faces_bords and noms_bords
  Noms noms_bords_and_jnts;
  IntTab sz_bords_and_jnts;
  get_bords_infos(noms_bords_and_jnts, sz_bords_and_jnts);

  int nfaces = 0;
  bool full_face_mesh = domaine_dis_base.non_nul() && ref_cast(Domaine_VF, domaine_dis_base.valeur()).elem_faces().size()>0;
  // If the domain has faces (eg:domain computation), we can create a face mesh (all faces, incl internal ones), else only a boundary mesh
  if (full_face_mesh)
    {
      // Faces mesh - by construction (see build_mc_face_mesh) it will have the same face numbering as in TRUST.
      dom_->build_mc_face_mesh(domaine_dis_base.valeur());
      const MEDCouplingUMesh *faces_mesh = dom_->get_mc_face_mesh();
      MCAuto<MEDCouplingUMesh> face_mesh2 = faces_mesh->clone(false); // perform a super light copy, no data array copied
      face_mesh2->setName(mfumesh_->getName());  // names have to be aligned ...
      mfumesh_->setMeshAtLevel(-1, face_mesh2, false);
      nfaces = faces_mesh->getNumberOfCells();
    }
  else // Boundary mesh only
    {
      MCAuto<MEDCouplingUMesh> boundary_mesh(MEDCouplingUMesh::New(mcumesh_->getName(), mesh_dimension_ - 1));
      boundary_mesh->setCoords(mcumesh_->getCoords());

      for (int j = 0; j < noms_bords_and_jnts.size(); j++) nfaces += sz_bords_and_jnts(j);
      boundary_mesh->allocateCells(nfaces);

      for (int b=0; b < noms_bords_and_jnts.size(); b++)
        {
          const Frontiere& front = ::mes_faces_fr(dom_.valeur(),b);
          const IntTab& som_fac = front.les_sommets_des_faces();
          int nb_fac = som_fac.dimension(0), nb_som_fac = som_fac.dimension(1);
          const Nom& typ_f_trust = front.faces().type(front.faces().type_face());

          int boundary_mesh_dimension = -1;
          INTERP_KERNEL::NormalizedCellType typ_fac_mc = type_geo_trio_to_type_medcoupling(typ_f_trust, boundary_mesh_dimension);
          assert(boundary_mesh_dimension == mesh_dimension_ - 1);

          // Convert connectivity from TRUST to MED:
          IntTab som_fac_cpy = som_fac; // a deep copy since next method modify this in place:
          conn_trust_to_med(som_fac_cpy, typ_f_trust, true);
          // Insert it in the MC mesh, **always as a polygon/segment**:
          for (int face_idx=0; face_idx < nb_fac; face_idx++)
            {
              int nvertices = nb_som_fac;
              for (int k = 0; k < nb_som_fac; k++)
                if (som_fac_cpy(face_idx, k) < 0) nvertices--; // Non constant number of vertices (polygons)
              boundary_mesh->insertNextCell(typ_fac_mc, nvertices, som_fac_cpy.addr() + face_idx*nb_som_fac);
            }
        }
      mfumesh_->setMeshAtLevel(-1, boundary_mesh, false);
    }

  // Register groups
  std::vector<const DataArrayIdType *> grps;
  std::vector<MCAuto<DataArrayIdType>> grps_mem;  // just for memory management -> will ensure proper array deletion when destroyed

  int face_idx = 0, start, end, nb_bords = dom_->nb_front_Cl();
  for (int b=0; b < nb_bords; b++, face_idx=end)  // not joints
    {
      MCAuto<DataArrayIdType> g(DataArrayIdType::New());
      start=face_idx, end=start+sz_bords_and_jnts[b];
      Cerr << " grp " << noms_bords_and_jnts[b] << " " << start << " " << end << finl;

      g->alloc(end-start);
      g->iota(start);
      g->setName(noms_bords_and_jnts[b].getChar());
      grps_mem.push_back(g);
      grps.push_back(g);
    }

  // Joint do not necessary have consecutive face numbers and must be handled separately
  for (int j=0; j < dom_->nb_joints(); j++)
    {
      const Joint& jnt = dom_->joint(j);
      const ArrOfInt& ric = jnt.joint_item(Joint::FACE).items_communs();
      assert(ric.size_array() == jnt.nb_faces());
      MCAuto<DataArrayIdType> g(DataArrayIdType::New());
      g->alloc(ric.size_array());
      mcIdType* gP = g->getPointer();
      if (full_face_mesh) // we can preserve original TRUST numbers
        for (int k=0; k<jnt.nb_faces(); k++) gP[k] = ric(k);
      else                // otherwise just take next available ids:
        for (int k=0; k<jnt.nb_faces(); k++) gP[k] = face_idx++;
      g->setName(jnt.le_nom().getChar());
      grps_mem.push_back(g);
      grps.push_back(g);
    }
  // Save all this:
  mfumesh_->setGroupsAtLevel(-1, grps);
}

/*! @brief Fill the face mesh of the MEDFileUMesh member  mfumesh_
 */
void EcrMED::fill_faces_and_boundaries_OLD(const REF(Domaine_dis_base)& domaine_dis_base)
{
  int ncells = (int)mcumesh_->getNumberOfCells();
  ArrsOfInt familles;

  // Remplit le tableau all_faces_bords ainsi que noms_bords et familles
  IntTabs all_faces_bord;
  Noms type_face;
  Noms noms_bords;
  creer_all_faces_bord_OLD(type_face, all_faces_bord,  noms_bords,familles);
  // connectivite Trio a MED
  for (int j=0; j<type_face.size(); j++)
    conn_trust_to_med(all_faces_bord[j],type_face[j], true);


  // Family for the cells:
  int global_family_id = -1000;
  MCAuto<DataArrayInt> famArr(DataArrayInt::New());
  famArr->alloc(ncells);
  famArr->fillWithValue(global_family_id);
  mfumesh_->setFamilyFieldArr(0, famArr);
  // Name the family and check unicity:
  Nom family_name = noms_bords.search(dom_->le_nom()) != -1 ? "cpy_" : "";
  family_name += dom_->le_nom();
  mfumesh_->addFamily(family_name.getString(), global_family_id);

  int nfaces=-1;
  // If the domain has faces (eg:domain computation), we can create a face mesh (all faces, incl internal ones), else only a boundary mesh
  if (domaine_dis_base.non_nul() && ref_cast(Domaine_VF, domaine_dis_base.valeur()).elem_faces().size()>0)
    {
      // Faces mesh:
      dom_->build_mc_face_mesh(domaine_dis_base.valeur());
      const MEDCouplingUMesh *faces_mesh = dom_->get_mc_face_mesh();
      MCAuto<MEDCouplingUMesh> face_mesh2 = faces_mesh->clone(false); // perform a super light copy, no data array copied
      face_mesh2->setName(mfumesh_->getName());  // names have to be aligned ...
      mfumesh_->setMeshAtLevel(-1, face_mesh2, false);
      nfaces = faces_mesh->getNumberOfCells();
    }
  else
    {
      // Boundary mesh:
      MCAuto<MEDCouplingUMesh> boundary_mesh(MEDCouplingUMesh::New(mcumesh_->getName(), mesh_dimension_ - 1));
      boundary_mesh->setCoords(mcumesh_->getCoords());
      nfaces = 0;
      int nb_type_face = familles.size();
      for (int j = 0; j < nb_type_face; j++)
        nfaces += familles[j].size_array();
      boundary_mesh->allocateCells(nfaces);
      for (int j = 0; j < nb_type_face; j++)
        {
          int size = familles[j].size_array();
          if (size)
            {
              // Converting TRUST to MC boundary cell:
              int boundary_mesh_dimension = -1;
              INTERP_KERNEL::NormalizedCellType type_boundary_cell = type_geo_trio_to_type_medcoupling(type_face[j],
                                                                                                       boundary_mesh_dimension);
              assert(boundary_mesh_dimension == mesh_dimension_ - 1);
              int nverts = all_faces_bord[j].dimension(1);
              for (int i = 0; i < size; i++)
                {
                  int nvertices = nverts;
                  for (int k = 0; k < nverts; k++)
                    if (all_faces_bord[j].addr()[i * nverts + k] < 0)
                      nvertices--; // Some face type has not a constant number of vertices (eg: Polygon)
                  boundary_mesh->insertNextCell(type_boundary_cell, nvertices, all_faces_bord[j].addr() + i * nverts);
                }
            }
        }
      mfumesh_->setMeshAtLevel(-1, boundary_mesh, false);
    }

  // Family (with possible renum of the boundary cells)
  MCAuto<DataArrayInt> family_array(DataArrayInt::New());
  family_array->alloc(nfaces);
  int nb_type_face = familles.size();
  int face = 0;
  for (int j = 0; j < nb_type_face; j++)
    for (int i = 0; i < familles[j].size_array(); i++)
      {
        int family_id = familles[j][i];
        family_array->setIJ(face, 0, family_id);
        face++;
      }
  // Faces internes (faimily_id=0):
  for (; face<nfaces; face++)
    family_array->setIJ(face, 0, 0);
  mfumesh_->setFamilyFieldArr(-1, family_array);
  // Naming family on boundaries:
  for (int i = 0; i < noms_bords.size(); i++)
    {
      int family_id = -(i + 1);
      mfumesh_->addFamily(noms_bords[i].getString(), family_id);
      std::vector<std::string> grps(1);
      grps[0] = noms_bords[i].getString();
      mfumesh_->setGroupsOnFamily(noms_bords[i].getString(), grps);
    }
  // Faces internes:
  std::string name = "faces_internes";
  mfumesh_->addFamily(name, 0);
  std::vector<std::string> grps(1);
  grps[0] = name;
  mfumesh_->setGroupsOnFamily(name, grps);
}

/*! @brief Ecrit le domaine dom dans le fichier nom_fichier_
 *
 * @param append = false nouveau fichier, append = true ajout du domaine dans le fichier
 */
void EcrMED::ecrire_domaine_dis(const REF(Domaine_dis_base)& domaine_dis_base, bool append)
{
  if (Objet_U::dimension==0)
    Process::exit("Dimension is not defined. Check your data file.");
#ifndef MEDCOUPLING_
  med_non_installe(); // actually MEDCoupling ... but will do.
#else

  // Retrieve (or build!) the MEDCouplingUMesh associated with the domain:
  if (dom_->get_mc_mesh() == nullptr)  // was not already filled when the domain was read from MED for example
    {
      Cerr << "EcrMED: Creating a MEDCouplingUMesh object for the domain '" << dom_->le_nom() << "'" << finl;
      dom_->build_mc_mesh();
    }
  else
    Cerr << "EcrMED: Found an existing MEDCouplingUMesh object for the domain '" << dom_->le_nom() << "'" << finl;
  mcumesh_ = dom_->get_mc_mesh();
  mesh_dimension_ = mcumesh_->getMeshDimension();
  MEDCouplingUMesh *mc_no_const = const_cast<MEDCouplingUMesh *>(mcumesh_);  // because of setCoords() and setMeshAtLevel()

  // Prepare final MEDFileUMesh object:
  mfumesh_ = MEDFileUMesh::New();
  mfumesh_->setName(mcumesh_->getName());      // name must be provided
  mfumesh_->setCoords(mc_no_const->getCoords());  // should be the same coord array for all levels (i.e. dom->les_sommets())
  mfumesh_->setMeshAtLevel(0, mc_no_const, false);

  // Check the mesh
#ifndef NDEBUG
  mcumesh_->checkConsistency();
#endif

  // Faces and group of faces representing boundaries:
  char *grp_mode = getenv("TRUST_MED_WITH_GRP");
  int new_mode = 0;  // 0: family only, 1: groups only
  if (grp_mode) new_mode = atoi(grp_mode);

  if (new_mode)
    fill_faces_and_boundaries(domaine_dis_base);
  else
    fill_faces_and_boundaries_OLD(domaine_dis_base);

  // Write:
  int option = (append ? 1 : 2); /* 2: reset file. 1: append, 0: overwrite objects */
  Cerr<<"Writing file '" << nom_fichier_<<"' with mesh name '" << mfumesh_->getName() << "' (append=" << (append ? "true": "false") << ") ..."<<finl;
  if (major_mode_)
    mfumesh_->write40(nom_fichier_.getString(), option);
  else
    mfumesh_->write(nom_fichier_.getString(), option);
#endif
}

/*! @brief Permet d'ecrire le tableau de valeurs val comme un champ dans le fichier med de nom nom_fichier_, avec pour support le domaine de nom nom_dom.
 *
 *   @param type: CHAMPPOINT,CHAMPMAILLE,CHAMPFACES
 *   @param nom_cha1 le nom du champ
 *   @param unite : les unites
 *   @param type_elem le type des elems du domaine
 *   @param time le temps
 */
void EcrMED::ecrire_champ(const Nom& type, const Nom& nom_cha1, const DoubleTab& val, const Noms& unite,
                          const Noms& noms_compo, const Nom& type_elem, double time)
{
  const Nom& nom_dom = dom_->le_nom();
#ifdef MEDCOUPLING_
  // Create MEDCouplingField
  MEDCoupling::TypeOfField field_type;
  if (type == "CHAMPMAILLE")
    field_type = MEDCoupling::ON_CELLS;
  else if (type == "CHAMPPOINT")
    field_type = MEDCoupling::ON_NODES;
  else if (type == "CHAMPFACES")
    field_type = MEDCoupling::ON_CELLS;
  else
    {
      Cerr << "Field type " << type << " is not supported yet." << finl;
      Process::exit();
      return;
    }
  std::string file_name = nom_fichier_.getString();
  std::string field_name = nom_cha1.getString();
  // Get the previous timestep:
  if (timestep_.find(field_name)==timestep_.end())
    {
      int timestep = 0;
      std::vector<std::string> field_names = GetAllFieldNames(file_name);
      if (std::find(field_names.begin(), field_names.end(), field_name) != field_names.end())
        {
          std::vector<std::pair<std::pair<True_int, True_int>, double> > ts = GetAllFieldIterations(file_name, field_name);
          timestep = ts[ts.size() - 1].first.first + 1;
        }
      timestep_.insert({field_name,timestep});
    }
  else
    timestep_.at(field_name)++;
  MCAuto<MEDCouplingFieldDouble> field(MEDCouplingFieldDouble::New(field_type, MEDCoupling::ONE_TIME));
  field->setName(field_name);
  field->setTime(time, timestep_.at(field_name), -1);
  field->setTimeUnit("s");

  // Try to get directly the mesh from the domain:
  if (dom_->get_mc_mesh() != nullptr)
    {
      if (type == "CHAMPFACES")
        field->setMesh(dom_->get_mc_face_mesh());
      else
        field->setMesh(dom_->get_mc_mesh());
    }
  else
    {
      // Get mesh from the file (less optimal but sometime necessary: eg: call from latatoother::interpreter())
      std::string mesh_name = nom_dom.getString();
      if (nom_dom!="PARTICULES")
        {
          const MCAuto<MEDCouplingUMesh> umesh = MEDCoupling::ReadUMeshFromFile(file_name, mesh_name, 0);
          field->setMesh(umesh);
        }
      else
        {
          MEDCoupling::CheckFileForRead(file_name);
          MCAuto<MEDFileMesh> mm(MEDFileMesh::New(file_name,mesh_name));
          MEDFileMesh *mmPtr(mm);
          MEDFileUMesh *mmuPtr=dynamic_cast<MEDFileUMesh *>(mmPtr);
          if(!mmuPtr)
            {
              std::ostringstream oss;
              oss << "ReadUMeshFromFile : With fileName=\""<< file_name << "\", meshName=\""<< mesh_name << "\" exists but it is not an unstructured mesh !";
              throw INTERP_KERNEL::Exception(oss.str());
            }

          const MCAuto<MEDCouplingUMesh> umesh_particles = mmuPtr->getMeshAtLevel(1);
          field->setMesh(umesh_particles);
        }
    }
  // Fill array:
  int size = val.dimension(0);
  if (size>0)
    {
      int nb_comp = val.nb_dim() == 1 ? 1 : val.dimension(1);
      MCAuto<DataArrayDouble> array(DataArrayDouble::New());
      array->useArray(val.addr(), false, MEDCoupling::DeallocType::CPP_DEALLOC, size, nb_comp);
      // Units:

      if (nb_comp > 1)
        for (int i = 0; i < nb_comp; i++)
          array->setInfoOnComponent(i, noms_compo[i].getString() + "[" + unite[i].getString() + "]");
      else
        array->setInfoOnComponent(0, "[" + unite[0].getString() + "]");
      field->setArray(array);
      // Write
      MCAuto<MEDFileField1TS> file(MEDFileField1TS::New());
      file->setFieldNoProfileSBT(field);
      file->write(file_name, 0);
    }
#else
  med_non_installe();
#endif
}

#endif
