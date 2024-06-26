#include "syscall.h"

void merge(int a[], int left, int mid, int right){
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
	int a[50], n, i, c;
	char* filename;
	int fileID;
	PrintString("Enter the length of array of float: \n");
	n = ReadInt();
	PrintString("Enter all element of array: \n");
	for (i = 0; i < n; i++)
	{
		a[i] = ReadFloat();
	}
	
	mergeSort(a, 0, n-1);

	filename = "mergesort.txt";
	c = Create(filename);
	if(c == -1){
		PrintString("Can't create file mergesort.txt");
		Halt();
	}

	fileID = Open(filename, 0);
	if (fileID == -1)
	{
		PrintString("\nFAILED: Can't open file ");
		PrintString(filename);
	}
	
	else
	{
		for (i = 0; i < n; i++) {
		    WriteFloat(a[i], fileID);
		    Write(" ", 1, fileID); // Separate the integers by spaces
		}
		// Always remember to close the opened file.
		CloseFile(fileID);
	}
	
	PrintString("The result is in file mergesort.txt\n");
	
	Halt();

	return 0;

}

