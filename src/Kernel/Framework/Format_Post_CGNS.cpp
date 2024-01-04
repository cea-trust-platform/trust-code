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

#include <Format_Post_CGNS.h>
#include <Comm_Group_MPI.h>
#include <TRUST_2_CGNS.h>
#include <Param.h>

#ifdef HAS_CGNS
#define CGNS_STR_SIZE 32
#endif

#ifdef MPI_
#ifdef INT_is_64_
#define MPI_ENTIER MPI_LONG
#else
#define MPI_ENTIER MPI_INT
#endif
#endif

Implemente_instanciable_sans_constructeur(Format_Post_CGNS, "Format_Post_CGNS", Format_Post_base);

Format_Post_CGNS::Format_Post_CGNS()
{
  reset();
}

void Format_Post_CGNS::reset()
{
  cgns_basename_ = "??";
}

Sortie& Format_Post_CGNS::printOn(Sortie& os) const
{
  Process::exit("Format_Post_CGNS::printOn - Should not be called ! ");
  return os;
}

Entree& Format_Post_CGNS::readOn(Entree& is)
{
  verify_if_cgns(__func__);
  return Format_Post_base::readOn(is);
}

void Format_Post_CGNS::set_param(Param& param)
{
  verify_if_cgns(__func__);
  param.ajouter("nom_fichier", &cgns_basename_, Param::REQUIRED);
}

int Format_Post_CGNS::initialize_by_default(const Nom& file_basename)
{
  verify_if_cgns(__func__);
  cgns_basename_ = file_basename;
  return 1;
}

int Format_Post_CGNS::initialize(const Nom& file_basename, const int format, const Nom& option_para)
{
  verify_if_cgns(__func__);
  cgns_basename_ = file_basename;
  return 1;
}

int Format_Post_CGNS::ecrire_entete(const double temps_courant,const int reprise,const int est_le_premier_post)
{
  verify_if_cgns(__func__);
#ifdef HAS_CGNS
  if (est_le_premier_post)
    {
      std::string fn = cgns_basename_.getString() + ".cgns"; // file name
      if (is_parallel())
        {
          cgp_mpi_comm(Comm_Group_MPI::get_trio_u_world()); // initialise MPI_COMM_WORLD

          /*
           * Elie Saikali XXX NOTA BENE XXX
           * We need sometimes to write planes, boundaries where some processors have no elements/vertices ...
           * This wont work because the default PIO mode of CGNS is COLLECTIVE. We want it to be INDEPENDENT !
           */
          if (cgp_pio_mode((CGNS_ENUMT(PIOmode_t)) CGP_INDEPENDENT) != CG_OK)
            Cerr << "Error Format_Post_CGNS::ecrire_entete : cgp_pio_mode !" << finl, cgp_error_exit();

          if (cgp_open(fn.c_str(), CG_MODE_WRITE, &fileId_) != CG_OK)
            Cerr << "Error Format_Post_CGNS::ecrire_entete : cgp_open !" << finl, cgp_error_exit();
        }
      else
        {
          if (cg_open(fn.c_str(), CG_MODE_WRITE, &fileId_) != CG_OK)
            Cerr << "Error Format_Post_CGNS::ecrire_entete : cg_open !" << finl, cg_error_exit();
        }
      Cerr << "**** CGNS file " << fn << " opened !" << finl;
    }
#endif
  return 1;
}

int Format_Post_CGNS::ecrire_temps(const double t)
{
#ifdef HAS_CGNS
  time_post_.push_back(t); // add time_post
  flowId_elem_++, flowId_som_++; // increment
  fieldId_elem_ = 0, fieldId_som_ = 0; // reset
  solname_elem_written_ = false, solname_som_written_ = false; // reset
#endif
  return 1;
}

int Format_Post_CGNS::finir(const int est_le_dernier_post)
{
#ifdef HAS_CGNS
  if (est_le_dernier_post)
    {
      if (is_parallel()) finir_par_();
      else finir_();
    }
#endif
  return 1;
}

int Format_Post_CGNS::ecrire_domaine(const Domaine& domaine, const int est_le_premier_post)
{
#ifdef HAS_CGNS
  if (is_parallel())
    ecrire_domaine_par_(domaine, domaine.le_nom());
  else
    ecrire_domaine_(domaine, domaine.le_nom());

  // Si on a des frontieres domaine, on les ecrit egalement
  const LIST(REF(Domaine)) &bords = domaine.domaines_frontieres();
  for (auto &itr : bords) ecrire_domaine(itr.valeur(), est_le_premier_post);
#endif
  return 1;
}

int Format_Post_CGNS::ecrire_champ(const Domaine& domaine, const Noms& unite_, const Noms& noms_compo, int ncomp, double temps,
                                   const Nom& id_du_champ, const Nom& id_du_domaine, const Nom& localisation,
                                   const Nom& nature, const DoubleTab& valeurs)
{
#ifdef HAS_CGNS
  const std::string LOC = Motcle(localisation).getString();
  /* 1 : if first time called ... build different supports for mixed locations */
  if (!is_parallel()) // TODO FIXME
    if (static_cast<int>(time_post_.size()) == 1)
      {
        if (static_cast<int>(fld_loc_map_.size()) == 0)
          fld_loc_map_.insert( { LOC , domaine.le_nom() });/* ici on utilise le 1er support */
        else
          {
            const bool in_map = (fld_loc_map_.count(LOC) != 0);
            if (!in_map) // XXX here we need a new support ... sorry
              {
                Nom nom_dom = domaine.le_nom();
                nom_dom += "_";
                nom_dom += LOC;
                Cerr << "Building new CGNS zone to host the field located at : " << LOC << " !" << finl;
                ecrire_domaine_(domaine, nom_dom);
                fld_loc_map_.insert( { LOC, nom_dom } );
              }
          }
      }

  /* 2 : on ecrit */
  const int nb_cmp = valeurs.dimension(1);

  if (is_parallel())
    for (int i = 0; i < nb_cmp; i++)
      ecrire_champ_par_(i /* compo */, temps, nb_cmp > 1 ? Motcle(noms_compo[i]) : id_du_champ, localisation, domaine.le_nom(),  valeurs);
  else
    for (int i = 0; i < nb_cmp; i++)
      ecrire_champ_(i /* compo */, temps, nb_cmp > 1 ? Motcle(noms_compo[i]) : id_du_champ, localisation, fld_loc_map_.at(LOC),  valeurs);
#endif
  return 1;
}

/*
 * ****************************************
 * METHODES PRIVEES CLASSE Format_Post_CGNS
 * ****************************************
 */

#ifdef HAS_CGNS

int Format_Post_CGNS::get_index_nom_vector(const std::vector<Nom>& vect, const Nom& nom)
{
  int ind = -1;
  auto it = find(vect.begin(), vect.end(), nom);

  if (it != vect.end()) // element found
    ind = it - vect.begin(); // XXX sinon utilse std::distance ...

  return ind;
}

/*
 * *********************
 * Pour ecriture domaine
 * *********************
 */
void Format_Post_CGNS::ecrire_domaine_par_(const Domaine& domaine, const Nom& nom_dom)
{
  /* 1 : Instance of TRUST_2_CGNS */
  TRUST_2_CGNS TRUST2CGNS;
  TRUST2CGNS.associer_domaine_TRUST(domaine);
  doms_written_.push_back(nom_dom);

  Motcle type_elem = domaine.type_elem().valeur().que_suis_je();
  CGNS_TYPE cgns_type_elem = TRUST2CGNS.convert_elem_type(type_elem);

  /* 2 : Fill coords */
  std::vector<double> xCoords, yCoords, zCoords;
  TRUST2CGNS.fill_coords(xCoords, yCoords, zCoords);

  const int icelldim = domaine.les_sommets().dimension(1), iphysdim = Objet_U::dimension;
  const int nb_som = domaine.nb_som(), nb_elem = domaine.nb_elem();
  const int nb_procs = Process::nproc(), proc_me = Process::me();

  /* 3 : Base write */
  baseId_.push_back(-123); // pour chaque dom, on a une baseId
  char basename[CGNS_STR_SIZE];
  strcpy(basename, nom_dom.getChar()); // dom name

  if (cg_base_write(fileId_, basename, icelldim, iphysdim, &baseId_.back()) != CG_OK)
    Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cg_base_write !" << finl, cgp_error_exit();

  /* 4 : We need global nb_elems/nb_soms => MPI_Allgather. Thats the only information required ! */
  std::vector<int> global_nb_elem, global_nb_som;
  global_nb_elem.assign(nb_procs, -123 /* default */);
  global_nb_som.assign(nb_procs, -123 /* default */);

#ifdef MPI_
//  grp.all_gather(&nb_elem, global_nb_elem.data(), 1); // Elie : pas MPI_CHAR desole
  MPI_Allgather(&nb_elem, 1, MPI_ENTIER, global_nb_elem.data(), 1, MPI_ENTIER, MPI_COMM_WORLD);
  MPI_Allgather(&nb_som, 1, MPI_ENTIER, global_nb_som.data(), 1, MPI_ENTIER, MPI_COMM_WORLD);
#endif

  /* 5 : CREATION OF FILE STRUCTURE : zones, coords & sections
   *
   *  - All processors THAT HAVE nb_elem > 0 write the same information.
   *  - Only zone meta-data is written to the library at this stage ... So no worries ^^
   */
  std::vector<int> coordsIdx, coordsIdy, coordsIdz, sectionId, proc_non_zero_elem;
  std::string zonename;
  int gridId;

  const auto min_nb_elem = std::min_element(global_nb_elem.begin(), global_nb_elem.end());
  int nb_zones_to_write = nb_procs;

  if (*min_nb_elem <= 0) // not all procs will write !
    {
      // remplir proc_non_zero_elem avec le numero de proc si nb_elem > 0 !!
      for (int i = 0; i < static_cast<int>(global_nb_elem.size()); i++)
        if (global_nb_elem[i] > 0) proc_non_zero_elem.push_back(i);

      nb_zones_to_write = static_cast<int>(proc_non_zero_elem.size());
    }

  const bool all_write = proc_non_zero_elem.empty(); // all procs will write !

  // on boucle seulement sur les procs qui n'ont pas des nb_elem 0
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
      zonename = nom_dom.getString() + "_" + std::to_string(indZ);
      zonename.resize(CGNS_STR_SIZE, ' ');

      /* 5.1 : Create zone */
      if (cg_zone_write(fileId_, baseId_.back(), zonename.c_str(), isize[0], CGNS_ENUMV(Unstructured), &zoneId_.back()) != CG_OK)
        Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cg_zone_write !" << finl, cgp_error_exit();

      if (cg_grid_write(fileId_, baseId_.back(), zoneId_.back(), "GridCoordinates", &gridId) != CG_OK)
        Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cg_grid_write !" << finl, cgp_error_exit();

      /* 5.2 : Construct the grid coordinates nodes */
      coordsIdx.push_back(-123), coordsIdy.push_back(-123);
      if (cgp_coord_write(fileId_, baseId_.back(), zoneId_.back(), CGNS_ENUMV(RealDouble), "CoordinateX", &coordsIdx.back()) != CG_OK)
        Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cgp_coord_write - X !" << finl, cgp_error_exit();

      if (cgp_coord_write(fileId_, baseId_.back(), zoneId_.back(), CGNS_ENUMV(RealDouble), "CoordinateY", &coordsIdy.back()) != CG_OK)
        Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cgp_coord_write - Y !" << finl, cgp_error_exit();

      if (icelldim > 2)
        {
          coordsIdz.push_back(-123);
          if (cgp_coord_write(fileId_, baseId_.back(), zoneId_.back(), CGNS_ENUMV(RealDouble), "CoordinateZ", &coordsIdz.back()) != CG_OK)
            Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cgp_coord_write - Z !" << finl, cgp_error_exit();
        }

      /* 5.3 : Construct the sections to host connectivity later */
      sectionId.push_back(-123);
      if (cgp_section_write(fileId_, baseId_.back(), zoneId_.back(), "Elem", cgns_type_elem, start, end, 0, &sectionId.back()) != CG_OK)
        Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cgp_section_write !" << finl, cgp_error_exit();
    }

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
      if (cgp_coord_write_data(fileId_, baseId_.back(), zoneId_[indx], coordsIdx[indx], &min, &max, xCoords.data()) != CG_OK)
        Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cgp_coord_write_data - X !" << finl, cgp_error_exit();

      if (cgp_coord_write_data(fileId_, baseId_.back(), zoneId_[indx], coordsIdy[indx], &min, &max, yCoords.data()) != CG_OK)
        Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cgp_coord_write_data - Y !" << finl, cgp_error_exit();

      if (icelldim > 2)
        if (cgp_coord_write_data(fileId_, baseId_.back(), zoneId_[indx], coordsIdz[indx], &min, &max, zCoords.data()) != CG_OK)
          Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cgp_coord_write_data - Z !" << finl, cgp_error_exit();

      /* 6.2 : Set element connectivity */
      std::vector<cgsize_t> elems;
      TRUST2CGNS.convert_connectivity(cgns_type_elem, elems);

      max = nb_elem; /* now we need local elem */
      if (cgp_elements_write_data(fileId_, baseId_.back(), zoneId_[indx], sectionId[indx], min, max, elems.data()) != CG_OK)
        Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cgp_elements_write_data !" << finl, cgp_error_exit();
    }
}

void Format_Post_CGNS::ecrire_domaine_(const Domaine& domaine, const Nom& nom_dom)
{
  /* 1 : Instance of TRUST_2_CGNS */
  TRUST_2_CGNS TRUST2CGNS;
  TRUST2CGNS.associer_domaine_TRUST(domaine);
  doms_written_.push_back(nom_dom);

  Motcle type_elem = domaine.type_elem().valeur().que_suis_je();
  CGNS_TYPE cgns_type_elem = TRUST2CGNS.convert_elem_type(type_elem);

  /* 2 : Fill coords */
  std::vector<double> xCoords, yCoords, zCoords;
  TRUST2CGNS.fill_coords(xCoords, yCoords, zCoords);

  const int icelldim = domaine.les_sommets().dimension(1), iphysdim = Objet_U::dimension, nb_som = domaine.nb_som(), nb_elem = domaine.nb_elem();
  int coordsId;

  /* 3 : Base write */
  baseId_.push_back(-123); // pour chaque dom, on a une baseId
  char basename[CGNS_STR_SIZE];
  strcpy(basename, nom_dom.getChar()); // dom name

  if (cg_base_write(fileId_, basename, icelldim, iphysdim, &baseId_.back()) != CG_OK)
    Cerr << "Error Format_Post_CGNS::ecrire_domaine_ : cg_base_write !" << finl, cg_error_exit();

  /* 4 : Vertex, cell & boundary vertex sizes */
  cgsize_t isize[3][1];
  isize[0][0] = nb_som;
  isize[1][0] = nb_elem;
  isize[2][0] = 0; /* boundary vertex size (zero if elements not sorted) */

  /* 5 : Create zone */
  zoneId_.push_back(-123);
  if (cg_zone_write(fileId_, baseId_.back(), basename /* Dom name */, isize[0], CGNS_ENUMV(Unstructured), &zoneId_.back()) != CG_OK)
    Cerr << "Error Format_Post_CGNS::ecrire_domaine_ : cg_zone_write !" << finl, cg_error_exit();

  /* 6 : Write grid coordinates */
  if (cg_coord_write(fileId_, baseId_.back(), zoneId_.back(), CGNS_ENUMV(RealDouble), "CoordinateX", xCoords.data(), &coordsId) != CG_OK)
    Cerr << "Error Format_Post_CGNS::ecrire_domaine_ : cg_coord_write - X !" << finl, cg_error_exit();

  if (cg_coord_write(fileId_, baseId_.back(), zoneId_.back(), CGNS_ENUMV(RealDouble), "CoordinateY", yCoords.data(), &coordsId) != CG_OK)
    Cerr << "Error Format_Post_CGNS::ecrire_domaine_ : cg_coord_write - Y !" << finl, cg_error_exit();

  if (icelldim > 2)
    if (cg_coord_write(fileId_, baseId_.back(), zoneId_.back(), CGNS_ENUMV(RealDouble), "CoordinateZ", zCoords.data(), &coordsId) != CG_OK)
      Cerr << "Error Format_Post_CGNS::ecrire_domaine_ : cg_coord_write - Z !" << finl, cg_error_exit();

  /* 7 : Set element connectivity */
  std::vector<cgsize_t> elems;
  cgsize_t start = 1, end;
  int nsom = TRUST2CGNS.convert_connectivity(cgns_type_elem, elems);
  end = start + static_cast<cgsize_t>(elems.size()) / nsom - 1;

  /* 8 : Write domaine */
  int sectionId;
  if (cg_section_write(fileId_, baseId_.back(), zoneId_.back(), "Elem", cgns_type_elem, start, end, 0, elems.data(), &sectionId) != CG_OK)
    Cerr << "Error Format_Post_CGNS::ecrire_domaine_ : cg_section_write !" << finl, cg_error_exit();
}

/*
 * ********************
 * Pour ecriture champs
 * ********************
 */
void Format_Post_CGNS::ecrire_champ_par_(const int comp, const double temps, const Nom& id_du_champ, const Nom& localisation, const Nom& nom_dom, const DoubleTab& valeurs)
{
  // TODO FIXME
}

void Format_Post_CGNS::ecrire_champ_(const int comp, const double temps, const Nom& id_du_champ, const Nom& localisation, const Nom& nom_dom, const DoubleTab& valeurs)
{
  std::string LOC = Motcle(localisation).getString();

  if (LOC == "FACES")
    {
      Cerr << "FACES FIELDS ARE NOT YET TREATED ... " << finl;
//      throw;
      return;
    }

  /* 1 : Increment fieldIds */
  if (LOC == "SOM") fieldId_som_++;
  else // ELEM // TODO FIXME FACES
    fieldId_elem_++;

  /* 2 : Get corresponding domain index */
  const int ind = get_index_nom_vector(doms_written_, nom_dom);
  assert(ind > -1);

  /* 3 : Write solution names for iterative data later */
  if (!solname_som_written_ && LOC == "SOM")
    {
      std::string solname = "FlowSolution" + std::to_string(temps) + "_" + LOC;
      solname.resize(CGNS_STR_SIZE, ' ');
      solname_som_ += solname;

      if (cg_sol_write(fileId_, baseId_[ind], zoneId_[ind], solname.c_str(), CGNS_ENUMV(Vertex), &flowId_som_) != CG_OK)
        Cerr << "Error Format_Post_CGNS::ecrire_champ_ : cg_sol_write !" << finl, cg_error_exit();

      solname_som_written_ = true;
    }

  if (!solname_elem_written_ && LOC == "ELEM")
    {
      std::string solname = "FlowSolution" + std::to_string(temps) + "_" + LOC;
      solname.resize(CGNS_STR_SIZE, ' ');
      solname_elem_ += solname;

      if (cg_sol_write(fileId_, baseId_[ind], zoneId_[ind], solname.c_str(), CGNS_ENUMV(CellCenter), &flowId_elem_) != CG_OK)
        Cerr << "Error Format_Post_CGNS::ecrire_champ_ : cg_sol_write !" << finl, cg_error_exit();

      solname_elem_written_ = true;
    }

  /* 4 : Fill field values & dump to cgns file */
  if (valeurs.dimension(1) == 1) /* No stride ! */
    {
      if (LOC == "SOM")
        {
          if (cg_field_write(fileId_, baseId_[ind], zoneId_[ind], flowId_som_, CGNS_ENUMV(RealDouble), id_du_champ.getChar(), valeurs.addr(), &fieldId_som_) != CG_OK)
            Cerr << "Error Format_Post_CGNS::ecrire_champ_ : cg_field_write !" << finl, cg_error_exit();
        }
      else // ELEM // TODO FIXME FACES
        {
          if (cg_field_write(fileId_, baseId_[ind], zoneId_[ind], flowId_elem_, CGNS_ENUMV(RealDouble), id_du_champ.getChar(), valeurs.addr(), &fieldId_elem_) != CG_OK)
            Cerr << "Error Format_Post_CGNS::ecrire_champ_ : cg_field_write !" << finl, cg_error_exit();
        }
    }
  else
    {
      std::vector<double> field_cgns; /* XXX TODO Elie Saikali : try DoubleTrav with addr() later ... mais je pense pas :p */
      for (int i = 0; i < valeurs.dimension(0); i++)
        field_cgns.push_back(valeurs(i, comp));

      if (LOC == "SOM")
        {
          if (cg_field_write(fileId_, baseId_[ind], zoneId_[ind], flowId_som_, CGNS_ENUMV(RealDouble), id_du_champ.getChar(), field_cgns.data(), &fieldId_som_) != CG_OK)
            Cerr << "Error Format_Post_CGNS::ecrire_champ_ : cg_field_write !" << finl, cg_error_exit();
        }
      else // ELEM // TODO FIXME FACES
        {
          if (cg_field_write(fileId_, baseId_[ind], zoneId_[ind], flowId_elem_, CGNS_ENUMV(RealDouble), id_du_champ.getChar(), field_cgns.data(), &fieldId_elem_) != CG_OK)
            Cerr << "Error Format_Post_CGNS::ecrire_champ_ : cg_field_write !" << finl, cg_error_exit();
        }
    }
}

/*
 * *******************
 * Pour ecriture iters
 * *******************
 */
void Format_Post_CGNS::finir_par_()
{
  Process::barrier();
  std::string fn = cgns_basename_.getString() + ".cgns"; // file name
  if (cgp_close (fileId_) != CG_OK)
    Cerr << "Error Format_Post_CGNS::ecrire_domaine_par_ : cgp_close !" << finl, cgp_error_exit();

  Cerr << "**** CGNS file " << fn << " closed !" << finl;
}

void Format_Post_CGNS::finir_()
{
  assert(static_cast<int>(baseId_.size()) == static_cast<int>(zoneId_.size()));
  const int nsteps = static_cast<int>(time_post_.size());

  std::vector<int> ind_doms_dumped;

  /* 1 : on iter juste sur le map fld_loc_map_; ie: pas domaine dis ... */
  for (auto &itr : fld_loc_map_)
    {
      const std::string& LOC = itr.first;
      const Nom& nom_dom = itr.second;
      const int ind = get_index_nom_vector(doms_written_, nom_dom);
      ind_doms_dumped.push_back(ind);
      assert(ind > -1);

      /* create BaseIterativeData */
      if (cg_biter_write(fileId_, baseId_[ind], "TimeIterValues", nsteps) != CG_OK)
        Cerr << "Error Format_Post_CGNS::finir : cg_biter_write !" << finl, cg_error_exit();

      /* go to BaseIterativeData level and write time values */
      if (cg_goto(fileId_, baseId_[ind], "BaseIterativeData_t", 1, "end") != CG_OK)
        Cerr << "Error Format_Post_CGNS::finir : cg_goto !" << finl, cg_error_exit();

      cgsize_t nuse = static_cast<cgsize_t>(nsteps);
      if (cg_array_write("TimeValues", CGNS_ENUMV(RealDouble), 1, &nuse, time_post_.data()) != CG_OK)
        Cerr << "Error Format_Post_CGNS::finir : cg_array_write !" << finl, cg_error_exit();

      /* create ZoneIterativeData */
      if (cg_ziter_write(fileId_, baseId_[ind], zoneId_[ind], "ZoneIterativeData") != CG_OK)
        Cerr << "Error Format_Post_CGNS::finir : cg_ziter_write !" << finl, cg_error_exit();

      if (cg_goto(fileId_, baseId_[ind], "Zone_t", zoneId_[ind], "ZoneIterativeData_t", 1, "end") != CG_OK)
        Cerr << "Error Format_Post_CGNS::finir : cg_goto !" << finl, cg_error_exit();

      cgsize_t idata[2];
      idata[0] = CGNS_STR_SIZE;
      idata[1] = nsteps;

      if (LOC == "SOM")
        {
          if (cg_array_write("FlowSolutionPointers", CGNS_ENUMV(Character), 2, idata, solname_som_.c_str()) != CG_OK)
            Cerr << "Error Format_Post_CGNS::finir : cg_array_write !" << finl, cg_error_exit();
        }
      else
        {
          if (cg_array_write("FlowSolutionPointers", CGNS_ENUMV(Character), 2, idata, solname_elem_.c_str()) != CG_OK)
            Cerr << "Error Format_Post_CGNS::finir : cg_array_write !" << finl, cg_error_exit();
        }

      if (cg_simulation_type_write(fileId_, baseId_[ind], CGNS_ENUMV(TimeAccurate)) != CG_OK)
        Cerr << "Error Format_Post_CGNS::finir : cg_simulation_type_write !" << finl, cg_error_exit();
    }

  /* 2 : on iter sur les autres domaines; ie: domaine dis */
  for (int i = 0; i < static_cast<int>(doms_written_.size()); i++)
    {
      if (std::find(ind_doms_dumped.begin(), ind_doms_dumped.end(), i) == ind_doms_dumped.end()) // indice pas dans ind_doms_dumped
        {
          const Nom& nom_dom = doms_written_[i];
          const int ind = get_index_nom_vector(doms_written_, nom_dom);
          assert(ind > -1);

          /* create BaseIterativeData */
          if (cg_biter_write(fileId_, baseId_[ind], "TimeIterValues", nsteps) != CG_OK)
            Cerr << "Error Format_Post_CGNS::finir : cg_biter_write !" << finl, cg_error_exit();

          /* go to BaseIterativeData level and write time values */
          if (cg_goto(fileId_, baseId_[ind], "BaseIterativeData_t", 1, "end") != CG_OK)
            Cerr << "Error Format_Post_CGNS::finir : cg_goto !" << finl, cg_error_exit();

          cgsize_t nuse = static_cast<cgsize_t>(nsteps);
          if (cg_array_write("TimeValues", CGNS_ENUMV(RealDouble), 1, &nuse, time_post_.data()) != CG_OK)
            Cerr << "Error Format_Post_CGNS::finir : cg_array_write !" << finl, cg_error_exit();

          /* create ZoneIterativeData */
          if (cg_ziter_write(fileId_, baseId_[ind], zoneId_[ind], "ZoneIterativeData") != CG_OK)
            Cerr << "Error Format_Post_CGNS::finir : cg_ziter_write !" << finl, cg_error_exit();

          if (cg_goto(fileId_, baseId_[ind], "Zone_t", zoneId_[ind], "ZoneIterativeData_t", 1, "end") != CG_OK)
            Cerr << "Error Format_Post_CGNS::finir : cg_goto !" << finl, cg_error_exit();

          if (cg_simulation_type_write(fileId_, baseId_[ind], CGNS_ENUMV(TimeAccurate)) != CG_OK)
            Cerr << "Error Format_Post_CGNS::finir : cg_simulation_type_write !" << finl, cg_error_exit();
        }
      else { /* Do Nothing */ }
    }

  /* 3 : close cgns file */
  std::string fn = cgns_basename_.getString() + ".cgns"; // file name
  if (cg_close(fileId_) != CG_OK)
    Cerr << "Error Format_Post_CGNS::finir : cg_close !" << finl, cg_error_exit();

  Cerr << "**** CGNS file " << fn << " closed !" << finl;
}

#endif
