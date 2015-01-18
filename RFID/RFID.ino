void setup() {
  Serial.begin(9600); //This is the speed at which the ID-12LA's serial connection operates
}

void loop () {
  //We declare these variables here so their values are set on each iteration of the loop() function
  byte i = 0;
  byte val = 0;
  byte tag_id[6];
  byte checksum = 0;
  byte bytesread = 0;
  byte tempbyte = 0;

  //Check to see if Serial has available data
  if(Serial.available() > 0) {
    //The transmission begins with a 'Start of Text' character (ASCII code = 2)
    if((val = Serial.read()) == 2) {
      //We want to read the next 12 ASCII characters (5 hexadecimal bytes followed by a checksum: each hexadecimal byte is two ASCII characters)
      bytesread = 0; 
      while (bytesread < 12) {
        //Check that the next byte is ready to be read by the Arduino
        if( Serial.available() > 0) {
          //Read the next characters
          val = Serial.read();
          
          //If we get a carriage return, line feed, Start of Text or End of Text before receiving 12 normal ASCII characters then stop
          if((val == 0x0D)||(val == 0x0A)||(val == 0x03)||(val == 0x02)) { 
            break;
          }

          // Convert ASCII recieved to hexadecimal for display
          if ((val >= '0') && (val <= '9')) {
            val = val - '0';
          } else if ((val >= 'A') && (val <= 'F')) {
            val = 10 + val - 'A';
          }

          //When we recieve two ASCII codes convert them to a single hexadecimal number
          if (bytesread & 1 == 1) { //This is a bitwise method of seeing if bytesread is even
            //Store the hexadecimal value in tag_id
            tag_id[bytesread >> 1] = (val | (tempbyte << 4));
            
            //If we're not at the checksum byte, we need to calculate the checksum
            if (bytesread >> 1 != 5) {
              checksum ^= tag_id[bytesread >> 1];
            };
            
          //If we have not yet recieved the next pair of ASCII characters store the single value we just read
          } else {
            tempbyte = val;
          };

          //Move to the next chracter to read
          bytesread++;
        } 
      } 

      //If we have read 12 bytes we have the tag id and the checksum
      if (bytesread == 12) {
        //If our checksum matches the recieved checksum
        if (tag_id[5] == checksum) {
          for (i=0; i<5; i++) {
            if (tag_id[i] < 16) Serial.print("0");
            Serial.print(tag_id[i], HEX);
            Serial.print(" ");
          }
          Serial.println();
        } else {
          Serial.println("Checksum failed");
        }
      //Reset bytesread
      bytesread = 0;
      }

    }
  }
}
