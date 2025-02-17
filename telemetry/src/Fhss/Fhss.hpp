/// CATS Flight Software
/// Copyright (C) 2022 Control and Telemetry Systems
///
/// This program is free software: you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published by
/// the Free Software Foundation, either version 3 of the License, or
/// (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <Random.hpp>
#include <Sx1280Driver.hpp>

#define FreqCorrectionMax ((int32_t)(100000 / FREQ_STEP))
#define FreqCorrectionMin ((int32_t)(-100000 / FREQ_STEP))

#define FREQ_HZ_TO_REG_VAL(freq) ((uint32_t)((double)(freq) / (double)FREQ_STEP))

extern volatile uint8_t FHSSptr;
extern uint8_t FHSSsequence[];
extern const uint32_t FHSSfreqs[];
extern uint_fast8_t sync_channel;
extern const uint8_t FHSS_SEQUENCE_CNT;

// create and randomise an FHSS sequence
void FHSSrandomiseFHSSsequence(uint32_t crc);
// The number of frequencies for this regulatory domain
uint32_t FHSSgetChannelCount(void);

// get the initial frequency, which is also the sync channel
static inline uint32_t GetInitialFreq() { return FHSSfreqs[sync_channel]; }

// Get the current sequence pointer
static inline uint8_t FHSSgetCurrIndex() { return FHSSptr; }

// Set the sequence pointer, used by RX on SYNC
static inline void FHSSsetCurrIndex(const uint8_t value) { FHSSptr = value % FHSS_SEQUENCE_CNT; }

// Advance the pointer to the next hop and return the frequency of that channel
static inline uint32_t FHSSgetNextFreq() {
  FHSSptr = (FHSSptr + 1) % FHSS_SEQUENCE_CNT;
  uint32_t freq = FHSSfreqs[FHSSsequence[FHSSptr]];
  return freq;
}

// get the number of entries in the FHSS sequence
static inline uint8_t FHSSgetSequenceCount() { return FHSS_SEQUENCE_CNT; }
