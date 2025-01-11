#include "ThreadPool.h"
#include <iostream>
using namespace std;


ThreadPool::ThreadPool(int min,int max):m_maxThread(max),m_minThread(min),m_stop(false),m_idleThread(min),m_curThread(min)
{
	m_manager = new thread(&ThreadPool::manager,this);
	for(int i = 0;i<min;i++){
		thread t(&ThreadPool::worker,this);
		m_workers.insert(std::make_pair(t.get_id(),move(t)));

	}
}


ThreadPool::manager(void){
	while(!m_stop.load()){
		this_thread::sleep_for(chrono::seconds(3));
		int idel = m_idelThread.load();
		int cur = m_curThread.load();
		if(cur >minThread && idel >cur/2){
			m_exitThread.store(2);
			m_condition.notify_all();

		}
		lock_guard<mutex> lck(m_idsMutex);
		if(!m_id.empty()){
			for(auto id:m_id)
			{
				auto it  = m_workers.find(id);
				if(it != m_workers.end()){
					(*it).second.join();
					m_workers.erase(it);
				}

			}
		}

		else if(idel ==0 && cur < m_maxThread){
			thread t(ThreadPool::worker,this);
			m_workers.insert(make_pair(t.get_id(),move(t)));
			m_curThread++;
			m_idleThread++;
		}
	}


}


ThreadPool::worker(void){
	while(!m_stop.load())
	{
		function<void(void)> task = nullptr;

		unique_lock<mutex> locker(m_queueMutex);
		while(m_tasks.empty() && !m_stop){
			m_condition.wait(locker);
			if(m_exitThread>0){
				m_exitThread--;
				m_curThread--;
				lock_guard<mutex> lck(m_idsMutex);
				m_id.emplace_back(this_thread::get_id());
				return;
			}
		}
	 	 if(!m_tasks.empty()){
                	cout << "取出了一个任务" << endl;
			task = move(m_tasks.front());
			m_tasks.pop_front();
       		 }
		if(task){
			m_idleThread--;
			task();
			m_idleThread++;
		}

	}

}

void ThreadPool::addTask(function<void(void)>task)
{

	{
		lock_guard<mutex> locker(m_queueMutex);
		m_tasks.emplace_back(task);

	}
	m_condition.notify_one();

}

ThreadPool::~ThreadPool()
{
	m_stop = true;
	m_condition.notify_all();
	for(auto it : m_workers)
	{
		thread& t = it.second;
		if(t.joinable())
		{
			t.join();
			cout << "线程" << t.get_id() << "将要退出了" << endl;

		}

	}
	if(m_manager->joinable()){
		m_manager->join();

	}
	delete m_manager;





}

