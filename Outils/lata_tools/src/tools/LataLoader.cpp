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

#include <LataLoader.h>
#include <LataJournal.h>
#include <LmlReader.h>
#include <TRUSTList.h>
#include <iostream>
#include <fstream>
#include <string>
#include <numeric>

#include <MEDCouplingRefCountObject.hxx>
#include <MEDCouplingFieldDouble.hxx>
#include <MEDCouplingMemArray.hxx>
#include <MEDCouplingUMesh.hxx>
#include <MEDCouplingCMesh.hxx>

using namespace MEDCoupling;

LataLoader::LataLoader(const char *file)
{
  filename = Nom(file);
  cerr << "LataLoader constructor " << filename << endl;
  try
    {
      set_Journal_level(0);

      LataOptions opt;
      LataOptions::extract_path_basename(filename, opt.path_prefix, opt.basename);
      opt.dual_mesh = true;
      opt.faces_mesh = true;
      opt.regularize = 2;
      opt.regularize_tolerance = 1e-7f;
      opt.user_fields_ = true;
      read_any_format_options(filename, opt);
      cerr << "LataLoader: initializing filter" << endl;
      // Read the source file to the lata database
      read_any_format(filename, opt.path_prefix, lata_db_);
      filter_.initialize(opt, lata_db_);
    }
  catch (LataDBError& err)
    {
      cerr << "Error in LataFilter::initialize " << filename << " " << err.describe() << endl;
      throw err;
    }
  PopulateDatabaseMetaData(0);
}

LataLoader::~LataLoader() { }

int LataLoader::GetNTimesteps(void)
{
  int n;
  try
    {
      n = filter_.get_nb_timesteps();
      // Timestep 0 contains global definitions.
      // If we have "real" timesteps, do not show timestep 0
      if (n > 1)
        n--;
    }
  catch (LataDBError& err)
    {
      cerr << "Error in getntimesteps " << filename << " " << err.describe() << endl;
      throw;
    }
  return n;
}

void LataLoader::GetTimes(std::vector<double>& times)
{
  int n;
  try
    {
      n = filter_.get_nb_timesteps();
      if (n == 1)
        times.push_back(0.);
      else
        for (int i = 1; i < n; i++)
          times.push_back(filter_.get_timestep(i));
    }
  catch (LataDBError& err)
    {
      cerr << "Error in gettimes " << filename << " " << err.describe() << endl;
      throw;
    }
  return;
}

void LataLoader::FreeUpResources(void)
{
}

void LataLoader::PopulateDatabaseMetaData(int timeState)
{
  try
    {
      cerr << "LataLoader::PopulateDatabaseMetaData : " << filename << " " << timeState << endl;

      const Noms geoms = filter_.get_exportable_geometry_names();

      for (int i_geom = 0; i_geom < geoms.size(); i_geom++)
        {
          cerr << " Domain : " << geoms[i_geom] << endl;
          const LataGeometryMetaData data = filter_.get_geometry_metadata(geoms[i_geom]);

          bool mesh_faces = false;
          if (data.internal_name_.finit_par("_centerfaces"))
            {
              //cerr<<"la "<<data.internal_name_<<endl;
              mesh_faces = true;
            }

          const std::string geom_name(data.displayed_name_);
          //  AddMeshToMetaData(md, geom_name, mt, extents, data.nblocks_, block_origin,                      data.dimension_, topo_dim);
          mesh_username_.add(data.displayed_name_);
          mesh_latafilter_name_.add(data.internal_name_);

          Field_UNames fields = filter_.get_exportable_field_unames(geoms[i_geom]);

          for (int i_field = 0; i_field < fields.size(); i_field++)
            {
              const LataFieldMetaData data2 = filter_.get_field_metadata(fields[i_field]);

              // Take localisation of source field
              Nom loc = data2.source_localisation_;
              std::string varname(data2.name_);
              varname += "_";
              varname += loc;
              varname += "_";
              varname += geom_name;
              if (data2.nb_components_ == 1)
                {
                  // Scalar field
                  // We append the geometry name to the component name:
                  register_fieldname(varname.c_str(), fields[i_field], 0);
                  //      if (mesh_faces==0)           AddScalarVarToMetaData(md, varname, geom_name, cent);
                }
              else if (data2.is_vector_ && data2.nb_components_ == data.dimension_)
                {
                  // Vector field
                  register_fieldname(varname.c_str(), fields[i_field], -1);
                  //AddVectorVarToMetaData(md, varname, geom_name, cent, data2.nb_components_);
                }
              else
                {
                  // Multiscalar field
                  // I chose to postfix the varname with the component name, perhaps not the best choice.
                  if (!mesh_faces)
                    {
                      for (int i_compo = 0; i_compo < data2.nb_components_; i_compo++)
                        {
                          std::string varname2(data2.name_);
                          varname2 += "_";
                          if (data2.component_names_.size() == data2.nb_components_)
                            {
                              varname2 += data2.component_names_[i_compo];
                            }
                          else
                            {
                              Nom n(i_compo);
                              varname2 += n;
                            }
                          varname2 += "_";
                          varname2 += loc;
                          varname2 += "_";
                          varname2 += geom_name;
                          register_fieldname(varname2.c_str(), fields[i_field], i_compo);
                          //   AddScalarVarToMetaData(md, varname2, geom_name, cent);
                        }
                    }
                }
            }
        }
      cerr << "End LataLoader::PopulateDatabaseMetaData" << endl;
    }
  catch (LataDBError& err)
    {
      cerr << "Error in PopulateDatabaseMetaData " << err.describe() << endl;
      throw;
    }
}

void LataLoader::register_fieldname(const char *visit_name, const Field_UName& uname, int component)
{
  if (field_username_.rang(visit_name) >= 0)
    {
      cerr << "Error in LataLoader::register_fieldname: duplicate field name " << visit_name << endl;
      cerr << "Ignoring field" << endl;
      return;
    }
  field_username_.add(visit_name);
  field_uname_.add(uname);
  field_component_.add(component);
}

void LataLoader::register_meshname(const char *visit_name, const char *latafilter_name)
{
  if (mesh_username_.rang(visit_name) >= 0)
    {
      cerr << "Error in LataLoader::register_meshname: duplicate name " << visit_name << endl;
      cerr << "Ignoring mesh" << endl;
      return;
    }
  mesh_username_.add(visit_name);
  mesh_latafilter_name_.add(latafilter_name);
}

MEDCouplingMesh* LataLoader::GetMesh(const char *meshname, int timestate, int block)
{
  MEDCouplingMesh *return_value = 0;
  try
    {
      cerr << " LataLoader::GetMesh ts=" << timestate << " block=" << block << " meshname=" << meshname << endl;

      // We have real timesteps in the database, add one to timestep index:
      if (filter_.get_nb_timesteps() > 1)
        timestate++;
      int index = -1;
      for (int i = 0; i < mesh_username_.size(); i++)
        {
          if (Motcle(mesh_username_[i]) == (meshname))
            {
              index = i;
              break;
            }
        }
      if (index < 0)
        {
          cerr << "internal error in LataLoader::GetMesh: name " << meshname << " not found" << endl;
          throw;
        }
      Domain_Id id(mesh_latafilter_name_[index], timestate, block);
      const Domain& geometry = filter_.get_geometry(id);

      const DomainUnstructured *geom_ptr = dynamic_cast<const DomainUnstructured*>(&geometry);
      const DomainIJK *ijk_ptr = dynamic_cast<const DomainIJK*>(&geometry);

      if (geom_ptr)
        {
          const DomainUnstructured& geom = *geom_ptr;

          const BigFloatTab& pos = geom.nodes_;
          const trustIdType nnodes = pos.dimension(0);
          assert(nnodes < std::numeric_limits<trustIdType>::max());
          //   const int dim3 = pos.dimension(1) == 3;
          const int dim = (int)pos.dimension(1);
          //ugrid->setMeshDimension(dim);
          MEDCouplingUMesh *ugrid = MEDCouplingUMesh::New(meshname, dim);
          DataArrayDouble *points = DataArrayDouble::New();
          points->alloc(nnodes, dim);
          double *pts = points->getPointer();
          std::copy(pos.addr(), pos.addr() + pos.size_array(), pts);
          ugrid->setCoords(points);
          points->decrRef();

          const BigTIDTab& conn = geom.elements_;
          assert(conn.dimension(0) < std::numeric_limits<trustIdType>::max());
          const mcIdType ncells = (mcIdType)conn.dimension(0);
          int nverts = (int)conn.dimension(1);

          INTERP_KERNEL::NormalizedCellType type_cell;
          switch(geom.elt_type_)
            {
            case Domain::point:
              type_cell = INTERP_KERNEL::NORM_POINT1;
              if (ncells == 0)
                nverts = 1;
              break;
            case Domain::line:
              type_cell = INTERP_KERNEL::NORM_SEG2;
              ugrid->setMeshDimension(1);
              break;
            case Domain::triangle:
              type_cell = INTERP_KERNEL::NORM_TRI3;
              ugrid->setMeshDimension(2);
              break;
            case Domain::quadri:
              type_cell = INTERP_KERNEL::NORM_QUAD4;
              ugrid->setMeshDimension(2);
              break;
            case Domain::tetra:
              type_cell = INTERP_KERNEL::NORM_TETRA4;
              break;
            case Domain::prism6:
              type_cell = INTERP_KERNEL::NORM_PENTA6;
              break;
            case Domain::hexa:
              type_cell = INTERP_KERNEL::NORM_HEXA8;
              break;
            case Domain::polyedre:
              type_cell = INTERP_KERNEL::NORM_POLYHED;
              break;
            case Domain::polygone:
              type_cell = INTERP_KERNEL::NORM_POLYGON;
              break;
            default:
              type_cell = INTERP_KERNEL::NORM_POLYHED;
              cerr << "LataLoader::GetMesh unknown elt type " << endl;
              throw;
              break;
            }
          mcIdType *verts = new mcIdType[nverts];
          if (type_cell == INTERP_KERNEL::NORM_POINT1 && ncells == 0)
            {
              throw;
            }
          /*
           * XXX : Elie Saikali cas generique
           */
          else if (type_cell == INTERP_KERNEL::NORM_POLYHED)
            {
              cerr << "type_cell == INTERP_KERNEL::NORM_POLYHED  !!!! " << endl;

              const BigTIDTab &elems = geom.elements_;
              const BigTIDTab &faces = geom.faces_; // elem -> nodes
              const BigTIDTab &ef = geom.elem_faces_; // elem -> nodes

              const trustIdType nb_elems = elems.dimension(0);
              ugrid->allocateCells(nb_elems);

              std::vector<trustIdType> conn, connIndex;
              trustIdType idx = 0;
              connIndex.push_back(idx);
              for (int i = 0; i < nb_elems; i++)
                {
                  conn.push_back(31); /* MC.normPolyhedre dans MC */
                  idx++;

                  for (int j = 0; j < ef.dimension(1); j++)
                    {
                      trustIdType fac = ef(i, j);

                      if (fac < 0)
                        continue;

                      bool is_not_last = true;

                      if (j + 1 < ef.dimension(1) && ef(i, j + 1) < 0)
                        is_not_last = false;

                      for (int jj = 0; jj < faces.dimension(1); jj++)
                        {
                          const trustIdType ff = faces(fac, jj);

                          if (ff < 0)
                            continue;

                          conn.push_back(faces(fac, jj));
                          idx++;
                        }

                      if (j < ef.dimension(1) - 1 && is_not_last)
                        {
                          conn.push_back(-1);
                          idx++;
                        }
                    }
                  connIndex.push_back(idx);
                }

              trustIdType marker = 0;
              trustIdType conn_size = static_cast<trustIdType>(conn.size());

              for (trustIdType i = 0; i < conn_size; i++)
                if (conn[i] < 0)
                  marker++;

              trustIdType num_nodes = conn_size - nb_elems - marker;
              trustIdType nfaces = nb_elems + marker;

              ArrOfInt_T<trustIdType> nodes(num_nodes), facesIndex(nfaces + 1), polyhedronIndex(nb_elems + 1);

              trustIdType face = 0, node = 0;

              for (trustIdType i = 0; i < nb_elems; i++)
                {
                  polyhedronIndex[i] = face; // Index des polyedres

                  trustIdType index = connIndex[i] + 1;
                  int nb_som = static_cast<int>(connIndex[i + 1] - index);
                  for (trustIdType j = 0; j < nb_som; j++)
                    {
                      if (j == 0 || conn[index + j] < 0)
                        facesIndex[face++] = node; // Index des faces:
                      if (conn[index + j] >= 0)
                        nodes[node++] = conn[index + j]; // Index local des sommets de la face
                    }
                }
              facesIndex[nfaces] = node;
              polyhedronIndex[nb_elems] = face;

              /*
               * Partie Polyedre
               */

              int nb_som_elem_max_ = 0;
              // detremination de nbsom_max
              TRUSTList<trustIdType> prov;
              int nb_face_elem_max_ = 0;
              int nb_som_face_max_ = 0;
              trustIdType nelem = polyhedronIndex.size_array() - 1;
              for (trustIdType ele = 0; ele < nelem; ele++)
                {
                  prov.vide();
                  int nbf = (int) (polyhedronIndex[ele + 1] - polyhedronIndex[ele]); // num of faces always int
                  if (nbf > nb_face_elem_max_)
                    nb_face_elem_max_ = nbf;
                  for (trustIdType f = polyhedronIndex[ele]; f < polyhedronIndex[ele + 1]; f++)
                    {
                      //Cerr<<" ici "<<ele << " " <<f <<" "<<FacesIndex(f+1)-FacesIndex(f)<<finl;
                      int nbs = (int) (facesIndex[f + 1] - facesIndex[f]);
                      if (nbs > nb_som_face_max_)
                        nb_som_face_max_ = nbs;
                      for (trustIdType s = facesIndex[f]; s < facesIndex[f + 1]; s++)
                        prov.add_if_not(nodes[s]);
                    }
                  int nbsom = prov.size();
                  if (nbsom > nb_som_elem_max_)
                    nb_som_elem_max_ = nbsom;
                }
              Cerr << " Polyhedron information nb_som_elem_max " << nb_som_elem_max_ << " nb_som_face_max " << nb_som_face_max_ << " nb_face_elem_max " << nb_face_elem_max_ << finl;
              IntTab_T<trustIdType> les_elems;
              les_elems.resize(nelem, nb_som_elem_max_);
              les_elems = -1;

              // on refait un tour pour determiiner les elems
              for (trustIdType ele = 0; ele < nelem; ele++)
                {
                  prov.vide();
                  for (trustIdType f = polyhedronIndex[ele]; f < polyhedronIndex[ele + 1]; f++)
                    for (trustIdType s = facesIndex[f]; s < facesIndex[f + 1]; s++)
                      prov.add_if_not(nodes[s]);
                  int nbsom = prov.size();
                  // on trie prov dans l'ordre croissant
                  // pas strictement necessaire mais permet de garder le meme tableau elem meme si on a effectue des permutation pour les faces
                  bool perm = 1;
                  while (perm)
                    {
                      perm = false;
                      for (int i = 0; i < nbsom - 1; i++)
                        if (prov[i] > prov[i + 1])
                          {
                            perm = true;
                            trustIdType sa = prov[i];
                            prov[i] = prov[i + 1];
                            prov[i + 1] = sa;
                          }
                    }
                  for (int s = 0; s < nbsom; s++)
                    les_elems(ele, s) = prov[s];
                }

              // Determination de Nodes_...
              TRUSTArray<int, trustIdType> Nodes_;
              Nodes_.resize_array(nodes.size_array());
              Nodes_ = -2;
              for (trustIdType ele = 0; ele < nelem; ele++)
                for (trustIdType f = polyhedronIndex[ele]; f < polyhedronIndex[ele + 1]; f++)
                  for (trustIdType s = facesIndex[f]; s < facesIndex[f + 1]; s++)
                    {
                      trustIdType somm_glob = nodes[s];
                      for (int sl = 0; sl < nb_som_elem_max_; sl++)
                        if (les_elems(ele, sl) == somm_glob)
                          {
                            Nodes_[s] = sl;
                            break;
                          }
                    }
              assert(min_array(Nodes_) > -1);

              // Connectivite TRUST -> MED
              IntTab_T<trustIdType> les_elems2(les_elems);

              trustIdType nele = les_elems.dimension(0);
              // cas face_bord vide
              IntTab_T<trustIdType> les_elemsn(les_elems2);
              ArrOfInt filter;

              int nb_som_max = les_elems2.dimension_int(1);
              filter.resize_array(nb_som_max);
              std::iota(filter.addr(), filter.addr() + nb_som_max, 0);

              int ns = filter.size_array();
              for (trustIdType el = 0; el < nele; el++)
                for (int n = 0; n < ns; n++)
                  les_elems2(el, n) = les_elemsn(el, filter[n]);

              ArrOfInt_T<trustIdType> nodes_glob;

              nodes_glob.resize_array(Nodes_.size_array());
              trustIdType nelemzzz = les_elems2.dimension_tot(0);
              for (trustIdType ele = 0; ele < nelemzzz; ele++)
                for (trustIdType f = polyhedronIndex[ele]; f < polyhedronIndex[ele + 1]; f++)
                  for (trustIdType s = facesIndex[f]; s < facesIndex[f + 1]; s++)
                    {
                      int somm_loc = Nodes_[s];
                      nodes_glob[s] = les_elems2(ele, somm_loc);
                    }

              // Polyedron is special, see page 10:
              // http://trac.lecad.si/vaje/chrome/site/doc8.3.0/extra/Normalisation_pour_le_couplage_de_codes.pdf
              for (trustIdType i = 0; i < nb_elems; i++)
                {
//                              cerr << "Gauthier etait la  i " << i << endl;
                  int size = 0;
                  for (trustIdType face = polyhedronIndex[i]; face < polyhedronIndex[i + 1]; face++)
                    size += (int) (facesIndex[face + 1] - facesIndex[face] + 1);

                  size--; // No -1 at the end of the cell

                  ArrOfTID cell_def(size);  // ArrOfTID whatever the template parameter, since TID == mcIdType.
                  size = 0;
                  for (trustIdType face = polyhedronIndex[i]; face < polyhedronIndex[i + 1]; face++)
                    {
                      for (trustIdType node = facesIndex[face]; node < facesIndex[face + 1]; node++)
                        cell_def[size++] = nodes_glob[node];

                      if (size < cell_def.size_array()) // Add -1 to mark the end of a face:
                        cell_def[size++] = -1;
                    }

                  ugrid->insertNextCell(type_cell, cell_def.size_array(), cell_def.addr());
                }
            }
          else
            {
              ugrid->allocateCells(ncells);

              for (mcIdType i = 0; i < ncells; i++)
                {
                  if ((1) && (type_cell == INTERP_KERNEL::NORM_TETRA4))
                    {
                      // Overflow was checked above:
                      const mcIdType som_Z = (mcIdType) conn(i, 0), som_A = (mcIdType) conn(i, 1), som_B = (mcIdType) conn(i, 2), som_C = (mcIdType) conn(i, 3);
                      int test = 1;
                      if (1)
                        {
                          float ZA[3], ZB[3], ZC[3], pdtvect[3], pdtscal;
                          int k;
                          for (k = 0; k < 3; k++)
                            {
                              ZA[k] = pos(som_A, k) - pos(som_Z, k);
                              ZB[k] = pos(som_B, k) - pos(som_Z, k);
                              ZC[k] = pos(som_C, k) - pos(som_Z, k);
                            }

                          //calcul pdt vect ZAxZB
                          pdtvect[0] = ZA[1] * ZB[2] - ZA[2] * ZB[1];
                          pdtvect[1] = ZA[2] * ZB[0] - ZA[0] * ZB[2];
                          pdtvect[2] = ZA[0] * ZB[1] - ZA[1] * ZB[0];
                          pdtscal = 0.;
                          for (k = 0; k < 3; k++)
                            {
                              pdtscal += pdtvect[k] * ZC[k];
                            }
                          if (pdtscal < 0)
                            test = -1;

                        }
                      if (test == 1)
                        {
                          verts[0] = som_Z;
                          verts[1] = som_A;
                          verts[2] = som_B;
                          verts[3] = som_C;
                        }
                      else
                        {
                          verts[0] = som_Z;
                          verts[1] = som_B;
                          verts[2] = som_A;
                          verts[3] = som_C;

                        }
                    }
                  else if (type_cell == INTERP_KERNEL::NORM_QUAD4)
                    {
                      // Nodes order is different in visit than in trio_u
                      verts[0] = (mcIdType) conn(i, 0);
                      verts[1] = (mcIdType) conn(i, 1);
                      verts[2] = (mcIdType) conn(i, 3);
                      verts[3] = (mcIdType) conn(i, 2);
                    }
                  else if (type_cell == INTERP_KERNEL::NORM_HEXA8)
                    {
                      // Nodes order is different in visit than in trio_u
                      verts[0] = (mcIdType) conn(i, 0);
                      verts[1] = (mcIdType) conn(i, 1);
                      verts[2] = (mcIdType) conn(i, 3);
                      verts[3] = (mcIdType) conn(i, 2);
                      verts[4] = (mcIdType) conn(i, 4);
                      verts[5] = (mcIdType) conn(i, 5);
                      verts[6] = (mcIdType) conn(i, 7);
                      verts[7] = (mcIdType) conn(i, 6);
                    }
                  else
                    {
                      for (int j = 0; j < nverts; j++)
                        verts[j] = (mcIdType) conn(i, j);
                    }
                  if (type_cell != INTERP_KERNEL::NORM_POLYHED)
                    ugrid->insertNextCell(type_cell, nverts, verts);
                }
            }
          ugrid->finishInsertingCells();

          delete[] verts;
          verts = 0;
          return_value = ugrid;

        }
      else if (ijk_ptr)
        {
          //  throw "KKKK";
          const DomainIJK& geom = *ijk_ptr;

          // Maillage regulier : on transmet la grille ijk

          const int dim = geom.coord_.size();
          MEDCouplingCMesh *sgrid = MEDCouplingCMesh::New(meshname);
          ArrOfInt ncoord(3);
          ncoord = 1;
          int i;
          for (i = 0; i < dim; i++)
            ncoord[i] = geom.coord_[i].size_array();
          //    sgrid->SetDimensions(ncoord[0], ncoord[1], ncoord[2]);

          for (i = 0; i < 3; i++)
            {

              const int n = ncoord[i];
              /*
               vtkFloatArray *c;
               c = vtkFloatArray::New();
               c->SetNumberOfTuples(n);
               data = (float *) c->GetVoidPointer(0);
               */
              DataArrayDouble *c = DataArrayDouble::New();
              c->alloc(n);
              double *data = c->getPointer();

              if (i < dim)
                {
                  const ArrOfFloat& coord = geom.coord_[i];
                  for (int j = 0; j < n; j++)
                    data[j] = coord[j];
                }
              else
                {
                  data[0] = 0.;
                }
              sgrid->setCoordsAt(i, c);

              c->decrRef();
            }


          return_value = sgrid;
        }
      else
        {
          cerr << "Error in LataLoader::GetMesh: unknown geometry type" << endl;
          throw;
        }

      filter_.release_geometry(geometry);
    }
  catch (LataDBError& err)
    {
      cerr << "Error in getmesh " << timestate << " " << block << " " << meshname << " " << err.describe() << endl;
      throw;
    }

  return return_value;
}

DataArray* LataLoader::GetVectorVar(int timestate, int block, const char *varname)
{
  DataArray *return_value = 0;
  try
    {
      cerr << "Getvectorvar time:" << timestate << "/" << filter_.get_nb_timesteps() - 1 << " block:" << block << " varname:" << varname << endl;

      if (filter_.get_nb_timesteps() > 1)
        timestate++;

      Field_UName field_uname;
      int component;
      get_field_info_from_visitname(varname, field_uname, component);
      /*
       if (component >= 0) {
       cerr << "Error: LataLoader::GetVectorVar called for scalar field" << endl;
       throw;
       }
       */
      Field_Id id(field_uname, timestate, block);

      const LataField_base& field = filter_.get_field(id);

      const Field<BigFloatTab> *float_field_ptr = dynamic_cast<const Field<BigFloatTab>*>(&field);
      const Field<BigTIDTab> *tid_field_ptr = dynamic_cast<const Field<BigTIDTab>*>(&field);
      const Field<BigIntTab> *int_field_ptr = dynamic_cast<const Field<BigIntTab>*>(&field);

      if (float_field_ptr)
        {
          DataArrayDouble *rv = DataArrayDouble::New();
          const Field<BigFloatTab>& fld = *float_field_ptr;
          const BigFloatTab& values = fld.data_;
          trustIdType ntuples = values.dimension(0);
          int dim = (int)values.dimension(1);
          rv->alloc(ntuples, dim);
          double *data = rv->getPointer();
          for (trustIdType i = 0; i < ntuples; i++)
            for (int j = 0; j < dim; j++)
              data[i * dim + j] = values(i, j);
          return_value = rv;
        }
      else if (int_field_ptr)
        {
          throw;

        }
      else
        {
          cerr << "Error in LataLoader::GetVectorVar: unknown data type" << endl;
          throw;
        }
      filter_.release_field(field);
    }
  catch (LataDBError& err)
    {
      cerr << "Error in getvectorvar " << timestate << " " << block << " " << varname << " " << err.describe() << endl;
      throw;
    }
  return return_value;
}

void LataLoader::get_field_info_from_visitname(const char *varname, Field_UName& uname, int& component) const
{
  const int k = field_username_.rang(varname);
  if (k < 0)
    {
      cerr << "Error in LataLoader::get_field_info_from_visitname: field " << varname << " not found" << endl;
      for (int i = 0; i < field_username_.size(); i++)
        cerr << field_username_[i] << " ";
      throw;
    }
  uname = field_uname_[k];
  component = field_component_[k];
}

MEDCouplingFieldDouble* LataLoader::GetFieldDouble(const char *varname, int timestate, int block)
{
  if (timestate == -1)
    timestate = filter_.get_nb_timesteps() - 2;
  const Noms geoms = filter_.get_exportable_geometry_names();
  TypeOfField cent;
  Field_UName field_uname;
  int component;
  get_field_info_from_visitname(varname, field_uname, component);

  {
    const LataGeometryMetaData data = filter_.get_geometry_metadata(field_uname.get_geometry());

    const LataFieldMetaData data2 = filter_.get_field_metadata(field_uname);

    switch(data2.localisation_)
      {
      case LataField_base::ELEM:
        cent = ON_CELLS;
        break;
      case LataField_base::SOM:
        cent = ON_NODES;
        break;
      default:
        throw;
      }
  }

  double time = filter_.get_timestep(timestate + 1);
  MEDCouplingFieldDouble *ret = MEDCouplingFieldDouble::New(cent, ONE_TIME);
  MEDCouplingMesh *mesh = GetMesh(field_uname.get_geometry(), timestate, block);
  DataArray *array = GetVectorVar(timestate, block, varname);
  ret->setMesh(mesh);
  ret->setArray((DataArrayDouble*) array);
  mesh->decrRef();
  array->decrRef();

  ret->setName(varname);
  ret->setTime(time, timestate, -1);
  ret->checkConsistencyLight();
  return ret;
}

std::vector<std::string> LataLoader::GetMeshNames()
{
  std::vector<std::string> names;
  const Noms& geoms = mesh_username_;
  for (int i = 0; i < geoms.size(); i++)
    names.push_back(geoms[i].getString());
  return names;

}
std::vector<std::string> LataLoader::GetFieldNames()
{
  std::vector<std::string> names;
  for (int i = 0; i < field_username_.size(); i++)
    {
      const Nom& name = field_username_[i];
      if ((!name.debute_par("mesh_quality/")) && (!name.debute_par("normals/")))
        names.push_back(name.getString());
    }
  return names;
}
std::vector<std::string> LataLoader::GetFieldNamesOnMesh(const std::string& domain_name)
{
  std::vector<std::string> names;
  std::vector<std::string> names_tot = GetFieldNames();
  Nom test("_");
  test += domain_name.c_str();
  for (int i = 0; i < names_tot.size(); i++)
    {
      const Nom& name = names_tot[i];
      if (name.finit_par(test))
        names.push_back(name.getString());
    }
  return names;
}
