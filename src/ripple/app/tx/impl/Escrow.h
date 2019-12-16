//------------------------------------------------------------------------------
/*
    
    Copyright (c) 2012, 2013 Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#ifndef RIPPLE_TX_ESCROW_H_INCLUDED
#define RIPPLE_TX_ESCROW_H_INCLUDED

#include <ripple/app/tx/impl/Transactor.h>

namespace ripple {

class EscrowCreate
    : public Transactor
{
public:
    explicit
    EscrowCreate (ApplyContext& ctx)
        : Transactor(ctx)
    {
    }

    static
    XRPAmount
    calculateMaxSpend(STTx const& tx);

    static
    NotTEC
    preflight (PreflightContext const& ctx);

    TER
    doApply() override;
};

//------------------------------------------------------------------------------

class EscrowFinish
    : public Transactor
{
public:
    explicit
    EscrowFinish (ApplyContext& ctx)
        : Transactor(ctx)
    {
    }

    static
    NotTEC
    preflight (PreflightContext const& ctx);

    static
    std::uint64_t
    calculateBaseFee (
        ReadView const& view,
        STTx const& tx);

    TER
    doApply() override;
};

//------------------------------------------------------------------------------

class EscrowCancel
    : public Transactor
{
public:
    explicit
    EscrowCancel (ApplyContext& ctx)
        : Transactor(ctx)
    {
    }

    static
    NotTEC
    preflight (PreflightContext const& ctx);

    TER
    doApply() override;
};

} // ripple

#endif
