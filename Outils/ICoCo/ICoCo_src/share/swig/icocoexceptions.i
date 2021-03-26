// Wrap and propagate C++ exceptions as Python exceptions
%exceptionclass ICoCo::WrongContext;
%rename (ICoCoWrongContext) ICoCo::WrongContext;
%exceptionclass ICoCo::WrongArgument;
%rename (ICoCoWrongArgument) ICoCo::WrongArgument;
%exceptionclass ICoCo::NotImplemented;
%rename (ICoCoNotImplemented) ICoCo::NotImplemented;

// The C++ class definition - copied over from ICoCoExceptions.hxx and augmented with __str__()
namespace ICoCo
{
class WrongContext : public std::exception
{
 public:
  WrongContext(const std::string& prob, const std::string& method, const std::string& precondition);
  ~WrongContext() throw();
  virtual const char* what() const throw();
  %extend {
    std::string __str__() const
    {
      return std::string(self->what());
    }
  }
};

class WrongArgument : public std::exception
{
 public:
  WrongArgument(const std::string& prob, const std::string& method, const std::string& arg,
                const std::string& condition);
  ~WrongArgument() throw()  { }
  virtual const char* what() const throw();
  %extend {
    std::string __str__() const
    {
      return std::string(self->what());
    }
  }
};


class NotImplemented
{
 public:
  NotImplemented(const std::string& prob, const std::string& method);
  ~NotImplemented() throw()   {}
  virtual const char* what() const throw();
  %extend {
    std::string __str__() const
    {
      return std::string(self->what());
    }
  }
 };
}  // namespace ICoCo

%exception {
  try {
    $action
  }
  catch (ICoCo::WrongContext& _e) {
    // Reraise with SWIG_Python_Raise
    SWIG_Python_Raise(SWIG_NewPointerObj((new ICoCo::WrongContext(static_cast< const ICoCo::WrongContext& >(_e))),SWIGTYPE_p_ICoCo__WrongContext,SWIG_POINTER_OWN), "ICoCo::WrongContext", SWIGTYPE_p_ICoCo__WrongContext);
    SWIG_fail;
  }
  catch (ICoCo::WrongArgument& _e) {
    SWIG_Python_Raise(SWIG_NewPointerObj((new ICoCo::WrongArgument(static_cast< const ICoCo::WrongArgument& >(_e))),SWIGTYPE_p_ICoCo__WrongArgument,SWIG_POINTER_OWN), "ICoCo::WrongArgument", SWIGTYPE_p_ICoCo__WrongArgument);
    SWIG_fail;
  }
  catch (ICoCo::NotImplemented& _e) {
    SWIG_Python_Raise(SWIG_NewPointerObj((new ICoCo::NotImplemented(static_cast< const ICoCo::NotImplemented& >(_e))),SWIGTYPE_p_ICoCo__NotImplemented,SWIG_POINTER_OWN), "ICoCo::NotImplemented", SWIGTYPE_p_ICoCo__NotImplemented);
    SWIG_fail;
  }
}

