#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define CE_PIN   (2)
#define CSN_PIN  (9)
#define RELE_OUT (7)  //D7
#define MOTOR    (10) //D10
#define LED (4)   //D4

#define DEBUG   //раскоментировать для вывода отладочной информации по последовательному порту

Servo myservo1;
Servo myservo2;
Servo myservo3;
Servo motor;

RF24 radio(CE_PIN, CSN_PIN); 

struct ReceiverMessage                                     // Создаём структуру данных
{
    uint16_t JOY1_Y;    //Вперёд-назад
    uint16_t JOY2_X;    //Вправо-влево
    uint16_t Container1;//Положение контейнера 1
    uint16_t Container2;//Положение контейнера 2
    bool ReverseState;  //Пин управления задним ходом
    bool LedState;      //Пин управления освещенем
};

struct ReceiverMessage MSG = {0};

void servo_test(void)
{
  //servo test!
      for(int i = 0; i < 180; i+=5){
        myservo1.write(i);
        delay(100);
    }

    delay(500);

    for(int i = 0; i < 180; i+=5){
        myservo2.write(i);
        delay(100);
    }
    
    delay(500);
    
    for(int i = 0; i < 180; i+=5){
        myservo3.write(i);
        delay(100);
    }

    delay(500);
}

void setup(){
  
#ifdef DEBUG
    Serial.begin(115200);
#endif
    delay(1000);

    radio.begin();                                        // Инициируем работу nRF24L01+
    radio.setChannel(5);                                  // Указываем канал приёма данных (от 0 до 127), 5 - значит приём данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
    radio.setDataRate     (RF24_250KBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
    radio.setPALevel      (RF24_PA_LOW);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openReadingPipe (1, 0x1234567890LL);            // Открываем 1 трубу с идентификатором 0x1234567890 для приема данных (на ожном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
    radio.startListening  ();                             // Включаем приемник, начинаем прослушивать открытую трубу
//  radio.stopListening   ();                             // Выключаем приёмник, если потребуется передать данные
  
    myservo1.attach(3);
    myservo2.attach(5); 
    myservo3.attach(6);
    motor.attach(MOTOR);
    
    pinMode(RELE_OUT,OUTPUT);
    pinMode(LED, OUTPUT);
    
    motor.writeMicroseconds(2300);
    delay(2000);
    motor.writeMicroseconds(800);
    delay(6000);
}


void loop(){
    if(radio.available()){                                // Если в буфере имеются принятые данные
        radio.read(&MSG, sizeof(MSG));                   // Читаем данные в массив data и указываем сколько байт читать
        
#ifdef DEBUG        
        Serial.print("JOY1_Y: "); Serial.println(MSG.JOY1_Y);
        Serial.print("JOY2_X: "); Serial.println(MSG.JOY2_X);
        Serial.print("Container1: "); Serial.println(MSG.Container1);
        Serial.print("Container2: "); Serial.println(MSG.Container2);
        Serial.print("ReverseState: "); Serial.println(MSG.ReverseState);
        Serial.print("LedState: "); Serial.println(MSG.LedState);
#endif

        if( MSG.ReverseState != digitalRead(RELE_OUT) )   //если задняя скорость ещё не включена
        {
           if(  true == MSG.ReverseState ){ //если включаем задний ход       
              digitalWrite(RELE_OUT, HIGH); //подаём высокий уровень на реле
              delay(100);                   //ожидаем смены фазы
           }
           else
             digitalWrite(RELE_OUT, LOW); //подаём низкий уровень на реле
        }
        
//////////////////////////////////// блок управления моторчиком ////////////////////////////////////

        if( MSG.JOY1_Y > 550 )          //если жмём вперёд
            motor.writeMicroseconds(map(MSG.JOY1_Y, 550, 1023, 800, 2300) );  //плавный пуск
        else if( MSG.JOY1_Y > 500 && MSG.JOY1_Y < 550)    //если ничего не жмём - состояние покоя !!!
            motor.writeMicroseconds(800);       //останавливаем мотор
        else if( MSG.JOY1_Y < 500 && MSG.ReverseState == true && digitalRead(RELE_OUT) == HIGH)   //если жмём назад и выставлен флаг заднего хода и реле переключено
            motor.writeMicroseconds(map(MSG.JOY1_Y, 0, 500, 800, 2300) );   //плавный пуск
            
//////////////////////////////////// блок управления моторчиком ////////////////////////////////////

//////////////////////////////////// блок управления рулём ////////////////////////////////////

        if( MSG.JOY2_X < 300 )    //если джойстик 2 нажали влево
            myservo1.write( map(MSG.JOY2_X, 0, 300, 0, 90) );         // Поворачиваем сервопривод на угол заданный с джойстика
        else if( MSG.JOY2_X > 300 && MSG.JOY2_X < 700 )    // если джойстик 2 в среднем положении (не трогаем)
            myservo1.write(90);        //ставим чётко 90 градусов (положение по середине)
        else if( MSG.JOY2_X > 300 )    //если джойстик 2 нажали вправо
            myservo1.write( map(MSG.JOY2_X, 700, 1023, 90, 180) );    // Поворачиваем сервопривод на угол заданный с джойстика

//////////////////////////////////// блок управления рулём ////////////////////////////////////

//////////////////////////////////// блок управления светом ////////////////////////////////////

        //if( MSG.LedState != digitalRead(LED) )
          digitalWrite(LED, MSG.LedState);  //if LedState == true -> HIGH else LOW

//////////////////////////////////// блок управления светом ////////////////////////////////////

//////////////////////////////////// блок управления контейнерами ////////////////////////////////////

        myservo2.write( map(MSG.Container1, 0, 1023, 0, 180) );    //устанавливаем значение сервы 2 для контейнера 1
        myservo3.write( map(MSG.Container2, 0, 1023, 0, 180) );    //устанавливаем значение сервы 3 для контейнера 2
        
//////////////////////////////////// блок управления контейнерами ////////////////////////////////////

        
    }//if(radio.available())
}


  
