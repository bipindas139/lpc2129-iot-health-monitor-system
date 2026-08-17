#include "header.h"

static u32 get_smooth(u32 *buffer, int idx, int max_len) {
	if (idx < 2 || idx >= (max_len - 2)) {
		return buffer[idx];
	}
	return (buffer[idx-2] + buffer[idx-1] + buffer[idx] + buffer[idx+1] + buffer[idx+2]) / 5;
}

void calculate_hr_spo2(u32 *ir_buffer, u32 *red_buffer, int buffer_length, 
		int *heart_rate, float *spo2, 
		int *hr_valid, int *spo2_valid) 
{
	int i;
	unsigned long int ir_sum = 0;
	unsigned long int red_sum = 0;
	u32 ir_dc, red_dc;

	u32 ir_max = 0, red_max = 0;
	u32 ir_min = 0xFFFFFFFF, red_min = 0xFFFFFFFF;
	u32 ir_ac, red_ac;
	u32 threshold;
	int last_peak_idx;
	int peak_count ;
	int sum_intervals ;

	// 1. Calculate DC components (Mean) and find Min/Max for AC Peak-to-Peak
	for (i = 0; i < buffer_length; i++) {
		ir_sum += ir_buffer[i];
		red_sum += red_buffer[i];

		if (ir_buffer[i] > ir_max) ir_max = ir_buffer[i];
		if (ir_buffer[i] < ir_min) ir_min = ir_buffer[i];

		if (red_buffer[i] > red_max) red_max = red_buffer[i];
		if (red_buffer[i] < red_min) red_min = red_buffer[i];
	}

	// DC is the average of the buffer. (Fits safely in 32-bit unsigned long)
	ir_dc = ir_sum / buffer_length;
	red_dc = red_sum / buffer_length;

	// AC is the Peak-to-Peak amplitude of the signal
	ir_ac = ir_max - ir_min;
	red_ac = red_max - red_min;

	// 2. Calculate SpO2 using standard Maxim AC/DC ratio approximation
	if (ir_ac > 0 && ir_dc > 0) {
		float ratio_red = (float)red_ac / (float)red_dc;
		float ratio_ir  = (float)ir_ac / (float)ir_dc;
		float ratio = ratio_red / ratio_ir;

		// Empirical SpO2 formula
		*spo2 = 110.0f - (25.0f * ratio);

		// Clamp limits
		if (*spo2 > 100.0f) *spo2 = 100.0f;
		if (*spo2 < 0.0f) *spo2 = 0.0f;

		*spo2_valid = (*spo2 > 50.0f && *spo2 <= 100.0f) ? 1 : 0;
	} else {
		*spo2 = -999.0f;
		*spo2_valid = 0;
	}

	// 3. Peak Detection for Heart Rate (Using IR Buffer)
	// Threshold is slightly above the DC mean to ignore baseline noise
	threshold = ir_dc + (ir_ac / 15); //4
	last_peak_idx = -1;
	peak_count = 0;
	sum_intervals = 0;

	for (i = 2; i < buffer_length - 2; i++) {
		u32 current_val = get_smooth(ir_buffer, i, buffer_length);
		u32 prev_val    = get_smooth(ir_buffer, i - 1, buffer_length);
		u32 next_val    = get_smooth(ir_buffer, i + 1, buffer_length);

		// Detect a local maximum above the threshold
		if (current_val > threshold && current_val > prev_val && current_val > next_val) {

			if (last_peak_idx != -1) {
				int interval = i - last_peak_idx;

				// Validate interval: At 100 SPS, 30 samples = 200 BPM. 150 samples = 40 BPM.
				if (interval > 30 && interval < 150) {
					sum_intervals += interval;
					peak_count++;
					last_peak_idx = i;
				} else if (interval >= 150) {
					// Interval too long, likely missed a beat due to noise. Reset timer.
					last_peak_idx = i;
				}
			} else {
				// First peak found
				last_peak_idx = i;
			}
		}
	}

	// 4. Calculate Final BPM
	if (peak_count > 0) {
		int avg_interval = sum_intervals / peak_count;
		// FS * 60 = 100 * 60 = 6000
		*heart_rate = 6000 / avg_interval; 
		*hr_valid = (*heart_rate >= 40 && *heart_rate <= 200) ? 1 : 0;
	} else {
		*heart_rate = -999;
		*hr_valid = 0;
	}
}
