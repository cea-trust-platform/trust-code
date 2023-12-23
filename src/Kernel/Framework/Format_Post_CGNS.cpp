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
#include <Param.h>

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
  Format_Post_base::readOn(is);
  return is;
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

  if (est_le_premier_post)
    {
      std::string fn = cgns_basename_.getString() + ".cgns"; // file name
      if (cg_open(fn.c_str(), CG_MODE_WRITE, &index_file_)) cg_error_exit();
    }

  return 1;
}

int Format_Post_CGNS::finir(const int est_le_dernier_post)
{
  if (est_le_dernier_post)
    {
      std::string fn = cgns_basename_.getString() + ".cgns"; // file name
      cg_close(index_file_);
      Cerr << "CGNS file " << fn << " closed !" << finl;
    }
  return 1;
}

int Format_Post_CGNS::ecrire_domaine(const Domaine& domaine,const int est_le_premier_post)
{
  verify_if_cgns(__func__);
  Motcle type_elem = domaine.type_elem().valeur().que_suis_je();
  const int dim = domaine.les_sommets().dimension(1), nb_som = domaine.nb_som(), nb_elem = domaine.nb_elem();
  const int icelldim = dim, iphysdim = Objet_U::dimension;

  int index_zone;
  char basename[33];

  strcpy(basename, domaine.le_nom().getChar()); // dom name

  cg_base_write(index_file_, basename, icelldim, iphysdim, &index_base_);

  /*
   * Fill coords
   */
  std::vector<double> xCoords(nb_som), yCoords(nb_som), zCoords;
  if (dim > 2) zCoords.resize(nb_som);

  for (int i = 0; i < nb_som; i++)
    {
      xCoords[i] = domaine.les_sommets()(i, 0);
      yCoords[i] = domaine.les_sommets()(i, 1);
      if (dim > 2) zCoords[i] = domaine.les_sommets()(i, 2);
    }

  const IntTab& les_elems = domaine.les_elems();

  cgsize_t isize[3][1], ielem[nb_elem][les_elems.dimension(1)]; // 8 for hexa

  /* vertex size */
  isize[0][0] = nb_som;
  /* cell size */
  isize[1][0] = nb_elem;
  /* boundary vertex size (zero if elements not sorted) */
  isize[2][0] = 0;

  /* create zone */
  cg_zone_write(index_file_, index_base_, basename /* Dom name */, isize[0], CGNS_ENUMV(Unstructured), &index_zone);

  int index_coord;

  /* write grid coordinates (user must use SIDS-standard names here) */
  cg_coord_write(index_file_, index_base_, index_zone, CGNS_ENUMV(RealDouble), "CoordinateX", xCoords.data(), &index_coord);
  cg_coord_write(index_file_, index_base_, index_zone, CGNS_ENUMV(RealDouble), "CoordinateY", yCoords.data(), &index_coord);

  if (dim > 2)
    cg_coord_write(index_file_, index_base_, index_zone, CGNS_ENUMV(RealDouble), "CoordinateZ", zCoords.data(), &index_coord);

  /* set element connectivity: */
  /* ---------------------------------------------------------- */
  /* do all the HEXA_8 elements (this part is mandatory): */
  /* maintain SIDS-standard ordering */
  /* index no of first element */
  int nelem_start = 1;

  for (int i = 0; i < nb_elem; i++)
    {
      ielem[i][0] = les_elems(i, 0) + 1; // 1
      ielem[i][1] = les_elems(i, 1) + 1; // 2
      ielem[i][2] = les_elems(i, 3) + 1; // 4
      ielem[i][3] = les_elems(i, 2) + 1; // 3
      ielem[i][4] = les_elems(i, 4) + 1; // 5
      ielem[i][5] = les_elems(i, 5) + 1; // 6
      ielem[i][6] = les_elems(i, 7) + 1; // 8
      ielem[i][7] = les_elems(i, 6) + 1; // 7
    }

  int nelem_end = nb_elem;

  /* unsorted boundary elements */
  int nbdyelem = 0, index_section;
  /* write CGNS_ENUMV(HEXA_8) element connectivity (user can give any name) */
  cg_section_write(index_file_, index_base_, index_zone, "Elem", CGNS_ENUMV(HEXA_8), nelem_start, nelem_end, nbdyelem, ielem[0], &index_section);


  return 1;
}
