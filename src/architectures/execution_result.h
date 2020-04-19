#ifndef __EXECUTION_RESULT_H__
#define __EXECUTION_RESULT_H__

#include <variant>
#include <string>

struct ExecutionResultSleep {
		int ticks;
};

struct ExecutionResultSynchronizedCall {};

struct ExecutionResultShutdown {
		bool reboot;
};

struct ExecutionResultError {
		std::string message;
};

using ExecutionResult = std::variant<ExecutionResultSleep, ExecutionResultSynchronizedCall, ExecutionResultShutdown, ExecutionResultError>;

#endif