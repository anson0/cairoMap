#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED
#include <exception>

class timer
{
public:
	class timer_exception : public std::exception
	{
		virtual const char *what() const throw();
	};
	timer();
	~timer();
	double time() const;
	void reset();
	bool suspend();
	bool resume();
	double elapsed();
private:
	bool suspended_;
	double last_tick_;
	double suspend_time_;
	double base_time_;
	double freq_;
};

#endif

