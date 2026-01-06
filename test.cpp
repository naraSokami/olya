#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <cstring>


extern "C" char **ft_split(char *str);

struct TestCase {
    const char* input;
    std::string expected_stdout;
    std::string expected_stderr;
};

const std::string RED    = "\033[31m";
const std::string GREEN  = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string RESET  = "\033[0m";


std::string run_ft_split_safe(const char* input, bool &crashed)
{
    crashed = false;
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        crashed = true;
        return "";
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        crashed = true;
        return "";
    }

    if (pid == 0) {
        close(pipefd[0]);
        FILE* out = fdopen(pipefd[1], "w");
        if (!out) exit(1);

        char *str = input ? strdup(input) : NULL;
        char **res = ft_split(str);

        for (int i = 0; res && res[i]; i++)
            fprintf(out, "%s\n", res[i]);

        for (int i = 0; res && res[i]; i++)
            free(res[i]);
        free(res);
        free(str);

        fclose(out);
        exit(0);
    } else {
        close(pipefd[1]);
        char buffer[1024];
        std::string output;
        ssize_t n;

        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            output.append(buffer, n);
        }
        close(pipefd[0]);

        int status;
        waitpid(pid, &status, 0);

        if (WIFSIGNALED(status) || !WIFEXITED(status) || WEXITSTATUS(status) != 0) {
            crashed = true;
            return "";
        }

        return output;
    }
}


int main()
{
    std::vector<TestCase> tests = {
		{ NULL, "", "" },
		{ nullptr, "", "" },
		{ "", "", "" },
		{ " ", "", "" },
		{ "\t", "", "" },
		{ "\n", "", "" },
		{ " \t\n", "", "" },
		{ "          a", "a\n", "" },
		{ "a          ", "a\n", "" },
		{ "\t\t\t\ta", "a\n", "" },
		{ "a\n\n\n\n", "a\n", "" },
		{ "     ", "", "" },
		{ "\t\t\t", "", "" },
		{ "\n\n\n", "", "" },
		{ " \n \t \n \t ", "", "" },
		{ "a", "a\n", "" },
		{ " a ", "a\n", "" },
		{ "\ta\t", "a\n", "" },
		{ "\na\n", "a\n", "" },
		{ "ab", "ab\n", "" },
		{ " ab ", "ab\n", "" },
		{ "a b", "a\nb\n", "" },
		{ "a\tb", "a\nb\n", "" },
		{ "a\nb", "a\nb\n", "" },
		{ "a \t\n b", "a\nb\n", "" },
		{ "\ta \n\tb\t\n", "a\nb\n", "" },
		{ "\n a\tb \n", "a\nb\n", "" },
		{ "a b c d e f g h", "a\nb\nc\nd\ne\nf\ng\nh\n", "" },
		{ "a\tb\tc\td\te", "a\nb\nc\nd\ne\n", "" },
		{ "a\nb\nc\nd\ne", "a\nb\nc\nd\ne\n", "" },
		{ "a b c ", "a\nb\nc\n", "" },
		{ " a b c", "a\nb\nc\n", "" },
		{ "\ta\tb\tc\t", "a\nb\nc\n", "" },
		{ "word", "word\n", "" },
		{ " word", "word\n", "" },
		{ "word ", "word\n", "" },
		{ "\nword", "word\n", "" },
		{ "word\n", "word\n", "" },
		{ "\n\nword\n\n", "word\n", "" },
		{ "\t\tword\t\t", "word\n", "" },
		{ "  word  ", "word\n", "" },
		{ "hello,world", "hello,world\n", "" },
		{ "hello.world", "hello.world\n", "" },
		{ "hello-world", "hello-world\n", "" },
		{ "hello_world", "hello_world\n", "" },
		{ "hello/world", "hello/world\n", "" },
		{ "foo, bar", "foo,\nbar\n", "" },
		{ "foo\t,bar", "foo\n,bar\n", "" },
		{ "foo,\tbar", "foo,\nbar\n", "" },
		{ "0", "0\n", "" },
		{ "0 1 2 3", "0\n1\n2\n3\n", "" },
		{ "001 002", "001\n002\n", "" },
		{ "42\t1337\n21", "42\n1337\n21\n", "" },
		{ "abc123", "abc123\n", "" },
		{ "abc123 def456", "abc123\ndef456\n", "" },
		{ "abc\t123", "abc\n123\n", "" },
		{ "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789\n", "" },
		{ "a          b", "a\nb\n", "" },
		{ "a\t\t\t\tb", "a\nb\n", "" },
		{ "a\n\n\n\nb", "a\nb\n", "" },
		{ " \n a \n b \n ", "a\nb\n", "" },
		{ "\tfoo\t\n\tbar\t", "foo\nbar\n", "" },
		{ "a b\n", "a\nb\n", "" },
		{ "a\nb ", "a\nb\n", "" },
		{ "a\tb\t", "a\nb\n", "" },
		{ "aa bb", "aa\nbb\n", "" },
		{ "aa\tbb", "aa\nbb\n", "" },
		{ "aa\nbb", "aa\nbb\n", "" },
		{ " \t\na\t\n ", "a\n", "" },
		{ "\n\t a \t\n b \n\t", "a\nb\n", "" },
		{ "a a a a a a a", "a\na\na\na\na\na\na\n", "" },
		{ "a\tb\ta\tb\ta\tb", "a\nb\na\nb\na\nb\n", "" },
		{ "a\t\nb", "a\nb\n", "" },
		{ "a\n\tb", "a\nb\n", "" },
		{ "a \n\t b", "a\nb\n", "" },
		{ "a  \t  b", "a\nb\n", "" },
		{ "a\n\n\t\nb", "a\nb\n", "" },
		{ "a,b c", "a,b\nc\n", "" },
		{ "a.b c", "a.b\nc\n", "" },
		{ "a-b c", "a-b\nc\n", "" },
		{ "a,\tb.\nc", "a,\nb.\nc\n", "" },
		{ "x+\ty=\nz", "x+\ny=\nz\n", "" },
		{ "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n", "" },
		{ "a b c \n \t \n", "a\nb\nc\n", "" },
		{ "a\tb\tc\t\n\t", "a\nb\nc\n", "" },
		{ "abCD", "abCD\n", "" },
		{ "abCD12", "abCD12\n", "" },
		{ "a\tb\nc", "a\nb\nc\n", "" },
		{ "a\nb\tc", "a\nb\nc\n", "" },
		{ "a \t \n b", "a\nb\n", "" },
		{ "!", "!\n", "" },
		{ "@ #", "@\n#\n", "" },
		{ "$\t%", "$\n%\n", "" },
		{ "a a", "a\na\n", "" },
		{ "a\tb", "a\nb\n", "" },
		{ "ok", "ok\n", "" },
		{ " \t a \t ", "a\n", "" },
		{ "\n\n\n\n\n", "", "" }
	};

    int passed = 0;
	int failed = 0;

	for (size_t i = 0; i < tests.size(); i++)
	{
		bool crashed = false;
		std::string stdout_result = run_ft_split_safe(tests[i].input, crashed);
		std::string stderr_result = "";

		if (crashed) {
			std::cout << RED << "Test " << i+1 << " CRASHED!" << RESET << "\n";
			std::cout << YELLOW << "\tInput: " << RESET << "\"" << (tests[i].input ? tests[i].input : "NULL") << "\"\n";
			std::cout << YELLOW << "\tExpected stdout: " << RESET << "\"" << tests[i].expected_stdout << "\"\n";
			std::cout << YELLOW << "\tExpected stderr: " << RESET << "\"" << tests[i].expected_stderr << "\"\n";
			failed++;
			continue;
		}

		if (stdout_result != tests[i].expected_stdout ||
			stderr_result != tests[i].expected_stderr)
		{
			std::cout << RED << "Test " << i+1 << " FAILED" << RESET << "\n";
			std::cout << "Input: \"" << (tests[i].input ? tests[i].input : "NULL") << "\"\n";
			std::cout << "Expected stdout:\n" << tests[i].expected_stdout;
			std::cout << "Actual stdout:\n" << stdout_result;
			std::cout << "Expected stderr:\n" << tests[i].expected_stderr;
			std::cout << "Actual stderr:\n" << stderr_result;
			failed++;
		}
		else {
			passed++;
		}
	}

	std::cout << "\nSummary: "
			<< GREEN << passed << " passed" << RESET << ", "
			<< RED << failed << " failed" << RESET << "\n";

	return passed == (int)tests.size() ? 0 : 1;

}
