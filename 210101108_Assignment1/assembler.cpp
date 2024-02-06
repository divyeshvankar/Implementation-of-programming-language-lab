#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

#define WORD 10

struct instruction
{
    // DS to store instruction
    string label;
    string opcode;
    string operand;

    instruction(string label = "", string opcode = "", string operand = "")
    {
        this->label = label;
        this->opcode = opcode;
        this->operand = operand;
    }

    bool is_comment()
    {
        return (label == ".");
    }

    bool has_label()
    {
        return !label.empty();
    }

    void print()
    {
        cout << "-----------------------------------\n";
        cout << "label = " << label << '\n';
        cout << "opcode = " << opcode << '\n';
        cout << "operand = " << operand << '\n';
        cout << "-----------------------------------\n";
    }
};

unordered_map<string, string> OPTAB;

// given OPTAB init
void initialize_OPTAB()
{
    OPTAB = {
        {"LDA", "00"},
        {"LDX", "04"},
        {"LDL", "08"},
        {"STA", "0C"},
        {"STX", "10"},
        {"STL", "14"},
        {"LDCH", "50"},
        {"STCH", "54"},
        {"ADD", "18"},
        {"SUB", "1C"},
        {"MUL", "20"},
        {"DIV", "24"},
        {"COMP", "28"},
        {"J", "3C"},
        {"JLT", "38"},
        {"JEQ", "30"},
        {"JGT", "34"},
        {"JSUB", "48"},
        {"RSUB", "4C"},
        {"TIX", "2C"},
        {"TD", "E0"},
        {"RD", "D8"},
        {"WD", "DC"}};
}

unordered_map<string, int> SYMTAB;

string strip(string s)
{
    s.erase(remove(s.begin(), s.end(), ' '), s.end());
    return s;
}

void process_line(string line, instruction *instr)
{
    // making instruction DS checking is it comment or not!
    string label, opcode, operand;
    label = strip(line.substr(0, WORD));
    if (label == ".")
        instr->label = label;
    else
    {
        opcode = strip(line.substr(WORD, WORD));
        operand = strip(line.substr(WORD * 2, WORD));
        instr->label = label;
        instr->opcode = opcode;
        instr->operand = operand;
    }
}

string process_line_2(string line, instruction *instr)
{
    string label, opcode, operand, LOCCTR;
    LOCCTR = strip(line.substr(0, WORD));

    if (LOCCTR == ".")
    {
        label = strip(line.substr(0, WORD));
        instr->label = label;
        return "";
    }
    else
    {
        label = strip(line.substr(WORD, WORD));
        opcode = strip(line.substr(WORD * 2, WORD));
        operand = strip(line.substr(WORD * 3, WORD));
        instr->label = label;
        instr->opcode = opcode;
        instr->operand = operand;
        return LOCCTR;
    }
}

string format_number(string input, int width, bool hex)
{
    int num;
    if (hex)
        num = stoi(input, NULL, 16);
    else
        num = stoi(input);

    stringstream temp;
    temp << std::hex << std::uppercase << std::setfill('0') << std::setw(width) << num;
    return temp.str();
}

string format_name(string name, int width)
{
    stringstream temp;
    temp << std::left << std::setfill(' ') << std::setw(width) << name;
    return temp.str();
}

int pass_1()
{
    fstream fin, fout;

    fin.open("input.txt", ios::in);
    fout.open("intermediate.txt", ios::out);

    int LOCCTR = 0, STADDR = 0, LENGTH;

   
    bool error = false;

    while (fin.good())
    {
        string line;
        getline(fin, line);

        instruction instr;
        process_line(line, &instr);

        if (!instr.is_comment())
        {
            if (instr.opcode == "START")
            {
                STADDR = stoi(instr.operand, nullptr, 16);
                LOCCTR = STADDR;
                fout << format_name(format_number(to_string(LOCCTR), 4, false), 10) << line << '\n';
            }
            else if (instr.opcode == "END")
            {
                LENGTH = LOCCTR - STADDR;
                fout << format_name("", 10) << line;
                break;
            }
            else
            {
                if (instr.has_label())
                {
                    if (SYMTAB.find(instr.label) == SYMTAB.end())
                        SYMTAB[instr.label] = LOCCTR;
                    else
                    {
                        error = true; // duplicate symbol
                        cout << "duplicate symbol\n";
                    }
                }

                fout << format_name(format_number(to_string(LOCCTR), 4, false), 10) << line << '\n';

                auto optab_entry = OPTAB.find(instr.opcode);
                if (optab_entry != OPTAB.end())
                    LOCCTR += 3;
                else if (instr.opcode == "WORD") //word size
                    LOCCTR += 3;
                else if (instr.opcode == "RESW")  //reserve word
                    LOCCTR += 3 * stoi(instr.operand);
                else if (instr.opcode == "RESB") //reserve byte
                    LOCCTR += stoi(instr.operand);
                else if (instr.opcode == "BYTE") //byte given
                {
                    if (instr.operand[0] == 'C')
                        LOCCTR += (instr.operand.length() - 3);
                    if (instr.operand[0] == 'X')
                        LOCCTR += (instr.operand.length() - 3) / 2;
                }
                else
                {
                    error = true; // invalid operation code
                    cout << "invalid operation code\n";
                }
            }
        }
        else
            fout << line << '\n';
    }

    return LENGTH;
}

void pass_2(int LENGTH)
{
    fstream fin, fout, fout2;

    fin.open("intermediate.txt", ios::in);
    fout.open("object.txt", ios::out);
    fout2.open("table.txt", ios::out);

    string text_record, staddr;
    bool error = false;

    while (fin.good())
    {
        string line;
        getline(fin, line);

        instruction instr;
        string LOCCTR = process_line_2(line, &instr);

        if (!instr.is_comment())
        {
            if (instr.opcode == "START")
            {
                stringstream header;
                header << "H";
                header << format_name(instr.label, 6);
                header << format_number(instr.operand, 6, true);
                header << format_number(to_string(LENGTH), 6, false);
                fout << header.str() << '\n';
                fout2 << line << '\n';
            }
            else if (instr.opcode == "END")
            {
                if (text_record.length())
                {
                    int length = (text_record.length()) / 2;
                    fout << "T" << format_number(staddr, 6, true) << format_number(to_string(length), 2, false) << text_record << '\n';
                }
                stringstream end;
                int first = SYMTAB[instr.operand];
                end << "E";
                end << format_number(to_string(first), 6, false);
                fout << end.str() << '\n';
                fout2 << line;
            }
            else
            {
                string obcode = "";
                auto optab_entry = OPTAB.find(instr.opcode);
                if (optab_entry != OPTAB.end())
                {
                    obcode = optab_entry->second;
                    if (!instr.operand.empty())
                    {
                        int operand = 0;
                        if (instr.operand.find(",") != string::npos)
                        {
                            instr.operand.resize(instr.operand.find(","));
                            operand += 32768;
                        }
                        auto symtab_entry = SYMTAB.find(instr.operand);
                        if (symtab_entry != SYMTAB.end())
                        {
                            operand += symtab_entry->second;
                            obcode += format_number(to_string(operand), 4, false);
                        }
                        else
                        {
                            obcode += format_number("0", 4, false);
                            error = true; // undefined symbol
                            cout << "undefined symbol\n";
                        }
                    }
                    else
                    {
                        obcode += format_number("0", 4, false);
                    }
                    fout2 << line << format_name(obcode, 10) << '\n';
                }
                else
                {
                    if (instr.opcode == "BYTE")
                    {
                        if (instr.operand[0] == 'C')
                        {
                            string constant = instr.operand.substr(2, instr.operand.length() - 3);
                            for (char ch : constant)
                            {
                                int x = ch;
                                obcode += format_number(to_string(x), 2, false);
                            }
                        }
                        else
                        {
                            string constant = instr.operand.substr(2, instr.operand.length() - 3);
                            obcode += format_number(constant, constant.length(), true);
                        }
                    }
                    else if (instr.opcode == "WORD")
                        obcode += format_number(instr.operand, 6, false);

                    fout2 << line << format_name(obcode, 10) << '\n';
                }

                if (text_record.empty())
                {
                    staddr = LOCCTR;
                }

                if (text_record.length() + obcode.length() <= 60 && instr.opcode != "RESW" && instr.opcode != "RESB")
                {
                    text_record += obcode;
                }
                else
                {
                    if (!text_record.empty())
                    {
                        int length = (text_record.length()) / 2;
                        fout << "T" << format_number(staddr, 6, true) << format_number(to_string(length), 2, false) << text_record << '\n';
                        text_record = obcode;
                        staddr = LOCCTR;
                    }
                }
            }
        }
        else
            fout2 << line << '\n';
    }
}

int main()
{
    // initialising OPTAB (given)
    initialize_OPTAB();

    // pass1 creating SYMTAB & assigning locatin counter
    int LENGTH = pass_1();

    
    pass_2(LENGTH);
    return 0;
}
