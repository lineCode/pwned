#ifndef PWNED_PROGRESS_HPP
#define PWNED_PROGRESS_HPP

#include <iostream>
#include <iomanip>
#include <ctime>
#include <boost/chrono.hpp>

namespace pwned { namespace progress {

using namespace boost::chrono;

struct DurationParts
{
  hours h;
  minutes m;
  seconds s;
  milliseconds ms;

  DurationParts(milliseconds d)
  {
    h = duration_cast<hours>(d);
    d -= h;
    m = duration_cast<minutes>(d);
    d -= m;
    s = duration_cast<seconds>(d);
    d -= s;
    ms = duration_cast<milliseconds>(d);
  }
};

std::ostream &operator<< (std::ostream &o, DurationParts const &dp)
{
  return o<< std::setfill('0')
    << std::setw(2)<< dp.h.count()<< ":"
    << std::setw(2)<< dp.m.count()<< ":"
    << std::setw(2)<< dp.s.count()<< "."
    << std::setw(3)<< dp.ms.count();
}

template <typename clock>
struct ProgressClock
{
  uint64_t total_ticks, ticks;
  int total_bar_width, bar_width;
  void(ProgressClock::* tick_ptr)();
  time_point<clock> start;
  int64_t elapsed_milliseconds;

  ProgressClock(uint64_t total_ticks)
  : total_ticks(total_ticks)
  , ticks(0)
  , total_bar_width(50)
  , bar_width(0)
  , tick_ptr(&ProgressClock::first_tick)
  , start(clock::now())
  , elapsed_milliseconds(0)
  {}

  void first_tick()
  {
    tick_ptr= &ProgressClock::continue_tick;
    std::cout<< '\n';
    continue_tick();
  }

  void continue_tick()
  {
    ticks+= 1;
    bar_width= total_bar_width* ticks/ total_ticks;
    elapsed_milliseconds= duration_cast<milliseconds>(clock::now()- start).count();
    print();
  }

  void tick()
  {
    (this->* tick_ptr)();
  }

  milliseconds
  estimate()
  {
    int64_t average_tick_milliseconds= 0;
    if(ticks> 0)
      average_tick_milliseconds= elapsed_milliseconds/ ticks;
    uint64_t remaining_ticks= total_ticks- ticks;
    auto estimate_ms= milliseconds(average_tick_milliseconds* remaining_ticks);
    return estimate_ms;
  }

  std::string bar() const
  {
    int blank= total_bar_width- bar_width;
    std::string bar= "|";
    if(bar_width> 0)
      bar+= std::string(bar_width, '=');
    bar+= ">";
    if(blank> 0)
      bar+= std::string(blank, ' ');
    bar+= "|";
    return bar;
  }

  void print()
  {
    DurationParts estimate_parts(estimate());
    int64_t percent= 100* ticks/ total_ticks;
    DurationParts elapsed_parts((milliseconds(elapsed_milliseconds)));
    std::cout<< "("
      << elapsed_parts <<") "
      << bar()<< " (% "<< std::setw(3)<< std::setfill(' ')
      << percent <<") ("
      << estimate_parts<< ")\r"<< std::flush;
  }

  ~ProgressClock()
  {
    std::cout<< std::endl;
  }

};

typedef ProgressClock<steady_clock> Progress;

} } // pwned progress

#endif

