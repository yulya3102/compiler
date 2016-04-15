#include "lcc.h"

#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>

#include <iostream>
#include <fstream>

int main(int argc, char ** argv)
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <input> <output>" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream in(argv[1]);
    std::error_code err;
    llvm::raw_fd_ostream out(argv[2], err, llvm::sys::fs::OpenFlags::F_None);
    lcc::compile(in, out, argv[1]);
}
