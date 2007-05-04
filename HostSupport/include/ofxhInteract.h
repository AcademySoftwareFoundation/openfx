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

      class Instance : public Descriptor, 
                       private Property::GetHook {
      protected:
        OFX::Host::ImageEffect::Instance&  _effect;
        void*                              _instanceData;
        Property::Set                      _argProperties;

        /// initialise the argument properties
        void initArgProp(OfxTime time, 
                         double  renderScaleX, 
                         double  renderScaleY);
                
        /// set pen props in the args
        void setPenArgProps(double  penX, 
                            double  penY,
                            double  pressure);
        
        /// set key args in the props
        void setKeyArgProps(int     key,
                            char*   keyString);
        
      public:
        Instance(Descriptor& desc, ImageEffect::Instance& effect);

        virtual ~Instance();

        // hooks to kOfxInteractPropViewportSize in the property set
        virtual void getViewportSize(double &width, double &height) = 0;

        // hooks to live kOfxInteractPropPixelScale in the property set
        virtual void getPixelScale(double& xScale, double& yScale) = 0;

        // hooks to kOfxInteractPropBackgroundColour in the property set
        virtual void getBackgroundColour(double &r, double &g, double &b) = 0;

        // implement
        virtual OfxStatus swapBuffers() = 0;
        virtual OfxStatus redraw() = 0;

        // returns the params the interact uses
        virtual void getSlaveToParam(std::vector<std::string>& params);

        // do nothing
        virtual int  getDimension(const std::string &name) OFX_EXCEPTION_SPEC;
        
        // don't know what to do
        virtual void reset(const std::string &name) OFX_EXCEPTION_SPEC;

        // the gethook virutals for viewport size, pixel scale, background colour
        virtual double getDoubleProperty(const std::string &name, int index) OFX_EXCEPTION_SPEC;
        virtual void getDoublePropertyN(const std::string &name, double *first, int n) OFX_EXCEPTION_SPEC;

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
