#include "iarduino_Encoder_tmr.h"																				//
																												//
volatile	iarduino_Encoder_tmr_VolatileClass IETVC;															//	Создаём объект IETVC.

#if defined(__AVR__)																							//
	static const int8_t encSTEPS[16] PROGMEM = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};							//	Таблица направлений поворота (в памяти программ).
	#define encSTEP(i)	((int8_t)pgm_read_byte(&encSTEPS[i]))													//	Чтение таблицы из памяти программ.
#else																											//
	static const int8_t encSTEPS[16] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};									//	Таблица направлений поворота.
	#define encSTEP(i)	(encSTEPS[i])																			//	Чтение таблицы.
#endif																											//
																												//
//			Конструктор класса:																					//
			iarduino_Encoder_tmr::iarduino_Encoder_tmr(uint8_t a, uint8_t b){									//	Параметры конструктора: вывод A, вывод B.
			static uint8_t enc_SUM=0;																			//	Определяем количество созданных экземпляров класса iarduino_Encoder_tmr.
			if( enc_SUM<8 ){																					//	Если уже подключено менее 8 экнодеров, то ...
				enc_NUM					= enc_SUM;																//	Сохраняем номер подключённого энкодера
				IETVC.enc_SUM			= ++enc_SUM;															//	Увеличиваем количество подключённых энкодеров
				IETVC.pin_NUMa[enc_NUM]	= a;																	//	Сохраняем № вывода «a» к которому подключён энкодер
				IETVC.pin_NUMb[enc_NUM]	= b;																	//	Сохраняем № вывода «b» к которому подключён энкодер
			#if defined(ESP32)																					//
				IETVC.pin_PRTa[enc_NUM]	= (uint32_t*)portInputRegister(digitalPinToPort(a));					//	Сохраняем адрес входного порта для вывода «a»
				IETVC.pin_PRTb[enc_NUM]	= (uint32_t*)portInputRegister(digitalPinToPort(b));					//	Сохраняем адрес входного порта для вывода «b»
			#else																								//
				IETVC.pin_PRTa[enc_NUM]	= (uint16_t*)portInputRegister(digitalPinToPort(a));					//	Сохраняем адрес входного порта для вывода «a»
				IETVC.pin_PRTb[enc_NUM]	= (uint16_t*)portInputRegister(digitalPinToPort(b));					//	Сохраняем адрес входного порта для вывода «b»
			#endif																								//
				IETVC.pin_MSKa[enc_NUM]	= digitalPinToBitMask(a);												//	Сохраняем маску вывода «a» для входного порта
				IETVC.pin_MSKb[enc_NUM]	= digitalPinToBitMask(b);												//	Сохраняем маску вывода «b» для входного порта
			}else{																								//	Иначе (если пытаются подключить 9 энкодер), то ...
				enc_NUM					= 8;																	//	Помечаем энкодер как лишний, номера 0-7 уже заняты
			}																									//
}																												//
																												//
//			Инициализация работы с энкодером																	//
bool		iarduino_Encoder_tmr::begin(void){																	//
			if(enc_NUM<8){																						//	Если данный энкодер получил свой номер (0-7), то ...
				pinMode(IETVC.pin_NUMa[enc_NUM], INPUT);														//	Конфигурируем вывод «a» энкодера как вход
				pinMode(IETVC.pin_NUMb[enc_NUM], INPUT);														//	Конфигурируем вывод «b» энкодера как вход
				IETVC.val_BITS[enc_NUM]	 = 0;																	//	Готовим ячейку для состояния выводов энкодера
				IETVC.val_BITS[enc_NUM]	|= (*IETVC.pin_PRTa[enc_NUM]&IETVC.pin_MSKa[enc_NUM])==0?0:1;			//	Читаем состояние вывода «a»
				IETVC.val_BITS[enc_NUM]	<<= 1;																	//	Сдвигаем прочитанный бит влево на 1 разряд
				IETVC.val_BITS[enc_NUM]	|= (*IETVC.pin_PRTb[enc_NUM]&IETVC.pin_MSKb[enc_NUM])==0?0:1;			//	Читаем состояние вывода «b»
				IETVC.val_STEP[enc_NUM]	 = 0;																	//	Обнуляем счётчик четвертей такта
				Timer_Begin(4000L);																				//	Разрешаем прерывания таймера на частоте 4кГц
				return true;																					//	Возвращаем флаг успеха.
			}	return false;																					//	Если энкодер оказался лишним (9 и далее), возвращаем false.
}																												//
																												//
//			Получение состояния энкодера																		//
int8_t		iarduino_Encoder_tmr::read(void){																	//
			if(enc_NUM<8){																						//	Если данный энкодер получил свой номер (0-7), то ...
				if(IETVC.val_STEP[enc_NUM] >=  4){IETVC.val_STEP[enc_NUM] -= 4; return encRIGHT;}				//	Если накопились 4 четверти такта вправо, то возвращаем encRIGHT
				if(IETVC.val_STEP[enc_NUM] <= -4){IETVC.val_STEP[enc_NUM] += 4; return encLEFT ;}				//	Если накопились 4 четверти такта влево,  то возвращаем encLEFT
			}	return 0;																						//	Если такта поворота не было, или энкодер лишний, возвращаем 0.
}																												//
																												//
//			Обработка прерываний таймера																		//
			Timer_Callback(Timer_Argument){																		//
				IETVC.i=IETVC.enc_SUM;																			//
				while(IETVC.i--){																				//	Проходим по всем подключённым энкодерам
					IETVC.j   = 0;																				//	Подготавливаем маску j
					IETVC.j  |= (*IETVC.pin_PRTa[IETVC.i]&IETVC.pin_MSKa[IETVC.i])==0?0:1;						//	Читаем состояние со входа «a» очередного энкодера в маску j
					IETVC.j <<= 1;																				//	Сдвигаем прочитанный бит маски j влево на 1 разряд
					IETVC.j  |= (*IETVC.pin_PRTb[IETVC.i]&IETVC.pin_MSKb[IETVC.i])==0?0:1;						//	Читаем состояние со входа «b» очередного энкодера в маску j
					if(IETVC.val_BITS[IETVC.i]					!= IETVC.j){									//	Если состояние выводов энкодера изменилось, то ...
						IETVC.j								|= IETVC.val_BITS[IETVC.i]<<2;						//	Склеиваем прошлое и новое состояние выводов в номер строки таблицы
						IETVC.val_BITS[IETVC.i]				 = IETVC.j & B00000011;								//	Запоминаем новое состояние выводов энкодера
						IETVC.val_STEP[IETVC.i]				+= encSTEP(IETVC.j);								//	Прибавляем четверть такта: +1 вправо, -1 влево, 0 если был перескок
						if(IETVC.val_STEP[IETVC.i] >  120){ IETVC.val_STEP[IETVC.i] =  120; }					//	Не копим больше 30 тактов: если скетч давно не звал read(),
						if(IETVC.val_STEP[IETVC.i] < -120){ IETVC.val_STEP[IETVC.i] = -120; }					//	лишние такты теряются, но счёт не переворачивается.
					}																							//
				}																								//
}																												//
