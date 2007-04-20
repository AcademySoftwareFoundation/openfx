#ifndef HOST_DEMO_HOST_DESCRIPTOR_H
#define HOST_DEMO_HOST_DESCRIPTOR_H

namespace MyHost {

  class MyHostDescriptor : public OFX::Host::Descriptor
  {
  public:
    MyHostDescriptor();
  };

  // my ofx host object
  extern MyHostDescriptor gOfxHost;

}

#endif // HOST_DEMO_HOST_DESCRIPTOR_H