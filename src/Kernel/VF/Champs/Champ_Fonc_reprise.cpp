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

#include <EcritureLectureSpecial.h>
#include <Champ_Generique_base.h>
#include <Champ_Fonc_reprise.h>
#include <LecFicDiffuseBin.h>
#include <Entree_complete.h>
#include <Probleme_base.h>
#include <stat_counters.h>
#include <Ecrire_YAML.h>
#include <TRUST_2_PDI.h>
#include <Op_Moyenne.h>
#include <Parser_U.h>
#include <Avanc.h>

extern void convert_to(const char *s, double& ob);
Implemente_instanciable(Champ_Fonc_reprise,"Champ_Fonc_reprise",Champ_Fonc_base);
// XD champ_fonc_reprise champ_don_base champ_fonc_reprise 0 This field is used to read a data field in a save file (.xyz or .sauv) at a specified time. It is very useful, for example, to run a thermohydraulic calculation with velocity initial condition read into a save file from a previous hydraulic calculation.
// XD  attr format chaine(into=["binaire","formatte","xyz","single_hdf","pdi"]) format 1 Type of file (the file format). If xyz format is activated, the .xyz file from the previous calculation will be given for filename, and if formatte or binaire is choosen, the .sauv file of the previous calculation will be specified for filename. In the case of a parallel calculation, if the mesh partition does not changed between the previous calculation and the next one, the binaire format should be preferred, because is faster than the xyz format. If pdi is used, the same constraints/advantages as binaire apply, but it produces one (HDF5) file per node on the filesystem instead of having one file per processor. The single_hdf format is still supported but is obsolete, the PDI format is recommended.
// XD attr filename chaine filename 0 Name of the save file.
// XD attr pb_name ref_Pb_base pb_name 0 Name of the problem.
// XD attr champ chaine champ 0 Name of the problem unknown. It may also be the temporal average of a problem unknown (like moyenne_vitesse, moyenne_temperature,...)
// XD attr fonction fonction_champ_reprise fonction 1 Optional keyword to apply a function on the field being read in the save file (e.g. to read a temperature field in Celsius units and convert it for the calculation on Kelvin units, you will use: fonction 1 273.+val )
// XD attr temps chaine time 0 Time of the saved field in the save file or last_time. If you give the keyword last_time instead, the last time saved in the save file will be used.

// XD fonction_champ_reprise objet_lecture nul 0 not_set
// XD attr mot chaine(into=["fonction"]) mot 0 not_set
// XD attr fonction listchaine fonction 0 n f1(val) f2(val) ... fn(val)] time

Sortie& Champ_Fonc_reprise::printOn(Sortie& s) const
{
  return s << que_suis_je() << " " << le_nom();
}


Entree& Champ_Fonc_reprise::readOn(Entree& s)
{
  Cerr<<"Usage : Champ_Fonc_reprise [xyz|formatte|binaire|single_hdf|pdi] fichier.xyz nom_pb nom_inco [fonction n f1(val) f2(val) ... fn(val)] [temps|last_time]"<<finl;
  Nom nom_fic,nom_pb,nom_champ;
  Motcle format_rep("xyz");
  // Lecture
  s>>nom_fic;
  if ((nom_fic==Motcle("xyz"))||(nom_fic==Motcle("binaire"))||(nom_fic==Motcle("single_hdf"))||(nom_fic==Motcle("pdi"))||(nom_fic==Motcle("formatte")))
    {
      format_rep=nom_fic;
      s>>nom_fic;
    }
  s>>nom_pb>>nom_champ;

  if (format_rep == "single_hdf" && Process::nproc() == 1)
    {
      Cerr << "Resuming with a single_hdf option is only available for a parallel calculation !" << finl;
      Process::exit();
    }

  int last_time=0;
  double un_temps=-1;
  Motcle ch, time;
  s>>ch;
  VECT(Parser_U) fxyz;
  if (ch=="FONCTION")
    {
      int nb_compo;
      s >> nb_compo;
      fxyz.dimensionner(nb_compo);
      Nom fonction;
      for (int i = 0; i<nb_compo; i++)
        {
          s >> fonction;
          fxyz[i].setNbVar(1);
          fxyz[i].setString(fonction);
          fxyz[i].addVar("val");
          fxyz[i].parseString();
        }
      s >> time;
      if (time=="LAST_TIME")
        last_time=1;
      else
        convert_to(time,un_temps);
    }
  else
    {
      if (ch=="LAST_TIME")
        last_time=1;
      else
        convert_to(ch,un_temps);
    }


  // On recupere le pb, puis ensuite on cherche le champ; on recupere le domaine_dis
  const Probleme_base& pb =ref_cast(Probleme_base,Interprete::objet(nom_pb));
  int reprend_champ_moyen=0;
  int reprend_modele_k_eps=0;
  int k_eps_realisable = 0;
  Nom nom_champ_inc;

  // Cas des champs moyens
  if (((Motcle)nom_champ).debute_par("MOYENNE_"))
    {
      nom_champ_inc=((Motcle)nom_champ).suffix("MOYENNE_");
      nom_champ+="_natif_";
      nom_champ+=pb.domaine().le_nom();
      reprend_champ_moyen=1;
    }
  // Cas du K-Epsilon
  if (((Motcle)nom_champ).debute_par("K_EPS"))
    {
      reprend_modele_k_eps=1;
      // cas du k-epsilon realisable
      if (Motcle(nom_champ).finit_par("_REALISABLE"))
        {
          k_eps_realisable = 1;
          nom_champ_inc=((Motcle)nom_champ).prefix("_REALISABLE");
        }
    }
  if (nom_champ_inc=="??")
    nom_champ_inc=nom_champ;
  OBS_PTR(Champ_base) ref_ch;
  ref_ch = pb.get_champ(Motcle(nom_champ_inc));
  if (sub_type(Champ_Inc_base,ref_ch.valeur()))
    Cerr << nom_champ_inc << " is an unknown of problem " << nom_pb << finl;
  else
    {
      Cerr << nom_champ_inc << " is not an unknown of problem " << nom_pb << finl;
      Process::exit();
    }

  associer_domaine_dis_base(pb.domaine_dis());
  // on cree un champ comme le ch_ref;
  const Champ_Inc_base& ch_inc=ref_cast(Champ_Inc_base,ref_ch.valeur());
  vrai_champ_.typer(ch_inc.que_suis_je());
  vrai_champ_->associer_domaine_dis_base(pb.domaine_dis());
  //vrai_champ_->fixer_nb_valeurs_temporelles(2);
  vrai_champ_->nommer(ch_inc.le_nom());
  vrai_champ_->fixer_nb_comp(ch_inc.nb_comp());
  //vrai_champ_->fixer_nb_valeurs_nodales(ch_inc.nb_valeurs_nodales());
  vrai_champ_->valeurs() = ch_inc.valeurs();
  vrai_champ_->set_via_ch_fonc_reprise(); // useful for PolyMAC for the moment !
  nb_compo_ = ch_inc.nb_comp();

  // creation des identifiants pdi (necessaire pour l'initialisation de PDI)
  Nom nom_champ_pdi = Motcle(pb.le_nom()) + "_" + Motcle(nom_champ);

  statistiques().begin_count(temporary_counter_);

  // Opening file + get file format
  int mode_lec_sa= EcritureLectureSpecial::mode_lec;
  OWN_PTR(Entree_Fichier_base) fic_rep;
  Entree_Brute input_data;
  int format_sauvegarde = 0;
  if(format_rep == "single_hdf")
    {
      // !! DEPRECATED HDF5 FILE !!
      Cerr << "WARNING::you are using a deprecated backup file format. Please switch to PDI." << finl;
      LecFicDiffuse test;
      if (!test.ouvrir(nom_fic))
        {
          Cerr << "Error! " << nom_fic << " file not found ! " << finl;
          Process::exit();
        }
      FichierHDFPar fic_hdf;
      fic_hdf.open(nom_fic, true);
      fic_hdf.read_dataset("/sauv", Process::me(),input_data);

      if(last_time)
        {
          un_temps = get_last_time(input_data);
          fic_hdf.read_dataset("/sauv", Process::me(), input_data);
          Cerr << "In the " << nom_fic << " file, we find the last time: " << un_temps << " and read the fields." << finl;
        }

      Nom ident_lu;
      input_data >> ident_lu;
      format_sauvegarde=0;
      if (ident_lu=="format_sauvegarde:")
        input_data >> format_sauvegarde;
      else
        {
          Cerr<<"This .sauv file is too old and the format is not supported anymore."<<finl;
          Process::exit();
        }
      fic_hdf.close();
    }
  else if (format_rep == "pdi")
    init_pdi(pb, nom_fic, nom_champ_pdi, last_time, un_temps, reprend_champ_moyen);
  else
    {
      if (format_rep == "xyz")
        {
          EcritureLectureSpecial::mode_lec=1;
          fic_rep.typer(EcritureLectureSpecial::Input);
          Cerr << "Opening xyz file " << nom_fic << " (" << EcritureLectureSpecial::Input << ")" << finl;
        }
      else
        {
          EcritureLectureSpecial::mode_lec=0;
          if (format_rep == "binaire")
            fic_rep.typer("LecFicDistribueBin");
          else
            fic_rep.typer("LecFicDistribue");
          Cerr << "Opening file " << nom_fic << " (LecFicDistribueBin)" << finl;
        }

      fic_rep->ouvrir(nom_fic);

      if(fic_rep->fail())
        {
          Cerr<<"Error while opening the file of resumption : " <<nom_fic<<finl;
          Process::exit();
        }

      if (fic_rep->eof())
        {
          Cerr << "Error in Champ_Fonc_reprise::reprendre" << finl;
          Cerr << "The resumption file does not exist" << finl;
          Cerr << "or could not be opened correctly." << finl;
          Process::exit();
        }

      if(last_time)
        {
          un_temps = get_last_time(fic_rep.valeur());
          fic_rep->close();
          fic_rep->ouvrir(nom_fic);
          Cerr << "In the " << nom_fic << " file, we find the last time: " << un_temps << " and read the fields." << finl;
        }

      Nom ident_lu;
      fic_rep.valeur() >> ident_lu;
      format_sauvegarde=0;
      if (ident_lu=="format_sauvegarde:")
        fic_rep.valeur() >> format_sauvegarde;
      else
        {
          // Version anterieure, on referme et on reouvre
          if (format_rep=="xyz")
            {
              fic_rep->close();
              fic_rep->ouvrir(nom_fic);
            }
          else
            {
              Cerr<<"This .sauv file is too old and the format is not supported anymore."<<finl;
              Process::exit();
            }
        }
    }

  // Creation des identifiants
  Nom nom_temps=Nom(un_temps,time_format_from(format_sauvegarde));
  Nom type=ref_ch->que_suis_je();
  Nom nom_ident;
  Nom nom_ident_champ_stat;
  Nom nom_ident_champ_keps;
  if (reprend_champ_moyen)
    {
      nom_ident="Operateurs_Statistique_tps";
      nom_ident+=pb.domaine().le_nom();
      nom_ident+=nom_temps;

      nom_ident_champ_stat=nom_champ;
      nom_ident_champ_stat+="Champ_Fonc";
      type.suffix("Champ");
      nom_ident_champ_stat+=type;
      nom_ident_champ_stat+=pb.domaine().le_nom();
      nom_ident_champ_stat+=nom_temps;
    }
  else if (reprend_modele_k_eps)
    {
      nom_ident="Modele_turbulence_hyd_K_Epsilon";
      if(k_eps_realisable)
        nom_ident+="_Realisable";
      nom_ident+=pb.domaine().le_nom();
      nom_ident+=nom_temps;
      nom_ident_champ_keps=nom_champ_inc;
      nom_ident_champ_keps+=type;
      nom_ident_champ_keps+=pb.domaine().le_nom();
      nom_ident_champ_keps+=nom_temps;
    }
  else
    {
      nom_ident=nom_champ_inc;
      nom_ident+=type;
      nom_ident+=pb.domaine().le_nom();
      nom_ident+=nom_temps;
    }

  if (ch=="FONCTION")
    if (fxyz.size()!=ch_inc.nb_comp())
      {
        Cerr<<"The number of components read " <<fxyz.size()<<" for the resume function"<<finl;
        Cerr<<"does not corresponds to the number of components "<<ch_inc.nb_comp()<<" of field target"<<finl;
        Process::exit();
      }

  if(format_rep == "single_hdf")
    read_field_from_file(s, input_data, pb, nom_ident, nom_ident_champ_stat, reprend_champ_moyen, nom_ident_champ_keps, reprend_modele_k_eps);
  else if(format_rep == "pdi")
    {
      Entree bidon;
      read_field_from_file(s, bidon, pb, nom_champ_pdi, "", reprend_champ_moyen, "", reprend_modele_k_eps, 1 /*pdi_format*/);
    }
  else
    read_field_from_file(s, fic_rep.valeur(), pb, nom_ident, nom_ident_champ_stat, reprend_champ_moyen, nom_ident_champ_keps, reprend_modele_k_eps);

  EcritureLectureSpecial::mode_lec=mode_lec_sa;
  statistiques().end_count(temporary_counter_);
  Cerr << "End of resuming the file " << nom_fic << " after " << statistiques().last_time(temporary_counter_) << " s" << finl;

  ////////////////////////////////////////
  // Transformation eventuelle du champ lu
  ////////////////////////////////////////
  if (fxyz.size())
    {
      DoubleTab& tab_valeurs=le_champ().valeurs();
      // Boucle sur les noeuds du champs
      int sz=tab_valeurs.dimension_tot(0);
      int nb_compo=fxyz.size();
      if (tab_valeurs.nb_dim()==1)
        {
          if (nb_compo!=1)
            {
              Cerr << "Error in the function read because the field " << le_champ().le_nom() << " has a single dimension." << finl;
              Process::exit();
            }
        }
      else if (nb_compo!=tab_valeurs.dimension_tot(1))
        {
          Cerr << "Error in the number of components of the function read:" << nb_compo << finl;
          Cerr << "The number of components of field " << le_champ().le_nom() << " is:" << tab_valeurs.dimension_tot(1) << finl;
          Process::exit();
        }
      for (int i=0; i<sz; i++)
        {
          for (int j=0; j<nb_compo; j++)
            {
              fxyz[j].setVar("val",tab_valeurs(i,j));
              // On fait la transformation des valeurs du champs
              tab_valeurs(i,j)=fxyz[j].eval();
            }
        }
    }

  if(format_rep == "pdi")
    TRUST_2_PDI::finalize();

  return s ;
}

void Champ_Fonc_reprise::init_pdi(const Probleme_base& pb, const Nom& nom_fic, const Nom& ch_ident, int last_time, double un_temps, int reprend_champ_moyen)
{
  TRUST_2_PDI::set_PDI_restart(1);

  Ecrire_YAML yaml_file;
  yaml_file.add_pb_base(pb, nom_fic);
  int nb_dim = le_champ().valeurs().nb_dim();
  const Nom& pbname = pb.le_nom();
  yaml_file.add_field(pbname, ch_ident, nb_dim);
  if(reprend_champ_moyen)
    {
      yaml_file.add_scalar(pbname, pbname + "_stat_nb_champs", "int", false /* same data for every proc */ );
      yaml_file.add_scalar(pbname, pbname + "_stat_tdeb", "double", false /* same data for every proc */);
      yaml_file.add_scalar(pbname, pbname + "_stat_tend", "double", false /* same data for every proc */);
    }
  std::string yaml_fname = "restart_" + pbname.getString() + "_" + le_champ().le_nom().getString() + ".yml";
  yaml_file.write_champ_fonc_restart_file(yaml_fname);
  TRUST_2_PDI::init(yaml_fname);

  TRUST_2_PDI pdi_interface;
  int last_iteration = -1;
  double tinit = last_time ? -1. : un_temps;
  pdi_interface.prepareRestart(last_iteration, tinit, last_time);

}

void Champ_Fonc_reprise::read_field_from_file(Entree& jdd, Entree& file, const Probleme_base& pb, const Nom& nom_ident,
                                              const Nom& nom_ident_champ_stat, int reprend_champ_moyen, const Nom& nom_ident_champ_keps, int reprend_modele_k_eps,
                                              int pdi_format)
{
  // Lecture du fichier
  if(!pdi_format)
    avancer_fichier(file, nom_ident);
  else
    le_champ().set_PDI_dname(nom_ident);

  if (reprend_champ_moyen)
    {
      double tdeb = -1,tfin=-1;
      int n; // Nombre d'operateurs statistiques
      if(pdi_format)
        {
          TRUST_2_PDI pdi_interface;
          std::string pbname = pb.le_nom().getString();
          pdi_interface.read(pbname + "_stat_nb_champs", &n);
          pdi_interface.read(pbname + "_stat_tdeb", &tdeb);
          pdi_interface.read(pbname + "_stat_tend", &tfin);
        }
      else
        {
          file >> n;
          file >> tdeb;
          file >> tfin;
          avancer_fichier(file,nom_ident_champ_stat);
        }
      // On cree un operateur_statistique qui va nous permettre de relire le champ moyen
      Op_Moyenne champ_moyen;
      //On construit un Champ_Generique_refChamp pour pouvoir associer un Champ_Generique_base
      //a l operateur
      OWN_PTR(Champ_Generique_base) champ;
      Nom ajout("");
      ajout += " refChamp { Pb_champ ";
      Nom pb_name = pb.le_nom();
      ajout += pb_name;
      ajout += " ";
      ajout += le_champ().le_nom();
      ajout += " }";
      Entree_complete s_complete(ajout,jdd);
      s_complete>>champ;
      champ_moyen.associer(pb.domaine_dis(),champ.valeur(),tdeb,tfin);
      champ_moyen.completer(pb, "");
      champ_moyen.set_pdi_name(nom_ident);
      champ_moyen.fixer_tstat_deb(tdeb,tfin);
      champ_moyen.reprendre(file);

      // On remplit le champ
      le_champ().valeurs() = champ_moyen.calculer_valeurs();
    }
  else if (reprend_modele_k_eps)
    {
      if(!pdi_format)
        avancer_fichier(file, nom_ident_champ_keps);
      le_champ().reprendre(file);
    }
  else
    le_champ().reprendre(file);
}

void Champ_Fonc_reprise::mettre_a_jour(double t)
{
  Champ_Fonc_base::mettre_a_jour(t);
}
