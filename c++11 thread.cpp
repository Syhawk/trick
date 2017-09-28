#include <cstdio>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <stack>
#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <ctime>
#include <random>
#include <fstream>

using namespace std;


const int N = 9;
const int M = 10;

mutex mu;
int cnt;
int ans;
bool flg;
condition_variable cond;
queue<string> que;


void producer() {
	string str;
	while (getline(cin, str)) {
		unique_lock<mutex> locker(mu);
		que.push(str);
		cnt += 1;
		locker.unlock();
		cond.notify_all();
	}
	flg = true;
}

void cal(string& str) {
	for (int i = 0; i < str.size(); ++i) {
		if (str[i] == 'a') {
			ans += 1;
		}
	}
}

void consumer() {
	while (!flg) {
		unique_lock<mutex> locker(mu);
		cond.wait(locker, []() { return !que.empty() || flg; });
		if (que.empty()) {
			return;
		}
		string str = que.front();
		que.pop();
		locker.unlock();
		cal(str);
	}
}

int main() {
	freopen("in.txt", "r", stdin);

	cnt = 0;
	ans = 0;
	flg = false;
	thread t1(producer);
	vector<thread> ts;
	for (int i = 0; i < M; ++i) {
		ts.push_back(thread(consumer));
	}

	t1.join();
	for (int i = 0; i < M; ++i) {
		ts[i].join();
	}

	cout << ans << endl;

	return 0;
}
