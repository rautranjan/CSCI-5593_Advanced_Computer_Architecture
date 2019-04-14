/*
 * Copyright (c) 2007 Mark D. Hill and David A. Wood
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __MEM_RUBY_STRUCTURES_ADPGPOLICY_HH__
#define __MEM_RUBY_STRUCTURES_ADPGPOLICY_HH__
#include "mem/ruby/structures/AbstractReplacementPolicy.hh"
#include "params/ADPGReplacementPolicy.hh"
#define PARTS 6

struct Cell {
	uint8_t priority :2;

	Cell() :
			priority(0) {
	}
};

class Partition {
	int ptr = 0, prePTR = 0;
	int startSet, endSet;

public:

	void init(int startSet, int endSet) {
		this->startSet = startSet;
		this->endSet = endSet;
	}

	void setPTR(Cell **cache, int m_assoc) {

		prePTR = ptr;
		ptr = 0;
		for (int i = startSet; i <= endSet; i++) {
			for (int j = 0; j < m_assoc; j++) {
				ptr += cache[i][j].priority + 1;
			}
		}

	}

	bool getPTRFluctuation() {
		if (ptr > prePTR)
			return 1; //increment in partition
		else
			return 0; //equals or decrement in partition value
	}

	int getEndSet() const {
		return endSet;
	}

	int getStartSet() const {
		return startSet;
	}

	int getPtr() const {
		return ptr;
	}

	int getPrePtr() const {
			return prePTR;
	}
};

/* Simple true ADPG replacement policy */

class ADPGPolicy: public AbstractReplacementPolicy {
public:
	typedef ADPGReplacementPolicyParams Params;
	ADPGPolicy(const Params * p);
	~ADPGPolicy();

	void touch(int64_t set, int64_t way, Tick time);
	int64_t getVictim(int64_t set) const;

	Cell **cache = new Cell*[m_num_sets];
	int gTR = 0, preGTR = 0; // GTR Registers

	Partition *partition;

	uint8_t incrementPriority(uint8_t priority) {

		switch (priority) {
		case 0:
			return 1;
		case 1:
			return 2;
		case 2:
			return 3;
		case 3:
			return 3;
		}

	}

	uint8_t decrementPriority(uint8_t priority) {

		switch (priority) {
		case 0:
			return 0;
		case 1:
			return 0;
		case 2:
			return 1;
		case 3:
			return 2;
		}

	}

	Partition * getPartitions(int m_num_sets) {
		int partValue = m_num_sets / PARTS;
		int startSetIndex = 0, endSetIndex = (partValue - 1), i;

		static Partition partition[PARTS];
		for (i = 0; i < PARTS - 1; i++) {

			partition[i].init(startSetIndex, endSetIndex);
			startSetIndex = endSetIndex + 1;
			endSetIndex += partValue;

		}

		partition[i].init(startSetIndex, m_num_sets - 1);

		return partition;
	}
};

#endif // __MEM_RUBY_STRUCTURES_ADPGPOLICY_HH__