
Valve::Valve(uint8_t drive_pin, float vi, float ms)
{
  set_drive_pin(drive_pin);
  set_tuning(vi, ms);
}

void Valve::set_drive_pin(uint8_t drive_pin)
{
  drivepin = drive_pin;
  pinMode(drivepin, OUTPUT);
  close_valve();
  clear_dists();
}

void Valve::set_tuning(float vi, float ms)
{
  cal_vi = vi;
  cal_delay = ms;
}

void Valve::calculate_timestamps(uint32_t flashtime_us)
{
  frame_complete = false;
  trigger_index = 0;
//
//  Serial.print("~~~~~~~\n");
//  Serial.println(drivepin);

  uint8_t active_pixels = 0;
  for (uint8_t i = 0; i < MAX_PIXELS; i++)
  {
    if (distances_mm[i] < 0) trigger_times_us[i] = 0;
    else
    {
      active_pixels += 1;
      float dropdist = floor_offset_mm - distances_mm[i];
      float est_trigtime = (-cal_vi + pow(pow(cal_vi, 2) - (2 * 9810 * -dropdist), 0.5)) / 9810;
      est_trigtime += (cal_delay / 1000);
      uint32_t trigtime = flashtime_us - (uint32_t)(est_trigtime * 1000000);
      
      float vi = calc_vi(last_trigger, trigtime);
      est_trigtime = (-vi + pow(pow(cal_vi, 2) - (2 * 9810 * -dropdist), 0.5)) / 9810;
      est_trigtime += (cal_delay / 1000);
      trigger_times_us[i] = flashtime_us - (uint32_t)(est_trigtime * 1000000);
      last_trigger = trigtime;
    }
  }

  if (active_pixels == 0)
  {
    frame_complete = true;
    return;
  }

  // debug printing
//  Serial.println("NEWSEG");
//  for (uint8_t i = 0; i < MAX_PIXELS; i++)
//  {
//    Serial.print(trigger_times_us[i]);
//    Serial.print("\t");
//    Serial.print(trigger_lens_us[i]);
//    Serial.print("\n");
//  }

  // Shift times based on time since last dispense
//  for (uint8_t i = 0; i < MAX_PIXELS; i++)
//  {
//    if (i == 0)
//    {
//      trigger_times_us[i] -= calc_extra_time(last_trigger, trigger_times_us[i]);
//    }
//    else if (trigger_times_us[i] > 0)
//    {
//      trigger_times_us[i] -= calc_extra_time(trigger_times_us[i-1], trigger_times_us[i]);
//    }
//  }

  // calculate valve open times based on time since last dispense
  for (uint8_t i = 0; i < MAX_PIXELS; i++)
  {
    if (i == 0)
    {
      trigger_lens_us[i] = calc_trigger_time(last_trigger, trigger_times_us[i]);
    }
    else if (trigger_times_us[i] > 0)
    {
      trigger_lens_us[i] = calc_trigger_time(trigger_times_us[i-1], trigger_times_us[i]);
    }
    else
    {
      last_trigger = trigger_times_us[i-1];
      break;
    }
  }

  // debug printing
  
//  for (uint8_t i = 0; i < MAX_PIXELS; i++)
//  {
//    Serial.print(trigger_times_us[i]);
//    Serial.print("\t");
//    Serial.print(trigger_lens_us[i]);
//    Serial.print("\n");
//  }

  clear_dists();
}

void Valve::add_single_dist(int16_t newdist)
{
  distances_mm[next_dist_i] = newdist;
  next_dist_i++;
}

void Valve::clear_dists()
{
  next_dist_i = 0;
  for (uint16_t d_i = 0; d_i < MAX_PIXELS; d_i++)
  {
    distances_mm[d_i] = -1;
  }
}

float Valve::calc_vi(uint32_t last_t, uint32_t this_t)
{
  uint32_t t_diff = this_t - last_t;
  if (t_diff < vi_ramp_start_us) return (cal_vi + vi_max_delta);
  if (t_diff > vi_ramp_end_us) return cal_vi;
  float percent = (t_diff - vi_ramp_start_us);
  percent /= (vi_ramp_end_us - vi_ramp_start_us);
  return (1 - percent) * vi_max_delta + cal_vi;
}

uint32_t Valve::calc_trigger_time(uint32_t last_t, uint32_t this_t)
{
  uint32_t t_diff = this_t - last_t;
  if (t_diff < trigger_time_ramp_start_us) return trigger_time_us_min;
  if (t_diff > trigger_time_ramp_end_us) return trigger_time_us_max;
  float percent = (t_diff - trigger_time_ramp_start_us);
  percent /= (trigger_time_ramp_end_us - trigger_time_ramp_start_us);
  return (percent * (trigger_time_us_max - trigger_time_us_min) + trigger_time_us_min);
}

uint32_t Valve::calc_extra_time(uint32_t last_t, uint32_t this_t)
{
  uint32_t t_diff = this_t - last_t;
  if (t_diff < extratime_ramp_start_us) return extratime_us_min;
  if (t_diff > extratime_ramp_end_us) return extratime_us_max;
  float percent = (t_diff - extratime_ramp_start_us);
  percent /= (extratime_ramp_end_us - extratime_ramp_start_us);
  return (percent * (extratime_us_max - extratime_us_min) + extratime_us_min);
}

bool Valve::cycle(uint32_t timenow_us)
{
  if (frame_complete) return true;

  if ((timenow_us > (trigger_times_us[trigger_index] - trigger_lens_us[trigger_index])) && (valve_open == false))
  {
    open_valve();
  }

  if ((timenow_us > trigger_times_us[trigger_index]) && (valve_open == true))
  {
    close_valve();
    trigger_index++;
    if (trigger_times_us[trigger_index] == 0)
    {
      frame_complete = true;
    }
  }

  return frame_complete;
}

void Valve::open_valve()
{
//  Serial.print(micros());
//  Serial.print(" OPENING ");
//  Serial.println(drivepin);
  digitalWrite(drivepin, HIGH);
  valve_open = true;
}

void Valve::close_valve()
{
//  Serial.print(micros());
//  Serial.print(" CLOSING ");
//  Serial.println(drivepin);
  digitalWrite(drivepin, LOW);
  valve_open = false;
}

void Valve::purge(uint32_t len_ms)
{
  open_valve();
  delay(len_ms);
  close_valve();
}
