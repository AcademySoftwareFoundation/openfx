#include <stdio.h>
#include "multibundle1.h"
#include "multibundle2.h"
#include "ofxsImageEffect.h"

namespace OFX {
namespace Plugin {
void getPluginIDs(OFX::PluginFactoryArray &ids) {
  static DotExamplePluginFactory p1;
  ids.push_back(&p1);
  static GammaExamplePluginFactory p2;
  ids.push_back(&p2);
}
}  // namespace Plugin
}  // namespace OFX
