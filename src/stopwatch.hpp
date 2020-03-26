#ifndef STOPWATCH_HPP
#define STOPWATCH_HPP

class Stopwatch {
  public:
    unsigned long long start_time;
    unsigned long long pause_time;
    unsigned long long total_paused;
    unsigned long long prev;
    unsigned long long now;
    bool watching = false;
    bool paused = false;
    void start(unsigned long long now);
    void pause(unsigned long long now);
    void unpause(unsigned long long now);
    void toggle_pause(unsigned long long now);
    void update_now(unsigned long long _now);
    double elapsed();
    double diff();
};

#endif
