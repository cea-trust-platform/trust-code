/****************************************************************************
* Copyright (c) 2025, CEA
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

#include <Ecrire_MED.h>
#include <Domaine.h>
#include <med++.h>
#include <Domaine_VF.h>
#include <TRUSTTabs.h>
#include <Char_ptr.h>
#include <medcoupling++.h>
#include <ctime>
#include <TRUST_2_MED.h>
#include <Sortie_Fichier_base.h>
#include <Synonyme_info.h>

#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#include <MEDFileMesh.hxx>
#include <MEDCouplingFieldDouble.hxx>
#pragma GCC diagnostic ignored "-Wreorder"
#include <MEDFileField.hxx>
using namespace MEDCoupling;
#endif

Implemente_instanciable_32_64(Ecrire_MED_32_64,"Write_MED",Interprete);
Add_synonym(Ecrire_MED,"Ecrire_MED");

// Anonymous namespace for local functions:
namespace
{

/*! @brief Loop on bords,raccords,joints
 */
template <typename _SIZE_>
const Frontiere_32_64<_SIZE_>& mes_faces_fr(const Domaine_32_64<_SIZE_>& domaine, int i)
{
  int nb_std = domaine.nb_front_Cl() + domaine.nb_groupes_faces();
  return i<nb_std ? domaine.frontiere(i) : domaine.joint(i-nb_std);
}

} // namespace
template <typename _SIZE_>
Sortie& Ecrire_MED_32_64<_SIZE_>::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}

template <typename _SIZE_>
Entree& Ecrire_MED_32_64<_SIZE_>::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

template <typename _SIZE_>
Ecrire_MED_32_64<_SIZE_>::Ecrire_MED_32_64(const Nom& file_name, const Domaine_t& dom):
  major_mode_(false),
  dom_(dom)
#ifdef MEDCOUPLING_
  ,  mcumesh_(nullptr)
#endif
{
  nom_fichier_ = Sortie_Fichier_base::root;
  if (nom_fichier_!="") nom_fichier_+="/";
  nom_fichier_ += file_name;
}

template <typename _SIZE_>
void Ecrire_MED_32_64<_SIZE_>::set_file_name_and_dom(const Nom& file_name, const Domaine_t& dom)
{
  nom_fichier_ = Sortie_Fichier_base::root;
  if (nom_fichier_!="") nom_fichier_+="/";
  nom_fichier_ += file_name;
  dom_ = dom;
}

// XD Ecrire_MED_32_64 interprete Write_MED -1 Write a domain to MED format into a file.
// XD attr nom_dom ref_domaine nom_dom 0 Name of domain.
// XD attr file chaine file 0 Name of file.
template <typename _SIZE_>
Entree& Ecrire_MED_32_64<_SIZE_>::interpreter(Entree& is)
{
  Cerr<<"syntax : Write_MED [ append ] nom_dom nom_fic "<<finl;
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
  if(! sub_type(Domaine_t, objet(nom_dom)))
    {
      Cerr << nom_dom << " type is " << objet(nom_dom).que_suis_je() << finl;
      Cerr << "Only Domaine type objects can be meshed" << finl;
      exit();
    }
  dom_ = ref_cast(Domaine_t, objet(nom_dom));
  ecrire_domaine(append);
  return is;
}

#ifndef MED_
template <typename _SIZE_>
void Ecrire_MED_32_64<_SIZE_>::ecrire_champ(const Nom& type,const Nom& nom_cha1,const DoubleTab& val,const Noms& unite, const Noms& noms_compo, const Nom& type_elem,double time)
{
  med_non_installe();
}

template <typename _SIZE_>
void Ecrire_MED_32_64<_SIZE_>::ecrire_domaine(bool m)
{
  med_non_installe();
}

template <typename _SIZE_>
void Ecrire_MED_32_64<_SIZE_>::ecrire_domaine_dis(const OBS_PTR(Domaine_dis_base)& domaine_dis_base, bool append)
{
  med_non_installe();
}
#else

/*! @brief For each bord get starting and ending index (by construction in TRUST, face indices at the
 * boundary are grouped)
 */
template <typename _SIZE_>
void Ecrire_MED_32_64<_SIZE_>::get_bords_infos(Noms& noms_bords_and_jnts, ArrOfInt_t& sz_bords_and_jnts) const
{
  const Domaine_t& dom = dom_.valeur();
  int nb_bords = dom.nb_front_Cl(), nb_faces_int = dom.nb_groupes_faces();

  // [ABN] TODO handle joints properly - they could be written too
  int nb_jnts = dom.nb_joints();
  nb_jnts = 0;

  noms_bords_and_jnts.dimensionner(nb_bords+nb_jnts+nb_faces_int);
  sz_bords_and_jnts.resize_array(nb_bords+nb_jnts+nb_faces_int);

  // Get border names and nb of faces:
  for(int i=0; i<nb_bords + nb_faces_int; i++)
    {
      const Frontiere_32_64<_SIZE_>& front = dom.frontiere(i);
      if (sub_type(Raccord_base_32_64<_SIZE_>,front))
        {
          noms_bords_and_jnts[i] = "type_raccord_";
          noms_bords_and_jnts[i] += front.le_nom();
        }
      else if (sub_type(Groupe_Faces_32_64<_SIZE_>,front))
        {
          noms_bords_and_jnts[i] = "groupes_faces_";
          noms_bords_and_jnts[i] += front.le_nom();
        }
      else
        noms_bords_and_jnts[i] = front.le_nom();
      sz_bords_and_jnts[i] = front.nb_faces();
    }
//  for(int i=0; i<nb_jnts; i++)
//    {
//      const Joint& jnt = dom.joint(i);
//      noms_bords_and_jnts[nb_bords+i] = jnt.le_nom();
//      sz_bords_and_jnts[nb_bords+i] = jnt.nb_faces();
//    }
}

template <typename _SIZE_>
void Ecrire_MED_32_64<_SIZE_>::ecrire_domaine(bool append)
{
  if (Process::nproc()>Process::multiple_files)
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << finl;
      Cerr << "*** WARNING *** The MED post-processing format will generate " << Process::nproc() << " files" << finl;
      Cerr << "This will slow I/O performances since it exceeds the limit of " << Process::multiple_files << finl;
      Cerr << "Use LATA format instead or contact TRUST support team." << finl;
      Cerr << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << finl;
    }
  OBS_PTR(Domaine_dis_base) domaine_dis_base; // Pas de domaine discretise
  ecrire_domaine_dis(domaine_dis_base, append);
}

/*! @brief Fill face and groups in the MEDCoupling object
 *
 * Two cases:
 * - either we want the full face mesh, in which case we guarantee that the face numbering will be identical to TRUST in the
 * final MED file
 * - or, we only write boundary faces. In this case we can still preserve face numbering for all 'classical' borders (i.e. not
 * joints) since by construction TRUST places those faces first, but faces of **joints** are renumbered.
 */
template <typename _SIZE_>
void Ecrire_MED_32_64<_SIZE_>::fill_faces_and_boundaries(const OBS_PTR(Domaine_dis_base)& domaine_dis_base)
{
  using Frontiere_t = Frontiere_32_64<_SIZE_>;

  // Fill arrays all_faces_bords and noms_bords
  Noms noms_bords_and_jnts;
  ArrOfInt_t sz_bords_and_jnts;
  get_bords_infos(noms_bords_and_jnts, sz_bords_and_jnts);

  int_t nfaces = 0;
  bool full_face_mesh = domaine_dis_base.non_nul() && ref_cast(Domaine_VF, domaine_dis_base.valeur()).elem_faces().size()>0;
  // If the domain has faces (eg:domain computation), we can create a face mesh (all faces, incl internal ones), else only a boundary mesh
  if (full_face_mesh)
    {
      assert((std::is_same<_SIZE_, int>::value));  // we have a Domaine_dis, so we discretized already, so we are 32bits ...
      // Faces mesh - by construction (see build_mc_face_mesh) it will have the same face numbering as in TRUST.
      dom_->build_mc_face_mesh(domaine_dis_base.valeur());
      const MEDCouplingUMesh *faces_mesh = dom_->get_mc_face_mesh();
      MCAuto<MEDCouplingUMesh> face_mesh2 = faces_mesh->clone(false); // perform a super light copy, no data array copied
      face_mesh2->setName(mfumesh_->getName());  // names have to be aligned ...
      mfumesh_->setMeshAtLevel(-1, face_mesh2, false);
      nfaces = static_cast<int>(faces_mesh->getNumberOfCells());
    }
  else // Boundary mesh only
    {
      MCAuto<MEDCouplingUMesh> boundary_mesh(MEDCouplingUMesh::New(mcumesh_->getName(), mesh_dimension_ - 1));
      boundary_mesh->setCoords(mcumesh_->getCoords());

      for (int j = 0; j < noms_bords_and_jnts.size(); j++) nfaces += sz_bords_and_jnts(j);
      boundary_mesh->allocateCells(nfaces);

      for (int b=0; b < noms_bords_and_jnts.size(); b++)
        {
          const Frontiere_t& front = ::mes_faces_fr(dom_.valeur(),b);
          const IntTab_t& som_fac = front.les_sommets_des_faces();
          int_t nb_fac = som_fac.dimension(0);
          int nb_som_fac = som_fac.dimension_int(1);
          const Nom& typ_f_trust = front.faces().type(front.faces().type_face());

          int boundary_mesh_dimension = -1;
          INTERP_KERNEL::NormalizedCellType typ_fac_mc = type_geo_trio_to_type_medcoupling(typ_f_trust, boundary_mesh_dimension);
          assert(boundary_mesh_dimension == mesh_dimension_ - 1);

          // Convert connectivity from TRUST to MED:
          IntTab_t som_fac_cpy = som_fac; // a deep copy since next method modify this in place:
          conn_trust_to_med(som_fac_cpy, typ_f_trust, true);
          // Insert it in the MC mesh, **always as a polygon/segment**:
          for (int_t face_idx=0; face_idx < nb_fac; face_idx++)
            {
              int nvertices = nb_som_fac;
              for (int k = 0; k < nb_som_fac; k++)
                if (som_fac_cpy(face_idx, k) < 0) nvertices--; // Non constant number of vertices (polygons)
              if(std::is_same<_SIZE_, mcIdType>::value)
                {
                  // Wild cast just for compiler (when _SIZE_!=mcIdType) - the test above ensures pointer types are aligned:
                  const mcIdType* where = (mcIdType *)(som_fac_cpy.addr() + face_idx*nb_som_fac);
                  boundary_mesh->insertNextCell(typ_fac_mc, nvertices, where);
                }
              else
                {
                  auto ptr = som_fac_cpy.addr() + face_idx*nb_som_fac;
                  std::vector<mcIdType> tmp(ptr, ptr+nvertices); // will copy and downcast
                  boundary_mesh->insertNextCell(typ_fac_mc, nvertices, tmp.data());
                }
            }
        }
      mfumesh_->setMeshAtLevel(-1, boundary_mesh, false);
    }

  // Register groups
  std::vector<const DataArrayIdType *> grps;
  std::vector<MCAuto<DataArrayIdType>> grps_mem;  // just for memory management -> will ensure proper array deletion when destroyed

  int_t face_idx = 0, start, end;
  int nb_bords = dom_->nb_front_Cl() + dom_->nb_groupes_faces();
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

//  [ABN] TODO handle joints properly - they could be written too.
//  // Joint do not necessary have consecutive face numbers and must be handled separately
//  for (int j=0; j < dom_->nb_joints(); j++)
//    {
//      const Joint& jnt = dom_->joint(j);
//      const ArrOfInt& ric = jnt.joint_item(JOINT_ITEM::FACE).items_communs();
//      assert(ric.size_array() == jnt.nb_faces());
//      MCAuto<DataArrayIdType> g(DataArrayIdType::New());
//      g->alloc(ric.size_array());
//      mcIdType* gP = g->getPointer();
//      if (full_face_mesh) // we can preserve original TRUST numbers
//        for (int k=0; k<jnt.nb_faces(); k++) gP[k] = ric(k);
//      else                // otherwise just take next available ids:
//        for (int k=0; k<jnt.nb_faces(); k++) gP[k] = face_idx++;
//      g->setName(jnt.le_nom().getChar());
//      grps_mem.push_back(g);
//      grps.push_back(g);
//    }
  // Save all this:
  mfumesh_->setGroupsAtLevel(-1, grps);
}

/*! @brief Ecrit le domaine dom dans le fichier nom_fichier_
 *
 * @param append = false nouveau fichier, append = true ajout du domaine dans le fichier
 */
template <typename _SIZE_>
void Ecrire_MED_32_64<_SIZE_>::ecrire_domaine_dis(const OBS_PTR(Domaine_dis_base)& domaine_dis_base, bool append)
{
  if (Objet_U::dimension==0)
    Process::exit("Dimension is not defined. Check your data file.");
#ifndef MEDCOUPLING_
  med_non_installe(); // actually MEDCoupling ... but will do.
#else

  // Retrieve (or build!) the MEDCouplingUMesh associated with the domain:
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
  fill_faces_and_boundaries(domaine_dis_base);

  // Write:
  int option = (append ? 1 : 2); /* 2: reset file. 1: append, 0: overwrite objects */
  Cerr<<"Writing file '" << nom_fichier_<<"' with mesh name '" << mfumesh_->getName() << "' (append=" << (append ? "true": "false") << ") ..."<<finl;
  if (major_mode_)
    mfumesh_->write40(nom_fichier_.getString(), option);
  else
    mfumesh_->write(nom_fichier_.getString(), option);
#endif
}

#if INT_is_64_ == 2
template <>
void Ecrire_MED_32_64<trustIdType>::ecrire_domaine_dis(const OBS_PTR(Domaine_dis_base)& domaine_dis_base, bool append)
{
  Process::exit("Ecrire_MED_32_64<trustIdType>::ecrire_domaine_dis() -- Not allowed with a 64b object!");
}
#endif

/*! @brief Permet d'ecrire le tableau de valeurs val comme un champ dans le fichier med de nom nom_fichier_, avec pour support le domaine de nom nom_dom.
 *
 *   @param type: CHAMPPOINT,CHAMPMAILLE,CHAMPFACES
 *   @param nom_cha1 le nom du champ
 *   @param unite : les unites
 *   @param type_elem le type des elems du domaine
 *   @param time le temps
 */
template <typename _SIZE_>
void Ecrire_MED_32_64<_SIZE_>::ecrire_champ(const Nom& type, const Nom& nom_cha1, const DoubleTab& val, const Noms& unite,
                                            const Noms& noms_compo, const Nom& type_elem, double time)
{
  if (Process::nproc()>Process::multiple_files)
    {
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << finl;
      Cerr << "*** WARNING *** The MED post-processing format will generate " << Process::nproc() << " files" << finl;
      Cerr << "This will slow I/O performances since it exceeds the limit of " << Process::multiple_files << finl;
      Cerr << "Use LATA format instead or contact TRUST support team." << finl;
      Cerr << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << finl;
      Cerr << finl;
    }
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

  // Get directly the U-mesh from the domain, except in some weird cases:
  if (nom_dom == "PARTICULES")
    {
      MEDCoupling::CheckFileForRead(file_name);
      std::string mesh_name = nom_dom.getString();
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
  else
    {
      if (type == "CHAMPFACES")
        field->setMesh(dom_->get_mc_face_mesh());
      else
        field->setMesh(dom_->get_mc_mesh());
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

#if INT_is_64_ == 2
template <>
void Ecrire_MED_32_64<trustIdType>::ecrire_champ(const Nom& type, const Nom& nom_cha1, const DoubleTab& val, const Noms& unite,
                                                 const Noms& noms_compo, const Nom& type_elem, double time)
{
  Process::exit("Ecrire_MED_32_64<trustIdType>::ecrire_champ() -- Not allowed with a 64b object!");
}
#endif


#endif


template class Ecrire_MED_32_64<int>;
#if INT_is_64_ == 2
template class Ecrire_MED_32_64<trustIdType>;
#endif


