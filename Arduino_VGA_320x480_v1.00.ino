// set Arduino Nano FuseA from default 0xff to 0xbf (enabling output of 16MHz CLKO on pin 8)

//#include <avr/sleep.h>        // not needed
#pragma GCC optimize ("O2")

/*
80 91 b2 00   lds r24, 0x00B2 ; 2 cycles, TCNT2
85 30         cpi r24, 0x05   ; 1 cycle
10 f4         brcc  .+4       ; 1 cycle and 2 cycles if branching
88 e1         ldi r24, 0x18   ; 1 cycle
85 b9         out 0x05, r24   ; 1 cycle, PORTB
28 e1         ldi r18, 0x18   ; 1 cycle
25 b9         out 0x05, r18   ; 1 cycle, PORTB

  PORTD = cset[*vrow++];      ; total 8 cycles
20 81         ld  r18, Z      ; 2 cycles
dc 01         movw  r26, r24  ; 1 cycles
a2 0f         add r26, r18    ; 1 cycles
b1 1d         adc r27, r1     ; 1 cycles
2c 91         ld  r18, X      ; 2 cycles
2b b9         out 0x0b, r18   ; 1 cycles, PORTD
*/

int vLine;
byte vram[25][40] = { 
  "Hello, World!",
  "",
  "This is a 16MHz Arduino Nano generating ",
  " - 320x480 pixels with 60Hz refresh rate",
  " - 40 characters text output per line   ",
  " - set of 128 characters with 8x8 pixels",
  " - writing to the VRAM or doing some I/O",
  "",
  "0123456789012345678901234567890123456789",
  "",                               
  "    **** PCI/OS BASIC V2 ****",                               
  "",                               
  " 64K RAM SYSTEM  38911 BASIC BYTES FREE",                               
  "",                               
  "READY.",                               
};

const byte charset[8][128] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x66, 0x66, 0x18, 0x62, 0x3c, 0x06, 0x0c, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x18, 0x3c, 0x3c, 0x06, 0x7e, 0x3c, 0x7e, 0x3c, 0x3c, 0x00, 0x00, 0x0e, 0x00, 0x70, 0x3c, 0x00, 0x18, 0x7c, 0x3c, 0x78, 0x7e, 0x7e, 0x3c, 0x66, 0x3c, 0x1e, 0x66, 0x60, 0x63, 0x66, 0x3c, 0x7c, 0x3c, 0x7c, 0x3c, 0x7e, 0x66, 0x66, 0x63, 0x66, 0x66, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x0c, 0x3c, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x66, 0x66, 0x3e, 0x66, 0x66, 0x0c, 0x18, 0x18, 0x66, 0x18, 0x00, 0x00, 0x00, 0x03, 0x66, 0x18, 0x66, 0x66, 0x0e, 0x60, 0x66, 0x66, 0x66, 0x66, 0x00, 0x00, 0x18, 0x00, 0x18, 0x66, 0x00, 0x3c, 0x66, 0x66, 0x6c, 0x60, 0x60, 0x66, 0x66, 0x18, 0x0c, 0x6c, 0x60, 0x77, 0x76, 0x66, 0x66, 0x66, 0x66, 0x66, 0x18, 0x66, 0x66, 0x63, 0x66, 0x66, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x00, 0x60, 0x00, 0x06, 0x00, 0x0e, 0x00, 0x60, 0x18, 0x06, 0x60, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x12, 0x0c, 0x18, 0x10,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x66, 0xff, 0x60, 0x0c, 0x3c, 0x18, 0x30, 0x0c, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x06, 0x6e, 0x38, 0x06, 0x06, 0x1e, 0x7c, 0x60, 0x0c, 0x66, 0x66, 0x18, 0x18, 0x30, 0x7e, 0x0c, 0x06, 0x00, 0x66, 0x66, 0x60, 0x66, 0x60, 0x60, 0x60, 0x66, 0x18, 0x0c, 0x78, 0x60, 0x7f, 0x7e, 0x66, 0x66, 0x66, 0x66, 0x60, 0x18, 0x66, 0x66, 0x63, 0x3c, 0x66, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x3c, 0x60, 0x3c, 0x06, 0x3c, 0x18, 0x3e, 0x60, 0x00, 0x00, 0x60, 0x18, 0x66, 0x7c, 0x3c, 0x7c, 0x3e, 0x7c, 0x3e, 0x7e, 0x66, 0x66, 0x63, 0x66, 0x66, 0x7e, 0x30, 0x30, 0x0c, 0x3c, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x66, 0x3c, 0x18, 0x38, 0x00, 0x30, 0x0c, 0xff, 0x7e, 0x00, 0x7e, 0x00, 0x0c, 0x76, 0x18, 0x0c, 0x1c, 0x66, 0x06, 0x7c, 0x18, 0x3c, 0x3e, 0x00, 0x00, 0x60, 0x00, 0x06, 0x0c, 0x00, 0x7e, 0x7c, 0x60, 0x66, 0x78, 0x78, 0x6e, 0x7e, 0x18, 0x0c, 0x70, 0x60, 0x6b, 0x7e, 0x66, 0x7c, 0x66, 0x7c, 0x3c, 0x18, 0x66, 0x66, 0x6b, 0x18, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x06, 0x7c, 0x60, 0x3e, 0x66, 0x3e, 0x66, 0x7c, 0x38, 0x06, 0x6c, 0x18, 0x7f, 0x66, 0x66, 0x66, 0x66, 0x66, 0x60, 0x18, 0x66, 0x66, 0x6b, 0x3c, 0x66, 0x0c, 0x30, 0x7c, 0x0c, 0x7e, 0x7f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0x06, 0x30, 0x67, 0x00, 0x30, 0x0c, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x18, 0x66, 0x18, 0x30, 0x06, 0x7f, 0x06, 0x66, 0x18, 0x66, 0x06, 0x00, 0x00, 0x30, 0x7e, 0x0c, 0x18, 0x00, 0x66, 0x66, 0x60, 0x66, 0x60, 0x60, 0x66, 0x66, 0x18, 0x0c, 0x78, 0x60, 0x63, 0x6e, 0x66, 0x60, 0x66, 0x78, 0x06, 0x18, 0x66, 0x66, 0x7f, 0x3c, 0x18, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x3e, 0x66, 0x60, 0x66, 0x7e, 0x18, 0x66, 0x66, 0x18, 0x06, 0x78, 0x18, 0x7f, 0x66, 0x66, 0x66, 0x66, 0x60, 0x3c, 0x18, 0x66, 0x66, 0x7f, 0x18, 0x66, 0x18, 0x30, 0x30, 0x0c, 0x18, 0x7f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x7c, 0x66, 0x66, 0x00, 0x18, 0x18, 0x66, 0x18, 0x18, 0x00, 0x18, 0x30, 0x66, 0x18, 0x60, 0x66, 0x06, 0x66, 0x66, 0x18, 0x66, 0x66, 0x18, 0x18, 0x18, 0x00, 0x18, 0x00, 0x00, 0x66, 0x66, 0x66, 0x6c, 0x60, 0x60, 0x66, 0x66, 0x18, 0x6c, 0x6c, 0x60, 0x63, 0x66, 0x66, 0x60, 0x3c, 0x6c, 0x66, 0x18, 0x66, 0x3c, 0x77, 0x66, 0x18, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x66, 0x66, 0x66, 0x60, 0x66, 0x60, 0x18, 0x3e, 0x66, 0x18, 0x06, 0x6c, 0x18, 0x6b, 0x66, 0x66, 0x7c, 0x3e, 0x60, 0x06, 0x18, 0x66, 0x3c, 0x3e, 0x3c, 0x3e, 0x30, 0x30, 0x62, 0x0c, 0x18, 0x30,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x66, 0x18, 0x46, 0x3f, 0x00, 0x0c, 0x30, 0x00, 0x00, 0x18, 0x00, 0x18, 0x60, 0x3c, 0x7e, 0x7e, 0x3c, 0x06, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x00, 0x18, 0x0e, 0x00, 0x70, 0x18, 0x00, 0x66, 0x7c, 0x3c, 0x78, 0x7e, 0x60, 0x3c, 0x66, 0x3c, 0x38, 0x66, 0x7e, 0x63, 0x66, 0x3c, 0x60, 0x0e, 0x66, 0x3c, 0x18, 0x3c, 0x18, 0x63, 0x66, 0x18, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x3e, 0x7c, 0x3c, 0x3e, 0x3c, 0x18, 0x06, 0x66, 0x3c, 0x06, 0x66, 0x3c, 0x63, 0x66, 0x3c, 0x60, 0x06, 0x60, 0x7c, 0x0e, 0x3e, 0x18, 0x36, 0x66, 0x0c, 0x7e, 0x3c, 0xfc, 0x3c, 0x18, 0x10,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00,
};

int main(void)
{
  setup();
  while(true) loop();
}

void setup()
{
  noInterrupts();                                 // disable interrupts before messing around with timer registers
  //set_sleep_mode(SLEEP_MODE_IDLE);              // not needed

  DDRC  = 0b11111111;
  DDRB  = 0b11011101;                             // pin8: CLKO, pin 10: VSYNC (timer1), pin 11: shift register latch (timer2), pin 12: HSync "by hand" inside ISR
  DDRD  = 0b11111111;                             // pins 0-7: parallel output
  PORTB = 0b00010000;
  PORTD = 0b00000000;

  GTCCR = 0b10000011;                             // set TSM, PSRSYNC und PSRASY to correlate all 3 timers

  // *************************
  // ***** Timer0: HSYNC *****
  // *************************
  TCNT0  = 0;
  TCCR0A = (1<<WGM01) | (0<<WGM00);               // mode 2: Clear Timer on Compare Match (CTC)
  TCCR0B = (0<<WGM02) | (0<<CS02) | (1<<CS01) | (0<<CS00); // x8 prescaler -> 0.5µs
  OCR0A  = 63;                                    // compare match register A (TOP) -> 32µs
  TIMSK0 = (1<<OCIE0A);                           // Output Compare Match A Interrupt Enable (not working: TOIE1 with ISR TIMER0_TOIE1_vect because it is already defined by timing functions)
  
  // *************************
  // ***** Timer1: VSYNC *****
  // *************************
  TCNT1  = 0;
  TCCR1A = (1<<COM1B1) | (1<<COM1B0) | (1<<WGM11) | (1<<WGM10);         // mode 15 (Fast PWM), set OC1B on Compare Match, clear OC1B at BOTTOM, controlling OC1B pin 10
  TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (1<<CS10); // x1024 prescaler -> 64µs
  OCR1A  = 259;                                   // compare match register A (TOP) -> 16.64ms
  OCR1B  = 0;                                     // compare match register B -> 64µs
  TIMSK1 = (1<<TOIE1);                            // enable timer overflow interrupt setting vLines = 0

  // ***************************
  // ***** Timer2: Load PE *****
  // ***************************
  TCNT2  = 0;
  TCCR2A = (1<<COM2A1) | (1<<COM2A0) | (1<<WGM21) | (1<<WGM20); // mode 7: Fast PWM, COM2A0=0: normal port HIGH, COM2A0=1: Toggle OC2A pin 11 on Compare Match
  TCCR2B = (1<<WGM22) | (0<<CS22) | (0<<CS21) | (1<<CS20) ;     // set x0 prescaler -> 62.5ns;
  OCR2A  = 7;                                     // compare match register A (TOP) -> 250ns
  TIMSK2 = 0;                                     // no interrupts here

  GTCCR = 0;                                      // clear TSM => all timers start synchronously
  
  UCSR0B = 0;                       	            // brute-force the USART off just in case...

  interrupts();
}

ISR(TIMER0_COMPA_vect)                            // VSYNC and scanline drawing
{
  if (TCNT2 & 1) PORTB = 16;                      // canceling out interrupt jitter using the fast timer
  PORTB = 16;                                     // if branching it takes a cycle longer

  PORTB = 0;                                      // start of HSYNC pulse
    PORTD = 0;                                    // prevent trash data from being fed into the shift register
    vLine++;                                      // use the time during the pulse to init pointers
    byte* vrow = vram[byte((vLine-36)>>4)];       // pointer to the vram row 0...24 to display
    byte* cset = charset[byte((vLine-36)>>1) & 0b00000111]; // pointer to the charset line 0..7 to use
    __builtin_avr_delay_cycles(36);               // should be 49 (3.8µs) but than this routine gets mysteriously unstable, also influences the phase at which data is fed to the shift register
  PORTB = 16;                                     // end of HSYNC pulse

  if (vLine >= 36 && vLine < 436)                 // skip 2 lines (VSYNC pulse) + 33 lines (vertical back porch)
  {  
    TCCR2A &= ~(1<<COM2A1);                       // enable OC2A toggling pin 11
    PORTD = cset[*vrow++];                        // start feeding data
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++]; 
    PORTD = cset[*vrow++];    
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++]; 
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++]; 
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];   
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    PORTD = cset[*vrow++];
    __builtin_avr_delay_cycles(2);                // wait for the last byte to toggle into the shift register
    TCCR2A |= (1<<COM2A1);                        // disable OC2A toggling pin 11
    PORTD = 0;                                    // following toggles will load zero
  }
}

void loop()
{
  //sleep_mode();                                 // not required
  for(int i=0; i<400; i++) { vram[15][i]++; vram[15][i] &= 127; }   // change VRAM
}

ISR(TIMER1_OVF_vect)
{
  vLine = 0;
}
