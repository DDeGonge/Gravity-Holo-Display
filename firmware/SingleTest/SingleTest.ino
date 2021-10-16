#include <vector>

#define Serial SERIAL_PORT_USBVIRTUAL

#define test_pin0 5
#define test_pin1 6
#define led_pin 13

using namespace std;

void setup() {
  pinMode(test_pin0, OUTPUT);
  digitalWrite(test_pin0, LOW);
  pinMode(test_pin1, OUTPUT);
  digitalWrite(test_pin1, LOW);
  pinMode(led_pin, OUTPUT);
  digitalWrite(led_pin, LOW
  );
  while(!Serial);
  Serial.println("Drip test\nt,N_SETS,DROPS_PER_SET,MS_ON,MS_OFF\n");
  Serial.println("Purge nozzle\np,PURGE_TIME_MS\n");
  Serial.println("Light test\nl,N_SETS,DROPS_PER_SET,MS_ON,MS_OFF,MS_TO_FLASH\n\n");
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

void loop() {
  char serial_data[100];

  // Start main response loop
  while(true)
  {
    clear_data(serial_data);
    if(respondToSerial(serial_data)){
      switch (serial_data[0]){
        case 't':{
          // Drip test
          // t,N_SETS,DROPS_PER_SET,MS_ON,MS_OFF
          vector<int32_t> args;
          parse_inputs(serial_data, args);
          drip_test(args[0], args[1], args[2], args[3]);
          break;
        }
        case 'p':{
          // Purge
          // p,PURGE_TIME_MS
          vector<int32_t> args;
          parse_inputs(serial_data, args);
          nozzle_purge(args[0]);
          break;
        }
        case 'l':{
          // Drip test
          // l,N_SETS,DROPS_PER_SET,MS_ON,MS_OFF,MS_TO_FLASH
          vector<int32_t> args;
          parse_inputs(serial_data, args);
          flash_test(args[0], args[1], args[2], args[3], args[4]);
          break;
        }
        case 's':{
          custom();
          break;
        }
      }
      Serial.println("DONE");
    }
  }
}

void custom()
{
//  digitalWrite(test_pin, HIGH);
//  delay(t_on_ms);
//  digitalWrite(test_pin, LOW);
}

void nozzle_purge(uint32_t t_on_ms)
{
  digitalWrite(test_pin0, HIGH);
  digitalWrite(test_pin1, HIGH);
  delay(t_on_ms);
  digitalWrite(test_pin0, LOW);
  digitalWrite(test_pin1, LOW);
}

void drip_test(uint32_t n_sets, uint32_t drips_per_set, uint32_t t_on_ms, uint32_t t_off_ms)
{
  uint16_t t_between_sets_ms = 100;
  for(uint32_t set_num = 0; set_num < n_sets; set_num++)
  {
    for(uint32_t drip_num = 0; drip_num < drips_per_set; drip_num++)
    {
      digitalWrite(test_pin0, HIGH);
      digitalWrite(test_pin1, HIGH);
      if (drip_num == 0) delay(t_on_ms + 1);
      else delay(t_on_ms);
      digitalWrite(test_pin0, LOW);
      digitalWrite(test_pin1, LOW);
      delay(t_off_ms);
    }
    delay(t_between_sets_ms);
  }
}

void flash_test(uint32_t n_sets, uint32_t drips_per_set, uint32_t t_on_ms, uint32_t t_off_ms, uint32_t t_flash_ms)
{
  uint16_t t_between_sets_ms = 200;
  for(uint32_t set_num = 0; set_num < n_sets; set_num++)
  {
    uint32_t tStart = millis();
    for(uint32_t drip_num = 0; drip_num < drips_per_set; drip_num++)
    {
      digitalWrite(test_pin0, HIGH);
      digitalWrite(test_pin1, HIGH);
      if (drip_num == 0) delay(t_on_ms);
      else delay(t_on_ms);
      digitalWrite(test_pin0, LOW);
      digitalWrite(test_pin1, LOW);
      delay(t_off_ms);
    }
    if (millis() > (tStart + t_flash_ms))
    {
      Serial.println("ERROR flash delay too short");
    }
    while (millis() < (tStart + t_flash_ms)) {};
    digitalWrite(led_pin, HIGH);
    delay(2);
    digitalWrite(led_pin, LOW);
    delay(t_between_sets_ms);
  }
}

void parse_inputs(char serial_data[100], vector<int32_t> &args){
  char delim = ',';
  uint32_t index = 0;
  vector<char> temp_arg_char;

  // Gets past command char to first data
  while(serial_data[index] != delim){index++;}
  index++;
  while(serial_data[index] != '\0'){
    temp_arg_char.push_back(serial_data[index]);
    index++;
    if(serial_data[index] == delim){
      args.push_back(vtoi(temp_arg_char));
      temp_arg_char.clear();
      index++;
    }
  }
  args.push_back(vtoi(temp_arg_char));
}

int32_t vtoi(vector<char> vec)
{
  int32_t ret = 0;
  int mult = pow(10 , (vec.size()-1));

  for(int i = 0; i <= vec.size(); i++) {
    ret += (vec[i] - '0') * mult;
    mult /= 10;
  }
  return ret;
}

// Read serial messages if exist
bool respondToSerial(char (&serial_data) [100])
{
  uint8_t index = 0;
  if (Serial.available() > 0){
    while (Serial.available() > 0) {
      char newchar = Serial.read();
      if(newchar != '\n'){
        serial_data[index] = newchar;
        index++;
      }
      else{
        break;
      }
    }
    return true;
  }
  return false;
}

void clear_data(char (&serial_data) [100]){
  for(uint16_t i = 0; i < 100; i++){
    serial_data[i]='\0';
    }
}
