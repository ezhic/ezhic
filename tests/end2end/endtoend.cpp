#include "BRunner.h"
#include "Timer.h"

#include <iostream>
#include <random>
#include <unistd.h>

using namespace ezhic;

class RandomTickTimer : public ezhic::Timer
{
  public:
    RandomTickTimer(unsigned step, unsigned count)
    : d_time(0), d_lastBucket(0), d_entropy(std::random_device()())
    {
        d_values.resize(count);
        std::vector<double> d_weights(count);
        double w = 100.0;
        for (unsigned i = 0; i < count; ++i, w *= 0.7) {
            d_values[i] = i * step;
            d_weights[i] = w;
        }
        d_weights.pop_back();
        d_responseTimeDist = std::discrete_distribution<unsigned>(
            d_weights.begin(), d_weights.end());
    }

    size_t now() const  // override
    {
        d_lastBucket = d_responseTimeDist(d_entropy);
        return tickLast();
    }

    Event::duration_t lastDelta() const
    {
        std::uniform_int_distribution<unsigned> dstr(
            d_values[d_lastBucket], d_values[d_lastBucket + 1]);
        return dstr(d_entropy);
    }

    size_t tickLast() const { return d_time += lastDelta(); }
    ezhic::Timer *clone() const { return new RandomTickTimer(*this); }
  private:
    mutable size_t d_time;
    mutable size_t d_lastBucket;
    mutable std::mt19937 d_entropy;
    mutable std::discrete_distribution<unsigned> d_responseTimeDist;
    std::vector<Event::duration_t> d_values;
};

class TickAndStick : public ezhic::Timer
{
  public:
    TickAndStick(RandomTickTimer ticker,
                 Event::duration_t triggerAt,
                 unsigned stickFor)
    : d_triggerAt(triggerAt)
    , d_stickFor(stickFor)
    , d_ticker(ticker)
    , d_counter(0)
    {
    }

    size_t now() const  // override
    {
        if (d_counter) {
            ++d_counter;
            if (d_counter >= d_stickFor)
                d_counter = 0;
            return d_ticker.tickLast();
        }
        auto t = d_ticker.now();
        if (d_ticker.lastDelta() >= d_triggerAt) {
            d_counter = 1;
        }
        return t;
    }

    ezhic::Timer *clone() const { return new TickAndStick(*this); }
  private:
    const unsigned d_triggerAt;
    const unsigned d_stickFor;
    RandomTickTimer d_ticker;
    mutable unsigned d_counter;
};

struct Service
{
    typedef int result_type;
    int operator()() { return 17; }
};

unsigned printTrace(const Circuit &circuit, unsigned events)
{
    const auto &trace = circuit.eventTrace();
    auto it = trace.cbegin();
    auto first = *it;
    if (events + 1 == trace.size()) {
        std::cout << first << std::endl;
    } else if (events + 2 == trace.size()) {
        ++it;
        std::cout << *it << std::endl;
        std::cout << first << std::endl;
    } else {
        for (; events < trace.size(); ++events, ++it)
            std::cout << *it << std::endl;
    }
    return trace.size();
}

int main(int argc, const char **argv)
{
    unsigned timeStep = 30;
    unsigned timeCount = 10;
    unsigned timeout = 300;
    unsigned kTimeouts = 2;
    unsigned mTimeouts = 4;
    unsigned kExceptions = 2;
    unsigned mExceptions = 4;
    unsigned kProbe = 2;
    unsigned kMend = 2;
    unsigned stickFor = 24;  // set to 0 for mode-1 <short spikes>

    if (argc < 10) {
        std::cerr << "[{\"usage\": \"time-step time-count timeout "
                     "Kinmtimeouts kinMtimeouts Kinmexceptions kinMexceptions "
                     "probeafterK Kbeforeeend stickFor\"}]\n";
    } else {
        timeStep = atoi(argv[1]);
        timeCount = atoi(argv[2]);
        timeout = atoi(argv[3]);
        kTimeouts = atoi(argv[4]);
        mTimeouts = atoi(argv[5]);
        kExceptions = atoi(argv[6]);
        mExceptions = atoi(argv[7]);
        kProbe = atoi(argv[8]);
        kMend = atoi(argv[9]);
        stickFor = atoi(argv[10]) * 2;  // we do doubles.
    }

    RandomTickTimer randomTicker(timeStep, timeCount);
    TickAndStick timer(randomTicker, timeout, stickFor);
    std::ostream streamnull(0);
    Bundle bundle = make_mbundle(streamnull,
                                 timeout,
                                 {kTimeouts, mTimeouts},
                                 {kExceptions, mExceptions},
                                 kProbe,
                                 kMend);
    Circuit circuit;
    BRunner br(circuit, bundle, &timer);

    Service service;
    unsigned events = 0;
    for (unsigned i = 0; i < 1 << 30; ++i) {
        br.run(service);
        events = printTrace(circuit, events);
        usleep(100000);
    }
}