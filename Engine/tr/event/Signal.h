#pragma once
#include <boost/signals2.hpp>

namespace tr {

///
//
//  Tronus Engine Signal Slot:
//  The Engine is using the signals2 library from boost. I'm testing with
//  the system boost version. But everything with Boost >= 1.60 shoult be
//  working fine.
//
//  The signals2 library is kinda heavyweight, but its solving alot of issues we
//  are currently experiencing. Furthermore it is thread-safe which is going to
//  help with making the engine multithreaded. Also moving from the old
//  event-based approach to a sig slot system should be a more clear and safe
//  design choice. Event classes should no longer be needed.
//
//  Regarding the probably increaesd compile time, pre-compiled headers might be
//  something we should look into get going. (Looking at you CMake)
//
//  We "wrap" the boost::signals2 namespace into tr::sig. And embbed the signal
//  type directly into the tr namespace. But for further documentation you
//  should be looking at the official boost::singals2 documention (link is
//  below).
//
//  https://www.boost.org/doc/libs/1_67_0/doc/html/signals2.html
//
///

namespace sig = boost::signals2;

template<typename... T>
using Signal = sig::signal<T...>;
}
