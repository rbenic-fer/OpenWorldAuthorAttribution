#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>

using std::cerr;
using std::cout;
using std::endl;
using std::getline;
using std::ifstream;
using std::ios;
using std::size_t;
using std::sqrt;
using std::string;
using std::unordered_map;
using std::vector;

struct BitString {
	vector<char> bytes;
	int bitIndex;

	BitString(): bytes(0), bitIndex(8) {}

	void addBit(int bit) {
		if(bitIndex == 8) {
			bytes.push_back(0);
			bitIndex = 0;
		}
		if(bit & 1)
			(*bytes.rbegin()) ^= (1 << bitIndex);
		bitIndex++;
	}

	int getBit(int idx) const {
		return (bytes[idx >> 3] >> (idx & 7)) & 1;
	}

	size_t size() const {
		return ((bytes.size() - 1) << 3) + bitIndex;
	}
};

bool operator==(const BitString &s1, const BitString &s2) {
	if(s1.bytes.size() != s2.bytes.size())
		return false;
	if(s1.bitIndex != s2.bitIndex)
		return false;
	for(int i = 0; i < (int)s1.bytes.size(); i++)
		if(s1.bytes[i] != s2.bytes[i])
			return false;
	return true;
}

bool operator!=(const BitString &s1, const BitString &s2) {
	if(s1.bytes.size() != s2.bytes.size())
		return true;
	if(s1.bitIndex != s2.bitIndex)
		return true;
	for(int i = 0; i < (int)s1.bytes.size(); i++)
		if(s1.bytes[i] != s2.bytes[i])
			return true;
	return false;
}

struct BitStringHash {
	size_t operator()(const BitString &bitString) const {
		size_t sol = 0;
		int n = (int)bitString.size();
		for(int i = 0; i < n; i++)
			sol = 10007 * sol + bitString.getBit(i);
		return sol;
	}
};

typedef unordered_map<BitString, int, BitStringHash> BitStringMap;

struct BinaryInstruction {
	BitString opCode;
	vector<BitString> arguments;

	void addArgument(const BitString &argument) {
		arguments.push_back(argument);
	}
};

struct BinaryInstructionFormat {
	string str;

	BinaryInstructionFormat() {}

	BinaryInstructionFormat(const string &_str): str(_str) {}

	size_t size() const {
		return str.size();
	}

	bool match(const BitString &bitString) const {
		if(bitString.size() != str.size())
			return false;
		for(int i = 0; i < (int)str.size(); i++) {
			int bit = bitString.getBit(i);
			if((!bit && str[i] == '1') || (bit && str[i] == '0'))
				return false;
		}
		return true;
	}

	bool matchString(const string &str2) const {
		if(str2.size() != str.size())
			return false;
		for(int i = 0; i < (int)str.size(); i++)
			if((str2[i] == '0' && str[i] == '1') || (str2[i] == '1' && str[i] == '0'))
				return false;
		return true;
	}

	BinaryInstruction parseInstruction(const BitString &bitString) const {
		BinaryInstruction sol;
		int argIndex = -1;
		for(int i = 0; i < (int)str.size(); i++) {
			int bit = bitString.getBit(i);
			if(str[i] == '0' || str[i] == '1')
				sol.opCode.addBit(bit);
			else {
				if(!i || str[i - 1] != str[i]) {
					sol.addArgument(BitString());
					argIndex++;
				}
				sol.arguments[argIndex].addBit(bit);
			}
		}
		return sol;
	}
};

struct SimplifiedInstruction {
	int opCode;
	vector<int> arguments;

	void addArgument(int argument) {
		arguments.push_back(argument);
	}
};

bool operator!=(const SimplifiedInstruction &i1, const SimplifiedInstruction &i2) {
	if(i1.opCode != i2.opCode)
		return true;
	if(i1.arguments.size() != i2.arguments.size())
		return true;
	for(int i = 0; i < (int)i1.arguments.size(); i++)
		if(i1.arguments[i] != i2.arguments[i])
			return true;
	return false;
}

struct SimplifiedInstructionHash {
	size_t operator()(const SimplifiedInstruction &inst) const {
		size_t sol = inst.opCode;
		for(int i = 0; i < (int)inst.arguments.size(); i++)
			sol = 10007 * sol + inst.arguments[i];
		return sol;
	}
};

BitStringMap opCodeMap;
int opCodeCount = 0;

struct InstructionPattern {
	vector<SimplifiedInstruction> instructions;

	InstructionPattern(): instructions() {}

	size_t size() const {
		return instructions.size();
	}

	bool match(vector<BinaryInstruction> arr, int st) const {
		if((int)arr.size() - st < (int)instructions.size())
			return false;
		vector<BitString> args;
		for(int i = 0; i < (int)instructions.size(); i++) {
			BitStringMap::iterator mapEntry = opCodeMap.find(arr[st + i].opCode);
			if(mapEntry == opCodeMap.end() || (mapEntry -> second) != instructions[i].opCode)
				return false;
			for(int j = 0; j < (int)instructions[i].arguments.size(); j++) {
				if((int)args.size() <= instructions[i].arguments[j])
					args.resize(instructions[i].arguments[j] + 1);
				if(args[instructions[i].arguments[j]] == BitString())
					args[instructions[i].arguments[j]] = arr[st + i].arguments[j];
				else if(args[instructions[i].arguments[j]] != arr[st + i].arguments[j])
					return false;
			}
		}
		return true;
	}
};

bool operator==(const InstructionPattern &p1, const InstructionPattern &p2) {
	if(p1.instructions.size() != p2.instructions.size())
		return false;
	for(int i = 0; i < (int)p1.instructions.size(); i++)
		if(p1.instructions[i] != p2.instructions[i])
			return false;
	return true;
}

typedef unordered_map<char, int> CharMap;

InstructionPattern parsePattern(const vector<string> strings,
                                const vector<BinaryInstructionFormat> &formats) {
	InstructionPattern sol;
	CharMap charMap;
	for(int i = 0; i < (int)strings.size(); i++) {
		sol.instructions.push_back(SimplifiedInstruction());
		for(int j = 0; j < (int)formats.size(); j++)
			if(formats[j].matchString(strings[i])) {
				int argIndex = 0;
				BitString opCode;
				for(int k = 0; k < (int)formats[j].str.size(); k++) {
					if(formats[j].str[k] == '0')
						opCode.addBit(0);
					else if(formats[j].str[k] == '1')
						opCode.addBit(1);
					else if(!k || formats[j].str[k - 1] != formats[j].str[k]) {
						CharMap::const_iterator mapEntry = charMap.find(strings[i][k]);
						if(mapEntry == charMap.end()) {
							charMap[strings[i][k]] = argIndex;
							(*sol.instructions.rbegin()).addArgument(argIndex);
							argIndex++;
						}
						else
							(*sol.instructions.rbegin()).addArgument(mapEntry -> second);
					}
				}
				BitStringMap::iterator mapEntry = opCodeMap.find(opCode);
				if(mapEntry == opCodeMap.end()) {
					opCodeMap[opCode] = opCodeCount;
					(*sol.instructions.rbegin()).opCode = opCodeCount;
					opCodeCount++;
				}
				else
					(*sol.instructions.rbegin()).opCode = mapEntry -> second;
				break;
			}
	}
	return sol;
}

struct InstructionPatternHash {
	SimplifiedInstructionHash instructionHash;

	size_t operator()(const InstructionPattern &pattern) const {
		size_t sol = 0;
		for(int i = 0; i < (int)pattern.instructions.size(); i++)
			sol = 10007 * sol + instructionHash(pattern.instructions[i]);
		return sol;
	}
};

typedef unordered_map<InstructionPattern, int, InstructionPatternHash> InstructionPatternMap;

struct InstructionPatternFrequencyMap {
	InstructionPatternMap m;
	int n = 0;

	void add(const InstructionPattern &element) {
		m[element]++;
		n++;
	}
};

struct BitInputStream {
	ifstream inputStream;
	char byte;
	int bitIndex;

	BitInputStream(const string &fileName): inputStream(fileName, ios::binary), byte(0),
	                                        bitIndex(0) {}

	int getBit() {
		if(!bitIndex) {
			if(!inputStream.read((char *)&byte, 1))
				return -1;
			bitIndex = 8;
		}
		return (byte >> (--bitIndex)) & 1;
	}
};

vector<BinaryInstructionFormat> parseFormats(const string &fileName) {
	ifstream inputFile(fileName);
	vector<BinaryInstructionFormat> sol;
	while(!inputFile.eof()) {
		string str;
		getline(inputFile, str);
		if(str != "")
			sol.push_back(BinaryInstructionFormat(str));
	}
	return sol;
}

vector<InstructionPattern> parseDictionary(const string &fileName,
                                           const vector<BinaryInstructionFormat> &formats) {
	ifstream inputFile(fileName);
	vector<InstructionPattern> sol;
	vector<string> strings;
	while(!inputFile.eof()) {
		string str;
		getline(inputFile, str);
		if(str.empty()) {
			sol.push_back(parsePattern(strings, formats));
			strings = vector<string>();
		}
		else
			strings.push_back(str);
	}
	if(!strings.empty())
		sol.push_back(parsePattern(strings, formats));
	return sol;
}

vector<BinaryInstruction> parseInstructions(string fileName,
                                            const vector<BinaryInstructionFormat> &formats) {
	BitInputStream inputStream(fileName);
	vector<BinaryInstruction> sol;
	BitString bitString;
	int bit = inputStream.getBit();
	while(bit != -1) {
		bitString.addBit(bit);
		for(int i = 0; i < (int)formats.size(); i++)
			if(formats[i].match(bitString)) {
				sol.push_back(formats[i].parseInstruction(bitString));
				bitString = BitString();
			}
		bit = inputStream.getBit();
	}
	return sol;
}

InstructionPatternFrequencyMap mapDictionaryPatterns(const vector<BinaryInstruction> &code,
                                                     const vector<InstructionPattern> &dict) {
	InstructionPatternFrequencyMap sol;
	int i = 0;
	while(i < (int)code.size()) {
		for(int j = 0; j < (int)dict.size(); j++)
			if(dict[j].match(code, i)) {
				sol.add(dict[j]);
				i += dict[j].size() - 1;
				break;
			}
		i++;
	}
	return sol;
}

double sqr(double x) {
	return x * x;
}

double frequencyDistance(const InstructionPatternFrequencyMap &m1,
                         const InstructionPatternFrequencyMap &m2) {
	double sol = 0.0;
	for(InstructionPatternMap::const_iterator it = m1.m.cbegin(); it != m1.m.cend(); it++) {
		InstructionPatternMap::const_iterator it2 = m2.m.find(it -> first);
		if(it2 != m2.m.end())
			sol += sqr((it -> second) / (double)m1.n - (it2 -> second) / (double)m2.n);
	}
	for(InstructionPatternMap::const_iterator it = m2.m.cbegin(); it != m2.m.cend(); it++)
		if(m1.m.find(it -> first) == m1.m.end())
			sol += sqr((it -> second) / (double)m2.n);
	return sqrt(sol);
}

int main(int argc, char **argv) {
	if(argc < 3) {
		cerr << "Please provide at least 2 files for comparison" << endl;
		return 0;
	}
	vector<BinaryInstructionFormat> formats = parseFormats("formats.txt");
	vector<InstructionPattern> dict = parseDictionary("dict.txt", formats);
	vector<InstructionPatternFrequencyMap> maps;
	for(int i = 1; i < argc; i++)
		maps.push_back(mapDictionaryPatterns(parseInstructions(argv[i], formats), dict));
	for(int i = 1; i < argc - 1; i++)
		for(int j = i + 1; j < argc; j++) {
			double dist = frequencyDistance(maps[i - 1], maps[j - 1]);
			cout << "Programs " << argv[i] << " and " << argv[j];
			if(dist < 0.5)
				cout << " are work of the same author with probability " << 100 * (1 - dist)
				     << "%";
			else
				cout << " are not work of the same author with probability " << 100 * dist
				     << "%";
			cout << endl;
		}
	return 0;
}
