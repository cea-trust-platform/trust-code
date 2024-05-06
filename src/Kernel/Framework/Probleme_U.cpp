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

#include <Probleme_U.h>
#include <stat_counters.h>
#include <ICoCoExceptions.h>
#include <Ch_front_input_uniforme.h>
#include <Ch_input_uniforme.h>
#include <Champ_input_P0.h>

#ifdef VTRACE
#include <vt_user.h>
#endif


#include <Champ_Generique_base.h>
#include <Convert_ICoCoTrioField.h>


Implemente_base(Probleme_U,"Probleme_U",Objet_U);

using ICoCo::WrongArgument;
using ICoCo::TrioField;

/*! @brief A surcharger.
 */
Sortie& Probleme_U::printOn(Sortie& os) const
{
  return os;
}

/*! @brief A surcharger.
 */
Entree& Probleme_U::readOn(Entree& is)
{
  return is ;
}

/*! @brief This method is called once at the beginning, before any other one of the interface Problem.
 *
 * @throws WrongContext
 */
void Probleme_U::initialize()
{
}

/*! @brief This method is called once at the end, after any other one.
 *
 * It frees the memory and saves anything that needs to be saved.
 *
 * @throws WrongContext
 */
void Probleme_U::terminate()
{
}

/*! @brief Returns the present time.
 *
 * This value may change only at the call of validateTimeStep.
 *  A surcharger
 *
 * @return (double) present time
 * @throws WrongContext
 */
double Probleme_U::presentTime() const
{
  return 0;
}

/*! @brief Compute the value the Problem would like for the next time step.
 *
 * This value will not necessarily be used at the call of initTimeStep,
 *  but it is a hint.
 *  This method may use all the internal state of the Problem.
 *
 * @param (stop) Does the Problem want to stop ?
 * @return (double) The desired time step
 * @throws WrongContext
 */
double Probleme_U::computeTimeStep(bool& stop) const
{
  stop=true;
  return 0;
}

/*! @brief This method allocates and initializes the unknown and given fields for the future time step.
 *
 *  The value of the interval is imposed through the parameter dt.
 *  In case of error, returns false.
 *
 * @param (double dt) the time interval to allocate
 * @return (bool) true=OK, false=error, not able to tackle this dt
 * @throws WrongContext, WrongArgument
 */
bool Probleme_U::initTimeStep(double dt)
{
  return false;
}

/*! @brief Validates the calculated unknown by moving the present time at the end of the time step.
 *
 *  This method is allowed to free past values of the unknown and given
 *  fields.
 *
 * @throws WrongContext
 */
void Probleme_U::validateTimeStep()
{
}

/*! @brief Tells if the Problem unknowns have changed during the last time step.
 *
 * @return (bool) true=stationary, false=not stationary
 * @throws WrongContext
 */
bool Probleme_U::isStationary() const
{
  return false;
}

/*! @brief Tells to the Problem that stationary is reached or not
 *
 */
void Probleme_U::setStationary(bool)
{
}

/*! @brief Aborts the resolution of the current time step.
 *
 * @throws WrongContext
 */
void Probleme_U::abortTimeStep()
{
}

/*! @brief Reset the current time of the Problem to a given value.
 *
 * Particularly useful for the initialization of complex transients: the starting point of the transient
 * of interest is computed first, the time is reset to 0, and then the actual transient of interest starts with proper
 * initial conditions, and global time 0.
 *
 * @param[in] time the new current time.
 * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
 * @throws ICoCo::WrongContext exception if called inside the TIME_STEP_DEFINED context (see Problem documentation)
 */
void Probleme_U::resetTime(double time)
{
}

/*! @brief In the case solveTimeStep uses an iterative process, this method executes a single iteration.
 *
 *  It is thus possible to modify the given fields between iterations.
 *  converged is set to true if the process has converged, ie if the
 *  unknown fields are solution to the problem on the next time step.
 *  Otherwise converged is set to false.
 *  The return value indicates if the convergence process behaves normally.
 *  If false, the Problem wishes to abort the time step resolution.
 *
 * @param (bool& converged) It is a return value : true if the process has converged, false otherwise.
 * @return (bool) true=OK, false=unable to converge
 * @throws WrongContext
 */
bool Probleme_U::iterateTimeStep(bool& converged)
{
  return false;
}

/*! @brief Demande au probleme de postraiter ses champs, sondes,.
 *
 * .. Dans Probleme_U::run(), postraiter() est appele a chaque pas de
 *  temps avec force=0 et au debut et a la fin du calcul avec force=1.
 *  WEC : il serait bon que, un jour, postraiter soit const
 *
 * @param (force) 1=postraiter absolument, 0=postraiter si c'est necessaire.
 * @return (0 en cas d'erreur, 1 sinon.)
 */
int Probleme_U::postraiter(int force)
{
  return 0;
}

/*! @brief Doit-on imprimer les statistiques d'execution maintenant ?
 *
 */
int Probleme_U::limpr() const
{
  return 0;
}

/*! @brief Doit-on sauvegarder l'etat du probleme sur disque maintenant ?
 *
 */
int Probleme_U::lsauv() const
{
  return 0;
}

/*! @brief Sauvegarder l'etat du probleme sur disque
 *
 */
void Probleme_U::sauver() const
{
  return ;
}


/*! @brief ATTENTION :
 *
 *  Rentre ici tout ce qui ne correspond pas a l'API normale de Problem.
 *
 *  Actuellement on y met a jour les CLs et les termes sources, sachant
 *  que certains vont chercher eux-memes des informations dans les
 *  problemes voisins...
 *
 *  Programme de travail : tout ce qui est dans cette methode doit etre
 *  rendu independant de l'exterieur, et peut du coup rejoindre
 *  initTimeStep.
 *  Le reste passe dans l'interface d'echange de champs.
 *  Ce travail sera fini quand updateGivenFields sera vide et supprime !
 *
 *
 * @return (true=OK, false=error)
 */
bool  Probleme_U::updateGivenFields()
{
  return true;
}


/*! @brief Cette methode est une sorte de main() du Problem Elle peut etre utilisee si le probleme n'est couple a aucun autre.
 *
 *  (s'il n'a besoin d'aucun champ d'entree).
 *
 * @return (bool) true=OK, false=error
 */
bool Probleme_U::run()
{
  // Force the post process task at the beginning of the run
  Cerr<<"First postprocessing, this can take some minutes"<<finl;
  postraiter(1);
  Cerr<<"First postprocessing OK"<<finl;

  bool stop=false; // Does the Problem want to stop ?
  bool ok=true; // Is the time interval successfully solved ?

  // Compute the first time step
  double dt=computeTimeStep(stop);

  statistiques().end_count(initialisation_calcul_counter_);
  // Print the initialization CPU statistics
  if (!disable_TU)
    {
      if(GET_COMM_DETAILS)
        statistiques().print_communciation_tracking_details("Statistiques d'initialisation du calcul", 0);               // Into _csv.TU file

      statistiques().dump("Statistiques d'initialisation du calcul", 0);
      print_statistics_analyse("Statistiques d'initialisation du calcul", 0);
    }
  statistiques().reset_counters();
  statistiques().begin_count(temps_total_execution_counter_);

#ifdef VTRACE
  //VT_USER_END("Initialization");
  VT_BUFFER_FLUSH();
  VT_ON();
  VT_USER_START("Resolution");
#endif
  // Time step loop
  int tstep = 0;
  while(!stop)
    {
      // Begin the CPU measure of the time step
      statistiques().begin_count(timestep_counter_);

      ok=false; // Is the time interval successfully solved ?

      // Loop on the time interval tries
      while (!ok && !stop)
        {

          // Prepare the next time step
          if (!initTimeStep(dt))
            return false;

          // Backup unknown fields if necessary
          if (lsauv())
            sauver();

          // Solve the next time step
          ok=solveTimeStep();

          if (!ok)   // The resolution failed, try with a new time interval.
            {
              abortTimeStep();
              dt=computeTimeStep(stop);
            }
          else // The resolution was successful, validate and go to the next time step.
            validateTimeStep();
        }
      if (!ok) // Impossible to solve the next time step, the Problem has given up
        break;

      // Compute the next time step length
      dt=computeTimeStep(stop);

      // Stop the resolution if the Problem is stationnary
      if (isStationary())
        {
          stop=true;
          setStationary(stop);
        }
      // Post process task (Force the post processing/prints at the end of the run (stop=1))
      postraiter(stop);

      // Stop the CPU measure of the time step and print:
      statistiques().end_count(timestep_counter_);
      if (limpr())
        {
          double temps = statistiques().last_time(timestep_counter_);
          Cout << finl << "clock: Total time step: " << temps << " s" << finl << finl;
        }

      if(JUMP_3_FIRST_STEPS && tstep < 3)
        {
          //demarrage des compteurs CPU
          if(tstep == 2)
            {
              statistiques().set_three_first_steps_elapsed(true);
            }
        }
      else
        {
          statistiques().compute_avg_min_max_var_per_step(tstep);
        }

      tstep++;
#ifdef VTRACE
      // Flush the buffer regulary to avoid setting VT_MAX_FLUSHES=0 variable...
      VT_BUFFER_FLUSH();
#endif
    }
#ifdef VTRACE
  VT_USER_END("Resolution");
  VT_OFF();
#endif

  // Print the resolution CPU statistics
  if (!disable_TU)
    {
      if(GET_COMM_DETAILS)
        statistiques().print_communciation_tracking_details("Statistiques de resolution du probleme", 1);               // Into _csv.TU file

      statistiques().dump("Statistiques de resolution du probleme", 1);      // Into _csv.TU file
      print_statistics_analyse("Statistiques de resolution du probleme", 1); // Into        .TU file
    }

  // Reset the CPU counters
  statistiques().reset_counters();
  statistiques().begin_count(temps_total_execution_counter_);

  return ok;
}

/*! @brief This method has the same role as the method run, but it stops when reaching the time given in parameter.
 *
 *  If this time cannot be reached, the method returns false.
 *
 * @param (double time) time to reach
 * @return (true=OK, false=error)
 */
bool Probleme_U::runUntil(double time)
{

  // Error if time is already past
  if (time<presentTime())
    return false;

  bool stop=false; // Does the Problem want to stop ?
  bool ok=true; // Is the time interval successfully solved ?

  // Compute the first time step length
  double dt=computeTimeStep(stop);

  // Boucle sur les pas de temps
  while(!stop)
    {

      statistiques().begin_count(timestep_counter_);

      ok=false;

      // Loop on the time interval tries
      while (!ok && !stop)
        {

          // Do not go past time
          if (presentTime()+dt>time)
            dt=time-presentTime();

          // Prepare the next time step
          if (!initTimeStep(dt))
            return false;

          // Solve the next time step
          ok=solveTimeStep();

          if (!ok)   // The resolution failed, try with a new time interval.
            {
              abortTimeStep();
              dt=computeTimeStep(stop);
            }

          else // The resolution was successful, validate and go to the
            // next time step.
            validateTimeStep();
        }
      if (!ok) // Impossible to solve the next time step, the Problem has given up
        break;

      // time was reached
      if (presentTime()==time)
        return true;

      // Compute the next time step length
      dt=computeTimeStep(stop);

      statistiques().end_count(timestep_counter_);
      if (je_suis_maitre() && limpr())
        {
          double temps = statistiques().last_time(timestep_counter_);
          Cout << "clock: Total time step: " << temps << " s" << finl;
        }

      postraiter(0);
    }

  return ok;
}

/*! @brief pour recodage eventuel et appel unifie en python
 *
 */
bool Probleme_U::solveTimeStep(void)
{
  bool converged = false;
  bool ok        = true;

  while (!converged && ok)
    {
      ok= ok && updateGivenFields();
      ok = ok && iterateTimeStep(converged);
    }

  return ok;
}

/*! @brief Returns the future time (end of current computing interval) This value is valid between initTimeStep and either
 *
 *  validateTimeStep or abortTimeStep.
 *  A surcharger
 *
 * @return (double) future time
 * @throws WrongContext
 */
double Probleme_U::futureTime() const
{
  return 0;
}

/*! @brief This method is used to find the names of input fields understood by the Problem
 *
 * @param (Noms) list of names where the Problem appends its input field names.
 */
void Probleme_U::getInputFieldsNames(Noms& noms) const
{
}

/*! @brief This method is used to find in a hierarchy of problems the Champ_Input_Proto of a given name.
 *
 *  Implementation is not optimal in 2 ways :
 *   - no error if two input fields have the same name.
 *   - no optimisation of the number of REF objects created and destroyed.
 *
 * @param (string name) name of the input field we are looking for
 * @return (REF(Field_base)) found <=> non_nul(), then points to a Champ_Input_Proto of that name.
 */
REF(Field_base) Probleme_U::findInputField(const Nom& name) const
{
  REF(Field_base) ch;
  return ch;
}




/*! @brief This method is used to find the names of output fields understood by the Problem
 *
 * @param (Noms) list of names where the Problem appends its output field names.
 */
void Probleme_U::getOutputFieldsNames(Noms& noms) const
{
}

REF(Champ_Generique_base) Probleme_U::findOutputField(const Nom& name) const
{
  REF(Champ_Generique_base) ch;
  return ch;
}



/*! @brief This method is used to get a template of a field expected for the given name.
 *
 */
void Probleme_U::getInputFieldTemplate(const Nom& name, TrioField& afield) const
{
  REF(Field_base) ch=findInputField(name);
  if (!ch.non_nul())
    throw WrongArgument(le_nom().getChar(),"getInputFieldTemplate",name.getString(),"no input field of that name");

  // Du au fait qu'on ne peut pas faire une ref sur Champ_Input_Proto qui n'est pas un Objet_U...
  Champ_Input_Proto * chip = dynamic_cast<Champ_Input_Proto *>(ch.operator->());
  if (!chip)
    throw WrongArgument(le_nom().getChar(),"getInputFieldTemplate",name.getString(),"field of this name is not an input field");

  chip->getTemplate(afield);
}

/*! @brief This method is used to provide the Problem with an input field.
 *
 */
void Probleme_U::setInputField(const Nom& name, const TrioField& afield)
{
  REF(Field_base) ch=findInputField(name);
  if (!ch.non_nul())
    throw WrongArgument(le_nom().getChar(),"setInputField",name.getString(),"no input field of that name");
  if (!est_egal(afield._time1,presentTime()))
    throw WrongArgument(le_nom().getChar(),"setInputField","afield","Should be defined on current time interval");
  if (!est_egal(afield._time2,futureTime()))
    throw WrongArgument(le_nom().getChar(),"setInputField","afield","Should be defined on current time interval");
  if (strcmp(name.getChar(),afield.getCharName()))
    throw WrongArgument(le_nom().getChar(),"setInputField","afield","Should have the same name as the argument name ");

  // Du au fait qu'on ne peut pas faire une ref sur Champ_Input_Proto qui n'est pas un Objet_U...
  Champ_Input_Proto * chip = dynamic_cast<Champ_Input_Proto *>(ch.operator->());
  if (!chip)
    throw WrongArgument(le_nom().getChar(),"setInputField",name.getString(),"field of this name is not an input field");

  chip->setValue(afield);
}

void Probleme_U::getOutputField(const Nom& name,  TrioField& afield) const
{

  REF(Champ_Generique_base) ref_ch=findOutputField(name);
  if (!ref_ch.non_nul())
    throw WrongArgument(le_nom().getChar(),"getOutputField",name.getString(),"no output field of that name");

  const Champ_Generique_base& ch = ref_ch.valeur();
  build_triofield(ch, afield);
  afield.setName(name.getString());
}

// For now: set a field value provided the field has only one item.
void Probleme_U::setInputDoubleValue(const Nom& name, const double val)
{
  REF(Field_base) ch = findInputField(name);
  if (!ch.non_nul())
    throw WrongArgument(le_nom().getChar(),"setInputDoubleValue",name.getString(),"no input field of that name");
  if (ch->nb_comp() != 1)
    throw WrongArgument(le_nom().getChar(),"getOutputDoubleValue",name.getString(),"invalid field size!!");

  // Wa can not do a REF on Champ_Input_Proto which is not an Objet_U...
  Champ_Input_Proto * chip = dynamic_cast<Champ_Input_Proto *>(ch.operator ->());
  if (!chip)
    throw WrongArgument(le_nom().getChar(),"setInputField",name.getString(),"field of this name is not an input field");
  chip->setDoubleValue(val);
}

void Probleme_U::setInputIntValue(const Nom& name, const int& val)
{
  // add value in ICoCoScalarRegister
  reg.setInputIntValue(name, val);
}

int Probleme_U::getOutputIntValue(const Nom& name) const
{
  return reg.getOutputIntValue(name);
}
