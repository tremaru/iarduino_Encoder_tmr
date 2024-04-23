#include "iarduino_Encoder_tmr.h"																				//
																												//
volatile	iarduino_Encoder_tmr_VolatileClass IETVC;															//	Создаём объект IETVC.
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
			}																									//
}																												//
																												//
//			Инициализация работы с энкодером																	//
bool		iarduino_Encoder_tmr::begin(void){																	//
			if(IETVC.enc_SUM<8){																				//	Если данный энкодер был подключен в первой восьмёрке, то ...
				pinMode(IETVC.pin_NUMa[enc_NUM], INPUT);														//	Конфигурируем вывод «a» энкодера как вход
				pinMode(IETVC.pin_NUMb[enc_NUM], INPUT);														//	Конфигурируем вывод «b» энкодера как вход
				Timer_Begin(4000L);																				//	Разрешаем прерывания таймера на частоте 4кГц
				return true;																					//	Возвращаем флаг успеха.
			}	return false;																					//	Если пытаются подключить 9 энкодер, возвращаем false.
}																												//
																												//
//			Получение состояния энкодера																		//
int8_t		iarduino_Encoder_tmr::read(void){																	//
			if(IETVC.enc_SUM<8){																				//	Если данный энкодер был подключен в первой восьмёрке, то ...
				if(IETVC.val_BITS[enc_NUM] == B10000111){IETVC.val_BITS[enc_NUM]=B00000011; return encLEFT ;}	//
				if(IETVC.val_BITS[enc_NUM] == B01001011){IETVC.val_BITS[enc_NUM]=B00000011; return encRIGHT;}	//
				return false;																					//
			}																									//
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
					if((IETVC.val_BITS[IETVC.i] & B00000011)	!= IETVC.j){									//	Если последние два бита в val_BITS не совпадают с прочитанными в маске j, то ...
						IETVC.val_BITS[IETVC.i]					<<=2;											//	Сдвигаем все биты val_BITS влево на 2 разряда
						IETVC.val_BITS[IETVC.i]					|= IETVC.j;										//	Заполняем последние два бита val_BITS значением из маски j
					}																							//
				}																								//
}																												//
