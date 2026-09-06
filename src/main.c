#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_ARGS 128 /* including the terminating NULL slot */

struct shell {
	char *line; /* dynamically-sized line buffer (owned) */
	size_t line_cap; /* allocated capacity of "line" */
	char *argv[MAX_ARGS];
	size_t argc;
    int interactive;
    int last_status;
};

static int shell_init(struct shell *shell)
{
    if (shell == NULL)
        return -1;

    memset(shell, 0, sizeof(*shell));
    shell->interactive = isatty(STDIN_FILENO);

    return 0;
}

static void shell_destroy(struct shell *shell)
{
    if (!shell)
        return;

    free(shell->line);
    memset(shell, 0, sizeof(*shell));
}

static void shell_prompt(const struct shell *shell)
{
    if (!shell->interactive)
        return;

    fputs("$ ", stdout);
    fflush(stdout);
}

static int shell_read_line(struct shell *shell)
{
    ssize_t nread;

    errno = 0;
    nread = getline(&shell->line, &shell->line_cap, stdin);

    if (nread < 0) {
        if (errno)
            perror("getline");
        else if (shell->interactive)
            fputc('\n', stdout);
        return -1;
    }

    return 0;
}

static int shell_parse(struct shell *shell)
{
    char *token;

    shell->argc = 0;
    token = strtok(shell->line, " \t\r\n");

    while (token) {
        if (shell->argc + 1 >= MAX_ARGS) {
            fprintf(
                stderr,
                "error: too many arguments (maximum %d)\n",
                MAX_ARGS -1
            );
            return -1;
        }

        shell->argv[shell->argc++] = token;
        token = strtok(NULL, " \t\r\n");
    }

    shell->argv[shell->argc] = NULL;

    return 0;
}

static int shell_execute(struct shell *shell)
{    
    fprintf(stderr, "exec: %s\n", shell->argv[0]);

    // TODO: Execute the command stored in shell->argv by forking a child, and
    // returning its exit status.

    return 0;
}

static int shell_run(struct shell *shell)
{
    for (;;) {
        shell_prompt(shell);

        if (shell_read_line(shell) != 0)
            break;

        if (shell_parse(shell) != 0)
            continue;

        if (shell->argc == 0)
            continue;

        if (!strcmp(shell->argv[0], "exit"))
            break;

        shell->last_status = shell_execute(shell);
    }

    return shell->last_status;
}

int main(void)
{
    struct shell shell;
    int status;

    if (shell_init(&shell) != 0)
        return EXIT_FAILURE;

    status = shell_run(&shell);
    shell_destroy(&shell);

    return status;
}
