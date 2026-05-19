// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2019-2022 The MasterWin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "libzerocoin/Params.h"
#include "chainparams.h"
#include "random.h"
#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

using namespace std;
using namespace boost::assign;

struct SeedSpec6 {
    uint8_t addr[0];
    uint16_t port;
};

#include "chainparamsseeds.h"

/**
 * Main network
 */

//! Convert the pnSeeds6 array into usable address objects.
static void convertSeed6(std::vector<CAddress>& vSeedsOut, const SeedSpec6* data, unsigned int count)
{
    // It'll only connect to one or two seed nodes because once it connects,
    // it'll get a pile of addresses with newer timestamps.
    // Seed nodes are given a random 'last seen time' of between one and two
    // weeks ago.
    const int64_t nOneWeek = 7 * 24 * 60 * 60;
    for (unsigned int i = 0; i < count; i++) {
        struct in6_addr ip;
        memcpy(&ip, data[i].addr, sizeof(ip));
        CAddress addr(CService(ip, data[i].port));
        addr.nTime = GetTime() - GetRand(nOneWeek) - nOneWeek;
        vSeedsOut.push_back(addr);
    }
}

//   What makes a good checkpoint block?
// + Is surrounded by blocks with reasonable timestamps
//   (no blocks before with a timestamp after, none after with
//    timestamp before)
// + Contains no strange transactions
static Checkpoints::MapCheckpoints mapCheckpoints =
    boost::assign::map_list_of
    (0, uint256("0x0000036c78e3d52adc852a690c253a04b60e23b14119076815a2e533ad9c108e"))
    (438420, uint256("0xa689d6da0612c100eea0d79a5ee63ebe2c249d3baf767c070abbec082fee58fa"))
    (824190, uint256("0x6d416e291dbdd62e76196e9e27ba75a01628d0789ca8e35f4d61bc2cbb38dafe"))
    (1350000, uint256("0xcb256e01a631770a2f6b63ca7c32668ca1c6076801d398a834f424c51fa3347d"));
static const Checkpoints::CCheckpointData data = {
    &mapCheckpoints,
    1687865214, // * UNIX timestamp of last checkpoint block
    2929157,		// * total number of transactions between genesis and last checkpoint
                //   (the tx=... number in the SetBestChain debug.log lines)
    3000        // * estimated number of transactions per day after checkpoint
};

static Checkpoints::MapCheckpoints mapCheckpointsTestnet =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataTestnet = {
    &mapCheckpointsTestnet,
    1576238080,
    0,
    250};

static Checkpoints::MapCheckpoints mapCheckpointsRegtest =
    boost::assign::map_list_of(0, uint256("0x001"));
static const Checkpoints::CCheckpointData dataRegtest = {
    &mapCheckpointsRegtest,
    1576238081,
    0,
    100};

libzerocoin::ZerocoinParams* CChainParams::Zerocoin_Params(bool useModulusV1) const
{
    assert(this);
    static CBigNum bnHexModulus = 0;
    if (!bnHexModulus)
        bnHexModulus.SetHex(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsHex = libzerocoin::ZerocoinParams(bnHexModulus);
    static CBigNum bnDecModulus = 0;
    if (!bnDecModulus)
        bnDecModulus.SetDec(zerocoinModulus);
    static libzerocoin::ZerocoinParams ZCParamsDec = libzerocoin::ZerocoinParams(bnDecModulus);

    if (useModulusV1)
        return &ZCParamsHex;

    return &ZCParamsDec;
}

class CMainParams : public CChainParams
{
public:
    CMainParams()
    {
        networkID = CBaseChainParams::MAIN;
        strNetworkID = "main";
        /**
         * The message start string is designed to be unlikely to occur in normal data.
         * The characters are rarely used upper ASCII, not valid as UTF-8, and produce
         * a large 4-byte int at any alignment.
         */
        pchMessageStart[0] = 0xb2;
        pchMessageStart[1] = 0x5e;
        pchMessageStart[2] = 0xca;
        pchMessageStart[3] = 0x1d;
		vAlertPubKey = ParseHex("049f9257493e65991c933096a0ed29d0465a8510142d919416eab2d6ad1fca029e2e3de4ca10d0d21fb655d9a3984476828742dba4aaffb349a3ec32c27a1d9bf5");
		nDefaultPort = 33555;
        bnProofOfWorkLimit = ~uint256(0) >> 20; // MasterWin starting difficulty is 1 / 2^12
        //nSubsidyHalvingInterval = 210000;
        nMaxReorganizationDepth = 100;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 0;
        nTargetTimespan = 30 * 60;
        nTargetSpacing = 1.5 * 60;  // MW: 1,5 minute
        nMaturity = 41;
        nMasternodeCountDrift = 20;
        nMaxMoneyOut = 100000000 * COIN;


        /** Height or Time Based Activations **/
        nLastPOWBlock = 100;
        nModifierUpdateBlock = 999999999;

        /** ===== MW v5 hard-fork: PoS-only + dev-fee, no masternodes =====
         *  Activation at block 2020328 -- the next block produced after the
         *  current stuck tip (2,020,327) on 2026-05-17. All blocks at and
         *  below 2,020,327 are validated by legacy v4.x consensus rules
         *  unchanged, so the historical chain is fully preserved.
         *  From 2,020,328 onward: PoW blocks rejected, masternode-payment
         *  enforcement gone, 25% dev-fee vout required in the coinstake. */
        nV5ForkHeight = 2020328;
        nV5DevFeePercent = 25;
        strV5DevFeeAddress = "MAzMpMaLr1Yxd2zzkNirNzeTGNp69q1XxM";

        nZerocoinStartHeight = 1576790167;
        nZerocoinStartTime = 1576775767;
        nBlockEnforceSerialRange = 253; //Enforce serial range starting this block
        nBlockRecalculateAccumulators = ~1; //Trigger a recalculation of accumulators
        nBlockFirstFraudulent = ~1; //First block that bad serials emerged
        nBlockLastGoodCheckpoint = ~1; //Last valid accumulator checkpoint
        nBlockEnforceInvalidUTXO = 999999999; //Start enforcing the invalid UTXO's
        nInvalidAmountFiltered = 0 * COIN; //Amount of invalid coins filtered through exchanges, that should be considered valid
        nBlockZerocoinV2 = 9999999; //!> The block that zerocoin v2 becomes active - roughly Tuesday, May 8, 2018 4:00:00 AM GMT
        nEnforceNewSporkKey = 1574428163; //!> Sporks signed after (GMT):  Wednesday, June 12, 2019 5:00:00 AM GMT must use the new spork key
        nRejectOldSporkKey = 1560394800; //!> Fully reject old spork key after (GMT): Thursday, June 13, 2019 3:00:00 AM

        /**
         * Build the genesis block. Note that the output of the genesis coinbase cannot
         * be spent as it did not originally exist in the database.
         *
         * CBlock(hash=00000ffd590b14, ver=1, hashPrevBlock=00000000000000, hashMerkleRoot=e0028e, nTime=1390095618, nBits=1e0ffff0, nNonce=28917698, vtx=1)
         *   CTransaction(hash=e0028e, ver=1, vin.size=1, vout.size=1, nLockTime=0)
         *     CTxIn(COutPoint(000000, -1), coinbase 04ffff001d01044c5957697265642030392f4a616e2f3230313420546865204772616e64204578706572696d656e7420476f6573204c6976653a204f76657273746f636b2e636f6d204973204e6f7720416363657074696e6720426974636f696e73)
         *     CTxOut(nValue=50.00000000, scriptPubKey=0xA9037BAC7050C479B121CF)
         *   vMerkleTree: e0028e
         */
        const char* pszTimestamp = ".Masternode with Tombola system.";
        CMutableTransaction txNew;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
		txNew.vout[0].SetEmpty();
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("047c924e95759e704db45a29476511168010cc49381378817e39eb3fa1a91331215d0a2adf5d361b3998bb8a71ee2a37bdbac6efb14e5ade6829ad83ba6f871d7c") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock = 0;
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 1;
        genesis.nTime = 1576775767;
        genesis.nBits = 0x1e0ffff0;
        genesis.nNonce = 7162664;

        hashGenesisBlock = genesis.GetHash();
        assert(hashGenesisBlock == uint256("0x0000036c78e3d52adc852a690c253a04b60e23b14119076815a2e533ad9c108e"));
        assert(genesis.hashMerkleRoot == uint256("0x307c2fcf0b20df9e82cc3b7595812899f1254cd9d70879c745db4a2029bbc946"));

        vSeeds.push_back(CDNSSeedData("1", "seed1.masterwin.online"));
	    vSeeds.push_back(CDNSSeedData("2", "seed2.masterwin.online"));
        vSeeds.push_back(CDNSSeedData("3", "seed3.masterwin.online"));
	    vSeeds.push_back(CDNSSeedData("4", "seed4.masterwin.online"));
	    vSeeds.push_back(CDNSSeedData("5", "seed5.masterwin.online"));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1,50); // Start with 'M'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(3, 17); 
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 29); // start with 'z'
		base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

		convertSeed6(vFixedSeeds, pnSeed6_main, ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fSkipProofOfWorkCheck = false;
        fTestnetToBeDeprecatedFieldRPC = false;
        fHeadersFirstSyncingActive = false;

        nPoolMaxTransactions = 3;
        strSporkKey = "0400994128f49c097b6911500a0f40fd6f0886f716d2f56100f375f3977c9efc7756271d36315abaa10cdf97a00c013d6efd8852d7da0b79becbff96436c05f419";
        strObfuscationPoolDummyAddress = "M8XZERVkVmBcr7XVVtErbRuERMh6kfuJd7";
        nStartMasternodePayments = 1576849275; // Wednesday, June 12, 2019 8:00:00 AM GMT

        /** Zerocoin */
        zerocoinModulus = "25195908475657893494027183240048398571429282126204032027777137836043662020707595556264018525880784"
            "4069182906412495150821892985591491761845028084891200728449926873928072877767359714183472702618963750149718246911"
            "6507761337985909570009733045974880842840179742910064245869181719511874612151517265463228221686998754918242243363"
            "7259085141865462043576798423387184774447920739934236584823824281198163815010674810451660377306056201619676256133"
            "8441436038339044149526344321901146575444541784240209246165157233507787077498171257724679629263863563732899121548"
            "31438167899885040445364023527381951378636564391212010397122822120720357";
        nMaxZerocoinSpendsPerTransaction = 7; // Assume about 20kb each
        nMinZerocoinMintFee = 1 * CENT; //high fee required for zerocoin mints
        nMintRequiredConfirmations = 20; //the maximum amount of confirmations until accumulated in 19
        nRequiredAccumulation = 1;
        nDefaultSecurityLevel = 100; //full security level for accumulators
        nZerocoinHeaderVersion = 4; //Block headers must be this version once zerocoin is active
        nZerocoinRequiredStakeDepth = 200; //The required confirmations for a zmw to be stakable

        nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee
        blacklistedAddresses = std::set<std::string>{
            "MUqGH9fvKxjSSJhLQuKMM1yD8EsuZnc6P3",
        };
        
    vMasternodeTiers.push_back ({
      1,
      {
        {   4000 * COIN, 1 }
      }
    });
	
    vMasternodeTiers.push_back ({
      420000,
      {
        { 4000 * COIN, 2 },
        { 15000 * COIN, 7 },
        { 30000 * COIN, 16 },
        { 60000 * COIN, 28 },
        { 100000 * COIN, 47 }
      }
    });

    vMasternodeTiers.push_back ({
      450000,
      {
        {15000 * COIN, 4 },
        {30000 * COIN, 8 },
        {60000 * COIN, 18 },
        {100000 * COIN, 28 },
        {150000 * COIN, 42 }
      }
    });

    vMasternodeTiers.push_back ({
      840000,
      {
        { 15000 * COIN, 65 },
        { 30000 * COIN, 0 },
        { 60000 * COIN, 0 },
        { 100000 * COIN, 0 },
        { 125000 * COIN, 0 },
        { 150000 * COIN, 0 },
        { 175000 * COIN, 0 },
        { 200000 * COIN, 0 }
      }
    });

    vMasternodeTiers.push_back ({
      860000,
      {
        { 15000 * COIN, 0 },
        { 30000 * COIN, 65 },
        { 60000 * COIN, 0 },
        { 100000 * COIN, 0 },
        { 125000 * COIN, 0 },
        { 150000 * COIN, 0 },
        { 175000 * COIN, 0 },
        { 200000 * COIN, 0 }
      }
    });

    vMasternodeTiers.push_back ({
      880000,
      {
        { 15000 * COIN, 0 },
        { 30000 * COIN, 0 },
        { 60000 * COIN, 65 },
        { 100000 * COIN, 0 },
        { 125000 * COIN, 0 },
        { 150000 * COIN, 0 },
        { 175000 * COIN, 0 },
        { 200000 * COIN, 0 }
      }
    });

    vMasternodeTiers.push_back ({
      900000,
      {
        { 15000 * COIN, 0 },
        { 30000 * COIN, 0 },
        { 60000 * COIN, 0 },
        { 100000 * COIN, 65 },
        { 125000 * COIN, 0 },
        { 150000 * COIN, 0 },
        { 175000 * COIN, 0 },
        { 200000 * COIN, 0 }
      }
    });

    vMasternodeTiers.push_back ({
      925000,
      {
        { 15000 * COIN, 0 },
        { 30000 * COIN, 0 },
        { 60000 * COIN, 0 },
        { 100000 * COIN, 0 },
        { 125000 * COIN, 65 },
        { 150000 * COIN, 0 },
        { 175000 * COIN, 0 },
        { 200000 * COIN, 0 }
      }
    });

    vMasternodeTiers.push_back ({
      950000,
      {
        { 15000 * COIN, 0 },
        { 30000 * COIN, 0 },
        { 60000 * COIN, 0 },
        { 100000 * COIN, 0 },
        { 125000 * COIN, 0 },
        { 150000 * COIN, 65 },
        { 175000 * COIN, 0 },
        { 200000 * COIN, 0 }
      }
    });

    vMasternodeTiers.push_back ({
      975000,
      {
        { 15000 * COIN, 0 },
        { 30000 * COIN, 0 },
        { 60000 * COIN, 0 },
        { 100000 * COIN, 0 },
        { 125000 * COIN, 0 },
        { 150000 * COIN, 0 },
        { 175000 * COIN, 65 },
        { 200000 * COIN, 0 }
      }
    });

    vMasternodeTiers.push_back ({
      1000000,
      {
        { 15000 * COIN, 0 },
        { 30000 * COIN, 0 },
        { 60000 * COIN, 0 },
        { 100000 * COIN, 0 },
        { 125000 * COIN, 0 },
        { 150000 * COIN, 0 },
        { 175000 * COIN, 0 },
        { 200000 * COIN, 65 }
      }
    });

    vMasternodeTiers.push_back ({
      1000100,
      {
        { 200000 * COIN, 65 }
      }
    });
  }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return data;
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams
{
public:
    CTestNetParams()
    {
        networkID = CBaseChainParams::TESTNET;
        strNetworkID = "test";
        pchMessageStart[0] = 0x43;
        pchMessageStart[1] = 0x76;
        pchMessageStart[2] = 0x65;
        pchMessageStart[3] = 0xba;
		vAlertPubKey = ParseHex("0478f8fed74015e8e7899ddd7221fd7e5a9f5ffe3b97d43aac68535b3b0b449a4675ebc3b4b49433a2b70da9308206a594d35ed89334b949021a9f775a98a83480");
		nDefaultPort = 33538;
        nEnforceBlockUpgradeMajority = 51;
        nRejectBlockOutdatedMajority = 75;
        nToCheckBlockUpgradeMajority = 100;
        nMinerThreads = 0;
        nTargetTimespan = 1 * 60; // MasterWin: 1 day
        nTargetSpacing = 1 * 60;  // MasterWin: 1 minute
        nLastPOWBlock = 200;
        // v5 hard-fork inherits mainnet defaults on testnet/regtest (just init);
        // mainnet's values are the canonical ones for production.
        nV5ForkHeight = 999999999;
        nV5DevFeePercent = 25;
        strV5DevFeeAddress = "MAzMpMaLr1Yxd2zzkNirNzeTGNp69q1XxM";
        nMaturity = 15;
        nMasternodeCountDrift = 4;
        nModifierUpdateBlock = 51197; //approx Mon, 17 Apr 2017 04:00:00 GMT
        nMaxMoneyOut = 43199500 * COIN;
        nZerocoinStartHeight = 201576;
        nZerocoinStartTime = 1501776000;
        nBlockEnforceSerialRange = 1; //Enforce serial range starting this block
        nBlockRecalculateAccumulators = 9908000; //Trigger a recalculation of accumulators
        nBlockFirstFraudulent = 9891737; //First block that bad serials emerged
        nBlockLastGoodCheckpoint = 9891730; //Last valid accumulator checkpoint
        nBlockEnforceInvalidUTXO = 9902850; //Start enforcing the invalid UTXO's
        nInvalidAmountFiltered = 0; //Amount of invalid coins filtered through exchanges, that should be considered valid
        nBlockZerocoinV2 = 444020; //!> The block that zerocoin v2 becomes active
        nEnforceNewSporkKey = 1521604800; //!> Sporks signed after Wednesday, March 21, 2018 4:00:00 AM GMT must use the new spork key
        nRejectOldSporkKey = 1522454400; //!> Reject old spork key after Saturday, March 31, 2018 12:00:00 AM GMT

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1576238085;
        genesis.nNonce = 2402015;

        hashGenesisBlock = genesis.GetHash();
        //assert(hashGenesisBlock == uint256("0x0000041e482b9b9691d98eefb48473405c0b8ec31b76df3797c74a78680ef818"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("", ""));

        base58Prefixes[PUBKEY_ADDRESS] = std::vector<unsigned char>(1, 38); // Testnet mwx addresses start with 'g' or 'y'
        base58Prefixes[SCRIPT_ADDRESS] = std::vector<unsigned char>(1, 19);  // Testnet mwx script addresses start with '8' or '9'
        base58Prefixes[SECRET_KEY] = std::vector<unsigned char>(1, 239);     // Testnet private keys start with '9' or 'c' (Bitcoin defaults)
        // Testnet mwx BIP32 pubkeys start with 'DRKV'
        base58Prefixes[EXT_PUBLIC_KEY] = boost::assign::list_of(0x3a)(0x80)(0x61)(0xa0).convert_to_container<std::vector<unsigned char> >();
        // Testnet mwx BIP32 prvkeys start with 'DRKP'
        base58Prefixes[EXT_SECRET_KEY] = boost::assign::list_of(0x3a)(0x80)(0x58)(0x37).convert_to_container<std::vector<unsigned char> >();
        // Testnet mwx BIP44 coin type is '1' (All coin's testnet default)
        base58Prefixes[EXT_COIN_TYPE] = boost::assign::list_of(0x80)(0x00)(0x00)(0x01).convert_to_container<std::vector<unsigned char> >();

		convertSeed6(vFixedSeeds, pnSeed6_test, ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        nPoolMaxTransactions = 2;
        strSporkKey = "0400994128f49c097b6911500a0f40fd6f0886f716d2f56100f375f3977c9efc7756271d36315abaa10cdf97a00c013d6efd8852d7da0b79becbff96436c05f419";
        strObfuscationPoolDummyAddress = "M8XZERVkVmBcr7XVVtErbRuERMh6kfuJd7";
        nStartMasternodePayments = 1576238087; //Fri, 09 Jan 2015 21:05:58 GMT
        nBudget_Fee_Confirmations = 3; // Number of confirmations for the finalization fee. We have to make this very short
                                       // here because we only have a 8 block finalization window on testnet
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataTestnet;
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams
{
public:
    CRegTestParams()
    {
        networkID = CBaseChainParams::REGTEST;
        strNetworkID = "regtest";
        strNetworkID = "regtest";
        pchMessageStart[0] = 0x69;
        pchMessageStart[1] = 0xcf;
        pchMessageStart[2] = 0x7e;
        pchMessageStart[3] = 0xac;
        //nSubsidyHalvingInterval = 150;
        nEnforceBlockUpgradeMajority = 750;
        nRejectBlockOutdatedMajority = 950;
        nToCheckBlockUpgradeMajority = 1000;
        nMinerThreads = 1;
        nTargetTimespan = 24 * 60 * 60; // MasterWin: 1 day
        nTargetSpacing = 1 * 60;        // MasterWin: 1 minutes
        bnProofOfWorkLimit = ~uint256(0) >> 1;
        genesis.nTime = 1576238186;
        genesis.nBits = 0x207fffff;
        genesis.nNonce = 2402015;

        hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 33535;
        //assert(hashGenesisBlock == uint256("0x4f023a2120d9127b21bbad01724fdb79b519f593f2a85b60d3d79160ec5f29df"));

        vFixedSeeds.clear(); //! Testnet mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Testnet mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fAllowMinDifficultyBlocks = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = false;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;
    }
    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        return dataRegtest;
    }
};
static CRegTestParams regTestParams;

/**
 * Unit test
 */
class CUnitTestParams : public CMainParams, public CModifiableParams
{
public:
    CUnitTestParams()
    {
        networkID = CBaseChainParams::UNITTEST;
        strNetworkID = "unittest";
        nDefaultPort = 24094;
        vFixedSeeds.clear(); //! Unit test mode doesn't have any fixed seeds.
        vSeeds.clear();      //! Unit test mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fAllowMinDifficultyBlocks = false;
        fMineBlocksOnDemand = true;
    }

    const Checkpoints::CCheckpointData& Checkpoints() const
    {
        // UnitTest share the same checkpoints as MAIN
        return data;
    }

    //! Published setters to allow changing values in unit test cases
    //virtual void setSubsidyHalvingInterval(int anSubsidyHalvingInterval) { nSubsidyHalvingInterval = anSubsidyHalvingInterval; }
    virtual void setEnforceBlockUpgradeMajority(int anEnforceBlockUpgradeMajority) { nEnforceBlockUpgradeMajority = anEnforceBlockUpgradeMajority; }
    virtual void setRejectBlockOutdatedMajority(int anRejectBlockOutdatedMajority) { nRejectBlockOutdatedMajority = anRejectBlockOutdatedMajority; }
    virtual void setToCheckBlockUpgradeMajority(int anToCheckBlockUpgradeMajority) { nToCheckBlockUpgradeMajority = anToCheckBlockUpgradeMajority; }
    virtual void setDefaultConsistencyChecks(bool afDefaultConsistencyChecks) { fDefaultConsistencyChecks = afDefaultConsistencyChecks; }
    virtual void setAllowMinDifficultyBlocks(bool afAllowMinDifficultyBlocks) { fAllowMinDifficultyBlocks = afAllowMinDifficultyBlocks; }
    virtual void setSkipProofOfWorkCheck(bool afSkipProofOfWorkCheck) { fSkipProofOfWorkCheck = afSkipProofOfWorkCheck; }
};
static CUnitTestParams unitTestParams;


static CChainParams* pCurrentParams = 0;

CModifiableParams* ModifiableParams()
{
    assert(pCurrentParams);
    assert(pCurrentParams == &unitTestParams);
    return (CModifiableParams*)&unitTestParams;
}

const CChainParams& Params()
{
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams& Params(CBaseChainParams::Network network)
{
    switch (network) {
    case CBaseChainParams::MAIN:
        return mainParams;
    case CBaseChainParams::TESTNET:
        return testNetParams;
    case CBaseChainParams::REGTEST:
        return regTestParams;
    case CBaseChainParams::UNITTEST:
        return unitTestParams;
    default:
        assert(false && "Unimplemented network");
        return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network)
{
    SelectBaseParams(network);
    pCurrentParams = &Params(network);
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
