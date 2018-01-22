#include "catch.hpp"
#include "tr/util/Log.h"

TEST_CASE("Logging")
{
    tr::Log log;
    log << "Hallo Welt\n";
    log << true;
    log << 3.12f;
}
