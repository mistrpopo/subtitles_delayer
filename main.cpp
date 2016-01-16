#include "srt_delay.h"

#include <iostream>
#include <string>
using std::cout;
using std::endl;
using std::string;
#include <cstdlib>

static const unsigned int number_of_args = 4;
static const string required_args[number_of_args] =
{
	"subtitles_delayer",
	"<in_filename>",
	"<out_filename>",
	"<delay_ms>"
};

void usage();
void done();

int main(int argc, char** argv)
{
	if (argc != number_of_args)
	{
		usage();
		return EXIT_FAILURE;
	}
	string in_filename = argv[1];
	string out_filename = argv[2];
	long long delay = atoll(argv[3]);
	if (delay == 0) return EXIT_FAILURE;

	if (!srt_delay(in_filename, out_filename, delay)) return EXIT_FAILURE;
	
	done();
	return EXIT_SUCCESS;
}

void usage()
{
	cout << "usage: " << endl;
	for (unsigned int i = 0; i < number_of_args; ++i)
	{
		cout << required_args[i] << " ";
	}
	cout << endl;
}

void done()
{
	cout << "Done!" << endl;
}