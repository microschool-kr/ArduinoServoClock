#include <virtuabotixRTC.h>               //RTC모듈 라이브러
#include <Adafruit_PWMServoDriver.h>      //서보모터 드라이버(PCA모듈) 드라이버
Adafruit_PWMServoDriver pwmH = Adafruit_PWMServoDriver(0x40);    //시간 부분 (SBEC와 연결된 서보드라이버)버
Adafruit_PWMServoDriver pwmM = Adafruit_PWMServoDriver(0x41);    //분 부분 (A0 납땜한 서보드라이)

int servoFrequency = 50;      //서보 주파수 설정
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 조율 필요 
int segmentHOn[14] = {270,270,200,290,290,170,240,300,300,150,280,282,200,250};   //시간 부분의 서보모터가 켜졌을 때(8모양으로 표시될 때) 서보의 각도 
int segmentMOn[14] = {300,250,250,280,320,270,270,270,280,280,260,300,260,305};   //분 부분의 서보모터가 켜졌을 때(8모양으로 표시될 때) 서보의 각도
int segmentHOff[14] = {100,100,400,400,100,350,100,100,100,380,430,100,400,100};    //시간 부분의 서보모터가 꺼졌을 때(검은색만 보일때 ) 서보의 각도
int segmentMOff[14] = {100,100,390,460,100,400,100,100,100,420,450,100,450,100};    //분 부분의 서보모터가 꺼졌을 때(검은색만 보일때) 서보의 각도
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int digits[10][7] = {{1,1,1,1,1,1,0},{0,1,1,0,0,0,0},{1,1,0,1,1,0,1},{1,1,1,1,0,0,1},{0,1,1,0,0,1,1},
                     {1,0,1,1,0,1,1},{1,0,1,1,1,1,1},{1,1,1,0,0,0,0},{1,1,1,1,1,1,1},{1,1,1,1,0,1,1}};    //각 숫자를 표현하기 위한 서보모터의 위치별 동작여부 

virtuabotixRTC myRTC(6, 7, 8);    //RTC모듈의 핀 설정 6, 7, 8 - CLK, DAT, RST
int hourTens = 0;                 //시간과 분을 10의자리, 1의자리로 나눌 변수 생성
int hourUnits = 0;
int minuteTens = 0;
int minuteUnits = 0;

int prevHourTens = 8;           //가운데 서보모터가 움직일때 비켜주는 서보모터 변수 생성
int prevHourUnits = 8;          
int prevMinuteTens = 8;
int prevMinuteUnits = 8;

int midOffset = 100;            //가운데 서보모터와 인접한 서보모터가 움직이는 양 

void setup() 
{
  Serial.begin(57600);
  pwmH.begin();                             //보드 시작
  pwmM.begin();
  pwmH.setOscillatorFrequency(27000000);    //서보모터 주파수 설정 
  pwmM.setOscillatorFrequency(27000000);
  pwmH.setPWMFreq(servoFrequency);          
  pwmM.setPWMFreq(servoFrequency);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  myRTC.setDS1302Time(00, 39, 1, 5, 10, 3, 2022);        //시간 설정 (초,분,시,요일,일,월,년) 시간 설정후 주석처리, 한 번 더 업로드 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  for(int i=0 ; i<=13 ; i++)    //모든 서보가 켜진 상태 (88:88표시)
  {
    pwmH.setPWM(i, 0, segmentHOn[i]);
    delay(10);
    pwmM.setPWM(i, 0, segmentMOn[i]);
    delay(10);
  }
  delay(2000);
}

void loop()
{
 
  myRTC.updateTime();                 //시간 업데이트
  int temp = myRTC.hours;             //시간값 저장 
  Serial.print("hours");
  Serial.println(temp);
  hourTens = temp / 10;               //시간을 10의 자리, 1의 자리로 분리 후 저장 (만약 10의자리와 1의자리가 바뀌어서 나온다면 수정)
  hourUnits = temp % 10;
  temp = myRTC.minutes;               //분값 저장 
  Serial.print("minutes");
  Serial.println(temp);
  minuteTens = temp / 10;             //시간을 10의 자리, 1의 자리로 분리 후 저장 (만약 10의자리와 1의자리가 바뀌어서 나온다면 수정)
  minuteUnits = temp % 10;

  if(minuteUnits != prevMinuteUnits)//만약 저장된 분과 새로 저장한 분이 다르면 디스플레이 업데이트 
     updateDisplay();
  
   

  prevHourTens = hourTens;            
  prevHourUnits = hourUnits;
  prevMinuteTens = minuteTens;
  prevMinuteUnits = minuteUnits;

  delay(500);
  
}

void updateDisplay ()                               //디스플레이 업데이트 함수 
{
  
  updateMid();                                      //가운데 서보를 제외한 가장자리의 서보 먼저 동작후, 마지막에 중간 서보 동작 
  for (int i=0 ; i<=5 ; i++)                       
  {
    if(digits[hourTens][i]==1)                      //시간의 10의자리 업데이트 
      pwmH.setPWM(i+7, 0, segmentHOn[i+7]);
    else
      pwmH.setPWM(i+7, 0, segmentHOff[i+7]);
    delay(10);
    if(digits[hourUnits][i]==1)                     //시간의 1의자리 업데이트 
      pwmH.setPWM(i, 0, segmentHOn[i]);
    else
      pwmH.setPWM(i, 0, segmentHOff[i]);
    delay(10);
    if(digits[minuteTens][i]==1)                    //분의 10의자리 업데이트 
      pwmM.setPWM(i+7, 0, segmentMOn[i+7]);
    else
      pwmM.setPWM(i+7, 0, segmentMOff[i+7]);
    delay(10);
    if(digits[minuteUnits][i]==1)                   //분의 1의자리 업데이트 
      pwmM.setPWM(i, 0, segmentMOn[i]);
    else
      pwmM.setPWM(i, 0, segmentMOff[i]);
    delay(10);
  }
}

void updateMid()                                              //가운데 서보를 움직이고 인접한 서보를 움직이는 함수
{
  if(digits[minuteUnits][6]!=digits[prevMinuteUnits][6])      //분 부분에서 1의자리 인접한 서보 움직임 
  {
    if(digits[prevMinuteUnits][1]==1)
      pwmM.setPWM(1, 0, segmentMOn[1]-midOffset);
    if(digits[prevMinuteUnits][6]==1)
      pwmM.setPWM(5, 0, segmentMOn[5]+midOffset);
  }
  delay(100);                                                 //중간 서보가 움직인 후 인접한 서보 움직임 (딜레이) 
  if(digits[minuteUnits][6]==1)                               //만약 필요하다면 분 1의자리 부분의 중간 서보 움직임 
    pwmM.setPWM(6, 0, segmentMOn[6]);
  else
    pwmM.setPWM(6, 0, segmentMOff[6]);
  if(digits[minuteTens][6]!=digits[prevMinuteTens][6])        //분 부분의 10의자리 인접한 서보 움직임 
  {
    if(digits[prevMinuteTens][1]==1)
      pwmM.setPWM(8, 0, segmentMOn[8]-midOffset);
    if(digits[prevMinuteTens][6]==1)
      pwmM.setPWM(12, 0, segmentMOn[12]+midOffset);
  }
  delay(100);                                                 //중간 서보가 움직인 후 인접한 서보 움직임 (딜레이)  
  if(digits[minuteTens][6]==1)                                //만약 필요하다면 분 10의자리 부분의 중간 서보 움직임
    pwmM.setPWM(13, 0, segmentMOn[13]);
  else
    pwmM.setPWM(13, 0, segmentMOff[13]);
  if(digits[hourUnits][6]!=digits[prevHourUnits][6])          //시간 부분 1의자리 인접한 서보 움직임
  {
    if(digits[prevHourUnits][1]==1)
      pwmH.setPWM(1, 0, segmentHOn[1]-midOffset);
    if(digits[prevHourUnits][6]==1)
      pwmH.setPWM(5, 0, segmentHOn[5]+midOffset);
  }
  delay(100);                                                 //중간 서보가 움직인 후 인접한 서보 움직임 (딜레이)  
  if(digits[hourUnits][6]==1)                                 //Move Hour units middle segment if required
    pwmH.setPWM(6, 0, segmentHOn[6]);
  else
    pwmH.setPWM(6, 0, segmentHOff[6]);
  if(digits[hourTens][6]!=digits[prevHourTens][6])            //시간 부분 10의자리 인접한 서보 움직임
  {
    if(digits[prevHourTens][1]==1)
      pwmH.setPWM(8, 0, segmentHOn[8]-midOffset);
    if(digits[prevHourTens][6]==1)
      pwmH.setPWM(12, 0, segmentHOn[12]+midOffset);
  }
  delay(100);                                                 //중간 서보가 움직인 후 인접한 서보 움직임 (딜레이) 
  if(digits[hourTens][6]==1)                                  //필요시 시간 부분 10의자리 서보 움직임 
    pwmH.setPWM(13, 0, segmentHOn[13]);
  else
    pwmH.setPWM(13, 0, segmentHOff[13]);
    
    
    
}
