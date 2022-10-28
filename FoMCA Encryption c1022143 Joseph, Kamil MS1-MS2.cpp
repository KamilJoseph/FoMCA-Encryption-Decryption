// The text encryption program in C++ and ASM with a very simple example encryption method - it simply adds 1 to the character.

#include <string>     // for std::string
#include <chrono>     // for date & time functions
#include <ctime>      // for date & time string functions
#include <iostream>   // for std::cout <<
#include <fstream>    // for file I/O
#include <iomanip>    // for fancy output
#include <functional> // for std::reference_wrapper
#include <vector>     // for std::vector container

constexpr char const * STUDENT_NAME = "Kamil Joseph";        // Replace with your full name
constexpr int  ENCRYPTION_ROUTINE_ID = 3;                    // Replace -1 with your encryption id
constexpr char ENCRYPTION_KEY = 'g';                         // Replace '?' with your encryption key
constexpr int  MAX_CHARS = 6;                                // feel free to alter this, but must be 6 when submitting!

constexpr char STRING_TERMINATOR = '$';                      // custom string terminator
constexpr char LINE_FEED_CHARACTER = '\n';                   // line feed character (hhhmmm, this comment seems a bit unnecassary...)
constexpr char CARRIAGE_RETURN_CHARACTER = '\r';             // carriage return character

char original_chars[MAX_CHARS] ;                             // Original character string
char encrypted_chars[MAX_CHARS];                             // Encrypted character string
char decrypted_chars[MAX_CHARS];                             // Decrypted character string, don't forget to delete default value when testing your decryption!


//---------------------------------------------------------------------------------------------------------------
//----------------- C++ FUNCTIONS -------------------------------------------------------------------------------

/// <summary>
/// get a single character from the user via Windows function _getwche
/// </summary>
/// <param name="a_character">the resultant character, pass by reference</param>
void get_char (char& a_character)
{
  a_character = (char)_getwche (); // https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/getche-getwche

  __asm {
      or BYTE PTR[ecx],  32             // converts the upper case value of Ekey into lower case the difference between them is 32 
  }

  if (a_character == STRING_TERMINATOR) // skip further checks if user entered string terminating character
  {
    return;
  }
  if (a_character == LINE_FEED_CHARACTER || a_character == CARRIAGE_RETURN_CHARACTER)  // treat all 'new line' characters as terminating character
  {
    a_character = STRING_TERMINATOR;
    return;
  }

}

//---------------------------------------------------------------------------------------------------------------
//----------------- ENCRYPTION ROUTINE --------------------------------------------------------------------------

/// <summary>
/// 'encrypt' the characters in original_chars, up to length
/// output 'encrypted' characters in to encrypted_chars
/// </summary>
/// <param name="length">length of string to encrypt, pass by value</param>
/// <param name="EKey">encryption key to use during encryption, pass by value</param>
void encrypt_chars (int length, char EKey)
{
  char temp_char;                    // Temporary character store

  for (int i = 0; i < length; ++i)   // Encrypt characters one at a time
  {
    temp_char = original_chars [i];  // Get the next char from original_chars array
                                     
    __asm
    {
      push   eax                     // pushing edx, ecx and eax on to the stack to reuse 
      push   ecx                     // *******************//
      push   edx                     // ******************//

      lea    eax, EKey               // loading address value of ekey "parameter 2" into eax register. 

      movzx  ecx, temp_char          // storing parameter 1 value into ecx reg

      push edx                       // push the values that you want to pass as parameters before the call instruction      
      push ecx                       // *******************//
      push eax                       // *******************//

      call   encrypt_3               // Call function

      add esp, 12                    // removing parameters from the stack scrubbing the stack
        
      mov    temp_char, dl           // moves the lower 8 bit value stores in eax into temp_char
      mov    temp_char, al           // *******************// 
     
      pop    edx                     // Restoring the value of the registers
      pop    ecx                     // *******************//
      pop    eax                     // *******************//
    }

    encrypted_chars [i] = temp_char; // Store encrypted char in the encrypted_chars array
  }

  return;

  // Inputs: register EAX = 32-bit address of Ekey
  //                  ECX = the character to be encrypted (in the low 8-bit field, CL)
  // Output: register EDX = the encrypted value of the source character (in the low 8-bit field, DL)

  __asm
  {
  encrypt_3:

      push ebp                         // same call site base pointer on to the stack. Setup new stack frame.
          mov ebp, esp                 // **************************//

          mov eax, [ebp + 8]           // accessing parameters and moving it into registers. 
          mov ecx, [ebp + 12]          // **************************//
          mov edx, [ebp + 10]          // **************************//


          push  edx                    // pushing new value of registers onto the stack. 
          push  ecx                    // **************************//
          push  eax                    // **************************//

          movzx eax, byte ptr[eax]     // retrieving the actual value stored in eax
          rol   al, 1                  // rotating destination to left of lower 8 bit of al value 
          not   al                     // inverts al registers. Changing its value 
          rol   al, 1                  // rotating left 2 times the value of al
          rol   al, 1                  // **************************//

          mov   edx, eax               // moves the mangled value of eax into edx
          pop   eax                    // restores value of eax reg ('Ekey')
          mov   byte ptr[eax], dl      // moves lower bit address value of edx into eax
          pop   ecx                    // restoring the value of ecx 
          xor ecx, edx                 // swaps each others value
          mov   eax, ecx               // moves new value of ecx into eax register

          ror   al, 1                  // rotating right value of al 3 times.
          ror   al, 1                  // **************************//
          ror   al, 1                  // **************************//

          pop   edx                    // restores value of edx     
          mov   edx, eax               // moving new value of eax into edx mangles with ekey again
                               
          mov esp, ebp 
          pop ebp                      // pop the original value of EBP back into EBP from the stack. 

          ret
  }


}
//*** end of encrypt_chars function
//---------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
//----------------- DECRYPTION ROUTINE --------------------------------------------------------------------------

/// <summary>
/// 'decrypt' the characters in encrypted_chars, up to length
/// output 'decrypted' characters in to decrypted_chars
/// </summary>
/// <param name="length">length of string to decrypt, pass by value</param>
/// <param name="EKey">encryption key to used during the encryption process, pass by value</param>
void decrypt_chars (int length, char EKey)
{
    char temp_char;                    // Temporary character store

    for (int i = 0; i < length; ++i)   // Encrypt characters one at a time
    {
        temp_char = original_chars[i]; // Get the next char from original_chars array
                                       // Note the lamentable lack of comments below!
        __asm
        {
            push   eax                 // pushing edx, ecx and eax on to the stack to reuse 
            push   ecx                 //
            push   edx                 //

            lea    eax, EKey           // Storing value of Ekey into eax
            movzx  ecx, temp_char      // Storing value of temp_char used for later

            push edx                   // push the values that you want to pass as parameters before the call instruction      
            push ecx                   // ********************//
            push eax                   // ********************//

            call   decrypt_3           // Call function

            add esp, 12                // removing parameters from the stack. Scrubing the stack. 

            mov    temp_char, dl       // Stores lower 8 bit value of edx/eax registers onto  tempchar 
            mov    temp_char, al       //

            pop    edx                 // restoring value of the register 
            pop    ecx                 // *******************//
            pop    eax                 // *******************//
        }

       decrypted_chars[i] = temp_char; // Store encrypted char in the encrypted_chars array
    }

    return;

    __asm
    {
    decrypt_3:
        push ebp                       // same call site base pointer on to the stack. Setup new stack frame.
            mov ebp, esp               // **************************//

            push  edx                  // pushing new value of registers onto the stack. 
            push  ecx                  // **************************//
            push  eax                  // **************************//

            movzx eax, byte ptr[eax]   // retrieving the actual value stored in eax

            rol   al, 1                // mangles the value of Ekey
            not   al                   // *************************//
            rol   al, 1                // *************************//
            rol   al, 1                // *************************//
            
            mov   edx, eax             // moves the mangled value of eax into edx
            pop   eax                  // restores value of eax reg ('Ekey')
            mov   byte ptr[eax], cl    // moves lower bit address value of ecx into eax

            ror   cl, 1                // mangling the value of ecx register 3 times 
            ror   cl, 1                // ************************//
            ror   cl, 1                // ************************//


            pop   ecx                  // restoring the value of ecx 
            mov   eax, ecx             // moves new value of ecx into eax register


            mov esp, ebp               // pop the original value of EBP back into EBP from the stack.
            pop ebp                      


            ret
    }
}
//*** end of decrypt_chars function
//---------------------------------------------------------------------------------------------------------------





//************ MAIN *********************************************************************************************
//************ YOU DO NOT NEED TO EDIT ANYTHING BELOW THIS LINE *************************************************
//************ BUT FEEL FREE TO HAVE A LOOK *********************************************************************

void get_original_chars (int& length)
{
  length = 0;

  char next_char;
  do
  {
    next_char = 0;
    get_char (next_char);
    if (next_char != STRING_TERMINATOR)
    {
      original_chars [length++] = next_char;
    }
  }
  while ((length < MAX_CHARS) && (next_char != STRING_TERMINATOR));
}

std::string get_date ()
{
  std::time_t now = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now ());
  char buf[16] = { 0 };
  tm time_data;
  localtime_s (&time_data, &now);
  std::strftime (buf, sizeof(buf), "%d/%m/%Y", &time_data);
  return std::string{ buf };
}

std::string get_time ()
{
  std::time_t now = std::chrono::system_clock::to_time_t (std::chrono::system_clock::now ());
  char buf[16] = { 0 };
  tm time_data;
  localtime_s (&time_data, &now);
  std::strftime (buf, sizeof (buf), "%H:%M:%S", &time_data);
  return std::string{ buf };
}

// support class to help output to multiple streams at the same time
struct multi_outstream
{
  void add_stream (std::ostream& stream)
  {
    streams.push_back (stream);
  }

  template <class T>
  multi_outstream& operator<<(const T& data)
  {
    for (auto& stream : streams)
    {
      stream.get () << data;
    }
    return *this;
  }

private:
  std::vector <std::reference_wrapper <std::ostream>> streams;
};

int main ()
{
  int char_count = 0;  // The number of actual characters entered (upto MAX_CHARS limit)

  std::cout << "Please enter upto " << MAX_CHARS << " alphabetic characters: ";
  get_original_chars (char_count);	// Input the character string to be encrypted


  //*****************************************************
  // Open a file to store results (you can view and edit this file in Visual Studio)

  std::ofstream file_stream;
  file_stream.open ("log.txt", std::ios::app);
  file_stream << "Date: " << get_date () << " Time: " << get_time () << "\n";
  file_stream << "Name:                  " << STUDENT_NAME << "\n";
  file_stream << "Encryption Routine ID: '" << ENCRYPTION_ROUTINE_ID << "'" << "\n";
  file_stream << "Encryption Key:        '" << ENCRYPTION_KEY;

  multi_outstream output;
  output.add_stream (file_stream);
  output.add_stream (std::cout);


  //*****************************************************
  // Display and save to the log file the string just input

  output << "\n\nOriginal string  = ";
  output << std::right << std::setw (MAX_CHARS) << std::setfill (' ') << original_chars;

  // output each original char's hex value
  output << " Hex = ";
  for (int i = 0; i < char_count; ++i)
  {
    int const original_char = static_cast <int> (original_chars [i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::right << std::setw (2) << std::setfill ('0') << original_char << " ";
  }


  //*****************************************************
  // Encrypt the string and display/save the result

  encrypt_chars (char_count, ENCRYPTION_KEY);

  output << "\n\nEncrypted string = ";
  output << std::right << std::setw (MAX_CHARS) << std::setfill (' ') << encrypted_chars;

  // output each encrypted char's hex value
  output << " Hex = ";
  for (int i = 0; i < char_count; ++i)
  {
    int const encrypted_char = static_cast <int> (encrypted_chars [i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::right << std::setw (2) << std::setfill ('0') << encrypted_char << " ";
  }


  //*****************************************************
  // Decrypt the encrypted string and display/save the result

  decrypt_chars (char_count, ENCRYPTION_KEY);

  output << "\n\nDecrypted string = ";
  output << std::right << std::setw (MAX_CHARS) << std::setfill (' ') << decrypted_chars;

  // output each decrypted char's hex value
  output << " Hex = ";
  for (int i = 0; i < char_count; ++i)
  {
    int const decrypted_char = static_cast <int> (decrypted_chars [i]) & 0xFF; // ANDing with 0xFF prevents static_cast padding 8 bit value with 1s
    output << std::hex << std::right << std::setw (2) << std::setfill ('0') << decrypted_char << " ";
  }


  //*****************************************************
  // End program

  output << "\n\n";
  file_stream << "-------------------------------------------------------------\n\n";
  file_stream.close ();

  system ("PAUSE"); // do not use in your other programs! just a hack to pause the program before exiting

  return 0;
}

//**************************************************************************************************************
