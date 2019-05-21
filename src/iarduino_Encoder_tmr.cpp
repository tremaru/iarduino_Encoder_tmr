#include "iarduino_Encoder_tmr.h"

volatile	iarduino_Encoder_tmr_VolatileClass IETVC;
			iarduino_Encoder_tmr::iarduino_Encoder_tmr(uint8_t a, uint8_t b){
			if(IETVC.enc_SUM<8){																				//	Если уже подключено менее 8 экнодеров, то ...
				enc_NUM					=IETVC.enc_SUM;															//	Сохраняем номер подключённого энкодера
				IETVC.enc_SUM			++;																		//	Увеличиваем количество подключённых энкодеров
				IETVC.pin_NUMa[enc_NUM]	=a;																		//	Сохраняем № вывода «a» к которому подключён энкодер
				IETVC.pin_NUMb[enc_NUM]	=b;																		//	Сохраняем № вывода «b» к которому подключён энкодер
				IETVC.pin_PRTa[enc_NUM]	=portInputRegister(digitalPinToPort(a));								//	Сохраняем адрес входного порта для вывода «a»
				IETVC.pin_PRTb[enc_NUM]	=portInputRegister(digitalPinToPort(b));								//	Сохраняем адрес входного порта для вывода «b»
				IETVC.pin_MSKa[enc_NUM]	=digitalPinToBitMask(a);												//	Сохраняем маску вывода «a» для входного порта
				IETVC.pin_MSKb[enc_NUM]	=digitalPinToBitMask(b);												//	Сохраняем маску вывода «b» для входного порта
			}
}

//			Инициализация работы с энкодером
bool		iarduino_Encoder_tmr::begin(void){
			if(IETVC.enc_SUM<8){																				//	Если данный энкодер был подключен в первой восьмёрке, то ...
				pinMode					(IETVC.pin_NUMa[enc_NUM], INPUT);										//	Конфигурируем вывод «a» энкодера как вход
				pinMode					(IETVC.pin_NUMb[enc_NUM], INPUT);										//	Конфигурируем вывод «b» энкодера как вход
				func_TIMER2_SETREG(4000L);																		//	Разрешаем прерывания 2 таймера и устанавливаем его работу в режиме CTC на частоту 4кГц
			}
}

//			Получение состояния энкодера
int8_t		iarduino_Encoder_tmr::read(void){
			if(IETVC.enc_SUM<8){																				//	Если данный энкодер был подключен в первой восьмёрке, то ...
				if(IETVC.val_BITS[enc_NUM] == B10000111){IETVC.val_BITS[enc_NUM]=B00000011; return encLEFT ;}
				if(IETVC.val_BITS[enc_NUM] == B01001011){IETVC.val_BITS[enc_NUM]=B00000011; return encRIGHT;}
				return false;
			}
}

//			установка значений регистров второго таймера под нужную частоту
void		iarduino_Encoder_tmr::func_TIMER2_SETREG(uint32_t i){
				uint16_t j; /* значение предделителя 1,8,32,64,128,256,1024 */
				uint8_t  k; /* значение битов CS22-CS20 в регистре TCCR2B   */
				if(F_CPU/255/  1<i)		{j=   1; k=1;}else
				if(F_CPU/255/  8<i)		{j=   8; k=2;}else
				if(F_CPU/255/ 32<i)		{j=  32; k=3;}else
				if(F_CPU/255/ 64<i)		{j=  64; k=4;}else
				if(F_CPU/255/128<i)		{j= 128; k=5;}else
				if(F_CPU/255/256<i)		{j= 256; k=6;}else
										{j=1024; k=7;}
				TCCR2A					= 0<<COM2A1	| 0<<COM2A0	| 0<<COM2B1	| 0<<COM2B0	| 1<<WGM21	| 0<<WGM20;	//	Биты COM2... = «0» (каналы A и B таймера отключены), биты WGM21 и WGM20 = «10» (таймер 2 в режиме CTC)
				TCCR2B					= 0<<FOC2A	| 0<<FOC2B	| 0<<WGM22	| k;								//	Биты FOC2... = «0» (без принудительной установки результата сравнения), бит WGM22 = «0» (таймер 2 в режиме CTC), биты CS22-CS20 = k (значение предделителя)
				OCR2A					= (uint8_t)(F_CPU/(j*i))-1;												//	Значение регистра сравнения OCR2A настраивается под частоту переполнения счётного регистра TCNT2=i.  i=F_CPU/(предделитель*(OCR2A+1)) => OCR2A = (F_CPU/(предделитель*i))-1
				TIMSK2					= 0<<OCIE2B	| 1<<OCIE2A	| 0<<TOIE2;										//	Разрешаем прерывание по совпадению счётного регистра TCNT2 и регистра сравнения OCR2A
				SREG					= 1<<7;																	//	Устанавливаем флаг глобального разрешения прерываний
}

//			запуск по вектору прерывания TIMER2_COMPA_vect - совпадение счётного регистра TCNT2 и регистра сравнения OCR2A
			ISR(TIMER2_COMPA_vect){
				IETVC.i=IETVC.enc_SUM;
				while(IETVC.i--){																				//	Проходим по всем подключённым энкодерам
					IETVC.j				= 0;																	//	Подготавливаем маску j
					IETVC.j				|= (*IETVC.pin_PRTa[IETVC.i]&IETVC.pin_MSKa[IETVC.i])==0?0:1;			//	Читаем состояние со входа «a» очередного энкодера в маску j
					IETVC.j				<<=1;																	//	Сдвигаем прочитанный бит маски j влево на 1 разряд
					IETVC.j				|= (*IETVC.pin_PRTb[IETVC.i]&IETVC.pin_MSKb[IETVC.i])==0?0:1;			//	Читаем состояние со входа «b» очередного энкодера в маску j
					if((IETVC.val_BITS[IETVC.i] & B00000011)	!= IETVC.j){									//	Если последние два бита в val_BITS не совпадают с прочитанными в маске j, то ...
						IETVC.val_BITS[IETVC.i]					<<=2;											//	Сдвигаем все биты val_BITS влево на 2 разряда
						IETVC.val_BITS[IETVC.i]					|= IETVC.j;										//	Заполняем последние два бита val_BITS значением из маски j
					}
				}
}