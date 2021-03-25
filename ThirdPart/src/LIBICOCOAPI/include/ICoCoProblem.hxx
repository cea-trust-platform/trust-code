// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found in the TRUST repository:
//
//    https://sourceforge.net/projects/trust/

#ifndef ICoCoProblem_included
#define ICoCoProblem_included

#ifdef ICOCO_VERSION
#error "ICOCO_VERSION already defined!! Are you including twice two versions of the ICoCo section ??"
#else
#define ICOCO_VERSION "2.0"
#endif

#include <string>
#include <vector>

/*! @brief The namespace ICoCo (Interface for code coupling) encompasses all the classes
 * and methods needed for the coupling of codes.
 * See the class Problem and the function getProblem() to start with.
 */
namespace ICoCo
{
class TrioField;
class MEDDoubleField;
class MEDIntField;
class MEDStringField;

/*! @brief The various possible types for fields or scalar values.
 */
enum class ValueType
{
  Double,  ///< Double scalar value or field type
  Int,     ///< Int scalar value or field type
  String   ///< String scalar value or field type
};

/*! @brief API that a code has to implement in order to comply with the ICoCo (version 2) norm.
 *
 * This abstract class represents the methods that a given code may implement to comply (partially or fully) to the
 * ICoCo standard. For organisation and documentation purposes the interface is separated into several sections but this
 * does not correspond to any code constraint.
 * Note that not all the methods need to be implemented! Notably the methods belonging to the sections
 *   - Restorable
 *   - Field I/O
 *   - Scalar values I/O
 *
 * are not always needed since a code might not have any integer field to work with for example.
 * Consequently, default implementation for all methods of this interface is to raise a ICoCo::NotImplemented exception.
 *
 * Finally, the interface may be wrapped in Python using SWIG or PyBind11. For an example of the former see the
 * TRUST implementation of ICoCo.
 */
class Problem
{
 public:
  // ******************************************************
  // section Problem
  // ******************************************************

  /*! @brief Constructor.
   *
   * Internal set up and initialisation of the code should not be done here, but rather in initialize().
   */
  Problem();

  /*! @brief Destructor.
   *
   * The release of the resources allocated by the code should not be performed here, but rather in terminate().
   */
  virtual ~Problem();

  int getICoCoMajorVersion() const
  {
    return 2;
  }

  /*! @brief Provides the relative path of a data file to be used by the code.
   *
   * This method must be called before initialize()
   *
   * @param[in] datafile relative path to the data file.
   * @throws ICoCo::WrongContext exception if called multiple times or after initialize().
   * @throws ICoCo::WrongArgument if an invalid path is provided.
   */
  virtual void setDataFile(const std::string& datafile);

  /*! @brief Provides the MPI communicator to be used by the code for parallel computations.
   *
   * This method must be called before initialize(). The communicator should include all the processes
   * to be used by the code. For a sequential run, the call to setMPIComm is optional or mpicomm should be
   * nullptr.
   *
   * @param[in] mpicomm pointer to a MPI communicator. Type void* to avoid to include mpi.h for sequential codes.
   * @throws ICoCo::WrongContext exception if called multiple times or after initialize.
   */
  virtual void setMPIComm(void* mpicomm);

  /*! @brief (Mandatory) Initialize the current problem instance.
   *
   * In this method the code should allocate all its internal structures and be ready to execute. File reads, memory
   * allocations, and other operations likely to fail should be performed here and not in the previous methods. This
   * method must be called only once (after a potential call to setMPIComm() and/or setDataFile()) and cannot be called
   * again before terminate() has been performed.
   *
   * @return true if all OK, otherwise false.
   * @throws ICoCo::WrongContext exception if called multiple times.
   */
  virtual bool initialize();

  /*! @brief (Mandatory) Terminate the current problem instance and release all allocated resources.
   *
   * Terminate the computation, free the memory and save whatever needs to be saved. This method is called once
   * at the end of the computation or after a non-recoverable error.
   * No other ICoCo method except setDataFile(), setMPIComm() and initialize() may be called after this.
   *
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @throws ICoCo::WrongContext exception if called after initTimeStep() without a subsequent call to abortTimeStep()
   * or validateTimeStep().
   */
  virtual void terminate();

  // ******************************************************
  // section UnsteadyProblem
  // ******************************************************

  /*! @brief Returns the current time of the simulation.
   *
   * Can be called any time between initialize() and terminate().
   * The current time can only change during a call to validateTimeStep().
   *
   * @return the current time of the simulation
   * @throws ICoCo::WrongContext exception if called before initialize().
   */
  virtual double presentTime() const;

  /*! @brief Return the next preferred time step for this code, and whether the code wants to stop.
   *
   * Both data are only indicative, the supervisor is not required to take them into account.
   * This method can be called whenever the Problem has been initialized but the computation time step is not defined.
   *
   * @param[out] stop set to true if the code wants to stop.
   * @return the preferred time step for this code (only valid if stop is false).
   * @throws ICoCo::WrongContext exception if called before initialize().
   */
  virtual double computeTimeStep(bool& stop) const;

  /*! @brief (Mandatory) Provides the next time step (time increment) to be used by the code.
   *
   * After this call (if successful), the computation time step is defined to ]t, t + dt] where t is the value
   * returned by presentTime(). Can be called whenever the computation time step is not defined.
   *
   * @param[in] dt the time step to be used by the code
   * @return false means that given time step is not compatible with the code time scheme.
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @throws ICoCo::WrongContext exception if called several times without resolution.
   * @throws ICoCo::WrongArgument exception if dt is invalid (dt <= 0.0).
   */
  virtual bool initTimeStep(double dt);

  /*! @brief (Mandatory) Perform the computation on the current time interval.
   *
   * Can be called whenever the computation time step is defined.
   *
   * @return true if computation was successful, false otherwise.
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual bool solveTimeStep();

  /*! @brief (Mandatory) Validate the computation performed by solveTimeStep.
   *
   * Can be called whenever the computation time step is defined. After this call:
   * - the present time has been advanced to the end of the computation time-step
   * - the computation time step is undefined, so the input and output fields are not accessible any more.
   *
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual void validateTimeStep();

  /*! @brief (Mandatory) Indicates whether the code should compute a stationary solution or a transient one.
   *
   * New in version 2 of ICoCo. By default the code is assumed to be in stationary mode False (i.e. set up
   * for a transient computation).
   *
   * @param[in] stationary true if the code should compute a stationary solution.
   * @throws ICoCo::WrongContext exception if called before initialize().
   */
  virtual void setStationaryMode(bool stationaryMode);

  /*! @brief Return whether the solution is constant on the computation time step.
   *
   * Can be called whenever the computation time step is defined.
   *
   * @return true if the solution is constant on the computation time step.
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @throws ICoCo::WrongContext exception if called between initTimeStep() and validateTimeStep() (or abortTimeStep()),
   * meaning we shouldn't request this information while the computation of a new time-step is in progress.
   */
  virtual bool isStationary() const;

  /*! @brief (Mandatory) Abort the computation on the current time-step.
   *
   * Can be called whenever the computation time-step is defined, instead of validateTimeStep().
   * After this call, the present time is left unchanged, and the computation time-step is undefined.
   *
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual void abortTimeStep();

  /*! @brief Reset the current time of the Problem to a given value.
   *
   * New in version 2 of ICoCo.
   * Can be called before the first initTimeStep() or after validateTimeStep() or abortTimeStep().
   * Particularly useful for the initialisation of complex transients: the starting point of the transient
   * of interest is computed first, the time is reset to 0, and then the actual transient of intereset starts with
   * proper initial conditions, and global time 0.
   *
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @throws ICoCo::WrongContext exception if called between initTimeStep() and validateTimeStep() (or abortTimeStep()).
   */
  virtual void resetTime(double time);

  // ******************************************************
  // section IterativeUnsteadyProblem
  // ******************************************************

  /*! @brief  Perform a single iteration of computation inside the time step.
   *
   * This method is relevant for codes having inner iterations for the computation of a single time step.
   * Calling iterateTimeStep() until converged is true is equivalent to calling solveTimeStep(), within the code's
   * convergence threshold.
   * Can be called whenever the computation time-step is defined.
   *
   * @param[out] converged set to true if the solution is not evolving any more.
   * @return false if the computation fails;
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual bool iterateTimeStep(bool& converged);

  // ******************************************************
  // section Restorable
  // ******************************************************

  /*! @brief Save the state of the code.
   *
   * The saved state is identified by the combination of label and method arguments.
   * If save() has already been called with the same two arguments, the saved state is overwritten.
   *
   * @param[in] label a user- (or code-) defined value identifying the state.
   * @param[in] method a string specifying which method is used to save the state of the code. A code can provide
   * different methods (for example in memory, on disk, etc.).
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @throws ICoCo::WrongContext exception if called after a call to initTimeStep() and before validateTimeStep() (or
   * abortTimeStep()), meaning we shouldn't save a previous time-step while the computation of a new time-step is in
   * progress.
   * @throws ICoCo::WrongArgument exception if the method or label argument is invalid.
   */
  virtual void save(int label, const std::string& method) const;

  /*! @brief Restore the state of the code.
   *
   * After restore, the code should behave exactly like after the corresponding call to save (except of course for
   * save/restore methods, since the list of saved states may have changed). The state to be restored is identified by
   * the combination of label and method arguments. The save() method must have been called at some point or in some
   * previous run with this combination.
   *
   * @param[in] label a user- (or code-) defined value identifying the state.
   * @param[in] method a string specifying which method is used to restore the state of the code. A code can provide
   * different methods (for example in memory, on disk, etc.).
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @throws ICoCo::WrongContext exception if called after a call to initTimeStep() and before validateTimeStep() (or
   * abortTimeStep()), meaning we shouldn't restore while the computation of a new time-step is in progress.
   * @throws ICoCo::WrongArgument exception if the method or label argument is invalid.
   */
  virtual void restore(int label, const std::string& method);

  /*! @brief Discard a previously saved state of the code.
   *
   * After this call, the save-point cannot be restored anymore. This method can be used to free the space occupied by
   * unused saved states.
   *
   * @param[in] label a user- (or code-) defined value identifying the state.
   * @param[in] method a string specifying which method is used to restore the state of the code. A code can provide
   * different methods (for example in memory, on disk, etc.).
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @throws ICoCo::WrongArgument exception if the method or label argument is invalid.
   */
  virtual void forget(int label, const std::string& method) const;

  // ******************************************************
  // section Field I/O. Reminder: all methods are **optional** not all of them need to be implemented!
  // ******************************************************

  /*! @brief Get the list of input fields accepted by the code.
   *
   * @return the list of field names that represent inputs of the code
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @sa getInputValuesNames()
   */
  virtual std::vector<std::string> getInputFieldsNames() const;

  /*! @brief Get the list of output fields that can be provided by the code.
   *
   * @return the list of field names that can be produced by the code
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @sa getOutputValuesNames()
   */
  virtual std::vector<std::string> getOutputFieldsNames() const;

  /*! @brief Get the type of a field managed by the code (input or output)
   *
   * The three possible types are int, double and string, as defined in the ValueType enum.
   *
   * @param[in] name field name
   * @return one of ValueType::Double, ValueType::Int or ValueType::String
   * @throws ICoCo::WrongArgument exception if the field name is invalid.
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @sa ValueType
   * @sa getValueType()
   */
  virtual ValueType getFieldType(const std::string& name) const;

  virtual std::string getMeshUnit() const;

  virtual std::string getFieldUnit(const std::string& fieldName) const;

  // ******************************************************
  //     subsection TrioField fields I/O
  // ******************************************************

  /*! @brief Retrieves an empty shell for an input field. This shell can be filled by the caller and then be
   * given to the code via setInputField(). The field has the TrioField format.
   *
   * The code uses this method to populate 'afield' with all the data that represents the context
   * of the field (i.e. its support mesh, its discretization -- on nodes, on elements, ...).
   * The remaining job for the caller of this method is to fill the actual values of the field itself.
   * When this is done the field can be sent back to the code through the method setInputField().
   * This method is not mandatory but is useful to know the mesh, discretization... on which an input field is
   * expected.
   *
   * @param[in] name name of the field for which we would like the empty shell
   * @param[out] afield field object (in TrioField format) that will be populated with all the contextual information.
   * Any previous information in this object will be discarded.
   * @throws ICoCo::WrongArgument exception if the field name is invalid.
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual void getInputFieldTemplate(const std::string& name, TrioField& afield) const;

  /*! @brief Provides the code with input data in the form of a TrioField.
   *
   * The method getInputFieldTemplate(), if implemented, may be used first to prepare an empty shell of the field to
   * pass to the code.
   *
   * @param[in] name name of the field that is given to the code.
   * @param[in] afield field object (in TrioField format) containing the input data to be read by the code. The name
   * of the field set on this instance (with the Field::setName() method) should not be checked. However its time value
   * should be.
   *
   * Any previous information in this object will be discarded.
   *
   * @throws ICoCo::WrongArgument exception if the field name ('name' parameter) is invalid.
   * @throws ICoCo::WrongArgument exception if the time property of 'afield' does not belong to the currently computed
   * time-step ]t, t + dt]
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual void setInputField(const std::string& name, const TrioField& afield);

  /*! @brief Retrieves output data from the code in the form of a TrioField.
   *
   * Gets the output field corresponding to name from the code into the afield argument.
   *
   * @param[in] name name of the field that the caller requests from the code.
   * @param[out] afield field object (in TrioField format) populated with the data read by the code. The name
   * and time properties of the field should be set in accordance with the 'name' parameter and with the current
   * time step being computed.
   * Any previous information in this object will be discarded.
   * @throws ICoCo::WrongArgument exception if the field name ('name' parameter) is invalid.
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual void getOutputField(const std::string& name, TrioField& afield) const;

  /*! @brief Update a previously retrieved output field.
   *
   * New in version 2. This methods allows the code to implement a more efficient update of a given output field,
   * thus avoiding the caller to invoke getOutputField() each time.
   * A previous call to getOutputField with the same name must have been done prior to this call.
   * The code should check the consistency of the field object with the requested data.
   *
   * @param[in] name name of the field that the caller requests from the code.
   * @param[out] afield field object (in TrioField format) updated with the data read from the code. Notably the time
   * indicated in the field should be updated to the current time step being computed.
   * @throws ICoCo::WrongArgument exception if the field name ('name' parameter) is invalid.
   * @throws ICoCo::WrongArgument exception if the field object is inconsistent with the field being requested.
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual void updateOutputField(const std::string& name, TrioField& afield) const;

  // ******************************************************
  //     subsection MED fields I/O: double, int and string.
  // ******************************************************

  /*! @brief Retrieves an empty shell of the field expected by the code to receive input data, using the MEDDoubleField
   *  field format.
   *
   * See getInputFieldTemplate().
   * @sa getInputFieldTemplate().
   */
  virtual void getInputMEDDoubleFieldTemplate(const std::string& name, MEDDoubleField& afield) const;

  /*! @brief
   *
   * @throws ICoCo::WrongContext exception if called before initialize().
   */
  virtual void setInputMEDDoubleField(const std::string& name, const MEDDoubleField& afield);

  /*! @brief
   *
   * @throws ICoCo::WrongContext exception if called before initialize().
   */
  virtual void getOutputMEDDoubleField(const std::string& name, MEDDoubleField& afield) const;

  /*! @brief
   *
   * @throws ICoCo::WrongContext exception if called before initialize().
   */
  virtual void updateOutputMEDDoubleField(const std::string& name, MEDDoubleField& afield) const;

  /*! @brief Similar to getInputMEDDoubleFieldTemplate() but for MEDIntField.
   * @sa getInputMEDDoubleFieldTemplate()
   */
  virtual void getInputMEDIntFieldTemplate(const std::string& name, MEDIntField& afield) const;

  /*! @brief Similar to setInputMEDDoubleField() but for MEDIntField.
   * @sa setInputMEDDoubleField()
   */
  virtual void setInputMEDIntField(const std::string& name, const MEDIntField& afield);

  /*! @brief Similar to getOutputMEDDoubleField() but for MEDIntField.
   * @sa getOutputMEDDoubleField()
   */
  virtual void getOutputMEDIntField(const std::string& name, MEDIntField& afield) const;

  /*! @brief Similar to updateOutputMEDDoubleField() but for MEDIntField.
   * @sa updateOutputMEDDoubleField()
   */
  virtual void updateOutputMEDIntField(const std::string& name, MEDIntField& afield) const;

  /*! @brief Similar to getInputMEDDoubleFieldTemplate() but for MEDStringField.
   *
   * \b WARNING:  at the time of writing, MEDStringField are not yet implemented anywhere.
   * @sa getInputMEDDoubleFieldTemplate()
   */
  virtual void getInputMEDStringFieldTemplate(const std::string& name, MEDStringField& afield) const;

  /*! @brief Similar to setInputMEDDoubleField() but for MEDStringField.
   *
   * \b WARNING:  at the time of writing, MEDStringField are not yet implemented anywhere.
   * @sa setInputMEDDoubleField()
   */
  virtual void setInputMEDStringField(const std::string& name, const MEDStringField& afield);

  /*! @brief Similar to getOutputMEDDoubleField() but for MEDStringField.
   *
   * \b WARNING:  at the time of writing, MEDStringField are not yet implemented anywhere.
   * @sa getOutputMEDDoubleField()
   */
  virtual void getOutputMEDStringField(const std::string& name, MEDStringField& afield) const;

  /*! @brief Similar to updateOutputMEDDoubleField() but for MEDStringField.
   *
   * \b WARNING:  at the time of writing, MEDStringField are not yet implemented anywhere.
   * @sa updateOutputMEDDoubleField()
   */
  virtual void updateOutputMEDStringField(const std::string& name, MEDStringField& afield) const;

  /*! @brief Get MEDCoupling major version, if the code was built with MEDCoupling support
   *
   * This can be used to assess compatibility between codes when coupling them.
   *
   * @return the MEDCoupling major version number (typically 7, 8, 9, ...)
   */
  virtual int getMEDCouplingMajorVersion() const;

  /*! @brief Indicates whether the code was built with a 64-bits version of MEDCoupling.
   *
   * Implemented if the code was built with MEDCoupling support.
   * This can be used to assess compatibility between codes when coupling them.
   *
   * @return the MEDCoupling major version number
   */
  virtual bool isMEDCoupling64Bits() const;

  // ******************************************************
  // section Scalar values I/O
  // ******************************************************

  /*! @brief Get the list of input scalars accepted by the code.
   *
   * @return the list of scalar names that represent inputs of the code
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @sa getInputFieldsNames()
   */
  virtual std::vector<std::string> getInputValuesNames() const;

  /*! @brief Get the list of output scalars that can be provided by the code.
   *
   * @return the list of scalar names that can be returned by the code
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @sa getOutputFieldsNames()
   */
  virtual std::vector<std::string> getOutputValuesNames() const;

  /*! @brief Get the type of a scalar managed by the code (input or output)
   *
   * The three possible types are int, double and string, as defined in the ValueType enum.
   *
   * @param[in] name scalar value name
   * @return one of ValueType::Double, ValueType::Int or ValueType::String
   * @throws ICoCo::WrongArgument exception if the scalar name is invalid.
   * @throws ICoCo::WrongContext exception if called before initialize().
   * @sa ValueType
   * @sa getFieldType()
   */
  virtual ValueType getValueType(const std::string& name) const;

  virtual std::string getValueUnit(const std::string& varName) const;

  /*! @brief Provides the code with a scalar double data.
   *
   * @param[in] name name of the scalar value that is given to the code.
   * @param[in] val value passed to the code.
   * @throws ICoCo::WrongArgument exception if the scalar name ('name' parameter) is invalid.
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual void setIntputDoubleValue(const std::string& name, const double& val);

  /*! @brief Retrieves a scalar double value from the code.
   *
   * @param[in] name name of the scalar value to be read from the code.
   * @return the double value read from the code.
   * @throws ICoCo::WrongArgument exception if the scalar name ('name' parameter) is invalid.
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual double getOutputDoubleValue(const std::string& name) const;

  /*! @brief Provides the code with a scalar integer data.
   *
   * @param[in] name name of the scalar value that is given to the code.
   * @param[in] val value passed to the code.
   * @throws ICoCo::WrongArgument exception if the scalar name ('name' parameter) is invalid.
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual void setIntputIntValue(const std::string& name, const int& val);

  /*! @brief Retrieves a scalar integer value from the code.
   *
   * @param[in] name name of the scalar value to be read from the code.
   * @return the integer value read from the code.
   * @throws ICoCo::WrongArgument exception if the scalar name ('name' parameter) is invalid.
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual int getOutputIntValue(const std::string& name) const;

  /*! @brief Provides the code with a scalar string data.
   *
   * @param[in] name name of the scalar value that is given to the code.
   * @param[in] val value passed to the code.
   * @throws ICoCo::WrongArgument exception if the scalar name ('name' parameter) is invalid.
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual void setIntputStringValue(const std::string& name, const std::string& val);

  /*! @brief Retrieves a scalar string value from the code.
   *
   * @param[in] name name of the scalar value to be read from the code.
   * @return the string read from the code.
   * @throws ICoCo::WrongArgument exception if the scalar name ('name' parameter) is invalid.
   * @throws ICoCo::WrongContext exception if called before initTimeStep().
   */
  virtual std::string getOutputStringValue(const std::string& name) const;
};

}  // namespace ICoCo

/*! @brief (Mandatory) Retrieves an instance of the class defined by the code (and inheriting Problem).
 *
 * The main purpose of this function is to facilitate the instanciation of a problem when the code is loaded
 * in the supervisor using a dlopen() mechanism. Code implementing the ICoCo interface should implement this
 * method by returning a new instance of their derived class of Problem.
 * No default implementation can be provided for this.
 *
 * @return a new instance of the derived class of Problem implemented by the code.
 */
extern "C" ICoCo::Problem* getProblem();

#endif
