# Copyright OpenFX and contributors to the OpenFX project.
# SPDX-License-Identifier: BSD-3-Clause

include ../Makefile.master

UNAME_SYSTEM := $(shell uname -s)

# Note: Add -DDEBUG to turn on logging
CXXFLAGS = --std=c++20 -fvisibility=hidden -I../../../include -I../../../Support/include -I../../../Support/Plugins/include -DOFX_SUPPORTS_OPENGLRENDER -DOFX_SUPPORTS_OPENCLRENDER

ifeq ($(UNAME_SYSTEM), Linux)
	LDFLAGS = -shared -fvisibility=hidden
	BUNDLE_DIR = MakiFX.ofx.bundle/Contents/Linux-x86-64/
else
	LDFLAGS = -shared -fvisibility=hidden -L. -lOpenCL
	LDFLAGS += ${ARCH_FLAGS}
	BUNDLE_DIR = MakiFX.ofx.bundle/Contents/Win64/
endif

BetaFX.ofx: TransformGPU.o TransformKernel.o CustomCL.o CustomCLKernelKernel.o PluginRegistration.o ofxsCore.o ofxsImageEffect.o ofxsInteract.o ofxsLog.o ofxsMultiThread.o ofxsParams.o ofxsProperty.o ofxsPropertyValidation.o
	$(CXX) $^ -o $@ $(LDFLAGS)
	mkdir -p $(BUNDLE_DIR)
	cp BetaFX.ofx $(BUNDLE_DIR)

%.o: ../../../Support/Library/%.cpp
	$(CXX) -c $< $(CXXFLAGS)

clean:
	rm -f *.o *.ofx
	rm -fr BetaFX.ofx.bundle

install: BetaFX.ofx
	cp -fr BetaFX.ofx.bundle build
