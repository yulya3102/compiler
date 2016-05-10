#include "common.h"

#include <lcc/lcc.h>

#include <queue>
#include <unistd.h>
#include <stdexcept>
#include <string.h>
#include <sstream>
#include <sys/wait.h>

struct p2open
{
    p2open(const char * command, int expected_exit_code = 0)
    : expected_exit_code(expected_exit_code)
    {
        // TODO: error handling
        int process_input[2];
        pipe(process_input);
        int process_output[2];
        pipe(process_output);
        child = fork();
        if (child == 0)
        {
            dup2(process_input[0], STDIN_FILENO);
            close(process_input[0]);
            close(process_input[1]);
            dup2(process_output[1], STDOUT_FILENO);
            close(process_output[0]);
            close(process_output[1]);

            execl(command, command, nullptr);
        }
        close(process_input[0]);
        close(process_output[1]);

        inputfd = process_input[1];
        outputfd = process_output[0];
    }

    void write(int i)
    {
        write_buffer.push(i);
    }

    void full_write(int fd, char * buf, size_t count)
    {
        while (count > 0)
        {
            ssize_t r = ::write(fd, buf, count);
            if (r == -1)
                throw std::runtime_error(strerror(errno));

            count -= r;
            buf += r;
        }
    }

    std::vector<int> read()
    {
        const size_t size = 4096;
        char buffer[size];
        while (!write_buffer.empty())
        {
            int r = snprintf(buffer, size, "%d\n", write_buffer.front());
            full_write(inputfd, buffer, r);
            write_buffer.pop();
        }
        close(inputfd);

        std::stringstream output;
        int r;
        do
        {
            r = ::read(outputfd, buffer, size);
            if (r == -1)
                throw std::runtime_error(strerror(errno));

            output << std::string(buffer, r);
        }
        while (r != 0);

        std::vector<int> result;
        while (true)
        {
            int i;
            output >> i;
            if (!output)
                break;
            result.push_back(i);
        }

        return result;
    }

    ~p2open()
    {
        int status;
        waitpid(child, &status, 0);
        if (WIFEXITED(status) && (WEXITSTATUS(status) != expected_exit_code))
            throw std::runtime_error("Unexpected return code of a program: "
                                     + std::to_string(WEXITSTATUS(status)));
        if (WIFSIGNALED(status))
            throw std::runtime_error("Program was terminated by a signal "
                                     + std::to_string(WTERMSIG(status)));
    }

    int expected_exit_code;
    pid_t child;
    int inputfd, outputfd;
    std::queue<int> write_buffer;
};

std::vector<int> test_compiled(const std::string & code, const std::vector<int> & input,
                               lcc::Optimisations opt, int expected_retcode)
{
    std::stringstream in(code);
    std::string compiled = lcc::create_temp_file("test_compiled_XXXXXX");
    lcc::compile_executable(in, compiled, opt);

    p2open proc(compiled.c_str());
    for (auto i : input)
        proc.write(i);
    return proc.read();
}
