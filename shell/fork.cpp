/*
Jonathan Tousley
CS 3100
Dr. Kenneth Sundberg
Assignment 5 - I/O Redirection
*/

#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <string>
#include <vector>
#include <cstring>
#include <chrono>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <fstream>

const int STANDARD_READ = 0;
const int STANDARD_WRITE = 1;
const int STANDARD_IN = 0;
const int STANDARD_OUT = 1;
const int FIRST_COMMAND = 0;
const int SECOND_COMMAND = 1;
const int THIRD_COMMAND = 2;
const int HISTORY_OFFSET = 1;

void printTimes(double a);
void parseString(std::string input, std::vector<std::vector<char*>> &args, bool&, bool&, bool&, bool&, std::string&, std::string&, int& commandsToExecute);
std::string getHistoryCommand(std::string input, const std::vector<std::string> &History);
double runBasicCommand(std::vector<char*> &args, bool initialInput, bool finalOutput, std::string initialInputFile, std::string finalOutputFile);
double runDoubleCommand(std::vector<std::vector<char*>> args, bool initialInput, bool finalOutput, std::string initialInputFile, std::string finalOutputFile);
double runTripleCommand(std::vector<std::vector<char*>> args, bool initialInput, bool finalOutput, std::string initialInputFile, std::string finalOutputFile);


int main(void)
{
	std::string input;
	std::vector<std::string> History;	//saves all the commands entered into the prompt
	std::vector<std::vector<char*>> args;	//vector of vectors of char pointers; char* = individual words, vectors = list of words that makes a command, vector = list of commands
	auto childProcessTimes = 0.0;		//saves the time executing child processes
	auto startTime = std::chrono::system_clock::now();
	auto endTime = std::chrono::system_clock::now();
	std::string finalOutputFile;
	std::string initialInputFile;
	bool finalOutput = false;
	bool initialInput = false;
	bool hasPipe = false;
	bool hasTwoPipes = false;

	//reset variables
	do{
		try{
		input.clear();
		finalOutputFile = "";
		initialInputFile = "";
		finalOutput = false;
		initialInput = false;
		hasPipe = false;
		hasTwoPipes = false;
		auto commandsToExecute = 0;

		std::cout << "[Cmd]: ";
		std::getline(std::cin, input);

		if(input[0] == '^'){
			input = getHistoryCommand(input, History);
		}

		History.push_back(input);

		if(input == "ptime"){
			printTimes(childProcessTimes);		//prints the time
		}
		else if(input == "history"){
			for(uint i = 0; i < History.size(); i++){
				std::cout << i + HISTORY_OFFSET << ": " << History[i] << std::endl;
			}
		}
		else if(input == "exit"){
			//do nothing
		}
		else{
			args.clear();
			parseString(input, args, hasPipe, hasTwoPipes, initialInput, finalOutput, initialInputFile, finalOutputFile, commandsToExecute);
			if(!hasPipe){
				childProcessTimes += runBasicCommand(args[FIRST_COMMAND], initialInput, finalOutput, initialInputFile, finalOutputFile);
			}
			if(hasPipe && !hasTwoPipes){
				childProcessTimes += runDoubleCommand(args, initialInput, finalOutput, initialInputFile, finalOutputFile);
			}
			else{
				childProcessTimes += runTripleCommand(args, initialInput, finalOutput, initialInputFile, finalOutputFile);
			} // else
			childProcessTimes += std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
		}
	}
	catch(std::string errorMessage){ std::cout << errorMessage << std::endl; }
	} //do
	while(input != "exit");

	return 0;
}

void parseString(
	std::string input, std::vector<std::vector<char*>> &arguments,
	bool& hasPipe, bool& hasTwoPipes, bool& initialInput, bool& finalOutput,
	std::string& initialInputFile, std::string& finalOutputFile, int& commandsToExecute
){
	std::vector<char*> tempArguments;
	bool push = true;
	auto wordStart = 0;
	uint i = 0;
	for(; i < input.length(); i++){
		if(input[i] == '|'){
			tempArguments.push_back(nullptr);
			arguments.push_back(tempArguments);
			tempArguments.clear();
			if(hasPipe){
				hasTwoPipes = true;
			}
			hasPipe = true;
			commandsToExecute++;
			i+=2;
			wordStart = i;
		}
		else if(input[i] == '<'){
			initialInput = true;
			tempArguments.push_back(nullptr);
			i+=2;
			std::stringstream ss;
			while(input[i] != ' ' && i < input.length()){
				ss << input[i];
				i++;
			}
			initialInputFile = ss.str();
			wordStart = i + 1;
			push = false;
		}
		else if(input[i] == '>'){
			finalOutput = true;
			tempArguments.push_back(nullptr);
			i+=2;
			std::stringstream ss;
			while(input[i] != ' ' && i < input.length()){
				ss << input[i];
				i++;
			}
			finalOutputFile = ss.str();
			wordStart = i + 1;
			push = false;
		}
		else if(input[i] == ' '){
			tempArguments.push_back(strdup(input.substr(wordStart, i - wordStart).c_str()));
			wordStart = i + 1;
			push = true;
		}
	} // for
	if(push){
		tempArguments.push_back(strdup(input.substr(wordStart, i - wordStart).c_str()));
		tempArguments.push_back(nullptr);
	}
	arguments.push_back(tempArguments);
}

std::string getHistoryCommand(std::string input, const std::vector<std::string> &History)
{
	uint i = 0;
	bool found = false;
	for(; i < input.length(); i++){
		if(std::isdigit(input[i])){
			found = true;
			break;
		}
	}
	if(!found){
		throw static_cast<std::string>("Invalid command");
	}

	int histCommand = std::stoi(input.substr(i, input.length()));

	if(histCommand < 0 || histCommand > static_cast<int>(History.size())){
		return input;
	}

	return History[histCommand - 1];
}

void printTimes(double totalTime)
{
	auto sec = totalTime / 1000000000;
	auto milli = totalTime / 1000000;
	auto micro = totalTime / 1000;
	std::cout << "Total time spent executing child processes:\n"
		<< sec << " seconds\n"
		<< milli << " milliseconds\n"
		<< micro << " microseconds\n";
}

double runTripleCommand(std::vector<std::vector<char*>> args, bool initialInput, bool finalOutput, std::string initialInputFile, std::string finalOutputFile)
{
	int STDOUT = dup(STANDARD_OUT);
	int STDIN = dup(STANDARD_IN);
	int PIPE[2];
	int secondPipe[2];
	pipe(PIPE); //read on 0, write on 1
	pipe(secondPipe);

	auto startTime = std::chrono::system_clock::now();

	auto child1 = fork();
	if(child1 < 0){
		std::cerr << "Error occured while trying to fork" << std::endl;
		exit(0);
	}
	if(child1 == 0){
		if(initialInput){
			freopen(initialInputFile.c_str(), "r", stdin);
		}

		dup2(PIPE[STANDARD_WRITE], STANDARD_OUT);	//redirect std::out to pipe
		close(PIPE[STANDARD_IN]);		//read end

		if( execvp(args[FIRST_COMMAND][0], args[FIRST_COMMAND].data()) < 0 ){
			std::cerr << "bash: " << args[FIRST_COMMAND][0] << ": cmd not found\n";
			exit(0);
		}

		if(initialInput)
			fclose(stdin);
	}

	auto child2 = fork();
	if(child2 < 0){
		std::cerr << "Error occured while trying to fork" << std::endl;
		exit(0);
	}
	if(child2 == 0){
		dup2(PIPE[STANDARD_READ], STANDARD_IN);	//redirect std::in to pipe
		close(PIPE[STANDARD_WRITE]);

		dup2(secondPipe[STANDARD_WRITE], STANDARD_OUT);	//redirect std::out to pipe
		close(secondPipe[STANDARD_IN]);		//read end

		if( execvp(args[SECOND_COMMAND][0], args[SECOND_COMMAND].data()) < 0 ){
			std::cerr << "bash: " << args[SECOND_COMMAND][0] << ": cmd not found\n";
			exit(0);
		}
	}

	auto child3 = fork();
	if(child3 < 0){
		std::cerr << "Error occured while trying to fork" << std::endl;
		exit(0);
	}
	if(child3 == 0){
		dup2(secondPipe[STANDARD_READ], STANDARD_IN);	//redirect std::in to pipe
		close(secondPipe[STANDARD_WRITE]);

		if(finalOutput){
			freopen(finalOutputFile.c_str(), "w", stdout);
		}

		if( execvp(args[THIRD_COMMAND][0], args[THIRD_COMMAND].data()) < 0 ){
			std::cerr << "bash: " << args[THIRD_COMMAND][0] << ": cmd not found\n";
			exit(0);
		}
		if(finalOutput)
			fclose(stdout);
	}

	waitpid(child1, nullptr, 0);

	close(PIPE[STANDARD_WRITE]); //write end
	close(PIPE[STANDARD_READ]); //read end

	waitpid(child2, nullptr, 0);

	close(secondPipe[STANDARD_WRITE]);
	close(secondPipe[STANDARD_READ]);

	waitpid(child3, nullptr, 0);

	dup2(STDIN, STANDARD_IN);
	dup2(STDOUT, STANDARD_OUT);

	auto endTime = std::chrono::system_clock::now();

	return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();

}

double runDoubleCommand(std::vector<std::vector<char*>> args, bool initialInput, bool finalOutput, std::string initialInputFile, std::string finalOutputFile)
{
	int STDOUT = dup(STANDARD_OUT);
	int STDIN = dup(STANDARD_IN);
	int PIPE[2];
	pipe(PIPE); //read on 0, write on 1

	auto startTime = std::chrono::system_clock::now();

	auto child1 = fork();
	if(child1 < 0){
		std::cerr << "Error occured while trying to fork" << std::endl;
		exit(0);
	}
	if(child1 == 0){
		if(initialInput){
			freopen(initialInputFile.c_str(), "r", stdin);
		}

		dup2(PIPE[STANDARD_WRITE], STANDARD_OUT);	//redirect std::out to pipe
		close(PIPE[STANDARD_IN]);		//read end

		if( execvp(args[FIRST_COMMAND][0], args[FIRST_COMMAND].data()) < 0 ){
			std::cerr << "bash: " << args[FIRST_COMMAND][0] << ": cmd not found\n";
			exit(0);
		}

		if(initialInput)
			fclose(stdin);
	}

	auto child2 = fork();
	if(child2 < 0){
		std::cerr << "Error occured while trying to fork" << std::endl;
		exit(0);
	}
	if(child2 == 0){
		dup2(PIPE[STANDARD_READ], STANDARD_IN);	//redirect std::in to pipe
		close(PIPE[STANDARD_WRITE]);

		if(finalOutput){
			freopen(finalOutputFile.c_str(), "w", stdout);
		}

		if( execvp(args[SECOND_COMMAND][0], args[SECOND_COMMAND].data()) < 0 ){
			std::cerr << "bash: " << args[SECOND_COMMAND][0] << ": cmd not found\n";
			exit(0);
		}
		if(finalOutput)
			fclose(stdout);
	}

	waitpid(child1, nullptr, 0);

	close(PIPE[STANDARD_WRITE]); //write end
	close(PIPE[STANDARD_READ]); //read end

	waitpid(child2, nullptr, 0);

	dup2(STDIN, STANDARD_IN);
	dup2(STDOUT, STANDARD_OUT);
	auto endTime = std::chrono::system_clock::now();

	return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
}

double runBasicCommand(std::vector<char*> &args, bool initialInput, bool finalOutput, std::string initialInputFile, std::string finalOutputFile)
{
	auto startTime = std::chrono::system_clock::now();
	auto pid = fork();
	if(pid < 0){
		std::cerr << "Error occured while trying to fork" << std::endl;
		exit(0);
	}
	if(pid == 0){
		//child process begins
		if(initialInput){
			freopen(initialInputFile.c_str(), "r", stdin);
		}

		if(finalOutput){
			freopen(finalOutputFile.c_str(), "w", stdout);
		}

		if( execvp(args[FIRST_COMMAND], args.data()) < 0 ){
			std::cout << "bash: " << args[FIRST_COMMAND] << ": cmd not found\n";
			exit(0);
		}

		if(initialInput)
			fclose(stdin);
		if(finalOutput)
			fclose(stdout);
	}
	waitpid(pid, nullptr, 0);
	auto endTime = std::chrono::system_clock::now();

	return std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count();
}
