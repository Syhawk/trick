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




/*
	chrono::steady_clock::time_point tp = chrono::steady_clock::now() + chrono::seconds(4);

	thread t1(factorial1, 6);
	this_thread::sleep_for(chrono::seconds(3));
	this_thread::sleep_until(tp);
	t1.join();

	unique_lock<mutex> locker(mu);
	//locker.try_lock_for(chrono::seconds(3));
	//locker.try_lock_until(tp);

	cond.wait_for(locker, chrono::seconds(3));
	cond.wait_until(locker, tp);

	promise<int> pro;
	future<int> fu = pro.get_future();
	fu.wait_for(chrono::seconds(3));
	fu.wait_until(tp);
*/

/*
	promise<int> pro;
	shared_future<int> sf = pro.get_future();
	//future<int> fu = pro.get_future();
	//shared_future<int> sf = fu.share();

	future<int> fu1 = async(launch::async | launch::deferred, factorial, sf);
	future<int> fu2 = async(launch::async | launch::deferred, factorial, sf);
	future<int> fu3 = async(launch::async | launch::deferred, factorial, sf);
	//future<int> fu = async(launch::async | launch::deferred, factorial, 4);
	
	pro.set_value(4);
	cout << fu1.get() << endl;
	cout << fu2.get() << endl;
*/

/*
	thread t1(thread_1);
	packaged_task<int()> t(bind(factorial1, 4));
	future<int> fu = t.get_future();
	{
		unique_lock<mutex> locker(mu);
		task_que.push_front(move(t));
	}
	cond.notify_one();
	cout << fu.get() << endl;
	t1.join();
*/
