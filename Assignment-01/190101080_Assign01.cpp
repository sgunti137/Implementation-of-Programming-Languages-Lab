#include <iostream>
#include <cstdio>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

// programLength -> Length of program, startAdd -> starting address of the program
int LOCCTR = 0, programLength = 0, startAdd = 0;
map<string, int> SYMTAB; // symbol table
map<string, string> OPTAB; // opcode table

// initialise the opcode table.
void initializeOptab() {
  OPTAB["ADD"] = "18";
  OPTAB["AND"] = "40";
  OPTAB["COMP"] = "28";
  OPTAB["DIV"] = "24";
  OPTAB["J"] = "3C";
  OPTAB["JEQ"] = "30";
  OPTAB["JGT"] = "34";
  OPTAB["JLT"] = "38";
  OPTAB["JSUB"] = "48";
  OPTAB["LDA"] = "00";
  OPTAB["LDCH"] = "50";
  OPTAB["LDL"] = "08";
  OPTAB["LDX"] = "04";
  OPTAB["MUL"] = "20";
  OPTAB["OR"] = "44";
  OPTAB["RD"] = "D8";
  OPTAB["RSUB"] = "4C";
  OPTAB["STA"] = "0C";
  OPTAB["STCH"] = "54";
  OPTAB["STL"] = "14";
  OPTAB["STSW"] = "E8";
  OPTAB["STX"] = "10";
  OPTAB["SUB"] = "1C";
  OPTAB["TD"] = "E0";
  OPTAB["TIX"] = "2C";
  OPTAB["WD"] = "DC";
}

// get label, opcode and operand from the input
void parseInput(int index, string& data, string& label, string& opcode, string& operand) {
  int counter = 0;
  vector<string> answer(3, "");
  if(data[0] == '.') {
    label = ".";
    return;
  }
  for(int i = index; i < data.size(); ++i) {
    if(data[i] != ' ') {
      answer[counter].push_back(data[i]);
    } else {
      while(data[i] == ' ') i++;
      counter++;
      i--;
    }
  }
  label = answer[0];
  opcode = answer[1];
  operand = answer[2];
  return;
}

// convert a hexadecimal string to decimal integer
int hexToInt(string operand) {
  int res = 0, power = 1;
  for(int i = operand.size() - 1; i >= 0; --i) {
    if(operand[i] >= '0' && operand[i] <= '9') {
      res += (power * (operand[i] - '0'));
    } else {
      res += (power * (operand[i] - 'A' + 10));
    }
    power *= 16;
  }
  return res;
}

// convert decimal integer into hexadecimal string
string intToHex(int n) {
  string res = "";
  while(n) {
    int rem = n % 16;
    if(rem < 10) {
      res.push_back(rem + '0');
    } else {
      res.push_back(char(rem + 'A' - 10));
    }
    n /= 16;
  }
  reverse(res.begin(), res.end());
  return res;
}

// convert a string in decimal into decimal integer
int convertToInt(string& s) {
  int res = 0, power = 1;
  for(int i = s.size() - 1; i >= 0; --i) {
    res += (power * (s[i] - '0'));
    power *= 10;
  }
  return res;
}

// returns the length of constant when encountered a BYTE opcode.
int lengthOfConstant(string& s) {
  if((s[0] == 'X' || s[0] == 'x') && s[1] == '\'') {
    return 1;
  }
  int i = 0;
  if((s[0] == 'C' || s[0] == 'c') && s[1] == '\'') {
    for(i = 2; i < s.size(); ++i) {
      if(s[i] == '\'') {
        i -= 2;
        break;
      }
    }
  }
  return i;
}

// match the required size of string by adding a certain character at the beginning of the string
void matchSizeFront(string& s, int size, char waste) {
  reverse(s.begin(), s.end());
  while(s.size() < size) s.push_back(waste);
  reverse(s.begin(), s.end());
  return;
}

// match the required size of string by adding a certain character at the end of the string
void matchSizeBack(string& s, int size, char waste) {
  while(s.size() < size) s.push_back(waste);
  return;
}

// returns the object code when the opcode is either WORD or BYTE.
string constantString(string operand) {
  string res = "";
  if((operand[0] == 'C' || operand[0] == 'c') && operand[1] == '\'') {
    for(int i = 2; i < operand.size(); ++i) {
      if(operand[i] == '\'') {
        break;
      }
      res += intToHex(int(operand[i]));
    }
    return res;
  }
  if((operand[0] == 'X' || operand[0] == 'x') && operand[1] == '\'') {
    for(int i = 2; i < operand.size(); ++i) {
      if(operand[i] == '\'') {
        break;
      }
      res += operand[i];
    }
    return res;
  }
  res = intToHex(convertToInt(operand));
  matchSizeFront(res, 6, '0');
  return res;
}

// function for pass1
void pass1() {
  cout << "PASS 1 STARTED" << endl;
  // opening files.
  ifstream input("input.txt");
  ofstream intermediate("intermediate.txt");
  string data;

  // taking input from input.txt file.
  while(getline(input, data)) {
    string label, opcode, operand;
    parseInput(0, data, label, opcode, operand);

    // handling the case when opcode is START
    if(opcode == "START") {
      LOCCTR = hexToInt(operand);
      programLength = LOCCTR;
      startAdd = LOCCTR;
      matchSizeBack(operand, 7, ' ');
      operand += data;
      intermediate << operand << endl;
      continue;
    }

    // handling the case when opcode is END
    // break the while loop as this is the last line.
    if(opcode == "END") {
      intermediate << "       " << data << endl;
      break;
    }

    // handling the case when the input line is a comment line. 
    if(label == ".") {
      intermediate << data << endl;
      continue;
    }

    // if there is a symbol in label, then update the symbol table
    if(label != "") {
      if(SYMTAB.find(label) != SYMTAB.end()) {
        cout << "ERROR: Duplicate Label" << endl;
        break;
      } else {
        SYMTAB[label] = LOCCTR;
      }
    }

    // write to the intermediate file with adding location counter.
    string res = intToHex(LOCCTR);
    matchSizeBack(res, 7, ' ');
    res += data;
    intermediate << res << endl;

    // update the location counter based on the opcode.
    if(OPTAB.find(opcode) != OPTAB.end()) {
      LOCCTR += 3;
    } else if(opcode == "WORD") {
      LOCCTR += 3;
    } else if(opcode == "RESW") {
      LOCCTR += (3 * convertToInt(operand));
    } else if(opcode == "RESB") {
      LOCCTR += convertToInt(operand);
    } else if(opcode == "BYTE") {
      LOCCTR += lengthOfConstant(operand);
    } else {
      cout << "ERROR: Invalid Operand" << endl;
      break;
    }
  }
  // store the length of the program, used in pass2.
  programLength = LOCCTR - programLength;
  
  // closing the files.
  input.close();
  intermediate.close();
  cout << "PASS 1 DONE" << endl;
}

// function for pass2
void pass2() {
  cout << "PASS 2 STARTED" << endl;

  // opening files.
  ifstream intermediate("intermediate.txt");
  ofstream listing("listing.txt");
  ofstream outputObj("output.o");
  ofstream caretFile("objectWithCarets.txt");

  // initializing variables.
  string data, textRecord = "", startLoc = "";
  string caretHeader = " ^     ^ ", caretLine = caretHeader;
  
  // taking input from intermediate.txt file.
  while(getline(intermediate, data)) {
    
    // handling the case of the comment line input.
    if(data[0] == '.') {
      listing << data << endl;
      continue;
    }
    string objectCode = "";
    string label, opcode, operand, locCounter;
    locCounter = data.substr(0, 4);
    // get the label, opcode, operand from input.
    parseInput(7, data, label, opcode, operand);
    
    // handling the case when opcode is START
    if(opcode == "START") {
      listing << data << endl;
      matchSizeBack(label, 6, ' ');
      matchSizeFront(operand, 6, '0');
      string length = intToHex(programLength);
      matchSizeFront(length, 6, '0');
      outputObj << "H" << label << operand << length << endl;
      caretFile << "H" << label << operand << length << endl;
      caretFile << " ^     ^     ^" << endl;
      startLoc = operand;
      continue;
    }

    // handling the case when opcode is END
    if(opcode == "END") {
      listing << data << endl;
      if(textRecord.size()) {
        matchSizeFront(startLoc, 6, '0');
        string hexSize = intToHex(textRecord.size() / 2);
        matchSizeFront(hexSize, 2, '0');
        textRecord = "T" + startLoc + hexSize + textRecord;
        outputObj << textRecord << endl;
        caretFile << textRecord << endl;
        caretFile << caretLine << endl;
      }
      string end = intToHex(startAdd);
      matchSizeFront(end, 6, '0');
      textRecord = "E" + end;
      outputObj << textRecord << endl;
      caretFile << textRecord << endl;
      caretFile << " ^" << endl;
      break;
    }

    // find the object code based on opcode and operand.
    if(OPTAB.find(opcode) != OPTAB.end()) {
      if(SYMTAB.find(operand) != SYMTAB.end()) {
        objectCode = OPTAB[opcode] + intToHex(SYMTAB[operand]);
      } else if(operand == "BUFFER,X") {
        objectCode = OPTAB[opcode] + intToHex(SYMTAB["BUFFER"] + hexToInt("8000"));
      } else {
        objectCode = OPTAB[opcode];
        matchSizeBack(objectCode, 6, '0');
      }
    } else if(opcode == "BYTE" || opcode == "WORD") {
      objectCode = constantString(operand);
    }
    matchSizeBack(data, 35, ' ');
    data += objectCode;
    listing << data << endl;
    if(startLoc == "" && objectCode != "") {
      startLoc = locCounter;
    }

    // update the object files when the textrecord size is exceeded or objectcode is 0.
    if((objectCode.size() == 0) || (textRecord.size() + objectCode.size() > 60)) {
      if(textRecord.size() == 0) continue;
      matchSizeFront(startLoc, 6, '0');
      string hexSize = intToHex(textRecord.size() / 2);
      matchSizeFront(hexSize, 2, '0');
      textRecord = "T" + startLoc + hexSize + textRecord;
      outputObj << textRecord << endl;
      caretFile << textRecord << endl;
      caretFile << caretLine << endl;
      textRecord = objectCode;
      caretLine = caretHeader;
      startLoc = "";
      if(objectCode.size() != 0) {
        startLoc = locCounter;
        caretLine = caretHeader;
        caretLine.push_back('^');
        matchSizeBack(caretLine, caretLine.size() + objectCode.size() - 1, ' ');
      }
    } else if (objectCode.size()) {
      textRecord += objectCode;
      caretLine.push_back('^');
      matchSizeBack(caretLine, caretLine.size() + objectCode.size() - 1, ' ');
    }
  }

  // closing the files.
  intermediate.close();
  listing.close();
  outputObj.close();
  caretFile.close();
  cout << "PASS 2 DONE" << endl;
}

int main() {
  initializeOptab();
  pass1();
  pass2();
}