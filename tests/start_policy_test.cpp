#include <grevir/arduino_esp32/start_policy.hpp>

#include <atomic>
#include <chrono>
#include <future>
#include <thread>

namespace irq = grevir::interrupt;
namespace esp = grevir::arduino_esp32;

struct RecursiveApplication {};
struct ConcurrentApplication {};

int main() {
  using Recursive = esp::TimerStartPolicy<RecursiveApplication>;
  bool nested_body_ran = false;
  bool nested_reported_progress = false;
  const auto initial = Recursive::execute([&]() noexcept {
    const auto nested = Recursive::execute([&]() noexcept {
      nested_body_ran = true;
      return irq::StartResult{};
    });
    nested_reported_progress = nested.outcome == irq::SetupOutcome::in_progress
      && nested.disposition == irq::CallDisposition::in_progress;
    return irq::StartResult{};
  });
  if (nested_body_ran || !nested_reported_progress
      || initial.outcome != irq::SetupOutcome::success
      || initial.disposition != irq::CallDisposition::initiated) { return 1; }
  const auto recursive_replay = Recursive::execute([&]() noexcept {
    nested_body_ran = true;
    return irq::StartResult{};
  });
  if (nested_body_ran || recursive_replay.outcome != irq::SetupOutcome::success
      || recursive_replay.disposition != irq::CallDisposition::replayed) { return 2; }

  using Concurrent = esp::TimerStartPolicy<ConcurrentApplication>;
  std::promise<void> entered;
  std::promise<void> release;
  std::promise<void> contender_finished;
  const auto release_signal = release.get_future().share();
  auto entered_signal = entered.get_future();
  auto finished_signal = contender_finished.get_future();
  irq::StartResult first{};
  irq::StartResult concurrent{};
  std::atomic<bool> concurrent_body_ran{false};
  std::thread starter([&] {
    first = Concurrent::execute([&]() noexcept {
      entered.set_value();
      release_signal.wait();
      return irq::StartResult{irq::SetupOutcome::registration_failed,
        irq::SetupOutcome::registration_failed, irq::CallDisposition::initiated};
    });
  });
  entered_signal.wait();
  std::thread contender([&] {
    concurrent = Concurrent::execute([&]() noexcept {
      concurrent_body_ran.store(true);
      return irq::StartResult{};
    });
    contender_finished.set_value();
  });
  const bool returned_while_starting = finished_signal.wait_for(
    std::chrono::seconds{2}) == std::future_status::ready;
  release.set_value();
  starter.join();
  contender.join();
  if (!returned_while_starting || concurrent_body_ran.load()
      || concurrent.outcome != irq::SetupOutcome::in_progress
      || concurrent.disposition != irq::CallDisposition::in_progress) { return 3; }
  const auto replay = Concurrent::execute([&]() noexcept {
    concurrent_body_ran.store(true);
    return irq::StartResult{};
  });
  if (concurrent_body_ran.load()
      || first.outcome != irq::SetupOutcome::registration_failed
      || replay.outcome != first.outcome
      || replay.originating_failure != first.originating_failure
      || replay.disposition != irq::CallDisposition::replayed) { return 4; }
  return 0;
}
