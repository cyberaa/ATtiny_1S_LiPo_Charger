#include <avr/sleep.h>
#include <avr/wdt.h>

#define chargePin 0
#define lipoPin A3
#define vccPin A1
#define ledPin 0
#define SLEEPTIME 200
/*
  LED configuration:
  red : charging (source) green : complete (sink)
*/

#ifndef cbi
#define cbi(sfr,bit)(_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr,bit)(_SFR_BYTE(sfr) |= ~_BV(bit))
#endif

float lipoVoltage=0, vcc=0;
volatile boolean f_wdt = 1;

void setup()
{
  pinMode(chargePin,OUTPUT);
  digitalWrite(chargePin,LOW);
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,LOW);
  initWatchdog(6);  // sleep for 1 sec
}

void loop()
{
  if(f_wdt)
    f_wdt = 0;
  vcc = (float)map(analogRead(vccPin),0,1023,0,5.15);
  if(vcc > 5.10)
    reduceVcc();
  lipoVoltage = (float)map(analogRead(lipoPin),0,1023,0.00,4.10);
  if(lipoVoltage > 4.05)
  {
    digitalWrite(chargePin,LOW);
    digitalWrite(ledPin,HIGH);
  }
  else
  {
    digitalWrite(chargePin,HIGH);
    digitalWrite(ledPin,LOW);
  }
  gotoSleep();
}

void reduceVcc()
{
  digitalWrite(chargePin,HIGH);
  digitalWrite(ledPin,HIGH);
  delay(500);
  digitalWrite(chargePin,LOW);
  digitalWrite(ledPin,LOW);
  delay(500);
}


void initWatchdog(uint8_t time)
{
  /*
  watchdog time (time to wake up after entering sleep)
  0 = 16ms
  1 = 32ms
  2 = 64ms
  3 = 128ms
  4 = 256ms
  5 = 512ms
  6 = 1024ms
  7 = 2048ms
  8 = 4096ms
  9 = 8192ms
  */
  byte bb;
  int ww;
  if(time > 9)
    time = 9;
  if(time > 7)
    time |= (1<<5);
  time |= (1<<WDCE);
  ww = bb;
  MCUSR &= ~(1<<WDRF);
  WDTCR |= (1<<WDCE)|(1<<WDE);
  WDTCR = bb;
  WDTCR |= _BV(WDIE);
}

ISR(WDT_vect)
{
  f_wdt = 1;
}

void gotoSleep()
{
  cbi(ADCSRA,ADEN);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
  sleep_disable();
  sbi(ADCSRA,ADEN);
}
