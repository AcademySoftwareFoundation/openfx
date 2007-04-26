// ofx
#include "ofxCore.h"
#include "ofxImageEffect.h"

// ofx host
#include "ofxhBinary.h"
#include "ofxhPropertySuite.h"
#include "ofxhClip.h"
#include "ofxhImageEffect.h"

namespace OFX {

  namespace Host {

    namespace Clip {

      //
      // Descriptor
      //

      static Property::PropSpec clipDescriptorStuffs[] = {
        { kOfxPropType, Property::eString, 1, false, kOfxTypeImageEffectHost },
        { kOfxPropName, Property::eString, 1, false, "uk.co.uk.thefoundry.Clip.Descriptor" },
        { kOfxPropLabel, Property::eString, 1, false, "clip" } ,
        { kOfxPropShortLabel, Property::eString, 1, false, "clip" },
        { kOfxPropLongLabel, Property::eString, 1, false, "clip" },
        
        { kOfxImageEffectPropSupportedComponents, Property::eString, 0, true, "" },
        /// xxx ??

        { kOfxImageEffectPropTemporalClipAccess,   Property::eInt, 1, false, "0" },
        { kOfxImageClipPropOptional, Property::eInt, 1, false, "0" },
        { kOfxImageClipPropIsMask,   Property::eInt, 1, false, "0" },
        { kOfxImageClipPropFieldExtraction, Property::eString, 1, false, kOfxImageFieldDoubled },
        { kOfxImageEffectPropSupportsTiles,   Property::eInt, 1, false, "1" },  
        { 0 },
      };
      
      Descriptor::Descriptor() : _properties(clipDescriptorStuffs) {
      }

      /// get a handle on the clip descriptor for the C api
      OfxImageClipHandle Descriptor::getHandle() {
        return (OfxImageClipHandle)this;
      }

      /// get a handle on the properties of the clip descriptor for the C api
      OfxPropertySetHandle Descriptor::getPropHandle() {
        return _properties.getHandle();
      }

      Property::Set& Descriptor::getProps() {
        return _properties;
      }

      //
      // Instance
      //

      OfxImageClipHandle Instance::getHandle() {
        return (OfxImageClipHandle)this;
      }

      /// get a handle on the properties of the clip descriptor for the C api
      OfxPropertySetHandle Instance::getPropHandle() {
        return _properties.getHandle();
      }

      Property::Set& Instance::getProps() {
        return _properties;
      }

      static Property::PropSpec clipInstanceStuffs[] = { 
        { kOfxImageEffectPropPixelDepth, Property::eString, 1, true, kOfxBitDepthNone },
        { kOfxImageEffectPropComponents, Property::eString, 1, true, kOfxImageComponentNone },
        { kOfxImageClipPropUnmappedPixelDepth, Property::eString, 1, true, kOfxBitDepthNone },
        { kOfxImageClipPropUnmappedComponents, Property::eString, 1, true, kOfxImageComponentNone },
        { kOfxImageEffectPropPreMultiplication, Property::eString, 1, true, kOfxImageOpaque },
        { kOfxImagePropPixelAspectRatio, Property::eDouble, 1, true, "1.0" },
        { kOfxImageEffectPropFrameRate, Property::eDouble, 1, true, "25.0" },
        { kOfxImageEffectPropFrameRange, Property::eDouble, 2, true, "0" },
        { kOfxImageClipPropFieldOrder, Property::eString, 1, true, kOfxImageFieldNone },
        { kOfxImageClipPropConnected, Property::eInt, 1, true, "0" },
        { kOfxImageEffectPropUnmappedFrameRange, Property::eDouble, 2, true, "0" },
        { kOfxImageEffectPropUnmappedFrameRate, Property::eDouble, 1, true, "25.0" },
        { kOfxImageClipPropContinuousSamples, Property::eInt, 1, true, "0" },
        { 0 },
      };

      Instance::Instance(Descriptor& desc, ImageEffect::Instance* effectInstance) : 
        _properties(desc.getProps()), _effectInstance(effectInstance)
      {
        // this will a parameters that are needed in an instance but not a 
        // Descriptor
        _properties.addProperties(clipInstanceStuffs);
        int i = 0;
        while(clipInstanceStuffs[i].name) {
          Property::PropSpec& spec = clipInstanceStuffs[i];

          switch (spec.type) {
          case Property::eDouble:
            _properties.setGetHook<Property::DoubleValue>(spec.name, this, this);
            break;
          case Property::eString:
            _properties.setGetHook<Property::StringValue>(spec.name, this, this);
            break;
          case Property::eInt:
            _properties.setGetHook<Property::IntValue>(spec.name, this, this);
            break;
          default:
            break;
          }

          i++;
        }
      }

      // do nothing
      int Instance::getDimension(const std::string &name) OFX_EXCEPTION_SPEC {
        throw Property::Exception(kOfxStatErrMissingHostFeature);
      }

      void Instance::setProperty(const std::string &name, double value, int index) OFX_EXCEPTION_SPEC { 
        throw Property::Exception(kOfxStatErrMissingHostFeature); 
      }

      void Instance::setPropertyN(const std::string &name, double *first, int n) OFX_EXCEPTION_SPEC { 
        throw Property::Exception(kOfxStatErrMissingHostFeature); 
      }

      void Instance::setProperty(const std::string &name, int value, int index) OFX_EXCEPTION_SPEC { 
        throw Property::Exception(kOfxStatErrMissingHostFeature); 
      }

      void Instance::setPropertyN(const std::string &name, int *first, int n) OFX_EXCEPTION_SPEC { 
        throw Property::Exception(kOfxStatErrMissingHostFeature); 
      }

      void Instance::setProperty(const std::string &name, const char* value, int index) OFX_EXCEPTION_SPEC { 
        throw Property::Exception(kOfxStatErrMissingHostFeature); 
      }

      void Instance::setPropertyN(const std::string &name, const char** first, int n) OFX_EXCEPTION_SPEC { 
        throw Property::Exception(kOfxStatErrMissingHostFeature); 
      }

      // don't know what to do
      void Instance::reset(const std::string &name) OFX_EXCEPTION_SPEC {
        throw Property::Exception(kOfxStatErrMissingHostFeature);
      }

      // get the virutals for viewport size, pixel scale, background colour
      void Instance::getProperty(const std::string &name, double &ret, int index) OFX_EXCEPTION_SPEC
      {
        int max = getDimension(name);        
        if(index>=max) throw Property::Exception(kOfxStatErrValue);

        double* values = new double[max];
        getPropertyN(name,values,max);
        ret = values[index];

        delete [] values;
      }

      // get the virutals for viewport size, pixel scale, background colour
      void Instance::getProperty(const std::string &name, int &ret, int index) OFX_EXCEPTION_SPEC
      {
        int max = getDimension(name);        
        if(index>=max) throw Property::Exception(kOfxStatErrValue);

        int* values = new int[max];
        getPropertyN(name,values,max);
        ret = values[index];

        delete [] values;
      }

      // get the virutals for viewport size, pixel scale, background colour
      void Instance::getProperty(const std::string &name, const char* &ret, int index) OFX_EXCEPTION_SPEC
      {
        int max = getDimension(name);        
        if(index>=max) throw Property::Exception(kOfxStatErrValue);

        char** values = new char*[max];
        getPropertyN(name,(const char**)values,max);
        ret = values[index];

        delete [] values;
      }

      void Instance::getPropertyN(const std::string &name, double* first, int n) OFX_EXCEPTION_SPEC
      {
        int max = getDimension(name);        
        if(n>max) throw Property::Exception(kOfxStatErrValue);

        OfxStatus st = kOfxStatOK;

        if(name==kOfxImagePropPixelAspectRatio){
          if(n>1) throw Property::Exception(kOfxStatErrValue);
          st = getAspectRatio(first[0]);
        }
        else if(name==kOfxImageEffectPropFrameRate){
          if(n>1) throw Property::Exception(kOfxStatErrValue);
          st = getFrameRate(first[0]);
        }
        else if(name==kOfxImageEffectPropFrameRange){
          if(n>2) throw Property::Exception(kOfxStatErrValue);
          st = getFrameRange(first[0],first[1]);
        }
        else if(name==kOfxImageEffectPropUnmappedFrameRate){
          if(n>1) throw Property::Exception(kOfxStatErrValue);
          st = getUnmappedFrameRate(first[0]);
        }
        else if(name==kOfxImageEffectPropUnmappedFrameRange){
          if(n>2) throw Property::Exception(kOfxStatErrValue);
          st = getUnmappedFrameRange(first[0],first[1]);
        }
        else
          throw Property::Exception(kOfxStatErrValue);

        if(st!=kOfxStatOK) throw Property::Exception(st);
      }

      void Instance::getPropertyN(const std::string &name, int* first, int n) OFX_EXCEPTION_SPEC
      {
        int max = getDimension(name);        
        if(n>max) throw Property::Exception(kOfxStatErrValue);

        OfxStatus st = kOfxStatOK;

        if(name==kOfxImageClipPropConnected){
          if(n>1) throw Property::Exception(kOfxStatErrValue);
          st = getConnected(first[0]);
        }
        else if(name==kOfxImageClipPropContinuousSamples){
          if(n>1) throw Property::Exception(kOfxStatErrValue);
          st = getContinuousSamples(first[0]);
        }
        else
          throw Property::Exception(kOfxStatErrValue);

        if(st!=kOfxStatOK) throw Property::Exception(st);
      }

      void Instance::getPropertyN(const std::string &name, const char** first, int n) OFX_EXCEPTION_SPEC
      {
        int max = getDimension(name);        
        if(n>max) throw Property::Exception(kOfxStatErrValue);

        OfxStatus st = kOfxStatOK;

        if(name==kOfxImageEffectPropPixelDepth){
          if(n>1) throw Property::Exception(kOfxStatErrValue);

          std::string pixelDepth;

          st = getPixelDepth(pixelDepth);
          if(st!=kOfxStatOK) throw Property::Exception(st);

          first[0] = new char[pixelDepth.size()+1];
          strcpy((char*)first[0],pixelDepth.c_str());
        }
        else if(name==kOfxImageEffectPropComponents){
          if(n>1) throw Property::Exception(kOfxStatErrValue);

          std::string components;

          st = getComponents(components);
          if(st!=kOfxStatOK) throw Property::Exception(st);

          first[0] = new char[components.size()+1];
          strcpy((char*)first[0],components.c_str());
        }
        else if(name==kOfxImageClipPropUnmappedPixelDepth){
          if(n>1) throw Property::Exception(kOfxStatErrValue);

          std::string unmappedBitDepth;

          st = getUnmappedBitDepth(unmappedBitDepth);
          if(st!=kOfxStatOK) throw Property::Exception(st);

          first[0] = new char[unmappedBitDepth.size()+1];
          strcpy((char*)first[0],unmappedBitDepth.c_str());
        }
        else if(name==kOfxImageClipPropUnmappedComponents){
          if(n>1) throw Property::Exception(kOfxStatErrValue);

          std::string unmappedComponents;

          st = getUnmappedComponents(unmappedComponents);
          if(st!=kOfxStatOK) throw Property::Exception(st);

          first[0] = new char[unmappedComponents.size()+1];
          strcpy((char*)first[0],unmappedComponents.c_str());
        }
        else if(name==kOfxImageEffectPropPreMultiplication){
          if(n>1) throw Property::Exception(kOfxStatErrValue);

          std::string premult;

          st = getPremult(premult);
          if(st!=kOfxStatOK) throw Property::Exception(st);

          first[0] = new char[premult.size()+1];
          strcpy((char*)first[0],premult.c_str());
        }
        else if(name==kOfxImageClipPropFieldOrder){
          if(n>1) throw Property::Exception(kOfxStatErrValue);

          std::string field;

          st = getField(field);
          if(st!=kOfxStatOK) throw Property::Exception(st);

          first[0] = new char[field.size()+1];
          strcpy((char*)first[0],field.c_str());
        }
        else
          throw Property::Exception(kOfxStatErrValue);

        if(st!=kOfxStatOK) throw Property::Exception(st);
      }

      std::string Instance::getName()
      {
        return _properties.getProperty<Property::StringValue>(kOfxPropName,0);
      }

      OfxStatus Instance::instanceChangedAction(std::string why,
                                     OfxTime     time,
                                     double      renderScaleX,
                                     double      renderScaleY)
      {
        Property::PropSpec stuff[] = {
          { kOfxPropType, Property::eString, 1, true, kOfxTypeClip },
          { kOfxPropName, Property::eString, 1, true, getName().c_str() },
          { kOfxPropChangeReason, Property::eString, 1, true, why.c_str() },
          { kOfxPropTime, Property::eDouble, 1, true, "0" },
          { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "0" },
          { 0 }
        };

        Property::Set inArgs(stuff);

        // add the second dimension of the render scale
        inArgs.setProperty<Property::DoubleValue>(kOfxPropTime,0,time);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);
        inArgs.setProperty<Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);

        if(_effectInstance){
          return _effectInstance->mainEntry(kOfxActionBeginInstanceChanged,this->getHandle(),inArgs.getHandle(),0);
        }

        return kOfxStatFailed;
     }


      //
      // Image
      //

      static Property::PropSpec imageStuffs[] = {
        { kOfxPropType, Property::eString, 1, false, kOfxTypeImage },
        { kOfxImageEffectPropPixelDepth, Property::eString, 1, true, kOfxBitDepthNone  },
        { kOfxImageEffectPropComponents, Property::eString, 1, true, kOfxImageComponentNone },
        { kOfxImageEffectPropPreMultiplication, Property::eString, 1, true, kOfxImageOpaque  },
        { kOfxImageEffectPropRenderScale, Property::eDouble, 2, true, "1.0" },
        { kOfxImagePropPixelAspectRatio, Property::eDouble, 1, true, "1.0"  },
        { kOfxImagePropData, Property::ePointer, 1, true, "0" },
        { kOfxImagePropBounds, Property::eInt, 4, true, "0" },
        { kOfxImagePropRegionOfDefinition, Property::eInt, 4, true, "0", },
        { kOfxImagePropRowBytes, Property::eInt, 1, true, "0", },
        { kOfxImagePropField, Property::eInt, 1, true, "0", },
        { kOfxImagePropUniqueIdentifier, Property::eString, 1, true, "" },
        { 0 }
      };

      Image::Image() : _properties(imageStuffs) {}      

      // construction based on clip instance
      Image::Image(Clip::Instance& instance,
                  double renderScaleX, 
                  double renderScaleY,
                  void* data,
                  int bx1, int by1, int bx2, int by2,
                  int rodx1, int rody1, int rodx2, int rody2,
                  int rowBytes,
                  std::string field,
                  std::string uniqueIdentifier) : _properties(imageStuffs)
      {
        Property::Set& clipProperties = instance.getProps();
        
        // get and set the clip instance pixel depth
        std::string pixelDepth = clipProperties.getProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropPixelDepth,0);
        _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropPixelDepth,0,pixelDepth.c_str());
        
        // get and set the clip instance components
        std::string components = clipProperties.getProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropComponents,0);
        _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropComponents,0,components.c_str());
        
        // get and set the clip instance premultiplication
        std::string premultiplication = clipProperties.getProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropPreMultiplication,0);
        _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImageEffectPropPreMultiplication,0,premultiplication.c_str());

        // get and set the clip instance pixel aspect ratio
        double aspectRatio = clipProperties.getProperty<OFX::Host::Property::DoubleValue>(kOfxImagePropPixelAspectRatio,0);
        _properties.setProperty<OFX::Host::Property::DoubleValue>(kOfxImagePropPixelAspectRatio,0,aspectRatio);        

        // set other data
        _properties.setProperty<OFX::Host::Property::DoubleValue>(kOfxImageEffectPropRenderScale,0,renderScaleX);        
        _properties.setProperty<OFX::Host::Property::DoubleValue>(kOfxImageEffectPropRenderScale,1,renderScaleY);        
        _properties.setProperty<OFX::Host::Property::PointerValue>(kOfxImagePropData,0,data);
        _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImagePropBounds,0,bx1);
        _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImagePropBounds,1,by1);
        _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImagePropBounds,2,bx2);
        _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImagePropBounds,3,by2);
        _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImagePropRegionOfDefinition,0,rodx1);
        _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImagePropRegionOfDefinition,1,rody1);
        _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImagePropRegionOfDefinition,2,rodx2);
        _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImagePropRegionOfDefinition,3,rody2);        
        _properties.setProperty<OFX::Host::Property::IntValue>(kOfxImagePropRowBytes,0,rowBytes);
        _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImageClipPropFieldOrder,0,field.c_str());
        _properties.setProperty<OFX::Host::Property::StringValue>(kOfxImagePropUniqueIdentifier,0,uniqueIdentifier.c_str());
      }

      Image::~Image() {
      }

      OfxImageClipHandle Image::getHandle() {
        return (OfxImageClipHandle)this;
      }

    } // Clip

  } // Host

} // OFX
