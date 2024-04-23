//	Библиотека для работы с энкодерами: http://iarduino.ru/shop/Expansion-payments/enkoder-trema-modul.html
//  Версия: 1.0.2
//  Последнюю версию библиотеки Вы можете скачать по ссылке: http://iarduino.ru/file/297.html
//  Подробное описание функции бибилиотеки доступно по ссылке: http://wiki.iarduino.ru/page/enkoder-trema-modul/
//  Библиотека является собственностью интернет магазина iarduino.ru и может свободно использоваться и распространяться!
//  При публикации устройств или скетчей с использованием данной библиотеки, как целиком, так и её частей,
//  в том числе и в некоммерческих целях, просим Вас опубликовать ссылку: http://iarduino.ru
//  Автор библиотеки: Панькин Павел
//  Если у Вас возникли технические вопросы, напишите нам: shop@iarduino.ru

#ifndef iarduino_Encoder_tmr_h
#define iarduino_Encoder_tmr_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#if defined(ESP32)																//
	static hw_timer_t *Esp32Timer = timerBegin(2, 3, true);						//	Определяем структуру настройки 2 таймера, предделитель = 3 (потом его изменим), флаг = true - счёт вперёд.
	extern void timer_callback_ESP32(void);										//	Подключаем функцию обработки прерываний 2 таймера для плат ESP32.
#elif defined(ESP8266)															//
	extern void timer_callback_ESP8266(void);									//	Подключаем функцию обработки прерываний 1 таймера для плат ESP8266.
#elif defined(RENESAS_CORTEX_M4)												//
	#include <FspTimer.h>														//	Подключаем библиотеку управления таймерами для плат Arduino UNO R4.
	static FspTimer objTimer;													//	Создаём объект для работы с таймером.
	extern void timer_callback_R4(timer_callback_args_t*);						//	Подключаем функцию обработки прерываний таймера для плат Arduino UNO R4.
#endif																			//
																				//
#define	encLEFT						-1											//	Константа указывающая, что цвет применяется ко всем светодиодам.
#define	encRIGHT					1											//	Константа указывающая, что цвет применяется ко всем светодиодам.
																				//
class iarduino_Encoder_tmr{														//
	public:							iarduino_Encoder_tmr	(uint8_t, uint8_t);	//	Создание экземпляра класса			(№ вывода, № вывода).
					bool			begin					(void);				//	Инициализация работы с энкодером	(без параметров)	Возвращает флаг инициализации.
					int8_t			read					(void);				//	Чтение состояния энкодера			(без параметров)	Возвращает направление последнего такта поворота: 0 нет, encLEFT =-1 или encRIGHT =+1.
	private:																	//
		volatile	uint8_t			enc_NUM = 0;								//	Определяем переменную для хранения номера подключённого энкодера.
		#include	"iarduino_Encoder_tmr_Timer.h"								//	Подключаем функцию конфигурирования таймера Timer_Begin( частота Гц ).
};																				//
																				//
class iarduino_Encoder_tmr_VolatileClass{										//
	public:																		//
		volatile	uint8_t			enc_SUM;									//	Объявляем переменную для хранения количества подключённых энкодеров.
		volatile	uint8_t			pin_NUMa[8];								//	Объявляем массив с номерами выводов к которым подключены энкодеры.
		volatile	uint8_t			pin_NUMb[8];								//	Объявляем массив с номерами выводов к которым подключены энкодеры.
	#if defined(ESP32)															//
		volatile	uint32_t*		pin_PRTa[8];								//	Объявляем массив с указателями на адреса входных портов выводов.
		volatile	uint32_t*		pin_PRTb[8];								//	Объявляем массив с указателями на адреса входных портов выводов.
		volatile	uint32_t		pin_MSKa[8];								//	Объявляем массив с масками выводов для входных портов выводов.
		volatile	uint32_t		pin_MSKb[8];								//	Объявляем массив с масками выводов для входных портов выводов.
	#else																		//
		volatile	uint16_t*		pin_PRTa[8];								//	Объявляем массив с указателями на адреса входных портов выводов.
		volatile	uint16_t*		pin_PRTb[8];								//	Объявляем массив с указателями на адреса входных портов выводов.
		volatile	uint16_t		pin_MSKa[8];								//	Объявляем массив с масками выводов для входных портов выводов.
		volatile	uint16_t		pin_MSKb[8];								//	Объявляем массив с масками выводов для входных портов выводов.
	#endif																		//
		volatile	uint8_t			val_BITS[8];								//	Объявляем массив с последними значениями прочитанными с выводов энкодера (abababab).
		volatile	int8_t			i,j;										//	Объявляем переменные для функции обработчика прерываний таймера.
};																				//
																				//
#endif																			//
