// Copyright (c) 2022-2024 The MetabaseNet developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef METABASENET_ERROR_H
#define METABASENET_ERROR_H

namespace metabasenet
{

typedef enum
{
    FAILED = -1,
    OK = 0,
    /* moduler */
    ERR_UNAVAILABLE,
    /* container */
    ERR_NOT_FOUND,
    ERR_ALREADY_HAVE,
    ERR_MISSING_PREV,
    /* system */
    ERR_SYS_DATABASE_ERROR,
    ERR_SYS_OUT_OF_DISK_SPACE,
    ERR_SYS_STORAGE_ERROR,
    ERR_SYS_OUT_OF_MEMORY,
    /* block */
    ERR_BLOCK_TYPE_INVALID,
    ERR_BLOCK_OVERSIZE,
    ERR_BLOCK_PROOF_OF_WORK_INVALID,
    ERR_BLOCK_PROOF_OF_STAKE_INVALID,
    ERR_BLOCK_TIMESTAMP_OUT_OF_RANGE,
    ERR_BLOCK_COINBASE_INVALID,
    ERR_BLOCK_COINSTAKE_INVALID,
    ERR_BLOCK_TRANSACTIONS_INVALID,
    ERR_BLOCK_DUPLICATED_TRANSACTION,
    ERR_BLOCK_SIGOPCOUNT_OUT_OF_BOUND,
    ERR_BLOCK_TXHASH_MISMATCH,
    ERR_BLOCK_SIGNATURE_INVALID,
    ERR_BLOCK_INVALID_FORK,
    ERR_BLOCK_CERTTX_OUT_OF_BOUND,
    /* transaction */
    ERR_TRANSACTION_INVALID,
    ERR_TRANSACTION_OVERSIZE,
    ERR_TRANSACTION_OUTPUT_INVALID,
    ERR_TRANSACTION_INPUT_INVALID,
    ERR_TRANSACTION_TIMESTAMP_INVALID,
    ERR_TRANSACTION_NOT_ENOUGH_FEE,
    ERR_TRANSACTION_STAKE_REWARD_INVALID,
    ERR_TRANSACTION_SIGNATURE_INVALID,
    ERR_TRANSACTION_CONFLICTING_INPUT,
    ERR_TRANSACTION_TOO_MANY_CERTTX,
    ERR_TRANSACTION_IS_LOCKED,
    ERR_TRANSACTION_AT_BLACKLIST,
    /* wallet */
    ERR_WALLET_INVALID_AMOUNT,
    ERR_WALLET_INSUFFICIENT_FUNDS,
    ERR_WALLET_SIGNATURE_FAILED,
    ERR_WALLET_TX_OVERSIZE,
    ERR_WALLET_NOT_FOUND,
    ERR_WALLET_IS_LOCKED,
    ERR_WALLET_IS_UNLOCKED,
    ERR_WALLET_IS_ENCRYPTED,
    ERR_WALLET_IS_UNENCRYPTED,
    ERR_WALLET_FAILED,
    /* count */
    ERR_MAX_COUNT
} Errno;

extern const char* ErrorString(const Errno& err);

} // namespace metabasenet

#endif //METABASENET_ERROR_H
