#ifndef CLIENTVERSION_H
#define CLIENTVERSION_H

#if defined(HAVE_CONFIG_H)
#include "ticoin-config.h"
#else
//
//ticoin client versioning and copyright year
//

//ticoin These need to be macros, as version.cpp's and ticoin-qt.rc's voodoo requires it
#define CLIENT_VERSION_MAJOR       0
#define CLIENT_VERSION_MINOR       9
#define CLIENT_VERSION_REVISION    5
#define CLIENT_VERSION_BUILD       0

//ticoin Set to true for release, false for prerelease or test build
#define CLIENT_VERSION_IS_RELEASE  true

//ticoin Copyright year (2009-this)
//ticoin Todo: update this when changing our copyright comments in the source
#define COPYRIGHT_YEAR 2015

#endif //HAVE_CONFIG_H

//ticoin Converts the parameter X to a string after macro replacement on X has been performed.
//ticoin Don't merge these into one macro!
#define STRINGIZE(X) DO_STRINGIZE(X)
#define DO_STRINGIZE(X) #X

#endif //ticoin CLIENTVERSION_H
