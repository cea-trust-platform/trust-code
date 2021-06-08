// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found at the following URL:
//
//    https://github.com/cea-trust-platform/icoco-coupling

#ifndef ICoCoProblem_included
#define ICoCoProblem_included

#ifdef ICOCO_VERSION
#   error "ICOCO_VERSION already defined!! Are you including twice two versions of the ICoCo interface?"
#else
#   define ICOCO_VERSION "2.0"
#   define ICOCO_MAJOR_VERSION 2
#   define ICOCO_MINOR_VERSION 0
#endif

#include <vector>
#include <string>

/*! @brief The namespace ICoCo (Interface for code coupling) encompasses all the classes
 * and methods needed for the coupling of codes.
 * See the class Problem and the function getProblem() to start with.
 */
namespace ICoCo
{
  class MEDDoubleField;
  class MEDIntField;
  class MEDStringField;
  class TrioField;

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
   * ICoCo standard. For organization and documentation purposes the interface is separated into several sections but
   * this does not correspond to any code constraint.
   * Note that not all the methods need to be implemented! Notably the methods belonging to the sections
   *   - Restorable
   *   - Field I/O
   *   - Scalar values I/O
   * are not always needed since a code might not have any integer field to work with for example.
   * Consequently, default implementation for all methods of this interface is to raise an ICoCo::NotImplemented
   * exception.
   *
   * Some of the methods may not be called when some conditions are not met (i.e. when not in the correct context). Thus
   * in this documentation we define the "TIME_STEP_DEFINED context" as the context that the code finds itself, when the method
   * initTimeStep() has been called, and the method validateTimeStep() (or abortTimeStep()) has not yet been called.
   * This is the status in which the current computation time step is well defined.
   *
   * Within the computation of a time step (so within TIME_STEP_DEFINED), the temporal semantic of the fields (and
   * scalar values) is not imposed by the norm. Said differently, it does not require the fields to be defined at the
   * start/middle/end of the current time step, this semantic must be agreed on between the codes being coupled.
   * Fields and scalar values that are set within the TIME_STEP_DEFINED context are invalidated after a call to
   * validateTimeStep() (or abortTimeStep()). They need to be set at each time step. However, fields and scalar values that are
   * set outside of this context (before the first time step for example, or after the resolution of the last time step) are
   * permanent.
   *
   * Finally, the ICoCo interface may be wrapped in Python using SWIG or PyBind11. For an example of the former see the
   * TRUST implementation of ICoCo. Notably the old methods returning directly MEDCoupling::MEDCouplingFieldDouble objects
   * (version 1.x of ICoCo) are easily re-instanciated in Python SWIG.
   */
  class Problem
  {

  public :

    /*! @brief Return ICoCo interface major version number.
     * @return ICoCo interface major version number (2 at present)
     */
    static int GetICoCoMajorVersion() { return ICOCO_MAJOR_VERSION; }

    // ******************************************************
    // section Problem
    // ******************************************************

    /*! @brief Constructor.
     *
     * Internal set up and initialization of the code should not be done here, but rather in initialize().
     */
    Problem();

    /*! @brief Destructor.
     *
     * The release of the resources allocated by the code should not be performed here, but rather in terminate().
     */
    virtual ~Problem();

    /*! @brief (Optional) Provide the relative path of a data file to be used by the code.
     *
     * This method must be called before initialize().
     *
     * @param[in] datafile relative path to the data file.
     * @throws ICoCo::WrongContext exception if called multiple times or after initialize().
     * @throws ICoCo::WrongArgument if an invalid path is provided.
     */
    virtual void setDataFile(const std::string& datafile);

    /*! @brief (Optional) Provide the MPI communicator to be used by the code for parallel computations.
     *
     * This method must be called before initialize(). The communicator should include all the processes
     * to be used by the code. For a sequential code, the call to setMPIComm is optional or mpicomm should be
     * nullptr.
     *
     * @param[in] mpicomm pointer to a MPI communicator. Type void* to avoid to include mpi.h for sequential codes.
     * @throws ICoCo::WrongContext exception if called multiple times or after initialize().
     */
    virtual void setMPIComm(void* mpicomm);

    /*! @brief (Mandatory) Initialize the current problem instance.
     *
     * In this method the code should allocate all its internal structures and be ready to execute. File reads, memory
     * allocations, and other operations likely to fail should be performed here, and not in the constructor (and not in
     * the setDataFile() or in the setMPIComm() methods either).
     * This method must be called only once (after a potential call to setMPIComm() and/or setDataFile()) and cannot be
     * called again before terminate() has been performed.
     *
     * @return true if all OK, otherwise false.
     * @throws ICoCo::WrongContext exception if called multiple times or after initialize().
     */
    virtual bool initialize();

    /*! @brief (Mandatory) Terminate the current problem instance and release all allocated resources.
     *
     * Terminate the computation, free the memory and save whatever needs to be saved. This method is called once
     * at the end of the computation or after a non-recoverable error.
     * No other ICoCo method except setDataFile(), setMPIComm() and initialize() may be called after this.
     *
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the TIME_STEP_DEFINED context (see Problem documentation).
     */
    virtual void terminate();


    // ******************************************************
    // section TimeStepManagement
    // ******************************************************

    /*! @brief (Mandatory) Return the current time of the simulation.
     *
     * Can be called any time between initialize() and terminate().
     * The current time can only change during a call to validateTimeStep() or to resetTime().
     *
     * @return the current (physical) time of the simulation
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual double presentTime() const;

    /*! @brief (Mandatory) Return the next preferred time step (time increment) for this code, and whether the code
     * wants to stop.
     *
     * Both data are only indicative, the supervisor is not required to take them into account. This method is
     * however marked as mandatory, since most of the coupling schemes expect the code to provide this
     * information (those schemes then typically compute the minimum of the time steps of all the codes being coupled).
     * Hence a possible implementation is to return a huge value, if a precise figure can not be computed.
     *
     * Can be called whenever the code is outside the TIME_STEP_DEFINED context (see Problem documentation).
     *
     * @param[out] stop set to true if the code wants to stop. It can be used for example to indicate that, according to
     * a certain criterion, the end of the transient computation is reached from the code point of view.
     * @return the preferred time step for this code (only valid if stop is false).
     * @throws ICoCo::WrongContext exception if called inside the TIME_STEP_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual double computeTimeStep(bool& stop) const;

    /*! @brief (Mandatory) Provide the next time step (time increment) to be used by the code.
     *
     * After this call (if successful), the computation time step is defined to ]t, t + dt] where t is the value
     * returned by presentTime(). The code enters the TIME_STEP_DEFINED context.
     *
     * @param[in] dt the time step to be used by the code
     * @return false means that given time step is not compatible with the code time scheme.
     *
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the TIME_STEP_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called several times without resolution.
     * @throws ICoCo::WrongArgument exception if dt is invalid (dt <= 0.0).
     */
    virtual bool initTimeStep(double dt);

    /*! @brief (Mandatory) Perform the computation on the current time interval.
     *
     * Can be called whenever the code is inside the TIME_STEP_DEFINED context (see Problem documentation).
     *
     * @return true if computation was successful, false otherwise.
     * @throws ICoCo::WrongContext exception if called outside the TIME_STEP_DEFINED context (see Problem documentation).
     */
    virtual bool solveTimeStep();

    /*! @brief (Mandatory) Validate the computation performed by solveTimeStep.
     *
     * Can be called whenever the code is inside the TIME_STEP_DEFINED context (see Problem documentation).
     * After this call:
     * - the present time has been advanced to the end of the computation time step
     * - the computation time step is undefined (the code leaves the TIME_STEP_DEFINED context).
     *
     * @throws ICoCo::WrongContext exception if called outside the TIME_STEP_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called before the solveTimeStep() method.
     * @sa abortTimeStep()
     */
    virtual void validateTimeStep();

    /*! @brief (Mandatory) Set whether the code should compute a stationary solution or a transient one.
     *
     * New in version 2 of ICoCo. By default the code is assumed to be in stationary mode False (i.e. set up
     * for a transient computation).
     * If set to True, solveTimeStep() can be used either to solve a time step in view of an asymptotic solution,
     * or to solve directly for the steady-state, whatever the time step provided by initTimeStep() (whose call is still
     * needed). The stationary mode status of the code can only be modified by this method (or by a call to terminate()
     * followed by initialize()).
     *
     * Can be called whenever the code is outside the TIME_STEP_DEFINED context (see Problem documentation).
     *
     * @param[in] stationaryMode true if the code should compute a stationary solution.
     *
     * @throws ICoCo::WrongContext exception if called inside the TIME_STEP_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual void setStationaryMode(bool stationaryMode);

    /*! @brief (Mandatory) Indicate whether the code should compute a stationary solution or a transient one.
     *
     * See also setStationaryMode().
     *
     * Can be called whenever the code is outside the TIME_STEP_DEFINED context (see Problem documentation).
     *
     * @return true if the code has been set to compute a stationary solution.
     *
     * @throws ICoCo::WrongContext exception if called inside the TIME_STEP_DEFINED context (see Problem documentation).
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual bool getStationaryMode() const;


    /*! @brief (Optional) Return whether the solution is constant on the computation time step.
     *
     * Used to know if the steady-state has been reached. This method can be called whenever the computation time step
     * is not defined.
     *
     * @return true if the solution is constant on the computation time step.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception  if called inside the TIME_STEP_DEFINED context (see Problem documentation),
     * meaning we shouldn't request this information while the computation of a new time step is in progress.
     */
    virtual bool isStationary() const;

    /*! @brief (Optional) Abort the computation on the current time step.
     *
     * Can be called whenever the computation time step is defined, instead of validateTimeStep().
     * After this call, the present time is left unchanged, and the computation time step is undefined
     * (the code leaves the TIME_STEP_DEFINED context).
     *
     * @throws ICoCo::WrongContext exception if called outside the TIME_STEP_DEFINED context (see Problem documentation).
     * @sa validateTimeStep()
     */
    virtual void abortTimeStep();

    /*! @brief (Optional) Reset the current time of the Problem to a given value.
     *
     * New in version 2 of ICoCo.
     * Particularly useful for the initialization of complex transients: the starting point of the transient
     * of interest is computed first, the time is reset to 0, and then the actual transient of interest starts with proper
     * initial conditions, and global time 0.
     *
     * Can be called outside the TIME_STEP_DEFINED context (see Problem documentation).
     *
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the TIME_STEP_DEFINED context (see Problem documentation)
     */
    virtual void resetTime(double time);

    /*! @brief  (Optional) Perform a single iteration of computation inside the time step.
     *
     * This method is relevant for codes having inner iterations for the computation of a single time step.
     * Calling iterateTimeStep() until converged is true is equivalent to calling solveTimeStep(), within the code's
     * convergence threshold.
     * Can be called (potentially several times) inside the TIME_STEP_DEFINED context (see Problem documentation).
     *
     * @param[out] converged set to true if the solution is not evolving any more.
     * @return false if the computation failed.
     * @throws ICoCo::WrongContext exception if called outside the TIME_STEP_DEFINED context (see Problem documentation)
     * @sa solveTimeStep()
     */
    virtual bool iterateTimeStep(bool& converged);

    // ******************************************************
    // section Restorable
    // ******************************************************

    /*! @brief (Optional) Save the state of the code.
     *
     * The saved state is identified by the combination of label and method arguments.
     * If save() has already been called with the same two arguments, the saved state is overwritten.
     * This method is const indicating that saving the state of the code should not change its behaviour with respect to
     * all other ICoCo methods. Implementation may rely on a mutable attribute (e.g. if saving to memory is desired).
     *
     * @param[in] label a user- (or code-) defined value identifying the state.
     * @param[in] method a string specifying which method is used to save the state of the code. A code can provide
     * different methods (for example in memory, on disk, etc.).
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the TIME_STEP_DEFINED context (see Problem documentation),
     * meaning we shouldn't save a previous time step while the computation of a new time step is in progress.
     * @throws ICoCo::WrongArgument exception if the method or label argument is invalid.
     */
    virtual void save(int label, const std::string& method) const;

    /*! @brief (Optional) Restore the state of the code.
     *
     * After restore, the code should behave exactly like after the corresponding call to save (except of course for
     * save/restore methods, since the list of saved states may have changed).
     * The state to be restored is identified by the combination of label and method arguments.
     * The save() method must have been called at some point or in some previous run with this combination.
     *
     * @param[in] label a user- (or code-) defined value identifying the state.
     * @param[in] method a string specifying which method is used to restore the state of the code. A code can provide
     * different methods (for example in memory, on disk, etc.).
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongContext exception if called inside the TIME_STEP_DEFINED context (see Problem documentation),
     * meaning we shouldn't restore while the computation of a new time step is in progress.
     * @throws ICoCo::WrongArgument exception if the method or label argument is invalid.
     */
    virtual void restore(int label, const std::string& method);

    /*! @brief (Optional) Discard a previously saved state of the code.
     *
     * After this call, the save-point cannot be restored anymore. This method can be used to free the space occupied by
     * unused saved states.
     * This method is const indicating that forgeting a previous state of the code should not change its behaviour with
     * respect to all other ICoCo methods. Implementation may rely on a mutable attribute (e.g. if saving to memory is
     * desired).
     *
     * @param[in] label a user- (or code-) defined value identifying the state.
     * @param[in] method a string specifying which method is used to restore the state of the code. A code can provide
     * different methods (for example in memory, on disk, etc.).
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the method or label argument is invalid.
     */
    virtual void forget(int label, const std::string& method) const;


    // ******************************************************
    // section Field I/O. Reminder: all methods are **optional** not all of them need to be implemented!
    // ******************************************************

    /*! @brief (Optional) Get the list of input fields accepted by the code.
     *
     * @return the list of field names that represent inputs of the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getInputFieldsNames() const;

    /*! @brief (Optional) Get the list of output fields that can be provided by the code.
     *
     * @return the list of field names that can be produced by the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getOutputFieldsNames() const;

    /*! @brief (Optional) Get the type of a field managed by the code (input or output)
     *
     * The three possible types are int, double and string, as defined in the ValueType enum.
     *
     * @param[in] name field name
     * @return one of ValueType::Double, ValueType::Int or ValueType::String
     * @throws ICoCo::WrongArgument exception if the field name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @sa ValueType
     */
    virtual ValueType getFieldType(const std::string& name) const;

    /*! @brief (Optional) Get the (length) unit used to define the meshes supporting the fields.
     *
     * @return length unit in which the mesh coordinates should be understood (e.g. "m", "cm", ...)
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getMeshUnit() const;

    /*! @brief (Optional) Get the physical unit used for a given field.
     *
     * @return unit in which the field values should be understood (e.g. "W", "J", "Pa", ...)
     * @throws ICoCo::WrongArgument exception if the field name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getFieldUnit(const std::string& fieldName) const;


    // ******************************************************
    //     subsection MED*Field fields I/O
    // ******************************************************

    /*! @brief (Optional) Retrieve an empty shell for an input field. This shell can be filled by the caller and then be
     * given to the code via setInputField(). The field has the MEDDoubleField format.
     *
     * The code uses this method to populate 'afield' with all the data that represents the context
     * of the field (i.e. its support mesh, its discretization -- on nodes, on elements, ...).
     * The remaining job for the caller of this method is to fill the actual values of the field itself.
     * When this is done the field can be sent back to the code through the method setInputField().
     * This method is not mandatory but is useful to know the mesh, discretization... on which an input field is
     * expected.
     *
     * See Problem documentation for more details on the time semantic of a field.
     *
     * @param[in] name name of the field for which we would like the empty shell
     * @param[out] afield field object (in MEDDoubleField format) that will be populated with all the contextual information.
     * Any previous information in this object will be discarded.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the field name is invalid.
     */
    virtual void getInputMEDDoubleFieldTemplate(const std::string& name, MEDDoubleField& afield) const;


    /*! @brief (Optional) Provide the code with input data in the form of a MEDDoubleField.
     *
     * The method getInputFieldTemplate(), if implemented, may be used first to prepare an empty shell of the field to
     * pass to the code.
     *
     * See Problem documentation for more details on the time semantic of a field.
     *
     * @param[in] name name of the field that is given to the code.
     * @param[in] afield field object (in MEDDoubleField format) containing the input data to be read by the code. The name
     * of the field set on this instance (with the Field::setName() method) should not be checked. However its time value
     * should be to ensure it is within the proper time interval ]t, t+dt].
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the field name ('name' parameter) is invalid.
     * @throws ICoCo::WrongArgument exception if the time property of 'afield' does not belong to the currently computed
     * time step ]t, t + dt]
     */
    virtual void setInputMEDDoubleField(const std::string& name, const MEDDoubleField& afield);


    /*! @brief (Optional) Retrieve output data from the code in the form of a MEDDoubleField.
     *
     * Gets the output field corresponding to name from the code into the afield argument.
     *
     * See Problem documentation for more details on the time semantic of a field.
     *
     * @param[in] name name of the field that the caller requests from the code.
     * @param[out] afield field object (in MEDDoubleField format) populated with the data read by the code. The name
     * and time properties of the field should be set in accordance with the 'name' parameter and with the current
     * time step being computed.
     * Any previous information in this object will be discarded.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the field name ('name' parameter) is invalid.
     */
    virtual void getOutputMEDDoubleField(const std::string& name, MEDDoubleField& afield) const;

    /*! @brief (Optional) Update a previously retrieved output field.
     *
     * (New in version 2) This methods allows the code to implement a more efficient update of a given output field,
     * thus avoiding the caller to invoke getOutputMEDDoubleField() each time.
     * A previous call to getOutputMEDDoubleField() with the same name must have been done prior to this call.
     * The code should check the consistency of the field object with the requested data (same support mesh,
     * discretization -- on nodes, on elements, etc.).
     *
     * See Problem documentation for more details on the time semantic of a field.
     *
     * @param[in] name name of the field that the caller requests from the code.
     * @param[out] afield field object (in MEDDoubleField format) updated with the data read from the code. Notably the time
     * indicated in the field should be updated to be within the current time step being computed.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @throws ICoCo::WrongArgument exception if the field name ('name' parameter) is invalid.
     * @throws ICoCo::WrongArgument exception if the field object is inconsistent with the field being requested.
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
     * @b WARNING: at the time of writing, MEDStringField are not yet implemented anywhere.
     * @sa getInputMEDDoubleFieldTemplate()
     */
    virtual void getInputMEDStringFieldTemplate(const std::string& name, MEDStringField& afield) const;

    /*! @brief Similar to setInputMEDDoubleField() but for MEDStringField.
     *
     * @b WARNING: at the time of writing, MEDStringField are not yet implemented anywhere.
     * @sa setInputMEDDoubleField()
     */
    virtual void setInputMEDStringField(const std::string& name, const MEDStringField& afield);

    /*! @brief Similar to getOutputMEDDoubleField() but for MEDStringField.
     *
     * @b WARNING: at the time of writing, MEDStringField are not yet implemented anywhere.
     * @sa getOutputMEDDoubleField()
     */
    virtual void getOutputMEDStringField(const std::string& name, MEDStringField& afield) const;

    /*! @brief Similar to updateOutputMEDDoubleField() but for MEDStringField.
     *
     * @b WARNING: at the time of writing, MEDStringField are not yet implemented anywhere.
     * @sa updateOutputMEDDoubleField()
     */
    virtual void updateOutputMEDStringField(const std::string& name, MEDStringField& afield) const;

    /*! @brief (Optional) Get MEDCoupling major version, if the code was built with MEDCoupling support.
     *
     * This can be used to assess compatibility between codes when coupling them.
     *
     * @return the MEDCoupling major version number (typically 7, 8, 9, ...)
     */
    virtual int getMEDCouplingMajorVersion() const;

    /*! @brief (Optional) Indicate whether the code was built with a 64-bits version of MEDCoupling.
     *
     * Implemented if the code was built with MEDCoupling support.
     * This can be used to assess compatibility between codes when coupling them.
     *
     * @return the MEDCoupling major version number
     */
    virtual bool isMEDCoupling64Bits() const;

    // ******************************************************
    //     subsection TrioField fields I/O: double, int and string.
    // ******************************************************

    /*! @brief Similar to getInputMEDDoubleFieldTemplate() but for TrioField.
     * @sa getInputMEDDoubleFieldTemplate()
     */
    virtual void getInputFieldTemplate(const std::string& name, TrioField& afield) const;

    /*! @brief Similar to setInputMEDDoubleField() but for TrioField.
     * @sa setInputMEDDoubleField()
     */
    virtual void setInputField(const std::string& name, const TrioField& afield);

    /*! @brief Similar to getOutputMEDDoubleField() but for TrioField.
     * @sa getOutputMEDDoubleField()
     */
    virtual void getOutputField(const std::string& name, TrioField& afield) const;

    /*! @brief Similar to updateOutputMEDDoubleField() but for TrioField.
     * @sa updateOutputMEDDoubleField()
     */
    virtual void updateOutputField(const std::string& name, TrioField& afield) const;


    // ******************************************************
    // section Scalar values I/O
    // ******************************************************

    /*! @brief (Optional) Get the list of input scalars accepted by the code.
     *
     * @return the list of scalar names that represent inputs of the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getInputValuesNames() const;

    /*! @brief (Optional) Get the list of output scalars that can be provided by the code.
     *
     * @return the list of scalar names that can be returned by the code
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::vector<std::string> getOutputValuesNames() const;

    /*! @brief (Optional) Get the type of a scalar managed by the code (input or output)
     *
     * The three possible types are int, double and string, as defined in the ValueType enum.
     *
     * @param[in] name scalar value name
     * @return one of ValueType::Double, ValueType::Int or ValueType::String
     * @throws ICoCo::WrongArgument exception if the scalar name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     * @sa ValueType
     */
    virtual ValueType getValueType(const std::string& name) const;

    /*! @brief (Optional) Get the physical unit used for a given value.
     *
     * @return unit in which the scalar value should be understood (e.g. "W", "J", "Pa", ...)
     * @throws ICoCo::WrongArgument exception if the value name is invalid.
     * @throws ICoCo::WrongContext exception if called before initialize() or after terminate().
     */
    virtual std::string getValueUnit(const std::string& varName) const;

    /*! @brief (Optional) Provide the code with a scalar double data.
     *
     * See Problem documentation for more details on the time semantic of a scalar value.
     *
     * @param[in] name name of the scalar value that is given to the code.
     * @param[in] val value passed to the code.
     * @throws ICoCo::WrongArgument exception if the scalar name ('name' parameter) is invalid.
     */
    virtual void setIntputDoubleValue(const std::string& name, const double& val);

    /*! @brief (Optional) Retrieve a scalar double value from the code.
     *
     * See Problem documentation for more details on the time semantic of a scalar value.
     *
     * @param[in] name name of the scalar value to be read from the code.
     * @return the double value read from the code.
     * @throws ICoCo::WrongArgument exception if the scalar name ('name' parameter) is invalid.
     */
    virtual double getOutputDoubleValue(const std::string& name) const;

    /*! @brief (Optional) Similar to setIntputDoubleValue() but for an int value.
     * @sa setIntputDoubleValue()
     */
    virtual void setIntputIntValue(const std::string& name, const int& val);

    /*! @brief (Optional) Similar to getOutputDoubleValue() but for an int value.
     * @sa getOutputDoubleValue()
     */
    virtual int getOutputIntValue(const std::string& name) const;

    /*! @brief (Optional) Similar to setIntputDoubleValue() but for an string value.
     * @sa setIntputDoubleValue()
     */
    virtual void setIntputStringValue(const std::string& name, const std::string& val);

    /*! @brief (Optional) Similar to getOutputDoubleValue() but for an string value.
     * @sa getOutputDoubleValue()
     */
    virtual std::string getOutputStringValue(const std::string& name) const;
  };
}

/*! @brief (Mandatory) Retrieve an instance of the class defined by the code (and inheriting Problem).
 *
 * The main purpose of this function is to facilitate the instantiation of a problem when the code is loaded
 * in the supervisor using a dlopen() mechanism. Code implementing the ICoCo interface should implement this
 * method by returning a new instance of their derived class of Problem.
 * No default implementation can be provided for this.
 *
 * @return a new instance of the derived class of Problem implemented by the code.
 */
extern "C" ICoCo::Problem* getProblem();

#endif
