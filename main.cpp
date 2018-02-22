

#include <string.h>
#include <stdio.h>

using namespace std;


int main(int argc, char** argv) {

	for(int i = 0; i < argc; i++){
		printf("argument %i : %s", i, argv[i]);
	}

    return 0;
}

