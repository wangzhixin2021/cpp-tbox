#include <gtest/gtest.h>
#include "timers.h"
#include <tbox/event/loop.h>
#include <tbox/event/timer_event.h>
#include <tbox/base/scope_exit.hpp>

using namespace std;
using namespace std::chrono;
using namespace tbox;
using namespace tbox::event;
using namespace tbox::eventx;

TEST(Timers, doEvery)
{
}

TEST(Timers, doAfter)
{
    Loop *sp_loop = event::Loop::New();
    Timers timers(sp_loop);
    SetScopeExitAction([sp_loop]{ delete sp_loop;});

    auto start_time = system_clock::now();
    Timers::Token token;
    token = timers.doAfter(milliseconds(1000),
        [&] (const Timers::Token &t){
            EXPECT_EQ(t, token);
            auto d = system_clock::now() - start_time;
            EXPECT_GT(d, milliseconds(995));
            EXPECT_LT(d, milliseconds(1005));
        }
    );
    sp_loop->exitLoop(chrono::milliseconds(1500));
    sp_loop->runLoop();

    timers.cleanup();
}

TEST(Timers, cancel)
{
    Loop *sp_loop = event::Loop::New();
    Timers timers(sp_loop);
    SetScopeExitAction([sp_loop]{ delete sp_loop;});

    bool tag = false;
    auto token = timers.doAfter(milliseconds(100),
        [&] (const Timers::Token &){
            tag = true;
        }
    );
    timers.cancel(token);
    sp_loop->exitLoop(chrono::milliseconds(200));
    sp_loop->runLoop();

    timers.cleanup();

    EXPECT_FALSE(tag);
}

TEST(Timers, doAt)
{
    Loop *sp_loop = event::Loop::New();
    Timers timers(sp_loop);
    SetScopeExitAction([sp_loop]{ delete sp_loop;});

    auto start_time = system_clock::now();
    Timers::Token token;
    token = timers.doAt(start_time + milliseconds(1000),
        [&] (const Timers::Token &t){
            EXPECT_EQ(t, token);
            auto d = system_clock::now() - start_time;
            EXPECT_GT(d, milliseconds(995));
            EXPECT_LT(d, milliseconds(1005));
        }
    );
    sp_loop->exitLoop(chrono::milliseconds(1500));
    sp_loop->runLoop();

    timers.cleanup();
}

