#pragma once

#include <XUtil/XIPC.hpp>
#include <XUtil/XStr.hpp>

#define TEST_SHMQUEUE 0 //加锁互斥的情况下，单线程每秒可达10万笔
#define TEST_MQ 0 //单线程每秒可达25万笔，所以使用MQ是共享内存消息队列最高效的方式
#define TEST_MEMORY 1 //加锁互斥的情况下，单线程写可达100万次，读可达50万次

class XSHMQueue : public XUtil::XSharedMemory<>
{
	typedef XUtil::XSharedMemory<> Base;
public:
	typedef boost::interprocess::interprocess_mutex shm_mutex_t;
	//typedef boost::interprocess::interprocess_upgradable_mutex shm_mutex_t;
	typedef boost::interprocess::deque<char_string, char_string_allocator> char_string_queue;
	struct SHMQueue 
	{
		//不要在共享内存里使用引用

		//Mutex to protect access to the queue
		shm_mutex_t mutex_;
		//Condition to wait when the queue is empty
   		boost::interprocess::interprocess_condition  cond_;
		char_string_queue msg_queue_;

		SHMQueue(char_string_allocator& msg_alloc)
		:msg_queue_(msg_alloc) {}

		void push(const char_string&& msg) {
			//This will call lock_upgradable()
			//boost::interprocess::upgradable_lock<shm_mutex_t> upgrade_lock(mutex_);
			//This calls unlock_upgradable_and_lock()
			//boost::interprocess::scoped_lock<shm_mutex_t> lock(std::move(upgrade_lock));
			boost::interprocess::scoped_lock<shm_mutex_t> lock(mutex_);
			msg_queue_.emplace_back(msg);
			cond_.notify_one();
		}
		
		bool pop(std::string& msg, size_t timeout = 0) {
			//This will call lock_upgradable()
			//boost::interprocess::upgradable_lock<shm_mutex_t> upgrade_lock(mutex_);
			//This calls unlock_upgradable_and_lock()
			//boost::interprocess::scoped_lock<shm_mutex_t> lock(std::move(upgrade_lock));
			boost::interprocess::scoped_lock<shm_mutex_t> lock(mutex_);
			if(msg_queue_.empty()) {
				if(timeout) {
					if(!cond_.timed_wait(lock, boost::posix_time::microsec_clock::universal_time() + boost::posix_time::microseconds(timeout))) {
						return false;
					}
				} 
				if(msg_queue_.empty()) {
					return false;
				}
			}
			msg = msg_queue_.front().c_str();
			msg_queue_.pop_front();
			return true;
		}
	};
private:
	std::shared_ptr<char_allocator> char_alloc_ptr_;
	std::shared_ptr<char_string_allocator> char_string_alloc_ptr_;
	SHMQueue* shm_queue_ptr_;
public:
	XSHMQueue(const char *segment_name, size_t segment_size):Base(segment_name, segment_size) {
		if(is_open()) {
			char_alloc_ptr_ = std::make_shared<char_allocator>(segment_ptr_->get_segment_manager());
			char_string_alloc_ptr_ = std::make_shared<char_string_allocator>(segment_ptr_->get_segment_manager());
			try
			{
				shm_queue_ptr_ = segment_ptr_->construct<SHMQueue>("queue")(*char_string_alloc_ptr_);
			}
			catch (boost::interprocess::interprocess_exception &ex)
			{
				if (boost::interprocess::already_exists_error == ex.get_error_code())
				{
					shm_queue_ptr_ = segment_ptr_->find_or_construct<SHMQueue>("queue")(*char_string_alloc_ptr_);
				}
			}
			catch (boost::interprocess::bad_alloc &ex)
			{

			}
		}
	}
	~XSHMQueue()
	{
		char_alloc_ptr_.reset();
		char_string_alloc_ptr_.reset();
	}

	void write_to_queue(const char* msg) {
		char_string shm_msg(msg, *char_alloc_ptr_);
		shm_queue_ptr_->push(std::move(shm_msg));
	}

	bool read_from_queue(std::string& msg) {
		char_string shm_msg(*char_alloc_ptr_);
		return shm_queue_ptr_->pop(msg);
	}
};

class XSHMMemory : public XUtil::XSharedMemory<true>
{
	typedef XUtil::XSharedMemory<true> Base;
public:
	typedef boost::interprocess::interprocess_mutex shm_mutex_t;
	//typedef boost::interprocess::interprocess_upgradable_mutex shm_mutex_t;
	struct SHMMemory
	{
		//不要在共享内存里使用引用
		
		//Mutex to protect access to the queue
		shm_mutex_t mutex_;
		char_string msg_;

		SHMMemory(char_allocator& msg_alloc):msg_(msg_alloc) {}
	};
private:
	std::shared_ptr<char_allocator> char_alloc_ptr_;
	SHMMemory* shm_memory_ptr_;
public:
	XSHMMemory(const char *segment_name, size_t segment_size):Base(segment_name, segment_size) {
		if(is_open()) {
			char_alloc_ptr_ = std::make_shared<char_allocator>(segment_ptr_->get_segment_manager());
			try
			{
				shm_memory_ptr_ = segment_ptr_->construct<SHMMemory>("memory")(*char_alloc_ptr_);
			}
			catch (boost::interprocess::interprocess_exception &ex)
			{
				if (boost::interprocess::already_exists_error == ex.get_error_code())
				{
					shm_memory_ptr_ = segment_ptr_->find_or_construct<SHMMemory>("memory")(*char_alloc_ptr_);
				}
			}
			catch (boost::interprocess::bad_alloc &ex)
			{

			}
		}
	}
	~XSHMMemory()
	{
		char_alloc_ptr_.reset();
	}
	
	inline void write(const char* msg) {
		//This will call lock_upgradable()
		//boost::interprocess::upgradable_lock<shm_mutex_t> upgrade_lock(shm_memory_ptr_->mutex_);
		//This calls unlock_upgradable_and_lock()
		//boost::interprocess::scoped_lock<shm_mutex_t> lock(std::move(upgrade_lock));
		boost::interprocess::scoped_lock<shm_mutex_t> lock(shm_memory_ptr_->mutex_);
		shm_memory_ptr_->msg_ = msg;
	}

	inline void read(std::string& msg) {
		//This will call lock_upgradable()
		//boost::interprocess::upgradable_lock<shm_mutex_t> upgrade_lock(shm_memory_ptr_->mutex_);
		//This calls unlock_upgradable_and_lock()
		//boost::interprocess::scoped_lock<shm_mutex_t> lock(std::move(upgrade_lock));
		boost::interprocess::scoped_lock<shm_mutex_t> lock(shm_memory_ptr_->mutex_);
		msg = shm_memory_ptr_->msg_.c_str();
	}
};




