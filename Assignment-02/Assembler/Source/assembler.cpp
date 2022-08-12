#include <bits/stdc++.h>
using namespace std;

// Structs required
// Opcode Table
map<string, string> optab; // {mnemonic, machinecode}

// Symbol Table
struct symTab {
  string symbol = "";
  int address = 0;
  bool type = 0;
  int controlSection = 0;
};
vector<symTab> symtab;

// Register Table
map<string, string> regtab;

// External Table
struct extTab {
  string symbol = "";
  bool type = 0;
  int controlSection = 0;
};
vector<extTab> exttab;

// Literal Table
struct litTab {
  string name = "";
  string operandValue = "";
  int length = -1;
  int address = 0;
};
vector<litTab> littab;

// Location Table
struct locTab {
  string sectionName = "";
  int loc = 0;
  int startAddress = 0;
  int length = 0;
};
vector<locTab> loctab;

void initiliseTables() {
  optab["LDA"] = "00";
	optab["LDX"] = "04";
	optab["LDL"] = "08";
	optab["LDB"] = "68";
	optab["LDT"] = "74";
	optab["STA"] = "0C";
	optab["STX"] = "10";
	optab["STL"] = "14";
	optab["LDCH"] = "50";
	optab["STCH"] = "54";
	optab["ADD"] = "18";
	optab["SUB"] = "1C";
	optab["MUL"] = "20";
	optab["DIV"] = "24";
	optab["COMP"] = "28";
	optab["COMPR"] = "A0";
	optab["CLEAR"] = "B4";
	optab["J"] = "3C";
	optab["JLT"] = "38";
	optab["JEQ"] = "30";
	optab["JGT"] = "34";
	optab["JSUB"] = "48";
	optab["RSUB"] = "4C";
	optab["TIX"] = "2C";
	optab["TIXR"] = "B8";
	optab["TD"] = "E0";
	optab["RD"] = "D8";
	optab["WD"] = "DC";
  regtab["A"] = "0";
  regtab["X"] = "1";
  regtab["L"] = "2";
  regtab["B"] = "3";
  regtab["S"] = "4";
  regtab["T"] = "5";
  regtab["F"] = "6";
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
  // while(res.size() < 4) res.push_back('0');
  reverse(res.begin(), res.end());
  return res;
}

// convert a string in decimal into decimal integer
int convertToInt(string& s) {
  int res = 0, power = 1;
  for(int i = s.size() - 1; i >= 0; --i) {
    if(s[i] < '0' || s[i] > '9') {
      return 0;
    }
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

bool evaluateExpression(string operand, int& value, bool& expressionType, int curSection) {
  int relative = 0;
  bool relate = 0;
  string res = "";
  if(operand == "*") {
    value = loctab.back().loc;
    relative++;
  } else {
    int prev = 1;
    for(int i = 0; i < operand.size(); ++i) {
      
      if(operand[i] == '-' || operand[i] == '+') {
        
        bool found = 0;
        for(int j = 0; j < symtab.size(); ++j) {
          if(symtab[j].symbol == res) {
            
            if(symtab[j].controlSection != curSection) {
              relate = 1;
              break;
            }
            value += (prev * symtab[j].address);
            found = 1;
            relative += prev;
            break;
          }
        }

        if(!found) 
          value += (prev * (convertToInt(res)));

				res = ""; 

        if(operand[i] == '-') 
          prev = -1;
				else 
          prev = 1;

      } else {
        res.push_back(operand[i]);
      }
    }

    bool found = 0;
    for(int i = 0; i < symtab.size(); ++i) {
      if(res == symtab[i].symbol) {
        if(symtab[i].controlSection != loctab.size()) {
          relate = 1;
          break;
        }
        value += (prev * symtab[i].address);
        found = 1;
        relative += prev;
        break;
      }
    }

    if(!found) {
      value += (prev * convertToInt(res));
    }
  }

  if(!relative && !relate) 
    expressionType = 1;

  return relate;
}

int formatType(string opcode) {
  if(opcode[0] == '+') {
    return 4; 
  } 
  
  if(opcode == "CLEAR" || opcode == "COMPR" || opcode == "TIXR") {
    return 2;
  }
  
  for(auto x: optab) 
    if(opcode == x.first) {
      return 3;
    }
    
  return 0;
}

// used to seperate strings by the delimiter
vector<string> parseStrings(string operand, vector<char> delimiters) {
  vector<string> res;
  string value;
  for(int i = 0; i < operand.size(); ++i) {
    for(char x: delimiters) {
      if(operand[i] == x) {
        res.push_back(value);
        value = "";
      } else {
        value.push_back(operand[i]);
      }
    }
  }
  if(value.size()) {
    res.push_back(value);
  }
  return res;
}

void pass1() {
  cout << "PASS1 STARTED" << endl;

  // opening files.
  ifstream input("input.txt");
  ofstream intermediate("intermediate.txt");
  string data;

  // taking input from input.txt file.
  while(getline(input, data)) {
    if(data == "") {
      intermediate << data << endl;
      continue;
    }

    bool extendedFormat = 0, optabExist = 0;
    string opcode2 = "";

    string label, opcode, operand;
    parseInput(0, data, label, opcode, operand);

    // handling the case when opcode is START
    if(opcode == "START") {
      loctab.resize(loctab.size() + 1);
      loctab.back().sectionName = label;
      loctab.back().startAddress = hexToInt(operand);
      loctab.back().loc = loctab.back().startAddress;
      matchSizeFront(operand, 4, '0');
      matchSizeBack(operand, 8, ' ');
      operand += data;
      intermediate << operand << endl;
      continue;
    }

    // handling the case when opcode is END
    // break the while loop as this is the last line.
    if(opcode == "END") {
      intermediate << data << endl;
      break;
    }

    // Comment Line
    if(label == ".") {
      intermediate << data << endl;
      continue;
    }

    if(opcode == "CSECT") {
      loctab.back().length = loctab.back().loc - loctab.back().startAddress;
      loctab.resize(loctab.size() + 1);
      loctab.back().sectionName = label;
      loctab.back().loc = loctab.back().startAddress = 0;

      operand = "";
      matchSizeBack(operand, 4, '0');
      matchSizeBack(operand, 8, ' ');
      operand += data;
      intermediate << operand << endl;

    } else if(opcode == "EQU") {
      int value = 0;
      bool expressionType = 0;
      bool ok = evaluateExpression(operand, value, expressionType, loctab.size());
      // cout << value << endl;

      // search for label in symtab
      bool exist = 0;
      for(int i = 0; i < symtab.size(); ++i)
        if(symtab[i].symbol == label && symtab[i].controlSection == loctab.size()) {
          exist = 1;
          symtab[i].address = value;
          symtab[i].type = expressionType;
        }

      // if label is not in symtab then add it
      if(!exist) {
        symtab.resize(symtab.size() + 1);
        symtab.back().symbol = label;
        symtab.back().address = loctab.back().loc;
        symtab.back().controlSection = loctab.size();
      }

      symtab.back().address = value;
      symtab.back().type = expressionType;

      string val = intToHex(value);
      matchSizeBack(val, 8, ' ');
      val += data;
      intermediate << val << endl;

    } else {
      if(label != "") {

        // search for label in symtab
        bool exist = 0;
        for(int i = 0; i < symtab.size(); ++i)
          if(symtab[i].symbol == label && symtab[i].controlSection == loctab.size())
            exist = 1;

        // if label is not in symtab then add it
        if(!exist) {
          symtab.resize(symtab.size() + 1);
          symtab.back().address = loctab.back().loc;
          symtab.back().symbol = label;
          symtab.back().controlSection = loctab.size();
        }
      }

      // extended format
      if(opcode[0] == '+') {
        extendedFormat = 1;
        opcode2 = opcode;
        reverse(opcode2.begin(), opcode2.end());
        opcode2.pop_back();
        reverse(opcode2.begin(), opcode2.end());
      } else {
				opcode2 = opcode;
			}

      // Searching Opcode Table
      for(auto x: optab)
        if(x.first == opcode2) {
          optabExist = 1;
        }

      // search for literals
      bool exist = 0;
      if(operand[0] == '=') {
        for(int i = 0; i < littab.size(); ++i) {
          if(littab[i].name == operand) {
            exist = 1;
          }
        }
      }

      // add the literal if it doesnot eist in littab
      if(!exist && operand[0] == '=') {
        littab.resize(littab.size() + 1);
        littab.back().address = loctab.back().loc;
        littab.back().name = operand;
        littab.back().address = -1;
        reverse(operand.begin(), operand.end());
        operand.pop_back();
        reverse(operand.begin(), operand.end());
        littab.back().length = lengthOfConstant(operand);
        littab.back().operandValue = constantString(operand);
        reverse(operand.begin(), operand.end());
        operand.push_back('+');
        reverse(operand.begin(), operand.end());
      }

      if(opcode == "EXTDEF") {

        // search for extdef in exttab
        bool exist = 0;
        vector<string> values = parseStrings(operand, {','});

        for(string value: values)
          for(int i = 0; i < exttab.size(); ++i) {
            if(exttab[i].symbol == value && exttab[i].controlSection == loctab.size())
              exist = 1;

          // add to exttab if it does not exist  
          if(!exist) {
            exttab.resize(exttab.size() + 1);
            exttab.back().controlSection = loctab.size();
            exttab.back().type = 1;
          }
        }

        matchSizeFront(data, data.size() + 8, ' ');
        intermediate << data << endl;

      } else if(opcode == "EXTREF") {

        // search for extdef in exttab
        bool exist = 0;
        vector<string> values = parseStrings(operand, {','});

        for(string value: values)
          for(int i = 0; i < exttab.size(); ++i) {
            if(exttab[i].symbol == value && exttab[i].controlSection == loctab.size())
              exist = 1;

            // add to exttab if it does not exist  
            if(!exist) {
              exttab.resize(exttab.size() + 1);
              exttab.back().controlSection = loctab.size();
              exttab.back().type = 0;
            }
          }

        matchSizeFront(data, data.size() + 8, ' ');
        intermediate << data << endl;

      } else if(opcode == "LTORG") {
        intermediate << data << endl;

        // assign address for all previously defined literals
        // whose address are not assigned before
        for(int i = 0; i < littab.size(); ++i) 

          // add all literals into intermediate file
          if(littab[i].address == -1) {
            littab[i].address = loctab.back().loc;
            string value = intToHex(loctab.back().loc);
            matchSizeFront(value, 4, '0');
            matchSizeBack(value, 8, ' ');
            value.push_back('*');
            matchSizeBack(value, 16, ' ');
            value += littab[i].name;
            intermediate << value << endl;

            value = littab[i].name;
            reverse(value.begin(), value.end());
            value.pop_back();
            reverse(value.begin(), value.end());
            loctab.back().loc += lengthOfConstant(value);
          }

      } else {
        string value = intToHex(loctab.back().loc);
        matchSizeFront(value, 4, '0');
        matchSizeBack(value, 8, ' ');
        value += data;
        intermediate << value << endl;
      }

    }

    // increase location counter
    if(optabExist && (opcode == "COMPR" || opcode == "CLEAR" || opcode == "TIXR")) {
      loctab.back().loc += 2;
    } else if(optabExist && !extendedFormat) {
      loctab.back().loc += 3;
    } else if(optabExist && extendedFormat) {
      loctab.back().loc += 4;
    } else if(opcode == "WORD") {
      loctab.back().loc += 3;
    } else if(opcode == "RESW") {
      loctab.back().loc += (3 * convertToInt(operand));
    } else if(opcode == "RESB") {
      loctab.back().loc += convertToInt(operand);
    } else if(opcode == "BYTE") {
      loctab.back().loc += lengthOfConstant(operand);
    }
  }

  // assign address for all previously defined literals
  // whose address are not assigned before
  for(int i = 0; i < littab.size(); ++i) 
    if(littab[i].address == -1) {
      littab[i].address = loctab.back().loc;
      string value = intToHex(loctab.back().loc);
      matchSizeFront(value, 4, '0');
      matchSizeBack(value, 8, ' ');
      value.push_back('*');
      matchSizeBack(value, 16, ' ');
      value += littab[i].name;
      intermediate << value << endl;

      value = littab[i].name;
      reverse(value.begin(), value.end());
      value.pop_back();
      reverse(value.begin(), value.end());
      loctab.back().loc += lengthOfConstant(value);
    }

  loctab.back().length = loctab.back().loc - loctab.back().startAddress;

  // closing files
  input.close();
  intermediate.close();

  cout << "PASS 1 DONE" << endl;

}

void pass2() {
  cout << "PASS2 STARTED" << endl;

  // opening files.
  ifstream intermediate("intermediate.txt");
  ofstream listing("listing.txt");
  ofstream output("output.txt");
  string data;

  string textRecord = "", startLoc = "";
  int recordLength = 0;
  vector<string> modificationRecords;

  int curSection = 1;

  // taking input from input.txt file.
  while(getline(intermediate, data)) {
    if(data == "") {
      listing << data << endl;
      continue;
    }

    bool extendedFormat = 0, optabExist = 0;

    string label, opcode, operand;
    string location = data.substr(0, 4);

    if(startLoc == "" && location != "    ") {
      startLoc = location;
      matchSizeFront(startLoc, 6, '0');
    }

    parseInput(8, data, label, opcode, operand);

    matchSizeBack(data, 50, ' ');

    // handling the case when opcode is START
    if(opcode == "START") {
      listing << data << endl;

      matchSizeBack(label, 6, ' ');
      matchSizeFront(operand, 6, '0');

      string val = intToHex(loctab[curSection - 1].length);
      matchSizeFront(val, 6, '0');

      output << "H^" << label << "^" << operand << "^" << val << endl;

      continue;
    }

    // handling the case when opcode is END
    // break the while loop as this is the last line.
    if(label == "END") {
      continue;
    }

    // comment line
    if(label == ".") {
      listing << data << endl;
      continue;
    }

    string objectCode = "";

    if(label == "*") {
      for(int i = 0; i < littab.size(); ++i) {
        if(littab[i].name == opcode) {
          objectCode = littab[i].operandValue;
          break;
        }
      }
      assert(objectCode.size() > 0);
      data += objectCode;
      listing << data << endl;
    }

    else if(formatType(opcode) == 2) {
      for(auto x: optab) {
        if(x.first == opcode) {
          objectCode = x.second;
          break;
        }
      }
      
      objectCode += regtab[operand.substr(0,1)];
      if(operand.size() > 1 && operand[1] == ',') {
        objectCode += regtab[operand.substr(2,1)];
      }
      matchSizeBack(objectCode, 4, '0');
      data += objectCode;
      listing << data << endl;
    }

    else if(formatType(opcode) == 3) {

      map<char, int> bits;
      bits['n'] = (1 << 17);
      bits['i'] = (1 << 16);
      bits['x'] = (1 << 15);
      bits['b'] = (1 << 14);
      bits['p'] = (1 << 13);
      bits['e'] = (1 << 12);

      // opcode part
      int value = 0;
      for(auto x: optab) {
        if(x.first == opcode) {
          value += hexToInt(x.second);
          value = value << 16;
          break;
        }
      }

      if(!operand.size()) {
        value += (bits['i'] + bits['n']);
        data += intToHex(value);
        objectCode = intToHex(value);
        // cout << value << endl;
        listing << data << endl;
        // continue;
      }

      else if(operand[0] == '#') {
        bool exist = 0;
        for(int i = 0; i < symtab.size(); ++i) {
          if(symtab[i].symbol == operand.substr(1, operand.size() - 1) && symtab[i].controlSection == curSection) {
            exist = 1;
            value += (symtab[i].address - hexToInt(location) - 3);            
          }
        }

        if(exist) {
          value += (bits['i'] + bits['p']);
        } else {
          value += (bits['i'] + hexToInt(operand.substr(1, operand.size() - 1)));
        }

        string val = intToHex(value);
        matchSizeFront(val, 6, '0');
        data += val;
        objectCode = val;
        listing << data << endl;
      }
      
      else if(operand[0] == '@') {
        bool exist = 0;
        for(int i = 0; i < symtab.size(); ++i) {
          if(symtab[i].symbol == operand.substr(1, operand.size() - 1) && symtab[i].controlSection == curSection) {
            exist = 1;
            int res = (symtab[i].address - hexToInt(location) - 3);            
            res = (res & ((1 << 12) - 1));
            value += (res + bits['p'] + bits['n']); 
          }
        }

        if(!exist) {
          value += (hexToInt(operand.substr(1, operand.size() - 1)) + bits['n']);
        }
        
        string val = intToHex(value);
        matchSizeFront(val, 6, '0');
        data += val;
        objectCode = val;
        listing << data << endl;
      }

      else if(operand[0] == '=') {
        bool exist = 0;
        for(int i = 0; i < littab.size(); ++i) {
          if(littab[i].name == operand) {
            exist = 1;
            int res = (littab[i].address - hexToInt(location) - 3);
            res = (res & ((1 << 12) - 1));
            value += res;
            break;
          }
        }
          
        value += (bits['p'] + bits['n'] + bits['i']);
        string val = intToHex(value);
        matchSizeFront(val, 6, '0');
        data += val;
        objectCode = val;
        listing << data << endl;
      }

      else if(operand.size() > 1 && operand.substr(operand.size() - 2, 2) == ",X") {
        value += (bits['n'] + bits['i'] + bits['x'] + bits['p']);
        operand = operand.substr(0, operand.size() - 2);

        bool exist = 0;
        for(int i = 0; i < symtab.size(); ++i) {
          if(symtab[i].symbol == operand && symtab[i].controlSection == curSection) {
            exist = 1;
            int res = (symtab[i].address - hexToInt(location) - 3);            
            res = (res & ((1 << 12) - 1));
            value += (res); 
          }
        }

        string val = intToHex(value);
        matchSizeFront(val, 6, '0');
        data += val;
        objectCode = val;
        listing << data << endl;
      }

      else {
        bool exist = 0;
        for(int i = symtab.size() - 1; i >= 0; --i) {
          if(symtab[i].symbol == operand && symtab[i].controlSection == curSection) {
            exist = 1;
            int res = (symtab[i].address - hexToInt(location) - 3);
            res = (res & ((1 << 12) - 1));
            value += res;
            break;
          }
        }

        value += (bits['p'] + bits['n'] + bits['i']);
        string val = intToHex(value);
        matchSizeFront(val, 6, '0');
        data += val;
        objectCode = val;
        listing << data << endl;

      }

    }
  
    else if(formatType(opcode) == 4) {

      // modification record handling
      string modi = "M^", temp;
      temp = intToHex(1 + hexToInt(location));
      matchSizeFront(temp, 6, '0');
      modi += temp;
      modi += "^05^+";
      string op = operand;
      if(operand.substr(operand.size() - 2, 2) == ",X") {
        op = operand.substr(0, operand.size() - 2);
      }
      modi += op;
      modificationRecords.push_back(modi);

      int value = 0;
      map<char, int> bits;
      bits['n'] = (1 << 25);
      bits['i'] = (1 << 24);
      bits['x'] = (1 << 23);
      bits['b'] = (1 << 22);
      bits['p'] = (1 << 21);
      bits['e'] = (1 << 20);

      for(auto x: optab) {
        if(x.first == opcode.substr(1, opcode.size() - 1)) {
          value += hexToInt(x.second);
          break;
        }
      }

      value = value << 24;
      value += (bits['n'] + bits['i'] + bits['e']);

      int res = 0;
      if(operand.substr(operand.size() - 2, 2) == ",X") {
        value += bits['x'];

        for(int i = 0; i < symtab.size(); ++i) {
          if(symtab[i].symbol == operand.substr(0, operand.size() - 2) && symtab[i].controlSection == curSection) {
            res = (symtab[i].address - hexToInt(location) - 3);
            res = (res & ((1 << 12) - 1));
            value += res;
            break; 
          }
        }
      } else if(operand.size()) {

        bool exist = 0;
        for(int i = 0; i < symtab.size(); ++i) {
          if(symtab[i].symbol == operand && symtab[i].controlSection == curSection) {
            res += symtab[i].address;
            exist = 1;
            break; 
          }
        }

        if(!exist) {
          for(int i = 0; i < exttab.size(); ++i) {
            if(exttab[i].symbol == operand && exttab[i].controlSection == curSection) {
              res = 0;
            }
          }
        }

      }

      res = (res & ((1 << 20) - 1));
      value += res;
      string val = intToHex(value);
      data += val;
      objectCode = val;
      listing << data << endl;
    }
  
    else if(opcode == "CSECT") {
      // Text records for prev control section.
      if(recordLength > 0) {
        string t = intToHex(recordLength / 2);
        matchSizeFront(t, 2, '0');
        string text = "T^" + startLoc + "^" + t + textRecord;
        output << text << endl;
        startLoc = "";
        textRecord = "";
        recordLength = 0;
      }


      // Modification records for prev control section
      for(string x: modificationRecords) {
        output << x << endl;
      }
      modificationRecords.clear();

      // End record for before control section
      output << "E";
      if(curSection == 1) {
        string beg = intToHex(loctab[curSection - 1].startAddress);
        matchSizeFront(beg, 6, '0');
        output << "^" << beg;
      }
      output << endl << endl;

      for(int i = 0; i < loctab.size(); ++i) {
        if(loctab[i].sectionName == label) {
          listing << data << endl;
          curSection++;
          break;
        }
      }
      
      // Header record for the next control section
      matchSizeBack(label, 6, ' ');
      string beg = intToHex(loctab[curSection - 1].startAddress);

      string val = intToHex(loctab[curSection - 1].length);
      matchSizeFront(val, 6, '0');
      matchSizeFront(beg, 6, '0');
      output << "H^" << label << "^" << beg << "^" << val << endl;
    }

    else if(opcode == "WORD") {
      // modification record handling
      string add = "";
      char prev = '+';
      for(char x: operand) {
        if(x == '+' || x == '-') {
          string modi = "M^", temp;
          temp = location;
          matchSizeFront(temp, 6, '0');
          modi += temp;
          modi += "^06^";
          modi.push_back(prev);
          modi += add;
          add = "";
          prev = x;
          modificationRecords.push_back(modi);
        } else {
          add.push_back(x);
        }
      }

      if(add.size()) {
        string modi = "M^", temp;
        temp = location;
        matchSizeFront(temp, 6, '0');
        modi += temp;
        modi += "^06^";
        modi.push_back(prev);
        modi += add;
        add = "";
        modificationRecords.push_back(modi);
      }


      bool expressionType = 0;
      int value = 0;
      bool reference = evaluateExpression(operand, value, expressionType, curSection);
      string val = intToHex(value);
      matchSizeFront(val, 6, '0');
      data += val;
      objectCode = val;
      listing << data << endl;
    }

    else if(opcode == "BYTE") {
      int value = 0;
      string t = "";
      if((operand[0] == 'C' || operand[0] == 'c') && operand[1] == '\'') {
        for(int i = 2; i < operand.size() - 1; ++i) {
          value += (int) operand[i];
          value = value << 8;
        }
        value = value >> 8;
      } else if((operand[0] == 'X' || operand[0] == 'x') && operand[1] == '\'') {
        for(int i = 2; i < operand.size() - 1; ++i) {
          t.push_back(operand[i]);
        }
        value = hexToInt(t);
      }

      string val = intToHex(value);
      // matchSizeFront(val, 6, '0');
      data += val;
      objectCode = val;
      listing << data << endl;
    }

    else {
      if(opcode == "EXTDEF") {
        vector<string> values = parseStrings(operand, {','});
        output << "D";
        for(string x: values) {
          output << "^" << x << "^";
          string ans = "";
          for(int i = 0; i < symtab.size(); ++i) {
            if(symtab[i].symbol == x && curSection == symtab[i].controlSection) {
              ans = intToHex(symtab[i].address);
              matchSizeFront(ans, 6, '0');
              break;
            }
          }
          output << ans;
        }
        output << endl;
      } else if(opcode == "EXTREF") {
        vector<string> values = parseStrings(operand, {','});
        output << "R";
        for(string x: values) {
          matchSizeBack(x, 6, ' ');
          output << "^" << x;
        }
        output << endl;
      }
      listing << data << endl;
    }


    if(objectCode == "" && recordLength) {
      string t = intToHex(recordLength / 2);
      matchSizeFront(t, 2, '0'); 
      string text = "T^" + startLoc + "^" + t + textRecord;
      output << text << endl;
      startLoc = "";
      recordLength = 0;
      textRecord = "";
    } else if(objectCode == "") {
      startLoc = "";
      recordLength = 0;
      textRecord = "";
    } else if(objectCode.size() > 0 && recordLength + objectCode.size() > 60) {
      string t = intToHex(recordLength / 2);
      matchSizeFront(t, 2, '0');
      string text = "T^" + startLoc + "^" + t + textRecord;
      output << text << endl;
      startLoc = location;
      matchSizeFront(startLoc, 6, '0');
      textRecord = "^";
      textRecord += objectCode;
      recordLength = objectCode.size();
    } else if(objectCode.size()) {
      textRecord += "^";
      textRecord += objectCode;
      recordLength += objectCode.size();
    }

  }

  // Text records for prev control section.
  if(recordLength > 0) {
    string t = intToHex(recordLength / 2);
    matchSizeFront(t, 2, '0');
    string text = "T^" + startLoc + "^" + t + textRecord;
    output << text << endl;
    startLoc = "";
    textRecord = "";
    recordLength = 0;
  }

  for(string x: modificationRecords) {
    output << x << endl;
  }
  output << "E" << endl;

  //closing files
  intermediate.close();
  listing.close();
  output.close();

  cout << "PASS2 ENDED" << endl;
}

int main() {
  initiliseTables();
  pass1();
  pass2();
}
