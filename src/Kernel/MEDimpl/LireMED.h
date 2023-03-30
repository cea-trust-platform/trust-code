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

#ifndef LireMED_included
#define LireMED_included

/*! @brief Classe LireMED Lire un fichier MED
 *
 *     Structure du jeu de donnee (en dimension 2) :
 *     LireMED dom medfile
 */
#include <Interprete_geometrique_base.h>
#include <TRUSTTabs_forward.h>
#include <med++.h>
#include <medcoupling++.h>

#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#include <MEDFileMesh.hxx>
#endif

class Nom;
class Domaine;

class LireMED : public Interprete_geometrique_base
{
  Declare_instanciable(LireMED);
public :

  LireMED(const Nom& file_name, const Nom& mesh_name);
  Entree& interpreter_(Entree&) override;
  void lire_geom(bool subDom=true);

protected:
  Nom nom_fichier_;              ///< Name of the MED file to read
  Nom nom_mesh_ = "--any--";     ///< Name of the mesh in the MED file to read
  bool axi1d_ = false;           ///< Are we in Axi1D
  bool convertAllToPoly_ = false;///< Should the mesh be converted to polygons/polyedrons
  bool isVEFForce_ = false;      ///< Should we force element type to be VEF compatible
  int isFamilyShort_ = 0;
  int space_dim_ = -1;           ///< Space dimension read in the MED file
  Elem_geom type_elem_;          ///< Highest dimension element type
  Nom type_face_;                ///< Boundary element type
  Noms noms_bords_;              ///< Names of the boundaries
  Noms exclude_grps_;            ///< Names of the (face) groups to skip when reading the file
#ifdef MEDCOUPLING_
  MEDCoupling::MEDCouplingAxisType axis_type_  = MEDCoupling::MEDCouplingAxisType::AX_CART ;
  MEDCoupling::MCAuto<MEDCoupling::MEDCouplingUMesh> mcumesh_;
  MEDCoupling::MCAuto<MEDCoupling::MEDFileUMesh> mfumesh_;
#endif

  void interp_old_syntax(Entree& is, Nom& nom_dom_trio);
  Nom type_medcoupling_to_type_geo_trio(int type_cell, bool cell_from_boundary) const;
  void retrieve_MC_objects();
  void prepare_som_and_elem(DoubleTab& sommets2, IntTab& les_elems2);
  void finalize_sommets(const DoubleTab& sommets2, DoubleTab& sommets) const;
  void write_sub_dom_datasets() const;
  void read_boundaries(IntVect& indices_bords, ArrOfInt& familles, IntTab& all_faces_bords);
  void fill_frontieres(const IntVect& indices_bords, const ArrOfInt& familles, const IntTab& all_faces_bords);

};

#endif
