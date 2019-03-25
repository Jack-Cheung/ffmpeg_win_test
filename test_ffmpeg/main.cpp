#include "controller.hpp"

using namespace ffcv;
using namespace fc;
int main(int argc, char** argv)
{
	try
	{
		CConfig conf;
		CController ctrl(conf);
		ctrl.Process();
	}
	catch (fc::exception& e)
	{
		elog("[${name}]: ${what}", ("name", e.name())("what", e.top_message()));
	}
	catch (...)
	{
		elog("other exception");
	}
	system("pause");
	return 0;
}