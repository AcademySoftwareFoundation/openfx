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

      /// properties common to the desciptor and instance
      /// the desc and set them, the instance cannot
      static Property::PropSpec clipDescriptorStuffs[] = {
        { kOfxPropType, Property::eString, 1, true, kOfxTypeClip },
        { kOfxPropName, Property::eString, 1, true, "SET ME ON CONSTRUCTION" },
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
      
      ////////////////////////////////////////////////////////////////////////////////
      // props to clips descriptors and instances

      // base ctor, for a descriptor
      CommonProps::CommonProps()
        : _properties(clipDescriptorStuffs) 
      {
      }

      /// props to clips and 
      CommonProps::CommonProps(const CommonProps &v)
        : _properties(v._properties) 
      {
        /// we are an instance, we need to reset the props to read only
        const Property::PropertyMap &map = _properties.getProperties();
        Property::PropertyMap::const_iterator i;
        for(i = map.begin(); i != map.end(); ++i) {
          (*i).second->setPluginReadOnly(false);
        } 
      }

      /// name of the clip
      const std::string &CommonProps::getShortLabel() const
      {
        const std::string &s = _properties.getStringProperty(kOfxPropShortLabel);
        if(s == "") {
          const std::string &s2 = _properties.getStringProperty(kOfxPropLabel);
          if(s2 == "") {
            return _properties.getStringProperty(kOfxPropName);
          }
        }
        return s;
      }
      
      /// name of the clip
      const std::string &CommonProps::getLabel() const
      {
        const std::string &s = _properties.getStringProperty(kOfxPropShortLabel);
        if(s == "") {
          return _properties.getStringProperty(kOfxPropName);
        }
        return s;
      }
      
      /// name of the clip
      const std::string &CommonProps::getLongLabel() const
      {
        const std::string &s = _properties.getStringProperty(kOfxPropLongLabel);
        if(s == "") {
          const std::string &s2 = _properties.getStringProperty(kOfxPropLabel);
          if(s2 == "") {
            return _properties.getStringProperty(kOfxPropName);
          }
        }
        return s;
      }
      
      /// return a std::vector of supported comp
      const std::vector<std::string> &CommonProps::getSupportedComponents() const
      {
        Property::String *p;
        assert(_properties.fetchProperty(kOfxImageEffectPropSupportedComponents, p) == kOfxStatOK);
        return p->getValues();
      }
      
      /// is the given component supported
      bool CommonProps::isSupportedComponent(const std::string &comp) const
      {
        return _properties.findStringPropValueIndex(kOfxImageEffectPropSupportedComponents, comp) != -1;
      }
      
      /// does the clip do random temporal access
      bool CommonProps::temporalAccess() const
      {
        return _properties.getIntProperty(kOfxImageEffectPropTemporalClipAccess);
      }
      
      /// is the clip optional
      bool CommonProps::isOptional() const
      {
        return _properties.getIntProperty(kOfxImageClipPropOptional);
      }
      
      /// is the clip a nominal 'mask' clip
      bool CommonProps::isMask() const
      {
        return _properties.getIntProperty(kOfxImageClipPropIsMask);
      }
      
      /// how does this clip like fielded images to be presented to it
      const std::string &CommonProps::getFieldExtraction()
      {
        return _properties.getStringProperty(kOfxImageClipPropFieldExtraction);
      }
      
      /// is the clip a nominal 'mask' clip
      bool CommonProps::supportsTiles() const
      {
        return _properties.getIntProperty(kOfxImageEffectPropSupportsTiles);
      }

      Property::Set& CommonProps::getProps() 
      {
        return _properties;
      }

      /// get a handle on the properties of the clip descriptor for the C api
      OfxPropertySetHandle CommonProps::getPropHandle() 
      {
        return _properties.getHandle();
      }

      ////////////////////////////////////////////////////////////////////////////////
      /// descriptor
      Descriptor::Descriptor(std::string name) 
        : CommonProps()
      {
        _properties.setStringProperty(kOfxPropName,name);
      }

      /// get a handle on the clip descriptor for the C api
      OfxImageClipHandle Descriptor::getHandle() 
      {
        return (OfxImageClipHandle)this;
      }
      
      /// extra properties for the instance, these are fetched from the host
      /// via a get hook and some virtuals
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

      Instance::Instance(Descriptor& desc, ImageEffect::Instance* effectInstance) 
        : CommonProps(desc)
        , _effectInstance(effectInstance)
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

      /// get a handle on the clip descriptor for the C api
      OfxImageClipHandle Instance::getHandle() 
      {
        return (OfxImageClipHandle)this;
      }
      
      // do nothing
      int Instance::getDimension(const std::string &name) OFX_EXCEPTION_SPEC 
      {
        if(name == kOfxImageEffectPropUnmappedFrameRange || name == kOfxImageEffectPropFrameRange)
          return 2;
        return 1;
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
          return _effectInstance->mainEntry(kOfxActionBeginInstanceChanged, _effectInstance->getHandle(),inArgs.getHandle(),0);
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

      Image::Image()
        : Property::Set(imageStuffs) 
        , _referenceCount(1)
      {
      }

      /// called during ctor to get bits from the clip props into ours
      void Image::getClipBits(Clip::Instance& instance)
      {
        Property::Set& clipProperties = instance.getProps();
        
        // get and set the clip instance pixel depth
        const std::string &depth = clipProperties.getStringProperty(kOfxImageEffectPropPixelDepth);
        setStringProperty(kOfxImageEffectPropPixelDepth, depth); 
        
        // get and set the clip instance components
        const std::string &comps = clipProperties.getStringProperty(kOfxImageEffectPropComponents);
        setStringProperty(kOfxImageEffectPropComponents, comps);
        
        // get and set the clip instance premultiplication
        setStringProperty(kOfxImageEffectPropPreMultiplication, clipProperties.getStringProperty(kOfxImageEffectPropPreMultiplication));

        // get and set the clip instance pixel aspect ratio
        setDoubleProperty(kOfxImagePropPixelAspectRatio, clipProperties.getDoubleProperty(kOfxImagePropPixelAspectRatio));
        
        // get and set the clip instance premultiplication
        setStringProperty(kOfxImageEffectPropPreMultiplication, clipProperties.getStringProperty(kOfxImageEffectPropPreMultiplication));

        // get and set the clip instance pixel aspect ratio
        setDoubleProperty(kOfxImagePropPixelAspectRatio, clipProperties.getDoubleProperty(kOfxImagePropPixelAspectRatio));
      }

      /// make an image from a clip instance
      Image::Image(Clip::Instance& instance)
        : Property::Set(imageStuffs) 
        , _referenceCount(1)
      {
        getClipBits(instance);
      }      

      // construction based on clip instance
      Image::Image(Clip::Instance& instance,
                   double renderScaleX, 
                   double renderScaleY,
                   void* data,
                   const OfxRectI &bounds,
                   const OfxRectI &rod,
                   int rowBytes,
                   std::string field,
                   std::string uniqueIdentifier) 
        : Property::Set(imageStuffs)
        , _referenceCount(1)
      {
        getClipBits(instance);

        // set other data
        setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleX, 0);    
        setDoubleProperty(kOfxImageEffectPropRenderScale,renderScaleY, 1);        
        setPointerProperty(kOfxImagePropData,data);
        setIntProperty(kOfxImagePropBounds,bounds.x1, 0);
        setIntProperty(kOfxImagePropBounds,bounds.y1, 1);
        setIntProperty(kOfxImagePropBounds,bounds.x2, 2);
        setIntProperty(kOfxImagePropBounds,bounds.y2, 3);
        setIntProperty(kOfxImagePropRegionOfDefinition,rod.x1, 0);
        setIntProperty(kOfxImagePropRegionOfDefinition,rod.y1, 1);
        setIntProperty(kOfxImagePropRegionOfDefinition,rod.x2, 2);
        setIntProperty(kOfxImagePropRegionOfDefinition,rod.y2, 3);        
        setIntProperty(kOfxImagePropRowBytes,rowBytes);
        
        setStringProperty(kOfxImagePropField,field);
        setStringProperty(kOfxImageClipPropFieldOrder,field);
        setStringProperty(kOfxImagePropUniqueIdentifier,uniqueIdentifier);
      }

      Image::~Image() {
      }

      // release the reference 
      void Image::releaseReference()
      {
        _referenceCount -= 1;
        if(_referenceCount <= 0)
          delete this;
      }

    } // Clip

  } // Host

} // OFX
