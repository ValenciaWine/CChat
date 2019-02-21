#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#include "user.h"

#define NOPS 2


int main(int argc, char *argv[]) {

		if (init_user()<0)
				fprintf(stderr, "Error enviando mensaje para usuario\n");
		

	return 0;
}
