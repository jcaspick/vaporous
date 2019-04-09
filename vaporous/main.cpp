#include "Demo.h"
#include <time.h>

int main() {
	srand(time(NULL));
	Demo demo;
	demo.init();
	demo.run();

	return 0;
}