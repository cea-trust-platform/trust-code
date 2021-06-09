// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found in the TRUST repository:
//
//    https://sourceforge.net/projects/trust/

#include <ICoCoTrioField.hxx>
#include <ICoCoExceptions.hxx>
#include <iomanip>  // used for setprecision()
#include <iostream>
#include <string.h>

namespace ICoCo
{
  TrioField::TrioField()
  : _type(0)
    , _mesh_dim(0)
    , _space_dim(0)
    , _nbnodes(0)
    , _nodes_per_elem(0)
    , _nb_elems(0)
    , _itnumber(0)
    , _connectivity(0)
    , _coords(0)
    , _time1(0.)
    , _time2(0.)
    , _nb_field_components(0)
    , _field(0)
    , _has_field_ownership(false)
    {
    }

  TrioField::TrioField(const TrioField& OtherField)
  : ICoCo::Field()
  {
    throw ICoCo::NotImplemented("_", "TrioField::(copy constructor)");
  }

  TrioField::~TrioField()
  {
    clear();
  }

  void TrioField::clear()
  {
    if (_connectivity)
      delete[] _connectivity;
    if (_coords)
      delete[] _coords;
    if (_field && _has_field_ownership)
      delete[] _field;
    _connectivity = 0;
    _coords = 0;
    _field = 0;
    _has_field_ownership = false;
  }

  // Returns the number of value locations
  int TrioField::nb_values() const
  {
    if (_type == 0)
      return _nb_elems;
    else if (_type == 1)
      return _nbnodes;
    throw 0;
  }

  void TrioField::save(std::ostream& os) const
  {
    os << std::setprecision(12);
    os << getName() << std::endl;
    os << _type << std::endl;
    os << _mesh_dim << std::endl;
    os << _space_dim << std::endl;
    os << _nbnodes << std::endl;
    os << _nodes_per_elem << std::endl;
    os << _nb_elems << std::endl;

    os << _itnumber << std::endl;
    for (int i = 0; i < _nb_elems; i++)
      {
        for (int j = 0; j < _nodes_per_elem; j++)
          os << " " << _connectivity[i * _nodes_per_elem + j];
        os << std::endl;
      }

    for (int i = 0; i < _nbnodes; i++)
      {
        for (int j = 0; j < _space_dim; j++)
          os << " " << _coords[i * _space_dim + j];
        os << std::endl;
      }

    os << _time1 << std::endl;
    os << _time2 << std::endl;
    os << _nb_field_components << std::endl;

    if (_field)
      {
        os << 1 << std::endl;
        for (int i = 0; i < nb_values(); i++)
          {
            for (int j = 0; j < _nb_field_components; j++)
              os << " " << _field[i * _nb_field_components + j];
            os << std::endl;
          }
      }
    else
      os << 0 << std::endl;

    os << _has_field_ownership << std::endl;
  }

  void TrioField::restore(std::istream& in)
  {
    std::string name;
    in >> name;
    setName(name);
    in >> _type;
    in >> _mesh_dim;
    in >> _space_dim;
    in >> _nbnodes;
    in >> _nodes_per_elem;
    in >> _nb_elems;

    in >> _itnumber;
    if (_connectivity)
      delete[] _connectivity;
    _connectivity = new int[_nodes_per_elem * _nb_elems];
    for (int i = 0; i < _nb_elems; i++)
      {
        for (int j = 0; j < _nodes_per_elem; j++)
          in >> _connectivity[i * _nodes_per_elem + j];
      }
    if (_coords)
      delete[] _coords;
    _coords = new double[_nbnodes * _space_dim];
    for (int i = 0; i < _nbnodes; i++)
      {
        for (int j = 0; j < _space_dim; j++)
          in >> _coords[i * _space_dim + j];
      }

    in >> _time1;
    in >> _time2;
    in >> _nb_field_components;
    int test;
    in >> test;
    if (test)
      {
        if (_field)
          delete[] _field;
        _field = new double[_nb_field_components * nb_values()];
        for (int i = 0; i < nb_values(); i++)
          {
            for (int j = 0; j < _nb_field_components; j++)
              in >> _field[i * _nb_field_components + j];
          }
      }
    else
      _field = 0;

    in >> _has_field_ownership;
  }

  void TrioField::set_standalone()
  {
    if (!_field)
      {
        _field = new double[_nb_field_components * nb_values()];
        _has_field_ownership = true;
      }
    else if (!_has_field_ownership)
      {
        double* tmp_field = new double[_nb_field_components * nb_values()];
        memcpy(tmp_field, _field, _nb_field_components * nb_values() * sizeof(double));
        _field = tmp_field;
        _has_field_ownership = true;
      }
  }

  void TrioField::dummy_geom()
  {
    _type = 0;
    _mesh_dim = 2;
    _space_dim = 2;
    _nbnodes = 3;
    _nodes_per_elem = 3;
    _nb_elems = 1;
    _itnumber = 0;
    if (_connectivity)
      delete[] _connectivity;
    _connectivity = new int[3];
    _connectivity[0] = 0;
    _connectivity[1] = 1;
    _connectivity[2] = 2;
    if (_coords)
      delete[] _coords;
    _coords = new double[6];
    _coords[0] = 0;
    _coords[1] = 0;
    _coords[2] = 1;
    _coords[3] = 0;
    _coords[4] = 0;
    _coords[5] = 1;
    _time1 = 0;
    _time2 = 1;
    _nb_field_components = 1;
    if (_field && _has_field_ownership)
      delete[] _field;
    _has_field_ownership = false;
    _field = 0;
  }

  TrioField& TrioField::operator=(const TrioField& NewField)
  {
    throw ICoCo::NotImplemented("_", "TrioField::(assignment operator)");
  }

}  // end namespace ICoCo
