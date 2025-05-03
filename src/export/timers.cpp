#include <core/timer_system.hpp>
#include <plugin_export.h>

PLUGIFY_WARN_PUSH()

#if defined(__clang)
PLUGIFY_WARN_IGNORE("-Wreturn-type-c-linkage")
#elif defined(_MSC_VER)
PLUGIFY_WARN_IGNORE(4190)
#endif

/**
 * @brief Creates a new timer that executes a callback function at specified delays.
 *
 * This function initializes a timer that will call the provided callback function
 * after the specified delay in seconds. The timer can be configured with various flags
 * to control its behavior.
 *
 * @param delay The time delay in seconds between each callback execution.
 * @param callback The function to be called when the timer expires.
 * @param flags Flags that modify the behavior of the timer (e.g., no-map change, repeating).
 * @param userData An array intended to hold user-related data, allowing for elements of any type.
 * @return An id of the newly created CTimer object, or -1 if the timer could not be created.
 */
extern "C" PLUGIN_API uint32_t CreateTimer(double delay, TimerCallback callback, TimerFlag flags, const plg::vector<plg::any>& userData) {
	return g_TimerSystem.CreateTimer(delay, callback, flags, userData);
}

/**
 * @brief Stops and removes an existing timer.
 *
 * This function terminates the specified timer, preventing it from calling
 * its callback function. Once killed, the timer cannot be reused.
 *
 * @param id An id of the CTimer object to be stopped and removed.
 */
extern "C" PLUGIN_API void KillsTimer(uint32_t id) {
	g_TimerSystem.KillTimer(id);
}

/**
 * @brief Reschedules an existing timer with a new delay.
 *
 * This function changes the delay of the specified timer to the new value.
 * The timer will continue to execute its callback function at the new delay.
 *
 * @param id An id of the CTimer object to be rescheduled.
 * @param newDelay The new delay in seconds between each callback execution.
 */
extern "C" PLUGIN_API void RescheduleTimer(uint32_t id, double newDelay) {
	g_TimerSystem.RescheduleTimer(id, newDelay);
}

/**
 * @brief Returns the number of seconds in between game server ticks.
 *
 * @return The tick interval value.
 */
extern "C" PLUGIN_API double GetTickInterval() {
	return TimerSystem::GetTickedInterval();
}

/**
 * @brief Returns the simulated game time.
 *
 * @return The ticked time value.
 */
extern "C" PLUGIN_API double GetTickedTime() {
	return TimerSystem::GetTickedTime();
}

PLUGIFY_WARN_POP()
