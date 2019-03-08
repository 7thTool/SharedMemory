#include "shm.hpp"

int main(int argc, char* argv[])
{
	//argv[0]是app名字路径
	for(size_t i = 0; i < argc; i++)
	{
		std::cout << argv[i] << " ";
	}
	std::cout << std::endl;
	if(argc < 5) {
		std::cout << "please input:./shmcreate name size num msg" << std::endl;
		return -1;
	}

	//boost::posix_time::ptime  now ( boost::posix_time::second_clock::local_time() ); //机器的本地时间，比如北京时间，与电脑设置的时区有关
	//boost::posix_time::ptime  now ( boost::posix_time::second_clock::universal_time() );//机器的GMT时间，本地时间加减时区偏移量
	boost::posix_time::ptime now(boost::posix_time::microsec_clock::universal_time());

#if TEST_SHMQUEUE
	now = boost::posix_time::microsec_clock::universal_time();

	XSHMQueue shmqueue(argv[1],XUtil::strto<size_t>(argv[2]));
	for(size_t i = 0, j = XUtil::strto<size_t>(argv[3]); i < j; ++i)
	{
		shmqueue.write_to_queue(argv[4]);
		//std::cout << "write " << i << " " << argv[4] << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	std::cout << "queue:" << boost::posix_time::microsec_clock::universal_time() - now << std::endl;
#endif//

#if TEST_MQ	
	now = boost::posix_time::microsec_clock::universal_time();

	const char* mq_name = "message_queue";
	const size_t max_msg_size = 102400;
	boost::interprocess::message_queue::remove(mq_name);
	boost::interprocess::message_queue mq(boost::interprocess::create_only //only create
                                                , mq_name //name
                                                , 1000 //max message number
                                                , max_msg_size //max message size
    );
	const char* msg = argv[4];
	const size_t msg_len = strlen(msg);
	for(size_t i = 0, j = XUtil::strto<size_t>(argv[3]); i < j; ++i)
	{
		mq.send(msg, msg_len, 0);
		//std::cout << "write " << i << " " << argv[4] << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}

	std::cout << mq_name << ":" << boost::posix_time::microsec_clock::universal_time() - now << std::endl;
#endif

#if TEST_MEMORY
	now = boost::posix_time::microsec_clock::universal_time();

	XSHMMemory shmmemory(argv[1],XUtil::strto<size_t>(argv[2]));
	for(size_t i = 0, j = XUtil::strto<size_t>(argv[3]); i < j; ++i)
	{
		shmmemory.write(argv[4]);
		//std::cout << "write " << i << " " << argv[4] << std::endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));
		//std::this_thread::yield();
	}

	std::cout << "memory:" << boost::posix_time::microsec_clock::universal_time() - now << std::endl;
#endif//

	return 0;
}

