#include "shm.hpp"

int main(int argc, char* argv[])
{
	//argv[0]是app名字路径
	for(size_t i = 0; i < argc; i++)
	{
		std::cout << argv[i] << " ";
	}
	std::cout << std::endl;
	if(argc < 3) {
		std::cout << "please input:./shmread name size num msg" << std::endl;
		return -1;
	}

	//boost::posix_time::ptime  now ( boost::posix_time::second_clock::local_time() ); //机器的本地时间，比如北京时间，与电脑设置的时区有关
	//boost::posix_time::ptime  now ( boost::posix_time::second_clock::universal_time() );//机器的GMT时间，本地时间加减时区偏移量
	boost::posix_time::ptime now(boost::posix_time::microsec_clock::universal_time());

#if TEST_SHMQUEUE
	now = boost::posix_time::microsec_clock::universal_time();

	XSHMQueue shmqueue(argv[1],XUtil::strto<size_t>(argv[2]));
	for(size_t i = 0, j = XUtil::strto<size_t>(argv[3]); i < j;)
	{
		std::string msg;
		if(shmqueue.read_from_queue(msg)) {
			//std::cout << msg << std::endl;
			++i;
		}
	}

	std::cout << "queue:" << boost::posix_time::microsec_clock::universal_time() - now << std::endl;
#endif//

#if TEST_MQ	
	now = boost::posix_time::microsec_clock::universal_time();

	const char* mq_name = "message_queue";//Open a message queue.
	const size_t max_msg_size = 102400;
    boost::interprocess::message_queue mq(boost::interprocess::open_only
                                                , mq_name
    );
    unsigned int priority;
    boost::interprocess::message_queue::size_type recvd_size;

	char msg[100] = {0};
	size_t msg_len = 0;
	for(size_t i = 0, j = XUtil::strto<size_t>(argv[3]); i < j; ++i)
	{
		mq.receive(msg, max_msg_size, msg_len, priority);
		//std::cout << "receive " << i << " " << msg << std::endl;
	}

	std::cout << mq_name << ":" << boost::posix_time::microsec_clock::universal_time() - now << std::endl;
#endif

#if TEST_MEMORY
	now = boost::posix_time::microsec_clock::universal_time();

	XSHMMemory shmmemory(argv[1],XUtil::strto<size_t>(argv[2]));
	for(size_t i = 0, j = XUtil::strto<size_t>(argv[3]); i < j; ++i)
	{
		std::string msg;
		shmmemory.read(msg);
		//std::cout << "read " << i << " " << msg << std::endl;
	}

	std::cout << "memory:" << boost::posix_time::microsec_clock::universal_time() - now << std::endl;
#endif//

	return 0;
}

