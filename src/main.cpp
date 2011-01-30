#include <cstdlib>
#include <iostream>
#include <sstream>

#include "Process.h"
#include "Tracer.h"
#include "StringToType.hpp"

static void usage(const char * progName)
{
    std::cerr << "Usage : " << progName << " [-b addr] [-v level] { -p | -e | -c } arg" << std::endl;
}

static void startAction(unsigned char verbose, size_t minPC, Process const & process)
{
    std::cout << process << std::endl;
    Tracer tracer(process, verbose, minPC);

    tracer.init();
    //tracer.startTracing();
}

static void selectAction(unsigned char flag, unsigned char verbose, size_t minPC, char *argv[])
{
    switch (flag)
    {
	case 'e':
	    startAction(verbose, minPC, Process(argv));
	case 'p':
	    startAction(verbose, minPC, Process(stringToType<pid_t > (argv[0])));
	    break;
    }
}

static void parse(int argc, char *argv[])
{
    int opt;
    unsigned char verbose = 0, flag = 0;
    size_t address = 0;
    bool stop = false;

    while (false == stop && (opt = getopt(argc, argv, "b:v:p:e:")) != -1)
    {
	std::stringstream ss;
	switch (opt)
	{
	    case 'b':
		ss << std::string(argv[optind - 1]);
		ss >> std::hex >> address;
		break;
	    case 'v':
		verbose = stringToType<unsigned char>(argv[optind - 1]);
		break;
	    case 'p':
		flag = 'p';
		stop = true;
		break;
	    case 'e':
		flag = 'e';
		stop = true;
		break;
	    default: /* '?' */
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
    }

    if (0 == flag)
    {
	usage(argv[0]);
    }
    else
    {
	selectAction(flag, verbose, address, argv + (optind - 1));
    }
}

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
	parse(argc, argv);
    }
    else
    {
	usage(argv[0]);
    }
    return (EXIT_SUCCESS);
}
