#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Wire.h>      // this is needed even tho we aren't using it
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>
#include <TimedAction.h>

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

// The STMPE610 uses hardware SPI on the shield, and #8
#define STMPE_CS 8
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// Size of the color selection boxes and the paintbrush size
#define BOXSIZE 40
#define TEXTSIZE 2
#define INSIDE_BOX BOXSIZE/3
#define PENRADIUS 3
#define HEIGHT_STEP_SIZE 0.1

#define BLACK    0x0000
#define BLUE     0x001F
#define RED      0xF800
#define GREEN    0x07E0
#define CYAN     0x07FF
#define MAGENTA  0xF81F
#define YELLOW   0xFFE0 
#define WHITE    0xFFFF
#define GREY     tft.color565(64, 64, 64)
#define LAVENDER 0xFF1F


#define SD_CS 4

enum {spare, singlePhase, tower};
int currentShape = tower;

namespace
{
  TS_Point leftButton(0, 0, 0);
  TS_Point rightButton(BOXSIZE*3, 0, 0);
  TS_Point heightBox(BOXSIZE*4, BOXSIZE, 0);
  TS_Point increaseHeight(BOXSIZE*6.5, BOXSIZE, 0);
  TS_Point decreaseHeight(BOXSIZE*6.5, BOXSIZE*2, 0);

  TS_Point goButton(BOXSIZE*4, BOXSIZE*3.5, 0);

  TS_Point calculatedCurrent(3, 3, 0);
  TS_Point measuredMagneticField(3, BOXSIZE*2+3, 0);
  TS_Point distance(3, BOXSIZE*4+3, 0);
  TS_Point cancelButton(BOXSIZE*4, BOXSIZE*4, 0);
  
  double towerHeight = 10.5;

  double calculatedCurrentValue = -1.0;
  double calculatedMagneticFieldValue = -1.0;
  double calculatedDistanceValue = -1.0;

  double calibratedCurrent = -1.0;
}

int SetResetPin = 2;
int xInput = A0;
int yInput = A2;
int zInput = A4;

int data[167];
int i_x = 0;
int i_y = 0;
int i_z = 0;

double magneticFlux = 0.0;

void sendResetSignal()
{
  digitalWrite(SetResetPin, HIGH);
  delay(1);
  digitalWrite(SetResetPin, LOW);
}

int findMax(int* ptr)
{
  int max = *ptr;
  for (int i = 0; i < 167; i++)
  {
    if(max < *ptr)
    {
      max = *ptr;
    }
    ptr++;
  }
  return max;
}

int findMin(int* ptr)
{
  int min = *ptr;
  for (int i = 0; i < 167; i++)
  {
    if(min > *ptr)
    {
      min = *ptr;
    }
    ptr++;
  }
  return min;
}

void findVpp(const int analogPin, int* dataArray, int& i, double& Vpp) // in mV
{
  
  if(micros() % 100 == 0)
  {
    int xValue = analogRead(xInput);
    int yValue = analogRead(yInput);
    int zValue = analogRead(zInput);
    int sumOfsinglePhase = sqrt(pow(xValue, 2) + pow(yValue, 2) + pow(zValue, 2));
    dataArray[(i)] = sumOfsinglePhase;
    if(i == 166)
    {
      int max = findMax(&dataArray[0]);
      int min = findMin(&dataArray[0]);
      Vpp = (max - min) * (5.0/1023.0) * 1000;
      
      /*Serial.print(analogPin);
      Serial.print(" ");
      Serial.print(max);
      Serial.print(", ");
      Serial.print(min);
      Serial.print(", ");
      Serial.print(Vpp);
      Serial.print("     ");*/
    }
    i++;    
    if(i == 167)
    {
      i = 0;
    }
  }
}

TimedAction setReset = TimedAction(1000, sendResetSignal);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(SetResetPin, OUTPUT);

  // Rotation needed otherwise letters print in portrait instead of landscape 
  tft.begin();
  tft.setRotation(1);
  
  if (!ts.begin()) {
    Serial.println("Couldn't start touchscreen controller");
    while (1);
  }
  Serial.println("Touchscreen started");

  setupButtons();
}

void loop() {

    TS_Point p;

  if ( ! ts.touched() ) {
      return;
  }
  // This loop flushes the buffer so that only one point is registered
  while ( ts.touched() ) {
      p = ts.getPoint();
  }

  // Scale from ~0->4000 to tft.width using the calibration #'s
  // Needed for 
  int16_t  y = p.y;
  p.y = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());
  p.x = map(y, TS_MINY, TS_MAXY, 0, tft.width());

  if(isLeftButtonPushed(p)){
    removeButtonOutlines();
    tft.drawRect(leftButton.x, leftButton.y, BOXSIZE, BOXSIZE, RED);
    decreaseShape();
  }
  else if(isRightButtonPushed(p)){
    removeButtonOutlines();
    tft.drawRect(rightButton.x, rightButton.y, BOXSIZE, BOXSIZE, RED);
    increaseShape();
  }
  else if(isIncreaseHeightPushed(p)){
    removeButtonOutlines();
    tft.drawRect(increaseHeight.x, increaseHeight.y, BOXSIZE, BOXSIZE, RED);
    towerHeight += HEIGHT_STEP_SIZE;
    writeHeight(towerHeight);
  }
  else if(isDecreaseHeightPushed(p)){
    removeButtonOutlines();
    tft.drawRect(decreaseHeight.x, decreaseHeight.y, BOXSIZE, BOXSIZE, RED);
    towerHeight -= HEIGHT_STEP_SIZE;
    writeHeight(towerHeight);
  }
  else if(isGoButtonPushed(p))
  {
    displayMode();
  }

}

void displayMode()
{
  delay(2000);
  setupDisplayMode();

  // This loop overrides the original loop() function
  double oldCurrent = calculatedCurrentValue;
  double oldField = calculatedMagneticFieldValue;
  double oldDistance = calculatedDistanceValue;

  findVpp(xInput, data, i_x, calculatedMagneticFieldValue);
  if(currentShape == tower)
  {
    calculatedCurrentValue = calibrate240TypeL(towerHeight, calculatedMagneticFieldValue/10.638);
    writeValue(calculatedCurrentValue*100, calculatedCurrent, "(A)"); 
  }
  else if(currentShape == singlePhase)
  {
    calculatedCurrentValue = calibrateSinglePhaseLine(towerHeight, calculatedMagneticFieldValue/(10.638*1000000));   
  }
  writeValue(towerHeight, distance, "(m)");
  while(1)
  {

    TS_Point p;

    //Since we can't return while the screen is not touched like the main loop() function
    //Work for drawing and calculations should be done in this loop
    while ( ! ts.touched() ) {
      findVpp(xInput, data, i_x, calculatedMagneticFieldValue);
      if(micros() % 16700 == 0)
      {
        Serial.print("magneticFlux ");
        Serial.print(calculatedMagneticFieldValue);
        Serial.println();
      }
      //do current calculation
      if(oldCurrent != calculatedCurrentValue)
      {
        oldCurrent = calculatedCurrentValue;
          if(currentShape == tower)
          {
            writeValue(calculatedCurrentValue*100, calculatedCurrent, "(A)");
          }
          else if(currentShape == singlePhase)
          {
            writeValue(calculatedCurrentValue, calculatedCurrent, "(A)");   
          }
      }

      if(oldField != calculatedMagneticFieldValue)
      {
        oldField = calculatedMagneticFieldValue;
        writeValue(calculatedMagneticFieldValue/10.638, measuredMagneticField, "(uT)");
      }
      
      if(currentShape == tower)
      {
        calculatedDistanceValue = calculateTowerDistance(calculatedMagneticFieldValue/10.638, calculatedCurrentValue); 
      }
      else if(currentShape == singlePhase)
      {
        calculatedDistanceValue = calculateDistance(calculatedMagneticFieldValue/(10.638*1000000), calculatedCurrentValue);   
      }
      
      if(oldDistance != calculatedDistanceValue)
      {
        oldDistance = calculatedDistanceValue;
        writeValue(calculatedDistanceValue, distance, "(m)");
      }
    }
    while ( ts.touched() ) {
        p = ts.getPoint();
    }

    // Scale from ~0->4000 to tft.width using the calibration #'s
    int16_t  y = p.y;
    p.y = map(p.x, TS_MAXX, TS_MINX, 0, tft.height());
    p.x = map(y, TS_MINY, TS_MAXY, 0, tft.width());

    //Go back to loop() if cancel button pushed
    if(isCancelButtonPushed(p))
    {
      setupButtons();
      return;
    }
  }
}

void setupDisplayMode()
{
  tft.fillScreen(WHITE);

  tft.setCursor(calculatedCurrent.x, calculatedCurrent.y);
  tft.print("Calculated Current: ");

  tft.setCursor(measuredMagneticField.x, measuredMagneticField.y);
  tft.print("Measured Magnetic Field: ");

  tft.setCursor(distance.x, distance.y);
  tft.print("Distance: ");

  tft.fillRect(cancelButton.x, cancelButton.y, BOXSIZE*4, BOXSIZE*2, RED);

  tft.setCursor(cancelButton.x+BOXSIZE+5, cancelButton.y+BOXSIZE*0.75);
  tft.print("Cancel");
}

void setupButtons()
{
  tft.fillScreen(WHITE);
  //Picture arrow buttons
  tft.fillRect(leftButton.x, leftButton.y, BOXSIZE, BOXSIZE, LAVENDER);
  tft.fillRect(rightButton.x, rightButton.y, BOXSIZE, BOXSIZE, LAVENDER);

  //setup for writing
  tft.setTextSize(TEXTSIZE);
  tft.setFont();
  tft.setTextColor(BLACK);

  tft.setCursor(leftButton.x+INSIDE_BOX, leftButton.y+INSIDE_BOX);
  tft.print("<");

  tft.setCursor(rightButton.x+INSIDE_BOX, rightButton.y+INSIDE_BOX);
  tft.print(">");

  //make plus/minus buttons
  tft.fillRect(increaseHeight.x, increaseHeight.y, BOXSIZE, BOXSIZE, LAVENDER);
  tft.drawRect(increaseHeight.x, increaseHeight.y, BOXSIZE, BOXSIZE, WHITE);
  tft.fillRect(decreaseHeight.x, decreaseHeight.y, BOXSIZE, BOXSIZE, LAVENDER);
  tft.drawRect(decreaseHeight.x, decreaseHeight.y, BOXSIZE, BOXSIZE, WHITE);

  tft.setCursor(heightBox.x+5, heightBox.y-INSIDE_BOX*1.5);
  tft.setTextColor(BLACK);
  tft.print("Line Height");
  tft.setTextColor(BLACK);

  tft.setCursor(increaseHeight.x+INSIDE_BOX, increaseHeight.y+INSIDE_BOX);
  tft.print("+");

  tft.setCursor(decreaseHeight.x+INSIDE_BOX, decreaseHeight.y+INSIDE_BOX);
  tft.print("-");

  writeHeight(towerHeight);

  tft.fillRect(goButton.x, goButton.y, BOXSIZE*3.5, BOXSIZE*2, LAVENDER);

  tft.setTextColor(BLACK);
  tft.setCursor(goButton.x+INSIDE_BOX*1.25, goButton.y+BOXSIZE*0.75);
  
  tft.print("Calibrate");

  draw(currentShape);
 }

void writeHeight(double height)
{
  tft.fillRect(heightBox.x, heightBox.y, BOXSIZE*2.5, BOXSIZE*2, LAVENDER);
  tft.setTextColor(BLACK);
  tft.setCursor(heightBox.x+INSIDE_BOX, heightBox.y+BOXSIZE*0.75);
  
  tft.println(height, 1);

  tft.setCursor(heightBox.x+BOXSIZE*1.65, heightBox.y+BOXSIZE*0.75);
  tft.print("(m)");
}

void writeValue(double value, TS_Point location, String unit)
{
  tft.fillRect(location.x+BOXSIZE, location.y+BOXSIZE*0.75, BOXSIZE*2, BOXSIZE, WHITE);
  tft.setTextColor(BLACK);
  tft.setCursor(location.x+BOXSIZE, location.y+BOXSIZE*0.75);
  
  tft.println(value, 1);

  tft.setCursor(location.x+BOXSIZE*3, location.y+BOXSIZE*0.75);
  tft.print(unit);
}

void removeButtonOutlines()
{
  tft.drawRect(rightButton.x, rightButton.y, BOXSIZE, BOXSIZE, GREY);
  tft.drawRect(leftButton.x, leftButton.y, BOXSIZE, BOXSIZE, GREY);
  tft.drawRect(increaseHeight.x, increaseHeight.y, BOXSIZE, BOXSIZE, WHITE);
  tft.drawRect(decreaseHeight.x, decreaseHeight.y, BOXSIZE, BOXSIZE, WHITE);
}


void increaseShape()
{
  if(currentShape == tower)
  {
    currentShape = 0;
  }
  else
  {
    currentShape++;
  }
  draw(currentShape);
}

void decreaseShape()
{
  if(currentShape == spare)
  {
    currentShape = tower;
  }
  else
  {
    currentShape--;
  }
  draw(currentShape);
}

void draw(int shape)
{
  TS_Point drawPoint(40, 120, 0);
  tft.fillRect(0, BOXSIZE, BOXSIZE*4, BOXSIZE*5, WHITE);
  if(shape == spare)
  {
    //tft.fillspare(drawPoint.x+20, drawPoint.y, 60, RED);
    tft.setCursor(drawPoint.x-BOXSIZE, drawPoint.y-BOXSIZE);
    tft.print("Spare");
    tft.setCursor(drawPoint.x-BOXSIZE, drawPoint.y);
    tft.print("For future");
    tft.setCursor(drawPoint.x-BOXSIZE, drawPoint.y+BOXSIZE);
    tft.print("Expansion");
  }
  else if(shape == singlePhase)
  {
    //tft.fillRect(drawPoint.x, drawPoint.y, BOXSIZE/2, BOXSIZE*3, BLACK);
    
  tft.setCursor(drawPoint.x-BOXSIZE, drawPoint.y-BOXSIZE);
  tft.print("Single Phase");
  }
  else if(shape == tower)
  {
    tft.setCursor(drawPoint.x-BOXSIZE, drawPoint.y-BOXSIZE);
    tft.print("Three Phase");
    tft.setCursor(drawPoint.x-BOXSIZE, drawPoint.y);
    tft.print("240V type L");
    //bmpDraw("Tower.bmp", 15, BOXSIZE);
  }
}

bool isLeftButtonPushed(const TS_Point& pointPushed)
{
  return ((pointPushed.x < leftButton.x+BOXSIZE) &&
    (pointPushed.x > leftButton.x) &&
    (pointPushed.y < leftButton.y+BOXSIZE) && 
    (pointPushed.y > leftButton.y));
}

bool isRightButtonPushed(const TS_Point& pointPushed)
{
  return ((pointPushed.x < rightButton.x+BOXSIZE) &&
    (pointPushed.x > rightButton.x) &&
    (pointPushed.y < rightButton.y+BOXSIZE) && 
    (pointPushed.y > rightButton.y));
}

bool isIncreaseHeightPushed(const TS_Point& pointPushed)
{
  return ((pointPushed.x < increaseHeight.x+BOXSIZE) &&
    (pointPushed.x > increaseHeight.x) &&
    (pointPushed.y < increaseHeight.y+BOXSIZE) && 
    (pointPushed.y > increaseHeight.y));
}

bool isDecreaseHeightPushed(const TS_Point& pointPushed)
{
  return ((pointPushed.x < decreaseHeight.x+BOXSIZE) &&
    (pointPushed.x > decreaseHeight.x) &&
    (pointPushed.y < decreaseHeight.y+BOXSIZE) && 
    (pointPushed.y > decreaseHeight.y));
}

bool isGoButtonPushed(const TS_Point& pointPushed)
{
  return ((pointPushed.x < goButton.x+BOXSIZE*3.5) &&
    (pointPushed.x > goButton.x) &&
    (pointPushed.y < goButton.y+BOXSIZE*2) && 
    (pointPushed.y > goButton.y));
}

bool isCancelButtonPushed(const TS_Point& pointPushed)
{
  return ((pointPushed.x < cancelButton.x+BOXSIZE*4) &&
    (pointPushed.x > cancelButton.x) &&
    (pointPushed.y < cancelButton.y+BOXSIZE*4) && 
    (pointPushed.y > cancelButton.y));
}

#define PI 3.14159
//distanceInM and Magnetic Flux, returns current
double calibrateSinglePhaseLine(double distanceFromLine, double magneticFlux)
{
  return PI*2*distanceFromLine*magneticFlux/(4.0*PI*pow(10,-7));
}

double calculateDistance(double magneticFlux, double current)
{
  return 4.0*PI*pow(10,-7)*current/(2*PI*magneticFlux);
}

//distance in meters and flux in microteslas
//this function returns the ratio between the current flowing through the
// transmission line being measured and 100A.
double calibrate240TypeL(double distanceFromLine, double magneticFlux)
{
  if(distanceFromLine == 20.5)
     return magneticFlux / 0.21506;
  else if(distanceFromLine ==  20.0)
     return magneticFlux / 0.22780; 
  else if(distanceFromLine == 19.5)
     return magneticFlux / 0.24157;
  else if(distanceFromLine == 19.0)
     return magneticFlux / 0.25647;
  else if(distanceFromLine == 18.5)
     return magneticFlux / 0.27263;
  else if(distanceFromLine == 18.0)
     return magneticFlux / 0.29018;
  else if(distanceFromLine == 17.5)
     return magneticFlux / 0.30928;
  else if(distanceFromLine == 17.0)
     return magneticFlux / 0.33009;
  else if(distanceFromLine == 16.5)
     return magneticFlux / 0.35282;
  else if(distanceFromLine == 16.0)
     return magneticFlux / 0.37769;
  else if(distanceFromLine == 15.5)
     return magneticFlux / 0.40496;
  else if(distanceFromLine == 15.0)
     return magneticFlux / 0.43492;
  else if(distanceFromLine == 14.5)
     return magneticFlux / 0.46792;
  else if(distanceFromLine == 14.0)
     return magneticFlux / 0.50434;
  else if(distanceFromLine == 13.5)
     return magneticFlux / 0.54465;
  else if(distanceFromLine == 13.0)
     return magneticFlux / 0.58937;
  else if(distanceFromLine == 12.5)
     return magneticFlux / 0.63912;
  else if(distanceFromLine == 12.0)
     return magneticFlux / 0.69461;
  else if(distanceFromLine == 11.5)
     return magneticFlux / 0.75670;
  else if(distanceFromLine == 11.0)
     return magneticFlux / 0.82638;
  else if(distanceFromLine == 10.5)
     return magneticFlux / 0.90482;
  else if(distanceFromLine == 10.0)
     return magneticFlux / 0.99341;
  else if(distanceFromLine == 9.5)
     return magneticFlux / 1.0938;
  else if(distanceFromLine == 9.0)
     return magneticFlux / 1.2080;
  else if(distanceFromLine == 8.5)
     return magneticFlux / 1.3382;
  else
    return 0.0;
}

//function calculates distance based on a trendline for 240Kv Type L data.
// The Magnetic flux and current Ration from calubration are required, the 
// distance from the lowest line is returned
double calculateTowerDistance(double magneticFlux, double currentRatio)
{
  double ICR = 1/currentRatio; //inverse current ratio
  double distance = -1.0346*pow(ICR * magneticFlux, 5) 
    + 9.3444*pow(ICR * magneticFlux, 4)
    - 32.453*pow(ICR * magneticFlux, 3)
    + 55.068*pow(ICR * magneticFlux, 2)
    - 49.543*ICR*magneticFlux
    + 28.572;
  return distance;
}

