//Sorting Program - Christopher Haynes - 10748982
#include <Windows.h>
#include <conio.h>
#include <iostream>
#include <fstream>	//Used for file operations
#include <time.h>	//Used for timing operations and prng seeding
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code)&0x8000) ? 1 : 0)	//Definition of a key being pressed
using namespace std;

void Menu(void);					//Menu function, handles all user input
void SelectionSort(void);			//Performs a selection sort
void InsertionSort(void);			//Performs an insertion sort
void BubbleSort(void);				//Performs a bubble sort
void MergeDivide(int, int);			//Divides an array up as the recursive step in a merge sort
void MergeMerge(int, int, int);		//Recombines the divided array to finish the merge sort
void QuickSort(int, int);			//Orders data around a pivot value, recursive step in quick sort
void QuickDivide(int, int, int);	//Divides the semi-sorted array to be quick sorted again
void FileToArray(int);				//Reads data values from a file and stores them into an array
void ResetArray(void);				//Set the b[] array to equal the a[] array
void RandomSetter(int);				//Set the array values randomly up to value of 100,000
void ShowArray(void);				//Print the b[] array to the screen
void gotoxy(int, int);				//Go to the equivalent x,y co-ordinate on the console screen
void clrscr(void);					//Clear the console screen

float totalTime;	//The overall time taken, on average, to complete a sort
float resetTime;	//The overall time taken to reset the array accross all repititions (used for more accurate timing)
DWORD startTime, endTime, startResetTime, endResetTime;	//Used to record the tick counts for timining measurements
int repeats = 100;	//The amount of times a sort will be repeated (used for accurate timing with small data set sizes)

int i, j, k;							//used for iterations (i and j used in within functions) (k used for repeating sorting functions for accurate timing)
int a[100000], b[100000], c[100000];	//a[] array contains the original raw unsorted data, b[] and c[] arrays are used for sorting and storing the sorted data
int dataLength;							//The amount of items in the data set (used to set ranges for the sorts and to reset the arrays)
bool showSort = 0;						//Holds the users selection of whether to print array values to the screen
int sortType = 0;						//Holds the users selection of which sort to perform (1 - selection, 2 - insertion, 3 - bubble, 4 - merge, 5 - quick)

//-----------------------------------------------------------------------------
// Name: Menu()
// Desc: Handle the user inputs and store them for later use.
//-----------------------------------------------------------------------------
void Menu(void)
{
	int menuChoice = 0;	//Variable to hold the users input for the current menu item

	while (menuChoice == 0) { //If the user gives an invalid input; the menuChoice is set back to 0 and the menu option loops

		cout << "Press 1 for Selection Sort\n" << "Press 2 for Insertion Sort\n" << "Press 3 for Bubble Sort\n" 
			 << "Press 4 for Merge Sort\n" << "Press 5 for Quick Sort\n";	//Print the user options
		cin >> menuChoice;													//Record the user input

		if (menuChoice == 1) { sortType = 1; }			//Set the sortType to the matching user choice
		else if (menuChoice == 2) { sortType = 2; }
		else if (menuChoice == 3) { sortType = 3; }
		else if (menuChoice == 4) { sortType = 4; }
		else if (menuChoice == 5) { sortType = 5; }
		else { menuChoice = 0; cout << "\nINVALID INPUT\nPress any key to try again"; _getch(); clrscr(); }
	}	//If an invalid input is given; menuChoice is reset to 0, keeping the user in the current while loop

	clrscr();		//Clear the screen for the next menu option
	menuChoice = 0;	//Set menuChoice back to 0 for the next menu option

	while (menuChoice == 0) {
		
		cout << "Press 1 for 10 random numbers\n" << "Press 2 for 100 random numbers\n" << "Press 3 for 1000 random numbers\n" 
			 << "Press 4 for 10,000 random numbers\n" << "Press 5 for 100,000 random numbers\n" << "Press 6 for '10.dat' numbers\n"
			 << "Press 7 for '100.dat' numbers\n" << "Press 8 for '1000.dat' numbers\n" << "Press 9 for '10,000.dat' numbers\n";
		cin >> menuChoice;

		repeats = 100;	//By default set the number of repeats to be performed to 100
		if (menuChoice == 1) { dataLength = 10; repeats = 100000; RandomSetter(10); }			//The data length is set to corresepond to the users choice,
		else if (menuChoice == 2) { dataLength = 100; repeats = 10000; RandomSetter(100); }		//the number of repeats is set appropriately considering the data length
		else if (menuChoice == 3) { dataLength = 1000; RandomSetter(1000); }					//(smaller samples need more repeats for higher accuracy), and
		else if (menuChoice == 4) { dataLength = 10000; RandomSetter(10000); }					//the array is populated either using the random array setter or 
		else if (menuChoice == 5) { dataLength = 100000; RandomSetter(100000); }				//by reading the data values from the chosen file.
		else if (menuChoice == 6) { dataLength = 10; repeats = 100000; FileToArray(1); }
		else if (menuChoice == 7) { dataLength = 100; repeats = 10000; FileToArray(2); }
		else if (menuChoice == 8) { dataLength = 1000; FileToArray(3);}
		else if (menuChoice == 9) { dataLength = 10000; FileToArray(4);}
		else { menuChoice = 0; cout << "\nINVALID INPUT\nPress any key to try again"; _getch(); clrscr(); }
	}

	clrscr();		//Clear the screen for the next menu option
	menuChoice = 0;	//Set menuChoice back to 0 for the next menu option

	while (menuChoice == 0) {
	
		cout << "Press 1 to print array values and sort times\n" << "Press 2 to only print sort times\n";
		cin >> menuChoice;

		if (menuChoice == 1) { showSort = 1; }
		else if (menuChoice == 2) { showSort = 0; }
		else { menuChoice = 0; cout << "\nINVALID INPUT\nPress any key to try again"; _getch(); clrscr(); }
	}
	
	clrscr(); //Clear the screen before continuing
}

void main(void) 
{
	while (!KEY_DOWN(VK_ESCAPE)) { //The escape key is used to exit the program (whilst it is not in the menu, or sorting)

		Menu(); //Call the menu to take the user input

		if (showSort == 1) { cout << "Inital Data Order : \n"; ShowArray(); }	//If the user chose to show the array values; print the unsorted array data

		startTime = GetTickCount();

		//Each of the following "if" statements are for one sort. Each starts by getting the tickcount for the start time, then repeating the 
		//given sort "repeats" number of times, resetting the array back to the unsorted version between each sort. Once it has finished the end time tickcount is recorded
		if (sortType == 1) { for (k = 0; k < repeats; k++) { ResetArray(); SelectionSort(); } endTime = GetTickCount(); }
		if (sortType == 2) { for (k = 0; k < repeats; k++) { ResetArray(); InsertionSort(); } endTime = GetTickCount();}
		if (sortType == 3) { for (k = 0; k < repeats; k++) { ResetArray(); BubbleSort(); } endTime = GetTickCount(); }
		if (sortType == 4) { for (k = 0; k < repeats; k++) { ResetArray(); MergeDivide(0, dataLength - 1); } endTime = GetTickCount(); }
		if (sortType == 5) { for (k = 0; k < repeats; k++) { ResetArray(); QuickSort(0, dataLength - 1); } endTime = GetTickCount(); }

		if (showSort == 1) { cout << "Sorted Data Order : \n"; ShowArray(); }	//If the user chose to show the array values; print the sorted array data

		startResetTime = GetTickCount(); ResetArray(); endResetTime = GetTickCount();		//Calculate how long it takes to run the ResetArray() Function
		resetTime = (((float)endResetTime - (float)startResetTime) / 1000) * repeats;		//Set the overall resetTime to the be (time taken for reset) * (repeats)
																							//That value is then divided by 1000 to put it into seconds
		totalTime = ((((float)endTime - (float)startTime) / 1000)/ repeats) - resetTime;	//totalTime is then defined as the time taken to run the sort ("repeats" times)
																							//divided by 1000 to put it into seconds, and then divided by "repeats" to give an mean completion time
		cout << "For " << dataLength << " items of data, sort took " << totalTime << " seconds.\n\n";	//Print the data length and time taken to the screen

		cout << "Press ESC to Exit, any other key to continue\n\n";
		_getch(); clrscr();
	}
}

//-----------------------------------------------------------------------------
// Name: SelectionSort()
// Desc: Perform a selection sort
//-----------------------------------------------------------------------------
void SelectionSort(void)
{
	int min, minIndex;	//min holds the current minimum value, minIndex holds the array position of the current minimum value

	for (j = 0; j < dataLength; j++) { //Outer loop for checking every value in the array

		min = b[j];		//Set the minimum value to the value in the first array position
		minIndex = j;	//Set the index to match the location of the current minimum value

		for (i = j + 1; i < dataLength; i++) {	//Inner loop for comparing every value above the current starting point

			if (b[i] < min) {	//If a value is found that is lower than the stored the minimum
				min = b[i];		//Set that new value as the current minimum
				minIndex = i;	//Update the index to match the location of the new minimum value
			}
		}
								//Once the whole unsorted section of the array has been checked
		b[minIndex] = b[j];		//Move the first unsorted value to the location the minimum value was discovered
		b[j] = min;				//Set the first unsorted position to be the discovered minimum value
	}
}

//-----------------------------------------------------------------------------
// Name: InsertionSort()
// Desc: Perform an insertion sort
//-----------------------------------------------------------------------------
void InsertionSort(void) 
{
	int numHold; //Used to temporarily hold a value during a swap

	for (i = 1; i < dataLength; i++) { //Loop from the second position in the array to the last
	
		j = i; //Set j to equal i (so that j can be modified without affecting the outer loop)

		while (b[j - 1] > b[j] && j >= 1) {	//If the previous value in the array is higher than the current value
											//and the position being checked is not the first position in the array.
			numHold = b[j - 1];		//Temporarily store the number below 
			b[j - 1] = b[j];		//Set the position below to the current number 	
			b[j] = numHold;			//Set the current position to the held number 
			j--;					//Decrease j by 1 so that the next check still follows the same value through the array.
		}
	}
}

//-----------------------------------------------------------------------------
// Name: BubbleSort()
// Desc: Perform a bubble sort
//-----------------------------------------------------------------------------
void BubbleSort(void) 
{
	int numHold, passCount = -1, flag = 1;	//numHold for temporary number storage for swapping
											//passCount for tracking how many times the full data range has been examined
											//flag is 1 when the list is still unsorted, and 0 when the list is sorted
	while (flag == 1) {	//Keep running until a pass completes without swapping any numbers
	
		flag = 0;		//Set the flag to 0 (it will remain 0 as long as no numbers are swapped)
		passCount++;	//Increment the passCount so it accuratly represents the current number of passes

		for (i = dataLength - 1; i > 0 + passCount; i--) {	//Loop runs from the last position in the array to the first unsorted position
															//The loop gets 1 smaller each run, as one value is guarenteed to be sorted
			if (b[i] < b[i - 1]) {		//If the current position is less than the position below it
			
				numHold = b[i];			//Swap the numbers
				b[i] = b[i - 1];
				b[i - 1] = numHold;
				flag = 1;				//Set the flag to 1 to represent that the list is not yet sorted
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Name: MergeDivide()
// Desc: Recursive function for dividing an array into it's smallest unit
//-----------------------------------------------------------------------------
void MergeDivide(int start, int end)	//start is the first position in the array and end is the last position in the array
{										//Both start and end are inclusive
	if (end - start >= 1) { //If the array being divided has a range of at least 2.

		int mid = (end + start) / 2;	//The midpoint is the centre value between the start and end (also used inclusivley)
										//Using an integer "floors" the value, for example; (3 + 0)/2 = 1 (NOT 1.5 or 2)
		MergeDivide(start, mid);		//Call the divide function again for the first half of the array
		MergeDivide(mid + 1, end);		//Call the divide function again for the last half of the array

		MergeMerge(start, mid, end);	//Merge the two half arrays back together
	}
}

//-----------------------------------------------------------------------------
// Name: MergeMerge()
// Desc: Merges two sorted arrays of size n into one sorted array of size 2n
//-----------------------------------------------------------------------------
void MergeMerge(int start, int mid, int end)	//start is the first position in the array, mid is the mid point and end is the last position in the array
{												//and end is the last position in the array.
	int lowCount = start;		//Position of first (unsorted) index in the "low" set
	int highCount = mid + 1;	//Position of first (unsorted) index in the "high" set

	for (int mainCount = start; mainCount < end + 1; mainCount++) {				//Loop to go through all positions of both sets
	
		if (highCount > end) { c[mainCount] = b[lowCount]; lowCount++; }		//If highCount is greater than end, then all the values from the "high" set
																				//have already been sorted, so use the next "low" set value and increment the count.
		else if (lowCount > mid){ c[mainCount] = b[highCount]; highCount++; }	//If lowCount is greater than mid, then all the values from the "low" set
																				//have already been sorted, so use the next "high" set value and increment the count.
		else{
			if (b[lowCount] <= b[highCount]) {	//If the next available value in the "low" set is less than or equal to the next available value in the "high" set.
												//("Less than or equal to" is used to keep the sort stable for repeated values)
				c[mainCount] = b[lowCount];		//Store the "low" set value into the work array
				lowCount++;						//Increment lowCount so that the next index position in the "low" set is the one being examined.
			}
			else {								//Else case covers when the value in the "high" set is less than the value in the "low" set.
	
				c[mainCount] = b[highCount];	//Store the "high" set value into the work array
				highCount++;					//Increment highCount so that the next index position in the "high" set is the one being examined.
			}
		}
	}
	for (int copyCount = start; copyCount < end + 1; copyCount++) { //Loop to cover all positions in both sets
	
		b[copyCount] = c[copyCount];	//Now that the given sets have been sorted, the sorted values from the work array can be copied back to the main array.
	}
}

//-----------------------------------------------------------------------------
// Name: QuickSort()
// Desc: Sorts the "pivot" value and places all values lower than it lower
//		 in the array, and all values higher than it higher in the array
//-----------------------------------------------------------------------------
void QuickSort(int min, int max) 
{
	int pivot = max;	//Set the pivot to be the index of the last position in the array
	int numHold;		//Used as a temporary storage when swapping numbers

	if (min < max) {	//Only sort if there is at least two elements in the range (breaks out of recursion)

		for (i = min; i < pivot; i++) {	//Loop running from start of the range up to the current pivot position

			if (b[i] > b[pivot]) {		//If a value lower in the array than the pivot has a higher value than it

				if (i == pivot - 1) {	//If the array position being checked is directly below the pivot

					numHold = b[i];		//Swap the numbers
					b[i] = b[pivot];
					b[pivot] = numHold;
					pivot--;
				}
				else {					//If the array position being checked in anywhere else below the pivot

					numHold = b[pivot - 1];		//Store the value from the position below the pivot
					b[pivot - 1] = b[pivot];	//Copy the pivot value to that position
					b[pivot] = b[i];			//Set the pivot position to the value being checked
					b[i] = numHold;				//Move the stored number to the position of the value being checked
					pivot--;					//Reduce the pivot index by one so it still points to the pivot value
					i--;						//Reduce the loop counter by one as there is now one more value 												
				}								//below the pivot which needs to be checked
			}
		}

		QuickDivide(min, pivot, max); //Call the QuickDivide function to create two subranges either side of the pivot
	}
}

//-----------------------------------------------------------------------------
// Name: QuickDivide()
// Desc: Define two new subranges either side of a pivot and call QuickSort for each
//-----------------------------------------------------------------------------
void QuickDivide(int min, int pivot, int max) 
{
	QuickSort(min, pivot - 1);	//Sort the range below the pivot 
	QuickSort(pivot + 1, max);	//Sort the range above the pivot
}

//-----------------------------------------------------------------------------
// Name: FileToArray()
// Desc: Copies values stored in a file into an array
//-----------------------------------------------------------------------------
void FileToArray(int fileChoice) 
{
	ifstream data; //Data type for handling file operations
	if (fileChoice == 1) { data.open("10.dat"); }		//Based on the users input, the corresponding 
	if (fileChoice == 2) { data.open("100.dat"); }		//file is opened and stored within "data"
	if (fileChoice == 3) { data.open("1000.dat"); }
	if (fileChoice == 4) { data.open("10000.dat"); }

	int index = 0;
	while (!data.eof())		//Whilst the end of the file has not been reached
	{
		data >> a[index];	//The next value in the file is stored at position "index" in the a[] array
		index++;			//Increment "index" to point at the next empty space in the array
	}

	data.close();	//Once file operations have been completed, close the file
	ResetArray();	//Copy the a[] array (raw data array) into the b[] array (sorting array)
}

//-----------------------------------------------------------------------------
// Name: ResetArray()
// Desc: Copies the a[] array (raw data array) into the b[] array (sorting array)
//-----------------------------------------------------------------------------
void ResetArray(void) 
{
	for (i = 0; i < dataLength; i++) { //Loop for every index up to the user defined data set size

		b[i] = a[i]; //Copy the value in the a[] array into the same position in the b[] array
	}
}

//-----------------------------------------------------------------------------
// Name: RandomSetter()
// Desc: Generates random numbers between 0 and 9999, up to a range of "size"
//-----------------------------------------------------------------------------
void RandomSetter(int size) 
{
	srand (time(NULL));	//Seed the pseudo random number generator

	for (i = 0; i < size; i++) { //Loop covering every index up to the user defined "size"

		a[i] = rand() % 10000;	//Set the a[] array to a random value between 0 and 9999
		b[i] = a[i];			//Copt the value in the a[] array into the same position in the b[] array
	}
}

//-----------------------------------------------------------------------------
// Name: ShowArray()
// Desc: Print the b[] array to the screen
//-----------------------------------------------------------------------------
void ShowArray(void)
{
	for (int i = 0; i < dataLength; i++) { //Loop to run through every item defined in the array (array will never have more items than dataLength)

		cout << i << "= " << b[i] << "\n";	//Print the array position, followed by its value
	}
	cout << "\n\n";
}

//-----------------------------------------------------------------------------
// Name: gotoxy()
// Desc: Go to the provided x, y co-ordinates on the console screen
//-----------------------------------------------------------------------------
void gotoxy(int x, int y)       
{
	static HANDLE hStdout = NULL;
	COORD coord;

	coord.X = x;
	coord.Y = y;
	if (!hStdout) { hStdout = GetStdHandle(STD_OUTPUT_HANDLE); }
	SetConsoleCursorPosition(hStdout, coord);
}

//-----------------------------------------------------------------------------
// Name: clrscr()
// Desc: Clear the console screen
//-----------------------------------------------------------------------------
void clrscr(void)
{
	static HANDLE hStdout = NULL;
	static CONSOLE_SCREEN_BUFFER_INFO csbi;
	const COORD startCoords = { 0,0 };
	DWORD dummy;

	if (!hStdout)
	{
		hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(hStdout, &csbi);
	}
	FillConsoleOutputCharacter(hStdout, ' ', csbi.dwSize.X * csbi.dwSize.Y, startCoords, &dummy); //this should be all one line
	gotoxy(0, 0);
}