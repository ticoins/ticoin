//ticoin Copyright (c) 2011 The LevelDB Authors. All rights reserved.
//ticoin Use of this source code is governed by a BSD-style license that can be
//ticoin found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_PORT_PORT_H_
#define STORAGE_LEVELDB_PORT_PORT_H_

#include <string.h>

//ticoin Include the appropriate platform specific file below.  If you are
//ticoin porting to a new platform, see "port_example.h" for documentation
//ticoin of what the new port_<platform>.h file must provide.
#if defined(LEVELDB_PLATFORM_POSIX)
#  include "port/port_posix.h"
#elif defined(LEVELDB_PLATFORM_CHROMIUM)
#  include "port/port_chromium.h"
#elif defined(LEVELDB_PLATFORM_WINDOWS)
#  include "port/port_win.h"
#endif

#endif  //ticoin STORAGE_LEVELDB_PORT_PORT_H_
