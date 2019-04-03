#pragma once

#ifndef __console_h__
#define __console_h__

#include <vector>
#include <string>


// Command-line interface namespace - session variables, not tab variables
namespace cli {
	//The console is not tab-specific, but session-specific.
	bool listening = false;
	//The console history, including only user input (reverse order, of course)
	std::vector<std::string> history;
	//Message history in the console
	std::vector<std::string> messages;
	//The text currently in the console
	std::string input = std::string();
	//Interpret and execute the commands from the input
	int honor(const std::string& command);
	//Digest the CLI input
	void gulp() {
		history.push_back(input);
		input.erase(input.begin());
		honor(input);
		input.clear();
	}
	//Send a message to the console
	void send_message(const std::string& mess, int urgency = 0) {
		if (urgency) {
			messages.push_back('<' + std::to_string(urgency) + '>' + mess);
		}
		else {
			messages.push_back(mess);
		}
	}
	//The string that should show up on the command-line bar presently
	std::string getfield() {
		if (listening) {
			return input;
		}
		else {
			if (messages.size() > 0)
				return messages.back();
		}
		return std::string();
	}
}

//Interpret and execute the commands from the input
int cli::honor(const std::string& command) {

	return 0;
}



#endif