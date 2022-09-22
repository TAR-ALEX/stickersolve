
/*
BSD 3-Clause License

Copyright (c) 2022, Alex Tarasov
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <atomic>
#include <estd/semaphore.h>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>


namespace estd {
	class thread_safe_queue_error : public std::runtime_error {
		using std::runtime_error::runtime_error;
	};

	using namespace std;

	// Implements a thread saffe result queue that can be closed when it is done sending results.
	template <class T>
	class thread_safe_queue : protected std::queue<T> {
		mutable std::mutex m;

		bool isClosedB = false;
		std::size_t numResultsI = 0;
		bool errorState = false;

		int maxElements = 0;
		Semaphore maxSemaphore;
		Semaphore numElements;
		Semaphore closedSemaphore;

		inline void elementAdded() {
			if (maxElements > 0) maxSemaphore.wait();
		}

		inline void elementRemoved() {
			if (maxElements > 0) maxSemaphore.notify();
		}

	public:
		thread_safe_queue() {}

		thread_safe_queue(int maxElements) : maxSemaphore(maxElements), maxElements(maxElements) {}

		inline void close() {
			std::lock_guard<std::mutex> l(m);
			isClosedB = true;
			numElements.notify(10000);	  // some large number
			closedSemaphore.notify(10000);// some large number
		}

		inline bool closed() {
			std::lock_guard<std::mutex> l(m);
			return isClosedB;
		}

		inline void wait() {
			closedSemaphore.wait();
			closedSemaphore.notify();//add it back after consuming so other waiting threads can continue
		}

		explicit operator vector<T>() {
			vector<T> result;
			try {
				while (true) { result.push_back(pop()); }
			} catch (...) {}
			return result;
		}

		explicit operator deque<T>() {
			deque<T> result;
			try {
				while (true) { result.push_back(pop()); }
			} catch (...) {}
			return result;
		}

		bool hasNext() {
			numElements.wait();
			{
				std::lock_guard<std::mutex> l(m);
				numElements.notify();
				return !std::queue<T>::empty();
			}
		}

		inline T& front() {
			numElements.wait();
			{
				std::lock_guard<std::mutex> l(m);
				numElements.notify();
				if (std::queue<T>::empty()) {
					errorState = true;
					throw thread_safe_queue_error("Error: trying to read from an empty closed result queue");
				}
				return std::queue<T>::front();
			}
		}

		inline bool empty() const {
			std::lock_guard<std::mutex> l(m);
			return std::queue<T>::empty();
		}

		inline std::size_t size() const {
			std::lock_guard<std::mutex> l(m);
			return std::queue<T>::size();
		}

		inline std::size_t numResults() const {
			std::lock_guard<std::mutex> l(m);
			return numResultsI;
		}

		inline T pop() {
			numElements.wait();
			{
				std::lock_guard<std::mutex> l(m);

				if (std::queue<T>::empty()) {
					errorState = true;
					numElements.notify();
					throw thread_safe_queue_error("Error: trying to pop from an empty closed result queue");
				}

				T f = std::queue<T>::front();
				elementRemoved();
				std::queue<T>::pop();
				return f;
			}
		}

		inline void push(T&& x) {
			elementAdded();
			std::lock_guard<std::mutex> l(m);
			if (isClosedB) { throw thread_safe_queue_error("Error: trying to push to a closed result queue"); }
			std::queue<T>::push(x);
			numResultsI++;
			numElements.notify();
		}

		inline void push(const T& x) {
			elementAdded();
			std::lock_guard<std::mutex> l(m);
			if (isClosedB) { throw thread_safe_queue_error("Error: trying to push to a closed result queue"); }
			std::queue<T>::push(x);
			numResultsI++;
			numElements.notify();
		}

		template <class... _Args>
		inline void emplace(_Args&&... a) {
			elementAdded();
			std::lock_guard<std::mutex> l(m);
			if (isClosedB) { throw thread_safe_queue_error("Error: trying to push to a closed result queue"); }
			std::queue<T>::emplace(a...);
			numResultsI++;
			numElements.notify();
		}

		bool eof() const {
			std::lock_guard<std::mutex> l(m);
			return errorState;
		}

		inline explicit operator bool() const { return !eof(); }

		thread_safe_queue& operator>>(T& val) {
			try {
				val = pop();
			} catch (...) {}
			return *this;
		}

		thread_safe_queue& operator<<(const T& val) {
			try {
				push(val);
			} catch (...) {}
			return *this;
		}

		thread_safe_queue& operator<<(T&& val) {
			try {
				push(val);
			} catch (...) {}
			return *this;
		}
	};
}// namespace estd
