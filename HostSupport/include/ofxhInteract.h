#ifndef OFX_INTERACT_H
#define OFX_INTERACT_H

namespace OFX {

  namespace Host {

    namespace Interact {
  
      class Descriptor {
      protected:
        Property::Set _properties;

      public:
        Descriptor();
        Descriptor(const Descriptor& desc);

        /// grab a handle on the parameter for passing to the C API
        OfxParamHandle getHandle() {
          return (OfxParamHandle)this;
        }
        
        /// grab a handle on the properties of this parameter for the C api
        OfxPropertySetHandle getPropHandle() {
          return _properties.getHandle();
        }

        const Property::Set &getProperties() const {
          return _properties;
        }

      };

      class Instance : public Descriptor, public Property::DoubleSetHook, public Property::DoubleGetHook {
      protected:
        OFX::Host::ImageEffect::Instance&  _effect;
        void*                              _instanceData;

      public:
        Instance(Descriptor& desc, ImageEffect::Instance& effect);

        virtual ~Instance();

        // hooks to kOfxInteractPropViewportSize in the property set
        virtual OfxStatus getViewportSize(double &width, double &height) = 0;

        // hooks to live kOfxInteractPropPixelScale in the property set
        virtual OfxStatus getPixelScale(double& xScale, double& yScale) = 0;

        // hooks to kOfxInteractPropBackgroundColour in the property set
        virtual OfxStatus getBackgroundColour(double &r, double &g, double &b) = 0;

        // implement
        virtual OfxStatus swapBuffers() = 0;
        virtual OfxStatus redraw() = 0;

        // returns the params the interact uses
        virtual OfxStatus getSlaveToParam(std::vector<std::string>& params);

        // do nothing
        virtual int  getDimension(const std::string &name) {
          throw Property::Exception(kOfxStatErrMissingHostFeature);
        }
        
        virtual void setProperty(const std::string &name, double value, int index) {
          throw Property::Exception(kOfxStatErrMissingHostFeature);
        }
        virtual void setPropertyN(const std::string &name, double *first, int n) {
          throw Property::Exception(kOfxStatErrMissingHostFeature);
        }
        
        // don't know what to do
        virtual void reset(const std::string &name) {
          throw Property::Exception(kOfxStatErrMissingHostFeature);
        }

        // get the virutals for viewport size, pixel scale, background colour
        virtual void getProperty(const std::string &name, double &ret, int index);
        virtual void getPropertyN(const std::string &name, double *first, int n);

        // interact action - kOfxInteractActionDraw 
        // 
        // Params -
        //
        //    time              - the effect time at which changed occured
        //    renderScale       - the render scale
        virtual OfxStatus drawAction(OfxTime time, double renderScaleX, double renderScaleY);

        // interact action - kOfxInteractActionPenMotion
        //
        // Params  -
        //
        //    time              - the effect time at which changed occured
        //    renderScale       - the render scale
        //    penX              - the X position
        //    penY              - the Y position
        //    pressure          - the pen pressue 0 to 1
        virtual OfxStatus penMotionAction(OfxTime time, 
                                          double renderScaleX, 
                                          double renderScaleY,
                                          double penX, 
                                          double penY,
                                          double pressure);

        // interact action - kOfxInteractActionPenUp
        //
        // Params  -
        //
        //    time              - the effect time at which changed occured
        //    renderScale       - the render scale
        //    penX              - the X position
        //    penY              - the Y position
        //    pressure          - the pen pressue 0 to 1
        virtual OfxStatus penUpAction(OfxTime time, 
                                      double  renderScaleX, 
                                      double  renderScaleY,
                                      double  penX, 
                                      double  penY,
                                      double  pressure);

        // interact action - kOfxInteractActionPenDown
        //
        // Params  -
        //
        //    time              - the effect time at which changed occured
        //    renderScale       - the render scale
        //    penX              - the X position
        //    penY              - the Y position
        //    pressure          - the pen pressue 0 to 1
        virtual OfxStatus penDownAction(OfxTime time, 
                                        double  renderScaleX, 
                                        double  renderScaleY,
                                        double  penX, 
                                        double  penY,
                                        double  pressure);

        // interact action - kOfxInteractActionkeyDown
        //
        // Params  -
        //
        //    time              - the effect time at which changed occured
        //    renderScale       - the render scale
        //    key               - the pressed key
        //    keyString         - the pressed key string
        virtual OfxStatus keyDownAction(OfxTime time, 
                                        double  renderScaleX, 
                                        double  renderScaleY,
                                        int     key,
                                        char*   keyString);

        // interact action - kOfxInteractActionkeyUp
        //
        // Params  -
        //
        //    time              - the effect time at which changed occured
        //    renderScale       - the render scale
        //    key               - the pressed key
        //    keyString         - the pressed key string
        virtual OfxStatus keyUpAction(OfxTime time, 
                                      double  renderScaleX, 
                                      double  renderScaleY,
                                      int     key,
                                      char*   keyString);

        // interact action - kOfxInteractActionkeyRepeat
        //
        // Params  -
        //
        //    time              - the effect time at which changed occured
        //    renderScale       - the render scale
        //    key               - the pressed key
        //    keyString         - the pressed key string
        virtual OfxStatus keyRepeatAction(OfxTime time, 
                                          double  renderScaleX, 
                                          double  renderScaleY,
                                          int     key,
                                          char*   keyString);

        // interact action - kOfxInteractActionLoseFocus
        // 
        // Params -
        //
        //    time              - the effect time at which changed occured
        //    renderScale       - the render scale
        virtual OfxStatus gainFocusAction(OfxTime time, 
                                          double  renderScaleX, 
                                          double  renderScaleY);

        // interact action - kOfxInteractActionLoseFocus
        // 
        // Params -
        //
        //    time              - the effect time at which changed occured
        //    renderScale       - the render scale
        virtual OfxStatus loseFocusAction(OfxTime  time, 
                                          double   renderScaleX, 
                                          double   renderScaleY);
      };

    } // Interact 

  } // Host

} // OFX

#endif // OFX_INTERACT_H