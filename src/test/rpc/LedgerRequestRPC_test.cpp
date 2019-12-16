//------------------------------------------------------------------------------
/*
    
    Copyright (c) 2012-2016 Ripple Labs Inc.
    Copyright (c) 2019 Ripple Alpha Association.

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

#include <ripple/protocol/ErrorCodes.h>
#include <ripple/protocol/jss.h>
#include <test/jtx.h>
#include <ripple/beast/unit_test.h>
#include <ripple/app/ledger/LedgerMaster.h>

namespace ripple {

namespace RPC {

class LedgerRequestRPC_test : public beast::unit_test::suite
{
public:

    void testLedgerRequest()
    {
        using namespace test::jtx;

        Env env(*this);

        env.close();
        env.close();
        BEAST_EXPECT(env.current()->info().seq == 5);

        {
            // arbitrary text is converted to 0.
            auto const result = env.rpc("ledger_request", "arbitrary_text");
            BEAST_EXPECT(RPC::contains_error(result[jss::result]) &&
                result[jss::result][jss::error_message] ==
                "Ledger index too small");
        }

        {
            auto const result = env.rpc("ledger_request", "-1");
            BEAST_EXPECT(RPC::contains_error(result[jss::result]) &&
                result[jss::result][jss::error_message] ==
                "Ledger index too small");
        }

        {
            auto const result = env.rpc("ledger_request", "0");
            BEAST_EXPECT(RPC::contains_error(result[jss::result]) &&
                result[jss::result][jss::error_message] ==
                    "Ledger index too small");
        }

        {
            auto const result = env.rpc("ledger_request", "1");
            BEAST_EXPECT(!RPC::contains_error(result[jss::result]) &&
                result[jss::result][jss::ledger_index] == 1 &&
                    result[jss::result].isMember(jss::ledger));
            BEAST_EXPECT(result[jss::result][jss::ledger].
                isMember(jss::ledger_hash) &&
                    result[jss::result][jss::ledger]
                        [jss::ledger_hash].isString());
        }

        {
            auto const result = env.rpc("ledger_request", "2");
            BEAST_EXPECT(!RPC::contains_error(result[jss::result]) &&
                result[jss::result][jss::ledger_index] == 2 &&
                    result[jss::result].isMember(jss::ledger));
            BEAST_EXPECT(result[jss::result][jss::ledger].
                isMember(jss::ledger_hash) &&
                    result[jss::result][jss::ledger]
                        [jss::ledger_hash].isString());
        }

        {
            auto const result = env.rpc("ledger_request", "3");
            BEAST_EXPECT(!RPC::contains_error(result[jss::result]) &&
                result[jss::result][jss::ledger_index] == 3 &&
                    result[jss::result].isMember(jss::ledger));
            BEAST_EXPECT(result[jss::result][jss::ledger].
                isMember(jss::ledger_hash) &&
                    result[jss::result][jss::ledger]
                        [jss::ledger_hash].isString());

            auto const ledgerHash = result[jss::result]
                [jss::ledger][jss::ledger_hash].asString();

            {
                auto const r = env.rpc("ledger_request", ledgerHash);
                BEAST_EXPECT(!RPC::contains_error(r[jss::result]) &&
                    r[jss::result][jss::ledger_index] == 3 &&
                        r[jss::result].isMember(jss::ledger));
                BEAST_EXPECT(r[jss::result][jss::ledger].
                    isMember(jss::ledger_hash) &&
                        r[jss::result][jss::ledger]
                            [jss::ledger_hash] == ledgerHash);
            }
        }

        {
            std::string ledgerHash(64, 'q');

            auto const result = env.rpc("ledger_request", ledgerHash);

            BEAST_EXPECT(RPC::contains_error(result[jss::result]) &&
                result[jss::result][jss::error_message] ==
                    "Invalid field 'ledger_hash'.");
        }

        {
            std::string ledgerHash(64, '1');

            auto const result = env.rpc("ledger_request", ledgerHash);

            BEAST_EXPECT(!RPC::contains_error(result[jss::result]) &&
                result[jss::result][jss::have_header] == false);
        }

        {
            auto const result = env.rpc("ledger_request", "4");
            BEAST_EXPECT(RPC::contains_error(result[jss::result]) &&
                result[jss::result][jss::error_message] ==
                    "Ledger index too large");
        }

        {
            auto const result = env.rpc("ledger_request", "5");
            BEAST_EXPECT(RPC::contains_error(result[jss::result]) &&
                result[jss::result][jss::error_message] ==
                    "Ledger index too large");
        }

    }

    void testEvolution()
    {
        using namespace test::jtx;
        Env env {*this, FeatureBitset{}}; //the hashes being checked below assume
                                     //no amendments
        Account const gw { "gateway" };
        auto const USD = gw["USD"];
        env.fund(XRP(100000), gw);
        env.close();

        env.memoize("bob");
        env.fund(XRP(1000), "bob");
        env.close();

        env.memoize("alice");
        env.fund(XRP(1000), "alice");
        env.close();

        env.memoize("carol");
        env.fund(XRP(1000), "carol");
        env.close();

        auto result = env.rpc ( "ledger_request", "1" ) [jss::result];
        BEAST_EXPECT(result[jss::ledger][jss::ledger_index]     == "1");
        BEAST_EXPECT(result[jss::ledger][jss::total_coins]      == "5000000000000000");
        BEAST_EXPECT(result[jss::ledger][jss::closed]           == true);
        BEAST_EXPECT(result[jss::ledger][jss::ledger_hash]      == "DE92CBEF48579C7189BC7DF4BBD71735B02496E6EA96FA4BFB967DD68A6F99ED");
        BEAST_EXPECT(result[jss::ledger][jss::parent_hash]      == "0000000000000000000000000000000000000000000000000000000000000000");
        BEAST_EXPECT(result[jss::ledger][jss::account_hash]     == "263C2AF60F9419085699C59DA1FB66D2F1E6DFEA8A2176F154367DEF905B3319");
        BEAST_EXPECT(result[jss::ledger][jss::transaction_hash] == "0000000000000000000000000000000000000000000000000000000000000000");

        result = env.rpc ( "ledger_request", "2" ) [jss::result];
        BEAST_EXPECT(result[jss::ledger][jss::ledger_index]     == "2");
        BEAST_EXPECT(result[jss::ledger][jss::total_coins]      == "5000000000000000");
        BEAST_EXPECT(result[jss::ledger][jss::closed]           == true);
        BEAST_EXPECT(result[jss::ledger][jss::ledger_hash]      == "C541ADD67498DD880FF069B2F1FEE3B08620BBC3F14D4F890FD2828051791853");
        BEAST_EXPECT(result[jss::ledger][jss::parent_hash]      == "DE92CBEF48579C7189BC7DF4BBD71735B02496E6EA96FA4BFB967DD68A6F99ED");
        BEAST_EXPECT(result[jss::ledger][jss::account_hash]     == "0FB47F81A89BB0B67DB2BD7A87C5139300B362F5A41ACEF9F3BDC82E723856AC");
        BEAST_EXPECT(result[jss::ledger][jss::transaction_hash] == "0000000000000000000000000000000000000000000000000000000000000000");

        result = env.rpc ( "ledger_request", "3" ) [jss::result];
        BEAST_EXPECT(result[jss::ledger][jss::ledger_index]     == "3");
        BEAST_EXPECT(result[jss::ledger][jss::total_coins]      == "4999999999999982");
        BEAST_EXPECT(result[jss::ledger][jss::closed]           == true);
        BEAST_EXPECT(result[jss::ledger][jss::ledger_hash]      == "F589AF8EB7E02D7D730EC4E9FEA241AB9F1FF383032A9D35B1CED4967083F9F7");
        BEAST_EXPECT(result[jss::ledger][jss::parent_hash]      == "C541ADD67498DD880FF069B2F1FEE3B08620BBC3F14D4F890FD2828051791853");
        BEAST_EXPECT(result[jss::ledger][jss::account_hash]     == "F7F3DC304097E796A1B688CEF16A633677B2FEB0B6BB212FD0BA4DE9EEBC6588");
        BEAST_EXPECT(result[jss::ledger][jss::transaction_hash] == "59B401AFA958F572C4D20BC3DDDF405844CD9D111C70F01D75CEB6B74C0E24B6");

        result = env.rpc ( "ledger_request", "4" ) [jss::result];
        BEAST_EXPECT(result[jss::ledger][jss::ledger_index]     == "4");
        BEAST_EXPECT(result[jss::ledger][jss::total_coins]      == "4999999999999964");
        BEAST_EXPECT(result[jss::ledger][jss::closed]           == true);
        BEAST_EXPECT(result[jss::ledger][jss::ledger_hash]      == "B7F2F6B1FB253A79F237458B081F261E71CFBBC4086A87DFB6F51B0F0E625DE9");
        BEAST_EXPECT(result[jss::ledger][jss::parent_hash]      == "F589AF8EB7E02D7D730EC4E9FEA241AB9F1FF383032A9D35B1CED4967083F9F7");
        BEAST_EXPECT(result[jss::ledger][jss::account_hash]     == "7BD51B185B2C519FB951C9FB9F3CD8B5C485EB0764D794329466F779342B0E4C");
        BEAST_EXPECT(result[jss::ledger][jss::transaction_hash] == "6B5AF7654C5A6AD946B149E6D9536F9590CC55D3A3645D68C8955667060121FA");

        result = env.rpc ( "ledger_request", "5" ) [jss::result];
        BEAST_EXPECT(result[jss::ledger][jss::ledger_index]     == "5");
        BEAST_EXPECT(result[jss::ledger][jss::total_coins]      == "4999999999999946");
        BEAST_EXPECT(result[jss::ledger][jss::closed]           == true);
        BEAST_EXPECT(result[jss::ledger][jss::ledger_hash]      == "4B733BC7398EC3FD19210E55CE8949F8D7D8FC0DAB538DEBFD03A9B09FD618EE");
        BEAST_EXPECT(result[jss::ledger][jss::parent_hash]      == "B7F2F6B1FB253A79F237458B081F261E71CFBBC4086A87DFB6F51B0F0E625DE9");
        BEAST_EXPECT(result[jss::ledger][jss::account_hash]     == "7D354E5004D83D21114745CD9BECEC7C22B2A863D27ADA99047E2636128A1406");
        BEAST_EXPECT(result[jss::ledger][jss::transaction_hash] == "EC7A6B1C04C4C3483D63A59FCDC274D52BE77F9985253D772EA697F74B4FA59D");

        result = env.rpc ( "ledger_request", "6" ) [jss::result];
        BEAST_EXPECT(result[jss::error]         == "invalidParams");
        BEAST_EXPECT(result[jss::status]        == "error");
        BEAST_EXPECT(result[jss::error_message] == "Ledger index too large");
    }

    void testBadInput()
    {
        using namespace test::jtx;
        Env env { *this };
        Account const gw { "gateway" };
        auto const USD = gw["USD"];
        env.fund(XRP(100000), gw);
        env.close();

        Json::Value jvParams;
        jvParams[jss::ledger_hash] = "AB868A6CFEEC779C2FF845C0AF00A642259986AF40C01976A7F842B6918936C7";
        jvParams[jss::ledger_index] = "1";
        auto result = env.rpc ("json", "ledger_request", jvParams.toStyledString()) [jss::result];
        BEAST_EXPECT(result[jss::error]         == "invalidParams");
        BEAST_EXPECT(result[jss::status]        == "error");
        BEAST_EXPECT(result[jss::error_message] == "Exactly one of ledger_hash and ledger_index can be set.");

        // the purpose in this test is to force the ledger expiration/out of
        // date check to trigger
        env.timeKeeper().adjustCloseTime(weeks{3});
        result = env.rpc ( "ledger_request", "1" ) [jss::result];
        BEAST_EXPECT(result[jss::error]         == "noCurrent");
        BEAST_EXPECT(result[jss::status]        == "error");
        BEAST_EXPECT(result[jss::error_message] == "Current ledger is unavailable.");

    }

    void testMoreThan256Closed()
    {
        using namespace test::jtx;
        using namespace std::chrono_literals;
        Env env {*this};
        Account const gw {"gateway"};
        env.app().getLedgerMaster().tune(0, 1h);
        auto const USD = gw["USD"];
        env.fund(XRP(100000), gw);

        int const max_limit = 256;

        for (auto i = 0; i < max_limit + 10; i++)
        {
            Account const bob {std::string("bob") + std::to_string(i)};
            env.fund(XRP(1000), bob);
            env.close();
        }

        auto result = env.rpc ( "ledger_request", "1" ) [jss::result];
        BEAST_EXPECT(result[jss::ledger][jss::ledger_index]     == "1");
        BEAST_EXPECT(result[jss::ledger][jss::total_coins]      == "5000000000000000");
        BEAST_EXPECT(result[jss::ledger][jss::closed]           == true);
        BEAST_EXPECT(result[jss::ledger][jss::ledger_hash]      == "DE92CBEF48579C7189BC7DF4BBD71735B02496E6EA96FA4BFB967DD68A6F99ED");
        BEAST_EXPECT(result[jss::ledger][jss::parent_hash]      == "0000000000000000000000000000000000000000000000000000000000000000");
        BEAST_EXPECT(result[jss::ledger][jss::account_hash]     == "263C2AF60F9419085699C59DA1FB66D2F1E6DFEA8A2176F154367DEF905B3319");
        BEAST_EXPECT(result[jss::ledger][jss::transaction_hash] == "0000000000000000000000000000000000000000000000000000000000000000");
    }

    void testNonAdmin()
    {
        using namespace test::jtx;
        Env env { *this, envconfig(no_admin) };
        Account const gw { "gateway" };
        auto const USD = gw["USD"];
        env.fund(XRP(100000), gw);
        env.close();

        auto const result = env.rpc ( "ledger_request", "1" )  [jss::result];
        // The current HTTP/S ServerHandler returns an HTTP 403 error code here
        // rather than a noPermission JSON error.  The JSONRPCClient just eats that
        // error and returns an null result.
        BEAST_EXPECT(result.type() == Json::nullValue);

    }

    void run () override
    {
        testLedgerRequest();
        testEvolution();
        testBadInput();
        testMoreThan256Closed();
        testNonAdmin();
    }
};

BEAST_DEFINE_TESTSUITE(LedgerRequestRPC,app,ripple);

} // RPC
} // ripple

