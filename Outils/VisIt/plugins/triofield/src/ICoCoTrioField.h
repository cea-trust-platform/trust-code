//////////////////////////////////////////////////////////////////////////////
//
// File:        ICoCoTrioField.h
// Directory:   $TRUST_ROOT/Kernel/ICoCo
// Version:     /main/5
//
//////////////////////////////////////////////////////////////////////////////

// ICoCo file common to several codes
// ICoCoTrioField.h
// version 1.2 10/05/2010

#ifndef _ICoCoTrioField_included_
#define _ICoCoTrioField_included_

#include <ICoCoField.h>
namespace ICoCo
{

//////////////////////////////////////////////////////////////////////////////
//
// .DESCRIPTION
//     class TrioField, used for coupling Trio codes via the ICoCo interface
//     This structure contains all the necessary information
//     for constructing a ParaMEDMEM::ParaFIELD (with the addition of the MPI
//     communicator).
//     This structure can either own or not _field values (_has_field_ownership)
//     For _coords, _connectivity and _field, a null pointer means no data allocated.
//     _coords and _connectivity tables, when allocated, are always owned by the TrioField.
//
//////////////////////////////////////////////////////////////////////////////
class TrioField:public Field
{
public:

  TrioField();
  TrioField(const TrioField& OtherField);
  ~TrioField();
  void clear();
  void set_standalone();
  void dummy_geom();
  TrioField& operator=(const TrioField& NewField);
  void save(std::ostream& os) const;
  void restore(std::istream& in);
  int nb_values() const ;

public:
  int _type ; // 0 elem 1 nodes
  int _mesh_dim;
  int _space_dim;
  int _nbnodes;
  int _nodes_per_elem;
  int _nb_elems;
  int _itnumber;
  int* _connectivity;
  double* _coords;

  double _time1,_time2;
  int _nb_field_components;
  double* _field;
  bool _has_field_ownership;
};
}

#endif
