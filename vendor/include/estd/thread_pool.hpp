// BSD 3-Clause License

// Copyright (c) 2022, Alex Tarasov
// All rights reserved.

// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:

// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.

// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.

// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.

// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <condition_variable>
#include <estd/semaphore.h>
#include <estd/thread_safe_queue.h>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>


namespace estd {
	/**
     * thread_pool class creates a pool of threads that pick up and exacute jobs
     * the class will keep the threads alive as long as there is at least one job running or on the queue.
     * If there are no jobs the queue will close and all threads will stop picking up new tasks and
     * eventually terminate. scheduling to a queue in this state will throw errors
     */
	class thread_pool {
	private:
		int numThreads = 0;
		void allocateThreads(int num) {
			numThreads = num;
			for (int i = 0; i < num; i++) {
				threads.emplace_back([this]() {
					auto queuePtr = this->tasks;
					auto& queue = *queuePtr;
					auto err = this->errors;

					std::function<void()> task;

					while (queue >> task) {
						try {
							// cerr << "got task\n";
							task();
						} catch (std::exception& e) { err->push(std::runtime_error(e.what())); } catch (...) {
							err->push(std::runtime_error("Unknown exception caught."));
						}
						numTasks--;
						taskChange.notify();
						// cerr << "task ended\n";
					}
				});
			}
		}

		inline void init() {
			tasks = make_shared<thread_safe_queue<std::function<void()>>>(numThreads*100);
			errors = make_shared<thread_safe_queue<std::runtime_error>>();
		}

		atomic_int32_t numTasks = 0;
		Semaphore taskChange;
		shared_ptr<thread_safe_queue<std::function<void()>>> tasks;
		shared_ptr<thread_safe_queue<std::runtime_error>> errors;
		vector<thread> threads;

	public:
		bool forwardExceptions = true;
		thread_pool() {
			numThreads = 1;
			init();
		}
		thread_pool(const thread_pool& toCpy) {
			numThreads = toCpy.numThreads;
			init();
		}
		thread_pool(int th) {
			numThreads = th;
			init();
		}

		inline int getNumThreads() { return numThreads; }

		inline void schedule(std::function<void()> f) {
			numTasks++;
			taskChange.notify();
			tasks->push(f);
		}

		inline void wait() {
			allocateThreads(numThreads);

			while (numTasks > 0) taskChange.wait();
			tasks->close();
			for (auto& t : threads) t.join();
			threads.clear();
			errors->close();

			tasks = make_shared<thread_safe_queue<std::function<void()>>>(numThreads*100);

			std::runtime_error e("");

			if (forwardExceptions && *errors >> e) {
				errors = make_shared<thread_safe_queue<std::runtime_error>>();
				throw e;
			} else {
				errors = make_shared<thread_safe_queue<std::runtime_error>>();
			}
		}
	};
}// namespace estd
