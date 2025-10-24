#include <iostream>
#include <fstream>
#include <memory>

class bf_vm
{
public:
    bf_vm() : ptr(instr_memory), instr_memory(), parser_stack() {}

    inline void instr_plus() { *ptr += 1; }

    inline void instr_minus() { *ptr -= 1; }

    inline void instr_left() { ptr -= 1; }

    inline void instr_right() { ptr += 1; }

    inline void instr_output() { std::cout << *ptr; }

    inline void instr_input() { *ptr = std::cin.get(); }

    inline void instr_push_parser_index(int index) { parser_stack.push(index); }

    inline int instr_jump_script_index(int index)
    {
        if (*ptr == 0)
        {
            parser_stack.pop();
            return index;
        }
        else
        {
            return parser_stack.top();
        }
    }

    void run(const uint8_t* script, size_t script_size)
    {
        for (int i = 0; i < script_size; i++)
        {
            switch (script[i])
            {
            case '+':
                instr_plus();
                break;
            case '-':
                instr_minus();
                break;
            case '<':
                instr_left();
                break;
            case '>':
                instr_right();
                break;
            case '.':
                instr_output();
                break;
            case ',':
                instr_input();
                break;
            case '[':
                instr_push_parser_index(i);
                break;
            case ']':
                i = instr_jump_script_index(i);
                break;
            default:
                break;
            }
        }
    }

private:
    char* ptr;
    char instr_memory[30000];
    std::stack<int> parser_stack;
};

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        std::unique_ptr<bf_vm> vm = std::make_unique<bf_vm>();
        std::cout << "~ ";
        std::string script;
        while (std::cin >> script)
        {
            if (script == "q" || script == "quit")
                break;
            vm->run((uint8_t*)script.data(), script.size());
            std::cout << "\n~ ";
        }
    }
    else
    {
        for (int i = 1; i < argc; ++i)
        {
            std::ifstream file(argv[i]);
            if (!file.is_open())
            {
                std::cerr << (i > 1 ? "\n" : "") << "> Error: Script File <" << argv[i] << "> Don't Open\n";
                continue;
            }
            else
            {
                std::cout << (i > 1 ? "\n" : "") << "> Usage: Run Script File <" << argv[i] << ">\n";
            }
            file.seekg(0, std::ios::end);
            size_t file_size = file.tellg();
            file.seekg(0, std::ios::beg);
            std::vector<uint8_t> script(file_size);
            file.read(reinterpret_cast<char*>(script.data()), file_size);
            file.close();
            std::unique_ptr<bf_vm> vm = std::make_unique<bf_vm>();
            vm->run(script.data(), script.size());
        }
    }

    return 0;
}