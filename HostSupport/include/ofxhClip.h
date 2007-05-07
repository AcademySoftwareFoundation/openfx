#ifndef OFX_CLIP_H
#define OFX_CLIP_H

namespace OFX {

  namespace Host {


    namespace ImageEffect {
      class Instance;
    }

    namespace Clip {
      // forward declarations
      class Image;

      /// Base to both descriptor and instance it 
      /// is used to basically fetch common properties 
      /// by function name
      class CommonProps {
      protected :
        Property::Set _properties;   

      public :
        /// base ctor, for a descriptor
        CommonProps();

        /// ctor, when copy constructing an instance from a descripto
        explicit CommonProps(const CommonProps &other);

        /// name of the clip
        const std::string &getName() const
        {
          return _properties.getStringProperty(kOfxPropName);
        }

        /// name of the clip
        const std::string &getShortLabel() const;
        
        /// name of the clip
        const std::string &getLabel() const;
        
        /// name of the clip
        const std::string &getLongLabel() const;

        /// return a std::vector of supported comp
        const std::vector<std::string> &getSupportedComponents() const;
        
        /// is the given component supported
        bool isSupportedComponent(const std::string &comp) const;

        /// does the clip do random temporal access
        bool temporalAccess() const;

        /// is the clip optional
        bool isOptional() const;

        /// is the clip a nominal 'mask' clip
        bool isMask() const;
        
        /// how does this clip like fielded images to be presented to it
        const std::string &getFieldExtraction();

        /// is the clip a nominal 'mask' clip
        bool supportsTiles() const;

        // get props
        Property::Set &getProps();

        /// get a handle on the properties of the clip descriptor for the C api
        OfxPropertySetHandle getPropHandle();
      };
      
      /// a clip descriptor
      class Descriptor : public CommonProps {
      public:
        /// constructor
        Descriptor(std::string name);
        
        /// get a handle on the clip descriptor for the C api
        OfxImageClipHandle getHandle();
                
        /// get the name property
        const std::string &getName()
        {
          return _properties.getStringProperty(kOfxPropName);
        }

      };

      /// a clip instance
      class Instance : public CommonProps 
                     , private Property::GetHook
                     , private Property::NotifyHook {
      protected:
        ImageEffect::Instance*  _effectInstance;

      public:
        Instance(Descriptor& desc, ImageEffect::Instance* effectInstance = 0);

        /// get a handle on the clip descriptor for the C api
        OfxImageClipHandle getHandle();
        
        /// notify override properties
        virtual void notify(const std::string &name, bool isSingle, int indexOrN)  OFX_EXCEPTION_SPEC;
        
        /// get hook override
        virtual void reset(const std::string &name) OFX_EXCEPTION_SPEC;

        // get the virutals for viewport size, pixel scale, background colour
        virtual double getDoubleProperty(const std::string &name, int index) OFX_EXCEPTION_SPEC;

        // get the virutals for viewport size, pixel scale, background colour
        virtual void getDoublePropertyN(const std::string &name, double *values, int count) OFX_EXCEPTION_SPEC;

        // get the virutals for viewport size, pixel scale, background colour
        virtual int getIntProperty(const std::string &name, int index) OFX_EXCEPTION_SPEC;

        // get the virutals for viewport size, pixel scale, background colour
        virtual void getIntPropertyN(const std::string &name, int *values, int count) OFX_EXCEPTION_SPEC;

        // get the virutals for viewport size, pixel scale, background colour
        virtual const std::string &getStringProperty(const std::string &name, int index) OFX_EXCEPTION_SPEC;

        // do nothing
        virtual int  getDimension(const std::string &name) OFX_EXCEPTION_SPEC;

        // instance changed action
        OfxStatus instanceChangedAction(std::string why,
                                        OfxTime     time,
                                        double      renderScaleX,
                                        double      renderScaleY);

        // properties of an instance that are live

        // Pixel Depth -
        //
        //  kOfxBitDepthNone (implying a clip is unconnected, not valid for an image)
        //  kOfxBitDepthByte
        //  kOfxBitDepthShort
        //  kOfxBitDepthFloat
        virtual const std::string &getPixelDepth() = 0;

        // Components -
        //
        //  kOfxImageComponentNone (implying a clip is unconnected, not valid for an image)
        //  kOfxImageComponentRGBA
        //  kOfxImageComponentAlpha
        virtual const std::string &getComponents() = 0;

        // Unmapped Pixel Depth -
        //
        //  kOfxBitDepthNone (implying a clip is unconnected image)
        //  kOfxBitDepthByte
        //  kOfxBitDepthShort
        //  kOfxBitDepthFloat
        virtual const std::string &getUnmappedBitDepth() = 0;

        // Unmapped Components -
        //
        //  kOfxImageComponentNone (implying a clip is unconnected, not valid for an image)
        //  kOfxImageComponentRGBA
        //  kOfxImageComponentAlpha
        virtual const std::string &getUnmappedComponents() = 0;

        // PreMultiplication -
        //
        //  kOfxImageOpaque - the image is opaque and so has no premultiplication state
        //  kOfxImagePreMultiplied - the image is premultiplied by it's alpha
        //  kOfxImageUnPreMultiplied - the image is unpremultiplied
        virtual const std::string &getPremult() = 0;

        // Pixel Aspect Ratio -
        //
        //  The pixel aspect ratio of a clip or image.
        virtual double getAspectRatio() = 0;

        // Frame Rate -
        //
        //  The frame rate of a clip or instance's project.
        virtual double getFrameRate() = 0;

        // Frame Range (startFrame, endFrame) -
        //
        //  The frame range over which a clip has images.
        virtual void getFrameRange(double &startFrame, double &endFrame) = 0;

        /// Field Order - Which spatial field occurs temporally first in a frame.
        /// \returns 
        ///  - kOfxImageFieldNone - the clip material is unfielded
        ///  - kOfxImageFieldLower - the clip material is fielded, with image rows 0,2,4.... occuring first in a frame
        ///  - kOfxImageFieldUpper - the clip material is fielded, with image rows line 1,3,5.... occuring first in a frame
        virtual const std::string &getFieldOrder() = 0;
        
        // Connected -
        //
        //  Says whether the clip is actually connected at the moment.
        virtual bool getConnected() = 0;

        // Unmapped Frame Rate -
        //
        //  The unmaped frame range over which an output clip has images.
        virtual double getUnmappedFrameRate() = 0;

        // Unmapped Frame Range -
        //
        //  The unmaped frame range over which an output clip has images.
        virtual void getUnmappedFrameRange(double &unmappedStartFrame, double &unmappedEndFrame) = 0;

        // Continuous Samples -
        //
        //  0 if the images can only be sampled at discreet times (eg: the clip is a sequence of frames),
        //  1 if the images can only be sampled continuously (eg: the clip is infact an animating roto spline and can be rendered anywhen). 
        virtual bool getContinuousSamples() = 0;

        /// override this to fill in the image at the given time.
        /// The bounds of the image on the image plane should be 
        /// 'appropriate', typically the value returned in getRegionsOfInterest
        /// on the effect instance. Outside a render call, the optionalBounds should
        /// be 'appropriate' for the.
        /// If bounds is not null, fetch the indicated section of the canonical image plane.
        virtual Clip::Image* getImage(OfxTime time, OfxRectD *optionalBounds) = 0;

        /// override this to return the rod on the clip
        virtual OfxRectD getRegionOfDefinition(OfxTime time) = 0;
      };

      // instance of an image class
      class Image : public Property::Set {
      protected :
        /// called during ctors to get bits from the clip props into ours
        void getClipBits(Clip::Instance& instance);
        int _referenceCount; ///< reference count on this image

      public:
        // default constructor
        virtual ~Image();
        
        /// basic ctor, makes empty property set but sets not value
        Image();

        /// construct from a clip instance, but leave the
        /// filling it to the calling code via the propery set
        explicit Image(Clip::Instance& instance); 

        // Render Scale (renderScaleX,renderScaleY) -
        //
        // The proxy render scale currently being applied.
        // ------
        // Data -
        //
        // The pixel data pointer of an image.
        // ------
        // Bounds (bx1,by1,bx2,by2) -
        //
        // The bounds of an image's pixels. The bounds, in PixelCoordinates, are of the 
        // addressable pixels in an image's data pointer. The order of the values is 
        // x1, y1, x2, y2. X values are x1 &lt;= X &lt; x2 Y values are y1 &lt;= Y &lt; y2 
        // ------
        // ROD (rodx1,rody1,rodx2,rody2) -
        //
        // The full region of definition. The ROD, in PixelCoordinates, are of the 
        // addressable pixels in an image's data pointer. The order of the values is 
        // x1, y1, x2, y2. X values are x1 &lt;= X &lt; x2 Y values are y1 &lt;= Y &lt; y2 
        // ------
        // Row Bytes -
        //
        // The number of bytes in a row of an image.
        // ------
        // Field -
        //
        // kOfxImageFieldNone - the image is an unfielded frame
        // kOfxImageFieldBoth - the image is fielded and contains both interlaced fields
        // kOfxImageFieldLower - the image is fielded and contains a single field, being the lower field (rows 0,2,4...)
        // kOfxImageFieldUpper - the image is fielded and contains a single field, being the upper field (rows 1,3,5...)        
        // ------
        // Unique Identifier -
        //
        // Uniquely labels an image. This is host set and allows a plug-in to differentiate between images. This is 
        // especially useful if a plugin caches analysed information about the image (for example motion vectors). The 
        // plugin can label the cached information with this identifier. If a user connects a different clip to the 
        // analysed input, or the image has changed in some way then the plugin can detect this via an identifier change
        // and re-evaluate the cached information. 

        // construction based on clip instance
        Image(Clip::Instance& instance,     // construct from clip instance taking pixel depth, components, pre mult and aspect ratio
              double renderScaleX, 
              double renderScaleY,
              void* data,
              const OfxRectI &bounds,
              const OfxRectI &rod,
              int rowBytes,
              std::string field,
              std::string uniqueIdentifier);

        // OfxImageClipHandle getHandle();
        OfxPropertySetHandle getPropHandle() { return Property::Set::getHandle(); }

        /// release the reference count, which, if zero, deletes this
        void releaseReference();

        /// add a reference to this image
        void addReference() {_referenceCount++;}
      };

    } // Memory

  } // Host

} // OFX

#endif // OFX_CLIP_H
