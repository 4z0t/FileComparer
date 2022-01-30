#include <iostream>
#include <filesystem>
#include "FileComparer.h"
#include "Task10.h"
#include "Task9.h"



void file_compare()
{
	FileComparer fc;
	//fc.start("C:\\Users\\4z0t\\Documents\\lab6test","results.txt");
	fc.start("E:\\Program Files (x86)", "results.txt");

}

void task10()
{
	Task10 t;
	t.start("C:\\Users\\4z0t\\Downloads\\Учебный архив к ЛР6\\2021-2022 учебный год");
	//t.start("C:\\Users\\4z0t\\Documents\\lab6test");
}

void task9()
{
	Task9 t;
	//t.start("C:\\Users\\4z0t\\Documents\\lab6test","BRUH");
	t.start("C:\\Users\\4z0t\\Downloads\\Учебный архив к ЛР6\\2021-2022 учебный год","ЭМФ");
}

int main()
{
	setlocale(LC_CTYPE, "Rus");
	file_compare();
}
