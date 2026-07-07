/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: off; c-basic-offset: 2 -*- */
// Copyright OpenFX and contributors to the OpenFX project.
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include "spdlog/spdlog.h"

// Some build environments opt in to fmt's strict mode via
// FMT_ENFORCE_COMPILE_STRING, which rejects any non-FMT_STRING format
// string that fmt can't compile-time-check itself. In C++20 (where fmt's
// own consteval machinery handles the check), the wrapper is redundant
// and some older spdlogs trip on FMT_STRING-wrapped plain strings, so
// we only opt in when fmt itself tells us it cannot consteval-check.
#if defined(FMT_ENFORCE_COMPILE_STRING) && !FMT_USE_CONSTEVAL && defined(FMT_STRING)
#  define OFX_FMT_STRING(s) FMT_STRING(s)
#else
#  define OFX_FMT_STRING(s) s
#endif
