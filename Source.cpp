#include <iostream>
#include <Windows.h>
#include <thread>
#include <queue>
#include <mutex>
using namespace std;

HANDLE m_hSemaphore;

void takeTask2(queue<int> &Queue1, queue<int> &Queue2, mutex &mSleep1, mutex &mQueue1, mutex &mQueue2, mutex &mCPU, mutex &fullQueue)
{
	for (int i = 0; i < 3; i++)
	{
		WaitForSingleObject(m_hSemaphore, INFINITE);
		int sleepTime = 1000;
		if (!Queue1.empty())
		{
			mQueue1.lock();
			sleepTime = Queue1.front();
			Queue1.pop();
			mQueue1.unlock();
			mSleep1.lock();
			cout << "CPU2:sleep Time from Queue1 " << sleepTime << endl;
			Sleep(sleepTime);
			mSleep1.unlock();
		}
		else if (!Queue2.empty())
		{
			mQueue2.lock();
			sleepTime = Queue2.front();
			Queue2.pop();
			mQueue2.unlock();
			mSleep1.lock();
			cout << "CPU2:sleep Time from Queue2 " << sleepTime << endl;
			Sleep(sleepTime);
			mSleep1.unlock();
		}
		else
		{
			mSleep1.lock();
			cout << "CPU2:wait 1 sec\n";
			Sleep(sleepTime);
			mSleep1.unlock();
		}
	}
}


void takeTask(queue<int> &Queue1, queue<int> &Queue2, mutex &mSleep1, mutex &mQueue1, mutex &mQueue2, mutex &mCPU, mutex &fullQueue)
{
	for (int i = 0; i < 4; i++)
	{
		int sleepTime = 3000;
		if (!Queue1.empty())
		{
			mQueue1.lock();
			sleepTime = Queue1.front();
			Queue1.pop();
			mQueue1.unlock();
			mSleep1.lock();
			cout << "CPU1:sleep Time from Queue1 " << sleepTime<<endl;
			if ( (Queue1.size() == 2) || (Queue2.size() == 2) )  
				ReleaseSemaphore(m_hSemaphore, 1, NULL);
			Sleep(sleepTime);
			if ((Queue1.size() == 2) || (Queue2.size() == 2))
				ReleaseSemaphore(m_hSemaphore, 1, NULL);
			mSleep1.unlock();
		}
		else if (!Queue2.empty())
		{
			mQueue2.lock();
			sleepTime = Queue2.front();
			Queue2.pop();
			mQueue2.unlock();
			mSleep1.lock();
			cout << "CPU1:sleep Time from Queue2 " << sleepTime << endl;
			if ((Queue1.size() == 2) || (Queue2.size() == 2))
				ReleaseSemaphore(m_hSemaphore, 1, NULL);
			Sleep(sleepTime);
			mSleep1.unlock();
		}
		else
		{
			mSleep1.lock();
			cout << "CPU1:wait 3 sec\n";
			Sleep(sleepTime);
			mSleep1.unlock();
		}

	}
}

void putTimeInQueue(queue<int> &CPUQueue1, queue<int> &CPUQueue2, mutex &mSleep, mutex &mQueue1, mutex &mQueue2)
{
	for (int i = 0; i < 6; i++) 
	{
		mSleep.lock();
		cout << "Sleep 2 sec\n";
		Sleep(2000);
		mSleep.unlock();
		int a = (i + 1) * 1000*2;
		//int a = 4000;
		if (i % 2 == 0)
		{
			a = 8000;
			mQueue1.lock();
			CPUQueue1.push(a);
			cout << "a was pushed into CPUQueue1\n";
			mQueue1.unlock();
		}
		else
		{
			a = 2000;
			mQueue2.lock();
			CPUQueue2.push(a);
			cout << "a was pushed into CPUQueue2\n";
			mQueue2.unlock();
		}
		if ((CPUQueue1.size() == 2) || (CPUQueue2.size() == 2))
			ReleaseSemaphore(m_hSemaphore, 1, NULL);
	}
}


int main()
{
	queue <int> CPUQueue1, CPUQueue2;
	mutex mSleep1, mSleep,mSleep2, mQueue1, mQueue2, mCPU, fullQueue;
	m_hSemaphore = CreateSemaphore(NULL, 0, 1000, NULL);
	thread CPU1(takeTask, ref(CPUQueue1), ref(CPUQueue2), ref(mSleep1), ref(mQueue1), ref(mQueue2) , ref(mCPU), ref(fullQueue));
	thread CPU2(takeTask2, ref(CPUQueue1), ref(CPUQueue2), ref(mSleep2), ref(mQueue1), ref(mQueue2), ref(mCPU), ref(fullQueue));
	thread Queue(putTimeInQueue, ref(CPUQueue1), ref(CPUQueue2), ref(mSleep), ref(mQueue1), ref(mQueue2));
	CPU1.join();
	CPU2.join();
	Queue.join();


	return 0;
}