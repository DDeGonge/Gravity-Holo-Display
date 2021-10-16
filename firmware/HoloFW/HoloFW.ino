#include <string>
#include "HoloLib.h"
#include "Coms.h"
#include "Pins.h"


#define flash_delay_us 300000

#define Serial SERIAL_PORT_USBVIRTUAL

using namespace std;

void setup() {
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW);
//  while (!Serial);
}

void loop() {
  uint16_t nValves = 64;
  Valve valves[nValves];
  for (uint16_t i = 0; i < nValves; i++)
  {
    valves[i].set_drive_pin(valvePins[i]);
    valves[i].set_tuning(vel_tune[i], delay_tune[i]);
  }
  
  char serial_data[MAX_MSG_LEN];
  unsigned long startTime_us = micros();
  unsigned long t_elapsed_us;
  bool allValvesDone, lastResult;
  while (true)
  {
    t_elapsed_us = micros() - startTime_us;
    clear_data(serial_data);
    if (respondToSerial(serial_data)) 
    {
      // Parse input into data chunks
      if (parse_serial(serial_data, valves))
      {
        uint32_t flashtime = micros() + flash_delay_us;
  
        // Calculate timestamps for all valves
        for (uint16_t i = 0; i < nValves; i++)
        {
          valves[i].calculate_timestamps(flashtime);
        }
  
        // Run valve cycles until all are done
        while (true)
        {
          allValvesDone = true;
          for (uint16_t i = 0; i < nValves; i++)
          {
            lastResult = valves[i].cycle(micros());
            allValvesDone = !lastResult ? false : allValvesDone;
          }
          if (allValvesDone) break;
        }
  
        // Flash LED
        delayMicroseconds(flashtime - micros());
//        Serial.print(micros());
//        Serial.println(" FLASH");
        digitalWrite(led_pin, HIGH);
        delayMicroseconds(flash_len_us);
        digitalWrite(led_pin, LOW);
      }

      Serial.println("ok");
    }
  }
}

//void test_loop() {
//  Valve onevalve(test_pin);
//
//  float testdists[MAX_PIXELS] = {100,150,200,250,-1,-1,-1,-1}; // {0,50,90,130,170,210,-1,-1};
//  Serial.println(micros());
//  uint32_t flashtime = micros() + 300000;
//  onevalve.calculate_timestamps(testdists, flashtime);
//
//  while (true)
//  {
//    if (onevalve.cycle(micros())) break;
//  }
//
////  delay(3000);
//}

bool parse_serial(char ser_data[], struct Valve * allValves)
{
  char delim = ',', dStart = '-', dEnd = '|', purge = 'p';
  uint32_t index = 0;
  uint32_t valve_index = 0;
  string temp_arg_str = "";
  while (ser_data[index] != '\0') {
    temp_arg_str += ser_data[index];
    index++;
    if (ser_data[index] == dStart)
    {
      valve_index = stoi(temp_arg_str);
      temp_arg_str = "";
      index++;
    }
    else if (ser_data[index] == delim)
    {
      allValves[valve_index].add_single_dist(stoi(temp_arg_str));
      temp_arg_str = "";
      index++;
    }
    else if (ser_data[index] == dEnd)
    {
      allValves[valve_index].add_single_dist(stoi(temp_arg_str));
      temp_arg_str = "";
      index++;
    }
    else if (ser_data[index] == purge)
    {
      for (uint16_t i = 0; i < 8; i++)
      {
        allValves[i].open_valve();
        delay(30);
      }
      for (uint16_t i = 0; i < 8; i++)
      {
        allValves[i].close_valve();
        delay(30);
      }
//      for (uint16_t i = 0; i < 8; i++)
//        allValves[i].purge(100);
      return false;
    }
  }
  return true;
}
