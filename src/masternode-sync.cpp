// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2019-2022 The MasterWin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

// clang-format off
#include "main.h"
#include "activemasternode.h"
#include "masternode-sync.h"
#include "masternode-payments.h"
#include "masternode-budget.h"
#include "masternode.h"
#include "masternodeman.h"
#include "spork.h"
#include "util.h"
#include "addrman.h"
// clang-format on

class CMasternodeSync;
CMasternodeSync masternodeSync;

CMasternodeSync::CMasternodeSync()
{
    Reset();
}

bool CMasternodeSync::IsSynced()
{
    return RequestedMasternodeAssets == MASTERNODE_SYNC_FINISHED;
}

bool CMasternodeSync::IsBlockchainSynced()
{
    static bool fBlockchainSynced = false;
    static int64_t lastProcess = GetTime();

    // if the last call to this function was more than 60 minutes ago (client was in sleep mode) reset the sync process
    if (GetTime() - lastProcess > 60 * 60) {
        Reset();
        fBlockchainSynced = false;
    }
    lastProcess = GetTime();

    if (fBlockchainSynced) return true;

    if (fImporting || fReindex) return false;

    TRY_LOCK(cs_main, lockMain);
    if (!lockMain) return false;

    CBlockIndex* pindex = chainActive.Tip();
    if (pindex == NULL) return false;


    // Bypass the "tip younger than 1 hour" gate when EITHER:
    //   a) `-bypassmnsync=1` is set explicitly, OR
    //   b) the chain has crossed the MW v5 fork height (post-v5 the
    //      masternode-sync FSM is functionally obsolete).
    // Without one of these, an abandoned-coin scenario locks staking out
    // permanently: stale tip -> IsBlockchainSynced=false -> mnsync stuck ->
    // no stake produced -> tip stays stale forever -> deadlock.
    // Behaviour change is purely local; the block produced is still
    // validated by every other node by the regular consensus rules.
    bool fV5Active = Params().IsV5Active(pindex->nHeight + 1);
    if (!fV5Active && !GetBoolArg("-bypassmnsync", false)) {
        if (pindex->nTime + 60 * 60 < GetTime())
            return false;
    }

    fBlockchainSynced = true;

    return true;
}

void CMasternodeSync::Reset()
{
    lastMasternodeList = 0;
    lastMasternodeWinner = 0;
    lastBudgetItem = 0;
    mapSeenSyncMNB.clear();
    mapSeenSyncMNW.clear();
    mapSeenSyncBudget.clear();
    lastFailure = 0;
    nCountFailures = 0;
    sumMasternodeList = 0;
    sumMasternodeWinner = 0;
    sumBudgetItemProp = 0;
    sumBudgetItemFin = 0;
    countMasternodeList = 0;
    countMasternodeWinner = 0;
    countBudgetItemProp = 0;
    countBudgetItemFin = 0;
    RequestedMasternodeAssets = MASTERNODE_SYNC_INITIAL;
    RequestedMasternodeAttempt = 0;
    nAssetSyncStarted = GetTime();
}

void CMasternodeSync::AddedMasternodeList(uint256 hash)
{
    if (mnodeman.mapSeenMasternodeBroadcast.count(hash)) {
        if (mapSeenSyncMNB[hash] < MASTERNODE_SYNC_THRESHOLD) {
            lastMasternodeList = GetTime();
            mapSeenSyncMNB[hash]++;
        }
    } else {
        lastMasternodeList = GetTime();
        mapSeenSyncMNB.insert(make_pair(hash, 1));
    }
}

void CMasternodeSync::AddedMasternodeWinner(uint256 hash)
{
    if (masternodePayments.mapMasternodePayeeVotes.count(hash)) {
        if (mapSeenSyncMNW[hash] < MASTERNODE_SYNC_THRESHOLD) {
            lastMasternodeWinner = GetTime();
            mapSeenSyncMNW[hash]++;
        }
    } else {
        lastMasternodeWinner = GetTime();
        mapSeenSyncMNW.insert(make_pair(hash, 1));
    }
}

void CMasternodeSync::AddedBudgetItem(uint256 hash)
{
    if (budget.mapSeenMasternodeBudgetProposals.count(hash) || budget.mapSeenMasternodeBudgetVotes.count(hash) ||
        budget.mapSeenFinalizedBudgets.count(hash) || budget.mapSeenFinalizedBudgetVotes.count(hash)) {
        if (mapSeenSyncBudget[hash] < MASTERNODE_SYNC_THRESHOLD) {
            lastBudgetItem = GetTime();
            mapSeenSyncBudget[hash]++;
        }
    } else {
        lastBudgetItem = GetTime();
        mapSeenSyncBudget.insert(make_pair(hash, 1));
    }
}

bool CMasternodeSync::IsBudgetPropEmpty()
{
    return sumBudgetItemProp == 0 && countBudgetItemProp > 0;
}

bool CMasternodeSync::IsBudgetFinEmpty()
{
    return sumBudgetItemFin == 0 && countBudgetItemFin > 0;
}

void CMasternodeSync::GetNextAsset()
{
    switch (RequestedMasternodeAssets) {
    case (MASTERNODE_SYNC_INITIAL):
    case (MASTERNODE_SYNC_FAILED): // should never be used here actually, use Reset() instead
        ClearFulfilledRequest();
        RequestedMasternodeAssets = MASTERNODE_SYNC_SPORKS;
        break;
    case (MASTERNODE_SYNC_SPORKS):
        RequestedMasternodeAssets = MASTERNODE_SYNC_LIST;
        break;
    case (MASTERNODE_SYNC_LIST):
        RequestedMasternodeAssets = MASTERNODE_SYNC_MNW;
        break;
    case (MASTERNODE_SYNC_MNW):
        RequestedMasternodeAssets = MASTERNODE_SYNC_BUDGET;
        break;
    case (MASTERNODE_SYNC_BUDGET):
        LogPrintf("CMasternodeSync::GetNextAsset - Sync has finished\n");
        RequestedMasternodeAssets = MASTERNODE_SYNC_FINISHED;
        break;
    }
    RequestedMasternodeAttempt = 0;
    nAssetSyncStarted = GetTime();
}

std::string CMasternodeSync::GetSyncStatus()
{
    // MasterWin v5: Process() pins the state to FINISHED immediately so
    // only INITIAL (the first scheduler tick) and FINISHED are reachable
    // in practice.  The remaining cases stay around as documentation /
    // safety nets and are rephrased to drop the masternode / budget
    // terminology that no longer applies in v5.
    switch (masternodeSync.RequestedMasternodeAssets) {
    case MASTERNODE_SYNC_INITIAL:
        return _("Initializing...");
    case MASTERNODE_SYNC_SPORKS:
        return _("Synchronizing sporks...");
    case MASTERNODE_SYNC_LIST:
    case MASTERNODE_SYNC_MNW:
    case MASTERNODE_SYNC_BUDGET:
        return _("Synchronizing legacy peer data...");
    case MASTERNODE_SYNC_FAILED:
        return _("Synchronization failed");
    case MASTERNODE_SYNC_FINISHED:
        return _("Up to date");
    }
    return "";
}

void CMasternodeSync::ProcessMessage(CNode* pfrom, std::string& strCommand, CDataStream& vRecv)
{
    if (strCommand == "ssc") { //Sync status count
        int nItemID;
        int nCount;
        vRecv >> nItemID >> nCount;

        if (RequestedMasternodeAssets >= MASTERNODE_SYNC_FINISHED) return;

        //this means we will receive no further communication
        switch (nItemID) {
        case (MASTERNODE_SYNC_LIST):
            if (nItemID != RequestedMasternodeAssets) return;
            sumMasternodeList += nCount;
            countMasternodeList++;
            break;
        case (MASTERNODE_SYNC_MNW):
            if (nItemID != RequestedMasternodeAssets) return;
            sumMasternodeWinner += nCount;
            countMasternodeWinner++;
            break;
        case (MASTERNODE_SYNC_BUDGET_PROP):
            if (RequestedMasternodeAssets != MASTERNODE_SYNC_BUDGET) return;
            sumBudgetItemProp += nCount;
            countBudgetItemProp++;
            break;
        case (MASTERNODE_SYNC_BUDGET_FIN):
            if (RequestedMasternodeAssets != MASTERNODE_SYNC_BUDGET) return;
            sumBudgetItemFin += nCount;
            countBudgetItemFin++;
            break;
        }

        LogPrint("masternode", "CMasternodeSync:ProcessMessage - ssc - got inventory count %d %d\n", nItemID, nCount);
    }
}

void CMasternodeSync::ClearFulfilledRequest()
{
    TRY_LOCK(cs_vNodes, lockRecv);
    if (!lockRecv) return;

    BOOST_FOREACH (CNode* pnode, vNodes) {
        pnode->ClearFulfilledRequest("getspork");
        pnode->ClearFulfilledRequest("mnsync");
        pnode->ClearFulfilledRequest("mnwsync");
        pnode->ClearFulfilledRequest("busync");
    }
}

void CMasternodeSync::Process()
{
    static int tick = 0;

    if (tick++ % MASTERNODE_SYNC_TIMEOUT != 0) return;

    // MasterWin v5: masternodes/budgets are deprecated. SPORK_8/9/10/13
    // are off at the consensus layer; no peer gossips a masternode list,
    // winners, or budget items, so every legacy mnsync stage just times
    // out after 25s.  Worse, when the state machine finally reaches
    // FINISHED, the "Resync if we lost all masternodes" branch below sees
    // CountEnabled() == 0 (always true in v5), calls Reset(), and kicks
    // the whole dance back to INITIAL -- stranding the wallet at ~50% on
    // the "Synchronizing additional data" progress bar indefinitely.
    //
    // Pin the state machine at FINISHED so IsSynced() returns true the
    // moment the blockchain finishes syncing.  Sporks still propagate
    // via normal gossip independently of this state machine.
    if (RequestedMasternodeAssets != MASTERNODE_SYNC_FINISHED) {
        RequestedMasternodeAssets = MASTERNODE_SYNC_FINISHED;
        RequestedMasternodeAttempt = 0;
        nAssetSyncStarted = GetTime();
    }
}
