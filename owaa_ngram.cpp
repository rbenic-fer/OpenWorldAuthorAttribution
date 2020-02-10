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

	InstructionPattern(const vector<BinaryInstruction> &arr, int st, int en): instructions() {
		BitStringMap argMap;
		int argCount = 0;
		for(int i = st; i < en; i++) {
			instructions.push_back(SimplifiedInstruction());
			BitStringMap::iterator mapEntry = opCodeMap.find(arr[i].opCode);
			if(mapEntry == opCodeMap.end()) {
				opCodeMap[arr[i].opCode] = opCodeCount;
				(*instructions.rbegin()).opCode = opCodeCount;
				opCodeCount++;
			}
			else
				(*instructions.rbegin()).opCode = mapEntry -> second;
			for(int j = 0; j < (int)arr[i].arguments.size(); j++) {
				mapEntry = argMap.find(arr[i].arguments[j]);
				if(mapEntry == argMap.end()) {
					argMap[arr[i].arguments[j]] = argCount;
					(*instructions.rbegin()).addArgument(argCount);
					argCount++;
				}
				else
					(*instructions.rbegin()).addArgument(mapEntry -> second);
			}
		}
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

InstructionPatternFrequencyMap mapNGramsWithN(const vector<BinaryInstruction> &code, int n) {
	InstructionPatternFrequencyMap sol;
	for(int i = 0; i <= (int)code.size() - n; i++)
		sol.add(InstructionPattern(code, i, i + n));
	return sol;
}

double sqr(double x) {
	return x * x;
}

double frequencyDistanceSquared(const InstructionPatternFrequencyMap &m1,
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
	return sol;
}

double frequencyDistanceTotal(const vector<InstructionPatternFrequencyMap> &v1,
                              const vector<InstructionPatternFrequencyMap> &v2) {
	double sol = 0.0;
	for(int i = 0; i < (int)v1.size(); i++)
		sol += frequencyDistanceSquared(v1[i], v2[i]);
	return sqrt(sol) / (double)v1.size();
}

const int MIN_N = 3, MAX_N = 12;

vector<InstructionPatternFrequencyMap> mapNGrams(const vector<BinaryInstruction> &code) {
	vector<InstructionPatternFrequencyMap> sol;
	for(int i = MIN_N; i <= MAX_N; i++)
		sol.push_back(mapNGramsWithN(code, i));
	return sol;
}

int main(int argc, char **argv) {
	if(argc < 3) {
		cerr << "Please provide at least 2 files for comparison" << endl;
		return 0;
	}
	vector<BinaryInstructionFormat> formats = parseFormats("formats.txt");
	vector<vector<InstructionPatternFrequencyMap>> maps;
	for(int i = 1; i < argc; i++)
		maps.push_back(mapNGrams(parseInstructions(argv[i], formats)));
	for(int i = 1; i < argc - 1; i++)
		for(int j = i + 1; j < argc; j++) {
			double dist = frequencyDistanceTotal(maps[i - 1], maps[j - 1]);
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
