int command = 0;
int analogIn[2];
int digitalIn[8];

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
   command = Serial.read();
   if(command == 100)
   {
     analogIn[0] = analogRead(0);
     Serial.println(analogIn[0]);
   }
   else if(command == 101)
   {
     analogIn[1] = analogRead(1);
     Serial.println(analogIn[1]);
   }
   else if(command == 200)
   {
     for(int i=0; i<8; i++)
     {
       digitalIn[i] = digitalRead(i+2); //i+2 because pinning
       Serial.print(digitalIn[i]);
     }
     Serial.println();
   } 
   else if(command == 254)
   {
     Serial.println("ok");
   }
 }
