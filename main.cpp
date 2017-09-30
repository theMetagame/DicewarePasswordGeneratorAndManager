#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdint>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <stdio.h>
#include <time.h>

#include <conio.h>

using namespace std;

string generatePassphrase(vector<string> &wordlist);
void flushCin();
string getCurrentDateTime();

class PasswordDatabase {
private:
	vector<string> labels;
	vector<string> passwords;
	vector<string> dates;
	vector<string> notes;

	bool unsavedChanges = false;

public:
	PasswordDatabase() {
		unsavedChanges = false;
	}
	PasswordDatabase(string decryptedFile) {
		unsavedChanges = false;
		stringstream buffer(decryptedFile);
		string line = "";
		getline(buffer, line, '\n');
		while (!buffer.eof()) {
			getline(buffer, line, '\n');
			labels.push_back(line);
			getline(buffer, line, '\n');
			passwords.push_back(line);
			getline(buffer, line, '\n');
			dates.push_back(line);
			getline(buffer, line, '\n');
			notes.push_back(line);
		}
	}
	vector<string> getLabels() {
		return labels;
	}
	vector<string> getPasswords() {
		return passwords;
	}
	vector<string> getDates() {
		return dates;
	}
	vector<string> getNotes() {
		return notes;
	}

	bool hasUnsavedChanges() {
		return unsavedChanges;
	}

	void flagUnsavedChanges() {
		unsavedChanges = true;
	}

	void printCensoredEntry(int id) {
		cout << labels[id] << ": " << endl;
		string temp = passwords[id];
		bool afterSpace = true;
		for (int i = 0; i < temp.length(); i++) {
			if (temp[i] != ' ' && !afterSpace) temp[i] = '*';
			else if (temp[i] != ' ') {
				afterSpace = false;
			}
			else {
				temp[i] = '_';
				afterSpace = true;
			}
		}
		cout << "\t" << temp << endl;
	}

	void printEntry(int id) {
		cout << labels[id] << ": " << endl;
		cout << "\t" << passwords[id] << endl;
		cout << "Password created at:" << endl;
		cout << "\t" << dates[id] << endl;
		cout << "Notes:" << endl;
		cout << "\t" << notes[id] << endl;
	}

	void addNewPassphrase(vector<string> &wordlist) {
		cout << "What is the name of the site this passphrase is for?" << endl;
		string name = "";
		getline(cin, name, '\n');
		string passphrase = generatePassphrase(wordlist);

		labels.push_back(name);
		passwords.push_back(passphrase);

		notes.emplace_back();

		dates.push_back(getCurrentDateTime());

		unsavedChanges = true;
		cout << "Passphrase added." << endl;
	}

	void editEntry(int id, vector<string> &wordlist) {
		while (true) {
			cout << "Edit options for " + labels[id] + ":\n\t1. Change label.\n\t2. Change passphrase.\n\t3. Delete entry.\n\t4. Edit password notes\n\t5. Cancel edit." << endl << endl;
			int choice = 0;
			cout << "Enter selection> ";
			cin >> choice;
			flushCin();
			if (choice == 1) {
				cout << "Please enter a new name for "+labels[id]+":" << endl;
				string name = "";
				getline(cin, name, '\n');
				labels[id] = name;
				cout << "Name changed to: " + labels[id] << endl;
				unsavedChanges = true;
				
				break;
			}
			else if (choice == 2) {
				cout << "Changing passphrase for " + labels[id]+"..." << endl;
				passwords[id] = generatePassphrase(wordlist);
				cout << "Passphrase changed." << endl;
				dates[id] = getCurrentDateTime();
				unsavedChanges = true;
				break;
			}
			else if (choice == 3) {
				cout << "Are you sure you want to delete " + labels[id] + "? (yes/no)" << endl;
				
				
				
					string input = "";
					getline(cin, input);
					if (input == "yes") {
						labels[id] = labels.back();
						passwords[id] = passwords.back();
						dates[id] = dates.back();
						notes[id] = notes.back();
						labels.pop_back();
						passwords.pop_back();
						dates.pop_back();
						notes.pop_back();
						cout << "Entry deleted." << endl;
						unsavedChanges = true;
						break;
					}
					
				
			}
			else if (choice == 4) {
				cout << "Editing notes for " << labels[id] << "..." << endl;
				cout << "Previously, the notes for this passphrase were:" << endl << notes[id] << endl;
				cout << "Enter new line of notes now:" << endl;
				string notes;
				getline(cin, notes, '\n');
				cout << "Notes updated." << endl;
			}
			else if (choice == 5) {

				break;
			}
			else {
				cout << "Error: invalid choice. Please try again." << endl;
			}
		}
	}
};

string getCurrentDateTime() {
	char buffer[80];
	time_t raw;
	struct tm * timeinfo;
	time(&raw);
	timeinfo = localtime(&raw);
	strftime(buffer, 80, "%B %e %Y %I:%M:%S %p", timeinfo);
	string result = string(buffer);
	return result;
}

int toInt(string value) {
	stringstream buffer;
	buffer << value;
	int result;
	buffer >> result;
	return result;
}


int searchForEntry(PasswordDatabase &db) {
	string input = "";
	int numOfResults = 0;
	vector<int> matchedResultIDs;
	while (true) {
		numOfResults = 0;
		matchedResultIDs.clear();
		cout << "Please enter search term>";
		getline(cin, input);
		for (int i = 0; i < db.getLabels().size(); i++) {
			if (db.getLabels()[i].find(input) != string::npos ) {
				numOfResults++;
				matchedResultIDs.push_back(i);
			}
		}
		if (numOfResults > 0) {
			cout << numOfResults << " result/s found:" << endl;
			break;
		}
		else {
			cout << "No results were found. Would you like to try again? (Y/N)";
			getline(cin, input);
			if (input != "Y") return -1;
			cout << endl;
		}
	}
	for (int i = 0; i < matchedResultIDs.size(); i++) {
		cout << (i + 1) << ". ";
		db.printCensoredEntry(matchedResultIDs[i]);
	}
	while (true) {
		cout << endl << "Please select an entry>";
		getline(cin, input);
		if (toInt(input) >= 1 && toInt(input) <= matchedResultIDs.size()) {
			return matchedResultIDs[toInt(input) - 1];
		}
		else {
			cout << "Error: Invalid input. Please try again." << endl;
		}
	}
}

void printBox(string value) {
	const char topLeft = 201;
	const char topRight = 187;
	const char bottomLeft = 200;
	const char bottomRight = 188;
	const char topBottom = 205;
	const char leftRight = 186;

	stringstream buffer;
	buffer << value;
	vector<string> lines;
	while (!buffer.eof()) {
		string line = "";
		getline(buffer, line);
		lines.push_back(line);
	}

	int maxLength = 0;
	for (string s : lines) {
		if (s.length() > maxLength) maxLength = s.length();
	}

	string floor = "";
	floor.assign(maxLength, topBottom);

	string top = "";
	top.push_back(topLeft);
	top += floor;
	top.push_back(topRight);

	cout << top << endl;

	for (string s : lines) {
		cout << leftRight << setw(maxLength) << setfill(' ') << left << s << leftRight << endl;
	}

	string bottom = "";
	bottom.push_back(bottomLeft);
	bottom += floor;
	bottom.push_back(bottomRight);

	cout << bottom << endl;

}

//Random num generator declared here to make sure it is always the same regardless of platform or toolset.
static uint32_t nextSeed = 1;
uint32_t random(void) { //RANDOM_MAX is 32767
	nextSeed = nextSeed * 1103515245 + 12345;
	return (uint32_t)(nextSeed / 65536) % 32768;
}
void seedGenerator(uint32_t seed) {
	nextSeed = seed;
}

string encryptDecrypt(string data, uint32_t key, string passphrase) {
	string result = data;
	seedGenerator(key);
	if (passphrase.length() != 0) {
		for (int i = 0; i < data.length(); i++) {
			result[i] ^= passphrase[random() % passphrase.length()];
		}
	}
	return result;
}

void flushCin() {
	cin.clear();
	cin.ignore(INT_MAX, '\n');
}

uint32_t generateNewKey() {
	uint32_t result = 0;
	cout << "New source of entropy required." << endl;
	cout << "Please start pressing random characters on your keyboard" << endl;
	for (int i = 0; i < 32; i++) {
		cout << (32 - i) << " random characters remaining...";
		char c = _getch();
		if (c % 2 == 0) {
			result += 0;
		}
		else {
			result += 1;
		}
		result = result << 1;
		cout << endl;
	}
	cout << "Done. Press enter to continue...";
	while (_getch() != 0x0D);
	cout << endl;
	return result;
}

string generatePassphrase(vector<string> &wordlist) {
	string result = "";
	string append = "";
	int words = 0;
	while (true) {
		cout << "How many words should the passphrase be?";
		cin >> words;
		if (words > 0) break;
		cout << "Error: number of words must be greater than zero. Try again." << endl;
	}
	cout << "Do you wish to append any special characters to the end of your passphrase? (y/n)";
	if (_getch() == 0x79) {
		cout << endl << "Please add any characters you would like to append to your passphrase: " << endl;
		cin >> append;
	}
	cout << endl;
	seedGenerator(generateNewKey());
	for (int i = 0; i < words; i++) {
		result += wordlist[random() % wordlist.size()];
		if (i < words - 1 || !append.empty()) result += " ";
	}
	result += append;
	cout << "Done. Your new passphrase is:" << endl;
	cout << result << endl;
	cout << endl << endl << "-Statistics for this passphrase-" << endl;
	double totalPossibleWordCombos = pow(wordlist.size(), words);
	cout << "Current wordlist is " << wordlist.size() << " words long." << endl;
	cout << "Based of passphrase length and size of your wordlist total possible passphrases:" << endl << totalPossibleWordCombos << endl;
	cout << "Assuming your adversary is capable of a trillion guesses per second and they know your wordlist, length of passphrase, and any characters you added to the end-\nAverage time needed to crack this passphrase in years:" << endl;
	double guessesPerYear = 1000000000000.0*60.0*60.0*24.0*365.0;
	double maxYearsToGuess = totalPossibleWordCombos / guessesPerYear;
	double averageYearsToGuess = maxYearsToGuess / 2.0;
	cout << "About " << averageYearsToGuess << " years. Heat death of the universe in about 1x10^100 years." << endl;

	cout << "Press enter to continue...";
	while (_getch() != 0x0D);
	cout << endl;
	return result;
}

uint32_t getKeyFromUser() {
	string input;
	bool keepLooping = true;
	while (keepLooping) {
		cout << "Please enter your key:";
		input = "";
		for (int i = 0; i < 8;i++) {
			char c = _getch();
			if (c < 48 || (c > 57 && c < 97) || c>102) {
				//if invalid character has been entered
				cout << "Error: invalid character entered. Please try again." << endl;
				break;
			}
			else {
				input += c;
			}
			if (i == 7) keepLooping = false;
		}
	}
	cout << endl;
	uint32_t result = 0;
	for (int i = 7; i>=0; i--) {
		int n = 0;
		char c = input[i];
		if (c >= 0x30 && c <= 0x39) {
			//this is a number
			n = c - 0x30;
		}
		else {
			//this is a letter
			n = c - 87;
		}
		n *= pow(16, 7 - i);
		result += n;
	}
	return result;
}

string getPassphraseFromUser() {
	string result;
	cout << "Please enter your passphrase: ";
	char c = 0;
	while (true) {
		c = _getch();
		if (c == 0xD) break;
		result += c;
	}
	cout << endl;
	return result;
}

string toHex(uint32_t num) {
	stringstream buffer;
	buffer << hex << num;
	string result(buffer.str());
	cout << dec;
	return result;
}

string decryptFile(PasswordDatabase &db, uint32_t &key, string &passphrase, string encryptedData) {
	string result = "";
	while (true) {
		key = getKeyFromUser();
		passphrase = getPassphraseFromUser();
		cout << "Decrypting file..." << endl;
		result=encryptDecrypt(encryptedData, key, passphrase);
		if (result.substr(0, 26) == "ABCDEFGHIJKLMNOPQRSTUVWXYZ") break; //success
		cout << "Error: incorrect key or passphrase. Please try again." << endl;
	}
	cout << "Decryption successful." << endl;
	return result;
}

void writeAndEncryptFile(PasswordDatabase db, uint32_t key, string passphrase, string filename) {
	cout << "Saving and encrypting file..." << endl;
	string data = "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	for (int i = 0; i < db.getLabels().size(); i++) {
		data += db.getLabels()[i] + "\n";
		data += db.getPasswords()[i] + "\n";
		data += db.getDates()[i]+"\n";
		data += db.getNotes()[i];
		if (i != db.getLabels().size() - 1) data += "\n";
	}
	data = encryptDecrypt(data, key, passphrase);

	ofstream passwordFile(filename, ofstream::out | ofstream::binary);
	//passwordFile << data;
	passwordFile.write(data.c_str(), sizeof(char)*data.size());
	passwordFile.close();

	cout << "Done. " << endl;
}

void changeKeyPass(PasswordDatabase &db, uint32_t &key, string &passphrase, vector<string> &wordlist) {
	while (true) {
		cout << "Password database options:\n\t1. Change key.\n\t2. Change passphrase.\n\t3. Cancel." << endl << endl;
		int choice = 0;
		cout << "Enter selection> ";
		cin >> choice;
		flushCin();
		if (choice == 1) {
			cout << "Changing key..." << endl;
			key = generateNewKey();
			cout << "Your new key is: " + toHex(key) << endl;
			cout << "Do not lose this!!" << endl;
			db.flagUnsavedChanges();
			break;
		}
		else if (choice == 2) {
			cout << "Changing passphrase..." << endl;
			passphrase = generatePassphrase(wordlist);
			cout << "Passphrase changed! Do not lose the new passphrase!!!" << endl;
			db.flagUnsavedChanges();
			break;
		}
		else if (choice == 3) {
			break;

		}
		else {
			cout << "Error: invalid choice. Please try again." << endl;
		}
	}
}

int main(int argc, char* argv[]) {

	string nameOfWordList = "wordlist.txt";
	string nameOfPasswords = "passwords.txt";

	vector<string> wordlist;
	string encryptedPasswordsData;

	PasswordDatabase db;
	uint32_t key = 0;
	string passphrase = "";

	if (argc == 1) {
		//use default file names and locations
	}
	else if(argc==3)  {
		nameOfWordList = argv[1];
		nameOfPasswords = argv[2];
	}
	else {
		//illegal number of arguments. Display help and halt.
		cout << "Illegal number of cmd-line arguments. Correct usage:" << endl << endl
			<< argv[0] << endl
			<< "\tStarts the program with default wordlist and password filenames/locations." << endl
			<< "\tThe default files are: \"" + nameOfWordList + "\" and \"" + nameOfPasswords + "\"." << endl
			<< argv[0] << " [Path to wordlist] [Path to passwords]" << endl;
		return 0;
	}

	ifstream wordListFile;
	wordListFile.open(nameOfWordList);
	if (!wordListFile) {
		cout << "Error: enable to open \"" + nameOfWordList + "\". Halting." << endl;
		return 0;
	} 
	while (!wordListFile.eof()) {
		string word="";
		getline(wordListFile, word, '\n');
		wordlist.push_back(word);
	}
	wordListFile.close();

	ifstream passwordsFile;
	passwordsFile.open(nameOfPasswords, ios::binary);
	if (!passwordsFile) {
		cout << "Unable to open passwords file. Entering first time setup." << endl;
		cout << "Creating key..." << endl;
		key = generateNewKey();
		cout << "Done creating key. Creating passphrase..." << endl;
		passphrase = generatePassphrase(wordlist);
		cout << "Passphrase created." << endl
			<< endl << "-----------------------------" << endl
			<< "KEY: " << setw(8) << setfill('0') << right << hex << key << endl
			<< "PASSPHRASE: " << passphrase << endl
			<< "-----------------------------" << endl << endl
			<< "***NOTE: These are required to access your passwords." << endl
			<< "Recovery of any password is not possible if you lose these." << endl
			<< "Type \"yes\" if you acknowledge this notice: ";
		flushCin();
		while (true) {
			string input = "";
			getline(cin, input);
			if (input == "yes") break;
			cout << "You must type \"yes\" and agree to the notice to continue using this software." << endl
				<< "Type \"yes\" if you acknowledge this notice: ";
		}
		writeAndEncryptFile(db, key, passphrase, nameOfPasswords);
		cout << "Password database created." << endl;
	}
	else {
		//encryptedPasswordsData << passwordsFile.rdbuf();

		encryptedPasswordsData.assign((istreambuf_iterator<char>(passwordsFile)),
			(istreambuf_iterator<char>()));

		string decrypted=decryptFile(db, key, passphrase, encryptedPasswordsData);
		passwordsFile.close();
		db = PasswordDatabase(decrypted);
	}
	
	cout << endl;
	string greeting = "Welcome to diceware password generator and manager!\nSoftware by Metgame (Austin Scott) V1.0";
	printBox(greeting);
	cout << endl;

	bool keeplooping = true;
	while (keeplooping) {
		cout << "Menu:\n\t1. Add new passphrase.\n\t2. View existing passphrase.\n\t3. Edit existing passphrases.\n\t4. Edit database key/passphrase.\n\t5. Save and Quit" << endl;
		if (db.hasUnsavedChanges()) {
			cout << "***WARNING: You have unsaved changes." << endl;
		}
		else {
			cout << endl;
		}
		int choice = 0;
		cout << "Choose option: ";
		cin >> choice;
		int selection = 0;
		flushCin();
		switch (choice) {
		case 1:
			db.addNewPassphrase(wordlist);
			break;
		case 2:
			selection = searchForEntry(db);
			if (selection == -1) break;
			db.printEntry(selection);
			break;
		case 3:
			selection = searchForEntry(db);
			if (selection == -1) break;
			db.editEntry(selection, wordlist);
			break;
		case 4:
			changeKeyPass(db, key, passphrase, wordlist);
			break;
		case 5:
			writeAndEncryptFile(db, key, passphrase, nameOfPasswords);
			keeplooping = false;
			break;
		default:
			cout << "Error: invalid choice. Please choose again." << endl;
		}
	}

}