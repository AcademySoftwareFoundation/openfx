// Copyright Contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause
// https://github.com/AcademySoftwareFoundation/OpenFx

#include <stdlib.h>
#include <proto/exec.h>

struct Library* ExpatBase = 0;
struct ExpatIFace* IExpat = 0;


void setup() __attribute__((constructor));
void cleanup() __attribute__((destructor));


void setup()
{
	ExpatBase = OpenLibrary("expat.library", 4);
	IExpat = (struct ExpatIFace*)GetInterface(ExpatBase, "main", 1, NULL);
	if ( IExpat == 0 )  {
		DebugPrintF("Can't open expat.library\n");
	}
}


void cleanup()
{
	if ( IExpat != 0 )  {
		DropInterface((struct Interface*)IExpat);
		IExpat = 0;
	}

	if ( ExpatBase != 0 )  {
		CloseLibrary(ExpatBase);
		ExpatBase = 0;
	}
}
