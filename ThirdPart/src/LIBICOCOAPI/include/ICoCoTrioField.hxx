// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found at the following URL:
//
//    https://github.com/cea-trust-platform/icoco-coupling

#ifndef ICoCoTrioField_included
#define ICoCoTrioField_included

#include <ICoCoField.hxx>
namespace ICoCo
{

  /*! @brief Field data stored internally as a TrioField object.
   *
   *  This structure contains all the necessary information
   *  for constructing MEDCouplingFieldDouble (with the addition of the MPI
   *  communicator).
   *  This structure can either own or not _field values (_has_field_ownership)
   *  For _coords, _connectivity and _field, a null pointer means no data allocated.
   *  _coords and _connectivity tables, when allocated, are always owned by the TrioField.
   *
   *  Copy constructor and assignment operator raise an exception as they are not implemented.
   */
  class TrioField : public Field
  {
  public:
    /*! @brief Builds an empty field.
     */
    TrioField();

    /*! @brief Copy constructor - FORBIDDEN.
     * @throws ICoCo::NotImplemented
     */
    TrioField(const TrioField& OtherField);

    /*! @brief Assignment operator - FORBIDDEN.
     * @throws ICoCo::NotImplemented
     */
    TrioField& operator=(const TrioField& NewField);

    /*! @brief Destructor.
     */
    ~TrioField();

    /*! @brief Clear and reset all internal data structures.
     *
     * After the call to clear(), all pointers are null and field ownership is false.
     * Arrays are deleted if necessary
     */
    void clear();

    /*! @brief Acquire field ownership.
     *
     * After a call to set_standalone(), field ownership is true and field is allocated
     * to the size _nb_field_components*nb_values().
     * The values of the field have been copied if necessary.
     */
    void set_standalone();

    /*! @brief Used to simulate a 0D geometry (Cathare/Trio for example).
     */
    void dummy_geom();

    /*! @brief Save field to a .field file
     */
    void save(std::ostream& os) const;

    /*! @brief Restore field from a .field file
     */
    void restore(std::istream& in);

    /*! @brief The size of field is nb_values()*_nb_field_components
     */
    int nb_values() const;

  public:
    int _type;         ///< 0 field on element, 1 field on nodes
    int _mesh_dim;     ///< Mesh dimension
    int _space_dim;    ///< Space dimension
    int _nbnodes;      ///< Number of nodes
    int _nodes_per_elem;  ///< Number of nodes for a single element
    int _nb_elems;     ///< Number of elements
    int _itnumber;     ///< (time) iteration number
    int* _connectivity;  ///< Nodal connectivity array
    double* _coords;   ///< Coordinate array

    double _time1, _time2;
    int _nb_field_components;
    double* _field;
    bool _has_field_ownership;
  };
}  // namespace ICoCo

#endif
