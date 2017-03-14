#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-NDC");
    BOOST_CHECK(GetBoolArg("-NDC"));
    BOOST_CHECK(GetBoolArg("-NDC", false));
    BOOST_CHECK(GetBoolArg("-NDC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-NDCo"));
    BOOST_CHECK(!GetBoolArg("-NDCo", false));
    BOOST_CHECK(GetBoolArg("-NDCo", true));

    ResetArgs("-NDC=0");
    BOOST_CHECK(!GetBoolArg("-NDC"));
    BOOST_CHECK(!GetBoolArg("-NDC", false));
    BOOST_CHECK(!GetBoolArg("-NDC", true));

    ResetArgs("-NDC=1");
    BOOST_CHECK(GetBoolArg("-NDC"));
    BOOST_CHECK(GetBoolArg("-NDC", false));
    BOOST_CHECK(GetBoolArg("-NDC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noNDC");
    BOOST_CHECK(!GetBoolArg("-NDC"));
    BOOST_CHECK(!GetBoolArg("-NDC", false));
    BOOST_CHECK(!GetBoolArg("-NDC", true));

    ResetArgs("-noNDC=1");
    BOOST_CHECK(!GetBoolArg("-NDC"));
    BOOST_CHECK(!GetBoolArg("-NDC", false));
    BOOST_CHECK(!GetBoolArg("-NDC", true));

    ResetArgs("-NDC -noNDC");  // -NDC should win
    BOOST_CHECK(GetBoolArg("-NDC"));
    BOOST_CHECK(GetBoolArg("-NDC", false));
    BOOST_CHECK(GetBoolArg("-NDC", true));

    ResetArgs("-NDC=1 -noNDC=1");  // -NDC should win
    BOOST_CHECK(GetBoolArg("-NDC"));
    BOOST_CHECK(GetBoolArg("-NDC", false));
    BOOST_CHECK(GetBoolArg("-NDC", true));

    ResetArgs("-NDC=0 -noNDC=0");  // -NDC should win
    BOOST_CHECK(!GetBoolArg("-NDC"));
    BOOST_CHECK(!GetBoolArg("-NDC", false));
    BOOST_CHECK(!GetBoolArg("-NDC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--NDC=1");
    BOOST_CHECK(GetBoolArg("-NDC"));
    BOOST_CHECK(GetBoolArg("-NDC", false));
    BOOST_CHECK(GetBoolArg("-NDC", true));

    ResetArgs("--noNDC=1");
    BOOST_CHECK(!GetBoolArg("-NDC"));
    BOOST_CHECK(!GetBoolArg("-NDC", false));
    BOOST_CHECK(!GetBoolArg("-NDC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-NDC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-NDC", "eleven"), "eleven");

    ResetArgs("-NDC -bar");
    BOOST_CHECK_EQUAL(GetArg("-NDC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-NDC", "eleven"), "");

    ResetArgs("-NDC=");
    BOOST_CHECK_EQUAL(GetArg("-NDC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-NDC", "eleven"), "");

    ResetArgs("-NDC=11");
    BOOST_CHECK_EQUAL(GetArg("-NDC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-NDC", "eleven"), "11");

    ResetArgs("-NDC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-NDC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-NDC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-NDC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-NDC", 0), 0);

    ResetArgs("-NDC -bar");
    BOOST_CHECK_EQUAL(GetArg("-NDC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-NDC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-NDC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-NDC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-NDC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--NDC");
    BOOST_CHECK_EQUAL(GetBoolArg("-NDC"), true);

    ResetArgs("--NDC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-NDC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noNDC");
    BOOST_CHECK(!GetBoolArg("-NDC"));
    BOOST_CHECK(!GetBoolArg("-NDC", true));
    BOOST_CHECK(!GetBoolArg("-NDC", false));

    ResetArgs("-noNDC=1");
    BOOST_CHECK(!GetBoolArg("-NDC"));
    BOOST_CHECK(!GetBoolArg("-NDC", true));
    BOOST_CHECK(!GetBoolArg("-NDC", false));

    ResetArgs("-noNDC=0");
    BOOST_CHECK(GetBoolArg("-NDC"));
    BOOST_CHECK(GetBoolArg("-NDC", true));
    BOOST_CHECK(GetBoolArg("-NDC", false));

    ResetArgs("-NDC --noNDC");
    BOOST_CHECK(GetBoolArg("-NDC"));

    ResetArgs("-noNDC -NDC"); // NDC always wins:
    BOOST_CHECK(GetBoolArg("-NDC"));
}

BOOST_AUTO_TEST_SUITE_END()
