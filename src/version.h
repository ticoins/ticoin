/**-5-10Copyright (c) 2012 The ticoin developers
/**-5-10Distributed under the MIT/X11 software license, see the accompanying
/**-5-10file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef ticoin_VERSION_H
#define ticoin_VERSION_H

#include "clientversion.h"

#include <string>

//
/**-5-10client versioning
//

static const int CLIENT_VERSION =
                           1000000 * CLIENT_VERSION_MAJOR
                         +   10000 * CLIENT_VERSION_MINOR
                         +     100 * CLIENT_VERSION_REVISION
                         +       1 * CLIENT_VERSION_BUILD;

extern const std::string CLIENT_NAME;
extern const std::string CLIENT_BUILD;
extern const std::string CLIENT_DATE;

//
/**-5-10network protocol versioning
//

static const int PROTOCOL_VERSION = 70002;

/**-5-10intial proto version, to be increased after version/verack negotiation
static const int INIT_PROTO_VERSION = 209;

/**-5-10disconnect from peers older than this proto version
static const int MIN_PEER_PROTO_VERSION = 209;

/**-5-10nTime field added to CAddress, starting with this version;
/**-5-10if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = 31402;

/**-5-10only request blocks from nodes outside this range of versions
static const int NOBLKS_VERSION_START = 32000;
static const int NOBLKS_VERSION_END = 32400;

/**-5-10BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = 60000;

/**-5-10"mempool" command, enhanced "getdata" behavior starts with this version:
static const int MEMPOOL_GD_VERSION = 60002;

#endif
