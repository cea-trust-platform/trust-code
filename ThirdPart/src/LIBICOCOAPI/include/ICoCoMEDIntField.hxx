// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found at the following URL:
//
//    https://github.com/cea-trust-platform/icoco-coupling

#ifndef ICoCoMEDIntField_included
#define ICoCoMEDIntField_included

#include "ICoCoField.hxx"

namespace MEDCoupling
{
  class MEDCouplingFieldInt32;
}

namespace ICoCo
{
  /*! @brief Field data stored internally as a MEDCoupling object.
   *
   * This class is a wrapper around a MEDCoupling::MEDCouplingFieldInt32 object, which holds the field data.
   * In version 2 of ICoCo, MEDCoupling objects are not anymore exposed directly into the API
   * of ICoCo::Problem. The rationale is to make the interface ICoCo::Problem free of external dependencies
   * (MEDCoupling particularly).
   *
   * @sa the MEDCoupling documentation, notably the reference counter mechanism used to manage the lifecycle of
   * MEDCoupling objects.
   */
  class ICOCO_EXPORT MEDIntField : public ICoCo::Field
  {
  public:
    /*! Builds an empty field (internal MEDCoupling object not set).
     */
    MEDIntField();

    /*! @brief Builds a field and assign its internal MEDCouplingField object.
     *
     * The name of 'this' is also automatically set the name of the MEDCouplingField object. If 'field' is null
     * the name is set to the empty string.
     *
     * @param field MEDCoupling field instance to use for field data. The field reference counter is incremented.
     */
    MEDIntField(MEDCoupling::MEDCouplingFieldInt32* field);

    /*! @brief Copy construcotr.
     */
    MEDIntField(const MEDIntField& field);

    /*! @brief Assignement operator.
     * @param field another MEDDoubleField instance. The previous internal MEDCoupling field reference (if any) has
     * its counter decremented.
     */
    MEDIntField& operator=(const MEDIntField& field);

    /*! @brief Destructor.
     */
    virtual ~MEDIntField();

    /*! @brief Get the internal MEDCoupling field object.
     * @return a pointer to the MEDCouplingField object detained by this instance. Note that the corresponding
     * object should not be deleted, or its reference counter should not be decreased! Doing so will result in an
     * invalid instance of the current MEDField.
     */
    MEDCoupling::MEDCouplingFieldInt32 *getMCField() const;

    /*! @brief Set the internal MEDCoupling field object.
     *
     * Any previously set field is discarded (its reference counter is decreased) and the reference counter of the
     * field being set is increased.
     *
     * The name of 'this' is also automatically set the name of the MEDCouplingField object. If 'field' is nullptr
     * the name is reset to the empty string.
     *
     * @param field MEDCouplingFieldDouble object to be used.
     */
    void setMCField(MEDCoupling::MEDCouplingFieldInt32 * f);

  private:
    MEDCoupling::MEDCouplingFieldInt32 *_field;
  };
} // namespace ICoCo

#endif
