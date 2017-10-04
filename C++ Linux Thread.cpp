/*
g++ main.cpp -lpthread -o main

man pthreads

*/



#include <iostream>
#include <pthread.h>
#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>
#include <asm-generic/errno-base.h>

using namespace std;

// ******************************************
void* thread_fun(void* arg) {
	string* str = (string*)arg;
	if (*str == "1") {
		printf("return quit\n");
		return (void*)1;
	}
	if (*str == "2") {
		cout << "pthread_exit quit" << endl;
		pthread_exit((void*)2);
	}
	if (*str == "3") {
		cout << "exit\n" << endl;
		exit(0);
	}
}

// ******************************************
void* thread_fun1(void* arg) {
    cout << "thread 1" << endl;
    return (void*)1;
}

void* thread_fun2(void* arg) {
    cout << "thread 2" << endl;

    pthread_detach(pthread_self());
    pthread_exit((void*)2);
}

// ******************************************
void* thread_cancel(void* arg) {
    int state_val;
    state_val = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    if (state_val != 0) {
        cout << "set cancel state failed" << endl;
    }
    cout << "new thread" << endl;
    sleep(4);
    
    cout << "ready to cancel" << endl;
    state_val = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    if (state_val != 0) {
        cout << "set cancel state failure" << endl;
    }
    int type_val;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    if (type_val != 0) {
        cout << "set cancel type error" << endl;
    }

    printf("first cancel point\n");
    printf("second cancel point\n");
    
    return (void*)2;
}

// ******************************************
void* thread_kill(void* arg) {
//    sleep(1);
    cout << "new thread" << endl;
    return (void*)0;
}


// ******************************************
void signal1(int arg) {
    cout << "signal one" << endl;
}

void signal2(int arg) {
    cout << "signal two" << endl;
}

void* thread_signal1(void* arg) {
    cout << "new thread one" << endl;
    
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_handler = signal1;
    
    sigaction(SIGQUIT, &act, NULL);
    
    pthread_sigmask(SIG_BLOCK, &act.sa_mask, NULL);
    
    sleep(2);
}

void* thread_signal2(void* arg) {
    cout << "new thread two" << endl;
    
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    
    sigaddset(&act.sa_mask, SIGQUIT);
    act.sa_handler = signal2;
    
    sigaction(SIGQUIT, &act, NULL);
    
    pthread_sigmask(SIG_BLOCK, &act.sa_mask, NULL);
    
    sleep(2);
}


// ******************************************
void clean_first(void* arg) {
    cout << (char*)arg << " clean first\n";
}

void clean_second(void* arg) {
    cout << (char*)arg << " clean second\n";
}

void* thread_clean1(void* arg) {
    cout << "new thread 1\n";
    
    pthread_cleanup_push(clean_first, (void*)"thread1");
    pthread_cleanup_push(clean_second, (void*)"thread1");

    pthread_cleanup_pop(0);
    pthread_cleanup_pop(1);
}

void* thread_clean2(void* arg) {
    cout << "new thread 2\n";

    pthread_cleanup_push(clean_first, (void*)"thread2");
    pthread_cleanup_push(clean_second, (void*)"thread2");

//    pthread_exit((void*)2);
    pthread_cleanup_pop(1);
    pthread_cleanup_pop(2);
    pthread_exit((void*)2);
   
}


// ******************************************
pthread_rwlock_t rwlock;
int num;
void* thread_rwlock1(void* arg) {
    int err;
    pthread_rwlock_rdlock(&rwlock);
//    pthread_rwlock_wrlock(&rwlock);
    printf("thread 1 print num %d \n", num);
    sleep(2);
    cout << "thread 1 end\n";
    
    pthread_rwlock_unlock(&rwlock);
}
void* thread_rwlock2(void* arg) {
    int err;
//    pthread_rwlock_rdlock(&rwlock);
    pthread_rwlock_wrlock(&rwlock);
    printf("thread 2 print num %d \n", num);
    sleep(2);
    cout << "thread 2 end\n";
    
    pthread_rwlock_unlock(&rwlock);
}


// ******************************************
#define BUFFER_SIZE 5
#define PRODUCT_CNT 50

struct ProductCons {
    vector<int> buffer;
    pthread_mutex_t lock;
    int read_pos;
    int write_pos;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    ProductCons() : buffer(vector<int>(BUFFER_SIZE, 0)), read_pos(0), write_pos(0) {
        pthread_mutex_init(&lock, NULL);
        pthread_cond_init(&not_empty, NULL);
        pthread_cond_init(&not_full, NULL);
    }
};

struct ProductCons buffer;

void finish(struct ProductCons* p) {
    p->buffer.clear();
    p->read_pos = 0;
    p->write_pos = 0;
    pthread_mutex_destroy(&(p->lock));
    pthread_cond_destroy(&(p->not_empty));
    pthread_cond_destroy(&(p->not_full));
}

void put(struct ProductCons* p, int data) {
    pthread_mutex_lock(&(p->lock));
    if ((p->write_pos + 1) % BUFFER_SIZE == p->read_pos) {
        cout << "producer wait for not full.\n";
        pthread_cond_wait(&(p->not_full), &(p->lock));
    }
    
    p->buffer[p->write_pos] = data;
    p->write_pos = (p->write_pos + 1) % BUFFER_SIZE;
    
    pthread_cond_signal(&(p->not_empty));
    pthread_mutex_unlock(&(p->lock));
}

int get(struct ProductCons* p) {
    int data;
    
    pthread_mutex_lock(&(p->lock));
    
    if (p->read_pos == p->write_pos) {
        cout << "consumer wait for not empty\n";
        pthread_cond_wait(&(p->not_empty), &(p->lock));
    }
    
    data = p->buffer[p->read_pos];
    p->read_pos = (p->read_pos + 1) % BUFFER_SIZE;
    
    pthread_cond_signal(&(p->not_full));
    pthread_mutex_unlock(&(p->lock));
    
    return data;
}

void* producer(void* arg) {
    for (int i = 0; i <= PRODUCT_CNT; ++i) {
        sleep(1);
        printf("put the %d product...\n", i);
        put(&buffer, i);
        printf("put the %d product success\n", i);
    }
    cout << "producer stopped\n";
}

void* consumer(void* arg) {
    int cnt = 0;
    while (cnt < PRODUCT_CNT) {
        sleep(2);
        cout << "get product...\n";
        int val = get(&buffer);
        printf("get the %d product success\n", val);
        ++cnt;
    }
    
    cout << "consumer stopped\n";
}


// ******************************************

int main(int argc, char* argv[]) {

/*
// ******************************************
	int err;
	pthread_t tid;
	string str = (char*)argv[1];

	err = pthread_create(&tid, NULL, thread_fun, (void*)(&str));
	if (err != 0) {
		printf("create new thread error.");
		return 0;
	}
	usleep(1);
*/

/*
// ******************************************
    int err1;
    int err2;
    pthread_t tid1;
    pthread_t tid2;
    void* rval1;
    void* rval2;
    
    err1 = pthread_create(&tid1, NULL, thread_fun1, NULL);
    usleep(10);
    err2 = pthread_create(&tid2, NULL, thread_fun2, NULL);
    usleep(10);
    
    if (err1 || err2) {
        cout << "new thread error." << endl;
        return 0;
    } else {
        cout << "new thread successful" << endl;
    }
    
    cout << "return code join1 is " << pthread_join(tid1, &rval1) << endl;
    cout << "return code join2 is " << pthread_join(tid2, &rval2) << endl;
    
    cout << "thread 1 exit code is " << (int*)rval1 << endl;
    cout << "thread 2 eixt code is " << (int*)rval2 << endl;
    cout << "main function" << endl;
*/

/*
// ******************************************
    pthread_t tid;
    int err;
    int cval;
    int jval;
    void* rval;
    
    err = pthread_create(&tid, NULL, thread_cancel, NULL);
    if (err != 0) {
        cout << "new thread error" << endl;
        return 0;
    }
    
    sleep(2);
    cval = pthread_cancel(tid);
    if (cval != 0) {
        cout << "pthread_cancel tid error" << endl;
    }
    jval = pthread_join(tid, &rval);
    cout << "thread join code is " << jval << endl;
    cout << "thread exit code is " << (int*)rval << endl;
*/

/*
// ******************************************
    pthread_t tid;
    int err;
    int s;
    void* rval;
pthread
    err = pthread_create(&tid, NULL, thread_kill, NULL);
    if (err != 0) {
        printf("create new thread error.\n");
        return 0;
    }
//    sleep(1);

    s = pthread_kill(tid, 0);
    if (s == ESRCH) {
        cout << "thread id is not found." << endl;
    }

    pthread_join(tid, &rval);
    cout << "main thread" << endl;
*/

/*
// ******************************************
    pthread_t tid1;
    pthread_t tid2;
    int err1;
    int err2;
    int s;
    
    err1 = pthread_create(&tid1, NULL, thread_signal1, NULL);
    usleep(1);
    err2 = pthread_create(&tid2, NULL, thread_signal2, NULL);
    if (err1 || err2) {
        cout << "create thread one or two error" << endl;
        return 0;
    }
    
    sleep(1);
    
    s = pthread_kill(tid1, SIGQUIT);
    if (s != 0) {
        cout << "send signal to thread1 failure" << endl;
    }
    s = pthread_kill(tid2, SIGQUIT);
    if (s != 0) {
        cout << "send signal to thread2 failure" << endl;
    }
    
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
*/

/*
// ******************************************
    pthread_t tid1;
    pthread_t tid2;
    int err1;
    int err2;
    err1 = pthread_create(&tid1, NULL, thread_clean1, NULL);
    err2 = pthread_create(&tid2, NULL, thread_clean2, NULL);
    
    if (err1 || err2) {
        cout << "create thread error." << endl;
        return 0;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
*/   


/*
// ******************************************
    pthread_t tid1;
    pthread_t tid2;
    int err1;
    int err2;
    int err_mutex;

    num = 0;
    err_mutex = pthread_rwlock_init(&rwlock, NULL);
    if (err_mutex) {
        cout << "init rw_mutex failure\n";
        return 0;
    }
    
    err1 = pthread_create(&tid1, NULL, thread_rwlock1, NULL);
    err2 = pthread_create(&tid2, NULL, thread_rwlock2, NULL);
    if (err1 || err2) {
        cout << "create thread failure\n";
        return 0;
    }
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    pthread_rwlock_destroy(&rwlock);
*/

/*
// ******************************************
    pthread_t tid1;
    pthread_t tid2;
    
    pthread_create(&tid1, NULL, producer, NULL);
    pthread_create(&tid2, NULL, consumer, NULL);
    
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    
    finish(&buffer);
*/

// ******************************************
    
	return 0;
}
