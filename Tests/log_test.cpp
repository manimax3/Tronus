#include "catch.hpp"
#include "tr/util/Log.h"

struct CustomType
{
    operator std::string() const{
        return "CustomType";
    }
};

TEST_CASE("Logging")
{
    using namespace tr;
    tr::Log log;
    log.log("==Log Test==", LogLevel::WARNING);
    log.log("Hallo Welt", LogLevel::ERROR);
    log.log(10);
    
    CustomType t;
    log.log(t);
}
