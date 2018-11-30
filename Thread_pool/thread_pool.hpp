#pragma once 

#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <queue>

using namespace std;

typedef int (*cal_t)(int ,int);

class Task
{
  public:
    Task(int x,int y,cal_t handler_task):_x(x),_y(y),_handler_task(handler_task)
    {}
    
    ~Task()
    {}

    void Run()
    {
      _z = _handler_task(_x,_y);
    }

    void Show()
    {
      cout << "thread" << pthread_self() << " handler Task done, " << "Result is :" << _z << endl;
    }

  private:
    int _x;
    int _y;
    int _z;
    cal_t _handler_task;
};

class ThreadPool
{
  public:
    ThreadPool(int num):_thread_nums(num),is_stop(false)
    {}

    void InitThread()
    {
      pthread_mutex_init(&lock,NULL);
      pthread_cond_init(&cond,NULL);

      int i = 0;
      for(i=0; i<_thread_nums; i++)
      {
        pthread_t tid;
        pthread_create(&tid,NULL,thread_routine,(void*)this);
      }
    }

    ~ThreadPool()
    {
      pthread_mutex_destroy(&lock);
      pthread_cond_destroy(&cond);
    }

    void LockQueue()
    {
      pthread_mutex_lock(&lock);
    }

    void Unlock()
    {
      pthread_mutex_unlock(&lock);
    }

    bool Isempty()
    {
      return t_queue.size() == 0;
    }

    void IdealThread()
    {
      if(is_stop)
      {
        Unlock();
        _thread_nums--;
        pthread_exit((void*)0);
        cout << "pthread" << pthread_self() << "quit" << endl;
        return;
      }
      pthread_cond_wait(&cond,&lock);
    }

    void AddTask(Task& t)
    {
      LockQueue();
      if(is_stop)
      {
        Unlock();
      }
      t_queue.push(t); 
      NoticeOneThread();
      Unlock();
    }

    Task GetTask()
    {
      Task t = t_queue.front();
      t_queue.pop();
      return t;
    }

    void Stop()
    {
      LockQueue();
      is_stop = true;
      Unlock();
      while(_thread_nums > 0)
      {
        NoticeAllThread();
      }
    }

  private:
    void NoticeOneThread()
    {
      pthread_cond_signal(&cond);
    }

    void NoticeAllThread()
    {
      pthread_cond_broadcast(&cond);
    }

    static void* thread_routine(void* arg)
    {
      ThreadPool *tp = (ThreadPool*)arg;
      pthread_detach(pthread_self());
      while(1)
      {
        tp->LockQueue();
        while(tp->Isempty())
        {
          tp->IdealThread();
        }
        Task t = tp->GetTask();
        tp->Unlock();

        t.Run();
        t.Show();
      }
    }

  private:
    int _thread_nums;
    queue<Task> t_queue;
    bool is_stop;
    pthread_mutex_t lock;
    pthread_cond_t cond;
};
