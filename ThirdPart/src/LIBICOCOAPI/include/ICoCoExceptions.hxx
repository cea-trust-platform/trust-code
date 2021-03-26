// ICoCo file common to several codes
// Version 2 -- 02/2021
//
// WARNING: this file is part of the official ICoCo API and should not be modified.
// The official version can be found at the following URL:
//
//    https://github.com/cea-trust-platform/icoco-coupling

#include <exception>
#include <string>

#ifndef ICoCoExceptions_included
#define ICoCoExceptions_included

namespace ICoCo
{
/*! @brief Exception raised when an ICoCo method is called at the wrong place.
 *
 * This exception is raised whenver an ICoCo method is called when it shouldn't.
 * This is for example the case if the Problem::initTimeStep() method is called when
 * Problem::initialize() hasn't been called yet.
 */
class WrongContext : public std::exception
{
 public:
  /*! @brief Constructor.
   *
   * @param prob problem name
   * @param method name of the method where the exception occurred
   * @param precondition detail of the condition that wasn't met
   */
  WrongContext(const std::string& prob, const std::string& method, const std::string& precondition);

  /*! @brief Destructor
   */
  ~WrongContext() throw()
  {
  }

  /*! @brief Gets the exception message.
   * @return the exception full message.
   */
  virtual const char* what() const throw();

 private:
  std::string prob;          ///< Name of the problem in which exception occurred
  std::string method;        ///< Method in which exception occurred
  std::string precondition;  ///< Precondition which was not met by the Problem state
};

/*! @brief Exception raised when an ICoCo method is called with an invalid argument.
 *
 * This exception is raised whenver an ICoCo method is called with inappropriate arguments.
 * This is for example the case if the Problem::getOutputField() is called with a non-existing field
 * name.
 */
class WrongArgument : public std::exception
{
 public:
  /*! @brief Constructor.
   *
   * @param prob problem name
   * @param method name of the method where the exception occurred
   * @param arg name of the faulty argument
   * @param condition detail of the condition that wasn't met
   */
  WrongArgument(const std::string& prob, const std::string& method, const std::string& arg,
                const std::string& condition);

  /*! @brief Destructor
   */
  ~WrongArgument() throw()
  {
  }

  /*! @brief Gets the exception message.
   * @return the exception full message.
   */
  virtual const char* what() const throw();

 private:
  std::string prob;       ///< name of the problem in which exception occurred
  std::string method;     ///< method in which exception occurred
  std::string arg;        ///< argument for which exception occurred
  std::string condition;  ///< condition which was not met by the argument
};

/*! @brief Exception raised when one tries to call an ICoCo method which is not implemented.
 *
 * By default all ICoCo methods raise this exception. The code implementing the norm
 * should derive the Problem class and override (at least partially) this default
 * implementation.
 */
class NotImplemented : public std::exception
{
 public:
  /*! @brief Constructor.
   *
   * @param prob problem name
   * @param method name of the method where the exception occurred
   */
  NotImplemented(const std::string& prob, const std::string& method);

  /*! @brief Destructor
   */
  ~NotImplemented() throw()
  {
  }

  /*! @brief Gets the exception message.
   * @return the exception full message.
   */
  virtual const char* what() const throw();

 private:
  std::string prob;    ///< Problem in which exception occurred
  std::string method;  ///< method in which exception occurred
};

}  // namespace ICoCo

#endif
