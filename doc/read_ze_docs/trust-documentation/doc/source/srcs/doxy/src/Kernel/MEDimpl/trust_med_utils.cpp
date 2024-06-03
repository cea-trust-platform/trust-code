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

#include <trust_med_utils.h>
#include <TRUSTTabs.h>
#include <Motcle.h>
#include <Noms.h>
#include <Char_ptr.h>
#include <sys/stat.h>
#include <fstream>

#include <medcoupling++.h>
#ifdef MEDCOUPLING_
#include <MEDLoader.hxx>
#endif

#include <numeric>

void traite_nom_fichier_med(Nom& nom_fic)
{
  Nom nom_fic2(nom_fic);
  nom_fic2.prefix(".med");
  if (nom_fic2==nom_fic)
    {
      Cerr<<"Error : the med field must have a .med extension"<<finl;
      Cerr<<nom_fic2 <<" "<<nom_fic<<finl;
      Process::exit();
    }

  nom_fic2.prefix("_0000");
  nom_fic2+=".med";
  nom_fic=nom_fic2.nom_me(nom_fic.me());
  // on essaye en premier les fichiers _0000.med
  {
    std::ifstream test(nom_fic);
    if (!test)
      nom_fic=nom_fic2;
  }
  {
    std::ifstream test(nom_fic);
    if (!test)
      {
        // on essaye Cas_0000.med
        nom_fic=nom_fic2.nom_me(0);
        std::ifstream test2(nom_fic);
        if (!test2)
          {
            Cerr<<"med file "<<nom_fic<<" not found."<<finl;
            Process::exit();
          }
      }
  }
}

extern "C" int MEDimport(char*,char*);
void test_version(Nom& nom)
{
#ifdef MED_
  // on regarde si le fichier est d'une version differente, si oui
  // on cree un fichier au format MED majeur courant, et on change le nom du fichier
  med_bool med_ok, hdf_ok;
  if (MEDfileCompatibility(nom, &med_ok, &hdf_ok))
    {
      Cerr<<"Problem when trying to open the file "<<nom<<finl;
      Process::exit();
    }

  if (hdf_ok && med_ok) return; //pas besoin de convertir

  // On serialise pour eviter que le fichier soit cree plusieurs fois en //

  Nom nom2bis("Conv_");
  // ajout prefixe Conv_
  Nom nom2 ;

  char* nomtmp = new char[strlen(nom)+1];
  int compteur=(int)strlen(nom);

  strcpy(nomtmp,nom);

  const char* ptr=nomtmp+compteur;
  bool pas_de_slach=false;

  while((*ptr!='/')&&(compteur>0))  // recherche du dernier slach dans le nom du fichier
    {
      ptr--;
      compteur--;
      if (*ptr=='/') pas_de_slach=true;
    }

  if (!pas_de_slach) // Cas ou le fichier med est dans le repertoire courant
    {
      nom2=nom2bis;
      nom2+=nom;
    }
  else // Cas ou le fichier med est defini par un chemin
    {
      Nom cible(ptr);
      cible.suffix("/");  // nom du fichier
      nom2bis+= cible;   // nom du fichier + prefix Conv_

      Nom nomentier(nom);
      nomentier.prefix(cible);  // nom du chemin pour trouver fichier

      nomentier+=nom2bis;  // nom du chemin + prefixe Conv . nom fichier
      nom2=nomentier;
    }

  delete [] nomtmp;

  for  (int p=0; p<Process::nproc(); p++)
    {
      if (p==Process::me())
        {
          bool a_creer= false;
          std::ifstream test(nom2);
          if (test)
            {
              struct stat org,newf;
              stat(nom,&org);
              stat(nom2,&newf);
              if (org.st_mtime>newf.st_mtime)
                a_creer=true;
            }
          else
            a_creer=true;
          if (a_creer)
            {
              Cerr<<"Creation of the file "<<nom2<<" to newer format."<<finl;
              MEDimport(Char_ptr(nom),Char_ptr(nom2));
            }
          else
            Cerr<<"The file "<<nom2<<" is up to date."<<finl;
        }
      Process::barrier();
    }
  nom=nom2;
  return;
#endif
}

void read_med_field_names(const Nom& nom_fic, Noms& noms_chps, ArrOfDouble& temps_sauv)
{
#if defined(MEDCOUPLING_) && defined(MED_)
  using namespace std;
  using namespace MEDCoupling;
  using MCTimeLabel = pair< pair<True_int,True_int>, double>;
  const string fnam(nom_fic.getString());

  vector<string> nams(GetAllFieldNames(fnam));
  noms_chps.resize((int)nams.size());
  for(const auto& s: nams)
    noms_chps.add(Nom(s));

  vector<double> tims;
  for(const auto& fldName: nams)
    {
      vector< MCTimeLabel > it(GetAllFieldIterations(fnam, fldName));
      vector<double> other_tims;
      other_tims.resize(it.size());
      // return only the double representing the time:
      auto lambd = [](const MCTimeLabel& cplx_pair) { return cplx_pair.second; };
      transform(it.begin(), it.end(),
                back_inserter(other_tims),
                lambd);
      std::sort(other_tims.begin(), other_tims.end());
      if (tims.size() == 0)
        tims = other_tims;
      else if (tims != other_tims)
        Process::exit("LireMED::read_med_field_names() - different timesteps between the various fields found in the MED file !!");
    }
  temps_sauv.resize((int)tims.size());
  std::copy(tims.begin(), tims.end(), temps_sauv.addr());
#else
  med_non_installe();
#endif
}

#ifdef MED_
// renvoit le type med a partir du type trio
med_geometry_type type_geo_trio_to_type_med(const Nom& type_elem_,med_axis_type& rep)
{
  rep=MED_CARTESIAN;
  Motcle type_elem;
  type_elem=type_elem_;
  type_elem.prefix("_AXI");
  if (type_elem!=Motcle(type_elem_))
    {
      rep=MED_SPHERICAL;
      Cerr<<"#"<<type_elem<<"#"<<Motcle(type_elem_)<<"#"<<(type_elem!=Motcle(type_elem_))<<finl;
      if (type_elem=="QUADRILATERE_2D")
        type_elem="SEGMENT_2D";
      if (type_elem=="RECTANGLE_2D")
        {
          type_elem="RECTANGLE";
          rep=MED_CYLINDRICAL;
        }
    }
  med_geometry_type type_elem_med;
  if ((type_elem=="RECTANGLE") || (type_elem=="QUADRANGLE"))
    type_elem_med=MED_QUAD4;
  else if  ((type_elem=="HEXAEDRE")|| (type_elem=="HEXAEDRE_VEF"))
    type_elem_med=MED_HEXA8;
  else if  (type_elem=="TRIANGLE")
    type_elem_med=MED_TRIA3;
  else if  (type_elem=="TETRAEDRE")
    type_elem_med=MED_TETRA4;
  else if ((type_elem=="SEGMENT_2D") || (type_elem=="SEGMENT"))
    type_elem_med=MED_SEG2;
  else if (type_elem=="TRIANGLE_3D")
    type_elem_med=MED_TRIA3;
  else if (type_elem=="QUADRANGLE_3D")
    type_elem_med=MED_QUAD4;
  else if (type_elem=="PRISME")
    type_elem_med=MED_PENTA6;
  else if (type_elem=="POLYEDRE")
    type_elem_med=MED_POLYHEDRON;
  else if (type_elem=="POLYGONE")
    type_elem_med=MED_POLYGON;
  else if (type_elem=="POLYGONE_3D")
    type_elem_med=MED_POLYGON;
  else if(type_elem=="SEGMENT")
    type_elem_med=MED_SEG2;
  else if(type_elem=="PRISME_HEXAG")
    type_elem_med=MED_OCTA12;
  else if(type_elem=="POINT_1D")
    type_elem_med=MED_POINT1;
  else
    {
      Cerr<<type_elem<< " no available code" <<finl;
      Process::exit();
      type_elem_med=MED_QUAD4;
    }
  return type_elem_med;
}

med_geometry_type type_geo_trio_to_type_med(const Nom& type_elem)
{
  med_axis_type rep;
  return type_geo_trio_to_type_med(type_elem,rep);
}

#ifdef MEDCOUPLING_
/*! @brief Return MEDCoupling type from TRUST type
 * See file NormalizedGeometricTypes in MEDCoupling includes.
 */
INTERP_KERNEL::NormalizedCellType type_geo_trio_to_type_medcoupling(const Nom& type_elem_, int& mesh_dimension)
{
  Motcle type_elem;
  type_elem=type_elem_;
  type_elem.prefix("_AXI");
  if (type_elem!=Motcle(type_elem_))
    {
      if (type_elem == "QUADRILATERE_2D")
        type_elem = "SEGMENT_2D";
      if (type_elem == "RECTANGLE_2D")
        {
          type_elem = "RECTANGLE";
        }
    }
  mesh_dimension = -1;
  INTERP_KERNEL::NormalizedCellType type_cell;
  if ((type_elem=="RECTANGLE") || (type_elem=="QUADRANGLE") || (type_elem=="QUADRANGLE_3D"))
    {
      type_cell = INTERP_KERNEL::NORM_QUAD4;
      mesh_dimension = 2;
    }
  else if  ((type_elem=="HEXAEDRE") || (type_elem=="HEXAEDRE_VEF"))
    {
      type_cell = INTERP_KERNEL::NORM_HEXA8;
      mesh_dimension = 3;
    }
  else if  ((type_elem=="TRIANGLE") || (type_elem=="TRIANGLE_3D"))
    {
      type_cell = INTERP_KERNEL::NORM_TRI3;
      mesh_dimension = 2;
    }
  else if  (type_elem=="TETRAEDRE")
    {
      type_cell = INTERP_KERNEL::NORM_TETRA4;
      mesh_dimension = 3;
    }
  else if ((type_elem=="SEGMENT") || (type_elem=="SEGMENT_2D"))
    {
      type_cell = INTERP_KERNEL::NORM_SEG2;
      mesh_dimension = 1;
    }
  else if (type_elem=="PRISME")
    {
      type_cell = INTERP_KERNEL::NORM_PENTA6;
      mesh_dimension = 3;
    }
  else if (type_elem=="POLYEDRE")
    {
      type_cell = INTERP_KERNEL::NORM_POLYHED;
      mesh_dimension = 3;
    }
  else if ((type_elem=="POLYGONE") || (type_elem=="POLYGONE_3D"))
    {
      type_cell = INTERP_KERNEL::NORM_POLYGON;
      mesh_dimension = 2;
    }
  else if(type_elem=="PRISME_HEXAG")
    {
      type_cell = INTERP_KERNEL::NORM_HEXGP12;
      mesh_dimension = 3;
    }
  else if ((type_elem=="POINT") || (type_elem=="POINT_1D"))
    {
      type_cell = INTERP_KERNEL::NORM_POINT1;
      mesh_dimension = 0;
    }
  else
    {
      Cerr<<type_elem<< " no available cell." <<finl;
      Process::exit();
      return INTERP_KERNEL::NORM_POINT1;
    }
  assert(mesh_dimension>=0);
  return type_cell;
}
#endif


#endif

/*! @brief Passage de la connectivite TRUST a MED si toMED=true de MED a trio si toMED=false
 */
void conn_trust_to_med(IntTab& les_elems, const Nom& type_elem, bool toMED)
{
#ifdef MED_
  int nele=les_elems.dimension(0);
  // cas face_bord vide
  if (nele==0) return;
  med_geometry_type type_elem_med;
  type_elem_med=type_geo_trio_to_type_med(type_elem);
  IntTab les_elemsn(les_elems);
  ArrOfInt filter;
  switch (type_elem_med)
    {
    case MED_POINT1  :
      filter.resize_array(1) ;
      filter[0] = 0 ;
      break ;
    case MED_SEG2    :
      filter.resize_array(2) ;
      std::iota(filter.addr(), filter.addr()+2, 0);
      break ;
    case MED_SEG3    :
      break ;
    case MED_TRIA3   :
      filter.resize_array(3) ;
      std::iota(filter.addr(), filter.addr()+3, 0);
      break ;
    case MED_QUAD4   :
      filter.resize_array(4) ;
      filter[0] = 0 ;
      filter[1] = 2 ;
      filter[2] = 3 ;
      filter[3] = 1 ;
      break ;
    case MED_TRIA6   :
    case MED_QUAD8   :
      break ;
    case MED_TETRA4  :
      filter.resize_array(4) ;
      std::iota(filter.addr(), filter.addr()+4, 0);
      break ;
    case MED_PYRA5   :
      filter.resize_array(5) ;
      filter[0] = 0 ;
      filter[1] = 3 ;  // 2nd element in med are 4th in vtk (array begin at 0 !)
      filter[2] = 2 ;
      filter[3] = 1 ;  // 4th element in med are 2nd in vtk (array begin at 0 !)
      filter[4] = 4 ;
      break ;
    case MED_PENTA6  :
      filter.resize_array(6) ;
      std::iota(filter.addr(), filter.addr()+6, 0);
      break ;
    case MED_HEXA8   :
      filter.resize_array(8) ;
      filter[0] = 0 ;
      filter[1] = 2 ;
      filter[2] = 3 ;
      filter[3] = 1 ;
      filter[4] = 4 ;
      filter[5] = 6 ;
      filter[6] = 7 ;
      filter[7] = 5 ;
      break ;
    case MED_TETRA10 :
    case MED_PYRA13  :
    case MED_PENTA15 :
    case MED_HEXA20  :
      break ;

    case MED_OCTA12:
    case MED_POLYGON:
    case MED_POLYHEDRON:
      {
        int nb_som_max=les_elems.dimension(1);
        filter.resize_array(nb_som_max);
        std::iota(filter.addr(), filter.addr()+nb_som_max, 0);
        break ;
      }
    default:
      Cerr<<"case not scheduled"<<finl;
      Process::exit();
    }

  int ns=filter.size_array();
  if (ns==0)
    {
      Cerr<<"Problem for filtering operation "<<finl;
      Process::exit();
    }
  if (toMED)
    {
      for (int el=0; el<nele; el++)
        for (int n=0; n<ns; n++)
          les_elems(el,n)=les_elemsn(el,filter[n]);
    }
  else
    {
      for (int el=0; el<nele; el++)
        for (int n=0; n<ns; n++)
          les_elems(el,filter[n])=les_elemsn(el,n);
    }
#endif
}

