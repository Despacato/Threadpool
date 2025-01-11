#include <thread>
#include <vector>
#include <atomic>
#include <deque>
#include <funcitonal>
#include <mutex>
#include <condition_variable>
#include <map>

using namespace std;

class ThreadPool
{
public:



private:

	thread* m_manager; //管理者线程
	map<thread::id,thread> m_workers; //存储工作线程
	vector<thread::id> m_id; //存放已经退出的线程id
	atomic<int> m_minThread; //最小线程数
	atomic<int> m_maxThread;//最大线程数
	atomic<int> m_curThread;//当前线程数量
	atomic<int> m_idleThread;//空闲线程数量
	atomic<int> m_exitThread;//退出的线程数量
	atomic<bool> m_stop; //线程池开关

	deque<function<void(void)>> m_tasks; //任务队列
	mutex m_queueMutex;
	mutex m_idsMutex;
	condition_variable m_condition;
	void manager(void);
	void worker(void);


};
