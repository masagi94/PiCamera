/*
* File name   : clock.c
* Description : ...
* Author      : Tali Whittemore, Mauricio Salazar Giraldo
* Date        : 2018/04/09
*/
#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <lcd.h>

//#define LED1 1
//#define LED2 2

// The wiringPi pins the buttons are connected to
#define BUTTON1 25
#define BUTTON2 24
#define OFFBUTTON 20
#define LED 23

// ADC wiringPi pin connections
#define ADC_CLK 14
#define ADC_CS 10
#define ADC_DIO 6

// These variables are used to track the current camera mode.
int mode = 0;
int zoom = 0;
int usbReady = 0;
int takePicture = 0;
int offButton = 0;


// Helper functions
void ButtonISR();
void configure();
void led_on();
void led_off();
void blink_led();
void createPicCommand();
void updateLCD();
void setZoom();
int ADCResult();
void onLCD();
void offLCD();

// These variables are the different configurations for pictures
// that we can take with the camera.

char normalPic[70] = "/normalPic.jpg --timeout 1 --nopreview";
char negPic[75] = "/negPic.jpg -ifx negative --timeout 1 --nopreview";
char sketchPic[75] = "/sketchPic.jpg -ifx sketch --timeout 1 --nopreview";
char embossPic[75] = "/embossPic.jpg -ifx emboss --timeout 1 --nopreview";
char oilPic[75] = "/oilPic.jpg -ifx oilpaint --timeout 1 --nopreview";
char cartoonPic[75] = "/cartoonPic.jpg -ifx cartoon --timeout 1 --nopreview";

char zoom2xPic[70] = "/zoom2xPic.jpg -roi 0.2,0.2,1,1 --timeout 1 --nopreview";
char negZoom2xPic[75] = "/negZoom2xPic.jpg -ifx negative -roi 0.2,0.2,1,1 --timeout 1 --nopreview";
char sketchZoom2xPic[75] = "/sketchZoom2xPic.jpg -ifx sketch -roi 0.2,0.2,1,1 --timeout 1 --nopreview";
char embossZoom2xPic[75] = "/embossZoom2xPic.jpg -ifx emboss -roi 0.2,0.2,1,1 --timeout 1 --nopreview";
char oilZoom2xPic[75] = "/oilZoom2xPic.jpg -ifx oilpaint -roi 0.2,0.2,1,1 --timeout 1 --nopreview";
char cartoonZoom2xPic[75] = "/cartoonZoom2xPic.jpg -ifx cartoon -roi 0.2,0.2,1,1 --timeout 1 --nopreview";

char zoom4xPic[70] = "/zoom4xPic.jpg -roi 0.4,0.4,1,1 --timeout 1 --nopreview";
char negZoom4xPic[75] = "/negZoom4xPic.jpg -ifx negative -roi 0.4,0.4,1,1 --timeout 1 --nopreview";
char sketchZoom4xPic[75] = "/sketchZoom4xPic.jpg -ifx sketch -roi 0.4,0.4,1,1 --timeout 1 --nopreview";
char embossZoom4xPic[75] = "/embossZoom4xPic.jpg -ifx emboss -roi 0.4,0.4,1,1 --timeout 1 --nopreview";
char oilZoom4xPic[75] = "/oilZoom4xPic.jpg -ifx oilpaint -roi 0.4,0.4,1,1 --timeout 1 --nopreview";
char cartoonZoom4xPic[75] = "/cartoonZoom4xPic.jpg -ifx cartoon -roi 0.4,0.4,1,1 --timeout 1 --nopreview";

char zoom6xPic[70] = "/zoom6xPic.jpg -roi 0.6,0.6,1,1 --timeout 1 --nopreview";
char negZoom6xPic[75] = "/negZoom6xPic.jpg -ifx negative -roi 0.6,0.6,1,1 --timeout 1 --nopreview";
char sketchZoom6xPic[75] = "/sketchZoom6xPic.jpg -ifx sketch -roi 0.6,0.6,1,1 --timeout 1 --nopreview";
char embossZoom6xPic[75] = "/embossZoom6xPic.jpg -ifx emboss -roi 0.6,0.6,1,1 --timeout 1 --nopreview";
char oilZoom6xPic[75] = "/oilZoom6xPic.jpg -ifx oilpaint -roi 0.6,0.6,1,1 --timeout 1 --nopreview";
char cartoonZoom6xPic[75] = "/cartoonZoom6xPic.jpg -ifx cartoon -roi 0.6,0.6,1,1 --timeout 1 --nopreview";

char zoom8xPic[70] = "/zoom8xPic.jpg -roi 0.8,0.8,1,1 --timeout 1 --nopreview";
char negZoom8xPic[75] = "/negZoom8xPic.jpg -ifx negative -roi 0.8,0.8,1,1 --timeout 1 --nopreview";
char sketchZoom8xPic[75] = "/sketchZoom8xPic.jpg -ifx sketch -roi 0.8,0.8,1,1 --timeout 1 --nopreview";
char embossZoom8xPic[75] = "/embossZoom8xPic.jpg -ifx emboss -roi 0.8,0.8,1,1 --timeout 1 --nopreview";
char oilZoom8xPic[75] = "/oilZoom8xPic.jpg -ifx oilpaint -roi 0.8,0.8,1,1 --timeout 1 --nopreview";
char cartoonZoom8xPic[75] = "/cartoonZoom8xPic.jpg -ifx cartoon -roi 0.8,0.8,1,1 --timeout 1 --nopreview";

char zoom10xPic[70] = "/zoom10xPic.jpg -roi 1,1,1,1 --timeout 1 --nopreview";
char negZoom10xPic[75] = "/negZoom10xPic.jpg -ifx negative -roi 1,1,1,1 --timeout 1 --nopreview";
char sketchZoom10xPic[75] = "/sketchZoom10xPic.jpg -ifx sketch -roi 1,1,1,1 --timeout 1 --nopreview";
char embossZoom10xPic[75] = "/embossZoom10xPic.jpg -ifx emboss -roi 1,1,1,1 --timeout 1 --nopreview";
char oilZoom10xPic[75] = "/oilZoom10xPic.jpg -ifx oilpaint -roi 1,1,1,1 --timeout 1 --nopreview";
char cartoonZoom10xPic[75] = "/cartoonZoom10xPic.jpg -ifx cartoon -roi 1,1,1,1 --timeout 1 --nopreview";






char picPath[50];
//char UsbName[20];
 //FILE *fp;

int fd;
int ISRBlock = 0;



// The main execution of the code
int main(){
    char picCommand[25];
    char usbName[20];
    char openCommand[20];
    wiringPiSetup();

    // Sets up the signals and buttons for use
    configure();


    // When initialize wiring failed, print message to screen
    if(wiringPiSetup() == -1){
        printf("setup wiringPi failed !\n");
        return -1;
    }

    fd = lcdInit(2,16,4, 5,4, 0,1,2,3,4,5,6,7);
    //lcdClear(fd);
    if (fd == -1){
        printf ("lcdInit failed\n") ;
        return 1;
    }
    

    onLCD();


    while(offButton == 0){
        
        // printf("usb1: %s\n", usbName);
        createPicCommand(picCommand, usbName);
        printf("%s\n", picCommand);
        updateLCD(fd, usbName);
        delay(100);

        int num = (int) ADCResult();
        int result = (num*100)/255;
                
        //printf("%d\n",result);

        setZoom(result);

        
        if(takePicture == 1){
            if(usbReady == 1){

                printf("TAKING PIC\n");
                led_on(LED);
                createPicCommand(picCommand, usbName);
                
                delay(500);
                printf("-%s\n", picCommand);
                delay(500);
                int ret1 = system(picCommand);
                
                //printf("ret1 = %d\n", ret1);
                
                if (ret1 == 0){
                    updateLCD(fd, "Picture Saved!");
                }
                else{
                    updateLCD(fd, "Error");
                    blink_led(LED);
                }

                delay(2000);
                led_off(LED);
            }
        
            else{
                updateLCD(fd, "No USB Found!");
                blink_led(LED);
                delay(2000);
            }

            takePicture = 0;  
        }
    }

    offLCD();


    return 0;
}


void setZoom(int level){
    if(0 <= level && level <= 17)
        zoom = 0;
    else if(18 <= level && level <= 35)
        zoom = 2;
    else if(36 <= level && level <= 51)
        zoom = 4;
    else if(52 <= level && level <= 68)
        zoom = 6;
    else if(69 <= level && level <= 85)
        zoom = 8;
    else if(86 <= level && level <= 100)
        zoom = 10;
}




void onLCD(){
    lcdPosition(fd, 0, 0);
    lcdPrintf(fd, "%s", "                ");
    lcdPosition(fd, 0, 0);
    lcdPrintf(fd, "%s", "Powering on...");
    lcdPosition(fd, 0, 1);
    lcdPrintf(fd, "%s", "                ");

    delay(2000);
}

void offLCD(){

    lcdClear(fd);
    delay(100);
    lcdPrintf(fd, "%s", "Powering off...");
    lcdPosition(fd, 0, 0);
    lcdPosition(fd, 0, 1);
    lcdPrintf(fd, "%s", "   Goodbye :)   ");
    delay(2000);
    lcdClear(fd);
    delay(500);
}


void updateLCD(int fd, char * title){
    char camMode[20] = "";
    char currentZoom[5] = "";

    sprintf(currentZoom, "%d", zoom);

    lcdPosition(fd, 0, 0);
    lcdPrintf(fd, "%s", "                ");
    lcdPosition(fd, 0, 0);
    lcdPrintf(fd, "%s", title);

    if(mode == 0){
        strcat(camMode, "Normal   - ");
    }
    else if(mode == 1){
        strcat(camMode, "Negative - ");
    }
    else if(mode == 2){
        strcat(camMode, "Sketch - ");
    }
    else if(mode == 3){
        strcat(camMode, "Emboss - ");
    }
    else if(mode == 4){
        strcat(camMode, "Oil - ");
    }
    else if(mode == 5){
        strcat(camMode, "Cartoon - ");
    }

    strcat(camMode, currentZoom);
    strcat(camMode, "x");



    lcdPosition(fd, 0, 1);
    lcdPrintf(fd, "%s", "                ");
    lcdPosition(fd, 0, 1);
    lcdPrintf(fd, "%s", camMode);
    
    delay(100);
}

// This helper function will return the path to the connected USB
// device. This way, any USB can be used, since the path to the 
// device will update automatically once it's plugged in.
void createPicCommand(char * command, char * usbName){

    FILE *fp;
    char path[1035] = "";

    fp = popen("ls /media/pi/", "r");

    if (fp == NULL){
        printf("Failed to run command\n");
        exit(1);
    }

    fgets(path, sizeof(path)-1, fp);
    strtok(path, "\n");

    // If no device connected, cant take pictures
    if(strcmp(path,"") == 0){
        usbReady = 0;
        strcpy(usbName, "Connect USB");
    }
    else{
        usbReady = 1;
        strcpy(command, "raspistill -o /media/pi/");
        strcpy(usbName, path);
        strcat(command, path);
        strcpy(picPath, "/media/pi/");
        strcat(picPath, path);
        
        if(zoom == 0){
            if(mode == 0){
                strncat(command, normalPic, 200);
                strcat(picPath, "/normalPic.jpg");
            }
            else if(mode == 1){
                strncat(command, negPic, 200);
                strcat(picPath, "/negPic.jpg"); 
            }
            else if(mode == 2){
                strncat(command, sketchPic, 200);
                strcat(picPath, "/sketchPic.jpg"); 
            }
            else if(mode == 3){
                strncat(command, embossPic, 200);
                strcat(picPath, "/embossPic.jpg"); 
            }
            else if(mode == 4){
                strncat(command, oilPic, 200);
                strcat(picPath, "/oilPic.jpg"); 
            }
            else if(mode == 5){
                strncat(command, cartoonPic, 200);
                strcat(picPath, "/cartoonPic.jpg"); 
            }   
        }
        else if(zoom == 2){
            if(mode == 0){
                strncat(command, zoom2xPic, 200);
                strcat(picPath, "/zoom2xPic.jpg");
            }
            else if(mode == 1){
                strncat(command, negZoom2xPic, 200);
                strcat(picPath, "/negZoom2xPic.jpg");
            }
            else if(mode == 2){
                strncat(command, sketchZoom2xPic, 200);
                strcat(picPath, "/sketchZoom2xPic.jpg"); 
            }
            else if(mode == 3){
                strncat(command, embossZoom2xPic, 200);
                strcat(picPath, "/embossZoom2xPic.jpg"); 
            }
            else if(mode == 4){
                strncat(command, oilZoom2xPic, 200);
                strcat(picPath, "/oilZoom2xPic.jpg"); 
            }
            else if(mode == 5){
                strncat(command, cartoonZoom2xPic, 200);
                strcat(picPath, "/cartoonZoom2xPic.jpg"); 
            }   
        }
        else if(zoom == 4){
            if(mode == 0){
                strncat(command, zoom4xPic, 200);
                strcat(picPath, "/zoom4xPic.jpg");
            }
            else if(mode == 1){
                strncat(command, negZoom4xPic, 200);
                strcat(picPath, "/negZoom4xPic.jpg");
            }
            else if(mode == 2){
                strncat(command, sketchZoom4xPic, 200);
                strcat(picPath, "/sketchZoom4xPic.jpg"); 
            }
            else if(mode == 3){
                strncat(command, embossZoom4xPic, 200);
                strcat(picPath, "/embossZoom4xPic.jpg"); 
            }
            else if(mode == 4){
                strncat(command, oilZoom4xPic, 200);
                strcat(picPath, "/oilZoom4xPic.jpg"); 
            }
            else if(mode == 5){
                strncat(command, cartoonZoom4xPic, 200);
                strcat(picPath, "/cartoonZoom4xPic.jpg"); 
            }   
        }
        else if(zoom == 6){
            if(mode == 0){
                strncat(command, zoom6xPic, 200);
                strcat(picPath, "/zoom6xPic.jpg");
            }
            else if(mode == 1){
                strncat(command, negZoom6xPic, 200);
                strcat(picPath, "/negZoom6xPic.jpg");
            }
            else if(mode == 2){
                strncat(command, sketchZoom6xPic, 200);
                strcat(picPath, "/sketchZoom6xPic.jpg"); 
            }
            else if(mode == 3){
                strncat(command, embossZoom6xPic, 200);
                strcat(picPath, "/embossZoom6xPic.jpg"); 
            }
            else if(mode == 4){
                strncat(command, oilZoom6xPic, 200);
                strcat(picPath, "/oilZoom6xPic.jpg"); 
            }
            else if(mode == 5){
                strncat(command, cartoonZoom6xPic, 200);
                strcat(picPath, "/cartoonZoom6xPic.jpg"); 
            }   
        }
        else if(zoom == 8){
            if(mode == 0){
                strncat(command, zoom8xPic, 200);
                strcat(picPath, "/zoom8xPic.jpg");
            }
            else if(mode == 1){
                strncat(command, negZoom8xPic, 200);
                strcat(picPath, "/negZoom8xPic.jpg");
            }
            else if(mode == 2){
                strncat(command, sketchZoom8xPic, 200);
                strcat(picPath, "/sketchZoom8xPic.jpg"); 
            }
            else if(mode == 3){
                strncat(command, embossZoom8xPic, 200);
                strcat(picPath, "/embossZoom8xPic.jpg"); 
            }
            else if(mode == 4){
                strncat(command, oilZoom8xPic, 200);
                strcat(picPath, "/oilZoom8xPic.jpg"); 
            }
            else if(mode == 5){
                strncat(command, cartoonZoom8xPic, 200);
                strcat(picPath, "/cartoonZoom8xPic.jpg"); 
            }   
        }
        else if(zoom == 10){
            if(mode == 0){
                strncat(command, zoom10xPic, 200);
                strcat(picPath, "/zoom10xPic.jpg");
            }
            else if(mode == 1){
                strncat(command, negZoom10xPic, 200);
                strcat(picPath, "/negZoom10xPic.jpg");
            }
            else if(mode == 2){
                strncat(command, sketchZoom10xPic, 200);
                strcat(picPath, "/sketchZoom10xPic.jpg"); 
            }
            else if(mode == 3){
                strncat(command, embossZoom10xPic, 200);
                strcat(picPath, "/embossZoom10xPic.jpg"); 
            }
            else if(mode == 4){
                strncat(command, oilZoom10xPic, 200);
                strcat(picPath, "/oilZoom10xPic.jpg"); 
            }
            else if(mode == 5){
                strncat(command, cartoonZoom10xPic, 200);
                strcat(picPath, "/cartoonZoom10xPic.jpg"); 
            }   
        }
    }
    
    pclose(fp);
}


// Button that toggles between the different picture modes.
void Button1ISR(){
   
    if(mode == 5){
        mode = 0;
    }
    else{
        mode++;
    }
}

// When button 2 is pressed, capture a picture with the current settings.
void Button2ISR(){
    if(takePicture == 0)
        takePicture = 1;
}

void OffButtonISR(){
    printf("Off Button Pressed\n");
    offButton = 1;
}



// Configures the signals and buttons for use. The buttons are connected
// to pull-up resistors to avoid floating values, since they all connect to ground.
void configure(){
    //configuring LEDS, button, ADC, H bridge
    pinMode(LED, OUTPUT);
    //pinMode(LED2, OUTPUT);

    pinMode(BUTTON1, INPUT);
    pinMode(BUTTON2, INPUT);
    pinMode(OFFBUTTON, INPUT);
    //pinMode(BUTTON3, INPUT);

    pinMode(ADC_CS, OUTPUT);
    pinMode(ADC_CLK, OUTPUT);
    // pinMode(ADC_MISO, INPUT);
    // pinMode(ADC_MOSI, OUTPUT);
    pinMode(ADC_DIO, OUTPUT);
 


    // use pull-up resistors for buttons 
    pullUpDnControl(BUTTON1, PUD_UP);
    pullUpDnControl(BUTTON2, PUD_UP);
    pullUpDnControl(OFFBUTTON, PUD_UP);
    //pullUpDnControl(BUTTON3, PUD_UP);



    // use pull-down resistors for ADC
    pullUpDnControl(ADC_CS, PUD_DOWN);
    pullUpDnControl(ADC_CLK, PUD_DOWN);
    // pullUpDnControl(ADC_MISO, PUD_DOWN);
    // pullUpDnControl(ADC_MOSI, PUD_DOWN);


    // use pull-down resistors for the  motor driver
    pullUpDnControl(LED, PUD_UP);
    //pullUpDnControl(LED2, PUD_UP);
    // pullUpDnControl(IN3, PUD_DOWN);
    // pullUpDnControl(IN4, PUD_DOWN);

    // configure ISR for button press
    wiringPiISR(BUTTON1, INT_EDGE_FALLING, Button1ISR);
    wiringPiISR(BUTTON2, INT_EDGE_FALLING, Button2ISR);
    wiringPiISR(OFFBUTTON, INT_EDGE_FALLING, OffButtonISR);

    //wiringPiISR(BUTTON3, INT_EDGE_FALLING, Button3ISR);

}



// Turn on an LED
void led_on(int n){
    digitalWrite(n, HIGH);
}
 
// Turn off an LED
void led_off(int n){
    digitalWrite(n, LOW);
}

void blink_led(int n){
    digitalWrite(n, HIGH);
    delay(100);
    digitalWrite(n, LOW);
    delay(100);
    digitalWrite(n, HIGH);
    delay(100);
    digitalWrite(n, LOW);
    delay(100);
    digitalWrite(n, HIGH);
    delay(100);
    digitalWrite(n, LOW);
    
}


/* 
    This method reads the values the adc outputs. In order to read the values, we looked at the data sheet and 
    sample code provided in the Adeept user manual. To understand why we set ADC_CS to 0 and 1, along with every other
    ADC variable, it is best to refer to the ADC0832-N timing diagram provided in the datasheet.
*/
int ADCResult(){   // based upon Adeept lesson 12
 
    uint8_t data1 = 0;
    uint8_t data2 = 0;
    uint8_t i;
 
    // Here we will set up the ADC to read in values.
    digitalWrite(ADC_CS, 0);        // setting chip select to 0

    digitalWrite(ADC_CLK, 0);      // setting slave clock to 0
    digitalWrite(ADC_DIO, 1);        // setting data in/out to 1      
    delayMicroseconds(2);              // wait 2 microseconds (half a clock cycle)


    digitalWrite(ADC_CLK, 1);        // setting slave clock to 1     
    delayMicroseconds(2);             // wait 2 microseconds (half a clock cycle)

    digitalWrite(ADC_CLK, 0);       // setting slave clock to 0 (one full clock cycle completed)
    digitalWrite(ADC_DIO, 1);     // setting data in/out to 1  
    delayMicroseconds(2);             // wait 2 microseconds (half a clock cycle)

    digitalWrite(ADC_CLK, 1);       // setting slave clock to 1  
    delayMicroseconds(2);             // wait 2 microseconds (half a clock cycle)

    digitalWrite(ADC_CLK, 0);       // setting slave clock to 0 (second full clock cycle completed)
    digitalWrite(ADC_DIO, 0);         // setting data in/out to 0
    delayMicroseconds(2);             // wait 2 microseconds (half a clock cycle)

    digitalWrite(ADC_CLK, 1);;       // setting slave clock to 1  
    digitalWrite(ADC_DIO, 1);     // setting data in/out to 1  
    delayMicroseconds(2);             // wait 2 microseconds (half a clock cycle)

    digitalWrite(ADC_CLK, 0);       // setting slave clock to 0 (third full clock cycle completed)
    digitalWrite(ADC_DIO, 1);   // setting data in/out to 1    
    delayMicroseconds(2);             // wait 2 microseconds (half a clock cycle)

    // We have finished setting up the ADC for reading in, now we can actually read from it
    // This for-loop reads in the MSB first data that the ADC outputs.
    for (i=0; i<8; i++){
 
        digitalWrite(ADC_CLK, 1);         // setting slave clock to 1  
        delayMicroseconds(2);             // wait 2 microseconds (half a clock cycle)

        digitalWrite(ADC_CLK, 0);         // setting slave clock to 0
        delayMicroseconds(2);             // wait 2 microseconds (half a clock cycle)

        pinMode(ADC_DIO, INPUT);    // read one bit per clock cycle
        data1=data1<<1 | digitalRead(ADC_DIO);   // shift data left one bit

    }
 
    // This loop reads in the second half of the ADC output, the LSB first data. This data should match
    // the first data read.
    for (i=0; i<8; i++){
 
        data2= data2 |  (uint8_t)(digitalRead(ADC_DIO))<<i;   // shift data left one bit
        digitalWrite(ADC_CLK, 1);         // setting slave clock to 1  
        delayMicroseconds(2);              // wait 2 microseconds (half a clock cycle)

        digitalWrite(ADC_CLK, 0);         // setting slave clock to 0
        delayMicroseconds(2);                         // wait 2 microseconds (half a clock cycle)
                                                            // read one bit per clock cycle

    }
 
    // After we are done reading in from the ADC, we set the chip-select back to 1 until the next time 
    // we need to read from it.
 
    digitalWrite(ADC_CS, 1);
    pinMode(ADC_DIO, OUTPUT);     // setting data in/out back to output, to be ready to trigger the next communication

    
    //  If both data1 and data2 match, it means we successfully read from the ADC. Return
    // the value read.
    if (data1 == data2){
        return(data1);
    }
 
    else{
        return(0);
    }
}