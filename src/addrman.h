//ticoin Copyright (c) 2012 Pieter Wuille
//ticoin Distributed under the MIT/X11 software license, see the accompanying
//ticoin file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef _ticoin_ADDRMAN
#define _ticoin_ADDRMAN 1

#include "netbase.h"
#include "protocol.h"
#include "sync.h"
#include "util.h"

#include <map>
#include <set>
#include <stdint.h>
#include <vector>

#include <openssl/rand.h>

/** Extended statistics about a CAddress */
class CAddrInfo : public CAddress
{
private:
    //ticoin where knowledge about this address first came from
    CNetAddr source;

    //ticoin last successful connection by us
    int64_t nLastSuccess;

    //ticoin last try whatsoever by us:
    //ticoin int64_t CAddress::nLastTry

    //ticoin connection attempts since last successful attempt
    int nAttempts;

    //ticoin reference count in new sets (memory only)
    int nRefCount;

    //ticoin in tried set? (memory only)
    bool fInTried;

    //ticoin position in vRandom
    int nRandomPos;

    friend class CAddrMan;

public:

    IMPLEMENT_SERIALIZE(
        CAddress* pthis = (CAddress*)(this);
        READWRITE(*pthis);
        READWRITE(source);
        READWRITE(nLastSuccess);
        READWRITE(nAttempts);
    )

    void Init()
    {
        nLastSuccess = 0;
        nLastTry = 0;
        nAttempts = 0;
        nRefCount = 0;
        fInTried = false;
        nRandomPos = -1;
    }

    CAddrInfo(const CAddress &addrIn, const CNetAddr &addrSource) : CAddress(addrIn), source(addrSource)
    {
        Init();
    }

    CAddrInfo() : CAddress(), source()
    {
        Init();
    }

    //ticoin Calculate in which "tried" bucket this entry belongs
    int GetTriedBucket(const std::vector<unsigned char> &nKey) const;

    //ticoin Calculate in which "new" bucket this entry belongs, given a certain source
    int GetNewBucket(const std::vector<unsigned char> &nKey, const CNetAddr& src) const;

    //ticoin Calculate in which "new" bucket this entry belongs, using its default source
    int GetNewBucket(const std::vector<unsigned char> &nKey) const
    {
        return GetNewBucket(nKey, source);
    }

    //ticoin Determine whether the statistics about this entry are bad enough so that it can just be deleted
    bool IsTerrible(int64_t nNow = GetAdjustedTime()) const;

    //ticoin Calculate the relative chance this entry should be given when selecting nodes to connect to
    double GetChance(int64_t nNow = GetAdjustedTime()) const;

};

//ticoin Stochastic address manager
//
//ticoin Design goals:
//ticoin  * Only keep a limited number of addresses around, so that addr.dat and memory requirements do not grow without bound.
//ticoin  * Keep the address tables in-memory, and asynchronously dump the entire to able in addr.dat.
//ticoin  * Make sure no (localized) attacker can fill the entire table with his nodes/addresses.
//
//ticoin To that end:
//ticoin  * Addresses are organized into buckets.
//ticoin    * Address that have not yet been tried go into 256 "new" buckets.
//ticoin      * Based on the address range (/16 for IPv4) of source of the information, 32 buckets are selected at random
//ticoin      * The actual bucket is chosen from one of these, based on the range the address itself is located.
//ticoin      * One single address can occur in up to 4 different buckets, to increase selection chances for addresses that
//ticoin        are seen frequently. The chance for increasing this multiplicity decreases exponentially.
//ticoin      * When adding a new address to a full bucket, a randomly chosen entry (with a bias favoring less recently seen
//ticoin        ones) is removed from it first.
//ticoin    * Addresses of nodes that are known to be accessible go into 64 "tried" buckets.
//ticoin      * Each address range selects at random 4 of these buckets.
//ticoin      * The actual bucket is chosen from one of these, based on the full address.
//ticoin      * When adding a new good address to a full bucket, a randomly chosen entry (with a bias favoring less recently
//ticoin        tried ones) is evicted from it, back to the "new" buckets.
//ticoin    * Bucket selection is based on cryptographic hashing, using a randomly-generated 256-bit key, which should not
//ticoin      be observable by adversaries.
//ticoin    * Several indexes are kept for high performance. Defining DEBUG_ADDRMAN will introduce frequent (and expensive)
//ticoin      consistency checks for the entire data structure.

//ticoin total number of buckets for tried addresses
#define ADDRMAN_TRIED_BUCKET_COUNT 64

//ticoin maximum allowed number of entries in buckets for tried addresses
#define ADDRMAN_TRIED_BUCKET_SIZE 64

//ticoin total number of buckets for new addresses
#define ADDRMAN_NEW_BUCKET_COUNT 256

//ticoin maximum allowed number of entries in buckets for new addresses
#define ADDRMAN_NEW_BUCKET_SIZE 64

//ticoin over how many buckets entries with tried addresses from a single group (/16 for IPv4) are spread
#define ADDRMAN_TRIED_BUCKETS_PER_GROUP 4

//ticoin over how many buckets entries with new addresses originating from a single group are spread
#define ADDRMAN_NEW_BUCKETS_PER_SOURCE_GROUP 32

//ticoin in how many buckets for entries with new addresses a single address may occur
#define ADDRMAN_NEW_BUCKETS_PER_ADDRESS 4

//ticoin how many entries in a bucket with tried addresses are inspected, when selecting one to replace
#define ADDRMAN_TRIED_ENTRIES_INSPECT_ON_EVICT 4

//ticoin how old addresses can maximally be
#define ADDRMAN_HORIZON_DAYS 30

//ticoin after how many failed attempts we give up on a new node
#define ADDRMAN_RETRIES 3

//ticoin how many successive failures are allowed ...
#define ADDRMAN_MAX_FAILURES 10

//ticoin ... in at least this many days
#define ADDRMAN_MIN_FAIL_DAYS 7

//ticoin the maximum percentage of nodes to return in a getaddr call
#define ADDRMAN_GETADDR_MAX_PCT 23

//ticoin the maximum number of nodes to return in a getaddr call
#define ADDRMAN_GETADDR_MAX 2500

/** Stochastical (IP) address manager */
class CAddrMan
{
private:
    //ticoin critical section to protect the inner data structures
    mutable CCriticalSection cs;

    //ticoin secret key to randomize bucket select with
    std::vector<unsigned char> nKey;

    //ticoin last used nId
    int nIdCount;

    //ticoin table with information about all nIds
    std::map<int, CAddrInfo> mapInfo;

    //ticoin find an nId based on its network address
    std::map<CNetAddr, int> mapAddr;

    //ticoin randomly-ordered vector of all nIds
    std::vector<int> vRandom;

    //ticoin number of "tried" entries
    int nTried;

    //ticoin list of "tried" buckets
    std::vector<std::vector<int> > vvTried;

    //ticoin number of (unique) "new" entries
    int nNew;

    //ticoin list of "new" buckets
    std::vector<std::set<int> > vvNew;

protected:

    //ticoin Find an entry.
    CAddrInfo* Find(const CNetAddr& addr, int *pnId = NULL);

    //ticoin find an entry, creating it if necessary.
    //ticoin nTime and nServices of found node is updated, if necessary.
    CAddrInfo* Create(const CAddress &addr, const CNetAddr &addrSource, int *pnId = NULL);

    //ticoin Swap two elements in vRandom.
    void SwapRandom(unsigned int nRandomPos1, unsigned int nRandomPos2);

    //ticoin Return position in given bucket to replace.
    int SelectTried(int nKBucket);

    //ticoin Remove an element from a "new" bucket.
    //ticoin This is the only place where actual deletes occur.
    //ticoin They are never deleted while in the "tried" table, only possibly evicted back to the "new" table.
    int ShrinkNew(int nUBucket);

    //ticoin Move an entry from the "new" table(s) to the "tried" table
    //ticoin @pre vvUnkown[nOrigin].count(nId) != 0
    void MakeTried(CAddrInfo& info, int nId, int nOrigin);

    //ticoin Mark an entry "good", possibly moving it from "new" to "tried".
    void Good_(const CService &addr, int64_t nTime);

    //ticoin Add an entry to the "new" table.
    bool Add_(const CAddress &addr, const CNetAddr& source, int64_t nTimePenalty);

    //ticoin Mark an entry as attempted to connect.
    void Attempt_(const CService &addr, int64_t nTime);

    //ticoin Select an address to connect to.
    //ticoin nUnkBias determines how much to favor new addresses over tried ones (min=0, max=100)
    CAddress Select_(int nUnkBias);

#ifdef DEBUG_ADDRMAN
    //ticoin Perform consistency check. Returns an error code or zero.
    int Check_();
#endif

    //ticoin Select several addresses at once.
    void GetAddr_(std::vector<CAddress> &vAddr);

    //ticoin Mark an entry as currently-connected-to.
    void Connected_(const CService &addr, int64_t nTime);

public:

    IMPLEMENT_SERIALIZE
    (({
        //ticoin serialized format:
        //ticoin * version byte (currently 0)
        //ticoin * nKey
        //ticoin * nNew
        //ticoin * nTried
        //ticoin * number of "new" buckets
        //ticoin * all nNew addrinfos in vvNew
        //ticoin * all nTried addrinfos in vvTried
        //ticoin * for each bucket:
        //ticoin   * number of elements
        //ticoin   * for each element: index
        //
        //ticoin Notice that vvTried, mapAddr and vVector are never encoded explicitly;
        //ticoin they are instead reconstructed from the other information.
        //
        //ticoin vvNew is serialized, but only used if ADDRMAN_UNKOWN_BUCKET_COUNT didn't change,
        //ticoin otherwise it is reconstructed as well.
        //
        //ticoin This format is more complex, but significantly smaller (at most 1.5 MiB), and supports
        //ticoin changes to the ADDRMAN_ parameters without breaking the on-disk structure.
        {
            LOCK(cs);
            unsigned char nVersion = 0;
            READWRITE(nVersion);
            READWRITE(nKey);
            READWRITE(nNew);
            READWRITE(nTried);

            CAddrMan *am = const_cast<CAddrMan*>(this);
            if (fWrite)
            {
                int nUBuckets = ADDRMAN_NEW_BUCKET_COUNT;
                READWRITE(nUBuckets);
                std::map<int, int> mapUnkIds;
                int nIds = 0;
                for (std::map<int, CAddrInfo>::iterator it = am->mapInfo.begin(); it != am->mapInfo.end(); it++)
                {
                    if (nIds == nNew) break; //ticoin this means nNew was wrong, oh ow
                    mapUnkIds[(*it).first] = nIds;
                    CAddrInfo &info = (*it).second;
                    if (info.nRefCount)
                    {
                        READWRITE(info);
                        nIds++;
                    }
                }
                nIds = 0;
                for (std::map<int, CAddrInfo>::iterator it = am->mapInfo.begin(); it != am->mapInfo.end(); it++)
                {
                    if (nIds == nTried) break; //ticoin this means nTried was wrong, oh ow
                    CAddrInfo &info = (*it).second;
                    if (info.fInTried)
                    {
                        READWRITE(info);
                        nIds++;
                    }
                }
                for (std::vector<std::set<int> >::iterator it = am->vvNew.begin(); it != am->vvNew.end(); it++)
                {
                    const std::set<int> &vNew = (*it);
                    int nSize = vNew.size();
                    READWRITE(nSize);
                    for (std::set<int>::iterator it2 = vNew.begin(); it2 != vNew.end(); it2++)
                    {
                        int nIndex = mapUnkIds[*it2];
                        READWRITE(nIndex);
                    }
                }
            } else {
                int nUBuckets = 0;
                READWRITE(nUBuckets);
                am->nIdCount = 0;
                am->mapInfo.clear();
                am->mapAddr.clear();
                am->vRandom.clear();
                am->vvTried = std::vector<std::vector<int> >(ADDRMAN_TRIED_BUCKET_COUNT, std::vector<int>(0));
                am->vvNew = std::vector<std::set<int> >(ADDRMAN_NEW_BUCKET_COUNT, std::set<int>());
                for (int n = 0; n < am->nNew; n++)
                {
                    CAddrInfo &info = am->mapInfo[n];
                    READWRITE(info);
                    am->mapAddr[info] = n;
                    info.nRandomPos = vRandom.size();
                    am->vRandom.push_back(n);
                    if (nUBuckets != ADDRMAN_NEW_BUCKET_COUNT)
                    {
                        am->vvNew[info.GetNewBucket(am->nKey)].insert(n);
                        info.nRefCount++;
                    }
                }
                am->nIdCount = am->nNew;
                int nLost = 0;
                for (int n = 0; n < am->nTried; n++)
                {
                    CAddrInfo info;
                    READWRITE(info);
                    std::vector<int> &vTried = am->vvTried[info.GetTriedBucket(am->nKey)];
                    if (vTried.size() < ADDRMAN_TRIED_BUCKET_SIZE)
                    {
                        info.nRandomPos = vRandom.size();
                        info.fInTried = true;
                        am->vRandom.push_back(am->nIdCount);
                        am->mapInfo[am->nIdCount] = info;
                        am->mapAddr[info] = am->nIdCount;
                        vTried.push_back(am->nIdCount);
                        am->nIdCount++;
                    } else {
                        nLost++;
                    }
                }
                am->nTried -= nLost;
                for (int b = 0; b < nUBuckets; b++)
                {
                    std::set<int> &vNew = am->vvNew[b];
                    int nSize = 0;
                    READWRITE(nSize);
                    for (int n = 0; n < nSize; n++)
                    {
                        int nIndex = 0;
                        READWRITE(nIndex);
                        CAddrInfo &info = am->mapInfo[nIndex];
                        if (nUBuckets == ADDRMAN_NEW_BUCKET_COUNT && info.nRefCount < ADDRMAN_NEW_BUCKETS_PER_ADDRESS)
                        {
                            info.nRefCount++;
                            vNew.insert(nIndex);
                        }
                    }
                }
            }
        }
    });)

    CAddrMan() : vRandom(0), vvTried(ADDRMAN_TRIED_BUCKET_COUNT, std::vector<int>(0)), vvNew(ADDRMAN_NEW_BUCKET_COUNT, std::set<int>())
    {
         nKey.resize(32);
         RAND_bytes(&nKey[0], 32);

         nIdCount = 0;
         nTried = 0;
         nNew = 0;
    }

    //ticoin Return the number of (unique) addresses in all tables.
    int size()
    {
        return vRandom.size();
    }

    //ticoin Consistency check
    void Check()
    {
#ifdef DEBUG_ADDRMAN
        {
            LOCK(cs);
            int err;
            if ((err=Check_()))
                LogPrintf("ADDRMAN CONSISTENCY CHECK FAILED!!! err=%i\n", err);
        }
#endif
    }

    //ticoin Add a single address.
    bool Add(const CAddress &addr, const CNetAddr& source, int64_t nTimePenalty = 0)
    {
        bool fRet = false;
        {
            LOCK(cs);
            Check();
            fRet |= Add_(addr, source, nTimePenalty);
            Check();
        }
        if (fRet)
            LogPrint("addrman", "Added %s from %s: %i tried, %i new\n", addr.ToStringIPPort().c_str(), source.ToString().c_str(), nTried, nNew);
        return fRet;
    }

    //ticoin Add multiple addresses.
    bool Add(const std::vector<CAddress> &vAddr, const CNetAddr& source, int64_t nTimePenalty = 0)
    {
        int nAdd = 0;
        {
            LOCK(cs);
            Check();
            for (std::vector<CAddress>::const_iterator it = vAddr.begin(); it != vAddr.end(); it++)
                nAdd += Add_(*it, source, nTimePenalty) ? 1 : 0;
            Check();
        }
        if (nAdd)
            LogPrint("addrman", "Added %i addresses from %s: %i tried, %i new\n", nAdd, source.ToString().c_str(), nTried, nNew);
        return nAdd > 0;
    }

    //ticoin Mark an entry as accessible.
    void Good(const CService &addr, int64_t nTime = GetAdjustedTime())
    {
        {
            LOCK(cs);
            Check();
            Good_(addr, nTime);
            Check();
        }
    }

    //ticoin Mark an entry as connection attempted to.
    void Attempt(const CService &addr, int64_t nTime = GetAdjustedTime())
    {
        {
            LOCK(cs);
            Check();
            Attempt_(addr, nTime);
            Check();
        }
    }

    //ticoin Choose an address to connect to.
    //ticoin nUnkBias determines how much "new" entries are favored over "tried" ones (0-100).
    CAddress Select(int nUnkBias = 50)
    {
        CAddress addrRet;
        {
            LOCK(cs);
            Check();
            addrRet = Select_(nUnkBias);
            Check();
        }
        return addrRet;
    }

    //ticoin Return a bunch of addresses, selected at random.
    std::vector<CAddress> GetAddr()
    {
        Check();
        std::vector<CAddress> vAddr;
        {
            LOCK(cs);
            GetAddr_(vAddr);
        }
        Check();
        return vAddr;
    }

    //ticoin Mark an entry as currently-connected-to.
    void Connected(const CService &addr, int64_t nTime = GetAdjustedTime())
    {
        {
            LOCK(cs);
            Check();
            Connected_(addr, nTime);
            Check();
        }
    }
};

#endif
