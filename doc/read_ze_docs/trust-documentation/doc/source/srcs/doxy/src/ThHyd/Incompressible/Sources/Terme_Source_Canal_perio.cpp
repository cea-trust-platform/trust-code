/****************************************************************************
* Copyright (c) 2022, CEA
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

#include <Terme_Source_Canal_perio.h>
#include <Motcle.h>
#include <Double.h>
#include <Probleme_base.h>
#include <communications.h>
#include <Domaine_VF.h>
#include <Domaine_Cl_dis_base.h>
#include <Periodique.h>
#include <Neumann_paroi.h>
#include <Convection_Diffusion_std.h>
#include <Fluide_Incompressible.h>
#include <Operateur_Diff_base.h>
#include <Navier_Stokes_std.h>
#include <EFichier.h>
#include <Param.h>

Implemente_base_sans_constructeur_ni_destructeur(Terme_Source_Canal_perio,"Terme_Source_Canal_perio",Source_base);


Terme_Source_Canal_perio::Terme_Source_Canal_perio():
  direction_ecoulement_(-1),
  velocity_weighting_(0),
  bord_periodique_(""),
  surface_bord_(0.0),
  h(0.0), coeff(0.0), u_etoile(0.0),
  deb_(0.0),
  source_(0.0),
  debnm1_(0.0),
  debit_ref_(0.0),
  dernier_temps_calc_(-1.0),  // why??
  is_debit_impose_(0),
  debit_impose_(0.0)
{
}

Terme_Source_Canal_perio::~Terme_Source_Canal_perio()
{}

Sortie& Terme_Source_Canal_perio::printOn(Sortie& s ) const
{
  return s << que_suis_je() ;
}

//
//   //// readOn
//   //

Entree& Terme_Source_Canal_perio::readOn(Entree& is )
{
  Param param(que_suis_je());
  // Valeurs par defaut
  u_etoile = 0.;
  h = 1.;
  coeff = 10.;
  velocity_weighting_ = 0;
  // FIN valeurs par defaut
  dir_source_.resize(dimension);
  dir_source_=0;
  set_param(param);
  param.lire_avec_accolades_depuis(is);
  return is;
}

void Terme_Source_Canal_perio::set_param(Param& param)
{
  param.ajouter_non_std("direction_ecoulement",(this));
  param.ajouter("u_etoile",&u_etoile);
  param.ajouter("coeff",&coeff);
  param.ajouter("h",&h);
  param.ajouter("bord",&bord_periodique_);
  param.ajouter_non_std("debit_impose",(this));
  param.ajouter_non_std("velocity_weighting",(this));
}

void Terme_Source_Canal_perio::associer_pb(const Probleme_base& pb)
{}

int Terme_Source_Canal_perio::lire_motcle_non_standard(const Motcle& mot, Entree& is)
{
  int retval = 1;

  if (mot=="direction_ecoulement")
    {
      Cerr <<"The direction_ecoulement option is obsolete, you must now use the bord option to specify the boundary where periodicity is applied."<<finl;
      Process::exit();
    }
  else if (mot=="debit_impose")
    {
      is >> debit_impose_;
      is_debit_impose_=1;
    }
  else if (mot=="velocity_weighting")
    {
      is >> velocity_weighting_;
      if (velocity_weighting_!=0 && velocity_weighting_!=1)
        {
          Cerr << "velocity_weighting value should be 0 or 1." << finl;
          Process::exit();
        }
      if (!sub_type(Convection_Diffusion_std,equation()))
        {
          Cerr << "velocity_weighting option is available only for a Canal_perio source term in the energy equation." << finl;
          Process::exit();
        }
    }
  else retval = -1;

  return retval;
}

void Terme_Source_Canal_perio::completer()
{
  Source_base::completer();
  if (sub_type(Convection_Diffusion_std,equation()))
    {
      set_fichier("Canal_perio");
      set_description("Energy source term = Integral(P*dv) [W]");
    }
  int nb_bords = equation().domaine_dis().valeur().nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = equation().domaine_Cl_dis().valeur().les_conditions_limites(n_bord);
      if (sub_type(Periodique,la_cl.valeur()))
        {
          const Periodique& perio = ref_cast(Periodique,la_cl.valeur());
          const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
          if ( bord_periodique_ == le_bord.le_nom() ) // Le bord periodique est specifie
            {
              if (perio.est_periodique_selon_un_axe())
                {
                  // Cas ou le bord est periodique selon un axe, alors on fixe la direction de l'ecoulement (VDF)
                  direction_ecoulement_ = perio.direction_periodicite();
                }
              else
                {
                  // Cas general ou le bord periodique n'est pas oriente selon un axe:
                  dir_source_ = perio.direction_perio();
                  dir_source_ *= -1;
                  // On adimensionnalise:
                  double norme = norme_array(dir_source_);
                  dir_source_ /= norme;
                }
              // On recupere la surface
              surface_bord_ = 0.5 * le_bord.frontiere().get_aire();
              return;
            }
        }
    }
  Cerr << "********************************************************" << finl;
  Cerr << "Error for the definition of the Canal_perio source term." << finl;
  if (bord_periodique_=="")
    {
      Cerr << "It seems the channel is not parallel to an axis. Direction_ecoulement keyword is useless in this case." << finl;
      Cerr << "So try to use the following syntax to specify the name of the periodic boundary:" << finl;
      Cerr << "Canal_perio { bord name }" << finl;
    }
  else
    {
      Cerr << bord_periodique_ << " is not a boundary with a periodic boundary condition." << finl;
    }
  exit();
}

void Terme_Source_Canal_perio::write_flow_rate(const Nom& ext_nom_source_, double debit_e) const
{
  double tps = equation().schema_temps().temps_courant();
  double tps_init = equation().schema_temps().temps_init();
  double dt = equation().schema_temps().pas_de_temps();
  int premiere_ecriture = (!equation().probleme().reprise_effectuee() && deb_==0 ? 1 : 0);

  // Write the flow rate file if equation is Navier Stokes:
  Nom filename(nom_du_cas());
  filename+="_Channel_Flow_Rate_";
  filename+=ext_nom_source_ ;
  if (!flow_rate_file_.is_open())
    {
      flow_rate_file_.ouvrir(filename, (premiere_ecriture?ios::out:ios::app));
      flow_rate_file_.setf(ios::scientific);
    }
  // on met des commentaires dans l'entete du fichier
  if ((tps <= (tps_init+dt)) && premiere_ecriture)
    {
      if (equation().probleme().is_dilatable()==1)
        flow_rate_file_ << "# Time t     Flow rate Q(t) in [kg.s-1] if SI units used" << finl;
      else
        flow_rate_file_ << "# Time t     Flow rate Q(t) in [m3.s-1] if SI units used" << finl;
    }
  flow_rate_file_ << tps+dt << " " << debit_e << finl;

  if (deb_==0)
    {
      deb_ = 1;
      debit_ref_ = debit_e;
      if (is_debit_impose_)
        debit_ref_=debit_impose_;
      debnm1_ = debit_ref_;
      source_ = 0.;

      // Deplacer dans completer ?
      // Read the restart file:
      if (equation().probleme().reprise_effectuee())
        {
          Nom filename2(nom_du_cas());
          filename2+="_Channel_Flow_Rate_repr_";
          filename2+=ext_nom_source_ ;
          EFichier fichier_reprise;
          if (!fichier_reprise.ouvrir(filename2))
            {
              Cerr << "File " << filename2 << " not found !" << finl;
              Cerr << "Since the 1.6.8 version, you absolutly need this file to restart the calculation." << finl;
              Cerr << "Look for a file named *Channel_Flow_Rate_repr* and renamed it for example." << finl;
              exit();
            }
          else
            {
              // Check header
              Nom str;
              fichier_reprise >> str;
              if (str=="#")
                {
                  // Read the whole line:
                  std::string line;
                  std::getline(fichier_reprise.get_ifstream(), line);
                }
              else
                {
                  // Reopen:
                  fichier_reprise.ouvrir(filename);
                }
              // Read up to the time:
              int time_found = 0;
              while (!fichier_reprise.eof() && !time_found)
                {
                  double temps_ecrit;
                  fichier_reprise >> temps_ecrit;
                  fichier_reprise >> debnm1_;
                  fichier_reprise >> debit_ref_;
                  fichier_reprise >> source_;
                  //std::getline(fichier_reprise.get_ifstream(), line);
                  if (est_egal(temps_ecrit,tps_init,1e-5))
                    {
                      Cerr << "Source canal_perio read values in the file " << filename << " for the time t= " << temps_ecrit << finl;
                      time_found = 1;
                    }
                }
              if (time_found==0)
                {
                  Cerr << "Sorry, we didn't find the time " << tps_init << " in the file " << filename << finl;
                  Cerr << "We can't restart the calculation." << finl;
                  exit();
                }
            }
        }
    }
}

double Terme_Source_Canal_perio::compute_heat_flux() const
{
  // On recupere flux_bords operateur de diffusion
  const Operateur_base& op_base = equation().operateur(0).l_op_base(); // Diffusion operator
  assert(sub_type(Operateur_Diff_base,op_base)); // Check
  const DoubleTab& flux_bords = op_base.flux_bords();
  // If flux_bords is not build (diffusion_implicit algorithm for example, we calculate it):
  if (flux_bords.size()==0)
    {
      DoubleTab dummy(equation().inconnue().valeurs());
      equation().operateur(0).ajouter(equation().inconnue(), dummy);
    }
  // Loop on boundaries to evaluate total heat flux:
  double heat_flux=0;
  int nb_bords = equation().domaine_dis().valeur().nb_front_Cl();
  for (int n_bord=0; n_bord<nb_bords; n_bord++)
    {
      const Cond_lim& la_cl = equation().domaine_Cl_dis().valeur().les_conditions_limites(n_bord);
      if (sub_type(Neumann_paroi,la_cl.valeur()))
        {
          // Loop on boundary faces with imposed flux condition (Neumann)
          const Front_VF& frontiere_dis = ref_cast(Front_VF,la_cl.frontiere_dis());
          int ndeb = frontiere_dis.num_premiere_face();
          int nfin = ndeb + frontiere_dis.nb_faces();
          for (int num_face=ndeb; num_face<nfin; num_face++)
            heat_flux += flux_bords(num_face,0);
        }
    }
  heat_flux=mp_sum(heat_flux);
  return heat_flux;
}

ArrOfDouble Terme_Source_Canal_perio::source_convection_diffusion(double debit_e) const
{
  // Compute heat_flux:
  double heat_flux = compute_heat_flux();

  const Domaine_VF& domaine_vf = ref_cast(Domaine_VF,equation().domaine_dis().valeur());
  const double volume = domaine_vf.domaine().volume_total();
  int size = domaine_vf.nb_faces();
  ArrOfDouble s(size);
  if (velocity_weighting_) // It seems this algorithm do not imply dT/dt -> 0
    {
      // Compute source term for Energy
      // Expression from TU Delft Master Thesis
      // Source = -u*Sum(imposed_heat_flux)/(Volume*Ubulk)
      // Ubulk=FlowRate/Area(PeriodicBoundary)
      // Loop on the faces
      const DoubleTab& vitesse = ref_cast(Convection_Diffusion_std,equation()).vitesse_transportante().valeurs();
      for (int num_face=0; num_face<size; num_face++)
        {
          double velocity = 0;
          if (direction_ecoulement_>=0) // Ecoulement selon un axe
            velocity = vitesse(num_face,direction_ecoulement_);
          else // Cas general
            for (int i=0; i<dimension; i++)
              velocity += vitesse(num_face,i) * dir_source_[i];
          s[num_face]=-velocity*heat_flux/(volume*debit_e/surface_bord_);
        }
    }
  else
    {
      // Compute source term with
      // Source = -Sum(imposed_heat_flux)/Volume
      // Loop on the faces
      for (int num_face=0; num_face<size; num_face++)
        s[num_face]=-heat_flux/volume;
    }
  return s;
}

/*! @brief Term source calculation (called by VDF and VEF implementations) TODO: returning an ArrOfDouble is baaad.
 *
 */
ArrOfDouble Terme_Source_Canal_perio::source() const
{
  double tps = equation().schema_temps().temps_courant();
  double tps_init = equation().schema_temps().temps_init();
  double dt = equation().schema_temps().pas_de_temps();
  int premiere_ecriture = (!equation().probleme().reprise_effectuee() && deb_==0 ? 1 : 0);
  Nom ext_nom_source_  = equation().probleme().le_nom();
  ext_nom_source_ +=  "_" ;
  ext_nom_source_ +=  bord_periodique_ ;

  // Pourquoi ?
  if (est_different(dernier_temps_calc_,tps))
    {
      // Compte flow rate:
      double debit_e= 0.;
      calculer_debit(debit_e);

      if (je_suis_maitre())
        {
          if (sub_type(Convection_Diffusion_std,equation()))
            {
              // Write nothing if equation is Energy
            }
          else
            write_flow_rate(ext_nom_source_, debit_e);
        }
      if (sub_type(Convection_Diffusion_std,equation()))
        {
          if (equation().probleme().is_dilatable())
            {
              Cerr << "Source term not validated yet for Quasi Compressible formulation." << finl;
              Cerr << "Contact TRUST support." << finl;
              exit();
            }
          if(equation().schema_temps().pas_de_temps_locaux().size()>0)
            {
              Cerr << "Source term not validated yet for steady option for the Convection_Diffusion_std equation." << finl;
              Cerr << "Contact TRUST support." << finl;
              exit();
            }
          return source_convection_diffusion(debit_e);
        }
      else if (sub_type(Navier_Stokes_std,equation()))
        {
          // Compute source term for Navier Stokes

          // Master process sends values to all processes:
          envoyer_broadcast(debit_ref_, 0);
          envoyer_broadcast(debnm1_, 0);
          envoyer_broadcast(source_, 0);

          // On conserve le test (avec dt_min) car appel a t=0 (dt=0) au cours de preparer calculer pour loi de paroi TBLE
          double dt_min = equation().schema_temps().pas_temps_min();
          const DoubleVect& dt_locaux = equation().schema_temps().pas_de_temps_locaux();
          double si = 0;
          if (sup_ou_egal(dt,dt_min))
            {
              // si = [ 2*(Q(0)-Q(t(n))) - (Q(0)-Q(t(n-1))) ] / [ coeff * dt * aire(bord) ]
              if( dt_locaux.size()>0)
                {
                  Cerr << "Periodic BC plus source term not validated yet for steady option." << finl;
                  Cerr << "Contact TRUST support." << finl;
                  exit();

                  //du au fait que ce calcul implique une division par dt, dans le cas ou le dt est variable par face,
                  // calculer_debit(debit_e) renvoye [ 2*(Q(0)-Q(t(n))) - (Q(0)-Q(t(n-1))) ]/dt_locaux
                  si = debit_e/(coeff*surface_bord_);
                }
              else
                {
                  si = (2.*(debit_ref_-debit_e)-(debit_ref_-debnm1_))/(coeff*dt*surface_bord_);
                  debnm1_ = debit_e;
                }
            }

          source_ += si;
          dernier_temps_calc_ = tps;

          if (je_suis_maitre())
            {
              // Write the pressure gradient file if equation is Navier Stokes
              Nom filename(nom_du_cas());
              filename+="_Pressure_Gradient_";
              filename+= ext_nom_source_ ;
              if (!pressure_gradient_file_.is_open())
                {
                  pressure_gradient_file_.ouvrir(filename, (premiere_ecriture?ios::out:ios::app));
                  pressure_gradient_file_.setf(ios::scientific);
                }
              if ((tps <= (tps_init+dt)) && premiere_ecriture)
                {
                  if (equation().probleme().is_dilatable()==1)
                    pressure_gradient_file_ << "# Time t     gradP(t)      gradP(t)-gradP(t-dt) in [kg.s-2.m-2] if SI units used" << finl;
                  else
                    pressure_gradient_file_ << "# Time t     gradP(t)      gradP(t)-gradP(t-dt) in [m.s-2] if SI units used" << finl;
                }
              pressure_gradient_file_ << tps+dt << " " << source_ << "  " << si << finl;

              // Write the restart file:
              filename=nom_du_cas();
              filename+="_Channel_Flow_Rate_repr_";
              filename+=ext_nom_source_;
              if (!restart_file_.is_open())
                {
                  restart_file_.ouvrir(filename, (premiere_ecriture?ios::out:ios::app));
                  restart_file_.setf(ios::scientific);
                }
              if ((tps <= (tps_init+dt)) && premiere_ecriture)
                {
                  if (equation().probleme().is_dilatable()==1)
                    restart_file_ << "# Time t       Flow rate Q(t)    Flow rate Q(0) in [kg.s-1]   gradP(t) in [kg.s-2.m-2] if SI units used" << finl;
                  else
                    restart_file_ << "# Time t       Flow rate Q(t)    Flow rate Q(0) in [m3.s-1]   gradP(t) in [m.s-2] if SI units used" << finl;
                }
              restart_file_ << tps+dt << "   " <<  debit_e << "      " << debit_ref_ << "                 " << source_ << finl;
            }
        }
      else
        {
          Cerr << "You can't use canal_perio on the equation " << equation().que_suis_je() << finl;
          exit();
        }
    }
  if (sub_type(Convection_Diffusion_std,equation()))
    {
      Cerr << "Error! Contact TRUST support." << finl;
      exit();
    }

  // Essayer de virer direction_ecoulement_ (attention assert en VDF et ecarts possibles)
  ArrOfDouble s;
  s.resize(dimension);
  s = 0.;
  if (direction_ecoulement_>=0) // Ecoulement selon un axe
    s[direction_ecoulement_] = source_;
  else // Cas general
    for (int i=0; i<dimension; i++)
      s[i] = source_ * dir_source_[i];
  return s;
}


DoubleTab& Terme_Source_Canal_perio::calculer(DoubleTab& resu) const
{
  resu = 0;
  return ajouter(resu);
}
