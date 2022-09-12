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

#include <initializer_list>
#include <ostream>
#include <vector>

namespace estd {

	class ostream_proxy : public std::ostream {
	private:
		std::vector<std::ostream*> fwd = {};

	public:
		ostream_proxy& operator=(const ostream_proxy& other) {
			this->fwd = other.fwd;
			return *this;
		}

		ostream_proxy() {}

		ostream_proxy(std::initializer_list<std::ostream*> ostrms) {
			for (auto& ostrm : ostrms) this->forward(ostrm);
		}

		ostream_proxy(const ostream_proxy& other) { this->fwd = other.fwd; }

		ostream_proxy(ostream_proxy&& other) { this->fwd = other.fwd; }

		template <typename T>
		ostream_proxy& operator<<(T s) {
			for (auto ostrm : fwd) { (*ostrm) << s; }

			return *this;
		}
		ostream_proxy& operator<<(std::ios& (*pf)(std::ios&)) {
			for (auto ostrm : fwd) { (*ostrm) << pf; }

			return *this;
		}
		ostream_proxy& operator<<(std::ios_base& (*pf)(std::ios_base&)) {
			for (auto ostrm : fwd) { (*ostrm) << pf; }

			return *this;
		}
		ostream_proxy& operator<<(std::ostream& (*pf)(std::ostream&)) {
			for (auto ostrm : fwd) { (*ostrm) << pf; }

			return *this;
		}
		void forward(std::ostream& ostrm) { fwd.push_back(&ostrm); }
		void forward(std::ostream* ostrm) { fwd.push_back(ostrm); }
	};

};// namespace estd
