#include <Arduino.h>

#define MAX_PIXELS 8
#define floor_offset_mm 310
#define flash_len_us 2000

/* Signle actuator object */
struct Valve
{
  Valve() {};
  Valve(uint8_t drive_pin, float vi, float ms);

  public:

  void add_single_dist(int16_t newdist);
  void clear_dists();
  void calculate_timestamps(uint32_t flashtime_us);
  uint32_t calc_trigger_time(uint32_t last_t, uint32_t this_t);
  uint32_t calc_extra_time(uint32_t last_t, uint32_t this_t);
  float calc_vi(uint32_t last_t, uint32_t this_t);
  bool cycle(uint32_t timenow_us);

  void set_drive_pin(uint8_t drive_pin);
  void set_tuning(float vi, float ms);
  void open_valve();
  void close_valve();
  void purge(uint32_t len_ms);

  private:
  int16_t distances_mm[MAX_PIXELS] = {-1};
  uint16_t next_dist_i = 0;
  uint32_t trigger_times_us[MAX_PIXELS] = {0};  // Timestamps to drop (closing valve)
  uint32_t trigger_lens_us[MAX_PIXELS] = {0};   // How long to keep valve open
  uint8_t trigger_index = 0;      // Increment after closing valve
  uint32_t last_trigger = 0;
  bool valve_open = false;
  bool frame_complete = false;

  uint8_t drivepin;
  float cal_vi = 0;
  float cal_delay = 0;

  uint32_t vi_max_delta = 150;
  uint32_t vi_ramp_start_us = 25000;
  uint32_t vi_ramp_end_us = 80000;

  uint32_t trigger_time_us_min = 10000;
  uint32_t trigger_time_us_max = 10000;
  uint32_t trigger_time_ramp_start_us = 30000;
  uint32_t trigger_time_ramp_end_us = 60000;

  uint32_t extratime_us_min = 0;
  uint32_t extratime_us_max = 0; // 30000;
  uint32_t extratime_ramp_start_us = 25000;
  uint32_t extratime_ramp_end_us = 150000;
};
