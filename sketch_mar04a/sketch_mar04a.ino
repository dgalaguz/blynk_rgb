//#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "203ed0f06e9244e1ab1b87233c062af3";
char ssid[] = "ASUS";
char pass[] = "asuswifinet";

int rPin = 5;
int gPin = 4;
int bPin = 14;

int Mode = 0;
int ON = 0;

int rZebraColor;
int gZebraColor;
int bZebraColor;

int rMaxVal = 1023;
int gMaxVal = 305;
int bMaxVal = 215;

float defaultGamma = 1.9;

int whiteTemp = 3000;
int maxWhiteTemp = 5000;
int minWhiteTemp = 2000;
float whiteBrightness = 1.0;
float whiteGamma = 2.8;
int wrMaxVal = 1023;
int wgMaxVal = 1023;
int wbMaxVal = 1023;

SimpleTimer timer;
bool firstLoop = true;

int whiteColors[][3] = 
{
  {255,141,11},
  //2000 K
  {255,146,29},
  //2100 K
  {255,152,41},
  //2200 K
  {255,157,51},
  //2300 K
  {255,162,60},
  //2400 K
  {255,166,69},
  //2500 K
  {255,170,77},
  //2600 K
  {255,174,84},
  //2700 K
  {255,178,91},
  //2800 K
  {255,182,98},
  //2900 K
  {255,185,105},
  //3000 K
  {255,189,111},
  //3100 K
  {255,192,118},
  //3200 K
  {255,195,124},
  //3300 K
  {255,198,130},
  //3400 K
  {255,201,135},
  //3500 K
  {255,203,141},
  //3600 K
  {255,206,146},
  //3700 K
  {255,208,151},
  //3800 K
  {255,211,156},
  //3900 K
  {255,213,161},
  //4000 K
  {255,215,166},
  //4100 K
  {255,217,171},
  //4200 K
  {255,219,175},
  //4300 K
  {255,221,180},
  //4400 K
  {255,223,184},
  //4500 K
  {255,225,188},
  //4600 K
  {255,226,192},
  //4700 K
  {255,228,196},
  //4800 K
  {255,229,200},
  //4900 K
  {255,231,204},
  //5000 K
};

void setRawColor(int r, int g, int b){
  analogWrite(rPin, r);
  analogWrite(gPin, g);
  analogWrite(bPin, b);
}

void setGammaCorrectedColor(int r, int g, int b){
  setRawColor(
    getGammaCorrectedValue(r, defaultGamma, 1023, rMaxVal),
    getGammaCorrectedValue(g, defaultGamma, 1023, gMaxVal),
    getGammaCorrectedValue(b, defaultGamma, 1023, bMaxVal));
}

void setWhiteColor(){
  int colorArrSize = sizeof(whiteColors)/sizeof(whiteColors[0]);
  float colorStep = (maxWhiteTemp - minWhiteTemp) / (float)(colorArrSize - 1);
  int baseColorIndex = (whiteTemp - minWhiteTemp)/colorStep;
  float coeficient = (whiteTemp - minWhiteTemp - (colorStep * baseColorIndex))/colorStep;
  
  int r = getMiddleColor(((whiteColors[baseColorIndex][0] + 1) * 4) - 1, ((whiteColors[baseColorIndex+1][0] + 1) * 4) - 1, coeficient) * whiteBrightness;
  int g = getMiddleColor(((whiteColors[baseColorIndex][1] + 1) * 4) - 1, ((whiteColors[baseColorIndex+1][1] + 1) * 4) - 1, coeficient) * whiteBrightness;
  int b = getMiddleColor(((whiteColors[baseColorIndex][2] + 1) * 4) - 1, ((whiteColors[baseColorIndex+1][2] + 1) * 4) - 1, coeficient) * whiteBrightness;

  r = getGammaCorrectedValue(r, whiteGamma, 1023, wrMaxVal);
  g = getGammaCorrectedValue(g, whiteGamma, 1023, wgMaxVal);
  b = getGammaCorrectedValue(b, whiteGamma, 1023, wbMaxVal);

  setRawColor(r,g,b);

}

int getGammaCorrectedValue(int i, float gamma, int max_in, int max_out){
  return (int)(pow((float)i / (float)max_in, gamma) * max_out + 0.5);
}

int getMiddleColor(int firstColor, int secondColor, float coeficient){
  if(firstColor == secondColor){
    return firstColor;
  }

  int diff;
  if (firstColor > secondColor){
    diff = firstColor - secondColor;
    return firstColor - diff * (1.0 - coeficient);
  }
  else
  {
    diff = secondColor - firstColor;
    return firstColor + diff * coeficient;
  }
}

BLYNK_WRITE(V7)
{
  Mode = param.asInt();
  Serial.println("Mode selector value updated");
}

BLYNK_WRITE(V0)
{
  ON = param.asInt();
  Serial.println("On/Off toggle value updated");
}

BLYNK_WRITE(V1)
{
  rZebraColor = param[0].asInt();
  gZebraColor = param[1].asInt();
  bZebraColor = param[2].asInt();

  if(Mode != 1){
    Blynk.virtualWrite(V7, 1);
    Mode = 1;
  }
  
  Serial.println("RGB zebra value updated");
}

BLYNK_WRITE(V10)
{
  rMaxVal = param.asInt();
  Serial.println("rMaxVal value updated");
}

BLYNK_WRITE(V11)
{
  gMaxVal = param.asInt();
  Serial.println("gMaxVal value updated");
}

BLYNK_WRITE(V12)
{
  bMaxVal = param.asInt();
  Serial.println("bMaxVal value updated");
}

BLYNK_WRITE(V13)
{
  defaultGamma = param.asFloat();
  Serial.println("bMaxVal value updated");
}

BLYNK_WRITE(V2)
{
  whiteTemp = param.asInt();

  if(Mode != 2){
    Blynk.virtualWrite(V7, 2);
    Mode = 2;
  }
  
  Serial.println("whiteTemp value updated");
}

BLYNK_WRITE(V3)
{
  whiteBrightness = param.asInt()/1000.0;

  if(Mode != 2){
    Blynk.virtualWrite(V7, 2);
  Mode = 2;
  }
  
  Serial.println("whiteBrightness value updated");
}

BLYNK_WRITE(V14)
{
  wrMaxVal = param.asInt();
  Serial.println("rMaxVal value updated");
}

BLYNK_WRITE(V15)
{
  wgMaxVal = param.asInt();
  Serial.println("gMaxVal value updated");
}

BLYNK_WRITE(V16)
{
  wbMaxVal = param.asInt();
  Serial.println("bMaxVal value updated");
}

BLYNK_WRITE(V17)
{
  whiteGamma = param.asFloat();
  Serial.println("bMaxVal value updated");
}

void setup()
{
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);

  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  
  while(!Blynk.connected()){
    delay(100);
  }

  Blynk.run();

  while(Mode == 0){
  Blynk.syncVirtual(V7);
  delay(100);
  }

  int currentMode = Mode;
  Blynk.syncAll();
  Blynk.virtualWrite(V7, Mode);
  
  Serial.println("Connected and synchronized");
}

void loop()
{
  
  Blynk.run();

  
  if(!ON){
    setRawColor(0,0,0);
    return;
  }

  switch(Mode){
    case 1:
      setGammaCorrectedColor(rZebraColor, gZebraColor, bZebraColor);
      break;
    case 2:
      setWhiteColor();
      break;
  }
 
}
