
#pragma once
#include <Arduino.h>
#include "LiquidCrystal.h"
#include "Rejim.h"
#include "KeypadShield.h"

#define MAX_HOUR 23		 //максимальное количество часов беспрерывной работы
#define SPEED_AMOUNT 9	 //количество скоростей электромотора

enum
{
	MODE_HOUR = 1,
	MODE_MINUTE,
	MODE_SPEED,
	MODE_REJIM,
};
uint8_t selectMode = MODE_REJIM; //текущая активная настройка (по умолчанию при старте MODE_REJIM)

//Пины для подключения LCD Keypad Shield
const uint8_t rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

const uint8_t qtyMode = 4;		//общее количество пользовательских настроек

void updateLCD(uint8_t updateSelectMode)	//обновим значения активного параметра
{
	switch (updateSelectMode)	//обновляем не весь текст, а только то что было изменено
	{
	case MODE_HOUR: //обновить часы
		lcd.setCursor(6, 1);
		lcd.cursor();
		if (rejim[currentActiveRegime].hour < 10) //если переменная hour имеет однозначное число - добавим нуль (1 = 01; ... ; 9 = 09)
		{
			lcd.print("0");
		}
		lcd.print(rejim[currentActiveRegime].hour);
		lcd.setCursor(6, 1);
		break;
	case MODE_MINUTE: //обновить минуты
		lcd.setCursor(9, 1);
		if (rejim[currentActiveRegime].minute < 10)
		{
			lcd.print("0");
		}
		lcd.print(rejim[currentActiveRegime].minute); //если переменная minute имеет однозначное число - добавим нуль
		lcd.setCursor(9, 1);
		lcd.cursor();
		break;
	case MODE_SPEED: //обновить скорость
		lcd.setCursor(6, 0);
		lcd.print(rejim[currentActiveRegime].speedRot);
		lcd.setCursor(6, 0);
		lcd.cursor();
		break;
	case MODE_REJIM: //обновить номер режима
		for (updateSelectMode = 1; updateSelectMode < qtyMode; updateSelectMode++)
		{ 
			updateLCD(updateSelectMode);	//отрисуем все настройки на экране
		}
		lcd.setCursor(14, 0);
		lcd.print(currentActiveRegime);
		lcd.setCursor(14, 0);
		lcd.cursor();
		break;
	case 5: //старт-стоп
		//startWork();
		break;
	}
}

void resetTextLCD()			//сброс всего LCD текста на базовый
{ //перепишем базовый текст на экране
	lcd.clear();
	lcd.home();
	lcd.print("SPEED:      <Rx>");
	lcd.setCursor(0, 1);
	lcd.print("TIME:   :");

	updateLCD(MODE_REJIM); //покажем на экране номер активного режима и его параметры
}

void updateMode(uint8_t pressedButton)
{
	switch (pressedButton)
	{
	case BTN_UP: //меняем один из параметров режима в зависимости от того какое сейчас значение selectMode
		if (selectMode == MODE_HOUR)
		{
			rejim[currentActiveRegime].hour++; // увеличиваем количесто часов
			if (rejim[currentActiveRegime].hour > MAX_HOUR)
			{
				rejim[currentActiveRegime].hour = 0;
			}					  // закольцовываем
			updateLCD(MODE_HOUR); //поставим курсор в нужное место
		}

		else if (selectMode == MODE_MINUTE)
		{
			rejim[currentActiveRegime].minute++; // увеличиваем количесто минут
			if (rejim[currentActiveRegime].minute > 59)
			{
				rejim[currentActiveRegime].minute = 0;
			}						// закольцовываем минуты
			updateLCD(MODE_MINUTE); //поставим курсор в нужное место
		}

		else if (selectMode == MODE_SPEED)
		{
			rejim[currentActiveRegime].speedRot++; // увеличиваем переменную номера режима
			if (rejim[currentActiveRegime].speedRot > SPEED_AMOUNT)
			{
				rejim[currentActiveRegime].speedRot = 1;
			}					   // закольцовываем
			updateLCD(MODE_SPEED); //поставим курсор в нужное место
		}
		else if (selectMode == MODE_REJIM)
		{
			currentActiveRegime++; // увеличиваем переменную номера режима
			if (currentActiveRegime >= NUM_OF(rejim))
			{
				currentActiveRegime = 0;
			}					   // закольцовываем
			updateLCD(MODE_REJIM); //поставим курсор в нужное место
		}
		break;

	case BTN_DOWN: //меняем один из параметров режима в зависимости от того какое сейчас значение selectMode
		if (selectMode == MODE_HOUR)
		{
			if (rejim[currentActiveRegime].hour == 0)
			{
				rejim[currentActiveRegime].hour = MAX_HOUR;
			} // закольцовываем часы
			else
			{
				rejim[currentActiveRegime].hour--;
			}					  // уменьшаем количесто часов
			updateLCD(MODE_HOUR); //поставим курсор в нужное место
		}
		else if (selectMode == MODE_MINUTE)
		{
			if (rejim[currentActiveRegime].minute == 0)
			{
				rejim[currentActiveRegime].minute = 59;
			} // закольцовываем минуты
			else
			{
				rejim[currentActiveRegime].minute--;
			}						// уменьшаем количесто минут
			updateLCD(MODE_MINUTE); //поставим курсор в нужное место
		}
		else if (selectMode == MODE_SPEED)
		{
			rejim[currentActiveRegime].speedRot--; // уменьшаем переменную номера режима
			if (rejim[currentActiveRegime].speedRot < 1)
			{
				rejim[currentActiveRegime].speedRot = SPEED_AMOUNT;
			}					   // закольцовываем скорость
			updateLCD(MODE_SPEED); //поставим курсор в нужное место
		}
		else if (selectMode == MODE_REJIM)
		{
			if (currentActiveRegime == 0)
			{
				currentActiveRegime = (NUM_OF(rejim) - 1);
			} // закольцовываем перебор режимов
			else
			{
				currentActiveRegime--;
			}					   // уменьшаем переменную номера режима
			updateLCD(MODE_REJIM); //поставим курсор в нужное место
		}
		break;

	case BTN_LEFT: //перелистываем на предыдущую активную настройку
		selectMode--;
		if (selectMode <= 0)
		{
			selectMode = qtyMode;
		}					   // закольцовываем
		updateLCD(selectMode); //поставим курсор в нужное место
		break;

	case BTN_RIGHT: //перелистываем на следующую активную настройку
		selectMode++;
		if (selectMode > qtyMode)
		{
			selectMode = 1;
		}					   // закольцовываем
		updateLCD(selectMode); //поставим курсор в нужное место
		break;

	case BTN_SELECT:
		//startWork(); //старт
		break;

	default:
		break;
	}
}
