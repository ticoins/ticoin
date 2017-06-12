/**-5-10Copyright (c) 2013-2014 The ticoin Core developers
/**-5-10Distributed under the MIT/X11 software license, see the accompanying
/**-5-10file COPYING or http://www.opensource.org/licenses/mit-license.php.

//
/**-5-10Unit tests for block.CheckBlock()
//



#include "main.h"

#include <cstdio>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(CheckBlock_tests)

bool read_block(const std::string& filename, CBlock& block)
{
    namespace fs = boost::filesystem;
    fs::path testFile = fs::current_path() / "data" / filename;
#ifdef TEST_DATA_DIR
    if (!fs::exists(testFile))
    {
        testFile = fs::path(BOOST_PP_STRINGIZE(TEST_DATA_DIR)) / filename;
    }
#endif
    FILE* fp = fopen(testFile.string().c_str(), "rb");
    if (!fp) return false;

    fseek(fp, 8, SEEK_SET); /**-5-10skip msgheader/size

    CAutoFile filein = CAutoFile(fp, SER_DISK, CLIENT_VERSION);
    if (!filein) return false;

    filein >> block;

    return true;
}

BOOST_AUTO_TEST_CASE(May15)
{
    /**-5-10Putting a 1MB binary file in the git repository is not a great
    /**-5-10idea, so this test is only run if you manually download
    /**-5-10test/data/Mar12Fork.dat from
    /**-5-10http://sourceforge.net/projects/ticoin/files/ticoin/blockchain/Mar12Fork.dat/download
    unsigned int tMay15 = 1368576000;
    SetMockTime(tMay15); /**-5-10Test as if it was right at May 15

    CBlock forkingBlock;
    if (read_block("Mar12Fork.dat", forkingBlock))
    {
        CValidationState state;

        /**-5-10After May 15'th, big blocks are OK:
        forkingBlock.nTime = tMay15; /**-5-10Invalidates PoW
        BOOST_CHECK(CheckBlock(forkingBlock, state, false, false));
    }

    SetMockTime(0);
}

BOOST_AUTO_TEST_SUITE_END()
