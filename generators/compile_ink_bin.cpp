#include <system.h>
#include <compiler.h>

// #include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cout << "Usage: " << argv[0] << " <file>.ink.json";
        return -1;
    }

    ink::compiler::run(argv[1], "story.ink.bin");
}

// TODO: Why can't the linker find this!?!?
namespace ink {
    #define A 54059 /* a prime */
    #define B 76963 /* another prime */
    #define C 86969 /* yet another prime */
    #define FIRSTH 37 /* also prime */

	hash_t hash_string(const char* string) {
		hash_t h = FIRSTH;
		while (*string) {
			h = (h * A) ^ (string[0] * B);
			string++;
		}
		return h; // or return h % C;
	}
}