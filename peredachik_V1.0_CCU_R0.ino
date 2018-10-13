#include <SPI.h>                                          // Подключаем библиотеку для работы с шиной SPI
#include <nRF24L01.h>                                     // Подключаем файл настроек из библиотеки RF24
#include <RF24.h>  

#define CE_PIN   (9)
#define CSN_PIN  (10)

#define JOYSTICK1_X  (A0)
#define JOYSTICK1_Y  (A1)
#define JOYSTICK1_BUTTON (4) // D4

#define JOYSTICK2_X  (A2)
#define JOYSTICK2_Y  (A3)
#define JOYSTICK2_BUTTON (3)  //D3

#define CONTAINER1 (A4) //Потенциометр
#define CONTAINER2 (A5) //Потенциометр

RF24           radio(CE_PIN, CSN_PIN);                     // Создаём объект radio для работы с библиотекой RF24, указывая номера выводов nRF24L01+ (CE, CSN)
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
int ReverseCurrent = 0;
bool flagReverseBtn = false;
bool flagLedBtn = false;


void setup(){

    delay(1000);
    //Serial.begin(115200);
    
    radio.begin();                                        // Инициируем работу nRF24L01+
    radio.setChannel(5);                                  // Указываем канал передачи данных (от 0 до 127), 5 - значит передача данных осуществляется на частоте 2,405 ГГц (на одном канале может быть только 1 приёмник и до 6 передатчиков)
    radio.setDataRate     (RF24_250KBPS);                   // Указываем скорость передачи данных (RF24_250KBPS, RF24_1MBPS, RF24_2MBPS), RF24_1MBPS - 1Мбит/сек
    radio.setPALevel      (RF24_PA_LOW);                 // Указываем мощность передатчика (RF24_PA_MIN=-18dBm, RF24_PA_LOW=-12dBm, RF24_PA_HIGH=-6dBm, RF24_PA_MAX=0dBm)
    radio.openWritingPipe (0x1234567890LL);               // Открываем трубу с идентификатором 0x1234567890 для передачи данных (на одном канале может быть открыто до 6 разных труб, которые должны отличаться только последним байтом идентификатора)
}

void loop(){
    MSG.JOY1_Y = analogRead(JOYSTICK1_Y);
    MSG.JOY2_X = analogRead(JOYSTICK2_X);
    if( LOW == digitalRead(JOYSTICK2_BUTTON) ){   //если нажали кнопку заднего хода
      if( false == flagReverseBtn){
          ReverseCurrent = millis();
          flagReverseBtn = true;
      }
      if(millis() - ReverseCurrent > 200){ //и удерживаем её в течении 200 мс
          MSG.ReverseState = true;       //переключаем задний ход
              //Serial.print("JOY2_BTN_STATE: ");
    //Serial.println( digitalRead(JOYSTICK2_BUTTON) );
      }
    }
    else      //иначе
    {
        MSG.ReverseState = false;       //оставляем ход вперёд
        flagReverseBtn = false;
        ReverseCurrent = 0;             //обнуляем счётчик (для дребезга)
    }
    
    if( LOW == digitalRead(JOYSTICK1_BUTTON) )   //если нажали кнопку включения света
    {
        if( false == flagLedBtn){     //выставляем флаг на отпускание кнопки
           flagLedBtn = true;
        }
    }
    else      //иначе кнопку отпустили
    {
        if( true == flagLedBtn ){
            if(  true == MSG.LedState )     //если свет включен
                MSG.LedState = false;        //выключаем свет
            else
                MSG.LedState = true;         //иначе, включаем свет
            flagLedBtn = false;
        }
    }
      
    MSG.Container1 = analogRead(JOYSTICK1_X); //analogRead(CONTAINER1);
    MSG.Container2 = analogRead(JOYSTICK2_Y); //analogRead(CONTAINER2);
    radio.write(&MSG, sizeof(MSG));                     // отправляем данные из структуры MSG указывая сколько байт мы хотим отправить. Отправить данные можно с проверкой их доставки: if( radio.write(&data, sizeof(data)) ){данные приняты приёмником;}else{данные не приняты приёмником;}
}

