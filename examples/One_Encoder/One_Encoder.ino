// ВЫВОДИМ НАПРАВЛЕНИЕ ОДНОГО ТАКТА ПОВОРОТА: //
// Поворот по     чс: Выводим '<'.            //
// Поворот против чс: Выводим '>'.            //  Если перепутать выводы A и B, то выводимые символы поменяются местами.
// Нажатие на кнопку: Включаем светодиод.     //  Кнопка сама включает светодиод, т.к. подключена к 13 выводу.
                                              //
uint8_t pinA = 11;                            //  Вывод Arduino подключённый к выводу A энкодера (можно использовать любые выводы).
uint8_t pinB = 12;                            //  Вывод Arduino подключённый к выводу B энкодера (можно использовать любые выводы).
uint8_t pinK = 13;                            //  Вывод Arduino подключённый к выводу K энкодера (кнопка).
                                              //
#include <iarduino_Encoder_tmr.h>             //  Подключаем библиотеку iarduino_Encoder_tmr для работы с энкодерами через аппаратный таймер.
iarduino_Encoder_tmr enc(pinA,pinB);          //  Объявляем объект enc для работы с энкодером указывая (№ вывода A, № вывода B).
                                              //  При использовании библиотеки iarduino_Encoder_tmr можно подключить до 8 энкодеров.
void setup(){                                 //
     Serial.begin(9600);                      //  Инициируем передачу данных в монитор последовательного порта на скорости 9600 бит/сек.
     enc.begin();                             //  Инициируем работу с энкодером.
}                                             //
                                              //
void loop(){                                  //
     int a=enc.read();                        //  Читаем состояние энкодера, функция возвращает: 0 нет поворота, encLEFT = -1, или encRIGHT = +1.
     if( a ){                                 //  Если энкодер зафиксировал поворот, то ...
        if(a==encLEFT ){Serial.println("<");} //  Если энкодер зафиксировал поворот влево,  выводим символ '<'.
        if(a==encRIGHT){Serial.println(">");} //  Если энкодер зафиксировал поворот вправо, выводим символ '>'.
     }                                        //
}                                             //