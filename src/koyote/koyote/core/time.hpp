
#pragma once
//
// Created by Gabriel Lugo on 3/27/2021.
// Flugel Framework: https://github.com/GTLugo/flugel_framework
//

#include "koyote/core/std.hpp"
#include "koyote/core/log.hpp"

namespace koyote {
  // duration types
  using nanosecs = std::chrono::duration<double, std::nano>;
  using microsecs = std::chrono::duration<double, std::micro>;
  using millisecs = std::chrono::duration<double, std::milli>;
  using secs = std::chrono::duration<double>;
  using mins = std::chrono::duration<double, std::ratio<60>>;
  using hours = std::chrono::duration<double, std::ratio<3600>>;
  // clock types
  using clock_system = std::chrono::system_clock;
  using clock_steady = std::chrono::steady_clock;
  using clock_accurate = std::chrono::high_resolution_clock;
  // time point
  using time_point = clock_steady::time_point;
  using time_point_a = clock_accurate::time_point;
  using time_point_s = clock_system::time_point;

  class stop_watch {
  public:
    stop_watch() {
      start();
    }

    explicit stop_watch(time_point time_point) {
      start(time_point);
    }

    void start() { start(clock_steady::now()); }

    void start(time_point timePoint) {
      start_ = timePoint;
    }

    template<class Duration>
    [[nodiscard]] double start_time() const {
      return Duration{(start_).time_since_epoch()}.count();
    }

    template<class Duration>
    [[nodiscard]] double get_time_elapsed() const {
      return Duration{(clock_steady::now() - start_)}.count();
    }
  private:
    time_point start_{};
  };

  class Time {
  public:

    static void i__engine_internal_init(double tick_rate = 128., u32 bail_count = 1024U) {
      // This is awful and messy, but it'll prevent anyone outside the App class
      // from reinitializing Time, which would cause the engine, the app, life,
      // the universe, and all catgirls to die.
      if (!virgin_) return;
      koyote::Log::trace("Initializing Time...");

      tick_rate_ = tick_rate;
      bail_count_ = bail_count;
      game_last_ = time_point{clock_steady::now()};
      game_current_ = time_point{clock_steady::now()};
      fixed_time_step_ = secs{1. / tick_rate_};
    }
//    explicit Time(double tickRate, uint32_t bailCount = 1024U)
//      : tickRate_{tickRate},
//        bailCount_{bailCount},
//        stopwatch_{ClockSteady::now()},
//        gameLast_{ClockSteady::now()},
//        gameCurrent_{ClockSteady::now()} {
//      fixedTimeStep_ = Seconds{1. / tickRate_};
//    }
//    ~Time() = default;

    [[nodiscard]] static double tick_rate() {
      return tick_rate_;
    }

    template<class Duration>
    [[nodiscard]] static double fixed_step() {
      return Duration{fixed_time_step_}.count();
    }

    template<class Duration>
    [[nodiscard]] static double start() {
      return stopwatch().start_time<Duration>();
    }

    template<class Duration>
    [[nodiscard]] static double since_start() {
      return stopwatch().get_time_elapsed<Duration>();
    }

    template<typename Duration>
    [[nodiscard]] static double now() {
      return Duration{clock_steady::now().time_since_epoch()}.count();
    }

    template<class Duration>
    [[nodiscard]] static double delta() {
      return Duration{delta_}.count();
    }

    template<class Duration>
    [[nodiscard]] static double lag() {
      return Duration{lag_}.count();
    }
    
    // These should NEVER be called from anywhere other than the internal app class.

    static void i__engine_internal_update() {
      // FLUGEL_ENGINE_TRACE("Update!");
      game_current_ = clock_steady::now();
      // Seconds::duration()
      delta_ = std::chrono::duration_cast<secs>(game_current_ - game_last_);
      game_last_ = game_current_;
      lag_ += delta_;
      step_count_ = 0U;
    }

    static void i__engine_internal_tick() {
      // FLUGEL_ENGINE_TRACE("Tick!");
      lag_ -= fixed_time_step_;
      ++step_count_;
    }

    [[nodiscard]] static bool i__engine_internal_should_do_tick() {
      if (step_count_ >= bail_count_) {
        koyote::Log::warn("Struggling to catch up with physics rate.");
      }

      return lag_.count() >= fixed_time_step_.count() && step_count_ < bail_count_;
    }
  private:
    static inline bool virgin_{true};
    // fixed number of ticks per second. this will be used for physics and anything else in fixed update
    static inline double tick_rate_{};
    static inline secs fixed_time_step_{};
    // bail out of the fixed updates if iterations exceeds this amount to prevent lockups
    // on extremely slow systems where updateFixed may be longer than fixedTimeStep_
    static inline u32 bail_count_{};

    static inline time_point game_last_{}; // when last frame started
    static inline time_point game_current_{}; // when this frame started
    static inline secs delta_{secs{1. / 60.}}; // how much time last frame took
    static inline secs lag_{secs::zero()}; // how far behind the game is from real world
    static inline u32 step_count_{0U};

    static const stop_watch& stopwatch() {
      static const stop_watch sw{clock_steady::now()};
      return sw;
    };
  };
}