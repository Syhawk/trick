#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <Windows.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

using namespace std;

void run(void* ptr) {
	char* p = (char*)ptr;
  cout << "Info :" << p << endl;

  DWORD tid = GetCurrentThreadId();
  DWORD pid = GetCurrentProcessId();

  cout << "tid = " << tid << ", pid = " << pid << endl;
}

int flg;
struct Node {
  vector<int>* ptr;
  int start;
  int length;
  int dest;
  int td;
  int index;
  int* flg_ptr;
};

void find(void* arg) {
  Node* ptr = (Node*)arg;
  for (int i = ptr->start; i < ptr->ptr->size() && i < ptr->start + ptr->length; ++i) {
    if (*(ptr->flg_ptr) != -1) {
      cout << "I'm fool to find value(" << ptr->dest << "), thread num is " << ptr->td << endl;
      return;
    }
    if (ptr->ptr->at(i) == ptr->dest) {
      *(ptr->flg_ptr) = ptr->td;
      ptr->index = i;
      cout << "dest value(" << ptr->dest << ") is found by thread " << ptr->td << " at index " << ptr->index << endl;
      return;
    }
  }
}

int main() {
/*
  char str[] = "Start";
	HANDLE hd = (HANDLE)_beginthread(run, 0, &str);
  WaitForSingleObject(hd, INFINITE);
	
  cout << "Main : " << endl;
  DWORD tid = GetCurrentThreadId();
  DWORD pid = GetCurrentProcessId();

  cout << "tid = " << tid << ", pid = " << pid << endl;
*/


  flg = -1;
  vector<int> nums;
  for (int i = 0; i < 1000; ++i) {
    nums.push_back(i);
  }

  for (int i = 0; i < 10; ++i) {
    Node node;
    node.ptr = &nums;
    node.start = i * 100;
    node.length = 100;
    node.dest = 783;
    node.td = i;
    node.index = -1;
    node.flg_ptr = &flg;

    _beginthread(find, 0, &node);
    Sleep(10);
    
  }

	system("pause");

  _endthread();

	return 0;
}
