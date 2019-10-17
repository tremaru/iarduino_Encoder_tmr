[![](https://iarduino.ru/img/logo.svg)](https://iarduino.ru)[![](https://wiki.iarduino.ru/img/git-shop.svg?3)](https://iarduino.ru) [![](https://wiki.iarduino.ru/img/git-wiki.svg?2)](https://wiki.iarduino.ru) [![](https://wiki.iarduino.ru/img/git-lesson.svg?2)](https://lesson.iarduino.ru)[![](https://wiki.iarduino.ru/img/git-forum.svg?2)](http://forum.trema.ru)

# iarduino\_Encoder\_tmr

**This is a library for Arduino IDE. It allows to work with [Encoder](https://iarduino.ru/shop/Expansion-payments/enkoder-trema-modul.html) module made by iArduino.ru**

**Данная библиотека для Arduino IDE позволяет работать с модулем [Энкодер](https://iarduino.ru/shop/Expansion-payments/enkoder-trema-modul.html) от [iArduino.ru](https://iarduino.ru)**

> Подробнее про установку библиотеки читайте в нашей [инструкции](https://wiki.iarduino.ru/page/Installing_libraries/).

> Подробнее про подключение к [Arduino UNO](https://iarduino.ru/shop/boards/arduino-uno-r3.html)/[Piranha UNO](https://iarduino.ru/shop/boards/piranha-uno-r3.html) читайте на нашей [wiki](https://wiki.iarduino.ru/page/enkoder-trema-modul/#h3_3)


| Модель | Ссылка на магазин |
|---|---|
| <p>Энкодер (Trema-модуль)</p> <img src="https://wiki.iarduino.ru/img/resources/783/783.svg" width="100px"></img>| https://iarduino.ru/shop/Expansion-payments/enkoder-trema-modul.html |


## Назначение функций:

**Подробное описание работы с библиотекой и примеры смотрите на [нашем сайте](https://wiki.iarduino.ru/page/enkoder-trema-modul/#h3_6)**

**Подключение библиотеки**

```C++
#include <iarduino_Encoder_tmr.h> // Подключаем библиотеку.
iarduino_Encoder_tmr ОБЪЕКТ( №_ВЫВОДА_A , №_ВЫВОДА_B ); //Создаём объект (один объект работает с одним энкодером).
```

**Инициализация работы** 

```C++
ОБЪЕКТ.begin(); // Инициализация работы с энкодером.
```

**Чтение состояния** 

```C++
ОБЪЕКТ.read(); // Чтение состояния энкодера
```

