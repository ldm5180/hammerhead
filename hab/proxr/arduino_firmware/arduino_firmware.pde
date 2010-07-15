int active = 1;
int ack = -1;
int previous_value = 0;
int analogIn[2];
int digitalIn[8] = {0,0,0,0,0,0,0,0};

 void setup() 
 {
  Serial.begin(9600);
  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);  
  pinMode(5, INPUT);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
 }
 
 void loop()
 { 
   delay(2000);
   
   while(active == 1)
   {  
      delay(1000);
      
      previous_value = analogIn[0];
      analogIn[0] = analogRead(0);
      if(analogIn[0] != previous_value)
      {
	  Serial.print(100, BYTE);
     	  Serial.print(analogIn[0]);
          Serial.print('\n');
      }

      previous_value = analogIn[1];
      analogIn[1] = analogRead(1);
      if(analogIn[1] != previous_value)
      {
	  Serial.print(101, BYTE);
          Serial.print(analogIn[1]);
          Serial.print('\n');
      }     
   
      for(int i=0; i<8; i++)
      {
          previous_value = digitalIn[i];
          digitalIn[i] = digitalRead(i+2);
          if(digitalIn[i] != previous_value)
          {
	      Serial.print(110+i, BYTE);
	      Serial.print(digitalIn[i]);
              Serial.print('\n');
          }
      }
   }
 }
