#include "thread_pool.hpp"

const int num = 5;

int Add(int x,int y)
{
  return x + y;
}

int main()
{
  ThreadPool* tp = new ThreadPool(num);
  tp->InitThread();
  int nums = 1;
  while(1)
  {
    sleep(1);
    Task t(nums,nums-1,Add);
    tp->AddTask(t);
    nums++;
  }
  return 0;
}
