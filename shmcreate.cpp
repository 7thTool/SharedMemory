
#include <XUtil/XSharedMemory.hpp>
#include <XUtil/XStr.hpp>

int main(int argc, char* argv[])
{
	//argv[0]是app名字路径
	for(size_t i = 0; i < argc; i++)
	{
		std::cout << argv[i] << " ";
	}
	std::cout << std::endl;
	if(argc < 3) {
		std::cout << "please input:./shmcreate name size" << std::endl;
		return -1;
	}

	XUtil::XSharedMemoryRemove shm_remove(argv[1]);

	XUtil::XSharedMemory<> shm(argv[1],XUtil::strto<size_t>(argv[2]));
	
	return 0;
}

