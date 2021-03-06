// Copyright (c) 2017 The CRC32C Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#include "./crc32c_sse42.h"

#if defined(HAVE_SSE42)

#define TESTED_EXTEND CRC32C_Extend_SSE42
#include "./crc32c_extend_unittests.h"
#undef TESTED_EXTEND

#endif  // defined(HAVE_SSE42)
