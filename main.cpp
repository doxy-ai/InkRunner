#include <battery/embed.hpp>

#include <system.h>
#include <choice.h>
#include <runner.h>
#include <story.h>

#include <iostream>
#include <sstream>
#include <limits>

#include "wait_for_key.hpp"

void print_wrapped(const std::string& text, std::size_t max_width = 100, std::size_t indent_size = 2) {
	std::istringstream words(text);
	std::string word;
	std::string line;
	bool first_line = true;

	while (words >> word) {
		std::size_t line_limit = first_line ? max_width : max_width - indent_size;

		// Would adding this word exceed the limit?
		if (!line.empty() && line.length() + 1 + word.length() > line_limit) {
			if (!first_line)
				std::cout << std::string(indent_size, ' ');
			std::cout << line << std::endl;

			line.clear();
			first_line = false;
		}

		if (!line.empty())
			line += ' ';
		line += word;
	}

	// Print remaining text
	if (!line.empty()) {
		if (!first_line)
			std::cout << std::string(indent_size, ' ');
		std::cout << line << std::endl;
	}
}

int read_int() {
	int value;
	while (true) {
        std::cin >> value;

        // Valid integer AND no extra junk after it
        if (!std::cin.fail() && std::cin.peek() == '\n')
            return value;

        // Something went wrong
        std::cout << "Your choice must be a valid number: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
	return value;
}

int prompt_choice(int max) {
	std::cout << "Enter your choice and confirm with `enter`: ";

	int id = read_int();
	while(!(0 <= id && id < max)) {
		std::cout << "Please select a choice between 0 and " << (max - 1) << ": ";
		id = read_int();
	}

	std::cout << std::endl;
	return id;
}

int main() {
	const auto compiled = b::embed<RELATIVE_STORY_INK_BIN_PATH>();
	auto* story = ink::runtime::story::from_binary((unsigned char*)compiled.data(), compiled.size());
	auto ink = story->new_runner();

	ink->bind("my_ink_function", [](int a, int b){
		return a + b;
	});

	std::cout << "You can press any key to continue... enjoy the story!\n\n" << std::endl;

	while (true) {
 		while (ink->can_continue()) {
			print_wrapped(ink->getline());
			wait_for_key();
		}

		if(!ink->has_choices())
			break;

		int id = 0;
		std::cout << "\n";
		for(auto& choice: *ink) {
			std::cout << (id++) << ": ";
			print_wrapped(choice.text());
		}
		ink->choose(prompt_choice(ink->num_choices()));
	}

#ifdef __EMSCRIPTEN__
    std::cout << "\n\nRefresh to play again!" << std::endl;
#endif
}