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
        { kOfxPropType, Property::eString, 1, true, kOfxTypeImageEffectHost },
        { kOfxPropName, Property::eString, 1, true, "SETMEONCONSTRUCTION" },
        { kOfxPropLabel, Property::eString, 1, false, "clip" } ,
        { kOfxPropShortLabel, Property::eString, 1, false, "clip" },
        { kOfxPropLongLabel, Property::eString, 1, false, "clip" },
        
        { kOfxImageEffectPropSupportedComponents, Property::eString, 0, false, "" },

        { kOfxImageEffectPropTemporalClipAccess,   Property::eInt, 1, false, "0" },
        { kOfxImageClipPropOptional, Property::eInt, 1, false, "0" },
        { kOfxImageClipPropIsMask,   Property::eInt, 1, false, "0" },
        { kOfxImageClipPropFieldExtraction, Property::eString, 1, false, kOfxImageFieldDoubled },
        { kOfxImageEffectPropSupportsTiles,   Property::eInt, 1, false, "1" },  
        { 0 },
      };
      
      Descriptor::Descriptor(std::string name) : 
        _properties(clipDescriptorStuffs) 
      {
        _properties.setStringProperty(kOfxPropName,name);
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
          case Property::eString:
          case Property::eInt:
            _properties.setGetHook(spec.name, this);
            break;
          default:
            break;
          }

          i++;
        }
      }

      // do nothing
      int Instance::getDimension(const std::string &name) OFX_EXCEPTION_SPEC 
      {
        if(name == kOfxImageEffectPropUnmappedFrameRange || name == kOfxImageEffectPropFrameRange)
          return 2;
        return 1;
      }

      int Instance::upperGetDimension(const std::string &name) {
        return _properties.getDimension(name);
      }

      // don't know what to do
      void Instance::reset(const std::string &name) OFX_EXCEPTION_SPEC {
        printf("failing in %s\n", __PRETTY_FUNCTION__);
        throw Property::Exception(kOfxStatErrMissingHostFeature);
      }

      // get the virutals for viewport size, pixel scale, background colour
      void Instance::getDoublePropertyN(const std::string &name, double *values, int n) OFX_EXCEPTION_SPEC
      {
        if(name==kOfxImagePropPixelAspectRatio){
          if(n>1) throw Property::Exception(kOfxStatErrValue);
          *values = getAspectRatio();
        }
        else if(name==kOfxImageEffectPropFrameRate){
          if(n>1) throw Property::Exception(kOfxStatErrValue);
          *values = getFrameRate();
        }
        else if(name==kOfxImageEffectPropFrameRange){
          if(n>2) throw Property::Exception(kOfxStatErrValue);
          getFrameRange(values[0], values[1]);
        }
        else if(name==kOfxImageEffectPropUnmappedFrameRate){
          if(n>1) throw Property::Exception(kOfxStatErrValue);
          *values =  getUnmappedFrameRate();
        }
        else if(name==kOfxImageEffectPropUnmappedFrameRange){
          if(n>2) throw Property::Exception(kOfxStatErrValue);
          getUnmappedFrameRange(values[0], values[1]);
        }
        else
          throw Property::Exception(kOfxStatErrValue);
      }

      // get the virutals for viewport size, pixel scale, background colour
      double Instance::getDoubleProperty(const std::string &name, int n) OFX_EXCEPTION_SPEC
      {
        if(name==kOfxImagePropPixelAspectRatio){
          if(n!=0) throw Property::Exception(kOfxStatErrValue);
          return getAspectRatio();
        }
        else if(name==kOfxImageEffectPropFrameRate){
          if(n!=0) throw Property::Exception(kOfxStatErrValue);
          return getFrameRate();
        }
        else if(name==kOfxImageEffectPropFrameRange){
          if(n>1) throw Property::Exception(kOfxStatErrValue);
          double range[2];
          getFrameRange(range[0], range[1]);
          return range[n];
        }
        else if(name==kOfxImageEffectPropUnmappedFrameRate){
          if(n>0) throw Property::Exception(kOfxStatErrValue);
          return getUnmappedFrameRate();
        }
        else if(name==kOfxImageEffectPropUnmappedFrameRange){
          if(n>1) throw Property::Exception(kOfxStatErrValue);
          double range[2];
          getUnmappedFrameRange(range[0], range[1]);
          return range[n];
        }
        else
          throw Property::Exception(kOfxStatErrValue);
      }

      // get the virutals for viewport size, pixel scale, background colour
      int Instance::getIntProperty(const std::string &name, int n) OFX_EXCEPTION_SPEC
      {
        if(n!=0) throw Property::Exception(kOfxStatErrValue);
        if(name==kOfxImageClipPropConnected){
          return getConnected();
        }
        else if(name==kOfxImageClipPropContinuousSamples){
          return getContinuousSamples();
        }
        else
          throw Property::Exception(kOfxStatErrValue);
      }

      // get the virutals for viewport size, pixel scale, background colour
      void Instance::getIntPropertyN(const std::string &name, int *values, int n) OFX_EXCEPTION_SPEC
      {
        if(n!=0) throw Property::Exception(kOfxStatErrValue);
        *values = getIntProperty(name, 0);
      }

      // get the virutals for viewport size, pixel scale, background colour
      const std::string &Instance::getStringProperty(const std::string &name, int n) OFX_EXCEPTION_SPEC
      {
        if(n!=0) throw Property::Exception(kOfxStatErrValue);
        if(name==kOfxImageEffectPropPixelDepth){
          return getPixelDepth();
        }
        else if(name==kOfxImageEffectPropComponents){
          return getComponents();
        }
        else if(name==kOfxImageClipPropUnmappedPixelDepth){
          return getUnmappedBitDepth();
        }
        else if(name==kOfxImageClipPropUnmappedComponents){
          return getUnmappedComponents();
        }
        else if(name==kOfxImageEffectPropPreMultiplication){
          return getPremult();
        }
        else if(name==kOfxImageClipPropFieldOrder){
          return getFieldOrder();
        }
        else
          throw Property::Exception(kOfxStatErrValue);
      }

      const std::string &Instance::getName()
      {
        return _properties.getStringProperty(kOfxPropName);
      }

      const std::string &Instance::getLabel()
      {
        const std::string &s = _properties.getStringProperty(kOfxPropLabel);
        if(s == "") {
          return _properties.getStringProperty(kOfxPropName);
        }
        return s;
      }

      // notify override properties
      void Instance::notify(const std::string &name, bool isSingle, int indexOrN)  OFX_EXCEPTION_SPEC
      {
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
        inArgs.setDoubleProperty(kOfxPropTime,time);
        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 0);
        inArgs.setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleY, 1);

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
        { kOfxImagePropField, Property::eString, 1, true, "", },
        { kOfxImagePropUniqueIdentifier, Property::eString, 1, true, "" },
        { 0 }
      };

      Image::Image() : Property::Set(imageStuffs) {}      

      // construction based on clip instance
      Image::Image(Clip::Instance& instance,
                   double renderScaleX, 
                   double renderScaleY,
                   void* data,
                   int bx1, int by1, int bx2, int by2,
                   int rodx1, int rody1, int rodx2, int rody2,
                   int rowBytes,
                   std::string field,
                   std::string uniqueIdentifier) : Property::Set(imageStuffs)
      {
        Property::Set& clipProperties = instance.getProps();
        
        // get and set the clip instance pixel depth
        setStringProperty(kOfxImageEffectPropPixelDepth, clipProperties.getStringProperty(kOfxImageEffectPropPixelDepth));
        
        // get and set the clip instance components
        setStringProperty(kOfxImageEffectPropComponents, clipProperties.getStringProperty(kOfxImageEffectPropComponents));
        
        // get and set the clip instance premultiplication
        setStringProperty(kOfxImageEffectPropPreMultiplication, clipProperties.getStringProperty(kOfxImageEffectPropPreMultiplication));

        // get and set the clip instance pixel aspect ratio
        setDoubleProperty(kOfxImagePropPixelAspectRatio, clipProperties.getDoubleProperty(kOfxImagePropPixelAspectRatio));

        // set other data
        setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 1);    
        setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 2);        
        setPointerProperty(kOfxImagePropData,data);
        setIntProperty(kOfxImagePropBounds,bx1, 0);
        setIntProperty(kOfxImagePropBounds,by1, 1);
        setIntProperty(kOfxImagePropBounds,bx2, 2);
        setIntProperty(kOfxImagePropBounds,by2, 3);
        setIntProperty(kOfxImagePropRegionOfDefinition,rodx1, 0);
        setIntProperty(kOfxImagePropRegionOfDefinition,rody1, 1);
        setIntProperty(kOfxImagePropRegionOfDefinition,rodx2, 2);
        setIntProperty(kOfxImagePropRegionOfDefinition,rody2, 3);        
        setIntProperty(kOfxImagePropRowBytes,rowBytes);
        
        setStringProperty(kOfxImagePropField,field);
        setStringProperty(kOfxImageClipPropFieldOrder,field);
        setStringProperty(kOfxImagePropUniqueIdentifier,uniqueIdentifier);
      }

      Image::~Image() {
      }

    } // Clip

  } // Host

} // OFX
