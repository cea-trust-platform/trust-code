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
#include <TRUST_2_CGNS.h>
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
      if (cg_open(fn.c_str(), CG_MODE_WRITE, &fileId_)) cg_error_exit();
      Cerr << "**** CGNS file " << fn << " opened !" << finl;
    }
#endif
  return 1;
}

int Format_Post_CGNS::finir(const int est_le_dernier_post)
{
#ifdef HAS_CGNS
  if (est_le_dernier_post)
    {
      std::string fn = cgns_basename_.getString() + ".cgns"; // file name
      cg_close(fileId_);
      Cerr << "**** CGNS file " << fn << " closed !" << finl;
    }
#endif
  return 1;
}

int Format_Post_CGNS::ecrire_domaine(const Domaine& domaine,const int est_le_premier_post)
{
  verify_if_cgns(__func__);
#ifdef HAS_CGNS
  /* 1 : Instance of TRUST_2_CGNS */
  TRUST_2_CGNS TRUST2CGNS;
  TRUST2CGNS.associer_domaine_TRUST(domaine);

  Motcle type_elem = domaine.type_elem().valeur().que_suis_je();
  CGNS_TYPE cgns_type_elem = TRUST2CGNS.convert_elem_type(type_elem);

  /* 2 : Fill coords */
  std::vector<double> xCoords, yCoords, zCoords;
  TRUST2CGNS.fill_coords(xCoords, yCoords, zCoords);

  const int dim = domaine.les_sommets().dimension(1), nb_som = domaine.nb_som(), nb_elem = domaine.nb_elem();
  const int icelldim = dim, iphysdim = Objet_U::dimension;
  int zoneId, coordsId;

  /* 3 : Base write */
  char basename[99];
  strcpy(basename, domaine.le_nom().getChar()); // dom name
  cg_base_write(fileId_, basename, icelldim, iphysdim, &baseId_);

  /* 4 : Vertex, cell & boundary vertex sizes */
  cgsize_t isize[3][1];
  isize[0][0] = nb_som;
  isize[1][0] = nb_elem;
  isize[2][0] = 0; /* boundary vertex size (zero if elements not sorted) */

  /* 5 : Create zone */
  cg_zone_write(fileId_, baseId_, basename /* Dom name */, isize[0], CGNS_ENUMV(Unstructured), &zoneId);

  /* 6 : Write grid coordinates */
  cg_coord_write(fileId_, baseId_, zoneId, CGNS_ENUMV(RealDouble), "CoordinateX", xCoords.data(), &coordsId);
  cg_coord_write(fileId_, baseId_, zoneId, CGNS_ENUMV(RealDouble), "CoordinateY", yCoords.data(), &coordsId);
  if (dim > 2) cg_coord_write(fileId_, baseId_, zoneId, CGNS_ENUMV(RealDouble), "CoordinateZ", zCoords.data(), &coordsId);

  /* 7 : Set element connectivity */
  std::vector<cgsize_t> elems;
  cgsize_t start = 1, end;
  int nsom = TRUST2CGNS.convert_connectivity(cgns_type_elem, elems);
  end = start + static_cast<cgsize_t>(elems.size()) / nsom - 1;

  /* 8 : Write domaine */
  int sectionId;
  cg_section_write(fileId_, baseId_, zoneId, "Elem", cgns_type_elem, start, end, 0, elems.data(), &sectionId);
#endif

  return 1;
}
