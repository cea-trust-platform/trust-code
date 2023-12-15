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


#include <IJK_Lata_writer.h>

void dumplata_newtime(const char *filename, double time)
{
  if (Process::je_suis_maitre())
    {
      SFichier master_file;
      master_file.ouvrir(filename, ios::app);
      master_file << "TEMPS " << time << finl;
    }
}

void dumplata_header(const char *filename)
{
  if (Process::je_suis_maitre())
    {
      SFichier master_file;

      master_file.set_bin(0);
      master_file.ouvrir(filename);
      master_file << "LATA_V2.1" << finl;
      master_file << "IJK specific format" << finl;
      master_file << "TRUST Version " << TRUST_VERSION << finl;
      Nom format = (sizeof(int)==8 ? "INT64" : "INT32");
      master_file << "Format LITTLE_ENDIAN,C_INDEXING,C_ORDERING,F_MARKERS_NO,"<<format<<",REAL32" << finl;
      master_file.close();
    }
}

void dumplata_add_geometry(const char *filename, const IJK_Splitting& splitting)
{
  if (Process::je_suis_maitre())
    {
      SFichier master_file;
      Nom prefix = Nom(filename) + Nom(".");
      SFichier binary_file;
      binary_file.set_bin(1);
      ArrOfFloat tmp;
      int n;

      Nom basename(filename);
      master_file.set_bin(0);
      master_file.ouvrir(basename, ios::app);
      Noms fname(3);
      const Nom& geomname = splitting.get_grid_geometry().le_nom();
      if (geomname == "??")
        {
          Cerr << "Error in  dumplata_header: geometry has no name" << finl;
          Process::exit();
        }
      for (int dir = 0; dir < 3; dir++)
        {
          fname[dir] = prefix + geomname + Nom(".coord") + Nom((char)('x'+dir));
          int i;
          binary_file.ouvrir(fname[dir]);
          const ArrOfDouble& coord = splitting.get_grid_geometry().get_node_coordinates(dir);
          n = coord.size_array();
          tmp.resize_array(n);
          for (i = 0; i < n; i++)
            tmp[i] = (float)coord[i]; // LATA in float ... pfff
          binary_file.put(tmp.addr(), n, 1);
          binary_file.close();
        }
      master_file << "Geom " << geomname << " type_elem=HEXAEDRE" << finl;
      master_file << "Champ SOMMETS_IJK_I " << fname[0] << " geometrie=" << geomname << " size=" << splitting.get_grid_geometry().get_nb_elem_tot(0)+1 << " composantes=1" << finl;
      master_file << "Champ SOMMETS_IJK_J " << fname[1] << " geometrie=" << geomname << " size=" << splitting.get_grid_geometry().get_nb_elem_tot(1)+1 << " composantes=1" << finl;
      master_file << "Champ SOMMETS_IJK_K " << fname[2] << " geometrie=" << geomname << " size=" << splitting.get_grid_geometry().get_nb_elem_tot(2)+1 << " composantes=1" << finl;
      master_file.close();
    }
}

void dumplata_ft_field(const char *filename, const char *meshname,
                       const char *field_name, const char *localisation,
                       const ArrOfInt& field, int step)
{
  Nom prefix = Nom(filename) + Nom(".") + Nom(step) + Nom(".") + Nom(meshname) + Nom(".");
  Nom fdfield = prefix + field_name;
  const int nval = field.size_array();
  const int nvaltot = Process::mp_sum(nval);
  EcrFicPartageBin file;
  file.ouvrir(fdfield);
  file.put(field.addr(), field.size_array(), 1);
  file.syncfile();
  file.close();
  if (Process::je_suis_maitre())
    {
      SFichier master_file;
      master_file.ouvrir(filename, ios::app);
      // NO_INDEXING car ce n'est pas un indice de sommet ou de facette
      // (par defaut dans le format lata, les champs d'ints sont supposes etres des indices)
      Nom format = (sizeof(int)==8 ? "INT64" : "INT32");
      master_file << "Champ " << field_name << " " << basename(fdfield) << " geometrie=" << meshname ;
#ifdef INT_is_64_
      master_file << " file_offset=6";
#endif
      master_file << " size=" << (int)nvaltot << " composantes=1 format="<<format<<",NO_INDEXING localisation="
                  << localisation << finl;
    }

}

void dumplata_ft_field(const char *filename, const char *meshname,
                       const char *field_name, const char *localisation,
                       const ArrOfDouble& field, int step)
{
  Nom prefix = Nom(filename) + Nom(".") + Nom(step) + Nom(".") + Nom(meshname) + Nom(".");
  Nom fdfield = prefix + field_name;
  const int nval = field.size_array();
  const int nvaltot = Process::mp_sum(nval);
  EcrFicPartageBin file;
  file.ouvrir(fdfield);
  const int n = field.size_array();
  ArrOfFloat tmp(n);
  for (int i = 0; i < n; i++)
    tmp[i] = (float)field[i];

  file.put(tmp.addr(), field.size_array(), 1);
  file.syncfile();
  file.close();
  if (Process::je_suis_maitre())
    {
      SFichier master_file;
      master_file.ouvrir(filename, ios::app);
      int nb_compo ;

      const DoubleTab* zfld = dynamic_cast<const DoubleTab*>(&field);
      if (zfld)
        {
          nb_compo = zfld->dimension(1);
        }
      else
        {
          nb_compo = 1;
        }
      master_file << "Champ " << field_name << " " << basename(fdfield) << " geometrie=" << meshname;
#ifdef INT_is_64_
      master_file << " file_offset=6";
#endif
      master_file << " size=" << (int)nvaltot/nb_compo << " composantes=" << nb_compo << " localisation="
                  << localisation << finl;
    }

}

void dumplata_finish(const char *filename)
{
  if (Process::je_suis_maitre())
    {
      SFichier master_file;
      master_file.ouvrir(filename, ios::app);
      master_file << "FIN" << finl;
    }
}

Nom dirname(const Nom& filename)
{
  Nom path, name;
  split_path_filename(filename, path, name);
  return path;
}
Nom basename(const Nom& filename)
{
  Nom path, name;
  split_path_filename(filename, path, name);
  return name;
}
