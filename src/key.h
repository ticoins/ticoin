//ticoin Copyright (c) 2009-2010 Satoshi Nakamoto
//ticoin Copyright (c) 2009-2013 The ticoin developers
//ticoin Distributed under the MIT/X11 software license, see the accompanying
//ticoin file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef ticoin_KEY_H
#define ticoin_KEY_H

#include "allocators.h"
#include "hash.h"
#include "serialize.h"
#include "uint256.h"

#include <stdexcept>
#include <vector>

//ticoin secp256k1:
//ticoin const unsigned int PRIVATE_KEY_SIZE = 279;
//ticoin const unsigned int PUBLIC_KEY_SIZE  = 65;
//ticoin const unsigned int SIGNATURE_SIZE   = 72;
//
//ticoin see www.keylength.com
//ticoin script supports up to 75 for single byte push

/** A reference to a CKey: the Hash160 of its serialized public key */
class CKeyID : public uint160
{
public:
    CKeyID() : uint160(0) { }
    CKeyID(const uint160 &in) : uint160(in) { }
};

/** A reference to a CScript: the Hash160 of its serialization (see script.h) */
class CScriptID : public uint160
{
public:
    CScriptID() : uint160(0) { }
    CScriptID(const uint160 &in) : uint160(in) { }
};

/** An encapsulated public key. */
class CPubKey {
private:
    //ticoin Just store the serialized data.
    //ticoin Its length can very cheaply be computed from the first byte.
    unsigned char vch[65];

    //ticoin Compute the length of a pubkey with a given first byte.
    unsigned int static GetLen(unsigned char chHeader) {
        if (chHeader == 2 || chHeader == 3)
            return 33;
        if (chHeader == 4 || chHeader == 6 || chHeader == 7)
            return 65;
        return 0;
    }

    //ticoin Set this key data to be invalid
    void Invalidate() {
        vch[0] = 0xFF;
    }

public:
    //ticoin Construct an invalid public key.
    CPubKey() {
        Invalidate();
    }

    //ticoin Initialize a public key using begin/end iterators to byte data.
    template<typename T>
    void Set(const T pbegin, const T pend) {
        int len = pend == pbegin ? 0 : GetLen(pbegin[0]);
        if (len && len == (pend-pbegin))
            memcpy(vch, (unsigned char*)&pbegin[0], len);
        else
            Invalidate();
    }

    //ticoin Construct a public key using begin/end iterators to byte data.
    template<typename T>
    CPubKey(const T pbegin, const T pend) {
        Set(pbegin, pend);
    }

    //ticoin Construct a public key from a byte vector.
    CPubKey(const std::vector<unsigned char> &vch) {
        Set(vch.begin(), vch.end());
    }

    //ticoin Simple read-only vector-like interface to the pubkey data.
    unsigned int size() const { return GetLen(vch[0]); }
    const unsigned char *begin() const { return vch; }
    const unsigned char *end() const { return vch+size(); }
    const unsigned char &operator[](unsigned int pos) const { return vch[pos]; }

    //ticoin Comparator implementation.
    friend bool operator==(const CPubKey &a, const CPubKey &b) {
        return a.vch[0] == b.vch[0] &&
               memcmp(a.vch, b.vch, a.size()) == 0;
    }
    friend bool operator!=(const CPubKey &a, const CPubKey &b) {
        return !(a == b);
    }
    friend bool operator<(const CPubKey &a, const CPubKey &b) {
        return a.vch[0] < b.vch[0] ||
               (a.vch[0] == b.vch[0] && memcmp(a.vch, b.vch, a.size()) < 0);
    }

    //ticoin Implement serialization, as if this was a byte vector.
    unsigned int GetSerializeSize(int nType, int nVersion) const {
        return size() + 1;
    }
    template<typename Stream> void Serialize(Stream &s, int nType, int nVersion) const {
        unsigned int len = size();
        ::WriteCompactSize(s, len);
        s.write((char*)vch, len);
    }
    template<typename Stream> void Unserialize(Stream &s, int nType, int nVersion) {
        unsigned int len = ::ReadCompactSize(s);
        if (len <= 65) {
            s.read((char*)vch, len);
        } else {
            //ticoin invalid pubkey, skip available data
            char dummy;
            while (len--)
                s.read(&dummy, 1);
            Invalidate();
        }
    }

    //ticoin Get the KeyID of this public key (hash of its serialization)
    CKeyID GetID() const {
        return CKeyID(Hash160(vch, vch+size()));
    }

    //ticoin Get the 256-bit hash of this public key.
    uint256 GetHash() const {
        return Hash(vch, vch+size());
    }

    //ticoin Check syntactic correctness.
    //
    //ticoin Note that this is consensus critical as CheckSig() calls it!
    bool IsValid() const {
        return size() > 0;
    }

    //ticoin fully validate whether this is a valid public key (more expensive than IsValid())
    bool IsFullyValid() const;

    //ticoin Check whether this is a compressed public key.
    bool IsCompressed() const {
        return size() == 33;
    }

    //ticoin Verify a DER signature (~72 bytes).
    //ticoin If this public key is not fully valid, the return value will be false.
    bool Verify(const uint256 &hash, const std::vector<unsigned char>& vchSig) const;

    //ticoin Verify a compact signature (~65 bytes).
    //ticoin See CKey::SignCompact.
    bool VerifyCompact(const uint256 &hash, const std::vector<unsigned char>& vchSig) const;

    //ticoin Recover a public key from a compact signature.
    bool RecoverCompact(const uint256 &hash, const std::vector<unsigned char>& vchSig);

    //ticoin Turn this public key into an uncompressed public key.
    bool Decompress();

    //ticoin Derive BIP32 child pubkey.
    bool Derive(CPubKey& pubkeyChild, unsigned char ccChild[32], unsigned int nChild, const unsigned char cc[32]) const;
};


//ticoin secure_allocator is defined in allocators.h
//ticoin CPrivKey is a serialized private key, with all parameters included (279 bytes)
typedef std::vector<unsigned char, secure_allocator<unsigned char> > CPrivKey;

/** An encapsulated private key. */
class CKey {
private:
    //ticoin Whether this private key is valid. We check for correctness when modifying the key
    //ticoin data, so fValid should always correspond to the actual state.
    bool fValid;

    //ticoin Whether the public key corresponding to this private key is (to be) compressed.
    bool fCompressed;

    //ticoin The actual byte data
    unsigned char vch[32];

    //ticoin Check whether the 32-byte array pointed to be vch is valid keydata.
    bool static Check(const unsigned char *vch);
public:

    //ticoin Construct an invalid private key.
    CKey() : fValid(false) {
        LockObject(vch);
    }

    //ticoin Copy constructor. This is necessary because of memlocking.
    CKey(const CKey &secret) : fValid(secret.fValid), fCompressed(secret.fCompressed) {
        LockObject(vch);
        memcpy(vch, secret.vch, sizeof(vch));
    }

    //ticoin Destructor (again necessary because of memlocking).
    ~CKey() {
        UnlockObject(vch);
    }

    friend bool operator==(const CKey &a, const CKey &b) {
        return a.fCompressed == b.fCompressed && a.size() == b.size() &&
               memcmp(&a.vch[0], &b.vch[0], a.size()) == 0;
    }

    //ticoin Initialize using begin and end iterators to byte data.
    template<typename T>
    void Set(const T pbegin, const T pend, bool fCompressedIn) {
        if (pend - pbegin != 32) {
            fValid = false;
            return;
        }
        if (Check(&pbegin[0])) {
            memcpy(vch, (unsigned char*)&pbegin[0], 32);
            fValid = true;
            fCompressed = fCompressedIn;
        } else {
            fValid = false;
        }
    }

    //ticoin Simple read-only vector-like interface.
    unsigned int size() const { return (fValid ? 32 : 0); }
    const unsigned char *begin() const { return vch; }
    const unsigned char *end() const { return vch + size(); }

    //ticoin Check whether this private key is valid.
    bool IsValid() const { return fValid; }

    //ticoin Check whether the public key corresponding to this private key is (to be) compressed.
    bool IsCompressed() const { return fCompressed; }

    //ticoin Initialize from a CPrivKey (serialized OpenSSL private key data).
    bool SetPrivKey(const CPrivKey &vchPrivKey, bool fCompressed);

    //ticoin Generate a new private key using a cryptographic PRNG.
    void MakeNewKey(bool fCompressed);

    //ticoin Convert the private key to a CPrivKey (serialized OpenSSL private key data).
    //ticoin This is expensive.
    CPrivKey GetPrivKey() const;

    //ticoin Compute the public key from a private key.
    //ticoin This is expensive.
    CPubKey GetPubKey() const;

    //ticoin Create a DER-serialized signature.
    bool Sign(const uint256 &hash, std::vector<unsigned char>& vchSig) const;

    //ticoin Create a compact signature (65 bytes), which allows reconstructing the used public key.
    //ticoin The format is one header byte, followed by two times 32 bytes for the serialized r and s values.
    //ticoin The header byte: 0x1B = first key with even y, 0x1C = first key with odd y,
    //ticoin                  0x1D = second key with even y, 0x1E = second key with odd y,
    //ticoin                  add 0x04 for compressed keys.
    bool SignCompact(const uint256 &hash, std::vector<unsigned char>& vchSig) const;

    //ticoin Derive BIP32 child key.
    bool Derive(CKey& keyChild, unsigned char ccChild[32], unsigned int nChild, const unsigned char cc[32]) const;

    //ticoin Load private key and check that public key matches.
    bool Load(CPrivKey &privkey, CPubKey &vchPubKey, bool fSkipCheck);
};

struct CExtPubKey {
    unsigned char nDepth;
    unsigned char vchFingerprint[4];
    unsigned int nChild;
    unsigned char vchChainCode[32];
    CPubKey pubkey;

    friend bool operator==(const CExtPubKey &a, const CExtPubKey &b) {
        return a.nDepth == b.nDepth && memcmp(&a.vchFingerprint[0], &b.vchFingerprint[0], 4) == 0 && a.nChild == b.nChild &&
               memcmp(&a.vchChainCode[0], &b.vchChainCode[0], 32) == 0 && a.pubkey == b.pubkey;
    }

    void Encode(unsigned char code[74]) const;
    void Decode(const unsigned char code[74]);
    bool Derive(CExtPubKey &out, unsigned int nChild) const;
};

struct CExtKey {
    unsigned char nDepth;
    unsigned char vchFingerprint[4];
    unsigned int nChild;
    unsigned char vchChainCode[32];
    CKey key;

    friend bool operator==(const CExtKey &a, const CExtKey &b) {
        return a.nDepth == b.nDepth && memcmp(&a.vchFingerprint[0], &b.vchFingerprint[0], 4) == 0 && a.nChild == b.nChild &&
               memcmp(&a.vchChainCode[0], &b.vchChainCode[0], 32) == 0 && a.key == b.key;
    }

    void Encode(unsigned char code[74]) const;
    void Decode(const unsigned char code[74]);
    bool Derive(CExtKey &out, unsigned int nChild) const;
    CExtPubKey Neuter() const;
    void SetMaster(const unsigned char *seed, unsigned int nSeedLen);
};

/** Check that required EC support is available at runtime */
bool ECC_InitSanityCheck(void);

#endif
