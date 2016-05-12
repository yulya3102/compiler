#include "common.h"

#include <lcc/lcc.h>

#include <queue>
#include <unistd.h>
#include <stdexcept>
#include <string.h>
#include <sstream>
#include <sys/wait.h>
#include <chrono>
#include <iostream>

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

    void exit()
    {
        int status;
        waitpid(child, &status, 0);
        exited = true;
        if (WIFEXITED(status) && (WEXITSTATUS(status) != expected_exit_code))
            throw testing::nonzero_retcode(WEXITSTATUS(status));
        if (WIFSIGNALED(status))
            throw testing::killed_by_signal(WTERMSIG(status));
    }

    ~p2open()
    {
        if (!exited)
            exit();
    }

    int expected_exit_code;
    pid_t child;
    int inputfd, outputfd;
    std::queue<int> write_buffer;
    bool exited;
};

std::vector<int> test_compiled(const std::string & code, const std::vector<int> & input,
                               lcc::Optimisations opt, const char * log_message, int expected_retcode)
{
    std::stringstream in(code);
    std::string compiled = lcc::create_temp_file("test_compiled_XXXXXX");
    lcc::compile_executable(in, compiled, opt);

    auto start = std::chrono::system_clock::now();
    p2open proc(compiled.c_str(), expected_retcode);
    for (auto i : input)
        proc.write(i);
    auto r = proc.read();
    proc.exit();
    auto end = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<int, std::milli>>(end - start);
    if (log_message)
        std::cout << log_message << ": " << duration.count() << " ms" << std::endl;

    return r;
}

std::vector<int> random_input(size_t size, int limit)
{
    static std::mt19937 generator;
    std::uniform_int_distribution<int> distribution(0, limit);

    std::vector<int> result;
    for (size_t i = 0; i < size; ++i)
    {
        int random = distribution(generator);
        result.push_back(random);
    }

    return result;
}
