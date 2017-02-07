#ifdef _WIN32
#include <windows.h>
#elif defined __linux__
#include <sys/time.h>
#else
#error "Unknown OS"
#endif
#include "timer.h"

const char *timer::timer_exception::what() const throw()
{
	return "No precision timer";
}

timer::timer()
{
#ifdef _WIN32
	LARGE_INTEGER c, f;
	if(!QueryPerformanceFrequency(&f)){
		timer::timer_exception t;
		throw t;
	}
	freq_ = (double)f.QuadPart;
	QueryPerformanceCounter(&c);
	base_time_ = (double)c.QuadPart / freq_;
#else
    timezone tz;
    timeval t;
    gettimeofday(&t, &tz);
    base_time_ = t.tv_sec + 1.e-6*t.tv_usec;
    freq_ = -1; // that is, not relevant
#endif
	suspend_time_ = last_tick_ = base_time_;
	suspended_ = false;
}

double timer::time() const
{
#ifdef _WIN32
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	return (double)c.QuadPart / freq_ - base_time_;
#else
    timezone tz;
    timeval t;
    gettimeofday(&t, &tz);
    return (double)t.tv_sec + 1.e-6*t.tv_usec - base_time_;
#endif

}

void timer::reset()
{
#ifdef _WIN32
    LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	last_tick_ = base_time_ = (double)c.QuadPart / freq_;
#else
    timezone tz;
    timeval t;
    gettimeofday(&t, &tz);
    last_tick_ = base_time_ = t.tv_sec + 1.e-6*t.tv_usec;
#endif
}


bool timer::suspend()
{
#ifdef _WIN32
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	if(suspended_) return false;
	suspend_time_ = (double)c.QuadPart / freq_;
#else
    timezone tz;
    timeval t;
    gettimeofday(&t, &tz);
    suspend_time_ = t.tv_sec + 1.e-6*t.tv_usec;
#endif
	return suspended_ = true;
}

bool timer::resume()
{
#ifdef _WIN32
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	if(!suspended_) return false;
	double time_suspended = (double)c.QuadPart / freq_ - suspend_time_;
	base_time_ += time_suspended;
#else
    timezone tz;
    timeval t;
    gettimeofday(&t, &tz);
    double time_suspended = t.tv_sec + 1.e-6*t.tv_usec - suspend_time_;
#endif
	base_time_ += time_suspended;
	suspended_ = false;
	return true;
}

double timer::elapsed()
{
	double time_now = time();
	double ret = time_now - last_tick_;
	last_tick_ = time_now;
	return ret<0?0:ret;
}


timer::~timer()
{
	
}



