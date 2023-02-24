// Copyright Contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
// https://github.com/AcademySoftwareFoundation/OpenFx

#ifndef HOST_DEMO_EFFECT_INSTANCE_H
#define HOST_DEMO_EFFECT_INSTANCE_H

namespace MyHost {

  // class definition
  class MyEffectInstance : public OFX::Host::ImageEffect::Instance {
  public:
    MyEffectInstance(OFX::Host::ImageEffect::ImageEffectPlugin* plugin,
                     OFX::Host::ImageEffect::Descriptor& desc,
                     const std::string& context);

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // overridden for ImageEffect::Instance
    
    /// get default output fielding. This is passed into the clip prefs action
    /// and  might be mapped (if the host allows such a thing)
    virtual const std::string &getDefaultOutputFielding() const;
    
    /// make a clip
    OFX::Host::ImageEffect::ClipInstance* newClipInstance(OFX::Host::ImageEffect::Instance* plugin,
                                                          OFX::Host::ImageEffect::ClipDescriptor* descriptor,
                                                          int index);

    
    /// vmessage
    virtual OfxStatus vmessage(const char* type,
                               const char* id,
                               const char* format,
                               va_list args);       
    
    virtual OfxStatus setPersistentMessage(const char* type,
                                           const char* id,
                                           const char* format,
                                           va_list args);

    virtual OfxStatus clearPersistentMessage();       

    // The size of the current project in canonical coordinates. 
    // The size of a project is a sub set of the kOfxImageEffectPropProjectExtent. For example a 
    // project may be a PAL SD project, but only be a letter-box within that. The project size is 
    // the size of this sub window. 
    virtual void getProjectSize(double& xSize, double& ySize) const;

    // The offset of the current project in canonical coordinates. 
    // The offset is related to the kOfxImageEffectPropProjectSize and is the offset from the origin 
    // of the project 'subwindow'. For example for a PAL SD project that is in letterbox form, the
    // project offset is the offset to the bottom left hand corner of the letter box. The project 
    // offset is in canonical coordinates. 
    virtual void getProjectOffset(double& xOffset, double& yOffset) const;

    // The extent of the current project in canonical coordinates. 
    // The extent is the size of the 'output' for the current project. See ProjectCoordinateSystems 
    // for more infomation on the project extent. The extent is in canonical coordinates and only 
    // returns the top right position, as the extent is always rooted at 0,0. For example a PAL SD 
    // project would have an extent of 768, 576. 
    virtual void getProjectExtent(double& xSize, double& ySize) const;

    // The pixel aspect ratio of the current project 
    virtual double getProjectPixelAspectRatio() const;

    // The duration of the effect 
    // This contains the duration of the plug-in effect, in frames. 
    virtual double getEffectDuration() const;

    // For an instance, this is the frame rate of the project the effect is in. 
    virtual double getFrameRate() const;

    /// This is called whenever a param is changed by the plugin so that
    /// the recursive instanceChangedAction will be fed the correct frame 
    virtual double getFrameRecursive() const;

    /// This is called whenever a param is changed by the plugin so that
    /// the recursive instanceChangedAction will be fed the correct
    /// renderScale
    virtual void getRenderScaleRecursive(double &x, double &y) const;


    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // overridden for Param::SetInstance
    
    /// make a parameter instance
    ///
    /// Client host code needs to implement this
    virtual OFX::Host::Param::Instance* newParam(const std::string& name, OFX::Host::Param::Descriptor& Descriptor);        
    
    /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
    ///
    /// Client host code needs to implement this
    virtual OfxStatus editBegin(const std::string& name);
    
    /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
    ///
    /// Client host code needs to implement this
    virtual OfxStatus editEnd();


    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // overridden for Progress::ProgressI
    
    /// Start doing progress. 
    virtual void progressStart(const std::string &message, const std::string &messageid);
    
    /// finish yer progress
    virtual void progressEnd();
    
    /// set the progress to some level of completion, returns
    /// false if you should abandon processing, true to continue
    virtual bool progressUpdate(double t);        

    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////
    // overridden for TimeLine::TimeLineI

    /// get the current time on the timeline. This is not necessarily the same
    /// time as being passed to an action (eg render)
    virtual double timeLineGetTime();
    
    /// set the timeline to a specific time
    virtual void timeLineGotoTime(double t);
    
    /// get the first and last times available on the effect's timeline
    virtual void timeLineGetBounds(double &t1, double &t2);    

  };

}

#endif // HOST_DEMO_EFFECT_INSTANCE_H
