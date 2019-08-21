// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found in the TRUST repository:
//
//    https://sourceforge.net/projects/trust/

#ifndef ICoCoField_included
#define ICoCoField_included
#include <string>

namespace ICoCo
{
/*! @brief Top abstract class defining field objects that can be exchanged via the ICoCo interface.
 *
 * The Field class holds the name of the field.
 */
class Field
{
 public:
  /*! @brief Set the name of the field.
   * @param name name of the field
   */
  void setName(const std::string& name);

  /*! @brief Retrieves the name of the field.
   * @return name of the field.
   */
  const std::string& getName() const;

  /*!
   * @brief Retrieves the name of the field as a char *
   * @return name of the field.
   */
  const char* getCharName() const;

 protected:
  Field();
  virtual ~Field();

 private:
  std::string* _name;
};
}  // namespace ICoCo
#endif
