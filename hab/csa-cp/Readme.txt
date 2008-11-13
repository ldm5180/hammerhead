
Copyright (C) 2004-2008, Regents of the University of Colorado.
This work was supported by NASA contracts NNJ05HE10G and NNC06CB40C.




1)Establishing communications

PC transmits 0x11 and looks for 0x22 from the instrument (loop)


2)Determining the number of sessions in the instrument
After establishing communications, the PC requests the number of sessions
(files) stored in the instrument:
PC transmits 0x01 and wait for 0x01 followed by the number of sessions
as illustrated in the code below


 int terminate = 300;
  num_of_recs = -1;  //number of records (sessions)

  
   do{
        if(!(terminate%40)) // every 40 counts,  check for messages in queue
             GetApplication()->PumpWaitingMessages();
        if(terminate==50){                // 
             gen_delay(200);                // delay 200ms before proceeding
             flush_buf();          //flush the buffer
                           }
        if(SWCReceiveCharacter(PortId)==0x01)
         {
          gen_delay(10);
          num_of_recs = SWCReceiveCharacter(PortId);
          gen_delay(10);
          terminate = 1;
           }
       SWCTransmitCharacter(PortId, 0x01);
       gen_delay(10);
      } while(--terminate);


3)Downloading a session 

 3.1- PC transmits 0x82
 3.2- PC transmits the number of the session to download
 3.3- PC transmits 0x02
 3.4- PC receives 0x02
 3.5- PC receives the number of the session requested to download

 (Steps 3.1 to 3.5 are repeated until the requested session to download is received
from the instrument.)


 3.6- PC sends 0x03
 3.7- PC receives 0x03
 3.8- 
 //the CSA-CP downloads in handshake mode using the following code

  int in, timeout;
  short int data_index=0;
  TDialog *DwnDlg;
  FILE *log_out, *log_out2;
  char line_buf[68];
  char filename_unf[120];
  short int cnt = 0;

 do{
  in = SWCWaitForCharacter(PortId,60000L);

  if((in!=(-1)) && (in!=SWCTIMEDOUT) )
  {

    timeout = 0;
    in &= 0x007F;

    SWCTransmitCharacter(PortId, in);
    // WaitForTransmit();

   if( ShowProgressBar )
         DwnDlgGauge->UpdateGauge(increm);


    if(in==0x00)  // end of file
      end_download=true;
    else{
      if(in=='\r')
            in = '\n';

     line_buf[data_index++] = in;

     if((in=='\n')||(data_index>66)){
       line_buf[data_index] = '\0';
       fprintf(log_out,"%s",line_buf);
       fprintf(log_out2,"%s",line_buf);
       data_index = 0;
       ++linenum;
                                   }
     } // close if it's not end of file
  }  // close if (in!=(-1) )

  else{
      if((++timeout>10000)||(in==SWCTIMEDOUT) )
      // if(++timeout>10000)
         {
         timeout = 22000;
         end_download=true;
         }
      }

 }while(end_download==false);


4) Clearing all the data in the instrument
 4.1- PC transmits 0x04
 4.2- PC receives 0x04
 4.3- PC receives 0


Please note that the instrument will timeout if it stays idle for over 5 seconds.









 
