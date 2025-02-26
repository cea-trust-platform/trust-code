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

#include <Format_Post_base.h>
#include <Lata_2_Other.h>
#include <EFichierBin.h>
#include <LataFilter.h>
#include <LmlReader.h>
#include <PE_Groups.h>
#include <Polyedre.h>
#include <SFichier.h>
#include <Domaine.h>
#include <EChaine.h>
#include <list>

Implemente_instanciable(Lata_2_Other, "lata_to_other|lata_2_other", Interprete);
// XD format_lata_to_med objet_lecture nul 0 not_set
// XD attr mot chaine(into=["format_post_sup"]) mot 0 not_set
// XD attr format chaine(into=["lml","lata","lata_v2","med"]) format 1 generated file post_med.data use format (MED or LATA or LML keyword).

// XD lata_to_other interprete lata_to_other -1 To convert results file written with LATA format to MED or LML format. Warning: Fields located at faces are not supported yet.
// XD attr format chaine(into=["lml","lata","lata_v2","med"]) format 1 Results format (MED or LATA or LML keyword).
// XD attr file chaine file 0 LATA file to convert to the new format.
// XD attr file_post chaine file_post 0 Name of file post.

Entree& Lata_2_Other::readOn(Entree& is)
{
  Cerr << __FILE__ << (int) __LINE__ << " not coded" << finl;
  Process::exit();
  return is;
}
Sortie& Lata_2_Other::printOn(Sortie& is) const
{
  Cerr << __FILE__ << (int) __LINE__ << " not coded" << finl;
  Process::exit();
  return is;
}

/*! @brief Convert a Lata domain object ('Domain') into a TRUST domain object ('Domaine')
 */
void convert_domain_to_Domaine(const Domain& dom, Domaine& dom_trio)
{
  Nom type_elem = dom.lata_element_name(dom.elt_type_);
  if (type_elem == "PRISM6")
    type_elem = "PRISME";

  Cerr << "Reading from Lata_DB an ELEM of type " << type_elem << " ... Typing the TRUST Domaine !" << finl;
  dom_trio.typer(type_elem);
  dom_trio.type_elem()->associer_domaine(dom_trio);

  /*
   * XXX Elie Saikali => pour polyedre/polygon faut remplir plus de choses ;)
   */
  if (type_elem == "POLYEDRE")
    {
      // dom_trio tjrs en 32 ... faut bricoler ...
      Domaine_32_64<trustIdType> dom_trio_poubelle;
      dom_trio_poubelle.typer(type_elem);

      if (dom.get_domain_type() == Domain::UNSTRUCTURED)
        {
          const DomainUnstructured& geom = dom.cast_DomainUnstructured();
          const auto& elems = geom.elements_; // elem -> nodes
          const auto& faces = geom.faces_; // elem -> nodes
          const auto& ef = geom.elem_faces_; // elem -> nodes
          const auto& nod = geom.nodes_;

          trustIdType dim1_tmp = nod.dimension(0);
          // Check that the Lata domain fits in 32b
          if (dim1_tmp >= std::numeric_limits<int>::max())
            Process::exit("LATA file is too big and does not fit into 32bits!!");

          const trustIdType nb_elems = elems.dimension(0);

          std::vector<trustIdType> conn, connIndex;
          trustIdType idx = 0;
          connIndex.push_back(idx);
          for (trustIdType i = 0; i < nb_elems; i++)
            {
              conn.push_back(31); /* MC.normPolyhedre dans MC */
              idx++;

              for (int j = 0; j < ef.dimension(1); j++)
                {
                  trustIdType fac = ef(i, j);

                  if (fac < 0)
                    continue;

                  bool is_not_last = true;

                  if (j+1 < ef.dimension(1) && ef(i, j+1) < 0)
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

          IntTab_T<trustIdType> les_elems;
          trustIdType face = 0, node = 0;

          for (trustIdType i = 0; i < nb_elems; i++)
            {
              polyhedronIndex[i] = face; // Index des polyedres

              trustIdType index = connIndex[i] + 1;
              int nb_som = static_cast<int>(connIndex[i + 1] - index);
              for (int j = 0; j < nb_som; j++)
                {
                  if (j == 0 || conn[index + j] < 0)
                    facesIndex[face++] = node; // Index des faces:
                  if (conn[index + j] >= 0)
                    nodes[node++] = conn[index + j]; // Index local des sommets de la face
                }
            }
          facesIndex[nfaces] = node;
          polyhedronIndex[nb_elems] = face;

          // welcome to Bricorama ....
          auto& poly_poubelle = ref_cast(Polyedre_32_64<trustIdType>, dom_trio_poubelle.type_elem().valeur());
          auto& poly = ref_cast(Polyedre, dom_trio.type_elem().valeur());

          poly_poubelle.affecte_connectivite_numero_global(nodes, facesIndex, polyhedronIndex, les_elems);

          // back to life ... faut tout caster car dom_trio est 32 bit
          poly.set_nb_som_face_max(poly_poubelle.get_nb_som_elem_max());

          // step 1 : polyhedronIndex
          ArrOfInt tmp;
          polyhedronIndex.from_tid_to_int(tmp);
          poly.getsetPolyhedronIndex() = tmp;

          // step 2 : les_elems
          IntTab tmp1;
          les_elems.from_tid_to_int(tmp1);
          dom_trio.les_elems() = tmp1;

          // step 3 : facesIndex
          ArrOfInt tmp2;
          facesIndex.from_tid_to_int(tmp2);
          poly.getsetFacesIndex() = tmp2;

          // step 4 : Nodes
          ArrOfInt tmp3;
          const auto& nd = poly_poubelle.getNodes(); // attention l'attribue pas nodes local
          tmp3.resize((int) nd.size_array());
          for (auto i = 0; i < nd.size_array(); i++)
            tmp3[i] = static_cast<int>(nd[i]);
          poly.getsetNodes() = tmp3;

          // step  5 : enfin tab sommets
          DoubleTab& som = dom_trio.les_sommets();
          int dim1 = (int) dim1_tmp;
          int dim2 = geom.nodes_.dimension_int(1);
          som.resize(dim1, dim2);
          for (int i1 = 0; i1 < dim1; i1++)
            for (int i2 = 0; i2 < dim2; i2++)
              som(i1, i2) = geom.nodes_(i1, i2);
        }
      else throw;
    }
  else
    {
      //      dom_trio.les_sommets()=geom.nodes_;
      // mais geom.nodes est un FloatTab
      DoubleTab& som = dom_trio.les_sommets();
      int nx = 1, ny = 1, nz = 1;
      if (dom.get_domain_type() == Domain::UNSTRUCTURED)
        {
          const DomainUnstructured& geom = dom.cast_DomainUnstructured();
          trustIdType dim1_tmp = geom.nodes_.dimension(0);
          // Check that the Lata domain fits in 32b
          if (dim1_tmp >= std::numeric_limits<int>::max())
            Process::exit("LATA file is too big and does not fit into 32bits!!");
          int dim1 = (int) dim1_tmp;
          int dim2 = geom.nodes_.dimension_int(1);
          som.resize(dim1, dim2);
          for (int i1 = 0; i1 < dim1; i1++)
            for (int i2 = 0; i2 < dim2; i2++)
              som(i1, i2) = geom.nodes_(i1, i2);
        }
      else
        {
          assert(dom.get_domain_type() == Domain::IJK);
          const DomainIJK& geom = dom.cast_DomainIJK();
          const int dim = geom.coord_.size();

          nx = geom.coord_[0].size_array();
          ny = geom.coord_[1].size_array();
          nz = 1;
          if (dim > 2)
            nz = geom.coord_[2].size_array();

          som.resize(nx * ny * nz, dim);

          for (int i = 0; i < nx; i++)
            for (int j = 0; j < ny; j++)
              for (int k = 0; k < nz; k++)
                {
                  int nn = i * ny * nz + j * nz + k;
                  som(nn, 0) = geom.coord_[0][i];
                  som(nn, 1) = geom.coord_[1][j];
                  if (dim > 2)
                    som(nn, 2) = geom.coord_[2][k];
                }
        }

      if (dom.get_domain_type() == Domain::UNSTRUCTURED)
        {
          const DomainUnstructured& geom = dom.cast_DomainUnstructured();
#if INT_is_64_ == 2
          IntTab tmp;
          geom.elements_.from_tid_to_int(tmp);
          dom_trio.les_elems() = tmp;
#else
          dom_trio.les_elems() = geom.elements_;
#endif
        }
      else
        {
          dom_trio.les_elems().resize((nx - 1) * (ny - 1) * (nz - 1), dom_trio.nb_som_elem());
          IntTab& elems = dom_trio.les_elems();
          for (int i = 0; i < nx - 1; i++)
            for (int j = 0; j < ny - 1; j++)
              for (int k = 0; k < nz - 1; k++)
                {
                  int nn = i * (ny - 1) * (nz - 1) + j * (nz - 1) + k;
                  nn = i + j * (nx - 1) + k * ((nx - 1) * (ny - 1));
                  elems(nn, 0) = i * ny * nz + j * nz + k;
                  elems(nn, 1) = (i + 1) * ny * nz + j * nz + k;
                  elems(nn, 2) = (i) * ny * nz + (j + 1) * nz + k;
                  elems(nn, 3) = (i + 1) * ny * nz + (j + 1) * nz + k;
                  if (elems.dimension(1) > 4)
                    for (int ii = 0; ii < 4; ii++)
                      elems(nn, 4 + ii) = elems(nn, ii) + 1;
                }
        }
    }

  dom_trio.faces_bord().associer_domaine(dom_trio);
  dom_trio.faces_raccord().associer_domaine(dom_trio);
  dom_trio.faces_joint().associer_domaine(dom_trio);
  dom_trio.fixer_premieres_faces_frontiere();

  if (dom.id_.timestep_ != 0)
    dom_trio.deformable() = true;
}

Entree& Lata_2_Other::interpreter(Entree& is)
{
  Cerr << "syntax  Lata_to_Other::interpreter format_post_supp [nom_lata||NOM_DU_CAS] [nom_fichier_sortie||NOM_DU_CAS]   " << finl;
  Nom nom_lata, nom_fic;
  Nom format_post_supp;
  is >> format_post_supp >> nom_lata >> nom_fic;

  std::set<std::string> formats( { "LML", "LATA", "LATA_V2", "MED" });
  Nom tmp = format_post_supp;
  if (formats.count(tmp.majuscule().getString()) == 0)
    {
      Cerr << "Error, format " << format_post_supp << " is not supported!" << finl;
      Process::exit();
    }

  // Creation d'un sous-groupe contenant uniquement le processeur maitre
  ArrOfInt liste_pe(1);
  OWN_PTR(Comm_Group) group;
  // on se met en non axi le temps de la conversion
  int axi_sa = axi;
  axi = 0;
  PE_Groups::create_group(liste_pe, group);

  if (PE_Groups::enter_group(group.valeur()))
    {
      if (nom_lata == "NOM_DU_CAS")
        nom_lata = nom_du_cas() + ".lata";
      if (nom_fic == "NOM_DU_CAS")
        nom_fic = nom_du_cas();
      Nom filename(nom_lata);
      LataOptions opt;
      LataDB lata_db;
      LataFilter filter;
      set_Journal_level(0);

      LataOptions::extract_path_basename(filename, opt.path_prefix, opt.basename);
      opt.dual_mesh = true;
      opt.faces_mesh = false;
      // on ne veut jamais creer le domaine IJK
      opt.regularize = 0;
      opt.regularize_tolerance = 1e-7f;
      read_any_format_options(filename, opt);
      // Read the source file to the lata database
      read_any_format(filename, opt.path_prefix, lata_db);

      filter.initialize(opt, lata_db);

      // On met dual_mesh a "false" si on lit un domaine de polyedres PolyMAC_P0P1NC (pas encore supporte)
      // et on-reouvre la database:
      Noms geom_names = filter.get_exportable_geometry_names();
      int elt_type = filter.get_geometry(Domain_Id(geom_names[0], 1, -1)).elt_type_;
      Cerr << elt_type << finl;
      if (elt_type == Domain::polyedre || elt_type == Domain::polygone)
        {
          opt.dual_mesh = false;
          filter.initialize(opt, lata_db);
        }

      Nom nom_pdb(nom_fic);
      Nom format_post_(format_post_supp);
      if (format_post_ == "lata_v2")
        format_post_ = "lata";

      // copie de Postraiter_domaine

      Nom type("Format_Post_");
      type += format_post_;
      OWN_PTR(Format_Post_base) post_typer;
      post_typer.typer(type.getChar());
      Format_Post_base& post = ref_cast(Format_Post_base, post_typer.valeur());
      Nom nom_2(nom_pdb);
      Nom format_post_bis(format_post_);
      if (format_post_bis.debute_par("lata"))
        format_post_bis = "lata";
      Nom suffix(".");
      suffix += format_post_bis;
      nom_2.prefix(suffix);
      //      int est_le_premie_post=0;
      int format_binaire_ = 0;
      if (format_post_bis != "lata")
        post.initialize_by_default(nom_2);
      else
        post.initialize(nom_2, format_binaire_, "SIMPLE");

      // ecriture des domaines
      {

        Noms geoms = filter.get_exportable_geometry_names();

        for (int i = 0; i < geoms.size(); i++)
          {
            Domain_Id id(geoms[i], 1, -1);
            const Domain& dom = filter.get_geometry(id);

            Domaine dom_trio;
            dom_trio.nommer(geoms[i]);
            convert_domain_to_Domaine(dom, dom_trio);
            int est_le_premier_post = (i == 0);
            post.ecrire_entete(0., 0, est_le_premier_post);
            int reprise = 0;
            double t_init = 0.;
            post.preparer_post(dom_trio.le_nom(), est_le_premier_post, reprise, t_init);
            post.ecrire_domaine(dom_trio, est_le_premier_post);

            filter.release_geometry(dom);
          }
      }
      // les champs
      const char *suffix_vector_names[] = { "X", "Y", "Z" };

      for (int t = 1; t < lata_db.nb_timesteps(); t++)
        {
          int timestate = t;
          //    if (filter.get_nb_timesteps() > 1)      timestate++;
          Noms geoms = filter.get_exportable_geometry_names();
          double time = filter.get_timestep(t);
          post.ecrire_temps(time);
          for (int i = 0; i < geoms.size(); i++)
            {
              Domain_Id id(geoms[i], timestate, -1);
              const Domain& dom = filter.get_geometry(id);

              Domaine dom_trio;
              dom_trio.nommer(geoms[i]);
              convert_domain_to_Domaine(dom, dom_trio);
              Field_UNames fields = filter.get_exportable_field_unames(geoms[i]);
              for (int j = 0; j < fields.size(); j++)
                {
                  //const Nom& nom= fields[j].get_field_name();
                  //  if ((nom!=Motcle("ELEM_FACES"))&& (nom!=Motcle("FACES"))&&  (nom!=Motcle("ELEMENTS")))
                  {

                    int est_le_premie_post = (i == 0);
                    post.init_ecriture(time, -1., est_le_premie_post, dom_trio);
                    Field_Id fieldid(fields[j], timestate, -1);

                    DoubleTab values;
                    try
                      {
                        const FieldFloat& field = filter.get_float_field(fieldid);
                        {
                          const BigFloatTab& values_lata = field.data_;
                          assert(values_lata.size_array() < std::numeric_limits<int>::max());
                          int dim1 = (int)values_lata.dimension(0);
                          int dim2 = values_lata.dimension_int(1);
                          values.resize(dim1, dim2);
                          for (int i1 = 0; i1 < dim1; i1++)
                            for (int i2 = 0; i2 < dim2; i2++)
                              values(i1, i2) = values_lata(i1, i2);
                        }

                        Nom nom_type;
                        if (field.localisation_ == LataField_base::ELEM)
                          nom_type = "ELEM";                  //"CHAMPMAILLE";
                        else if (field.localisation_ == LataField_base::SOM)
                          nom_type = "SOM";                  // CHAMPPOINT";
                        else
                          {
                            Cerr << nom_type << " not coded ...... " << finl;
                            Cerr << __FILE__ << ":" << (int) __LINE__ << finl;
                            exit();

                          }
                        //Nom type_elem=dom.lata_element_name(dom.elt_type_);
                        Noms unites(values.dimension(1));
                        Noms noms_post(values.dimension(1));
                        Cerr << "Extraction " << time << " " << geoms[i] << " " << fields[j].get_field_name() << finl;
                        Nom nom_post, ajout;

                        nom_post = fields[j].get_field_name();
                        // pour eviter qu'en med on est le champ aux faces et sur le dual avec le meme nom
                        if (dom_trio.le_nom().finit_par("_faces"))
                          nom_post += "_Centre";
                        ajout = "_";
                        ajout += nom_type;
                        ajout += "_";
                        ajout += geoms[i];
                        for (int ii = 0; ii < values.dimension(1); ii++)
                          {
                            noms_post[ii] = nom_post;
                            if (values.dimension(1) == dom_trio.les_sommets().dimension(1))
                              noms_post[ii] += suffix_vector_names[ii];
                            else
                              noms_post[ii] += Nom(ii);
                            noms_post[ii] += ajout;
                          }
                        nom_post += ajout;
                        //exit();
                        //meddrive.ecrire_champ(type,nom_fic,geoms[i],fields[j].get_field_name(),values,unites,type_elem   ,time,0);

                        post.ecrire_champ(dom_trio, unites, noms_post, -1, time, nom_post, geoms[i], nom_type, "iii", values);
                        filter.release_field(field);
                      }
                    catch (...)
                      {
                        Cerr << fieldid.uname_.get_field_name() << " is not a FloatField !!!" << finl;
                      }
                  }
                }
              filter.release_geometry(dom);
            }

        }
      int fin = 1;
      post.finir(fin);

      //  delete [] les_domaines;
      //delete [] list_champ;
      PE_Groups::exit_group();
    }
  barrier();
  // on remet eventuellement axi
  axi = axi_sa;
  return is;
}
