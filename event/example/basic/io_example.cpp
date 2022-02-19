#include <unistd.h>
#include <iostream>
#include <cstring>
#include <tbox/event/loop.h>
#include <tbox/event/fd_event.h>

using namespace std;
using namespace tbox;
using namespace tbox::event;

void FdCallback(int fd, short event)
{
    cout << "on read event" << endl;
    char input_buff[200];
    int rsize = read(fd, input_buff, sizeof(input_buff));
    input_buff[rsize - 1] = '\0';
    cout << "fd: " << fd << " INPUT is [" << input_buff << "]" << endl;
}

void FdWCallback(int fd, short event)
{
    char input_buff[200];
    memset(input_buff,0, sizeof(input_buff));
    cout << "on write event"<< endl << "please input data:" << endl;
    cin >> input_buff;
    input_buff[sizeof(input_buff) - 1] = '\0';
    write(fd, input_buff, sizeof(input_buff));
    cout << endl << "write:" << "[" << input_buff << "] to fd:" << fd << endl;
}

void PrintUsage(const char *process_name)
{
    cout << "Usage:" << process_name << " libevent|libev|epoll" << endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        PrintUsage(argv[0]);
        return 0;
    }

    Loop::Engine loop_engine;
    if (string(argv[1]) == "libevent")
        loop_engine = Loop::Engine::kLibevent;
    else if (string(argv[1]) == "libev")
        loop_engine = Loop::Engine::kLibev;
    else if (string(argv[1]) == "epoll")
        loop_engine = Loop::Engine::kEpoll;
    else {
        PrintUsage(argv[0]);
        return 0;
    }

    Loop* sp_loop = Loop::New(loop_engine);
    if (sp_loop == nullptr) {
        cout << "fail, exit" << endl;
        return 0;
    }

    FdEvent* sp_fd = sp_loop->newFdEvent();
    sp_fd->initialize(STDIN_FILENO, FdEvent::kReadEvent, Event::Mode::kPersist);
    using std::placeholders::_1;
    sp_fd->setCallback(std::bind(FdCallback, STDIN_FILENO, _1));
    sp_fd->enable();

    FdEvent* sp_fdw = sp_loop->newFdEvent();
    sp_fdw->initialize(STDIN_FILENO, FdEvent::kReadEvent, Event::Mode::kPersist);
    sp_fdw->setCallback(std::bind(FdWCallback, STDIN_FILENO, _1));
    sp_fdw->enable();

    sp_loop->runLoop(Loop::Mode::kForever);

    delete sp_fd;
    delete sp_fdw;
    delete sp_loop;
    return 0;
}
