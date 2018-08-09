#include "catch.hpp"
#include "tr/util/Log.h"


TEST_CASE("Logging")
{
    using namespace tr;
    tr::Log().warn("==Log Test==");
    tr::Log().error("Hallo Welt");
}
