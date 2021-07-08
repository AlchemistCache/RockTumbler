/*Код программы для управления миниатюрной галтовочной машиной.

Электронные компоненты для сборки устройства:
- Arduino uno;
- LCD Keypad Shield 16х2;
- Драйвер двигателя L298N;
- Коллекторный двигатель.

Краткое описание устройства.
Программа хранит в постоянной памяти три режима работы. 
Каждый режим содержит в себе настройки скорости вращения галтовочной емкости и время цикла(часы и минуты).
*/

#include <Arduino.h>
#include "LCD.h"
#include "BigRus1602.h"
#include "MyEEPROM.h"

//Пины для подключения драйвера двигателя
#define EN_A 11		//PWM
#define IN1 3
#define IN2 2

#define MIN_PWM_SPEED 50 //нижняя граница ШИМ при которой работает двигатель
#define MAX_PWM_SPEED 100 //верхняя граница ШИМ при которой двигатель вращается с еще приемлемой скорость

BigRus1602 lcdRus(&lcd);

void stopWork()
{
	digitalWrite(IN1, LOW);
	digitalWrite(IN2, LOW);
	analogWrite(EN_A, LOW);
	resetTextLCD();
}

void startWork()
{
	lcd.noCursor();
	EEPROM.put((uint16_t)&myStructAddr, rejim); //обновим eeprom

	//пересчитаем часы и минуты работы в миллис
	//матемаическая операция разбита на две части для избежания переполнения при расчете
	uint32_t t1 = rejim[currentActiveRegime].hour * 60ul * 60000ul; //миллис в часы
	uint32_t t2 = (rejim[currentActiveRegime].minute) * 60000ul;	//миллис в минуты

	uint32_t millisWorkTime = t1 + t2;							   //общая длительность работы (часы + минуты)
	uint32_t startTime = millis();								   //время начала работы
	int32_t remainingTime = startTime + millisWorkTime - millis(); //оставшееся время работы
	uint8_t currentPwmSpeed = MIN_PWM_SPEED;					   //начальная скорость работы двигателя (задается через ШИМ)

	//переменные для выведения оставшегося времени работы на LCD (Ч:М:С)
	uint32_t rt;
	uint16_t timeHours, timeMins, timeSecs;

	//запускаем двигатель
	uint16_t pwmSpeed = map(rejim[currentActiveRegime].speedRot, 1, SPEED_AMOUNT, MIN_PWM_SPEED, MAX_PWM_SPEED);
	digitalWrite(IN1, HIGH);
	digitalWrite(IN2, LOW);
	analogWrite(EN_A, currentPwmSpeed);

	lcd.clear();

	while (remainingTime > 1)
	{								 //пересчитаем сколько времени осталось до конца рабочего режима
		rt = remainingTime / 1000ul; //оставшееся время в секундах
		timeHours = (rt / 3600ul);
		timeMins = (rt % 3600ul) / 60ul;
		timeSecs = (rt % 3600ul) % 60ul;

		//выведем на экран оставшееся время работы (часы, минуты,секунды) remainingTime
		if (timeHours >= 1)
		{
			lcdRus.print(timeHours);
			lcdRus.print(":", (lcdRus.getCarret() - 1));
			lcdRus.print(timeMins, (lcdRus.getCarret() - 1));
			lcdRus.print("   ", lcdRus.getCarret()); //LCD.clear выполняется долго, быстрее заместить "хвост" пустотой
		}
		else
		{
			lcdRus.print(timeMins);
			lcdRus.print(":", (lcdRus.getCarret() - 1));
			lcdRus.print(timeSecs, (lcdRus.getCarret() - 1));
			lcdRus.print("   ", lcdRus.getCarret()); //LCD.clear выполняется долго, быстрее заместить "хвост" пустотой
		}

		//плавно повышаем ШИМ скорость двигателя пока не достигнем рабочей ШИМ частоты
		if (currentPwmSpeed < pwmSpeed)
		{
			currentPwmSpeed++;
			analogWrite(EN_A, currentPwmSpeed);
		}

		remainingTime = startTime + millisWorkTime - millis(); //пересчитаем оставшееся время работы

		if (detectButton() != BTN_NONE) //при нажатии любой кнопки остановим двигатель
			remainingTime = 0;			//для остановки занулим оставшееся время работы
	}
	stopWork();
}

void setup()
{
	pinMode(EN_A, OUTPUT);
	pinMode(IN1, OUTPUT);
	pinMode(IN2, OUTPUT);

	Serial.begin(9600);
	lcd.begin(16, 2);
	lcdRus.begin();
	lcd.setCursor(16, 0);
	char hello[16] = "Rock tumbler";
	for (uint16_t i = 0; i < sizeof(hello) - 1; i++) //выводим приветствие по одной букве
	{
		lcd.setCursor(i + 1, 0);
		lcd.print(hello[i]);
		Serial.print(hello[i]);
		delay(100);
	}

	readEEPROM();
	delay(1000);

	resetTextLCD(); //отрисуем основной текст на экране
}

void loop()
{ //постоянно проверяем кнопки кейпадшилда на нажатие
	uint8_t pressedButton = detectButton();
	if (pressedButton == BTN_SELECT)
	{
		startWork();
		updateEEPROM();
	}
	else if (pressedButton != BTN_NONE)
		updateMode(pressedButton);
}