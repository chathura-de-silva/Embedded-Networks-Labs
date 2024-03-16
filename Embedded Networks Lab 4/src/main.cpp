#include <Arduino.h>

void delay_ms(int milidelay);
void delay_us(long microdelay);

// Initialize serial communication with the specified baud rate
void initializeSerialCommunication(unsigned long baudRate)
{
  // Calculate the baud rate register value
  UBRR0 = (F_CPU / 16 / baudRate) - 1;

  // Enable transmitter
  UCSR0B |= (1 << TXEN0);

  // Set frame format: 8 data bits, 1 stop bit
  UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

// Transmit a single character over serial communication
void transmitCharacter(unsigned char data)
{
  // Wait for the transmit buffer to be empty
  while (!(UCSR0A & (1 << UDRE0)))
    ;

  // Transmit the data
  UDR0 = data;
}

// Transmit an integer over serial communication
void transmitInteger(int number)
{
  // If the number is a single digit, transmit it directly
  if (number >= 0 && number < 10)
  {
    transmitCharacter(number + '0');
    return;
  }

  // Transmit each digit recursively
  transmitInteger(number / 10);
  transmitCharacter((number % 10) + '0');
}

// Transmit an integer followed by a newline character over serial communication
void transmitIntegerWithNewline(int number)
{
  // Transmit the integer
  transmitInteger(number);

  // Wait for the transmit buffer to be empty
  while (!(UCSR0A & (1 << UDRE0)))
    ;

  // Transmit a newline character
  UDR0 = '\n';
}

int counter = 0;

int main()
{
  initializeSerialCommunication(9600);
  while (true)
  {
    for (int i = 0; i < 5;i++)
    {
      delay_ms(20);
      counter++;
    }
    transmitIntegerWithNewline(counter);
  }
}

void delay_ms(int milidelay)
{
  OCR1A = 250;
  int i = 0;
  TCNT1 = 0;

  TCCR1B |= (1 << CS10) | (1 << CS11);
  while (i < milidelay)
  {
    if (TIFR1 & (1 << OCF1A))
    {
      TCNT1 = 0;
      TIFR1 |= (1 << OCF1A);
      i += 1;
    }
  }
  TCNT1 = 0;
}

void delay_us(long microdelay)
{
  TCCR0B |= (1 << CS00);
  OCR0A = 16;
  long i = 0;
  TCNT0 = 0;
  while (i < microdelay)
  {
    if (TCNT0 & (1 << OCF0A))
    {
      TCNT0 = 0;
      TIFR0 |= 1 << OCF0A;
      i++;
    }
  }
  TCNT0 = 0;
}
