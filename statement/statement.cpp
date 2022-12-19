#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
using namespace std;
using namespace nlohmann;

//для создания рандомных значений
#define NUMBER_RECORDS 2

struct ZAP
{
	int personnelNumber;
	int month;
	float simply;
	float side;
	float overtime;
	float deductionAmount;
	float amountIssued;
};

string FIELD_1 = "|№ п/п",
       FIELD_2 = "|Таб. №|",
       FIELD_3 = "|Месяц|",
       FIELD_4 = "|Повременно|",
       FIELD_5 = "|Сдельно|",
       FIELD_6 = "|Сверхурочно|",
       FIELD_7 = "|Сумма удержаний|",
       FIELD_8 = "|Сумма к выдаче|";

void menu(vector<ZAP>& sheet, vector<ZAP>& workSheet, string fileName);
string getFileName();
int getAmountZAP(string fileName);
void addNote(vector<ZAP>& sheet, string fileName);
void closeProgram();
void createOrOpen(vector<ZAP>& sheet, string fileName);
void open(vector<ZAP>& sheet, string fileName);
json getStruct(string fileName);
void serializeStruct(vector<ZAP>& sheet, json jsonData, int i);
void create(vector<ZAP>& sheet, string fileName);
void createJsonFile(json jsonData, string fileName);
void printData(vector<ZAP>& sheet, string fileName);
void printCap(bool isMonth = true);
void requestMonth(vector<ZAP>& sheet,
                  vector<ZAP>& workSheet,
                  string fileName);
void findCopyAndPrintSheet(vector<ZAP>& sheet,
                           vector<ZAP>& workSheet,
                           int requestedServiceNumber);
void copySheet(vector<ZAP>& sheet,
               vector<ZAP>& workSheet,
               int numberOfLoopExecution,
               int i);
void printSelectedMonth(vector<ZAP>& workSheet,
                        int previousMonth,
                        int index);
void printRecords(vector<ZAP>& workSheet, int serialNumber, bool isMonth = true);
void printTotals(json sum);
void printSheetFieldsAndRecords(vector<ZAP>& workSheet, int serialNumber);
void printSheetAndFields(vector<ZAP>& workSheet, int serialNumber);
void printMaxIndexOvertime(int maxIndexPersonnelNumber);

int main()
{
	string fileName;
	json jsonData;

	srand(time(NULL));
	setlocale(LC_ALL, "ru");

	fileName = getFileName();
	vector<ZAP> sheet,
	            workSheet;
	createOrOpen(sheet, fileName);
	menu(sheet, workSheet, fileName);

	system("PAUSE");
	return 0;
}

void menu(vector<ZAP>& sheet, vector<ZAP>& workSheet, string fileName)
{
	string menuNumber;

	while (1)
	{
		cout << "\n1 - Вывести содержимое файла" << endl;
		cout << "2 - Добавить запись" << endl;
		cout << "3 - Расчитать ведомость" << endl;
		cout << "4 - Закрыть программу" << endl;
		cout << "\nВведите номер пункта меню: ";
		cin >> menuNumber;

		if (menuNumber == "1")
			printData(sheet, fileName);
		else if (menuNumber == "2")
			addNote(sheet, fileName);
		else if (menuNumber == "3")
			requestMonth(sheet, workSheet, fileName);
		else if (menuNumber == "4")
			closeProgram();
		else
			cout << "Такого пункта меню нет :)\nПопробуй ещё раз" << endl;
	}
}

string getFileName()
{
	string fileName = "Sheet",
	       fileType = ".json",
	       changeFileName;

	while (1)
	{
		cout << "По умолчанию имя файла \"" << fileName << "\"\nХотите его изменить? y/n: ";
		cin >> changeFileName;

		if (changeFileName == "y")
		{
			cout << "\nВведите новое имя файла: ";
			cin >> fileName;
			break;
		}
		if (changeFileName == "n") break;
		if (changeFileName != "y" && changeFileName != "n")
			cout << "Не корректный ввод" << endl;
	}
	fileName += fileType;
	return fileName;
}

int getAmountZAP(string fileName)
{
	json ZAP = getStruct(fileName);
	bool isErrorFileNotFound = ZAP.contains("Error file not found");
	int amountZAP;

	if (isErrorFileNotFound)
		return NUMBER_RECORDS;

	amountZAP = ZAP.size();
	return amountZAP;
}

void addNote(vector<ZAP>& sheet, string fileName)
{
	int personnelNumber;
	int month;
	float simply;
	float side;
	float overtime;
	float deductionAmount;
	float amountIssued;

	int indexNewNote;

	json dataJson = getStruct(fileName);

	cout << "Введите данные" << endl;

	cout << "Табельный номер:" << endl;
	cin >> personnelNumber;

	cout << "Номер месяца:" << endl;
	cin >> month;

	cout << "Сумма начисленний (руб. коп.) повременно:" << endl;
	cin >> simply;

	cout << "Сумма начисленний (руб. коп.) сдельно:" << endl;
	cin >> side;

	cout << "Сумма начисленний (руб. коп.) сверхурочно:" << endl;
	cin >> overtime;

	cout << "Сумма удержаний:" << endl;
	cin >> deductionAmount;

	amountIssued = simply + side + overtime + deductionAmount;

	json enteredData{
		{"personnelNumber", personnelNumber},
		{"month", month},
		{"simply", simply},
		{"side", side},
		{"overtime", overtime},
		{"deductionAmount", deductionAmount},
		{"amountIssued", amountIssued},
	};

	dataJson.insert(dataJson.end(), enteredData);
	createJsonFile(dataJson, fileName);

	indexNewNote = getAmountZAP(fileName) - 1;
	serializeStruct(sheet, dataJson, indexNewNote);

	cout << "Запись успешно добавлена" << endl;
}

void closeProgram()
{
	cout << "Программа завершилась";
	exit(0);
}

void createOrOpen(vector<ZAP>& sheet, string fileName)
{
	ifstream jsonFile(fileName, ifstream::binary);
	bool isOpen = jsonFile.is_open();
	jsonFile.close();

	if (isOpen)
		open(sheet, fileName);
	else
		create(sheet, fileName);
}

void open(vector<ZAP>& sheet, string fileName)
{
	int amountZAP = getAmountZAP(fileName);
	json dataJson = getStruct(fileName);
	for (int i = 0; i < amountZAP; i++)
		serializeStruct(sheet, dataJson, i);
	printData(sheet, fileName);
}

json getStruct(string fileName)
{
	ifstream jsonFile(fileName, ifstream::binary);
	bool isOpen = jsonFile.is_open();

	if (isOpen)
	{
		json jsonData = json::parse(jsonFile);
		jsonFile.close();
		return jsonData;
	}
	jsonFile.close();
	return {{"Error file not found", true}};
}

void serializeStruct(vector<ZAP>& sheet, json jsonData, int i)
{
	int personnelNumber;
	int month;
	float simply;
	float side;
	float overtime;
	float deductionAmount;
	float amountIssued;

	personnelNumber = jsonData[i]["personnelNumber"].get<int>();
	month = jsonData[i]["month"].get<int>();
	simply = jsonData[i]["simply"].get<float>();
	side = jsonData[i]["side"].get<float>();
	overtime = jsonData[i]["overtime"].get<float>();
	deductionAmount = jsonData[i]["deductionAmount"].get<float>();
	amountIssued = jsonData[i]["amountIssued"].get<float>();

	sheet.push_back(ZAP{
		personnelNumber,
		month,
		simply,
		side,
		overtime,
		deductionAmount,
		amountIssued
	});
}

void create(vector<ZAP>& sheet, string fileName)
{
	int personnelNumber;
	int month;
	float simply;
	float side;
	float overtime;
	float deductionAmount;
	float amountIssued;

	int amountZAP = getAmountZAP(fileName);
	json jsonData{};
	string userResponse;

	while (1)
	{
		cout << "Хотите чтобы данные заполнились рандомно? (y/n): ";
		cin >> userResponse;

		if (userResponse == "y")
		{
			for (int i = 0; i < amountZAP; i++)
			{
				personnelNumber = rand() % 99999;
				month = rand() % 12;
				simply = (float)(rand()) / ((float)(RAND_MAX / 9));
				side = (float)(rand()) / ((float)(RAND_MAX / 9));
				overtime = (float)(rand()) / ((float)(RAND_MAX / 9));
				deductionAmount = (float)(rand()) / ((float)(RAND_MAX / 9));
				amountIssued = simply + side + overtime + deductionAmount;

				sheet.push_back(ZAP{
					personnelNumber,
					month,
					simply,
					side,
					overtime,
					deductionAmount,
					amountIssued
				});

				jsonData[i]["personnelNumber"] = personnelNumber;
				jsonData[i]["month"] = month;
				jsonData[i]["simply"] = simply;
				jsonData[i]["side"] = side;
				jsonData[i]["overtime"] = overtime;
				jsonData[i]["deductionAmount"] = deductionAmount;
				jsonData[i]["amountIssued"] = amountIssued;
			}
			break;
		}

		if (userResponse == "n")
		{
			for (int i = 0; i < amountZAP; i++)
			{
				cout << "Введите данные " << i + 1 << endl;

				cout << "Табельный номер:" << endl;
				cin >> personnelNumber;

				cout << "Номер месяца:" << endl;
				cin >> month;

				cout << "Сумма начисленний (руб. коп.) повременно:" << endl;
				cin >> simply;

				cout << "Сумма начисленний (руб. коп.) сдельно:" << endl;
				cin >> side;

				cout << "Сумма начисленний (руб. коп.) сверхурочно:" << endl;
				cin >> overtime;

				cout << "Сумма удержаний:" << endl;
				cin >> deductionAmount;

				amountIssued = simply + side + overtime + deductionAmount;

				sheet.push_back(ZAP{
					personnelNumber,
					month,
					simply,
					side,
					overtime,
					deductionAmount,
					amountIssued
				});

				jsonData[i]["personnelNumber"] = personnelNumber;
				jsonData[i]["month"] = month;
				jsonData[i]["simply"] = simply;
				jsonData[i]["side"] = side;
				jsonData[i]["overtime"] = overtime;
				jsonData[i]["deductionAmount"] = deductionAmount;
				jsonData[i]["amountIssued"] = amountIssued;
			}
			break;
		}

		if (userResponse != "y" && userResponse != "n")
			cout << "Не корректный ввод. Повторите попытку :)" << endl;
	}
	createJsonFile(jsonData, fileName);
}

void createJsonFile(json jsonData, string fileName)
{
	ofstream jsonFile(fileName);
	jsonFile << jsonData;
	cout << "Файл " << fileName << " успешно создан" << endl;
	jsonFile.close();
}

void printData(vector<ZAP>& sheet, string fileName)
{
	printCap();

	for (int i = 0; i < sheet.size(); i++)
		printRecords(sheet, i);
}

void printCap(bool isMonth)
{
	if (isMonth)
		cout
			<< "\n"
			<< left
			<< setw(FIELD_1.size()) << FIELD_1
			<< setw(FIELD_2.size()) << FIELD_2
			<< setw(FIELD_3.size()) << FIELD_3
			<< setw(FIELD_4.size()) << FIELD_4
			<< setw(FIELD_5.size()) << FIELD_5
			<< setw(FIELD_6.size()) << FIELD_6
			<< setw(FIELD_7.size()) << FIELD_7
			<< setw(FIELD_8.size()) << FIELD_8
			<< endl;
	else
		cout
			<< "\n"
			<< left
			<< setw(FIELD_1.size()) << FIELD_1
			<< setw(FIELD_2.size()) << FIELD_2
			<< setw(FIELD_4.size()) << FIELD_4
			<< setw(FIELD_5.size()) << FIELD_5
			<< setw(FIELD_6.size()) << FIELD_6
			<< setw(FIELD_7.size()) << FIELD_7
			<< setw(FIELD_8.size()) << FIELD_8
			<< endl;
}

void requestMonth(vector<ZAP>& sheet,
                  vector<ZAP>& workSheet,
                  string fileName)
{
	string requestedServiceNumber;
	bool isEnd;

	cout << "\nВведите запрашиваемый месяц. По завершению ввода введите exit:\n";
	while (true)
	{
		cin >> requestedServiceNumber;
		isEnd = requestedServiceNumber == "exit";
		if (isEnd) break;
		findCopyAndPrintSheet(sheet,
		                      workSheet,
		                      stoi(requestedServiceNumber)
		);
	}
}

void copySheet(vector<ZAP>& sheet,
               vector<ZAP>& workSheet,
               int numberOfLoopExecution,
               int i)
{
	int personnelNumber;
	int month;
	float simply;
	float side;
	float overtime;
	float deductionAmount;
	float amountIssued;

	personnelNumber = sheet[i].personnelNumber;
	month = sheet[i].month;
	simply = sheet[i].simply;
	side = sheet[i].side;
	overtime = sheet[i].overtime;
	deductionAmount = sheet[i].deductionAmount;
	amountIssued = sheet[i].amountIssued;

	workSheet.push_back(ZAP{
		personnelNumber,
		month,
		simply,
		side,
		overtime,
		deductionAmount,
		amountIssued
	});
	workSheet[numberOfLoopExecution].personnelNumber = sheet[i].personnelNumber;
	workSheet[numberOfLoopExecution].month = sheet[i].
		month;
	workSheet[numberOfLoopExecution].simply = sheet[i].simply;
	workSheet[numberOfLoopExecution].side = sheet[i].side;
	workSheet[numberOfLoopExecution].overtime = sheet[i].overtime;
	workSheet[numberOfLoopExecution].deductionAmount = sheet[i].deductionAmount;
	workSheet[numberOfLoopExecution].amountIssued =
		sheet[i].amountIssued;
}

void findCopyAndPrintSheet(vector<ZAP>& sheet,
                           vector<ZAP>& workSheet,
                           int requestedServiceNumber)
{
	bool isFoundMonth;
	int month,
	    maxIndexPersonnelNumber = 0,
	    previousMonth = NULL;
	int numberOfLoopExecution = 0;
	float simply = 0,
	      side = 0,
	      overtime = 0,
	      deductionAmount = 0,
	      amountIssued = 0,
	      maxOvertime = FLT_MIN;
	json sum{
		{"simply", simply},
		{"side", side},
		{"overtime", overtime},
		{"deductionAmount", deductionAmount},
		{"amountIssued", amountIssued}
	};

	for (int i = 0; i < sheet.size(); i++)
	{
		month = sheet[i].month;
		isFoundMonth = requestedServiceNumber == month;
		if (isFoundMonth)
		{
			copySheet(sheet, workSheet, numberOfLoopExecution, i);
			printSelectedMonth(workSheet, previousMonth, numberOfLoopExecution);

			sum["simply"] = sum["simply"].get<float>() + sheet[i].simply;
			sum["side"] = sum["side"].get<float>() + sheet[i].side;
			sum["overtime"] = sum["overtime"].get<float>() + sheet[i].overtime;
			sum["deductionAmount"] = sum["deductionAmount"].get<float>() + sheet[i].deductionAmount;
			sum["amountIssued"] = sum["amountIssued"].get<float>() + sheet[i].amountIssued;

			//максимальное сверхурочное начисление
			if (maxOvertime < sheet[i].overtime)
			{
				maxOvertime = sheet[i].overtime;
				maxIndexPersonnelNumber = sheet[i].personnelNumber;
			}

			previousMonth = month;
			numberOfLoopExecution++;
		}
	}

	if (!numberOfLoopExecution)
	{
		cout << "Ты что-то попутал такого месяца нет :)\n";
		return;
	}
	printTotals(sum);
	printMaxIndexOvertime(maxIndexPersonnelNumber);
}

void printSelectedMonth(vector<ZAP>& workSheet,
                        int previousMonth,
                        int index)
{
	int currentMonth;
	bool isSameServiceNumber;

	currentMonth = workSheet[index].month;
	isSameServiceNumber = currentMonth == previousMonth;
	if (isSameServiceNumber)
		printRecords(workSheet, index, false);
	else
		printSheetFieldsAndRecords(workSheet, index);
}

void printRecords(vector<ZAP>& sheet, int serialNumber, bool isMonth)
{
	if (isMonth)
		cout
			<< setw(FIELD_1.size()) << serialNumber + 1
			<< setw(FIELD_2.size()) << sheet[serialNumber].personnelNumber
			<< setw(FIELD_3.size()) << sheet[serialNumber].month
			<< setw(FIELD_4.size()) << sheet[serialNumber].simply
			<< setw(FIELD_5.size()) << sheet[serialNumber].side
			<< setw(FIELD_6.size()) << sheet[serialNumber].overtime
			<< setw(FIELD_7.size()) << sheet[serialNumber].deductionAmount
			<< setw(FIELD_8.size()) << sheet[serialNumber].amountIssued
			<< endl;
	else
		cout
			<< setw(FIELD_1.size()) << serialNumber + 1
			<< setw(FIELD_2.size()) << sheet[serialNumber].personnelNumber
			<< setw(FIELD_4.size()) << sheet[serialNumber].simply
			<< setw(FIELD_5.size()) << sheet[serialNumber].side
			<< setw(FIELD_6.size()) << sheet[serialNumber].overtime
			<< setw(FIELD_7.size()) << sheet[serialNumber].deductionAmount
			<< setw(FIELD_8.size()) << sheet[serialNumber].amountIssued
			<< endl;
}

void printTotals(json sum)
{
	cout
		<< "\n"
		<< left
		<< setw(FIELD_1.size() + FIELD_2.size()) << "Итого:"
		<< setw(FIELD_4.size()) << sum["simply"].get<float>()
		<< setw(FIELD_5.size()) << sum["side"].get<float>()
		<< setw(FIELD_6.size()) << sum["overtime"].get<float>()
		<< setw(FIELD_7.size()) << sum["deductionAmount"].get<float>()
		<< setw(FIELD_8.size()) << sum["amountIssued"].get<float>()
		<< endl;
}

void printSheetFieldsAndRecords(vector<ZAP>& workSheet, int serialNumber)
{
	printSheetAndFields(workSheet, serialNumber);
	printRecords(workSheet, serialNumber, false);
}

void printSheetAndFields(vector<ZAP>& workSheet, int serialNumber)
{
	cout
		<< "\nРасчетная ведомость за "
		<< workSheet[serialNumber].month
		<< " месяц"
		<< endl;
	printCap(false);
}

void printMaxIndexOvertime(int maxIndexPersonnelNumber)
{
	cout << "\nМаксимальное сверхурочные начисления у " << maxIndexPersonnelNumber << endl;
}
