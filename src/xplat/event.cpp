#include "uart/xplat/event.hpp"
#include <stdexcept>

#if _WIN32
    #include <Windows.h>
#elif __linux__ || __APPLE__ || __CYGWIN__ || __QNXNTO__
    #include <cerrno>
    #include <pthread.h>
    #include <ctime>
#else
    #error "Unknown System"
#endif

#if __APPLE__
    #include <ctime>
    #include <sys/time.h>
    #include <mach/clock.h>
    #include <mach/mach.h>
#endif

namespace uart::xplat
{
struct Event::Impl
{
#if _WIN32
    HANDLE EventHandle{ nullptr };
#elif __linux__ || __APPLE__ || __CYGWIN__ || __QNXNTO__
    pthread_mutex_t Mutex{};
    pthread_cond_t Condition{};
    bool IsTriggered{ false };
#else
    #error "Unknown System"
#endif

    Impl()
    {
#if _WIN32
        EventHandle = CreateEvent(
            nullptr,
            false,
            false,
            nullptr);

        if (EventHandle == nullptr)
        {
            throw std::runtime_error("EventHandle could not be received");
        }

#elif __linux__ || __APPLE__ || __CYGWIN__ || __QNXNTO__

        pthread_mutex_init(&Mutex, nullptr);

        pthread_cond_init(&Condition, nullptr);

#else

    #error "Unknown System"

#endif
    }
};

Event::Event()
    : _pi(new Impl()) {}

Event::~Event()
{
#if _WIN32
    CloseHandle(_pi->EventHandle);
#endif

    delete _pi;
}

void Event::wait()
{
#if _WIN32

    DWORD result;

    result = WaitForSingleObject(
        _pi->EventHandle,
        INFINITE);

    if (result == WAIT_OBJECT_0)
    {
        return;
    }

#elif __linux__ || __APPLE__ || __CYGWIN__ || __QNXNTO__

    pthread_mutex_lock(&_pi->Mutex);

    int errorCode = pthread_cond_wait(
        &_pi->Condition,
        &_pi->Mutex);

    pthread_mutex_unlock(&_pi->Mutex);

    if (errorCode == 0)
    {
        return;
    }

#else

    #error "Unknown System"

#endif

    throw std::runtime_error("Wait error");
}

Event::WaitResult Event::waitUs(uint32_t timeoutUs)
{
#if _WIN32

    DWORD result;

    result = WaitForSingleObject(
        _pi->EventHandle,
        timeoutUs / 1000);

    if (result == WAIT_OBJECT_0)
    {
        return WAIT_SIGNALED;
    }

    if (result == WAIT_TIMEOUT)
    {
        return WAIT_TIMEDOUT;
    }

#elif __linux__ || __CYGWIN__ || __QNXNTO__

    pthread_mutex_lock(&_pi->Mutex);

    timespec now{};
    clock_gettime(CLOCK_REALTIME, &now);

    uint32_t numOfSecs = timeoutUs / 1000000;
    uint32_t numOfNanoseconds = (timeoutUs % 1000000) * 1000;

    now.tv_sec += numOfSecs;
    now.tv_nsec += numOfNanoseconds;

    if (now.tv_nsec > 1000000000)
    {
        now.tv_nsec %= 1000000000;
        now.tv_sec++;
    }

    int errorCode = pthread_cond_timedwait(
        &_pi->Condition,
        &_pi->Mutex,
        &now);

    pthread_mutex_unlock(&_pi->Mutex);

    if (errorCode == 0)
    {
        return WAIT_SIGNALED;
    }

    if (errorCode == ETIMEDOUT)
    {
        return WAIT_TIMEDOUT;
    }

#elif __APPLE__

    pthread_mutex_lock(&_pi->Mutex);

    clock_serv_t cclock = 0;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);

    timespec now{};
    now.tv_sec = mts.tv_sec;
    now.tv_nsec = mts.tv_nsec;

    uint32_t numOfSecs = timeoutUs / 1000000;
    uint32_t numOfNanoseconds = (timeoutUs % 1000000) * 1000;

    now.tv_sec += numOfSecs;
    now.tv_nsec += numOfNanoseconds;

    if (now.tv_nsec > 1000000000)
    {
        now.tv_nsec %= 1000000000;
        now.tv_sec++;
    }

    int errorCode = pthread_cond_timedwait(
        &_pi->Condition,
        &_pi->Mutex,
        &now);

    pthread_mutex_unlock(&_pi->Mutex);

    if (errorCode == 0)
    {
        return WAIT_SIGNALED;
    }

    if (errorCode == ETIMEDOUT)
    {
        return WAIT_TIMEDOUT;
    }

#else

    #error "Unknown System"

#endif

    throw std::runtime_error("Wait error");
}

Event::WaitResult Event::waitMs(uint32_t timeoutMs)
{
#if _WIN32

    DWORD result;

    result = WaitForSingleObject(
        _pi->EventHandle,
        timeoutMs);

    if (result == WAIT_OBJECT_0)
    {
        return WAIT_SIGNALED;
    }

    if (result == WAIT_TIMEOUT)
    {
        return WAIT_TIMEDOUT;
    }

#elif __linux__ || __APPLE__ || __CYGWIN__ || __QNXNTO__

    return waitUs(timeoutMs * 1000);

#else

    #error "Unknown System"

#endif

    throw std::runtime_error("Wait error");
}

void Event::signal()
{
#if _WIN32

    if (!SetEvent(_pi->EventHandle))
    {
        throw std::runtime_error("Could not set Event");
    }

#elif __linux__ || __APPLE__ || __CYGWIN__ || __QNXNTO__

    pthread_mutex_lock(&_pi->Mutex);

    _pi->IsTriggered = true;

    pthread_cond_signal(&_pi->Condition);

    pthread_mutex_unlock(&_pi->Mutex);

#else

    #error "Unknown System"

#endif
}

} // namespace uart::xplat