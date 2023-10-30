﻿#include <iostream>
#include <fstream>
using namespace std;

int DegV(int** mas, int countV, int i); //счетчик степени вершины i
void Connection(int** mas, bool*& visited, int i, int countV); //проверка на связность графа
bool Most(int** mas, int countV, bool*& visited); //проверка на ребро-мост
bool Proverka_Cycle(int** mas, int countV); //проверка на существование эйлерова цикла
bool Proverka_Chain(int** mas, int countV); //проверка на существование эйлеровой цепи
void Poisk_Fleury(int** mas, int countV, int countE, bool*& visited, ofstream& f_out); //построение цикла/цепи

int main() {
	clock_t t1 = clock(); //переменная для определения времени в начале программы
	ifstream f_in("input.txt"); //открытие файла для чтения
	int countV; f_in >> countV; //количество вершин в графе

	int** mas = new int* [countV]; //создание массива для матрицы смежности
	for (int i = 0; i < countV; i++)
		mas[i] = new int[countV];

	for (int i = 0; i < countV; i++) //заполнение массива данными из матрицы смежности
		for (int j = 0; j < countV; j++)
			f_in >> mas[i][j];
	f_in.close(); //закрытие файла для чтения

	int countE = 0; //количество ребер в графе
	for (int i = 0; i < countV; i++)
		for (int j = 0; j < countV; j++)
			if (mas[i][j] == 1) countE++;
	countE /= 2;

	bool* visited = new bool[countV]; //массив пройденных вершин для проверки на связность графа
	for (int m = 0; m < countV; m++) visited[m] = false; //значение false означает, что вершина не пройдена

	Connection(mas, visited, 0, countV); //проходим все возможные вершины, начиная с первой

	ofstream f_out("output.txt"); //открытие файла для записи
	bool flag = true; //логическая переменная для определения связности/несвязности графа
	for (int m = 0; m < countV; m++)
		if (!visited[m]) { f_out << "No"; flag = false; break; } //если встретилась непройденная вершина, значит, граф несвязный, печатаем в файле "No"

	if (flag) Poisk_Fleury(mas, countV, countE, visited, f_out); //если граф связный, начинаем поиск эйлерова цикла либо эйлеровой цепи

	f_out.close(); //закрытие файла для записи
	for (int i = 0; i < countV; i++) delete[]mas[i]; //освобождение памяти
	delete[]mas; delete[]visited;

	clock_t t2 = clock();  //переменная для определения времени в конце программы
	cout << "Time: " << (t2 - t1 + .0) / CLOCKS_PER_SEC << " seconds" << endl; //считаем время
}

int DegV(int** mas, int countV, int i) { //счетчик степени вершины i
	int deg = 0;
	for (int j = 0; j < countV; j++)
		deg += mas[i][j]; //суммируем все единицы из строки i
	return deg; //возвращаем степень вершины i
}

void Connection(int** mas, bool*& visited, int i, int countV) { //проверка на связность графа
	if (visited[i]) return; //если вершина уже пройдена, выходим из функции
	visited[i] = true; //иначе отмечаем вершину пройденной
	for (int j = 0; j < countV; j++)
		if (mas[i][j]) Connection(mas, visited, j, countV); //отмечаем пройденными все возможные вершины
}

bool Most(int** mas, int countV, bool*& visited) { //проверка на ребро-мост
	for (int k = 0; k < countV; k++) visited[k] = false; //значение false означает, что вершина не пройдена
	Connection(mas, visited, 0, countV); //проходим все возможные вершины, начиная с первой
	for (int k = 0; k < countV; k++)
		if (!visited[k]) return false; //если встретилась непройденная вершина, значит, мы попали на ребро-мост
	return true;
}

bool Proverka_Cycle(int** mas, int countV) { //проверка на существование эйлерова цикла
	for (int i = 0; i < countV; i++) {
		int deg = DegV(mas, countV, i);
		if (deg % 2 != 0) return false; //если у вершины нечетная степень, то эйлерова цикла быть не может
	}
	return true; //если все условия ложны, то возможно сущетсвование эйлерова цикла
}

bool Proverka_Chain(int** mas, int countV) { //проверка на существование эйлеровой цепи
	int k = 0; //счетчик нечетных вершин
	for (int i = 0; i < countV; i++) {
		int deg = DegV(mas, countV, i);
		if (deg % 2 != 0) k++; //если у вершины нечетная степень, то увеличиваем k на 1
		if (k > 2) return false; //если вершин с нечетной степенью больше 2, то эйлеровой цепи быть не может
	}
	if (k == 2) return true; //если вершин с нечетной стпенью ровно 2, то возможно существование эйлеровой цепи
	else return false; //если  вершин с нечетной стпенью меньше 2, то эйлеровой цепи быть не может
}

void Poisk_Fleury(int** mas, int countV, int countE, bool*& visited, ofstream& f_out) { //построение цикла/цепи
	int k = 0, //счетчик пройденных ребер в графе
		i = 0, j = 0, //координаты ячейки матрицы смежности
		step = 0, //переменная для перемещения сверху вниз/снизу вверх по матрице смежности
		lastv = 0; //переменная для запоминания последней пройденной вершины

	if (Proverka_Cycle(mas, countV) == true) //если возможно наличие эйлерова цикла,
		f_out << "Cycle" << endl << i + 1; //записываем в файл "Cycle", и т.к. можно начать с любой вершины, начнем с первой

	else { //если эйлерова цикла точно быть не может
		if (Proverka_Chain(mas, countV) == false) { //если и эйлеровой цепи точно быть не может,
			f_out << "No";  return; //то записываем в файл "No", выходим из функции
		}
		//если возможно наличие эйлеровой цепи:
		int V1 = -1, V2 = -1; //номера вершин с нечетной степенью
		for (int m = 0; m < countV; m++)
			if (DegV(mas, countV, m) % 2 != 0) { //находим номера вершин с нечетной степенью
				V1 < 0 ? V1 = m : V2 = m;
				if (V1 > 0 && V2 > 0) break;
			}
		i = V1; f_out << "Chain" << endl << i + 1; //записываем в файл "Chain", и т.к. нужно начинать с вершины с нечетной стпенью, начнем с первой из них
		lastv = i; //запоминаем записанную вершину
	}
	while (k < countE) { //пока есть непройденные ребра
		if (mas[i][j] && lastv == i) //если сущетсвует ребро между i и j и последняя записанная вершина равна i
			if (DegV(mas, countV, i) == 1) { //если текущей вершине инцидентно всего одно ребро, то переходим по нему
				f_out << " " << j + 1; //записываем вершину, к которой пришли, в файл
				mas[i][j] = mas[j][i] = 0; //отмечаем ребро пройденным
				k++; i = lastv = j; //увеличиваем k на 1, запоминаем записанную вершину
			}
			else { //если текущей вершине инцидентно несколько ребер
				mas[i][j] = mas[j][i] = 0; //удаляем ребро для проверки, не является ли оно мостом
				if (Most(mas, countV, visited)) { //если ребро мостом не является
					f_out << " " << j + 1; //записываем вершину, к которой пришли, в файл
					k++; i = lastv = j; //увеличиваем k на 1, запоминаем записанную вершину
				}
				else { //если ребро может являться мостом
					mas[i][j] = mas[j][i] = 1; //возвращаем удаленное ребро
					if (!Most(mas, countV, visited)) { //граф несвязный, но не из-за возвращенного ребра
						f_out << " " << j + 1; //записываем вершину, к которой пришли, в файл
						mas[i][j] = mas[j][i] = 0; //отмечаем ребро пройденным
						k++; i = lastv = j; //увеличиваем k на 1, запоминаем записанную вершину
					}
				}
			}

		if (step == 0) //идем сверху вниз по матрице смежности
			if (j + 1 >= countV) //если мы находимся на последнем столбце
				if (i + 1 < countV) { i++; j = 0; } //если мы не находимся на последней строке, то переходим на следующую строку, на первый столбец
				else step = 1; //если мы дошли до конца матрицы, то начинаем проход по матрице снизу вверх
			else j++; //если мы не на последнем столбце, то переходим на следующий столбец

		else //идем снизу вверх по матрице смежности
			if (j - 1 < 0) //если мы находимся на первом столбце
				if (i - 1 >= 0) { i--; j = countV - 1; } //если мы не находимся на первой строке, то переходим на предыдущую строку, на последний столбец
				else step = 0; //если мы дошли до начала матрицы, то начинаем проход по матрице сверху вниз
			else j--; //если мы не на первом столбце, то переходим на предыдущий столбец
	}
}