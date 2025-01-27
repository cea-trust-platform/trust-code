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

#include <Comm_Group_MPI.h>
#include <Ecrire_CGNS.h>
#include <Domaine.h>
#include <unistd.h>

#ifdef HAS_CGNS

#ifdef MPI_
#if INT_is_64_ == 1
#define MPI_ENTIER MPI_LONG
#else
#define MPI_ENTIER MPI_INT
#endif
#endif

void Ecrire_CGNS::cgns_set_base_name(const Nom& fn)
{
  baseFile_name_ = fn.getString();
}

void Ecrire_CGNS::cgns_init_MPI()
{
#ifdef MPI_
  if (Option_CGNS::MULTIPLE_FILES && !postraiter_domaine_)
    {
      if (cgp_mpi_comm(/* XXX */ MPI_COMM_SELF) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_init_MPI : cgp_pio_mode !" << finl, TRUST_CGNS_ERROR();
    }
  else
    {
      if (cgp_mpi_comm(/* XXX MPI_COMM_WORLD */ Comm_Group_MPI::get_trio_u_world()) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_init_MPI : cgp_pio_mode !" << finl, TRUST_CGNS_ERROR();
    }

  /*
   * Elie Saikali XXX NOTA BENE XXX : We need sometimes to write planes, boundaries where some processors have no elements/vertices ...
   * This wont work because the default PIO mode of CGNS is COLLECTIVE. We want it to be INDEPENDENT !
   */
  if (cgp_pio_mode((CGNS_ENUMT(PIOmode_t)) CGP_INDEPENDENT) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_init_MPI : cgp_pio_mode !" << finl, TRUST_CGNS_ERROR();
#endif
}

/*
 * XXX : Elie Saikali - Attention pour le moment on a plusieurs options CGNS et du coup plusieurs cas :
 *
 * - SEQUENTIEL => facile => un seul chemin
 *
 * - PARALLEL => plusieurs cas => plusieurs chemins
 *
 *      *** Par default, on ecrit dans un seul fichier, sans lien et avec parallelisme dans la zone.
 *      *** Par default, pour postraiter_domaine, on fait PARALLEL_OVER_ZONES pour visualizer le decoupage
 *
 *      Cependant, on peut changer ca avec les OPTION_CGNS
 *
 *        - MULTIPLE_FILES : sauf si postraiter_domaine, on sort dans ce cas un fichier par proc et un fichier link a la fin.
 *                  Si postraiter_domaine, on fait le default !
 *
 *        - PARALLEL_OVER_ZONES : on sort un fichier avec parallelisme sur les zones. utile pour voir le decoupage d'un maillage par example
 *
 *        - USE_LINKS : utile pour un vrai calcul et si le calcul ne se termine pas correctement. Sauf pour l'interpret postraiter_domaine,
 *                  on sort un fichier // par dt_post et on sort un fichier link a la fin.
 */

void Ecrire_CGNS::cgns_open_file()
{
  if (Process::is_parallel()) cgns_init_MPI(); // 1er truc a faire
  std::string fn = baseFile_name_ + ".cgns"; // file name


  if (Process::is_parallel() && (!Option_CGNS::MULTIPLE_FILES || (Option_CGNS::MULTIPLE_FILES && postraiter_domaine_) ))
    {
#ifdef MPI_
      if (!Option_CGNS::USE_LINKS || postraiter_domaine_) // si sans link, on ouvre
        {
          if (cgp_open(fn.c_str(), CG_MODE_WRITE, &fileId_) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_open_file : cgp_open !" << finl, TRUST_CGNS_ERROR();

          Cerr << "**** Parallel CGNS file " << fn << " opened !" << finl;
        }
#endif
    }
  else
    {
      if (Process::is_parallel() && Option_CGNS::MULTIPLE_FILES)
        fn = (Nom(baseFile_name_)).nom_me(Process::me()).getString() + ".cgns"; // file name

      if (!Option_CGNS::USE_LINKS || postraiter_domaine_) // si sans link, on ouvre
        {
          if (cg_open(fn.c_str(), CG_MODE_WRITE, &fileId_) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_open_file : cg_open !" << finl, TRUST_CGNS_ERROR();

          Cerr << "**** CGNS file " << fn << " opened !" << finl;
        }
    }
}

void Ecrire_CGNS::cgns_close_file()
{
  if (Option_CGNS::USE_LINKS && !postraiter_domaine_)
    {
      cgns_write_final_link_file();
      return;
    }

  std::string fn = baseFile_name_ + ".cgns"; // file name

  if (Process::is_parallel() && (!Option_CGNS::MULTIPLE_FILES || (Option_CGNS::MULTIPLE_FILES && postraiter_domaine_) ))
    {
#ifdef MPI_
      if (Option_CGNS::PARALLEL_OVER_ZONE || postraiter_domaine_)
        cgns_write_iters_par_over_zone();
      else
        cgns_write_iters_par_in_zone();

      //  Process::barrier();
      if (cgp_close (fileId_) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_close_file : cgp_close !" << finl, TRUST_CGNS_ERROR();

      Cerr << "**** Parallel CGNS file " << fn << " closed !" << finl;
#endif
    }
  else
    {
      if (Process::is_parallel() && Option_CGNS::MULTIPLE_FILES)
        fn = (Nom(baseFile_name_)).nom_me(Process::me()).getString() + ".cgns"; // file name

      cgns_write_iters_seq();

      if (cg_close(fileId_) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_close_file : cg_close !" << finl, TRUST_CGNS_ERROR();

      Cerr << "**** CGNS file " << fn << " closed !" << finl;
    }

  /* dernier truc a faire ! */
  if (Process::is_parallel() && Option_CGNS::MULTIPLE_FILES)
    cgns_write_link_file_for_multiple_files();
}

void Ecrire_CGNS::cgns_add_time(const double t)
{
  if (Option_CGNS::USE_LINKS && !postraiter_domaine_) cgns_open_close_link_files(t);

  time_post_.push_back(t); // add time_post
  flowId_elem_++, flowId_som_++; // increment
  fieldId_elem_ = 0, fieldId_som_ = 0; // reset
  solname_elem_written_ = false, solname_som_written_ = false; // reset
}

void Ecrire_CGNS::cgns_write_domaine(const Domaine * dom,const Nom& nom_dom, const DoubleTab& som, const IntTab& elem, const Motcle& type_e)
{
  std::string nom_dom_modifie = TRUST_2_CGNS::modify_domaine_name_for_post(nom_dom);

  if (Option_CGNS::USE_LINKS && !postraiter_domaine_)
    if (!grid_file_opened_)
      cgns_open_grid_base_link_file(), grid_file_opened_ = true; // On ouvre pour .grid.cgns

  if (Process::is_parallel() && (!Option_CGNS::MULTIPLE_FILES || (Option_CGNS::MULTIPLE_FILES && postraiter_domaine_) ))
    {
      if (Option_CGNS::PARALLEL_OVER_ZONE || postraiter_domaine_)
        cgns_write_domaine_par_over_zone(dom, Nom(nom_dom_modifie), som, elem, type_e);
      else
        cgns_write_domaine_par_in_zone(dom, Nom(nom_dom_modifie), som, elem, type_e);
    }
  else
    cgns_write_domaine_seq(dom, Nom(nom_dom_modifie), som, elem, type_e);
}

void Ecrire_CGNS::cgns_write_field(const Domaine& domaine, const Noms& noms_compo, double temps,
                                   const Nom& id_du_champ, const Nom& id_du_domaine, const Nom& localisation,
                                   const DoubleTab& valeurs)
{
  const std::string LOC = Motcle(localisation).getString();

  if (LOC == "FACES")
    {
      Cerr << "THE FACE FIELD " << Motcle(id_du_champ) << " WILL NOT BE WRITTEN ... " << finl; // throw;
      return;
    }

  /* 1 : if first time called ... build different supports for mixed locations */
  cgns_fill_field_loc_map(domaine, LOC);

  /* 2 : on ecrit */
  const int nb_cmp = valeurs.dimension(1);

  if (Process::is_parallel() && (!Option_CGNS::MULTIPLE_FILES || (Option_CGNS::MULTIPLE_FILES && postraiter_domaine_) ))
    {
      for (int i = 0; i < nb_cmp; i++)
        {
          if (Option_CGNS::PARALLEL_OVER_ZONE || postraiter_domaine_)
            cgns_write_field_par_over_zone(i /* compo */, temps, nb_cmp > 1 ? Motcle(noms_compo[i]) : id_du_champ, id_du_domaine, localisation, fld_loc_map_.at(LOC), valeurs);
          else
            cgns_write_field_par_in_zone(i /* compo */, temps, nb_cmp > 1 ? Motcle(noms_compo[i]) : id_du_champ, id_du_domaine, localisation, fld_loc_map_.at(LOC), valeurs);
        }
    }
  else
    for (int i = 0; i < nb_cmp; i++)
      cgns_write_field_seq(i /* compo */, temps, nb_cmp > 1 ? Motcle(noms_compo[i]) : id_du_champ, id_du_domaine, localisation, fld_loc_map_.at(LOC), valeurs);
}

/*
 * *********************************** *
 * METHODES PRIVEES CLASSE Ecrire_CGNS *
 * *********************************** *
 */

void Ecrire_CGNS::cgns_fill_field_loc_map(const Domaine& domaine, const std::string& LOC)
{
  if (static_cast<int>(time_post_.size()) == 1)
    {
      if (!Option_CGNS::USE_LINKS || postraiter_domaine_)
        {
          if (static_cast<int>(fld_loc_map_.size()) == 0)
            fld_loc_map_.insert( { LOC, domaine.le_nom() });/* ici on utilise le 1er support */
          else
            {
              const bool in_map = (fld_loc_map_.count(LOC) != 0);
              if (!in_map) // XXX here we need a new support ... sorry
                {
                  Nom nom_dom = domaine.le_nom();
                  nom_dom += "_";
                  nom_dom += LOC;
                  Cerr << "Building new CGNS zone to host the field located at : " << LOC << " !" << finl;
                  Motcle type_e = domaine.type_elem()->que_suis_je();
                  if (Process::is_parallel() && (!Option_CGNS::MULTIPLE_FILES || (Option_CGNS::MULTIPLE_FILES && postraiter_domaine_) ))
                    {
                      if (Option_CGNS::PARALLEL_OVER_ZONE || postraiter_domaine_)
                        cgns_write_domaine_par_over_zone(&domaine, nom_dom, domaine.les_sommets(), domaine.les_elems(), type_e);
                      else
                        cgns_write_domaine_par_in_zone(&domaine, nom_dom, domaine.les_sommets(), domaine.les_elems(), type_e);
                    }
                  else
                    cgns_write_domaine_seq(&domaine, nom_dom, domaine.les_sommets(), domaine.les_elems(), type_e); // XXX Attention

                  fld_loc_map_.insert( { LOC, nom_dom });
                }
            }
        }
      else // Option_CGNS::USE_LINKS
        {
          assert (Option_CGNS::USE_LINKS);
          if (static_cast<int>(fld_loc_map_.size()) == 0)
            {
              fld_loc_map_.insert( { LOC, domaine.le_nom() });
              cgns_open_solution_link_file(0, LOC, time_post_.back()); // 1st sol file to open here !!
            }
          else
            {
              const bool in_map = (fld_loc_map_.count(LOC) != 0);
              if (!in_map)
                {
                  Cerr << "A second CGNS file will be written to host the fields located at : " << LOC << " !" << finl;
                  fld_loc_map_.insert( { LOC, domaine.le_nom() });
                  cgns_open_solution_link_file(1, LOC, time_post_.back());  // 2nd sol file to open here !!
                }
            }
        }
    }
}

/*
 * ******************** *
 * VERSION SEQUENTIELLE *
 * ******************** *
 */
void Ecrire_CGNS::cgns_write_domaine_seq(const Domaine * domaine,const Nom& nom_dom, const DoubleTab& les_som, const IntTab& les_elem, const Motcle& type_elem)
{
  /* 1 : Instance of TRUST_2_CGNS */
  TRUST_2_CGNS TRUST2CGNS;
  TRUST2CGNS.associer_domaine_TRUST(domaine, les_som, les_elem, postraiter_domaine_);
  doms_written_.push_back(nom_dom);

  CGNS_TYPE cgns_type_elem = TRUST2CGNS.convert_elem_type(type_elem);

  /* 2 : Fill coords */
  std::vector<double> xCoords, yCoords, zCoords;
  TRUST2CGNS.fill_coords(xCoords, yCoords, zCoords);

  const int icelldim = les_som.dimension(1), iphysdim = Objet_U::dimension, nb_som = les_som.dimension(0), nb_elem = les_elem.dimension(0);
  True_int coordsId;

  /* 3 : Base write */
  baseId_.push_back(-123); // pour chaque dom, on a une baseId
  char basename[CGNS_STR_SIZE];
  strcpy(basename, nom_dom.getChar()); // dom name

  if (cg_base_write(fileId_, basename, icelldim, iphysdim, &baseId_.back()) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_domaine_seq : cg_base_write !" << finl, TRUST_CGNS_ERROR();

  /* 4 : Vertex, cell & boundary vertex sizes */
  cgsize_t isize[3][1];
  isize[0][0] = nb_som;
  isize[1][0] = nb_elem;
  isize[2][0] = 0; /* boundary vertex size (zero if elements not sorted) */

  const bool is_polyedre = (type_elem == "POLYEDRE" || type_elem == "PRISME" || type_elem == "PRISME_HEXAG");

  if (Option_CGNS::USE_LINKS)
    cgns_fill_info_grid_link_file(basename, cgns_type_elem, icelldim, nb_som, nb_elem, is_polyedre);

  zoneId_.push_back(-123);

  /* 5 : Write all */
  if (nb_elem) // XXX cas // mais MULTIPLE_FILES
    {
      /* 5.1 : Create zone & grid coords */
      cgns_helper_.cgns_write_zone_grid_coord<TYPE_ECRITURE::SEQ>(icelldim, fileId_, baseId_, basename /* Dom name */, isize[0],
                                                                  zoneId_, xCoords, yCoords, zCoords, coordsId, coordsId, coordsId);

      /* 5.2 : Set element connectivity */
      True_int sectionId;
      cgsize_t start = 1, end;

      if (cgns_type_elem == CGNS_ENUMV(NGON_n)) // cas polyedre
        {
          assert (domaine != nullptr);
          std::vector<cgsize_t> sf, sf_offset;

          end = start + static_cast<cgsize_t>(TRUST2CGNS.convert_connectivity_ngon(sf, sf_offset, is_polyedre)) -1;

          if (cg_poly_section_write(fileId_, baseId_.back(), zoneId_.back(), "NGON_n", CGNS_ENUMV(NGON_n), start, end, 0, sf.data(), sf_offset.data(), &sectionId))
            TRUST_CGNS_ERROR();

          if (is_polyedre) // Pas pour polygone
            {
              std::vector<cgsize_t> ef, ef_offset;

              start = end + 1;
              end = start + static_cast<cgsize_t>(TRUST2CGNS.convert_connectivity_nface(ef, ef_offset)) -1;

              if (cg_poly_section_write(fileId_, baseId_.back(), zoneId_.back(), "NFACE_n", CGNS_ENUMV(NFACE_n), start, end, 0, ef.data(), ef_offset.data(), &sectionId))
                TRUST_CGNS_ERROR();
            }
        }
      else
        {
          std::vector<cgsize_t> elems;

          int nsom = TRUST2CGNS.convert_connectivity(cgns_type_elem, elems);
          end = start + static_cast<cgsize_t>(elems.size()) / nsom - 1;

          if (cg_section_write(fileId_, baseId_.back(), zoneId_.back(), "Elem", cgns_type_elem, start, end, 0, elems.data(), &sectionId) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_domaine_seq : cg_section_write !" << finl, TRUST_CGNS_ERROR();
        }
    }
}

void Ecrire_CGNS::cgns_write_field_seq(const int comp, const double temps, const Nom& id_du_champ, const Nom& id_du_domaine, const Nom& localisation, const Nom& nom_dom, const DoubleTab& valeurs)
{
  std::string LOC = Motcle(localisation).getString();
  Motcle id_du_champ_modifie = TRUST_2_CGNS::modify_field_name_for_post(id_du_champ, id_du_domaine, LOC, fieldId_som_, fieldId_elem_);
  Nom& id_champ = id_du_champ_modifie;

  /* 2 : Get corresponding domain index */
  const int ind = TRUST_2_CGNS::get_index_nom_vector(doms_written_, nom_dom);
  assert(ind > -1);

  const int nb_vals = valeurs.dimension(0);

  /* quel fileID ?? */
  True_int fileId = fileId_;
  if (Option_CGNS::USE_LINKS && !postraiter_domaine_)
    TRUST_2_CGNS::modify_fileId_for_post(fld_loc_map_, LOC, fileId2_, fileId);

  if (nb_vals)
    {
      /* 3 : Write solution names for iterative data later */
      cgns_helper_.cgns_sol_write<TYPE_ECRITURE::SEQ>(1 /* nb_zones_to_write */, fileId, baseId_[ind], ind, temps, zoneId_, LOC, solname_som_, solname_elem_,
                                                      solname_som_written_, solname_elem_written_, flowId_som_, flowId_elem_);

      /* 4 : Fill field values & dump to cgns file */
      cgns_helper_.cgns_field_write_data<TYPE_ECRITURE::SEQ>(fileId, baseId_[ind], ind, zoneId_, LOC, flowId_som_, flowId_elem_, comp,
                                                             id_champ, valeurs, fieldId_som_, fieldId_elem_);
    }
}

void Ecrire_CGNS::cgns_write_iters_seq()
{
  assert(static_cast<int>(baseId_.size()) == static_cast<int>(zoneId_.size()));
  std::vector<int> ind_doms_dumped;

  /* 1 : on iter juste sur le map fld_loc_map_; ie: pas domaine dis ... */
  for (auto &itr : fld_loc_map_)
    {
      const std::string& LOC = itr.first;
      const Nom& nom_dom = itr.second;
      const int ind = TRUST_2_CGNS::get_index_nom_vector(doms_written_, nom_dom);
      ind_doms_dumped.push_back(ind);
      assert(ind > -1);

      cgns_helper_.cgns_write_iters<TYPE_ECRITURE::SEQ>(true /* has_field */, 1 /* nb_zones_to_write */, fileId_, baseId_[ind], ind, zoneId_, LOC, solname_som_, solname_elem_, time_post_);
    }

  /* 2 : on iter sur les autres domaines; ie: domaine dis */
  for (int i = 0; i < static_cast<int>(doms_written_.size()); i++)
    {
      if (std::find(ind_doms_dumped.begin(), ind_doms_dumped.end(), i) == ind_doms_dumped.end()) // indice pas dans ind_doms_dumped
        {
          const Nom& nom_dom = doms_written_[i];
          const int ind = TRUST_2_CGNS::get_index_nom_vector(doms_written_, nom_dom);
          assert(ind > -1);

          cgns_helper_.cgns_write_iters<TYPE_ECRITURE::SEQ>(false /* has_field */, 1 /* nb_zones_to_write */, fileId_, baseId_[ind], ind, zoneId_, "rien", solname_som_, solname_elem_, time_post_);
        }
      else { /* Do Nothing */ }
    }
}

/*
 * *************************** *
 * VERSION PARALLELE OVER ZONE *
 * *************************** *
 */
void Ecrire_CGNS::cgns_write_domaine_par_over_zone(const Domaine * domaine,const Nom& nom_dom, const DoubleTab& les_som, const IntTab& les_elem, const Motcle& type_elem)
{
#ifdef MPI_
  assert (!Option_CGNS::USE_LINKS);
  doms_written_.push_back(nom_dom);

  /* 1 : Instance of TRUST_2_CGNS */
  T2CGNS_.push_back(TRUST_2_CGNS());
  TRUST_2_CGNS& TRUST2CGNS = T2CGNS_.back();
  TRUST2CGNS.associer_domaine_TRUST(domaine, les_som, les_elem, postraiter_domaine_);
  CGNS_TYPE cgns_type_elem = TRUST2CGNS.convert_elem_type(type_elem);

  /* 2 : Fill coords */
  std::vector<double> xCoords, yCoords, zCoords;
  TRUST2CGNS.fill_coords(xCoords, yCoords, zCoords);

  const int icelldim = les_som.dimension(1), iphysdim = Objet_U::dimension,
            nb_som = les_som.dimension(0), nb_elem = les_elem.dimension(0), proc_me = Process::me();

  /* 3 : Base write */
  baseId_.push_back(-123); // pour chaque dom, on a une baseId
  char basename[CGNS_STR_SIZE];
  strcpy(basename, nom_dom.getChar()); // dom name

  if (cg_base_write(fileId_, basename, icelldim, iphysdim, &baseId_.back()) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_over_zone : cg_base_write !" << finl, TRUST_CGNS_ERROR();

  /* 4 : We need global nb_elems/nb_soms => MPI_Allgather. Thats the only information required ! */
  const bool is_polyedre = (type_elem == "POLYEDRE" || type_elem == "PRISME" || type_elem == "PRISME_HEXAG");

  TRUST2CGNS.fill_global_infos(); // XXX

  if (cgns_type_elem == CGNS_ENUMV(NGON_n)) /*cas polygone/polyedre */
    TRUST2CGNS.fill_global_infos_poly(is_polyedre);

  /* 5 : CREATION OF FILE STRUCTURE : zones, coords & sections
   *
   *  - All processors THAT HAVE nb_elem > 0 write the same information.
   *  - Only zone meta-data is written to the library at this stage ... So no worries ^^
   */
  std::vector<True_int> coordsIdx, coordsIdy, coordsIdz, sectionId, sectionId2;
  std::string zonename;

  int nb_zones_to_write = TRUST2CGNS.nb_procs_writing();
  const bool all_write = TRUST2CGNS.all_procs_write(); // all procs will write !

  // on boucle seulement sur les procs qui n'ont pas des nb_elem 0
  zoneId_.clear(); // XXX commencons par ca
  const std::vector<int>& global_nb_elem = TRUST2CGNS.get_global_nb_elem(), global_nb_som = TRUST2CGNS.get_global_nb_som();
  const std::vector<int>& proc_non_zero_elem = TRUST2CGNS.get_proc_non_zero_elem();

  for (int i = 0; i != nb_zones_to_write; i++)
    {
      const int indZ = all_write ? i : proc_non_zero_elem[i]; // procID
      const int ne_loc = global_nb_elem[indZ], ns_loc = global_nb_som[indZ]; /* nb_elem & nb_som local */
      assert (ne_loc > 0);

      cgsize_t start = 1, end = ne_loc;
      cgsize_t isize[3][1];
      isize[0][0] = ns_loc;
      isize[1][0] = end;
      isize[2][0] = 0; /* boundary vertex size (zero if elements not sorted) */

      zoneId_.push_back(-123);
      zonename = nom_dom.nom_me(indZ).getString();
      zonename.resize(CGNS_STR_SIZE, ' ');

      coordsIdx.push_back(-123), coordsIdy.push_back(-123);
      if (icelldim > 2)
        coordsIdz.push_back(-123);

      /* 5.1 : Create zone & Construct the grid coordinates nodes */
      cgns_helper_.cgns_write_zone_grid_coord<TYPE_ECRITURE::PAR_OVER>(icelldim, fileId_, baseId_, zonename.c_str(), isize[0],
                                                                       zoneId_, xCoords, yCoords, zCoords,
                                                                       coordsIdx.back(), coordsIdy.back(), coordsIdz.empty() ? coordsIdy.back() /* inutile */ : coordsIdz.back());

      /* 5.2 : Construct the sections to host connectivity later */
      sectionId.push_back(-123);

      if (cgns_type_elem == CGNS_ENUMV(NGON_n)) // cas polyedre
        {
          if (is_polyedre) // Pas pour polygone
            {
              end = start + static_cast<cgsize_t>(TRUST2CGNS.get_global_nb_face_som()[indZ]) -1;
              cgsize_t maxoffset = static_cast<cgsize_t>(TRUST2CGNS.get_global_nb_face_som_offset()[indZ]);

              if (cgp_poly_section_write(fileId_, baseId_.back(), zoneId_.back(), "NGON_n", CGNS_ENUMV(NGON_n), start, end, maxoffset, 0, &sectionId.back()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_over_zone : cgp_poly_section_write !" << finl, TRUST_CGNS_ERROR();

              sectionId2.push_back(-123);
              start = end + 1;

              end = start + static_cast<cgsize_t>(TRUST2CGNS.get_global_nb_elem_face()[indZ]) -1;
              maxoffset = static_cast<cgsize_t>(TRUST2CGNS.get_global_nb_elem_face_offset()[indZ]);

              if (cgp_poly_section_write(fileId_, baseId_.back(), zoneId_.back(), "NFACE_n", CGNS_ENUMV(NFACE_n), start, end, maxoffset, 0, &sectionId2.back()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_over_zone : cgp_poly_section_write !" << finl, TRUST_CGNS_ERROR();
            }
          else // polygon
            {
              end = start + static_cast<cgsize_t>(TRUST2CGNS.get_global_nb_elem()[indZ]) -1; /* ici pareil comme get_global_nb_elem_som ... fais moi confiance ... */
              cgsize_t maxoffset = static_cast<cgsize_t>(TRUST2CGNS.get_global_nb_elem_som_offset()[indZ]);

              if (cgp_poly_section_write(fileId_, baseId_.back(), zoneId_.back(), "NGON_n", CGNS_ENUMV(NGON_n), start, end, maxoffset, 0, &sectionId.back()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_over_zone : cgp_poly_section_write !" << finl, TRUST_CGNS_ERROR();
            }
        }
      else
        {
          if (cgp_section_write(fileId_, baseId_.back(), zoneId_.back(), "Elem", cgns_type_elem, start, end, 0, &sectionId.back()) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_over_zone : cgp_section_write !" << finl, TRUST_CGNS_ERROR();
        }
    }

  zoneId_par_.push_back(zoneId_); // XXX : Dont touch

//  Process::barrier();

  /* 6 : Write grid coordinates & set connectivity */
  if (nb_elem > 0) // this proc will write !
    {
      cgsize_t min = 1, max = nb_som;
      int indx = -123;
      if (all_write) indx = proc_me;
      else
        for (int i = 0; i < nb_zones_to_write; i++)
          if (proc_non_zero_elem[i] == proc_me)
            {
              indx = i;
              break;
            }

      /* 6.1 : Write grid coordinates */
      cgns_helper_.cgns_write_grid_coord_data<TYPE_ECRITURE::PAR_OVER>(icelldim, fileId_, baseId_, zoneId_par_.back()[indx],
                                                                       coordsIdx[indx], coordsIdy[indx], coordsIdz.empty() ? -123 : coordsIdz[indx],
                                                                       min, max, xCoords, yCoords, zCoords);

      /* 6.2 : Set element connectivity */
      if (cgns_type_elem == CGNS_ENUMV(NGON_n)) // cas polyedre
        {
          if (is_polyedre) // Pas pour polygone
            {
              const std::vector<cgsize_t>& fs = TRUST2CGNS.get_local_fs(), fs_offset = TRUST2CGNS.get_local_fs_offset();
              max = min + TRUST2CGNS.get_nb_fs() - 1;

              if (cgp_poly_elements_write_data(fileId_, baseId_.back(), zoneId_par_.back()[indx], sectionId[indx], min, max, fs.data(), fs_offset.data()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_over_zone : cgp_poly_elements_write_data !" << finl, TRUST_CGNS_ERROR();

              const std::vector<cgsize_t> ef = TRUST2CGNS.get_local_ef(), ef_offset = TRUST2CGNS.get_local_ef_offset();
              min = max + 1, max = min + TRUST2CGNS.get_nb_ef() - 1;

              if (cgp_poly_elements_write_data(fileId_, baseId_.back(), zoneId_par_.back()[indx], sectionId2[indx], min, max, ef.data(), ef_offset.data()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_over_zone : cgp_poly_elements_write_data !" << finl, TRUST_CGNS_ERROR();
            }
          else
            {
              const std::vector<cgsize_t>& es = TRUST2CGNS.get_local_es(), es_offset = TRUST2CGNS.get_local_es_offset();
              max = min + TRUST2CGNS.get_nb_es() -1;

              if (cgp_poly_elements_write_data(fileId_, baseId_.back(), zoneId_par_.back()[indx], sectionId[indx], min, max, es.data(), es_offset.data()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_over_zone : cgp_poly_elements_write_data !" << finl, TRUST_CGNS_ERROR();
            }
        }
      else
        {
          std::vector<cgsize_t> elems;
          TRUST2CGNS.convert_connectivity(cgns_type_elem, elems);

          max = nb_elem; /* now we need local elem */
          if (cgp_elements_write_data(fileId_, baseId_.back(), zoneId_par_.back()[indx], sectionId[indx], min, max, elems.data()) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_over_zone : cgp_elements_write_data !" << finl, TRUST_CGNS_ERROR();
        }
    }
  TRUST2CGNS.clear_vectors();
#endif
}

void Ecrire_CGNS::cgns_write_field_par_over_zone(const int comp, const double temps, const Nom& id_du_champ, const Nom& id_du_domaine, const Nom& localisation, const Nom& nom_dom, const DoubleTab& valeurs)
{
#ifdef MPI_
  assert (!Option_CGNS::USE_LINKS);
  std::string LOC = Motcle(localisation).getString();
  Motcle id_du_champ_modifie = TRUST_2_CGNS::modify_field_name_for_post(id_du_champ, id_du_domaine, LOC, fieldId_som_, fieldId_elem_);
  Nom& id_champ = id_du_champ_modifie;

  /* 2 : Get corresponding domain index */
  const int proc_me = Process::me(), nb_vals = valeurs.dimension(0);
  const int ind = TRUST_2_CGNS::get_index_nom_vector(doms_written_, nom_dom);
  assert(ind > -1);

  /* 3 : CREATION OF FILE STRUCTURE
   *
   *  - All processors THAT HAVE nb_vals > 0 write the same information.
   *  - Only field meta-data is written to the library at this stage ... So no worries ^^
   *  - And just once per dt !
   */
  const TRUST_2_CGNS& TRUST2CGNS = T2CGNS_[ind];

  const int nb_zones_to_write = TRUST2CGNS.nb_procs_writing();
  const bool all_write = TRUST2CGNS.all_procs_write(); // all procs will write !

  cgns_helper_.cgns_sol_write<TYPE_ECRITURE::PAR_OVER>(nb_zones_to_write, fileId_, baseId_[ind], ind, temps, zoneId_par_[ind], LOC, solname_som_, solname_elem_,
                                                       solname_som_written_, solname_elem_written_, flowId_som_, flowId_elem_);

  cgns_helper_.cgns_field_write<TYPE_ECRITURE::PAR_OVER>(nb_zones_to_write, fileId_, baseId_[ind], ind, zoneId_par_[ind], LOC,
                                                         flowId_som_, flowId_elem_, id_champ.getChar(), fieldId_som_, fieldId_elem_);

  /* 4 : Fill field values & dump to cgns file */
  if (nb_vals > 0) // this proc will write !
    {
      cgsize_t min = 1, max = nb_vals;
      int indx = -123;
      const std::vector<int>& proc_non_zero_write= TRUST2CGNS.get_proc_non_zero_elem();
      if (all_write) indx = proc_me;
      else
        for (int i = 0; i < nb_zones_to_write; i++)
          if (proc_non_zero_write[i] == proc_me)
            {
              indx = i;
              break;
            }

      cgns_helper_.cgns_field_write_data<TYPE_ECRITURE::PAR_OVER>(fileId_, baseId_[ind], indx /* XXX */, zoneId_par_[ind], LOC, flowId_som_, flowId_elem_,
                                                                  fieldId_som_, fieldId_elem_, comp, min, max, valeurs);
    }
#endif
}

void Ecrire_CGNS::cgns_write_iters_par_over_zone()
{
#ifdef MPI_
//  assert(static_cast<int>(baseId_.size()) == static_cast<int>(zoneId_.size())); // XXX No not for // !!!
  std::vector<int> ind_doms_dumped;

  /* 1 : on iter juste sur le map fld_loc_map_; ie: pas domaine dis ... */
  for (auto &itr : fld_loc_map_)
    {
      const std::string& LOC = itr.first;
      const Nom& nom_dom = itr.second;
      const int ind = TRUST_2_CGNS::get_index_nom_vector(doms_written_, nom_dom);
      ind_doms_dumped.push_back(ind);
      assert(ind > -1);

      const TRUST_2_CGNS& TRUST2CGNS = T2CGNS_[ind];
      const int nb_zones_to_write = TRUST2CGNS.nb_procs_writing();

      cgns_helper_.cgns_write_iters<TYPE_ECRITURE::PAR_OVER>(true /* has_field */, nb_zones_to_write, fileId_, baseId_[ind], ind, zoneId_par_[ind], LOC, solname_som_, solname_elem_, time_post_);
    }

  /* 2 : on iter sur les autres domaines; ie: domaine dis */
  for (int i = 0; i < static_cast<int>(doms_written_.size()); i++)
    {
      if (std::find(ind_doms_dumped.begin(), ind_doms_dumped.end(), i) == ind_doms_dumped.end()) // indice pas dans ind_doms_dumped
        {
          const Nom& nom_dom = doms_written_[i];
          const int ind = TRUST_2_CGNS::get_index_nom_vector(doms_written_, nom_dom);
          assert(ind > -1);

          const TRUST_2_CGNS& TRUST2CGNS = T2CGNS_[ind];
          const int nb_zones_to_write = TRUST2CGNS.nb_procs_writing();

          cgns_helper_.cgns_write_iters<TYPE_ECRITURE::PAR_OVER>(false /* has_field */, nb_zones_to_write, fileId_, baseId_[ind], ind, zoneId_par_[ind], "rien", solname_som_, solname_elem_, time_post_);
        }
      else { /* Do Nothing */ }
    }
#endif
}

/*
 * ************************* *
 * VERSION PARALLELE IN ZONE *
 * ************************* *
 */
void Ecrire_CGNS::cgns_write_domaine_par_in_zone(const Domaine * domaine,const Nom& nom_dom, const DoubleTab& les_som, const IntTab& les_elem, const Motcle& type_elem)
{
#ifdef MPI_
  doms_written_.push_back(nom_dom);

  /* 1 : Instance of TRUST_2_CGNS */
  T2CGNS_.push_back(TRUST_2_CGNS());
  TRUST_2_CGNS& TRUST2CGNS = T2CGNS_.back();
  TRUST2CGNS.associer_domaine_TRUST(domaine, les_som, les_elem, postraiter_domaine_);
  CGNS_TYPE cgns_type_elem = TRUST2CGNS.convert_elem_type(type_elem);

  /* 2 : Fill coords */
  std::vector<double> xCoords, yCoords, zCoords;
  TRUST2CGNS.fill_coords(xCoords, yCoords, zCoords);

  const int icelldim = les_som.dimension(1), nb_elem = les_elem.dimension(0), iphysdim = Objet_U::dimension, proc_me = Process::me();

  /* 3 : Base write */
  baseId_.push_back(-123); // pour chaque dom, on a une baseId
  char basename[CGNS_STR_SIZE];
  strcpy(basename, nom_dom.getChar()); // dom name

  if (cg_base_write(fileId_, basename, icelldim, iphysdim, &baseId_.back()) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_in_zone : cg_base_write !" << finl, TRUST_CGNS_ERROR();

  /* 4 : CREATION OF FILE STRUCTURE : zones, coords & sections
   *
   *  - All processors write the same information.
   *  - XXX XXX XXX Only ONE zone meta-data is written to the library at this stage ...
   */
  const bool is_polyedre = (type_elem == "POLYEDRE" || type_elem == "PRISME" || type_elem == "PRISME_HEXAG");

  TRUST2CGNS.fill_global_infos(); // XXX

  if (cgns_type_elem == CGNS_ENUMV(NGON_n)) /*cas polygone/polyedre */
    TRUST2CGNS.fill_global_infos_poly(is_polyedre);

  const int ns_tot = TRUST2CGNS.get_ns_tot(), ne_tot = TRUST2CGNS.get_ne_tot();
  assert (ne_tot > 0 && ns_tot > 0);

  /* 4.1 : Create zone & grid */
  cgsize_t isize[3][1];
  isize[0][0] = ns_tot;
  isize[1][0] = ne_tot;
  isize[2][0] = 0; /* boundary vertex size (zero if elements not sorted) */

  if (Option_CGNS::USE_LINKS)
    cgns_fill_info_grid_link_file(basename, cgns_type_elem, icelldim, ns_tot, ne_tot, is_polyedre);

  True_int coordsIdx = -123, coordsIdy = -123, coordsIdz = -123, sectionId = -123, sectionId2 = -123;
  zoneId_.push_back(-123);

  cgns_helper_.cgns_write_zone_grid_coord<TYPE_ECRITURE::PAR_IN>(icelldim, fileId_, baseId_, basename /* Dom name */, isize[0],
                                                                 zoneId_, xCoords, yCoords, zCoords, coordsIdx, coordsIdy, coordsIdz);

  /* 4.2 : Construct the sections to host connectivity later */
  cgsize_t start = -123, end = -123;
  if (cgns_type_elem == CGNS_ENUMV(NGON_n)) // cas polyedre
    {
      cgsize_t maxoffset = -123;

      if (is_polyedre) // Pas pour polygone
        {
          const int nb_fs = TRUST2CGNS.get_nfs_tot();
          const int nb_fs_offset = TRUST2CGNS.get_nfs_offset_tot();

          start = 1, end = start + nb_fs - 1;
          maxoffset = nb_fs_offset;
          assert(start <= end);

          if (cgp_poly_section_write(fileId_, baseId_.back(), zoneId_.back(), "NGON_n", CGNS_ENUMV(NGON_n), start, end, maxoffset, 0, &sectionId) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_in_zone : cgp_poly_section_write !" << finl, TRUST_CGNS_ERROR();

          const int nb_ef = TRUST2CGNS.get_nef_tot();
          const int nb_ef_offset = TRUST2CGNS.get_nef_offset_tot();

          start = end + 1, end = start + nb_ef - 1;
          maxoffset = nb_ef_offset;
          assert(start <= end);

          if (cgp_poly_section_write(fileId_, baseId_.back(), zoneId_.back(), "NFACE_n", CGNS_ENUMV(NFACE_n), start, end, maxoffset, 0, &sectionId2) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_in_zone : cgp_poly_section_write !" << finl, TRUST_CGNS_ERROR();
        }
      else // polygon
        {
          const int nb_es = ne_tot;
          const int nb_es_offset = TRUST2CGNS.get_nes_offset_tot();

          start = 1, end = start + nb_es - 1;
          maxoffset = nb_es_offset;

          if (cgp_poly_section_write(fileId_, baseId_.back(), zoneId_.back(), "NGON_n", CGNS_ENUMV(NGON_n), start, end, maxoffset, 0, &sectionId) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_in_zone : cgp_poly_section_write !" << finl, TRUST_CGNS_ERROR();
        }
    }
  else
    {
      start = 1, end = ne_tot;
      assert(start <= end);

      if (cgp_section_write(fileId_, baseId_.back(), zoneId_.back(), "Elem", cgns_type_elem, start, end, 0, &sectionId) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_in_zone : cgp_section_write !" << finl, TRUST_CGNS_ERROR();
    }

  /* 5 : Write grid coordinates & set connectivity */
  if (nb_elem > 0) // seulement si le proc a qlq chose a ecrire
    {
      const std::vector<int>& incr_max_som = TRUST2CGNS.get_global_incr_max_som(), incr_min_som = TRUST2CGNS.get_global_incr_min_som();

      cgsize_t min = incr_min_som[proc_me], max = incr_max_som[proc_me];
      assert (min < max);

      /* 5.1 : Write grid coordinates */
      cgns_helper_.cgns_write_grid_coord_data<TYPE_ECRITURE::PAR_IN>(icelldim, fileId_, baseId_, zoneId_.back(),
                                                                     coordsIdx, coordsIdy, coordsIdz, min, max, xCoords, yCoords, zCoords);

      /* 5.2 : Set element connectivity */
      if (cgns_type_elem == CGNS_ENUMV(NGON_n)) // cas polyedre
        {
          if (is_polyedre)
            {
              const std::vector<cgsize_t>& fs = TRUST2CGNS.get_local_fs(), fs_offset = TRUST2CGNS.get_local_fs_offset();
              const std::vector<int>& incr_min_face_som = TRUST2CGNS.get_global_incr_min_face_som(), incr_max_face_som = TRUST2CGNS.get_global_incr_max_face_som();
              min = incr_min_face_som[proc_me], max = incr_max_face_som[proc_me];
              assert (min < max);

              if (cgp_poly_elements_write_data(fileId_, baseId_.back(), zoneId_.back(), sectionId, min, max, fs.data(), fs_offset.data()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_in_zone : cgp_poly_elements_write_data !" << finl, TRUST_CGNS_ERROR();

              const std::vector<cgsize_t> ef = TRUST2CGNS.get_local_ef(), ef_offset = TRUST2CGNS.get_local_ef_offset();
              const std::vector<int>& incr_min_elem_face = TRUST2CGNS.get_global_incr_min_elem_face(), incr_max_elem_face = TRUST2CGNS.get_global_incr_max_elem_face();
              min = incr_max_face_som.back() + incr_min_elem_face[proc_me]; // BOOM
              max = incr_max_face_som.back() + incr_max_elem_face[proc_me]; // BEEM
              assert (min < max);

              if (cgp_poly_elements_write_data(fileId_, baseId_.back(), zoneId_.back(), sectionId2, min, max, ef.data(), ef_offset.data()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_in_zone : cgp_poly_elements_write_data !" << finl, TRUST_CGNS_ERROR();
            }
          else
            {
              const std::vector<cgsize_t>& es = TRUST2CGNS.get_local_es(), es_offset = TRUST2CGNS.get_local_es_offset();
              const std::vector<int>& incr_max_elem = TRUST2CGNS.get_global_incr_max_elem(), incr_min_elem = TRUST2CGNS.get_global_incr_min_elem();
              min = incr_min_elem[proc_me], max = incr_max_elem[proc_me];
              assert (min < max);

              if (cgp_poly_elements_write_data(fileId_, baseId_.back(), zoneId_.back(), sectionId, min, max, es.data(), es_offset.data()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_in_zone : cgp_poly_elements_write_data !" << finl, TRUST_CGNS_ERROR();
            }
        }
      else
        {
          std::vector<cgsize_t> elems;
          TRUST2CGNS.convert_connectivity(cgns_type_elem, elems);

          const std::vector<int>& incr_max_elem = TRUST2CGNS.get_global_incr_max_elem(), incr_min_elem = TRUST2CGNS.get_global_incr_min_elem();
          min = incr_min_elem[proc_me], max = incr_max_elem[proc_me];
          assert (min < max);

          if (cgp_elements_write_data(fileId_, baseId_.back(), zoneId_.back(), sectionId, min, max, elems.data()) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_domaine_par_in_zone : cgp_elements_write_data !" << finl, TRUST_CGNS_ERROR();
        }
    }
  TRUST2CGNS.clear_vectors();
#endif
}

void Ecrire_CGNS::cgns_write_field_par_in_zone(const int comp, const double temps, const Nom& id_du_champ, const Nom& id_du_domaine, const Nom& localisation, const Nom& nom_dom, const DoubleTab& valeurs)
{
#ifdef MPI_
  const int proc_me = Process::me(), nb_vals = valeurs.dimension(0);
  const int ind = TRUST_2_CGNS::get_index_nom_vector(doms_written_, nom_dom);
  assert(ind > -1);

  std::string LOC = Motcle(localisation).getString();
  Motcle id_du_champ_modifie = TRUST_2_CGNS::modify_field_name_for_post(id_du_champ, id_du_domaine, LOC, fieldId_som_, fieldId_elem_);
  Nom& id_champ = id_du_champ_modifie;

  /* quel fileID ?? */
  True_int fileId = fileId_;
  if (Option_CGNS::USE_LINKS && !postraiter_domaine_)
    TRUST_2_CGNS::modify_fileId_for_post(fld_loc_map_, LOC, fileId2_, fileId);

  /* 1 : CREATION OF FILE STRUCTURE
   *
   *  - All processors write the same information.
   *  - Only field meta-data is written to the library at this stage ... So no worries ^^
   *  - And just once per dt !
   */
  cgns_helper_.cgns_sol_write<TYPE_ECRITURE::PAR_IN>(1 /* nb_zones_to_write */, fileId, baseId_[ind], ind, temps, zoneId_, LOC, solname_som_, solname_elem_,
                                                     solname_som_written_, solname_elem_written_, flowId_som_, flowId_elem_);

  cgns_helper_.cgns_field_write<TYPE_ECRITURE::PAR_IN>(1 /* nb_zones_to_write */, fileId, baseId_[ind], ind, zoneId_, LOC,
                                                       flowId_som_, flowId_elem_, id_champ.getChar(), fieldId_som_, fieldId_elem_);

  /* 2 : Fill field values & dump to cgns file */
  if (nb_vals > 0) // this proc will write !
    {
      cgsize_t min = -123, max = -123;
      const TRUST_2_CGNS& TRUST2CGNS = T2CGNS_[ind];

      if (LOC == "SOM")
        {
          const std::vector<int>& incr_max_som = TRUST2CGNS.get_global_incr_max_som(), incr_min_som = TRUST2CGNS.get_global_incr_min_som();
          min = incr_min_som[proc_me], max = incr_max_som[proc_me];
        }
      else
        {
          const std::vector<int>& incr_max_elem = TRUST2CGNS.get_global_incr_max_elem(), incr_min_elem = TRUST2CGNS.get_global_incr_min_elem();
          min = incr_min_elem[proc_me], max = incr_max_elem[proc_me];
        }

      cgns_helper_.cgns_field_write_data<TYPE_ECRITURE::PAR_IN>(fileId, baseId_[ind], ind, zoneId_, LOC, flowId_som_, flowId_elem_,
                                                                fieldId_som_, fieldId_elem_, comp, min, max, valeurs);
    }
#endif
}

void Ecrire_CGNS::cgns_write_iters_par_in_zone()
{
  cgns_write_iters_seq();
}

/*
 * ***************** *
 * METHODS POUR LINK *
 * ***************** *
 */
void Ecrire_CGNS::cgns_fill_info_grid_link_file(const char* basename, const CGNS_TYPE& cgns_type_elem, const int icelldim, const int nb_som, const int nb_elem, const bool is_polyedre)
{
  if (connectname_.empty())
    {
      cellDim_ = icelldim;
      baseZone_name_ = std::string(basename);
      sizeId_ = { nb_som, nb_elem };
      if (cgns_type_elem == CGNS_ENUMV(NGON_n)) // cas polyedre
        {
          if (is_polyedre)
            connectname_ = { "NGON_n", "NFACE_n" };
          else
            connectname_.push_back("NGON_n");
        }
      else
        connectname_.push_back("Elem");
    }
}

void Ecrire_CGNS::cgns_open_close_link_files(const double t)
{
  if (grid_file_opened_)
    {
      cgns_close_grid_solution_link_file(0 /* only one index here */, baseFile_name_ + ".grid.cgns", true);
      grid_file_opened_ = false;
    }

  if (!time_post_.empty()) /* 1er fois, on fais dans cgns_write_field => fill field_loc_map */
    {
      assert ((static_cast<int>(fld_loc_map_.size()) <= 2)); // ELEM, SOM au max pour le moment
      for (auto itr = fld_loc_map_.begin(); itr != fld_loc_map_.end(); ++itr)
        {
          const int ind = static_cast<int>(std::distance(fld_loc_map_.begin(), itr));
          cgns_close_grid_solution_link_file(ind, baseFile_name_);
          cgns_open_solution_link_file(ind, itr->first, t);
        }
    }
}

void Ecrire_CGNS::cgns_open_grid_base_link_file()
{
  assert(Option_CGNS::USE_LINKS && !postraiter_domaine_);
  std::string fn = baseFile_name_ + ".grid.cgns"; // file name
  unlink(fn.c_str());
  if (Process::is_parallel())
    {
#ifdef MPI_
      if (cgp_open(fn.c_str(), CG_MODE_WRITE, &fileId_) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_open_grid_file : cgp_open !" << finl, TRUST_CGNS_ERROR();

      Cerr << "**** Parallel CGNS file " << fn << " opened !" << finl;
#endif
    }
  else
    {
      if (cg_open(fn.c_str(), CG_MODE_WRITE, &fileId_) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_open_grid_file : cg_open !" << finl, TRUST_CGNS_ERROR();

      Cerr << "**** CGNS file " << fn << " opened !" << finl;
    }
}

void Ecrire_CGNS::cgns_open_solution_link_file(const int ind, const std::string& LOC, const double t, bool is_link)
{
  assert(Option_CGNS::USE_LINKS && !postraiter_domaine_);
  const bool mult_loc = (static_cast<int>(fld_loc_map_.size()) > 1);

  std::string fn;
  True_int& fileId = (ind == 0 ? fileId_ : fileId2_); // XXX : ref

  if (is_link)
    fn = !mult_loc ? baseFile_name_ + ".cgns" : baseFile_name_ + "_" + LOC + ".cgns"; // file name
  else
    fn = baseFile_name_ + "_" + LOC + ".solution." + cgns_helper_.convert_double_to_string(t) + ".cgns"; // file name

  unlink(fn.c_str());

  if (Process::is_parallel())
    {
#ifdef MPI_
      if (cgp_open(fn.c_str(), CG_MODE_WRITE, &fileId) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_open_solution_file : cgp_open !" << finl, TRUST_CGNS_ERROR();

      if (is_link)
        Cerr << "**** Parallel CGNS file " << fn << " opened !" << finl;
#endif
    }
  else
    {
      if (cg_open(fn.c_str(), CG_MODE_WRITE, &fileId) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_open_solution_file : cg_open !" << finl, TRUST_CGNS_ERROR();

      if (is_link)
        Cerr << "**** CGNS file " << fn << " opened !" << finl;
    }

  if (cg_base_write(fileId, baseZone_name_.c_str(), cellDim_, Objet_U::dimension, &baseId_[0]) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_open_solution_file : cg_base_write !" << finl, TRUST_CGNS_ERROR();

  cgsize_t isize[3][1];
  isize[0][0] = sizeId_[0];
  isize[1][0] = sizeId_[1];
  isize[2][0] = 0;

  if (cg_zone_write(fileId, baseId_[0], baseZone_name_.c_str() /* Dom name */, isize[0], CGNS_ENUMV(Unstructured), &zoneId_[0]) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_write_domaine_seq : cgns_open_solution_file !" << finl, TRUST_CGNS_ERROR();

  std::string linkfile = baseFile_name_ + ".grid.cgns"; // file name
  linkfile = TRUST_2_CGNS::remove_slash_linkfile(linkfile);

  std::string linkpath = "/" + baseZone_name_ + "/" + baseZone_name_ + "/GridCoordinates/";

  if (cg_goto(fileId, baseId_[0], "Zone_t", 1, "end") != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_open_solution_file : cg_goto !" << finl, TRUST_CGNS_ERROR();

  if (cg_link_write("GridCoordinates", linkfile.c_str(), linkpath.c_str()) != CG_OK)
    Cerr << "Error Ecrire_CGNS::cgns_open_solution_file : cg_link_write !" << finl, TRUST_CGNS_ERROR();

  for (auto &itr : connectname_)
    {
      linkpath = "/" + baseZone_name_ + "/" + baseZone_name_ + "/" + itr + "/";

      if (cg_link_write(itr.c_str(), linkfile.c_str(), linkpath.c_str()) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_open_solution_file : cg_link_write !" << finl, TRUST_CGNS_ERROR();
    }
}

void Ecrire_CGNS::cgns_close_grid_solution_link_file(const int ind, const std::string& fn, bool is_cerr)
{
  assert(Option_CGNS::USE_LINKS && !postraiter_domaine_);
  const True_int fileId = (ind == 0 ? fileId_ : fileId2_);

  if (Process::is_parallel())
    {
#ifdef MPI_
      if (cgp_close(fileId) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_close_solution_file : cgp_close !" << finl, TRUST_CGNS_ERROR();

      if (is_cerr) Cerr << "**** Parallel CGNS file " << fn << " closed !" << finl;
#endif
    }
  else
    {
      if (cg_close(fileId) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_close_solution_file : cg_close !" << finl, TRUST_CGNS_ERROR();

      if (is_cerr) Cerr << "**** CGNS file " << fn << " closed !" << finl;
    }
}

void Ecrire_CGNS::cgns_write_final_link_file()
{
  const bool mult_loc = (static_cast<int>(fld_loc_map_.size()) > 1);

  for (auto itr = fld_loc_map_.begin(); itr != fld_loc_map_.end(); ++itr)
    {
      const int ind = static_cast<int>(std::distance(fld_loc_map_.begin(), itr));

      // XXX a pas oublier, dernier sol fichier ... faut le fermer
      cgns_close_grid_solution_link_file(ind, baseFile_name_);

      // Fichier link maintenant
      const std::string& LOC = itr->first;
      cgns_open_solution_link_file(ind, LOC, -123., true /* dernier fichier => link */);

      const True_int fileId = (ind == 0 ? fileId_ : fileId2_);

      // link solutions
      for (auto& itr_t : time_post_)
        {
          std::string solname = "FlowSolution" + cgns_helper_.convert_double_to_string(itr_t) + "_" + LOC;

          std::string linkfile = baseFile_name_ + "_" + LOC + ".solution." + cgns_helper_.convert_double_to_string(itr_t) + ".cgns"; // file name
          linkfile = TRUST_2_CGNS::remove_slash_linkfile(linkfile);

          std::string linkpath = "/" + baseZone_name_ + "/" + baseZone_name_ + "/" + solname + "/";

          if (cg_link_write(solname.c_str(), linkfile.c_str(), linkpath.c_str()) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_final_link_file : cg_link_write !" << finl, TRUST_CGNS_ERROR();
        }

      cgns_helper_.cgns_write_iters<TYPE_ECRITURE::SEQ>(true /* has_field */, 1 /* nb_zones_to_write */, fileId, baseId_[0], 0 /* 1st Zone */, zoneId_, LOC, solname_som_, solname_elem_, time_post_);

      cgns_close_grid_solution_link_file(ind, !mult_loc ? baseFile_name_ + ".cgns" : baseFile_name_ + "_" + LOC + ".cgns", true); // on ferme
    }
}

void Ecrire_CGNS::cgns_write_link_file_for_multiple_files()
{
  if (postraiter_domaine_) return; /* Do nothing */

  Process::barrier();
  if (Process::je_suis_maitre()) // Only master proc writes !
    {
      std::string fn = baseFile_name_ + ".cgns"; // file name
      Cerr << "Option_CGNS::MULTIPLE_FILES is used ... so we write a unique link file " << fn << " ..." << finl;

      True_int fileId_l = -123, baseId_l = -123, zoneId_l = -123;
      True_int fileId = -123, baseId = 1, zoneId = 1, cell_dim = -123, phys_dim = -123;
      True_int nbndry = -123, iparent_flag = -123, nsols = -123, nsections = -123;
      char basename[CGNS_STR_SIZE], zonename[CGNS_STR_SIZE], sectionname[CGNS_STR_SIZE], solname[CGNS_STR_SIZE];

      cgsize_t isize[3][1], istart, iend;
      std::vector<std::string> connectname, sols;
      CGNS_TYPE itype;
      CGNS_LOC loc;

      /* Step 1 : on ouvre baseFile_name_0000.cgns et on lit */
      fn = (Nom(baseFile_name_)).nom_me(0).getString() + ".cgns"; // file name

      if (cg_open(fn.c_str(), CG_MODE_READ, &fileId) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_open !" << finl, TRUST_CGNS_ERROR();

      if (cg_base_read(fileId, baseId, basename, &cell_dim, &phys_dim) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_base_read !" << finl, TRUST_CGNS_ERROR();

      if (cg_zone_read(fileId, baseId, zoneId, zonename, isize[0]) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_zone_read !" << finl, TRUST_CGNS_ERROR();

      if (cg_nsections(fileId, baseId, zoneId, &nsections) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_nsections !" << finl, TRUST_CGNS_ERROR();

      for (int index_sect = 1; index_sect <= nsections; index_sect++)
        {
          if (cg_section_read(fileId, baseId, zoneId, index_sect, sectionname, &itype, &istart, &iend, &nbndry, &iparent_flag) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_section_read !" << finl, TRUST_CGNS_ERROR();

          connectname.push_back(std::string(sectionname));
        }

      if (cg_nsols(fileId, baseId, zoneId, &nsols) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_nsols !" << finl, TRUST_CGNS_ERROR();

      for (int i = 1; i <= nsols; i++)
        {
          if (cg_sol_info(fileId, baseId, zoneId, i, solname, &loc) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_sol_info !" << finl, TRUST_CGNS_ERROR();

          sols.push_back(std::string(solname));
        }

      if (cg_close(fileId) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_close !" << finl, TRUST_CGNS_ERROR();

      /* Step 2 : on ouvre le link file, on laisse ouvert et on ecrit dans la premiere noeud ... */
      fn = baseFile_name_ + ".cgns"; // file name
      unlink(fn.c_str());

      if (cg_open(fn.c_str(), CG_MODE_WRITE, &fileId_l) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_open !" << finl, TRUST_CGNS_ERROR();

      Cerr << "**** CGNS file " << fn << " opened !" << finl;

      if (cg_base_write(fileId_l, basename, cell_dim, phys_dim, &baseId_l) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_base_write !" << finl, TRUST_CGNS_ERROR();

      if (cg_biter_write(fileId_l, baseId_l, "TimeIterValues", static_cast<int>(sols.size())) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file : cg_biter_write !" << finl, TRUST_CGNS_ERROR();

      if (cg_goto(fileId_l, baseId_l, "BaseIterativeData_t", 1, "end") != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file : cg_goto !" << finl, TRUST_CGNS_ERROR();

      std::string linkfile = (Nom(baseFile_name_)).nom_me(0).getString() + ".cgns"; // file name
      linkfile = TRUST_2_CGNS::remove_slash_linkfile(linkfile);

      std::string linkpath = "/" + std::string(basename) + "/TimeIterValues/TimeValues/";

      if (cg_link_write("TimeValues", linkfile.c_str(), linkpath.c_str()) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_link_write !" << finl, TRUST_CGNS_ERROR();

      if (cg_simulation_type_write(fileId_l, baseId_l, CGNS_ENUMV(TimeAccurate)) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file : cg_simulation_type_write !" << finl, TRUST_CGNS_ERROR();

      /* Step 3 : On lit tous les fichiers pour recuperer sizes, et on ecrit les links */
      for (int proc = 0; proc < Process::nproc(); proc++)
        {
          /* on lit les cgns files */
          fn = (Nom(baseFile_name_)).nom_me(proc).getString() + ".cgns"; // file name

          if (cg_open(fn.c_str(), CG_MODE_READ, &fileId) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_open !" << finl, TRUST_CGNS_ERROR();

          if (cg_zone_read(fileId, baseId, zoneId, zonename, isize[0]) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_zone_read !" << finl, TRUST_CGNS_ERROR();

          if (cg_close(fileId) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_close !" << finl, TRUST_CGNS_ERROR();

          /* on ecrit dans le link file : 2eme noeud sous domaine */
          Nom new_zonename = Nom(zonename).nom_me(proc);
          linkfile = fn; // file name

          if (cg_zone_write(fileId_l, baseId_l, new_zonename.getChar(), isize[0], CGNS_ENUMV(Unstructured), &zoneId_l) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_base_write !" << finl, TRUST_CGNS_ERROR();

          linkpath = "/" + std::string(basename) + "/" + std::string(zonename) + "/GridCoordinates/";

          if (cg_goto(fileId_l, baseId_l, "Zone_t", proc + 1, "end") != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_goto !" << finl, TRUST_CGNS_ERROR();

          if (cg_link_write("GridCoordinates", linkfile.c_str(), linkpath.c_str()) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_link_write !" << finl, TRUST_CGNS_ERROR();

          for (auto &itr : connectname)
            {
              linkpath = "/" + std::string(basename) + "/" + std::string(zonename) + "/" + itr + "/";
              if (cg_link_write(itr.c_str(), linkfile.c_str(), linkpath.c_str()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_link_write !" << finl, TRUST_CGNS_ERROR();
            }

          // link solutions
          for (auto &itr : sols)
            {
              linkpath = "/" + std::string(basename) + "/" + std::string(zonename) + "/" + itr + "/";
              if (cg_link_write(itr.c_str(), linkfile.c_str(), linkpath.c_str()) != CG_OK)
                Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_link_write !" << finl, TRUST_CGNS_ERROR();
            }

          if (cg_ziter_write(fileId_l, baseId_l, zoneId_l, "ZoneIterativeData") != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_ziter_write !" << finl, TRUST_CGNS_ERROR();

          if (cg_goto(fileId_l, baseId_l, "Zone_t", zoneId_l, "ZoneIterativeData_t", 1, "end") != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_goto !" << finl, TRUST_CGNS_ERROR();

          linkpath = "/" + std::string(basename) + "/" + std::string(zonename) + "/ZoneIterativeData/FlowSolutionPointers/";
          if (cg_link_write("FlowSolutionPointers", linkfile.c_str(), linkpath.c_str()) != CG_OK)
            Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_link_write !" << finl, TRUST_CGNS_ERROR();
        }

      /* Step 4 : on ferme le link file, on laisse ouvert et on ecrit la base */
      if (cg_close(fileId_l) != CG_OK)
        Cerr << "Error Ecrire_CGNS::cgns_write_link_file_for_multiple_files : cg_close !" << finl, TRUST_CGNS_ERROR();

      Cerr << "**** CGNS file " << baseFile_name_ + ".cgns" << " closed !" << finl;
    }
}

#endif /* HAS_CGNS */
