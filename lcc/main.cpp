#include "lcc.h"

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <fstream>

void usage(const char * program)
{
    std::cerr << "Usage: " << program << " [-O{0,1,2}] <input> <output>" << std::endl;
    exit(EXIT_FAILURE);
}

int main(int argc, char ** argv)
{
    if (argc < 3)
        usage(argv[0]);

    lcc::Optimisations opt = lcc::Optimisations::ACC;
    const char * input = argv[1];
    const char * output = argv[2];

    if (argv[1][0] == '-')
    {
        if (argv[1][1] != 'O')
            usage(argv[0]);
        switch (argv[1][2])
        {
            case '0':
                opt = lcc::Optimisations::NONE;
                break;
            case '1':
                opt = lcc::Optimisations::TCO;
                break;
            case '2':
                opt = lcc::Optimisations::TCO;
                break;
            default:
                usage(argv[0]);
        }

        if (argc != 4)
            usage(argv[0]);

        input = argv[2];
        output = argv[3];
    }

    std::ifstream in(input);
    std::error_code err;
    llvm::raw_fd_ostream out(output, err, llvm::sys::fs::OpenFlags::F_None);
    lcc::compile_llvm(in, out, input, opt);
}
