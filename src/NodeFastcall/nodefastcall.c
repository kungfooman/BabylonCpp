#include <stdio.h>
#include <stdlib.h>
#include <direct.h>

int main(int argc, char *argv[]) {
	char exepath[512];

	snprintf(exepath, sizeof(exepath), "%s", argv[0]);

	// replace last slash/backslash with \0 terminator
	for (int i = 511; i >= 0; i--) {
		// is slash/backslash?
		if (exepath[i] == '/' || exepath[i] == '\\') {
			// if so, terminate string here
			exepath[i] = '\0';
			break;
		}
	}

	//printf("exepath=\"%s\"", exepath);
	chdir(exepath); // go where there .exe is
	// todo: remove hardcoded path
	system("node D:/BabylonCpp/src/NodeFastcall/main.js");
	system("pause");
	return 0;
}
