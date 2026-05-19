// Copyright (c) 2012-2014 The Bitcoin developers
// Copyright (c) 2014-2015 The Dash developers
// Copyright (c) 2015-2018 The PIVX developers
// Copyright (c) 2019-2022 The MasterWin developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_VERSION_H
#define BITCOIN_VERSION_H

/**
 * network protocol versioning
 */

// v5 Neon Edition: bump protocol version so old v4.x nodes are rejected
// from the v5 mesh. Once block 2,020,328 is mined under v5 rules, the
// hard-fork is irreversible -- but we additionally cut v4 nodes off at
// the wire level so they cannot keep gossiping the dead v4 chain through
// the v5 network.
static const int PROTOCOL_VERSION = 71000;

//! initial proto version, to be increased after version/verack negotiation
static const int INIT_PROTO_VERSION = 209;

//! In this version, 'getheaders' was introduced.
static const int GETHEADERS_VERSION = 70077;

//! disconnect from peers older than this proto version
//! v5 requires 71000 -- any node still on the old v4 protocol (70922) is
//! considered legacy and dropped immediately on version handshake.
static const int MIN_PEER_PROTO_VERSION_BEFORE_ENFORCEMENT = 71000;
static const int MIN_PEER_PROTO_VERSION_AFTER_ENFORCEMENT = 71000;

static const int MIN_PEER_PROTO_VERSION_MNW_VIN = 71000;

//! masternodes older than this proto version use old strMessage format for mnannounce
static const int MIN_PEER_MNANNOUNCE = 70915;

//! nTime field added to CAddress, starting with this version;
//! if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = 31402;

//! BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = 60000;

//! "mempool" command, enhanced "getdata" behavior starts with this version
static const int MEMPOOL_GD_VERSION = 60002;

//! "filter*" commands are disabled without NODE_BLOOM after and including this version
static const int NO_BLOOM_VERSION = 70005;


#endif // BITCOIN_VERSION_H
