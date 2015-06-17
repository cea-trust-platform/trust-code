/****************************************************************************
* Copyright (c) 2015, CEA
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
//////////////////////////////////////////////////////////////////////////////
//
// File:        LataToMED.cpp
// Directory:   $TRUST_ROOT/src/Kernel/MEDimpl
// Version:     /main/34
//
//////////////////////////////////////////////////////////////////////////////

#include <LataToMED.h>
#include <Domaine.h>
//#include <EcrMED.h>
#include <EFichierBin.h>
#include <List_Nom.h>
#include <EChaine.h>
#include <Motcle.h>
#include <communications.h>
#include <PE_Groups.h>
#include <Lire_Fichier.h>
#include <SFichier.h>

#include <LataFilter.h>
#include <LmlReader.h>
#include  <Format_Post.h>

Implemente_instanciable(LataToMED,"Lata_To_MED",Interprete);
Implemente_instanciable(latatoother,"lata_to_other",Interprete);


// Description:
//    Simple appel a: Interprete::printOn(Sortie&)
// Precondition:
// Parametre: Sortie& os
//    Signification: un flot de sortie
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Sortie&
//    Signification: le flot de sortie modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition: la methode ne modifie pas l'objet
Sortie& LataToMED::printOn(Sortie& os) const
{
  return Interprete::printOn(os);
}


// Description:
//    Simple appel a: Interprete::readOn(Entree&)
// Precondition:
// Parametre: Entree& is
//    Signification: un flot d'entree
//    Valeurs par defaut:
//    Contraintes:
//    Acces: entree/sortie
// Retour: Entree&
//    Signification: le flot d'entree modifie
//    Contraintes:
// Exception:
// Effets de bord:
// Postcondition:
Entree& LataToMED::readOn(Entree& is)
{
  return Interprete::readOn(is);
}

void convert_domain_to_Domaine(  const Domain& dom , Domaine& dom_trio)
{





  //      dom_trio.les_sommets()=geom.nodes_;
  // mais geom.nodes est un FloatTab
  DoubleTab& som= dom_trio.les_sommets();
  int nx=1,ny=1,nz=1;
  if (dom.get_domain_type()==Domain::UNSTRUCTURED)
    {
      const DomainUnstructured& geom = dom.cast_DomainUnstructured();
      int dim1=geom.nodes_.dimension(0);
      int dim2=geom.nodes_.dimension(1);
      som.resize(dim1,dim2);
      for (int i1=0; i1<dim1; i1++)
        for (int i2=0; i2<dim2; i2++)
          som(i1,i2)=geom.nodes_(i1,i2);
    }
  else
    {
      assert(dom.get_domain_type()==Domain::IJK);
      const DomainIJK& geom = dom.cast_DomainIJK();
      const int dim = geom.coord_.size();


      nx=geom.coord_[0].size_array();
      ny=geom.coord_[1].size_array();
      nz=1;
      if (dim>2) nz=geom.coord_[2].size_array();

      som.resize(nx*ny*nz,dim);

      for (int i=0; i<nx; i++)
        for (int j=0; j<ny; j++)
          for (int k=0; k<nz; k++)
            {
              int nn=i*ny*nz+j*nz+k;
              som(nn,0)=geom.coord_[0][i];
              som(nn,1)=geom.coord_[1][j];
              if (dim>2)
                som(nn,2)=geom.coord_[2][k];
            }

    }

  Zone zone_bidon;
  dom_trio.add(zone_bidon);
  Zone& zone=dom_trio.zone(0);
  zone.nommer("PAS_NOMME");
  Nom type_elem=dom.lata_element_name(dom.elt_type_);
  if (type_elem=="PRISM6") type_elem="PRISME";
  // zone.type_elem()=type_ele;
  zone.typer(type_elem);

  zone.type_elem().associer_zone(zone);
  zone.associer_domaine(dom_trio);
  if (dom.get_domain_type()==Domain::UNSTRUCTURED)
    {
      const DomainUnstructured& geom = dom.cast_DomainUnstructured();
      zone.les_elems()=geom.elements_;
    }
  else
    {

      zone.les_elems().resize((nx-1)*(ny-1)*(nz-1),zone.nb_som_elem());
      IntTab& elems=zone.les_elems();
      for (int i=0; i<nx-1; i++)
        for (int j=0; j<ny-1; j++)
          for (int k=0; k<nz-1; k++)
            {
              int nn=i*(ny-1)*(nz-1)+j*(nz-1)+k;
              nn=i+j*(nx-1)+k*((nx-1)*(ny-1));
              elems(nn,0)=i*ny*nz+j*nz+k;
              elems(nn,1)=(i+1)*ny*nz+j*nz+k;
              elems(nn,2)=(i)*ny*nz+(j+1)*nz+k;
              elems(nn,3)=(i+1)*ny*nz+(j+1)*nz+k;
              if (elems.dimension(1)>4)
                for (int ii=0; ii<4; ii++) elems(nn,4+ii)=elems(nn,ii)+1;
            }
      // a coder
      //Process::exit();
    }
  zone.faces_bord().associer_zone(zone);
  zone.faces_raccord().associer_zone(zone);
  zone.faces_joint().associer_zone(zone);


  zone.type_elem().associer_zone(zone);
  zone.fixer_premieres_faces_frontiere();



  if (dom.id_.timestep_!=0)
    dom_trio.deformable()=1;

}

Entree& latatoother::interpreter(Entree& is)
{
  Cerr<<"syntax  Latatoother::interpreter format_post_supp [nom_lata||NOM_DU_CAS] [nom_fichier_sortie||NOM_DU_CAS]   "<<finl;
  Nom nom_dom, nom_lata,nom_fic;
  Nom format_post_supp;
  is >>format_post_supp>> nom_lata>>nom_fic ;

  // Creation d'un sous-groupe contenant uniquement le processeur maitre
  ArrOfInt liste_pe(1, 0);
  DERIV(Comm_Group) group;
  // on se met en non axi le temps de la conversion
  int axi_sa=axi;
  axi=0;
  PE_Groups::create_group(liste_pe, group);

  if (PE_Groups::enter_group(group.valeur()))
    {

      if (nom_lata=="NOM_DU_CAS")
        nom_lata=nom_du_cas()+".lata";
      if (nom_fic=="NOM_DU_CAS")
        nom_fic=nom_du_cas();
      Nom filename(nom_lata);
      LataOptions opt;
      LataDB  lata_db;
      LataFilter  filter;
      set_Journal_level(0);

      LataOptions::extract_path_basename(filename, opt.path_prefix, opt.basename);
      opt.dual_mesh = true;
      opt.faces_mesh = false;
      // on ne veut jamais creer le domaine IJK
      opt.regularize = 0;
      opt.regularize_tolerance = 1e-7;
      read_any_format_options(filename, opt);
      // Read the source file to the lata database
      read_any_format(filename, opt.path_prefix, lata_db);

      filter.initialize(opt, lata_db);



      Nom nom_pdb(nom_fic);
      Nom format_post_(format_post_supp);
      if (format_post_=="lata")
        format_post_+="_V1";

      // copie de Postraiter_domaine

      Nom type("Format_Post_");
      type+=format_post_;
      Format_Post  post_typer;
      post_typer.typer_direct(type);
      Format_Post_base& post=ref_cast(Format_Post_base,post_typer.valeur());
      Nom nom_2(nom_pdb);
      Nom format_post_bis(format_post_);
      if (format_post_bis.debute_par("lata"))
        format_post_bis="lata";
      Nom suffix(".");
      suffix+=format_post_bis;
      nom_2.prefix(suffix);
      //      int est_le_premie_post=0;
      int format_binaire_=0;
      if (format_post_bis!="lata")
        post.initialize_by_default(nom_2);
      else
        post.initialize(nom_2,format_binaire_,"SIMPLE");


      // ecriture des domaines
      {

        Noms geoms = filter.get_exportable_geometry_names();

        for (int i = 0; i < geoms.size(); i++)
          {
            Domain_Id id(geoms[i], 1, -1);
            const Domain& dom = filter.get_geometry(id);

            Domaine dom_trio;
            dom_trio.nommer(geoms[i]);
            convert_domain_to_Domaine(dom,dom_trio);
            int est_le_premier_post=(i==0);
            post.ecrire_entete(0.,0,est_le_premier_post);
            int reprise = 0;
            double t_init = 0.;
            post.preparer_post(dom_trio.le_nom(),est_le_premier_post,reprise,t_init);
            post.ecrire_domaine(dom_trio, est_le_premier_post);
            //meddrive.ecrire_domaine(nom_fic,dom_trio,dom_trio.le_nom(),mode);


            filter.release_geometry(dom);
          }
      }
      // les champs
      const char *suffix_vector_names[] = { "X", "Y", "Z" };

      for (int t = 1; t < lata_db.nb_timesteps(); t++)
        {
          int timestate=t;
          //    if (filter.get_nb_timesteps() > 1)      timestate++;
          Noms geoms = filter.get_exportable_geometry_names();
          double time=filter.get_timestep(t);
          post.ecrire_temps(time);
          for (int i = 0; i < geoms.size(); i++)
            {
              Domain_Id id(geoms[i], timestate, -1);
              const Domain& dom = filter.get_geometry(id);

              Domaine dom_trio;
              dom_trio.nommer(geoms[i]);
              convert_domain_to_Domaine(dom,dom_trio);
              Field_UNames fields = filter.get_exportable_field_unames(geoms[i]);
              for (int j = 0; j < fields.size(); j++)
                {
                  //const Nom& nom= fields[j].get_field_name();
                  //  if ((nom!=Motcle("ELEM_FACES"))&& (nom!=Motcle("FACES"))&&  (nom!=Motcle("ELEMENTS")))
                  {

                    int est_le_premie_post=(i==0);
                    post.init_ecriture(time,-1.,est_le_premie_post,dom_trio);
                    Field_Id fieldid(fields[j],timestate,-1);

                    DoubleTab values;
                    try
                      {
                        const FieldFloat& field = filter.get_float_field(fieldid);
                        {
                          const FloatTab& values_lata = field.data_;
                          int dim1=values_lata.dimension(0);
                          int dim2=values_lata.dimension(1);
                          values.resize(dim1,dim2);
                          for (int i1=0; i1<dim1; i1++)
                            for (int i2=0; i2<dim2; i2++)
                              values(i1,i2)=values_lata(i1,i2);
                        }


                        Nom nom_type;
                        if (field.localisation_==LataField_base::ELEM)
                          nom_type="ELEM";//"CHAMPMAILLE";
                        else if (field.localisation_==LataField_base::SOM)
                          nom_type="SOM";// CHAMPPOINT";
                        else
                          {
                            Cerr<<nom_type<<" not coded ...... "<<finl;
                            Cerr<<__FILE__<<":"<<__LINE__<<finl;
                            exit();

                          }
                        Nom type_elem=dom.lata_element_name(dom.elt_type_);
                        Noms unites(values.dimension(1));
                        Noms noms_post(values.dimension(1));
                        Cerr<<"Extraction "<< time<<" "<< geoms[i]<<" "<< fields[j].get_field_name()<<finl;
                        Nom nom_post,ajout;

                        nom_post=fields[j].get_field_name();
                        // pour eviter qu'en med on est le champ aux faces et sur le dual avec le meme nom
                        if (dom_trio.le_nom().finit_par("_faces")) nom_post+="_Centre";
                        ajout="_";
                        ajout+=nom_type;
                        ajout+="_";
                        ajout+=geoms[i];
                        for (int ii=0; ii<values.dimension(1); ii++)
                          {
                            noms_post[ii]=nom_post;
                            if (values.dimension(1)==dom_trio.les_sommets().dimension(1))
                              noms_post[ii]+=suffix_vector_names[ii];
                            else
                              noms_post[ii]+=Nom(ii);
                            noms_post[ii]+=ajout;
                          }
                        nom_post+=ajout;
                        //exit();
                        //meddrive.ecrire_champ(type,nom_fic,geoms[i],fields[j].get_field_name(),values,unites,type_elem   ,time,0);

                        post.ecrire_champ(dom_trio,unites,noms_post,-1,time,time,nom_post,geoms[i],nom_type,"iii",values);
                        filter.release_field(field);
                      }
                    catch(...)
                      {
                        Cerr<<fieldid.uname_.get_field_name()<<" is not a FloatField !!!"<<finl;
                      }
                  }
                }
              filter.release_geometry(dom);
            }


        }
      int fin=1;
      post.finir(fin);


      //  delete [] les_domaines;
      //delete [] list_champ;
      PE_Groups::exit_group();
    }
  barrier();
  // on remet eventuellement axi
  axi=axi_sa;
  return is;
}

Entree& latatoother::readOn(Entree& is)
{
  Cerr<<__FILE__<<__LINE__<<" not coded"<<finl;
  Process::exit();
  return is;
}
Sortie& latatoother::printOn(Sortie& is) const
{
  Cerr<<__FILE__<<__LINE__<<" not coded"<<finl;
  Process::exit();
  return is;
}

Entree& LataToMED::interpreter(Entree& is)
{
  Cerr<<"Syntax Latatomed::interpreter nom_lata||NOM_DU_CAS nom_fichier_sortie||NOM_DU_CAS   "<<finl;
  Nom nom1,nom2;

  is >> nom1>> nom2;
  Nom instruction("med ");
  instruction+=nom1;
  instruction+=" ";
  instruction+=nom2;
  Cerr<<"command generated by latatomed "<<instruction<<finl;
  {
    latatoother latatoother;
    EChaine cmd(instruction);
    latatoother.interpreter(cmd);
  }

  return is;
}
