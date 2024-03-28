#include "syscall.h"

int getFileLength(int fileID)
{
    int value = Seek(-1, fileID);
    Seek(0, fileID);
    return value;
}
int string_length(const char *str) {
    int length = 0;
    while (*str != '\0') {
        ++length;
        ++str;
    }
    return length;
}

void intToString(int value, char* str) {
	int i = 0;
	int isNegative = 0;
	int start, end;
	char temp;

	if (value == 0) {
	str[i++] = '0';
	str[i] = '\0';
	return;
	}

	if (value < 0) {
	isNegative = 1;
	value = -value;
	}

	// Fill in the string from the end
	while (value != 0) {
	int rem = value % 10;
	str[i++] = (rem > 9)? (rem-10) + 'a' : rem + '0';
	value = value / 10;
	}

	if (isNegative) {
	str[i++] = '-';
	}

	str[i] = '\0'; // Null-terminate string

	// Reverse the string
	start = 0;
	end = i - 1;
	while (start < end) {
	temp = str[start];
	str[start] = str[end];
	str[end] = temp;
	start++;
	end--;
	}
}

void merge(int a[],  int left, int mid, int right){
	int L[50];
	int R[50];
	int nL, nR;
	int i, j; // index of subArray
	nL = mid - left + 1; // tu left den mid
	nR = right - mid; // tu mid + 1 den right

	//copy subarray
	for(i = 0; i < nL;i++)
		L[i] = a[left + i];
	for(j = 0; j < nR; j++)
		R[j] = a[mid + 1 + j];
	//merge two subArrays to main Array
	i = j = 0;
	while(i < nL && j < nR){
		if(L[i] < R[j])
			a[left++] = L[i++];
		else
			a[left++] = R[j++];
	}
	
	while(i < nL){
		a[left++] = L[i++];
	}
	while(j < nR){
		a[left++] = R[j++];
	}
}

void mergeSort(int a[], int left, int right){
	if(left < right){
		int mid = (left + right) / 2;
		mergeSort(a, left, mid);
		mergeSort(a, mid + 1, right);
		merge(a, left, mid, right);
	}
}


int main() {
	int a[100];
	int n;
	int i, j;
	char* filename;
	int fileID;
	int len;
	PrintString("Enter the length of array of integer: \n");
	n = ReadInt();
	PrintString("Enter all element of array: \n");
	for (i = 0; i < n; i++)
	{
		a[i] = ReadInt();
	}

	mergeSort(a, 0, n-1);

	filename = "mergesort.txt";
	Create(filename);
	fileID = Open(filename, 0);

	if (fileID == -1)
	{
		PrintString("\nFAILED: Can't open file ");
		PrintString(filename);
	}
	
	else
	{
		char buffer[50];
		for (i = 0; i < n; i++) {
		    intToString(a[i], buffer);
		    len = string_length(buffer);
		    // Write the string representation of the integer followed by a space or newline
		    Write(buffer, len, fileID);
		    Write(" ", 1, fileID); // Separate the integers by spaces
		}
		// Always remember to close the opened file.
		CloseFile(fileID);
	}
	
	PrintString("The result is in file mergesort.txt\n");
	
	Halt();

	return 0;

}

