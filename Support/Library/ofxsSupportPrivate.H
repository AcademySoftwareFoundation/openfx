#ifndef _ofxsSupportPrivate_H_
#define _ofxsSupportPrivate_H_

#include "ofxsImageEffect.H"
#include "ofxsLog.H"

/** @brief Namespace private to the ofx support library.
 */
namespace OFX {

  /** @brief turns a field string into and enum */
  FieldEnum mapStrToFieldEnum(const std::string &str)  throw(std::invalid_argument);

  namespace Private {
    /** @brief Pointer to the host */
    extern OfxHost *gHost;

    /** @brief Pointer to the effect suite */
    extern OfxImageEffectSuiteV1 *gEffectSuite;

    /** @brief Pointer to the property suite */
    extern OfxPropertySuiteV1    *gPropSuite;

    /** @brief Pointer to the  interact suite */
    extern OfxInteractSuiteV1    *gInteractSuite;

    /** @brief Pointer to the parameter suite */
    extern OfxParameterSuiteV1   *gParamSuite;

    /** @brief Pointer to the general memory suite */
    extern OfxMemorySuiteV1      *gMemorySuite;

    /** @brief Pointer to the threading suite */
    extern OfxMultiThreadSuiteV1 *gThreadSuite;

    /** @brief Pointer to the message  suite */
    extern OfxMessageSuiteV1     *gMessageSuite;

    /** @brief Support lib function called on an ofx load action */
    void loadAction(void);
  
    /** @brief Support lib function called on an ofx unload action */
    void unloadAction(void);
    
    /** @brief The plugin function that gets passed the host structure. 
     */
    void setHost(OfxHost *host);

    /** @brief The main entry point for the plugin
     */
    OfxStatus
      mainEntry(const char		*actionRaw,
		const void		*handleRaw,
		OfxPropertySetHandle	 inArgsRaw,
		OfxPropertySetHandle	 outArgsRaw);
  

  };

  /** @brief The validation code has its own namespace */
  namespace Validation {

    /** @brief This is uses to hold a property value, used by the property checking classes.

    Could have been a union, but std::string can't be in one.
    */
    struct ValueHolder {
      std::string  vString;
      int    vInt;
      double vDouble;
      void  *vPointer;
  
      ValueHolder(void) {}
      ValueHolder(char  *s) : vString(s) {}
      ValueHolder(const std::string &s) : vString(s) {}
      ValueHolder(int    i) : vInt(i) {}
      ValueHolder(double d) : vDouble(d) {}
      ValueHolder(void  *p) : vPointer(p) {}
  
      ValueHolder &operator = (char *v)  {vString = v; return *this;}
      ValueHolder &operator = (std::string v)  {vString = v; return *this;}
      ValueHolder &operator = (void *v)  {vPointer = v; return *this;}
      ValueHolder &operator = (int v)    {vInt = v; return *this;}
      ValueHolder &operator = (double v) {vDouble = v; return *this;}
  
      operator const char * () {return vString.c_str();}
      operator std::string &() {return vString;}
      operator int    &() {return vInt;}
      operator double &() {return vDouble;}
      operator void * &() {return vPointer;}
    };
    
    /** @brief Enum used in the varargs list of the PropertyDescription constructor  */
    enum DescriptionTag {
      eDescDefault,  /** @brief following values are the default to check against */
      eDescFinished  /** @brief we have finished the description */
    };

    /** @brief  class to describe properties, check their default and set their values */
    class PropertyDescription
    {
    public :
      /** @brief name of the property */
      std::string _name;
    
      /** @brief Was it validated */
      bool _exists;

      /** @brief dimension of the property */
      int      _dimension;

      /** @brief What type of property is it */
      OFX::PropertyTypeEnum _ilk;
  
      /** @brief The default value that this property should have. Empty implies no default (eg: a host name has no default). */
      std::vector<ValueHolder> _defaultValue; 
    
    public :
      /** @brief var args constructor that is use to describe properties */
      PropertyDescription(char *name, OFX::PropertyTypeEnum ilk, int dimension, ...);

      /** @brief Die! Die! Die! */
      virtual ~PropertyDescription(void) {}
  
      /** @brief See if the property exists in the containing property set and has the correct dimension */
      void validate(bool checkDefaults, PropertySet &propSet);
    };

    /** @brief Describes a set of properties */
    class PropertySetDescription {
    protected :
      /** @brief name of the property set */
      std::string           _setName;

      /** @brief the descriptions of each property */
      std::vector<PropertyDescription *> _descriptions;

      /** @brief The descriptions of each property */
      std::vector<PropertyDescription *> _deleteThese;
    
    public :
      /** @brief constructor. 

      The varargs zero terminated are made from pairs of PropertyDescription * and ints indicating the number of properties pointed to.
      These are to come from static arrays and need not be deleted
      */
      PropertySetDescription(char *setName, ...);// [PropertyDescription *v, int nSetToThese]

      /** @brief destructor */
      virtual ~PropertySetDescription();

      /** @brief add another property in */
      void addProperty(PropertyDescription *desc, bool deleteOnDestruction = true);

      /** @brief See if all properties exist and have the correct dimensions */
      void validate(PropertySet &propSet, bool checkDefaults = true, bool logOrdinaryMessages = false); 
    };

    /** @brief This validates the host struct and property handle passed back. Note that this is done by the support lib during the load action */
    void validateHostProperties(OfxHost *host);
  
    /** @brief This validates the properties passed into an action's in/out args handles */
    void validateActionArgumentsProperties(const std::string &action, OfxPropertySetHandle inArgs, OfxPropertySetHandle outArgs);

    /** @brief Validates parameter properties */
    void validateParameterProperties(ParamTypeEnum type, 
				     OFX::PropertySet paramProps,
				     bool checkDefaults);

    /** @brief Validates an image's properties */
    void validateImageProperties(OfxPropertySetHandle handle);

    /** @brief Validates an image's properties */
    void validateClipDescriptorProperties(OfxPropertySetHandle handle);

    void validatePluginDescriptorProperties(OfxPropertySetHandle handle);

    /** @brief initialises the validation code, call this in on load */
    void initialise(void);
  };

};

#endif
