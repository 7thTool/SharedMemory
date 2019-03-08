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

	XSHMQueue shmwrite(argv[1],XUtil::strto<size_t>(argv[2]));
	for(size_t i = 0, j = XUtil::strto<size_t>(argv[3]); i < j; ++i)
	{
		shmwrite.write_to_queue(argv[4]);
		//std::cout << "write " << i << " " << argv[4] << std::endl;
	}

	std::cout << boost::posix_time::microsec_clock::universal_time() - now << std::endl;

	return 0;
}

